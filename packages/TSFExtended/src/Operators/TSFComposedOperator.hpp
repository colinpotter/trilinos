/* @HEADER@ */
/* ***********************************************************************
// 
//           TSFExtended: Trilinos Solver Framework Extended
//                 Copyright (2004) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// **********************************************************************/
/* @HEADER@ */

#ifndef TSFCOMPOSEDOPERATOR_HPP
#define TSFCOMPOSEDOPERATOR_HPP

#include "TSFConfigDefs.hpp"
#include "TSFCoreLinearOp.hpp"
#include "TSFCoreVectorStdOps.hpp"
#include "TSFCoreVectorSpace.hpp"
#include "TSFOpDescribableByTypeID.hpp"
#include "Teuchos_RefCountPtr.hpp"

namespace TSFExtended
{
  /**
   * ComposedOperator is a composition of two linear operators.
   */
  template <class Scalar> 
  class ComposedOperator : public OpDescribableByTypeID<Scalar>,
                           public Handleable<TSFCore::LinearOp<Scalar> >
  {
  public:
    GET_RCP(TSFCore::LinearOp<Scalar>);
    /** 
     * Construct a pair of linear operators.
     */
    ComposedOperator(const LinearOperator<Scalar>& left, 
                     const LinearOperator<Scalar>& right)
      : left_(left), right_(right) {;}

    /** Virtual dtor */
    virtual ~ComposedOperator(){;}

    /** 
     * Apply operator to a vector in the domain space and return a vector
     * in the range space.
     */
    virtual void apply(
                       const TSFCore::ETransp            M_trans
                       ,const TSFCore::Vector<Scalar>    &x
                       ,TSFCore::Vector<Scalar>          *y
                       ,const Scalar            alpha = 1.0
                       ,const Scalar            beta  = 0.0
                       ) const 
    {
      Vector<Scalar> v;
      if (M_trans == TSFCore::NOTRANS)
        {
          v = right_.range().createMember();
          v.zero();
          right_.ptr()->apply(M_trans, x, v.ptr().get(), 1.0, 0.0);
          left_.ptr()->apply(M_trans, *(v.ptr()), y, alpha, beta);
        }
      else
        {
          v = left_.range().createMember();
          v.zero();
          left_.ptr()->apply(M_trans, x, v.ptr().get(), 1.0, 0.0);
          right_.ptr()->apply(M_trans, *(v.ptr()), y, alpha, beta);
        }
    }

    /** Return the domain of the operator */
    virtual RefCountPtr< const TSFCore::VectorSpace<Scalar> > domain() const {return right_.domain().ptr();}

    /** Return the range of the operator */
    virtual RefCountPtr< const TSFCore::VectorSpace<Scalar> > range() const {return left_.range().ptr();}

    /* */
    //    GET_RCP(TSFCore::LinearOp<Scalar>);

  private:

    LinearOperator<Scalar> left_;  
    LinearOperator<Scalar> right_; 

  };
}

#endif
