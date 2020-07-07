/****************************************************************************/
/*!

  @file       FileDir_SysKeyCache.hpp
  @author     MartinKi, TorstenS
  @ingroup    FileDirectory_Files

  @brief      Cache for syskey generation

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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

#ifndef FILEDIR_SYSKEYCACHE_H
#define FILEDIR_SYSKEYCACHE_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types

#include "FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_RWRegionWrapper.hpp"

#include "Container/Container_Hash.hpp"

class SAPDBMem_IRawAllocator;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @class   FileDir_SysKeyCache
 * @ingroup FileDirectory_Classes
 * @author  MartinKi, TorstenS
 *
 * @brief Cache with current syskey values of syskey tables.
 *
 * All methods are multi-thread safe.
 */
class FileDir_SysKeyCache
{
public:
    /*!
     * @brief Constructor.
     */
    FileDir_SysKeyCache(SAPDBMem_IRawAllocator &allocator);

    /*!
     * @brief Initializes and clears the cache.
     */
    void Initialize();

    /*!
     * @brief Determines a new syskey for the given file.
     *
     * If there is no entry for fileNo present in the cache, false is
     * returned. Otherwise a unique new syskey is returned.
     */
    SAPDB_Bool GetNewSysKeyForFile(
        const FileDir_FileNo& fileNo,
        tgg00_Surrogate&      sysKey);

    /*!
     * @brief Adds an entry for fileNo to the cache and returns a new
     * syskey.
     *
     * If the fileNo to be added exists already in the cache, then
     * that value will be used and incremented just as if
     * GetNewSysKeyForFile had been called.
     */
    SAPDB_Bool AddAndGetNewSysKeyForFile(
        const FileDir_FileNo& fileNo,
        tgg00_Surrogate&      sysKey);

    /*!
     * @brief Removes an entry from the cache.
     *
     * No error will be set in case that there is no entry for fileNo.
     */
    void DeleteSysKeyFile(const FileDir_FileNo& fileNo);

private:
    // Copy constructor is not supported
    FileDir_SysKeyCache(const FileDir_SysKeyCache&);

    // Assignment operator is not supported
    FileDir_SysKeyCache& operator=(const FileDir_SysKeyCache&);

    class CacheItem
    {
    public:
        CacheItem();

        CacheItem(const tgg00_Surrogate& sysKey);

        void GetNextSysKey(tgg00_Surrogate& newSysKey);

    private:
        tgg00_Surrogate m_systemKey;
    };

    typedef Container_Hash<FileDir_FileNo, CacheItem> SysKeyCache;
    typedef SysKeyCache::Iterator                     Iterator;
    typedef SysKeyCache::ConstIterator                ConstIterator;

    FileDir_RWRegionWrapper     m_hashLock;
    SysKeyCache                 m_sysKeyCache;
};

/*===========================================================================*
 *  END CLASS                                                                *
 *===========================================================================*/

#endif // FILEDIR_SYSKEYCACHE_H
