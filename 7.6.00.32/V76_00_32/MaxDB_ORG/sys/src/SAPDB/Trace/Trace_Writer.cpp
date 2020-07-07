/*!**************************************************************************

  module      : Trace_Writer.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area:
  description : 

  last changed: 2000-11-27  18:15
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hkb38.h"      // PASCAL: k38dump
#include "hkb52_1.h"    // PASCAL: k52dump
#include "hkb57.h"      // PASCAL: k57dump

#include "hbd90.h"      // PASCAL: b90dump
#include "hbd91.h"      // PASCAL: b91dump
#include "hbd20_7.h"    // PASCAL: b20dump

#include "hgg01.h"      // PASCAL: g01dump/g01region
#include "hgg02.h"      // PASCAL: g02dump
#include "hgg08.h"      // PASCAL: g08surrogate/...

#include "heo51.h"      // RTE   : vfinish
#include "heo56.h"      // RTE   : Vsuspend/Vresume
#include "heo58.h"      // RTE   : vIsDumpWanted/vIsTraceWanted

#include "heo55k.h"     // vbegexcl

#include "Converter/Converter_IDump.hpp"

#include "Pager/Pager_IDump.hpp"

#include "FileDirectory/FileDir_ISharedDirectory.hpp"

#include "FrameControl/FrameCtrl_ICommon.hpp"

#include "FreeBlockManagement/FBM_IManager.hpp"

#include "KernelCommon/Kernel_Dump.hpp"

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_ISystem.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Tasking/RTETask_SuspendReason.h"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

#include "Trace/Trace_Manager.hpp"
#include "Trace/Trace_Writer.hpp"
#include "Trace/Trace_Messages.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define TRACE_ENTER_REGIONS   SAPDB_TRUE

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

Trace_Writer*   Trace_Writer::m_Instance = NULL;


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Singleton interface                                                       */
/*---------------------------------------------------------------------------*/

bool
Trace_Writer::CreateInstance( const RTETask_ITask &task )
{
    if ( NULL == m_Instance )
    {
        m_Instance = new( RTEMem_Allocator::Instance() ) Trace_Writer( task );
    }
    return( NULL != m_Instance );
}

/*---------------------------------------------------------------------------*/

Trace_Writer::Trace_Writer( const RTETask_ITask &task ) : m_Task( task )
{
    const SAPDB_UInt pageSize = FrameCtrl_ICommon::GetInstance().GetPageSize();
    m_Frame.Assign( RTE_ISystem::Instance().AllocSystemPages( pageSize ), pageSize );
}

/*---------------------------------------------------------------------------*/

void
Trace_Writer::UserForcedDump(
    const RTETask_ITask    &task,
    const tsp00_VFilename  &FileName,
    tgg00_DumpStateSet     &DumpStateSet )
{
    const SAPDB_Int normalRegion   = 1;
    const SAPDB_Int autosaveRegion = 2;
    const SAPDB_Int maxDataRegions = g01region_cnt( rgnData_egg00 );

    SAPDB_Int   partition;

    vbegexcl( task.ID(), g08save0 + normalRegion );
    vbegexcl( task.ID(), g08save0 + autosaveRegion );
    vbegexcl( task.ID(), g08surrogate );

    for( partition = 0; partition < maxDataRegions; partition++ )
        vbegexcl( task.ID(), g08data1 + partition );

    {
        Kernel_Dump DumpFile( FileName, m_Frame );

        Dump( task, DumpFile, DumpStateSet, TRACE_ENTER_REGIONS );
    }

    for( partition = 0; partition < maxDataRegions; partition++ )
        vendexcl (task.ID(), g08data1 + partition);

    vendexcl( task.ID(), g08surrogate );
    vendexcl( task.ID(), g08save0 + autosaveRegion );
    vendexcl( task.ID(), g08save0 + normalRegion );
}

/*---------------------------------------------------------------------------*/

void
Trace_Writer::SystemForcedDump()
{
    tgg00_DumpStateSet  DumpStateSet;
    tsp00_VFilename     FileName = Trace_Manager::GetInstance().GetDumpFile();
    Kernel_Dump         DumpFile( FileName, m_Frame );

    DumpStateSet.clear();
    DumpStateSet.addElement( dumpAll_egg00 );

    Dump( m_Task, DumpFile, DumpStateSet, ! TRACE_ENTER_REGIONS );
}

/*---------------------------------------------------------------------------*/

void
Trace_Writer::Run()
{
    Trace_Manager::GetInstance().TraceWriterRegistration( m_Task.ID());

    while( SAPDB_TRUE )
    {
        Suspend();

        // In case of vabort is called the trace writer starts
        // to flush the complete trace and the dump
        if ( SAPDB_TRUE == vIsTraceWanted() )
        {
            {
                Trace_Exception startMsg( __CONTEXT__, TRACE_START_FLUSH_VTRACE );
                RTE_Message( startMsg );
                startMsg.ClearMessageList();

                Trace_Manager::GetInstance().Flush( m_Task.ID());

                Trace_Exception stopMsg( __CONTEXT__, TRACE_STOP_FLUSH_VTRACE );
                RTE_Message( stopMsg );
            }

            if( SAPDB_TRUE == vIsDumpWanted())

                SystemForcedDump();
            break;
        }
        else
            Trace_Manager::GetInstance().Flush( m_Task.ID());
    }
    vfinish();
}

/*---------------------------------------------------------------------------*/

void
Trace_Writer::Resume()
{
    vresume( m_Task.ID());
}

/*---------------------------------------------------------------------------*/

void
Trace_Writer::Suspend()
{
    vsuspend( m_Task.ID(), TSK_VSUSPEND_REASON_NO_WORK);
}

/*---------------------------------------------------------------------------*/

void
Trace_Writer::Dump(
    const RTETask_ITask  &task,
    Kernel_Dump          &Dump,
    tgg00_DumpStateSet   &DumpStateSet,
    tsp00_Bool           bEnterRegions )
{
    {
        Trace_Exception startMsg( __CONTEXT__, TRACE_START_FLUSH_DUMP );
        RTE_Message( startMsg );
    }

    if(
        ( DumpStateSet.includes( dumpConverter_egg00 )) ||
        ( DumpStateSet.includes( dumpAll_egg00       ))
    )
        Converter_IDump::Instance().Dump( task, Dump, bEnterRegions );

    if(
        ( DumpStateSet.includes( dumpFbm_egg00 )) ||
        ( DumpStateSet.includes( dumpAll_egg00 ))
    )
        FBM_IManager::Instance().Dump( task.ID(), Dump );

    if( ( DumpStateSet.includes( dumpPagerWriter_egg00 )) ||
            ( DumpStateSet.includes( dumpAll_egg00 ))) {
        Pager_IDump::GetInstance().Dump( task.ID(), Dump );
    }

    if( ( DumpStateSet.includes( dumpFileDir_egg00 )) ||
            ( DumpStateSet.includes( dumpAll_egg00 ))) {
        FileDir_ISharedDirectory::Instance().Dump( task.ID(), Dump );
    }

    // ====================> ADD NEW C++ DUMP METHODS HERE!!!! <=============================
    //if( ( DumpStateSet.includes( dumpAll ))
    //    myDump();
    // ====================> ADD NEW C++ DUMP METHODS HERE!!!! <=============================

    Dump.Flush();

    // ===========================
    // support old pascal dump
    // ===========================
    if( !Dump.ErrorOccured() )
    {
        tsp00_PageAddr  pFrame = reinterpret_cast <tsp00_PageAddr>(m_Frame.GetDataPtr());
        tsp00_VfReturn  dumpError;
        tgg00_VfFileref dumpFile;
        tsp00_ErrText   errorText;
        SAPDB_Int4      dumpPageNo  = Dump.GetPageNo();
        SAPDB_Int4      dumpPagePos = 1;

        dumpError.becomes( vf_ok );
        dumpFile.no      = Dump.GetFileNo();
        dumpFile.buf_cnt = 0;

        // Dump backup and recovery manager
        if(
            ( vf_ok == dumpError                       ) &&
            (( DumpStateSet.includes( dumpBackup_egg00)) ||
             ( DumpStateSet.includes( dumpAll_egg00  )))
        )
        {
            k38dump_headmaster( dumpFile, *pFrame, dumpPageNo, dumpPagePos,
                                dumpError, errorText);
        }
        // Dump SQL lock manager
        if(
            ( vf_ok == dumpError                           ) &&
            (( DumpStateSet.includes( dumpKbLocklist_egg00)) ||
             ( DumpStateSet.includes( dumpAll_egg00      )))
        )
        {
            k52dump_locklist( task.ID(), bEnterRegions, dumpFile, *pFrame,
                              dumpPageNo, dumpPagePos, dumpError, errorText);
        }
        // Dump restart record
        if(
            ( vf_ok == dumpError                           ) &&
            (( DumpStateSet.includes( dumpRestartRec_egg00)) ||
             ( DumpStateSet.includes( dumpAll_egg00      )))
        )
        {
            k57dump_restartrec( dumpFile, *pFrame, dumpPageNo, dumpPagePos,
                                dumpError, errorText);
        }
        // Dump common kernel globals
        if(
            ( vf_ok == dumpError                              ) &&
            (( DumpStateSet.includes( dumpConfiguration_egg00)) ||
             ( DumpStateSet.includes( dumpAll_egg00         )))
        )
        {
            g01dump( dumpFile, *pFrame, dumpPageNo, dumpPagePos,
                     dumpError, errorText);
        }
        // Dump kernel code pages
        if(
            ( vf_ok == dumpError                               ) &&
            (( DumpStateSet.includes( dumpTransformation_egg00)) ||
             ( DumpStateSet.includes( dumpAll_egg00          )))
        )
        {
            g02dump( dumpFile, *pFrame, dumpPageNo, dumpPagePos,
                     dumpError, errorText);
        }

        // Dump garbage collector
        if(
            ( vf_ok == dumpError                         ) &&
            (( DumpStateSet.includes( dumpGarbcoll_egg00)) ||
             ( DumpStateSet.includes( dumpAll_egg00    )))
        )
        {
            bd91DumpGarbColl( dumpFile, m_Frame.GetLength(), *pFrame,
                              dumpPageNo, dumpPagePos, dumpError, errorText);
        }
        // Dump data cache control blocks
        if(
            ( vf_ok == dumpError                          ) &&
            (( DumpStateSet.includes( dumpDataCache_egg00)) ||
             ( DumpStateSet.includes( dumpAll_egg00     )))
        )
        {
            bd20DumpDataCache( dumpFile, *pFrame, dumpPageNo, dumpPagePos,
                               dumpError, errorText);
        }
        // Dump data cache frames
        if(( vf_ok == dumpError ) && ( DumpStateSet.includes( dumpAll_egg00 )))
        {
            bd20DumpDataCacheFrames( dumpFile, *pFrame, dumpPageNo, dumpPagePos,
                                     dumpError, errorText);
        }
    }
    {
        Trace_Exception stopMsg( __CONTEXT__, TRACE_STOP_FLUSH_DUMP );
        RTE_Message( stopMsg );
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
