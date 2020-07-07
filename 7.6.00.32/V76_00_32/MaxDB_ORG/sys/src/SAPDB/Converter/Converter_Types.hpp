/*!
  @file           Converter_Types.hpp
  @author         TorstenS
  @author         TillL
  @ingroup        Converter
  @brief          Common definitions of this component
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2000-2004 SAP AG

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

#ifndef CONVERTER_TYPES_HPP
#define CONVERTER_TYPES_HPP

#include "Converter/Converter_Version.hpp"

#include "KernelCommon/Kernel_Common.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"

#include "SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"


/*===========================================================================*
*  EXTERNAL VARIABLES                                                       *
**/

/// check level for debug
extern SAPDBDiag_CheckTopic Converter_Check;
/// trace level for debug
extern SAPDBTrace_Topic     Converter_Trace;

/// @brief          number of entries of converter parent index
typedef SAPDB_UInt2 Converter_IndexEntryCount;

/// number which identifies a specific converter parent index entry
typedef SAPDB_Invalidatable<SAPDB_UInt2, SAPDB_MAX_UINT2> Converter_IndexEntryNo;

/// number of entries on a converter page
typedef SAPDB_UInt2 Converter_PageEntryCount;

/// number which identifies a specific converter page entry
typedef SAPDB_Invalidatable<SAPDB_UInt2, SAPDB_MAX_UINT2> Converter_PageEntryNo;

/// number of entries of converter map
typedef SAPDB_UInt4 Converter_MapEntryCount;

/// number which identifies a specific converter map entry
typedef SAPDB_Invalidatable<SAPDB_UInt4, SAPDB_MAX_UINT4> Converter_MapEntryNo;

/// constants for different converter page types
typedef enum {
    Converter_IndexPageType,
    Converter_LeafPageType
}
Converter_PageType;

inline const SAPDB_Char*
Converter_PageTypeStrings( const Converter_PageType  pageType )
{
    switch( pageType )
    {
    case Converter_IndexPageType:   return(( SAPDB_Char* )"index" );
    case Converter_LeafPageType:    return(( SAPDB_Char* )"leaf" );
    default:                        return(( SAPDB_Char* )"unknown" );
    }
}

// AFTER MODIFICATION OF RETURN CODE, PLEASE ADJUST SAPDB_ToString( Converter_ReturnCode ...
/// constants for different converter error reasons
typedef enum {
    Converter_Ok                        = 0,
    Converter_Shutdown                  = 1,
    Converter_MemoryAllocationFailed    = 2,
    Converter_ExecutionCanceled         = 3,
    Converter_SystemError               = 4,
    Converter_PageNoAlreadyInUse        = 5,
    Converter_FrameAllocationFailed     = 6,
    Converter_ConverterPageIOPending    = 7,
    Converter_InvalidBlockAddressFound  = 8,
    Converter_PageNoNotReserved         = 9,
    Converter_PageNoAlreadyFree         = 10,
    Converter_DuplicatePageNo           = 11,
    Converter_SavePointActive           = 12,
    Converter_NoPageNotAvailable        = 13,
    Converter_NoConverterEntry          = 14,
    Converter_PageNoOutOfRange          = 15,
    Converter_IncompatibleMode          = 16,
    Converter_TempSpaceExhausted        = 17,
    Converter_LAST_AND_NOT_USEABLE_ENUM
    // 1. do not use
    // 2. do not append after this
    // 3. do update SAPDB_ToString( Converter_ReturnCode ...
}Converter_ReturnCode;

inline const SAPDB_ToStringClass 
SAPDB_ToString( const Converter_ReturnCode  retCode )
{
    switch( retCode )
    {
    case Converter_Ok:                          return SAPDB_ToStringClass( "Okay" );
    case Converter_Shutdown:                    return SAPDB_ToStringClass( "Shutdown" );
    case Converter_MemoryAllocationFailed:      return SAPDB_ToStringClass( "OutOfMemory" );
    case Converter_ExecutionCanceled:           return SAPDB_ToStringClass( "Canceled" );
    case Converter_SystemError:                 return SAPDB_ToStringClass( "SystemError" );
    case Converter_PageNoAlreadyInUse:          return SAPDB_ToStringClass( "PageNoAlreadyInUse" );
    case Converter_FrameAllocationFailed:       return SAPDB_ToStringClass( "FrameAllocationFailed" );
    case Converter_ConverterPageIOPending:      return SAPDB_ToStringClass( "ConverterPageIOPending" );
    case Converter_InvalidBlockAddressFound:    return SAPDB_ToStringClass( "InvalidBlockAddressFound" );
    case Converter_PageNoNotReserved:           return SAPDB_ToStringClass( "PageNoNotReserved" );
    case Converter_PageNoAlreadyFree:           return SAPDB_ToStringClass( "PageNoAlreadyFree" );
    case Converter_DuplicatePageNo:             return SAPDB_ToStringClass( "DuplicatePageNo" );
    case Converter_SavePointActive:             return SAPDB_ToStringClass( "SavePointActive" );
    case Converter_NoPageNotAvailable:          return SAPDB_ToStringClass( "NoPageNotAvailable" );
    case Converter_NoConverterEntry:            return SAPDB_ToStringClass( "NoConverterEntry" );
    case Converter_PageNoOutOfRange:            return SAPDB_ToStringClass( "PageNoOutOfRange" );
    case Converter_IncompatibleMode:            return SAPDB_ToStringClass( "IncompatibleMode" );
    case Converter_TempSpaceExhausted:          return SAPDB_ToStringClass( "TempSpaceExhausted" );
    default:                                    return SAPDB_ToStringClass( "Unknown" );
    }
}

/// constants for garbage collection needed requests
typedef enum {
    Converter_GCNotWanted = 0,
    Converter_GCNotNeeded,
    Converter_GCNeeded
}
Converter_GarbageCollectionReturnCode;


#endif // CONVERTER_TYPES_HPP

