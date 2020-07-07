/*!
  @file     Converter_MapSectionData.hpp
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


#ifndef CONVERTER_MAP_SECTION_DATA_HPP
#define CONVERTER_MAP_SECTION_DATA_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_Types.hpp"
#include "Converter/Converter_Chain.hpp"
#include "Converter/Converter_Dump.hpp"
#include "Converter/Converter_MapEntry.hpp"
#include "Container/Container_Vector.hpp"

#include "RunTime/RTE_Types.hpp"


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 **/

class Converter_MapSection;
class Converter_PermMapSection;
class Converter_Map;
class Data_PageRecoveryMode;

/*===========================================================================*
 *  DEFINES                                                                  *
 **/

/*!
  @class    Converter_MapSectionData
  @brief    This class is a base class. It's used to offer the base facilities
            to manage a separate converter section, which is responsible for
            parts of the converter map and protected by it's own latch.
            The base class Converter_MapSectionData is able to manage the
            temporary part of the map.
            The access onto the Converter_MapSectionData will be done by
            a Converter_MapSection handler ONLY, which is responsible for the
            locking of the Converter_MapSectionData object, i.e. non access
            onto an object of this class is possible WITHOUT a lock handler.
 */

class Converter_MapSectionData
{
    friend class Converter_MapSection;
    friend class Converter_PermMapSection;

public:

    ///  vector which contains converter map entries (== the map column)
    typedef Container_Vector<Converter_MapEntry> MapVector;

    /// doubly linked chain of map entries
    typedef Converter_Chain<MapVector, Converter_MapEntry>   MapEntryChain;

public:

    /*!
       @brief   Default constructor
     */

    Converter_MapSectionData()
    :
    m_SectionNo( 0 ),
    m_RegionId( 0 ),
    m_NumUsedDataPages( 0 ),
    m_MaxUsedEntryNo( 0 ),
    m_pRecoveryMode( NULL ),
    m_pMap( NULL )
    {}

    /*!
    @brief   Initialize all members of the map section data to their initial values
    @param   regionId     [in] Identification of the region
    @param   sectionNo    [in] Logical number of the section( begining with 0)
    @param   recoveryMode [in] Recovery mode for the section
    @param   map          [in] Converter map vector, containing page handler
    @return  none
    */

    void Initialize(
        const RTE_RegionId      segionId,
        const SAPDB_UInt        sectionNo,
        Data_PageRecoveryMode   &recoveryMode,
        Converter_Map           &map );

    /*!
       @brief   Dumps the map section information into the kernel dump file.
       @param   dumpEntry [in|out] dump entry containing the dump info
       @return  none
    */

    void Dump( struct ConverterMapSection &dumpEntry )
    {
        dumpEntry.dmpSectionNo          = m_SectionNo;
        dumpEntry.dmpRegionId           = m_RegionId;
        dumpEntry.dmpUsedPageNoCount    = m_NumUsedDataPages;
        dumpEntry.dmpMaxUsedEntryNo     = m_MaxUsedEntryNo;
        dumpEntry.dmpUnusedEntryChain   = m_UnusedEntryChain.Begin().GetPosition();
        dumpEntry.dmpPartEntryChain     = m_PartiallyUsedEntryChain.Begin().GetPosition();
        dumpEntry.dmpFullEntryChain     = m_FullUsedEntryChain.Begin().GetPosition();
    }

private:

    /*!
       @brief   Release/Reset map section data
       @return  none
    */

    void Shutdown();

private:

    /// section identification

    SAPDB_UInt  m_SectionNo;

    /// latch needed for concurrent access

    RTE_RegionId  m_RegionId;

    /// number of all used data pages within the area managed by this section

    SAPDB_UInt   m_NumUsedDataPages;

    /// maximum entry number associated with a used page containing data page nos

    Converter_MapEntryNo    m_MaxUsedEntryNo;

    /// doubly linked chain of unused map entries

    MapEntryChain  m_UnusedEntryChain;

    /// doubly linked chains of partially used map entries

    MapEntryChain  m_PartiallyUsedEntryChain;

    /// doubly linked chains of completely filled map entries

    MapEntryChain  m_FullUsedEntryChain;

    /// pointer to the recovery mode of the corresponding map control.
    /// All sections of the map control have the same recovery mode

    Data_PageRecoveryMode   *m_pRecoveryMode;

    /// pointer to corresponding map control. All sections of the
    /// map control are using the same map.

    Converter_Map   *m_pMap;
};

#endif  /* CONVERTER_MAP_SECTION_DATA_HPP */

