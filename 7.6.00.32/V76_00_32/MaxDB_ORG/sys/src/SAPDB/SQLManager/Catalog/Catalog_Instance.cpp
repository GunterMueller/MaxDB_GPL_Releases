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

#include "hak01.h"
#include "hak103.h"
#include "hak51.h"
#include "hbd998.h"

#include "SAPDB/SQLManager/SQLMan_ChangedUserScope.hpp"
#include "SAPDB/SQLManager/SQLMan_Context.hpp"
#include "SAPDB/SQLManager/SQLMan_Tracing.hpp"
#include "SAPDB/SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Types.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Instance.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Cache.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp"
#include "SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp"
#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h"

#include "SAPDB/RunTime/MemoryManagement/RTEMem_RawAllocator.hpp"

#include "KernelCommon/Kernel_OpMsg.hpp"

#include "hgg01_3.h"
#include "ggg00.h"

#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"


#include "hbd01.h"
#include "hbd01_1.h"

Catalog_Instance* Catalog_Instance::m_pInstance = 0;

//-------------------------------------------------------------------------------------------------

void Catalog_Instance::CreateInstance()
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(Catalog_Instance));
    if (0 == m_pInstance)
    {
        m_pInstance = new((void*) Space) Catalog_Instance;
    }
}

//-------------------------------------------------------------------------------------------------

Catalog_IAuthorization& Catalog_IAuthorization::GetInstance()
{
    return Catalog_Instance::Instance();
}

//-------------------------------------------------------------------------------------------------

Catalog_Interface& Catalog_Interface::GetInstance()
{
    return Catalog_Instance::Instance();
}

//-------------------------------------------------------------------------------------------------

void Catalog_Interface::Initialize()
{
    Catalog_Instance::CreateInstance();
}

//-------------------------------------------------------------------------------------------------

Catalog_Instance::Catalog_Instance()
: m_schemaCreated(false)
, m_tablesCreated(false)
, m_unicode(g01unicode > 0)
, m_schemaId()
, m_ownerId()
, m_schema(0)
, m_sysdba(0)
, m_catalogSchemaName(a01_i_catalog)
, m_blankname(a01_il_b_identifier)
, m_omsSchemaName(a01_i_oms_schema)
, m_tempname(a01_i_temp)
{
    m_schemaId[0] = 0xff;
    m_schemaId[1] = 0xff;
    m_schemaId[7] = 8;
}

//-------------------------------------------------------------------------------------------------

bool Catalog_Instance::CatalogSchemaCreated() const
{
    return m_schemaCreated;
}

//-------------------------------------------------------------------------------------------------

bool Catalog_Instance::ExistCatalogTables() const
{
    return m_tablesCreated;
}

//-------------------------------------------------------------------------------------------------

const Catalog_ObjectId& Catalog_Instance::GetCatalogOwnerId() const
{
    return m_ownerId;
}

//-------------------------------------------------------------------------------------------------

const SQLMan_Identifier& Catalog_Instance::GetCatalogSchemaName() const
{
    return m_catalogSchemaName;
}

//-------------------------------------------------------------------------------------------------

const Catalog_ObjectId& Catalog_Instance::GetCatalogSchemaId() const
{
    return m_schemaId;
}

//-------------------------------------------------------------------------------------------------

void Catalog_Instance::Abort(const Msg_List& errorList)
{
    SQLMan_Context* pContext = SQLMan_Context::GetContext();
    if (pContext)
    {
        Msg_List& list = pContext->GetErrorList();
        list.AppendNewMessage(errorList);
        RTE_Crash(list);
    }
    RTE_Crash(errorList);
}

//-------------------------------------------------------------------------------------------------

static const char* TheCatalogDomains[] = {
  "create domain catalog_objectid char(8) byte"
    ,
  "create domain catalog_identifier char(32)"
};

//-------------------------------------------------------------------------------------------------

static const char* TheCatalogTables[] = {
  "create table syscatalog.authorization ("
  "authorizationid    catalog_objectid,"
  "userid             fixed(10),"
  "defaultcode        char(1) byte,"
  "roleinfo           char(1) byte,"
  "isuser             boolean,"
  "isusergroup        boolean,"
  "isrole             boolean,"
  "isschema           boolean,"
  "userkind           char(1) byte,"
  "issysdba           boolean,"
  "isexclusive        boolean,"
  "isreplication      boolean,"
  "password           char(24) byte,"
  "supportpassword    char(24) byte,"
  "groupid            catalog_objectid,"
  "ownerid            catalog_objectid,"
  "createdate         timestamp,"
  "passwordchangedate timestamp,"
  "alterdate          timestamp,"
  "schemaid           fixed(10),"
  "timeout            fixed(10),"
  "costwarning        fixed(10),"
  "costlimit          fixed(10),"
  "cachelimit         fixed(10),"
  "rolecount          integer,"
  "hascomment         boolean,"
  "name               catalog_identifier, "
  "namelength         integer,"
  "connectenabled     boolean,"
  "unique(name), "
  "primary key(authorizationid)) internal"
  ,
  "create table syscatalog.schemaprivileges ("
  "schemaid     catalog_objectid,"
  "grantee      catalog_objectid,"
  "grantor      catalog_objectid,"
  "createin     boolean,"
  "dropin       boolean,"
  "alterin      boolean,"
  "grantoption  boolean,"
  "grantdate    timestamp,"
  "primary key(schemaid, grantee)) internal"
  ,
  "create table syscatalog.classcontainers ("
  "schemaid       catalog_objectid,"
  "containerno    fixed(10),"
  "guid           char(16) byte,"
  "fileno         char(8) byte,"
  "keyfiles       fixed(2),"
  "isvarcontainer boolean,"
  "cachedkeys     boolean,"
  "createdate     timestamp,"
  "size           fixed(5),"
  "classname      char(64) ascii,"
  "unique(fileno),"
  "primary key (schemaid, containerno, guid)) internal"
};

externC tsp00_TaskId a362OnlineTaskId;

//-------------------------------------------------------------------------------------------------

int Catalog_Instance::CreateCatalogTables()
{
    SQLMan_Context* pContext = SQLMan_Context::GetContext();
    if (pContext)
    {
        a51SwitchUser (*pContext, g01glob.sysuser_name);
        if (pContext->IsOk()) 
        {
            m_ownerId = g01glob.sysuser_id;
            tsp00_TaskId currentOnlineTaskId = a362OnlineTaskId;
            a362OnlineTaskId = pContext->TransContext().trTaskId_gg00; // this task is allowed to run even kernel isn't online
            KSQL_Connection con;
            if (con.isConnected())
            {
                pContext->SetInDDLTrigger();
                ExecuteSqlStatement(con, "create schema syscatalog");
                const bool registerCatalogTables = true;
                this->Restart(!registerCatalogTables);
                m_tablesCreated = false;
                if (m_unicode)
                {
                    pContext->a_defaultcode = csp_unicode;
                }
                this->CreateCatalogDomains(con);
                this->CreateCatalogTables(con);
                Catalog_ISessionCache& cache = pContext->GetSessionCatalogCache();
                cache.EnableCatalogTables();
                this->RegisterCatalogTables();
                m_tablesCreated = true;
                // store sysdba user and catalog schema into authorization table
                const bool cUpdate = true;
                m_schema->Outcopy (*pContext, !cUpdate);
                m_sysdba->Outcopy (*pContext, !cUpdate);
                // store control user into authorization table
                Catalog_AuthorizationObject::StoreControlUser(*pContext);
                // create the oms default schema
                ExecuteSqlStatement(con, "create schema oms");
                if (!pContext->IsOk())
                {   // DDT: ExecuteSqlStatement already writes an messages with abort !?
                    Msg_List msg(Msg_List::Error,SDBMSG_CATALOG_CREATE_OBJECT_FAILED);
                    this->Abort (msg);
                }
            }
            a362OnlineTaskId = currentOnlineTaskId;
        }
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------

void Catalog_Instance::CreateCatalogDomains (KSQL_Connection& con)
{
    for (int ix = 0; ix < sizeof(TheCatalogDomains) / sizeof(TheCatalogDomains[0]); ++ix)
    {
        this->ExecuteSqlStatement(con, TheCatalogDomains[ix]); 
    }
}

//-------------------------------------------------------------------------------------------------

void Catalog_Instance::CreateCatalogTables (KSQL_Connection& con)
{
    for (int ix = 0; ix < sizeof(TheCatalogTables) / sizeof(TheCatalogTables[0]); ++ix)
    {
        this->ExecuteSqlStatement(con, TheCatalogTables[ix]); 
    }
}

//-------------------------------------------------------------------------------------------------

void Catalog_Instance::ExecuteSqlStatement(KSQL_Connection& con, const char* createStatement)
{
    KSQL_Statement stmt = con.createStatement();
    int errCode = stmt.asciiExecute(createStatement);
    if (0 != errCode)
    {
        SAPDB_ToStringClass errCodeStr = ToStr(errCode);
        Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_CREATE_OBJECT_FAILED,
	                    Msg_OptArg("ERRORTEXT",errCodeStr),
	                    Msg_OptArg("SQLSTATEMENT",createStatement));
        this->Abort(msg);
    }
}

//-------------------------------------------------------------------------------------------------

bool Catalog_Instance::IncopySpecialAuthorizationObject
(
 SQLMan_Context&              context, 
 const SQLMan_Identifier&     name, 
 Catalog_AuthorizationObject& dest, 
 bool&                        isCopied
 ) const  
{
    const Catalog_AuthorizationObject* pObj;
    if (name == a01_i_catalog)
    {
        pObj = m_schema;
    }
    else
    {
        if (name == g01glob.sysuser_name)
        {
            pObj = m_sysdba;
        }
        else
        {
            return false;
        }
    }
    if (m_schemaCreated)
    {
        dest.suser() = pObj->suser();
        isCopied = true;
    }
    else
    {
        isCopied = false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------------

bool Catalog_Instance::IncopySpecialAuthorizationObject
(
 SQLMan_Context&              context, 
 const Catalog_ObjectId&      authId, 
 Catalog_AuthorizationObject& dest, 
 bool&                        isCopied
 ) const  
{
    const Catalog_AuthorizationObject* pObj;
    if (authId == m_schemaId)
    {
        pObj = m_schema;
    }
    else
    {
        if (authId == m_ownerId)
        {
            pObj = m_sysdba;
        }
        else
        {
            return false;
        }
    }
    if (m_schemaCreated)
    {
        dest.suser() = pObj->suser();
        isCopied = true;
    }
    else
    {
        isCopied = false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------------

bool Catalog_Instance::IsUnicodeCatalog() const
{
    return m_unicode;
}

//-------------------------------------------------------------------------------------------------

void Catalog_Instance::Migrate()
{
    SQLMan_Context* pContext = SQLMan_Context::GetContext();
    if (pContext)
    {
        bool appendConnectEnabled = false;
        {
            const bool cConsiderResultTables = true; 
            const bool cConsiderSYSDDTables  = true;
            Catalog_TableDescriptor auth (*pContext, SQLMan_Identifier ("SYSCATALOG", /*ASCIISource*/true), 
				                                     SQLMan_Identifier ("AUTHORIZATION", /*ASCIISource*/true), 
													 !cConsiderResultTables, !cConsiderSYSDDTables);
            if (auth)
            {
                if (!auth.ContainsColumn(SQLMan_Identifier ("CONNECTENABLED", /*ASCIISource*/true)))
                {
                    appendConnectEnabled = true;
                }
            }
        }
        if (appendConnectEnabled)
        {
            Kernel_OpInfo ("MIGRAT") << "appending column CONNECTENABLED ...";
            KSQL_Connection con;
            if (con.isConnected())
            {
                pContext->SetInDDLTrigger();
                ExecuteSqlStatement (con, "alter table syscatalog.authorization add connectenabled boolean");
                if (con.isOk())
                {
                    ExecuteSqlStatement (con, 
                        "update syscatalog.authorization set connectenabled = "
                        "decode (isuser, TRUE, isuser, isusergroup)");
                }
                m_sysdba->EnableConnect();
            }
        }
    }
 }

//-------------------------------------------------------------------------------------------------

void Catalog_Instance::MigrateToUnicode()
{
    SQLMan_Context* pContext = SQLMan_Context::GetContext();
    if (pContext)
    {
        this->Restart();
        SQLMan_ChangedUserScope sysuserContext(*pContext, g01glob.sysuser_name);
        if (pContext->IsOk()) 
        {
            m_ownerId = g01glob.sysuser_id;
            KSQL_Connection con;
            if (con.isConnected())
            {
                pContext->SetInDDLTrigger();
                this->ExecuteSqlStatement(con, "alter table syscatalog.authorization modify name char(32) unicode");
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------

void Catalog_Instance::RegisterCatalogTables()
{
    Catalog_Cache::RegisterCatalogTables ();
}

//-------------------------------------------------------------------------------------------------

bool Catalog_Instance::Restart()
{
    const bool registerCatalogTables = true;
    return Restart (registerCatalogTables);
}

//-------------------------------------------------------------------------------------------------

bool Catalog_Instance::Restart(bool RegisterCatalogTables)
{
    SQLMan_Identifier catalog(a01_i_catalog);
    Catalog_ISessionCache& cache = SQLMan_Context::GetContext()->GetSessionCatalogCache();
    Catalog_AuthorizationObject* p = cache.LoadAuthorizationObject(catalog);
    // look for catalog schema
    if (p) {
        m_schema = p->CreateCopy(RTEMem_RawAllocator::Instance());;
    }
    else
    {
        // if not present, a migration will be necessary
        return false;
    }
    p = cache.LoadAuthorizationObject(g01glob.sysuser_name);
    if (p) {
        m_sysdba = p->CreateCopy(RTEMem_RawAllocator::Instance());
        m_sysdba->EnableConnect();
        m_ownerId = p->GetObjectId();
    }
    if (!p)
    {
        Msg_List msg(Msg_List::Error, SDBMSG_CATALOG_RESTART_FAILED);
        this->Abort (msg);
    }
    if (RegisterCatalogTables)
    {
        this->RegisterCatalogTables();
        const bool cConsiderResultTables = true;
        const bool cConsiderSYSDDTables  = true;
        Catalog_TableDescriptor authorization (*SQLMan_Context::GetContext(), m_catalogSchemaName, 
            SQLMan_Identifier("AUTHORIZATION", /*ASCIISource*/true), !cConsiderResultTables, !cConsiderSYSDDTables);
        if (authorization)
        {
            m_authorizationId = authorization.GetTableId();
            bd998GetIndexFileNoForTable (m_authorizationId, 1, *reinterpret_cast<tgg00_Surrogate*>(&m_authorizationIndexId));
        }
        Catalog_TableDescriptor classContainers (*SQLMan_Context::GetContext(), m_catalogSchemaName, 
            SQLMan_Identifier("CLASSCONTAINERS", /*ASCIISource*/true), !cConsiderResultTables, !cConsiderSYSDDTables);
        if (classContainers)
        {
            tgg00_Surrogate indexId = classContainers.GetTableId();
            bd998GetIndexFileNoForTable (indexId, 1, *reinterpret_cast<tgg00_Surrogate*>(&m_containersIndexId));
        }
        if (m_authorizationIndexId.IsNull() || m_containersIndexId.IsNull())
        {
            Msg_List msg(Msg_List::Error, SDBMSG_CATALOG_RESTART_FAILED);
            this->Abort (msg);
        }
    }
    m_tablesCreated = true;
    m_schemaCreated = true;
    return true;
}

//-------------------------------------------------------------------------------------------------

SAPDB_Bool Catalog_Instance::CreateFile(tgg00_TransContext& trans)
{
    tgg00_Surrogate sys1CatalogTabId;
    tgg00_Surrogate sys2CatalogTabId;

    FileDir_FileNo fileNo = FileDir_ISharedDirectory::Instance().GetNewFileNo();
    SAPDB_MemCopyNoCheck( &sys1CatalogTabId, &fileNo, sizeof(sys1CatalogTabId) );

    fileNo = FileDir_ISharedDirectory::Instance().GetNewFileNo();
    SAPDB_MemCopyNoCheck( &sys2CatalogTabId, &fileNo, sizeof(sys2CatalogTabId) );

    this->InitTreeId( sys1CatalogTabId, sys2CatalogTabId );
    
    tgg00_ExtendedFiletypeSet eFiletypeSet;
    eFiletypeSet.clear();

    bd01CreatePermTableFile( trans, m_sys1CatalogTreeId, eFiletypeSet, 0 );
    if ( trans.trError_gg00 != e_ok ) {
        return false;
    }

    bd01CreatePermTableFile( trans, m_sys2CatalogTreeId, eFiletypeSet, 0 );
    if ( trans.trError_gg00 != e_ok ) {
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

SAPDB_Bool Catalog_Instance::RestartCatalog(
    tgg00_TransContext&    trans,
    const tgg00_Surrogate& sys1CatalogTabId,
    const tgg00_Surrogate& sys2CatalogTabId)
{
    this->InitTreeId( sys1CatalogTabId, sys2CatalogTabId );

    b01filestate( trans, m_sys1CatalogTreeId );
    if ( trans.trError_gg00 != e_ok ) {
        return false;
    }

    b01filestate( trans, m_sys2CatalogTreeId );
    if ( trans.trError_gg00 != e_ok ) {
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------

const tgg00_Surrogate& Catalog_Instance::GetSys1CatalogTabId() const
{
    return m_sys1CatalogTreeId.fileTabId_gg00();
}

//-------------------------------------------------------------------------------------------------

const tgg00_Surrogate& Catalog_Instance::GetSys2CatalogTabId() const
{
    return m_sys2CatalogTreeId.fileTabId_gg00();
}

//-------------------------------------------------------------------------------------------------

const tgg00_FileId& Catalog_Instance::GetSys1CatalogTreeId() const
{
    return m_sys1CatalogTreeId;
}

//-------------------------------------------------------------------------------------------------

const tgg00_FileId& Catalog_Instance::GetSys2CatalogTreeId() const
{
    return m_sys2CatalogTreeId;
}

//-------------------------------------------------------------------------------------------------

SAPDB_Bool Catalog_Instance::IsSys1CatalogTable(
        const tgg00_Surrogate& tabId) const
{
    return tabId == m_sys1CatalogTreeId.fileTabId_gg00();
}

//-------------------------------------------------------------------------------------------------

SAPDB_Bool Catalog_Instance::IsSys2CatalogTable(
        const tgg00_Surrogate& tabId) const
{
    return tabId == m_sys2CatalogTreeId.fileTabId_gg00();
}

//-------------------------------------------------------------------------------------------------

SAPDB_Bool Catalog_Instance::IsCatalogTable(
        const tgg00_Surrogate& tabId) const
{
    return
        (tabId == m_sys1CatalogTreeId.fileTabId_gg00()) ||
        (tabId == m_sys2CatalogTreeId.fileTabId_gg00());
}

//-------------------------------------------------------------------------------------------------

void Catalog_Instance::SetSys1CatalogTreeId(const tgg00_FileId& treeId)
{
    m_sys1CatalogTreeId = treeId;
}

//-------------------------------------------------------------------------------------------------

void Catalog_Instance::SetSys2CatalogTreeId(const tgg00_FileId& treeId)
{
    m_sys2CatalogTreeId = treeId;
}

//-------------------------------------------------------------------------------------------------

void Catalog_Instance::InitTreeId(
    const tgg00_Surrogate& sys1CatalogTabId,
    const tgg00_Surrogate& sys2CatalogTabId)
{
    m_sys1CatalogTreeId                  = b01niltree_id;
    m_sys1CatalogTreeId.fileTabId_gg00() = sys1CatalogTabId;
    m_sys1CatalogTreeId.fileType_gg00()  = tgg00_FiletypeSet::build( 2, ftsPerm_egg00, ftsConcurrent_egg00 );
    m_sys1CatalogTreeId.fileTfn_gg00().becomes( tfnSys_egg00 );

    m_sys2CatalogTreeId                  = b01niltree_id;
    m_sys2CatalogTreeId.fileTabId_gg00() = sys2CatalogTabId;
    m_sys2CatalogTreeId.fileType_gg00()  = tgg00_FiletypeSet::build( 2, ftsPerm_egg00, ftsConcurrent_egg00 );
    m_sys2CatalogTreeId.fileTfn_gg00().becomes( tfnSys_egg00 );
}

//-------------------------------------------------------------------------------------------------

