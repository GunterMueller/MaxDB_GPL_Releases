/*


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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



*/

#include "SAPDB/SQLManager/SQLMan_Context.hpp"
#include "SAPDB/SQLManager/SQLMan_Tracing.hpp"
#include "SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_ContainerObject.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Instance.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_IsoLevelScope.hpp"
#include "SQLManager/Catalog/Catalog_LockedSecondaryKeyScope.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_RequestScope.hpp"
#include "SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp" 
#include "SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp" 

#include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h"

#include "SAPDB/KernelCommon/Kernel_VTrace.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp"

//-----------------------------------------------------------------------------

Catalog_ContainerObject::Catalog_Key::Catalog_Key(const Catalog_ObjectId& SchemaId, SAPDB_UInt4 ContainerNo, const Catalog_GUID& Guid)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_AuthorizationObject::Catalog_Key::Catalog_Key", Catalog_Trace, 1);   
     this->SetEntryType(TYPE_CONTAINER);
     this->SetSurrogate(SchemaId);
     this->slinkage()[1] = 1;
     SAPDB_UInt4* pInt4 = reinterpret_cast<SAPDB_UInt4*>(&this->sidentifier()[0]);
     *pInt4++ = ContainerNo;
     SAPDB_MemCopyNoCheck(pInt4, &Guid[0], sizeof(Guid));
     this->skeylen = KeyLength();
}

//-----------------------------------------------------------------------------

const Catalog_ObjectId& Catalog_ContainerObject::Catalog_Key::GetSchemaId() const
{
    return *reinterpret_cast<const Catalog_ObjectId*>(&stableid()); 
}

//-----------------------------------------------------------------------------

SAPDB_UInt4 Catalog_ContainerObject::Catalog_Key::GetContainerNo() const
{
    return *reinterpret_cast<const SAPDB_UInt4*>(&this->sidentifier()[0]);
}

//-----------------------------------------------------------------------------

const Catalog_GUID& Catalog_ContainerObject::Catalog_Key::GetGuid() const
{
    const SAPDB_UInt4* pInt4 = reinterpret_cast<const SAPDB_UInt4*>(&this->sidentifier()[0]);
    ++pInt4;
    return *reinterpret_cast<const Catalog_GUID*>(pInt4);
}

//-----------------------------------------------------------------------------

int Catalog_ContainerObject::Catalog_Key::KeyLength()
{

    return Catalog_Object::DefaultKeyLen + sizeof(SAPDB_UInt4) + sizeof(Catalog_GUID);
}

//-----------------------------------------------------------------------------

Catalog_ContainerObject::Catalog_ContainerObject(
        const Catalog_ObjectId&  SchemaId,
        SAPDB_UInt4              ContainerNo,
        const Catalog_GUID&      Guid,
        int                      NoOfKeyFiles,
        bool                     CachedKeys,
        bool                     IsVarContainer,
        SAPDB_UInt4              Size,
        const FileDir_FileNo&    FileNo,
        const SQLMan_Identifier& ClassName)
        : Catalog_Object (Catalog_Key::KeyLength(), sizeof(Catalog_ContainerObject) - CATALOG_OFFSET(Catalog_ContainerObject::ContainerInfo, m_noOfKeyFiles)) 
{
    new((void*) this) Catalog_Key(SchemaId, ContainerNo, Guid);
    ContainerInfo* pInfo = reinterpret_cast<ContainerInfo*>(this);
    pInfo->m_noOfKeyFiles   = NoOfKeyFiles;
    pInfo->m_isVarContainer = IsVarContainer;
    pInfo->m_cachedKeys     = CachedKeys;
    pInfo->m_size           = Size;
    pInfo->m_fileNo         = FileNo;
    SAPDB_MemCopyNoCheck (&pInfo->m_className[0], &ClassName[0], sizeof(pInfo->m_className));
}

//-----------------------------------------------------------------------------

bool Catalog_ContainerObject::CreateContainer 
(
 SQLMan_Context&          Context,
 const Catalog_ObjectId&  SchemaId,
 SAPDB_UInt4              ContainerNo,
 const Catalog_GUID&      Guid,
 int                      NoOfKeyFiles,
 bool                     CachedKeys,
 bool                     IsVarContainer,
 SAPDB_UInt4              Size,
 const FileDir_FileNo&    FileNo,
 const SQLMan_Identifier& ClassName
 )
{
    ContainerInfo containerInfo;
    const bool cUnlock = true;
    const bool cNoWait = true;
    Lock (Context, SchemaId, ContainerNo, Guid, !cUnlock, !cNoWait);
    if (Context.IsOk())
    {
        // check, if the container already exists. if we would insert without this check,
        // error 200 would leave an exlusive lock which may result in a deadlock
        IncopyResult cpr = reinterpret_cast<Catalog_ContainerObject*>(&containerInfo)->Incopy (Context, SchemaId, ContainerNo, Guid);
        if (!Context.IsOk())
        {
            return false;
        }
        if (icp_ok == cpr)
        {
            // container has already been created
            Lock (Context, SchemaId, ContainerNo, Guid, cUnlock, !cNoWait);
            Context.SetReturnCode(SQL_DUPLICATE_KEY);
            return false;
        }
        Catalog_ContainerObject* p = new ((void*) &containerInfo) 
            Catalog_ContainerObject(SchemaId, ContainerNo, Guid, NoOfKeyFiles, CachedKeys, IsVarContainer, Size, FileNo, ClassName);
        return p->Outcopy(Context);
    }
    return false;
}

//-----------------------------------------------------------------------------

bool Catalog_ContainerObject::Drop
(
 SQLMan_Context&          Context,
 const Catalog_ObjectId&  SchemaId,
 SAPDB_UInt4              ContainerNo,
 const Catalog_GUID&      Guid
 )
{
    SAPDBTRACE_ROUTINE_DEBUG ("Catalog_ContainerObject::Drop", Catalog_Trace, 1);
    if (Context.IsOk())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        const char* pState;
        const char* pMsg = 0;
        {
            SQLMan_SQLStatementContext newStatementScope(Context);
            KSQL_Connection con;
            if (con.isConnected())
            {
                Context.SetInDDLTrigger();
                KSQL_PreparedStatement stmt = con.createPreparedStatement();
                stmt.prepare("DELETE SYSCATALOG.CLASSCONTAINERS WHERE "
                    "SCHEMAID = ? AND CONTAINERNO = ? AND GUID = ?");
                int colNo = 0;
                stmt.bind(++colNo, SchemaId);
                stmt.bind(++colNo, ContainerNo);
                stmt.bind(++colNo, CONST_CAST(Catalog_GUID*, &Guid), sizeof(Guid));
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
            Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED,
	                        Msg_OptArg("IFR_ERROR",errCodeStr),
                            Msg_OptArg("DESCRIPTION",(pMsg?pMsg:"")));
            Msg_List& errList = Context.GetErrorList();
            errList.AppendNewMessage(msg);
            Context.SetReturnCode(rc);
        }
    }
    return Context.IsOk();
}

//-----------------------------------------------------------------------------

void Catalog_ContainerObject::DropSchemaContainers 
(
 SQLMan_Context&         Context,
 const Catalog_ObjectId& SchemaId
 )
{
    SAPDBTRACE_ROUTINE_DEBUG ("Catalog_ContainerObject::Drop", Catalog_Trace, 1);
    if (Context.IsOk())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        const char* pState;
        const char* pMsg = 0;
        {
            SQLMan_SQLStatementContext newStatementScope(Context);
            KSQL_Connection con;
            if (con.isConnected())
            {
                Context.SetInDDLTrigger();
                Catalog_IsoLevelScope isoScope (Context, 1);
                KSQL_PreparedStatement stmt = con.createPreparedStatement();
                stmt.prepare("DELETE SYSCATALOG.CLASSCONTAINERS WHERE SCHEMAID = ?");
                int colNo = 0;
                stmt.bind(++colNo, SchemaId);
                rc = stmt.execute();
                if (SQL_ROW_NOT_FOUND == rc)
                {
                    rc = 0;
                }
                if (0 != rc) 
                {
                    stmt.describeLastError(pState, pMsg);
                }
            }
        }
        if (0 != rc)
        {
            SAPDB_ToStringClass errCodeStr = ToStr(rc);
            Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED,
                Msg_OptArg("IFR_ERROR",errCodeStr),
                Msg_OptArg("DESCRIPTION",(pMsg?pMsg:"")));
            Msg_List& errList = Context.GetErrorList();
            errList.AppendNewMessage(msg);
            Context.SetReturnCode(rc);
        }
    }
}

//-----------------------------------------------------------------------------

void Catalog_ContainerObject::GetContainerInfo 
(
 SQLMan_Context&         Context,
 const FileDir_FileNo&   FileNo,
 Catalog_GUID&           Guid, 
 Catalog_ObjectId&       SchemaObjectId, 
 SAPDB_UInt4&            ContainerNo
 )
{
    SAPDBTRACE_ROUTINE_DEBUG ("Catalog_ContainerObject::Drop", Catalog_Trace, 1);
    if (Context.IsOk())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        const char* pState;
        const char* pMsg = 0;
        {
            SQLMan_SQLStatementContext newStatementScope(Context);
            KSQL_Connection con;
            if (con.isConnected())
            {
                Context.SetInDDLTrigger();
                Catalog_IsoLevelScope isoScope (Context, 0);
                KSQL_PreparedStatement stmt = con.createPreparedStatement();
                stmt.prepare("SELECT GUID, SCHEMAID, CONTAINERNO INTO ?, ?, ? "
                    "FROM SYSCATALOG.CLASSCONTAINERS WHERE FILENO = ?");
                int colNo = 0;
                stmt.bind(++colNo, &Guid, sizeof(Guid));
                stmt.bind(++colNo, SchemaObjectId);
                stmt.bind(++colNo, ContainerNo);
                stmt.bind(++colNo, const_cast<FileDir_FileNo*>(&FileNo), sizeof(FileNo));
                int length = sizeof(FileNo);
                const char* pFileNo = FileNo.GetCharArray() + sizeof(FileNo) - 1;
                while (0 == *pFileNo)
                {
                    --length;
                    --pFileNo;
                }
                Catalog_LockedSecondaryKeyScope secondaryKeyLockedScope(Context);
                rc = secondaryKeyLockedScope.Lock (Catalog_Instance::Instance().GetClassContainersIndexId(), 
                    reinterpret_cast<const SAPDB_Byte*>(FileNo.GetCharArray()), length, 0);
                if (0 == rc) {
                    rc = stmt.execute();
                }
                if (0 != rc) 
                {
                    stmt.describeLastError(pState, pMsg);
                }
            }
        }
        if (0 != rc)
        {
            SAPDB_ToStringClass errCodeStr = ToStr(rc);
            Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED,
                Msg_OptArg("IFR_ERROR",errCodeStr),
                Msg_OptArg("DESCRIPTION",(pMsg?pMsg:"")));
            Msg_List& errList = Context.GetErrorList();
            errList.AppendNewMessage(msg);
            Context.SetReturnCode(rc);
        }
    }
}

//-----------------------------------------------------------------------------

const Catalog_ContainerObject::Catalog_Key& Catalog_ContainerObject::GetKey() const
{
    return *reinterpret_cast<const Catalog_Key*>(this);
}

//-----------------------------------------------------------------------------

SAPDB_UInt4 Catalog_ContainerObject::GetSize() const
{
    return reinterpret_cast<const ContainerInfo*>(this)->m_size;
}

//-----------------------------------------------------------------------------

FileDir_FileNo Catalog_ContainerObject::GetFileNo() const
{
    return reinterpret_cast<const ContainerInfo*>(this)->m_fileNo;
}

//-----------------------------------------------------------------------------

Catalog_ContainerObject* Catalog_ContainerObject::Incopy (SQLMan_Context& Context, const Catalog_Object::Catalog_Key& Key, SAPDBMem_IRawAllocator& Allocator,  bool NoWait)
{
    ContainerInfo* pInfo = new (Allocator) ContainerInfo;
    if (pInfo)
    {
        const Catalog_ContainerObject::Catalog_Key* pKey = reinterpret_cast<const Catalog_ContainerObject::Catalog_Key*>(&Key);
        Catalog_ContainerObject* pContainerObj     = reinterpret_cast<Catalog_ContainerObject*>(pInfo);
        Catalog_ContainerObject::Catalog_Key* pInfoKey = reinterpret_cast<Catalog_ContainerObject::Catalog_Key*>(pInfo);
        *pInfoKey = *pKey;
        Catalog_Object::IncopyResult icp = pContainerObj->Incopy (Context, pKey->GetSchemaId(), pKey->GetContainerNo(), pKey->GetGuid(), NoWait);
        if (icp_ok == icp)
        {
            return pContainerObj;
        }
        destroy (pInfo, Allocator);
    }
    return 0;
}

//-----------------------------------------------------------------------------

Catalog_Object::IncopyResult Catalog_ContainerObject::Incopy (
                                      SQLMan_Context&          Context, 
                                      const Catalog_ObjectId&  SchemaId,
                                      SAPDB_UInt4              ContainerNo,
                                      Catalog_GUID             Guid,
                                      bool                     NoWait)
{
    Catalog_RequestScope requestScope(Context);
    if (Context.IsOk())
    {
        Kernel_VTrace vTrace;
        if (!Catalog_Trace.TracesLevel(1) && requestScope.AnyVtrace())
        {
            vTrace << "CATALOG ACCESS : Container " << FlushLine;
            vTrace << "SchemaId    " << SAPDBTrace_Hex (&SchemaId[0], sizeof(SchemaId)) << FlushLine;
            vTrace << "ContainerNo " << ContainerNo << FlushLine;
            vTrace << "GUID        " << SAPDBTrace_Hex (&Guid, sizeof(Guid)) << FlushLine;
        }
        KSQL_PreparedStatement::SQLCode rc = 0;
        const char* pState;
        const char* pMsg = 0;
        {
            SQLMan_SQLStatementContext newStatementScope(Context);
            if (!Context.IsOk())
            {
                rc = Context.ReturnCode();
            }
            else
            {
                KSQL_Connection con;
                if (con.isConnected())
                {
                    Context.SetInDDLTrigger();
                    // initialize the object key
                    ContainerInfo* pInfo = reinterpret_cast<ContainerInfo*>(this);
                    //new((void*) pInfo) Catalog_ContainerObject::Catalog_Key(SchemaId, ContainerNo, Guid);
                    KSQL_PreparedStatement stmt = con.createPreparedStatement();
                    Catalog_IsoLevelScope isoScope (Context, 0);
                    const bool cUnlock = true;
                    if (NoWait)
                    {
                        if (!Lock (Context,  SchemaId, ContainerNo, Guid, !cUnlock, true))
                        {
                            rc = Context.ReturnCode();
                        }
                    }
                    if (0 == rc)
                    {
                        stmt.prepare("SELECT SCHEMAID, CONTAINERNO, GUID, FILENO, KEYFILES, ISVARCONTAINER,"
                            "CACHEDKEYS, SIZE, CLASSNAME INTO ?,?,?,?,?,?,?,?,? FROM SYSCATALOG.CLASSCONTAINERS "
                            "WHERE SCHEMAID = ? AND CONTAINERNO = ? and GUID = ?");
                        int colNo = 0;
                        // bind out parameters
                        stmt.bind(++colNo, pInfo->m_schemaId);
                        stmt.bind(++colNo, pInfo->m_containerNo);
                        stmt.bind(++colNo, &pInfo->m_guid, sizeof(pInfo->m_guid));
                        stmt.bind(++colNo, &pInfo->m_fileNo, sizeof(pInfo->m_fileNo));
                        stmt.bind(++colNo, pInfo->m_noOfKeyFiles);
                        stmt.bind(++colNo, pInfo->m_isVarContainer);
                        stmt.bind(++colNo, pInfo->m_cachedKeys);
                        stmt.bind(++colNo, pInfo->m_size);
                        IFR_Length length = sizeof(pInfo->m_className);
                        stmt.bindAsciiOutput(++colNo, &pInfo->m_className[0], &length, false);
                        // bind in parameters
                        stmt.bind(++colNo, SchemaId);
                        stmt.bind(++colNo, ContainerNo);
                        stmt.bind(++colNo, &Guid, sizeof(Guid));
                        rc = stmt.execute();
                        if (0 != rc)
                        {
                            if (!Catalog_Trace.TracesLevel(1) && requestScope.AnyVtrace())
                            {
                                vTrace << "CATALOG CONTAINER ACCESS ERROR : " << rc << FlushLine;
                            }
                            if (SQL_ROW_NOT_FOUND == rc)
                            {
                                return icp_not_found;
                            }
                            stmt.describeLastError(pState, pMsg);
                            SAPDB_ToStringClass errCodeStr = ToStr(rc);
                            Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED,
                                Msg_OptArg("IFR_ERROR",errCodeStr),
                                Msg_OptArg("DESCRIPTION",(pMsg?pMsg:"")));
                            Msg_List& errList = Context.GetErrorList();
                            errList.AppendNewMessage(msg);
                        }
                        else
                        {
                            if (NoWait)
                            {
                                Lock (Context,  SchemaId, ContainerNo, Guid, cUnlock, true);
                            }
                            if (!Catalog_Trace.TracesLevel(1) && requestScope.AnyVtrace())
                            {
                                vTrace << "CATALOG CONTAINER ACCESS OK" << FlushLine;
                            }
                        }
                    }
                }
            }
        }
        if (0 == rc)
        {
            return icp_ok;
        }
        else
        {
            vTrace << "set returncode " << rc << FlushLine;
            Context.SetReturnCode(rc);
            return icp_error;
        }
    }
    return icp_not_found;
}

//-----------------------------------------------------------------------------

bool Catalog_ContainerObject::IsVarObjectContainer() const
{
    return reinterpret_cast<const ContainerInfo*>(this)->m_isVarContainer;
}

//-----------------------------------------------------------------------------

bool Catalog_ContainerObject::Lock
(
 SQLMan_Context&          Context,
 const Catalog_ObjectId&  SchemaId,
 SAPDB_UInt4              ContainerNo,
 const Catalog_GUID&      Guid,
 bool                     DoUnlock,
 bool                     NoWait
 )
{
    SAPDBTRACE_ROUTINE_DEBUG ("Catalog_ContainerObject::Lock", Catalog_Trace, 1);
    if (Context.IsOk())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        const char* pState;
        const char* pMsg = 0;
        {
            SQLMan_SQLStatementContext newStatementScope(Context);
            KSQL_Connection con;
            if (con.isConnected())
            {
                Context.SetInDDLTrigger();
                KSQL_PreparedStatement stmt = con.createPreparedStatement();
                if (DoUnlock)
                {
                    stmt.prepare("UNLOCK ROW SYSCATALOG.CLASSCONTAINERS KEY "
                        "SCHEMAID = ?, CONTAINERNO = ?, GUID = ? IN EXCLUSIVE MODE");
                }
                else
                {
                    if (NoWait)
                    {
                        stmt.prepare("LOCK (NOWAIT) ROW SYSCATALOG.CLASSCONTAINERS KEY "
                            "SCHEMAID = ?, CONTAINERNO = ?, GUID = ? IN EXCLUSIVE MODE");
                    }
                    else
                    {
                        stmt.prepare("LOCK ROW SYSCATALOG.CLASSCONTAINERS KEY "
                            "SCHEMAID = ?, CONTAINERNO = ?, GUID = ? IN EXCLUSIVE MODE");
                    }
                }
                int colNo = 0;
                stmt.bind(++colNo, SchemaId);
                stmt.bind(++colNo, ContainerNo);
                stmt.bind(++colNo, CONST_CAST(Catalog_GUID*, &Guid), sizeof(Guid));
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
            Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED,
	                        Msg_OptArg("IFR_ERROR",errCodeStr),
                            Msg_OptArg("DESCRIPTION",(pMsg?pMsg:"")));
            Msg_List& errList = Context.GetErrorList();
            errList.AppendNewMessage(msg);
            Context.SetReturnCode(rc);
        }
    }
    return Context.IsOk();
}

//-----------------------------------------------------------------------------

bool Catalog_ContainerObject::Outcopy(SQLMan_Context& Context)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_ContainerObject::CreateContainer", Catalog_Trace, 1);
    if (Context.IsOk())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        const char* pState;
        const char* pMsg = 0;
        {
            SQLMan_SQLStatementContext newStatementScope(Context);
            KSQL_Connection con;
            if (con.isConnected())
            {
                SAPDBTRACE_WRITELN( Catalog_Trace, 5, "connected");
                Context.SetInDDLTrigger();
                KSQL_PreparedStatement stmt = con.createPreparedStatement();
                stmt.prepare("INSERT INTO SYSCATALOG.CLASSCONTAINERS VALUES "
                    "(?,?,?,?,?,?,?,timestamp,?,?)");
                int colNo = 0;
                ContainerInfo* pInfo = reinterpret_cast<ContainerInfo*>(this);
                stmt.bind(++colNo, pInfo->m_schemaId);
                stmt.bind(++colNo, pInfo->m_containerNo);
                stmt.bind(++colNo, &pInfo->m_guid, sizeof(pInfo->m_guid));
                stmt.bind(++colNo, &pInfo->m_fileNo, sizeof(pInfo->m_fileNo));
                stmt.bind(++colNo, pInfo->m_noOfKeyFiles);
                stmt.bind(++colNo, pInfo->m_isVarContainer);
                stmt.bind(++colNo, pInfo->m_cachedKeys);
                stmt.bind(++colNo, pInfo->m_size);
                stmt.bindAsciiInput(++colNo, &pInfo->m_className[0], sizeof(pInfo->m_className), false);
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
            Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED,
	                        Msg_OptArg("IFR_ERROR",errCodeStr),
                            Msg_OptArg("DESCRIPTION",(pMsg?pMsg:"")));
            Msg_List& errList = Context.GetErrorList();
            errList.AppendNewMessage(msg);
            Context.SetReturnCode(rc);
        }
    }
    return Context.IsOk();
}

//-----------------------------------------------------------------------------

bool Catalog_ContainerObject::UseCachedKeys() const
{
    return reinterpret_cast<const ContainerInfo*>(this)->m_cachedKeys;
}