/*@HEADER
// ***********************************************************************
// 
//       Tifpack: Tempated Object-Oriented Algebraic Preconditioner Package
//                 Copyright (2009) Sandia Corporation
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
// ***********************************************************************
//@HEADER
*/

#ifndef TIFPACK_PERTURBEDMATRIX_HPP
#define TIFPACK_PERTURBEDMATRIX_HPP

#include "Tifpack_ConfigDefs.hpp"
#include "Tpetra_CrsGraph.hpp"
#include "Tpetra_Object.hpp"
class Tpetra_Comm;
class Tpetra_Map;
//! Tifpack_PerturbedMatrix: Supports the use of diagonal perturbations in Tifpack.



class Tifpack_PerturbedMatrix: public Tpetra_Object {

 public:
  //@{ \name Constructors/Destructor

  //! Constructor using Tpetra_RowMatrix.
  /*! Creates an Tifpack_PerturbedMatrix object from the
	 Tpetra_RowMatrix interface. 
    \param In
            RowMatrix - An object that has implemented the Tpetra_RowMatrix interface.
  */
  Tifpack_PerturbedMatrix(const Tpetra_RowMatrix & UserMatrix);
  
  //! Copy constructor.
  Tifpack_PerturbedMatrix(const Tifpack_PerturbedMatrix & Source);

  //! Tifpack_PerturbedMatrix Destructor
  virtual ~Tifpack_PerturbedMatrix();
  //@}

  //@{ \name Initialization methods.

  //! Set absolute threshold value
  void SetAbsoluteThreshold( double Athresh) {Athresh_ = Athresh; return;}

  //! Set relative threshold value
  void SetRelativeThreshold( double Rthresh) {Rthresh_ = Rthresh; return;}


  //@}
#endif // TIFPACK_PERTURBEDMATRIX_HPP
