/*!
  @file		Converter_PermStaticMapControl.cpp
  @author   TorstenS
  @ingroup  Converter
  @brief    Manages the concurrent access to the structure mapping
            permanent static pageNo's to their physical block address
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


#include "Converter/Converter_PermStaticMapControl.hpp"
#include "Converter/Converter_Exception.hpp"
#include "Converter/Converter_MapError.hpp"
#include "Converter/Converter_Messages.hpp"
#include "Converter/Converter_Map.hpp"
#include "Converter/Converter_PermMapSection.hpp"
#include "Converter/Converter_Pager.hpp"

#include "DataAccess/Data_Types.hpp"

#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
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
Converter_PermStaticMapControl::RequestSpecifiedNewPageNo(
    const RTETask_ITask                             &task,
    const Data_PageNo                               pageNo,
    Kernel_SynchronizedTaskWaitList::WaitContext    &waitContext,
    Kernel_SynchronizedTaskWaitList                 &freePageNoWaitQueue,          // PTS 1116298 UH 2002-06-21
    SAPDB_Bool                                      &bWaitUntilSavepointFinished ) // PTS 1116298 UH 2002-06-21
{
    SAPDBTRACE_ROUTINE_DEBUG( "StaticMapControl::RequestSpecifiedNewPageNo", Converter_Trace, 5 );

    const SAPDB_UInt sectionNo  = CalculateSectionNo( pageNo );
    SAPDB_Bool       bFound     = SAPDB_FALSE;
    bWaitUntilSavepointFinished = SAPDB_FALSE; // 1128496 TS 2004-03-15


    Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);

    do
    {
        const Converter_Map::RequestSpecifiedNewPageNoRc retCode =
            m_Map.RequestSpecifiedNewPageNo( section, pageNo );

        switch( retCode )
        {
        case Converter_Map::rsnpRcOkay:
            {
                bFound = SAPDB_TRUE;
            }
            break;
        case Converter_Map::rsnpRcPendingIo:
            {
                m_RequestSpecifiedNewPageNoWaitQueue.InsertTask( task.ID(), waitContext );

                section.Unlock(); // leave region for suspend

                m_RequestSpecifiedNewPageNoWaitQueue.Wait( task.ID());

                section.Lock();  // re-entrance of region after suspend
            }
            break;
        case Converter_Map::rsnpRcPageNoInUse:
            {
                // PTS 1116298 UH 2002-06-21
                if ( section.IsSavepointActive() ) // PTS 1116298 UH 2002-06-21 new block
                {
                    SAPDBTRACE_WRITELN( Converter_Trace, 5, "Suspend Task " << task.ID() <<
                                        " because savepoint is active" );

                    freePageNoWaitQueue.InsertTask( task.ID(), waitContext );
                    bWaitUntilSavepointFinished = SAPDB_TRUE;
                    return( SAPDB_TRUE );
                }
                else
                {
                    Converter_Exception pageAlreadyInUse (__CONTEXT__,  CONVERTER_PAGE_IN_USE,
                                                          SAPDB_ToString( pageNo, _T_d ));

                    RTE_Message( pageAlreadyInUse );
                    return( SAPDB_FALSE );
                }
                // PTS 1116298 UH 2002-06-21
            }
        case Converter_Map::rsnpRcFrameAllocationFailed: // crash
        default:
            {
                Converter_MapError mapError(__CONTEXT__, GetRecoveryMode(), pageNo,
                                            (Converter_ReturnCode) retCode);
                RTE_Crash( mapError );
            }
            break;
        }
    }
    while( ! bFound );

    return( bFound ); // bFound needed because of AIX and DEC return value handling
}

/*---------------------------------------------------------------------------*/

void
Converter_PermStaticMapControl::WriteChangedConverterPage(
    const RTETask_ITask      &task,
    Converter_PermMapSection &section,
    Converter_LeafPage       &leafPage,
    Converter_Pager          &pager,
    SAPDB_UInt               &totalPages,
    SAPDB_UInt               &totalIO )
{
    // Attention: This is a virtual method! The corresponding method from
    // PermMapControl::WriteChangedConverterPage is a little bit easier, because
    // no m_RequestSpecifiedNewPageNoWaitQueue handling is needed.

    SAPDBTRACE_ROUTINE_DEBUG( "StaticMapControl::WriteChangedConverterPage", Converter_Trace, 5 );

    section.Unlock(); // release region during i/o

    pager.Write( leafPage );

    totalPages++;
    totalIO++;

    section.Lock(); // lock region again after i/o

    if( ! m_RequestSpecifiedNewPageNoWaitQueue.IsEmpty() )
        m_RequestSpecifiedNewPageNoWaitQueue.ResumeAllWaitingTasks( task.ID());
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
