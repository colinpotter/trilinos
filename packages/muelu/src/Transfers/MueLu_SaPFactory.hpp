#ifndef MUELU_SAPFACTORY_HPP
#define MUELU_SAPFACTORY_HPP

#include <iostream>
#include "MueLu_OperatorFactory.hpp"

namespace MueLu {
template<class SC, class LO, class GO, class NO, class LMO>
class SaPFactory;
}

#include "MueLu_UseDefaultTypes.hpp"
#include "MueLu_UseShortNames.hpp"

namespace MueLu {

/*!
  @class SaPFactory class.
  @brief Factory for building Smoothed Aggregation prolongators.

  Right now this factory assumes a 1D problem.  Aggregation is hard-coded to divide
  the # fine dofs by 3.
*/

template<class SC, class LO, class GO, class NO, class LMO>
class SaPFactory : public OperatorFactory<SC,LO,GO,NO, LMO> {

  template<class AA, class BB, class CC, class DD, class EE>
  inline friend std::ostream& operator<<(std::ostream& os, SaPFactory<AA,BB,CC,DD, EE> &factory);

  typedef Level<SC,LO,GO,NO, LMO> Level;

  private:
/*
     AggFact_
     CoalesceFact_
     diagonalView_ = 'current' % diagonal view label (default == current view)
*/
     GO maxCoarseSize_;
     SC dampingFactor_;
     bool doQR_;
     bool reUseP_;
     bool reUsePtent_;
     bool reUseGraph_;
     bool reUseAggregates_;

  public:
    //! @name Constructors/Destructors.
    //@{

    //! Constructor.
    SaPFactory() {
      Teuchos::OSTab tab(this->out_);
      MueLu_cout(Teuchos::VERB_HIGH) << "SaPFactory: Instantiating a new factory" << std::endl;
    }

    //! Destructor.
    virtual ~SaPFactory() {}
    //@}

    //! @name Build methods.
    //@{

    /*!
      @brief Build method.

      Builds smoothed aggregation prolongator and returns it in <tt>coarseLevel</tt>.
    */
    bool Build(Level &fineLevel, Level &coarseLevel) {
      Teuchos::OSTab tab(this->out_);
      MueLu_cout(Teuchos::VERB_HIGH) << "SaPFactory: Building a prolongator" << std::endl;

      /*
        1) Grab the fine level matrix and determine the # fine dofs.
        2) Divide by 3 to get the # coarse dofs.
        3) Set up the blocking (sparsity) of the prolongator
        4) Normalize each column.
        5) Voila!
      */
      Teuchos::RCP< Operator > Op = fineLevel.GetA();
      GO nFineDofs = Op->getGlobalNumRows();
      GO nCoarseDofs = nFineDofs/3;
      //prolongator is nFineDofs by nCoarseDofs
      //Teuchos::RCP<Cthulhu::CrsOperator> Ptent = Teuchos::rcp( new Cthulhu::CrsOperator(Op->Rowmap(), 2) );
      Teuchos::RCP< Cthulhu::Operator > Ptent = Teuchos::rcp( new Cthulhu::CrsOperator(Op->Rowmap(), 2) );


      return true;
    }
    //@}

    //! @name Set methods.
    //@{
    void SetMaxCoarseSize(GO maxCoarseSize) {
      maxCoarseSize_ = maxCoarseSize;
    }

    void SetDampingFactor(SC dampingFactor) {
      dampingFactor_ = dampingFactor;
    }

    void TentativeWithQR(bool value) {
      doQR_ = value;
    }

    void ReUseP(bool value) {
      reUseP_ = value;
    }

    void ReUsePtent(bool value) {
      reUsePtent_ = value;
    }

    void ReUseAggregates(bool value) {
      reUseAggregates_ = value;
    }

    void ReUseGraph(bool value) {
      reUseGraph_ = value;
    }
    //@}

    //! @name Get methods.
    //@{

    GO GetMaxCoarseSize() {
      return maxCoarseSize_;
    }

    SC GetDampingFactor() {
      return dampingFactor_;
    }

    bool TentativeWithQR() {
      return doQR_;
    }

    bool ReUseP() {
      return reUseP_;
    }

    bool ReUsePtent() {
      return reUsePtent_;
    }

    bool ReUseAggregates() {
      return reUseAggregates_;
    }

    bool ReUseGraph() {
      return reUseGraph_;
    }

    //@}
/*
//TODO
function [this] = SaPFactory(CoalesceFact,AggFact, diagonalView) //copy ctor
function SetDiagonalView(this, diagonalView)
function [z] = GetNeeds(this)
function  AggInfo = BuildAggregates(FineLevel, ...
function  [P,CoarseNull] = MakeTentative(AggInfo,Amat,nullspace,QROnOrOff,OutputLevel)
function  [P] = MakeNoQRTentative(AggInfo,Amat,nullspace,OutputLevel)
*/

}; //class SaPFactory

//! Friend print function.
template<class SC, class LO, class GO, class NO, class LMO>
std::ostream& operator<<(std::ostream& os, SaPFactory<SC,LO,GO,NO, LMO> &factory) {
  os << "Printing an SaPFactory object" << std::endl;
  return os;
}

} //namespace MueLu

#endif //ifndef MUELU_SAPFACTORY_HPP
