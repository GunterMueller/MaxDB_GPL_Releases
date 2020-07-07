/*****************************************************************************/
/*!
  @file         FileDir_SingleIndexEntry.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_SingleIndexEntry.

\if EMIT_LICENCE
  ========== licence begin  GPL
  Copyright (C) 2005 SAP AG

  This program is free software; you can redisytribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
\endif
*/
/*****************************************************************************/

#ifndef FILEDIR_SINGLEINDEXENTRY_HPP
#define FILEDIR_SINGLEINDEXENTRY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_IndexEntry.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_FileTypes.hpp"
#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @author  MartinKi
 * @ingroup FileDirectory_Classes
 *
 * @brief File directory entry class for a single index; only used
 * during migraton from 7.3
 *
 * This class represents a single index on disk. It will not be
 * entered in the regular file directory hash and is only accessible
 * via special methods in FileDir_SharedDirectory. This object allows
 * changing its type from Data_SingleIndexFile to
 * Data_IndexFile. Since it is derived from FileDir_IndexEntry is can
 * then act as a regular index entry. The object also allows changing
 * its index id. This behaviour is necessary during the migration of
 * the SQL catalog.
 */
class FileDir_SingleIndexEntry : public FileDir_IndexEntry
{
private:
    friend class FileDir_SharedDirectory;
    friend class FileDir_Directory;

    //no copy-construction or assignment
    FileDir_SingleIndexEntry(const FileDir_SingleIndexEntry&);
    FileDir_SingleIndexEntry operator=(FileDir_SingleIndexEntry&);

public:
    /*!
     * @brief Constructor for a single index entry.
     */
    FileDir_SingleIndexEntry(
        const RTE_TaskId         taskId,
        SAPDBMem_IRawAllocator&  allocator,
        const FileDir_FileNo     fileNo,
        const Converter_Version& converterVersion,
        const FileDir_FileNo     parentFileNo,
        const SAPDB_UInt2        indexId,
        const Data_PageNo        rootPageNo,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion);

    /// Recreate from persistent data.
    FileDir_SingleIndexEntry(
        SAPDBMem_IRawAllocator& allocator,
        FileDir_Entry&          persistentEntry);

    /// Destructor.
    ~FileDir_SingleIndexEntry();

    /*!
     * @brief Returns the file type. Either Data_SingleIndexFile or
     * Data_IndexFile.
     */
    Data_FileType GetFileType() const;

    /*!
     * @brief Persistently switches the file type of this entry to
     * Data_IndexFile by updating the file type on the file directory
     * data page.
     */
    SAPDB_Bool SwitchTypeToIndex(RTE_TaskId taskId);

    /*!
     * @brief Persistently changes the index id of this object to
     * indexId by updating the index id on the file directory data
     * page.
     */
    void SetIndexId(RTE_TaskId taskId, SAPDB_UInt2 indexId);
};

#endif // FILEDIR_SINGLEINDEXENTRY_HPP
