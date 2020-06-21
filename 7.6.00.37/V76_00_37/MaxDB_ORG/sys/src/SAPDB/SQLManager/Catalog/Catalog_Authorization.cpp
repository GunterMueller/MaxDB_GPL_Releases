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

#include "SQLManager/Catalog/Catalog_Instance.hpp"
#include "SQLManager/Catalog/Catalog_Authorization.hpp"
#include "SQLManager/Catalog/Catalog_AuthorizationObject.hpp"
#include "SQLManager/Catalog/Catalog_SchemaObject.hpp"
#include "SQLManager/Catalog/Catalog_UserObject.hpp"
//#include "SQLManager/Catalog/Catalog_Messages.hpp"
#include "SQLManager/KernelSQL/KSQL_Connection.hpp" 
#include "SQLManager/KernelSQL/KSQL_PreparedStatement.hpp" 
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SQLManager/SQLMan_SQLStatementContext.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDB/SAPDBCommon/Messages/SDBMsg_Catalog.h"

#include "hak01.h"


/*!
  @brief class providing methods for implementing schema privileges 
 */
class Catalog_SchemaGrantRevoke
{
public :
    /*!
       @brief constructor
     */
    Catalog_SchemaGrantRevoke(SQLMan_Context& context,
        bool isGrant,
        bool changeAlterIn,
        bool changeCreateIn,
        bool changeDropIn);
    /*!
       @brief destructor
     */
    ~Catalog_SchemaGrantRevoke();
    /*!
       @brief inserts a schema privilege into the corresponding catalog table
     */
    int InsertSchemaPrivilege (const Catalog_ObjectId& schemaIdId,  const Catalog_ObjectId& grantee);
    /*!
       @brief inserts or updates a schema privilege into the corresponding catalog table
     */
    int StoreSchemaPrivilege (const Catalog_ObjectId& schemaIdId,  const Catalog_ObjectId& grantee);
private :
    SQLMan_Context&         m_context;
    KSQL_Connection         m_con;
    bool                    m_isGrant;
    bool                    m_changeAlterIn;
    bool                    m_changeCreateIn;
    bool                    m_changeDropIn;
    int                     m_privCount;
    bool                    m_insertPrepared;
    KSQL_PreparedStatement  m_insertStatement;
    KSQL_PreparedStatement  m_updateStatement;
};

Catalog_SchemaGrantRevoke::Catalog_SchemaGrantRevoke(
    SQLMan_Context& context,
    bool isGrant,
    bool changeAlterIn,
    bool changeCreateIn,
    bool changeDropIn)
    : m_context(context)
    , m_con()
    , m_isGrant(isGrant)
    , m_changeAlterIn(changeAlterIn)
    , m_changeCreateIn(changeCreateIn)
    , m_changeDropIn(changeDropIn)
    , m_privCount(0)
    , m_insertPrepared(false)
    , m_insertStatement(m_con.createPreparedStatement())
    , m_updateStatement(m_con.createPreparedStatement())
{
    if (m_con.isConnected())
    {
        m_context.SetInDDLTrigger();
        char updateStatement[150];
        strcpy (updateStatement, "UPDATE SYSCATALOG.SCHEMAPRIVILEGES SET ");
        if (changeAlterIn)
        {
            ++m_privCount;
            m_updateStatement.bind(m_privCount, m_isGrant);
            strcat (updateStatement, "ALTERIN = ?,");
        }
        if (changeCreateIn)
        {
            ++m_privCount;
            m_updateStatement.bind(m_privCount, m_isGrant);
            strcat (updateStatement, "CREATEIN = ?,");
        }
        if (changeDropIn)
        {
            ++m_privCount;
            m_updateStatement.bind(m_privCount, m_isGrant);
            strcat (updateStatement, "DROPIN = ?,");
        }
        updateStatement[strlen(updateStatement)-1] = ' ';
        strcat (updateStatement, "WHERE SCHEMAID = ? AND GRANTEE = ?");
        m_updateStatement.prepare(updateStatement);
    }
}

//-------------------------------------------------------------------------------------------------------------

Catalog_SchemaGrantRevoke::~Catalog_SchemaGrantRevoke()
{
    m_context.SetInDDLTrigger(false);
}

//-------------------------------------------------------------------------------------------------------------

int Catalog_SchemaGrantRevoke::InsertSchemaPrivilege (const Catalog_ObjectId& schemaId,  const Catalog_ObjectId& grantee)
{
    if (!m_insertPrepared)
    {
        m_insertPrepared = true;
        m_insertStatement.prepare("INSERT INTO SYSCATALOG.SCHEMAPRIVILEGES VALUES (?,?,?,?,?,?,false, timestamp)");
    }
    m_insertStatement.bind(1, *reinterpret_cast<const tgg00_Surrogate*>(&schemaId));
    m_insertStatement.bind(2, *reinterpret_cast<const tgg00_Surrogate*>(&grantee));
    m_insertStatement.bind(3, m_context.a_curr_user_id);
    m_insertStatement.bind(4, m_changeCreateIn);
    m_insertStatement.bind(5, m_changeDropIn);
    m_insertStatement.bind(6, m_changeAlterIn);
    KSQL_PreparedStatement::SQLCode rc = m_insertStatement.execute();
    if (0 != rc)
    {
        const char* pState;
        const char* pMsg = 0;
        m_updateStatement.describeLastError(pState, pMsg);
        Msg_List& errList = m_context.GetErrorList();
        errList.AppendNewMessage(Msg_List(Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED, Msg_OptArg("RETURNCODE", rc), Msg_OptArg("DESCRIPTION", pMsg?pMsg:"")));
    }
    return rc;
}

//-------------------------------------------------------------------------------------------------------------

int Catalog_SchemaGrantRevoke::StoreSchemaPrivilege (const Catalog_ObjectId& schemaId,  const Catalog_ObjectId& grantee)
{
    m_updateStatement.bind(m_privCount+1, *reinterpret_cast<const tgg00_Surrogate*>(&schemaId));
    m_updateStatement.bind(m_privCount+2, *reinterpret_cast<const tgg00_Surrogate*>(&grantee));
    KSQL_PreparedStatement::SQLCode rc = m_updateStatement.execute();
    if (SQL_ROW_NOT_FOUND == rc)
    {
        return this->InsertSchemaPrivilege (schemaId, grantee);
    }
    else
    {
        if (0 != rc)
        {
            const char* pState;
            const char* pMsg = 0;
            m_updateStatement.describeLastError(pState, pMsg);
            Msg_List& errList = m_context.GetErrorList();
            errList.AppendNewMessage(Msg_List(Msg_List::Error, SDBMSG_CATALOG_UPDATE_FAILED, Msg_OptArg("RETURNCODE", rc), Msg_OptArg("DESCRIPTION", pMsg?pMsg:"")));
        }
    }
    return rc;
}

//-------------------------------------------------------------------------------------------------------------

bool Catalog_Authorization::SelectSchemaPrivilege (SQLMan_Context&         context,
                                                  const Catalog_ObjectId& schemaId, 
                                                  const Catalog_ObjectId& grantee,
                                                  const char*             requiredPriv)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::SelectSchemaPrivilege", Catalog_Trace, 1);
    if (context.IsOk())
    {
        KSQL_Connection con;
        if (con.isConnected())
        {
            SQLMan_SQLStatementContext newStatementScope(context);
            context.SetInDDLTrigger();
            KSQL_PreparedStatement stmt = con.createPreparedStatement();
            char selectStatement[150];
            strcpy (selectStatement, "SELECT COUNT(*) INTO ? FROM SYSCATALOG.SCHEMAPRIVILEGES WHERE SCHEMAID  = ? "
                "AND (GRANTEE = ? OR GRANTEE = x'0000000000000002') AND ");
            strcat (selectStatement, requiredPriv);
            stmt.prepare(selectStatement);

            int count;
            stmt.bind(1, count);
            stmt.bind(2, schemaId);
            stmt.bind(3, grantee);
            KSQL_PreparedStatement::SQLCode rc = stmt.execute();
            if (0 != rc)
            {
                const char* pState;
                const char* pMsg = 0;
                stmt.describeLastError(pState, pMsg);
                Msg_List& errList = context.GetErrorList();
                errList.AppendNewMessage(Msg_List(Msg_List::Error, SDBMSG_CATALOG_ACCESS_FAILED, Msg_OptArg("RETURNCODE", rc), Msg_OptArg("DESCRIPTION", pMsg?pMsg:"")));
                context.SetReturnCode(rc);
            }
            else
            {
                if (count > 0) 
                {
                    return true;
                }
            }
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------------------------

void Catalog_Authorization::DeleteAuthorizationObject (SQLMan_Context& context, const SQLMan_Identifier& authName)
{
    Catalog_AuthorizationObject::Delete (context, authName);
}

//-------------------------------------------------------------------------------------------------------------

void Catalog_Authorization::CheckCreateInPrivilege(SQLMan_Context& context, const SQLMan_Identifier& schemaName, int errorPos)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::CheckCreateInPrivilege", Catalog_Trace, 1);
    this->CheckSchemaPrivilege (context, schemaName, cak_i_createin, errorPos);
}

//-------------------------------------------------------------------------------------------------------------

void Catalog_Authorization::CheckDropInPrivilege(SQLMan_Context& context, const SQLMan_Identifier& schemaName, int errorPos)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::CheckDropInPrivilege", Catalog_Trace, 1);
    this->CheckSchemaPrivilege (context, schemaName, cak_i_dropin, errorPos);
}

//-------------------------------------------------------------------------------------------------------------

void Catalog_Authorization::CheckAlterInPrivilege(SQLMan_Context& context, const SQLMan_Identifier& schemaName, int errorPos)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::CheckAlterInPrivilege", Catalog_Trace, 1);
    this->CheckSchemaPrivilege (context, schemaName, cak_i_alterin, errorPos);
}

//-------------------------------------------------------------------------------------------------------------

void Catalog_Authorization::CheckSchemaPrivilege(SQLMan_Context& context, const SQLMan_Identifier& schemaName, int requiredPriv, int errorPos)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::CheckSchemaPrivilege", Catalog_Trace, 1);
    if ((schemaName != context.a_curr_user_name) && (schemaName != Catalog_Instance::Instance().TempIdentifier()))
    {
         Catalog_SchemaObject* pSchema = this->ExistsSchema (context, schemaName);
         if (pSchema)
         {
             if (pSchema->GetSchemaOwnerId() != context.a_curr_user_id)
             {
                 Catalog_ObjectId schemaId = pSchema->GetObjectId();
                 char wantedPriv[10];
                 switch (requiredPriv)
                 {
                 case cak_i_createin :
                     strcpy(&wantedPriv[0], "CREATEIN"); 
                     break;
                 case cak_i_dropin :
                     strcpy(&wantedPriv[0], "DROPIN");
                     break;
                 case cak_i_alterin :
                     strcpy(&wantedPriv[0], "ALTERIN");
                     break;
                 }
                 if (!this->SelectSchemaPrivilege (context, schemaId, context.a_curr_user_id, wantedPriv))
                 {
                     context.ThrowError (e_missing_privilege, errorPos, requiredPriv);
                 }
             }
         }
         else
         {
             context.ThrowError (e_unknown_sql_schema, errorPos, schemaName);
         }
    }
}

//-------------------------------------------------------------------------------------------------------------

Catalog_UserObject* Catalog_Authorization::ExistsRole (SQLMan_Context& context, const SQLMan_Identifier& roleName)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::ExistsRole", Catalog_Trace, 1);
    Catalog_AuthorizationObject* pUser = context.GetSessionCatalogCache().LoadAuthorizationObject(roleName);
    if (pUser)
    {
        if (pUser->IsRoleObject())
        {
            return REINTERPRET_CAST (Catalog_UserObject*, pUser);
        }
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------

Catalog_UserObject* Catalog_Authorization::ExistsUser (SQLMan_Context& context, const SQLMan_Identifier& userName)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::ExistsUser", Catalog_Trace, 1);
    Catalog_AuthorizationObject* pUser = context.GetSessionCatalogCache().LoadAuthorizationObject(userName);
    if (pUser)
    {
        if (pUser->IsUserObject())
        {
            return REINTERPRET_CAST (Catalog_UserObject*, pUser);
        }
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------

Catalog_SchemaObject* Catalog_Authorization::ExistsSchema (SQLMan_Context& context, const SQLMan_Identifier& schemaName)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::ExistsSchema", Catalog_Trace, 1);
    Catalog_AuthorizationObject* pSchema = context.GetSessionCatalogCache().LoadAuthorizationObject(schemaName);
    if (pSchema)
    {
        if (pSchema->IsSchemaObject())
        {
            return REINTERPRET_CAST (Catalog_SchemaObject*, pSchema);
        }
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------

void Catalog_Authorization::ForEach (SQLMan_Context& context, Catalog_ICallBack& callback, bool eachUser, bool eachSchema, bool eachRole)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::ForEach", Catalog_Trace, 1);
    Catalog_AuthorizationObject::ForEach(context, callback, eachUser, eachSchema, eachRole);
}

//-------------------------------------------------------------------------------------------------------------

Catalog_ObjectId Catalog_Authorization::GetSchemaId (SQLMan_Context& context, const SQLMan_Identifier& schemaName, int errorPos)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::GetSchemaId", Catalog_Trace, 1);
    if (schemaName == context.a_curr_schema)
    {
        return Catalog_ObjectId(context.a_curr_schema_id);
    }
    if (schemaName == Catalog_Instance::Instance().TempIdentifier())
    {
        Catalog_ObjectId objectId;
        SAPDB_MemCopyNoCheck (&objectId, &cak_temp_user_id, sizeof(objectId));
        return objectId;
    }
    Catalog_SchemaObject* pSchema = this->ExistsSchema (context, schemaName);
    if (pSchema)
    {
        return pSchema->GetObjectId();
    }
    if (errorPos > 0)
    {
        context.ThrowError (e_unknown_sql_schema, errorPos, schemaName);
    }
    return Catalog_ObjectId();
}

//-------------------------------------------------------------------------------------------------------------

const SQLMan_Identifier& Catalog_Authorization::GetSchemaName (SQLMan_Context& context, const Catalog_ObjectId& schemaId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::GetSchemaName", Catalog_Trace, 1);
    if (schemaId == context.a_curr_schema_id)
    {
        return *reinterpret_cast<SQLMan_Identifier*>(&context.a_curr_schema);
    }
    if (0 == memcmp (&cak_temp_user_id, &schemaId, sizeof(schemaId)))
    {
        return Catalog_Instance::Instance().TempIdentifier();
    }
    if (0 == memcmp (&cak_sys_user_id, &schemaId, sizeof(schemaId)))
    {
        return *reinterpret_cast<SQLMan_Identifier*>(&a01_i_sysdd[0]);
    }
    Catalog_AuthorizationObject* pSchema = context.GetSessionCatalogCache().LoadAuthorizationObject(schemaId);
    if (pSchema)
    {
        if (pSchema->IsSchemaObject())
        {
            return pSchema->GetName();
        }
    }
    return Catalog_Instance::Instance().BlankIdentifier();
}

//-------------------------------------------------------------------------------------------------------------

int Catalog_Authorization::GrantRevokeSchemaPrivilege (SQLMan_Context& context,
        bool                     isGrant,
        bool                     alterIn,
        bool                     createIn,
        bool                     dropIn,
        int                      cntSchemas,
        tsp00_KnlIdentifierPtr   pSchemas,
        int                      cntUsers,
        tsp00_KnlIdentifierPtr   pGrantee)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::GrantRevokeSchemaPrivilege", Catalog_Trace, 1);
    int ret = 0;
    int rc  = 0;
    int e   = e_ok;
    {
        SQLMan_SQLStatementContext newStatementScope(context);
        Catalog_SchemaGrantRevoke grantRevoke(context, isGrant, alterIn, createIn, dropIn);
        for (int idx = 0; ((idx < cntSchemas) && (e_ok == e)); ++idx, ++pSchemas)
        {
            Catalog_SchemaObject* pSchema = this->ExistsSchema (context, *pSchemas);
            if (0 == pSchema)
            {
                e   = e_unknown_sql_schema;
                ret = idx + 1;
                break;
            }
            if (pSchema->GetSchemaOwnerId() != context.a_curr_user_id)
            {
                e   = e_missing_privilege;
                ret = idx + 1;
                break;
            }
            Catalog_ObjectId schemaId = pSchema->GetObjectId();
            for (int udx = 0; udx < cntUsers; ++udx, ++pGrantee)
            {
                Catalog_ObjectId granteeId;
                if (*pGrantee == a01_i_public)
                {
                    SAPDB_MemCopyNoCheck (&granteeId[0], &cak_public_id[0], sizeof(granteeId));
                }
                else
                {
                    Catalog_UserObject* pUser = this->ExistsUser (context, *pGrantee);
                    if (!pUser)
                    {
                        // is it a role ?
                        pUser = this->ExistsRole (context, *pGrantee);
                    }
                    if (0 == pUser)
                    {
                        e   = e_unknown_user;
                        ret = udx + 1;
                        break;
                    }
                    else
                    {
                        granteeId = pUser->GetObjectId();
                    }
                }
                rc = grantRevoke.StoreSchemaPrivilege(schemaId, granteeId);
                if (0 != rc)
                {
                    ret = udx + 1;
                    break;
                }
            }
        }
    }
    // error must be set outside new statement scope !
    if (e_ok != e)
    {
        context.ThrowError (e);
    }
    else
    {
        if (0 != rc)
        {
            context.SetReturnCode(rc);
        }
    }
    return ret;
}

//-------------------------------------------------------------------------------------------------

bool Catalog_Authorization::LoadAuthorizationObject (SQLMan_Context& context, const SQLMan_Identifier& schemaName, bool useCache, void* pResBuf)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::LoadAuthorizationObject", Catalog_Trace, 1);
    return Catalog_AuthorizationObject::icp_ok == reinterpret_cast<Catalog_AuthorizationObject*>(pResBuf)->Incopy(context, schemaName, useCache);
}

//-------------------------------------------------------------------------------------------------

bool Catalog_Authorization::LoadAuthorizationObject (SQLMan_Context& context, const Catalog_ObjectId& objectId, void* pResBuf)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::LoadAuthorizationObject(2)", Catalog_Trace, 1);
    return Catalog_AuthorizationObject::icp_ok == reinterpret_cast<Catalog_AuthorizationObject*>(pResBuf)->Incopy(context, objectId);
}

//-------------------------------------------------------------------------------------------------

bool Catalog_Authorization::LoadAuthorizationObject (SQLMan_Context& context, SAPDB_Int4 schemaId, Catalog_IAuthorization::LockRequest lockRequest, void* pResBuf)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::LoadAuthorizationObject(3)", Catalog_Trace, 1);
    return Catalog_AuthorizationObject::icp_ok == reinterpret_cast<Catalog_AuthorizationObject*>(pResBuf)->Incopy(context, schemaId, lockRequest);
}

//-------------------------------------------------------------------------------------------------

void Catalog_Authorization::Lock (SQLMan_Context&   Context, 
                       const Catalog_ObjectId&      AuthId,
                       bool                         LockExclusive)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::Lock", Catalog_Trace, 1);
    Catalog_AuthorizationObject::Lock(Context, AuthId, LockExclusive);
}

//-------------------------------------------------------------------------------------------------

bool Catalog_Authorization::NextAuthorizationObject (SQLMan_Context& context, 
                                                     const Catalog_ObjectId& objectId,
                                                     bool  userGroup, 
                                                     bool  nonUserGroup, 
                                                     void* pResBuf)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::NextAuthorizationObject", Catalog_Trace, 1);
    return reinterpret_cast<Catalog_AuthorizationObject*>(pResBuf)->Next (context, objectId, userGroup, nonUserGroup);
}

//-------------------------------------------------------------------------------------------------------------

void Catalog_Authorization::Rename (SQLMan_Context& context, 
        const SQLMan_Identifier& oldName,
        const SQLMan_Identifier& newName) 
{
     SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::Rename", Catalog_Trace, 1);
     Catalog_AuthorizationObject::Rename (context, oldName, newName);
}

//-------------------------------------------------------------------------------------------------------------

void Catalog_Authorization::UpdateGroupMembers (SQLMan_Context& context, Catalog_AuthorizationObject* pGroup)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::UpdateGroupMembers", Catalog_Trace, 1);
    Catalog_AuthorizationObject::UpdateGroupMembers (context, pGroup); 
}

//-------------------------------------------------------------------------------------------------------------

void Catalog_Authorization::UpdatePasswords (SQLMan_Context& context, Catalog_AuthorizationObject* p)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::UpdatePasswords", Catalog_Trace, 1);
    p->UpdatePasswords (context); 
}

//-------------------------------------------------------------------------------------------------------------

void Catalog_Authorization::UpdateUserOwner (SQLMan_Context&         context, 
                                             const Catalog_ObjectId& oldOwnerId, 
                                             const Catalog_ObjectId& newOwnerId,
                                             bool                    GroupsOnly)
{
    SAPDBTRACE_METHOD_DEBUG ("Catalog_Authorization::UpdateUserOwner", Catalog_Trace, 1);
    Catalog_AuthorizationObject::UpdateUserOwner(context, oldOwnerId, newOwnerId, GroupsOnly);
}