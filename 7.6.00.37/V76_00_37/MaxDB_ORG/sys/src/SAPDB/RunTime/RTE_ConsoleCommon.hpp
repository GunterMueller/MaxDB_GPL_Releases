/*!
  @file           RTE_ConsoleCommon.hpp
  @author         Stefan Paeck
  @special area   Console (XCons) <-> Console Thread
  @brief          Common definitions and declarations
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



#ifndef RTE_CONSOLECOMMON_HPP
#define RTE_CONSOLECOMMON_HPP

#if (defined (LINUX) && !defined(HAS_NPTL) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)) || defined (SUN)
#define HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
#endif

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "geo001.h"

#if !defined (_WIN32) && !defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)  /*&if $OSSPEC != WIN32 && !(($OSSPEC == LINUX && $__HAS_NPTL != 1) || $MACH = SUN) */
#include    "geo07.h"                                              /* nocheck */
#endif                                                             /*&endif*/

#include    "RunTime/RTE_UniqueId.h"
#include    "RunTime/Communication/RTEComm_PacketHeader.h"



/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

#if defined(_WIN32)
typedef RTE_UniqueIdBuffer  RTE_ConsoleSemID; 
#else
typedef key_t               RTE_ObjKey; 
typedef int                 RTE_ConsoleSemID;
#endif



#if defined(_WIN32)
#define RTE_CONSOLE_UNDEF_SEM_ID        ""
#else
#define RTE_CONSOLE_UNDEF_SEM_ID        ((RTE_ConsoleSemID)-1)
#endif

#define RTE_CONSOLE_UNDEF_SHM_ID        ""


#define RTE_CONSOLE_CLIENT_MODE         0
#define RTE_CONSOLE_SERVER_MODE         1

#define RTE_CONSOLE_UNLIMITED_LENGTH    SAPDB_MAX_UINT4
/*===========================================================================*
 *  Console Error Codes                                                      *
 *===========================================================================*/

#define RTE_CONS_NO_ERROR           0
#define RTE_CONS_ERROR              1
#define RTE_CONS_FATAL_ERROR        2
#define RTE_CONS_TIMEOUT            3   //Timeout on own semaphore occurred
#define RTE_CONS_REMOTE_TIMEOUT     4   //Timeout on remote side occurred


/*===========================================================================*
 *  Communication types                                                      *
 *===========================================================================*/

#define RTE_CONS_COM_STANDARD   0x0001
/* #define RTE_CONS_COM_XML     0x0002*/

/*===========================================================================*
 *  Pipe/Mailslot Requests                                                   *
 *===========================================================================*/
/*! @name Pipe/Mailslot requests */
 /*@{*/

/*!
   @description    Structures for the connect requests

 */


typedef struct
{
    RTE_OSPid               pid;
    SAPDB_UInt              commType;               /* RTE_CONS_COM_STANDARD,... */
    RTE_UniqueIdBuffer      consoleShmID;
} RTE_ConsoleOpenData;

typedef SAPDB_UInt8     RTE_ConsoleHandle;                   

typedef union
{
    RTE_ConsoleOpenData     connectData;
    RTE_ConsoleHandle       hConnect;
} RTE_ConsoleRequestData;


typedef struct
{
    SAPDB_Byte                  header[PACKET_HEADER_BUFFER_SIZE];
    RTE_ConsoleRequestData      data; 
} RTE_ConsoleRequest;


/*@}*/
/*===========================================================================*
 *  Communication                                                            *
 *===========================================================================*/

//Request/respond types (RTE_Header)
#define     OPEN_REQ    0x01
#define     CLOSE_REQ   0x02
#define     DATA_REQ    0x03

#define     OPEN_REP    0x10
#define     CLOSE_REP   0x20
#define     DATA_REP    0x40
#define     TIMEOUT     (OPEN_REP | CLOSE_REP | DATA_REP)

#define     UNDEF_TYPE  0xFF




/*===========================================================================*
 *  PROTOTYPS                                                                *
 *===========================================================================*/


#if !defined (_WIN32)
/*!
   @brief       Converting given UNIX error number in error string
   @param       err [in] - error numberMessage list
   @return      Pointer to error string

 */
SAPDB_Char const *  RTE_ConsoleUNIXError    (int const                          err);
#endif

#endif  /* RTE_CONSOLECOMMON_HPP */
