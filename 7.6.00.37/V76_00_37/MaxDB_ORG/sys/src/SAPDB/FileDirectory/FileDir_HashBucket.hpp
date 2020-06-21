/*****************************************************************************/
/*!

  @file         FileDir_HashBucket.hpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Header file for class FileDir_HashBucket.

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

#ifndef FILEDIR_HASHBUCKET_HPP
#define FILEDIR_HASHBUCKET_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_BaseEntry.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"

#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "RunTime/RTE_Types.hpp"

class FileDir_Generic;

/*!
 * @class FileDir_HashBucket
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief A class representing a variable sized hash bucket used to
 * store pointers to FileDir_Entries.
 *
 * Locking for the buckets has to be provided externally, e.g. via
 * FileDir_BucketLocks.
 */
class FileDir_HashBucket
{
public:
    /// Indicates whether readers might be about in methods that may
    /// alter the bucket structure.
    enum Concurrency
    {
        NoConcurrency,
        ConcurrentReads
    };

    /*!
     * @brief Constructs a hash bucket.
     */
    FileDir_HashBucket();


    /*!
     * @brief Destructs the hash bucket. The bucket must be empty.
     */
    ~FileDir_HashBucket();

    /*!
     * @brief Copy-constructor. <em>Only to be used for empty hash
     *        buckets, needed for newarray()</em>
     */
    FileDir_HashBucket(const FileDir_HashBucket& hb);

    /*!
     * @brief Returns true if bucket initialization was successful.
     */
    SAPDB_Bool Initialize();

    /*!
     * @brief Returns a pointer to the file directory entry with
     *        fileNo. The caller has to make sure no write operations
     *        to the bucket are in progress by calling
     *        LockShared()/UnlockShared() before/after calling Find.
     *
     * @param fileNo [in] the file id of the entry to be returned.
     *
     * @return a pointer to the desired file directory entry or 0 if
     *         it is not present in this bucket
     */
    FileDir_BaseEntry* Find(const FileDir_FileNo& fileNo) const;

    /*!
     * @brief Returns true if the bucket is empty.
     */
    bool IsEmpty() const;

    /*!
     * @brief Returns a smart pointer to the file directory entry
     *        following the one contained in node. The hash bucket has
     *        to be locked externally when calling this method.
     *
     * @warning The caller has to insure that node is pointing to a
     *          valid address, e.g. by not releasing a lock held on
     *          this bucket in between calls to this method.
     *
     * @param node [in/out] pointer to the node whose successor should
     *                      be returned; on exit node will point to
     *                      this successor or is 0 if there is no
     *                      successor
     * @param pointer [out] pointer to retrieved file directory entry;
     *                      invalid if none was found
     * @return true if a successor was found, false otherwise
     */
    SAPDB_Bool GetNextEntry(
        FileDir_Node*&   node,
        FileDir_Generic& pointer);

    /*!
     * @brief Returns the node of the file directory entry
     *        following the one contained in node. The hash bucket has
     *        to be locked externally when calling this method.
     *
     * @warning The caller has to insure that node is pointing to a
     *          valid address, e.g. by not releasing a lock held on
     *          this bucket in between calls to this method.
     *
     * @param node [in/out] pointer to the node whose successor should
     *                      be returned; on exit node will point to
     *                      this successor or is 0 if there is no
     *                      successor
     * @return true if a successor was found, false otherwise
     */
    SAPDB_Bool GetNextNode(FileDir_Node*& node);


    /*!
     * @brief Add file directory entry node to this bucket.
     *
     * @param taskId [in] task id of calling task
     * @param node   [in] pointer to bucket node for file entry
     * @param bucketSize [out] contains number of nodes in this bucket
     *                         upon return
     * @param concurrencyType [in]
     *   - NoConcurreny: no one else is working on (reading) this bucket
     *                   during the add operation (-> bucket is write locked)
     *   - ConcurrentReads: the bucket contents can be read while the add is
     *                   in progress; make sure that bucket contents are
     *                   always correct by using when mem barrier before
     *                   queuing in the new entry
     *
     * @return
     *         - FileDir_Okay on success
     *         - FileDir_FileNoExists if a node pointing to an entry
     *           with the same file id already exists
     */
    FileDir_ReturnCode AddNode(
        const RTE_TaskId taskId,
        FileDir_Node*    node,
        SAPDB_UInt&      bucketSize,
        Concurrency      concurrencyType = ConcurrentReads);

    /*!
     * @brief Remove node for entry with file id fileNo from this
     *        bucket. The bucket has to be locked externally.
     *
     * @param taskId [in] task id of calling task
     * @param fileNo [in] the file id of the file directory entry to
     *                    be removed
     *
     * @return a pointer to the removed file directory entry or 0 if
     *         the entry was not found in this bucket
     */
    FileDir_BaseEntry* Remove(
        const RTE_TaskId      taskId,
        const FileDir_FileNo& fileNo);

    /*!
     * @brief Removes the first node in this bucket and returns a
     *        pointer to that node. The caller has to make sure no
     *        write operations to the bucket are in progress by
     *        calling LockShared()/UnlockShared() before/after calling
     *        Find.
     *
     * @return a pointer to the removed hash node or 0 if
     *         the bucket is empty
     */
    FileDir_Node* RemoveFirstNode();

    /*!
     * @brief Returns the number of elements in the bucket. No locking
     *        is used, so the value returned has been read dirty.
     */
    SAPDB_UInt GetSize() const;

    /*!
     * @brief Removes all content from bucket, but does not destroy it.
     */
    void Clear();
private:
    int m_size;  ///< number of nodes in bucket

    FileDir_Node* m_startNode;
};

/*************************************************************************/

inline bool FileDir_HashBucket::IsEmpty() const
{
    return m_startNode == 0;
}

/*************************************************************************/

inline SAPDB_UInt FileDir_HashBucket::GetSize() const
{
    return static_cast<SAPDB_UInt>(m_size);
}

/*************************************************************************/

inline FileDir_HashBucket::FileDir_HashBucket()
    : m_startNode(0),
      m_size(0)
{
}

/**************************************************************************/

inline FileDir_HashBucket::FileDir_HashBucket(const FileDir_HashBucket& hb)
    : m_startNode(0),
      m_size(0)
{
    SAPDBERR_ASSERT_ARGUMENT( hb.m_startNode == 0 );
}

/**************************************************************************/

inline SAPDB_Bool FileDir_HashBucket::Initialize()
{
    return true;
}

/**************************************************************************/

inline FileDir_HashBucket::~FileDir_HashBucket()
{
    SAPDBERR_ASSERT_STATE( IsEmpty() );
}

/**************************************************************************/

/*!
 * caller has to make sure bucket is locked !
 */
inline FileDir_BaseEntry* FileDir_HashBucket::Find(
    const FileDir_FileNo& fileNo) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_HashBucket::Find", FileDir_Trace, 6 );

    FileDir_Node* n = m_startNode;
    while ( n != 0 ) {
        RTESys_ReadMemoryBarrier();
        if ( n->entry->GetFileNo() == fileNo ) {
            return n->entry;
        }
        n = n->next;
    }
    return 0;
}

/**************************************************************************/

#endif // FILEDIR_HASHBUCKET_HPP
