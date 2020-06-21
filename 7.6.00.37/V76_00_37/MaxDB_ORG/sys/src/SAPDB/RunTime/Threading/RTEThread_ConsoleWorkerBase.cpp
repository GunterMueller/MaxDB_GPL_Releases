/*!
  @file           RTEThread_ConsoleWorkerBase.cpp
  @author         StefanP
  @special area   Kernel Console Thread
  @brief          Base Class for the Request Processing
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#if defined (_WIN32)    /*&if $OSSPEC = WIN32*/ 
#include  <stdio.h>
#endif                  /*&endif*/

#include    "RunTime/RTE_KGS.hpp"
#include    "RunTime/RTE_KSS.h"
#include    "RunTime/RTE_CompilerFeatures.h"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/Threading/RTEThread_ConsoleWorkerBase.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_Console_Thread_Messages.hpp"

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

SAPDB_Bool  RTEThread_ConsoleWorkerBase::Initialize 
(
    RTE_UniqueIdBuffer     const           shmID,
    RTE_OSPid   const                      remotePID,
    SAPDB_UInt4 const                      myRef,
    SAPDB_UInt4 const                      remoteRef,
    SAPDBErr_MessageList   &               messageList
)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleWorkerBase::Initialize", 
                             Console_Trace, 1);
    
    m_RemotePID = remotePID; // Save the remote pid

#if defined (_WIN32)
    if (!m_CommPort.Initialize (RTE_KGS::Instance().GetKGS()->szServerDB, 
                                NULL, 
                                shmID, 
                                pkss->consoleShmSize,
                                myRef,
                                remotePID,
                                remoteRef,
                                pkss->consoleSemTimeout,
                                messageList))
#else
    if (!m_CommPort.Initialize (RTE_KGS::Instance().GetKGS()->serverdb, 
                                NULL, 
                                shmID, 
                                RTE_KGS::Instance().GetKGS()->consoleShmSize,
                                myRef,
                                remotePID,
                                remoteRef,
                                RTE_KGS::Instance().GetKGS()->consoleSemTimeout,
                                messageList))
#endif
    {
        return false;
    }

    m_Initialized = true;

    return true;
}

