/*!
  @file           geo906.h
  @author         RaymondR
  @special area   DBMServer communication
  @brief          DBMServer communication
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



#ifndef GEO906_H
#define GEO906_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo001.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define DBM_TRANSFER_BUFFER_SIZE        16384
#define DBM_MIN_RESPONSE_PACKET_SIZE    100


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

typedef struct teo906_Connection 
{
#if defined (_WIN32)
    HANDLE                      CancelThread;
    BOOL                        ItsAShmConnection;
    struct tos906c_shmServer    *ShmComm;
#else
    teo001_Pid                  CancelProcess;
#endif

#if defined (DUMP_PACKET_DATA)
    char*                       pSendDataBuffer;
#endif

    void                        *pCommObj;
} teo906_Connection;

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO906_H */
