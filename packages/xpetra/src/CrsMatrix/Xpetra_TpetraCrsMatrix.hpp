#ifndef XPETRA_TPETRACRSMATRIX_HPP
#define XPETRA_TPETRACRSMATRIX_HPP

/* this file is automatically generated - do not edit (see script/tpetra.py) */

#include "Xpetra_TpetraConfigDefs.hpp"

#include "Tpetra_CrsMatrix.hpp"

#include "Xpetra_CrsMatrix.hpp"
#include "Xpetra_TpetraMap.hpp"
#include "Xpetra_TpetraMultiVector.hpp"
#include "Xpetra_TpetraVector.hpp"
#include "Xpetra_TpetraCrsGraph.hpp"
#include "Xpetra_Exceptions.hpp"

namespace Xpetra {

  // TODO: move that elsewhere
  // template <class Scalar, class LocalOrdinal, class GlobalOrdinal, class Node, class LocalMatOps>
  // const Tpetra::CrsMatrix<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps> toTpetraCrsMatrix(const Xpetra::DistObject<char, LocalOrdinal, GlobalOrdinal, Node> &);
  //

  template <class Scalar, class LocalOrdinal = int, class GlobalOrdinal = LocalOrdinal, class Node = Kokkos::DefaultNode::DefaultNodeType, class LocalMatOps = typename Kokkos::DefaultKernels<Scalar,LocalOrdinal,Node>::SparseOps>
  class TpetraCrsMatrix
    : public CrsMatrix<Scalar,LocalOrdinal,GlobalOrdinal,Node,LocalMatOps>
  {

    // The following typedef are used by the XPETRA_DYNAMIC_CAST() macro.
    typedef TpetraCrsMatrix<Scalar,LocalOrdinal,GlobalOrdinal,Node,LocalMatOps> TpetraCrsMatrixClass;
    typedef TpetraVector<Scalar,LocalOrdinal,GlobalOrdinal,Node> TpetraVectorClass;

  public:

    //! @name Constructor/Destructor Methods
    //@{

    //! Constructor specifying fixed number of entries for each row.
    TpetraCrsMatrix(const Teuchos::RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > > &rowMap, size_t maxNumEntriesPerRow, ProfileType pftype=DynamicProfile, const Teuchos::RCP< Teuchos::ParameterList > &plist=Teuchos::null)
      : mtx_(Teuchos::rcp(new Tpetra::CrsMatrix< Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps >(toTpetra(rowMap), maxNumEntriesPerRow, toTpetra(pftype), plist))) { }

    //! Constructor specifying (possibly different) number of entries in each row.
    TpetraCrsMatrix(const Teuchos::RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > > &rowMap, const ArrayRCP< const size_t > &NumEntriesPerRowToAlloc, ProfileType pftype=DynamicProfile, const Teuchos::RCP< Teuchos::ParameterList > &plist=Teuchos::null)
      : mtx_(Teuchos::rcp(new Tpetra::CrsMatrix< Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps >(toTpetra(rowMap), NumEntriesPerRowToAlloc, toTpetra(pftype), plist))) { }

    //! Constructor specifying column Map and fixed number of entries for each row.
    TpetraCrsMatrix(const Teuchos::RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > > &rowMap, const Teuchos::RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > > &colMap, size_t maxNumEntriesPerRow, ProfileType pftype=DynamicProfile, const Teuchos::RCP< Teuchos::ParameterList > &plist=Teuchos::null)
      : mtx_(Teuchos::rcp(new Tpetra::CrsMatrix< Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps >(toTpetra(rowMap), toTpetra(colMap), maxNumEntriesPerRow, toTpetra(pftype), plist))) { }

    //! Constructor specifying column Map and number of entries in each row.
    TpetraCrsMatrix(const Teuchos::RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > > &rowMap, const Teuchos::RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > > &colMap, const ArrayRCP< const size_t > &NumEntriesPerRowToAlloc, ProfileType pftype=DynamicProfile, const Teuchos::RCP< Teuchos::ParameterList > &plist=Teuchos::null)
      : mtx_(Teuchos::rcp(new Tpetra::CrsMatrix< Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps >(toTpetra(rowMap), toTpetra(colMap), NumEntriesPerRowToAlloc, toTpetra(pftype), plist))) { }

    //! Constructor specifying a previously constructed graph.
    TpetraCrsMatrix(const Teuchos::RCP< const CrsGraph< LocalOrdinal, GlobalOrdinal, Node, LocalMatOps > > &graph, const Teuchos::RCP< Teuchos::ParameterList > &plist=Teuchos::null)
      : mtx_(Teuchos::rcp(new Tpetra::CrsMatrix< Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps >(toTpetra(graph), plist))) { }

    //! Destructor.
    virtual ~TpetraCrsMatrix() { }

    //@}

    //! @name Insertion/Removal Methods
    //@{

    //! Insert matrix entries, using global IDs.
    void insertGlobalValues(GlobalOrdinal globalRow, const ArrayView< const GlobalOrdinal > &cols, const ArrayView< const Scalar > &vals) { mtx_->insertGlobalValues(globalRow, cols, vals); }

    //! Scale the current values of a matrix, this = alpha*this.
    void scale(const Scalar &alpha) { mtx_->scale(alpha); }

    //@}

    //! @name Transformational Methods
    //@{

    //! Signal that data entry is complete, specifying domain and range maps.
    void fillComplete(const RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > > &domainMap, const RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > > &rangeMap, OptimizeOption os=DoOptimizeStorage) { mtx_->fillComplete(toTpetra(domainMap), toTpetra(rangeMap), toTpetra(os)); }

    //! Signal that data entry is complete.
    void fillComplete(OptimizeOption os=DoOptimizeStorage) { mtx_->fillComplete(toTpetra(os)); }

    //@}

    //! @name Methods implementing RowMatrix
    //@{

    //! Returns the Map that describes the row distribution in this matrix.
    const RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > >  getRowMap() const { return toXpetra(mtx_->getRowMap()); }

    //! Returns the Map that describes the column distribution in this matrix.
    const RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > >  getColMap() const { return toXpetra(mtx_->getColMap()); }

    //! Returns the CrsGraph associated with this matrix.
    RCP< const CrsGraph< LocalOrdinal, GlobalOrdinal, Node, LocalMatOps > > getCrsGraph() const { return toXpetra(mtx_->getCrsGraph()); }

    //! Number of global elements in the row map of this matrix.
    global_size_t getGlobalNumRows() const { return mtx_->getGlobalNumRows(); }

    //! Number of global columns in the matrix.
    global_size_t getGlobalNumCols() const { return mtx_->getGlobalNumCols(); }

    //! Returns the number of matrix rows owned on the calling node.
    size_t getNodeNumRows() const { return mtx_->getNodeNumRows(); }

    //! Returns the number of columns connected to the locally owned rows of this matrix.
    size_t getNodeNumCols() const { return mtx_->getNodeNumCols(); }

    //! Returns the global number of entries in this matrix.
    global_size_t getGlobalNumEntries() const { return mtx_->getGlobalNumEntries(); }

    //! Returns the local number of entries in this matrix.
    size_t getNodeNumEntries() const { return mtx_->getNodeNumEntries(); }

    //! Returns the current number of entries on this node in the specified local row.
    size_t getNumEntriesInLocalRow(LocalOrdinal localRow) const { return mtx_->getNumEntriesInLocalRow(localRow); }

    //! Returns the number of global diagonal entries, based on global row/column index comparisons.
    global_size_t getGlobalNumDiags() const { return mtx_->getGlobalNumDiags(); }

    //! Returns the number of local diagonal entries, based on global row/column index comparisons.
    size_t getNodeNumDiags() const { return mtx_->getNodeNumDiags(); }

    //! Returns the maximum number of entries across all rows/columns on all nodes.
    size_t getGlobalMaxNumRowEntries() const { return mtx_->getGlobalMaxNumRowEntries(); }

    //! Returns the maximum number of entries across all rows/columns on this node.
    size_t getNodeMaxNumRowEntries() const { return mtx_->getNodeMaxNumRowEntries(); }

    //! If matrix indices are in the local range, this function returns true. Otherwise, this function returns false.
    bool isLocallyIndexed() const { return mtx_->isLocallyIndexed(); }

    //! If matrix indices are in the global range, this function returns true. Otherwise, this function returns false.
    bool isGloballyIndexed() const { return mtx_->isGloballyIndexed(); }

    //! Returns true if fillComplete() has been called and the matrix is in compute mode.
    bool isFillComplete() const { return mtx_->isFillComplete(); }

    //! Returns the Frobenius norm of the matrix.
    typename ScalarTraits< Scalar >::magnitudeType getFrobeniusNorm() const { return mtx_->getFrobeniusNorm(); }

    //! Extract a list of entries in a specified local row of the matrix. Put into storage allocated by calling routine.
    void getLocalRowCopy(LocalOrdinal LocalRow, const ArrayView< LocalOrdinal > &Indices, const ArrayView< Scalar > &Values, size_t &NumEntries) const { mtx_->getLocalRowCopy(LocalRow, Indices, Values, NumEntries); }

    //! Extract a const, non-persisting view of global indices in a specified row of the matrix.
    void getGlobalRowView(GlobalOrdinal GlobalRow, ArrayView< const GlobalOrdinal > &indices, ArrayView< const Scalar > &values) const { mtx_->getGlobalRowView(GlobalRow, indices, values); }

    //! Extract a const, non-persisting view of local indices in a specified row of the matrix.
    void getLocalRowView(LocalOrdinal LocalRow, ArrayView< const LocalOrdinal > &indices, ArrayView< const Scalar > &values) const { mtx_->getLocalRowView(LocalRow, indices, values); }

    //@}

    //! @name Methods implementing Operator
    //@{

    //! Computes the sparse matrix-multivector multiplication.
    void apply(const MultiVector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &X, MultiVector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &Y, Teuchos::ETransp mode=Teuchos::NO_TRANS, Scalar alpha=ScalarTraits< Scalar >::one(), Scalar beta=ScalarTraits< Scalar >::zero()) const { mtx_->apply(toTpetra(X), toTpetra(Y), mode, alpha, beta); }

    //! Returns the Map associated with the domain of this operator. This will be null until fillComplete() is called.
    const RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > >  getDomainMap() const { return toXpetra(mtx_->getDomainMap()); }

    //! 
    const RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > >  getRangeMap() const { return toXpetra(mtx_->getRangeMap()); }

    //@}

    //! @name Overridden from Teuchos::Describable
    //@{

    //! Return a simple one-line description of this object.
    std::string description() const { return mtx_->description(); }

    //! Print the object with some verbosity level to an FancyOStream object.
    void describe(Teuchos::FancyOStream &out, const Teuchos::EVerbosityLevel verbLevel=Teuchos::Describable::verbLevel_default) const { mtx_->describe(out, verbLevel); }

    //@}

    //! Get a copy of the diagonal entries owned by this node, with local row idices.
    void getLocalDiagCopy(Vector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &diag) const { 
      XPETRA_DYNAMIC_CAST(TpetraVectorClass, diag, tDiag, "Xpetra::TpetraCrsMatrix.getLocalDiagCopy() only accept Xpetra::TpetraVector as input arguments.");
      mtx_->getLocalDiagCopy(*tDiag.getTpetra_Vector()); 
      // mtx_->getLocalDiagCopy(toTpetra(diag)); 
    }

    //! Implements DistObject interface
    //{@

    //! Access function for the Tpetra::Map this DistObject was constructed with.
    const Teuchos::RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > > getMap() const { return rcp( new TpetraMap< LocalOrdinal, GlobalOrdinal, Node >(mtx_->getMap()) ); }

    //! Import.
    void doImport(const DistObject<char, LocalOrdinal, GlobalOrdinal, Node> &source, 
                  const Import< LocalOrdinal, GlobalOrdinal, Node > &importer, CombineMode CM) { 

      XPETRA_DYNAMIC_CAST(const TpetraCrsMatrixClass, source, tSource, "Xpetra::TpetraCrsMatrix::doImport only accept Xpetra::TpetraCrsMatrix as input arguments.");//TODO: remove and use toTpetra()
      RCP< const Tpetra::CrsMatrix<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps> > v = tSource.getTpetra_CrsMatrix();
      //mtx_->doImport(toTpetraCrsMatrix(source), *tImporter.getTpetra_Import(), toTpetra(CM));
      mtx_->doImport(*v, toTpetra(importer), toTpetra(CM));
    }

    //! Export.
    void doExport(const DistObject<char, LocalOrdinal, GlobalOrdinal, Node> &dest,
                  const Import< LocalOrdinal, GlobalOrdinal, Node >& importer, CombineMode CM) {

      XPETRA_DYNAMIC_CAST(const TpetraCrsMatrixClass, dest, tDest, "Xpetra::TpetraCrsMatrix::doImport only accept Xpetra::TpetraCrsMatrix as input arguments.");//TODO: remove and use toTpetra()
      RCP< const Tpetra::CrsMatrix<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps> > v = tDest.getTpetra_CrsMatrix();
      mtx_->doExport(*v, toTpetra(importer), toTpetra(CM)); 

    }

    //! Import (using an Exporter).
    void doImport(const DistObject<char, LocalOrdinal, GlobalOrdinal, Node> &source,
                  const Export< LocalOrdinal, GlobalOrdinal, Node >& exporter, CombineMode CM) {

      XPETRA_DYNAMIC_CAST(const TpetraCrsMatrixClass, source, tSource, "Xpetra::TpetraCrsMatrix::doImport only accept Xpetra::TpetraCrsMatrix as input arguments.");//TODO: remove and use toTpetra()
      RCP< const Tpetra::CrsMatrix<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps> > v = tSource.getTpetra_CrsMatrix();
      mtx_->doImport(*v, toTpetra(exporter), toTpetra(CM));

    }

    //! Export (using an Importer).
    void doExport(const DistObject<char, LocalOrdinal, GlobalOrdinal, Node> &dest,
                  const Export< LocalOrdinal, GlobalOrdinal, Node >& exporter, CombineMode CM) {

      XPETRA_DYNAMIC_CAST(const TpetraCrsMatrixClass, dest, tDest, "Xpetra::TpetraCrsMatrix::doImport only accept Xpetra::TpetraCrsMatrix as input arguments.");//TODO: remove and use toTpetra()
      RCP< const Tpetra::CrsMatrix<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps> > v = tDest.getTpetra_CrsMatrix();
      mtx_->doExport(*v, toTpetra(exporter), toTpetra(CM)); 

    }

    // @}

    //! @name Xpetra specific
    //@{

    //! TpetraCrsMatrix constructor to wrap a Tpetra::CrsMatrix object
    TpetraCrsMatrix(const Teuchos::RCP<Tpetra::CrsMatrix<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps> > &mtx) : mtx_(mtx) {  }

    //! Get the underlying Tpetra matrix
    RCP<const Tpetra::CrsMatrix<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps> > getTpetra_CrsMatrix() const { return mtx_; }
    
    //! Get the underlying Tpetra matrix
    RCP<Tpetra::CrsMatrix<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps> > getTpetra_CrsMatrixNonConst() const { return mtx_; } //TODO: remove
 
   //@}
    
  private:
    
    RCP< Tpetra::CrsMatrix<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps> > mtx_;

  }; // TpetraCrsMatrix class

  // TODO: move that elsewhere
  // template <class Scalar, class LocalOrdinal, class GlobalOrdinal, class Node, class LocalMatOps>
  // const Tpetra::DistObject<char, LocalOrdinal, GlobalOrdinal, Node> toTpetraCrsMatrix(const DistObject<char, LocalOrdinal, GlobalOrdinal, Node> &mtx) {
  //   typedef TpetraCrsMatrix<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps> TpetraCrsMatrixClass;
  //   XPETRA_DYNAMIC_CAST(const TpetraCrsMatrixClass, mtx, tMtx, "toTpetra");
  //   return *tMtx.getTpetra_CrsMatrix();
  // }
  //

} // Xpetra namespace

#define XPETRA_TPETRACRSMATRIX_SHORT
#endif // XPETRA_TPETRACRSMATRIX_HPP
