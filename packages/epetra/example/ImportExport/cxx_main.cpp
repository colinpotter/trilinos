#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "mpi.h"
#include "Epetra_MpiComm.h"
#include "Epetra_Map.h"
#include "Epetra_LocalMap.h"
#include "Epetra_Vector.h" 
#include "Epetra_Import.h"
#include "Epetra_Export.h"
int main(int argc, char *argv[])
{
  MPI_Init(&argc,&argv);
  Epetra_MpiComm Comm( MPI_COMM_WORLD );

  cout << Comm <<endl;

  if (Comm.NumProc()!=3) cout << "Must be run on 3 MPI processes." << endl;

  int NumMyElements = 2;
  Epetra_Map SourceMap(-1, NumMyElements, 0, Comm);
  int NumGlobalElements = SourceMap.NumGlobalElements();
  
  Epetra_Vector SourceX(SourceMap);

  // Define SourceX = [0, 1, 2, 3, 4, 5]' with 2 elements per process

  SourceX[0] = (double) SourceMap.GID(0);
  SourceX[1] = (double) SourceMap.GID(1);

  cout << SourceX; // Print it to stdout

  // Construct first target map: PE0 gets all elements of SourceX

  int TargetOneNumMyElements;
  if (Comm.MyPID()==0) TargetOneNumMyElements = NumGlobalElements;
  else TargetOneNumMyElements = 0;

  Epetra_Map TargetOneMap(-1, TargetOneNumMyElements, 0, Comm);

  Epetra_Import ImporterOne(TargetOneMap, SourceMap);

  Epetra_Vector TargetOneX(TargetOneMap);

  TargetOneX.Import(SourceX, ImporterOne, Insert);

  cout << TargetOneX << endl;

  int * GlobalElementList = new int[2];
  GlobalElementList[0] = 5 - SourceMap.GID(0);
  GlobalElementList[1] = 5 - SourceMap.GID(1);
  
  Epetra_Map TargetTwoMap(-1, 2, GlobalElementList, 0, Comm);

  Epetra_Import ImporterTwo(TargetTwoMap, TargetOneMap);

  Epetra_Vector TargetTwoX(TargetTwoMap);

  TargetTwoX.Import(TargetOneX, ImporterTwo, Insert);

  cout << TargetTwoX << endl;
  
  Epetra_LocalMap TargetThreeMap(6, 0, Comm);

  Epetra_Import ImporterThree(TargetThreeMap, TargetOneMap);

  Epetra_Vector TargetThreeX(TargetThreeMap);

  TargetThreeX.Import(TargetOneX, ImporterThree, Insert);

  cout << TargetThreeX << endl;
  
  Epetra_Export ExporterOne(TargetThreeMap, SourceMap);

  Epetra_Vector TargetFourX(SourceMap);

  TargetFourX.Import(TargetThreeX, ExporterOne, Add);

  cout << TargetFourX << endl;
  


  MPI_Finalize();
  return 0;
}
