#include "CEpetra_Comm.h"
#include "CTrilinos_enums.h"
#include "CTrilinos_exceptions.hpp"

#include "CEpetra_UnitTestHelpers.hpp"
#include "Teuchos_UnitTestHarness.hpp"


namespace {


/**********************************************************************
CT_Epetra_Comm_ID_t Epetra_Comm_Cast ( CTrilinos_Object_ID_t id );
 **********************************************************************/

TEUCHOS_UNIT_TEST( Epetra_Comm , Cast )
{
  ECHO(CEpetra_Test_CleanSlate());

  ECHO(CT_Epetra_Comm_ID_t selfID = UnitTest_Create_Comm());

  /* If no exception thrown, then test was successful */
}


/**********************************************************************
void Epetra_Comm_Destroy ( CT_Epetra_Comm_ID_t * selfID );
 **********************************************************************/

TEUCHOS_UNIT_TEST( Epetra_Comm , Destroy )
{
  ECHO(CEpetra_Test_CleanSlate());

  ECHO(CT_Epetra_Comm_ID_t selfID = UnitTest_Create_Comm());

  ECHO(Epetra_Comm_Destroy(&selfID));

  /* Now check the result of the call to the wrapper function */
  TEST_EQUALITY(selfID.type, CT_Invalid_ID);
  TEST_EQUALITY_CONST(selfID.index, -1);
}


/**********************************************************************
void Epetra_Comm_Barrier ( CT_Epetra_Comm_ID_t selfID );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_Broadcast_Double ( 
  CT_Epetra_Comm_ID_t selfID, double * MyVals, int Count, int Root );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_Broadcast_Int ( 
  CT_Epetra_Comm_ID_t selfID, int * MyVals, int Count, int Root );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_Broadcast_Long ( 
  CT_Epetra_Comm_ID_t selfID, long * MyVals, int Count, int Root );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_Broadcast_Char ( 
  CT_Epetra_Comm_ID_t selfID, char * MyVals, int Count, int Root );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_GatherAll_Double ( 
  CT_Epetra_Comm_ID_t selfID, double * MyVals, double * AllVals, 
  int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_GatherAll_Int ( 
  CT_Epetra_Comm_ID_t selfID, int * MyVals, int * AllVals, int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_GatherAll_Long ( 
  CT_Epetra_Comm_ID_t selfID, long * MyVals, long * AllVals, 
  int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_SumAll_Double ( 
  CT_Epetra_Comm_ID_t selfID, double * PartialSums, 
  double * GlobalSums, int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_SumAll_Int ( 
  CT_Epetra_Comm_ID_t selfID, int * PartialSums, int * GlobalSums, 
  int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_SumAll_Long ( 
  CT_Epetra_Comm_ID_t selfID, long * PartialSums, long * GlobalSums, 
  int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_MaxAll_Double ( 
  CT_Epetra_Comm_ID_t selfID, double * PartialMaxs, 
  double * GlobalMaxs, int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_MaxAll_Int ( 
  CT_Epetra_Comm_ID_t selfID, int * PartialMaxs, int * GlobalMaxs, 
  int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_MaxAll_Long ( 
  CT_Epetra_Comm_ID_t selfID, long * PartialMaxs, long * GlobalMaxs, 
  int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_MinAll_Double ( 
  CT_Epetra_Comm_ID_t selfID, double * PartialMins, 
  double * GlobalMins, int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_MinAll_Int ( 
  CT_Epetra_Comm_ID_t selfID, int * PartialMins, int * GlobalMins, 
  int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_MinAll_Long ( 
  CT_Epetra_Comm_ID_t selfID, long * PartialMins, long * GlobalMins, 
  int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_ScanSum_Double ( 
  CT_Epetra_Comm_ID_t selfID, double * MyVals, double * ScanSums, 
  int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_ScanSum_Int ( 
  CT_Epetra_Comm_ID_t selfID, int * MyVals, int * ScanSums, 
  int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_ScanSum_Long ( 
  CT_Epetra_Comm_ID_t selfID, long * MyVals, long * ScanSums, 
  int Count );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_MyPID ( CT_Epetra_Comm_ID_t selfID );
 **********************************************************************/


/**********************************************************************
int Epetra_Comm_NumProc ( CT_Epetra_Comm_ID_t selfID );
 **********************************************************************/


/**********************************************************************/

//
// Template Instantiations
//


#ifdef TEUCHOS_DEBUG

#  define DEBUG_UNIT_TEST_GROUP( T ) \

#else

#  define DEBUG_UNIT_TEST_GROUP( T )

#endif


#define UNIT_TEST_GROUP( T ) \
  DEBUG_UNIT_TEST_GROUP( T )


} // namespace

