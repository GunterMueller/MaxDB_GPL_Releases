/****************************************************************************

  module      : RTE_Types.hpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RunTime Typedefinitions
  description : Contains all common RunTime Type definitions

  last changed: 2000-06-15  14:25
  see also    : RunTime/RTE_Types.h

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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


#ifndef RTE_TYPES_HPP
#define RTE_TYPES_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

// All RTE type definitions are shared with C-Sources !!!!
extern "C"
{
#include "RunTime/RTE_Types.h"
}
#include "SAPDBCommon/SAPDB_ToString.hpp"

/*!
  @brief Convert RTE_VolumeType into readable string
  @param type [in] type to convert
 */

inline SAPDB_ToStringClass SAPDB_ToString(RTE_VolumeType type)
{
    switch (type)
    {
    case RTE_VolumeTypeSystem: 
        return SAPDB_ToStringClass("System Devspace");
    case RTE_VolumeTypeLog: 
        return SAPDB_ToStringClass("Log Volume");
    case RTE_VolumeTypeMirrorLog: 
        return SAPDB_ToStringClass("Mirrored Log Volume");
    case RTE_VolumeTypeData: 
        return SAPDB_ToStringClass("Data Volume");
    case RTE_VolumeTypeTrace: 
        return SAPDB_ToStringClass("Trace Volume");
    default:                          
        break;
    }
    return SAPDB_ToStringClass((SAPDB_Int4)type);
}

#endif  /* RTE_TYPES_HPP */
