// ///////////////////////////////
// cxx_main.cpp

#include "TSFCoreEpetraVectorSpace.hpp"
#include "TSFCoreEpetraLinearOp.hpp"
#include "TSFCoreEpetraMultiVector.hpp"
#include "TSFCoreTestingTools.hpp"
#include "Epetra_SerialComm.h"
#include "Epetra_LocalMap.h"
#include "Epetra_CrsMatrix.h"
#include "Epetra_MultiVector.h"
#include "dynamic_cast_verbose.hpp"
#include "Teuchos_CommandLineProcessor.hpp"
#include "Teuchos_Time.hpp"
#include "Teuchos_oblackholestream.hpp"
#ifdef RTOp_USE_MPI
#  include "TSFCoreSimpleMPIVectorSpace.hpp"
#  include "Epetra_MpiComm.h"
#endif

// Define this if you want to see only Epetra-based computations
//#define EPETRA_ADAPTERS_EPETRA_ONLY

// Define this if you want to exclude opeations with Epetra_Operator
#define EPETRA_ADAPTERS_EXCLUDE_EPETRA_OPERATOR

//
// Some helper functions
//

namespace {

double relErr( const double& v1, const double& v2 )
{
	return std::fabs(v1 - v2) / ( 1.0 + std::fabs(v1) + std::fabs(v2) );
}

bool testRelErr(
	const std::string    &v1_name
	,const double        &v1
	,const std::string   &v2_name
	,const double        &v2
	,const std::string   &maxRelErr_name
	,const double        &maxRelErr
	,bool                verbose
	,std::ostream        &out
	)
{
	const double rel_err = relErr( v1, v2 );
	const bool success = ( rel_err <= maxRelErr );
	if(verbose) {
		out << "\nCheck: rel_err(" << v1_name << "," << v2_name << ")\n"
			<< "       = rel_err(" << v1 << "," << v2 << ") "
			<< "= " << rel_err
			<< " <= " << maxRelErr_name << " = " << maxRelErr << " : "
			<<  (success ? "passed!" : "failed!") << std::endl;
	}
	return success;
}

} // namespace

namespace TSFCore {

///
/** Testing program for TSFCore/Epetra adpaters.
 *
 * This testing program shows how you can easily mix and match
 * different implementations of vectors and multi-vectors for serial
 * and SPMD MPI implementations.  This code is worth study to show how
 * this is done.
 */
int main_body( int argc, char* argv[] ) {

	typedef double Scalar;

	using DynamicCastHelperPack::dyn_cast;
	using Teuchos::CommandLineProcessor;
	using Teuchos::RefCountPtr;
	using Teuchos::rcp;
	using Teuchos::rcp_static_cast;
	using Teuchos::rcp_const_cast;
	using Teuchos::set_extra_data;
	
	bool verbose = true;
	bool dumpAll = false;
#ifdef RTOp_USE_MPI
	bool useMPI  = true;
#else
	bool useMPI  = false;
#endif
	bool success = true;
	bool result;

	int procRank = 0;

	const Scalar err_tol = 1e-10; // Todo: Make this adjustable!

	Scalar rel_err;

	MPI_Init(&argc,&argv);
	
	try {

		Teuchos::Time timer("");

		//
		// Read options from the commandline
		//

		int     local_dim         = 4;
		int     num_mv_cols       = 4;
		double  max_rel_err       = 1e-13;
		double  scalar            = 1.0;

		CommandLineProcessor  clp(false); // Don't throw exceptions
		clp.setOption( "verbose", "quiet", &verbose, "Determines if any output is printed or not." );
		clp.setOption( "dump-all", "no-dump", &dumpAll, "Determines if quantities are dumped or not." );
		clp.setOption( "local-dim", &local_dim, "Number of vector elements per process." );
		clp.setOption( "num-mv-cols", &num_mv_cols, "Number columns in each multi-vector (>=4)." );
		clp.setOption( "max-rel-err", &max_rel_err, "Maximum relative error for tests." );
		clp.setOption( "scalar", &scalar, "A scalar used in all computations." );
		CommandLineProcessor::EParseCommandLineReturn parse_return = clp.parse(argc,argv);
		if( parse_return != CommandLineProcessor::PARSE_SUCCESSFULL ) return parse_return;

		TEST_FOR_EXCEPTION(
			num_mv_cols < 4, std::logic_error
			,"Error, num-mv-cols must be >= 4!"
			);

		//
		// Get basic MPI info
		//

		MPI_Comm mpiComm = MPI_COMM_NULL;
		int numProc = 1;
#ifdef RTOp_USE_MPI
		mpiComm = MPI_COMM_WORLD;
		MPI_Comm_size( mpiComm, &numProc );
		MPI_Comm_rank( mpiComm, &procRank );
#endif

		//
		// Setup the output stream
		//
		
		Teuchos::oblackholestream black_hole_out;
		std::ostream &out = ( procRank == 0 ? std::cout : black_hole_out );

		if(verbose)
			out
				<< "\n***"
				<< "\n*** Creating two vector spaces (an Epetra-based and a non-Epetra-based)"
				<< "\n***\n";

		//
		// Create two different vector spaces (one Epetra and one non-Epetra)
		// that should be compatible
		//
		RefCountPtr<const Epetra_Comm> epetra_comm;
		RefCountPtr<const Epetra_Map> epetra_map;
		RefCountPtr<const VectorSpace<Scalar> > epetra_vs;
#ifndef EPETRA_ADAPTERS_EPETRA_ONLY
		RefCountPtr<const VectorSpace<Scalar> > non_epetra_vs;
#endif
#ifdef RTOp_USE_MPI
		if(useMPI) {
			//
			// Create parallel vector spaces using compatible maps
			//
			if(verbose)
				out << "\nCreating TSFCore::EpetraVectorSpace using Epetra_MpiComm ...\n";
			epetra_comm = rcp(new Epetra_MpiComm(mpiComm));
			epetra_map = rcp(new Epetra_Map(-1,local_dim,0,*epetra_comm));
			epetra_vs = rcp(new EpetraVectorSpace(epetra_map));
#ifndef EPETRA_ADAPTERS_EPETRA_ONLY
			// Non-Epetra vector space
			if(verbose)
				out << "\nCreating TSFCore::SimpleMPIVectorSpace ...\n";
			non_epetra_vs = rcp(new SimpleMPIVectorSpace<Scalar>(mpiComm,local_dim));
#endif
		}
		else {
#endif
			//
			// Create serial vector spaces (i.e. VectorSpace::isInCore()==true)
			//
			// Epetra vector space
			if(verbose)
				out << "\nCreating TSFCore::EpetraVectorSpace using Epetra_SerialComm ...\n";
			epetra_comm = rcp(new Epetra_SerialComm);
			epetra_map = rcp(new Epetra_LocalMap(local_dim,0,*epetra_comm));
			epetra_vs = rcp(new EpetraVectorSpace(epetra_map));
#ifndef EPETRA_ADAPTERS_EPETRA_ONLY
			if(verbose)
				out << "\nCreating TSFCore::SerialVectorSpace ...\n";
			non_epetra_vs = rcp(new SerialVectorSpace<Scalar>(local_dim));
#endif
#ifdef RTOp_USE_MPI
		}
#endif

		const int global_dim = local_dim * numProc;

		if(verbose)
			out
				<< "\nscalar              = " << scalar
				<< "\nlocal_dim           = " << local_dim
				<< "\nglobal_dim          = " << global_dim
				<< "\nnum_mv_cols         = " << num_mv_cols
				<< "\nepetra_vs.dim()     = " << epetra_vs->dim()
				<< "\nnon_epetra_vs.dim() = " << non_epetra_vs->dim()
				<< std::endl;


		//
		// Create vectors and multi-vectors from each vector space
		//

		RefCountPtr<Vector<Scalar> >
			ev1 = epetra_vs->createMember(),
			ev2 = epetra_vs->createMember();
#ifndef EPETRA_ADAPTERS_EPETRA_ONLY
		RefCountPtr<Vector<Scalar> >
			nev1 = non_epetra_vs->createMember(),
			nev2 = non_epetra_vs->createMember();
#endif

		RefCountPtr<MultiVector<Scalar> >
			eV1 = epetra_vs->createMembers(num_mv_cols),
			eV2 = epetra_vs->createMembers(num_mv_cols);
#ifndef EPETRA_ADAPTERS_EPETRA_ONLY
		RefCountPtr<MultiVector<Scalar> >
			neV1 = non_epetra_vs->createMembers(num_mv_cols),
			neV2 = non_epetra_vs->createMembers(num_mv_cols);
#endif

		//
		// Check for compatibility of the vector and Multi-vectors
		// w.r.t. RTOps
		//

		if(verbose)
			out
				<< "\n***"
				<< "\n*** Testing individual vector/multi-vector RTOps"
				<< "\n***\n";

		assign( &*ev1, 0.0 );
		assign( &*ev2, scalar );
#ifndef EPETRA_ADAPTERS_EPETRA_ONLY
		assign( &*nev1, 0.0 );
		assign( &*nev2, scalar );
#endif
		assign( &*eV1, 0.0 );
		assign( &*eV2, scalar );
#ifndef EPETRA_ADAPTERS_EPETRA_ONLY
		assign( &*neV1, 0.0 );
		assign( &*neV2, scalar );
#endif

		Scalar
			ev1_nrm = norm_1(*ev1),
			ev2_nrm = norm_1(*ev2),
			eV1_nrm = norm_1(*eV1),
			eV2_nrm = norm_1(*eV2);
#ifndef EPETRA_ADAPTERS_EPETRA_ONLY
		Scalar
			nev1_nrm = norm_1(*nev1),
			nev2_nrm = norm_1(*nev2),
			neV1_nrm = norm_1(*neV1),
			neV2_nrm = norm_1(*neV2);
#endif

		const std::string s1_n = "fabs(scalar)*global_dim";
		const double s1 = fabs(scalar)*global_dim;
		
		testRelErr("norm_1(ev1)",ev1_nrm,"0",0,"max_rel_err",max_rel_err,verbose,out) || (success=false);
		testRelErr("norm_1(ev2)",ev2_nrm,s1_n,s1,"max_rel_err",max_rel_err,verbose,out) || (success=false);
#ifndef EPETRA_ADAPTERS_EPETRA_ONLY
		testRelErr("norm_1(nev1)",nev1_nrm,"0",0,"max_rel_err",max_rel_err,verbose,out) || (success=false);
		testRelErr("norm_1(nev2)",nev2_nrm,s1_n,s1,"max_rel_err",max_rel_err,verbose,out) || (success=false);
#endif
		testRelErr("norm_1(eV1)",eV1_nrm,"0",0,"max_rel_err",max_rel_err,verbose,out) || (success=false);
		testRelErr("norm_1(eV2)",eV2_nrm,s1_n,s1,"max_rel_err",max_rel_err,verbose,out) || (success=false);
#ifndef EPETRA_ADAPTERS_EPETRA_ONLY
		testRelErr("norm_1(neV1)",neV1_nrm,"0",0,"max_rel_err",max_rel_err,verbose,out) || (success=false);
		testRelErr("norm_1(neV2)",neV2_nrm,s1_n,s1,"max_rel_err",max_rel_err,verbose,out) || (success=false);
#endif

		if(verbose)
			out
				<< "\n***"
				<< "\n*** Test RTOps with two or more arguments"
				<< "\n***\n";

		if(verbose) out << "\nPerforming ev1 = ev2 ...\n";
		timer.start();
 		assign( &*ev1, *ev2 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		testRelErr("norm_1(ev1)",norm_1(*ev1),"norm_1(ev2)",ev2_nrm,"max_rel_err",max_rel_err,verbose,out) || (success=false);

		if(verbose) out << "\nPerforming eV1 = eV2 ...\n";
		timer.start();
 		assign( &*eV1, *eV2 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		testRelErr("norm_1(eV1)",norm_1(*eV1),"norm_1(eV2)",eV2_nrm,"max_rel_err",max_rel_err,verbose,out) || (success=false);

#ifndef EPETRA_ADAPTERS_EPETRA_ONLY

		if(verbose) out << "\nPerforming ev1 = nev2 ...\n";
		timer.start();
 		assign( &*ev1, *nev2 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		testRelErr("norm_1(ev1)",norm_1(*ev1),"norm_1(nev2)",nev2_nrm,"max_rel_err",max_rel_err,verbose,out) || (success=false);

		if(verbose) out << "\nPerforming nev1 = ev2 ...\n";
		timer.start();
 		assign( &*nev1, *ev2 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		testRelErr("norm_1(nev1)",norm_1(*nev1),"norm_1(ev2)",ev2_nrm,"max_rel_err",max_rel_err,verbose,out) || (success=false);

		if(verbose) out << "\nPerforming nev1 = nev2 ...\n";
		timer.start();
 		assign( &*nev1, *nev2 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		testRelErr("norm_1(nev1)",norm_1(*nev1),"norm_1(nev2)",nev2_nrm,"max_rel_err",max_rel_err,verbose,out) || (success=false);

		if(verbose) out << "\nPerforming eV1 = neV2 ...\n";
		timer.start();
 		assign( &*eV1, *neV2 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		testRelErr("norm_1(eV1)",norm_1(*eV1),"norm_1(neV2)",neV2_nrm,"max_rel_err",max_rel_err,verbose,out) || (success=false);

		if(verbose) out << "\nPerforming neV1 = eV2 ...\n";
		timer.start();
 		assign( &*neV1, *eV2 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		testRelErr("norm_1(neV1)",norm_1(*neV1),"norm_1(eV2)",eV2_nrm,"max_rel_err",max_rel_err,verbose,out) || (success=false);

		if(verbose) out << "\nPerforming neV1 = neV2 ...\n";
		timer.start();
 		assign( &*neV1, *neV2 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		testRelErr("norm_1(neV1)",norm_1(*neV1),"norm_1(neV2)",neV2_nrm,"max_rel_err",max_rel_err,verbose,out) || (success=false);

#endif

		const std::string s2_n = "scalar^2*global_dim*num_mv_cols";
		const double s2 = scalar*scalar*global_dim*num_mv_cols;

		RefCountPtr<MultiVector<Scalar> >
			T = eV1->domain()->createMembers(num_mv_cols);

		if(verbose)
			out
				<< "\n***"
				<< "\n*** Test Epetra_MultiVector::Multiply(...) using raw Epetra objects"
				<< "\n***\n";

		if(1) {

			// Get constant references to Epetra_MultiVector objects in eV1 and eV2
			const Epetra_MultiVector
				&eeV1
				= *dyn_cast<const EpetraMultiVector>(const_cast<const MultiVector<Scalar>&>(*eV1)).epetra_multi_vec();
			const Epetra_MultiVector
				&eeV2
				= *dyn_cast<const EpetraMultiVector>(const_cast<const MultiVector<Scalar>&>(*eV2)).epetra_multi_vec();

			// Get the Epetra_MultiVector object inside of T to be modified.  Note that the following is the recommended
			// way to do this since it gives the greatest flexibility in the implementation of TSFCore::EpetraMultiVector.
			RefCountPtr<Epetra_MultiVector>  eT;
			RefCountPtr<const EpetraVectorSpace> eT_range, eT_domain;
			dyn_cast<EpetraMultiVector>(*T).setUninitialized(&eT,&eT_range,&eT_domain);

			if(verbose) out << "\nPerforming eeV1'*eeV2 ...\n";
			timer.start();
			eT->Multiply( 'T', 'N', 1.0, eeV1, eeV2, 0.0 );
			timer.stop();
			if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";

			dyn_cast<EpetraMultiVector>(*T).initialize(eT,eT_range,eT_domain);

			testRelErr("norm_1(eeV1'*eeV2)",norm_1(*T),s2_n,s2,"max_rel_err",max_rel_err,verbose,out) || (success=false);
			
		}

		if(verbose)
			out
				<< "\n***"
				<< "\n*** Test MultiVector::apply(...)"
				<< "\n***\n";

		if(verbose) out << "\nPerforming eV1'*eV2 ...\n";
		timer.start();
		eV1->apply( TRANS, *eV2, &*T );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		testRelErr("norm_1(eV1'*eV2)",norm_1(*T),s2_n,s2,"max_rel_err",max_rel_err,verbose,out) || (success=false);
		if(verbose && dumpAll) out << "\neV1'*eV2 =\n" << *T;

#ifndef EPETRA_ADAPTERS_EPETRA_ONLY

		if(verbose) out << "\nPerforming neV1'*eV2 ...\n";
		timer.start();
		neV1->apply( TRANS, *eV2, &*T );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		testRelErr("norm_1(neV1'*eV2)",norm_1(*T),s2_n,s2,"max_rel_err",max_rel_err,verbose,out) || (success=false);
		if(verbose && dumpAll) out << "\nneV1'*eV2 =\n" << *T;

		if(verbose) out << "\nPerforming eV1'*neV2 ...\n";
		timer.start();
		eV1->apply( TRANS, *neV2, &*T );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		testRelErr("norm_1(eV1'*neV2)",norm_1(*T),s2_n,s2,"max_rel_err",max_rel_err,verbose,out) || (success=false);
		if(verbose && dumpAll) out << "\neV1'*neV2 =\n" << *T;

		if(verbose) out << "\nPerforming neV1'*neV2 ...\n";
		timer.start();
		neV1->apply( TRANS, *neV2, &*T );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		testRelErr("norm_1(neV1'*neV2)",norm_1(*T),s2_n,s2,"max_rel_err",max_rel_err,verbose,out) || (success=false);
		if(verbose && dumpAll) out << "\nneV1'*neV2 =\n" << *T;

#endif

#ifndef EPETRA_ADAPTERS_EXCLUDE_EPETRA_OPERATOR

		if(verbose)
			out
				<< "\n***"
				<< "\n*** Creating a diagonal Epetra_Operator"
				<< "\n***\n";

		RefCountPtr<Epetra_Operator>  epetra_op;

		if(1) {
			// Create a diagonal matrix with 0.5 on the diagonal
			RefCountPtr<Epetra_CrsMatrix>
				epetra_mat = rcp(new Epetra_CrsMatrix(::Copy,*epetra_map,1));
			double values[1] = { 0.5 };
			int indices[1];
			const int IB = epetra_map->IndexBase(), offset = procRank*local_dim;
			for( int k = 0; k < local_dim; ++k ) {
				indices[0] = offset + k + IB;  // global column
				epetra_mat->InsertGlobalValues(
					offset + k + IB     // GlobalRow
					,1                  // NumEntries
					,values             // Values
					,indices            // Indices
					);
			}
			epetra_mat->FillComplete();
			epetra_op = epetra_mat;
		}

		RefCountPtr<const LinearOp<Scalar> >
			Op = rcp(new EpetraLinearOp(epetra_op));
		
		if(verbose)
			out
				<< "\n***"
				<< "\n*** Mix and match vector and Multi-vectors with Epetra opeator"
				<< "\n***\n";
		
		RefCountPtr<Vector<Scalar> >
			ey  = epetra_vs->createMember();
		RefCountPtr<MultiVector<Scalar> >
			eY  = epetra_vs->createMembers(num_mv_cols);
#ifndef EPETRA_ADAPTERS_EPETRA_ONLY
		RefCountPtr<Vector<Scalar> >
			ney = non_epetra_vs->createMember();
		RefCountPtr<MultiVector<Scalar> >
			neY = non_epetra_vs->createMembers(num_mv_cols);
#endif

		if(verbose) out << "\nPerforming ey = 2*Op*ev1 ...\n";
		timer.start();
		Op->apply( NOTRANS, *ev1, &*ey, 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(ey) = " << norm_1(*ey) << std::endl;

		if(verbose) out << "\nPerforming eY = 2*Op*eV1 ...\n";
		timer.start();
		Op->apply( NOTRANS, *eV1, &*eY, 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(eY) = " << norm_1(*eY) << std::endl;

#ifndef EPETRA_ADAPTERS_EPETRA_ONLY

		if(verbose) out << "\nPerforming ney = 2*Op*ev1 ...\n";
		timer.start();
		Op->apply( NOTRANS, *ev1, &*ney, 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(ney) = " << norm_1(*ney) << std::endl;

		if(verbose) out << "\nPerforming neY = 2*Op*eV1 ...\n";
		timer.start();
		Op->apply( NOTRANS, *eV1, &*neY, 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(neY) = " << norm_1(*neY) << std::endl;

		if(verbose) out << "\nPerforming ey = 2*Op*nev1 ...\n";
		timer.start();
		Op->apply( NOTRANS, *nev1, &*ey, 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(ey) = " << norm_1(*ey) << std::endl;

		if(verbose) out << "\nPerforming eY = 2*Op*neV1 ...\n";
		timer.start();
		Op->apply( NOTRANS, *neV1, &*eY, 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(eY) = " << norm_1(*eY) << std::endl;

		if(verbose) out << "\nPerforming ney = 2*Op*nev1 ...\n";
		timer.start();
		Op->apply( NOTRANS, *nev1, &*ney, 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(ney) = " << norm_1(*ney) << std::endl;

		if(verbose) out << "\nPerforming neY = 2*Op*neV1 ...\n";
		timer.start();
		Op->apply( NOTRANS, *neV1, &*neY, 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(neY) = " << norm_1(*neY) << std::endl;

#endif

		if(verbose)
			out
				<< "\n***"
				<< "\n*** Testing Multi-vector views with Epetra operator"
				<< "\n***\n";

		const Range1D col_rng(1,2);
		const int numCols = 2;
		const int cols[] = { 3, 4 };

		RefCountPtr<const MultiVector<Scalar> >
			eV1_v1  = rcp_static_cast<const MultiVector<Scalar> >(eV1)->subView(col_rng),
			eV1_v2  = rcp_static_cast<const MultiVector<Scalar> >(eV1)->subView(numCols,cols);
#ifndef EPETRA_ADAPTERS_EPETRA_ONLY
		RefCountPtr<const MultiVector<Scalar> >
			neV1_v1  = rcp_static_cast<const MultiVector<Scalar> >(neV1)->subView(col_rng),
			neV1_v2  = rcp_static_cast<const MultiVector<Scalar> >(neV1)->subView(numCols,cols);
#endif

		if(verbose) out << "\nPerforming eY_v1 = 2*Op*eV1_v1 ...\n";
		timer.start();
		Op->apply( NOTRANS, *eV1_v1, &*eY->subView(col_rng), 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(eY) = " << norm_1(*eY->subView(col_rng)) << std::endl;

		if(verbose) out << "\nPerforming eY_v2 = 2*Op*eV1_v2 ...\n";
		timer.start();
		Op->apply( NOTRANS, *eV1_v2, &*eY->subView(numCols,cols), 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(eY) = " << norm_1(*eY->subView(numCols,cols)) << std::endl;

#ifndef EPETRA_ADAPTERS_EPETRA_ONLY

		if(verbose) out << "\nPerforming neY_v1 = 2*Op*eV1_v1 ...\n";
		timer.start();
		Op->apply( NOTRANS, *eV1_v1, &*neY->subView(col_rng), 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(eY) = " << norm_1(*neY->subView(col_rng)) << std::endl;

		if(verbose) out << "\nPerforming eY_v1 = 2*Op*neV1_v1 ...\n";
		timer.start();
		Op->apply( NOTRANS, *neV1_v1, &*eY->subView(col_rng), 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(eY) = " << norm_1(*eY->subView(col_rng)) << std::endl;

		if(verbose) out << "\nPerforming neY_v2 = 2*Op*eV1_v2 ...\n";
		timer.start();
		Op->apply( NOTRANS, *eV1_v2, &*neY->subView(numCols,cols), 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(eY) = " << norm_1(*neY->subView(numCols,cols)) << std::endl;

		if(verbose) out << "\nPerforming eY_v2 = 2*Op*neV1_v2 ...\n";
		timer.start();
		Op->apply( NOTRANS, *neV1_v2, &*eY->subView(numCols,cols), 2.0 );
		timer.stop();
		if(verbose) out << "  time = " << timer.totalElapsedTime() << " sec\n";
		if(verbose) out << "  norm_1(eY) = " << norm_1(*eY->subView(numCols,cols)) << std::endl;

#endif

#endif // EPETRA_ADAPTERS_EXCLUDE_EPETRA_OPERATOR

		if(verbose) {
			if(success)
				out << "\nCongratulations! All of the tests seem to have run sucessfully!\n";
			else
				out << "\nOh no! at least one of the tests did not check out!\n";
		}

	} // end try
	catch( const std::exception &excpt ) {
		if(verbose)
			std::cerr << "*** Caught a standard exception (procRank="<<procRank<<"): " << excpt.what() << std::endl;
		success = -1;
	}
	catch( ... ) {
		if(verbose)
			std::cerr << "*** Caught an unknown exception (procRank="<<procRank<<")!\n";
		success = -1;
	}

 	MPI_Finalize();

	return (success ? 0 : -1);

}

} // namespace TSFCore

int main( int argc, char* argv[] ) {
	return TSFCore::main_body(argc,argv);
}
