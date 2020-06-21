/*!
 * \file    LVC_ILoadComponentLibraryCache.hpp
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

#ifndef  __LVC_ILOADCOMPONENTLIBRARYCACHE_HPP
#define  __LVC_ILOADCOMPONENTLIBRARYCACHE_HPP

#include "gsp00.h"
#include "Oms/OMS_Defines.h"
/*!
   @brief defines the interface for Cache with information about loaded libraries
 */

class LVC_ILoadComponentLibraryCache
{
public :
  /*
  @brief searches cached library information. 
  */
  virtual bool Find(const tsp00_C8 &packageId, HANDLE* hInstance, tsp00_C32 &MD5Footprint) = 0;
  /*
  @brief inserts library information into cache 
  */
  virtual void Insert(const tsp00_C8 &packageId, HANDLE const* hInstance, const tsp00_C32 &MD5Footprint) = 0;
  /*
  @brief returns a reference to the instance implementing the interface, see LVC_LoadComponentLibraryCache.cpp
  */
  static LVC_ILoadComponentLibraryCache& GetInstance();
};

#endif