#include "Ifpack_CrsRiluk.h"
#include "Epetra_Object.h" 
#include "Epetra_Comm.h"
#include "Epetra_Map.h"
#include "Epetra_CrsGraph.h"
#include "Epetra_CrsMatrix.h"
#include "Epetra_Vector.h"
#include "Epetra_MultiVector.h"

//==============================================================================
Ifpack_CrsRiluk::Ifpack_CrsRiluk(const Epetra_CrsMatrix &A, const Ifpack_IlukGraph & Graph) 
  : A_(A),
    Graph_(Graph),
    Allocated_(false),
    ValuesInitialized_(false),
    Factored_(false),
    RelaxValue_(0.0),
    ShiftValue_(0.0),
    IndexBase_(Graph.IndexBase()),
    NumGlobalRows_(Graph.NumGlobalRows()),
    NumGlobalCols_(Graph.NumGlobalCols()),
    NumGlobalDiagonals_(Graph.NumGlobalDiagonals()),
    NumGlobalNonzeros_(Graph.NumGlobalNonzeros()),
    NumMyRows_(Graph.NumMyRows()),
    NumMyCols_(Graph.NumMyCols()),
    NumMyDiagonals_(Graph.NumMyDiagonals()),
    NumMyNonzeros_(Graph.NumMyNonzeros()),
    OverlapX_(0),
    OverlapY_(0),
    OverlapMode_(Zero)
{
  int ierr = Allocate();
}

//==============================================================================
Ifpack_CrsRiluk::Ifpack_CrsRiluk(const Ifpack_CrsRiluk & FactoredMatrix) 
  : A_(FactoredMatrix.A_),
    Graph_(FactoredMatrix.Graph_),
    Allocated_(FactoredMatrix.Allocated_),
    ValuesInitialized_(FactoredMatrix.ValuesInitialized_),
    Factored_(FactoredMatrix.Factored_),
    RelaxValue_(FactoredMatrix.RelaxValue_),
    ShiftValue_(FactoredMatrix.ShiftValue_),
    IndexBase_(FactoredMatrix.IndexBase_),
    NumGlobalRows_(FactoredMatrix.NumGlobalRows_),
    NumGlobalCols_(FactoredMatrix.NumGlobalCols_),
    NumGlobalDiagonals_(FactoredMatrix.NumGlobalDiagonals_),
    NumGlobalNonzeros_(FactoredMatrix.NumGlobalNonzeros_),
    NumMyRows_(FactoredMatrix.NumMyRows_),
    NumMyCols_(FactoredMatrix.NumMyCols_),
    NumMyDiagonals_(FactoredMatrix.NumMyDiagonals_),
    NumMyNonzeros_(FactoredMatrix.NumMyNonzeros_),
    OverlapX_(0),
    OverlapY_(0),
    OverlapMode_(FactoredMatrix.OverlapMode_)
{
  L_ = new Epetra_CrsMatrix(FactoredMatrix.L());
  U_ = new Epetra_CrsMatrix(FactoredMatrix.U());
  D_ = new Epetra_Vector(Graph_.L_Graph().RowMap());
  
}

//==============================================================================
int Ifpack_CrsRiluk::Allocate() {

  // Allocate Epetra_CrsMatrix using ILUK graphs
  L_ = new Epetra_CrsMatrix(Copy, Graph_.L_Graph());
  U_ = new Epetra_CrsMatrix(Copy, Graph_.U_Graph());
  D_ = new Epetra_Vector(Graph_.L_Graph().RowMap());
  
  
    SetAllocated(true);
    return(0);
}
//==============================================================================
Ifpack_CrsRiluk::~Ifpack_CrsRiluk(){


  delete L_;
  delete U_;
  delete D_; // Diagonal is stored separately.  We store the inverse.

  if (OverlapX_!=0) delete OverlapX_;
  if (OverlapY_!=0) delete OverlapY_;

  ValuesInitialized_ = false;
  Factored_ = false;
  Allocated_ = false;
}

//==========================================================================
int Ifpack_CrsRiluk::InitValues() {

  // if (!Allocated()) return(-1); // This test is not needed at this time.  All constructors allocate.

  int ierr = 0;
  int i, j;
  int * InI=0, * LI=0, * UI = 0;
  double * InV=0, * LV=0, * UV = 0;
  int NumIn, NumL, NumU;
  bool DiagFound;
  int NumNonzeroDiags = 0;

  Epetra_CrsMatrix * OverlapA = (Epetra_CrsMatrix *) &A_;

  if (Graph_.LevelOverlap()>0 && Graph_.L_Graph().DomainMap().DistributedGlobal()) {
  
  OverlapA = new Epetra_CrsMatrix(Copy, *Graph_.OverlapGraph());
  OverlapA->Import(A_, *Graph_.OverlapImporter(), Insert);
  }

  // Get Maximun Row length
  int MaxNumEntries = OverlapA->MaxNumEntries();

  InI = new int[MaxNumEntries]; // Allocate temp space
  LI = new int[MaxNumEntries];
  UI = new int[MaxNumEntries];
  InV = new double[MaxNumEntries];
  LV = new double[MaxNumEntries];
  UV = new double[MaxNumEntries];

  double *DV;
  ierr = D_->ExtractView(&DV); // Get view of diagonal
    

  // First we copy the user's matrix into L and U, regardless of fill level

  for (i=0; i< NumMyRows_; i++) {

    OverlapA->ExtractMyRowCopy(i, MaxNumEntries, NumIn, InV, InI); // Get Values and Indices
    
    // Split into L and U (we don't assume that indices are ordered).
    
    NumL = 0; 
    NumU = 0; 
    DiagFound = false;
    
    for (j=0; j< NumIn; j++) {
      int k = InI[j];

      if (k==i) {
	DiagFound = true;
	DV[i] += (1.0+ShiftValue_) * InV[j]; // Store shifted diagonal in Epetra_Vector D_
      }

      else if (k < 0) return(-1); // Out of range

      else if (k < i) {
	LI[NumL] = k;
	LV[NumL] = InV[j];
	NumL++;
      }
      else if (k<NumMyRows_) {
	UI[NumU] = k;
	UV[NumU] = InV[j];
	NumU++;
      }
    }
    
    // Check in things for this row of L and U

    if (DiagFound) NumNonzeroDiags++;
    if (NumL) L_->ReplaceMyValues(i, NumL, LV, LI);
    if (NumU) U_->ReplaceMyValues(i, NumU, UV, UI);
    
  }

  delete [] LI;
  delete [] UI;
  delete [] LV;
  delete [] UV;
  delete [] InI;
  delete [] InV;

  if (Graph_.LevelOverlap()>0 && Graph_.L_Graph().DomainMap().DistributedGlobal()) delete OverlapA;


  L_->TransformToLocal();
  U_->TransformToLocal();

  // At this point L and U have the values of A in the structure of L and U, and diagonal vector D

  SetValuesInitialized(true);
  SetFactored(false);

  int TotalNonzeroDiags = 0;
  Graph_.L_Graph().RowMap().Comm().SumAll(&NumNonzeroDiags, &TotalNonzeroDiags, 1);
  if (Graph_.LevelOverlap()==0 &&
      ((TotalNonzeroDiags!=NumGlobalRows_) || 
       (TotalNonzeroDiags!=NumGlobalDiagonals_))) ierr = 1;
  if (NumNonzeroDiags != NumMyDiagonals_) ierr = 1; // Diagonals are not right, warn user

  return(ierr);
}

//==========================================================================
int Ifpack_CrsRiluk::Factor() {

  // if (!Allocated()) return(-1); // This test is not needed at this time.  All constructors allocate.
  if (!ValuesInitialized()) return(-2); // Must have values initialized.
  if (Factored()) return(-3); // Can't have already computed factors.

  SetValuesInitialized(false);

  // MinMachNum should be officially defined, for now pick something a little 
  // bigger than IEEE underflow value

  double MinDiagonalValue = Epetra_MinDouble;
  double MaxDiagonalValue = 1.0/MinDiagonalValue;

  int ierr = 0;
  int i, j, k;
  int * LI=0, * UI = 0;
  double * LV=0, * UV = 0;
  int NumIn, NumL, NumU;

  // Get Maximun Row length
  int MaxNumEntries = L_->MaxNumEntries() + U_->MaxNumEntries() + 1;

  int * InI = new int[MaxNumEntries]; // Allocate temp space
  double * InV = new double[MaxNumEntries];
  int * colflag = new int[NumMyCols_];

  double *DV;
  ierr = D_->ExtractView(&DV); // Get view of diagonal

  int current_madds = 0; // We will count multiply-add as they happen

  // Now start the factorization.

  // Need some integer workspace and pointers
  int NumUU; 
  int * UUI;
  double * UUV;
  for (j=0; j<NumMyCols_; j++) colflag[j] = - 1;

  for(i=0; i<NumMyRows_; i++) {

 // Fill InV, InI with current row of L, D and U combined

    NumIn = MaxNumEntries;
    assert(L_->ExtractMyRowCopy(i, NumIn, NumL, InV, InI)==0);
    LV = InV;
    LI = InI;

    InV[NumL] = DV[i]; // Put in diagonal
    InI[NumL] = i;
    
    assert(U_->ExtractMyRowCopy(i, NumIn-NumL-1, NumU, InV+NumL+1, InI+NumL+1)==0);
    NumIn = NumL+NumU+1;
    UV = InV+NumL+1;
    UI = InI+NumL+1;

    // Set column flags
    for (j=0; j<NumIn; j++) colflag[InI[j]] = j;

    double diagmod = 0.0; // Off-diagonal accumulator

    for (int jj=0; jj<NumL; jj++) {
      j = InI[jj];
      double multiplier = InV[jj]; // current_mults++;

      InV[jj] *= DV[j];
      
      assert(U_->ExtractMyRowView(j, NumUU, UUV, UUI)==0); // View of row above

      if (RelaxValue_==0.0) {
	for (k=0; k<NumUU; k++) {
	  int kk = colflag[UUI[k]];
	  if (kk>-1) {
	    InV[kk] -= multiplier*UUV[k];
	    current_madds++;
	  }
	}
      }
      else {
	for (k=0; k<NumUU; k++) {
	  int kk = colflag[UUI[k]];
	  if (kk>-1) InV[kk] -= multiplier*UUV[k];
	  else diagmod -= multiplier*UUV[k];
	  current_madds++;
	}
      }
     }
    if (NumL) assert(L_->ReplaceMyValues(i, NumL, LV, LI)==0);  // Replace current row of L

    DV[i] = InV[NumL]; // Extract Diagonal value

    if (RelaxValue_!=0.0) {
      DV[i] += RelaxValue_*diagmod; // Add off diagonal modifications
      // current_madds++;
    }

    if (fabs(DV[i]) > MaxDiagonalValue) {
      if (DV[i] < 0) DV[i] = - MinDiagonalValue;
      else DV[i] = MinDiagonalValue;
    }
    else
      DV[i] = 1.0/DV[i]; // Invert diagonal value

    for (j=0; j<NumU; j++) UV[j] *= DV[i]; // Scale U by inverse of diagonal

    if (NumU) assert(U_->ReplaceMyValues(i, NumU, UV, UI)==0);  // Replace current row of L and U


    // Reset column flags
    for (j=0; j<NumIn; j++) colflag[InI[j]] = -1;
  }

  
  // Add up flops
 
  double current_flops = 2 * current_madds;
  double total_flops = 0;
    
  Graph_.L_Graph().RowMap().Comm().SumAll(&current_flops, &total_flops, 1); // Get total madds across all PEs

  // Now count the rest
  total_flops += (double) L_->NumGlobalNonzeros(); // Accounts for multiplier above
  total_flops += (double) D_->GlobalLength(); // Accounts for reciprocal of diagonal
  if (RelaxValue_!=0.0) total_flops += 2 * (double)D_->GlobalLength(); // Accounts for relax update of diag

  UpdateFlops(total_flops); // Update flop count

  delete [] InI;
  delete [] InV;
  delete [] colflag;
  
  SetFactored(true);

  return(ierr);

}

//=============================================================================
int Ifpack_CrsRiluk::Solve(bool Trans, const Epetra_Vector& x, 
				Epetra_Vector& y) {
  return(Solve(Trans,(Epetra_MultiVector &) x, (Epetra_MultiVector &) y));
}

//=============================================================================
int Ifpack_CrsRiluk::Solve(bool Trans, const Epetra_MultiVector& X, 
				Epetra_MultiVector& Y) {
//
// This function finds Y such that LDU Y = X or U(trans) D L(trans) Y = X
//

  bool Upper = true;
  bool Lower = false;
  bool UnitDiagonal = true;

  Epetra_MultiVector * X1 = (Epetra_MultiVector *) &X;
  Epetra_MultiVector * Y1 = (Epetra_MultiVector *) &Y;

  if (Graph_.LevelOverlap()>0 && Graph_.L_Graph().DomainMap().DistributedGlobal()) {
    if (OverlapX_==0) { // Need to allocate space for overlap X and Y
      OverlapX_ = new Epetra_MultiVector(Graph_.OverlapGraph()->RowMap(), X.NumVectors());
      OverlapY_ = new Epetra_MultiVector(Graph_.OverlapGraph()->RowMap(), Y.NumVectors());
    }
    OverlapX_->Import(X,*Graph_.OverlapImporter(), Insert); // Import X values for solve
    X1 = OverlapX_;
    Y1 = OverlapY_; // Set pointers for X1 and Y1 to point to overlap space
  }

  Epetra_Flops * counter = this->GetFlopCounter();
  if (counter!=0) {
    L_->SetFlopCounter(*counter);
    Y1->SetFlopCounter(*counter);
    U_->SetFlopCounter(*counter);
  }

  if (!Trans) {

    L_->Solve(Lower, Trans, UnitDiagonal, *X1, *Y1);
    Y1->Multiply(1.0, *D_, *Y1, 0.0); // y = D*y (D_ has inverse of diagonal)
    U_->Solve(Upper, Trans, UnitDiagonal, *Y1, *Y1); // Solve Uy = y
  }
  else
    {
      U_->Solve(Upper, Trans, UnitDiagonal, *X1, *Y1); // Solve Uy = y
      Y1->Multiply(1.0, *D_, *Y1, 0.0); // y = D*y (D_ has inverse of diagonal)
      L_->Solve(Lower, Trans, UnitDiagonal, *Y1, *Y1);
      
    } 

  // Export computed Y values as directed
  if (Graph_.LevelOverlap()>0 && Graph_.L_Graph().DomainMap().DistributedGlobal())
    Y.Export(*OverlapY_,*Graph_.OverlapImporter(), OverlapMode_);
  return(0);
}
// Non-member functions

ostream& operator << (ostream& os, const Ifpack_CrsRiluk& A)
{
  long olda = os.setf(ios::right,ios::adjustfield);
  long oldf = os.setf(ios::scientific,ios::floatfield);
  int oldp = os.precision(12);
  int LevelFill = A.Graph().LevelFill();
  int LevelOverlap = A.Graph().LevelOverlap();
  Epetra_CrsMatrix & L = (Epetra_CrsMatrix &) A.L();
  Epetra_CrsMatrix & U = (Epetra_CrsMatrix &) A.U();
  Epetra_Vector & D = (Epetra_Vector &) A.D();

  os.width(14);
  os << endl;
  os <<  "     Level of Fill = "; os << LevelFill;
  os << endl;
  os.width(14);
  os <<  "     Level of Overlap = "; os << LevelOverlap;
  os << endl;

  os.width(14);
  os <<  "     Lower Triangle = ";
  os << endl;
  os << L; // Let Epetra_CrsMatrix handle the rest.
  os << endl;

  os.width(14);
  os <<  "     Inverse of Diagonal = ";
  os << endl;
  os << D; // Let Epetra_Vector handle the rest.
  os << endl;

  os.width(14);
  os <<  "     Upper Triangle = ";
  os << endl;
  os << U; // Let Epetra_CrsMatrix handle the rest.
  os << endl;
 
  // Reset os flags

  os.setf(olda,ios::adjustfield);
  os.setf(oldf,ios::floatfield);
  os.precision(oldp);

  return os;
}
