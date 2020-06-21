/*!
  @file           Catalog_Authorization.hpp
  @author         ThomasA
  @brief          implementation of catalog authorization interface  
  @ingroup        Catalog

\if EMIT_LICENCE

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



\endif
*/

#ifndef CATALOG_AUTHORIZATION_HPP
#define CATALOG_AUTHORIZATION_HPP

#include "SQLManager/Catalog/Catalog_IAuthorization.hpp"

/*!
   @brief implementaion of the Catalog_IAuthorization interface
*/
class Catalog_Authorization : public Catalog_IAuthorization 
{
public :
    virtual void CheckCreateInPrivilege(SQLMan_Context& context, const SQLMan_Identifier& schemaName, int errorPos);
    virtual void CheckDropInPrivilege(SQLMan_Context& context, const SQLMan_Identifier& schemaName, int errorPos);
    virtual void CheckAlterInPrivilege(SQLMan_Context& context, const SQLMan_Identifier& schemaName, int errorPos);
    virtual void DeleteAuthorizationObject (SQLMan_Context& context, const SQLMan_Identifier& authName);
    virtual Catalog_UserObject* ExistsRole (SQLMan_Context& context, const SQLMan_Identifier& roleName);
    virtual Catalog_UserObject*   ExistsUser   (SQLMan_Context& context, const SQLMan_Identifier& userName);
    virtual Catalog_SchemaObject* ExistsSchema (SQLMan_Context& context, const SQLMan_Identifier& schemaName);
    virtual void ForEach (SQLMan_Context& context, Catalog_ICallBack& callback, bool eachUser, bool eachSchema, bool eachRole);
    virtual Catalog_ObjectId GetSchemaId (SQLMan_Context& context, const SQLMan_Identifier& schemaName, int errorPos);
    virtual const SQLMan_Identifier& GetSchemaName (SQLMan_Context& context,const Catalog_ObjectId& schemaId) const;
    virtual int GrantRevokeSchemaPrivilege (SQLMan_Context& context,
        bool                     isGrant,
        bool                     alterIn,
        bool                     createIn,
        bool                     dropIn,
        int                      cntSchemas,
        tsp00_KnlIdentifierPtr   pSchemas,
        int                      cntUsers,
        tsp00_KnlIdentifierPtr   pGrantee);
    virtual bool LoadAuthorizationObject (SQLMan_Context& context, const SQLMan_Identifier& schemaName, bool useCache, void* pResBuf);
    virtual bool LoadAuthorizationObject (SQLMan_Context& context, const Catalog_ObjectId& objectId, void* pResBuf);
    virtual bool LoadAuthorizationObject (SQLMan_Context& context, SAPDB_Int4 schemaId, Catalog_IAuthorization::LockRequest lockRequest, void* pResBuf);
    virtual void Lock (SQLMan_Context& Context, const Catalog_ObjectId& AuthId, bool LockExclusive);
    virtual bool NextAuthorizationObject (SQLMan_Context& context, 
        const Catalog_ObjectId& objectId, 
        bool  userGroup, 
        bool  nonUserGroup, 
        void* pResBuf);
    virtual void Rename (SQLMan_Context& context, 
        const SQLMan_Identifier& oldName,
        const SQLMan_Identifier& newName);
    virtual void UpdateGroupMembers (SQLMan_Context& context, Catalog_AuthorizationObject* pGroup);
    virtual void UpdatePasswords (SQLMan_Context& context, Catalog_AuthorizationObject* p);
    virtual void UpdateUserOwner (SQLMan_Context& context, const Catalog_ObjectId& oldOwnerId, const Catalog_ObjectId& newOwnerId, bool GroupsOnly);
private :
    /*!
       @brief  checks, if the current user has a required privilege for a given schema
     */
    void CheckSchemaPrivilege (SQLMan_Context& context, const SQLMan_Identifier& schemaName, int requiredPriv, int errorPos);
    /*!
       @brief returns true, if the given grantee has the required privilege for the given schema; otherwise false
     */
    bool SelectSchemaPrivilege (SQLMan_Context&        context,
                               const Catalog_ObjectId& schemaId, 
                               const Catalog_ObjectId& grantee,
                               const char*             requiredPriv);
};

#endif