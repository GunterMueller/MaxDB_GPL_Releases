/*!
  @file     Converter_PermMapSectionData.cpp
  @author   TorstenS
  @ingroup  Converter
  @brief    Describe the permanent converter map sections.
            For the locking of the map section data see
            module Converter_PermMapSection.hpp
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Converter/Converter_PermMapSectionData.hpp"
#include "Converter/Converter_Map.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

SAPDB_Bool
Converter_PermMapSectionData::Initialize(
    Converter_Map                           &map,
    RTESync_InterlockedCounter<SAPDB_UInt>  &numChangedConverterPages,
    const SAPDB_Bool                        bLastSaveDataSuccessful )
{
    m_SavepointIsActive          = false;
    m_LastSaveDataSuccessful     = bLastSaveDataSuccessful;
    m_PendingIO                  = 0;
    m_NumUpdatedPermPages        = 0;
    m_NumPendingUpdatedPermPages = 0;
    m_pNumChangedConverterPages  = &numChangedConverterPages;


    const SAPDB_UInt cMinimumParentRefs = 10;
    const SAPDB_UInt cMinimumListSize   = 512;

    if( ! m_ParentRefs.Reserve( cMinimumParentRefs )){
        return( SAPDB_FALSE );
    }
    if( ! m_FreePageNoList.Initialize( cMinimumListSize )){
        return( SAPDB_FALSE );
    }

    m_ChangedChain.Assign( map.m_ChangedInfoVector );

    return( SAPDB_TRUE );
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
