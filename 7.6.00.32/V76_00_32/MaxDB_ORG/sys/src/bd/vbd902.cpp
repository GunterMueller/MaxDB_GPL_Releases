/****************************************************************************

  module      : vbd902.cpp

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: BD :       Operations for processing object files
  description :


  version     : 7.6.0.0
  last changed: 2005-12-28
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1998-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hbd902.h"
#include "hgg01_3.h"
#include "Container/Container_Hash.hpp"
#include "liveCache/LVC_ContainerManager.hpp"
#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SQLManager/Catalog/Catalog_IAuthorization.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_Oms.hpp"
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"           

/*---------------------------------------------------------------------------*/
class CSchemaLockHashEntry
{
public:
    CSchemaLockHashEntry()
        : m_Count(0)
    {};
    CSchemaLockHashEntry(Catalog_ObjectId obj)
        : m_ObjectId(obj)
        , m_Count(0)
    {};
    int& Count() {
        return m_Count;
    }
    Catalog_ObjectId GetSchemaObjectId() {
        return m_ObjectId;
    }
private:
    Catalog_ObjectId    m_ObjectId;
    int                 m_Count;
};

/*---------------------------------------------------------------------------*/
typedef Container_Hash<SAPDB_UInt2, CSchemaLockHashEntry>  CSchemaLockHash;

/*---------------------------------------------------------------------------*/
class CSchemaShareLocks
{
public:
    CSchemaShareLocks(SAPDBMem_IRawAllocator &RawAllocator)
        : m_Hash(RawAllocator)
        , m_MaxKey(0)
    {};

    CSchemaLockHash::Iterator Find(SAPDB_UInt2 key) {
        return m_Hash.Find(key);
    }

    CSchemaLockHash::Iterator Insert(Catalog_ObjectId obj) {
        CSchemaLockHashEntry  entry(obj);
        m_MaxKey++;
        return m_Hash.Insert(m_MaxKey, entry);
    }

    CSchemaLockHash::Iterator Begin() {
        return m_Hash.Begin();
    }

    CSchemaLockHash::Iterator End() {
        return m_Hash.End();
    }
    SAPDB_UInt2 GetKey() {
        return m_MaxKey;
    }
private:
    CSchemaLockHash     m_Hash;
    SAPDB_UInt2         m_MaxKey;
};

/*---------------------------------------------------------------------------*/
extern SAPDBTrace_Topic ObjContainer_Trace;

static 
void WriteErrorToVtrace(const char str[], tgg00_BasisError err)
{
    if ( ObjContainer_Trace.TracesLevel(1) 
        || (g01vtrace.vtrBdObject_gg00) 
        || (g01vtrace.vtrOmsUpd_gg00)
        || (g01vtrace.vtrOmsNew_gg00)
        || (g01vtrace.vtrOmsFree_gg00)
        || (g01vtrace.vtrOmsGet_gg00))
    {
        Kernel_VTrace() << str << " error:" << err;
    }
}

/*---------------------------------------------------------------------------*/

externCpp void
bd902LockShareSchema(tgg00_TransContext   &Trans,
                     FileDir_Oms          &fd_smart,
                     const FileDir_FileNo &FileNo )
{
    if (!Kernel_IAdminInfo::Instance().KernelStateIsOnline())
        return;

    if (NULL == Trans.trSchemaShrLockHash_gg00)
    {
        SAPDBMem_IRawAllocator &RawAllocator = *(REINTERPRET_CAST(SAPDBMem_IRawAllocator*, Trans.trAllocator_gg00));
        Trans.trSchemaShrLockHash_gg00 = new (RawAllocator) CSchemaShareLocks(RawAllocator);
    }
    if (NULL == Trans.trSchemaShrLockHash_gg00)
    {
        Trans.trError_gg00 = e_no_more_memory;
        WriteErrorToVtrace("bd902LockShareSchema(1)", Trans.trError_gg00);
        return;
    }
    CSchemaShareLocks& SchemaLockHash = *reinterpret_cast<CSchemaShareLocks*>(Trans.trSchemaShrLockHash_gg00);
    SQLMan_Context &ctx = *SQLMan_Context::GetContext();

    CSchemaLockHash::Iterator iter = SchemaLockHash.Find(fd_smart.GetSchemaHashkey());
    CSchemaLockHash::Iterator end = SchemaLockHash.End();
    if (iter == end)
    {

        Catalog_GUID     Guid;
        SAPDB_UInt4      SchemaId;
        SAPDB_UInt4      ContainerNo;
        Catalog_ObjectId SchemaObjectId;
        LVC_ContainerManager::GetContainerInfo(ctx, FileNo, Guid, SchemaId, ContainerNo, SchemaObjectId);
        if ( e_ok != Trans.trError_gg00)
        {
            WriteErrorToVtrace("bd902LockShareSchema(3)", Trans.trError_gg00);
            return;
        }

        iter = SchemaLockHash.Begin();
        while (iter != end)
        {
            if (iter->value.GetSchemaObjectId() == SchemaObjectId)
                break;
            iter++;
        }
        if (iter == end)
           iter = SchemaLockHash.Insert (SchemaObjectId);
        if (iter == SchemaLockHash.End())
        {
            Trans.trError_gg00 = e_no_more_memory;
            WriteErrorToVtrace("bd902LockShareSchema(4)", Trans.trError_gg00);
            return;
        }
        fd_smart.SetSchemaHashkey(SchemaLockHash.GetKey());
    }

    if (0 == iter->value.Count())
    {
        Catalog_IAuthorization::GetInstance().Lock (ctx, iter->value.GetSchemaObjectId(), false);
        if ( e_ok != Trans.trError_gg00)
        {
            WriteErrorToVtrace("bd902LockShareSchema(5)", Trans.trError_gg00);
            return;
        }
    }
    iter->value.Count()++;
};

/*---------------------------------------------------------------------------*/

externCpp void
bd902RemoveLockShareSchemaFlag(tgg00_TransContext &Trans)
{
    if (0 == Trans.trSchemaShrLockHash_gg00)
        return;

    CSchemaShareLocks& SchemaLockHash =
        *reinterpret_cast<CSchemaShareLocks*>(Trans.trSchemaShrLockHash_gg00);
    CSchemaLockHash::Iterator iter = SchemaLockHash.Begin(), end = SchemaLockHash.End();
    while (iter != end)
    {
        iter->value.Count() = 0;
        iter++;
  }
};

/*---------------------------------------------------------------------------*/

externCpp void 
bd902FreeLockShareSchemaCache(tgg00_TransContext &Trans)
{
    if (Trans.trSchemaShrLockHash_gg00)
    {
        SAPDBMem_IRawAllocator &RawAllocator = *(REINTERPRET_CAST(SAPDBMem_IRawAllocator*, Trans.trAllocator_gg00));
        CSchemaShareLocks *SchemaLockHashPtr =
            reinterpret_cast<CSchemaShareLocks*>(Trans.trSchemaShrLockHash_gg00);
        destroy(SchemaLockHashPtr,RawAllocator);
        Trans.trSchemaShrLockHash_gg00         = 0;
    };
};
