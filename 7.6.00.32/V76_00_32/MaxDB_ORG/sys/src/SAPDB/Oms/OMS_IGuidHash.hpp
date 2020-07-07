/*!
 * \file    OMS_IGuidHash.hpp
 * \author  MarkusSi, Roterring
 * \brief   Hash-functions for class and container structures.
 */
/*


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



*/
#ifndef __OMS_IGUIDHASH_HPP
#define __OMS_IGUIDHASH_HPP

#include "Oms/OMS_Defines.h"
#include "gsp00.h"
#include "livecachetypes.h" // nocheck

class OMS_IGuidHash {
public:
  static tsp00_Int4 HashValue(ClassID guid, tsp00_Int4 maxSlot) {
    return ((tsp00_Uint4) omsMaskGuid(guid)) % ((tsp00_Uint4) maxSlot);
  }
  static tsp00_Int4 HashValue(ClassID guid, OmsSchemaHandle s, OmsContainerNo c, tsp00_Int4 maxSlot) {
    return ((tsp00_Uint4) (omsMaskGuid(guid) ^ (s * 48623) ^ (c * 7))) % ((tsp00_Uint4) maxSlot);
  }
};

#endif  // __OMS_IGUIDHASH_HPP
