// @HEADER
// ***********************************************************************
// 
// Moocho: Multi-functional Object-Oriented arCHitecture for Optimization
//                  Copyright (2003) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Roscoe A. Bartlett (rabartl@sandia.gov) 
// 
// ***********************************************************************
// @HEADER

#include <ostream>

#include "MoochoPack_EvalNewPointTailoredApproach_Step.hpp"
#include "MoochoPack_Exceptions.hpp"
#include "MoochoPack_moocho_algo_conversion.hpp"
#include "IterationPack_print_algorithm_step.hpp"
#include "ConstrainedOptPack_MatrixIdentConcatStd.hpp"
#include "NLPInterfacePack_NLPDirect.hpp"
#include "AbstractLinAlgPack_MatrixOpOut.hpp"
#include "AbstractLinAlgPack_VectorMutable.hpp"
#include "AbstractLinAlgPack_VectorStdOps.hpp"
#include "AbstractLinAlgPack_VectorOut.hpp"
#include "AbstractLinAlgPack_assert_print_nan_inf.hpp"
#include "AbstractLinAlgPack_LinAlgOpPack.hpp"
#include "Teuchos_dyn_cast.hpp"
#include "Teuchos_TestForException.hpp"

namespace MoochoPack {

EvalNewPointTailoredApproach_Step::EvalNewPointTailoredApproach_Step(
  const deriv_tester_ptr_t      &deriv_tester
  ,const bounds_tester_ptr_t    &bounds_tester
  , EFDDerivTesting             fd_deriv_testing
  )
  :deriv_tester_(deriv_tester)
  ,bounds_tester_(bounds_tester)
  ,fd_deriv_testing_(fd_deriv_testing)
{}

bool EvalNewPointTailoredApproach_Step::do_step(
  Algorithm& _algo, poss_type step_poss, IterationPack::EDoStepType type
  ,poss_type assoc_step_poss
  )
{
  using Teuchos::rcp;
  using Teuchos::dyn_cast;
  using AbstractLinAlgPack::assert_print_nan_inf;
  using LinAlgOpPack::V_MtV;
  using IterationPack::print_algorithm_step;

  NLPAlgo             &algo   = rsqp_algo(_algo);
  NLPAlgoState        &s      = algo.rsqp_state();
  NLPDirect           &nlp    = dyn_cast<NLPDirect>(algo.nlp());

  EJournalOutputLevel olevel = algo.algo_cntr().journal_output_level();
  std::ostream& out = algo.track().journal_out();

  // print step header.
  if( static_cast<int>(olevel) >= static_cast<int>(PRINT_ALGORITHM_STEPS) ) {
    using IterationPack::print_algorithm_step;
    print_algorithm_step( algo, step_poss, type, assoc_step_poss, out );
  }

  if(!nlp.is_initialized())
    nlp.initialize(algo.algo_cntr().check_results());

  Teuchos::VerboseObjectTempState<NLP>
    nlpOutputTempState(rcp(&nlp,false),Teuchos::getFancyOStream(rcp(&out,false)),convertToVerbLevel(olevel));

  const size_type
    n  = nlp.n(),
    m  = nlp.m(),
    r  = nlp.var_dep().size();

  TEST_FOR_EXCEPTION(
    m > r, TestFailed
    ,"EvalNewPointTailoredApproach_Step::do_step(...) : Error, "
    "Undecomposed equalities are supported yet!" );

  IterQuantityAccess<VectorMutable>
    &x_iq = s.x();

  if( x_iq.last_updated() == IterQuantity::NONE_UPDATED ) {
    if( static_cast<int>(olevel) >= static_cast<int>(PRINT_ALGORITHM_STEPS) ) {
      out << "\nx is not updated for any k so set x_k = nlp.xinit() ...\n";
    }
    x_iq.set_k(0) = nlp.xinit();
  }

  // Validate x
  if(algo.algo_cntr().check_results()) {
    assert_print_nan_inf(
      x_iq.get_k(0), "x_k", true
      , int(olevel) >= int(PRINT_ALGORITHM_STEPS) ? &out : NULL
      );
    if( nlp.num_bounded_x() > 0 ) {
      if(!bounds_tester().check_in_bounds(
           int(olevel)  >= int(PRINT_ALGORITHM_STEPS) ? &out : NULL
           ,int(olevel) >= int(PRINT_VECTORS)                // print_all_warnings
           ,int(olevel) >= int(PRINT_ITERATION_QUANTITIES)  // print_vectors
           ,nlp.xl(),        "xl"
           ,nlp.xu(),        "xu"
           ,x_iq.get_k(0),   "x_k"
           ))
      {
        TEST_FOR_EXCEPTION(
          true, TestFailed
          ,"EvalNewPointTailoredApproach_Step::do_step(...) : Error, "
          "the variables bounds xl <= x_k <= xu where violated!" );
      }
    }
  }

  Vector &x = x_iq.get_k(0);

  if( static_cast<int>(olevel) >= static_cast<int>(PRINT_ALGORITHM_STEPS) ) {
    out << "\n||x_k||inf = " << x.norm_inf() << std::endl;
  }
  if( static_cast<int>(olevel) >= static_cast<int>(PRINT_VECTORS) ) {
    out << "\nx_k = \n" << x;
  }

  // If c_k is not updated then we must compute it
  bool recalc_c = true;

  if( !s.c().updated_k(0) ) {
    s.c().set_k(0);
    recalc_c = true;
  }
  else {
    recalc_c = false;
  }
    
  // Get references to Z, Y, Uz and Uy
  MatrixOp
    &Z_k  = s.Z().set_k(0),
    &Y_k  = s.Y().set_k(0),
    *Uz_k = (m > r) ? &s.Uz().set_k(0) : NULL,
    *Uy_k = (m > r) ? &s.Uy().set_k(0) : NULL;
  MatrixIdentConcatStd
    &cZ_k = dyn_cast<MatrixIdentConcatStd>(Z_k);
  // Release any references to D in Y or Uy
   uninitialize_Y_Uy(&Y_k,Uy_k);
  // If Z has not been initialized or Z.D is being shared by someone else we need to reconstruct Z.D
  bool reconstruct_Z_D = (cZ_k.rows() == n || cZ_k.cols() != n-r || cZ_k.D_ptr().count() > 1);
  MatrixIdentConcatStd::D_ptr_t
    D_ptr = Teuchos::null;
  if( reconstruct_Z_D )
    D_ptr = nlp.factory_D()->create();
  else
    D_ptr = cZ_k.D_ptr();

  // Compute all the quantities.
  Teuchos::RefCountPtr<MatrixOp>
    GcU = (m > r) ? nlp.factory_GcU()->create() : Teuchos::null; // ToDo: Reuse GcU somehow? 
  VectorMutable
    &py_k  = s.py().set_k(0);
  nlp.unset_quantities();
  nlp.calc_point(
    x                                                     // x
    ,!s.f().updated_k(0) ? &s.f().set_k(0) : NULL         // f
    ,&s.c().get_k(0)                                      // c
    ,recalc_c                                             // recalc_c
    ,&s.Gf().set_k(0)                                     // Gf
    ,&py_k                                                // -inv(C)*c
    ,&s.rGf().set_k(0)                                    // rGf
    ,GcU.get()                                            // GcU
    ,const_cast<MatrixOp*>(D_ptr.get())                   // -inv(C)*N
    ,Uz_k                                                 // Uz
    );
  s.equ_decomp(   nlp.con_decomp()   );
  s.equ_undecomp( nlp.con_undecomp() );

  if( static_cast<int>(olevel) >= static_cast<int>(PRINT_ALGORITHM_STEPS) ) {
    out << "\nQuantities computed directly from NLPDirect nlp object ...\n";
    out << "\nf_k           = " << s.f().get_k(0);
    out << "\n||c_k||inf    = " << s.c().get_k(0).norm_inf();
    out << "\n||Gf_k||inf   = " << s.Gf().get_k(0).norm_inf();
    out << "\n||py_k||inf   = " << s.py().get_k(0).norm_inf();
    out << "\n||rGf_k||inf  = " << s.rGf().get_k(0).norm_inf();
    out << std::endl;
  }

  if( static_cast<int>(olevel) >= static_cast<int>(PRINT_VECTORS) ) {
    out	<< "\nc_k  = \n" << s.c().get_k(0);
    out	<< "\nGf_k = \n" << s.Gf().get_k(0);
    out	<< "\npy_k = \n" << s.py().get_k(0);
    out	<< "\nrGf_k = \n" << s.rGf().get_k(0);
    out << std::endl;
  }
  if( static_cast<int>(olevel) >= static_cast<int>(PRINT_ITERATION_QUANTITIES) ) {
    out << "\nD = -inv(C)*N = \n" << *D_ptr;
    out << std::endl;
  }

  if(algo.algo_cntr().check_results()) {
    assert_print_nan_inf(s.f().get_k(0),   "f_k",true,&out); 
    assert_print_nan_inf(s.c().get_k(0),   "c_k",true,&out); 
    assert_print_nan_inf(s.Gf().get_k(0),  "Gf_k",true,&out); 
    assert_print_nan_inf(s.py().get_k(0),  "py_k",true,&out);
    assert_print_nan_inf(s.rGf().get_k(0), "rGf_k",true,&out);
  }

  // Check the derivatives if we are checking the results
  if(		fd_deriv_testing() == FD_TEST
    || ( fd_deriv_testing() == FD_DEFAULT && algo.algo_cntr().check_results() )  )
  {
    
    if( olevel >= PRINT_ALGORITHM_STEPS ) {
      out	<< "\n*** Checking derivatives by finite differences\n";
    }
    
    const bool has_bounds = nlp.num_bounded_x() > 0;
    const bool nlp_passed = deriv_tester().finite_diff_check(
      &nlp
      ,x
      ,has_bounds ? &nlp.xl() : (const Vector*)NULL
      ,has_bounds ? &nlp.xu() : (const Vector*)NULL
      ,&s.c().get_k(0)
      ,&s.Gf().get_k(0)
      ,&s.py().get_k(0)
      ,&s.rGf().get_k(0)
      ,GcU.get()
      ,D_ptr.get()
      ,Uz_k
      ,olevel >= PRINT_VECTORS
      ,( olevel >= PRINT_ALGORITHM_STEPS ) ? &out : (std::ostream*)NULL
      );
    TEST_FOR_EXCEPTION(
      !nlp_passed, TestFailed
      ,"EvalNewPointTailoredApproach_Step::do_step(...) : Error, "
      "the tests of the nlp derivatives failed!" );
  }

  if( reconstruct_Z_D ) {
    //
    // Z = [      D     ] space_xD
    //     [      I     ] space_xI
    //        space_xI
    //
    cZ_k.initialize(
      nlp.space_x()                                          // space_cols
      ,nlp.space_x()->sub_space(nlp.var_indep())->clone()    // space_rows
      ,MatrixIdentConcatStd::TOP                             // top_or_bottom
      ,1.0                                                   // alpha
      ,D_ptr                                                 // D_ptr
      ,BLAS_Cpp::no_trans                                    // D_trans
      );
  }

  // Compute py, Y and Uy
  calc_py_Y_Uy( nlp, D_ptr, &py_k, &Y_k, Uy_k, olevel, out ); 

  // Compute Ypy = Y*py
  VectorMutable
    &Ypy_k  = s.Ypy().set_k(0);
  V_MtV( &Ypy_k, Y_k, BLAS_Cpp::no_trans, py_k );

  if( static_cast<int>(olevel) >= static_cast<int>(PRINT_ALGORITHM_STEPS) ) {
    out << "\nQuantities after computing final py and Ypy ...\n";
    out	<< "\n||py_k||inf   = " << s.py().get_k(0).norm_inf();
    out << "\n||Ypy_k||inf  = " << s.Ypy().get_k(0).norm_inf();
    out << std::endl;
  }

  if( static_cast<int>(olevel) >= static_cast<int>(PRINT_VECTORS) ) {
    out	<< "\npy_k = \n"  << s.py().get_k(0);
    out	<< "\nYpy_k = \n" << s.Ypy().get_k(0);
    out << std::endl;
  }

  if( static_cast<int>(olevel) >= static_cast<int>(PRINT_ITERATION_QUANTITIES) ) {
    out << "\nZ_k = \n" << s.Z().get_k(0);
    out << "\nY_k = \n" << s.Y().get_k(0);
    out << std::endl;
  }

  return true;

}

void EvalNewPointTailoredApproach_Step::print_step(
  const Algorithm& algo, poss_type step_poss, IterationPack::EDoStepType type
  ,poss_type assoc_step_poss, std::ostream& out, const std::string& L
  ) const
{
  out
    << L << "*** Evaluate the new point for the \"Tailored Approach\"\n"
    << L << "if nlp is not initialized then initialize the nlp\n"
    << L << "if x is not updated for any k then set x_k = xinit\n"
    << L << "Gf_k = Gf(x_k) <: space_x\n"
    << L << "For Gc(:,equ_decomp) = [ C' ; N' ] <: space_x:space_c(equ_decomp) compute:\n"
    << L << "  py_k = -inv(C)*c_k\n"
    << L << "  D = -inv(C)*N <: R^(n x (n-m))\n"
    << L << "  rGf_k = Gf_k(var_indep) + D'*Gf_k(var_dep)\n"
    << L << "  Z_k = [ D ; I ] <: R^(n x (n-m))\n"
    << L << "  if m > r Uz_k = Gc(var_indep,equ_undecomp)' + Gc(var_dep,equ_undecomp)'*D\n"
    << L << "if ( (fd_deriv_testing==FD_TEST)\n"
    << L << "    or (fd_deriv_testing==FD_DEFAULT and check_results==true\n"
    << L << "  ) then\n"
    << L << "  check Gf_k, py_k, rGf_k, D, Uz (if m > r) and Vz (if mI > 0) by finite differences.\n"
    << L << "end\n";
  print_calc_py_Y_Uy( out, L );
  out
    << L << "Ypy_k = Y_k * py_k\n"
    << L << "if c_k is not updated c_k = c(x_k) <: space_c\n"
    << L << "if mI > 0 and h_k is not updated h_k = h(x_k) <: space_h\n"
    << L << "if f_k is not updated f_k = f(x_k) <: REAL\n"
    ;
}

} // end namespace MoochoPack
