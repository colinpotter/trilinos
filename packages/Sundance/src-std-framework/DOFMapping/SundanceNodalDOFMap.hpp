/* @HEADER@ */
// ************************************************************************
// 
//                              Sundance
//                 Copyright (2005) Sandia Corporation
// 
// Copyright (year first published) Sandia Corporation.  Under the terms 
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government 
// retains certain rights in this software.
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
// Questions? Contact Kevin Long (krlong@sandia.gov), 
// Sandia National Laboratories, Livermore, California, USA
// 
// ************************************************************************
/* @HEADER@ */

#ifndef SUNDANCE_NODALDOFMAP_H
#define SUNDANCE_NODALDOFMAP_H

#include "SundanceDefs.hpp"
#include "SundanceSpatiallyHomogeneousDOFMapBase.hpp"
#include "SundanceCellSet.hpp"
#include "SundanceCellFilter.hpp"
#include "SundanceBasisFamily.hpp"
#include "SundanceObjectWithVerbosity.hpp"

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
    class NodalDOFMap : public SpatiallyHomogeneousDOFMapBase
    {
    public:
      /** */
      NodalDOFMap(const Mesh& mesh, int nFuncs,
        const CellFilter& maxCellFilter, 
        const ParameterList& verbParams = *DOFMapBase::defaultVerbParams());
      
      /** */
      virtual ~NodalDOFMap(){;}

      /** */
      RefCountPtr<const MapStructure> 
      getDOFsForCellBatch(int cellDim,
                          const Array<int>& cellLID,
                          const Set<int>& requestedFuncSet,
                          Array<Array<int> >& dofs,
                          Array<int>& nNodes) const ;

      /** */
      RefCountPtr<const MapStructure> mapStruct() const 
      {return structure_;}

      /** */
      int nFuncs() const {return nFuncs_;}

    protected:

      void init();

      void computeOffsets(int localCount)  ;

      void shareRemoteDOFs(const Array<Array<int> >& remoteNodes);

      CellFilter maxCellFilter_;
      int dim_;
      int nFuncs_;
      int nElems_;
      int nNodes_;
      int nNodesPerElem_;
      Array<int> elemDofs_;
      Array<int> nodeDofs_;
      RefCountPtr<const MapStructure> structure_;
    };
  }
}

#endif  /* DOXYGEN_DEVELOPER_ONLY */

#endif
