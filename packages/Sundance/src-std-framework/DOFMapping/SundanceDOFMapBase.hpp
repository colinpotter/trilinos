/* @HEADER@ */
/* @HEADER@ */

#ifndef SUNDANCE_DOFMAPBASE_H
#define SUNDANCE_DOFMAPBASE_H

#include "SundanceDefs.hpp"
#include "SundanceMesh.hpp"
#include "SundanceCellSet.hpp"
#include "TSFObjectWithVerbosity.hpp"

#ifndef DOXYGEN_DEVELOPER_ONLY

namespace SundanceStdFwk
{
 using namespace SundanceUtils;
using namespace SundanceStdMesh;
using namespace SundanceStdMesh::Internal;
  namespace Internal
  {
    using namespace Teuchos;

    /** 
     * 
     */
    class DOFMapBase : public TSFExtended::ObjectWithVerbosity<DOFMapBase>,
                       public TSFExtended::Printable
    {
    public:
      /** */
      DOFMapBase(const Mesh& mesh);

      
      /** */
      virtual ~DOFMapBase(){;}

      /** */
      bool isRemote(int cellDim, int cellLID, int& owner) const 
      {return (owner=mesh_.ownerProcID(cellDim, cellLID)) != localProcID_;}

      /** */
      virtual void getDOFsForCell(int cellDim, int cellLID,
                                  int funcID,
                                  Array<int>& dofs) const = 0 ;

      /** */
      virtual void getSingleFuncDOFsForCellBatch(int cellDim, const Array<int>& cellLID,
                                                 int funcID,
                                                 Array<int>& dofs, int& nNodes) const = 0 ;

      /** */
      virtual void getDOFsForCellBatch(int cellDim, const Array<int>& cellLID,
                                       Array<int>& dofs, int& nNodes) const = 0 ;

      /** */
      virtual void getDOFsForCellBatch(int cellDim, const Array<int>& cellLID,
                                       const Array<int>& funcID,
                                       Array<int>& dofs, int& nNodes) const = 0 ;

      /** */
      const CellSet& cellSet(int i) const {return cellSets_[i];}

      /** */
      int numCellSets() const {return cellSets_.size();}

      /** */
      const Array<int>& funcIDOnCellSet(int i) const 
      {return funcIDOnCellSets_[i];}

      /** */
      int cellDimOnCellSet(int i) const 
      {return cellDimOnCellSets_[i];}

      /** */
      int lowestLocalDOF() const {return lowestLocalDOF_;}

      /** */
      bool isLocalDOF(int dof) const 
      {return (dof >= lowestLocalDOF_ && dof < lowestLocalDOF_+numLocalDOFs_);}

      /** */
      int numLocalDOFs() const {return numLocalDOFs_;}

      /** */
      int numDOFs() const {return numDOFs_;}

      /** */
      const RefCountPtr<Array<int> >& ghostIndices() const 
      {return ghostIndices_;}


    protected:

      void setLowestLocalDOF(int low) {lowestLocalDOF_ = low;}

      void setNumLocalDOFs(int numDOFs) {numLocalDOFs_ = numDOFs;}

      void setTotalNumDOFs(int numDOFs) {numDOFs_ = numDOFs;}

      const Mesh& mesh() const {return mesh_;}

      const MPIComm& comm() const {return mesh().comm();}

      Array<CellSet>& cellSets() {return cellSets_;}

      Array<Array<int> >& funcIDOnCellSets() {return funcIDOnCellSets_;}

      Array<int>& cellDimOnCellSets() {return cellDimOnCellSets_;}

      void addGhostIndex(int dof) {ghostIndices_->append(dof);}

    private:
      int localProcID_;

      Mesh mesh_;

      Array<CellSet> cellSets_;

      Array<Array<int> > funcIDOnCellSets_;

      Array<int> cellDimOnCellSets_;

      int lowestLocalDOF_;

      int numLocalDOFs_;

      int numDOFs_;

      RefCountPtr<Array<int> > ghostIndices_;

      Array<Array<int> > dofsHaveBeenAssigned_;
      
    };
  }
}

#endif  /* DOXYGEN_DEVELOPER_ONLY */

#endif
