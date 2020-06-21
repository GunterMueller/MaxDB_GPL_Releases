/*!***************************************************************************

  module      : Catalog_AuthorizationDescriptor.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog Manager
  description : descriptor for the catalog table AUTHORIZATION

  last changed: 2000-12-06  19:04
  see also    : example.html ...
  first created:2000-03-09  18:21

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end




*****************************************************************************/


#include "SQLManager/Catalog/Catalog_AuthorizationDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_Cache.hpp"
#include "SQLManager/Catalog/Catalog_TableObject.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

#include "hsp40.h"

inline void Catalog_AuthorizationDescriptor::GetId (const tgg00_Rec& Row, Catalog_ObjectId& Id) const
{
    const SAPDB_Byte* pValue;
    int               valueLength;
    LocateColumnValue (SQLMan_Identifier("AUTHORIZATIONID", /*ASCIISource*/true), Row, pValue, valueLength);
    if (pValue)
    {
         SAPDB_MemCopyNoCheck (&Id[0], pValue + 1, sizeof(Id));
    }
}

//-----------------------------------------------------------------------------

inline void Catalog_AuthorizationDescriptor::GetName (const tgg00_Rec& Row, SQLMan_Identifier& Name) const
{
    const SAPDB_Byte* pValue;
    int               valueLength;
    LocateColumnValue (SQLMan_Identifier("NAME", /*ASCIISource*/true), Row, pValue, valueLength);
    if (pValue && valueLength <= sizeof(SQLMan_Identifier) + 1)
    {
        SAPDB_MemCopyNoCheck (&Name[0], pValue + 1, valueLength - 1); 
    }
    LocateColumnValue (SQLMan_Identifier("NAMELENGTH", /*ASCIISource*/true), Row, pValue, valueLength);
    if (pValue)
    {
        SAPDB_Int4     namelength;
        tsp00_NumError res;
        s40glint (const_cast<SAPDB_Byte*>(pValue), 2, (valueLength - 2) * 2, namelength, res);
        if (num_ok == res)
        {
            while (namelength < sizeof(Name))
            {
                Name[namelength] = 0;
                ++namelength;
            }
        }
    }
}

//-----------------------------------------------------------------------------

Catalog_AuthorizationDescriptor::Catalog_AuthorizationDescriptor(SQLMan_Context& Context)
: Catalog_CatalogTableDescriptor(Context, SQLMan_Identifier("AUTHORIZATION", /*ASCIISource*/true))
{
    if (m_pTable)
    {
        bool exists;
        Catalog_TableObject::Catalog_Key key(m_pTable->bsurrogate);
        this->Release();
        const Catalog_Object* pObj;
        const bool cNoWait = true;
        Catalog_Cache::Access (Context, key, !cNoWait, pObj, exists);
        m_pTable = reinterpret_cast<Catalog_TableObject*>(const_cast<Catalog_Object*>(pObj)); 
    }
}

//-----------------------------------------------------------------------------

tgg00_BasisError Catalog_AuthorizationDescriptor::DeleteTrigger (RTE_TaskId TaskId, const tgg00_Rec& OldRecord)
{
    Catalog_ObjectId id;
    this->GetId (OldRecord, id);
    SQLMan_Identifier name;
    this->GetName (OldRecord, name);
    Catalog_Cache::InvalidateAuthorization (TaskId, &id, &name);
    return e_ok;
}

//-----------------------------------------------------------------------------

void Catalog_AuthorizationDescriptor::InsertTrigger (RTE_TaskId TaskId, const tgg00_Rec& NewRecord)
{
    SQLMan_Identifier name;
    this->GetName (NewRecord, name);
    Catalog_Cache::InvalidateAuthorization (TaskId, 0, &name);
}

//-----------------------------------------------------------------------------

void Catalog_AuthorizationDescriptor::UpdateTrigger (RTE_TaskId TaskId, const tgg00_Rec& OldRecord, const tgg00_Rec& NewRecord)
{
    Catalog_ObjectId id;
    this->GetId (OldRecord, id);
    SQLMan_Identifier name;
    this->GetName (OldRecord, name);
    Catalog_Cache::InvalidateAuthorization (TaskId, &id, &name);
    SQLMan_Identifier newName;
    this->GetName (NewRecord, newName);
    if (name != newName)
    {
        Catalog_Cache::InvalidateAuthorization (TaskId, 0, &newName);
    }
}