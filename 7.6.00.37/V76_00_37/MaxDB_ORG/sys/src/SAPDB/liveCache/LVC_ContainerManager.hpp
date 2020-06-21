/*!**************************************************************************

  module      : LVC_ContainerManager.hpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: 
  description : provides functions on oms containers


  last changed: 2001-05-21  16:24
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/

#ifndef LVC_CONTAINERMANAGER_HPP
#define LVC_CONTAINERMANAGER_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"

class SQLMan_Context;
class Catalog_ObjectId;
class Catalog_GUID;
class SQLMan_Identifier;
class FileDir_FileNo;
struct tgg00_FileId;

/*!
   @brief provides functions on oms container, i.e. functions to create and drop containers as well as
          functions returning information about given containers
 */
class LVC_ContainerManager
{
public :
    /*!
       @brief results returned by RegisterContainer
    */
    enum RegistrationResult { REG_ERROR, REG_OK, REG_CREATED };
    /*!
       @brief tells whether a container exists and returns the file, if the container exists
     */
    static bool ExistsContainer
        (
        SQLMan_Context&          Context, 
        SAPDB_UInt4              SchemaId, 
        SAPDB_UInt4              ContainerNo, 
        const Catalog_GUID&      Guid,
        SAPDB_UInt4              ObjectSize,
        tgg00_FileId&            ContainerFile,
        bool&                    UseCachedKeys
        );
    /*!
       @brief drops a container identified by a schema, guid and container number
     */
    static bool DropContainer
        (
        SQLMan_Context&          Context, 
        SAPDB_UInt4              SchemaId, 
        SAPDB_UInt4              ContainerNo, 
        const Catalog_GUID&      Guid 
        );
    /*!
       @brief callback function used by the catalog delete trigger. Whenever a container
              is deleted in the catalog, the corresponding file is dropped via this
              callback function.
     */
    static bool DropContainerCallback 
        (
             SQLMan_Context& Context, 
             FileDir_FileNo& FileNo,
             bool            IsVarContainer
        );
    /*!
       @brief drops all containers of a given schema
     */
    static void DropSchemaContainers 
        (
            SQLMan_Context&         Context,
            const Catalog_ObjectId& SchemaId
        );
    /*!
       @brief yield information about a container identified by its fileNo
     */
    static void GetContainerInfo
        (
        SQLMan_Context&         Context,
        const FileDir_FileNo&   FileNo,
        Catalog_GUID&           Guid, 
        SAPDB_UInt4&            SchemaId, 
        SAPDB_UInt4&            ContainerNo,
        Catalog_ObjectId&       schemaObjectId
        );
    /*!
       @brief registers a container, i.e. creates a container if it does not
              already exists or returns the already existing container
     */
    static RegistrationResult RegisterContainer 
        (
        SQLMan_Context&          Context, 
        SAPDB_UInt4              SchemaId, 
        SAPDB_UInt4              ContainerNo, 
        const Catalog_GUID&      Guid, 
        bool                     KeyPartitioned,
        bool                     CachedKeys,
        bool                     IsVarObjectContainer,
        bool                     NoWait,
        SAPDB_UInt4              Size,
        SAPDB_UInt4              KeyPos,
        SAPDB_UInt4              KeySize,
        const SQLMan_Identifier& ClassName,
        bool                     UseInternalTrans,
        tgg00_FileId&            ContainerFile
        );
};

#endif