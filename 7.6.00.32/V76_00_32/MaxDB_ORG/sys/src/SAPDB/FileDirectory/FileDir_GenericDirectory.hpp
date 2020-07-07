/*****************************************************************************/
/*!

  @file         FileDir_GenericDirectory.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDirectory.

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

#ifndef FILEDIR_GENERICDIRECTORY_HPP
#define FILEDIR_GENERICDIRECTORY_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Iterator.hpp"
#include "FileDirectory/FileDir_BucketLocks.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_Vector.hpp"

#include "DataAccess/Data_Types.hpp"

#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

class FileDir_Index;
class FileDir_ShortColumn;
class FileDir_Table;
class FileDir_BaseEntry;
class FileDir_Entry;
class FileDir_HashBucket;
class FileDir_PageManager;
class FileDir_RWRegionWrapper;

class Converter_Version;
class RTESync_SpinlockPool;
class SAPDBMem_IRawAllocator;
class Kernel_Dump;
struct tgg00_TransContext;
class Msg_List;

/*!
 * @class FileDir_GenericDirectory
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief The FileDir_GenericDirectory class. This class manages a File Directory.
 */
class FileDir_GenericDirectory : public FileDir_IDirectory
{
private:
    // Allow iterator access to private members.
    friend class FileDir_IteratorObj;
    friend class FileDir_Test;

    typedef FileDir_BucketLocks::BucketNo BucketNo;

    SAPDB_UInt m_largestBucketSize;   ///< Number of entries in largest bucket



    /// Locks for hash bucket list.
    mutable FileDir_RWRegionWrapper* m_hashLock;
    FileDir_BucketLocks m_bucketLocks;

    SAPDBMem_IRawAllocator& m_allocator;

    bool m_initialized;

    bool m_useBucketLocks;

    /// Number of entries in file directory.
    SAPDB_UInt m_entryCount;

/***************************** Methods *****************************/

private:
    // Sun Studio compiler needs HashInfo to be a friend to be able to use BucketNo:
    struct HashInfo;
    friend struct HashInfo;
    struct HashInfo {
        FileDir_HashBucket* bucketList;
        BucketNo            bucketCount;
        /// Number of times the hash list has been resized.
        SAPDB_UInt          resizeCount;
        /// Number of buckets that actually have any content.
        BucketNo            usedBucketCount;

        HashInfo()
            : bucketList(0), bucketCount(0), resizeCount(0), usedBucketCount(0)
        {}
    };

    HashInfo* m_hash;

    // Copy and assignment operator are not supported
    FileDir_GenericDirectory& operator=(
        const FileDir_GenericDirectory &fdir );

    FileDir_GenericDirectory( const FileDir_GenericDirectory &fdir );

    SAPDB_Bool ResizeHash(const RTE_TaskId taskId);

    FileDir_BucketLocks& GetBucketLocks() { return m_bucketLocks; }

    BucketNo LockBucketExclusive(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_HashBucket*&  bucket) const;

    BucketNo LockBucketShared(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_HashBucket*&  bucket) const;

protected:
    /*!
     * @brief Removes links from parent of file entry to entry with
     *        fileNo. If fir != 0 no lookup of fileNo will be
     *        done. The file entry has to marked for deletion
     *        already.
     */
    FileDir_ReturnCode DetachFromParentEntry(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_BaseEntry*    fir);

    /// Possible return codes of IteratorGetFile
    enum IteratorGetRc {
        igOkay,
        igFileNoNotFound,
        igFileDirShuttingDown
    };

    /*!
     * @brief Retrieves an entry. For use by iterators only. Will also
     * return files that are marked deleted.
     *
     * @param transContext [in] pointer to transaction context of
     *                          calling transaction; FIXME: should be
     *                          replaced by taskId
     * @param fileNo       [in] fileNo of entry to retrieve
     * @param p      [out] p will either point to the retrieved entry on
     *                     success or be invalid on return
     */
    virtual IteratorGetRc IteratorGetFile(
        tgg00_TransContext*   transContext,
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      p) const;

    /*!
     * @brief Retrieves an entry. This method should be called if you
     * are accessing a session-local file directory from a different
     * session.
     *
     * This method makes sure that the entry is retrieved in a way
     * that is multi-thread safe.
     *
     * @param fileNo       [in] fileNo of entry to retrieve
     * @param fileEntry    [out] will either point to the retrieved entry on
     *                     success or be invalid on return
     * @param msgList [out] message list holding possible error messages
     */
    virtual FileDir_ReturnCode GetFileByOtherSession(
        const FileDir_FileNo&     fileNo,
        FileDir_Generic&          fileEntry,
        Msg_List&                 msgList) const;

    /*!
     * @brief Retrieves an entry.
     *
     * @param taskId [in]  task id of running task
     * @param fileNo [in]  file id to retrieve
     * @param fe      [out] p will either point to the retrieved entry on
     *                     success or be 0
     * @param msgList [out] message list holding possible error messages
     */
    virtual FileDir_ReturnCode GetEntry(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      fileEntry,
        Msg_List&             msgList) const;

    /*!
     * @brief Returns true if a resize of the file directory entry
     *        hash should be done to accommodate all entries.
     */
    bool ResizeNecessary() const;

    /*!
     * @brief Returns the bucket fileNo would be stored in.
     */
    BucketNo HashFunction(const FileDir_FileNo& fileNo) const;

    /*!
     * @brief Returns the bucket fileNo would be stored in for a hash
     *        size of bucketCount.
     */
    BucketNo HashFunction(
        const FileDir_FileNo& fileNo,
        const BucketNo        bucketCount) const;

    /*!
     * @brief Returns the hash value for fileNo.
     */
    SAPDB_UInt8 CalcHashValue(const FileDir_FileNo fileNo) const;

    /// Locking behaviour types.
    enum GeneralLockingBehaviour
    {
        /// Use strict locking: all accesses to the directory are
        /// synchronized, all hash buckets have their own lock. This
        /// should be used for a shared directory.
        FileDir_RegularLocking,

        /// Assume no concurrency between regular readers/writers to
        /// the directory. Only delete operations and iterator reads
        /// will be synchronized. Individual hash buckets will not
        /// have any locks.
        FileDir_MinimalLocking
    };

    /*!
     * Removes file directory entry from hash list without deleting
     * it. References to other file directory entries (e.g. index to
     * table) are not removed.
     *
     * @warning Once an entry has been removed from the hash, its
     * reference count should not be increased any more to avoid
     * synchronisation issues if the entry has been marked for delete
     * (see PTS 1139062)
     *
     * @return
     *  - true if file directory entry has been removed; fileEntry will
     *    point to the entry
     *  - false if the entry could not be found; fileEntry will
     *    be invalid
     */
    SAPDB_Bool RemoveFileEntry(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      fileEntry);
public:
    /// Locking behaviour AddFileEntry() should use when called.
    enum LockingBehaviour
    {
        /// Use locking.
        FileDir_UseLocking,

        /// Do not use locking.
        FileDir_NoLocking
    };

     /*!
     * @brief Constructs a file directory object
     */
    FileDir_GenericDirectory(
        SAPDBMem_IRawAllocator& alloc,
        GeneralLockingBehaviour lockingType);

    /*!
     * @name Startup/Shutdown
     */
    /*@{*/

     /*!
     * @brief Initializes this object.
     *
     * @param initialCapacity [in] initial capacity of the hash list;
     *                             a compile time constant is used if
     *                             initialCapacity is 0
     */
    virtual SAPDB_Bool Initialize(SAPDB_Int4 initialCapacity = 0);

    /*!
     * @brief Shut down the file directory. All in-memory structures
     *        for this FileDir-object and its entries are freed. No
     *        references to any file directory entries must exist when
     *        calling this method.
     *
     * @param taskId [in] Id of the calling task
     */
    FileDir_ReturnCode Shutdown(RTE_TaskId taskId);

    /*!
     * @brief Returns the allocator of the directory.
     */
    SAPDBMem_IRawAllocator& GetAllocator() const;

    /*@}*/

    /*!
     * @name Adding new entries/deleting entries
     */
    /*@{*/

    /*!
     * @brief Adds a file directory entry to the directory.
     *
     * @param taskId      [in] task id of running task, or
     *                         RTE_UnknownTaskId if not available
     * @param fileNo      [in] file id of entry to be added
     * @param entry       [in] pointer to entry to be added
     * @param msgList [out] message list holding possible error messages
     * @param lockType [in] FileDir_UseLocking: use rw-locks to protect
     *                      this add operation; this is the default
     *
     * @return
     *    - <em>FileDir_Okay</em>         on success
     *    - <em>FileDir_FileNoExists</em> if an entry with file id
     *                                    fileNo already exists
     *    - <em>FileDir_OutOfMemory</em>  if there is not enough memory
     *                                    to insert the entry into the
     *                                    directory
     */
    virtual FileDir_ReturnCode AddFileEntry(
        const RTE_TaskId         taskId,
        const FileDir_FileNo&    fileNo,
        FileDir_BaseEntry*       entry,
        Msg_List&                msgList,
        const LockingBehaviour   lockType = FileDir_UseLocking);

    /*!
     * @brief Retrieves a file directory entry from the directory.
     *
     * @param taskId      [in] task id of running task, or
     *                         RTE_UnknownTaskId if not available
     * @param fileNo      [in] file no of entry to be retrieved
     *
     * @return
     *   - true, if entry could be found
     *   - false, otherwise
     */
    virtual SAPDB_Bool GetFileEntry(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      fileEntry) const;

    /*!
     * @brief Removes a file directory entry from the file directory
     *        and destroys it. This method should only be called by
     *        the garbage collector.
     *
     * @return
     *    - FileDir_Okay           if the entry was successfully removed
     *    - FileDir_FileNoNotFound if no entry with file id fileNo was found
     *
     * FIXME: should only be accessible to garbage collector
     */
    FileDir_ReturnCode DelFile(
        RTE_TaskId            taskId,
        const FileDir_FileNo& fileNo,
        Msg_List&             msgList);

    /*!
     * @brief Return a new unused fileNo. This function does not
     *        recycle entries or check for duplicate fileNos.
     */
    FileDir_FileNo GetNewFileNo();

    /*!
     * @brief Return a new unused fileNo for a temporary object. This
     *        function does not recycle entries or check for duplicate
     *        fileNos.
     */
    FileDir_FileNo GetNewTempFileNo();

    /*!
     * @brief Adds a file directory entry for a table.
     *
     * @param taskId           [in] task id of running task, or
     *                              RTE_UnknownTaskId if not available
     * @param fileNo           [in] fileNo of table to be added
     * @param converterVersion [in] current converter version
     * @param rootPage         [in] root data base page of table
     * @param blobColCount     [in] number of blob columns defined for
     *                              this table
     * @param fileTypeSet      [in] file type set of the index file
     * @param fileVersion      [in] file version of thie index file
     * @param msgList          [out] message list holding possible error
     *                               messages
     * @param pointer          [out] if pointer is not null its contents
     *                               will point to the newly created table
     *                               entry on success
     *
     * @return
     *         - <em>FileDir_Okay</em>          on success
     *         - <em>FileDir_FileNoExists</em>  if an entry with file id
     *                                          fileNo already exists
     *         - <em>FileDir_OutOfMemory</em>   if a memory allocation
     *                                          failed along the way
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
        FileDir_Table* pointer = 0);

    /*!
     * @brief Adds a file directory entry for a shared temporary table.
     *
     * If this file directory is a session local file directory the
     * entry will be added to the shared file directory.
     *
     * @param taskId           [in] task id of running task, or
     *                              RTE_UnknownTaskId if not available
     * @param fileNo           [in] fileNo of table to be added
     * @param converterVersion [in] current converter version
     * @param rootPage         [in] root data base page of table
     * @param blobColCount     [in] number of blob columns defined for
     *                              this table
     * @param fileTypeSet      [in] file type set of the index file
     * @param msgList          [out] message list holding possible error
     *                               messages
     * @param pointer          [out] if pointer is not null its contents
     *                               will point to the newly created table
     *                               entry on success
     *
     * @return
     *         - <em>FileDir_Okay</em>          on success
     *         - <em>FileDir_FileNoExists</em>  if an entry with file id
     *                                          fileNo already exists
     *         - <em>FileDir_OutOfMemory</em>   if a memory allocation
     *                                          failed along the way
     */
    virtual FileDir_ReturnCode AddSharedTempTableFile(
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
     * @param taskId           [in] task id of running task, or
     *                              RTE_UnknownTaskId if not available
     * @param fileNo           [in] file id of index to be added
     * @param parentFileNo     [in] file id of table this index is defined
     *                              on
     * @param converterVersion [in] current converter version
     * @param indexId          [in] index id of the index; must not be in use
     * @param rootPage         [in] root data base page of index
     * @param fileTypeSet      [in] file type set of the index file
     * @param fileVersion      [in] file version of thie index file
     * @param msgList          [out] message list holding possible error
     *                               messages
     * @param indexPointer     [out] if pointer is not null its contents
     *                               will point to the newly created index
     *                               entry on success
     *
     * @return
     *         - <em>FileDir_Okay</em>          on success
     *         - <em>FileDir_FileNoExists</em>  if an entry with file id
     *                                          fileNo already exists
     *         - <em>FileDir_FileNoNotFound</em> if table this index is
     *                                           defined for does not exist
     *         - <em>FileDir_IndexExists</em> if table already has an index
     *                                        with indexId
     *         - <em>FileDir_FileTypeMismatch</em> if file entry this index
     *                                             is defined for is not a
     *                                             table entry
     *         - <em>FileDir_OutOfMemory</em>   if a memory allocation
     *                                          failed along the way
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
     * @param taskId           [in] task id of running task, or
     *                              RTE_UnknownTaskId if not available
     * @param fileNo           [in] fileNo of short column file to be added
     * @param parentFileNo     [in] fileNo of table this short column file
     *                              belongs to
     * @param converterVersion [in] current converter version
     * @param rootPage         [in] root data base page of short column file
     * @param fileTypeSet      [in] file type set of the short column file
     * @param fileVersion      [in] file version of thie short column file
     * @param msgList          [out] message list holding possible error
     *                               messages
     * @param shortColPointer  [out] if pointer is not null its contents
     *                               will point to the newly created short
     *                               column entry on success
     *
     * @return
     *         - <em>FileDir_Okay</em>          on success
     *         - <em>FileDir_FileNoExists</em>  if an entry with file id
     *                                          fileNo already exists
     *         - <em>FileDir_FileNoNotFound</em> if table this index is
     *                                           defined for does not exist
     *         - <em>FileDir_FileTypeMismatch</em> if file entry this index
     *                                             is defined for is not a
     *                                             table entry
     *         - <em>FileDir_OutOfMemory</em>   if a memory allocation
     *                                          failed along the way
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
     * @param taskId           [in] task id of running task, or
     *                              RTE_UnknownTaskId if not available
     * @param fileNo           [in] file id of object to be added
     * @param converterVersion [in] current converter version
     * @param rootPage         [in] root data base page of object
     * @param objBodySize      [in]
     * @param objPerPageCount  [in]
     * @param chainCount       [in]
     * @param msgList          [out] message list holding possible error
     *                               messages
     * @param pointer          [out] if pointer is not null its contents
     *                               will point to the newly created oms
     *                               entry on success
     *
     * @return
     *         - <em>FileDir_Okay</em>          on success
     *         - <em>FileDir_FileNoExists</em>  if an entry with file id
     *                                          fileNo already exists
     *         - <em>FileDir_OutOfMemory</em>   if a memory allocation
     *                                          failed along the way
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
     * @param taskId           [in] task id of running task, or
     *                              RTE_UnknownTaskId if not available
     * @param fileNo           [in] file id of object to be added
     * @param converterVersion [in] current converter version
     * @param rootPage         [in] root data base page of object
     * @param objBodySize      [in]
     * @param objPerPageCount  [in]
     * @param chainCount       [in]
     * @param keyPartitionCount [in]
     * @param msgList          [out] message list holding possible error
     *                               messages
     * @param pointer          [out] if pointer is not null its contents
     *                               will point to the newly created oms
     *                               entry on success
     *
     * @return
     *         - <em>FileDir_Okay</em>          on success
     *         - <em>FileDir_FileNoExists</em>  if an entry with file id
     *                                          fileNo already exists
     *         - <em>FileDir_OutOfMemory</em>   if a memory allocation
     *                                          failed along the way
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
     * @brief Adds a file directory entry for an OMS key partition.
     *
     * @param taskId           [in] task id of running task, or
     *                              RTE_UnknownTaskId if not available
     * @param fileNo           [in] file no of key partition to be added
     * @param parentFileNo     [in] file id of key entry this partition is
     *                              defined on
     * @param converterVersion [in] current converter version
     * @param partitionNo      [in] the id of this partition
     * @param rootPage         [in] root data base page of key partition
     * @param msgList          [out] message list holding possible error
     *                               messages
     * @param omsKeyPartitionPointer  [out] if pointer is not null its contents
     *                               will point to the newly created key
     *                               partion entry on success
     *
     * @return
     *         - <em>FileDir_Okay</em>          on success
     *         - <em>FileDir_FileNoExists</em>  if an entry with file id
     *                                          fileNo already exists
     *         - <em>FileDir_FileNoNotFound</em> if table this index is
     *                                           defined for does not exist
     *         - <em>FileDir_FileTypeMismatch</em> if file entry this index
     *                                             is defined for is not a
     *                                             table entry
     *         - <em>FileDir_OutOfMemory</em>   if a memory allocation
     *                                          failed along the way
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
     * @param taskId           [in] task id of running task, or
     *                              RTE_UnknownTaskId if not available
     * @param fileNo           [in] file id of object to be added
     * @param converterVersion [in] current converter version
     * @param rootPage         [in] root data base page of object
     * @param objBodySize      [in]
     * @param objPerPageCount  [in]
     * @param chainCount       [in]
     * @param msgList          [out] message list holding possible error
     *                               messages
     * @param pointer          [out] if pointer is not null its contents
     *                               will point to the newly created oms
     *                               entry on success
     *
     * @return
     *         - <em>FileDir_Okay</em>          on success
     *         - <em>FileDir_FileNoExists</em>  if an entry with file id
     *                                          fileNo already exists
     *         - <em>FileDir_OutOfMemory</em>   if a memory allocation
     *                                          failed along the way
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
     * @param taskId           [in] task id of running task, or
     *                              RTE_UnknownTaskId if not available
     * @param fileNo           [in] file id of object to be added
     * @param converterVersion [in] current converter version
     * @param rootPage         [in] root data base page of object
     * @param objBodySize      [in]
     * @param objPerPageCount  [in]
     * @param chainCount       [in]
     * @param msgList          [out] message list holding possible error
     *                               messages
     * @param pointer          [out] if pointer is not null its contents
     *                               will point to the newly created oms
     *                               entry on success
     *
     * @return
     *         - <em>FileDir_Okay</em>          on success
     *         - <em>FileDir_FileNoExists</em>  if an entry with file id
     *                                          fileNo already exists
     *         - <em>FileDir_OutOfMemory</em>   if a memory allocation
     *                                          failed along the way
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
     * @brief Sets a table smart pointer to the file directory entry for
     *        fileNo.
     *
     * @param taskId [in]  task id of running task
     * @param fileNo [in]  fileNo of table entry to be retrieved
     * @param p      [out] a SmartPointer object to the table entry
     *                     requested; if no entry exists for fileNo
     *                     or the entry is not of type
     *                     Data_TableFile, the SmartPointer object will be
     *                     invalid on return
     * @param msgList [out] message list holding possible error messages
     *
     * @return
     *         - FileDir_Okay             on success
     *         - FileDir_FileNoNotFound   if no entry with file id
     *                                    fileNo exists
     *         - FileDir_FileTypeMismatch if the entry with file id fileNo
     *                                    is not of type Data_TableFile
     */
    FileDir_ReturnCode GetTableFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Table&        p,
        Msg_List&             msgList);

    /*!
     * @brief Sets a short column smart pointer to the file directory
     *        entry for fileNo.
     *
     * @param taskId [in]  task id of running task
     * @param fileNo [in]  file id of table entry to be retrieved
     * @param p      [out] a SmartPointer object to the short column entry
     *                     requested; if no entry exists for fileNo
     *                     or the entry is not of type
     *                     Data_ShortColumnFile, the SmartPointer object
     *                     will be invalid on return
     * @param msgList [out] message list holding possible error messages
     *
     * @return
     *         - FileDir_Okay             on success
     *         - FileDir_FileNoNotFound   if no entry with file id
     *                                    fileNo exists
     *         - FileDir_FileTypeMismatch if the entry with file id fileNo
     *                                    is not of type Data_TableFile
     */
    FileDir_ReturnCode GetShortColumnFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_ShortColumn&  p,
        Msg_List&             msgList);

    /*!
     * @brief Sets an index smart pointer to the file directory entry for
     *        fileNo.
     *
     * @param taskId [in]  task id of running task
     * @param fileNo [in]  file id of index entry to be retrieved
     * @param p      [out] a SmartPointer object to the index entry
     *                     requested; if no entry exists for fileNo
     *                     or the entry is not of type
     *                     Data_TableFile, the SmartPointer object will be
     *                     invalid on return
     * @param msgList [out] message list holding possible error messages
     *
     * @return
     *         - FileDir_Okay             on success
     *         - FileDir_FileNoNotFound   if no entry with file id
     *                                    fileNo exists
     *         - FileDir_FileTypeMismatch if the entry with file id fileNo
     *                                    is not of type Data_IndexFile
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
     * @param taskId [in]  task id of running task
     * @param fileNo [in]  file id of index entry to be retrieved
     * @param p      [out] a SmartPointer object to the OMS entry
     *                     requested; if no entry exists for fileNo
     *                     or the entry is not of type
     *                     Data_OmsFile, the SmartPointer object will be
     *                     invalid on return
     * @param msgList [out] message list holding possible error messages
     *
     * @return
     *         - FileDir_Okay             on success
     *         - FileDir_FileNoNotFound   if no entry with file id
     *                                    fileNo exists
     *         - FileDir_FileTypeMismatch if the entry with file id fileNo
     *                                    is not of type Data_IndexFile
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
     * @param taskId [in]  task id of running task
     * @param fileNo [in]  file id of index entry to be retrieved
     * @param p      [out] a SmartPointer object to the OMS key entry
     *                     requested; if no entry exists for fileNo
     *                     or the entry is not of type
     *                     Data_OmsKeyFile, the SmartPointer object will be
     *                     invalid on return
     * @param msgList [out] message list holding possible error messages
     *
     * @return
     *         - FileDir_Okay             on success
     *         - FileDir_FileNoNotFound   if no entry with file id
     *                                    fileNo exists
     *         - FileDir_FileTypeMismatch if the entry with file id fileNo
     *                                    is not of type Data_IndexFile
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
     * @param taskId [in]  task id of running task
     * @param fileNo [in]  file id of index entry to be retrieved
     * @param p      [out] a SmartPointer object to the OMS entry
     *                     requested; if no entry exists for fileNo
     *                     or the entry is not of type
     *                     Data_OmsVarFile, the SmartPointer object will be
     *                     invalid on return
     * @param msgList [out] message list holding possible error messages
     *
     * @return
     *         - FileDir_Okay             on success
     *         - FileDir_FileNoNotFound   if no entry with file id
     *                                    fileNo exists
     *         - FileDir_FileTypeMismatch if the entry with file id fileNo
     *                                    is not of type Data_IndexFile
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
     * @param taskId [in]  task id of running task
     * @param fileNo [in]  file id of index entry to be retrieved
     * @param p      [out] a SmartPointer object to the OMS entry
     *                     requested; if no entry exists for fileNo
     *                     or the entry is not of type
     *                     Data_OmsFixedFile, the SmartPointer object will be
     *                     invalid on return
     * @param msgList [out] message list holding possible error messages
     *
     * @return
     *         - FileDir_Okay             on success
     *         - FileDir_FileNoNotFound   if no entry with file id
     *                                    fileNo exists
     *         - FileDir_FileTypeMismatch if the entry with file id fileNo
     *                                    is not of type Data_IndexFile
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
     * @param taskId [in]  task id of running task
     * @param fileNo [in]  file id of table entry to be retrieved
     * @param p      [out] a SmartPointer object to the entry
     *                     requested; if no entry exists for fileNo
     *                     the SmartPointer object will be invalid
     *                     on return
     * @param msgList [out] message list holding possible error messages
     *
     * @return
     *         - FileDir_Okay             on success
     *         - FileDir_FileNoNotFound   if no entry with file id
     *                                    fileNo exists
     *         - FileDir_ConsistentViewCancelled
     *                                    trError will be e_consview_cancelled
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
     * The garbage collector can see any file directory entry in the
     * private and shared directory no matter whether it would be
     * visible to the current transaction or not.
     *
     * @param taskId [in]  task id of running task
     * @param fileNo [in]  file id of table entry to be retrieved
     * @param p      [out] a SmartPointer object to the entry
     *                     requested; if no entry exists for fileNo
     *                     the SmartPointer object will be invalid
     *                     on return
     * @param msgList [out] message list holding possible error messages
     *
     * @return
     *         - FileDir_Okay             on success
     *         - FileDir_FileNoNotFound   if no entry with file id
     *                                    fileNo exists
     */
    FileDir_ReturnCode GcGetFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      p,
        Msg_List&             msgList);

    /*@}*/

    /*!
     * @brief Reassigns an index file entry to a different table.
     *
     * The index file will be removed from its previous table and
     * assigned to the new table.
     *
     * @param taskId [in]  task id of running task
     * @param indexFileNo [in] fileNo of index file entry to be reassigned
     * @param newTableFileNo [in] fileNo of table entry to assign index to
     * @param msgList [out] message list holding possible error messages
     *                               messages
     */
    FileDir_ReturnCode ReassignIndexFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& indexFileNo,
        const FileDir_FileNo& newTableFileNo,
        Msg_List&             msgList);

    /*!
     * @brief Reassigns a short column file entry to a different table.
     *
     * The short column file will be removed from its previous table and
     * assigned to the new table.
     *
     * @param taskId [in]  task id of running task
     * @param shortColumnFileNo [in] fileNo of short column file entry to
     *                               be reassigned
     * @param newTableFileNo [in] fileNo of table entry to assign short
     *                            column entry to
     * @param msgList [out] message list holding possible error messages
     *                               messages
     */
    FileDir_ReturnCode ReassignShortColumnFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& shortColumnFileNo,
        const FileDir_FileNo& newTableFileNo,
        Msg_List&             msgList);

    /*!
     * @brief Removes an index file from the table it is currently defined on.
     *
     * @param taskId [in]  task id of running task
     * @param indexFileNo [in] fileNo of index file entry that is to be
     *                         detached
     */
    FileDir_ReturnCode DetachIndexFile(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& indexFileNo);

    /*!
     * @name Statistics
     */
    /*@{*/

    /*!
     * @brief Returns the number of entries in the directory.
     */
    SAPDB_UInt GetEntryCount() const;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    SAPDB_UInt GetHashTableSlotCount() const;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    SAPDB_UInt GetUsedHashTableSlotCount() const;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    SAPDB_UInt GetMaxHashTableChainEntryCount() const;

    /*!
     * @brief Returns the number of entries in the directory.
     */
    SAPDB_UInt GetResizeHashTableCount() const;

    /*!
     * @brief Returns the number of entries in the file directory
     *        entry cache if applicable or 0.
     */
    SAPDB_UInt GetCacheEntryCount() const;

    /*!
     * @brief Returns the highest file id that has been handed out by
     *        the file directory.
     */
    FileDir_FileNo GetMaxFileNo() const;

    /*!
     * @brief Returns the highest file id for temp-files that has been
     *        handed out by the file directory.
     */
    FileDir_FileNo GetMaxTempFileNo() const;

    /*@}*/

    /*!
     * @brief Write contents into kernel dump.
     */
    void Dump(const RTE_TaskId taskId, Kernel_Dump& dump);


    /*!
     * @brief Removes pointers to deleted entries from pointer cache.
     * @copydoc FileDir_Directory::CleanUpPointerCache
     */
    void CleanUpPointerCache()
    {}

    /*!
     * @brief Sets the cached lock information of all file directory
     * entries in the cache to lockState.
     *
     * @warning Only the lock information of entries already in the
     * cache will be set to lockState. Entries that will enter the
     * cache at a later time will still have a lock state of
     * 'undefined'!
     */
    void SetAllCachedLockStates(const SAPDB_Int lockState)
    {}


private:
    LockingBehaviour m_lockingBehaviour;
};

/**********************************************************************/

inline bool FileDir_GenericDirectory::ResizeNecessary() const
{
    return m_entryCount > m_hash->bucketCount;
}

/**********************************************************************/

inline FileDir_GenericDirectory::BucketNo
FileDir_GenericDirectory::HashFunction(
    const FileDir_FileNo& fileNo) const
{
    return static_cast<BucketNo>(
        this->CalcHashValue( fileNo ) % m_hash->bucketCount );
}

/**********************************************************************/

inline FileDir_GenericDirectory::BucketNo
FileDir_GenericDirectory::HashFunction(
    const FileDir_FileNo& fileNo,
    const BucketNo        bucketCount) const
{
    return static_cast<BucketNo>(
        this->CalcHashValue( fileNo ) % bucketCount );
}

/**********************************************************************/

inline SAPDB_UInt8 FileDir_GenericDirectory::CalcHashValue(
    const FileDir_FileNo fileNo) const
{
    SAPDB_UInt8 hashValue = 0;
    const SAPDB_Char* s   = fileNo.GetCharArray();

    for ( int i=0; i < sizeof(FileDir_FileNo); i++, s++ ) {
        hashValue = 131*hashValue + *s;
    }
    return hashValue;
}

#endif // FILEDIR_GENERICDIRECTORY_HPP
