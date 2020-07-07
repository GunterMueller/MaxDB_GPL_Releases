/*!
  @file           Catalog_IAuthorization.hpp
  @author         ThomasA
  @brief          catalog authorization interface  
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

#ifndef CATALOG_IAUTHORIZATION_HPP
#define CATALOG_IAUTHORIZATION_HPP

class Catalog_ObjectId;
class Catalog_UserObject;
class Catalog_SchemaObject;
class Catalog_AuthorizationObject;
class Catalog_ICallBack;
class SQLMan_Identifier;
class SQLMan_Context;

/*!
   @brief defines the catalog interface concerning the authorization topic
*/

class Catalog_IAuthorization
{
public :
    enum LockRequest { LOCK_NONE, LOCK_SHARE, LOCK_EXCLUSIVE }; //!< supported lock requests
    /*!
       @brief checks, if the current user is allowed to create objects in the specified schema. If this
              is not true, an sql error is thrown.
       @param context sql manager context
       @param schemaName the schema to be checked
       @param errorPos current position in sql statement, used only if error is thrown
     */
    virtual void CheckCreateInPrivilege(SQLMan_Context& context, const SQLMan_Identifier& schemaName, int errorPos) = 0;
    /*!
       @brief checks, if the current user is allowed to drop objects in the specified schema. If this
              is not true, an sql error is thrown.
       @param context sql manager context
       @param schemaName the schema to be checked
       @param errorPos current position in sql statement, used only if error is thrown
     */
    virtual void CheckDropInPrivilege(SQLMan_Context& context, const SQLMan_Identifier& schemaName, int errorPos) = 0;
    /*!
       @brief checks, if the current user is allowed to alter objects in the specified schema. If this
              is not true, an sql error is thrown.
       @param context sql manager context
       @param schemaName the schema to be checked
       @param errorPos current position in sql statement, used only if error is thrown
     */
    virtual void CheckAlterInPrivilege(SQLMan_Context& context, const SQLMan_Identifier& schemaName, int errorPos) = 0;
    /*!
       @brief deletes an authorization object from the catalog
       @param context sql manager context
       @param authName the name identifying the authorization object
     */
    virtual void DeleteAuthorizationObject (SQLMan_Context& context, const SQLMan_Identifier& authName) = 0;
     /*!
      @brief checks existence of a given role
      @param context sql manager context
      @param userName the name of the checked role
      @return a pointer to a Catalog_UserObject, if the role exists, otherwise null
     */
    virtual Catalog_UserObject* ExistsRole (SQLMan_Context& context, const SQLMan_Identifier& roleName) = 0;
    /*!
      @brief checks existence of a given user
      @param context sql manager context
      @param userName the name of the checked user
      @return a pointer to a Catalog_UserObject, if the user exists, otherwise null
     */
    virtual Catalog_UserObject* ExistsUser (SQLMan_Context& context, const SQLMan_Identifier& userName) = 0;
    /*!
      @brief checks existence of a given schema
      @param context sql manager context
      @param schemaName the name of the checked schema
      @return a pointer to a Catalog_SchemaUserObject, if the schemar exists, otherwise null
     */
    virtual Catalog_SchemaObject* ExistsSchema (SQLMan_Context& context, const SQLMan_Identifier& schemaName) = 0;
    /*!
      @brief scans over all authorizations and calls callback method for each.
      @param context sql manager context
      @param callback the instance providing the callback funtion. This function is call for each
             qualified authorization object.
      @param eachUser if true, callback is called for each user
      @param eachSchema if true, callback is called for each schema
      @param eachRole if true, callback is called for each role
     */
    virtual void ForEach (SQLMan_Context& context, Catalog_ICallBack& callback, bool eachUser, bool eachSchema, bool eachRole) = 0;
    /*!
      @brief returns the objectId of a given schema. Throws an error, if the schema does not exist
      @param context sql manager context
      @param schemaName the name of the checked schema
      @param errorPos if > 0 an the error 'unknown name' is throw, if the schema does not exist
     */
    virtual Catalog_ObjectId GetSchemaId (SQLMan_Context& context, const SQLMan_Identifier& schemaName, int errorPos) = 0;
    /*!
       @brief returns the schema name if a given schema identifified by its schema identification
       @param context sql manager context
       @param schemaId the schema identification
       @return the corresponding schema name, or blank identifier, if the schema does not exist
     */
    virtual const SQLMan_Identifier& GetSchemaName (SQLMan_Context& context,const Catalog_ObjectId& schemaId) const = 0;
    /*!
       @brief stores or deletes meta data describing schema privileges
       @param context sql manager context
       @param isGrant if true, meta data is stored, otherwise meta data is deleted
       @param alterIn if true, grant/revoke ALTERIN privilege
       @param createIn if true, grant/revoke CREATEIN privilege
       @param dropIn if true, grant/revoke DROPIN privilege
       @param cntSchemas number of schema names contained in pSchemas
       @param pSchemas an array of identifiers identifying the schemas to which privileges
              are granted or revoked.
       @param cntUsers number of user names contained in pGrantee
       @param pGrantee an array of identifiers identifying the grantees to which privileges
              are granted or revoked.
       @return in case of a non existing schema/user the correspondig array index is returned   
     */
    virtual int GrantRevokeSchemaPrivilege (SQLMan_Context& context,
        bool                     isGrant,
        bool                     alterIn,
        bool                     createIn,
        bool                     dropIn,
        int                      cntSchemas,
        tsp00_KnlIdentifierPtr   pSchemas,
        int                      cntUsers,
        tsp00_KnlIdentifierPtr   pGrantee) = 0;
    /*!
       @brief returns true, if the required authorization object can be provided by this interface. If this is true
              the object is stored into the destination, except the interface has not already been restarted. In
              this case true is returned, but parameter isCopied contains false.
       @param context sql manager context
       @param name the name of the required authorization object ('CATALOG', or sysdba name)
       @param dest the instance where the result has to be copied
       @param isCopied true, if the authorization object has been copied into dest, false otherwise
       @return true, if the required name can be delivered by the interface, false otherwise
     */
    virtual bool IncopySpecialAuthorizationObject(SQLMan_Context&              context, 
                                                  const SQLMan_Identifier&     name, 
                                                  Catalog_AuthorizationObject& dest, 
                                                  bool& isCopied) const = 0;
    /*!
       @brief returns true, if the required authorization object can be provided by this interface. If this is true
              the object is stored into the destination, except the interface has not already been restarted. In
              this case true is returned, but parameter isCopied contains false.
       @param context sql manager context
       @param authId the identification of the required authorization object ('CATALOG' id, or sysdba is)
       @param dest the instance where the result has to be copied
       @param isCopied true, if the authorization object has been copied into dest, false otherwise
       @return true, if the required name can be delivered by the interface, false otherwise
     */
    virtual bool IncopySpecialAuthorizationObject(SQLMan_Context&              context, 
                                                  const Catalog_ObjectId&      authId, 
                                                  Catalog_AuthorizationObject& dest, 
                                                  bool& isCopied) const = 0;
    /*!
      @brief loads an authorization object (role, schema, user) identified by its name into a given buffer
     */
    virtual bool LoadAuthorizationObject (SQLMan_Context& context, const SQLMan_Identifier& schemaName,  bool useCache, void* pResBuf) = 0;
    /*!
      @brief loads an authorization object (role, schema, user) identified by its object id into a given buffer
     */
    virtual bool LoadAuthorizationObject (SQLMan_Context& context, const Catalog_ObjectId& objectId, void* pResBuf) = 0;
    /*!
      @brief loads an authorization object (schema) identified by its schema id into a given buffer
     */
    virtual bool LoadAuthorizationObject (SQLMan_Context& context, SAPDB_Int4 schemaId, LockRequest lockRequest, void* pResBuf) = 0;
    /*!
       @brief lock the schema meta data of the authorization identified by an authorization identifier
       @param Context sql manager context
       @param AuthId the identification of the required authorization object 
       @param LockExclusive true ==> an exlusive lock is required, false ==> an share lock is required
     */
    virtual void Lock (SQLMan_Context&              Context, 
                       const Catalog_ObjectId&      AuthId,
                       bool                         LockExclusive) = 0;
    /*!
      @brief loads the next authorization object (role, schema, user) with is greater than a given user id into a given buffer
     */
    virtual bool NextAuthorizationObject (SQLMan_Context& context, 
        const Catalog_ObjectId& objectId, 
        bool  userGroup, 
        bool  nonUserGroup, 
        void* pResBuf) = 0;
    /*!
      @brief assigns a new name to an authorization object
      @param context sql manager context
      @param oldName the name currently stored in the authorization object
      @param newName the new name to be assigned in the authorization object
     */  
    virtual void Rename (SQLMan_Context& context, 
        const SQLMan_Identifier& oldName,
        const SQLMan_Identifier& newName) = 0;
    /*!
      @brief updates the properties of all group members of a user group
      @param context sql manager context
      @param pGroup a pointer to the authorization object instance of the user group
    */
    virtual void UpdateGroupMembers (SQLMan_Context& context, Catalog_AuthorizationObject* pGroup) = 0;
    /*!
      @brief stores the password and support password of the given authorization object persistently.
      @param context sql manager context
      @param p a pointer to the authorization object instance
    */
    virtual void UpdatePasswords (SQLMan_Context& context, Catalog_AuthorizationObject* p) = 0;
    /*!
      @brief sets owner of all users owned by oldOwner to newOwner
      @param context sql manager context
      @param oldOwnerId user to be updated
      @param newOwnerId new owner id value
      @param GroupsOnly only usergroups are updated
     */
    virtual void UpdateUserOwner (SQLMan_Context& context, const Catalog_ObjectId& oldOwnerId, const Catalog_ObjectId& newOwnerId, bool GroupsOnly) = 0; 
    /*!
       @brief returns the singleton instance implementing the interface
     */
    static Catalog_IAuthorization& GetInstance();
};

#endif