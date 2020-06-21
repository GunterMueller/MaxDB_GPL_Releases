/*****************************************************************************/
/*!

  @file         FileDir_ISharedDirectory.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for interface class FileDir_ISharedDirectory.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
/*****************************************************************************/

#ifndef FILEDIR_ISHAREDDIRECTORY_HPP
#define FILEDIR_ISHAREDDIRECTORY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_Types.hpp"
#include "Converter/Converter_Version.hpp"
#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
class FileDir_BaseEntry;
class FileDir_ITableEntry;
class FileDir_Iterator;
class FileDir_GenericDirectory;
class FileDir_IDirectory;
class Converter_Version;
class Kernel_Dump;

struct tgg00_TransContext;
class SAPDBMem_IRawAllocator;

/*!
 * @class FileDir_ISharedDirectory
 * @ingroup FileDirectory_Interfaces
 * @author MartinKi
 *
 * @brief The FileDirectory interface class. This class defines all
 *        publicly accessible methods that are special for the
 *        persistent file directory.
 *
 * Regular users of the file directory need not include this file but
 * should use FileDir_Directory.hpp instead.
 *
 * @sa FileDir_Generic
 */
class FileDir_ISharedDirectory
{
public:
    /*!
     * @brief Destructor. Shutdown() must have been called prior to
     *        calling this destructor.
     */
    virtual ~FileDir_ISharedDirectory() {};

    /// Returns a reference to file directory singleton.
    static FileDir_ISharedDirectory& Instance();

    /// Returns a reference to the FileDir_IDirectory interface of the
    /// shared file directory.
    static FileDir_IDirectory& IFileDirInstance();

    /*!
     * @name Startup/Shutdown/Verify
     *
     */
    /*@{*/

    /*!
     * @brief Creates a new file directory starting with rootPageNo.
     *
     * @copydoc FileDir_SharedDirectory::Create
     */
    virtual FileDir_ReturnCode Create(RTE_TaskId taskId) = 0;

    /*!
     * @brief Restarts the file directory by loading all file
     *        directory pages into memory and creating all file
     *        directory entries valid for converterVersion.
     *
     * @copydoc FileDir_SharedDirectory::RestartFDir
     */
    virtual FileDir_ReturnCode RestartFDir(
        tgg00_TransContext&      trans,
        const Data_PageNo&       rootPageNo,
        const Converter_Version& converterVersion,
        const Converter_Version& restartConverterVersion,
        const SAPDB_Bool         cleanupFileDirPages,
        Msg_List&                msgList) = 0;

    /*!
     * @brief Shut down the file directory. All in-memory structures
     *        for this FileDir-object and its entries are freed. No
     *        references to any file directory entries must exist when
     *        calling this method.
     *
     * @copydoc FileDir_SharedDirectory::Shutdown
     */
    virtual FileDir_ReturnCode ShutdownSharedFDir(
        RTE_TaskId taskId = RTE_UnknownTaskId) = 0;

     /*!
     * @brief Return the rootPageNo of the file directory
     *
     * @copydoc FileDir_SharedDirectory::GetRootPageNo
     */
    virtual Data_PageNo GetRootPageNo(RTE_TaskId taskId = RTE_UnknownTaskId) = 0;

     /*!
     * @brief Checks all file directory pages and marks them used.
     */
    virtual FileDir_ReturnCode CheckDataWithUpdate(RTE_TaskId taskId) = 0;

     /*!
     * @brief Checks all file directory pages.
     */
    virtual FileDir_ReturnCode CheckData(RTE_TaskId taskId) = 0;
    /*@}*/

    /*!
     * @name Adding new entries/deleting entries
     */
    /*@{*/

    /*!
     * @brief Return a new unused fileNo. This function does not
     *        recycle entries or check for duplicate fileNos.
     */
    virtual FileDir_FileNo GetNewFileNo() = 0;

    /*!
     * @brief Return a new unused fileNo for a temporary object. This
     *        function does not recycle entries or check for duplicate
     *        fileNos.
     */
    virtual FileDir_FileNo GetNewTempFileNo() = 0;

    /*!
     * @brief Return a new unused fileNo for a temporary object that
     *        is shared among all sessions. This function does not
     *        recycle entries or check for duplicate fileNos.
     */
    virtual FileDir_FileNo GetNewTempSharedFileNo() = 0;

    /*@}*/

    /*!
     * @name Savepoint related
     */
    /*@{*/

    /*!
     * @brief Informs the file directory that a savepoint was initiated.
     *
     * @copydoc FileDir_SharedDirectory::BeginSavepoint
     */
    virtual void BeginSavepoint(RTE_TaskId taskId = RTE_UnknownTaskId) = 0;

    /*!
     * @brief Tells file directory to flush all dirty pages to disk.
     *
     * @copydoc FileDir_SharedDirectory::Flush
     */
    virtual void Flush(RTE_TaskId taskId = RTE_UnknownTaskId) = 0;

    /*!
     * @brief Tells file directory that savepoint has completed. All
     *        entries will have their persistent data parts updated
     *        with any deltas that might have accumulated during the
     *        savepoint.
     *
     * @copydoc FileDir_SharedDirectory::EndSavepoint
     */
    virtual void EndSavepoint(
        RTE_TaskId               taskId,
        const Converter_Version& newConverterVersion ) = 0;

    /*!
     * @brief Returns true if a savepoint is in progress.
     */
    virtual SAPDB_Bool IsSavepointInProgress() = 0;
    /*@}*/

    /*!
     * @brief Returns the allocator of the directory.
     */
//    virtual SAPDBMem_IRawAllocator& GetAllocator() const = 0;

    /*!
     * @brief Returns the highest file id that has been handed out by
     *        the file directory.
     */
    virtual FileDir_FileNo GetMaxFileNo() const = 0;

    /*!
     * @brief Returns the highest file id for temp-files that has been
     *        handed out by the file directory.
     */
    virtual FileDir_FileNo GetMaxTempFileNo() const = 0;

    /*!
     * @brief Returns an iterator over all file directory entries.
     *
     * @copydoc FileDir_SharedDirectory::GetFileDirIterator
     */
    virtual SAPDB_Bool GetFileDirIterator(
        FileDir_Iterator&   iterator,
        tgg00_TransContext* transContext = 0) = 0;

    /*!
     * @brief Returns the number of pages on disk used by the file
     *        directory.
     */
    virtual SAPDB_UInt GetPageCount() const = 0;

    /*!
     * @brief Marks the given page to be flushed to disk during the
     * next savepoint.
     *
     * @copydoc FileDir_SharedDirectory::MarkPageForWrite
     */
    virtual SAPDB_Bool MarkPageForWrite(
        const RTE_TaskId   taskId,
        const Data_PageNo& pageNo) = 0;

    /*!
     * @brief Write contents of shared directory to kernel dump.
     *
     * @copydoc FileDir_SharedDirectory::Dump
     */
    virtual void Dump(const RTE_TaskId taskId, Kernel_Dump& dump) = 0;

    /*!
     * @brief Write contents of shared file directory pages to file.
     *
     * @copydoc FileDir_SharedDirectory::Extract
     */
    virtual void Extract(
        const RTE_TaskId       taskId,
        const tsp00_VFilename& fileName) = 0;

    /*!
     * @name Statistics
     */
    /*@{*/

    /*!
     * @brief Returns the number of entries in the directory.
     */
    virtual SAPDB_UInt GetEntryCount() const = 0;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    virtual SAPDB_UInt GetHashTableSlotCount() const = 0;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    virtual SAPDB_UInt GetUsedHashTableSlotCount() const = 0;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    virtual SAPDB_UInt GetMaxHashTableChainEntryCount() const = 0;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    virtual SAPDB_UInt GetResizeHashTableCount() const = 0;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    virtual SAPDB_UInt GetStatisticsDeltaCount() const = 0;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    virtual SAPDB_UInt GetCurrentStatisticsDeltaCount() const = 0;
    /*@}*/


    /*!
     * @name Bad Indices
     */
    /*@{*/

    /*!
     * @brief Returns the number of bad indices in the shared file directory.
     */
    virtual SAPDB_UInt4 GetBadIndexCount() const = 0;

    /*!
     * @brief Increments the number of bad indices in the shared file
     *        directory.
     */
     virtual void IncrementBadIndexCount() = 0;

    /*!
     * @brief Increments the number of bad indices in the shared file
     *        directory.
     */
     virtual void DecrementBadIndexCount() = 0;

    /*@}*/


    /*!
     * @name Migration related
     */
    /*@{*/

    /*!
     * @brief Increase the highest internal file no counter used for
     * GetNewFile and MigrationGetNewFileNo by a constant value.
     *
     * @copydoc FileDir_SharedDirectory::AddFileNoSafetyMargin
     */
    virtual void AddFileNoSafetyMargin() = 0;

    /*!
     * @brief Returns a new unused fileNo. This method can only be
     *        used during a file directory migration.
     *
     * @copydoc FileDir_SharedDirectory::MigrationGetNewFileNo
     */
    virtual FileDir_FileNo MigrationGetNewFileNo() = 0;

    /*!
     * @brief Adds a short column file whose table is not present in
     *        the file directory. Necessary to be able to
     *        roll back/roll forward log from earlier releases.
     *
     * @copydoc FileDir_SharedDirectory::AddLooseShortColumnFile
     */
    virtual FileDir_ReturnCode AddLooseShortColumnFile(
        const RTE_TaskId         taskId,
        const FileDir_FileNo&    fileNo,
        const FileDir_FileNo&    parentFileNo,
        const Data_PageNo        rootPage,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion,
        Msg_List&                msgList) = 0;

    /*!
     * @brief Adds an index file whose table is not present in
     *        the file directory. Necessary to be able to
     *        roll back/roll forward log from earlier releases.
     *
     * @copydoc FileDir_SharedDirectory::AddLooseIndexFile
     */
    virtual FileDir_ReturnCode AddLooseIndexFile(
        const RTE_TaskId         taskId,
        const FileDir_FileNo&    fileNo,
        const FileDir_FileNo&    parentFileNo,
        const SAPDB_UInt2        indexId,
        const Data_PageNo        rootPage,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion,
        Msg_List&                msgList) = 0;
    /*@}*/
};
#endif // FILEDIR_ISHAREDDIRECTORY_HPP
