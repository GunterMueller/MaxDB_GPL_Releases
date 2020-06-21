/*!
  @file           Catalog_Instance.hpp
  @author         ThomasA
  @brief          Implements the catalog interface

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

#ifndef CATALOG_INSTANCE_HPP
#define CATALOG_INSTANCE_HPP

#include "SAPDB/SQLManager/Catalog/Catalog_Types.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Interface.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_Authorization.hpp"

#include "SAPDB/SAPDBCommon/SAPDB_Singleton.hpp"

#include "ggg00.h"

class KSQL_Connection;
//class SAPDBErr_MessageList;
class Msg_List;
class RTESync_IRWRegion;
class Catalog_Instance;
/*!
   @brief creates the catalog instance. This is a singleton providing the catalog interface.
*/
class Catalog_Instance : public Catalog_Interface, public Catalog_Authorization, public SAPDB_Singleton
{
public :
        /*!
       @brief returns true, if the catalog schema has already been created, otherwise false
     */
    virtual bool CatalogSchemaCreated() const;
    /*!
       @brief creates the catalog
     */
    virtual int CreateCatalogTables();
    /*!
       @brief creates the catalog file(s)
    */
    virtual SAPDB_Bool CreateFile(tgg00_TransContext& trans);
    /*!
       @brief returns true, if the catalog tables have already been created
     */
    virtual bool ExistCatalogTables() const;
    /*!
       @brief returns the id of the index created on table syscatalog.authorization
     */
    inline const Catalog_ObjectId& GetAuthorizationId() const;
    /*!
       @brief returns the id of the index created on table syscatalog.authorization
     */
    inline const Catalog_ObjectId& GetAuthorizationIndexId() const;
    /*!
       @brief returns the id of the index created on table syscatalog.classcontainers
     */
    inline const Catalog_ObjectId& GetClassContainersIndexId() const;
    /*!
       @brief returns the object id of the catalog owner
     */
    virtual const Catalog_ObjectId& GetCatalogOwnerId() const;
    /*!
       @brief returns the object id of the catalog schema
     */
    virtual const Catalog_ObjectId& GetCatalogSchemaId() const;
    /*!
       @brief returns the name of the catalog schema
     */
    virtual const SQLMan_Identifier& GetCatalogSchemaName() const;
    /*
       @brief returns the table surrogate of the first catalog file
    */
    virtual const tgg00_Surrogate& GetSys1CatalogTabId() const;
    /*
       @brief returns the table surrogate of the second catalog file
    */
    virtual const tgg00_Surrogate& GetSys2CatalogTabId() const;
    /*
       @brief returns the tree id of the first catalog file
    */
    virtual const tgg00_FileId& GetSys1CatalogTreeId() const;
    /*
       @brief returns the tree id of the second catalog file
    */
    virtual const tgg00_FileId& GetSys2CatalogTreeId() const;
    /*!
       @brief returns true, if the catalog is stored in unicode layout; otherwise false
     */
    virtual bool IsUnicodeCatalog() const;
    /*!
       @brief migrates the catalog
     */
    virtual void Migrate();
    /*!
       @brief migrates an ASCII catalog to a UNICODE catalog
     */
    virtual void MigrateToUnicode();
    /*!
       @brief initializes the catalog after a restart
     */
    virtual bool Restart();
    /*
       @brief restarts the catalog
    */
    virtual SAPDB_Bool RestartCatalog(
        tgg00_TransContext&    trans,
        const tgg00_Surrogate& sys1CatalogTabId,
        const tgg00_Surrogate& sys2CatalogTabId);
    /*
       @brief returns true if the table surrogate is that of the first catalog table
    */
    virtual SAPDB_Bool IsSys1CatalogTable(
        const tgg00_Surrogate& tabId) const;
    /*
       @brief returns true if the table surrogate is that of the second catalog table
    */
    virtual SAPDB_Bool IsSys2CatalogTable(
        const tgg00_Surrogate& tabId) const;
    /*
       @brief returns true if the table surrogate is that of a catalog table
    */
    virtual SAPDB_Bool IsCatalogTable(
        const tgg00_Surrogate& tabId) const;

    /*!
       @brief stores the given treeid as sys1 catalog tree
     */
    virtual void SetSys1CatalogTreeId(const tgg00_FileId& treeId);
    /*!
       @brief stores the given treeid as sys2 catalog tree
     */
    virtual void SetSys2CatalogTreeId(const tgg00_FileId& treeId);

    // end of methods inherited from Catalog_Interface

    // methods inherited from Catalog_Authorization
    virtual bool IncopySpecialAuthorizationObject(SQLMan_Context& context, const SQLMan_Identifier& name, Catalog_AuthorizationObject& dest, bool& isCopied) const;
    virtual bool IncopySpecialAuthorizationObject(SQLMan_Context& context, const Catalog_ObjectId& authId, Catalog_AuthorizationObject& dest, bool& isCopied) const;

    /*!
       @brief returns the catalog instance
     */
    inline static Catalog_Instance& Instance();
    /*!
       @brief returns the blank identifier in the current layout (ascii or unicode)
     */
    inline const SQLMan_Identifier& BlankIdentifier() const;
     /*!
       @brief returns the oms schema name
     */
    inline const SQLMan_Identifier& OmsSchemaName() const;
    /*!
       @brief returns the identifier 'TEMP' in the current layout (ascii or unicode)
     */
    inline const SQLMan_Identifier& TempIdentifier() const;
private :
    static Catalog_Instance* m_pInstance;
    Catalog_Instance();
//    void Abort(const SAPDBErr_MessageList& errorList);
    void Abort(const Msg_List& errorList);
    static void CreateInstance();
    void ExecuteSqlStatement (KSQL_Connection& con, const char* stmt);
    void CreateCatalogDomains (KSQL_Connection& con);
    void CreateCatalogTables (KSQL_Connection& con);
    void InitTreeId(
        const tgg00_Surrogate& sys1CatalogTabId,
        const tgg00_Surrogate& sys2CatalogTabId);
    void RegisterCatalogTables();
    bool Restart(bool RegisterCatalogTables);
    bool                         m_schemaCreated;
    bool                         m_tablesCreated;
    bool                         m_unicode;
    Catalog_AuthorizationObject* m_schema;
    Catalog_AuthorizationObject* m_sysdba;
    Catalog_ObjectId             m_schemaId;
    Catalog_ObjectId             m_ownerId;
    Catalog_ObjectId             m_authorizationId;
    Catalog_ObjectId             m_authorizationIndexId;
    Catalog_ObjectId             m_containersIndexId;
    SQLMan_Identifier            m_catalogSchemaName;
    SQLMan_Identifier            m_blankname;
    SQLMan_Identifier            m_omsSchemaName;
    SQLMan_Identifier            m_tempname;
    tgg00_FileId                 m_sys1CatalogTreeId;
    tgg00_FileId                 m_sys2CatalogTreeId;

    friend class Catalog_Interface;
};

//-----------------------------------------------------------------------

inline Catalog_Instance& Catalog_Instance::Instance()
{

    SAPDBERR_ASSERT_STATE( 0 != m_pInstance );
    return *m_pInstance;
}

//-----------------------------------------------------------------------

inline const SQLMan_Identifier& Catalog_Instance::BlankIdentifier() const
{
    return m_blankname;
}

//-----------------------------------------------------------------------

inline const Catalog_ObjectId& Catalog_Instance::GetAuthorizationId() const
{
    return m_authorizationId;
}

//-----------------------------------------------------------------------

inline const Catalog_ObjectId& Catalog_Instance::GetAuthorizationIndexId() const
{
    return m_authorizationIndexId;
}

//-----------------------------------------------------------------------

inline const Catalog_ObjectId& Catalog_Instance::GetClassContainersIndexId() const
{
    return m_containersIndexId;
}

//-----------------------------------------------------------------------

inline const SQLMan_Identifier& Catalog_Instance::OmsSchemaName() const
{
    return m_omsSchemaName;
}

//-----------------------------------------------------------------------

inline const SQLMan_Identifier& Catalog_Instance::TempIdentifier() const
{
    return m_tempname;
}

#endif
