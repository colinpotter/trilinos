// @HEADER
// ***********************************************************************
//
//         Zoltan2: Sandia Partitioning Ordering & Coloring Library
//
//                Copyright message goes here.   TODO
//
// ***********************************************************************
//
// Basic testing of Zoltan2::XpetraCrsGraphInput
/*!  \file XpetraCrsGraphInput.cpp
 *   \brief Test of Zoltan2::XpetraCrsGraphInput class.
 *  \todo add weights and coordinates
 */

#include <string>

#include <Zoltan2_XpetraCrsGraphInput.hpp>
#include <Zoltan2_PartitioningSolution.hpp>
#include <Zoltan2_TestHelpers.hpp>

#include <Teuchos_GlobalMPISession.hpp>
#include <Teuchos_DefaultComm.hpp>
#include <Teuchos_RCP.hpp>
#include <Teuchos_Comm.hpp>
#include <Teuchos_CommHelpers.hpp>

using namespace std;
using Teuchos::RCP;
using Teuchos::rcp;
using Teuchos::rcp_const_cast;
using Teuchos::Comm;
using Teuchos::DefaultComm;
using Teuchos::Array;
using Teuchos::ArrayView;

typedef UserInputForTests uinput_t;
typedef Tpetra::CrsGraph<lno_t, gno_t, node_t> tgraph_t;
typedef Xpetra::CrsGraph<lno_t, gno_t, node_t> xgraph_t;
typedef Epetra_CrsGraph egraph_t;

void printGraph(RCP<const Comm<int> > &comm, lno_t nvtx,
    const gno_t *vtxIds, const lno_t *offsets, const gno_t *edgeIds)
{
  int rank = comm->getRank();
  int nprocs = comm->getSize();
  comm->barrier();
  for (int p=0; p < nprocs; p++){
    if (p == rank){
      std::cout << rank << ":" << std::endl;
      for (lno_t i=0; i < nvtx; i++){
        std::cout << " vertex " << vtxIds[i] << ": ";
        for (lno_t j=offsets[i]; j < offsets[i+1]; j++){
          std::cout << edgeIds[j] << " ";
        }
        std::cout << std::endl;
      }
      std::cout.flush();
    }
    comm->barrier();
  }
  comm->barrier();
}

template <typename User>
int verifyInputAdapter(
  Zoltan2::XpetraCrsGraphInput<User, scalar_t> &ia, tgraph_t &graph)
{
  RCP<const Comm<int> > comm = graph.getComm();
  int fail = 0, gfail=0;

  if (!fail && ia.getLocalNumberOfVertices() != graph.getNodeNumRows())
    fail = 4;

  if (!fail && ia.getGlobalNumberOfVertices() != graph.getGlobalNumRows())
    fail = 5;

  if (!fail && ia.getLocalNumberOfEdges() != graph.getNodeNumEntries())
      fail = 6;

  if (!fail && ia.getGlobalNumberOfEdges() != graph.getGlobalNumEntries())
    fail = 7;

  gfail = globalFail(comm, fail);

  const gno_t *vtxIds=NULL, *edgeIds=NULL;
  const lno_t *offsets=NULL;
  size_t nvtx=0;

  if (!gfail){

    nvtx = ia.getVertexListView(vtxIds, offsets, edgeIds);

    if (nvtx != graph.getNodeNumRows())
      fail = 8;

    gfail = globalFail(comm, fail);

    if (gfail == 0){
      printGraph(comm, nvtx, vtxIds, offsets, edgeIds);
    }
    else{
      if (!fail) fail = 10;
    }
  }
  return fail;
}

int main(int argc, char *argv[])
{
  Teuchos::GlobalMPISession session(&argc, &argv);
  RCP<const Comm<int> > comm = DefaultComm<int>::getComm();
  int rank = comm->getRank();
  int fail = 0, gfail=0;

  // Create an object that can give us test Tpetra, Xpetra
  // and Epetra graphs for testing.

  RCP<uinput_t> uinput;

  try{
    uinput =
      rcp(new uinput_t(testDataFilePath+std::string("/simple.mtx"), comm));
  }
  catch(std::exception &e){
    TEST_FAIL_AND_EXIT(*comm, 0, string("input ")+e.what(), 1);
  }

  RCP<tgraph_t> tG;     // original graph (for checking)
  RCP<tgraph_t> newG;   // migrated graph

  tG = uinput->getTpetraCrsGraph();
  size_t nvtx = tG->getNodeNumRows();
  ArrayView<const gno_t> rowGids = tG->getRowMap()->getNodeElementList();

  // To test migration in the input adapter we need a Solution
  // object.  The Solution needs an IdentifierMap.
  // Our solution just assigns all objects to part zero.

  typedef Zoltan2::IdentifierMap<tgraph_t> idmap_t;

  RCP<const Zoltan2::Environment> env = rcp(new Zoltan2::Environment);

  ArrayRCP<const gno_t> gidArray = arcpFromArrayView(rowGids);
  RCP<const idmap_t> idMap = rcp(new idmap_t(env, comm, gidArray));

  int weightDim = 1;

  ArrayRCP<Zoltan2::MetricValues<scalar_t> > metrics;

  zoltan2_partId_t *p = new zoltan2_partId_t [nvtx];
  memset(p, 0, sizeof(zoltan2_partId_t) * nvtx);
  ArrayRCP<zoltan2_partId_t> solnParts(p, 0, nvtx, true);

  typedef Zoltan2::XpetraCrsGraphInput<tgraph_t, scalar_t>  adapter_t;
  typedef Zoltan2::PartitioningSolution<adapter_t> soln_t;
  soln_t solution(env, comm, idMap, weightDim);
  solution.setParts(gidArray, solnParts, metrics);

  /////////////////////////////////////////////////////////////
  // User object is Tpetra::CrsGraph
  if (!gfail){
    RCP<const tgraph_t> ctG = rcp_const_cast<const tgraph_t>(tG);
    RCP<Zoltan2::XpetraCrsGraphInput<tgraph_t, scalar_t> > tGInput;

    try {
      tGInput =
        rcp(new Zoltan2::XpetraCrsGraphInput<tgraph_t, scalar_t>(ctG));
    }
    catch (std::exception &e){
      TEST_FAIL_AND_EXIT(*comm, 0,
        string("XpetraCrsGraphInput ")+e.what(), 1);
    }

    if (rank==0)
      std::cout << "Input adapter for Tpetra::CrsGraph" << std::endl;

    fail = verifyInputAdapter<tgraph_t>(*tGInput, *tG);

    gfail = globalFail(comm, fail);

    if (!gfail){
      tgraph_t *mMigrate = NULL;
      try{
        tGInput->applyPartitioningSolution<adapter_t tgraph_t>(
          *tG, mMigrate, solution);
        newG = rcp(mMigrate);
      }
      catch (std::exception &e){
        fail = 11;
      }

      gfail = globalFail(comm, fail);

      if (!gfail){
        RCP<const tgraph_t> cnewG = rcp_const_cast<const tgraph_t>(newG);
        RCP<Zoltan2::XpetraCrsGraphInput<tgraph_t, scalar_t> > newInput;
        try{
          newInput = rcp(new Zoltan2::XpetraCrsGraphInput<tgraph_t, scalar_t>(cnewG));
        }
        catch (std::exception &e){
          TEST_FAIL_AND_EXIT(*comm, 0,
            string("XpetraCrsGraphInput 2 ")+e.what(), 1);
        }

        if (rank==0){
          std::cout <<
           "Input adapter for Tpetra::CrsGraph migrated to proc 0" <<
           std::endl;
        }
        fail = verifyInputAdapter<tgraph_t>(*newInput, *newG);
        if (fail) fail += 100;
        gfail = globalFail(comm, fail);
      }
    }
    if (gfail){
      printFailureCode(comm, fail);
    }
  }

  /////////////////////////////////////////////////////////////
  // User object is Xpetra::CrsGraph
  if (!gfail){
    RCP<xgraph_t> xG = uinput->getXpetraCrsGraph();
    RCP<const xgraph_t> cxG = rcp_const_cast<const xgraph_t>(xG);
    RCP<Zoltan2::XpetraCrsGraphInput<xgraph_t, scalar_t> > xGInput;

    try {
      xGInput =
        rcp(new Zoltan2::XpetraCrsGraphInput<xgraph_t, scalar_t>(cxG));
    }
    catch (std::exception &e){
      TEST_FAIL_AND_EXIT(*comm, 0,
        string("XpetraCrsGraphInput 3 ")+e.what(), 1);
    }

    if (rank==0){
      std::cout << "Input adapter for Xpetra::CrsGraph" << std::endl;
    }
    fail = verifyInputAdapter<xgraph_t>(*xGInput, *tG);

    gfail = globalFail(comm, fail);

    if (!gfail){
      xgraph_t *mMigrate =NULL;
      try{
        xGInput->applyPartitioningSolution<adapter_t, tgraph_t>(
          *xG, mMigrate, solution);
      }
      catch (std::exception &e){
        fail = 11;
      }

      gfail = globalFail(comm, fail);

      if (!gfail){
        RCP<const xgraph_t> cnewG(mMigrate);
        RCP<Zoltan2::XpetraCrsGraphInput<xgraph_t, scalar_t> > newInput;
        try{
          newInput =
            rcp(new Zoltan2::XpetraCrsGraphInput<xgraph_t, scalar_t>(cnewG));
        }
        catch (std::exception &e){
          TEST_FAIL_AND_EXIT(*comm, 0,
            string("XpetraCrsGraphInput 4 ")+e.what(), 1);
        }

        if (rank==0){
          std::cout <<
           "Input adapter for Xpetra::CrsGraph migrated to proc 0" <<
           std::endl;
        }
        fail = verifyInputAdapter<xgraph_t>(*newInput, *newG);
        if (fail) fail += 100;
        gfail = globalFail(comm, fail);
      }
    }
    if (gfail){
      printFailureCode(comm, fail);
    }
  }

#ifdef HAVE_EPETRA_DATA_TYPES
  /////////////////////////////////////////////////////////////
  // User object is Epetra_CrsGraph
  if (!gfail){
    RCP<egraph_t> eG = uinput->getEpetraCrsGraph();
    RCP<const egraph_t> ceG = rcp_const_cast<const egraph_t>(eG);
    RCP<Zoltan2::XpetraCrsGraphInput<egraph_t, double> > eGInput;

    try {
      eGInput =
        rcp(new Zoltan2::XpetraCrsGraphInput<egraph_t, double>(ceG));
    }
    catch (std::exception &e){
      TEST_FAIL_AND_EXIT(*comm, 0,
        string("XpetraCrsGraphInput 5 ")+e.what(), 1);
    }

    if (rank==0){
      std::cout << "Input adapter for Epetra_CrsGraph" << std::endl;
    }
    fail = verifyInputAdapter<egraph_t>(*eGInput, *tG);

    gfail = globalFail(comm, fail);

    if (!gfail){
      egraph_t *mMigrate =NULL;
      try{
        eGInput->applyPartitioningSolution<adapter_t, tgraph_t>(
          *eG, mMigrate, solution);
      }
      catch (std::exception &e){
        fail = 11;
      }

      gfail = globalFail(comm, fail);

      if (!gfail){
        RCP<const egraph_t> cnewG(mMigrate, true);
        RCP<Zoltan2::XpetraCrsGraphInput<egraph_t, double> > newInput;
        try{
          newInput =
            rcp(new Zoltan2::XpetraCrsGraphInput<egraph_t, double>(cnewG));
        }
        catch (std::exception &e){
          TEST_FAIL_AND_EXIT(*comm, 0,
            string("XpetraCrsGraphInput 6 ")+e.what(), 1);
        }

        if (rank==0){
          std::cout <<
           "Input adapter for Epetra_CrsGraph migrated to proc 0" <<
           std::endl;
        }
        fail = verifyInputAdapter<egraph_t>(*newInput, *newG);
        if (fail) fail += 100;
        gfail = globalFail(comm, fail);
      }
    }
    if (gfail){
      printFailureCode(comm, fail);
    }
  }
#endif

  /////////////////////////////////////////////////////////////
  // DONE

  if (rank==0)
    std::cout << "PASS" << std::endl;
}
