/*!
  @file           Catalog_ContainerDescriptor.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          descriptor for the catalog table CLASSCONTAINERS

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


#ifndef Catalog_ContainerDescriptor_HPP
#define Catalog_ContainerDescriptor_HPP

#include "SQLManager/Catalog/Catalog_CatalogTableDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_Cache.hpp"

class FileDir_FileNo;

/*!
   @brief describes the catalog table CLASSCONTAINERS
 */
class Catalog_ContainerDescriptor : public Catalog_CatalogTableDescriptor
{
public :
    /*!
       @brief constructor
     */
    Catalog_ContainerDescriptor(SQLMan_Context& Context);
    /*!
      @brief trigger called, whenever a row of table CLASSCONTAINERS is deleted
    */
    virtual tgg00_BasisError DeleteTrigger (RTE_TaskId TaskId, const tgg00_Rec& OldRecord);
    /*!
      @brief trigger called, whenever a row of table CLASSCONTAINERS is inserted
    */
    virtual void InsertTrigger (RTE_TaskId TaskId, const tgg00_Rec& NewRecord);
    /*!
      @brief trigger called, whenever a row of table CLASSCONTAINERS is updated
    */
    virtual void UpdateTrigger (RTE_TaskId TaskId, const tgg00_Rec& OldRecord, const tgg00_Rec& NewRecord);
private :
    /*!
       @brief extracts the schema identification from a catalog row describing a container
     */
    inline void GetSchemaId (const tgg00_Rec& Row, Catalog_ObjectId& Id) const;
    /*!
       @brief extracts the container number from a catalog row describing a container
     */
    inline SAPDB_UInt4 GetContainerNo (const tgg00_Rec& Row) const;
    /*!
       @brief extracts the guid from a catalog row describing a container
     */
    inline void GetGuid (const tgg00_Rec& Row, Catalog_GUID& Guid) const;
    /*!
       @brief extracts whether the container is a container of variable length from a catalog row describing a container
     */
    inline bool         GetIsVarContainer (const tgg00_Rec& Row);
    /*!
       @brief extracts the file number from a catalog row describing a container
     */
    inline void         GetFileNo (const tgg00_Rec& Row, FileDir_FileNo& FileNo);
    /*!
       @brief invalidates the entry in the shared catalog cache
     */
    void InvalidateCatalogCache (RTE_TaskId TaskId, const tgg00_Rec& Record);
};

#endif