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
#include <typeinfo>

#include "MoochoPack_DecompositionSystemHandlerStd_Strategy.hpp"
#include "MoochoPack_Exceptions.hpp"
#include "MoochoPack_moocho_algo_conversion.hpp"
#include "IterationPack_print_algorithm_step.hpp"
#include "ConstrainedOptPack_DecompositionSystem.hpp"
#include "NLPInterfacePack_NLPFirstOrder.hpp"
#include "AbstractLinAlgPack_BasisSystem.hpp"
#include "AbstractLinAlgPack_MatrixOpNonsing.hpp"
#include "AbstractLinAlgPack_MatrixOpOut.hpp"
#include "AbstractLinAlgPack_VectorMutable.hpp"
#include "AbstractLinAlgPack_VectorStdOps.hpp"
#include "AbstractLinAlgPack_VectorOut.hpp"
#include "AbstractLinAlgPack_LinAlgOpPack.hpp"
#include "AbstractLinAlgPack_MatrixSymIdent.hpp"
#include "AbstractLinAlgPack_assert_print_nan_inf.hpp"
#include "Teuchos_dyn_cast.hpp"
#include "Teuchos_TestForException.hpp"

namespace MoochoPack {

// Constructors / initializers

DecompositionSystemHandlerStd_Strategy::DecompositionSystemHandlerStd_Strategy()
{}

// Overridden from DecompositionSystemHandler_Strategy

bool DecompositionSystemHandlerStd_Strategy::update_decomposition(
  NLPAlgo                                &algo
  ,NLPAlgoState                          &s
  ,NLPFirstOrder                         &nlp
  ,EDecompSysTesting                     decomp_sys_testing
  ,EDecompSysPrintLevel                  decomp_sys_testing_print_level
  ,bool                                  *new_decomp_selected
  )
{
  using Teuchos::dyn_cast;

  EJournalOutputLevel olevel = algo.algo_cntr().journal_output_level();
  std::ostream& out = algo.track().journal_out();

  const size_type
    n  = nlp.n(),
    m  = nlp.m(),
    r  = s.decomp_sys().equ_decomp().size();

  // Get the iteration quantity container objects
  IterQuantityAccess<MatrixOp>
    *Gc_iq  = ( m  > 0 )              ? &s.Gc() : NULL,
    *Z_iq   = ( n > m && r > 0 )      ? &s.Z()  : NULL,
    *Y_iq   = ( r > 0 )               ? &s.Y()  : NULL,
    *Uz_iq  = ( m  > 0 && m  > r )    ? &s.Uz() : NULL,
    *Uy_iq  = ( m  > 0 && m  > r )    ? &s.Uy() : NULL;
  IterQuantityAccess<MatrixOpNonsing>
    *R_iq   = ( m > 0 )               ? &s.R()  : NULL;

  if( n > m ) {

    //
    // Update range/null decomposition
    //
    
    // Determine if we will test the decomp_sys or not
    const DecompositionSystem::ERunTests
      ds_test_what = ( ( decomp_sys_testing == DST_TEST
                 || ( decomp_sys_testing == DST_DEFAULT
                  && algo.algo_cntr().check_results() ) )
               ? DecompositionSystem::RUN_TESTS
               : DecompositionSystem::NO_TESTS );
    
    // Determine the output level for decomp_sys				
    DecompositionSystem::EOutputLevel ds_olevel;
    switch(olevel) {
      case PRINT_NOTHING:
      case PRINT_BASIC_ALGORITHM_INFO:
        ds_olevel = DecompositionSystem::PRINT_NONE;
        break;
      case PRINT_ALGORITHM_STEPS:
      case PRINT_ACTIVE_SET:
        ds_olevel = DecompositionSystem::PRINT_BASIC_INFO;
        break;
      case PRINT_VECTORS:
        ds_olevel = DecompositionSystem::PRINT_VECTORS;
        break;
      case PRINT_ITERATION_QUANTITIES:
        ds_olevel = DecompositionSystem::PRINT_EVERY_THING;
        break;
      default:
        assert(0); // Should not get here!
    };
    
    // Form the decomposition of Gc and update the decomposition system matrices
    if( olevel >= PRINT_ALGORITHM_STEPS ) {
      out << "\nUpdating the range/null decompostion matrices ...\n";
    }
    s.decomp_sys().update_decomp(
      &out                               // out
      ,ds_olevel                         // olevel
      ,ds_test_what                      // test_what
      ,Gc_iq->get_k(0)                   // Gc
      ,&Z_iq->set_k(0)                   // Z
      ,&Y_iq->set_k(0)                   // Y
      ,&R_iq->set_k(0)                   // R
      ,Uz_iq ? &Uz_iq->set_k(0) : NULL   // Uz
      ,Uy_iq ? &Uy_iq->set_k(0) : NULL   // Uy
      ,DecompositionSystem::MATRICES_ALLOW_DEP_IMPS // ToDo: Change this!
      );
    s.equ_decomp(   s.decomp_sys().equ_decomp()   );
    s.equ_undecomp( s.decomp_sys().equ_undecomp() );
    
    *new_decomp_selected = false;

  }
  else {
    //
    // Update decomposition
    //
    // R = C
    // Y = I
    //
    const BasisSystem &basis_sys = *nlp.basis_sys();
    basis_sys.update_basis(
      Gc_iq->get_k(0)                        // Gc
      ,&R_iq->set_k(0 )                      // C
      ,NULL                                  // D
      ,NULL                                  // GcUP
      ,BasisSystem::MATRICES_ALLOW_DEP_IMPS  // Meaningless
      ,static_cast<int>(olevel) >= static_cast<int>(PRINT_BASIC_ALGORITHM_INFO) ? &out : NULL
      );
    dyn_cast<MatrixSymIdent>(Y_iq->set_k(0)).initialize( nlp.space_x() );
    s.equ_decomp(   basis_sys.equ_decomp()   );
    s.equ_undecomp( basis_sys.equ_undecomp() );
  }
  
  return true;
}

void DecompositionSystemHandlerStd_Strategy::print_update_decomposition(
  const NLPAlgo                          &algo
  ,const NLPAlgoState                    &s
  ,std::ostream                          &out
  ,const std::string                     &L
  ) const
{
  using Teuchos::dyn_cast;

  const NLPFirstOrder &nlp = dyn_cast<const NLPFirstOrder>(algo.nlp());
  const size_type n  = nlp.n(), m = nlp.m(), r = nlp.basis_sys()->equ_decomp().size();
  out
    << L << "*** Updating the range/null decomposition.\n";
  if( n == m && m == r ) {
    out
      << L << "R = C\n"
      << L << "Y = I\n";
  }
  else {
    out
      << L << "begin update decomposition\n"
      << L << "(class = \'" << typeid(s.decomp_sys()).name() << "\')\n"
      ;
    s.decomp_sys().print_update_decomp( out, L + "  " );
    out
      << L << "end update decomposition\n"
      ;
  }
}

} // end namespace MoochoPack
