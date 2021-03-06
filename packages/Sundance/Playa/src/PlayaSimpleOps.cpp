/* @HEADER@ */
// ************************************************************************
// 
//                 Playa: Programmable Linear Algebra
//                 Copyright 2012 Sandia Corporation
// 
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
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
// Questions? Contact Kevin Long (kevin.long@ttu.edu)
// 

/* @HEADER@ */


#include "PlayaDefs.hpp"

#ifdef HAVE_TEUCHOS_EXPLICIT_INSTANTIATION

#include "PlayaSimpleAddedOpImpl.hpp"
#include "PlayaSimpleBlockOpImpl.hpp"
#include "PlayaSimpleComposedOpImpl.hpp"
#include "PlayaSimpleDiagonalOpImpl.hpp"
#include "PlayaSimpleIdentityOpImpl.hpp"
#include "PlayaSimpleScaledOpImpl.hpp"
#include "PlayaSimpleTransposedOpImpl.hpp"
#include "PlayaSimpleZeroOpImpl.hpp"

namespace Playa
{

template class SimpleAddedOp<double>;
template class SimpleBlockOp<double>;
template class SimpleComposedOp<double>;
template class SimpleDiagonalOp<double>;
template class SimpleIdentityOp<double>;
template class SimpleScaledOp<double>;
template class SimpleTransposedOp<double>;
template class SimpleZeroOp<double>;

template 
LinearOperator<double> operator*(const LinearOperator<double>& A,
  const LinearOperator<double>& B);

template 
LinearOperator<double> operator+(const LinearOperator<double>& A,
  const LinearOperator<double>& B);

template 
LinearOperator<double> operator*(const double& s,
  const LinearOperator<double>& A);

template 
LinearOperator<double> scaledOperator(const double& s,
  const LinearOperator<double>& A);

template 
LinearOperator<double> 
composedOperator(const Array<LinearOperator<double> >& A);

template 
LinearOperator<double> transposedOperator(const LinearOperator<double>& A);

template 
LinearOperator<double> diagonalOperator(const Vector<double>& v);

template
LinearOperator<double> identityOperator(const VectorSpace<double>& space);

template
LinearOperator<double> zeroOperator(const VectorSpace<double>& domain,
  const VectorSpace<double>& range);

template 
LinearOperator<double> makeBlockOperator(
  const VectorSpace<double>& domain,
  const VectorSpace<double>& range
  );

}


#endif
