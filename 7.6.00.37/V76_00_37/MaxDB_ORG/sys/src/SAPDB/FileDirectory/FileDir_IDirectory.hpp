/*****************************************************************************/
/*!

  @file         FileDir_IDirectory.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Interfaces

  @brief        Interface to session local file directory.

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

#ifndef FILEDIR_IDIRECTORY_HPP
#define FILEDIR_IDIRECTORY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_Vector.hpp"
#include "DataAccess/Data_FileTypes.hpp"
#include "DataAccess/Data_Types.hpp"
#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "Transaction/Trans_Context.hpp"

#include "ggg91.h" // tgg91_FileVersion
#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
class FileDir_BaseEntry;
class FileDir_ITableEntry;

struct tgg00_TransContext;
class  Trans_Context;
class Converter_Version;
class Kernel_Dump;
class SAPDBMem_IRawAllocator;
class Msg_List;

class FileDir_Generic;
class FileDir_Table;
class FileDir_Index;
class FileDir_ShortColumn;
class FileDir_Oms;
class FileDir_OmsKey;
class FileDir_OmsKeyPartition;
class FileDir_OmsFixed;
class FileDir_OmsVar;
class FileDir_Iterator;

/*!
 * @class FileDir_IDirectory
 * @ingroup (FileDirectory_Classes FileDirectory_Interfaces)
 * @author MartinKi
 *
 * @brief The FileDirectory interface class. This class defines all
 *        publicly accessible methods of the File Directory.
 *
 * Users of the file directory should only include this file and -as
 * necessary- the following files:
 *
 * - FileDir_Generic.hpp
 * - FileDir_Table.hpp
 * - FileDir_Index.hpp
 * - FileDir_Oms.hpp
 * - FileDir_OmsVar.hpp
 * - FileDir_OmsKey.hpp
 * - FileDir_OmsFixed.hpp
 *
 * It should not be necessary to include any more files for file
 * directory operations. For some special operations (startup,
 * savepoint, shutdown) that affect only the persistent part of the
 * file directory it may be necessary to include
 * FileDir_ISharedDirectory.hpp as well.
 */
class FileDir_IDirectory
{
public:
    /*!
     * @name Startup/Shutdown
     */
    /*@{*/

    /*!
     * @brief Initializes FileDirectory object.
     *
     * @copydoc FileDir_Directory::Initialize
     */
    virtual SAPDB_Bool Initialize(SAPDB_Int4 initialCapacity = 0) = 0;

    /*!
     * @brief Shuts down this (private) file directory. The persistent
     *        file directory is not affected.
     *
     * @copydoc FileDir_Directory::Shutdown
     */
    virtual FileDir_ReturnCode Shutdown(RTE_TaskId taskId) = 0;

    /*!
     * @brief Destructor. Shutdown() must have been called prior to
     *        calling this destructor.
     */
    virtual ~FileDir_IDirectory() {};

    /*!
     * @brief Returns the session-local file directory if available,
     *        otherwise the shared file directory will be returned.
     */
    static FileDir_IDirectory& Instance(const tgg00_TransIndex trIndex);
    // FileDir_IDirectory& Instance(const tgg00_TransIndex trIndex) is
    // defined in FileDir_Directory.cpp


    /*!
     * @brief Returns the session-local file directory. Must _not_ be
     *         called outside of a session.
     *
     * This method works faster than the Instance method using
     * tgg00_TransIndex. If possible it should be preferred over the
     * plain Instance method.
     */
    static FileDir_IDirectory& SessionInstance(
        const tgg00_TransContext& trans);

    /*!
     * @brief Returns the session-local file directory. Must _not_ be
     *         called outside of a session.
     *
     * This method works faster than the Instance method using
     * tgg00_TransIndex. If possible it should be preferred over the
     * plain Instance method.
     */
    static FileDir_IDirectory& SessionInstance(
        const Trans_Context& trans);

    /*@}*/

    /*!
     * @name Adding new entries/deleting entries
     */
    /*@{*/

    /*!
     * @brief Returns a new unused fileNo. This function does not
     *        recycle entries or check for duplicate fileNos.
     */
    virtual FileDir_FileNo GetNewFileNo() = 0;

    /*!
     * @brief Returns a new unused fileNo for a temporary object. This
     *        function does not recycle entries or check for duplicate
     *        fileNos.
     */
    virtual FileDir_FileNo GetNewTempFileNo() = 0;

    /*!
     * @brief Returns a ready to use tgg00_FileId for a table file
     * with file no fileNo.
     *
     * @param tableFileNo [in] file no of table file
     * @param fileTypeSet [in] file type of table file
     * @param fileVersion [in] file version of
     * @param tableFileId [out] resulting fileId
     */
    // implemented in FileDir_GenericDirectory.cpp
	static void BuildNewTableFileId(
		const FileDir_FileNo&    tableFileNo,
        const tgg00_FiletypeSet& fileTypeSet,
		const tgg91_FileVersion& fileVersion,
        tgg00_FileId&            tableFileId);

    /*!
     * @brief Returns a ready to use tgg00_FileId for an OMS file of
     *        type file type and with file no fileNo.
     */
    // implemented in FileDir_GenericDirectory.cpp
	static void BuildNewOmsFileId(
		const FileDir_FileNo&    fileNo,
		const tgg00_ObjFileType& fileType,
        const SAPDB_UInt1&       partitionNo,
        tgg00_FileId&            fileId);

    /*!
     * @brief Adds a file directory entry for a table.
     *
     * @copydoc FileDir_Directory::AddTableFile
     */
    virtual FileDir_ReturnCode AddTableFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPage,
        const SAPDB_UInt2        blobColCount,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion,
        Msg_List&                msgList,
        FileDir_Table*           pointer = 0) = 0;

    /*!
     * @brief Adds a file directory entry for a temporary table that
     *        should be accessible to all users.
     *
     * @copydoc FileDir_Directory::AddSharedTempTableFile
     */
    virtual FileDir_ReturnCode AddSharedTempTableFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPage,
        const SAPDB_UInt2        blobColCount,
        const tgg00_FiletypeSet& fileTypeSet,
        Msg_List&                msgList,
        FileDir_Table*           pointer = 0) = 0;

    /*!
     * @brief Adds a file directory entry for an index.
     *
     * @copydoc FileDir_Directory::AddIndexFile
     */
    virtual FileDir_ReturnCode AddIndexFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const FileDir_FileNo&    parentFileNo,
        const Converter_Version& converterVersion,
        const SAPDB_UInt2        indexId,
        const Data_PageNo        rootPage,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion,
        Msg_List&                msgList,
        FileDir_Index*           indexPointer = 0) = 0;

    /*!
     * @brief Adds a file directory entry for a short column file.
     *
     * @copydoc FileDir_Directory::AddShortColumnFile
     */
    virtual FileDir_ReturnCode AddShortColumnFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const FileDir_FileNo&    parentFileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo        rootPage,
        const tgg00_FiletypeSet& fileTypeSet,
        const tgg91_FileVersion& fileVersion,
        Msg_List&                msgList,
        FileDir_ShortColumn*     shortColPointer = 0) = 0;

    /*!
     * @brief Adds a file directory entry for an OMS object.
     *
     * @copydoc FileDir_Directory::AddOmsFile
     */
    virtual FileDir_ReturnCode AddOmsFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPage,
        const SAPDB_UInt2        objBodySize,
        const SAPDB_UInt2        objPerPageCount,
        const SAPDB_UInt2        chainCount,
        Msg_List&                msgList,
        FileDir_Oms*             pointer = 0) = 0;

    /*!
     * @brief Adds a file directory entry for an OMS key object.
     *
     * @copydoc FileDir_Directory::AddOmsKeyFile
     */
    virtual FileDir_ReturnCode AddOmsKeyFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPage,
        const SAPDB_UInt2        objBodySize,
        const SAPDB_UInt2        objPerPageCount,
        const SAPDB_UInt2        chainCount,
        const SAPDB_UInt2        keyPartitionCount,
        Msg_List&                msgList,
        FileDir_OmsKey*          pointer = 0) = 0;

    /*!
     * @brief Adds a file directory entry for an OMS key partition.
     *
     * @copydoc FileDir_Directory::AddOmsKeyPartitionFile
     */
    virtual FileDir_ReturnCode AddOmsKeyPartitionFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const FileDir_FileNo&    parentFileNo,
        const Converter_Version& converterVersion,
        const SAPDB_UInt2        partitionNo,
        const Data_PageNo&       rootPage,
        Msg_List&                msgList,
        FileDir_OmsKeyPartition* omsKeyPartitionPointer = 0) = 0;

    /*!
     * @brief Adds a file directory entry for an OMS var object.
     *
     * @copydoc FileDir_Directory::AddOmsVarFile
     */
    virtual FileDir_ReturnCode AddOmsVarFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPage,
        const SAPDB_UInt2        objBodySize,
        const SAPDB_UInt2        objPerPageCount,
        const SAPDB_UInt2        chainCount,
        Msg_List&                msgList,
        FileDir_OmsVar*          pointer = 0) = 0;

    /*!
     * @brief Adds a file directory entry for an OMS fixed object.
     *
     * @copydoc FileDir_Directory::AddOmsFixedFile
     */
    virtual FileDir_ReturnCode AddOmsFixedFile(
        RTE_TaskId               taskId,
        const FileDir_FileNo&    fileNo,
        const Converter_Version& converterVersion,
        const Data_PageNo&       rootPage,
        const SAPDB_UInt2        objBodySize,
        const SAPDB_UInt2        objPerPageCount,
        const SAPDB_UInt2        chainCount,
        Msg_List&                msgList,
        FileDir_OmsFixed*        pointer = 0) = 0;

    /*!
     * @brief Removes a file directory entry from the file directory
     *        and destroys it. This method should only be called by
     *        the garbage collector.
     *
     * @copydoc FileDir_Directory::DelFile
     */
    virtual FileDir_ReturnCode DelFile(
        RTE_TaskId            taskId,
        const FileDir_FileNo& fileNo,
        Msg_List&             msgList) = 0;

    /*@}*/

    /*!
     * @name Retrieving entries
     */
    /*@{*/

    /*!
     * @brief Retrieves an entry. This method should be called if you
     * are accessing a session-local file directory from a different
     * session.
     *
     * @copydoc FileDir_GenericDirectory::GetFileByOtherSession
     */
    virtual FileDir_ReturnCode GetFileByOtherSession(
        const FileDir_FileNo&     fileNo,
        FileDir_Generic&          fileEntry,
        Msg_List&                 msgList) const = 0;

    /*!
     * @brief Sets a table smart pointer to the file directory entry for
     *        fileNo.
     *
     * @copydoc FileDir_Directory::GetTableFile
     */
    virtual FileDir_ReturnCode GetTableFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Table&        p,
        Msg_List&             msgList) = 0;

    /*!
     * @brief Sets a table smart pointer to the file directory entry for
     *        fileNo.
     *
     * @copydoc FileDir_Directory::GetShortColumnFile
     */
    virtual FileDir_ReturnCode GetShortColumnFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_ShortColumn&  p,
        Msg_List&             msgList) = 0;

    /*!
     * @brief Sets an index smart pointer to the file directory entry for
     *        fileNo.
     *
     * @copydoc FileDir_Directory::GetIndexFile
     */
    virtual FileDir_ReturnCode GetIndexFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Index&        p,
        Msg_List&             msgList) = 0;

    /*!
     * @brief Sets a table smart pointer to the file directory entry for
     *        fileNo.
     *
     * @copydoc FileDir_Directory::GetOmsFile
     */
    virtual FileDir_ReturnCode GetOmsFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Oms&          p,
        Msg_List&             msgList) = 0;

    /*!
     * @brief Sets an OMSkey smart pointer to the file directory entry for
     *        fileNo.
     *
     * @copydoc FileDir_Directory::GetOmsKeyFile
     */
    virtual FileDir_ReturnCode GetOmsKeyFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_OmsKey&       p,
        Msg_List&             msgList) = 0;

    /*!
     * @brief Sets an OMS var smart pointer to the file directory entry for
     *        fileNo.
     *
     * @copydoc FileDir_Directory::GetOmsVarFile
     */
    virtual FileDir_ReturnCode GetOmsVarFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_OmsVar&       p,
        Msg_List&             msgList) = 0;

    /*!
     * @brief Sets an OMS fixed smart pointer to the file directory entry for
     *        fileNo.
     *
     * @copydoc FileDir_Directory::GetOmsFixedFile
     */
    virtual FileDir_ReturnCode GetOmsFixedFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_OmsFixed&     p,
        Msg_List&             msgList) = 0;

    /*!
     * @brief Sets a smart pointer to the file directory entry for
     *        fileNo. Use of this method should be avoided when
     *        possible. The typed methods GetTableFile(),
     *        GetIndexFile() etc. should be used instead.
     *
     * @copydoc FileDir_Directory::GetFile
     */
    virtual FileDir_ReturnCode GetFile(
        const RTE_TaskId&     taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      p,
        Msg_List&             msgList) = 0;

    /*!
     * @brief For use by garbage collector only. Sets a smart pointer
     *        to the file directory entry for fileNo.
     *
     * @copydoc FileDir_Directory::GcGetFile
     */
    virtual FileDir_ReturnCode GcGetFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      p,
        Msg_List&             msgList) = 0;

    /*@}*/

    /*!
     * @brief Attaches an index file to a different table file.
     * @copydoc FileDir_GenericDirectory::ReassignIndexFile
     */
    virtual FileDir_ReturnCode ReassignIndexFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& indexFileNo,
        const FileDir_FileNo& newTableFileNo,
        Msg_List&             msgList) = 0;

    /*!
     * @brief Attaches a short column file to a different table file.
     * @copydoc FileDir_GenericDirectory::ReassignShortColumnFile
     */
    virtual FileDir_ReturnCode ReassignShortColumnFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& shortColumnFileNo,
        const FileDir_FileNo& newTableFileNo,
        Msg_List&             msgList) = 0;

    /*!
     * @brief Removes the link of an index to a table. Only needed for
     *        very special cases.
     *  @copydoc FileDir_GenericDirectory::DetachIndexFile
     */
    virtual FileDir_ReturnCode DetachIndexFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& indexFileNo) = 0;

    /*!
     * @brief Returns the allocator of the directory.
     */
    virtual SAPDBMem_IRawAllocator& GetAllocator() const = 0;

    /*!
     * @name FileDir_FileNo related
     */
    /*@{*/

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
     * @brief Set the highest file id of the file directory. If
     *        highestFileNo is less than the internal file id counter,
     *        the internal file id counter will not be
     *        changed. <em>Can only be called during restart!</em>
     */
    virtual void Restart_SetFileNoHighWaterMark(
        FileDir_FileNo highestFileNo) = 0;

    /*@}*/


    /*!
     * @name Iterators
     */
    /*@{*/

    /*!
     * @brief Returns an iterator over all (temporary and persistent)
     *        file directory entries.
     *
     * @copydoc FileDir_Directory::GetFileDirIterator
     */
    virtual SAPDB_Bool GetFileDirIterator(
        FileDir_Iterator&   iterator,
        tgg00_TransContext* transContext = 0) = 0;

    /*!
     * @brief Returns an iterator over all private temporary file
     *        directory entries.
     *
     * @copydoc FileDir_Directory::GetPrivateFileDirIterator
     */
    virtual SAPDB_Bool GetPrivateFileDirIterator(
        FileDir_Iterator&   iterator,
        tgg00_TransContext* transContext = 0) = 0;

    /*!
     * @brief Returns an iterator over all (temporary and persistent)
     *        shared file directory entries.
     *
     * @copydoc FileDir_Directory::GetSharedFileDirIterator
     */
    virtual SAPDB_Bool GetSharedFileDirIterator(
        FileDir_Iterator&   iterator,
        tgg00_TransContext* transContext = 0) = 0;

    /*@}*/

    /*!
     * @name Statistics
     */
    /*@{*/

    /*!
     * @brief Returns the number of file entries that have been marked
     *        as deleted since instantiation of this file
     *        directory. This counter may wrap around, so tests for
     *        changes should not rely on the greater operator but use
     *        != instead.
     */
    virtual SAPDB_UInt GetDeleteOpCounter() const = 0;

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
     * @brief Returns the number of entries in the file directory
     *        entry cache if applicable or 0.
     *
     * @copydoc FileDir_GenericDirectory::GetCacheEntryCount
     */
    virtual SAPDB_UInt GetCacheEntryCount() const = 0;

    /*@}*/

    /*!
     * @name Session-local file directory related
     */
    /*@{*/

    /*!
     * @brief Removes pointers to deleted entries from pointer cache.
     * @copydoc FileDir_Directory::CleanUpPointerCache
     */
    virtual void CleanUpPointerCache() = 0;

    /*!
     * @brief Sets the cached lock information of all file directory
     * entries in the cache to lockState.
     *
     * @copydoc FileDir_GenericDirectory::SetAllCachedLockStates
     *
     * @warning Only the lock information of entries already in the
     * cache will be set to lockState. Entries that will enter the
     * cache at a later time will still have a lock state of
     * 'undefined'!
     */
    virtual void SetAllCachedLockStates(const SAPDB_Int lockState) = 0;
    /*@}*/

    /*!
     * @brief Write contents of directory to kernel dump.
     *
     * @copydoc FileDir_GenericDirectory::Dump
     */
    virtual void Dump(const RTE_TaskId taskId, Kernel_Dump& dump) = 0;
};

/**********************************************************************/

inline FileDir_IDirectory& FileDir_IDirectory::SessionInstance(
    const tgg00_TransContext& trans)
{
    FileDir_IDirectory* fdir =
        static_cast<FileDir_IDirectory*>( trans.trLocalFileDirPtr_gg00 );

    if ( 0 == fdir ) {
        return FileDir_IDirectory::Instance( trans.trIndex_gg00 );
    }

    return *fdir;
}

/**********************************************************************/

inline FileDir_IDirectory& FileDir_IDirectory::SessionInstance(
    const Trans_Context& trans)
{
    return FileDir_IDirectory::SessionInstance( 
        const_cast<Trans_Context&>( trans ).OldTrans() );
}

#endif // FILEDIR_IDIRECTORY_HPP
