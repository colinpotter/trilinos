/* @HEADER@ */
// ************************************************************************
// 
//                 Playa: Programmable Linear Algebra
//                 Copyright 2012 Sandia Corporation
// 
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Kevin Long (kevin.long@ttu.edu)
// 

/* @HEADER@ */

#ifndef PLAYA_EPETRAGHOSTIMPORTER_HPP
#define PLAYA_EPETRAGHOSTIMPORTER_HPP

#include "PlayaDefs.hpp"
#include "PlayaGhostImporter.hpp"
#include "PlayaGhostView.hpp"
#include "Epetra_Map.h"
#include "Epetra_Import.h"
#include "Teuchos_Utils.hpp"



namespace Playa
{
  using namespace Teuchos;


  /**
   * Ghost element importer for Epetra vectors
   */
  class EpetraGhostImporter : public GhostImporter<double>
    {
    public:
      /** */
      EpetraGhostImporter(const RCP<const Epetra_Map>& epetraMap,
                          int nGhost,
                          const int* ghostElements);
      /** virtual dtor */
      virtual ~EpetraGhostImporter() {;}

      /** 
       * Import the ghost elements of the given vector
       * as specified during construction of this object. 
       */
      virtual void importView(const Vector<double>& x,
                              RCP<GhostView<double> >& ghostView) const ;

    private:
      RCP<const Epetra_Map> localMap_;

      RCP<const Epetra_Map> ghostMap_;

      RCP<Epetra_Import> importer_;
    };
  
}

#endif
