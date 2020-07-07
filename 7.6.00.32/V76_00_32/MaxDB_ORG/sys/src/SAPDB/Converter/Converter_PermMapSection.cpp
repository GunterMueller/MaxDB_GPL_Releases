/*!
  @file     Converter_PermMapSection.cpp
  @author   TorstenS
  @ingroup  Converter
  @brief    This calls is used to manage the concurrent access
            to the Converter_PermMapSectionData information.
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

#include "Converter/Converter_PermMapSection.hpp"
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

Converter_PermMapSection::ChangedEntryIterator
Converter_PermMapSection::MarkPageAsUnchangedAndGoToNextChangedEntry(
    const Converter_MapEntryNo  entryNo )
{
    SAPDBERR_ASSERT_STATE( m_pSectionData->m_pMap->PageIsChanged( entryNo ));

    m_pSectionPermData->m_pNumChangedConverterPages->Decrement();

    return( ChangedEntryIterator
            (m_pSectionPermData->m_ChangedChain.Remove( entryNo ),
             *(m_pSectionData->m_pMap )));
}

/*---------------------------------------------------------------------------*/

void
Converter_PermMapSection::MarkPageAsChanged( const Converter_MapEntryNo  entryNo )
{
    if( m_pSectionData->m_pMap->PageIsChanged( entryNo ))
        return;

    m_pSectionPermData->m_ChangedChain.InsertFront( entryNo );
    m_pSectionPermData->m_pNumChangedConverterPages->Increment();
}


/*---------------------------------------------------------------------------*/

void
Converter_PermMapSection::MarkPageAsUnChanged( const Converter_MapEntryNo  entryNo )
{
    if( ! m_pSectionData->m_pMap->PageIsChanged( entryNo ))
        return;

    m_pSectionPermData->m_ChangedChain.Remove( entryNo );
    m_pSectionPermData->m_pNumChangedConverterPages->Decrement();
}

/*---------------------------------------------------------------------------*/

Data_PageNo
Converter_PermMapSection::GetMaxUsedPageNo() const
{
    return(( m_pSectionData->m_MaxUsedEntryNo + 1) *
           m_pSectionData->m_pMap->GetLeafPageCapacity() - 1 );
}

/*---------------------------------------------------------------------------*/

Converter_PermMapSection::ChangedEntryIterator::ChangedEntryIterator(
    const Parent    &changedChainIter,
    Converter_Map   &map )
        :
        Parent( changedChainIter ),
        m_pEntryVector( &map.m_EntryVector )
{}

/*---------------------------------------------------------------------------*/

Converter_MapEntry&
Converter_PermMapSection::ChangedEntryIterator::operator*() const
{
    // this iterator does not return its node but the corresponding
    // map entry with the same index position m_Pos
    return (*m_pEntryVector)[m_Pos];
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
