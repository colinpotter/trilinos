
#include "Epetra_CrsMatrix.h" 
#include "Epetra_MultiVector.h" 
#include "Epetra_LinearProblem.h" 
#include "Amesos_BaseSolver.h" 
#include "AztecOO.h"

#define MIN(a, b) (((a) < (b)) ? a : b)
#define MAX(a, b) (((a) > (b)) ? a : b)


typedef struct
{
    Epetra_LinearProblem *LP;   // Local problem to solve
    Amesos_BaseSolver *Solver;  // Local Subdomain solver
    Epetra_CrsMatrix *Cptr;     // Column separator
    int Dnr;                    // #local rows
    int Snr;                    // #remote rows
    int *DRowElems;             // local rows
    int *SRowElems;             // remote rows
    Teuchos::RCP<Epetra_CrsMatrix> Sbar; // Approx Schur complement
    AztecOO *innersolver;            // inner solver
} hyperlu_data;

typedef struct
{
    int sym;                    // flag for symmetry
    double Sdiagfactor;         // % of diagonals added to Schur complement
    int schurApproxMethod;      // ==1 implies blockdiagonal + A22
                                // ==2 implies diagonals
    int inner_maxiters;         // maximum iterations for inner solver
    double inner_tolerance;     // relative residual tolerance for inner solver
    string libName;             // library for the outer solver
} hyperlu_config;

int HyperLU_factor(Epetra_CrsMatrix *A, hyperlu_data *data, hyperlu_config 
                *config);
/*int HyperLU_factor(Epetra_CrsMatrix *A, int sym,
        Epetra_LinearProblem *&LP, Amesos_BaseSolver *&Solver, 
        Epetra_CrsMatrix *&Cptr, int &Dnr, 
        int *&DRowElems, int &Snr, int *&SRowElems,
        Teuchos::RCP<Epetra_CrsMatrix>& Sbar, double Sdiagfactor);*/

int hyperlu_solve(hyperlu_data *data, hyperlu_config *config,
    const Epetra_MultiVector& X, Epetra_MultiVector& Y);
