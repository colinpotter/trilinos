/*
 * MueLu_Interpreter_decl.hpp
 *
 *  Created on: Dec 7, 2011
 *      Author: wiesner
 */

#ifndef MUELU_MLINTERPRETER_DECL_HPP
#define MUELU_MLINTERPRETER_DECL_HPP

#include <Teuchos_ParameterList.hpp>

#include <Xpetra_Operator_fwd.hpp>
#include <Xpetra_MultiVector_fwd.hpp>

#include "MueLu_ConfigDefs.hpp"
#include "MueLu_HierarchyManager.hpp"
#include "MueLu_MLInterpreter_fwd.hpp"

#include "MueLu_Hierarchy_fwd.hpp"
#include "MueLu_SmootherFactory_fwd.hpp"

#include "MueLu_TentativePFactory_fwd.hpp"
#include "MueLu_SaPFactory_fwd.hpp"
#include "MueLu_PgPFactory_fwd.hpp"
#include "MueLu_TransPFactory_fwd.hpp"
#include "MueLu_GenericRFactory_fwd.hpp"
#include "MueLu_SmootherPrototype_fwd.hpp"
#include "MueLu_SmootherFactory_fwd.hpp"
#include "MueLu_TrilinosSmoother_fwd.hpp"
#include "MueLu_IfpackSmoother_fwd.hpp"
#include "MueLu_DirectSolver_fwd.hpp"
#include "MueLu_HierarchyHelpers_fwd.hpp"
#include "MueLu_RAPFactory_fwd.hpp"
#include "MueLu_CoalesceDropFactory_fwd.hpp"
#include "MueLu_UCAggregationFactory_fwd.hpp"
#include "MueLu_NullspaceFactory_fwd.hpp"

namespace MueLu {

  /*!
    @class MLInterpreter class.
    @brief Class that accepts ML-style parameters and builds a MueLu preconditioner.
  */

  template <class Scalar = double, class LocalOrdinal = int, class GlobalOrdinal = LocalOrdinal, class Node = Kokkos::DefaultNode::DefaultNodeType, class LocalMatOps = typename Kokkos::DefaultKernels<void,LocalOrdinal,Node>::SparseOps>
  class MLInterpreter : public HierarchyManager<Scalar, LocalOrdinal, GlobalOrdinal, Node, LocalMatOps> { 
#undef MUELU_MLINTERPRETER_SHORT
#include "MueLu_UseShortNames.hpp"

  public:
    //! @name Constructors/Destructors.
    //@{

    //! Constructor.
    MLInterpreter() : nullspace_(NULL) { }

    //! Constructor.
    MLInterpreter(Teuchos::ParameterList & paramList);

    //! Constructor.
    MLInterpreter(const std::string & xmlFileName);
    
    //! Destructor.
    virtual ~MLInterpreter() { }

    //@}

    //@{

    void SetParameterList(const Teuchos::ParameterList & paramList);

    //@}

    //@{

    //! Setup Hierarchy object
    virtual void SetupHierarchy(Hierarchy & H) const;

    //@}

    //! @name Example
    //TODO: should not be part of this class

    //! Build an example of valid ML parameter list
    static void FillMLParameterList(Teuchos::ParameterList & params);

  private:

    //@{

    //! @name Helper functions translating parameter list to factories
    //@{

    //! Read coarse solver options and build the corresponding smoother factory
    static RCP<SmootherFactory> GetCoarsestSolverFactory(const Teuchos::ParameterList & params);

    //! Read smoother options and build the corresponding smoother factory
    static RCP<SmootherFactory> GetSmootherFactory(const Teuchos::ParameterList & params, int level);

    //@}

    //! nullspace can be embedded in the ML parameter list
    int     nullspaceDim_;
    double* nullspace_; //TODO: replace by Teuchos::ArrayRCP<>

  }; // class MLInterpreter

} // namespace MueLu

#define MUELU_MLINTERPRETER_SHORT
#endif /* MUELU_MLINTERPRETER_DECL_HPP */
