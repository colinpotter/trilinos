
#include "Petra_BLAS_wrappers.h"
#include "TPetra_ScalarTraits.h"
namespace TPetra {

// TPetra::BLAS constructor
template<class scalarType>
BLAS<scalarType>::BLAS(void){}
// TPetra::BLAS constructor
template<class scalarType>
BLAS<scalarType>::BLAS(const TPetra::BLAS<scalarType>& BLAS){}
// TPetra::BLAS destructor
template<class scalarType>
BLAS<scalarType>::~BLAS(){}



//=============================================================================
template<>
void BLAS<float>::GEMM(char TRANSA, char TRANSB, int M, int N, int K,
		       float ALPHA, float * A, int LDA, float * B,
		       int LDB, float BETA, float * C, int LDC) const {

    sgemm_(&TRANSA, &TRANSB, &M, &N, &K, &ALPHA,
	   A, &LDA, B, &LDB, &BETA, C, &LDC);
}
//=============================================================================
template<>
void BLAS<double>::GEMM(char TRANSA, char TRANSB, int M, int N, int K,
			double ALPHA, double * A, int LDA, double * B,
			int LDB, double BETA, double * C, int LDC) const {
  
  dgemm_(&TRANSA, &TRANSB, &M, &N, &K, &ALPHA,
	 A, &LDA, B, &LDB, &BETA, C, &LDC);
}
//=============================================================================
template<class scalarType>
void BLAS<scalarType>::GEMM(char TRANSA, char TRANSB, int M, int N, int K,
			    scalarType ALPHA, scalarType * A, int LDA, scalarType * B,
			    int LDB, scalarType BETA, scalarType * C, int LDC) const {

  int incra, incca, incrb, inccb;
  if (TRANSA=='N') {
    incra = LDA; 
    incca = 1;
  }
  else {
    incca = LDA; 
    incra = 1;
  }
  if (TRANSB=='N') {
    incrb = LDB; 
    inccb = 1;
  }
  else {
    inccb = LDB; 
    incrb = 1;
  }
  
  scalarType * curC = C;
  scalarType * curB = B;
  scalarType * curA = A;
  scalarType zero = ScalarTraits<scalarType>::zero();
  scalarType one =  ScalarTraits<scalarType>::one();

  if (BETA==zero) 
    for (int i=0; i<M; i++) C[i] = zero;
  else 
    for (int i=0; i<M; i++) C[i] *= BETA;

  for (int i=0; i<M; i++) {
    for (int j=0; j<N; j++) {
      scalarType * tmpB = curB;
      scalarType * tmpA = curA;
      scalarType  tmpC = zero;
      for (int l=0; l<K; l++) {
	tmpC += (*tmpA) * (*tmpB); // Note: This is not optimal.  work on it later
	tmpA += incra;
	tmpB += inccb;
      }
      *curC += ALPHA*tmpC;
      curC += LDC;
      curB += incrb;
    }
    curA += incca;
    curB = B;
    curC = C+i+1;
  }
  return;
}

} // namespace TPetra
