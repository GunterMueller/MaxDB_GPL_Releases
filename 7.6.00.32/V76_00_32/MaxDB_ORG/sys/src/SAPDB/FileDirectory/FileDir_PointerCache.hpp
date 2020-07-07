/*****************************************************************************/
/*!
  @file         FileDir_PointerCache.hpp
  @ingroup      FileDirectory_Files
  @author       MartinKi

  @brief        Header file for FileDir smart pointer caching

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

#ifndef FILEDIR_POINTERCACHE_HPP
#define FILEDIR_POINTERCACHE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "Container/Container_Hash.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"

class FileDir_Generic;
class SAPDBMem_IRawAllocator;

/*!
 * @ingroup FileDirectory_Classes
 * @author MartinKi
 *
 * @brief Class to cache FileDir SmartPointer.
 */
class FileDir_PointerCache
{
private:
    // no copy-construction or assignment
    FileDir_PointerCache(const FileDir_PointerCache&);
    FileDir_PointerCache& operator=(const FileDir_PointerCache&);

    typedef Container_Hash<FileDir_FileNo, FileDir_Generic*> FilePtr_Hash;

    SAPDBMem_IRawAllocator  &m_alloc;
    FilePtr_Hash             m_pointerHash;
    int                      m_FDirChangeStamp;
public:
    /*!
     * @brief Constructs a pointer cache object.
     *
     * @param alloc [in]  allocator to use for cache
     */
    FileDir_PointerCache(SAPDBMem_IRawAllocator& alloc);

    /*!
     * @brief Destructs cache.
     *
     * All pointers within the cache will be destroyed immediately if
     * possible or will be set to self-destruct as soon as the last
     * reference to them is removed.
     */
    ~FileDir_PointerCache();

    /*!
     * @brief Removes all items from the cache.
     */
    void Clear();

    /*!
     * @brief Adds a pointer to a file directory entry to the cache.
     *
     * p must be a primary (synchronized) smart pointer to a file
     * directory entry; upon a successful return, p will be a
     * secondary (unsynchronized) pointer to the same entry.
     *
     * @param p [in/out] pointer to file directory entry
     *
     * @return
     *    - FileDir_Okay on success
     *    - FileDir_OutOfMemory not enough memory to create cache entry
     *    - FileDir_FileNoExists entry is already stored in the cache
     */
    FileDir_ReturnCode AddEntryToCache(FileDir_Generic& p);

    /*!
     * @brief Retrieves a pointer for a file directory entry from the cache.
     *
     * p will be a secondary (unsynchronized) pointer to the file
     * directory entry upon successful return. If the entry in the
     * cache has been marked deleted, FileDir_FileNoNotFound will be
     * returned and the stored pointer will be removed from cache.
     *
     * @param fileNo [in] file no of entry to be retrieved
     * @param p [out] pointer to entry; p will be invalid if entry
     *                does not exist in cache
     *
     * @return
     *    - FileDir_Okay on success
     *    - FileDir_FileNoNotFound if entry could not be found in the cache
     */
    FileDir_ReturnCode GetFile(
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      p);

    /*!
     * @brief Removes an entry from the cache.
     *
     * @param fileNo [in] file no of entry whose corresponding pointer
     *                    is to be removed from the cache
     */
    void RemoveEntryFromCache(const FileDir_FileNo& fileNo);

    /*!
     * @brief Returns the number of elements that are currently in the
     * cache.
     */
    SAPDB_UInt GetSize() const;

    /*!
     * @brief Removes pointers pointing to file directory entries that
     *        are marked as deleted from cache.
     *
     * The cache is only verified if changeStamp differs from the
     * cache's internal change stamp. If the cache is verified the
     * internal change stamp is set to changeStamps value.
     *
     * @param changeStamp [in] the change stamp of the calling
     *                         session-local file directory
     */
    void CleanUpCache(const SAPDB_Int changeStamp);

    void SetAllLockStates(const SAPDB_Int lockState);
};

/**************************************************************************/

inline FileDir_ReturnCode FileDir_PointerCache::GetFile(
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      p)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "FileDir_PointerCache::GetFile",
        FileDir_Trace, 5);

    FileDir_ReturnCode retCode = FileDir_Okay;

    FilePtr_Hash::Iterator iter = m_pointerHash.Find( fileNo );
    if ( iter.IsValid() ) {
        if ( Data_FileDeleted != iter->value->GetFileState() ) {
            p.AssignSecondary( *iter->value );
            retCode = FileDir_Okay;
        } else {
            RemoveEntryFromCache( fileNo );
            retCode = FileDir_FileNoNotFound;
        }
    } else {
        retCode = FileDir_FileNoNotFound;
    }
    return retCode;
}

/**************************************************************************/

inline SAPDB_UInt FileDir_PointerCache::GetSize() const
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "FileDir_PointerCache::GetSize", FileDir_Trace, 5);

    return m_pointerHash.GetSize();
}

/**************************************************************************/

#endif // FILEDIR_POINTERCACHE_HPP
