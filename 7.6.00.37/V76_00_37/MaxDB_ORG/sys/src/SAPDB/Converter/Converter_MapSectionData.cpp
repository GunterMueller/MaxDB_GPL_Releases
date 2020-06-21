/*!
  @file     Converter_MapSectionData.cpp
  @author   TorstenS
  @ingroup  Converter
  @brief    Describe the plain converter map sections.
            For the locking of the map section data see
            module Converter_MapSection.hpp
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

#include "Converter/Converter_MapSectionData.hpp"
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

void
Converter_MapSectionData::Initialize(
    const RTE_RegionId      regionId,
    const SAPDB_UInt        sectionNo,
    Data_PageRecoveryMode   &recoveryMode,
    Converter_Map               &map )
{
    m_RegionId          = regionId;
    m_SectionNo         = sectionNo;
    m_NumUsedDataPages  = 0;
    m_MaxUsedEntryNo    = 0;

    m_pRecoveryMode     = &recoveryMode;
    m_pMap              = &map;

    m_UnusedEntryChain.Assign( map.m_EntryVector );
    m_PartiallyUsedEntryChain.Assign( map.m_EntryVector );
    m_FullUsedEntryChain.Assign( map.m_EntryVector );
}

/*---------------------------------------------------------------------------*/

void
Converter_MapSectionData::Shutdown()
{
    m_UnusedEntryChain.Reset();
    m_PartiallyUsedEntryChain.Reset();
    m_FullUsedEntryChain.Reset();
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
