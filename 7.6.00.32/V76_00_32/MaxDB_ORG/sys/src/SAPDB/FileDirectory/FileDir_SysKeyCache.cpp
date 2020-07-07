/****************************************************************************/
/*!

  @file       FileDir_SysKeyCache.cpp
  @author     MartinKi, TorstenS
  @ingroup    FileDirectory_Files

  @brief      Implementation of cache for syskey generation.

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_SysKeyCache.hpp"

#include "FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_RWRegionWrapper.hpp"

#include "Container/Container_Hash.hpp"

#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types
#include "hsp30.h"   // PASCAL: s30surrogate_incr

/*****************************************************************************/

FileDir_SysKeyCache::FileDir_SysKeyCache(SAPDBMem_IRawAllocator &allocator)
    : m_sysKeyCache( allocator )
{}

/*****************************************************************************/

void FileDir_SysKeyCache::Initialize()
{
    m_sysKeyCache.Delete();
}

/*****************************************************************************/

SAPDB_Bool FileDir_SysKeyCache::GetNewSysKeyForFile(
    const FileDir_FileNo& fileNo,
    tgg00_Surrogate&      sysKey)
{
    FileDir_RWAutoRegion writeLock( m_hashLock, true );

    Iterator iter = m_sysKeyCache.Find( fileNo );
    if( ! iter.IsValid() ) {
        return false;
    }
    iter->value.GetNextSysKey( sysKey );
    return true;
}

/*****************************************************************************/

SAPDB_Bool FileDir_SysKeyCache::AddAndGetNewSysKeyForFile(
    const FileDir_FileNo& fileNo,
    tgg00_Surrogate&      sysKey)
{
    FileDir_RWAutoRegion writeLock( m_hashLock, true );

    Iterator iter = m_sysKeyCache.Find( fileNo );
    if( ! iter.IsValid())
    {
        iter = m_sysKeyCache.Insert( fileNo, CacheItem( sysKey ) );
        if( ! iter.IsValid() ) {
            return false;
        }
    }
    iter->value.GetNextSysKey( sysKey );
    return true;
}

/*****************************************************************************/

void FileDir_SysKeyCache::DeleteSysKeyFile(const FileDir_FileNo& fileNo)
{
    FileDir_RWAutoRegion writeLock( m_hashLock, true );

    Iterator iter = m_sysKeyCache.Find( fileNo );
    m_sysKeyCache.Delete( iter ); // iter.IsValid() is executed inside
}

/*****************************************************************************/

FileDir_SysKeyCache::CacheItem::CacheItem()
{
    const char cNilKey[] =
        { '\xFF', '\xFE', '\x00', '\x00',
          '\x00', '\x00', '\x00', '\x00' };
    m_systemKey.rawAssign( cNilKey );
}

/*****************************************************************************/

FileDir_SysKeyCache::CacheItem::CacheItem(const tgg00_Surrogate& sysKey )
    : m_systemKey( sysKey )
{}

/*****************************************************************************/

void FileDir_SysKeyCache::CacheItem::GetNextSysKey(tgg00_Surrogate& newSysKey)
{
    s30surrogate_incr( m_systemKey );
    newSysKey =  m_systemKey;
}

/*****************************************************************************/
