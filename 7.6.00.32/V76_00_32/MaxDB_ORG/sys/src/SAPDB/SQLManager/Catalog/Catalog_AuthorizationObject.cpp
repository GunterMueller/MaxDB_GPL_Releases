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

#include "SAPDB/SQLManager/SQLMan_Tracing.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_RequestScope.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_AuthorizationObject.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Cache.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_ICallBack.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Interface.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Instance.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_IsoLevelScope.hpp"
#include "SQLManager/Catalog/Catalog_LockedSecondaryKeyScope.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Object.hpp"
#include "SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp"
#include "SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp" 
#include "SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp" 
#include "SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h"
#include "SAPDB/SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp"

#include "SAPDB/KernelCommon/Kernel_VTrace.hpp"

#include "hak01.h"
#include "hkb53.h"

//-------------------------------------------------------------------------------------------

static const bool c_accessViaName = true;

// converts a null terminated string to an identifier

inline static int MakeIdentifier (const SQLMan_Identifier& sourceIdentifier, SQLMan_Identifier& destIdentifier)
{
    int length = sizeof(sourceIdentifier);
    while (0 == sourceIdentifier[length-1])
    {
        --length;
        if (0 == length) 
        {
            break;
        }
    }
    SAPDB_MemCopyNoCheck (&destIdentifier[0], &sourceIdentifier[0], length);
    if (length < sizeof(destIdentifier))
    {
        SAPDB_MemCopyNoCheck (&destIdentifier[length], &Catalog_Instance::Instance().BlankIdentifier()[0], sizeof(destIdentifier) - length);
    }
    return length;
}

//-------------------------------------------------------------------------------------------

Catalog_AuthorizationObject::Catalog_AuthorizationObject()
: Catalog_Object(0,0)
{
}

//-------------------------------------------------------------------------------------------

Catalog_AuthorizationObject::Catalog_Key::Catalog_Key(const Catalog_ObjectId& objectId)
{
     SAPDBTRACE_METHOD_DEBUG ("Catalog_AuthorizationObject::Catalog_Key::Catalog_Key", Catalog_Trace, 1);   
     this->SetEntryType(TYPE_AUTHORIZATION);
     this->SetSurrogate(objectId);
     this->slinkage()[1] = 1;
}

//-------------------------------------------------------------------------------------------

Catalog_AuthorizationObject* Catalog_AuthorizationObject::CreateCopy(SAPDBMem_IRawAllocator& alloc) const
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_AuthorizationObject::Catalog_Key::Catalog_Key", Catalog_Trace, 1); 
    Catalog_AuthorizationObject* p = reinterpret_cast<Catalog_AuthorizationObject*>(alloc.Allocate(sizeof(this->suser())));
    if (p)
    {
        SAPDB_MemCopyNoCheck (p, &this->suser(), sizeof(this->suser())); 
    }
    return p;
}

//-------------------------------------------------------------------------------------------

void Catalog_AuthorizationObject::Delete(SQLMan_Context& context, const SQLMan_Identifier& name)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Catalog_AuthorizationObject::Delete", Catalog_Trace, 1); 
    if (context.IsOk())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        {
            SQLMan_SQLStatementContext newStatementScope(context);
            KSQL_Connection con;
            if (con.isConnected())
            {
                context.SetInDDLTrigger();
                KSQL_PreparedStatement stmt = con.createPreparedStatement();
                stmt.prepare("DELETE SYSCATALOG.AUTHORIZATION WHERE name = ?");
                SQLMan_Identifier auxName;
                MakeIdentifier (name, auxName);
                stmt.bind(1, auxName);
                rc = stmt.execute();
                if ((0 != rc) && (SQL_ROW_NOT_FOUND != rc))
                {
                    const char* pState;
                    const char* pMsg = 0;
                    stmt.describeLastError(pState, pMsg);
                    SAPDB_ToStringClass errCodeStr = ToStr(rc);
                    Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED,
                        Msg_OptArg("IFR_ERROR",errCodeStr),
                        Msg_OptArg("DESCRIPTION",(pMsg?pMsg:"")));
                    Msg_List& errList = context.GetErrorList();
                    errList.AppendNewMessage(msg);
                }
            }
        }
        if (0 != rc)
        {
            context.SetReturnCode(rc);
        }
    }
}

//-------------------------------------------------------------------------------------------

void Catalog_AuthorizationObject::EnableConnect()
{
    this->suser().userconnect = true;
}

//-------------------------------------------------------------------------------------------

void Catalog_AuthorizationObject::Fetch (KSQL_ResultSet& rs)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_AuthorizationObject::Fetch", Catalog_Trace, 1); 
    int colNo = 0;
    new((void*) &this->syskey()) Catalog_AuthorizationObject::Catalog_Key(Catalog_ObjectId());
    rs.getSurrogate(++colNo, this->suser().usurrogate);
    SAPDB_Int4 dummy;
    rs.getInt4 (++colNo, dummy);
    rs.getBytes(++colNo, &this->suser().userchardefcode, 1);
    rs.getBytes(++colNo, &this->suser().userroleinfo, 1);
    bool isUser;
    rs.getBool(++colNo, isUser);
    bool isUserGroup;
    rs.getBool(++colNo, isUserGroup);
    bool isRole;
    rs.getBool(++colNo, isRole);
    bool isSchema;
    rs.getBool(++colNo, isSchema);
    rs.getBytes(++colNo, &this->suser().userkind, 1);
    rs.getBool(++colNo, this->suser().usersysdba);
    rs.getBool(++colNo, this->suser().uexclusive);
    rs.getBool(++colNo, this->suser().ureplication);
    rs.getBytes(++colNo, &this->suser().userpassw, sizeof(this->suser().userpassw));
    rs.getBytes(++colNo, &this->suser().usupportpasspw, sizeof(this->suser().usupportpasspw));
    rs.getSurrogate(++colNo, this->suser().usergroup_id);
    rs.getSurrogate(++colNo, this->suser().userowner_id);
    Catalog_Timestamp createDate;
    rs.getAscii (++colNo, createDate, sizeof(createDate));
    Catalog_Timestamp pwAlterDate;
    rs.getAscii (++colNo, pwAlterDate, sizeof(pwAlterDate));
    Catalog_Timestamp alterDate;
    rs.getAscii (++colNo, alterDate, sizeof(alterDate));
    rs.getInt4(++colNo, this->suser().uparams[schemaid]);
    rs.getInt4(++colNo, this->suser().uparams[maxtimeout]);
    rs.getInt4(++colNo, this->suser().uparams[costwarning]);
    rs.getInt4(++colNo, this->suser().uparams[costlimit]);
    rs.getInt4(++colNo, this->suser().uparams[cachelimit]);
    SAPDB_Int4 roleCount;
    rs.getInt4(++colNo, roleCount);
    this->suser().userrolecnt = roleCount;
    rs.getBool(++colNo, this->suser().ucomment);
    this->suser().username = a01_il_b_identifier;
    this->suser().ureclen = sizeof(this->suser());
    createDate.ToInt  (this->suser().ucreatedate, this->suser().ucreatetime);
    pwAlterDate.ToInt (this->suser().upwcreatedate, this->suser().upwcreatetime);
    alterDate.ToInt   (this->suser().ualterdate, this->suser().ualtertime);
    this->suser().urecordtyp.clear();
    if (isUser) {
        this->suser().urecordtyp.addElement(is_user_rec);
    }
    if (isUserGroup) {
        this->suser().urecordtyp.addElement(is_group_rec);
    }
    if (isRole) {
        this->suser().urecordtyp.addElement(is_role_rec);
    }
    if (isSchema) {
        this->suser().urecordtyp.addElement(is_schema_rec);
    }
    rs.getIdentifier(++colNo, *REINTERPRET_CAST(SQLMan_Identifier*, &this->suser().username));
    SAPDB_Int4 nameLength;
    rs.getInt4 (++colNo, nameLength);
    while (nameLength < sizeof(this->suser().username))
    {
        this->suser().username[nameLength++] = 0;
    }
    rs.getBool(++colNo, this->suser().userconnect);
}                

//-------------------------------------------------------------------------------------------

void Catalog_AuthorizationObject::ForEach (SQLMan_Context& context, Catalog_ICallBack& callback, bool eachUser, bool eachSchema, bool eachRole)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Catalog_AuthorizationObject::ForEach", Catalog_Trace, 1); 
    if (context.IsOk())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        {
            KSQL_Connection con;
            if (con.isConnected())
            {
                KSQL_PreparedStatement stmt = con.createPreparedStatement();
                {
                    SQLMan_SQLStatementContext newStatementScope(context);
                    context.SetInDDLTrigger();
                    Catalog_IsoLevelScope isoScope (context, 15);
                    stmt.setCursorName("SYSCATALOG_CURSOR");
                    char sqlStmt[128];
                    strcpy (sqlStmt, "DECLARE SYSCATALOG_CURSOR CURSOR FOR SELECT * FROM SYSCATALOG.AUTHORIZATION WHERE ");
                    if (eachUser)
                    {
                        strcat (sqlStmt, "ISUSERGROUP");
                    }
                    if (eachSchema)
                    {
                        if (eachUser)
                        {
                            strcat (sqlStmt, " OR ");
                        }
                        strcat(sqlStmt, "ISSCHEMA");
                    }
                    if (eachRole)
                    {
                        if (eachUser || eachSchema)
                        {
                            strcat (sqlStmt, " OR ");
                        }
                        strcat(sqlStmt, "ISROLE");
                    }
                    stmt.prepare(sqlStmt);
                    rc = stmt.execute();
                }
                if ((0 != rc) && (SQL_ROW_NOT_FOUND != rc))
                {
                    const char* pState;
                    const char* pMsg = 0;
                    stmt.describeLastError(pState, pMsg);
                    Msg_List& errList = context.GetErrorList();
                    errList.AppendNewMessage(Msg_List(Msg_List::Error, SDBMSG_CATALOG_ACCESS_FAILED, Msg_OptArg("RETURNCODE", rc), Msg_OptArg("DESCRIPTION", pMsg?pMsg:"")));
                }
                else
                {
                    KSQL_ResultSet rs = stmt.getResultSet();
                    Catalog_UserObjectPascal authorizationObject;
                    Catalog_AuthorizationObject* p = reinterpret_cast<Catalog_AuthorizationObject*>(&authorizationObject);
                    do
                    {
                        {
                            SQLMan_SQLStatementContext newStatementScope(context);
                            context.SetInDDLTrigger();
                            rc = rs.next();
                            if (0 != rc)
                            {
                                rs.close();
                                stmt.release();
                                if (SQL_ROW_NOT_FOUND == rc) 
                                {
                                    rc = 0;
                                }
                                break;
                            }
                            p->Fetch(rs);
                            if (!rs.isOK())
                            {
                                rs.close();
                                stmt.release();
                                rc = SQLManErr_Interface::GetInstance().GetReturnCode(e_internal_error, sqlm_internal);
                                break;
                            }
                        }
                        if (!callback.Callback(p)) 
                        {
                            SQLMan_SQLStatementContext newStatementScope(context);
                            context.SetInDDLTrigger();
                            rs.close();
                            stmt.release();
                            break;
                        }
                    }
                    while (true);
                }
            }
        }
        if (0 != rc)
        {
            context.SetReturnCode(rc);
        }
    }
}

//-------------------------------------------------------------------------------------------

Catalog_AuthorizationObject::IncopyResult Catalog_AuthorizationObject::Incopy (SQLMan_Context& context, const SQLMan_Identifier& name, bool useCache)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_AuthorizationObject::Incopy(1)", Catalog_Trace, 1); 
    bool isCopied;
    if (useCache && (Catalog_IAuthorization::GetInstance().IncopySpecialAuthorizationObject(context, name, *this, isCopied)))
    {
       return icp_ok;
    }
    else
    {
        char stmt[200];
        SQLMan_Identifier auxName;
        int length = MakeIdentifier (name, auxName);
        SAPDB_ToStringClass lengthStr = ToStr(length);
        strcpy (stmt, "SELECT * INTO ?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,? FROM"
            " SYSCATALOG.AUTHORIZATION WHERE "); 
        strcat (stmt, lengthStr);
        strcat (stmt, " =  NAMELENGTH AND NAME = ?");
        return this->Incopy (context, stmt, c_accessViaName, Catalog_ObjectId(), auxName, -1);
    }
}

//-------------------------------------------------------------------------------------------

Catalog_AuthorizationObject::IncopyResult Catalog_AuthorizationObject::Incopy (SQLMan_Context& context, const Catalog_ObjectId& authId)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_AuthorizationObject::Incopy(2)", Catalog_Trace, 1); 
    bool isCopied;
    if (Catalog_IAuthorization::GetInstance().IncopySpecialAuthorizationObject(context, authId, *this, isCopied))
    {
       return icp_ok;
    }
    else
    {
        char stmt[200];
        strcpy (stmt, "SELECT * INTO ?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,? FROM"
            " SYSCATALOG.AUTHORIZATION WHERE AUTHORIZATIONID = ?");
        return this->Incopy (context, stmt, !c_accessViaName, authId, SQLMan_Identifier(), -1);
    }
}

//-------------------------------------------------------------------------------------------

Catalog_AuthorizationObject::IncopyResult Catalog_AuthorizationObject::Incopy (SQLMan_Context& context, SAPDB_Int4 schemaId, Catalog_IAuthorization::LockRequest lockRequest)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_AuthorizationObject::Catalog_Key::Catalog_Key", Catalog_Trace, 1); 
    char stmt[200];
    strcpy (stmt, "SELECT * INTO ?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,? FROM"
        " SYSCATALOG.AUTHORIZATION WHERE SCHEMAID = ?");
    if (Catalog_IAuthorization::LOCK_NONE != lockRequest)
    {
        strcat (stmt, " WITH LOCK");
        if (Catalog_IAuthorization::LOCK_EXCLUSIVE == lockRequest)
        {
            strcat (stmt, " EXCLUSIVE");
        }
    }
    return this->Incopy (context, stmt, !c_accessViaName, Catalog_ObjectId(), SQLMan_Identifier(), schemaId);
}

//-------------------------------------------------------------------------------------------

Catalog_AuthorizationObject::IncopyResult Catalog_AuthorizationObject::Incopy (SQLMan_Context&          context, 
                                          const char*              sqlStmt, 
                                          bool                     viaName,
                                          const Catalog_ObjectId&  authid,
                                          const SQLMan_Identifier& name,
                                          SAPDB_Int4               schemaId)
{
    Catalog_RequestScope requestScope(context);
    if (context.IsOk() && Catalog_Interface::GetInstance().ExistCatalogTables())
    {
        Kernel_VTrace vTrace;
        if (!Catalog_Trace.TracesLevel(1) && requestScope.AnyVtrace())
        {
            vTrace << "CATALOG ACCESS : Authorization " << FlushLine;
            if (schemaId > 0)
            {
               vTrace << "Schema Id      : " << schemaId << FlushLine;
            }
            else
            {
                if (viaName)
                {
                    vTrace << "Required Name  : " << name << FlushLine;
                }
                else
                {
                    vTrace << "Required OID   : " << authid << FlushLine;
                }
            }
        }
        KSQL_PreparedStatement::SQLCode rc = 0;
        const char* pState;
        const char* pMsg = 0;
        {
            SQLMan_SQLStatementContext newStatementScope(context);
            if (!context.IsOk())
            {
                rc = context.ReturnCode();
            }
            else
            {
                KSQL_Connection con;
                if (con.isConnected())
                {
                    context.SetInDDLTrigger();
                    // initialize the object key
                    new((void*) &syskey()) Catalog_AuthorizationObject::Catalog_Key(Catalog_ObjectId());
                    KSQL_PreparedStatement stmt = con.createPreparedStatement();
                    
                    Catalog_IsoLevelScope isoScope (context, 0);
                    Catalog_LockedSecondaryKeyScope secondaryKeyLockedScope(context);
                    if (viaName)
                    {
                        Catalog_Instance& catalog = Catalog_Instance::Instance();
                        SAPDB_Byte definedByte = (catalog.IsUnicodeCatalog()) ? csp_unicode_def_byte : ' ';
                        rc = secondaryKeyLockedScope.Lock (catalog.GetAuthorizationIndexId(),
                            &name[0], name.GetLength(), definedByte);
                    }
                    bool              isUser;
                    bool              isUserGroup;
                    bool              isRole;
                    bool              isSchema;
                    SAPDB_Int4        nameLength;
                    Catalog_Timestamp createDate;
                    Catalog_Timestamp pwAlterDate;
                    Catalog_Timestamp alterDate;
                    if (0 == rc)
                    {
                        stmt.prepare(sqlStmt);
                        int colNo = 0;
                        stmt.bind(++colNo, this->suser().usurrogate);
                        SAPDB_Int4 dummy;
                        stmt.bind(++colNo, dummy);
                        stmt.bind(++colNo, &this->suser().userchardefcode, 1);
                        stmt.bind(++colNo, &this->suser().userroleinfo, 1);
                        stmt.bind(++colNo, isUser);
                        stmt.bind(++colNo, isUserGroup);
                        stmt.bind(++colNo, isRole);
                        stmt.bind(++colNo, isSchema);
                        stmt.bind(++colNo, &this->suser().userkind, 1);
                        stmt.bind(++colNo, this->suser().usersysdba);
                        stmt.bind(++colNo, this->suser().uexclusive);
                        stmt.bind(++colNo, this->suser().ureplication);
                        stmt.bind(++colNo, &this->suser().userpassw, sizeof(this->suser().userpassw));
                        stmt.bind(++colNo, &this->suser().usupportpasspw, sizeof(this->suser().usupportpasspw));
                        stmt.bind(++colNo, this->suser().usergroup_id);
                        stmt.bind(++colNo, this->suser().userowner_id);
                        IFR_Length length = sizeof(createDate);
                        stmt.bindAsciiOutput (++colNo, createDate, &length);
                        stmt.bindAsciiOutput (++colNo, pwAlterDate, &length);
                        stmt.bindAsciiOutput (++colNo, alterDate, &length);
                        stmt.bind(++colNo, this->suser().uparams[schemaid]);
                        stmt.bind(++colNo, this->suser().uparams[maxtimeout]);
                        stmt.bind(++colNo, this->suser().uparams[costwarning]);
                        stmt.bind(++colNo, this->suser().uparams[costlimit]);
                        stmt.bind(++colNo, this->suser().uparams[cachelimit]);
                        stmt.bind(++colNo, this->suser().userrolecnt);
                        stmt.bind(++colNo, this->suser().ucomment);
                        this->suser().username = a01_il_b_identifier; 
                        stmt.bind(++colNo, *REINTERPRET_CAST(SQLMan_Identifier*, &this->suser().username));
                        stmt.bind(++colNo, nameLength);
                        stmt.bind(++colNo, this->suser().userconnect);
                        if (schemaId > -1)
                        {
                            stmt.bind(++colNo, schemaId);
                        }
                        else
                        {
                            if (!viaName)
                            {
                                stmt.bind(++colNo, *CONST_CAST(Catalog_ObjectId*, &authid));
                            }
                            else
                            {
                                stmt.bind(++colNo, *CONST_CAST(SQLMan_Identifier*, &name));
                            }
                        }
                        rc = stmt.execute();
                    }
                    if (0 != rc)
                    {
                        if (!Catalog_Trace.TracesLevel(1) && requestScope.AnyVtrace())
                        {
                            vTrace << "CATALOG ACCESS ERROR : " << rc << FlushLine;
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
                        Msg_List& errList = context.GetErrorList();
                        errList.AppendNewMessage(msg);
                    }
                    else
                    {
                        if (!Catalog_Trace.TracesLevel(1) && requestScope.AnyVtrace())
                        {
                            vTrace << "CATALOG ACCESS RESULT" << FlushLine;
                            if (viaName)
                            {
                                vTrace << "Found OID      : " << Catalog_ObjectId(this->suser().usurrogate) << FlushLine;
                            }
                            else
                            {
                                vTrace << "Found Name     : " << SQLMan_Identifier(this->suser().username) << FlushLine; 
                            }
                        }
                        suser().ureclen = sizeof(suser());
                        createDate.ToInt  (this->suser().ucreatedate, this->suser().ucreatetime);
                        pwAlterDate.ToInt (this->suser().upwcreatedate, this->suser().upwcreatetime);
                        alterDate.ToInt   (this->suser().ualterdate, this->suser().ualtertime);
                        this->suser().urecordtyp.clear();
                        if (isUser) {
                            this->suser().urecordtyp.addElement(is_user_rec);
                        }
                        if (isUserGroup) {
                            this->suser().urecordtyp.addElement(is_group_rec);
                        }
                        if (isRole) {
                            this->suser().urecordtyp.addElement(is_role_rec);
                        }
                        if (isSchema) {
                            this->suser().urecordtyp.addElement(is_schema_rec);
                        }
                        while (nameLength < sizeof(this->suser().username))
                        {
                            this->suser().username[nameLength++] = 0;
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
            context.SetReturnCode(rc);
            return icp_error;
        }
    }
    return icp_not_found;
}

//-------------------------------------------------------------------------------------------

Catalog_Object* Catalog_AuthorizationObject::Incopy(SQLMan_Context& Context, const Catalog_ObjectId& AuthId, SAPDBMem_IRawAllocator& Allocator)
{
    Catalog_AuthorizationObject* pObj = reinterpret_cast<Catalog_AuthorizationObject*>(Allocator.Allocate(SizeOf()));
    if (pObj)
    {
        IncopyResult res = pObj->Incopy(Context, AuthId);
        if (icp_ok != res)
        {
            Allocator.Deallocate(pObj);
            pObj = 0;
        }
    }
    else
    {
        Context.SetNoMoreMemoryError(Msg_List(Msg_List::Error, SDBMSG_CATALOG_NO_MEMORY, Msg_OptArg("MEMREQUEST",SizeOf())));
    }
    return pObj;
}

//-------------------------------------------------------------------------------------------

void Catalog_AuthorizationObject::Lock(SQLMan_Context&          Context, 
                                       const Catalog_ObjectId&  ObjectId,
                                       bool                     LockExclusive) 
{
    Catalog_Instance& catalog             = Catalog_Instance::Instance();
    Catalog_ObjectId authorizationTableId = catalog.GetAuthorizationId();

    tgg00_BasisError& e = Context.TransContext().trError_gg00;

    e = e_ok;
    tgg00_Lkey key; 
    key.keyVal_gg00()[0] = 0x00;
    SAPDB_RangeMove (__FILE__, 1, sizeof(ObjectId), sizeof (key.keyVal_gg00()), &ObjectId, 1,
        &key.keyVal_gg00(), 2, sizeof(ObjectId), e);
    if (e_ok == e)
    {
        key.keyLen_gg00() = 1 + sizeof(ObjectId);
        tgg00_LockReqState reqState;
        tgg00_LockReqMode  grantedMode;
        const bool noWait        = false;
        const bool collisionTest = false;
        tgg00_LockReqMode_Param wantedMode = (LockExclusive) ? lckRowExcl_egg00 : lckRowShare_egg00;
        reqState.clear();
        k53lock (Context.TransContext(), authorizationTableId,
            key, wantedMode, reqState, noWait, collisionTest, grantedMode);
        if (e_wait_for_lock_release == e)
        {
            k53wait (Context.TransContext(), m_lock, mm_nil);
        }
    }
    if (e_ok != e)
    {
        Context.ThrowError (e);
    }
}

//-------------------------------------------------------------------------------------------


bool Catalog_AuthorizationObject::Next (SQLMan_Context& context, 
                                        const Catalog_ObjectId& objectId,
                                        bool groupUsers,
                                        bool nonGroupUsers)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_AuthorizationObject::Next", Catalog_Trace, 1); 
    bool nextFound = false;
    if (context.IsOk())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        const char* pState;
        const char* pMsg = 0;
        KSQL_Connection con;
        if (con.isConnected())
        {
            SQLMan_SQLStatementContext newStatementScope(context);
            KSQL_PreparedStatement stmt = con.createPreparedStatement();
            context.SetInDDLTrigger();
            stmt.setCursorName("SYSCATALOG_CURSOR");
            char sqlStmt[150];
            strcpy (sqlStmt, "DECLARE SYSCATALOG_CURSOR CURSOR FOR SELECT * FROM SYSCATALOG.AUTHORIZATION WHERE AUTHORIZATIONID > ? AND ROWNO < 2");
            if (groupUsers)
            {
                strcat (sqlStmt, " AND ISUSERGROUP ");
            }
            if (nonGroupUsers)
            {
                strcat (sqlStmt, " AND  ISUSER");
            }
            Catalog_IsoLevelScope isoScope (context, 0);
            stmt.prepare(sqlStmt);
            stmt.bind(1, *CONST_CAST(Catalog_ObjectId*, &objectId));
            rc = stmt.execute();
            if ((0 != rc) && (SQL_ROW_NOT_FOUND != rc))
            {
                stmt.describeLastError(pState, pMsg);
                Msg_List& errList = context.GetErrorList();
                errList.AppendNewMessage(Msg_List(Msg_List::Error, SDBMSG_CATALOG_ACCESS_FAILED, Msg_OptArg("RETURNCODE", rc), Msg_OptArg("DESCRIPTION", pMsg?pMsg:"")));
            }
            else
            {
                KSQL_ResultSet rs = stmt.getResultSet();
                rc = rs.next();
                if (0 == rc)
                {
                    this->Fetch(rs);
                    nextFound = rs.isOK();
                    if (!nextFound)
                    {
                        rc = SQLManErr_Interface::GetInstance().GetReturnCode(e_internal_error, sqlm_internal);
                    }
                }
                else
                {
                    if (SQL_ROW_NOT_FOUND == rc) 
                    {
                        rc = 0;
                    }
                }
                rs.close();
                stmt.release();
            }
        }
        if (0 != rc)
        {
            context.SetReturnCode(rc);
        }
    }
    return nextFound;
}

//-------------------------------------------------------------------------------------------

void Catalog_AuthorizationObject::Outcopy (SQLMan_Context& context, bool isUpdate)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_AuthorizationObject::Outcopy", Catalog_Trace, 1);
    SAPDBTRACE_WRITELN( Catalog_Trace, 5, "ok : " << context.IsOk() << " tables exist : " <<  Catalog_Interface::GetInstance().ExistCatalogTables());
    if (context.IsOk() && Catalog_Interface::GetInstance().ExistCatalogTables())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        {
            SQLMan_SQLStatementContext newStatementScope(context);
            KSQL_Connection con;
            if (con.isConnected())
            {
                SAPDBTRACE_WRITELN( Catalog_Trace, 5, "connected");
                context.SetInDDLTrigger();
                KSQL_PreparedStatement stmt = con.createPreparedStatement();
                if (isUpdate)
                {
                    stmt.prepare("UPDATE SYSCATALOG.AUTHORIZATION SET "
                        "authorizationid = ?, userid = ?, defaultcode = ?, roleinfo = ?, isuser = ?,"
                        "isusergroup = ?, isrole = ?, isschema = ?, userkind = ?, issysdba = ?,"
                        "isexclusive = ?, isreplication = ?, password = ?, supportpassword = ?,"
                        "groupid = ?, ownerid = ?, createdate = ?,  passwordchangedate = ?,"
                        "alterdate = ?, schemaid = ?, timeout = ?, costwarning = ?, costlimit = ?, cachelimit = ?,"
                        "rolecount = ?, hascomment = ?, name = ?, namelength = ?, connectenabled = ? where authorizationid = ?");
                  }
                else
                {
                    stmt.prepare("INSERT INTO SYSCATALOG.AUTHORIZATION VALUES "
                        "(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?, ?, ?, ?,"
                        " ?,?,?,?,?,?,?,?,?,?)");
                }
                int colNo = 0;
                stmt.bind(++colNo, this->suser().usurrogate);
                SAPDB_Int4 userId = 
                   this->suser().usurrogate[7] +
                   this->suser().usurrogate[6] * 256 +
                   this->suser().usurrogate[5] * 256 * 256 +
                   this->suser().usurrogate[4] * 256 * 256 * 256;
                stmt.bind(++colNo, userId);
                stmt.bind(++colNo, &this->suser().userchardefcode, 1);
                stmt.bind(++colNo, &this->suser().userroleinfo, 1);
                bool isUser = this->suser().urecordtyp.includes(is_user_rec);
                stmt.bind(++colNo, isUser);
                bool isUserGroup = this->suser().urecordtyp.includes(is_group_rec);
                stmt.bind(++colNo, isUserGroup);
                bool isRole = this->suser().urecordtyp.includes(is_role_rec);
                stmt.bind(++colNo, isRole);
                bool isSchema = this->suser().urecordtyp.includes(is_schema_rec);
                stmt.bind(++colNo, isSchema);
                stmt.bind(++colNo, &this->suser().userkind, 1);
                stmt.bind(++colNo, this->suser().usersysdba);
                stmt.bind(++colNo, this->suser().uexclusive);
                stmt.bind(++colNo, this->suser().ureplication);
                stmt.bind(++colNo, &this->suser().userpassw, sizeof(this->suser().userpassw));
                stmt.bind(++colNo, &this->suser().usupportpasspw, sizeof(this->suser().usupportpasspw));
                stmt.bind(++colNo, this->suser().usergroup_id);
                stmt.bind(++colNo, this->suser().userowner_id);
                Catalog_Timestamp createDate (this->suser().ucreatedate, this->suser().ucreatetime);
                stmt.bindAsciiInput (++colNo, createDate, sizeof(createDate));
                Catalog_Timestamp pwCreateDate (this->suser().upwcreatedate, this->suser().upwcreatetime);
                stmt.bindAsciiInput (++colNo, pwCreateDate, sizeof(pwCreateDate));
                Catalog_Timestamp alterDate (this->suser().ualterdate, this->suser().ualtertime);
                stmt.bindAsciiInput (++colNo, alterDate, sizeof(alterDate));
                stmt.bind(++colNo, this->suser().uparams[schemaid]);
                stmt.bind(++colNo, this->suser().uparams[maxtimeout]);
                stmt.bind(++colNo, this->suser().uparams[costwarning]);
                stmt.bind(++colNo, this->suser().uparams[costlimit]);
                stmt.bind(++colNo, this->suser().uparams[cachelimit]);
                stmt.bind(++colNo, this->suser().userrolecnt);
                stmt.bind(++colNo, this->suser().ucomment);
                SAPDB_Int4 nameLength = sizeof(this->suser().username);
                if (Catalog_Instance::Instance().OmsSchemaName() == this->suser().username)
                {
                    // oms schema name is stored with variable length
                    while (
                        (0 == this->suser().username[nameLength-1]) ||
                        (' ' == this->suser().username[nameLength-1])
                        ) 
                    {
                        --nameLength;
                    }
                }
                else
                {
                    while (0 == this->suser().username[nameLength-1])
                    {
                        --nameLength;
                    }
                }
                SQLMan_Identifier username(this->suser().username);
                if (nameLength < sizeof(username))
                {
                    SAPDB_MemCopyNoCheck (&username[nameLength], &Catalog_Instance::Instance().BlankIdentifier()[0], sizeof(username) - nameLength);
                }
                stmt.bind(++colNo, username);
                stmt.bind(++colNo, nameLength);
                stmt.bind(++colNo, this->suser().userconnect);
                if (isUpdate)
                {
                    stmt.bind(++colNo, this->suser().usurrogate);
                }
                rc = stmt.execute();
                if ((0 != rc) && (SQL_ROW_NOT_FOUND != rc))
                {
                    const char* pState;
                    const char* pMsg = 0;
                    stmt.describeLastError(pState, pMsg);
                    SAPDB_ToStringClass errCodeStr = ToStr(rc);
                    Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED,
                        Msg_OptArg("IFR_ERROR",errCodeStr),
                        Msg_OptArg("DESCRIPTION",(pMsg?pMsg:"")));
                    Msg_List& errList = context.GetErrorList();
                    errList.AppendNewMessage(msg);
                }
            }
        }
        if (0 != rc)
        {
            context.SetReturnCode(rc);
        }
    }
}

//-------------------------------------------------------------------------------------------

void Catalog_AuthorizationObject::Rename (SQLMan_Context& context, const SQLMan_Identifier& oldName, const SQLMan_Identifier& newName)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Catalog_AuthorizationObject::Rename", Catalog_Trace, 1); 
    if (context.IsOk())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        {
            SQLMan_SQLStatementContext newStatementScope(context);
            KSQL_Connection con;
            if (con.isConnected())
            {
                context.SetInDDLTrigger();
                KSQL_PreparedStatement stmt = con.createPreparedStatement();
                stmt.prepare("UPDATE SYSCATALOG.AUTHORIZATION SET name = ? WHERE name = ?");
                SQLMan_Identifier auxNewName;
                MakeIdentifier (newName, auxNewName);
                stmt.bind(1, auxNewName);
                SQLMan_Identifier auxOldName;
                MakeIdentifier (oldName, auxOldName);
                stmt.bind(2, auxOldName);
                rc = stmt.execute();
                if ((0 != rc) && (SQL_ROW_NOT_FOUND != rc))
                {
                    const char* pState;
                    const char* pMsg = 0;
                    stmt.describeLastError(pState, pMsg);
                    SAPDB_ToStringClass errCodeStr = ToStr(rc);
                    Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED,
                        Msg_OptArg("IFR_ERROR",errCodeStr),
                        Msg_OptArg("DESCRIPTION",(pMsg?pMsg:"")));
                    Msg_List& errList = context.GetErrorList();
                    errList.AppendNewMessage(msg);
                }
            }
        }
        if (SQL_DUPLICATE_SECONDARY_KEY == rc)
        {
            context.ThrowError (e_duplicate_name);
            return;
        }
        if (0 != rc)
        {
            context.SetReturnCode(rc);
        }
    }
}

//-------------------------------------------------------------------------------------------

int Catalog_AuthorizationObject::SizeOf()
{
	return sizeof(Catalog_UserObjectPascal);
}

//-------------------------------------------------------------------------------------------

void Catalog_AuthorizationObject::StoreControlUser(SQLMan_Context& context)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Catalog_AuthorizationObject::StoreControlUser", Catalog_Trace, 1); 
    Catalog_UserObjectPascal controlUser;
    SAPDB_MemFillNoCheck (&controlUser, 0, sizeof(controlUser));
    controlUser.usurrogate[0]   = 255;
    controlUser.usurrogate[1]   = 255;
    controlUser.userowner_id    = controlUser.usurrogate;
    controlUser.userchardefcode = csp_instance_code;
    controlUser.userroleinfo.becomes(role_none);
    controlUser.urecordtyp.clear();
    controlUser.urecordtyp.addElement(is_user_rec);
    controlUser.urecordtyp.addElement(is_group_rec);
    controlUser.userkind.becomes(ucontroluser);
    Catalog_Timestamp ts(0,0);
    ts.ToInt (controlUser.upwcreatedate, controlUser.upwcreatetime);
    controlUser.ucreatedate    = controlUser.upwcreatedate;
    controlUser.ucreatetime    = controlUser.upwcreatetime;
    controlUser.ualterdate     = controlUser.upwcreatedate;
    controlUser.ualtertime     = controlUser.upwcreatetime;
    controlUser.username       = a01controluser;
    controlUser.userconnect    = true;
    for (int ix = schemaid; ix <= cachelimit; ++ix)
    {
        controlUser.uparams[ix] = csp_maxint4; 
    }
    const bool c_isUpdate      = true;
    (reinterpret_cast<Catalog_AuthorizationObject*>(&controlUser))->Outcopy(context, !c_isUpdate);
}

//-------------------------------------------------------------------------------------------

void Catalog_AuthorizationObject::UpdateGroupMembers (SQLMan_Context& context, Catalog_AuthorizationObject* pGroup)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Catalog_AuthorizationObject::UpdatePasswords", Catalog_Trace, 1);
    if (context.IsOk())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        const char* pState;
        const char* pMsg = 0;
        {
            SQLMan_SQLStatementContext newStatementScope(context);
            if (!context.IsOk())
            {
                rc = context.ReturnCode();
            }
            else
            {
                KSQL_Connection con;
                if (con.isConnected())
                {
                    context.SetInDDLTrigger();
                    KSQL_PreparedStatement stmt = con.createPreparedStatement();
                    stmt.prepare("UPDATE SYSCATALOG.AUTHORIZATION SET "
                        "userkind = ?, ownerid = ?, isexclusive = ?, defaultcode = ?,"
                        "schemaid = ?, timeout = ?, costwarning = ?, costlimit = ?, cachelimit = ?, connectenabled = ?, "
                        "roleinfo = ?, rolecount = ? where isuser and groupid = ?");
                    int colNo = 0;
                    stmt.bind(++colNo, &pGroup->suser().userkind, 1);
                    stmt.bind(++colNo, pGroup->suser().userowner_id);      
                    stmt.bind(++colNo, pGroup->suser().uexclusive);
                    stmt.bind(++colNo, &pGroup->suser().userchardefcode, 1);
                    stmt.bind(++colNo, pGroup->suser().uparams[schemaid]);
                    stmt.bind(++colNo, pGroup->suser().uparams[maxtimeout]);
                    stmt.bind(++colNo, pGroup->suser().uparams[costwarning]);
                    stmt.bind(++colNo, pGroup->suser().uparams[costlimit]);
                    stmt.bind(++colNo, pGroup->suser().uparams[cachelimit]);
                    stmt.bind(++colNo, pGroup->suser().userconnect);
                    stmt.bind(++colNo, &pGroup->suser().userroleinfo, 1);
                    stmt.bind(++colNo, pGroup->suser().userrolecnt);
                    stmt.bind(++colNo, pGroup->suser().usurrogate);
                    rc = stmt.execute();
                    if (0 != rc)
                    {
                        if (SQL_ROW_NOT_FOUND == rc)
                        {
                            return;
                        }
                        stmt.describeLastError(pState, pMsg);
                        SAPDB_ToStringClass errCodeStr = ToStr(rc);
                        Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED,
	                                    Msg_OptArg("IFR_ERROR",errCodeStr),
                                        Msg_OptArg("DESCRIPTION",(pMsg?pMsg:"")));
                        Msg_List& errList = context.GetErrorList();
                        errList.AppendNewMessage(msg);
                    }
                }
            }
        }
        if (0 != rc)
        {
            context.SetReturnCode(rc);
        }
    }
}

//-------------------------------------------------------------------------------------------

void Catalog_AuthorizationObject::UpdatePasswords (SQLMan_Context& context)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_AuthorizationObject::UpdatePasswords", Catalog_Trace, 1);
    if (context.IsOk())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        const char* pState;
        const char* pMsg = 0;
        KSQL_Connection con;
        if (con.isConnected())
        {
            context.SetInDDLTrigger();
            KSQL_PreparedStatement stmt = con.createPreparedStatement();
            stmt.prepare("UPDATE SYSCATALOG.AUTHORIZATION SET "
                "password = ?, supportpassword = ? where authorizationid = ?");
            stmt.bind(1, &this->suser().userpassw, sizeof(this->suser().userpassw));
            stmt.bind(2, &this->suser().usupportpasspw, sizeof(this->suser().usupportpasspw));
            stmt.bind(3, this->suser().usurrogate);
            rc = stmt.execute();
            if (0 != rc)
            {
                if (SQL_ROW_NOT_FOUND != rc)
                {
                    stmt.describeLastError(pState, pMsg);
                }
                SAPDB_ToStringClass errCodeStr = ToStr(rc);
                Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED,
	                            Msg_OptArg("IFR_ERROR",errCodeStr),
                                Msg_OptArg("DESCRIPTION",(pMsg?pMsg:"")));
                Msg_List& errList = context.GetErrorList();
                errList.AppendNewMessage(msg);
                context.SetReturnCode(rc);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void Catalog_AuthorizationObject::UpdateUserOwner (SQLMan_Context&         context, 
                                                   const Catalog_ObjectId& oldOwnerId, 
                                                   const Catalog_ObjectId& newOwnerId,
                                                   bool                    GroupsOnly)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Catalog_AuthorizationObject::UpdateUserOwner", Catalog_Trace, 1); 
    if (context.IsOk() && Catalog_Interface::GetInstance().ExistCatalogTables())
    {
        KSQL_PreparedStatement::SQLCode rc = 0;
        const char* pState;
        const char* pMsg = 0;
        {
            SQLMan_SQLStatementContext newStatementScope(context);
            KSQL_Connection con;
            if (con.isConnected())
            {
                context.SetInDDLTrigger();
                KSQL_PreparedStatement stmt = con.createPreparedStatement();
                if (GroupsOnly)
                {
                    stmt.prepare("UPDATE SYSCATALOG.AUTHORIZATION SET OWNERID = ? WHERE "
                        "((ISUSERGROUP AND NOT ISUSER) OR (NOT ISUSERGROUP AND ISUSER)) AND OWNERID = ?");
                }
                else
                {
                    stmt.prepare("UPDATE SYSCATALOG.AUTHORIZATION SET OWNERID = ? WHERE "
                        "(ISUSER OR ISUSERGROUP) AND OWNERID = ?");
                }
                int colNo = 0;
                Catalog_ObjectId oldId(oldOwnerId);
                Catalog_ObjectId newId(newOwnerId);
                stmt.bind(++colNo, newId);
                stmt.bind(++colNo, oldId);
                rc = stmt.execute();
                if (SQL_ROW_NOT_FOUND == rc)
                {
                    rc = 0;
                }
                else
                {
                    if (0 != rc) 
                    {
                        stmt.describeLastError(pState, pMsg);
                    }
                }
            }
        }
        if (0 != rc)
        {
            SAPDB_ToStringClass errCodeStr = ToStr(rc);
            Msg_List msg(   Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED,
	                        Msg_OptArg("IFR_ERROR",errCodeStr),
                            Msg_OptArg("DESCRIPTION",(pMsg?pMsg:"")));
            Msg_List& errList = context.GetErrorList();
            errList.AppendNewMessage(msg);
            context.SetReturnCode(rc);
        }
    }
}