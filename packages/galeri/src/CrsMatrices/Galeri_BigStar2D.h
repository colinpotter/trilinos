// @HEADER
// ************************************************************************
//
//                  Galeri Matrix Generation Package
//                 Copyright (2006) Sandia Corporation
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
// ************************************************************************
// @HEADER

#ifndef GALERI_BIGSTAR2D_H
#define GALERI_BIGSTAR2D_H

#include "Galeri_Exception.h"
#include "Galeri_Utils.h"
#include "Epetra_Comm.h"
#include "Epetra_BlockMap.h"
#include "Epetra_CrsMatrix.h"

namespace Galeri {
namespace Matrices {

inline
Epetra_CrsMatrix* 
BigStar2D(const Epetra_Map* Map, const int nx, const int ny,
          const double a, const double b, const double c,
          const double d, const double e,
          const double z1, const double z2,
          const double z3, const double z4,
          const double bb, const double cc, const double dd, const double ee)

{
  Epetra_CrsMatrix* Matrix = new Epetra_CrsMatrix(Copy, *Map,  13);

  int NumMyElements = Map->NumMyElements();
  int* MyGlobalElements = Map->MyGlobalElements();

  int left, right, lower, upper;
  int left2, right2, lower2, upper2;
  double Values[13];
  int Indices[13];

  //        ee
  //    z3  e  z4
  // bb  b  a  c  cc
  //    z1  d  z2
  //        dd
  
  for (int i = 0 ; i < NumMyElements ; ++i) 
  {
    int NumEntries = 0;
    GetNeighboursCartesian2d(MyGlobalElements[i], nx, ny, 
			     left, right, lower, upper,
			     left2, right2, lower2, upper2);

    if (left != -1) 
    {
      Values[NumEntries] = b;
      Indices[NumEntries] = left;
      ++NumEntries;
    }
    if (right != -1) 
    {
      Values[NumEntries] = c;
      Indices[NumEntries] = right;
      ++NumEntries;
    }
    if (lower != -1) 
    {
      Values[NumEntries] = d;
      Indices[NumEntries] = lower;
      ++NumEntries;
    }
    if (upper != -1) 
    {
      Values[NumEntries] = e;
      Indices[NumEntries] = upper;
      ++NumEntries;
    }
    if (left != -1 && lower != -1) 
    {
      Values[NumEntries] = z1;
      Indices[NumEntries] = lower - 1;
      ++NumEntries;
    }
    if (right != -1 && lower != -1) 
    {
      Values[NumEntries] = z2;
      Indices[NumEntries] = lower + 1;
      ++NumEntries;
    }
    if (left != -1 && upper != -1) 
    {
      Values[NumEntries] = z3;
      Indices[NumEntries] = upper - 1;
      ++NumEntries;
    }
    if (right != -1 && upper != -1) 
    {
      Values[NumEntries] = z4;
      Indices[NumEntries] = upper + 1;
      ++NumEntries;
    }
    if (left2 != -1) 
    {
      Values[NumEntries] = bb;
      Indices[NumEntries] = left2;
      ++NumEntries;
    }
    if (right2 != -1) 
    {
      Values[NumEntries] = cc;
      Indices[NumEntries] = right2;
      ++NumEntries;
    }
    if (lower2 != -1) 
    {
      Values[NumEntries] = dd;
      Indices[NumEntries] = lower2;
      ++NumEntries;
    }
    if (upper2 != -1) 
    {
      Values[NumEntries] = ee;
      Indices[NumEntries] = upper2;
      ++NumEntries;
    }
    
    Values[NumEntries] = a;
    Indices[NumEntries] = MyGlobalElements[i];
    ++NumEntries;

    Matrix->InsertGlobalValues(MyGlobalElements[i], NumEntries, 
                               &Values[0], &Indices[0]);
  }

  Matrix->FillComplete();
  Matrix->OptimizeStorage();

  return(Matrix);
}

} // namespace Matrices
} // namespace Galeri
#endif
