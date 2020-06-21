/*!***************************************************************************

  module      : Catalog_ContainerDescriptor.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog Manager
  description : descriptor for the catalog table SYSCATALOG.CLASSCONTAINERS

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

#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/Catalog/Catalog_ContainerDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_Cache.hpp"
#include "SQLManager/Catalog/Catalog_ContainerObject.hpp"
#include "SQLManager/Catalog/Catalog_TableObject.hpp"
#include "liveCache/LVC_ContainerManager.hpp"
#include "Interfaces/Runtime/Util/IFRUtil_VDNNumber.h"
#include "SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "hsp40.h"

inline void Catalog_ContainerDescriptor::GetSchemaId (const tgg00_Rec& Row, Catalog_ObjectId& Id) const
{
    const SAPDB_Byte* pValue;
    int               valueLength;
    LocateColumnValue (SQLMan_Identifier("SCHEMAID", /*ASCIISource*/true), Row, pValue, valueLength);
    if (pValue)
    {
        SAPDB_MemCopyNoCheck (&Id[0], pValue + 1, sizeof(Id));
    }
}

//-----------------------------------------------------------------------------

inline SAPDB_UInt4  Catalog_ContainerDescriptor::GetContainerNo (const tgg00_Rec& Row) const
{
    const SAPDB_Byte* pValue;
    int               valueLength;
    LocateColumnValue (SQLMan_Identifier("CONTAINERNO", /*ASCIISource*/true), Row, pValue, valueLength);
    if (pValue)
    {
        SAPDB_Int4     containerNo;
        tsp00_NumError res;
        s40glint (const_cast<SAPDB_Byte*>(pValue), 2, (valueLength - 2) * 2, containerNo, res);
        return containerNo;
    }
    return 0;
}

//-----------------------------------------------------------------------------

inline void  Catalog_ContainerDescriptor::GetGuid (const tgg00_Rec& Row, Catalog_GUID& Guid) const
{
    const SAPDB_Byte* pValue;
    int               valueLength;
    LocateColumnValue (SQLMan_Identifier("GUID", /*ASCIISource*/true), Row, pValue, valueLength);
    if (pValue)
    {
        SAPDB_MemCopyNoCheck (&Guid[0], pValue + 1, sizeof(Guid));
        if (valueLength < sizeof(Guid))
        {
            SAPDB_MemFillNoCheck (&Guid[valueLength], 0, sizeof(Guid) - valueLength);
        }
    }
}

//-----------------------------------------------------------------------------

inline bool Catalog_ContainerDescriptor::GetIsVarContainer (const tgg00_Rec& Row)
{
    const SAPDB_Byte* pValue;
    int               valueLength;
    LocateColumnValue (SQLMan_Identifier("ISVARCONTAINER", /*ASCIISource*/true), Row, pValue, valueLength);
    if (pValue)
    {
        ++pValue;
        return *pValue != 0;
    }
    return false;
}

//-----------------------------------------------------------------------------

inline void Catalog_ContainerDescriptor::GetFileNo (const tgg00_Rec& Row, FileDir_FileNo& FileNo)
{
    const SAPDB_Byte* pValue;
    int               valueLength;
    LocateColumnValue (SQLMan_Identifier("FILENO", /*ASCIISource*/true), Row, pValue, valueLength);
    if (pValue)
    {
        SAPDB_MemCopyNoCheck (&FileNo, pValue + 1, sizeof(FileNo));
    }
}

//-----------------------------------------------------------------------------

Catalog_ContainerDescriptor::Catalog_ContainerDescriptor(SQLMan_Context& Context)
: Catalog_CatalogTableDescriptor(Context, SQLMan_Identifier("CLASSCONTAINERS", /*ASCIISource*/true))
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

void Catalog_ContainerDescriptor::InvalidateCatalogCache (RTE_TaskId TaskId, const tgg00_Rec& Record)
{
    Catalog_ObjectId schemaId;
    GetSchemaId (Record, schemaId);
    SAPDB_UInt4 containerNo = GetContainerNo (Record);
    Catalog_GUID guid;
    GetGuid (Record, guid);
    Catalog_ContainerObject::Catalog_Key key(schemaId, containerNo, guid);
    Catalog_Cache::Invalidate (TaskId, key);
}

//-----------------------------------------------------------------------------

tgg00_BasisError Catalog_ContainerDescriptor::DeleteTrigger (RTE_TaskId TaskId, const tgg00_Rec& OldRecord)
{
    // drop the container file first
    bool isVarContainer = GetIsVarContainer (OldRecord);
    FileDir_FileNo fileNo;
    GetFileNo (OldRecord, fileNo);
    SQLMan_Context* pContext = SQLMan_Context::GetContext();
    LVC_ContainerManager::DropContainerCallback(*pContext, fileNo, isVarContainer);
    // invalidate shared catalog cache entry
    this->InvalidateCatalogCache(TaskId, OldRecord);
    return pContext->TransContext().trError_gg00;
}

//-----------------------------------------------------------------------------

void Catalog_ContainerDescriptor::InsertTrigger (RTE_TaskId TaskId, const tgg00_Rec& NewRecord)
{
     this->InvalidateCatalogCache(TaskId, NewRecord);
}

//-----------------------------------------------------------------------------

void Catalog_ContainerDescriptor::UpdateTrigger (RTE_TaskId TaskId, const tgg00_Rec& OldRecord, const tgg00_Rec& NewRecord)
{
    // nothing to do
}