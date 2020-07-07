/*!========================================================================

  @file         RTECKC_TestKernelXFer.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        This paragraph appears in overviews.

                The rest of the description appears as detailed description

  @since        2003-11-06  16:36
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/Tasking/RTETask_ITask.hpp"
#include    "RunTime/Tasking/RTETask_ITaskClientConnection.hpp"
#include    "RunTime/Tasking/RTETask_Task.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/UDEServer/RTEUDE_ServerControl.hpp"

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


void UDEServerTransferTest()
{
    RTETask_ITaskClientConnection::ConnRet  ret;
    SAPDB_Byte*                             pData;
    SAPDB_UInt4                             dataLen;
    SAPDBErr_MessageList                    messageList;
    SAPDB_UInt4                             idx = 0;
    SAPDB_Char                              replyData[] = "Hallo hier is packet ";


    RTEUDE_ServerControl UDEServerControl;
    RTETask_ITask&       task = *RTETask_ITask::Self();

    if ( UDEServerControl.Start(task, (SAPDB_Char *)"ckc_client.exe", messageList ) != RTEUDE_ServerControl::NoError )
    {
        return;
    }
    
    RTETask_ITaskClientConnection& connection = *UDEServerControl.AcceptClientConnect( task );
    SAPDB_UInt4 maxSendDataLength             = connection.MaxSendDataLength();

    for (;;)
    {
        ret = connection.Receive( 0, pData, dataLen, messageList );

        if ( RTETask_ITaskClientConnection::NoError == ret )
        {
            strcpy (reinterpret_cast <SAPDB_Char *> (pData), replyData);
            sprintf (reinterpret_cast <SAPDB_Char *> (pData) + 
                     strlen (reinterpret_cast <SAPDB_Char *> (pData)), " %d, %p", idx++, pData);
            dataLen = strlen (reinterpret_cast <SAPDB_Char *> (pData)) + 1;

            ret = connection.Send( pData, dataLen, messageList );
        }

        if ( RTETask_ITaskClientConnection::NoError != ret )
        {
            RTE_Message ( messageList, MSG_DIAG );
            connection.Release();
            return;
        }
    }
}

/*---------------------------------------------------------------------------*/

void ClientTransferTest()
{
    RTETask_ITaskClientConnection::ConnRet  ret;
    SAPDB_Byte*                             pData;
    SAPDB_UInt4                             dataLen;
    SAPDBErr_MessageList                    messageList;
    SAPDB_UInt4                             idx = 0;
    SAPDB_Char                              replyData[] = "Hallo hier is packet ";

    RTETask_ITask&       task = *RTETask_ITask::Self();
    
    RTETask_ITaskClientConnection& connection = *task.AcceptClientConnect();
    SAPDB_UInt4 maxSendDataLength             = connection.MaxSendDataLength();

    for (;;)
    {
        ret = connection.Receive( 0, pData, dataLen, messageList );

        if ( RTETask_ITaskClientConnection::NoError == ret )
        {
            strcpy (reinterpret_cast <SAPDB_Char *> (pData), replyData);
            sprintf (reinterpret_cast <SAPDB_Char *> (pData) + 
                     strlen (reinterpret_cast <SAPDB_Char *> (pData)), " %d, %p", idx++, pData);
            dataLen = strlen (reinterpret_cast <SAPDB_Char *> (pData)) + 1;

            ret = connection.Send( pData, dataLen, messageList );
        }

        if ( RTETask_ITaskClientConnection::NoError != ret )
        {
            RTE_Message ( messageList, MSG_DIAG );
            connection.Release();
            return;
        }
    }
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/