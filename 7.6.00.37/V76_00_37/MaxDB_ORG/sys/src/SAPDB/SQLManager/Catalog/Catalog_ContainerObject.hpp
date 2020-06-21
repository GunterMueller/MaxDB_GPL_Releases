/*!
  @file           Catalog_ContainerObject.hpp
  @author         ThomasA 
  @ingroup        Catalog
  @brief          defines the memory representation of an oms class container

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
#ifndef CATALOG_CONTAINEROBJECT_HPP
#define CATALOG_CONTAINEROBJECT_HPP

#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SQLManager/Catalog/Catalog_Object.hpp"
#include "SQLManager/SQLMan_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

class SQLMan_Context;

/*!
   @brief defines the transient representation of a class container in the catalog cache
 */
class Catalog_ContainerObject : public Catalog_Object
{
public :
    /*!
       @brief defines the key of an container object
     */
    class Catalog_Key : public Catalog_Object::Catalog_Key
    {
    public :
        /*!
           @brief constructs the key of an oms class container object
         */
        Catalog_Key(const Catalog_ObjectId& SchemaId, SAPDB_UInt4 ContainerNo, const Catalog_GUID& Guid);

        /*!
           @brief extracts the schema identification from the key
         */
        const Catalog_ObjectId& GetSchemaId() const;
        /*!
           @brief extracts the container number from the key
         */
        SAPDB_UInt4 GetContainerNo() const;
        /*!
           @brief extracts the guid from the key
         */
        const Catalog_GUID& GetGuid() const;
        /*!
           @brief returns the length of the key in bytes 
         */
        static int KeyLength();
    };
    /*!
       @brief creates the container meta data in the catalog table SYSCATALOG.CLASSCONTAINERS
     */
    static bool CreateContainer 
        (
        SQLMan_Context&          Context,
        const Catalog_ObjectId&  SchemaId,
        SAPDB_UInt4              ContainerNo,
        const Catalog_GUID&      Guid,
        int                      NoOfKeyFiles,
        bool                     CachedKeys,
        bool                     IsVarContainer,
        SAPDB_UInt4              Size,
        const FileDir_FileNo&    FileNo,
        const SQLMan_Identifier& ClassName
        );
    /*!
       @brief drops a class containers by deleting the corresponding row in the catalog table
     */
    static bool Drop
        (
        SQLMan_Context&          Context,
        const Catalog_ObjectId&  SchemaId,
        SAPDB_UInt4              ContainerNo,
        const Catalog_GUID&      Guid
        );
    /*!
       @brief drops all class container of a given schema
     */
    static void DropSchemaContainers 
        (
            SQLMan_Context&         Context,
            const Catalog_ObjectId& SchemaId
        );
    /*!
       @brief yields properties of a class container
     */
    static void GetContainerInfo 
        (
        SQLMan_Context&         Context,
        const FileDir_FileNo&   FileNo,
        Catalog_GUID&           Guid, 
        Catalog_ObjectId&       SchemaObjectId, 
        SAPDB_UInt4&            ContainerNo
        );
    /*!
       @brief returns the transient key of the class container used inside the catalog cache
     */
    const Catalog_Key& GetKey() const;
    /*!
       @brief returns the identification of the file representing the class container 
     */
    FileDir_FileNo GetFileNo() const;
    /*!
       @brief returns the size of a class instance stored inside the container
     */
    SAPDB_UInt4 GetSize() const;
    /*!
       @brief reads the meta data of the container identified by its key from the catalog 
     */
    static Catalog_ContainerObject* Incopy (SQLMan_Context& Context, const Catalog_Object::Catalog_Key& Key, SAPDBMem_IRawAllocator& Allocator, bool NoWait);
    /*!
       @brief reads the meta data of the container identified by its schemaid, container number and guid from the catalog 
     */
    Catalog_Object::IncopyResult Incopy (
                                      SQLMan_Context&          Context, 
                                      const Catalog_ObjectId&  SchemaId,
                                      SAPDB_UInt4              ContainerNo,
                                      Catalog_GUID             Guid,
                                      bool                     NoWait = false);

    /*!
       @brief returns true, if the container does not contain instances of fixed length
     */
    bool IsVarObjectContainer() const;
    /*!
       @brief locks or unlocks the row identified by SchemaId, ContainerNo and Guid in SYSCATALOG.CLASSCONTAINERS
     */
    static bool Lock(
        SQLMan_Context&          Context,
        const Catalog_ObjectId&  SchemaId,
        SAPDB_UInt4              ContainerNo,
        const Catalog_GUID&      Guid,
        bool                     DoUnlock,
        bool                     NoWait);
    /*!
       @brief writes the transient representation of the container into the catalog table
     */
    bool Outcopy (SQLMan_Context& Context);
    /*!
       @brief returns the UseCachedKey property
     */
    bool UseCachedKeys() const;
private :
    /*!
       brief constructor
     */
    Catalog_ContainerObject(
        const Catalog_ObjectId&  SchemaId,
        SAPDB_UInt4              ContainerNo,
        const Catalog_GUID&      Guid,
        int                      NoOfKeyFiles,
        bool                     CachedKeys,
        bool                     IsVarContainer,
        SAPDB_UInt4              Size,
        const FileDir_FileNo&    FileNo,
        const SQLMan_Identifier& ClassName);

    /*!
       @brief defines the transient representation of a class container
     */
    struct ContainerInfo {
        SAPDB_Int2        m_recLen;
        SAPDB_Int2        m_keyLen;
        SAPDB_Int2        m_dummy1;
        SAPDB_Int2        m_dummy2;
        Catalog_ObjectId  m_schemaId;
        tsp00_C2          m_entryType;
        tsp00_C2          m_linkage;
        SAPDB_UInt4       m_containerNo;
        Catalog_GUID      m_guid;
        int               m_noOfKeyFiles;
        bool              m_isVarContainer;
        bool              m_cachedKeys;
        SAPDB_UInt4       m_size;
        FileDir_FileNo    m_fileNo;
        char              m_className[64];
        ContainerInfo()
            : m_recLen(sizeof(ContainerInfo))
            , m_keyLen(sizeof(m_schemaId) + sizeof(m_entryType) + sizeof(m_linkage) + sizeof(m_containerNo) + sizeof(m_guid))
        {
        }
    };
};

#endif