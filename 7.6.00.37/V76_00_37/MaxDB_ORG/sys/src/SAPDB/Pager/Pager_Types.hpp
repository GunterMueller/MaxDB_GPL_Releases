/*!
  @file           Pager_Types.hpp
  @author         Henrik
  @brief          global types and constants
  @see            


    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



#ifndef PAGER_TYPES_HPP
#define PAGER_TYPES_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
/// trace topic for Pager component
extern SAPDBTrace_Topic Pager_Trace;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/// enumeration of Pager orders
typedef enum ePagerOrder {
    poNil,
    poFlushDataCache,
    poFlushDataCacheBeforePrepare,
    poFlushDataCacheAfterPrepare,
    poFlushDataCacheWithTempFromLRURechain,
    poFlushConverterCache,
    poReadConverterCache,
    poReadSnapShotConverter
    } enumPO;

/// enumeration of cluster mode 
typedef enum eClusteredIO {
    cioYes,
    cioNo,
    cioUndef
    } enumCIO;

#endif  /* PAGER_TYPES_HPP */
