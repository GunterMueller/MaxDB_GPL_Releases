/*!
  @file     Converter_PermMapSectionData.hpp
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


#ifndef CONVERTER_PERM_MAP_SECTION_DATA_HPP
#define CONVERTER_PERM_MAP_SECTION_DATA_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_Types.hpp"
#include "Converter/Converter_Chain.hpp"
#include "Converter/Converter_Dump.hpp"
#include "Converter/Converter_FreePageList.hpp"
#include "Converter/Converter_ParentReference.hpp"
#include "Container/Container_Vector.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 **/

class Converter_PermMapSection;
class Converter_Map;
class SAPDBMem_IRawAllocator;

/*===========================================================================*
 *  DEFINES                                                                  *
 **/

/*!
  @class    Converter_PermMapSectionData
  @brief
 */

class Converter_PermMapSectionData
{
    friend class Converter_PermMapSection;

public:

    /// an object containing info about whether a leaf page is changed
    typedef Converter_ChainNode<Converter_MapEntryNo>   ChangedInfo;

    /*!
       @brief   constructor
       @param   allocator [in|out] for memory allocation
     */

    Converter_PermMapSectionData( SAPDBMem_IRawAllocator  &allocator)
    :
    m_SavepointIsActive( SAPDB_FALSE ),
    m_LastSaveDataSuccessful( SAPDB_FALSE ),
    m_PendingIO( 0 ),
    m_NumUpdatedPermPages( 0 ),
    m_NumPendingUpdatedPermPages( 0 ),
    m_FreePageNoList( allocator ),
    m_ParentRefs( allocator ),
    m_pNumChangedConverterPages( NULL )
    {}

    /*!
        @brief  Initialize all members of the map section data to their initial values
        @param  map                      [in|out] converter map vector, containing
                                                  page handler
        @param  numChangedConverterPages [in|out] reference to the map control global
                                                  dirty converter page counter
        @param   bLastSaveDataSuccessful [in]     last save data finished successfully
        @return true if execution was successful
    */

    SAPDB_Bool Initialize(
        Converter_Map                           &map,
        RTESync_InterlockedCounter<SAPDB_UInt>  &numChangedConverterPages,
        const SAPDB_Bool                        bLastSaveDataSuccessful );

    /*!
       @brief   Dumps the map section information into the kernel dump file.
       @param   dumpEntry [in|out] dump entry containing the dump info
       @return  none
    */

    void Dump( struct ConverterMapSection &dumpEntry )
    {
        dumpEntry.dmpSavepointIsActive          = m_SavepointIsActive;
        dumpEntry.dmpLastDataSuccessful         = m_LastSaveDataSuccessful;
        dumpEntry.dmpPendingIO                  = m_PendingIO;
        dumpEntry.dmpNumUpdatedPermPages        = m_NumUpdatedPermPages;
        dumpEntry.dmpNumPendingUpdatedPermPages = m_NumPendingUpdatedPermPages;
    }

private:

    /*!
    @brief  Shutdown is used to release the allocated memory
    @return none
    */

    void Shutdown()
    {
        m_FreePageNoList.Clear();
        m_ParentRefs.Delete();
        m_ChangedChain.Reset();
    }

private:

    /// flag is true if savepoint is active

    SAPDB_Bool  m_SavepointIsActive;

    /// flag is true if last complete databackup was successful
    
    SAPDB_Bool m_LastSaveDataSuccessful;

    /// Number of pending i/o orders. If only one pager is responsible for
    /// the section, this value could be only zero or one.

    SAPDB_UInt2 m_PendingIO;

    /// number of updated permanent pages since last successful full backup

    SAPDB_UInt  m_NumUpdatedPermPages;

    /// number of updated and saved permanent pages since last successful
    /// full backup. If a full backup is started the m_NumUpdatedPermPages
    /// will be transformed to m_NumPendingUpdatedPermPages. If the full
    /// database backup fails, the updated page count could be reseted.

    SAPDB_UInt  m_NumPendingUpdatedPermPages;

    /// list to hold pages given free during a savepoint. Members
    /// of this list will be given free after the savepoint

    Converter_FreePageList  m_FreePageNoList;

    /// list of parent references for index cleanup. It is needed to remove
    /// block addresses of removed converter leaf pages from converter index.

    Container_Vector<Converter_ParentReference> m_ParentRefs;

    /// doubly linked chain of changed map entries

    Converter_Chain<Container_Vector<ChangedInfo>, ChangedInfo> m_ChangedChain;

    /// pointer to corresponding member data in map control

    RTESync_InterlockedCounter<SAPDB_UInt> *m_pNumChangedConverterPages;
};

#endif  /* CONVERTER_PERM_MAP_SECTION_DATA_HPP */

