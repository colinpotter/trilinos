// @HEADER
// ***********************************************************************
//
//                Copyright message goes here.   TODO
//
// ***********************************************************************
// @HEADER

/*! \file Zoltan2_XpetraMultiVectorInput.hpp
    \brief Defines the XpetraMultiVectorInput adapter.
*/

#ifndef _ZOLTAN2_XPETRAMULTIVECTORINPUT_HPP_
#define _ZOLTAN2_XPETRAMULTIVECTORINPUT_HPP_

#include <Zoltan2_XpetraTraits.hpp>
#include <Zoltan2_VectorInput.hpp>
#include <Zoltan2_StridedData.hpp>
#include <Zoltan2_Util.hpp>

#include <Xpetra_EpetraMultiVector.hpp>
#include <Xpetra_TpetraMultiVector.hpp>

namespace Zoltan2 {

/*!  \brief An input adapter for Xpetra::MultiVector.

    The template parameter is the user's input object: 
    \li \c Epetra_MultiVector
    \li \c Tpetra::MultiVector
    \li \c Xpetra::MultiVector

    The \c scalar_t type, representing use data such as matrix values, is
    used by Zoltan2 for weights, coordinates, part sizes and
    quality metrics.
    Some User types (like Tpetra::CrsMatrix) have an inherent scalar type,
    and some
    (like Tpetra::CrsGraph) do not.  For such objects, the scalar type is
    set by Zoltan2 to \c float.  If you wish to change it to double, set
    the second template parameter to \c double.
*/

template <typename User, typename Scalar=typename InputTraits<User>::scalar_t>
  class XpetraMultiVectorInput : public VectorInput<User, Scalar> {
public:

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef Scalar scalar_t;
  typedef typename InputTraits<User>::lno_t    lno_t;
  typedef typename InputTraits<User>::gno_t    gno_t;
  typedef typename InputTraits<User>::gid_t    gid_t;
  typedef typename InputTraits<User>::node_t   node_t;
  typedef VectorInput<User, Scalar>       base_adapter_t;
  typedef User user_t;

  typedef Xpetra::MultiVector<scalar_t, lno_t, gno_t, node_t> x_mvector_t;
  typedef Xpetra::TpetraMultiVector<
    scalar_t, lno_t, gno_t, node_t> xt_mvector_t;
  typedef Xpetra::EpetraMultiVector xe_mvector_t;
#endif

  /*! \brief Destructor
   */
  ~XpetraMultiVectorInput() { }

  /*! \brief Constructor   
   *
   *  \param invector  the user's Xpetra, Tpetra or Epetra MultiVector object
   *  \param weights  a list of pointers to arrays of weights.
   *      The number of weights per multivector element is assumed to be
   *      \c weights.size().
   *  \param weightStrides  a list of strides for the \c weights.
   *     The weight for weight dimension \c n for multivector element
   *     \c k should be found at <tt>weights[n][weightStrides[n] * k]</tt>.
   *     If \c weightStrides.size() is zero, it is assumed all strides are one.
   *
   *  The values pointed to the arguments must remain valid for the
   *  lifetime of this InputAdapter.
   */

  XpetraMultiVectorInput(const RCP<const User> &invector,
    vector<const scalar_t *> &weights, vector<int> &weightStrides);

  /*! \brief Constructor for case when weights are not being used.
   *
   *  \param invector  the user's Xpetra, Tpetra or Epetra MultiVector object
   */

  XpetraMultiVectorInput(const RCP<const User> &invector);

  /*! \brief Access to xpetra wrapper multivector
   */

  const RCP<const x_mvector_t> &getVector() const
  {
    return vector_;
  }

  ////////////////////////////////////////////////////
  // The InputAdapter interface.
  ////////////////////////////////////////////////////

  string inputAdapterName()const {return string("XpetraMultiVector");}

  size_t getLocalNumberOfObjects() const { return getLocalLength();}

  int getNumberOfWeightsPerObject() const { return numWeights_;}

  ////////////////////////////////////////////////////
  // The VectorInput interface.
  ////////////////////////////////////////////////////

  int getNumberOfVectors() const {return vector_->getNumVectors();}

  int getNumberOfWeights() const {return numWeights_;}
  
  size_t getLocalLength() const {return vector_->getLocalLength();}
  
  size_t getGlobalLength() const {return vector_->getGlobalLength();}

  size_t getVector(const gid_t *&Ids, 
    const scalar_t *&elements, int &stride) const
  {
    return getVector(0, Ids, elements, stride);
  }

  size_t getVector(int i, const gid_t *&Ids, 
    const scalar_t *&elements, int &stride) const;

  size_t getVectorWeights(int dim, const scalar_t *&weights, int &stride) const
  {
    env_->localInputAssertion(__FILE__, __LINE__, "invalid dimension",
      dim >= 0 && dim < numWeights_, BASIC_ASSERTION);

    size_t length;

    weights_[dim]->getStridedList(length, weights, stride);

    return length;
  }

  template <typename Adapter>
    size_t applyPartitioningSolution(const User &in, User *&out,
         const PartitioningSolution<Adapter> &solution) const;

private:

  RCP<const User> invector_;
  RCP<const x_mvector_t> vector_;
  RCP<const Xpetra::Map<lno_t, gno_t, node_t> > map_;
  RCP<Environment> env_;    // for error messages, etc.
  lno_t base_;

  int numWeights_;
  Array<RCP<StridedData<lno_t, scalar_t> > > weights_;
};

//////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////

template <typename User, typename Scalar>
  XpetraMultiVectorInput<User, Scalar>::XpetraMultiVectorInput(
    const RCP<const User> &invector,
    vector<const Scalar *> &weights, vector<int> &weightStrides):
      invector_(invector), vector_(), map_(), 
      env_(rcp(new Environment)), base_(),
      numWeights_(weights.size()), weights_(weights.size())
{
  typedef StridedData<lno_t, scalar_t> input_t;

  vector_ = XpetraTraits<User>::convertToXpetra(invector);
  map_ = vector_->getMap();
  base_ = map_->getIndexBase();

  size_t length = vector_->getLocalLength();

  if (length > 0 && numWeights_ > 0){
    int stride = 1;
    for (int w=0; w < numWeights_; w++){
      if (weightStrides.size())
        stride = weightStrides[w];
      weights_[w] = rcp<input_t>(new input_t(
        ArrayView<const scalar_t>(weights[w], stride*length), stride));
    }
  }
}


template <typename User, typename Scalar>
  XpetraMultiVectorInput<User, Scalar>::XpetraMultiVectorInput(
    const RCP<const User> &invector):
      invector_(invector), vector_(), map_(), 
      env_(rcp(new Environment)), base_(),
      numWeights_(0), weights_(0)
{
  typedef StridedData<lno_t, scalar_t> input_t;

  vector_ = XpetraTraits<User>::convertToXpetra(invector);
  map_ = vector_->getMap();
  base_ = map_->getIndexBase();
}

template <typename User, typename Scalar>
  size_t XpetraMultiVectorInput<User>::getVector(int i, const gid_t *&Ids, 
    const Scalar *&elements, int &stride) const
{
  stride = 1;
  elements = NULL;
  if (map_->lib() == Xpetra::UseTpetra){
    const xt_mvector_t *tvector = 
      dynamic_cast<const xt_mvector_t *>(vector_.get());
     
    if (tvector->getLocalLength() > 0){
      ArrayRCP<const scalar_t> data = tvector->getData(i);
      elements = data.get();
    }
  }
  else if (map_->lib() == Xpetra::UseEpetra){
    const xe_mvector_t *evector = 
      dynamic_cast<const xe_mvector_t *>(vector_.get());
      
    if (evector->getLocalLength() > 0){
      ArrayRCP<const double> data = evector->getData(i);

      // Cast so this will compile when scalar_t is not double,
      // a case when this code should never execute.
      elements = reinterpret_cast<const scalar_t *>(data.get());
    }
  }
  else{
    throw logic_error("invalid underlying lib");
  }

  ArrayView<const gid_t> gids = map_->getNodeElementList();
  Ids = gids.getRawPtr();
  return gids.size();
}

template <typename User, typename Scalar>
  template <typename Adapter>
    size_t XpetraMultiVectorInput<User, Scalar>::applyPartitioningSolution(
      const User &in, User *&out, 
      const PartitioningSolution<Adapter> &solution) const
{
  size_t len = solution.getLocalNumberOfIds();
  const gid_t *gids = solution.getIdList();
  const partId_t *parts = solution.getPartList();
  ArrayRCP<gid_t> gidList = arcp(const_cast<gid_t *>(gids), 0, len, false);
  ArrayRCP<partId_t> partList = arcp(const_cast<partId_t *>(parts), 0, len, 
    false);
  ArrayRCP<lno_t> dummyIn;
  ArrayRCP<gid_t> importList;
  ArrayRCP<lno_t> dummyOut;
  size_t numNewRows;

  const RCP<const Comm<int> > comm = map_->getComm();

  try{
    // Get an import list
    numNewRows = convertSolutionToImportList<Adapter, lno_t>(
      solution, dummyIn, importList, dummyOut);
  }
  Z2_FORWARD_EXCEPTIONS;

  RCP<const User> inPtr = rcp(&in, false);
  lno_t localNumElts = numNewRows;

  RCP<const User> outPtr = XpetraTraits<User>::doMigration(
   inPtr, localNumElts, importList.get());

  out = const_cast<User *>(outPtr.get());
  outPtr.release();
  return numNewRows;
}
  
}  //namespace Zoltan2
  
#endif
