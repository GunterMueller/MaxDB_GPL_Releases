/*!
  @file     FrameCtrl_Manager.cpp
  @author   TorstenS
  @ingroup  FrameControl
  @brief    This class is used to handle i/o capable memory request

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

\endif
*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hbd20_3.h"

#include "FrameControl/FrameCtrl_Manager.hpp"
#include "FrameControl/FrameCtrl_Messages.hpp"
#include "FrameControl/FrameCtrl_Exception.hpp"
#include "KernelCommon/ParameterNames/KernelParam_FrameControl.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_PageAllocator.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"

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

/* -----------------------------------------------------------------------*/
/*!
    @brief  This method is used to get the configured value for a given 
            integer configuration parameter from the configuration file.
    @return (SAPDB_UInt) configuration value
 */
/* -----------------------------------------------------------------------*/

static SAPDB_UInt
GetConfiguredValue( const RTEConf_Parameter::Name paramName )
{
    RTEConf_Parameter::Integer  paramValue;
    SAPDBErr_MessageList        errMsg;

    if( ! RTEConf_ParameterAccess::Instance()->GetInteger(
                paramName, paramValue, errMsg ))
    {
        RTE_Crash( errMsg );
    }
    return( ( SAPDB_UInt ) paramValue );
}

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

FrameCtrl_Manager*  FrameCtrl_Manager::m_Instance = NULL;

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

FrameCtrl_Manager::FrameCtrl_Manager()
        :
        m_Capacity( 0 ),
        m_PageSize( GetConfiguredValue( UTF8( KERNELPARAM_PAGE_SIZE ))),
        m_UsedByConverter( 0 ),
        m_UsedByDataCache( 0 ),
        m_UsedByLogQueue( 0 ),
        m_UsedByIOManagement( 0 ),
        m_UsedByFileDirectory( 0 ),
        m_UsedByRestartRecord( 0 ),
        m_UsedByBlockAllocator( 0 ),
        m_UsedByCreateIndex( 0 )
{};

/*---------------------------------------------------------------------------*/

bool
FrameCtrl_Manager::CreateInstance()
{
    SAPDBTRACE_ROUTINE_DEBUG( "FrameCtrl_Manager::CreateInstance", FrameCtrl_Trace, 5 );

    SAPDBERR_ASSERT_STATE( NULL == m_Instance );

    if ( NULL != m_Instance )
        return( true );

    m_Instance = new( RTEMem_Allocator::Instance() ) FrameCtrl_Manager();

    if( NULL == m_Instance)
        return( false );

    m_Instance->Initialize();

    return( true );
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt
FrameCtrl_Manager::GetFreeFrameCount() const
{
    return( SAPDB_UInt ) RTEMem_PageAllocator::Instance().GetFreePages();
}

/*---------------------------------------------------------------------------*/

void
FrameCtrl_Manager::New(
    const FrameCtrl_RegistrationKey   registrationKey,
    const tsp00_TaskId                taskId,
    const SAPDB_Bool                  bStopIfMemoryExhausted,
    Data_PageFrame&                   frame )
{
    SAPDBTRACE_ROUTINE_DEBUG( "FrameCtrl_Manager::New", FrameCtrl_Trace, 5 );


#ifdef H01SAVEDEBUG
    Kernel_VTrace() << "> FrameCtrl_Manager::New - Free pages in RTE page allocator: "
        << this->GetFreeFrameCount() << FlushLine;
#endif
    void *pFrame = RTEMem_PageAllocator::Instance().Allocate( m_PageSize );

    if( NULL != pFrame )
    {
        SAPDBTRACE_WRITELN( FrameCtrl_Trace, 5,
                            "Provide Memory " << SAPDB_ToString( pFrame, _T_x )
                            << " From Page Pool" );
    }
    else if( FrameCtrl_DataCacheKey != registrationKey )
    {
        pFrame = bd20ShrinkDataCache( taskId );

        if( NULL != pFrame )
        {
            --m_UsedByDataCache;

            SAPDBTRACE_WRITELN( FrameCtrl_Trace, 5,
                                "Provide Memory " << SAPDB_ToString( pFrame, _T_x )
                                << " From Data Cache" );
        }
    }
    if( NULL == pFrame )
    {
        if( ! bStopIfMemoryExhausted )
        {
            frame.Deassign();
            return;
        }
        FrameCtrl_Exception errMsg( __CONTEXT__, FRAME_CTRL_NO_MORE_FRAMES );
        RTE_Crash( errMsg );
    }
    MaintainInternalCounter( registrationKey );
    frame.Assign( pFrame, m_PageSize );
}

/*---------------------------------------------------------------------------*/

void
FrameCtrl_Manager::Free(
    const FrameCtrl_RegistrationKey registrationKey,
    const tsp00_TaskId              taskId,
    const SAPDB_Bool                bIsShutdown,
    Data_PageFrame                  &frame )
{
    SAPDBTRACE_ROUTINE_DEBUG( "FrameCtrl_Manager::Free", FrameCtrl_Trace, 5 );

    SAPDBERR_ASSERT_STATE( frame.GetLength() == m_PageSize );
    SAPDBERR_ASSERT_STATE( NULL != frame.GetPointer( 0, m_PageSize ));

    SAPDB_Bool  bSuccessful = false;

    if(( FrameCtrl_DataCacheKey != registrationKey ) && ( ! bIsShutdown ))
    {
        tbd_nodeptr pFrame = (tbd_nodeptr) frame.GetPointer( 0, m_PageSize );

        if( bd20GrowDataCache( taskId, pFrame ))
        {
            bSuccessful = true;
            ++m_UsedByDataCache;
        }
    }
    if( ! bSuccessful ){
        RTEMem_PageAllocator::Instance().Deallocate( frame.GetPointer( 0, m_PageSize ));
    }
    frame.Deassign();
    MaintainInternalCounter( registrationKey, false );
}

/*---------------------------------------------------------------------------*/

void*
FrameCtrl_Manager::Allocate( SAPDB_ULong BlockCount )
{
    if (1 != BlockCount)
    {
        return 0;
    }
    const SAPDB_Bool bStopIfMemoryExhausted = true;
    Data_PageFrame frame = NewBlockAllocatorFrame( RTETask_ITask::Self()->ID(),
                           ! bStopIfMemoryExhausted );
    return frame.GetDataPtr();
}

/*---------------------------------------------------------------------------*/

void
FrameCtrl_Manager::Deallocate(
    void*       pFrame, 
    SAPDB_ULong BlockCount )
{
    const SAPDB_Bool    bIsShutdown = true;
    Data_PageFrame      frame ( pFrame, BlockCount * m_PageSize );
    FreeBlockAllocatorFrame ( RTETask_ITask::Self()->ID(), frame, ! bIsShutdown );
}

/*===========================================================================*
 *  PRIVATE METHODS                                                          *
 *===========================================================================*/

void
FrameCtrl_Manager::MaintainInternalCounter(
    const FrameCtrl_RegistrationKey registrationKey,
    const SAPDB_Bool                bIncrementCounter )
{
    // Note that the counters are modified dirty

    switch( registrationKey )
    {
    case FrameCtrl_ConverterKey:
        if( bIncrementCounter )
            ++m_UsedByConverter;
        else
            --m_UsedByConverter;
        break;
    case FrameCtrl_DataCacheKey:
        if( bIncrementCounter )
            ++m_UsedByDataCache;
        else
            --m_UsedByDataCache;
        break;
    case FrameCtrl_LogKey:
        if( bIncrementCounter )
            ++m_UsedByLogQueue;
        else
            --m_UsedByLogQueue;
        break;
    case FrameCtrl_IOManagementKey:
        if( bIncrementCounter )
            ++m_UsedByIOManagement;
        else
            --m_UsedByIOManagement;
        break;
    case FrameCtrl_FileDirectoryKey:
        if( bIncrementCounter )
            ++m_UsedByFileDirectory;
        else
            --m_UsedByFileDirectory;
        break;
    case FrameCtrl_RestartPageKey:
        if( bIncrementCounter )
            ++m_UsedByRestartRecord;
        else
            --m_UsedByRestartRecord;
         break;
    case FrameCtrl_BlockAllocatorKey:
        if( bIncrementCounter )
            ++m_UsedByBlockAllocator;
        else
            --m_UsedByBlockAllocator;
         break;
    case FrameCtrl_CreateIndexKey:
        if( bIncrementCounter )
            ++m_UsedByCreateIndex;
        else
            --m_UsedByCreateIndex;
         break;
    default:;
    };

    SAPDBTRACE_WRITELN( FrameCtrl_Trace, 5, "Statistic: " << NewLine <<
                        "Total Frames: "         << m_Capacity <<
                        " UsedByConverter:      " << m_UsedByConverter      <<
                        " UsedByDataCache:      " << m_UsedByDataCache      <<
                        " UsedByLogQueue:       " << m_UsedByLogQueue       <<
                        " UsedByIOManagement:   " << m_UsedByIOManagement   <<
                        " UsedByFDir:           " << m_UsedByFileDirectory  <<
                        " UsedByRestart:        " << m_UsedByRestartRecord  <<
                        " UsedByBlockAllocator: " << m_UsedByBlockAllocator <<
                        " UsedByCreateIndex:    " << m_UsedByCreateIndex    <<
                        " Available:            " << GetFreeFrameCount());
}

/*---------------------------------------------------------------------------*/
/*-------------- Implementation of Interface Instance methods ---------------*/
/*---------------------------------------------------------------------------*/

FrameCtrl_ICommon&
FrameCtrl_ICommon::GetInstance()
{
    return( FrameCtrl_Manager::GetInstance());
}

/*---------------------------------------------------------------------------*/

FrameCtrl_IConverter&
FrameCtrl_IConverter::GetInstance()
{
    return( FrameCtrl_Manager::GetInstance());
}

/*---------------------------------------------------------------------------*/

FrameCtrl_IDataCache&
FrameCtrl_IDataCache::GetInstance()
{
    return( FrameCtrl_Manager::GetInstance());
}

/*---------------------------------------------------------------------------*/

FrameCtrl_IFileDirectory&
FrameCtrl_IFileDirectory::GetInstance()
{
    return( FrameCtrl_Manager::GetInstance());
}

/*---------------------------------------------------------------------------*/

FrameCtrl_IIOManager&
FrameCtrl_IIOManager::GetInstance()
{
    return( FrameCtrl_Manager::GetInstance());
}

/*---------------------------------------------------------------------------*/

FrameCtrl_ILog&
FrameCtrl_ILog::GetInstance()
{
    return( FrameCtrl_Manager::GetInstance());
}

/*---------------------------------------------------------------------------*/

FrameCtrl_IRestartPage&
FrameCtrl_IRestartPage::GetInstance()
{
    return( FrameCtrl_Manager::GetInstance());
}

/*---------------------------------------------------------------------------*/

FrameCtrl_IBlockAllocator&
FrameCtrl_IBlockAllocator::GetInstance()
{
    return( FrameCtrl_Manager::GetInstance());
}

/*---------------------------------------------------------------------------*/

FrameCtrl_ICreateIndex&
FrameCtrl_ICreateIndex::GetInstance()
{
    return( FrameCtrl_Manager::GetInstance());
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
