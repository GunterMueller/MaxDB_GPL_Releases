/*!***************************************************************************

  module      : Catalog_Interface.hpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog 
  description : Catalog interface

  last changed: 2000-12-06  19:04
  see also    : example.html ...
  first created:2000-03-09  18:21

  -------------------------------------------------------------------------

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

*****************************************************************************/

#ifndef Catalog_Interface_hpp
#define Catalog_Interface_hpp

#include "RunTime/RTE_Types.hpp"

#include "ggg00.h"

class Catalog_ObjectId;

/*!
   @brief defines the interface of the catalog
 */

class Catalog_Interface
{
  public :
    /*!
       @brief returns true, if the catalog schema has already been created, otherwise false
     */
    virtual bool CatalogSchemaCreated() const = 0;
    /*!
       @brief creates the catalog
     */
    virtual int CreateCatalogTables() = 0;
    /*!
       @brief creates the catalog file(s)
    */
    virtual SAPDB_Bool CreateFile(tgg00_TransContext& trans) = 0;
    /*!
       @brief returns true, if the catalog tables have already been created
     */
    virtual bool ExistCatalogTables() const = 0;
    /*!
       @brief returns the object id of the catalog owner
     */
    virtual const Catalog_ObjectId& GetCatalogOwnerId() const = 0;
    /*!
       @brief returns the object id of the catalog schema
     */
    virtual const Catalog_ObjectId& GetCatalogSchemaId() const = 0;
    /*!
       @brief returns the table surrogate of the first catalog file
    */
    virtual const tgg00_Surrogate& GetSys1CatalogTabId() const = 0;
    /*!
       @brief returns the table surrogate of the second catalog file
    */
    virtual const tgg00_Surrogate& GetSys2CatalogTabId() const = 0;
    /*!
       @brief returns the tree id of the first catalog file
    */
    virtual const tgg00_FileId& GetSys1CatalogTreeId() const = 0;
    /*!
       @brief returns the tree id of the second catalog file
    */
    virtual const tgg00_FileId& GetSys2CatalogTreeId() const = 0;
    /*!
       @brief initializes the catalog
     */
    static void Initialize();
    /*!
       @brief returns true, if the catalog is stored in unicode layout; otherwise false
     */
    virtual bool IsUnicodeCatalog() const = 0;
    /*!
       @brief migrates the catalog
     */
    virtual void Migrate() = 0;
    /*!
       @brief migrates an ASCII catalog to a UNICODE catalog
     */
    virtual void MigrateToUnicode() = 0;
    /*!
       @brief initializes the catalog after a restart
     */
    virtual bool Restart() = 0;
    /*!
       @brief restarts the catalog
    */
    virtual SAPDB_Bool RestartCatalog(
        tgg00_TransContext&    trans,
        const tgg00_Surrogate& sys1CatalogTabId,
        const tgg00_Surrogate& sys2CatalogTabId) = 0;
    /*!
       @brief returns true if the table surrogate is that of the first catalog table
    */
    virtual SAPDB_Bool IsSys1CatalogTable(
        const tgg00_Surrogate& tabId) const = 0;
    /*!
       @brief returns true if the table surrogate is that of the second catalog table
    */
    virtual SAPDB_Bool IsSys2CatalogTable(
        const tgg00_Surrogate& tabId) const = 0;
    /*!
       @brief returns true if the table surrogate is that of a catalog table
    */
    virtual SAPDB_Bool IsCatalogTable(
        const tgg00_Surrogate& tabId) const = 0;
    /*!
       @brief stores the given treeid as sys1 catalog tree
     */
    virtual void SetSys1CatalogTreeId(const tgg00_FileId& treeId) = 0;
    /*!
       @brief stores the given treeid as sys2 catalog tree
     */
    virtual void SetSys2CatalogTreeId(const tgg00_FileId& treeId) = 0;
    /*!
      @brief returns a reference to the instance implementing the interface, see Catalog_Instance.cpp
     */
    static Catalog_Interface& GetInstance();
};

#endif
