#include <stdio.h>
#include <string.h>


#if defined(CRAY_T3X)
#include "fortran.h"

#define PREFIX
#define Petra_fcd fcd

#define dasum_ SASUM
#define daxpy_ SAXPY
#define dcopy_ SCOPY
#define ddot_ SDOT
#define dnrm2_ SNRM2
#define dscal_ SSCAL
#define idamax_ ISAMAX
#define dgemv_ SGEMV
#define dger_ SGER
#define dtrmv_ STRMV
#define dgemm_ SGEMM
#define dtrmm_ STRMM
#define dtrsm_ STRSM

#elif defined(INTEL_CXML)

#define PREFIX __stdcall
#define Petra_fcd char *, unsigned int

#define sasum_ SASUM
#define saxpy_ SAXPY
#define scopy_ SCOPY
#define sdot_  SDOT
#define snrm2_ SNRM2
#define sscal_ SSCAL
#define isamax_ ISAMAX
#define sgemv_ SGEMV
#define sger_ SGER
#define strmv_ STRMV
#define sgemm_ SGEMM
#define strmm_ STRMM
#define strsm_ STRSM

#define dasum_ DASUM
#define daxpy_ DAXPY
#define dcopy_ DCOPY
#define ddot_ DDOT
#define dnrm2_ DNRM2
#define dscal_ DSCAL
#define idamax_ IDAMAX
#define dgemv_ DGEMV
#define dger_ DGER
#define dtrmv_ DTRMV
#define dgemm_ DGEMM
#define dtrmm_ DTRMM
#define dtrsm_ DTRSM


#elif defined(INTEL_MKL)

#define PREFIX
#define Petra_fcd char *

#define sasum_  SASUM
#define saxpy_  SAXPY
#define scopy_  SCOPY
#define sdot_  SDOT
#define snrm2_  SNRM2
#define sscal_  SSCAL
#define isamax_  ISAMAX
#define sgemv_  SGEMV
#define sger_  SGER
#define strmv_  STRMV
#define sgemm_  SGEMM
#define strmm_  STRMM
#define strsm_  STRSM

#define dasum_  DASUM
#define daxpy_  DAXPY
#define dcopy_  DCOPY
#define ddot_  DDOT
#define dnrm2_  DNRM2
#define dscal_  DSCAL
#define idamax_  IDAMAX
#define dgemv_  DGEMV
#define dger_  DGER
#define dtrmv_  DTRMV
#define dgemm_  DGEMM
#define dtrmm_  DTRMM
#define dtrsm_  DTRSM

#else

/* Define fcd (Fortran Petra_fcd descriptor) */
#define PREFIX
#define Petra_fcd char * 

#if defined(__rs6000)
#define dasum_ dasum
#define daxpy_ daxpy
#define dcopy_ dcopy
#define ddot_ ddot
#define dnrm2_ dnrm2
#define dscal_ dscal
#define idamax_ idamax
#define dgemv_ dgemv
#define dger_ dger
#define dtrmv_ dtrmv
#define dgemm_ dgemm
#define dtrmm_ dtrmm
#define dtrsm_ dtrsm
#endif

#endif

// Double precision BLAS 1
extern "C" double PREFIX dasum_(int* n, double x[], int* incx);
extern "C" void PREFIX daxpy_(int* n, double* alpha, double x[], int* incx, double y[], int* incy);
extern "C" void PREFIX dcopy_(int* n, double *x, int* incx, double *y, int* incy);
extern "C" double PREFIX ddot_(int* n, double x[], int* incx, double y[], int* incy);
extern "C" double PREFIX dnrm2_(int* n, double x[], int* incx);
extern "C" void PREFIX dscal_(int* n, double* alpha, double *x, int* incx);
extern "C" int PREFIX idamax_(int* n, double *x, int* incx);

// Single precision BLAS 1
extern "C" float PREFIX sasum_(int* n, float x[], int* incx);
extern "C" void PREFIX saxpy_(int* n, float* alpha, float x[], int* incx, float y[], int* incy);
extern "C" void PREFIX scopy_(int* n, float *x, int* incx, float *y, int* incy);
extern "C" float PREFIX sdot_(int* n, float x[], int* incx, float y[], int* incy);
extern "C" float PREFIX snrm2_(int* n, float x[], int* incx);
extern "C" void PREFIX sscal_(int* n, float* alpha, float *x, int* incx);
extern "C" int PREFIX isamax_(int* n, float *x, int* incx);

// Double precision BLAS 2
extern "C" void PREFIX dgemv_(Petra_fcd, int* m, int* n, double* alpha, double A[], int* lda,
		       double x[], int* incx, double* beta, double y[], int* incy);
extern "C" void PREFIX dtrmv_(Petra_fcd, Petra_fcd, Petra_fcd, int *n, 
		      double *a, int *lda, double *x, int *incx);
extern "C" void PREFIX dger_(int *m, int *n, double *alpha, double *x, int *incx, double *y, 
		     int *incy, double *a, int *lda);


// Single precision BLAS 2
extern "C" void PREFIX sgemv_(Petra_fcd, int* m, int* n, float* alpha, float A[], int* lda,
		       float x[], int* incx, float* beta, float y[], int* incy);
extern "C" void PREFIX strmv_(Petra_fcd, Petra_fcd, Petra_fcd, int *n, 
		      float *a, int *lda, float *x, int *incx);
extern "C" void PREFIX sger_(int *m, int *n, float *alpha, float *x, int *incx, float *y, 
		     int *incy, float *a, int *lda);

// Double precision BLAS 3
extern "C" void PREFIX dgemm_(Petra_fcd, Petra_fcd, int *m, int *
		      n, int *k, double *alpha, double *a, int *lda, 
		      double *b, int *ldb, double *beta, double *c, int *ldc);
extern "C" void PREFIX dtrmm_(Petra_fcd, Petra_fcd, Petra_fcd, Petra_fcd, 
		      int *m, int *n, double *alpha, double *a, int * lda, double *b, int *ldb);
extern "C" void PREFIX dtrsm_(Petra_fcd, Petra_fcd, Petra_fcd, Petra_fcd, 
		      int *m, int *n, double *alpha, double *a, int *
		      lda, double *b, int *ldb);

// Single precision BLAS 3
extern "C" void PREFIX sgemm_(Petra_fcd, Petra_fcd, int *m, int *
		      n, int *k, float *alpha, float *a, int *lda, 
		      float *b, int *ldb, float *beta, float *c, int *ldc);
extern "C" void PREFIX strmm_(Petra_fcd, Petra_fcd, Petra_fcd, Petra_fcd, 
		      int *m, int *n, float *alpha, float *a, int * lda, float *b, int *ldb);
extern "C" void PREFIX strsm_(Petra_fcd, Petra_fcd, Petra_fcd, Petra_fcd, 
		      int *m, int *n, float *alpha, float *a, int *
		      lda, float *b, int *ldb);

extern "C" void PREFIX xerbla_(Petra_fcd, int *info);
