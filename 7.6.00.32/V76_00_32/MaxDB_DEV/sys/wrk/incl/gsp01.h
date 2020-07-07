/*!
  @file           gsp01.h
  @author         JoergM
  @special area   Communication
  @brief          

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


#ifndef GSP01_H
#define GSP01_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


enum tsp01_Service_Enum {
   srvUser_esp01,
   srvAsyncUser_esp01,
   srvUtility_esp01,
   srvDistribution_esp01,
   srvControl_esp01,
   srvEvent_esp01,
   srvStream_esp01,
   srvPing_esp01,
   srvLVCUser_esp01,
   srvOLTPUser_esp01
} ;

typedef enum tsp01_Service_Enum tsp01_Service_Enum ;

typedef ENUM_TYPE_MSP00 (tsp01_Service, tsp01_Service_Enum, srvEvent_esp01, tsp00_Uint1);

enum tsp01_CommErr_Enum{
    commErrOk_esp01,
    commErrNotOk_esp01,
    commErrTasklimit_esp01,
    commErrTimeout_esp01,
    commErrCrash_esp01,
    commErrStartRequired_esp01,
    commErrShutdown_esp01,
    commErrSendLineDown_esp01,
    commErrReceiveLineDown_esp01,
    commErrPacketLimit_esp01,
    commErrReleased_esp01,
    commErrWouldBlock_esp01,
    commErrUnknownRequest_esp01,
    commErrServerOrDBUnknown_esp01
} ;
typedef enum tsp01_CommErr_Enum tsp01_CommErr_Enum ;

typedef ENUM_TYPE_MSP00 (tsp01_CommErr, tsp01_CommErr_Enum, commErrWouldBlock_esp01, tsp00_Uint1);

struct tsp01_EnumDummyStruct {
    /* These are instantiated in this struct because otherwise,
     * any prototype using these enums will lead to a
     * cryptical error message
     * @see "http://p26326.bea.sap-ag.de/Entwicklung/cpp/cryptErrors.html"
     */
    tsp01_Service       serviceDummy;
    tsp01_CommErr       commErrDummy;
};

#define PACKET_CNT_MXSP01 2


#define RTE_ERROR_TEXT_MXSP01 (80+1)
#define OS_ERROR_TEXT_MXSP01  (80+1)

#define RTE_NO_ERROR_SP01  0
#define RTE_ERROR_SP01     1
#define OS_NO_ERROR_SP01   0

typedef char  tsp01_RteErrString[RTE_ERROR_TEXT_MXSP01];
typedef char  tsp01_OSErrString [OS_ERROR_TEXT_MXSP01];

typedef struct tsp01_RteError {
 tsp00_Int4         RteErrCode ;
 tsp01_RteErrString RteErrText ;
 tsp00_Int4         OsErrCode  ;
 tsp01_OSErrString  OsErrText  ;
} tsp01_RteError ;

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/



#endif  /* GSP01_H */
