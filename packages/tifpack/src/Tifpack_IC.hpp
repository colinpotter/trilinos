/*@HEADER
// ***********************************************************************
// 
//       Tifpack: Tempated Object-Oriented Algebraic Preconditioner Package
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
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ***********************************************************************
//@HEADER
*/

#ifndef TIFPACK_IC_HPP
#define TIFPACK_IC_HPP

#include "Tifpack_ConfigDefs.hpp"
#include "Tifpack_CondestType.hpp"
#include "Tifpack_ScalingType.hpp"
#include "Tifpack_Preconditioner.hpp"
#include "Tpetra_Vector.hpp"
#include "Tpetra_CrsMatrix.hpp"
#include "Tpetra_RowMatrix.hpp"
#include "Teuchos_RefCountPtr.hpp"

class Tpetra_Comm;
class Tpetra_Map;
class Tpetra_MultiVector;
namespace Teuchos {
  class ParameterList;
}

//! Tifpack_IC: A class for constructing and using an incomplete Cholesky factorization of a given Tpetra_RowMatrix.

/*! The Tifpack_IC class computes a threshold based incomplete 
 LDL^T factorization of a given Tpetra_RowMatrix.  The factorization 
 that is produced is a function of several parameters:
<ol>
  <li> Maximum number of entries per row/column in factor - The factorization will contain at most this number of nonzero
       terms in each row/column of the factorization.

  <li> Diagonal perturbation - Prior to computing the factorization, it is possible to modify the diagonal entries of the matrix
       for which the factorization will be computing.  If the absolute and relative perturbation values are zero and one,
       respectively, the
       factorization will be compute for the original user matrix A.  Otherwise, the factorization
       will computed for a matrix that differs from the original user matrix in the diagonal values only. Details can be found in \ref ifp_diag_pert.
</ol>

*/    

class Tifpack_IC: public Tifpack_Preconditioner {
      
 public:
  //! Tifpack_IC constuctor with variable number of indices per row.
  /*! Creates a Tifpack_IC object and allocates storage.  
    
    \param In 
           A - User matrix to be factored.
    \param In
           Graph - Graph generated by Tifpack_IlukGraph.
  */
  Tifpack_IC(Tpetra_RowMatrix* A);
  
  //! Tifpack_IC Destructor
  virtual ~Tifpack_IC();

  //! Set absolute threshold value
  void SetAbsoluteThreshold( double Athresh) {Athresh_ = Athresh; return;}

  //! Set relative threshold value
  void SetRelativeThreshold( double Rthresh) {Rthresh_ = Rthresh; return;}

  //! Set parameters using a Teuchos::ParameterList object.
  /* This method is only available if the Teuchos package is enabled.
     This method recognizes five parameter names: level_fill, drop_tolerance,
     absolute_threshold, relative_threshold and overlap_mode. These names are
     case insensitive. For level_fill the ParameterEntry must have type int, the 
     threshold entries must have type double and overlap_mode must have type
     Tpetra_CombineMode.
  */
  int SetParameters(Teuchos::ParameterList& parameterlis);
  
  int SetParameter(const string Name, const int Value)
  {
    TIFPACK_CHK_ERR(-98);
  }
  int SetParameter(const string Name, const double Value)
  {
    TIFPACK_CHK_ERR(-98);
  }

  const Tpetra_RowMatrix& Matrix() const
  {
    return(*A_);
  }

  Tpetra_RowMatrix& Matrix()
  {
    return(*A_);
  }

  bool IsInitialized() const
  {
    return(IsInitialized_);
  }

  //! Initialize L and U with values from user matrix A.
  /*! Copies values from the user's matrix into the nonzero pattern of L and U.
    \param In 
           A - User matrix to be factored.
    \warning The graph of A must be identical to the graph passed in to Tifpack_IlukGraph constructor.
             
   */
  int Initialize();

  //! Compute IC factor U using the specified graph, diagonal perturbation thresholds and relaxation parameters.
  /*! This function computes the RILU(k) factors L and U using the current:
    <ol>
    <li> Tifpack_IlukGraph specifying the structure of L and U.
    <li> Value for the RILU(k) relaxation parameter.
    <li> Value for the \e a \e priori diagonal threshold values.
    </ol>
    InitValues() must be called before the factorization can proceed.
   */
  int Compute();
  int ComputeSetup();

  //! If factor is completed, this query returns true, otherwise it returns false.
  bool IsComputed() const {return(IsComputed_);};

  // Mathematical functions.
  
  //! Returns the result of a Tifpack_IC forward/back solve on a Tpetra_MultiVector X in Y.
  /*! 
    \param In
    Trans -If true, solve transpose problem.
    \param In
    X - A Tpetra_MultiVector of dimension NumVectors to solve for.
    \param Out
    Y -A Tpetra_MultiVector of dimension NumVectorscontaining result.
    
    \return Integer error code, set to 0 if successful.
  */
  int ApplyInverse(const Tpetra_MultiVector& X, Tpetra_MultiVector& Y) const;

  int Apply(const Tpetra_MultiVector& X, Tpetra_MultiVector& Y) const;

  //! Returns the maximum over all the condition number estimate for each local ILU set of factors.
  /*! This functions computes a local condition number estimate on each processor and return the
      maximum over all processor of the estimate.
   \param In
    Trans -If true, solve transpose problem.
    \param Out
    ConditionNumberEstimate - The maximum across all processors of 
    the infinity-norm estimate of the condition number of the inverse of LDU.
  */
  double Condest(const Tifpack_CondestType CT = Tifpack_Cheap, 
                 const int MaxIters = 1550, 
                 const double Tol = 1e-9,
		 Tpetra_RowMatrix* Matrix_in = 0);

  double Condest() const
  {
    return(Condest_);
  }

  // Atribute access functions
  
  //! Get absolute threshold value
  double GetAbsoluteThreshold() {return Athresh_;}

  //! Get relative threshold value
  double GetRelativeThreshold() {return Rthresh_;}
    
  //! Returns the number of nonzero entries in the global graph.
  int NumGlobalNonzeros() const {return(U().NumGlobalNonzeros()+D().GlobalLength());};
 
  //! Returns the number of nonzero entries in the local graph.
  int NumMyNonzeros() const {return(U().NumMyNonzeros()+D().MyLength());};
  //! Returns the address of the D factor associated with this factored matrix.
  const Tpetra_Vector & D() const {return(*D_);};
    
  //! Returns the address of the U factor associated with this factored matrix.
  const Tpetra_CrsMatrix & U() const {return(*U_);};

  //@{ \name Additional methods required to support the Tpetra_Operator interface.

    //! If set true, transpose of this operator will be applied.
    /*! This flag allows the transpose of the given operator to be used implicitly.  Setting this flag
        affects only the Apply() and ApplyInverse() methods.  If the implementation of this interface 
	does not support transpose use, this method should return a value of -1.
      
    \param In
	   UseTranspose_in -If true, multiply by the transpose of operator, otherwise just use operator.

    \return Always returns 0.
  */
  int SetUseTranspose(bool UseTranspose_in) {UseTranspose_ = UseTranspose_in; return(0);};

    //! Returns 0.0 because this class cannot compute Inf-norm.
    double NormInf() const {return(0.0);};

    //! Returns false because this class cannot compute an Inf-norm.
    bool HasNormInf() const {return(false);};

    //! Returns the current UseTranspose setting.
    bool UseTranspose() const {return(UseTranspose_);};

    //! Returns the Tpetra_Map object associated with the domain of this operator.
    const Tpetra_Map & OperatorDomainMap() const {return(A_->OperatorDomainMap());};

    //! Returns the Tpetra_Map object associated with the range of this operator.
    const Tpetra_Map & OperatorRangeMap() const{return(A_->OperatorRangeMap());};

    //! Returns the Tpetra_BlockMap object associated with the range of this matrix operator.
    const Tpetra_Comm & Comm() const{return(Comm_);};
  //@}

    const char* Label() const
    {
      return(Label_);
    }

    int SetLabel(const char* Label_in)
    {
      strcpy(Label_,Label_in);
      return(0);
    }
 
  //! Prints basic information on iostream. This function is used by operator<<.
  virtual ostream& Print(std::ostream& os) const;

  //! Returns the number of calls to Initialize().
  virtual int NumInitialize() const
  {
    return(NumInitialize_);
  }

  //! Returns the number of calls to Compute().
  virtual int NumCompute() const
  {
    return(NumCompute_);
  }

  //! Returns the number of calls to ApplyInverse().
  virtual int NumApplyInverse() const
  {
    return(NumApplyInverse_);
  }

  //! Returns the time spent in Initialize().
  virtual double InitializeTime() const
  {
    return(InitializeTime_);
  }

  //! Returns the time spent in Compute().
  virtual double ComputeTime() const
  {
    return(ComputeTime_);
  }

  //! Returns the time spent in ApplyInverse().
  virtual double ApplyInverseTime() const
  {
    return(ApplyInverseTime_);
  }

  //! Returns the number of flops in the initialization phase.
  virtual double InitializeFlops() const
  {
    return(0.0);
  }

  virtual double ComputeFlops() const
  {
    return(ComputeFlops_);
  }

  virtual double ApplyInverseFlops() const
  {
    return(ApplyInverseFlops_);
  }


 private:
  
  double LevelOfFill() const
  {
    return(Lfil_);
  }

  double AbsoluteThreshold() const
  {
    return(Athresh_);
  }

  double RelativeThreshold() const
  {
    return(Rthresh_);
  }

  double DropTolerance() const
  {
    return(Droptol_);
  }

  Teuchos::RefCountPtr<Tpetra_RowMatrix> A_;
  const Tpetra_Comm & Comm_;
  Teuchos::RefCountPtr<Tpetra_CrsMatrix> U_;
  Teuchos::RefCountPtr<Tpetra_Vector> D_;
  bool UseTranspose_;

  double Condest_;
  double Athresh_;
  double Rthresh_;
  double Droptol_;
  int Lfil_;

  void * Aict_;
  void * Lict_;
  double * Ldiag_;
  char Label_[160];

  bool IsInitialized_;
  bool IsComputed_;
 
  //! Contains the number of successful calls to Initialize().
  int NumInitialize_;
  //! Contains the number of successful call to Compute().
  int NumCompute_;
  //! Contains the number of successful call to ApplyInverse().
  mutable int NumApplyInverse_;

  //! Contains the time for all successful calls to Initialize().
  double InitializeTime_;
  //! Contains the time for all successful calls to Compute().
  double ComputeTime_;
  //! Contains the time for all successful calls to ApplyInverse().
  mutable double ApplyInverseTime_;

  //! Contains the number of flops for Compute().
  double ComputeFlops_;
  //! Contain sthe number of flops for ApplyInverse().
  mutable double ApplyInverseFlops_;

};

#endif /* TIFPACK_IC_HPP */
