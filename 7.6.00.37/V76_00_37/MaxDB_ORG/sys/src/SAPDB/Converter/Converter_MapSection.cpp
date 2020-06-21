/*!
  @file     Converter_MapSection.cpp
  @author   TorstenS
  @ingroup  Converter
  @brief    This calls is used to manage the concurrent access
            to the Converter_MapSectionData information.
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

#include "Converter/Converter_MapSection.hpp"
#include "Converter/Converter_Map.hpp"

#include "heo55k.h"     // vbegexcl

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
Converter_MapSection::Lock()
{
    SAPDBERR_ASSERT_STATE( ! m_bIsLocked );
    SAPDBERR_ASSERT_STATE( GetTaskId() == m_TaskId );

    m_bIsLocked = true;

    vbegexcl( m_TaskId, m_pSectionData->m_RegionId );
}

/*---------------------------------------------------------------------------*/

void
Converter_MapSection::Unlock()
{
    if( m_bIsLocked )
    {
#           ifdef SAPDB_FAST
                const SAPDB_Bool bDoCheck = Converter_Check.ChecksLevel( 5 );
#           else
                const SAPDB_Bool bDoCheck = SAPDB_TRUE;
#           endif

        if(( m_bUnlockWithCheck ) && ( bDoCheck )){
            m_pSectionData->m_pMap->Verify( *this );
        }

        vendexcl (m_TaskId, m_pSectionData->m_RegionId );

        m_bIsLocked = false;
    }
}

/*---------------------------------------------------------------------------*/

void
Converter_MapSection::CheckDecMaxDataPageNo( const Converter_MapEntryNo entryNo)
{
    if( entryNo == m_pSectionData->m_MaxUsedEntryNo )
    {
        Converter_MapEntryNo newMaxUsedEntryNo = 0;
        {
            PartiallyUsedEntryIterator       iter    = PartiallyUsedEntriesBegin();
            const PartiallyUsedEntryIterator endIter = PartiallyUsedEntriesEnd();

            while( iter != endIter ){
                if( iter.GetPosition() > newMaxUsedEntryNo ){
                    newMaxUsedEntryNo = iter.GetPosition();
                }
                ++iter;
            }
        }
        {
            FullEntryUsedIterator       iter    = FullUsedEntriesBegin();
            const FullEntryUsedIterator endIter = FullUsedEntriesEnd();

            while( iter != endIter ){
                if( iter.GetPosition() > newMaxUsedEntryNo ){
                    newMaxUsedEntryNo = iter.GetPosition();
                }
                ++iter;
            }
        }
        m_pSectionData->m_MaxUsedEntryNo = newMaxUsedEntryNo;
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
