// @HEADER
// ***********************************************************************
// 
//                           Stokhos Package
//                 Copyright (2009) Sandia Corporation
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
// Questions? Contact Eric T. Phipps (etphipp@sandia.gov).
// 
// ***********************************************************************
// @HEADER

// ModelEvaluator implementing our problem
#include "twoD_diffusion_ME.hpp"

// Epetra communicator
#ifdef HAVE_MPI
#include "Epetra_MpiComm.h"
#else
#include "Epetra_SerialComm.h"
#endif

// AztecOO solver
#include "AztecOO.h"

// Stokhos Stochastic Galerkin
#include "Stokhos_Epetra.hpp"

// Timing utilities
#include "Teuchos_TimeMonitor.hpp"

// I/O utilities
#include "EpetraExt_VectorOut.h"

int main(int argc, char *argv[]) {
  int n = 32;                        // spatial discretization (per dimension)
  int num_KL = 2;                    // number of KL terms
  int p = 3;                         // polynomial order
  double mu = 0.1;                   // mean of exponential random field
  double s = 0.2;                    // std. dev. of exponential r.f.
  bool nonlinear_expansion = false;  // nonlinear expansion of diffusion coeff
                                     // (e.g., log-normal)
  bool symmetric = false;            // use symmetric formulation

// Initialize MPI
#ifdef HAVE_MPI
  MPI_Init(&argc,&argv);
#endif

  int MyPID;

  try {

    {
    TEUCHOS_FUNC_TIME_MONITOR("Total PCE Calculation Time");

    // Create a communicator for Epetra objects
    Teuchos::RCP<const Epetra_Comm> globalComm;
#ifdef HAVE_MPI
    globalComm = Teuchos::rcp(new Epetra_MpiComm(MPI_COMM_WORLD));
#else
    globalComm = Teuchos::rcp(new Epetra_SerialComm);
#endif
    MyPID = globalComm->MyPID();

    // Create Stochastic Galerkin basis and expansion
    Teuchos::Array< Teuchos::RCP<const Stokhos::OneDOrthogPolyBasis<int,double> > > bases(num_KL); 
    for (int i=0; i<num_KL; i++)
      bases[i] = Teuchos::rcp(new Stokhos::LegendreBasis<int,double>(p, true));
    Teuchos::RCP<const Stokhos::CompletePolynomialBasis<int,double> > basis = 
      Teuchos::rcp(new Stokhos::CompletePolynomialBasis<int,double>(bases,
		     1e-12));
    int sz = basis->size();
    Teuchos::RCP<Stokhos::Sparse3Tensor<int,double> > Cijk;
    if (nonlinear_expansion)
      Cijk = basis->computeTripleProductTensor(sz);
    else
      Cijk = basis->computeTripleProductTensor(num_KL+1);
    Teuchos::RCP<Stokhos::OrthogPolyExpansion<int,double> > expansion = 
      Teuchos::rcp(new Stokhos::AlgebraicOrthogPolyExpansion<int,double>(basis,
									 Cijk));
    if (MyPID == 0)
      std::cout << "Stochastic Galerkin expansion size = " << sz << std::endl;

    // Create stochastic parallel distribution
    int num_spatial_procs = -1;
    if (argc > 1)
      num_spatial_procs = std::atoi(argv[1]);
    Teuchos::ParameterList parallelParams;
    parallelParams.set("Number of Spatial Processors", num_spatial_procs);
    // parallelParams.set("Rebalance Stochastic Graph", true);
    // Teuchos::ParameterList& isorropia_params = 
    //   parallelParams.sublist("Isorropia");
    // isorropia_params.set("Balance objective", "nonzeros");
    Teuchos::RCP<Stokhos::ParallelData> sg_parallel_data =
      Teuchos::rcp(new Stokhos::ParallelData(basis, Cijk, globalComm,
					     parallelParams));
    Teuchos::RCP<const EpetraExt::MultiComm> sg_comm = 
      sg_parallel_data->getMultiComm();
    Teuchos::RCP<const Epetra_Comm> app_comm = 
      sg_parallel_data->getSpatialComm();

    // Create application
    Teuchos::RCP<twoD_diffusion_ME> model = 
      Teuchos::rcp(new twoD_diffusion_ME(app_comm, n, num_KL, mu, s, basis, 
					 nonlinear_expansion, symmetric));
    
    // Setup stochastic Galerkin algorithmic parameters
    Teuchos::RCP<Teuchos::ParameterList> sgParams = 
      Teuchos::rcp(new Teuchos::ParameterList);
    Teuchos::ParameterList& sgOpParams = 
      sgParams->sublist("SG Operator");
    Teuchos::ParameterList& sgPrecParams = 
      sgParams->sublist("SG Preconditioner");
    if (!nonlinear_expansion) {
      sgParams->set("Parameter Expansion Type", "Linear");
      sgParams->set("Jacobian Expansion Type", "Linear");
    }
    sgOpParams.set("Operator Method", "Matrix Free");
    sgPrecParams.set("Preconditioner Method", "Approximate Gauss-Seidel");
    sgPrecParams.set("Symmetric Gauss-Seidel", symmetric);
    sgPrecParams.set("Mean Preconditioner Type", "ML");
    Teuchos::ParameterList& precParams = 
      sgPrecParams.sublist("Mean Preconditioner Parameters");
    precParams.set("default values", "SA");
    precParams.set("ML output", 0);
    precParams.set("max levels",5);
    precParams.set("increasing or decreasing","increasing");
    precParams.set("aggregation: type", "Uncoupled");
    precParams.set("smoother: type","ML symmetric Gauss-Seidel");
    precParams.set("smoother: sweeps",2);
    precParams.set("smoother: pre or post", "both");
    precParams.set("coarse: max size", 200);
#ifdef HAVE_ML_AMESOS
    precParams.set("coarse: type","Amesos-KLU");
#else
    precParams.set("coarse: type","Jacobi");
#endif

    // Create stochastic Galerkin model evaluator
    Teuchos::RCP<Stokhos::SGModelEvaluator> sg_model =
      Teuchos::rcp(new Stokhos::SGModelEvaluator(model, basis, Teuchos::null,
						 expansion, sg_parallel_data, 
						 sgParams));

    // Set up stochastic parameters
    Teuchos::RCP<Stokhos::EpetraVectorOrthogPoly> sg_p_poly =
      sg_model->create_p_sg(0);
    for (int i=0; i<num_KL; i++) {
      sg_p_poly->term(i,0)[i] = 0.0;
      sg_p_poly->term(i,1)[i] = 1.0;
    }

    // Create vectors and operators
    Teuchos::RCP<const Epetra_Vector> sg_p = sg_p_poly->getBlockVector();
    Teuchos::RCP<Epetra_Vector> sg_x =
      Teuchos::rcp(new Epetra_Vector(*(sg_model->get_x_map())));
    sg_x->PutScalar(0.0);
    Teuchos::RCP<Epetra_Vector> sg_f = 
      Teuchos::rcp(new Epetra_Vector(*(sg_model->get_f_map())));
    Teuchos::RCP<Epetra_Vector> sg_dx = 
      Teuchos::rcp(new Epetra_Vector(*(sg_model->get_x_map())));
    Teuchos::RCP<Epetra_Operator> sg_J = sg_model->create_W();
    Teuchos::RCP<Epetra_Operator> sg_M = sg_model->create_WPrec()->PrecOp;

    
    // Setup InArgs and OutArgs
    EpetraExt::ModelEvaluator::InArgs sg_inArgs = sg_model->createInArgs();
    EpetraExt::ModelEvaluator::OutArgs sg_outArgs = sg_model->createOutArgs();
    sg_inArgs.set_p(1, sg_p);
    sg_inArgs.set_x(sg_x);
    sg_outArgs.set_f(sg_f);
    sg_outArgs.set_W(sg_J);
    sg_outArgs.set_WPrec(sg_M);

    // Evaluate model
    sg_model->evalModel(sg_inArgs, sg_outArgs);

    // Print initial residual norm
    double norm_f;
    sg_f->Norm2(&norm_f);
    if (MyPID == 0)
      std::cout << "\nInitial residual norm = " << norm_f << std::endl;

    // Setup AztecOO solver
    AztecOO aztec;
    if (symmetric)
      aztec.SetAztecOption(AZ_solver, AZ_cg);
    else
      aztec.SetAztecOption(AZ_solver, AZ_gmres);
    aztec.SetAztecOption(AZ_precond, AZ_none);
    aztec.SetAztecOption(AZ_kspace, 20);
    aztec.SetAztecOption(AZ_conv, AZ_r0);
    aztec.SetAztecOption(AZ_output, 1);
    aztec.SetUserOperator(sg_J.get());
    aztec.SetPrecOperator(sg_M.get());
    aztec.SetLHS(sg_dx.get());
    aztec.SetRHS(sg_f.get());

    // Solve linear system
    aztec.Iterate(1000, 1e-12);

    // Update x
    sg_x->Update(-1.0, *sg_dx, 1.0);

    // Save solution to file
    EpetraExt::VectorToMatrixMarketFile("stochastic_solution.mm", *sg_x);

    // Save mean and variance to file
    Teuchos::RCP<Stokhos::EpetraVectorOrthogPoly> sg_x_poly = 
      sg_model->create_x_sg(View, sg_x.get());
    Epetra_Vector mean(*(model->get_x_map()));
    Epetra_Vector std_dev(*(model->get_x_map()));
    sg_x_poly->computeMean(mean);
    sg_x_poly->computeStandardDeviation(std_dev);
    EpetraExt::VectorToMatrixMarketFile("mean_gal.mm", mean);
    EpetraExt::VectorToMatrixMarketFile("std_dev_gal.mm", std_dev);

    // Compute new residual & response function
    EpetraExt::ModelEvaluator::OutArgs sg_outArgs2 = sg_model->createOutArgs();
    Teuchos::RCP<Epetra_Vector> sg_g = 
      Teuchos::rcp(new Epetra_Vector(*(sg_model->get_g_map(0))));
    sg_f->PutScalar(0.0);
    sg_outArgs2.set_f(sg_f);
    sg_outArgs2.set_g(0, sg_g);
    sg_model->evalModel(sg_inArgs, sg_outArgs2);

    // Print initial residual norm
    sg_f->Norm2(&norm_f);
    if (MyPID == 0)
      std::cout << "\nFinal residual norm = " << norm_f << std::endl;

    // Print mean and standard deviation of responses
    Teuchos::RCP<Stokhos::EpetraVectorOrthogPoly> sg_g_poly =
      sg_model->create_g_sg(0, View, sg_g.get());
    Epetra_Vector g_mean(*(model->get_g_map(0)));
    Epetra_Vector g_std_dev(*(model->get_g_map(0)));
    sg_g_poly->computeMean(g_mean);
    sg_g_poly->computeStandardDeviation(g_std_dev);
    std::cout.precision(16);
    // std::cout << "\nResponse Expansion = " << std::endl;
    // std::cout.precision(12);
    // sg_g_poly->print(std::cout);
    std::cout << "\nResponse Mean =      " << std::endl << g_mean << std::endl;
    std::cout << "Response Std. Dev. = " << std::endl << g_std_dev << std::endl;

    if (norm_f < 1.0e-10 && MyPID == 0)
      std::cout << "Example Passed!" << std::endl;
    }

    Teuchos::TimeMonitor::summarize(std::cout);
    Teuchos::TimeMonitor::zeroOutTimers();

  }
  
  catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  catch (string& s) {
    std::cout << s << std::endl;
  }
  catch (char *s) {
    std::cout << s << std::endl;
  }
  catch (...) {
    std::cout << "Caught unknown exception!" <<std:: endl;
  }

#ifdef HAVE_MPI
  MPI_Finalize() ;
#endif

}
