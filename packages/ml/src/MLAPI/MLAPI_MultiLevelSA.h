#ifndef MLAPI_MULTILEVEL_H
#define MLAPI_MULTILEVEL_H

#include "ml_include.h"
#include "ml_agg_genP.h"
#include "MLAPI_Error.h"
#include "MLAPI_CompObject.h"
#include "MLAPI_TimeObject.h"
#include "MLAPI_Operator.h"
#include "MLAPI_Operator_Utils.h"
#include "MLAPI_MultiVector.h"
#include "MLAPI_InverseOperator.h"
#include "MLAPI_Expressions.h"
#include "MLAPI_BaseOperator.h"
#include "MLAPI_Workspace.h"
#include "MLAPI_Aggregation.h"
#include "MLAPI_Eig.h"
#include <vector>

namespace MLAPI {

/*!
\class MultiLevelSA

\brief Black-box multilevel smoothed aggregation preconditioner.

\author Marzio Sala, SNL 9214

\date Last updated on Feb-05.

*/

class MultiLevelSA : public BaseOperator, public CompObject, public TimeObject {

public:

  // @{ \name Constructors and destructors

  //! Constructs the hierarchy for given Operator and parameters.
  MultiLevelSA(const Operator FineMatrix, Teuchos::ParameterList& List)
  {
    ResetTimer();

    FineMatrix_ = FineMatrix;

    // get parameter from the input list
    int         MaxLevels     = List.get("max levels", 10);
    double      Damping       = List.get("aggregation: damping factor", 1.3333);
    string      EigenAnalysis = List.get("eigen-analysis: type", "Anorm");
    int         MaxCoarseSize = List.get("coarse: max size", 32);
    MultiVector EmptySpace;
    MultiVector ThisNS        = List.get("aggregation: null space", EmptySpace);
    int         NumPDEEqns    = List.get("PDE equations", 1);
    string      SmootherType  = List.get("smoother: type", "symmetric Gauss-Seidel");
    string      CoarseType    = List.get("coarse: type", "Amesos-KLU");
    
    // build up the default null space
    if (ThisNS.GetNumVectors() == 0) {
      ThisNS.Reshape(FineMatrix.GetDomainSpace(),1);
      ThisNS = 1.0;
    }

    MultiVector NextNS;     // contains the next-level null space

    A_.resize(MaxLevels);
    R_.resize(MaxLevels);
    P_.resize(MaxLevels);
    S_.resize(MaxLevels);

    // work on increasing hierarchies only.
    A_[0] = FineMatrix;

    double LambdaMax;
    Operator A;
    Operator C;
    Operator R;
    Operator P;
    Operator Ptent;
    Operator IminusA;
    InverseOperator S;

    int level;

    for (level = 0 ; level < MaxLevels - 1 ; ++level) {

      // only an alias
      A = A_[level];

      if (level)
        List.set("PDE equations", ThisNS.GetNumVectors());

      if (GetPrintLevel()) {
      ML_print_line("-", 80);
        cout << "current working level   = " << level << endl;
        cout << "number of global rows   = " << A.GetNumGlobalRows() << endl;
        cout << "number of global nnz    = " << A.GetNumGlobalNonzeros() << endl;
        cout << "threshold               = " << List.get("aggregation: threshold", 0.0) << endl;
        cout << "number of PDE equations = " << NumPDEEqns << endl;
        cout << "null space dimension    = " << ThisNS.GetNumVectors() << endl;
      }

      // load current level into database
      List.set("workspace: current level", level);

      GetPtent(A, List, ThisNS, Ptent, NextNS);
      ThisNS = NextNS;
      
      if (EigenAnalysis == "Anorm")
        LambdaMax = MaxEigAnorm(A,true);
      else if (EigenAnalysis == "cg")
        LambdaMax = MaxEigCG(A,true);
      else if (EigenAnalysis == "power-method")
        LambdaMax = MaxEigPowerMethod(A,true);
      else
        ML_THROW("incorrect parameter (" + EigenAnalysis + ")", -1);

      if (GetPrintLevel()) {
        cout << "omega                   = " << Damping << endl;
        cout << "lambda max              = " << LambdaMax << endl;
        cout << "damping factor          = " << Damping / LambdaMax << endl;
        cout << "smoother type           = " << SmootherType << endl;
        cout << "relaxation sweeps       = " << List.get("smoother: sweeps", 1) << endl;
        cout << "smoother damping        = " << List.get("smoother: damping factor", 0.67) << endl;
      }

      /* this is valid as well
      MultiVector Diag;
      Diag = GetDiagonal(A);
      Diag = (Damping / LambdaMax) / Diag;
      Operator Dinv = GetDiagonal(A.GetDomainSpace(),A.GetRangeSpace(),Diag);
      Operator I = Identity(A.GetDomainSpace(),A.GetRangeSpace());
      Operator DinvA = Dinv * A;
      Operator IminusA = I - (Damping / LambdaMax) * DinvA;
      */
      if (Damping) {
        IminusA = GetJacobiIterationOperator(A,Damping / LambdaMax);
        P = IminusA * Ptent;
      }
      else
        P = Ptent;

      R = GetTranspose(P);
      C = GetRAP(R,A,P);
      // build smoothers
      S.Reshape(A, SmootherType, List);

      // put operators and inverse in hierarchy
      R_[level    ] = R;
      P_[level    ] = P;
      A_[level + 1] = C;
      S_[level    ] = S;

      // break if coarse matrix is below specified tolerance
      if (C.GetNumGlobalRows() <= MaxCoarseSize) {
        ++level;
        break;
      }
    }

    if (GetPrintLevel()) {
      ML_print_line("-", 80);
      cout << "final level             = " << level << endl;
      cout << "number of global rows   = " << A_[level].GetNumGlobalRows() << endl;
      cout << "number of global nnz    = " << A_[level].GetNumGlobalNonzeros() << endl;
      cout << "coarse solver           = " << CoarseType << endl;
    }

    // set coarse solver
    S.Reshape(A_[level], CoarseType, List);
    S_[level] = S;
    MaxLevels_ = level + 1;

    if (GetPrintLevel())
      ML_print_line("-", 80);

    // set the label
    SetLabel("SA, L = " + GetString(level) +
             " smoother = " + SmootherType);

    // FIXME: update flops!
    UpdateTime();
  }

  //! Destructor.
  virtual ~MultiLevelSA()
  { }

  // @}
  // @{ \name Set and Get methods

  //! Returns a copy of the internally stored domain space.
  const Space GetOperatorDomainSpace() const 
  {
    return(FineMatrix_.GetDomainSpace());
  }

  //! Returns a copy of the internally stored range space.
  const Space GetOperatorRangeSpace() const 
  {
    return(FineMatrix_.GetRangeSpace());
  }

  //! Returns a copy of the internally stored domain space.
  inline const Space GetDomainSpace() const 
  {
    return(FineMatrix_.GetDomainSpace());
  }

  //! Returns a copy of the internally stored range space.
  inline const Space GetRangeSpace() const 
  {
    return(FineMatrix_.GetRangeSpace());
  }

  //! Returns a reference to the restriction operator of level \c i.
  inline const Operator& R(const int i) const
  {
    return(R_[i]);
  }

  //! Returns a reference to the operator of level \c i.
  inline const Operator& A(const int i) const
  {
    return(A_[i]);
  }

  //! Returns a reference to the prolongator operator of level \c i.
  inline const Operator& P(const int i) const
  {
    return(P_[i]);
  }

  //! Returns a reference to the inverse operator of level \c i.
  inline const InverseOperator& S(const int i) const
  {
    return(S_[i]);
  }

  //! Returns the actual number of levels
  inline int GetMaxLevels() const
  {
    return(MaxLevels_);
  }

  // @}
  // @{ \name Mathematical methods

  //! Applies the preconditioner to \c b_f, returns the result in \c x_f.
  int Apply(const MultiVector& b_f, MultiVector& x_f) const
  {
    ResetTimer();
    SolveMultiLevelSA(b_f,x_f,0);
    UpdateTime();

    return(0);
  }

  //! Recursively called core of the multi level preconditioner.
  int SolveMultiLevelSA(const MultiVector& b_f,MultiVector& x_f, int level) const 
  {
    if (level == MaxLevels_ - 1) {
      x_f = S(level) * b_f;
      return(0);
    }

    MultiVector r_f(P(level).GetRangeSpace());
    MultiVector r_c(P(level).GetDomainSpace());
    MultiVector z_c(P(level).GetDomainSpace());

    // reset flop counter
    S(level).SetFlops(0.0);
    A(level).SetFlops(0.0);
    R(level).SetFlops(0.0);
    P(level).SetFlops(0.0);
    
    // apply pre-smoother
    x_f = S(level) * b_f;
    // new residual
    r_f = b_f - A(level) * x_f;
    // restrict to coarse
    r_c = R(level) * r_f;
    // solve coarse problem
    SolveMultiLevelSA(r_c,z_c,level + 1);
    // prolongate back and add to solution
    x_f = x_f + P(level) * z_c;
    // apply post-smoother
    S(level).Apply(b_f,x_f); 

    UpdateFlops(2.0 * S(level).GetFlops());
    UpdateFlops(A(level).GetFlops());
    UpdateFlops(R(level).GetFlops());
    UpdateFlops(P(level).GetFlops());
    UpdateFlops(2.0 * x_f.GetGlobalLength());
    
    return(0);
  }

  // @}
  // @{ \name Miscellaneous methods

  //! Prints basic information about \c this preconditioner.
  std::ostream& Print(std::ostream& os, 
                      const bool verbose = true) const
  {
    if (GetMyPID() == 0) {
      os << endl;
      os << "*** MLAPI::MultiLevelSA, label = `" << GetLabel() << "'" << endl;
      os << endl;
      os << "Number of levels = " << GetMaxLevels() << endl;
      os << "Flop count       = " << GetFlops() << endl;
      os << "Cumulative time  = " << GetTime() << endl;
      os << "MFlops rate      = " << 1.0e-6 * GetFlops() / GetTime() << endl;
      os << endl;
    }
    return(os);
  }

  // @}

private:

  //! Maximum number of levels.
  int MaxLevels_;
  //! Fine-level matrix.
  Operator FineMatrix_;
  //! Contains the hierarchy of operators.
  vector<Operator> A_;
  //! Contains the hierarchy of restriction operators.
  vector<Operator> R_;
  //! Contains the hierarchy of prolongator operators.
  vector<Operator> P_;
  //! Contains the hierarchy of inverse operators.
  vector<InverseOperator> S_;
  //! Contains the hierarchy of inverse operators.

};

} // namespace MLAPI

#endif
