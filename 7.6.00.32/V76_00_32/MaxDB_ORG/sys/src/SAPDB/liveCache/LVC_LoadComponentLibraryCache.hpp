/*!
 * \file    LVC_LoadComponentLibraryCache.hpp
 * \author  FerdiF
 * \brief   Load a component library and calculate an MD5 footprint.
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

*/

#ifndef  __LVC_LOADCOMPONENTLIBRARYCACHE_HPP
#define  __LVC_LOADCOMPONENTLIBRARYCACHE_HPP

#include "liveCache/LVC_ILoadComponentLibraryCache.hpp"
#include "SAPDBCommon/SAPDB_Singleton.hpp"
#include "Container/Container_Hash.hpp"
#include "Oms/OMS_Defines.h"

//-------------------------------------------------------------------------------------------------

typedef struct LibraryCacheEntry
{
  tsp00_C32 MD5Footprint;
  HANDLE    hInstance;
} LibraryCacheEntry;

typedef Container_Hash<tsp00_C8, LibraryCacheEntry>  LibraryCache;

//-------------------------------------------------------------------------------------------------

class LVC_LoadComponentLibraryCache 
  : public LVC_ILoadComponentLibraryCache 
  , public SAPDB_SingletonTemplate<LVC_LoadComponentLibraryCache>
{
public:
  /* 
    CTOR 
  */
  LVC_LoadComponentLibraryCache();
  /* 
    DTOR 
  */
  ~LVC_LoadComponentLibraryCache();
  /*
  @brief searches cached library information. 
  */
  bool Find(const tsp00_C8 &packageId, HANDLE* hInstance, tsp00_C32 &MD5Footprint);
  /*
  @brief inserts library information into cache 
  */
  void Insert(const tsp00_C8 &packageId, HANDLE const* hInstance, const tsp00_C32 &MD5Footprint);
  /*
  @brief returns a reference to the instance implementing the interface, see LVC_LoadComponentLibraryCache.cpp
  */
  static LVC_ILoadComponentLibraryCache& GetInstance();
private:
  LibraryCache*  m_LibraryCache;
};
#endif