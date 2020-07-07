/*!
  @defgroup   FrameControl Frame Management
*/
/*!
  @file     FrameCtrl_Types.hpp
  @author   TorstenS
  @ingroup  FrameControl        
  @brief          

\if EMIT_LICENCE
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

\endif
*/



#ifndef FRAMECTRL_TYPES_HPP
#define FRAMECTRL_TYPES_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"

/// global token for frame control component trace
extern SAPDBTrace_Topic FrameCtrl_Trace;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


 /*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

 /*!
   @brief   identifier for page frame access
 */

typedef enum
{
    FrameCtrl_ConverterKey = 0,
    FrameCtrl_DataCacheKey,
    FrameCtrl_LogKey,
    FrameCtrl_IOManagementKey,
    FrameCtrl_FileDirectoryKey,
    FrameCtrl_RestartPageKey,
    FrameCtrl_BlockAllocatorKey,
    FrameCtrl_CreateIndexKey
   // update SAPDB_ToString( FrameCtrl_RegistrationKey ) !
}
FrameCtrl_RegistrationKey;        

/// get return code strings for FrameCtrl_RegistrationKey
inline const SAPDB_ToStringClass 
SAPDB_ToString( FrameCtrl_RegistrationKey const regKey )
{
    switch( regKey )
    {
        case FrameCtrl_ConverterKey:        return SAPDB_ToStringClass( "ConverterKey" );
        case FrameCtrl_DataCacheKey:        return SAPDB_ToStringClass( "DataCacheKey" );
        case FrameCtrl_LogKey:              return SAPDB_ToStringClass( "LogKey" );
        case FrameCtrl_IOManagementKey:     return SAPDB_ToStringClass( "IOManagementKey" );
        case FrameCtrl_FileDirectoryKey:    return SAPDB_ToStringClass( "RestartPageKey" );
        case FrameCtrl_BlockAllocatorKey:   return SAPDB_ToStringClass( "BlockAllocatorKey" );
        case FrameCtrl_CreateIndexKey:      return SAPDB_ToStringClass( "CreateIndexKey" );
        default: break;
    }
    return SAPDB_ToStringClass( "unknown key" );
}

#endif  /* FRAMECTRL_TYPES_HPP */
