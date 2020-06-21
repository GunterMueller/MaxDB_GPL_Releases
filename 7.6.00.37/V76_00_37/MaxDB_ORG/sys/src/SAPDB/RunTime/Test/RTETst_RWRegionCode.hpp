/*!
  @file           RTETst_RWRegionCode.hpp
  @author         RobinW
  @ingroup        Runtime
  @brief          Runtime Region Test registration

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


*****************************************************************************/

#ifndef RTETST_RWREGIONCODE_HPP
#define RTETST_RWREGIONCODE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
class SAPDBErr_MessageList;

/*!
  @brief Register RTE test routines for region code

  @param errList [out] message list filled in case of error
  @return true if call succeeds, false if fails
 */
SAPDB_Bool RTETst_RegisterRWRegionTest(SAPDBErr_MessageList &errList);

#endif /* RTETST_RWREGIONCODE_HPP */
