#ifndef AMESOS2_EXPLICITINSTANTIATIONHELPER_HPP
#define AMESOS2_EXPLICITINSTANTIATIONHELPER_HPP

#include <Tpetra_CrsMatrix.hpp>

#ifdef HAVE_AMESOS2_EPETRA
#include <Epetra_CrsMatrix.h>

#define AMESOS2_SOLVER_EPETRA_INST(SOLVERNAME) \
  template class SOLVERNAME<Epetra_CrsMatrix, Epetra_MultiVector>
#endif	// HAVE_AMESOS2_EPETRA

#define AMESOS2_SOLVER_TPETRA_INST(SOLVERNAME,S,LO,GO) \
  template class SOLVERNAME<Tpetra::CrsMatrix<S,LO,GO, \
					      Kokkos::DefaultNode::DefaultNodeType, \
					      Kokkos::DefaultKernels<S, \
								     LO, \
								     Kokkos::DefaultNode::DefaultNodeType>::SparseOps>, \
			    Tpetra::MultiVector<S,LO,GO, \
						Kokkos::DefaultNode::DefaultNodeType> >

#endif	// AMESOS2_EXPLICITINSTANTIATIONHELPER_HPP
