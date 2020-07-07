/*!***************************************************************************

  module      : RTE_KGS.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Kernel
  description : RTE Kernel Global Section singleton


  last changed: 2001-04-24  14:43
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/RTE_ISystem.hpp"
#include "RunTime/RTE_MySQLProxy.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_PageAllocator.hpp"
#include "RunTime/System/RTESys_SystemInfo.h"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/System/RTESys_MicroTime.h"
#include "geo002.h"
#ifndef WIN32
#include "RunTime/RTE_UNIXAuthenticate.hpp" /* nocheck */
#include "RunTime/System/RTESys_IO.h" /* nocheck */
#endif
#include "SAPDBCommon/Messages/SDBMsg_RunTime.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/
RTE_KGS * RTE_KGS::m_Instance = (RTE_KGS *)0;

/*---------------------------------------------------------------------------*/

RTE_KGS & RTE_KGS::Instance()
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTE_KGS));
  
  if ( !RTE_KGS::m_Instance )
  {
    RTE_KGS::m_Instance = new (Space) RTE_KGS();
  }
  return *m_Instance;
}

/*---------------------------------------------------------------------------*/

RTE_KGS::RTE_KGS()
 :m_RTE_ISystem(RTE_ISystem::Instance())
 ,m_RTEMem_Allocator(RTEMem_Allocator::Instance())
 ,m_RTEMem_PageAllocator(0)
{
    RTE_MySQLCrypting::Instance(); // force initialize of non thread save Instance() method...
}

/*---------------------------------------------------------------------------*/

void RTE_KGS::InitStatistic()
{
    // initialize own local node name
    SAPDB_MemFillNoCheck(m_Statistic.localNodeName, 0, sizeof(RTE_Nodename));
    SAPDB_Char const * ownNodeName = RTE_ISystem::Instance().GetLocalNodeName();
    if ( strlen(ownNodeName) >= sizeof(RTE_Nodename) )
    {
        SAPDBErr_MessageList errList( RTE_CONTEXT,
                                      RTEERR_LOCAL_NODE_TOO_LONG, // ownNodeName too long!!!
                                      ownNodeName );
        RTE_Crash(errList);
    }
    else
    {
        strcpy(m_Statistic.localNodeName, ownNodeName);
    }

    if ( *(m_KGS->XParam->szOfficialNode) )
    {
        strcpy(&m_Statistic.officialNodeName[0], m_KGS->XParam->szOfficialNode);
        // initialize official node name
        SAPDB_MemFillNoCheck(m_Statistic.officialNodeName, 0, sizeof(RTE_Nodename));
    }
    else
    {
        SAPDB_MemCopyNoCheck(m_Statistic.officialNodeName, m_Statistic.localNodeName, sizeof(RTE_Nodename));
    }

    m_LastTaskGroupStatisticsReset = 0;
    m_LastIOThreadStatisticsReset = 0;
    m_Statistic.timeOfAdminState = 0;
    m_Statistic.timeOfOnlineState = 0;
}

/*---------------------------------------------------------------------------*/

void RTE_KGS::NotifyLastIOThreadStatisticsReset()
{
    m_LastIOThreadStatisticsReset = RTESys_MicroSecondTime();
}

/*---------------------------------------------------------------------------*/

void RTE_KGS::NotifyLastTaskGroupStatisticsReset()
{
    m_LastTaskGroupStatisticsReset = RTESys_MicroSecondTime();
}

/*---------------------------------------------------------------------------*/

void RTE_KGS::SetDBState(DbState newState)
{
    DbState oldState;

#if defined(_WIN32)

    m_Statistic.processId = (SAPDB_UInt8)kgs.KernelPid;
    oldState = (RTE_KGS::DbState)*kgs.pDBState;
    *kgs.pDBState = (SAPDB_UInt4)newState;
    if ( kgs.fRunningAsNTService )
    {
        sql80k_NewSrvState(newState);
    }
    else
    {
        sql80kn_set_db_state(newState);
    }

#else

    m_Statistic.processId = (SAPDB_UInt8)m_KGS->pid;
    oldState = (RTE_KGS::DbState)m_KGS->state;
    m_KGS->state = (SAPDB_UInt4)newState;

#endif

    if ( oldState != newState )
    {
        RTE_Message( Msg_List( Msg_List::Info
                             , SDBMSG_RUNTIME_STATE_CHANGED
                             , Msg_Arg(SDBMSGTAG_RUNTIME_STATE_CHANGED__OLD_STATE, SAPDB_ToString(oldState))
                             , Msg_Arg(SDBMSGTAG_RUNTIME_STATE_CHANGED__STATE, SAPDB_ToString(newState)) ) );
        if ( RTEKGS_StateAdmin == newState )
        {
            m_LastTaskGroupStatisticsReset =
            m_LastIOThreadStatisticsReset = 
            m_Statistic.timeOfAdminState = RTESys_MicroSecondTime();
        } else if ( RTEKGS_StateOnline == newState )
        {
            m_Statistic.timeOfOnlineState = RTESys_MicroSecondTime();

            SAPDB_ULong SharedSizeInMBytes;
            SAPDB_ULong MemoryInstalledInMBytes;
            SAPDB_ULong MemoryLeftInMBytes;
            SAPDB_ULong MemoryLimitInBytes;
            SAPDB_ULong MemoryCommitedInBytes;
            SAPDB_ULong MemoryUsedInBytes;
            SAPDB_ULong MemoryMaxUsedInBytes;
            SAPDB_UInt8 MemorySuccessfullAllocs;
            SAPDB_UInt8 MemoryFailedAllocs;
            SAPDB_UInt8 MemoryCommits;
            SAPDB_UInt8 MemoryFrees;

            RTE_ISystem::Instance().GetMemoryStatistics( 
                                    MemoryLimitInBytes,
                                    MemoryCommitedInBytes,
                                    MemoryUsedInBytes,
                                    MemoryMaxUsedInBytes,
                                    MemorySuccessfullAllocs,
                                    MemoryFailedAllocs,
                                    MemoryCommits,
                                    MemoryFrees);

            SharedSizeInMBytes = (MemoryMaxUsedInBytes + ((1024*1024)-1)) / (1024*1024);
            RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                          RTEINFO_PHYSICAL_MEMORY_USED,
                                          SAPDB_ToString(SharedSizeInMBytes) ) );

            MemoryInstalledInMBytes = RTESys_PhysicalMemorySize();
            if ( MemoryInstalledInMBytes < SharedSizeInMBytes )
            {
                RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                              RTEWARN_PHYSICAL_MEMORY_EXCEEDED,
                                              SAPDB_ToString(MemoryInstalledInMBytes) ) );
            }

#define SAPDB_MINIMUM_AVAILIABLE_WARNING_MBYTES 8
            MemoryLeftInMBytes = RTESys_PhysicalMemoryAvailable();
            if ( MemoryLeftInMBytes <= SAPDB_MINIMUM_AVAILIABLE_WARNING_MBYTES )
            {
                RTE_Message( SAPDBErr_MessageList( RTE_CONTEXT,
                                              RTEWARN_VERY_LITTLE_PHYSICAL_MEMORY_LEFT,
                                              SAPDB_ToString(MemoryLeftInMBytes) ) );
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Char const * RTE_KGS::DBName() const
{
#if defined(WIN32)
    return RTE_KGS::Instance().GetKGS()->szServerDB;
#else
    return RTE_KGS::Instance().GetKGS()->serverdb;
#endif
}

/*---------------------------------------------------------------------------*/

RTE_KGS::DbState RTE_KGS::DBState() const
{
#if defined(_WIN32)
    return (RTE_KGS::DbState)*kgs.pDBState;
#else
    return (RTE_KGS::DbState)m_KGS->state;
#endif
}


#ifndef WIN32

/*---------------------------------------------------------------------------*/

static int my_save_chmod(SAPDB_Char const *path, SAPDB_Int4 newMode)
{
   int result;
   do {
        result = chmod(path, newMode);
   } while ( -1 == result && errno == EINTR );
   return result;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
RTE_SetDatabasePathOwnership( SAPDB_Char const *volume,
                              SAPDBErr_MessageList &errList )
{
    SAPDB_Int4 userId;
    SAPDB_Int4 groupId;
#if defined(WIN32)
    SAPDB_Char const *szServerDB = RTE_KGS::Instance().GetKGS()->szServerDB;
#else
    SAPDB_Char const *szServerDB = RTE_KGS::Instance().GetKGS()->serverdb;
#endif

    if ( !RTE_GetOwnerAndSupportGroupIdFromDatabaseName( szServerDB
                                                         ,userId
                                                         ,groupId
                                                         ,errList )
      || !RTE_SetFileOwnerAndGroup( volume
                                   ,userId
                                   ,groupId
                                   ,errList ) )
    {
        return false;
    }

    if ( my_save_chmod( volume, 0660 ) != 0 )
    {
        RTE_SystemRc rc;
        RTESys_ErrorMessage spaceToFill;
        rc = RTESys_GetLastError();

        if ( RTE_SYSTEMRC_FILE_NOT_FOUND != rc )
        {
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEERR_CHMOD_FAILED,
                                            volume,
                                            "0660",
                                            RTESys_StrError(rc, spaceToFill) );
            return false;
        }
    }
    return true;
}

/*---------------------------------------------------------------------------*/

extern "C" SAPDB_Bool
RTE_CSetDatabasePathOwnership( SAPDB_Char const *volume )
{ 
    SAPDBErr_MessageList errList;

    if ( !RTE_SetDatabasePathOwnership( volume
                                       ,errList ) )
    {
        RTE_Message (errList);
        return false;
    }

    return true;
}
#endif

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
