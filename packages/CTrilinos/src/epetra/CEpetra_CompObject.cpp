
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


#include "CTrilinos_config.h"

#include "CEpetra_Flops_Cpp.hpp"
#include "CEpetra_CompObject_Cpp.hpp"
#include "CEpetra_CompObject.h"
#include "Epetra_CompObject.h"
#include "Teuchos_RCP.hpp"
#include "CTrilinos_enums.h"
#include "CTrilinos_utils_templ.hpp"
#include "CTrilinos_Table.hpp"


namespace {


using Teuchos::RCP;
using CTrilinos::Table;


/* table to hold objects of type Epetra_CompObject */
Table<Epetra_CompObject>& tableOfCompObjects()
{
    static Table<Epetra_CompObject>
        loc_tableOfCompObjects(CT_Epetra_CompObject_ID, "CT_Epetra_CompObject_ID", false);
    return loc_tableOfCompObjects;
}

/* table to hold objects of type const Epetra_CompObject */
Table<const Epetra_CompObject>& tableOfConstCompObjects()
{
    static Table<const Epetra_CompObject>
        loc_tableOfConstCompObjects(CT_Epetra_CompObject_ID, "CT_Epetra_CompObject_ID", true);
    return loc_tableOfConstCompObjects;
}


} // namespace


//
// Definitions from CEpetra_CompObject.h
//


extern "C" {


CT_Epetra_CompObject_ID_t Epetra_CompObject_Cast ( 
  CTrilinos_Object_ID_t id )
{
    CTrilinos_Object_ID_t newid;
    if (id.is_const) {
        newid = CTrilinos::cast(tableOfConstCompObjects(), id);
    } else {
        newid = CTrilinos::cast(tableOfCompObjects(), id);
    }
    return CTrilinos::concreteType<CT_Epetra_CompObject_ID_t>(newid);
}

CTrilinos_Object_ID_t Epetra_CompObject_Abstract ( 
  CT_Epetra_CompObject_ID_t id )
{
    return CTrilinos::abstractType<CT_Epetra_CompObject_ID_t>(id);
}

CT_Epetra_CompObject_ID_t Epetra_CompObject_Create (  )
{
    return CTrilinos::concreteType<CT_Epetra_CompObject_ID_t>(
        tableOfCompObjects().store(new Epetra_CompObject()));
}

CT_Epetra_CompObject_ID_t Epetra_CompObject_Duplicate ( 
  CT_Epetra_CompObject_ID_t SourceID )
{
    return CTrilinos::concreteType<CT_Epetra_CompObject_ID_t>(
        tableOfCompObjects().store(new Epetra_CompObject(
        *CEpetra::getConstCompObject(SourceID))));
}

void Epetra_CompObject_Destroy ( CT_Epetra_CompObject_ID_t * selfID )
{
    CTrilinos_Object_ID_t aid
        = CTrilinos::abstractType<CT_Epetra_CompObject_ID_t>(*selfID);
    if (selfID->is_const) {
        tableOfConstCompObjects().remove(&aid);
    } else {
        tableOfCompObjects().remove(&aid);
    }
    *selfID = CTrilinos::concreteType<CT_Epetra_CompObject_ID_t>(aid);
}

void Epetra_CompObject_SetFlopCounter ( 
  CT_Epetra_CompObject_ID_t selfID, 
  CT_Epetra_Flops_ID_t FlopCounter_inID )
{
    CEpetra::getCompObject(selfID)->SetFlopCounter(
        *CEpetra::getConstFlops(FlopCounter_inID));
}

void Epetra_CompObject_SetFlopCounter_Matching ( 
  CT_Epetra_CompObject_ID_t selfID, 
  CT_Epetra_CompObject_ID_t CompObjectID )
{
    CEpetra::getCompObject(selfID)->SetFlopCounter(
        *CEpetra::getConstCompObject(CompObjectID));
}

void Epetra_CompObject_UnsetFlopCounter ( 
  CT_Epetra_CompObject_ID_t selfID )
{
    CEpetra::getCompObject(selfID)->UnsetFlopCounter();
}

CT_Epetra_Flops_ID_t Epetra_CompObject_GetFlopCounter ( 
  CT_Epetra_CompObject_ID_t selfID )
{
    return CEpetra::storeFlops(CEpetra::getConstCompObject(
        selfID)->GetFlopCounter());
}

void Epetra_CompObject_ResetFlops ( 
  CT_Epetra_CompObject_ID_t selfID )
{
    CEpetra::getConstCompObject(selfID)->ResetFlops();
}

double Epetra_CompObject_Flops ( CT_Epetra_CompObject_ID_t selfID )
{
    return CEpetra::getConstCompObject(selfID)->Flops();
}

void Epetra_CompObject_UpdateFlops_Int ( 
  CT_Epetra_CompObject_ID_t selfID, int Flops_in )
{
    CEpetra::getConstCompObject(selfID)->UpdateFlops(Flops_in);
}

void Epetra_CompObject_UpdateFlops_Long ( 
  CT_Epetra_CompObject_ID_t selfID, long int Flops_in )
{
    CEpetra::getConstCompObject(selfID)->UpdateFlops(Flops_in);
}

void Epetra_CompObject_UpdateFlops_Double ( 
  CT_Epetra_CompObject_ID_t selfID, double Flops_in )
{
    CEpetra::getConstCompObject(selfID)->UpdateFlops(Flops_in);
}

void Epetra_CompObject_UpdateFlops_Float ( 
  CT_Epetra_CompObject_ID_t selfID, float Flops_in )
{
    CEpetra::getConstCompObject(selfID)->UpdateFlops(Flops_in);
}

void Epetra_CompObject_Assign ( 
  CT_Epetra_CompObject_ID_t selfID, CT_Epetra_CompObject_ID_t srcID )
{
    Epetra_CompObject& self = *( CEpetra::getCompObject(selfID) );

    self = *CEpetra::getConstCompObject(srcID);
}


} // extern "C"


//
// Definitions from CEpetra_CompObject_Cpp.hpp
//


/* get Epetra_CompObject from non-const table using CT_Epetra_CompObject_ID */
const Teuchos::RCP<Epetra_CompObject>
CEpetra::getCompObject( CT_Epetra_CompObject_ID_t id )
{
    CTrilinos_Object_ID_t aid
            = CTrilinos::abstractType<CT_Epetra_CompObject_ID_t>(id);
    return tableOfCompObjects().get(aid);
}

/* get Epetra_CompObject from non-const table using CTrilinos_Object_ID_t */
const Teuchos::RCP<Epetra_CompObject>
CEpetra::getCompObject( CTrilinos_Object_ID_t id )
{
    return tableOfCompObjects().get(id);
}

/* get const Epetra_CompObject from either the const or non-const table
 * using CT_Epetra_CompObject_ID */
const Teuchos::RCP<const Epetra_CompObject>
CEpetra::getConstCompObject( CT_Epetra_CompObject_ID_t id )
{
    CTrilinos_Object_ID_t aid
            = CTrilinos::abstractType<CT_Epetra_CompObject_ID_t>(id);
    if (id.is_const) {
        return tableOfConstCompObjects().get(aid);
    } else {
        return tableOfCompObjects().get(aid);
    }
}

/* get const Epetra_CompObject from either the const or non-const table
 * using CTrilinos_Object_ID_t */
const Teuchos::RCP<const Epetra_CompObject>
CEpetra::getConstCompObject( CTrilinos_Object_ID_t id )
{
    if (id.is_const) {
        return tableOfConstCompObjects().get(id);
    } else {
        return tableOfCompObjects().get(id);
    }
}

/* store Epetra_CompObject in non-const table */
CT_Epetra_CompObject_ID_t
CEpetra::storeCompObject( Epetra_CompObject *pobj )
{
    return CTrilinos::concreteType<CT_Epetra_CompObject_ID_t>(
            tableOfCompObjects().storeShared(pobj));
}

/* store const Epetra_CompObject in const table */
CT_Epetra_CompObject_ID_t
CEpetra::storeConstCompObject( const Epetra_CompObject *pobj )
{
    return CTrilinos::concreteType<CT_Epetra_CompObject_ID_t>(
            tableOfConstCompObjects().storeShared(pobj));
}

/* dump contents of Epetra_CompObject and const Epetra_CompObject tables */
void
CEpetra::purgeCompObjectTables(  )
{
    tableOfCompObjects().purge();
    tableOfConstCompObjects().purge();
}



