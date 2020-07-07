/*!
  @file           Catalog_ISessionCache.hpp
  @author         ThomasA
  @brief          interface of a Catalog Session Cache
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


#ifndef CATALOG_ISESSIONCACHE_HPP
#define CATALOG_ISESSIONCACHE_HPP

class SharedSQL_SQLCache;
class Catalog_Object;
class Catalog_AuthorizationObject;
class Catalog_DBProcObject;
class Catalog_DBProcFileObject;
class Catalog_DBProcInfoObject;
class SharedSQL_PrepareHandle;
class SQLMan_ParseId;
class SQLMan_Context;
class Catalog_MessBlockObject;
class SQLMan_Identifier;
class Catalog_ObjectId;
class Catalog_ContainerObject;
class Catalog_GUID;

#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"

/*!
  @brief defines the interface of the session catalog cache
*/

class Catalog_ISessionCache
{
public :
        /*!
           @brief base class for Catalog_Reference, implements a reference to the shared catalog cache
         */
        class Reference
        {
        protected :
            ~Reference(); //!< destructor
            /*!
               @brief returns the pointer to the catalog object
             */
            const void* GetPtr() const
            {
                return m_pObject;
            }
            /*!
               @brief default constructor
             */
            Reference();
            /*!
               @brief constructor 
             */
            Reference(SQLMan_Context& Context, const Catalog_Object* pObject); 
            void Assign (const Reference& Source); //!< assignment  
            Reference(const Reference& Source);    //!< copy constructor
            const Catalog_Object* m_pObject;       //!< pointer to the catalog object lying in the shared cache
            SQLMan_Context*       m_pContext;      //!< current sqlmanager context
        };

        /*!
           @brief implements a reference to a catalog object of type X in the shared catalog cache
         */
        template<class X>
        class Catalog_Reference : public Reference
        {
        public :
            /*!
               @brief default constructor
             */
            Catalog_Reference()
                : Reference()
            {
            }
            /*!
               @brief constructor
             */
            Catalog_Reference(SQLMan_Context& Context, const X* p)
                : Reference(Context, p)
            {
            }
            /*!
               @brief, true if the catalog abject is assigned
             */
            operator bool () const { return (0 != m_pObject); }
            /*!
              @brief access to members and functions if X
             */
            const X* operator->() const { return reinterpret_cast<const X*>(this->GetPtr()); }
            /*!
              @brief assignment, updates reference counter
             */
            void operator= (const Catalog_Reference<X>& Source) { this->Assign(Source); }
            /*!
               @brief copy constructor,
             */
            Catalog_Reference(const Catalog_Reference<X>& Source) 
                : Reference(Source)
            {
            }
            friend class Catalog_ISessionCache;
        private :
        };
    /*!
       @brief diables access to the catalog tables. Catalog request are directed to the catalog files only
     */
    virtual void DisableCatalogTables() = 0;
    /*!
       @brief enables access to the catalog tables. Catalog request are directed to the catalog tables if required
     */
    virtual void EnableCatalogTables() = 0;
    /*!
    @brief returns a table desctiptor for a table identified by Schame and Tablename. The kernel order for name
           resoluation is used internally.
    */
    virtual Catalog_TableDescriptor GetTableDescriptor (const SQLMan_Identifier& Schema, const SQLMan_Identifier Tablename) = 0;
     /*!
    @brief loads the description of an authorization (role, schema, user) into the session cache
    @param authId the identification of the authorization
    */
    virtual Catalog_AuthorizationObject* LoadAuthorizationObject(const SQLMan_Surrogate& authId) = 0;
    /*!
    @brief loads the description of an authorization (role, schema, user) into the session cache
    @param authName the name of the authorization
    */
    virtual Catalog_AuthorizationObject* LoadAuthorizationObject(const SQLMan_Identifier& authName) = 0;
    /*!
    @brief returns the error code of the last operation
    */
    virtual SAPDB_Int2 GetLastError() const = 0;
    /*!
    @brief removes the column information of a select into statement and returns a handle to that info
           The information can be restored using method RestoreColumnInfo.
    */
    virtual void* KeepColumnInfo() = 0;
    /*!
    @brief removes the parameter information of a select into statement and returns a handle to that info
           The information can be restored using method RestoreIntoParameterInfo.
    */
    virtual void* KeepIntoParameterInfo() = 0;
    /*!
    @brief loads the byte code of a stored procedure
    @param procId  the internal name of the procedure
    @return a pointer to the code or NULL if the code does not exist
    */
    virtual Catalog_MessBlockObject* LoadCode(const SQLMan_Surrogate& procId) = 0;
    /*!
    @brief loads the description of an oms container
    @param SchemaId the schema containing the oms container
    @param ContainerNo the container number of the container
    @param Guid the class identification 
    */
    virtual Catalog_Reference<Catalog_ContainerObject> LoadContainerObject (const Catalog_ObjectId& SchemaId, SAPDB_UInt4 ContainerNo, const Catalog_GUID& Guid, bool NoWait) = 0;
    /*!
    @brief loads the description of a db-procedure into the session cache
    */
    virtual Catalog_DBProcObject* LoadDBProcObject(const SQLMan_Surrogate& procId) = 0;
    /*!
    @brief loads the info description of a db-procedure into the session cache
    */
    virtual Catalog_DBProcInfoObject* LoadDBProcInfoObject(const SQLMan_Surrogate& procId) = 0;
    /*!
    @brief loads the file description of a db-procedure into the session cache
    */
    virtual Catalog_DBProcFileObject* LoadDBProcFileObject(const SQLMan_Surrogate& procId) = 0;
    /*!
    @brief restored the parameter information of a select into statement. The parameter info has to
           be identified by a handle returned by a call of KeepIntoParameterInfo 
    */
    virtual void RestoreIntoInfo(void* handle) = 0;
    /*!
    @brief  stores the current plan into the shared sql plan cache
    @return true if the plan could be stored successfully, otherwise false
    */
    virtual bool StorePlan(SharedSQL_SQLCache& sharedSQLManager, 
                   SharedSQL_PrepareHandle*     handle, 
                   SQLMan_ParseId&              parseId,
                   bool                         prepareFlag) = 0;
};

#endif