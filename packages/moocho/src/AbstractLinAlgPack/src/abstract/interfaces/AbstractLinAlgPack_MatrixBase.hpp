// @HEADER
// ***********************************************************************
// 
// Moocho: Multi-functional Object-Oriented arCHitecture for Optimization
//                  Copyright (2003) Sandia Corporation
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
// Questions? Contact Roscoe A. Bartlett (rabartl@sandia.gov) 
// 
// ***********************************************************************
// @HEADER

#ifndef ABSTRACT_LINALG_PACK_MATRIX_BASE_H
#define ABSTRACT_LINALG_PACK_MATRIX_BASE_H

#include <stdexcept>

#include "AbstractLinAlgPack_Types.hpp"

namespace AbstractLinAlgPack {

///
/** Base class for all polymorphic matrices.
  */
class MatrixBase {
public:

  /// Thrown if matrices are incompatible
  class IncompatibleMatrices : public std::logic_error
  {public: IncompatibleMatrices(const std::string& what_arg) : std::logic_error(what_arg) {}};

  /// Virtual destructor
  virtual ~MatrixBase() {}

  /** @name Vector spaces for the columns and rows of the matrix */
  //@{

  /// Vector space for vectors that are compatible with the columns of the matrix.
  virtual const VectorSpace& space_cols() const = 0;

  /// Vector space for vectors that are compatible with the rows of the matrix.
  virtual const VectorSpace& space_rows() const = 0;

  //@}

  /** @name Dimensionality */
  //@{

  ///
  /** Return the number of rows in the matrix.
   *
   * The default implementation returns <tt>space_cols().dim()</tt>.
   */
  virtual size_type rows() const;

  ///
  /** Return the number of columns in the matrix.
   *
   * The default implementation returns <tt>space_rows().dim()</tt>.
   */
  virtual size_type cols() const;

  ///
  /** Return the number of nonzero elements in the matrix.
   *
   * The default is to just assume it is dense and to return
   * <tt>rows() * cols()</tt>.
   */
  virtual size_type nz() const;

  //@}

};	// end class MatrixBase

}	// end namespace AbstractLinAlgPack

#endif	// ABSTRACT_LINALG_PACK_MATRIX_BASE_H
