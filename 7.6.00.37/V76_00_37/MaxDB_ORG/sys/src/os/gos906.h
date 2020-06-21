/*!
  @file           gos906.h
  @author         RaymondR
  @brief          
  @see            

\if EMIT_LICENCE

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



\endif
*/



#ifndef GOS906_H
#define GOS906_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo906.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define CANCELSTRING_COS905C   "cancel"
#define CANCELSIZE_COS905C     7

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


typedef struct tos906c_shmServer 
{
    HANDLE               shmHandle;
    HANDLE               clientProcess;
    HANDLE               requestEvent;
    HANDLE               replyEvent;
    char                *packetMem;
    BOOL                 isAlive;
    BOOL                 replyAvailable;
    char                 filler [2];
    int                  replyOffset;
    char                *replyPacket;
    tsp00_Int4             replyLen;
    int                  requestOffset;
    char                *requestPacket;
    tsp00_Int4             requestLen;
} tos906c_shmServer;



# if defined( __cplusplus )
extern "C" {
#endif

BOOL sql906c_ItsAShmConnectRequest       ( int                   argc,
                                           char*                 argv [] );

teo906_Connection* sql906c_shmConnect    ( int                          argc,
                                           char*                        argv [],
                                           char const ** const          ppServerDB,
                                           char const ** const          ppDBRoot,
                                           SAPDB_Char * const           pSenderNodeId,
                                           RTE_OSPid VAR_VALUE_REF      pSenderPID);

tsp01_CommErr_Enum sql906c_shmReceive    ( teo906_Connection *   pConnection,
                                           char** const          RecvDataBuffer,
                                           int*                  RecvLen,
                                           char** const          SendDataBuffer,
                                           int* const            MaxSendLen );

tsp01_CommErr_Enum sql906c_shmSend       ( teo906_Connection *   pConnection,
                                           int                   SendLen );

void               sql906c_shmRelease    ( teo906_Connection *   pConnection );

tsp01_CommErr_Enum sql906c_shmSendCancel ( DWORD                                processId,
                                           tsp00_ErrText VAR_ARRAY_REF          errtext );

void sql906c_waitForEvent                ( HANDLE                event,
                                           HANDLE                process,
                                           BOOL                  *ok );
          
# if defined( __cplusplus )
}
#endif
            
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GOS906_H */
