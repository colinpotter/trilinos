#! /usr/bin/env python

# @HEADER
# ************************************************************************
#
#              PyTrilinos.Epetra: Python Interface to Epetra
#                   Copyright (2005) Sandia Corporation
#
# Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
# license for use of this work by or on behalf of the U.S. Government.
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA
# Questions? Contact Michael A. Heroux (maherou@sandia.gov)
#
# ************************************************************************
# @HEADER

# -------------------------------------------------------------------------
# This example shows how to redistribute Epetra distributed objects.
#
# The example creates two maps. The first map has all the elements on
# processor 0 only, the second has a linear distribution, so that 
# approximatively the same number of elements is given to each processor.
# Then, an Epetra.Import object is created, and used to move elements from
# a vector defined on processor 0 only, to a distributed vector.
# 
# This example should be run with more than one processor
# -------------------------------------------------------------------------

try:
  import setpath
  import Epetra
except:
  try:
    from PyTrilinos import Epetra
  except ImportError:
    raise ImportError, "error w/ Epetra"

# Initializes the communicator (or do-nothing in serial runs), then
# creates a communicator (Epetra.SerialComm in serial, or Epetra.MpiComm
# in parallel).
Comm  = Epetra.PyComm()

NumGlobalRows = 10
if Comm.MyPID() == 0:
  i = NumGlobalRows
else:
  i = 0

# Map1 contains all the elements on processor 0, Map2 is a linear map
# with elements on all processes
Map1 = Epetra.Map(-1, range(0, i), 0, Comm)
Map2 = Epetra.Map(NumGlobalRows, 0, Comm)

# Creates an importer from Map1 to Map2
Importer = Epetra.Import(Map2, Map1)
# Defines two vectors, X1 defined on Map1, and X2 defined on Map2,
# then fix the values of X1, and let X2 import the values of X1
X1 = Epetra.Vector(Map1)
X1.Random()
X2 = Epetra.Vector(Map2)
X2.Import(X1, Importer, Epetra.Insert)
print X2

# We now do exactly the same thing, but using an export element,
# and X1_bis should be equivalent to X1. The printed norm should
# be zero
Exporter = Epetra.Export(Map2, Map1)
X1_bis = Epetra.Vector(Map1)
X1_bis.Export(X2, Exporter, Epetra.Insert)

X1_bis.Update(-1.0, X1, 1.0)
(ierr, Norm) = X1_bis.Norm2()
if Comm.MyPID() == 0:
  print '||X1 - X1_bis||_2 = ', Norm
