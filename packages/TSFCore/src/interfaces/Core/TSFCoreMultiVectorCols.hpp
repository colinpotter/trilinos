// /////////////////////////////////////////////////////////////////////////////
// TSFCoreMultiVectorCols.hpp

#ifndef TSFCORE_MULTI_VECTOR_COLS_HPP
#define TSFCORE_MULTI_VECTOR_COLS_HPP

#include "TSFCoreMultiVectorColsDecl.hpp"
#include "TSFCoreMultiVector.hpp"
#include "TSFCoreVectorSpaceFactory.hpp"
#include "Teuchos_TestForException.hpp"

namespace TSFCore {

// Constructors/Initializers

template<class Scalar>
MultiVectorCols<Scalar>::MultiVectorCols()
{}

template<class Scalar>
MultiVectorCols<Scalar>::MultiVectorCols(
	Teuchos::RefCountPtr<Vector<Scalar> >  col_vec
	)
{
	this->initialize(col_vec);
}

template<class Scalar>
MultiVectorCols<Scalar>::MultiVectorCols(
	const  Teuchos::RefCountPtr<const VectorSpace<Scalar> >   &range
	,const  Teuchos::RefCountPtr<const VectorSpace<Scalar> >  &domain
	,Teuchos::RefCountPtr<Vector<Scalar> >                    col_vecs[]
	)
{
	this->initialize(range,domain,col_vecs);
}

template<class Scalar>
void MultiVectorCols<Scalar>::initialize(
	Teuchos::RefCountPtr<Vector<Scalar> >  col_vec
	)
{
#ifdef _DEBUG
	const char err_msg[] = "MultiVectorCols<Scalar>::initialize(...): Error!";
	TEST_FOR_EXCEPTION( col_vec.get() == NULL,           std::invalid_argument, err_msg ); 
	TEST_FOR_EXCEPTION( col_vec->space().get() == NULL,  std::invalid_argument, err_msg ); 
#endif
	range_  = col_vec->space();
	domain_ = range_->smallVecSpcFcty()->createVecSpc(1);
	col_vecs_.resize(1);
	col_vecs_[0] = col_vec;
}
	
template<class Scalar>
void MultiVectorCols<Scalar>::initialize(
	const  Teuchos::RefCountPtr<const VectorSpace<Scalar> >   &range
	,const  Teuchos::RefCountPtr<const VectorSpace<Scalar> >  &domain
	,Teuchos::RefCountPtr<Vector<Scalar> >                    col_vecs[]
	)
{
#ifdef _DEBUG
	const char err_msg[] = "MultiVectorCols<Scalar>::initialize(...): Error!";
	TEST_FOR_EXCEPTION( range.get()   == NULL, std::invalid_argument, err_msg ); 
	TEST_FOR_EXCEPTION( domain.get()  == NULL, std::invalid_argument, err_msg ); 
	TEST_FOR_EXCEPTION( range->dim()  == 0,    std::invalid_argument, err_msg ); 
	TEST_FOR_EXCEPTION( domain->dim() == 0,    std::invalid_argument, err_msg );
	// ToDo: Check the compatibility of the vectors in col_vecs!
#endif
	range_ = range;
	domain_ = domain;
	const Index num_cols = domain->dim();
	col_vecs_.resize(num_cols);
	if(col_vecs) {
		for( Index j = 1; j <= num_cols; ++j )
			col_vecs_[j-1] = col_vecs[j-1];
	}
	else {
		for( Index j = 1; j <= num_cols; ++j )
			col_vecs_[j-1] = range_->createMember();
	}
}

template<class Scalar>
void MultiVectorCols<Scalar>::set_uninitialized()
{
	col_vecs_.resize(0);
	range_ = Teuchos::null;
	domain_ = Teuchos::null;
}

// Overridden from LinearOp

template<class Scalar>
Teuchos::RefCountPtr<const VectorSpace<Scalar> >
MultiVectorCols<Scalar>::range() const
{
	return range_;
}

template<class Scalar>
Teuchos::RefCountPtr<const VectorSpace<Scalar> >
MultiVectorCols<Scalar>::domain() const
{
	return domain_;
}

// Overridden from MultiVector

template<class Scalar>
Teuchos::RefCountPtr<Vector<Scalar> >
MultiVectorCols<Scalar>::col(Index j)
{
	TEST_FOR_EXCEPTION(
		!(  1 <= j  && j <= col_vecs_.size() ), std::logic_error
		,"Error, j = " << j << " does not fall in the range [1,"<<col_vecs_.size()<< "]!"
		);
	return col_vecs_[j-1];
}

template<class Scalar>
Teuchos::RefCountPtr<MultiVector<Scalar> >
MultiVectorCols<Scalar>::subView( const Range1D& col_rng_in )
{
	const Index cols = domain_->dim();
	const Range1D col_rng = RangePack::full_range(col_rng_in,1,cols);
#ifdef _DEBUG
	TEST_FOR_EXCEPTION(
		!( col_rng.ubound() <= cols )
		,std::logic_error
		,"MultiVectorCols<Scalar>::subView(col_rng): Error, the input range col_rng = ["<<col_rng.lbound()<<","<<col_rng.ubound()<<"] "
		"is not in the range [1,"<<cols<<"]!"
		);
#endif
	return Teuchos::rcp(
		new MultiVectorCols<Scalar>(
			range_,domain_->smallVecSpcFcty()->createVecSpc(col_rng.size()),&col_vecs_[col_rng.lbound()-1]
			) );
}
	
} // end namespace TSFCore

#endif // TSFCORE_MULTI_VECTOR_COLS_HPP
