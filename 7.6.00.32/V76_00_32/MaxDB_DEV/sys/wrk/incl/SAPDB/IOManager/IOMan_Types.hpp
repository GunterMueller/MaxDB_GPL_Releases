/*!
  @defgroup   IOManagement I/O Management
*/
/*!
  @file     IOMan_Types.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Basic types of this component
  @see            

\if EMIT_LICENCE
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

\endif
*/


#ifndef IOMAN_TYPES_HPP
#define IOMAN_TYPES_HPP

#include "KernelCommon/Kernel_Common.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp"


/// external reference of IOManager trace
extern SAPDBTrace_Topic     IOMan_Trace;

/// external reference of IOManager checks
extern SAPDBDiag_CheckTopic IOMan_Check;

/// Logical volume number
typedef SAPDB_Invalidatable<SAPDB_Int2, SAPDB_MAX_INT2> IOMan_DeviceNo;

/// Logical volume number
typedef SAPDB_Invalidatable<SAPDB_Int2, SAPDB_MAX_INT2> IOMan_VolumeNo;

/// Logical block offset on volume
typedef SAPDB_Invalidatable<SAPDB_Int4, SAPDB_MAX_INT4> IOMan_BlockNo;

/// Number of blocks
typedef SAPDB_Int4 IOMan_BlockCount;

/// Number of clusters
typedef SAPDB_Int4 IOMan_ClusterCount;

/// cluster number
typedef SAPDB_Invalidatable<SAPDB_Int4, SAPDB_MAX_INT4> IOMan_ClusterNo;

/// ReturnCodes of component I/O Manager
typedef enum
{
    IOMan_Okay                      = 0,
    IOMan_NoMoreMemory              = 1,
    IOMan_Canceled                  = 2,
    IOMan_SystemError               = 4,
    IOMan_NotConfigured             = 5,
    IOMan_AttachFailed              = 6,
    IOMan_ConfigurationCorrupted    = 7,
    IOMan_FormatFailed              = 8,
    IOMan_BadVolume                 = 9,
    IOMan_VolumeCapacityMismatch    = 10,
    IOMan_ReadError                 = 11,
    IOMan_WriteError                = 12,
    IOMan_BadInfoPage               = 13,
    IOMan_NoConverterEntry          = 14,
    IOMan_BadDataPage               = 15,
    IOMan_NotPreparedForMigration   = 16,
    IOMan_PageNoOutOfRange          = 17,
    IOMan_DropNotPossible           = 18,
    IOMan_DropDBFull                = 19,
    IOMan_DropFailed                = 20,
    IOMan_VolumeOffline             = 21
    // 1. do not use
    // 2. do not append after this
    // 3. do update IOMan_ReturnCodeStrings
} IOMan_ReturnCode;

/// string mapping for IOMan return codes / IOMan_ReturnCode
/// get return codes as strings for diagnostic output
inline const SAPDB_ToStringClass SAPDB_ToString( const IOMan_ReturnCode  retCode )
{
    switch( retCode )
    {
    case IOMan_Okay:                    return SAPDB_ToStringClass( "Okay" );
    case IOMan_NoMoreMemory:            return SAPDB_ToStringClass( "NoMoreMemory" );
    case IOMan_Canceled:                return SAPDB_ToStringClass( "Canceled" );
    case IOMan_SystemError:             return SAPDB_ToStringClass( "SystemError" );
    case IOMan_NotConfigured:           return SAPDB_ToStringClass( "VolumeNotConfigured" );
    case IOMan_AttachFailed:            return SAPDB_ToStringClass( "AttachFailed" );
    case IOMan_ConfigurationCorrupted:  return SAPDB_ToStringClass( "ConfigurationCorrupted" );
    case IOMan_FormatFailed:            return SAPDB_ToStringClass( "FormatFailed" );
    case IOMan_BadVolume:               return SAPDB_ToStringClass( "BadVolume" );
    case IOMan_VolumeCapacityMismatch:  return SAPDB_ToStringClass( "VolumeCapacityMismatch" );
    case IOMan_ReadError:               return SAPDB_ToStringClass( "ReadError" );
    case IOMan_WriteError:              return SAPDB_ToStringClass( "WriteError" );
    case IOMan_BadInfoPage:             return SAPDB_ToStringClass( "BadInfoPage" );
    case IOMan_NoConverterEntry:        return SAPDB_ToStringClass( "NoConverterEntry" );
    case IOMan_BadDataPage:             return SAPDB_ToStringClass( "BadDataPage" );
    case IOMan_NotPreparedForMigration: return SAPDB_ToStringClass( "NotPreparedForMigration" );
    case IOMan_PageNoOutOfRange:        return SAPDB_ToStringClass( "PageNoOutOfRange" );
    case IOMan_DropNotPossible:         return SAPDB_ToStringClass( "DropNotPossible" );       
    case IOMan_DropDBFull:              return SAPDB_ToStringClass( "DropDBFull" );
    case IOMan_DropFailed:              return SAPDB_ToStringClass( "DropFailed" );
    case IOMan_VolumeOffline:           return SAPDB_ToStringClass( "VolumeOffline" );
    default:                            return SAPDB_ToStringClass( "Unknown" );
    }
}

#endif // IOMAN_TYPES_HPP

