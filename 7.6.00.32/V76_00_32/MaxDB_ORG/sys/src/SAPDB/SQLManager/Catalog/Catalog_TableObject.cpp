/*!***************************************************************************

  module      : Catalog_TableObject.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog Manager
  description : Catalog Table Object

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

#include "SAPDB/SQLManager/Catalog/Catalog_TableObject.hpp"
#include "SAPDB/SQLManager/SQLMan_Context.hpp"
#include "SAPDB/SQLManager/SQLMan_Tracing.hpp"
#include "SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h"

#if defined(KERNEL80)
#include "SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp" /* nocheck */
#include "SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp" /* nocheck */
#endif

RTESync_Spinlock Catalog_TableObject::m_refCountSynchronizer;

//----------------------------------------------------------------------------------------------------------

Catalog_TableObject::Catalog_Key::Catalog_Key(const Catalog_ObjectId& TableId)
: Catalog_Object::Catalog_Key()
{
    SetSurrogate(TableId);
    SetEntryType(TYPE_TABLE);
    slinkage()[1] = 1;
}

//----------------------------------------------------------------------------------------------------------

Catalog_TableObject::NextBuffer::NextBuffer()
: m_next(0)
{
}

//----------------------------------------------------------------------------------------------------------

int Catalog_TableObject::NextBuffer::BufferOffset()
{
    return CATALOG_OFFSET(Catalog_TableObject::NextBuffer, m_colBuffer);
}

//----------------------------------------------------------------------------------------------------------

void* Catalog_TableObject::NextBuffer::operator new (size_t size, SAPDBMem_IRawAllocator& allocator, int bufferLen)
{
    return allocator.Allocate (CATALOG_OFFSET(Catalog_TableObject::NextBuffer, m_colBuffer) + bufferLen);
}

//----------------------------------------------------------------------------------------------------------

Catalog_TableObject::ColumnBuffer::ColumnBuffer(const SQLMan_Identifier& tableName)
: m_sharedFlag(0)
, m_refCount(1)
, m_next(0)
, m_tableName(tableName)
{
}

//----------------------------------------------------------------------------------------------------------

int Catalog_TableObject::ColumnBuffer::BufferOffset()
{
    return CATALOG_OFFSET(Catalog_TableObject::ColumnBuffer, m_colBuffer);
}

//----------------------------------------------------------------------------------------------------------

void Catalog_TableObject::ColumnBuffer::IncRefCount()
{
    Catalog_TableObject::m_refCountSynchronizer.Lock();
    ++m_refCount;
    Catalog_TableObject::m_refCountSynchronizer.Unlock();
}

//----------------------------------------------------------------------------------------------------------

int Catalog_TableObject::ColumnBuffer::DecRefCount(SAPDBMem_IRawAllocator& allocator)
{
    Catalog_TableObject::m_refCountSynchronizer.Lock();
    --m_refCount;
    int refCount = m_refCount;
    if (0 == m_refCount)
    {
        allocator.Deallocate(this);
    }
    Catalog_TableObject::m_refCountSynchronizer.Unlock();
    return refCount;
}

//----------------------------------------------------------------------------------------------------------

void* Catalog_TableObject::ColumnBuffer::operator new (size_t size, SAPDBMem_IRawAllocator& allocator, int bufferLen)
{
    return allocator.Allocate (CATALOG_OFFSET(Catalog_TableObject::ColumnBuffer, m_colBuffer) + bufferLen);
}

//----------------------------------------------------------------------------------------------------------

void* Catalog_TableObject::CreateCopy (SAPDBMem_IRawAllocator& allocator, int offset, SAPDB_UInt4& heapUsage)
{
   SAPDBTRACE_METHOD_DEBUG ("Catalog_TableObject::CreateCopy", Catalog_Trace, 5);
   heapUsage = offset + sizeof(*this) - sizeof(this->bcolumn()) + this->blastindex * sizeof(void*);
   void* p = allocator.Allocate(heapUsage);
   if (p)
   {
       Catalog_TableObject* pTable = REINTERPRET_CAST(Catalog_TableObject*, REINTERPRET_CAST(unsigned char*, p) + offset);
       int moveLen = sizeof(*this) - sizeof(this->bcolumn()) + this->bmaxcol * sizeof(SAPDB_Int2);
       SAPDB_MemCopyNoCheck (pTable, this, moveLen);
       pTable->bptr             = NULL;
       pTable->bcolumnSplitIdx  = csp_maxint2;
       pTable->bcolumnExtension = NULL;
	   SQLMAN_TRACE_BUFFER(Catalog_Trace, 5 ,"table header",pTable,CATALOG_OFFSET(Catalog_TableObject, bptr));
       // calculate required memory for tablename and column information
       int colBufferSize = 0;
       int col;
       for (col = this->bfirstindex; col <= this->blastindex; ++col)
       {
          colBufferSize += CalcAlignedLength(GetColumnInfoLength(*bcolumn()[col-1])); 
       }
       // allocate memory for tablename and column information
       ColumnBuffer* pColumnBuffer = new(allocator, colBufferSize) 
           ColumnBuffer(*REINTERPRET_CAST(SQLMan_Identifier* ,this->btablen));
       if (!pColumnBuffer)
       {
           allocator.Deallocate(p);
           return NULL;
       }
       pTable->btablen = &pColumnBuffer->m_tableName;
	   heapUsage += sizeof (ColumnBuffer) + colBufferSize;
       pTable->bptr = REINTERPRET_CAST(tak_basecolinfo_ptr, pColumnBuffer);
       // copy all column information
       unsigned char* pCol = &pColumnBuffer->m_colBuffer;
       for (col = this->bfirstindex; col <= this->blastindex; ++col)
       {
           int moveLen = GetColumnInfoLength(*bcolumn()[col-1]);
           SAPDB_MemCopyNoCheck (pCol, bcolumn()[col-1], moveLen);
           pTable->bcolumn()[col-1] = REINTERPRET_CAST(Catalog_ColumnInfo*, pCol);
           pCol += CalcAlignedLength(moveLen);
       }
       SQLMAN_TRACE_BUFFER(Catalog_Trace, 5 ,"column buffer", pColumnBuffer, sizeof(ColumnBuffer) + colBufferSize - 1);
   }
   return p;
}

//--------------------------------------------------------------------------------

void Catalog_TableObject::DestroyColumns(SAPDBMem_IRawAllocator& allocator)
{
	SAPDBTRACE_METHOD_DEBUG ("Catalog_TableObject::Destroy", Catalog_Trace, 5);
    REINTERPRET_CAST(ColumnBuffer*, bptr)->DecRefCount(allocator);
}

//--------------------------------------------------------------------------------

#include "hak10.h"

void* Catalog_TableObject::CreateCopy (SAPDBMem_IRawAllocator& allocator)
{
   SAPDBTRACE_METHOD_DEBUG ("Catalog_TableObject::CreateCopy", Catalog_Trace, 5);
   int maxSize    = allocator.GetMaxAllocatableSize();
   int splitIndex = csp_maxint2;
   SAPDBTRACE_WRITELN( Catalog_Trace, 10, "maxSize : " << maxSize);
   int required = sizeof(*this) - sizeof(this->bcolumn()) + this->blastindex * sizeof(void*);
   if (required > maxSize)
   {
       splitIndex = (maxSize - (sizeof(*this) - sizeof(this->bcolumn()))) / sizeof(void*);
       required   = sizeof(*this) - sizeof(this->bcolumn()) + splitIndex * sizeof(void*);
   }
   void* p = allocator.Allocate(required);
   if (p)
   {
       class AutoPtr
       {
       public :
           AutoPtr(SAPDBMem_IRawAllocator& Allocator, Catalog_TableObject* pTable)
               : m_allocator(Allocator)
               , m_pTable(pTable)
           {
           }
           void Release()
           {
               m_pTable = 0;
           }
           ~AutoPtr()
           {
               if (m_pTable)
               {
                   m_pTable->Destroy (m_allocator);
               }
           }
       private :
           SAPDBMem_IRawAllocator& m_allocator;
           Catalog_TableObject*    m_pTable;
       };
       Catalog_TableObject* pTable = REINTERPRET_CAST(Catalog_TableObject*, REINTERPRET_CAST(unsigned char*, p));
       AutoPtr autoPtr(allocator, pTable);
       int moveLen = sizeof(*this) - sizeof(this->bcolumn()) + this->bmaxcol * sizeof(SAPDB_Int2);
       SAPDB_MemCopyNoCheck (pTable, this, moveLen);
       pTable->bptr = NULL;
       pTable->bcolumnExtension = NULL;
       if (splitIndex != csp_maxint2)
       {
           pTable->bcolumnExtension = (tak_columnarr*) allocator.Allocate((this->blastindex - splitIndex) * sizeof(void*));
           pTable->bcolumnSplitIdx  = splitIndex;
       }
	   SQLMAN_TRACE_BUFFER(Catalog_Trace, 5 ,"table header",pTable,CATALOG_OFFSET(Catalog_TableObject, bptr));
       // calculate required memory for tablename and column information
       int buffersNeeded = 1;
       int colBufferSize[16];
       int colBufferCnt [16];
       int col;
       int offset       = ColumnBuffer::BufferOffset();
       colBufferSize[0] = 0;
       colBufferCnt [0] = 0;
       int alignment    = 0;
       for (col = this->bfirstindex; col <= this->blastindex; ++col)
       {
           int required = GetColumnInfoLength(*bcolumn()[col-1]);
           if (offset + colBufferSize[buffersNeeded-1] + alignment + required > maxSize)
           {
               offset                       = NextBuffer::BufferOffset();
               colBufferSize[buffersNeeded] = 0;
               colBufferCnt [buffersNeeded] = 0;
               ++buffersNeeded;
               alignment = 0;
           }
           else
           {
               colBufferSize[buffersNeeded-1] += alignment;
           }
           alignment = CalcAlignedLength(required) - required;
           colBufferSize[buffersNeeded-1] += required;
           colBufferCnt [buffersNeeded-1]++; 
       }
       // allocate memory for tablename and column information
       SAPDBTRACE_WRITELN( Catalog_Trace, 10, "size of colbuffer[0] : " << colBufferSize[0]);
       ColumnBuffer* pColumnBuffer = new(allocator, colBufferSize[0]) 
           ColumnBuffer(*REINTERPRET_CAST(SQLMan_Identifier* ,this->btablen));
       if (!pColumnBuffer)
       {
           return NULL;
       }
       pTable->btablen = &pColumnBuffer->m_tableName;
       pTable->bptr = REINTERPRET_CAST(tak_basecolinfo_ptr, pColumnBuffer);
       // copy all column information
       int            currBuffer = 0;
       unsigned char* pCol       = &pColumnBuffer->m_colBuffer;
       NextBuffer**   pPrev      = &pColumnBuffer->m_next;
       for (col = this->bfirstindex; col <= this->blastindex; ++col)
       {
           if (0 == colBufferCnt [currBuffer])
           {
               ++currBuffer;
               *pPrev = new(allocator, colBufferSize[currBuffer]) NextBuffer;
               if (!*pPrev)
               {
                   return NULL;
               }
               pCol   = &(*pPrev)->m_colBuffer;
               pPrev  = &(*pPrev)->m_next;
           }
           --colBufferCnt [currBuffer];
           int moveLen = GetColumnInfoLength(*bcolumn()[col-1]);
           SAPDB_MemCopyNoCheck (pCol, bcolumn()[col-1], moveLen);
           if (col > splitIndex)
           {
               (*pTable->bcolumnExtension)[col - splitIndex - 1] = REINTERPRET_CAST(Catalog_ColumnInfo*, pCol);
           }
           else
           {
               pTable->bcolumn()[col-1] = REINTERPRET_CAST(Catalog_ColumnInfo*, pCol);
           }
           pCol += CalcAlignedLength(moveLen);
       }
       autoPtr.Release();
   }
   return p;
}

//--------------------------------------------------------------------------------

void Catalog_TableObject::Destroy (SAPDBMem_IRawAllocator& Allocator)
{
    if (tsynonym != btablekind)
    {
        ColumnBuffer* pColBuf = reinterpret_cast<ColumnBuffer*>(bptr);
        if (pColBuf)
        {
            NextBuffer* pNextBuf  = pColBuf->m_next;
            while (pNextBuf)
            {
                NextBuffer* pToFree = pNextBuf;
                pNextBuf = pNextBuf->m_next;
                Allocator.Deallocate(pToFree);
            }
            Allocator.Deallocate(pColBuf);
        }
        Allocator.Deallocate(bcolumnExtension);
    }
    Allocator.Deallocate(this);
}

//--------------------------------------------------------------------------------

Catalog_Object* Catalog_TableObject::Incopy(SQLMan_Context& Context, const Catalog_ObjectId& TableId, SAPDBMem_IRawAllocator& Allocator)
{
    // TODO
    tak_sysbufferaddress p;
    tgg00_BasisError     e;
    Catalog_TableObject::Catalog_Key key(TableId);
    pasbool localCacheUsageOnly   = Context.a_localCacheUsageOnly; 
    Context.a_localCacheUsageOnly = true;
    a10get_sysinfo (Context, key, d_release, p, e);
    Context.a_localCacheUsageOnly = localCacheUsageOnly;
    if (e_ok == e)
    {
        Catalog_TableObject* pTable = reinterpret_cast<Catalog_TableObject*>(p);
        void* pObj;
        if (tsynonym == pTable->btablekind)
        {
            pObj = Allocator.Allocate(pTable->breclen);
            if (pObj)
            {
                SAPDB_MemCopyNoCheck (pObj, pTable, pTable->breclen);
            }
        }
        else
        {
            pObj = pTable->CreateCopy(Allocator);
        }
        a10key_del (Context, p);
        if (0 == pObj)
        {
            Context.SetNoMoreMemoryError (Msg_List(Msg_List::Error, SDBMSG_CATALOG_NO_MEMORY/*, Msg_Arg("MEMREQUEST",?)*/));
        }
        return reinterpret_cast<Catalog_Object*>(pObj);
    }
    else
    {
        if (e != e_sysinfo_not_found)
        {
            Context.ThrowError (e);
        }
    }
    return 0;
}

//--------------------------------------------------------------------------------

void Catalog_TableObject::outcopy(SQLMan_Context& context)
{
#if defined(KERNEL80)
    if (context.IsOk())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        char* pState;
        char* pMsg = 0;
        {
            SQLMan_SQLStatementContext newStatementScope(context);
            KSQL_Connection con;
            if (con.isConnected())
            {
                context.SetInDDLTrigger();
                KSQL_PreparedStatement stmt = con.createPreparedStatement();
                stmt.prepare("INSERT INTO CATALOG.TABLES VALUES "
                             "(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,"
                             "?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,"
                             "?,?,?)");
                int colNo = 0;
                stmt.bind(++colNo, this->bsurrogate);
                stmt.bind(++colNo, this->bauthid);
                stmt.bind(++colNo, *REINTERPRET_CAST(SQLMan_Identifier*, this->btablen));
                stmt.bind(++colNo, &this->blinkexist, 1);
                stmt.bind(++colNo, &this->btreeid, sizeof(this->btreeid));
                stmt.bind(++colNo, this->bmaxcol);
                stmt.bind(++colNo, this->bmaxreclen);
                stmt.bind(++colNo, this->bkeycolcount);
                stmt.bind(++colNo, this->blenfixedcol);
                stmt.bind(++colNo, this->bvarcolcount);
                stmt.bind(++colNo, this->bcntdropcol);
                stmt.bind(++colNo, this->bstringcount);
                stmt.bind(++colNo, this->blongvarcolcnt);
                bool hasComment = (0 != this->bcomment);
                stmt.bind(++colNo, hasComment);
                stmt.bind(++colNo, this->bavgrowlen);
                stmt.bind(++colNo, this->bdatecreate);
                stmt.bind(++colNo, this->btimecreate);
                stmt.bind(++colNo, this->bupdstatdate);
                stmt.bind(++colNo, this->bupdstattime);
                stmt.bind(++colNo, this->bpages);
                stmt.bind(++colNo, this->brows);
                stmt.bind(++colNo, this->balterdate);
                stmt.bind(++colNo, this->baltertime);
                stmt.bind(++colNo, this->bsample);
                stmt.bind(++colNo, &this->bpriv_all_set, sizeof(this->bpriv_all_set));
                stmt.bind(++colNo, &this->bpriv_col_exist, sizeof(this->bpriv_col_exist));
                stmt.bind(++colNo, this->bunloaded);
                stmt.bind(++colNo, this->bindexexist);
                stmt.bind(++colNo, this->bv_level);
                stmt.bind(++colNo, this->bv_tabcount);
                stmt.bind(++colNo, &this->bv_tablekind, sizeof(this->bv_tablekind));
                stmt.bind(++colNo, this->bv_checkopt);
                stmt.bind(++colNo, this->bv_qualexist);
                stmt.bind(++colNo, &this->bv_distinct, sizeof(this->bv_distinct));
                stmt.bind(++colNo, this->bv_viewlist);
                stmt.bind(++colNo, this->bdef_stamp);
                stmt.bind(++colNo, this->bcorr_view);
                stmt.bind(++colNo, &this->bsqlmode, sizeof(this->bsqlmode));
                stmt.bind(++colNo, this->bnamed_constr);
                stmt.bind(++colNo, &this->bshow_kind, sizeof(this->bshow_kind));
                stmt.bind(++colNo, this->brecreate_view);
                stmt.bind(++colNo, &this->bshowkind,  sizeof(this->bshowkind));
                stmt.bind(++colNo, &this->battributes, sizeof(this->battributes));
                rc = stmt.execute();
                if (0 != rc)
                {
                    stmt.describeLastError(pState, pMsg);
                }
            }
        }
        if (0 != rc)
        {
            SAPDB_ToStringClass errCodeStr = ToStr(rc);
            //SAPDBErr_MessageList msg ("Catalog", __CONTEXT__, 
            //    SAPDBErr_MessageList::Error, CATALOG_ERR_CATALOG_UPDATE_FAILED_ID,
            //    "Interface runtime error", 1, errCodeStr);
            //SAPDBErr_MessageList& errList = context.GetErrorList();
            //errList.AppendNewMessage(msg);
            //if (pMsg)
            //{
            //    SAPDBErr_MessageList msg ("Catalog", __CONTEXT__, 
            //        SAPDBErr_MessageList::Error, CATALOG_ERR_CATALOG_UPDATE_FAILED_ID,
            //        "Interface runtime error", 1, pMsg);
            //    errList.AppendNewMessage(msg);
            //}
            Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED,
	                        Msg_Arg("IFR_ERROR",errCodeStr),
                            Msg_Arg("DESCRIPTION",(pMsg?pMsg:"")));
            Msg_List& errList = context.GetErrorList();
            errList.AppendNewMessage(msg);
        )

            context.ThrowError (e_system_error); // TODO
        } 
        else
        {
            //incopy(context, this->bsurrogate);
        }
    }
#endif
}


