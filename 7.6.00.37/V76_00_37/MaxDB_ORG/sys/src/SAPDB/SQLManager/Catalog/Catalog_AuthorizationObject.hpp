/*!
  @file           Catalog_AuthorizationObject.hpp
  @author         ThomasA 
  @ingroup        Catalog
  @brief          defines base class for role, user and schema object

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
#ifndef CATALOG_AUTHORIZATIONOBJECT_HPP
#define CATALOG_AUTHORIZATIONOBJECT_HPP
#include "SQLManager/Catalog/Catalog_Object.hpp"
#include "SQLManager/Catalog/Catalog_IAuthorization.hpp"

class Catalog_ObjectId;
class Catalog_ICallBack;
class Catalog_Instance;
class SQLMan_Context;
class SQLMan_Identifier;
class SAPDBMem_IRawAllocator;
class KSQL_ResultSet;

/*!
   @brief base class for role, user and schema catalog objects
*/
class Catalog_AuthorizationObject : public Catalog_Object
{
public :
    /*!
       @brief defines the key of an authorization object
     */
    class Catalog_Key : public Catalog_Object::Catalog_Key
    {
    public :
        /*!
           @brief constructs the key of an authorization object
         */
        Catalog_Key(const Catalog_ObjectId& objectId);
    };
    /*!
       @brief constructor, creates authorization object for CATALOG schema
    */
    Catalog_AuthorizationObject();
    /*!
       @brief creates a copy of the current authorization object instance
       @param alloc the allocator to be used for memory allocation for the copy
     */
    Catalog_AuthorizationObject* CreateCopy(SAPDBMem_IRawAllocator& alloc) const;
    /*!
       @brief delete the current instance from the catalog table
     */
    static void Delete(SQLMan_Context& context, const SQLMan_Identifier& name);
    /*!
       @brief calls collback function for each authirization object
     */
    static void ForEach (SQLMan_Context& context, Catalog_ICallBack& callback,  bool eachUser, bool eachSchema, bool eachRole);
    /*!
       @brief returns the name of the instance
     */
    inline const SQLMan_Identifier& GetName() const;
    /*!
       @brief returns the object identification of the instance
     */
    inline const Catalog_ObjectId& GetObjectId() const;
    /*!
       @brief returns the owner identification of the instance
     */
    inline const Catalog_ObjectId& GetOwnerId() const;
     /*!
       @brief returns the handle of the schema used by the OMS layer
     */
    inline SAPDB_UInt4 GetSchemaHandle() const;
    /*!
       @brief reads an authorization specified by its object id from the catalog table
     */
    IncopyResult Incopy (SQLMan_Context& context, const SQLMan_Identifier& name, bool useCache);
    /*!
       @brief reads an authorization specified by name from the catalog table
     */
    IncopyResult Incopy (SQLMan_Context& context, const Catalog_ObjectId& authId);
    /*!
       @brief reads an authorization (schema) specified by its schema id
     */
    IncopyResult Incopy (SQLMan_Context& context, SAPDB_Int4 schemaId, Catalog_IAuthorization::LockRequest lockRequest);
     /*!
       @brief creates an object from the catalog tables using the specified allocator 
     */
    static Catalog_Object* Incopy(SQLMan_Context& Context, const Catalog_ObjectId& AuthId, SAPDBMem_IRawAllocator& Allocator);
    /*!
       @brief returns true, if the instance describes a role
     */
    inline bool IsRoleObject() const;
    /*!
       @brief returns true, if the instance describes a schema
     */
    inline bool IsSchemaObject() const;
    /*!
       @brief returns true, if the instance describes a user
     */
    inline bool IsUserObject() const;
    /*!
       @brief locks the authorization object in the catalog
     */
    static void Lock(SQLMan_Context& Context,  const Catalog_ObjectId& ObjectId, bool LockExclusive);
    /*!
       @brief reads the next user from a given user id
       @return true, if a next user has been found; false otherwise
     */
    bool Next (SQLMan_Context& context, const Catalog_ObjectId& objectId,  bool groupUsers, bool nonGroupUsers); 
    /*!
       @brief copies the instance into the catalog tables
     */
    void Outcopy (SQLMan_Context& context, bool isUpdate);
    /*!
       @brief assigns a new name to an authorization object
     */
    static void Rename (SQLMan_Context& context, const SQLMan_Identifier& oldName, const SQLMan_Identifier& newName);   
    /*!
	   @brief returns the size of an authorisation object
     */
	static int SizeOf();
	/*!
      @brief stores the control user into table CATALOG.AUTHORIZATION
     */
    static void StoreControlUser(SQLMan_Context& context);
    /*!
      @brief updates the properties of all user group members 
     */
    static void UpdateGroupMembers (SQLMan_Context& context, Catalog_AuthorizationObject* pGroup);
    /*!
       @brief updates the passwords stored in the current instance into table CATALOG.AUTHORIZATION
    */
    void UpdatePasswords (SQLMan_Context& context);
     /*!
       @brief sets owner of all users owned by oldOwner to newOwner
     */
    static void UpdateUserOwner (SQLMan_Context& context, const Catalog_ObjectId& oldOwnerId, const Catalog_ObjectId& newOwnerId, bool GroupsOnly);
protected :
private :
    void EnableConnect();
    void Fetch (KSQL_ResultSet& rs);
    IncopyResult Incopy (SQLMan_Context& context, 
                 const char*              sqlStmt, 
                 bool                     viaName,
                 const Catalog_ObjectId&  authid,
                 const SQLMan_Identifier& name,
                 SAPDB_Int4               schemaId);
    
    friend class Catalog_Instance;
};

//------------------------------------------------------------------------------------

inline const SQLMan_Identifier& Catalog_AuthorizationObject::GetName() const
{
    return *REINTERPRET_CAST(const SQLMan_Identifier*, &suser().username);
}

//------------------------------------------------------------------------------------

inline const Catalog_ObjectId& Catalog_AuthorizationObject::GetObjectId() const
{
    return *REINTERPRET_CAST(const Catalog_ObjectId*, &suser().usurrogate);
}

//------------------------------------------------------------------------------------

inline const Catalog_ObjectId& Catalog_AuthorizationObject::GetOwnerId() const
{
    return *REINTERPRET_CAST(const Catalog_ObjectId*, &suser().userowner_id);
}

//------------------------------------------------------------------------------------

inline SAPDB_UInt4 Catalog_AuthorizationObject::GetSchemaHandle() const
{
    return suser().uparams[schemaid];
}

//------------------------------------------------------------------------------------

inline bool Catalog_AuthorizationObject::IsRoleObject() const
{
    return (suser().urecordtyp.includes(is_role_rec));
}

//------------------------------------------------------------------------------------

inline bool Catalog_AuthorizationObject::IsSchemaObject() const
{
    return (suser().urecordtyp.includes(is_schema_rec) || this->IsUserObject());
}

//------------------------------------------------------------------------------------

inline bool Catalog_AuthorizationObject::IsUserObject() const
{
    return (suser().urecordtyp.includes(is_user_rec) || suser().urecordtyp.includes(is_group_rec));
}

#endif