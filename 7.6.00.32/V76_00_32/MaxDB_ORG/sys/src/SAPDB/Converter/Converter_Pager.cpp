/*!
  @file     Converter_Pager.cpp
  @author   TorstenS
  @ingroup  Converter
  @brief    The class Converter_Pager is used to read converter pages
            from a data volume into the memory which is requested from
            component frame control.
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2002-2004 SAP AG

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

#include "Converter/Converter_Version.hpp"
#include "Converter/Converter_Converter.hpp"
#include "Converter/Converter_Exception.hpp"
#include "Converter/Converter_Messages.hpp"
#include "Converter/Converter_Page.hpp"
#include "Converter/Converter_Pager.hpp"

#include "FreeBlockManagement/FBM_Types.hpp"
#include "FreeBlockManagement/FBM_IManager.hpp"

#include "IOManager/IOMan_BlockAddress.hpp"
#include "IOManager/IOMan_IConverterIO.hpp"

#include "KernelCommon/Kernel_FileIO.hpp"

#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/RTE_Crash.hpp"

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

/*!
    @brief  This method is used to write a given converter page 
            into the rundirectory of the kernel. The name of the 
            external file is build among other things with the 
            block address of the given page.
    @param  page [in] converter page to be dumped
    @return none
*/

static void
DumpConverterPage(
    const IOMan_BlockAddress  &block,
    Converter_Page            &page )
{
    Kernel_FileIO   DumpFile( KERNEL_CONV_FILE, KERNEL_COR_FILE_EXTENSION,
                              SAPDB_ToString( block.GetVolumeNo(), _T_d ),
                              SAPDB_ToString( block.GetBlockNo(), _T_d ));
    DumpFile.Write( page );
    DumpFile.Close();
}

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

Converter_Pager::Converter_Pager(
    const RTETask_ITask        &task,
    const Converter_Version    &snapShotVersion )
        :
        m_Task( task ),
        m_SnapShotVersion( snapShotVersion ),
        m_FrameAllocator( task )
{}

/*---------------------------------------------------------------------------*/

void
Converter_Pager::Read(
    const IOMan_BlockAddress  &block,
    Converter_Page            &page )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_Pager::Read", Converter_Trace, 6 );

    SAPDBERR_ASSERT_STATE( m_Task.ID() == RTETask_ITask::Self()->ID());

    if( ! page.IsAssigned()){
        page.SetFrame( m_FrameAllocator.New());
    }

    IOMan_IConverterIO::GetInstance().ReadConverterPage( m_Task.ID(), block, page );

    SAPDBTRACE_WRITELN( Converter_Trace, 6, "Read block : "
                        << block.GetVolumeNo() << "/" << block.GetBlockNo()
                        << " pageVersion : "   << page.ReadVersion());

    if( FBM_IManager::Instance().SetBlockStateToOccupied( m_Task.ID(), block ))
        return;
    // fatal error occur
    Converter_Exception errMsg( __CONTEXT__, CONVERTER_ERR_SET_TO_OCCUPIED,
                                Converter_PageTypeStrings( page.ReadConverterPageType() ));
    DumpConverterPage( block, page );
    RTE_Crash( errMsg );
}

/*---------------------------------------------------------------------------*/

void
Converter_Pager::Write( Converter_Page   &page )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_Pager::Write", Converter_Trace, 6 );

    SAPDBERR_ASSERT_STATE( m_Task.ID() == RTETask_ITask::Self()->ID());

    const IOMan_BlockAddress  block = page.ReadBlockAddress();

    if( block.IsValid() )
    {
        if(( ! m_SnapShotVersion.IsValid()) || ( page.ReadVersion() > m_SnapShotVersion ))
        {
            FBM_IManager::Instance().SetBlockStateToFreeAfterSVP( m_Task.ID(), block );
        }

        SAPDBTRACE_WRITELN( Converter_Trace, 6, "Write old block : "
                            << block.GetVolumeNo()   << "/" << block.GetBlockNo()
                            << " pageVersion : "     << page.ReadVersion()
                            << " snapshotVersion : " << m_SnapShotVersion );
    }
    else
    {
        SAPDBTRACE_WRITELN( Converter_Trace, 6, "Write new block : "
                            << " pageVersion : "     << page.ReadVersion()
                            << " snapshotVersion : " << m_SnapShotVersion );
    }

    IOMan_IConverterIO::GetInstance().WriteConverterPage( m_Task.ID(), page );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
Converter_SnapShotPager::Read(
    const IOMan_BlockAddress  &block,
    Converter_Page            &page )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter_SnapShotPager::Read", Converter_Trace, 6 );

    SAPDBERR_ASSERT_STATE( m_Task.ID() == RTETask_ITask::Self()->ID());

    if( ! page.IsAssigned()){
        page.SetFrame( m_FrameAllocator.New());
    }

    IOMan_IConverterIO::GetInstance().ReadConverterPage( m_Task.ID(), block, page );

    SAPDBTRACE_WRITELN( Converter_Trace, 6, "Read block : "
                        << block.GetVolumeNo()   << "/" << block.GetBlockNo()
                        << " pageVersion : "     << page.ReadVersion()
                        << " snapshotVersion : " << m_SnapShotVersion );

    SAPDBERR_ASSERT_STATE( m_SnapShotVersion.IsValid());
    SAPDBERR_ASSERT_STATE( page.ReadVersion() <= m_SnapShotVersion );

    // Count all converter pages for the snapshot size

    Converter_Converter::Instance().IncrementSnapShotSize();

    if( Converter_IndexPageType == page.ReadConverterPageType())
    {
        // Do not mark the index pages as used in the FBM, if
        // the page is already part of the current converter

        if( m_Index.IsMember( block )){

            SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: Converter Index Page on block: " <<
                                block.GetVolumeNo() << "/" << block.GetBlockNo() << " not marked as used ");
            return;
        }
        SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: Converter Index Page on block: " <<
                            block.GetVolumeNo() << "/" << block.GetBlockNo() << " marked as used ");
    }
    else
    {
        SAPDBERR_ASSERT_STATE( Converter_LeafPageType == page.ReadConverterPageType());

        Converter_LeafPage  leafPage( page.GetFrame());

        if( Converter_Converter::Instance().IsLeafPageMember( m_Task, leafPage )){

            SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: Converter Leaf Page on block: " <<
                                block.GetVolumeNo() << "/" << block.GetBlockNo() << " not marked as used ");

            return;
        }
        SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: Converter Leaf Page on block: " <<
                            block.GetVolumeNo() << "/" << block.GetBlockNo() << " marked as used ");
    }

    // Mark only those converter pages (blocks) in the FBM, which are
    // changed after the snapshot was created. The unchanged converter
    // pages were set to occupied by the common converter restart/read
    // operation.

    if( FBM_IManager::Instance().SetBlockStateToOccupied( m_Task.ID(), block ))
    {
        Converter_Converter::Instance().IncrementNumUsedPermPages();
        return;
    }
    // fatal error occur
    Converter_Exception errMsg( __CONTEXT__, CONVERTER_ERR_SET_TO_OCCUPIED,
                                Converter_PageTypeStrings( page.ReadConverterPageType() ));
    DumpConverterPage( block, page );
    RTE_Crash( errMsg );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
