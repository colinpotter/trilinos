
// @HEADER
// ***********************************************************************
// 
//                      Didasko Tutorial Package
//                 Copyright (2005) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions about Didasko? Contact Marzio Sala (marzio.sala _AT_ gmail.com)
// 
// ***********************************************************************
// @HEADER

#include "Didasko_ConfigDefs.h"
#if defined(HAVE_DIDASKO_TEUCHOS)
#ifdef HAVE_MPI
#include "mpi.h"
#endif

#include "Teuchos_BLAS.hpp"

int main(int argc, char* argv[])
{
#ifdef HAVE_MPI
  MPI_Init(&argc,&argv);
#endif

  // Creating an instance of the BLAS class for double-precision kernels looks like:
  Teuchos::BLAS<int, double> blas;

  // This instance provides the access to all the BLAS kernels listed in Figure \ref{blas_kernels}:
  const int n = 10;
  double alpha = 2.0;
  double x[ n ];
  for ( int i=0; i<n; i++ ) { x[i] = i; }
  blas.SCAL( n, alpha, x, 1 );
  int max_idx = blas.IAMAX( n, x, 1 );
  cout<< "The index of the maximum magnitude entry of x[] is the "
      <<  max_idx <<"-th and x[ " << max_idx-1 << " ] = "<< x[max_idx-1] 
      << endl;
  
#ifdef HAVE_MPI
  MPI_Finalize();
#endif
  return 0;
}
#else

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  puts("Please configure Didasko with:\n"
       "--enable-teuchos");

  return 0;
}
#endif

