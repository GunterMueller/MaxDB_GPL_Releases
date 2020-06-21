/*!
  @file           FBM_Types.hpp
  @author         TorstenS
  @author         AlexanderK
  @ingroup        FBM

\if EMIT_LICENCE
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

\endif
*/


#ifndef FBM_TYPES_HPP
#define FBM_TYPES_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/// define abort flag
#define FBM_ABORT_IF_ERROR      true

/*===========================================================================*
*  MACROS                                                                   *
*===========================================================================*/

/*===========================================================================*
*  EXTERNAL VARIABLES                                                       *
*===========================================================================*/

/// Checks for Free Block Management
extern SAPDBDiag_CheckTopic FBM_Check;
/// Trace for Free Block Management
extern SAPDBTrace_Topic     FBM_Trace;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/// state of blocks in the FBM
enum FBM_BlockState {
    BlockState_Free         = 0,
    BlockState_Occupied     = 1,
    BlockState_FreeAfterSVP = 2,
    BlockState_BackUp       = 3
};

/// get block states as strings for diagnostic output
inline const SAPDB_ToStringClass SAPDB_ToString( const FBM_BlockState blockState )
{
    switch (blockState)
    {
        case BlockState_Free:
            return SAPDB_ToStringClass( "Free" );
        case BlockState_Occupied:
            return SAPDB_ToStringClass( "Occupied" );
        case BlockState_FreeAfterSVP:
            return SAPDB_ToStringClass( "FreeAfterSvp" );
        case BlockState_BackUp:
            return SAPDB_ToStringClass( "Backup" );
        default: break;
    }
    return SAPDB_ToStringClass( "Unknown" );
}

/// constants for different FBM return codes
typedef enum {
    FBM_Okay                    = 0,
    FBM_Shutdown                = 1,
    FBM_OutOfMemory             = 2,
    FBM_Canceled                = 3,
    FBM_SystemError             = 4,
    FBM_NoMoreBackupBlocks      = 5,
    FBM_AlreadyStarted          = 6,
    FBM_IncompatibleVolumeCount = 7,
    FBM_NoMoreSpace             = 8
}
FBM_ReturnCode;

/// get return codes as strings for diagnostic output
inline const SAPDB_ToStringClass SAPDB_ToString( const FBM_ReturnCode retCode )
{
    switch( retCode )
    {
    case FBM_Okay:                      return SAPDB_ToStringClass( "Okay" );
    case FBM_Shutdown:                  return SAPDB_ToStringClass( "Shutdown" );
    case FBM_OutOfMemory:               return SAPDB_ToStringClass( "OutOfMemory" );
    case FBM_Canceled:                  return SAPDB_ToStringClass( "Canceled" );
    case FBM_SystemError:               return SAPDB_ToStringClass( "SystemError" );
    case FBM_NoMoreBackupBlocks:        return SAPDB_ToStringClass( "NoMoreBackupBlocks" );
    case FBM_AlreadyStarted:            return SAPDB_ToStringClass( "AlreadyStarted" );
    case FBM_IncompatibleVolumeCount:   return SAPDB_ToStringClass( "IncompatibleVolumeCount" );
    case FBM_NoMoreSpace:               return SAPDB_ToStringClass( "NoMoreSpace" );
    default: break;
    }
    return SAPDB_ToStringClass( "Unknown" );
}


#endif //FBM_TYPES_HPP
