#! /usr/bin/env python

# --------------------------------------------------------------------------- #
# This example shows how to build rectangular and square matrices. The example
# works in serial and in parallel with no changed. The code also computes
# some typical operations on matrices.
#
# \author Marzio Sala, SNL 9214
#
# \date Last updated on 05-Aug-05
# --------------------------------------------------------------------------- #

try:
  import setpath
  import Epetra, ML
except:
  from PyTrilinos import Epetra, ML

# -------------------------------------------------------------------------- #
# Main driver.
# -------------------------------------------------------------------------- #
#
def main():

  # The Epetra communicator object can be obtained using ML.GetEpetra_Comm().
  Comm = ML.GetEpetra_Comm()

  # Creates a rectangular matrix, corresponding to non-smoothed
  # aggregation for a simple 1D problem
  NumGlobalRows = 12
  NumGlobalCols = NumGlobalRows / 3
  RowSpace = ML.Space(NumGlobalRows)
  ColSpace = ML.Space(NumGlobalCols)
  MyGlobalRows = RowSpace.GetMyGlobalElements()
  MyGlobalCols = ColSpace.GetMyGlobalElements()
  
  P = ML.PyMatrix(RowSpace, ColSpace)
  P.SetLabel("My_P")

  # Inserts elements of the matrix, using global row and column ID.
  # Off-processor elements can be set (for example, the entire matrix
  # can be specified on processor 0 only). 
  for i in MyGlobalRows:
    P[i, i / 3] = 1
  
  # Most of MLAPI functions will work only *after* having called
  # FillComplete() on the P object. `print P' will not print
  # anything before FillComplete()
  P.FillComplete()

  Comm.Barrier()

  print P

  # Modifies the already inserted elements. Note that new elements cannot 
  # be inserted after FillComplete().
  for i in MyGlobalRows:
    P[i, i / 3] = 10

  # Prints out the nonzeros of the matrix, row-by-row. Note that only
  # *local* elements can be queried.
  for i in MyGlobalRows:
    Indices, Values = P[i]
    for j in xrange(len(Indices)):
      print "P(%d, %d) = %e" %(i, Indices[j], Values[j])

  Comm.Barrier()

  # Prints out the nonzeros of the matrix, element-by-element
  # for a given row
  Row = MyGlobalRows[0]
  for Col in MyGlobalCols:
    print "P(%d, %d) = %f" %(Row, Col, P[Row, Col])

  # Build a tridiagonal matrix by summing up three matrices
  # (clearly there is a quicker way of doing this, by simply setting
  # all the row elements). All operations involving MLAPI objects
  # other than inserting new elements must be done *after* calling
  # FillComplete(). Note that, if you want to multiply an MLAPI
  # object by a number, you should put the number *after* the object.
  A_0 = ML.PyMatrix(RowSpace, RowSpace)
  A_1 = ML.PyMatrix(RowSpace, RowSpace)
  A_2 = ML.PyMatrix(RowSpace, RowSpace)
  for i in MyGlobalRows:
    A_0[i, i] = 2.0
    if i > 0:
      A_1[i, i - 1] = -1.0
    if i < NumGlobalRows - 1:
      A_2[i, i + 1] = -1.0

  A_0.FillComplete()
  A_1.FillComplete()
  A_2.FillComplete()

  # 1) Matrix add
  A = A_0 * 2 + A_1 + A_2
  # 2) Build the identity matrix
  I = ML.GetIdentity(RowSpace, ColSpace)
  # 3) Smooth the prolongator using a (random) polynomial in A
  P = (I * 0.9 - A * 1.23 + A * A * 2.5) * P
  # 4) Get the transpose
  R = ML.GetTranspose(P)
  # 5) Build the coarse matrix using RAP
  A_coarse = R * A * P
  # 6) Scale a matrix with the inverse of the diagonal
  diag = ML.GetDiagonal(A)
  diag.Reciprocal()
  D = ML.GetDiagonal(diag)
  A =  D * A
  # 7) Compute lambda max of A_coarse using the A-norm
  max_eig = ML.MaxEigAnorm(A)
  if Comm.MyPID() == 0:
    print "lambda_max(D * A) using A-norm = ", max_eig
  # 8) Compute lambda max of A_coarse using CG
  max_eig = ML.MaxEigCG(A)
  if Comm.MyPID() == 0:
    print "lambda_max(D * A) using CG     = ", max_eig
  # 9) Compute all the eigenvalues of a matrix, using LAPACK
  #    (MLAPI will convert the matrix into dense format). In this case,
  #    the imaginary part (EI) is zero
  ER = ML.MultiVector()
  EI = ML.MultiVector()
  ML.Eig(A, ER, EI)
  ER.Sort()
  if Comm.MyPID() == 0:
    for i in xrange(NumGlobalRows):
      print "lambda_%d = %e" % (i, ER[i,0])
  ER.Sort()
  # 10) Get the lower triangular part of a distributed matrix
  L = ML.PyMatrix(RowSpace, RowSpace)
  for i in MyGlobalRows:
    Indices, Values = A[i]
    for j in xrange(len(Indices)):
      if Indices[j] <= i:
        L[i, Indices[j]] = Values[j]
  L.FillComplete()

  print L
 
# This is a standard Python construct.  Put the code to be executed in a
# function [typically main()] and then use the following logic to call the
# function if the script has been called as an executable from the UNIX
# command
# line.  This also allows, for example, this file to be imported from a python
# debugger and main() called from there.
if __name__ == "__main__":
  main()
