#include "MoochoPack_MoochoThyraSolver.hpp"
#include "Thyra_DiagonalQuadraticResponseOnlyModelEvaluator.hpp"
#include "Thyra_VectorStdOps.hpp"
#include "Teuchos_GlobalMPISession.hpp"
#include "Teuchos_CommandLineProcessor.hpp"
#include "Teuchos_StandardCatchMacros.hpp"
#include "Teuchos_VerboseObject.hpp"
#include "Teuchos_XMLParameterListHelpers.hpp"
#include "Teuchos_DefaultComm.hpp"


int main( int argc, char* argv[] )
{
  using Teuchos::rcp;
  using Teuchos::RCP;
  using Teuchos::OSTab;
  using Teuchos::CommandLineProcessor;
  typedef Thyra::Ordinal Ordinal;
  using MoochoPack::MoochoSolver;
  using MoochoPack::MoochoThyraSolver;

  Teuchos::GlobalMPISession mpiSession(&argc, &argv);

  Teuchos::Time timer("");
  
  bool dummySuccess = true;

  Teuchos::RCP<Teuchos::FancyOStream>
    out = Teuchos::VerboseObjectBase::getDefaultOStream();

  try {
 
    //
    // A) Create the solver objects that will insert their command-line
    // options
    //

    MoochoThyraSolver solver;

    //
    // B) Get options from the command line
    //


    CommandLineProcessor clp;
    clp.throwExceptions(false);
    clp.addOutputSetupOptions(true);

    int localDim = 4;
    clp.setOption("local-dim", &localDim);

    double ps = 100.0;
    clp.setOption("ps", &ps);

    double p0 = 0.0;
    clp.setOption("p0", &p0);

    double condNum = 1.0;
    clp.setOption("cond-num", &condNum);

    double nonlinearTermFactor = 1e-3;
    clp.setOption("nonlinear-term-factor", &nonlinearTermFactor);

    double g_offset = 10.0;
    clp.setOption("g-offset", &g_offset);

    solver.setupCLP(&clp);

    CommandLineProcessor::EParseCommandLineReturn
      parse_return = clp.parse(argc,argv,&std::cerr);

    if( parse_return != CommandLineProcessor::PARSE_SUCCESSFUL )
      return parse_return;

    solver.readParameters( out.get() );

    const RCP<const Teuchos::Comm<Ordinal> > comm =
      Teuchos::DefaultComm<Ordinal>::getComm();
 
    //
    // C) Create the Thyra::ModelEvaluator object
    //
 
    *out << "\nCreate Thyra::DiagonalQuadraticResponseOnlyModelEvaluator object ...\n";
 
    Teuchos::RCP<Thyra::DiagonalQuadraticResponseOnlyModelEvaluator<double> >
      thyraModel = Thyra::diagonalQuadraticResponseOnlyModelEvaluator<double> (localDim, comm);

    const RCP<Thyra::VectorBase<double> > ps_vec = createMember(thyraModel->get_p_space(0));
    Thyra::V_S<double>(ps_vec.ptr(), ps);
    thyraModel->setSolutionVector(ps_vec);
    
    // ToDo: Set p0

    // ToDo: Set up diagonal consistent with condNum

    thyraModel->setNonlinearTermFactor(nonlinearTermFactor);

    thyraModel->setScalarOffset(g_offset);

    //
    // D) Solve the NLP
    //
    
    // Set the model
    solver.setModel(thyraModel);
    
    // Read the initial guess if one exists
    solver.readInitialGuess(out.get());
    
    // Solve the NLP
    const MoochoSolver::ESolutionStatus	solution_status = solver.solve();
    
    // Write the solution to file
    solver.writeFinalSolution(out.get());
    
    // Write the final parameters to file
    solver.writeParamsFile();
    
    //
    // E) Return the solution status (0 if successful)
    //
    
    if(solution_status == MoochoSolver::SOLVE_RETURN_SOLVED)
      *out << "\nEnd Result: TEST PASSED\n";
    else
      *out << "\nEnd Result: TEST FAILED\n";
    
    return solution_status;
    
  }
  TEUCHOS_STANDARD_CATCH_STATEMENTS(true, *out, dummySuccess)
    
  return MoochoSolver::SOLVE_RETURN_EXCEPTION;

}
