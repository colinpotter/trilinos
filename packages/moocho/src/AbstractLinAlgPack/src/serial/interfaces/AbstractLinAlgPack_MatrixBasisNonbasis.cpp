// //////////////////////////////////////////////////////////////////////////////////
// MatrixBasisNonbasis.cpp
//
// Copyright (C) 2001 Roscoe Ainsworth Bartlett
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the "Artistic License" (see the web site
//   http://www.opensource.org/licenses/artistic-license.html).
// This license is spelled out in the file COPYING.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// above mentioned "Artistic License" for more details.

#include <assert.h>

#include "SparseLinAlgPack/include/MatrixBasisNonbasis.h"
#include "SparseLinAlgPack/include/SpVectorClass.h"
#include "LinAlgPack/include/VectorOp.h"
#include "LinAlgPack/include/GenMatrixOp.h"
#include "LinAlgPack/include/LinAlgPackAssertOp.h"

namespace SparseLinAlgPack {

// Overridden from MatrixWithOp

void MatrixBasisNonbasis::Vp_StMtV(VectorSlice* vs_lhs, value_type alpha, BLAS_Cpp::Transp trans_rhs1
    , const VectorSlice& vs_rhs2, value_type beta) const
{
	using SparseLinAlgPack::Vp_StMtV;

	size_type n = rows(), m = cols();

	LinAlgPack::Vp_MtV_assert_sizes( vs_lhs->size()
		, n, m, trans_rhs1, vs_rhs2.size() );

	switch(trans_rhs1) {
		case BLAS_Cpp::no_trans:
			//
			// M = [ C' ; N' ]
			//
			// y = b*y + a * M * x
			//
			// y = b*y + a * [ C' ] * x
			//               [ N' ]
			//
			// y(1:m)   = b*y(1:m)   + a * C' * x
			// y(m+1:n) = b*y(m+1:n) + a * N' * x
			
			// y(1:m)   = b*y(1:m)   + a * C' * x
			Vp_StMtV( &(*vs_lhs)(1,m), alpha, C(), BLAS_Cpp::trans, vs_rhs2, beta );

			// y(m+1:n) = b*y(m+1:n) + a * N' * x
			if( rows() > cols() )
				Vp_StMtV( &(*vs_lhs)(m+1,n), alpha, N(), BLAS_Cpp::trans, vs_rhs2, beta );

			break;

		case BLAS_Cpp::trans:
			//
			// M' = [ C , N ]
			//
			// y = b*y + a * M' * x
			//
			// y = b*y + a * [ C , N ] * [ x(1:m)   ]
			//                           [ x(m+1:n) ]
			//
			// y = b*y + a * C * x(1:m) + a * N * x(m+1:n)

			// y = b*y + a * C * x(1:m)
			Vp_StMtV( vs_lhs, alpha, C(), BLAS_Cpp::no_trans, vs_rhs2(1,m), beta );
			
			// y = y + a * N * x(m+1:n)
			if( rows() > cols() )
				Vp_StMtV( vs_lhs, alpha, N(), BLAS_Cpp::no_trans, vs_rhs2(m+1,n) );

			break;

		default:
			assert(0);
	}
}

void MatrixBasisNonbasis::Vp_StMtV(VectorSlice* vs_lhs, value_type alpha, BLAS_Cpp::Transp trans_rhs1
    , const SpVectorSlice& sv_rhs2, value_type beta) const
{
	using SparseLinAlgPack::Vp_StMtV;

	size_type n = rows(), m = cols();
	// Assert that the dimensions of the aruments match up and if not
	// then throw an excption.
	LinAlgPack::Vp_MtV_assert_sizes( vs_lhs->size()
		, n, m, trans_rhs1, sv_rhs2.size() );

	switch(trans_rhs1) {
		case BLAS_Cpp::no_trans:
			//
			// M = [ C' ; N' ]
			//
			// y = b*y + a * M * x
			//
			// y = b*y + a * [ C' ] * x
			//               [ N' ]
			//
			// y(1:m)   = b*y(1:m)   + a * C' * x
			// y(m+1:n) = b*y(m+1:n) + a * N' * x
			
			// y(1:m)   = b*y(1:m)   + a * C' * x
			Vp_StMtV( &(*vs_lhs)(1,m), alpha, C(), BLAS_Cpp::trans, sv_rhs2, beta );

			// y(m+1:n) = b*y(m+1:n) + a * N' * x
			if( rows() > cols() )
				Vp_StMtV( &(*vs_lhs)(m+1,n), alpha, N(), BLAS_Cpp::trans, sv_rhs2, beta );

			break;

		case BLAS_Cpp::trans:
			//
			// M' = [ C , N ]
			//
			// y = b*y + a * M' * x
			//
			// y = b*y + a * [ C , N ] * [ x(1:m)   ]
			//                           [ x(m+1:n) ]
			//
			// y = b*y + a * C * x(1:m) + a * N * x(m+1:n)

			// y = b*y + a * C * x(1:m)
			Vp_StMtV( vs_lhs, alpha, C(), BLAS_Cpp::no_trans, sv_rhs2(1,m), beta );
			
			// y = y + a * N * x(m+1:n)
			if( rows() > cols() )
				Vp_StMtV( vs_lhs, alpha, N(), BLAS_Cpp::no_trans, sv_rhs2(m+1,n) );

			break;

		default:
			assert(0);
	}
}

}	// end namespace SparseLinAlgPack 
