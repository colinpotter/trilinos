// @HEADER
// ***********************************************************************
// 
//               TSFCore: Trilinos Solver Framework Core
//                 Copyright (2004) Sandia Corporation
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
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ***********************************************************************
// @HEADER

#include "MPITridiagLinearOp.hpp"
#include "sillyCgSolve.hpp"
#include "TSFCoreVectorStdOps.hpp"
#include "TSFCoreTestingTools.hpp"
#include "Teuchos_CommandLineProcessor.hpp"
#include "Teuchos_Time.hpp"
#include "Teuchos_oblackholestream.hpp"

//
// This example program is ment to show how easy it is to create MPI
// TSFCore objects and use them with an ANA (CG in this case).
//
// This example uses a silly concrete tridagonal matrix class called
// MPITridiagLinearOp that demonstrates how to write such
// subclasses.
//
template<class Scalar>
bool runCgSolveExample(
	MPI_Comm                                                       mpiComm
	,const int                                                     procRank
	,const int                                                     numProc
	,const int                                                     localDim
	,const Scalar                                                  diagScale
	,const bool                                                    verbose
	,const typename Teuchos::ScalarTraits<Scalar>::magnitudeType   tolerance
	,const int                                                     maxNumIters
	)
{
  using Teuchos::RefCountPtr;
  using Teuchos::rcp;
	typedef Teuchos::ScalarTraits<Scalar> ST;
	typedef typename ST::magnitudeType    ScalarMag;
	bool success = true;
	bool result;
	// Setup the output stream (do output only on root process!)
	Teuchos::oblackholestream black_hole_out;
	std::ostream &out = ( procRank == 0 ? std::cout : black_hole_out );
	if(verbose)
		out << "\n***\n*** Running silly CG solver using scalar type = \'" << ST::name() << "\' ...\n***\n";
	Teuchos::Time timer("");
	timer.start(true);
	//
	// (A) Setup a simple linear system with tridagonal operator:
	//
	//       [  a*2   -1                ]
	//       [ -1    a*2  -1            ]
	//  A =  [         .   .    .       ]
	//       [            -1  a*2    -1 ]
	//       [                 -1   a*2 ]
	//
	// (A.1) Create the tridagonal matrix operator
	if(verbose) out << "\nConstructing tridagonal matrix A of local dimmension = " << localDim
												<< " and diagonal multiplier = " << diagScale << " ...\n";
	const TSFCore::Index
		lowerDim = ( procRank == 0         ? localDim - 1 : localDim ),
		upperDim = ( procRank == numProc-1 ? localDim - 1 : localDim );
	std::vector<Scalar> lower(lowerDim), diag(localDim), upper(upperDim);
	const Scalar one = ST::one(), diagTerm = Scalar(2)*diagScale*ST::one();
	int k = 0;
	if(procRank > 0) lower[k] = +one; diag[k] = diagTerm; upper[k] = -one;           // First local row
	for( k = 1; k < localDim - 1; ++k ) {
		lower[k-1] = -one; diag[k] = diagTerm; upper[k] = -one;                        // Middle local rows
	}
	lower[k-1] = -one; diag[k] = diagTerm; if(procRank < numProc-1) upper[k] = -one; // Last local row
	RefCountPtr<const TSFCore::LinearOp<Scalar> >
		A = rcp(new MPITridiagLinearOp<Scalar>(mpiComm,localDim,&lower[0],&diag[0],&upper[0]));
	if(verbose) out << "\nGlobal dimension of A = " << A->domain()->dim() << std::endl;
	// (A.2) Create RHS vector b and set to a random value
	RefCountPtr<TSFCore::Vector<Scalar> > b = A->range()->createMember();
	TSFCore::seed_randomize<Scalar>(0);
	TSFCore::randomize( Scalar(-ST::one()), Scalar(+ST::one()), &*b );
	// (A.3) Create LHS vector x and set to zero
	RefCountPtr<TSFCore::Vector<Scalar> > x = A->domain()->createMember();
	TSFCore::assign( &*x, ST::one() ); // Here we use scalar traits to be general for all scalar types!
	//
	// (B) Solve the linear system with the silly CG solver
	//
	result = sillyCgSolve(TSFCore::LinearOpHandle<Scalar>(A),*b,maxNumIters,tolerance,&*x,verbose?&out:0);
	if(!result) success = false;
	//
	// (C) Check that the linear system was solved to the specified tolerance
	//
	RefCountPtr<TSFCore::Vector<Scalar> > r = A->range()->createMember();                     
	TSFCore::assign(&*r,*b);                                        // r = b
	A->apply(TSFCore::NOTRANS,*x,&*r,Scalar(-ST::one()),ST::one()); // r = -A*x + r
	const ScalarMag r_nrm = TSFCore::norm(*r), b_nrm = TSFCore::norm(*b);
	const ScalarMag rel_err = r_nrm/b_nrm, relaxTol = ScalarMag(10.0)*tolerance;
	result = rel_err <= relaxTol;
	if(!result) success = false;
	if(verbose)
		out
			<< "\n||b-A*x||/||b|| = "<<r_nrm<<"/"<<b_nrm<<" = "<<rel_err<<(result?" <= ":" > ")
			<<"10.0*tolerance = "<<relaxTol<<": "<<(result?"passed":"failed")<<std::endl;
	timer.stop();
	if(verbose) out << "\nTotal time = " << timer.totalElapsedTime() << " sec\n";

	return success;
} // end runCgSolveExample()

//
// Actual main driver program
//
int main(int argc, char *argv[])
{

	using Teuchos::CommandLineProcessor;

	bool success = true;
	bool verbose = true;
	bool result;

	MPI_Init(&argc,&argv);

	MPI_Comm mpiComm = MPI_COMM_WORLD;
	int procRank, numProc;
	MPI_Comm_size( mpiComm, &numProc );
	MPI_Comm_rank( mpiComm, &procRank );

	try {

		//
		// Read in commandline options
		//

		int    localDim    = 500;
		double diagScale   = 1.001;
		double tolerance   = 1e-4;
		int    maxNumIters = 300;

		CommandLineProcessor  clp(false); // Don't throw exceptions

		clp.setOption( "verbose", "quiet", &verbose, "Determines if any output is printed or not." );
		clp.setOption( "local-dim", &localDim, "Local dimension of the linear system." );
		clp.setOption( "diag-scale", &diagScale, "Scaling of the diagonal to improve conditioning." );
		clp.setOption( "tol", &tolerance, "Relative tolerance for linear system solve." );
		clp.setOption( "max-num-iters", &maxNumIters, "Maximum of CG iterations." );

		CommandLineProcessor::EParseCommandLineReturn parse_return = clp.parse(argc,argv);
		if( parse_return != CommandLineProcessor::PARSE_SUCCESSFUL ) return parse_return;

		TEST_FOR_EXCEPTION( localDim < 2, std::logic_error, "Error, localDim=" << localDim << " < 2 is not allowed!" );

		// Run using float
		result = runCgSolveExample<float>(mpiComm,procRank,numProc,localDim,diagScale,verbose,tolerance,maxNumIters);
		if(!result) success = false;

		// Run using double
		result = runCgSolveExample<double>(mpiComm,procRank,numProc,localDim,diagScale,verbose,tolerance,maxNumIters);
		if(!result) success = false;

#if defined(HAVE_COMPLEX) && defined(HAVE_TEUCHOS_COMPLEX)

		// Run using std::complex<float>
		result = runCgSolveExample<std::complex<float> >(mpiComm,procRank,numProc,localDim,diagScale,verbose,tolerance,maxNumIters);
		if(!result) success = false;

		// Run using std::complex<double>
		result = runCgSolveExample<std::complex<double> >(mpiComm,procRank,numProc,localDim,diagScale,verbose,tolerance,maxNumIters);
		if(!result) success = false;

#endif		

	}
	catch( const std::exception &excpt ) {
		std::cerr << "*** Caught standard exception : " << excpt.what() << std::endl;
		success = false;
	}
	catch( ... ) {
		std::cerr << "*** Caught an unknown exception\n";
		success = false;
	}

	if ( verbose && procRank==0 ) {
		if(success)   std::cout << "\nCongratulations! All of the tests checked out!\n";
		else          std::cout << "\nOh no! At least one of the tests failed!\n";
	}

 	MPI_Finalize();
	
	return success ? 0 : 1;

} // end main()
