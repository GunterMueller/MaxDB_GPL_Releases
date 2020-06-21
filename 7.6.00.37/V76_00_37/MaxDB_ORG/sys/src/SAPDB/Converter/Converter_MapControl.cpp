/*!
  @file     Converter_MapControl.cpp
  @author   TorstenS
  @author   AlexanderK
  @ingroup  Converter
  @brief    Base class for fundamental functionality of handling
            concurrent access to the converter map, which is used
            to transform pageNo's into block addresses.
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

#include "Converter/Converter_MapControl.hpp"
#include "Converter/Converter_MapSection.hpp"
#include "Converter/Converter_Dump.hpp"

#include "Converter/Converter_MapPeriodIterator.hpp"
#include "Converter/Converter_MapClusterIterator.hpp"
#include "Converter/Converter_MapSectionIterator.hpp"

#include "KernelCommon/Kernel_Dump.hpp"

#include "RunTime/Tasking/RTETask_ITask.hpp"


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
Converter_MapControl::Init (
    const RTETask_ITask  &task,
    const SAPDB_UInt     maxDataPnoInMap,
    const RTE_RegionId   &baseRegionId,
    const SAPDB_UInt     numConverterRegions,
    const SAPDB_UInt     clusterSize,
    const SAPDB_UInt     converterPageSize )
{
    SAPDBTRACE_ROUTINE_DEBUG( "MapControl::Init", Converter_Trace, 5 );

    SAPDBERR_ASSERT_RANGE( numConverterRegions, 1, MAX_CONVERTER_SECTIONS );
    SAPDBERR_ASSERT_STATE( clusterSize > 0 );

    m_NextSectionToSearchForNewPageNo = 0;

    m_Map.Initialize( clusterSize, numConverterRegions, converterPageSize );

    if(! m_SectionData.Resize( numConverterRegions )){
        return( SAPDB_FALSE );
    }

    for( SectionDataIterator iter = m_SectionData.Begin(); iter != m_SectionData.End(); ++iter )
    {
        const SAPDB_UInt sectionNo = GetSectionNo( iter );

        m_SectionData[ sectionNo ].Initialize( baseRegionId + sectionNo,
                                               sectionNo, m_RecoveryMode, m_Map );
    }

    return( Converter_Ok == Expand( task, maxDataPnoInMap ) );
}

/*---------------------------------------------------------------------------*/

void
Converter_MapControl::Shutdown( const RTETask_ITask  &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "MapControl::Shutdown", Converter_Trace, 5 );

    {
        Converter_MapSection    sections[ MAX_CONVERTER_SECTIONS ];

        SectionDataIterator         iter;
        const SectionDataIterator   endIter = m_SectionData.End();

        for( iter = m_SectionData.Begin(); iter != endIter; ++iter )
        {
            const SAPDB_UInt sectionNo = GetSectionNo( iter );

            sections[ sectionNo ].Lock( task, *iter );
            sections[ sectionNo ].Shutdown();
        }
    }
    // Do not destroy section data, because concurrent running
    // task should not fail/crash.
    // m_SectionData.Delete();

    m_Map.Shutdown();
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_MapControl::Expand(
    const RTETask_ITask &task,
    const SAPDB_UInt    newNumAllPages )
{
    SAPDBTRACE_ROUTINE_DEBUG( "MapControl::Expand", Converter_Trace, 5 );

    Converter_MapSection    sections[ MAX_CONVERTER_SECTIONS ];

    SectionDataIterator         iter;
    const SectionDataIterator   endIter = m_SectionData.End();

    for( iter = m_SectionData.Begin(); iter != endIter; ++iter )
    {
        sections[ GetSectionNo( iter ) ].Lock( task, *iter );
    }

    SAPDB_Int  addPeriodCount; // PTS 1126701 TS 2003-12-19

    if( ! m_Map.Expand( newNumAllPages, addPeriodCount )){
        return Converter_MemoryAllocationFailed;
    }

    if( 0 >= addPeriodCount ) // PTS 1126701 TS 2003-12-19
        return Converter_Ok;

    for( iter = m_SectionData.Begin(); iter != endIter; ++iter )
    {
        m_Map.ExpandOneSection( sections[ GetSectionNo( iter ) ], addPeriodCount );
    }

    return Converter_Ok;
}
/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_MapControl::RequestNewPageNo(
    const RTETask_ITask &task,
    Data_PageNo         &pageNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "MapControl::RequestNewPageNo", Converter_Trace, 5 );

    // ensure that the m_NextSectionToSearchForNewPageNo counter is always
    // in the range of zero and m_NumUsedSections - 1. Already a short temporay
    // overflow in case of concurrent access could run to a access violation
    // in the structures dealing with a sectionNo as an input parameter.

    SAPDB_Bool          bFound       = SAPDB_FALSE;
    const SAPDB_UInt    sectionCount = GetSectionCount();

    SAPDB_UInt startSectionForSearch = m_NextSectionToSearchForNewPageNo;
    startSectionForSearch = ((++startSectionForSearch) % sectionCount);

    m_NextSectionToSearchForNewPageNo = startSectionForSearch; //dirty counter is okay
    SAPDB_UInt nextSectionForSearch   = startSectionForSearch;

    do
    {
        Converter_MapSection section( task, m_SectionData[ nextSectionForSearch ] );

        const Converter_ReturnCode retCode = m_Map.RequestNewPageNo( section, pageNo );

        if( Converter_NoPageNotAvailable != retCode ){
            bFound = ( Converter_Ok == retCode );
            break;
        }

        nextSectionForSearch = (( ++nextSectionForSearch ) % sectionCount );

        if( nextSectionForSearch == startSectionForSearch ){
            break; // no free PageNo available
        }
    }
    while( SAPDB_TRUE );

    return( bFound );
}

/*---------------------------------------------------------------------------*/

void
Converter_MapControl::Dump(
    const RTETask_ITask &task,
    Kernel_Dump         &dump,
    const SAPDB_Bool    bEnterRegion )
{
    Converter_MapSection sections[ MAX_CONVERTER_SECTIONS ];

    // Attention: Don't check the correctness of the converter in case
    // of a dump, because it's possible that the dump was initiated by
    // a corrupted converter map!

    SectionDataIterator         iter;
    const SectionDataIterator   endIter = m_SectionData.End();

    for( iter = m_SectionData.Begin(); iter != endIter; ++iter )
    {
        const SAPDB_UInt sectionNo = GetSectionNo( iter );

        if( SAPDB_TRUE == bEnterRegion ){
            sections[ sectionNo ].Lock( task, *iter, SAPDB_FALSE );
        }

        struct ConverterMapSection  dumpEntry;

        dumpEntry.dmpIsPerm    = GetRecoveryMode().PageIsRecoverable();
        dumpEntry.dmpIsDynamic = GetRecoveryMode().UseDynamicPageAddressing();

        m_SectionData[ sectionNo ].Dump( dumpEntry );

        dump.InsertEntry( Kernel_Dump::DmpConverterMapSection,
                          Kernel_DumpPage::Entry( &dumpEntry, sizeof( dumpEntry )));
    }

    // Use the all sections spanned(!) iterator of the proper converter map

    if( 0 < GetSectionCount() )
        m_Map.DumpEntries( dump );
}

Converter_MapClusterIterator
Converter_MapControl::ClusterBegin(const SAPDB_UInt section, const SAPDB_UInt period, const IOMan_DeviceNo volNo)
{
    return Converter_MapClusterIterator(m_Map, m_Map.EntryClusterBeginNo(section, period), m_Map.EntryClusterEndNo(section, period), volNo);
}

Converter_MapClusterIterator
Converter_MapControl::ClusterEnd(const SAPDB_UInt section, const SAPDB_UInt period, const IOMan_DeviceNo volNo)
{
    return Converter_MapClusterIterator(m_Map, m_Map.EntryClusterEndNo(section, period), m_Map.EntryClusterEndNo(section, period), volNo);
}


Converter_MapPeriodIterator
Converter_MapControl::PeriodBegin(const SAPDB_UInt section, const IOMan_DeviceNo volNo)
{
    return Converter_MapPeriodIterator(*this, section, 0, m_Map.GetPeriodCount(), volNo);
}

Converter_MapPeriodIterator
Converter_MapControl::PeriodEnd(const SAPDB_UInt section, const IOMan_DeviceNo volNo)
{
    return Converter_MapPeriodIterator(*this, section , m_Map.GetPeriodCount(), 0, volNo);
}

Converter_MapSectionIterator
Converter_MapControl::SectionBegin(const IOMan_DeviceNo volNo, const RTETask_ITask &task)
{
    return Converter_MapSectionIterator(*this, 0 , GetSectionCount(), volNo, task);
}

Converter_MapSectionIterator
Converter_MapControl::SectionEnd(const IOMan_DeviceNo volNo, const RTETask_ITask &task)
{
    return Converter_MapSectionIterator(*this, GetSectionCount() , 0, volNo, task);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
