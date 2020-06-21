/*!
  @file           RTECons_StandardSession.hpp
  @author         StefanP
  @brief          Console (XCons) - Standard XCons Session
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

#ifndef RTECONS_STANDARDSESSION_HPP
#define RTECONS_STANDARDSESSION_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include	"RunTime/Console/RTECons_Main.h"
#include	"RunTime/Console/RTECons_Communication.hpp"
#include	"RunTime/RTE_ConsoleStandardDataRequests.h"

#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


extern SAPDBTrace_Topic Console_Trace; ///< global console trace object

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define TASKS_INFO_P_LINE                   2

#define ARGS(max, cnt)                      (((max)-(cnt)) >= 1)

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
/*!
  thread time in days, hours, minutes, seconds and milliseconds
 */
typedef struct thread_times
  {
    ULONG                         ulDays,
                                  ulHr,
                                  ulMin, 
                                  ulSec,
                                  ulMsec ;
  } XC_THREAD_TIMES ;

/*===========================================================================*
 *  PROTOTYPS                                                                *
 *===========================================================================*/
/*!
  @class          RTECons_StandardSession
   @description    With a instance of this class a standard console connection
              can be established. This is used e.g. by XCons.
              ('Standard' means that this sessions uses the standard
              protocol on the data side of the communication packets. A further 
              protocol may use e.g. XML which would result in a new class like 
              RTECons_XMLSession.)

 */

class RTECons_StandardSession
{
public:
  /*!
     @description    Constructor
     @param          request [in] -  Request parameter

   */

    RTECons_StandardSession     (RTE_ConsoleRequestRecord &         request);

  /*!
     @description    Destructor

   */

    ~RTECons_StandardSession    ()
        {SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::~RTECons_StandardSession", 
                                   Console_Trace, 9);}

  /*!
     @description    Initialize a console connection
     @param          messageList [in/out] - Message list
     @return value   NoError, Error, Offline
   */
 
    RTECons_Communication::CommRC  
                Create          (SAPDBErr_MessageList &             messageList);

  /*!
     @description    Establish the console connection
     @param          messageList [in/out] - Message list
     @return         RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT
   */
 

    SAPDB_UInt4 Connect         (SAPDBErr_MessageList &             messageList);

  /*!
     @description    Processing the request and writing the result to standard output
     @param          messageList [in/out] - Message list
     @return value   RTE_CONS_NO_ERROR       :   No error
                RTE_CONS_ERROR          :   An error occured                - disconnect is recommended
                RTE_CONS_FATAL_ERROR    :   A fatal error occured           - disconnect is not possible
                RTE_CONS_TIMEOUT        :   Timeout occurred                - disconnect is not necessary

   */


    SAPDB_UInt4 RequestToStandardOutput (SAPDBErr_MessageList &             messageList);

  /*!
     @description    Processing a request and returning the result


                  This method is meant for standard requests where the size
                  of the reply is known in advance. Therefor no dynamic memory
                  is needed to process the reply (e.g. COM_CANCEL)
                  Sollte geändert werden, dann können z.B. auch die AWE requests
                  behandelt werden. Der Aufrufer muss dann immer für einen ausreichend
                  gross dimensionierten Speicher sorgen. Aber dass wollte ich clientseitig
                  sowieso verlangen
                  !!!**********************************************************************
                  Attention
                  This function can only handle requests where the reply fits in one
                  packet. This is guarantied for all requests using CONSOLE_DATA_REC.
                  !!!**********************************************************************
     @param          commandType [in] - CON_REQ_...
     @param          pDataPacket [in/out] - Data in/out
     @param          dataLenOut [in] - Data length to be send
     @param          dataLenIn [out] - Data length to be read
     @param          messageList [in/out] - Message list
     @return value   RTE_CONS_NO_ERROR       :   No error
                RTE_CONS_ERROR          :   An error occured                - disconnect is recommended
                RTE_CONS_FATAL_ERROR    :   A fatal error occured           - disconnect is not possible
                RTE_CONS_TIMEOUT        :   Timeout on server side occurred - disconnect is not necessary

   */

/*
    SAPDB_UInt4 DataRequest     (SAPDB_UInt4 const                  commandType,
                                 SAPDB_Byte * const                 pDataPacket,
                                 SAPDB_UInt4 const                  dataLenOut,
                                 SAPDB_UInt4 const                  dataLenIn,
                                 SAPDBErr_MessageList &             messageList)
        {SAPDBTRACE_METHOD_DEBUG ("RTECons_StandardSession::DataRequest", Console_Trace, 1);

         return (Request (DATA_REQ, commandType, CON_REQUEST_SECURITY(commandType), pDataPacket, 
                          dataLenOut, dataLenIn, messageList));}
*/
  /*!
     @description    Release console connection
     @param          messageList [in/out] - Message list
     @return         RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT

   */
 
    SAPDB_UInt4  Release         (SAPDBErr_MessageList &             messageList);

private:
    SAPDB_UInt4 GetError        (SAPDB_UInt1 const                              expectedType,
                                 SAPDB_UInt4                                    messageLen,
                                 SAPDBErr_MessageList &                         messageList); 

    SAPDB_UInt4 SendRequest     (SAPDB_UInt4 const                              reqRespType,
                                 SAPDB_UInt4 const                              commandType,
                                 SAPDB_Bool const                               security,
                                 SAPDB_Byte * const                             pDataPacket,
                                 SAPDB_UInt4 const                              dataLen,
                                 SAPDBErr_MessageList &                         messageList);

    SAPDB_UInt4 GetFirstReplyHeader  (RTE_ConsoleStandardDataHeader &                header,
                                 SAPDB_UInt4 const                              expectedType,
                                 SAPDB_UInt4 &                                  bytesTotal,
                                 SAPDBErr_MessageList &                         messageList);

    SAPDB_UInt4 GetNextReplyHeader  (RTE_ConsoleStandardDataHeader &                header,
                                 SAPDB_UInt4 const                              expectedType,
                                 SAPDB_UInt4 &                                  bytesTotal,
                                 SAPDBErr_MessageList &                         messageList);

    SAPDB_UInt4 GetReplyData    (RTE_ConsoleStandardDataHeader &                header,
                                 SAPDB_UInt4 const                              expectedType,
                                 SAPDB_UInt4 const                              dataLen,
                                 SAPDB_Byte * const                             pData,
                                 SAPDBErr_MessageList &                         messageList);

    SAPDB_UInt4 Request         (SAPDB_UInt4 const                              reqRespType,
                                 SAPDB_UInt4 const                              commandType,
                                 SAPDB_Bool const                               security,
                                 SAPDB_Byte * const                             pDataPacket,
                                 SAPDB_UInt4 const                              dataLenOut,
                                 SAPDB_UInt4 const                              dataLenIn,
                                 SAPDBErr_MessageList &                         messageList);

    SAPDB_UInt4 TestRequest     (SAPDB_Byte *                                   testString,
                                 SAPDB_UInt4 const                              commandType,
                                 SAPDBErr_MessageList &                         messageList);

#if defined (_WIN32)
    SAPDB_UInt4 ShowIOInfo        ( SAPDBErr_MessageList &                         messageList );

    SAPDB_UInt4 ShowAIOInfo       ( SAPDBErr_MessageList &                         messageList );

    SAPDB_UInt4 ShowRegionInfo    ( SAPDBErr_MessageList &                         messageList );

    SAPDB_Char * KTStateName      ( SAPDB_ULong const                              ulState ) const;


    SAPDB_UInt4 ShowRTEInfo       ( SAPDBErr_MessageList &                         messageList );

    SAPDB_UInt4 ShowStorageInfo   ( SAPDBErr_MessageList &                         messageList );

    SAPDB_UInt4 ShowTaskInfo      ( RTE_ConsoleStandardDataPacket &                request,
                                    SAPDBErr_MessageList &                         messageList );

    SAPDB_UInt4 ShowUKTSlInfo     ( SAPDBErr_MessageList &                         messageList );

    void        ConvThreadTime    ( FILETIME *                                     pThreadTime,
                                    XC_THREAD_TIMES *                              pTime,
                                    XC_THREAD_TIMES *                              pSumTime ) const;

    SAPDB_UInt4 ShowThreadTimes   ( SAPDBErr_MessageList &                         messageList );

    void        ShowSuspends      ( CONS_SHOW_SUSPEND_REC &                        suspReasons ) const;

    void        ShowUKTQueue      ( SAPDB_ULong const                              ulUKTQueueLen,
                                    PCONS_SINGLE_QUEUE_INFO_REC                    pUKTUKTQueue ) const;
 
    void        ShowCOMQueue      ( SAPDB_ULong const                              ulCOMQueueLen,
                                    PCONS_SINGLE_QUEUE_INFO_REC                    pUKTCOMQueue ) const;

    SAPDB_Char * ReqTypeName      ( SAPDB_ULong const                              ulReqType ) const;

    void        ShowRUNQueue      ( SAPDB_ULong const                              ulRUNQueueLen,
                                    PCONS_SINGLE_QUEUE_INFO_REC                    pUKTRUNQueue ) const;

    SAPDB_UInt4 ShowUKTQueues     ( SAPDBErr_MessageList &                         messageList );

    void        ShowStateInfo     ( CONS_STATE_INFO_REC &                          stateInfo ) const;

    SAPDB_UInt4 ShowQueueInfo     ( SAPDBErr_MessageList &                         messageList );

    SAPDB_UInt4 ShowTaskCountInfo ( RTE_ConsoleStandardDataPacket &                request,
                                    SAPDBErr_MessageList &                         messageList );

    void        ShowTotalCountInfo( CONS_TOTAL_COUNTS_INFO_REC &                   totalCntInfo ) const;

    SAPDB_UInt4 ShowAWEInfo       ( SAPDBErr_MessageList &                         messageList );

    SAPDB_UInt4 ShowConnectionsInfo( SAPDBErr_MessageList &                        messageList );
#endif

    SAPDB_UInt4 ShowTaskMoveInfo  ( RTE_ConsoleStandardDataPacket &                request,
                                    SAPDBErr_MessageList &                         messageList );

    SAPDB_UInt4 ShowMoveInfo      ( SAPDBErr_MessageList &                         messageList );

    SAPDB_UInt4 ShowRWRegions     ( RTE_ConsoleStandardDataPacket &                request,
                                    SAPDBErr_MessageList &                         messageList );

    RTECons_Communication       m_CommPort;
    RTE_ConsoleRequestRecord    m_request;
    SAPDB_Bool                  m_FirstCall;
};


#endif  /* RTECONS_STANDARDSESSION_HPP */
