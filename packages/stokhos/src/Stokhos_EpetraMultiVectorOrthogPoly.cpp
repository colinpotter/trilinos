// @HEADER
// ***********************************************************************
// 
//                           Stokhos Package
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
// Questions? Contact Eric T. Phipps (etphipp@sandia.gov).
// 
// ***********************************************************************
// @HEADER

#include "Stokhos_EpetraMultiVectorOrthogPoly.hpp"

Stokhos::EpetraMultiVectorOrthogPoly::
EpetraMultiVectorOrthogPoly() :
  ProductContainer<Epetra_MultiVector>(),
  VectorOrthogPoly<Epetra_MultiVector>(),
  ProductEpetraMultiVector() 
{
}

Stokhos::EpetraMultiVectorOrthogPoly::
EpetraMultiVectorOrthogPoly(
  const Teuchos::RCP<const Stokhos::OrthogPolyBasis<int, double> >& basis,
  const Teuchos::RCP<const Epetra_BlockMap>& block_map) :
  ProductContainer<Epetra_MultiVector>(block_map),
  VectorOrthogPoly<Epetra_MultiVector>(basis, block_map),
  ProductEpetraMultiVector(block_map) 
{
}

Stokhos::EpetraMultiVectorOrthogPoly::
EpetraMultiVectorOrthogPoly(
  const Teuchos::RCP<const Stokhos::OrthogPolyBasis<int, double> >& basis,
  const Teuchos::RCP<const Epetra_BlockMap>& block_map,
  const Teuchos::RCP<const Epetra_BlockMap>& coeff_map,
  const Teuchos::RCP<const EpetraExt::MultiComm>& product_comm,
  int num_vectors) : 
  ProductContainer<Epetra_MultiVector>(block_map),
  VectorOrthogPoly<Epetra_MultiVector>(basis, block_map),
  ProductEpetraMultiVector(block_map, coeff_map, product_comm, num_vectors) 
{
}

Stokhos::EpetraMultiVectorOrthogPoly::
EpetraMultiVectorOrthogPoly(
  const Teuchos::RCP<const Stokhos::OrthogPolyBasis<int, double> >& basis,
  const Teuchos::RCP<const Epetra_BlockMap>& block_map,
  const Teuchos::RCP<const Epetra_BlockMap>& coeff_map,
  const Teuchos::RCP<const Epetra_BlockMap>& product_map,
  const Teuchos::RCP<const EpetraExt::MultiComm>& product_comm,
  int num_vectors) :
  ProductContainer<Epetra_MultiVector>(block_map),
  VectorOrthogPoly<Epetra_MultiVector>(basis, block_map),
  ProductEpetraMultiVector(block_map, coeff_map, product_map, product_comm, 
			   num_vectors)
{
}

Stokhos::EpetraMultiVectorOrthogPoly::
EpetraMultiVectorOrthogPoly(
  const Teuchos::RCP<const Stokhos::OrthogPolyBasis<int, double> >& basis,
  const Teuchos::RCP<const Epetra_BlockMap>& block_map,
  const Teuchos::RCP<const Epetra_BlockMap>& coeff_map,
  const Teuchos::RCP<const Epetra_BlockMap>& product_map,
  const Teuchos::RCP<const EpetraExt::MultiComm>& product_comm,
  Epetra_DataAccess CV,
  const Epetra_MultiVector& block_vector) :
  ProductContainer<Epetra_MultiVector>(block_map),
  VectorOrthogPoly<Epetra_MultiVector>(basis, block_map),
  ProductEpetraMultiVector(block_map, coeff_map, product_map, product_comm, CV,
			   block_vector)
{
}
    
Stokhos::EpetraMultiVectorOrthogPoly::
EpetraMultiVectorOrthogPoly(const Stokhos::EpetraMultiVectorOrthogPoly& v) :
  ProductContainer<Epetra_MultiVector>(v),
  VectorOrthogPoly<Epetra_MultiVector>(v),
  ProductEpetraMultiVector(v)
{
}

Stokhos::EpetraMultiVectorOrthogPoly::
~EpetraMultiVectorOrthogPoly() {}

Stokhos::EpetraMultiVectorOrthogPoly& 
Stokhos::EpetraMultiVectorOrthogPoly::
operator=(const Stokhos::EpetraMultiVectorOrthogPoly& v) {
  ProductEpetraMultiVector::operator=(v);
  this->basis_ = v.basis_;
  return *this;
}
      
void 
Stokhos::EpetraMultiVectorOrthogPoly::
reset(
  const Teuchos::RCP<const Stokhos::OrthogPolyBasis<int, double> >& new_basis,
  const Teuchos::RCP<const Epetra_BlockMap>& block_map,
  const Teuchos::RCP<const Epetra_BlockMap>& coeff_map,
  const Teuchos::RCP<const EpetraExt::MultiComm>& product_comm,
  int num_vectors) 
{
  ProductEpetraMultiVector::reset(block_map, coeff_map, product_comm, 
				  num_vectors);
  this->basis_ = new_basis;
}

void 
Stokhos::EpetraMultiVectorOrthogPoly::
reset(
  const Teuchos::RCP<const Stokhos::OrthogPolyBasis<int, double> >& new_basis,
  const Teuchos::RCP<const Epetra_BlockMap>& block_map,
  const Teuchos::RCP<const Epetra_BlockMap>& coeff_map,
  const Teuchos::RCP<const Epetra_BlockMap>& product_map,
  const Teuchos::RCP<const EpetraExt::MultiComm>& product_comm,
  int num_vectors) 
{
  ProductEpetraMultiVector::reset(block_map, coeff_map, product_map, 
				  product_comm, num_vectors);
  this->basis_ = new_basis;
}

void
Stokhos::EpetraMultiVectorOrthogPoly::
computeMean(Epetra_MultiVector& v) const
{
  int lid = this->map_->LID(0);
  v.Scale(1.0, *(this->coeff_[lid]));
}

void
Stokhos::EpetraMultiVectorOrthogPoly::
computeStandardDeviation(Epetra_MultiVector& v) const
{
  const Teuchos::Array<double>& nrm2 = this->basis_->norm_squared();
  v.PutScalar(0.0);
  int i_gid;
  for (int i=1; i<this->size(); i++) {
    i_gid = this->map_->GID(i);
    v.Multiply(nrm2[i_gid], *(this->coeff_[i]), *(this->coeff_[i]), 1.0);
  }
  for (int j=0; j<v.NumVectors(); j++)
    for (int i=0; i<v.MyLength(); i++)
      v[j][i] = std::sqrt(v[j][i]);
}
