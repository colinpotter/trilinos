#ifndef _IFP_LOCALMAT_H_
#define _IFP_LOCALMAT_H_

#include <iostream.h>
#include "ifp_BlockVec.h"
class ifp_LocalPrecon;

class ifp_LocalMat
{
protected:
    inline void solve_is_mult(const ifp_BlockVec& B, ifp_BlockVec& X) const;

public:
    virtual ~ifp_LocalMat() {} // virtual, to make sure we call derived destructor
    virtual double *& Data() = 0; // ref to ptr to double
    virtual const double *Data() const = 0;

    virtual ifp_LocalMat *CreateEmpty() const = 0;
    virtual ifp_LocalMat *CreateInv(ifp_LocalPrecon&) const = 0;
    virtual void SetToZero(int, int) = 0;
    virtual void MatCopy(const ifp_LocalMat& A) = 0;
    virtual void Print(ostream&) const = 0;

    virtual void Mat_Trans(ifp_LocalMat *B) const = 0;
    virtual void Mat_Mat_Add(const ifp_LocalMat *B, ifp_LocalMat *C, 
        double alpha = 1.0) const = 0;
    virtual void Mat_Mat_Mult(const ifp_LocalMat *B, ifp_LocalMat *C, 
        double alpha = 1.0, double beta = 0.0) const = 0;
    virtual void Mat_Vec_Mult(const ifp_BlockVec& B, ifp_BlockVec& C,
        double alpha = 1.0, double beta = 0.0) const = 0;
    virtual void Mat_Trans_Vec_Mult(const ifp_BlockVec& B, ifp_BlockVec&C,
        double alpha = 1.0, double beta = 0.0) const = 0;
    virtual void Mat_Vec_Solve(const ifp_BlockVec& b, 
        ifp_BlockVec& x) const = 0;
    virtual void Mat_Trans_Vec_Solve(const ifp_BlockVec& b, 
        ifp_BlockVec& x) const = 0;
};

inline void ifp_LocalMat::solve_is_mult(const ifp_BlockVec& B, ifp_BlockVec& X) const
{
    if (&B != &X)
    {
        Mat_Vec_Mult(B, X);
    }
    else
    {
        // solves must allow solves in place
        ifp_BlockVec T(B, -1);
        Mat_Vec_Mult(T, X);
    }
}

typedef ifp_LocalMat *ifp_LocalMatp;

#endif // _IFP_LOCALMAT_H_
