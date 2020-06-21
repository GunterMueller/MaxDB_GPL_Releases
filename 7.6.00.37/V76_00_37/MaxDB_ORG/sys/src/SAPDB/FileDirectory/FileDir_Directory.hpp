/*****************************************************************************/
/*!
  @file         FileDir_Directory.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_Directory.

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

#ifndef FILEDIR_DIRECTORY_HPP
#define FILEDIR_DIRECTORY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
// for old file directory compatibility
#include "gbd00.h"
#include "ggg00.h"
class FileDir_Compat;
class Converter_Version;
// end of  old file directory compatibility

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_GenericDirectory.hpp"
#include "FileDirectory/FileDir_PointerCache.hpp"
#include "FileDirectory/FileDir_RWRegionWrapper.hpp"

#include "DataAccess/Data_FileTypes.hpp"
#include "DataAccess/Data_Types.hpp"

#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "hkb50.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

struct tgg00_TransContext;

class SAPDBMem_IRawAllocator;
class Converter_Version;

class FileDir_BaseEntry;
class FileDir_Entry;
class FileDir_Generic;
class FileDir_Table;
class FileDir_Index;
class FileDir_ShortColumn;
class FileDir_Oms;
class FileDir_OmsKey;
class FileDir_OmsVar;
class FileDir_OmsFixed;
class FileDir_HashBucket;
class FileDir_PageManager;
class FileDir_SharedDirectory;
class FileDir_Iterator;

class Kernel_BlobDescription;
class Kernel_IndexDescription;

class FileDir_RWRegionWrapper;
class RTESync_SpinlockPool;
class tgg91_FileVersion;

class Msg_List;

/*!
 * @class FileDir_Directory
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief The FileDir_Directory class. This class manages the File Directory.
 *
 * Users of the file directory should not include this file but use
 * FileDir_IDirectory.hpp instead.
 * @sa FileDir_IDirectory
 */
class FileDir_Directory : public FileDir_GenericDirectory
{
/***************************** Methods *****************************/

public:
     /*!
     * @brief Constructs a file directory object
     */
    FileDir_Directory(SAPDBMem_IRawAllocator& alloc);

    /*!
     * @brief Initializes this object.
     */
    SAPDB_Bool Initialize(SAPDB_Int4 initialCapacity = 0);

    /*!
     * @brief Returns the allocator of the directory.
     */
    SAPDBMem_IRawAllocator& GetAllocator() const;

    SAPDB_Bool GetFileEntry(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      fileEntry) const;

    SAPDB_UInt GetCacheEntryCount() const;

private:
    // Copy and assignment operator are not supported
    FileDir_Directory& operator=( const FileDir_Directory &fdir );
    FileDir_Directory( const FileDir_Directory &fdir );

    // Allow iterator access to private members.
    friend class FileDir_Iterator;
    friend class FileDir_Test;

    enum FileDir_Status {
        stOk,
        stShuttingDown
    };

    FileDir_SharedDirectory&     m_sharedFd;
    mutable FileDir_PointerCache m_pointerCache;


    bool m_initialized;

    /// Counts the number of files deleted since file directory
    /// instantiation.
    SAPDB_UInt              m_delOpCounter;

    /// necessary to coordinate iterators and shutdown of file directory
    FileDir_Status m_status;
    SAPDB_Int      m_iteratorCounter;

    FileDir_ReturnCode AddEntry(
        const RTE_TaskId         taskId,
        const FileDir_FileNo&    fileNo,
        FileDir_BaseEntry*       entry,
        Msg_List&                msgList);


    SAPDB_Bool RegisterIterator(FileDir_IteratorObj& iterator);

    void UnregisterIterator(FileDir_IteratorObj& iterator);

    IteratorGetRc IteratorGetFile(
        tgg00_TransContext*   transContext,
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      p) const;

    SAPDBMem_IRawAllocator& GetEntryAllocator(FileDir_FileNo fileNo) const;

    FileDir_ReturnCode GetEntry(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      p,
        Msg_List&             msgList) const;

    FileDir_ReturnCode GetEntryPointer(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      p,
        Msg_List&             msgList);

public:
    /*!
     * @name Startup/Shutdown
     */
    /*@{*/


    /*!
     * @brief Shut down the file directory. All in-memory structures
     *        for this FileDir-object and its entries are freed. No
     *        references to any file directory entries must exist when
     *        calling this method.
     *
     * @param taskId [in] Id of the calling task
     */
    FileDir_ReturnCode Shutdown(RTE_TaskId taskId);

    /*@}*/

    /*!
     * @name Adding new entries/deleting entries
     */
    /*@{*/


    /*!
     * @brief Adds a file directory entry for a table.
     *
     * @copydoc FileDir_GenericDirectory::AddTableFile
     */
    FileDir_ReturnCode AddTableFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPage,
        const SAPDB_UInt2        blobColCount,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion,
        Msg_List&                msgList,
        FileDir_Table*           pointer = 0);

    /*!
     * @brief Adds a file directory entry for a temporary table that
     *        should be accessible to all users.
     *
     * @copydoc FileDir_GenericDirectory::AddSharedTempTableFile
     */
    FileDir_ReturnCode AddSharedTempTableFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPage,
        const SAPDB_UInt2        blobColCount,
        const tgg00_FiletypeSet& fileTypeSet,
        Msg_List&                msgList,
        FileDir_Table*           pointer = 0);

    /*!
     * @brief Adds a file directory entry for an index.
     *
     * @copydoc FileDir_GenericDirectory::AddIndexFile
     */
    FileDir_ReturnCode AddIndexFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const FileDir_FileNo&    parentFileNo,
        const Converter_Version& converterVersion,
        const SAPDB_UInt2        indexId,
        const Data_PageNo        rootPage,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion,
        Msg_List&                msgList,
        FileDir_Index*           indexPointer = 0);

    /*!
     * @brief Adds a file directory entry for a short column file.
     *
     * @copydoc FileDir_GenericDirectory::AddShortColumnFile
     */
    FileDir_ReturnCode AddShortColumnFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const FileDir_FileNo&    parentFileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo        rootPage,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion,
        Msg_List&                msgList,
        FileDir_ShortColumn*     shortColPointer = 0);

    /*!
     * @brief Adds a file directory entry for an OMS object.
     *
     * @copydoc FileDir_GenericDirectory::AddOmsFile
     */
    FileDir_ReturnCode AddOmsFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPage,
        const SAPDB_UInt2        objBodySize,
        const SAPDB_UInt2        objPerPageCount,
        const SAPDB_UInt2        chainCount,
        Msg_List&                msgList,
        FileDir_Oms*             pointer = 0);

    /*!
     * @brief Adds a file directory entry for an OMS key object.
     *
     * @copydoc FileDir_GenericDirectory::AddOmsKeyFile
     */
    FileDir_ReturnCode AddOmsKeyFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPage,
        const SAPDB_UInt2        objBodySize,
        const SAPDB_UInt2        objPerPageCount,
        const SAPDB_UInt2        chainCount,
        const SAPDB_UInt2        keyPartitionCount,
        Msg_List&                msgList,
        FileDir_OmsKey*          pointer = 0);

    /*!
     * @brief Adds a file directory entry for an OMS key partition file.
     *
     * @copydoc FileDir_GenericDirectory::AddOmsKeyPartitionFile
     */
    FileDir_ReturnCode AddOmsKeyPartitionFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const FileDir_FileNo&    parentFileNo,
        const Converter_Version& converterVersion,
        const SAPDB_UInt2        partitionNo,
        const Data_PageNo&       rootPage,
        Msg_List&                msgList,
        FileDir_OmsKeyPartition* omsKeyPartitionPointer);

    /*!
     * @brief Adds a file directory entry for an OMS var object.
     *
     * @copydoc FileDir_GenericDirectory::AddOmsVarFile
     */
    FileDir_ReturnCode AddOmsVarFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPage,
        const SAPDB_UInt2        objBodySize,
        const SAPDB_UInt2        objPerPageCount,
        const SAPDB_UInt2        chainCount,
        Msg_List&                msgList,
        FileDir_OmsVar*          pointer = 0);

    /*!
     * @brief Adds a file directory entry for an OMS fixed object.
     *
     * @copydoc FileDir_GenericDirectory::AddOmsFixedFile
     */
    FileDir_ReturnCode AddOmsFixedFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPage,
        const SAPDB_UInt2        objBodySize,
        const SAPDB_UInt2        objPerPageCount,
        const SAPDB_UInt2        chainCount,
        Msg_List&                msgList,
        FileDir_OmsFixed*        pointer = 0);

    /*!
     * @brief Removes a file directory entry from the file directory
     *        and destroys it. This method should only be called by
     *        the garbage collector.
     *
     * @copydoc FileDir_GenericDirectory::DelFile
     */
    FileDir_ReturnCode DelFile(
        RTE_TaskId            taskId,
        const FileDir_FileNo& fileNo,
        Msg_List&             msgList);

    /*@}*/

    /*!
     * @name Retrieving entries
     */
    /*@{*/

    /*!
     * @brief Sets a table smart pointer to the file directory entry for
     *        fileNo.
     *
     * @copydoc FileDir_GenericDirectory::GetTableFile
     */
    FileDir_ReturnCode GetTableFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Table&        p,
        Msg_List&             msgList);


    FileDir_ReturnCode GetShortColumnFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_ShortColumn&  p,
        Msg_List&             msgList);

    /*!
     * @brief Sets an index smart pointer to the file directory entry for
     *        fileNo.
     *
     * @copydoc FileDir_GenericDirectory::GetIndexFile
     */
    FileDir_ReturnCode GetIndexFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Index&        p,
        Msg_List&             msgList);

    /*!
     * @brief Sets an OMS smart pointer to the file directory entry for
     *        fileNo.
     *
     * @copydoc FileDir_GenericDirectory::GetOmsFile
     */
    FileDir_ReturnCode GetOmsFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Oms&          p,
        Msg_List&             msgList);

    /*!
     * @brief Sets an OMSkey smart pointer to the file directory entry for
     *        fileNo.
     *
     * @copydoc FileDir_GenericDirectory::GetOmsKeyFile
     */
    FileDir_ReturnCode GetOmsKeyFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_OmsKey&       p,
        Msg_List&             msgList);


    /*!
     * @brief Sets an OMS var smart pointer to the file directory entry for
     *        fileNo.
     *
     * @copydoc FileDir_GenericDirectory::GetOmsVarFile
     */
    FileDir_ReturnCode GetOmsVarFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_OmsVar&       p,
        Msg_List&             msgList);

    /*!
     * @brief Sets an OMS fixed smart pointer to the file directory entry for
     *        fileNo.
     *
     * @copydoc FileDir_GenericDirectory::GetOmsFixedFile
     */
    FileDir_ReturnCode GetOmsFixedFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_OmsFixed&     p,
        Msg_List&             msgList);

    /*!
     * @brief Sets a smart pointer to the file directory entry for
     *        fileNo. Use of this method should be avoided when
     *        possible. The typed methods GetTableFile(),
     *        GetIndexFile() etc. should be used instead.
     *
     * @copydoc FileDir_GenericDirectory::GetFile
     */
    FileDir_ReturnCode GetFile(
        const RTE_TaskId&     taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      p,
        Msg_List&             msgList);

    /*!
     * @brief For use by garbage collector only. Sets a smart pointer
     *        to the file directory entry for fileNo.
     *
     * @copydoc FileDir_GenericDirectory::GcGetFile
     */
    FileDir_ReturnCode GcGetFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      p,
        Msg_List&             msgList);

    /*@}*/

    /*!
     * @brief Set the highest file id of the file directory. If
     *        highestFileNo is less than the internal file id counter,
     *        the internal file id counter will not be
     *        changed. <em>Can only be called during restart!</em>
     *
     * @copydoc FileDir_SharedDirectory::Restart_SetFileNoHighWaterMark
     */
    void Restart_SetFileNoHighWaterMark(FileDir_FileNo highestFileNo);

    /*!
     * @brief Returns the number of file entries that have been marked
     *        as deleted since restart. This counter may wrap around,
     *        so tests for changes should not rely on the greater
     *        operator but use != instead.
     *
     * @copydoc FileDir_GenericDirectory::GetDeleteOpCounter
     */
    SAPDB_UInt GetDeleteOpCounter() const;

    /*!
     * @brief Returns an iterator over all (temporary and persistent)
     *        file directory entries.
     *
     * @param iterator [in/out] the iterator to initialize
     * @param transContext [in] if not 0 only file directory entries
     *                          visible within transContext are returned
     */
    SAPDB_Bool GetFileDirIterator(
        FileDir_Iterator&   iterator,
        tgg00_TransContext* transContext = 0);

    /*!
     * @brief Returns an iterator over all private temporary file
     *        directory entries.
     *
     * @param iterator [in/out] the iterator to initialize
     * @param transContext [in] if not 0 only file directory entries
     *                          visible within transContext are returned
     */
    SAPDB_Bool GetPrivateFileDirIterator(
        FileDir_Iterator&   iterator,
        tgg00_TransContext* transContext = 0);

    /*!
     * @brief Returns an iterator over all (temporary and persistent)
     *        shared file directory entries.
     *
     * @param iterator [in/out] the iterator to initialize
     * @param transContext [in] if not 0 only file directory entries
     *                          visible within transContext are returned
     */
    SAPDB_Bool GetSharedFileDirIterator(
        FileDir_Iterator&   iterator,
        tgg00_TransContext* transContext = 0);

    /*!
     * @brief Removes pointers to deleted entries from pointer cache.
     */
    void CleanUpPointerCache();

    /*!
     * @brief Sets the cached lock information of all file directory
     * entries in the cache to lockState.
     */
    void SetAllCachedLockStates(const SAPDB_Int lockState);

    friend class FileDir_IteratorObj;
};

/**********************************************************************/

#endif // FILEDIR_DIRECTORY_HPP
