
/*! @HEADER */
/*
************************************************************************

                CTrilinos:  C interface to Trilinos
                Copyright (2009) Sandia Corporation

Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
license for use of this work by or on behalf of the U.S. Government.

This library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2.1 of the
License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
USA
Questions? Contact M. Nicole Lemaster (mnlemas@sandia.gov)

************************************************************************
*/
/*! @HEADER */


/*! @file CTrilinos_TableRepos_Utils.hpp
 * @brief Central table repository utilities for CTrilinos. */


#ifndef CTRILINOS_TABLEREPOS_UTILS_HPP
#define CTRILINOS_TABLEREPOS_UTILS_HPP


#include "CTrilinos_config.h"


#include <string>

#include "Epetra_Distributor.h"
#include "Epetra_SerialComm.h"
#include "Epetra_BLAS.h"
#include "Epetra_Comm.h"
#include "Epetra_Operator.h"
#include "Epetra_MultiVector.h"
#include "Epetra_OffsetIndex.h"
#include "Epetra_Object.h"
#include "Epetra_RowMatrix.h"
#include "Epetra_CompObject.h"
#include "Epetra_Directory.h"
#include "Epetra_Flops.h"
#include "Epetra_SrcDistObject.h"
#ifdef HAVE_MPI
#include "Epetra_MpiComm.h"
#endif /* HAVE_MPI */
#include "Epetra_CrsMatrix.h"
#include "Epetra_CrsGraph.h"
#include "Epetra_DistObject.h"
#include "Epetra_Vector.h"
#include "Epetra_Export.h"
#include "Epetra_Map.h"
#include "Epetra_BlockMap.h"
#include "Epetra_Import.h"
#include "Epetra_Time.h"
#include "Epetra_JadMatrix.h"
#include "Epetra_LinearProblem.h"
#include "Epetra_LAPACK.h"
#include "Teuchos_CommandLineProcessor.hpp"
#include "Teuchos_ParameterList.hpp"
#include "Teuchos_ParameterEntry.hpp"
#include "Teuchos_any.hpp"
#ifdef HAVE_CTRILINOS_AMESOS
#include "Amesos_BaseSolver.h"
#endif /* HAVE_CTRILINOS_AMESOS */
#ifdef HAVE_CTRILINOS_AMESOS
#include "Amesos.h"
#endif /* HAVE_CTRILINOS_AMESOS */
#include "Epetra_FECrsMatrix.h"
#include "Epetra_IntSerialDenseVector.h"
#include "Epetra_SerialDenseMatrix.h"
#ifdef HAVE_CTRILINOS_AZTECOO
#include "AztecOO.h"
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
#include "AztecOO_StatusTest.h"
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
#include "AztecOO_StatusTestCombo.h"
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
#include "AztecOO_StatusTestMaxIters.h"
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
#include "AztecOO_StatusTestResNorm.h"
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
#include "Ifpack.h"
#endif /* HAVE_CTRILINOS_IFPACK */
#ifdef HAVE_CTRILINOS_IFPACK
#include "Ifpack_Preconditioner.h"
#endif /* HAVE_CTRILINOS_IFPACK */

#include "CTrilinos_enums.h"
#include "CTrilinos_Table.hpp"
#include "CTrilinos_utils_templ.hpp"
#include "Teuchos_RCP.hpp"


namespace CTrilinos {


template <class T>
class poly_checker
{
  public:
//    static const bool is_poly;
//    static const bool non_poly;
};

template <>
class poly_checker<Epetra_Distributor>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_Distributor>::is_poly = true;
const bool poly_checker<Epetra_Distributor>::non_poly = false;

template <>
class poly_checker<Epetra_SerialComm>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_SerialComm>::is_poly = true;
const bool poly_checker<Epetra_SerialComm>::non_poly = false;

template <>
class poly_checker<Epetra_BLAS>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_BLAS>::is_poly = true;
const bool poly_checker<Epetra_BLAS>::non_poly = false;

template <>
class poly_checker<Epetra_Comm>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_Comm>::is_poly = true;
const bool poly_checker<Epetra_Comm>::non_poly = false;

template <>
class poly_checker<Epetra_Operator>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_Operator>::is_poly = true;
const bool poly_checker<Epetra_Operator>::non_poly = false;

template <>
class poly_checker<Epetra_MultiVector>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_MultiVector>::is_poly = true;
const bool poly_checker<Epetra_MultiVector>::non_poly = false;

template <>
class poly_checker<Epetra_OffsetIndex>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_OffsetIndex>::is_poly = true;
const bool poly_checker<Epetra_OffsetIndex>::non_poly = false;

template <>
class poly_checker<Epetra_Object>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_Object>::is_poly = true;
const bool poly_checker<Epetra_Object>::non_poly = false;

template <>
class poly_checker<Epetra_RowMatrix>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_RowMatrix>::is_poly = true;
const bool poly_checker<Epetra_RowMatrix>::non_poly = false;

template <>
class poly_checker<Epetra_CompObject>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_CompObject>::is_poly = true;
const bool poly_checker<Epetra_CompObject>::non_poly = false;

template <>
class poly_checker<Epetra_Directory>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_Directory>::is_poly = true;
const bool poly_checker<Epetra_Directory>::non_poly = false;

template <>
class poly_checker<Epetra_Flops>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_Flops>::is_poly = true;
const bool poly_checker<Epetra_Flops>::non_poly = false;

template <>
class poly_checker<Epetra_SrcDistObject>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_SrcDistObject>::is_poly = true;
const bool poly_checker<Epetra_SrcDistObject>::non_poly = false;

#ifdef HAVE_MPI
template <>
class poly_checker<Epetra_MpiComm>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_MpiComm>::is_poly = true;
const bool poly_checker<Epetra_MpiComm>::non_poly = false;

#endif /* HAVE_MPI */
template <>
class poly_checker<Epetra_CrsMatrix>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_CrsMatrix>::is_poly = true;
const bool poly_checker<Epetra_CrsMatrix>::non_poly = false;

template <>
class poly_checker<Epetra_CrsGraph>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_CrsGraph>::is_poly = true;
const bool poly_checker<Epetra_CrsGraph>::non_poly = false;

template <>
class poly_checker<Epetra_DistObject>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_DistObject>::is_poly = true;
const bool poly_checker<Epetra_DistObject>::non_poly = false;

template <>
class poly_checker<Epetra_Vector>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_Vector>::is_poly = true;
const bool poly_checker<Epetra_Vector>::non_poly = false;

template <>
class poly_checker<Epetra_Export>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_Export>::is_poly = true;
const bool poly_checker<Epetra_Export>::non_poly = false;

template <>
class poly_checker<Epetra_Map>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_Map>::is_poly = true;
const bool poly_checker<Epetra_Map>::non_poly = false;

template <>
class poly_checker<Epetra_BlockMap>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_BlockMap>::is_poly = true;
const bool poly_checker<Epetra_BlockMap>::non_poly = false;

template <>
class poly_checker<Epetra_Import>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_Import>::is_poly = true;
const bool poly_checker<Epetra_Import>::non_poly = false;

template <>
class poly_checker<Epetra_Time>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_Time>::is_poly = true;
const bool poly_checker<Epetra_Time>::non_poly = false;

template <>
class poly_checker<Epetra_JadMatrix>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_JadMatrix>::is_poly = true;
const bool poly_checker<Epetra_JadMatrix>::non_poly = false;

template <>
class poly_checker<Epetra_LinearProblem>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_LinearProblem>::is_poly = true;
const bool poly_checker<Epetra_LinearProblem>::non_poly = false;

template <>
class poly_checker<Epetra_LAPACK>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_LAPACK>::is_poly = true;
const bool poly_checker<Epetra_LAPACK>::non_poly = false;

template <>
class poly_checker<Teuchos::ParameterList>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Teuchos::ParameterList>::is_poly = true;
const bool poly_checker<Teuchos::ParameterList>::non_poly = false;

#ifdef HAVE_CTRILINOS_AMESOS
template <>
class poly_checker<Amesos_BaseSolver>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Amesos_BaseSolver>::is_poly = true;
const bool poly_checker<Amesos_BaseSolver>::non_poly = false;

#endif /* HAVE_CTRILINOS_AMESOS */
template <>
class poly_checker<Epetra_FECrsMatrix>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_FECrsMatrix>::is_poly = true;
const bool poly_checker<Epetra_FECrsMatrix>::non_poly = false;

template <>
class poly_checker<Epetra_IntSerialDenseVector>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_IntSerialDenseVector>::is_poly = true;
const bool poly_checker<Epetra_IntSerialDenseVector>::non_poly = false;

template <>
class poly_checker<Epetra_SerialDenseMatrix>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Epetra_SerialDenseMatrix>::is_poly = true;
const bool poly_checker<Epetra_SerialDenseMatrix>::non_poly = false;

#ifdef HAVE_CTRILINOS_AZTECOO
template <>
class poly_checker<AztecOO_StatusTest>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<AztecOO_StatusTest>::is_poly = true;
const bool poly_checker<AztecOO_StatusTest>::non_poly = false;

#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
template <>
class poly_checker<AztecOO_StatusTestCombo>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<AztecOO_StatusTestCombo>::is_poly = true;
const bool poly_checker<AztecOO_StatusTestCombo>::non_poly = false;

#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
template <>
class poly_checker<AztecOO_StatusTestMaxIters>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<AztecOO_StatusTestMaxIters>::is_poly = true;
const bool poly_checker<AztecOO_StatusTestMaxIters>::non_poly = false;

#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
template <>
class poly_checker<AztecOO_StatusTestResNorm>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<AztecOO_StatusTestResNorm>::is_poly = true;
const bool poly_checker<AztecOO_StatusTestResNorm>::non_poly = false;

#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
template <>
class poly_checker<Ifpack_Preconditioner>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Ifpack_Preconditioner>::is_poly = true;
const bool poly_checker<Ifpack_Preconditioner>::non_poly = false;

#endif /* HAVE_CTRILINOS_IFPACK */
template <>
class poly_checker<Teuchos::CommandLineProcessor>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Teuchos::CommandLineProcessor>::is_poly = false;
const bool poly_checker<Teuchos::CommandLineProcessor>::non_poly = true;

template <>
class poly_checker<Teuchos::ParameterEntry>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Teuchos::ParameterEntry>::is_poly = false;
const bool poly_checker<Teuchos::ParameterEntry>::non_poly = true;

template <>
class poly_checker<Teuchos::any>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Teuchos::any>::is_poly = false;
const bool poly_checker<Teuchos::any>::non_poly = true;

#ifdef HAVE_CTRILINOS_AMESOS
template <>
class poly_checker<Amesos>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Amesos>::is_poly = false;
const bool poly_checker<Amesos>::non_poly = true;

#endif /* HAVE_CTRILINOS_AMESOS */
#ifdef HAVE_CTRILINOS_AZTECOO
template <>
class poly_checker<AztecOO>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<AztecOO>::is_poly = false;
const bool poly_checker<AztecOO>::non_poly = true;

#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
template <>
class poly_checker<Ifpack>
{
  public:
    static const bool is_poly;
    static const bool non_poly;
};

const bool poly_checker<Ifpack>::is_poly = false;
const bool poly_checker<Ifpack>::non_poly = true;

#endif /* HAVE_CTRILINOS_IFPACK */
bool poly_checker_runtime(CTrilinos_Universal_ID_t aid)
{
    switch (aid.table) {
        case CT_Epetra_Distributor_ID:
            return true;
            break;
        case CT_Epetra_SerialComm_ID:
            return true;
            break;
        case CT_Epetra_BLAS_ID:
            return true;
            break;
        case CT_Epetra_Comm_ID:
            return true;
            break;
        case CT_Epetra_Operator_ID:
            return true;
            break;
        case CT_Epetra_MultiVector_ID:
            return true;
            break;
        case CT_Epetra_OffsetIndex_ID:
            return true;
            break;
        case CT_Epetra_Object_ID:
            return true;
            break;
        case CT_Epetra_RowMatrix_ID:
            return true;
            break;
        case CT_Epetra_CompObject_ID:
            return true;
            break;
        case CT_Epetra_Directory_ID:
            return true;
            break;
        case CT_Epetra_Flops_ID:
            return true;
            break;
        case CT_Epetra_SrcDistObject_ID:
            return true;
            break;
#ifdef HAVE_MPI
        case CT_Epetra_MpiComm_ID:
            return true;
            break;
#endif /* HAVE_MPI */
        case CT_Epetra_CrsMatrix_ID:
            return true;
            break;
        case CT_Epetra_CrsGraph_ID:
            return true;
            break;
        case CT_Epetra_DistObject_ID:
            return true;
            break;
        case CT_Epetra_Vector_ID:
            return true;
            break;
        case CT_Epetra_Export_ID:
            return true;
            break;
        case CT_Epetra_Map_ID:
            return true;
            break;
        case CT_Epetra_BlockMap_ID:
            return true;
            break;
        case CT_Epetra_Import_ID:
            return true;
            break;
        case CT_Epetra_Time_ID:
            return true;
            break;
        case CT_Epetra_JadMatrix_ID:
            return true;
            break;
        case CT_Epetra_LinearProblem_ID:
            return true;
            break;
        case CT_Epetra_LAPACK_ID:
            return true;
            break;
        case CT_Teuchos_ParameterList_ID:
            return true;
            break;
#ifdef HAVE_CTRILINOS_AMESOS
        case CT_Amesos_BaseSolver_ID:
            return true;
            break;
#endif /* HAVE_CTRILINOS_AMESOS */
        case CT_Epetra_FECrsMatrix_ID:
            return true;
            break;
        case CT_Epetra_IntSerialDenseVector_ID:
            return true;
            break;
        case CT_Epetra_SerialDenseMatrix_ID:
            return true;
            break;
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTest_ID:
            return true;
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestCombo_ID:
            return true;
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestMaxIters_ID:
            return true;
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestResNorm_ID:
            return true;
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
        case CT_Ifpack_Preconditioner_ID:
            return true;
            break;
#endif /* HAVE_CTRILINOS_IFPACK */
        case CT_Teuchos_CommandLineProcessor_ID:
            return false;
            break;
        case CT_Teuchos_ParameterEntry_ID:
            return false;
            break;
        case CT_Teuchos_any_ID:
            return false;
            break;
#ifdef HAVE_CTRILINOS_AMESOS
        case CT_Amesos_ID:
            return false;
            break;
#endif /* HAVE_CTRILINOS_AMESOS */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_ID:
            return false;
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
        case CT_Ifpack_ID:
            return false;
            break;
#endif /* HAVE_CTRILINOS_IFPACK */
        default:
            throw std::string("invalid table id");
            break;
    }
}

/*! Central table repository for storing objects that are either
 *  polymorphic or non-polymorphic (but not both). */
template <bool POLY>
class TableReposPoly
{
  public:

    /*! constructor */
    TableReposPoly();
    /*! destructor */
    ~TableReposPoly();
};

template <>
class TableReposPoly<true>
{
  public:

    /*! constructor */
    TableReposPoly();

    /*! destructor */
    ~TableReposPoly();

    /*! retrieve the object */
    template <class T>
    const Teuchos::RCP<T> get(CTrilinos_Universal_ID_t id);

    /*! retrieve the object */
    template <class T>
    const Teuchos::RCP<const T> getConst(CTrilinos_Universal_ID_t id);

    /*! store a non-const RCP to object of type T */
    template <class T>
    CTrilinos_Universal_ID_t store(T* pobj, bool owned = true)
    { return getTable<T>().store(pobj, owned); }

    /*! store a const RCP to object of type T */
    template <class T>
    CTrilinos_Universal_ID_t store(const T* pobj, bool owned = false)
    { return getTable<T>().store(pobj, owned); }

    /*! remove an object from the table and invalidate the id struct */
    void remove(CTrilinos_Universal_ID_t * id);

    /*! dump the tables' content but keep their properties */
    void purge();

    /*! create an alias for the object in another table */
    CTrilinos_Universal_ID_t alias(CTrilinos_Universal_ID_t id, CTrilinos_Table_ID_t tab, bool keepold = true);

  private:
    /*! get the Ctrilinos::Table for the given type */
    template <class T>
    Table<T> & getTable();

    /*! create an alias for the object in another table */
    template <class T>
    CTrilinos_Universal_ID_t do_alias(Table<T> &tab, CTrilinos_Universal_ID_t &aid, bool keepold);

    /*! create an alias for the object in another table */
    template <class T>
    CTrilinos_Universal_ID_t do_alias_const(Table<T> &tab, CTrilinos_Universal_ID_t &aid, bool keepold);

    /*! retrieve the object */
    template <class T>
    const Teuchos::RCP<T> getPoly(CTrilinos_Universal_ID_t aid);

    /*! retrieve the object */
    template <class T>
    const Teuchos::RCP<const T> getConstPoly(CTrilinos_Universal_ID_t aid);

    Table<Epetra_Distributor> tab_Epetra_Distributor;
    Table<Epetra_SerialComm> tab_Epetra_SerialComm;
    Table<Epetra_BLAS> tab_Epetra_BLAS;
    Table<Epetra_Comm> tab_Epetra_Comm;
    Table<Epetra_Operator> tab_Epetra_Operator;
    Table<Epetra_MultiVector> tab_Epetra_MultiVector;
    Table<Epetra_OffsetIndex> tab_Epetra_OffsetIndex;
    Table<Epetra_Object> tab_Epetra_Object;
    Table<Epetra_RowMatrix> tab_Epetra_RowMatrix;
    Table<Epetra_CompObject> tab_Epetra_CompObject;
    Table<Epetra_Directory> tab_Epetra_Directory;
    Table<Epetra_Flops> tab_Epetra_Flops;
    Table<Epetra_SrcDistObject> tab_Epetra_SrcDistObject;
#ifdef HAVE_MPI
    Table<Epetra_MpiComm> tab_Epetra_MpiComm;
#endif /* HAVE_MPI */
    Table<Epetra_CrsMatrix> tab_Epetra_CrsMatrix;
    Table<Epetra_CrsGraph> tab_Epetra_CrsGraph;
    Table<Epetra_DistObject> tab_Epetra_DistObject;
    Table<Epetra_Vector> tab_Epetra_Vector;
    Table<Epetra_Export> tab_Epetra_Export;
    Table<Epetra_Map> tab_Epetra_Map;
    Table<Epetra_BlockMap> tab_Epetra_BlockMap;
    Table<Epetra_Import> tab_Epetra_Import;
    Table<Epetra_Time> tab_Epetra_Time;
    Table<Epetra_JadMatrix> tab_Epetra_JadMatrix;
    Table<Epetra_LinearProblem> tab_Epetra_LinearProblem;
    Table<Epetra_LAPACK> tab_Epetra_LAPACK;
    Table<Teuchos::ParameterList> tab_Teuchos_ParameterList;
#ifdef HAVE_CTRILINOS_AMESOS
    Table<Amesos_BaseSolver> tab_Amesos_BaseSolver;
#endif /* HAVE_CTRILINOS_AMESOS */
    Table<Epetra_FECrsMatrix> tab_Epetra_FECrsMatrix;
    Table<Epetra_IntSerialDenseVector> tab_Epetra_IntSerialDenseVector;
    Table<Epetra_SerialDenseMatrix> tab_Epetra_SerialDenseMatrix;
#ifdef HAVE_CTRILINOS_AZTECOO
    Table<AztecOO_StatusTest> tab_AztecOO_StatusTest;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
    Table<AztecOO_StatusTestCombo> tab_AztecOO_StatusTestCombo;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
    Table<AztecOO_StatusTestMaxIters> tab_AztecOO_StatusTestMaxIters;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
    Table<AztecOO_StatusTestResNorm> tab_AztecOO_StatusTestResNorm;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
    Table<Ifpack_Preconditioner> tab_Ifpack_Preconditioner;
#endif /* HAVE_CTRILINOS_IFPACK */

    bool call_me_lazy;  /* I was too lazy to deal with the commas in the init list, so... */
};

TableReposPoly<true>::TableReposPoly() :
    tab_Epetra_Distributor(CT_Epetra_Distributor_ID),
    tab_Epetra_SerialComm(CT_Epetra_SerialComm_ID),
    tab_Epetra_BLAS(CT_Epetra_BLAS_ID),
    tab_Epetra_Comm(CT_Epetra_Comm_ID),
    tab_Epetra_Operator(CT_Epetra_Operator_ID),
    tab_Epetra_MultiVector(CT_Epetra_MultiVector_ID),
    tab_Epetra_OffsetIndex(CT_Epetra_OffsetIndex_ID),
    tab_Epetra_Object(CT_Epetra_Object_ID),
    tab_Epetra_RowMatrix(CT_Epetra_RowMatrix_ID),
    tab_Epetra_CompObject(CT_Epetra_CompObject_ID),
    tab_Epetra_Directory(CT_Epetra_Directory_ID),
    tab_Epetra_Flops(CT_Epetra_Flops_ID),
    tab_Epetra_SrcDistObject(CT_Epetra_SrcDistObject_ID),
#ifdef HAVE_MPI
    tab_Epetra_MpiComm(CT_Epetra_MpiComm_ID),
#endif /* HAVE_MPI */
    tab_Epetra_CrsMatrix(CT_Epetra_CrsMatrix_ID),
    tab_Epetra_CrsGraph(CT_Epetra_CrsGraph_ID),
    tab_Epetra_DistObject(CT_Epetra_DistObject_ID),
    tab_Epetra_Vector(CT_Epetra_Vector_ID),
    tab_Epetra_Export(CT_Epetra_Export_ID),
    tab_Epetra_Map(CT_Epetra_Map_ID),
    tab_Epetra_BlockMap(CT_Epetra_BlockMap_ID),
    tab_Epetra_Import(CT_Epetra_Import_ID),
    tab_Epetra_Time(CT_Epetra_Time_ID),
    tab_Epetra_JadMatrix(CT_Epetra_JadMatrix_ID),
    tab_Epetra_LinearProblem(CT_Epetra_LinearProblem_ID),
    tab_Epetra_LAPACK(CT_Epetra_LAPACK_ID),
    tab_Teuchos_ParameterList(CT_Teuchos_ParameterList_ID),
#ifdef HAVE_CTRILINOS_AMESOS
    tab_Amesos_BaseSolver(CT_Amesos_BaseSolver_ID),
#endif /* HAVE_CTRILINOS_AMESOS */
    tab_Epetra_FECrsMatrix(CT_Epetra_FECrsMatrix_ID),
    tab_Epetra_IntSerialDenseVector(CT_Epetra_IntSerialDenseVector_ID),
    tab_Epetra_SerialDenseMatrix(CT_Epetra_SerialDenseMatrix_ID),
#ifdef HAVE_CTRILINOS_AZTECOO
    tab_AztecOO_StatusTest(CT_AztecOO_StatusTest_ID),
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
    tab_AztecOO_StatusTestCombo(CT_AztecOO_StatusTestCombo_ID),
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
    tab_AztecOO_StatusTestMaxIters(CT_AztecOO_StatusTestMaxIters_ID),
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
    tab_AztecOO_StatusTestResNorm(CT_AztecOO_StatusTestResNorm_ID),
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
    tab_Ifpack_Preconditioner(CT_Ifpack_Preconditioner_ID),
#endif /* HAVE_CTRILINOS_IFPACK */
    call_me_lazy(true)
{
}

TableReposPoly<true>::~TableReposPoly()
{
}

template <class T>
Table<T> & TableReposPoly<true>::getTable()
{
    Teuchos::RCP<T> dummy = Teuchos::null;
    /* I know this is bad, but it's for catching COMPILE-TIME errors! */
    return dummy->No_Function_For_This_Table_Type;
}

template <>
Table<Epetra_Distributor> & TableReposPoly<true>::getTable<Epetra_Distributor>()
{ return tab_Epetra_Distributor; }

template <>
Table<Epetra_SerialComm> & TableReposPoly<true>::getTable<Epetra_SerialComm>()
{ return tab_Epetra_SerialComm; }

template <>
Table<Epetra_BLAS> & TableReposPoly<true>::getTable<Epetra_BLAS>()
{ return tab_Epetra_BLAS; }

template <>
Table<Epetra_Comm> & TableReposPoly<true>::getTable<Epetra_Comm>()
{ return tab_Epetra_Comm; }

template <>
Table<Epetra_Operator> & TableReposPoly<true>::getTable<Epetra_Operator>()
{ return tab_Epetra_Operator; }

template <>
Table<Epetra_MultiVector> & TableReposPoly<true>::getTable<Epetra_MultiVector>()
{ return tab_Epetra_MultiVector; }

template <>
Table<Epetra_OffsetIndex> & TableReposPoly<true>::getTable<Epetra_OffsetIndex>()
{ return tab_Epetra_OffsetIndex; }

template <>
Table<Epetra_Object> & TableReposPoly<true>::getTable<Epetra_Object>()
{ return tab_Epetra_Object; }

template <>
Table<Epetra_RowMatrix> & TableReposPoly<true>::getTable<Epetra_RowMatrix>()
{ return tab_Epetra_RowMatrix; }

template <>
Table<Epetra_CompObject> & TableReposPoly<true>::getTable<Epetra_CompObject>()
{ return tab_Epetra_CompObject; }

template <>
Table<Epetra_Directory> & TableReposPoly<true>::getTable<Epetra_Directory>()
{ return tab_Epetra_Directory; }

template <>
Table<Epetra_Flops> & TableReposPoly<true>::getTable<Epetra_Flops>()
{ return tab_Epetra_Flops; }

template <>
Table<Epetra_SrcDistObject> & TableReposPoly<true>::getTable<Epetra_SrcDistObject>()
{ return tab_Epetra_SrcDistObject; }

#ifdef HAVE_MPI
template <>
Table<Epetra_MpiComm> & TableReposPoly<true>::getTable<Epetra_MpiComm>()
{ return tab_Epetra_MpiComm; }

#endif /* HAVE_MPI */
template <>
Table<Epetra_CrsMatrix> & TableReposPoly<true>::getTable<Epetra_CrsMatrix>()
{ return tab_Epetra_CrsMatrix; }

template <>
Table<Epetra_CrsGraph> & TableReposPoly<true>::getTable<Epetra_CrsGraph>()
{ return tab_Epetra_CrsGraph; }

template <>
Table<Epetra_DistObject> & TableReposPoly<true>::getTable<Epetra_DistObject>()
{ return tab_Epetra_DistObject; }

template <>
Table<Epetra_Vector> & TableReposPoly<true>::getTable<Epetra_Vector>()
{ return tab_Epetra_Vector; }

template <>
Table<Epetra_Export> & TableReposPoly<true>::getTable<Epetra_Export>()
{ return tab_Epetra_Export; }

template <>
Table<Epetra_Map> & TableReposPoly<true>::getTable<Epetra_Map>()
{ return tab_Epetra_Map; }

template <>
Table<Epetra_BlockMap> & TableReposPoly<true>::getTable<Epetra_BlockMap>()
{ return tab_Epetra_BlockMap; }

template <>
Table<Epetra_Import> & TableReposPoly<true>::getTable<Epetra_Import>()
{ return tab_Epetra_Import; }

template <>
Table<Epetra_Time> & TableReposPoly<true>::getTable<Epetra_Time>()
{ return tab_Epetra_Time; }

template <>
Table<Epetra_JadMatrix> & TableReposPoly<true>::getTable<Epetra_JadMatrix>()
{ return tab_Epetra_JadMatrix; }

template <>
Table<Epetra_LinearProblem> & TableReposPoly<true>::getTable<Epetra_LinearProblem>()
{ return tab_Epetra_LinearProblem; }

template <>
Table<Epetra_LAPACK> & TableReposPoly<true>::getTable<Epetra_LAPACK>()
{ return tab_Epetra_LAPACK; }

template <>
Table<Teuchos::ParameterList> & TableReposPoly<true>::getTable<Teuchos::ParameterList>()
{ return tab_Teuchos_ParameterList; }

#ifdef HAVE_CTRILINOS_AMESOS
template <>
Table<Amesos_BaseSolver> & TableReposPoly<true>::getTable<Amesos_BaseSolver>()
{ return tab_Amesos_BaseSolver; }

#endif /* HAVE_CTRILINOS_AMESOS */
template <>
Table<Epetra_FECrsMatrix> & TableReposPoly<true>::getTable<Epetra_FECrsMatrix>()
{ return tab_Epetra_FECrsMatrix; }

template <>
Table<Epetra_IntSerialDenseVector> & TableReposPoly<true>::getTable<Epetra_IntSerialDenseVector>()
{ return tab_Epetra_IntSerialDenseVector; }

template <>
Table<Epetra_SerialDenseMatrix> & TableReposPoly<true>::getTable<Epetra_SerialDenseMatrix>()
{ return tab_Epetra_SerialDenseMatrix; }

#ifdef HAVE_CTRILINOS_AZTECOO
template <>
Table<AztecOO_StatusTest> & TableReposPoly<true>::getTable<AztecOO_StatusTest>()
{ return tab_AztecOO_StatusTest; }

#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
template <>
Table<AztecOO_StatusTestCombo> & TableReposPoly<true>::getTable<AztecOO_StatusTestCombo>()
{ return tab_AztecOO_StatusTestCombo; }

#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
template <>
Table<AztecOO_StatusTestMaxIters> & TableReposPoly<true>::getTable<AztecOO_StatusTestMaxIters>()
{ return tab_AztecOO_StatusTestMaxIters; }

#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
template <>
Table<AztecOO_StatusTestResNorm> & TableReposPoly<true>::getTable<AztecOO_StatusTestResNorm>()
{ return tab_AztecOO_StatusTestResNorm; }

#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
template <>
Table<Ifpack_Preconditioner> & TableReposPoly<true>::getTable<Ifpack_Preconditioner>()
{ return tab_Ifpack_Preconditioner; }

#endif /* HAVE_CTRILINOS_IFPACK */
template <class T>
const Teuchos::RCP<T> TableReposPoly<true>::get(CTrilinos_Universal_ID_t aid)
{
    /* Shortcut if stored in the most obvious table */
    Table<T> &tab_maybe = getTable<T>();
    if (tab_maybe.isType(aid.table))
        return tab_maybe.get<T>(aid);
    else
        return getPoly<T>(aid);
}

template <class T>
const Teuchos::RCP<T> TableReposPoly<true>::getPoly(CTrilinos_Universal_ID_t aid)
{
    switch (aid.table) {
        case CT_Epetra_Distributor_ID:
            return tab_Epetra_Distributor.get<T>(aid);
            break;
        case CT_Epetra_SerialComm_ID:
            return tab_Epetra_SerialComm.get<T>(aid);
            break;
        case CT_Epetra_BLAS_ID:
            return tab_Epetra_BLAS.get<T>(aid);
            break;
        case CT_Epetra_Comm_ID:
            return tab_Epetra_Comm.get<T>(aid);
            break;
        case CT_Epetra_Operator_ID:
            return tab_Epetra_Operator.get<T>(aid);
            break;
        case CT_Epetra_MultiVector_ID:
            return tab_Epetra_MultiVector.get<T>(aid);
            break;
        case CT_Epetra_OffsetIndex_ID:
            return tab_Epetra_OffsetIndex.get<T>(aid);
            break;
        case CT_Epetra_Object_ID:
            return tab_Epetra_Object.get<T>(aid);
            break;
        case CT_Epetra_RowMatrix_ID:
            return tab_Epetra_RowMatrix.get<T>(aid);
            break;
        case CT_Epetra_CompObject_ID:
            return tab_Epetra_CompObject.get<T>(aid);
            break;
        case CT_Epetra_Directory_ID:
            return tab_Epetra_Directory.get<T>(aid);
            break;
        case CT_Epetra_Flops_ID:
            return tab_Epetra_Flops.get<T>(aid);
            break;
        case CT_Epetra_SrcDistObject_ID:
            return tab_Epetra_SrcDistObject.get<T>(aid);
            break;
#ifdef HAVE_MPI
        case CT_Epetra_MpiComm_ID:
            return tab_Epetra_MpiComm.get<T>(aid);
            break;
#endif /* HAVE_MPI */
        case CT_Epetra_CrsMatrix_ID:
            return tab_Epetra_CrsMatrix.get<T>(aid);
            break;
        case CT_Epetra_CrsGraph_ID:
            return tab_Epetra_CrsGraph.get<T>(aid);
            break;
        case CT_Epetra_DistObject_ID:
            return tab_Epetra_DistObject.get<T>(aid);
            break;
        case CT_Epetra_Vector_ID:
            return tab_Epetra_Vector.get<T>(aid);
            break;
        case CT_Epetra_Export_ID:
            return tab_Epetra_Export.get<T>(aid);
            break;
        case CT_Epetra_Map_ID:
            return tab_Epetra_Map.get<T>(aid);
            break;
        case CT_Epetra_BlockMap_ID:
            return tab_Epetra_BlockMap.get<T>(aid);
            break;
        case CT_Epetra_Import_ID:
            return tab_Epetra_Import.get<T>(aid);
            break;
        case CT_Epetra_Time_ID:
            return tab_Epetra_Time.get<T>(aid);
            break;
        case CT_Epetra_JadMatrix_ID:
            return tab_Epetra_JadMatrix.get<T>(aid);
            break;
        case CT_Epetra_LinearProblem_ID:
            return tab_Epetra_LinearProblem.get<T>(aid);
            break;
        case CT_Epetra_LAPACK_ID:
            return tab_Epetra_LAPACK.get<T>(aid);
            break;
        case CT_Teuchos_ParameterList_ID:
            return tab_Teuchos_ParameterList.get<T>(aid);
            break;
#ifdef HAVE_CTRILINOS_AMESOS
        case CT_Amesos_BaseSolver_ID:
            return tab_Amesos_BaseSolver.get<T>(aid);
            break;
#endif /* HAVE_CTRILINOS_AMESOS */
        case CT_Epetra_FECrsMatrix_ID:
            return tab_Epetra_FECrsMatrix.get<T>(aid);
            break;
        case CT_Epetra_IntSerialDenseVector_ID:
            return tab_Epetra_IntSerialDenseVector.get<T>(aid);
            break;
        case CT_Epetra_SerialDenseMatrix_ID:
            return tab_Epetra_SerialDenseMatrix.get<T>(aid);
            break;
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTest_ID:
            return tab_AztecOO_StatusTest.get<T>(aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestCombo_ID:
            return tab_AztecOO_StatusTestCombo.get<T>(aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestMaxIters_ID:
            return tab_AztecOO_StatusTestMaxIters.get<T>(aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestResNorm_ID:
            return tab_AztecOO_StatusTestResNorm.get<T>(aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
        case CT_Ifpack_Preconditioner_ID:
            return tab_Ifpack_Preconditioner.get<T>(aid);
            break;
#endif /* HAVE_CTRILINOS_IFPACK */
        default:
            throw std::string("invalid table id");
            break;
    }

    return Teuchos::null;
}

template <class T>
const Teuchos::RCP<const T> TableReposPoly<true>::getConst(CTrilinos_Universal_ID_t aid)
{
    /* Shortcut if stored in the most obvious table */
    Table<T> &tab_maybe = getTable<T>();
    if (tab_maybe.isType(aid.table))
        return tab_maybe.getConst<T>(aid);
    else
        return getConstPoly<T>(aid);
}

template <class T>
const Teuchos::RCP<const T> TableReposPoly<true>::getConstPoly(CTrilinos_Universal_ID_t aid)
{
    switch (aid.table) {
        case CT_Epetra_Distributor_ID:
            return tab_Epetra_Distributor.getConst<T>(aid);
            break;
        case CT_Epetra_SerialComm_ID:
            return tab_Epetra_SerialComm.getConst<T>(aid);
            break;
        case CT_Epetra_BLAS_ID:
            return tab_Epetra_BLAS.getConst<T>(aid);
            break;
        case CT_Epetra_Comm_ID:
            return tab_Epetra_Comm.getConst<T>(aid);
            break;
        case CT_Epetra_Operator_ID:
            return tab_Epetra_Operator.getConst<T>(aid);
            break;
        case CT_Epetra_MultiVector_ID:
            return tab_Epetra_MultiVector.getConst<T>(aid);
            break;
        case CT_Epetra_OffsetIndex_ID:
            return tab_Epetra_OffsetIndex.getConst<T>(aid);
            break;
        case CT_Epetra_Object_ID:
            return tab_Epetra_Object.getConst<T>(aid);
            break;
        case CT_Epetra_RowMatrix_ID:
            return tab_Epetra_RowMatrix.getConst<T>(aid);
            break;
        case CT_Epetra_CompObject_ID:
            return tab_Epetra_CompObject.getConst<T>(aid);
            break;
        case CT_Epetra_Directory_ID:
            return tab_Epetra_Directory.getConst<T>(aid);
            break;
        case CT_Epetra_Flops_ID:
            return tab_Epetra_Flops.getConst<T>(aid);
            break;
        case CT_Epetra_SrcDistObject_ID:
            return tab_Epetra_SrcDistObject.getConst<T>(aid);
            break;
#ifdef HAVE_MPI
        case CT_Epetra_MpiComm_ID:
            return tab_Epetra_MpiComm.getConst<T>(aid);
            break;
#endif /* HAVE_MPI */
        case CT_Epetra_CrsMatrix_ID:
            return tab_Epetra_CrsMatrix.getConst<T>(aid);
            break;
        case CT_Epetra_CrsGraph_ID:
            return tab_Epetra_CrsGraph.getConst<T>(aid);
            break;
        case CT_Epetra_DistObject_ID:
            return tab_Epetra_DistObject.getConst<T>(aid);
            break;
        case CT_Epetra_Vector_ID:
            return tab_Epetra_Vector.getConst<T>(aid);
            break;
        case CT_Epetra_Export_ID:
            return tab_Epetra_Export.getConst<T>(aid);
            break;
        case CT_Epetra_Map_ID:
            return tab_Epetra_Map.getConst<T>(aid);
            break;
        case CT_Epetra_BlockMap_ID:
            return tab_Epetra_BlockMap.getConst<T>(aid);
            break;
        case CT_Epetra_Import_ID:
            return tab_Epetra_Import.getConst<T>(aid);
            break;
        case CT_Epetra_Time_ID:
            return tab_Epetra_Time.getConst<T>(aid);
            break;
        case CT_Epetra_JadMatrix_ID:
            return tab_Epetra_JadMatrix.getConst<T>(aid);
            break;
        case CT_Epetra_LinearProblem_ID:
            return tab_Epetra_LinearProblem.getConst<T>(aid);
            break;
        case CT_Epetra_LAPACK_ID:
            return tab_Epetra_LAPACK.getConst<T>(aid);
            break;
        case CT_Teuchos_ParameterList_ID:
            return tab_Teuchos_ParameterList.getConst<T>(aid);
            break;
#ifdef HAVE_CTRILINOS_AMESOS
        case CT_Amesos_BaseSolver_ID:
            return tab_Amesos_BaseSolver.getConst<T>(aid);
            break;
#endif /* HAVE_CTRILINOS_AMESOS */
        case CT_Epetra_FECrsMatrix_ID:
            return tab_Epetra_FECrsMatrix.getConst<T>(aid);
            break;
        case CT_Epetra_IntSerialDenseVector_ID:
            return tab_Epetra_IntSerialDenseVector.getConst<T>(aid);
            break;
        case CT_Epetra_SerialDenseMatrix_ID:
            return tab_Epetra_SerialDenseMatrix.getConst<T>(aid);
            break;
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTest_ID:
            return tab_AztecOO_StatusTest.getConst<T>(aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestCombo_ID:
            return tab_AztecOO_StatusTestCombo.getConst<T>(aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestMaxIters_ID:
            return tab_AztecOO_StatusTestMaxIters.getConst<T>(aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestResNorm_ID:
            return tab_AztecOO_StatusTestResNorm.getConst<T>(aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
        case CT_Ifpack_Preconditioner_ID:
            return tab_Ifpack_Preconditioner.getConst<T>(aid);
            break;
#endif /* HAVE_CTRILINOS_IFPACK */
        default:
            throw std::string("invalid table id");
            break;
    }

    return Teuchos::null;
}

template <class T>
CTrilinos_Universal_ID_t TableReposPoly<true>::do_alias(
    Table<T> &tab, CTrilinos_Universal_ID_t &aid, bool keepold)
{
    CTrilinos_Universal_ID_t newid;

    switch (aid.table) {
        case CT_Epetra_Distributor_ID:
            newid = tab.alias(tab_Epetra_Distributor.get<T>(aid));
            if (!keepold) tab_Epetra_Distributor.remove(&aid);
            break;
        case CT_Epetra_SerialComm_ID:
            newid = tab.alias(tab_Epetra_SerialComm.get<T>(aid));
            if (!keepold) tab_Epetra_SerialComm.remove(&aid);
            break;
        case CT_Epetra_BLAS_ID:
            newid = tab.alias(tab_Epetra_BLAS.get<T>(aid));
            if (!keepold) tab_Epetra_BLAS.remove(&aid);
            break;
        case CT_Epetra_Comm_ID:
            newid = tab.alias(tab_Epetra_Comm.get<T>(aid));
            if (!keepold) tab_Epetra_Comm.remove(&aid);
            break;
        case CT_Epetra_Operator_ID:
            newid = tab.alias(tab_Epetra_Operator.get<T>(aid));
            if (!keepold) tab_Epetra_Operator.remove(&aid);
            break;
        case CT_Epetra_MultiVector_ID:
            newid = tab.alias(tab_Epetra_MultiVector.get<T>(aid));
            if (!keepold) tab_Epetra_MultiVector.remove(&aid);
            break;
        case CT_Epetra_OffsetIndex_ID:
            newid = tab.alias(tab_Epetra_OffsetIndex.get<T>(aid));
            if (!keepold) tab_Epetra_OffsetIndex.remove(&aid);
            break;
        case CT_Epetra_Object_ID:
            newid = tab.alias(tab_Epetra_Object.get<T>(aid));
            if (!keepold) tab_Epetra_Object.remove(&aid);
            break;
        case CT_Epetra_RowMatrix_ID:
            newid = tab.alias(tab_Epetra_RowMatrix.get<T>(aid));
            if (!keepold) tab_Epetra_RowMatrix.remove(&aid);
            break;
        case CT_Epetra_CompObject_ID:
            newid = tab.alias(tab_Epetra_CompObject.get<T>(aid));
            if (!keepold) tab_Epetra_CompObject.remove(&aid);
            break;
        case CT_Epetra_Directory_ID:
            newid = tab.alias(tab_Epetra_Directory.get<T>(aid));
            if (!keepold) tab_Epetra_Directory.remove(&aid);
            break;
        case CT_Epetra_Flops_ID:
            newid = tab.alias(tab_Epetra_Flops.get<T>(aid));
            if (!keepold) tab_Epetra_Flops.remove(&aid);
            break;
        case CT_Epetra_SrcDistObject_ID:
            newid = tab.alias(tab_Epetra_SrcDistObject.get<T>(aid));
            if (!keepold) tab_Epetra_SrcDistObject.remove(&aid);
            break;
#ifdef HAVE_MPI
        case CT_Epetra_MpiComm_ID:
            newid = tab.alias(tab_Epetra_MpiComm.get<T>(aid));
            if (!keepold) tab_Epetra_MpiComm.remove(&aid);
            break;
#endif /* HAVE_MPI */
        case CT_Epetra_CrsMatrix_ID:
            newid = tab.alias(tab_Epetra_CrsMatrix.get<T>(aid));
            if (!keepold) tab_Epetra_CrsMatrix.remove(&aid);
            break;
        case CT_Epetra_CrsGraph_ID:
            newid = tab.alias(tab_Epetra_CrsGraph.get<T>(aid));
            if (!keepold) tab_Epetra_CrsGraph.remove(&aid);
            break;
        case CT_Epetra_DistObject_ID:
            newid = tab.alias(tab_Epetra_DistObject.get<T>(aid));
            if (!keepold) tab_Epetra_DistObject.remove(&aid);
            break;
        case CT_Epetra_Vector_ID:
            newid = tab.alias(tab_Epetra_Vector.get<T>(aid));
            if (!keepold) tab_Epetra_Vector.remove(&aid);
            break;
        case CT_Epetra_Export_ID:
            newid = tab.alias(tab_Epetra_Export.get<T>(aid));
            if (!keepold) tab_Epetra_Export.remove(&aid);
            break;
        case CT_Epetra_Map_ID:
            newid = tab.alias(tab_Epetra_Map.get<T>(aid));
            if (!keepold) tab_Epetra_Map.remove(&aid);
            break;
        case CT_Epetra_BlockMap_ID:
            newid = tab.alias(tab_Epetra_BlockMap.get<T>(aid));
            if (!keepold) tab_Epetra_BlockMap.remove(&aid);
            break;
        case CT_Epetra_Import_ID:
            newid = tab.alias(tab_Epetra_Import.get<T>(aid));
            if (!keepold) tab_Epetra_Import.remove(&aid);
            break;
        case CT_Epetra_Time_ID:
            newid = tab.alias(tab_Epetra_Time.get<T>(aid));
            if (!keepold) tab_Epetra_Time.remove(&aid);
            break;
        case CT_Epetra_JadMatrix_ID:
            newid = tab.alias(tab_Epetra_JadMatrix.get<T>(aid));
            if (!keepold) tab_Epetra_JadMatrix.remove(&aid);
            break;
        case CT_Epetra_LinearProblem_ID:
            newid = tab.alias(tab_Epetra_LinearProblem.get<T>(aid));
            if (!keepold) tab_Epetra_LinearProblem.remove(&aid);
            break;
        case CT_Epetra_LAPACK_ID:
            newid = tab.alias(tab_Epetra_LAPACK.get<T>(aid));
            if (!keepold) tab_Epetra_LAPACK.remove(&aid);
            break;
        case CT_Teuchos_ParameterList_ID:
            newid = tab.alias(tab_Teuchos_ParameterList.get<T>(aid));
            if (!keepold) tab_Teuchos_ParameterList.remove(&aid);
            break;
#ifdef HAVE_CTRILINOS_AMESOS
        case CT_Amesos_BaseSolver_ID:
            newid = tab.alias(tab_Amesos_BaseSolver.get<T>(aid));
            if (!keepold) tab_Amesos_BaseSolver.remove(&aid);
            break;
#endif /* HAVE_CTRILINOS_AMESOS */
        case CT_Epetra_FECrsMatrix_ID:
            newid = tab.alias(tab_Epetra_FECrsMatrix.get<T>(aid));
            if (!keepold) tab_Epetra_FECrsMatrix.remove(&aid);
            break;
        case CT_Epetra_IntSerialDenseVector_ID:
            newid = tab.alias(tab_Epetra_IntSerialDenseVector.get<T>(aid));
            if (!keepold) tab_Epetra_IntSerialDenseVector.remove(&aid);
            break;
        case CT_Epetra_SerialDenseMatrix_ID:
            newid = tab.alias(tab_Epetra_SerialDenseMatrix.get<T>(aid));
            if (!keepold) tab_Epetra_SerialDenseMatrix.remove(&aid);
            break;
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTest_ID:
            newid = tab.alias(tab_AztecOO_StatusTest.get<T>(aid));
            if (!keepold) tab_AztecOO_StatusTest.remove(&aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestCombo_ID:
            newid = tab.alias(tab_AztecOO_StatusTestCombo.get<T>(aid));
            if (!keepold) tab_AztecOO_StatusTestCombo.remove(&aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestMaxIters_ID:
            newid = tab.alias(tab_AztecOO_StatusTestMaxIters.get<T>(aid));
            if (!keepold) tab_AztecOO_StatusTestMaxIters.remove(&aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestResNorm_ID:
            newid = tab.alias(tab_AztecOO_StatusTestResNorm.get<T>(aid));
            if (!keepold) tab_AztecOO_StatusTestResNorm.remove(&aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
        case CT_Ifpack_Preconditioner_ID:
            newid = tab.alias(tab_Ifpack_Preconditioner.get<T>(aid));
            if (!keepold) tab_Ifpack_Preconditioner.remove(&aid);
            break;
#endif /* HAVE_CTRILINOS_IFPACK */
        default:
            newid.table = CT_Invalid_ID;
            newid.index = -1;
            newid.is_const = aid.is_const;
            throw std::string("invalid table id or non-polymorphic class");
            break;
    }

    return newid;
}

template <class T>
CTrilinos_Universal_ID_t TableReposPoly<true>::do_alias_const(
    Table<T> &tab, CTrilinos_Universal_ID_t &aid, bool keepold)
{
    CTrilinos_Universal_ID_t newid;

    switch (aid.table) {
        case CT_Epetra_Distributor_ID:
            newid = tab.alias(tab_Epetra_Distributor.getConst<T>(aid));
            if (!keepold) tab_Epetra_Distributor.remove(&aid);
            break;
        case CT_Epetra_SerialComm_ID:
            newid = tab.alias(tab_Epetra_SerialComm.getConst<T>(aid));
            if (!keepold) tab_Epetra_SerialComm.remove(&aid);
            break;
        case CT_Epetra_BLAS_ID:
            newid = tab.alias(tab_Epetra_BLAS.getConst<T>(aid));
            if (!keepold) tab_Epetra_BLAS.remove(&aid);
            break;
        case CT_Epetra_Comm_ID:
            newid = tab.alias(tab_Epetra_Comm.getConst<T>(aid));
            if (!keepold) tab_Epetra_Comm.remove(&aid);
            break;
        case CT_Epetra_Operator_ID:
            newid = tab.alias(tab_Epetra_Operator.getConst<T>(aid));
            if (!keepold) tab_Epetra_Operator.remove(&aid);
            break;
        case CT_Epetra_MultiVector_ID:
            newid = tab.alias(tab_Epetra_MultiVector.getConst<T>(aid));
            if (!keepold) tab_Epetra_MultiVector.remove(&aid);
            break;
        case CT_Epetra_OffsetIndex_ID:
            newid = tab.alias(tab_Epetra_OffsetIndex.getConst<T>(aid));
            if (!keepold) tab_Epetra_OffsetIndex.remove(&aid);
            break;
        case CT_Epetra_Object_ID:
            newid = tab.alias(tab_Epetra_Object.getConst<T>(aid));
            if (!keepold) tab_Epetra_Object.remove(&aid);
            break;
        case CT_Epetra_RowMatrix_ID:
            newid = tab.alias(tab_Epetra_RowMatrix.getConst<T>(aid));
            if (!keepold) tab_Epetra_RowMatrix.remove(&aid);
            break;
        case CT_Epetra_CompObject_ID:
            newid = tab.alias(tab_Epetra_CompObject.getConst<T>(aid));
            if (!keepold) tab_Epetra_CompObject.remove(&aid);
            break;
        case CT_Epetra_Directory_ID:
            newid = tab.alias(tab_Epetra_Directory.getConst<T>(aid));
            if (!keepold) tab_Epetra_Directory.remove(&aid);
            break;
        case CT_Epetra_Flops_ID:
            newid = tab.alias(tab_Epetra_Flops.getConst<T>(aid));
            if (!keepold) tab_Epetra_Flops.remove(&aid);
            break;
        case CT_Epetra_SrcDistObject_ID:
            newid = tab.alias(tab_Epetra_SrcDistObject.getConst<T>(aid));
            if (!keepold) tab_Epetra_SrcDistObject.remove(&aid);
            break;
#ifdef HAVE_MPI
        case CT_Epetra_MpiComm_ID:
            newid = tab.alias(tab_Epetra_MpiComm.getConst<T>(aid));
            if (!keepold) tab_Epetra_MpiComm.remove(&aid);
            break;
#endif /* HAVE_MPI */
        case CT_Epetra_CrsMatrix_ID:
            newid = tab.alias(tab_Epetra_CrsMatrix.getConst<T>(aid));
            if (!keepold) tab_Epetra_CrsMatrix.remove(&aid);
            break;
        case CT_Epetra_CrsGraph_ID:
            newid = tab.alias(tab_Epetra_CrsGraph.getConst<T>(aid));
            if (!keepold) tab_Epetra_CrsGraph.remove(&aid);
            break;
        case CT_Epetra_DistObject_ID:
            newid = tab.alias(tab_Epetra_DistObject.getConst<T>(aid));
            if (!keepold) tab_Epetra_DistObject.remove(&aid);
            break;
        case CT_Epetra_Vector_ID:
            newid = tab.alias(tab_Epetra_Vector.getConst<T>(aid));
            if (!keepold) tab_Epetra_Vector.remove(&aid);
            break;
        case CT_Epetra_Export_ID:
            newid = tab.alias(tab_Epetra_Export.getConst<T>(aid));
            if (!keepold) tab_Epetra_Export.remove(&aid);
            break;
        case CT_Epetra_Map_ID:
            newid = tab.alias(tab_Epetra_Map.getConst<T>(aid));
            if (!keepold) tab_Epetra_Map.remove(&aid);
            break;
        case CT_Epetra_BlockMap_ID:
            newid = tab.alias(tab_Epetra_BlockMap.getConst<T>(aid));
            if (!keepold) tab_Epetra_BlockMap.remove(&aid);
            break;
        case CT_Epetra_Import_ID:
            newid = tab.alias(tab_Epetra_Import.getConst<T>(aid));
            if (!keepold) tab_Epetra_Import.remove(&aid);
            break;
        case CT_Epetra_Time_ID:
            newid = tab.alias(tab_Epetra_Time.getConst<T>(aid));
            if (!keepold) tab_Epetra_Time.remove(&aid);
            break;
        case CT_Epetra_JadMatrix_ID:
            newid = tab.alias(tab_Epetra_JadMatrix.getConst<T>(aid));
            if (!keepold) tab_Epetra_JadMatrix.remove(&aid);
            break;
        case CT_Epetra_LinearProblem_ID:
            newid = tab.alias(tab_Epetra_LinearProblem.getConst<T>(aid));
            if (!keepold) tab_Epetra_LinearProblem.remove(&aid);
            break;
        case CT_Epetra_LAPACK_ID:
            newid = tab.alias(tab_Epetra_LAPACK.getConst<T>(aid));
            if (!keepold) tab_Epetra_LAPACK.remove(&aid);
            break;
        case CT_Teuchos_ParameterList_ID:
            newid = tab.alias(tab_Teuchos_ParameterList.getConst<T>(aid));
            if (!keepold) tab_Teuchos_ParameterList.remove(&aid);
            break;
#ifdef HAVE_CTRILINOS_AMESOS
        case CT_Amesos_BaseSolver_ID:
            newid = tab.alias(tab_Amesos_BaseSolver.getConst<T>(aid));
            if (!keepold) tab_Amesos_BaseSolver.remove(&aid);
            break;
#endif /* HAVE_CTRILINOS_AMESOS */
        case CT_Epetra_FECrsMatrix_ID:
            newid = tab.alias(tab_Epetra_FECrsMatrix.getConst<T>(aid));
            if (!keepold) tab_Epetra_FECrsMatrix.remove(&aid);
            break;
        case CT_Epetra_IntSerialDenseVector_ID:
            newid = tab.alias(tab_Epetra_IntSerialDenseVector.getConst<T>(aid));
            if (!keepold) tab_Epetra_IntSerialDenseVector.remove(&aid);
            break;
        case CT_Epetra_SerialDenseMatrix_ID:
            newid = tab.alias(tab_Epetra_SerialDenseMatrix.getConst<T>(aid));
            if (!keepold) tab_Epetra_SerialDenseMatrix.remove(&aid);
            break;
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTest_ID:
            newid = tab.alias(tab_AztecOO_StatusTest.getConst<T>(aid));
            if (!keepold) tab_AztecOO_StatusTest.remove(&aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestCombo_ID:
            newid = tab.alias(tab_AztecOO_StatusTestCombo.getConst<T>(aid));
            if (!keepold) tab_AztecOO_StatusTestCombo.remove(&aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestMaxIters_ID:
            newid = tab.alias(tab_AztecOO_StatusTestMaxIters.getConst<T>(aid));
            if (!keepold) tab_AztecOO_StatusTestMaxIters.remove(&aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestResNorm_ID:
            newid = tab.alias(tab_AztecOO_StatusTestResNorm.getConst<T>(aid));
            if (!keepold) tab_AztecOO_StatusTestResNorm.remove(&aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
        case CT_Ifpack_Preconditioner_ID:
            newid = tab.alias(tab_Ifpack_Preconditioner.getConst<T>(aid));
            if (!keepold) tab_Ifpack_Preconditioner.remove(&aid);
            break;
#endif /* HAVE_CTRILINOS_IFPACK */
        default:
            newid.table = CT_Invalid_ID;
            newid.index = -1;
            newid.is_const = aid.is_const;
            throw std::string("invalid table id or non-polymorphic class");
            break;
    }

    return newid;
}

CTrilinos_Universal_ID_t TableReposPoly<true>::alias(
    CTrilinos_Universal_ID_t aid, CTrilinos_Table_ID_t tab, bool keepold)
{
    CTrilinos_Universal_ID_t newid;

    switch (tab) {
        case CT_Epetra_Distributor_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_Distributor, aid, keepold)
                                  : do_alias(tab_Epetra_Distributor, aid, keepold));
            break;
        case CT_Epetra_SerialComm_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_SerialComm, aid, keepold)
                                  : do_alias(tab_Epetra_SerialComm, aid, keepold));
            break;
        case CT_Epetra_BLAS_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_BLAS, aid, keepold)
                                  : do_alias(tab_Epetra_BLAS, aid, keepold));
            break;
        case CT_Epetra_Comm_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_Comm, aid, keepold)
                                  : do_alias(tab_Epetra_Comm, aid, keepold));
            break;
        case CT_Epetra_Operator_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_Operator, aid, keepold)
                                  : do_alias(tab_Epetra_Operator, aid, keepold));
            break;
        case CT_Epetra_MultiVector_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_MultiVector, aid, keepold)
                                  : do_alias(tab_Epetra_MultiVector, aid, keepold));
            break;
        case CT_Epetra_OffsetIndex_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_OffsetIndex, aid, keepold)
                                  : do_alias(tab_Epetra_OffsetIndex, aid, keepold));
            break;
        case CT_Epetra_Object_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_Object, aid, keepold)
                                  : do_alias(tab_Epetra_Object, aid, keepold));
            break;
        case CT_Epetra_RowMatrix_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_RowMatrix, aid, keepold)
                                  : do_alias(tab_Epetra_RowMatrix, aid, keepold));
            break;
        case CT_Epetra_CompObject_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_CompObject, aid, keepold)
                                  : do_alias(tab_Epetra_CompObject, aid, keepold));
            break;
        case CT_Epetra_Directory_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_Directory, aid, keepold)
                                  : do_alias(tab_Epetra_Directory, aid, keepold));
            break;
        case CT_Epetra_Flops_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_Flops, aid, keepold)
                                  : do_alias(tab_Epetra_Flops, aid, keepold));
            break;
        case CT_Epetra_SrcDistObject_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_SrcDistObject, aid, keepold)
                                  : do_alias(tab_Epetra_SrcDistObject, aid, keepold));
            break;
#ifdef HAVE_MPI
        case CT_Epetra_MpiComm_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_MpiComm, aid, keepold)
                                  : do_alias(tab_Epetra_MpiComm, aid, keepold));
            break;
#endif /* HAVE_MPI */
        case CT_Epetra_CrsMatrix_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_CrsMatrix, aid, keepold)
                                  : do_alias(tab_Epetra_CrsMatrix, aid, keepold));
            break;
        case CT_Epetra_CrsGraph_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_CrsGraph, aid, keepold)
                                  : do_alias(tab_Epetra_CrsGraph, aid, keepold));
            break;
        case CT_Epetra_DistObject_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_DistObject, aid, keepold)
                                  : do_alias(tab_Epetra_DistObject, aid, keepold));
            break;
        case CT_Epetra_Vector_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_Vector, aid, keepold)
                                  : do_alias(tab_Epetra_Vector, aid, keepold));
            break;
        case CT_Epetra_Export_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_Export, aid, keepold)
                                  : do_alias(tab_Epetra_Export, aid, keepold));
            break;
        case CT_Epetra_Map_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_Map, aid, keepold)
                                  : do_alias(tab_Epetra_Map, aid, keepold));
            break;
        case CT_Epetra_BlockMap_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_BlockMap, aid, keepold)
                                  : do_alias(tab_Epetra_BlockMap, aid, keepold));
            break;
        case CT_Epetra_Import_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_Import, aid, keepold)
                                  : do_alias(tab_Epetra_Import, aid, keepold));
            break;
        case CT_Epetra_Time_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_Time, aid, keepold)
                                  : do_alias(tab_Epetra_Time, aid, keepold));
            break;
        case CT_Epetra_JadMatrix_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_JadMatrix, aid, keepold)
                                  : do_alias(tab_Epetra_JadMatrix, aid, keepold));
            break;
        case CT_Epetra_LinearProblem_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_LinearProblem, aid, keepold)
                                  : do_alias(tab_Epetra_LinearProblem, aid, keepold));
            break;
        case CT_Epetra_LAPACK_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_LAPACK, aid, keepold)
                                  : do_alias(tab_Epetra_LAPACK, aid, keepold));
            break;
        case CT_Teuchos_ParameterList_ID:
            newid = (aid.is_const ? do_alias_const(tab_Teuchos_ParameterList, aid, keepold)
                                  : do_alias(tab_Teuchos_ParameterList, aid, keepold));
            break;
#ifdef HAVE_CTRILINOS_AMESOS
        case CT_Amesos_BaseSolver_ID:
            newid = (aid.is_const ? do_alias_const(tab_Amesos_BaseSolver, aid, keepold)
                                  : do_alias(tab_Amesos_BaseSolver, aid, keepold));
            break;
#endif /* HAVE_CTRILINOS_AMESOS */
        case CT_Epetra_FECrsMatrix_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_FECrsMatrix, aid, keepold)
                                  : do_alias(tab_Epetra_FECrsMatrix, aid, keepold));
            break;
        case CT_Epetra_IntSerialDenseVector_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_IntSerialDenseVector, aid, keepold)
                                  : do_alias(tab_Epetra_IntSerialDenseVector, aid, keepold));
            break;
        case CT_Epetra_SerialDenseMatrix_ID:
            newid = (aid.is_const ? do_alias_const(tab_Epetra_SerialDenseMatrix, aid, keepold)
                                  : do_alias(tab_Epetra_SerialDenseMatrix, aid, keepold));
            break;
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTest_ID:
            newid = (aid.is_const ? do_alias_const(tab_AztecOO_StatusTest, aid, keepold)
                                  : do_alias(tab_AztecOO_StatusTest, aid, keepold));
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestCombo_ID:
            newid = (aid.is_const ? do_alias_const(tab_AztecOO_StatusTestCombo, aid, keepold)
                                  : do_alias(tab_AztecOO_StatusTestCombo, aid, keepold));
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestMaxIters_ID:
            newid = (aid.is_const ? do_alias_const(tab_AztecOO_StatusTestMaxIters, aid, keepold)
                                  : do_alias(tab_AztecOO_StatusTestMaxIters, aid, keepold));
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestResNorm_ID:
            newid = (aid.is_const ? do_alias_const(tab_AztecOO_StatusTestResNorm, aid, keepold)
                                  : do_alias(tab_AztecOO_StatusTestResNorm, aid, keepold));
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
        case CT_Ifpack_Preconditioner_ID:
            newid = (aid.is_const ? do_alias_const(tab_Ifpack_Preconditioner, aid, keepold)
                                  : do_alias(tab_Ifpack_Preconditioner, aid, keepold));
            break;
#endif /* HAVE_CTRILINOS_IFPACK */
        default:
            newid.table = CT_Invalid_ID;
            newid.index = -1;
            newid.is_const = aid.is_const;
            throw std::string("invalid table id or non-polymorphic class");
            break;
    }

    return newid;
}

void TableReposPoly<true>::remove(CTrilinos_Universal_ID_t * aid)
{
    switch (aid->table) {
        case CT_Epetra_Distributor_ID:
            tab_Epetra_Distributor.remove(aid);
            break;
        case CT_Epetra_SerialComm_ID:
            tab_Epetra_SerialComm.remove(aid);
            break;
        case CT_Epetra_BLAS_ID:
            tab_Epetra_BLAS.remove(aid);
            break;
        case CT_Epetra_Comm_ID:
            tab_Epetra_Comm.remove(aid);
            break;
        case CT_Epetra_Operator_ID:
            tab_Epetra_Operator.remove(aid);
            break;
        case CT_Epetra_MultiVector_ID:
            tab_Epetra_MultiVector.remove(aid);
            break;
        case CT_Epetra_OffsetIndex_ID:
            tab_Epetra_OffsetIndex.remove(aid);
            break;
        case CT_Epetra_Object_ID:
            tab_Epetra_Object.remove(aid);
            break;
        case CT_Epetra_RowMatrix_ID:
            tab_Epetra_RowMatrix.remove(aid);
            break;
        case CT_Epetra_CompObject_ID:
            tab_Epetra_CompObject.remove(aid);
            break;
        case CT_Epetra_Directory_ID:
            tab_Epetra_Directory.remove(aid);
            break;
        case CT_Epetra_Flops_ID:
            tab_Epetra_Flops.remove(aid);
            break;
        case CT_Epetra_SrcDistObject_ID:
            tab_Epetra_SrcDistObject.remove(aid);
            break;
#ifdef HAVE_MPI
        case CT_Epetra_MpiComm_ID:
            tab_Epetra_MpiComm.remove(aid);
            break;
#endif /* HAVE_MPI */
        case CT_Epetra_CrsMatrix_ID:
            tab_Epetra_CrsMatrix.remove(aid);
            break;
        case CT_Epetra_CrsGraph_ID:
            tab_Epetra_CrsGraph.remove(aid);
            break;
        case CT_Epetra_DistObject_ID:
            tab_Epetra_DistObject.remove(aid);
            break;
        case CT_Epetra_Vector_ID:
            tab_Epetra_Vector.remove(aid);
            break;
        case CT_Epetra_Export_ID:
            tab_Epetra_Export.remove(aid);
            break;
        case CT_Epetra_Map_ID:
            tab_Epetra_Map.remove(aid);
            break;
        case CT_Epetra_BlockMap_ID:
            tab_Epetra_BlockMap.remove(aid);
            break;
        case CT_Epetra_Import_ID:
            tab_Epetra_Import.remove(aid);
            break;
        case CT_Epetra_Time_ID:
            tab_Epetra_Time.remove(aid);
            break;
        case CT_Epetra_JadMatrix_ID:
            tab_Epetra_JadMatrix.remove(aid);
            break;
        case CT_Epetra_LinearProblem_ID:
            tab_Epetra_LinearProblem.remove(aid);
            break;
        case CT_Epetra_LAPACK_ID:
            tab_Epetra_LAPACK.remove(aid);
            break;
        case CT_Teuchos_ParameterList_ID:
            tab_Teuchos_ParameterList.remove(aid);
            break;
#ifdef HAVE_CTRILINOS_AMESOS
        case CT_Amesos_BaseSolver_ID:
            tab_Amesos_BaseSolver.remove(aid);
            break;
#endif /* HAVE_CTRILINOS_AMESOS */
        case CT_Epetra_FECrsMatrix_ID:
            tab_Epetra_FECrsMatrix.remove(aid);
            break;
        case CT_Epetra_IntSerialDenseVector_ID:
            tab_Epetra_IntSerialDenseVector.remove(aid);
            break;
        case CT_Epetra_SerialDenseMatrix_ID:
            tab_Epetra_SerialDenseMatrix.remove(aid);
            break;
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTest_ID:
            tab_AztecOO_StatusTest.remove(aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestCombo_ID:
            tab_AztecOO_StatusTestCombo.remove(aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestMaxIters_ID:
            tab_AztecOO_StatusTestMaxIters.remove(aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_StatusTestResNorm_ID:
            tab_AztecOO_StatusTestResNorm.remove(aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
        case CT_Ifpack_Preconditioner_ID:
            tab_Ifpack_Preconditioner.remove(aid);
            break;
#endif /* HAVE_CTRILINOS_IFPACK */
        default:
            throw std::string("invalid table id");
            break;
    }
}

void TableReposPoly<true>::purge()
{
    tab_Epetra_Distributor.purge();
    tab_Epetra_SerialComm.purge();
    tab_Epetra_BLAS.purge();
    tab_Epetra_Comm.purge();
    tab_Epetra_Operator.purge();
    tab_Epetra_MultiVector.purge();
    tab_Epetra_OffsetIndex.purge();
    tab_Epetra_Object.purge();
    tab_Epetra_RowMatrix.purge();
    tab_Epetra_CompObject.purge();
    tab_Epetra_Directory.purge();
    tab_Epetra_Flops.purge();
    tab_Epetra_SrcDistObject.purge();
#ifdef HAVE_MPI
    tab_Epetra_MpiComm.purge();
#endif /* HAVE_MPI */
    tab_Epetra_CrsMatrix.purge();
    tab_Epetra_CrsGraph.purge();
    tab_Epetra_DistObject.purge();
    tab_Epetra_Vector.purge();
    tab_Epetra_Export.purge();
    tab_Epetra_Map.purge();
    tab_Epetra_BlockMap.purge();
    tab_Epetra_Import.purge();
    tab_Epetra_Time.purge();
    tab_Epetra_JadMatrix.purge();
    tab_Epetra_LinearProblem.purge();
    tab_Epetra_LAPACK.purge();
    tab_Teuchos_ParameterList.purge();
#ifdef HAVE_CTRILINOS_AMESOS
    tab_Amesos_BaseSolver.purge();
#endif /* HAVE_CTRILINOS_AMESOS */
    tab_Epetra_FECrsMatrix.purge();
    tab_Epetra_IntSerialDenseVector.purge();
    tab_Epetra_SerialDenseMatrix.purge();
#ifdef HAVE_CTRILINOS_AZTECOO
    tab_AztecOO_StatusTest.purge();
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
    tab_AztecOO_StatusTestCombo.purge();
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
    tab_AztecOO_StatusTestMaxIters.purge();
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_AZTECOO
    tab_AztecOO_StatusTestResNorm.purge();
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
    tab_Ifpack_Preconditioner.purge();
#endif /* HAVE_CTRILINOS_IFPACK */
}

template <>
class TableReposPoly<false>
{
  public:

    /*! constructor */
    TableReposPoly();

    /*! destructor */
    ~TableReposPoly();

    /*! retrieve the object */
    template <class T>
    const Teuchos::RCP<T> get(CTrilinos_Universal_ID_t id);

    /*! retrieve the object */
    template <class T>
    const Teuchos::RCP<const T> getConst(CTrilinos_Universal_ID_t id);

    /*! store a non-const RCP to object of type T */
    template <class T>
    CTrilinos_Universal_ID_t store(T* pobj, bool owned = true)
    { return getTable<T>().store(pobj, owned); }

    /*! store a const RCP to object of type T */
    template <class T>
    CTrilinos_Universal_ID_t store(const T* pobj, bool owned = false)
    { return getTable<T>().store(pobj, owned); }

    /*! remove an object from the table and invalidate the id struct */
    void remove(CTrilinos_Universal_ID_t * id);

    /*! dump the tables' content but keep their properties */
    void purge();

  private:
    /*! get the Ctrilinos::Table for the given type */
    template <class T>
    Table<T> & getTable();

    Table<Teuchos::CommandLineProcessor> tab_Teuchos_CommandLineProcessor;
    Table<Teuchos::ParameterEntry> tab_Teuchos_ParameterEntry;
    Table<Teuchos::any> tab_Teuchos_any;
#ifdef HAVE_CTRILINOS_AMESOS
    Table<Amesos> tab_Amesos;
#endif /* HAVE_CTRILINOS_AMESOS */
#ifdef HAVE_CTRILINOS_AZTECOO
    Table<AztecOO> tab_AztecOO;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
    Table<Ifpack> tab_Ifpack;
#endif /* HAVE_CTRILINOS_IFPACK */

    bool call_me_lazy;  /* I was too lazy to deal with the commas in the init list, so... */
};

TableReposPoly<false>::TableReposPoly() :
    tab_Teuchos_CommandLineProcessor(CT_Teuchos_CommandLineProcessor_ID),
    tab_Teuchos_ParameterEntry(CT_Teuchos_ParameterEntry_ID),
    tab_Teuchos_any(CT_Teuchos_any_ID),
#ifdef HAVE_CTRILINOS_AMESOS
    tab_Amesos(CT_Amesos_ID),
#endif /* HAVE_CTRILINOS_AMESOS */
#ifdef HAVE_CTRILINOS_AZTECOO
    tab_AztecOO(CT_AztecOO_ID),
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
    tab_Ifpack(CT_Ifpack_ID),
#endif /* HAVE_CTRILINOS_IFPACK */
    call_me_lazy(true)
{
}

TableReposPoly<false>::~TableReposPoly()
{
}

template <class T>
Table<T> & TableReposPoly<false>::getTable()
{
    Teuchos::RCP<T> dummy = Teuchos::null;
    /* I know this is bad, but it's for catching COMPILE-TIME errors! */
    return dummy->No_Function_For_This_Table_Type;
}

template <>
Table<Teuchos::CommandLineProcessor> & TableReposPoly<false>::getTable<Teuchos::CommandLineProcessor>()
{ return tab_Teuchos_CommandLineProcessor; }

template <>
Table<Teuchos::ParameterEntry> & TableReposPoly<false>::getTable<Teuchos::ParameterEntry>()
{ return tab_Teuchos_ParameterEntry; }

template <>
Table<Teuchos::any> & TableReposPoly<false>::getTable<Teuchos::any>()
{ return tab_Teuchos_any; }

#ifdef HAVE_CTRILINOS_AMESOS
template <>
Table<Amesos> & TableReposPoly<false>::getTable<Amesos>()
{ return tab_Amesos; }

#endif /* HAVE_CTRILINOS_AMESOS */
#ifdef HAVE_CTRILINOS_AZTECOO
template <>
Table<AztecOO> & TableReposPoly<false>::getTable<AztecOO>()
{ return tab_AztecOO; }

#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
template <>
Table<Ifpack> & TableReposPoly<false>::getTable<Ifpack>()
{ return tab_Ifpack; }

#endif /* HAVE_CTRILINOS_IFPACK */
template <class T>
const Teuchos::RCP<T> TableReposPoly<false>::get(CTrilinos_Universal_ID_t aid)
{
    /* Shortcut if stored in the most obvious table */
    Table<T> &tab_only = getTable<T>();
    return tab_only.get<T>(aid);
}

template <class T>
const Teuchos::RCP<const T> TableReposPoly<false>::getConst(CTrilinos_Universal_ID_t aid)
{
    /* Shortcut if stored in the most obvious table */
    Table<T> &tab_only = getTable<T>();
    return tab_only.getConst<T>(aid);
}

void TableReposPoly<false>::remove(CTrilinos_Universal_ID_t * aid)
{
    switch (aid->table) {
        case CT_Teuchos_CommandLineProcessor_ID:
            tab_Teuchos_CommandLineProcessor.remove(aid);
            break;
        case CT_Teuchos_ParameterEntry_ID:
            tab_Teuchos_ParameterEntry.remove(aid);
            break;
        case CT_Teuchos_any_ID:
            tab_Teuchos_any.remove(aid);
            break;
#ifdef HAVE_CTRILINOS_AMESOS
        case CT_Amesos_ID:
            tab_Amesos.remove(aid);
            break;
#endif /* HAVE_CTRILINOS_AMESOS */
#ifdef HAVE_CTRILINOS_AZTECOO
        case CT_AztecOO_ID:
            tab_AztecOO.remove(aid);
            break;
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
        case CT_Ifpack_ID:
            tab_Ifpack.remove(aid);
            break;
#endif /* HAVE_CTRILINOS_IFPACK */
        default:
            throw std::string("invalid table id");
            break;
    }
}

void TableReposPoly<false>::purge()
{
    tab_Teuchos_CommandLineProcessor.purge();
    tab_Teuchos_ParameterEntry.purge();
    tab_Teuchos_any.purge();
#ifdef HAVE_CTRILINOS_AMESOS
    tab_Amesos.purge();
#endif /* HAVE_CTRILINOS_AMESOS */
#ifdef HAVE_CTRILINOS_AZTECOO
    tab_AztecOO.purge();
#endif /* HAVE_CTRILINOS_AZTECOO */
#ifdef HAVE_CTRILINOS_IFPACK
    tab_Ifpack.purge();
#endif /* HAVE_CTRILINOS_IFPACK */
}


} // namespace CTrilinos


#endif
