/*!
  @file           vos03u.cpp
  @author         RaymondR
  @brief          component-kernel-communication
  @see            

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




/*
 * INCLUDE FILES
 */
#define RTE_USE_SOCK

#include "gos00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "gos003.h"
#include "geo003.h"
#include "heo03.h"
#include "hos23.h"
#include "hos33.h"
#include "geo40.h"
#include "geo401.h"
#include "geo03.h"
#include "geo43_0.h"
#include "RunTime/System/RTESys_SystemInfo.h"
#include "RunTime/Security/RTESec_SAPSSL.h"
#include "RunTime/Security/RTESec_SSL.h"
#include "RunTime/System/RTESys_MicroTime.h"
#include "RunTime/Tasking/RTETask_ITaskClientConnection.hpp"
#include "heo420.h"

/*
 *  DEFINES
 */
#define MOD__  "VOS3UC : "
#define MF__   MOD__"UNDEFINED"

#define LOCK    TRUE
#define UNLOCK  FALSE


/*
 *  MACROS
 */
#define MSG1(x) {if(DbgLevel>=1) sql60_msg_con_diag x;}
#define MSG2(x) {if(DbgLevel>=2) sql60_msg_con_diag x;}
#define MSG3(x) {if(DbgLevel>=3) sql60_msg_con_diag x;}
#define MSG4(x) {if(DbgLevel>=4) sql60_msg_con_diag x;}
#define MSG5(x) {if(DbgLevel>=5) sql60_msg_con_diag x;}
#define MSG6(x) {if(DbgLevel>=6) sql60_msg_con_diag x;}
#define MSG7(x) {if(DbgLevel>=7) sql60_msg_con_diag x;}
#define MSG8(x) {if(DbgLevel>=8) sql60_msg_con_diag x;}
#define MSG9(x) {if(DbgLevel>=9) sql60_msg_con_diag x;}

/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */

//
// --- User connection info record
//
typedef struct user_connect_info_record
  {
  ULONG                           ulPacketCnt;
  ULONG                           ulClientRef;
  ULONG                           ulConnStatus;
  ULONG                           ulProtocol;
  ULONG                           ulMaxCmdDataLen;
  char                            authenticationAllow[256];
  PCOMM_PACKET_REC                pCommPacketList[MAX_SQL_PACKETS];

  ULONG                           ulTraceConID;   // for racing only

  SHM_CONNECT_INFO_REC            Shm;   // - vos33c, internal use only!
  TCPIP_CONNECT_INFO_REC          TCPIP; // - vos23c, internal use only!
  teo40_NiConnectInfoRecord       NI;    // - veo40.c, SAP Network Interface
  TOSCM_COMM_CLASS               *commObj;
  } USER_CONNECT_INFO_REC;

typedef USER_CONNECT_INFO_REC     *PUSER_CONNECT_INFO_REC;


/*
 * EXTERNAL VARIABLES
 */
externC BOOLEAN    bRuntimeComponent;
externC BOOL       fCannotStartThread;

/*
 *  EXPORTED VARIABLES
 */

externC SQL_NODEIDC      szLocalNodeName = "";
externC TOSCM_COMMSTRUCTOR_FT  * sql03c_constructorPointer = NULL;

/*
 * LOCAL VARIABLES
 */
static  PVOID                     pHandles;
static  BOOL                      fInitialized         = FALSE;
static  ULONG                     ulCurrClientRef      = (ULONG)UNDEF;
static  ULONG                     ulTCPIP_InitResult   = TCPIP_INIT_NOTOK;
static  LONG                      lDumpReference       = UNDEF;
static  BOOL                      DbgLevel             = 0;
static  ULONG                     ulGlobalTraceConID   = 1;   // for racing only
static  LONG                      lExclusive           = 0;   // for racing only

#if defined(_WIN32)
 static  SECURITY_ATTRIBUTES      WorldSA;
 static  SECURITY_ATTRIBUTES      LockSA;
#else
 static  SIGNALFUNCTYPE           FetchedSigInt        = SIG_DFL;
#endif

/*
 * EXTERNAL FUNCTION PROTOTYPES
 */

/*
 * LOCAL FUNCTION PROTOTYPES
 */
LONG          sql03u_get_protocol    ( PSZ                    pszServerNode,
                                       PSZ                    pszServerDB,
                                       ULONG                  ulServiceType,
                                       PULONG                 pulProtocol,
                                       SAPDB_Bool &           ignoreHostnameInServerCert,
                                       TOSCM_COMMSTRUCTOR_FT  * & pfCommstructor,
                                       tsp00_ErrTextc &       pErrText );

VOID _System  sql03u_exithandler     ( ULONG                  ulTermCode );

tsp01_CommErr_Enum         sql03u_connect  (PSZ                    pszServerNode,
                                            PSZ                    pszServerDB,
                                            ULONG                  ulServiceType,
                                            ULONG                  ulPacketCnt,
                                            tsp00_Int4 &           ulMaxReplyDataLen,
                                            tsp00_Int4 &           ulMaxCmdDataLen,
                                            PVOID                  pSQLPacketList[],
                                            tsp00_Int4 &           ulClientRef,
                                            PSZ                    pszServerDBRoot,
                                            PSZ                    pszServerPgm,
                                            TOSCM_COMMSTRUCTOR_FT  *pfCommstructor,
                                            tsp00_ErrTextc &       errText );

tsp01_CommErr_Enum         sql03u_cancel   (ULONG                  ulClientRef,
                                            char *                 szSaprouterString, /* in case of NI another SaprouterString  */
                                            tsp00_ErrTextc &       errText );

tsp01_CommErr_Enum         sql03u_release  (ULONG                  ulClientRef,
                                            tsp00_ErrTextc &       errText );

tsp01_CommErr_Enum         sql03u_request  (ULONG                  ulClientRef,
                                            PVOID                  pCmdData,
                                            ULONG                  ulCmdDataLen,
                                            tsp00_Uint1            RTEFlags,
                                            tsp00_ErrTextc &       errText );

tsp01_CommErr_Enum         sql03u_replyavailable   (ULONG               ulClientRef,
                                                    tsp00_ErrTextc &    pErrText );
tsp01_CommErr_Enum         sql03u_receive   (ULONG                 ulClientRef,
                                             PVOID *               ppResData,
                                             tsp00_Int4 &          ulResDataLen,
                                             tsp00_ErrTextc &      errText );


#if defined(_WIN32)
 BOOL WINAPI  sql03u_catch_cancel_sig( DWORD                  dwCtrlType );
#else
 VOID _CDECL  sql03u_catch_cancel_sig( INT                    SigType );
#endif


tsp01_CommErr_Enum         sql03u_dump      (ULONG                 ulClientRef,
                                             tsp00_ErrTextc &      errText );

ULONG         sql03u_get_server_ref  ( PUSER_CONNECT_INFO_REC pConnInfo );

static TOSCM_COMM_CLASS *
sql03u_procserver_constructor (
    void          * commInfo,
    tsp00_ErrText   errtext,
    tsp01_CommErr * returncode);
/*
 * ========================== GLOBAL FUNCTIONS ================================
 */



externC tsp01_CommErr_Enum  SqlPingConnect ( char*                  szServerNode,    /* '\0' terminated node or saprouterstring */
                                             char*                  szServerDB,      /* '\0' terminated dbname                  */
                                             tsp00_Int4 &           reference,      /* connection reference                    */
                                             tsp00_ErrTextc &       errText )       /* '\0' terminated error string            */
  {
  #undef  MF__
  #define MF__ MOD__"SqlPingConnect"
  tsp01_CommErr_Enum        commError;
  tsp00_Int4                packetSize;
  void*                     packetList[2];
  tsp00_Int4                ulMaxReplyDataLen;

  DBGIN;

  sql03u_init();

  if (strlen (szServerDB) >= sizeof (SQL_DBNAMEC))
  {
      eo46BuildErrorStringRC ( errText, ERRMSG_COM_SERVERDB_NAME_TOO_LONG, 0 );
      DBGOUT;
      return commErrNotOk_esp01;
  }

  commError = (tsp01_CommErr_Enum)sql03u_connect ( szServerNode,
                                                   szServerDB,
                                                   (ULONG) srvPing_esp01,
                                                   1,
                                                   ulMaxReplyDataLen,
                                                   packetSize,
                                                   packetList,
                                                   reference,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   errText );

  DBGOUT
  return (commError) ;
  }

/*------------------------------*/


externC tsp01_CommErr_Enum SqlDBPing ( tsp00_Int4                         reference,
                                       tsp00_Uint4 &                      sendBufferSize,
                                       tsp00_Uint1 &                      hops,
                                       tsp00_DbNamec   &                  szServer,
                                       tsp00_Versionc  &                  szServerVersion,
                                       tsp00_ErrTextc &                   errText )
{
    tsp01_CommErr_Enum              commState;
    char*                           server;
    char*                           serverVersion;
    PUSER_CONNECT_INFO_REC          pConnInfo;
    tsp00_Uint4                     sendLen;
    ULONG                           tmp;

    // --- get the pointer to the internal communication structure
    if ( !sql09c_get_handle_data  ( pHandles, reference, (PVOID*)&pConnInfo, &tmp, &tmp ))
    {
        MSG1 (( INFO_CL_COMM_REQUEST_ERR, pConnInfo->ulTraceConID, commErrNotOk_esp01, ERRMSG_COM_ILL_REFERENCE ));
        eo46BuildErrorStringRC ( errText, ERRMSG_COM_ILL_REFERENCE, 0 );
        return commErrNotOk_esp01;
    }

    if ( sendBufferSize > pConnInfo->ulMaxCmdDataLen ) 
        sendBufferSize = pConnInfo->ulMaxCmdDataLen;

    sendBufferSize = ALIGN( sendBufferSize, SQL_PACKET_ALIGNMENT );

    sendLen =  eo420FillPingPacket ( (char*)pConnInfo->pCommPacketList[0]->pDataPart,
                                     sendBufferSize, hops, "PingClient", 0, "!!! SEND DATA !!!" );

    if ( 0 != sendLen )
    {
        commState = sql03u_request ( reference, pConnInfo->pCommPacketList[0]->pDataPart, sendLen, RSQL_NORMAL_EO003, errText );

        if ( commState == commErrOk_esp01 )
        {
            char*              pResPacket;
            tsp00_Int          resDataLen;

            commState = sql03u_receive ( reference, (void**)&pResPacket, resDataLen, errText );

            if ( commState == commErrOk_esp01 )
            {
                if ( eo420ExtractPingPacket( pResPacket, resDataLen, &hops, &server, &serverVersion ) )
                {
                    if ( server )
                    {
                        szServer[sizeof(tsp00_DbNamec)-1];
                        strncpy ( szServer, server, sizeof(tsp00_DbNamec));
                    }
                    if ( serverVersion )
                    {
                        szServerVersion[sizeof(tsp00_Versionc)-1];
                        strncpy ( szServerVersion, serverVersion, sizeof(tsp00_Versionc));
                    }
                }
                else
                {
                    MSGD (( ERR_PROTOCOL_ERROR, "PING REPLY" ));
                    eo46BuildErrorStringRC ( errText, ERRMSG_PROTOCOL_ERROR, 0 );
                    commState = commErrNotOk_esp01;
                }
            }
        }
    }
    else
    {
        MSGD (( ERR_PROTOCOL_ERROR, "PING REPLY" ));
        eo46BuildErrorStringRC ( errText, ERRMSG_PROTOCOL_ERROR, 0 );
        commState = commErrNotOk_esp01;
    }

    return commState;
}

/*------------------------------*/

externC void SqlPingRelease (tsp00_Int4 reference)
  {
  #undef  MF__
  #define MF__ MOD__"SqlPingRelease"
  tsp00_ErrTextc  errText;

  sql03u_release ( reference, errText );

  return;
  }

/*------------------------------*/

externC tsp01_CommErr_Enum  SqlAConnect (char                  *szServerNode,	/* '\0' terminated node or saprouterstring */
                                         tsp00_DbNamec &        szServerDB,    /* '\0' terminated dbname                  */
                                         tsp01_Service_Enum     service,
                                         tsp00_Int4             packet_cnt,			
                                         tsp00_Int4 &           reference,
                                         tsp00_Int4 &           sql_packet_size,
                                         void                   *sql_packet_list[],  /* tsp00_Array<tsp1_packet_ptr, ...> */
                                         tsp00_ErrTextc &       errText )          /* /* '\0' terminated error string   */
  {
  #undef  MF__
  #define MF__ MOD__"SqlAConnect"
  tsp01_CommErr_Enum        commError;
  tsp00_Int4                ulMaxReplyDataLen;

  DBGIN;

  DBG3 (( MF__, "pszServerNode '%s'", szServerNode ));
  DBG3 (( MF__, "pszServerDB   '%s'", szServerDB.asCharp () ));


  if (strlen (szServerDB.asCharp ()) >= sizeof (SQL_DBNAMEC))
  {
      eo46BuildErrorStringRC ( errText, ERRMSG_COM_SERVERDB_NAME_TOO_LONG, 0 );
      commError = commErrNotOk_esp01;
  }
  else
  {
    commError = sql03u_connect (szServerNode,
                                szServerDB.asCharp (),
                                (ULONG) service,
                                (ULONG) packet_cnt,
                                ulMaxReplyDataLen,
                                sql_packet_size,
                                sql_packet_list,
                                reference,
                                NULL,
                                NULL,
                                NULL,
                                errText);
  }

  if ( commError == commErrOk_esp01 )
  {
      lDumpReference = reference;
  }

  DBGOUT
  return (commError) ;
}

/*------------------------------*/

externC tsp01_CommErr_Enum  SqlDBConnect (char                  *szServerNode,        /* '\0' terminated node or saprouterstring */
                                          tsp00_DbNamec &        szServerDB,          /* '\0' terminated dbname                  */
                                          tsp01_Service_Enum     service,
                                          tsp00_Int4             packet_cnt,			
                                          tsp00_Int4 &           reference,
                                          tsp00_Int4 &           max_reply_packet_size,
                                          tsp00_Int4 &           sql_packet_size,
                                          void                   *sql_packet_list[],  /* tsp00_Array<tsp1_packet_ptr, ...> */
                                          tsp00_ErrTextc &       errText )            /* /* '\0' terminated error string   */
  {
  #undef  MF__
  #define MF__ MOD__"SqlDBConnect"
  tsp01_CommErr_Enum        commError;

  DBGIN;

  DBG3 (( MF__, "pszServerNode '%s'", szServerNode ));
  DBG3 (( MF__, "pszServerDB   '%s'", szServerDB.asCharp () ));


  if (strlen (szServerDB.asCharp ()) >= sizeof (SQL_DBNAMEC))
  {
      eo46BuildErrorStringRC ( errText, ERRMSG_COM_SERVERDB_NAME_TOO_LONG, 0 );
      commError = commErrNotOk_esp01;
  }
  else
  {
      commError = sql03u_connect (szServerNode,
          szServerDB.asCharp (),
          (ULONG) service,
          (ULONG) packet_cnt,
          max_reply_packet_size,
          sql_packet_size,
          sql_packet_list,
          reference,
          NULL,
          NULL,
          NULL,
          errText );
  }

  if ( commError == commErrOk_esp01 )
    lDumpReference = reference;

  DBGOUT
  return (commError) ;
  }

  /*------------------------------*/

externC SAPDB_Bool  SqlSSLAvailable     (tsp00_ErrTextc &      errText)
{
    if (RTESec_SAPSSLIsInitialized ())
    {
        return true;
    }

    tsp00_Pathc         niTracefileName;
    niTracefileName.Init ();

    if (!eo40NiIsInitialized ())
    {
        eo03NiBuildTracefileName (niTracefileName);
    }

    // if eo40NiIsInitialized () is true the niTracefileName was not build above. But this
    // does not matter because in this case the trace name is not needed
    if (commErrOk_esp01 != RTESec_SSLInit(niTracefileName, errText))
    {
        return false;
    }

    return true;
}

/*------------------------------*/

externC SAPDB_Bool  SqlBasicAuthenticationOnly ( tsp00_Int4                   reference,
                                                 tsp00_ErrTextc VAR_ARRAY_REF errText)
{
    PUSER_CONNECT_INFO_REC          pConnInfo;
    ULONG                           tmp;

    // --- get the pointer to the internal communication structure
    if ( !sql09c_get_handle_data  ( pHandles, reference, (PVOID*)&pConnInfo, &tmp, &tmp ))
    {
        MSG1 (( INFO_CL_COMM_REQUEST_ERR, pConnInfo->ulTraceConID, commErrNotOk_esp01, ERRMSG_COM_ILL_REFERENCE ));
        eo46BuildErrorStringRC ( errText, ERRMSG_COM_ILL_REFERENCE, 0 );
        return commErrNotOk_esp01;
    }
    return (( '\0' == pConnInfo->authenticationAllow[0] ) ||
            (   0 == strcmp( pConnInfo->authenticationAllow, "BASIC" )));
}

/*------------------------------*/

void sqlaconnect (	tsp00_TaskId                    pid,
                    tsp00_NodeId &                  servernode,			/* tsp00_Array<char, mxsp_c64> */
                    tsp00_DbName &                  serverdb,			  /* tsp00_Array<char, mxsp_c18> */
                    tsp01_Service_Enum              service,
                    tsp00_Int4          	        packet_cnt,			
                    tsp00_Int4 &                    reference,
                    tsp00_Int4 &                    sql_packet_size,
                    void *                          sql_packet_list[],  /* tsp00_Array<tsp1_packet_ptr, ...> */
                    tsp00_ErrText &                 errtext,			        /* tsp00_Array<char, mxsp_c40> */
                    tsp01_CommErr &                 returncode) 		    /* tsp00_Enum<tsp1_comm_error_Enum, ...> */
  {
  #undef  MF__
  #define MF__ MOD__"sqlaconnect"
  tsp00_NodeIdc     szServerNode;
  tsp00_DbNamec     szServerDB;
  tsp00_ErrTextc    pErrText;
  tsp00_Int4        ulMaxReplyDataLen;

  DBGIN;

  // ---  convert pascal names to null terminated strings
  eo46PtoC ( szServerNode, (tsp00_Byte*)servernode.asCharp(),  sizeof(tsp00_NodeId) );
  eo46PtoC ( szServerDB,   (tsp00_Byte*)serverdb.asCharp(),    sizeof(tsp00_DbName) );


  // ---  If no node name is given, database name into node/dbname parts.
  if ( (szServerNode.asCharp()) [ 0 ] == '\0' )
    sql17u_split_dbname ( szServerDB.asCharp (), szServerNode.asCharp () );

  DBG3 (( MF__, "pszServerNode '%s'", szServerNode.asCharp ( )));
  DBG3 (( MF__, "pszServerDB   '%s'", szServerDB.asCharp ( )));

  returncode.becomes(sql03u_connect ( szServerNode.asCharp (),
                                      szServerDB.asCharp (),
                                      (ULONG) service,
                                      (ULONG) packet_cnt,
                                      ulMaxReplyDataLen,
                                      sql_packet_size,
                                      sql_packet_list,
                                      reference,
                                      NULL,
                                      NULL,
                                      NULL,
                                      pErrText ));

  if ( commErrOk_esp01 != returncode )
  {
      eo46CtoP ( (tsp00_Byte*)errtext.asCharp(), pErrText, sizeof(tsp00_ErrText) );
  }
  else
  {
      lDumpReference = reference;
  }

  DBGOUT
  return;
  }

/*------------------------------*/

void sqlarelease (tsp00_Int4				reference)
  {
  #undef  MF__
  #define MF__ MOD__"sqlarelease"
  tsp00_ErrTextc  pErrText;

  DBGIN;

  lDumpReference = UNDEF;

  sql03u_release ( reference, pErrText );

  DBGOUT;
  return;
  }

/*------------------------------*/

void sqlDBRequest (  tsp00_Int4                      reference,
                     void *                          sql_packet_addr,
                     tsp00_Int4                      sql_packet_length,
                     SAPDB_Bool                      commit,
                     tsp00_ErrText &                 errtext,			
                     tsp01_CommErr &                 returncode) 		
  {
  #undef  MF__
  #define MF__ MOD__"sqlDBRequest"

  tsp00_ErrTextc    pErrText;

  DBGIN;

  lDumpReference = reference;

  ulCurrClientRef = (ULONG)UNDEF;

  returncode.becomes (sql03u_request ( reference, sql_packet_addr, sql_packet_length, 
                                       commit ? RSQL_COMMIT_EO003 : RSQL_NORMAL_EO003, 
                                       pErrText));

  if ( commErrOk_esp01 != returncode)
        eo46CtoP ( (tsp00_Byte*)errtext.asCharp(), pErrText, sizeof(tsp00_ErrText) );

  ulCurrClientRef = (ULONG)reference;


  DBGOUT;
  return;
  }

/*------------------------------*/

void sqlarequest (  tsp00_Int4                      reference,
                    void *                          sql_packet_addr,
                    tsp00_Int4                      sql_packet_length,
                    tsp00_ErrText &                 errtext,			
                    tsp01_CommErr &                 returncode) 		
  {
  #undef  MF__
  #define MF__ MOD__"sqlarequest"

  tsp00_ErrTextc    pErrText;

  DBGIN;

  lDumpReference = reference;

  ulCurrClientRef = (ULONG)UNDEF;

  returncode.becomes (sql03u_request (reference, sql_packet_addr,
                                      sql_packet_length, RSQL_NORMAL_EO003, pErrText));

  if ( commErrOk_esp01 != returncode)
        eo46CtoP ( (tsp00_Byte*)errtext.asCharp(), pErrText, sizeof(tsp00_ErrText) );

  ulCurrClientRef = (ULONG)reference;


  DBGOUT;
  return;
  }

/*------------------------------*/

void sqlareplyavailable (  tsp00_Int4				    reference,
                           tsp00_ErrText &			    errtext,			
                           tsp01_CommErr &              returncode)

  {
  #undef  MF__
  #define MF__ MOD__"sqlareplyavailable"

  tsp00_ErrTextc pErrText;

  DBGIN;

  returncode.becomes (sql03u_replyavailable ( reference, pErrText));

  if ( commErrOk_esp01 != returncode )
        eo46CtoP ( (tsp00_Byte*)errtext.asCharp(), pErrText, sizeof(tsp00_ErrText) );

  DBGOUT;

  return;
  }

/*------------------------------*/


void sqlareceive (	tsp00_Int4                      reference,
                    void **                         res_packet_ptr,
                    tsp00_Int4 &                    res_packet_length,
                    tsp00_ErrText &                 errtext,			
                    tsp01_CommErr &                 returncode)

  {
  #undef  MF__
  #define MF__ MOD__"sqlareceive"

  tsp00_ErrTextc pErrText;

  DBGIN;

  memset(pErrText,0,sizeof(tsp00_ErrTextc));
  lDumpReference   = reference;
  
  returncode.becomes (sql03u_receive (reference, res_packet_ptr,
                                      res_packet_length,
                                      pErrText));

  if (commErrOk_esp01 != returncode)
        eo46CtoP ( (tsp00_Byte*)errtext.asCharp(), pErrText, sizeof(tsp00_ErrText) );

  ulCurrClientRef = (ULONG)UNDEF;

  DBGOUT;
  return;
  }

/*------------------------------*/

void sqlacancel ( tsp00_Int4              reference)
  {
  #undef  MF__
  #define MF__ MOD__"sqlacancel"
  tsp00_ErrTextc pErrText;

  DBGIN;

  sql03u_cancel ( reference, NULL, pErrText );

  DBGOUT;
  return;
  }

/*------------------------------*/

externC tsp01_CommErr_Enum  SqlAPing ( char *                 pszServerNode,     /* '\0' terminated node or saprouterstring */
                                       SAPDB_Bool             captureServerCert,
                                       tsp00_Versionc &       szServerVersion,   /* '\0' terminated version from answering server */
                                       SAPDB_Byte &           certificateBuffer,
                                       SAPDB_UInt4            lenCertificateBuffer,
                                       tsp00_ErrTextc &       errText)          /* '\0' terminated error string   */
{
  #undef  MF__
  #define MF__ MOD__"SqlAPing"

    tsp01_CommErr_Enum        commError = commErrOk_esp01 ;

   
    if ( pszServerNode == NULL || *pszServerNode == '\0' )
    {
        commError = commErrNotOk_esp01 ;
        strcpy ( errText.asCharp(), "missing hostname" ) ;
    }
    else
    {
        if ( ulTCPIP_InitResult != TCPIP_INIT_OK )
            ulTCPIP_InitResult = sql43_tcpip_init ();

        if ( ulTCPIP_InitResult != TCPIP_INIT_OK )
        {
            strcpy ( errText.asCharp(), "TCP init failed" ) ;
            commError = commErrNotOk_esp01 ;
        }
        else
        {
            SAPDB_Bool              isSaprouterString = false;
            SAPDB_Bool              isNISSL = false;
            SAPDB_Bool              isSSL = false;
            SAPDB_Bool              ignoreHostnameInServerCert = false;

            commError = RTESec_IsSSLConnection (pszServerNode, 
                                                isSSL, 
                                                isNISSL, 
                                                ignoreHostnameInServerCert, 
                                                errText);


            if (commErrOk_esp01 == commError)
            {
                if (!isSSL)
                {
                    isSaprouterString = (eo40NiIsSaprouterSring ( pszServerNode ) == 1);
                }


                if (((isSaprouterString || captureServerCert) && !eo40NiIsInitialized ()) || 
                        (isSSL && !RTESec_SAPSSLIsInitialized ()))
                {
                    tsp00_Pathc         niTracefileName;

                    niTracefileName.Init ();
                    if (!eo40NiIsInitialized ())
                    {
                        eo03NiBuildTracefileName (niTracefileName);
                    }

                    if (isSaprouterString || captureServerCert) // => !eo40NiIsInitialized ()
                    {
                        commError = (tsp01_CommErr_Enum)eo40NiInit(niTracefileName, 0, errText);
                        if (commErrOk_esp01 != commError)
                        {
                            tsp00_ErrTextc  tmpErrText;
                            eo40NiFinish (tmpErrText);
                        }
                    }
                    else
                    {   // if eo40NiIsInitialized () is true the niTracefileName was not build above. But this
                        // does not matter because in this case the trace name is not needed
                        commError = RTESec_SSLInit(niTracefileName, errText);
                    }
                }

                if (commErrOk_esp01 == commError)
                {
                    if (isSSL || isSaprouterString)
                    {
                        commError = (tsp01_CommErr_Enum) eo03NiXserverPing (pszServerNode,
                                                                            szServerVersion, 
                                                                            certificateBuffer,
                                                                            lenCertificateBuffer,
                                                                            isSSL, 
                                                                            isNISSL,
                                                                            ignoreHostnameInServerCert,
                                                                            captureServerCert,
                                                                            errText);
                    }
                    else
                    {
                        if (strlen (pszServerNode) >= sizeof (SQL_NODEIDC))
                        {
                            eo46BuildErrorStringRC ( errText, ERRMSG_COM_SERVER_NODE_NAME_TOO_LONG, 0 );
                            commError = commErrNotOk_esp01;
                        }
                        else
                        {
                            commError = os23XServerPing (pszServerNode, 
                                                         szServerVersion, 
                                                         certificateBuffer,
                                                         lenCertificateBuffer,
                                                         captureServerCert,
                                                         errText);
                        }
                    }
                }

                if (isSSL || isSaprouterString || captureServerCert)
                {
                     eo03Finish ();
                }
            }
        }
    }

    DBGOUT;
    return ( commError ) ;
}

/*------------------------------*/

externC tsp01_CommErr_Enum SqlACancel ( tsp00_Int4             reference ,
                                        char                   *szSapRouterString, /* '\0' terminated node or saprouterstring */
                                        tsp00_ErrTextc &       errText )        /* '\0' terminated error string   */
{
  #undef  MF__
  #define MF__ MOD__"SqlACancel"
  tsp01_CommErr_Enum        commError;

  commError = sql03u_cancel ( reference, szSapRouterString, errText );

  return (commError) ;
}


/*------------------------------*/

VOID sqladump ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sqladump"
  tsp00_ErrTextc pErrText;

  DBGIN;

  if ( lDumpReference != UNDEF )
    sql03u_dump ( lDumpReference, pErrText );


  DBGOUT;
  return;
  }

/*------------------------------*/

tsp01_CommErr_Enum SqlDBDump ( tsp00_Int4                         reference,
                               tsp00_ErrTextc VAR_ARRAY_REF       errText )
  {
  #undef  MF__
  #define MF__ MOD__"sqladump"

  return sql03u_dump ( reference, errText );
  }

/*------------------------------*/

VOID sql03u_init ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql03u_init"
  static  BOOL            fCtrlHandlerSet      = FALSE;
  ACCESS_RIGHTS_REC       Access;
  static  BOOL            fAddFinishToExitList = TRUE;
  APIRET                  rc;
  REG_ENTRY_REC           RegistryEntries[1];


  DBGIN;

  if ( fInitialized == TRUE )
    {
    MSGCD (( ERR_CONN_INFO_INIT_TWICE ));
    ABORT();
    }
  else
    {
    fInitialized = TRUE;

    if ( !sql09c_init_handle_dir ( &pHandles, MAXCONNECTIONS ) )
      {
      MSGCD (( ERR_RESERVE_HANDLE_VALUES ));
      ABORT();
      }

    RegistryEntries[0].ulValueType  = REG_DWORD;
    RegistryEntries[0].ulValueSize  = sizeof(DWORD);
    RegistryEntries[0].pszValueName = REG_VN_DEBUG_LEVEL;
    RegistryEntries[0].pValue       = &DbgLevel;
    
    rc = sql50_reg_get_applic_values ( NULL, HKEY_CURRENT_USER, "", 1, RegistryEntries );
    
    if ( rc != NO_ERROR )
      DbgLevel = 0;

    // --- initialize Windows NT security
    sql49c_set_sync_access ();

    WorldSA.nLength           = sizeof(WorldSA);
    WorldSA.bInheritHandle    = FALSE;

    memset ( &Access, 0, sizeof (Access) );   // - this must be done first!
    Access.World.ulAccess     = SPECIFIC_RIGHTS_ALL |
                                STANDARD_RIGHTS_ALL;

    rc = sql49c_alloc_and_init_SD( &Access, &WorldSA.lpSecurityDescriptor );

    if ( rc != NO_ERROR )
      ABORT();

    LockSA.nLength            = sizeof(LockSA);
    LockSA.bInheritHandle     = FALSE;

    memset ( &Access, 0, sizeof (Access) );   // - this must be done first!
    rc = sql49c_alloc_and_init_SD( &Access, &LockSA.lpSecurityDescriptor );

    if ( rc != NO_ERROR )
      ABORT();

    // --- is a TCP/IP installed?
    ulTCPIP_InitResult = sql43_tcpip_init ();

    if ( fAddFinishToExitList )
      {
      fAddFinishToExitList = FALSE;

      rc = sql02u_add_exit_list ( sql03u_exithandler );

      if ( rc != NO_ERROR )
        {
        DBG1  (( MF__, "Set exit handler failed %d", rc ));
        MSGCD (( ERR_SET_EXIT_HANDLER_FAILED, rc ));
        ABORT();
        }
      }

    #if defined(_WIN32)
      if (( fCtrlHandlerSet          == FALSE  ) &&
          ( bRuntimeComponent        == FALSE  ) &&
          ( fCannotStartThread       == FALSE ))  // - 16 Bit environment?
        {
        if (SetConsoleCtrlHandler( sql03u_catch_cancel_sig, TRUE ))
          fCtrlHandlerSet = TRUE;
        }
    #else
     if ( FetchedSigInt != (SIGNALFUNCTYPE) sql03u_catch_cancel_sig )
       FetchedSigInt = signal ( SIGINT, sql03u_catch_cancel_sig );
    #endif

    }

  DBGOUT;
  return;
  }

/*------------------------------*/

VOID sql03u_finish ( VOID )
{
#undef  MF__
#define MF__ MOD__"sql03u_finish"
    ULONG                           ulReference = 0;
    tsp00_ErrTextc                  pErrText;

    DBGIN;

    if ( fInitialized == TRUE )
    {
        // --- search for open connections
        while ( sql09c_find_next_handle ( pHandles, &ulReference ))
        {
            sql03u_release ( ulReference, pErrText );
        }

        if ( WorldSA.lpSecurityDescriptor != NULL )
        {
            FREE_MEM ( WorldSA.lpSecurityDescriptor );
            WorldSA.lpSecurityDescriptor = NULL;
        }

        if ( LockSA.lpSecurityDescriptor != NULL )
        {
            FREE_MEM ( LockSA.lpSecurityDescriptor );
            LockSA.lpSecurityDescriptor = NULL;
        }

        if ( pHandles != NULL )
        {
            sql09c_release_handle_dir( pHandles );
            pHandles = NULL;
        }

        eo03Finish ();
        sql43_tcpip_cleanup();
        fInitialized = FALSE;
    }

    DBGOUT;
    return;
}

/*------------------------------*/

VOID sql03u_xconnect (PSZ                   pszServerNode,
                      PSZ                   pszServerDB,
                      PSZ                   pszServerDBRoot,
                      PSZ                   pszServerPgm,
                      TOSCM_COMMSTRUCTOR_FT *pfCommstructor,
                      tsp00_Int4 &          ulMaxCmdDataLen,
                      PVOID                 pSQLPacketList[],
                      tsp00_Int4 &          ulClientRef,
                      ULONG &               rc,
                      tsp00_ErrTextc &      errText )
  {
  #undef  MF__
  #define MF__ MOD__"sql03u_xconnect"
  tsp00_Int4        ulMaxReplyDataLen;

  DBGIN;

  if (strlen (pszServerDB) >= sizeof (SQL_DBNAMEC))
  {
      eo46BuildErrorStringRC ( errText, ERRMSG_COM_SERVERDB_NAME_TOO_LONG, 0 );
      rc = commErrNotOk_esp01;
      return;
  }

  if (strlen (pszServerDBRoot) >= sizeof (tsp00_Pathc))
  {
      eo46BuildErrorStringRC ( errText, ERRMSG_COM_SERVERDBROOT_NAME_TOO_LONG, 0 );
      rc = commErrNotOk_esp01;
      return;

  }

  if (strlen (pszServerPgm) >= sizeof (tsp00_Pathc))
  {
      eo46BuildErrorStringRC ( errText, ERRMSG_COM_SERVERPGM_NAME_TOO_LONG, 0 );
      rc = commErrNotOk_esp01;
      return;
  }


  rc = sql03u_connect (pszServerNode,
                           pszServerDB,
                           srvControl_esp01,
                           1,
                           ulMaxReplyDataLen,
                           ulMaxCmdDataLen,
                           pSQLPacketList,
                           ulClientRef,
                           pszServerDBRoot,
                           pszServerPgm,
                           pfCommstructor,
                           errText);

  DBGOUT;
  }


/*
 * ========================== LOCAL FUNCTIONS =================================
 */

static tsp01_CommErr_Enum sql03u_connect (PSZ              pszServerNode,
                                          PSZ              pszServerDB,
                                          ULONG            ulServiceType,
                                          ULONG            ulPacketCnt,
                                          tsp00_Int4 &     ulMaxReplyDataLen,
                                          tsp00_Int4 &     ulMaxCmdDataLen,
                                          PVOID            pSQLPacketList[],
                                          tsp00_Int4 &     ulClientRef,
                                          PSZ              pszServerDBRoot,
                                          PSZ              pszServerPgm,
                                          TOSCM_COMMSTRUCTOR_FT *pfCommstructor,
                                          tsp00_ErrTextc & errText )
  {
  #undef  MF__
  #define MF__ MOD__"sql03u_connect"
  static PID                      pidClientPid = (PID)UNDEF;

  tsp01_CommErr_Enum              ulCommState = commErrOk_esp01;
  ULONG                           ulRetry;
  PUSER_CONNECT_INFO_REC          pConnInfo;
  CONNECT_PARAM_REC               ConnParam;
  APIRET                          rc;
  UINT                            i;
  TOSCM_COMM_CLASS               *commObj;
  tsp01_CommErr_Enum              commError;
  ULONG                           ulTraceConIDTmp;
  TOSCM_COMMSTRUCTOR_FT *         commstructor = pfCommstructor;
  SAPDB_Bool                      retry64Bit = true;
  ULONG                           reference;
  BOOL                            ret = false;
  SAPDB_Bool                      ignoreHostnameInServerCert = false;
  SAPDB_Char					  localHost[] = "localhost";
  

  DBGIN;

  ulClientRef = (ULONG)UNDEF;
  // --- allocate space for the internal communication structure
  rc = ALLOC_MEM ((PPVOID)&pConnInfo, sizeof(USER_CONNECT_INFO_REC) );

  if( rc != NO_ERROR )
    {
    eo46BuildErrorStringRC ( errText, ERRMSG_ALLOC_MEMORY, rc );
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  // --- Get a new handle and combine it with the pointer to the
  //     internal communication structure.

  ret = sql09c_reserve_handle (pHandles, reference);
  if (ret)
  {
      ulClientRef = reference;
      ret = sql09c_put_handle_data (pHandles, ulClientRef, (PVOID)pConnInfo, (ULONG)UNDEF, (ULONG)UNDEF);
  }

  if (!ret)
  {
    MSGD (( ERR_TO_MANY_SESSIONS ));
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_TO_MANY_SESSIONS, 0 );

    FREE_MEM ( pConnInfo );

    DBGOUT;
    return ( commErrNotOk_esp01 );
  }

  // --- initialize internal communication structure
  memset (pConnInfo, 0, sizeof (*pConnInfo) );
  pConnInfo->ulClientRef  = ulClientRef;
  pConnInfo->ulConnStatus = CON_INITIALIZED;
  pConnInfo->ulProtocol   = 0;
  pConnInfo->ulPacketCnt  = ulPacketCnt;

  // --- Determine the protocol to use with the database kernel.
  if ( sql03u_get_protocol (pszServerNode, 
                            pszServerDB, 
                            ulServiceType,
                            &pConnInfo->ulProtocol, 
                            ignoreHostnameInServerCert,
                            commstructor,
                            errText ) != NO_ERROR )
    {
    sql09c_free_handle  ( pHandles, ulClientRef );
    FREE_MEM ( pConnInfo );
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  if ( pidClientPid == (PID)UNDEF )
    GETPROCESSID (&pidClientPid);

  // --- Initialize connect parameter
  memset ( &ConnParam, 0, sizeof(ConnParam) );
  ConnParam.ulPacketCnt       = ulPacketCnt;
  ConnParam.ulClientRef       = ulClientRef;
  ConnParam.ulServiceType     = ulServiceType;
  ConnParam.pWorldSA          = &WorldSA;          // PROT_SHM_EO003 only
  ConnParam.pLockSA           = &LockSA;           // PROT_SHM_EO003 only
  ConnParam.ulConnTimeout     = LOCAL_CONN_TIMEOUT;// PROT_SHM_EO003 only
  ConnParam.pszServerDB       = pszServerDB;
  ConnParam.pszServerNode     = pszServerNode;     // PROT_SOCKET_EO003 / PROT_NI_EO003 / PROT_NISSL_EO003 only
  ConnParam.pszClientNode     = NULL;              // comm. xserver/kernel only
  ConnParam.ulPacketSize      = 0;
  ConnParam.ulMaxDataLen      = 0;
  ConnParam.pidClientPID      = pidClientPid;
  ConnParam.ulCommFlagNo      = (ULONG)UNDEF;
  ConnParam.pszServerDBRoot   = pszServerDBRoot;
  ConnParam.pszServerPgm      = pszServerPgm;
  ConnParam.fAcknowledge      = FALSE;
  ConnParam.isSSLoverSAPRouterString = false;
  ConnParam.isSSL                    = false;
  ConnParam.encryption        = RTETask_ITaskClientConnection::NoEncryption;

  if ( DbgLevel )
    {
    WAIT_UNTIL_ALONE( &lExclusive );
    pConnInfo->ulTraceConID = ulGlobalTraceConID++;
    CLEARLOCK( &lExclusive );

    MSG1 (( INFO_CL_COMM_CONNECT_REQ, pConnInfo->ulTraceConID, GetCurrentThreadId(),
            ConnParam.pszServerNode, ConnParam.pszServerDB, ConnParam.ulClientRef, 
            ConnParam.ulServiceType, pConnInfo->ulProtocol ));
    }

// --- Try to connect
    for (ulCommState = commErrTasklimit_esp01, ulRetry = 0;
        (ulCommState == commErrTasklimit_esp01 && ulRetry <= RETRYCOUNT) || retry64Bit; ulRetry++)
    {
        // ---  Call the connect function for the requested protocol.
        switch ( pConnInfo->ulProtocol )
        {
        case PROT_SHM_EO003 :
            ulCommState = (tsp01_CommErr_Enum) sql33c_connect (&ConnParam, &pConnInfo->Shm,
                                                               errText );
            break;

        case PROT_SOCKET_EO003 :
            ulCommState = (tsp01_CommErr_Enum) sql23c_connect (&ConnParam, &pConnInfo->TCPIP,
                                                               errText );
            break;

        case PROT_NISSL_EO003 :
            ConnParam.isSSLoverSAPRouterString = true;
        case PROT_SSL_EO003 :
            ConnParam.isSSL = true;
            ConnParam.ignoreHostnameInServerCert = ignoreHostnameInServerCert;
        case PROT_NI_EO003 :
            ulCommState = (tsp01_CommErr_Enum) eo03NiConnect (&ConnParam, &pConnInfo->NI, errText); 
            break;

        case PROT_LOCAL_CONTROL_EO003 :
        case PROT_VMTDISPATCH_EO003 :
            // create object
            commObj = commstructor (pConnInfo, errText, &commError);

            if (commError == 0)
            {
                // connect
                commError = commObj->connect (&ConnParam,
                    pszServerDB, pszServerDBRoot, pszServerPgm, errText);
                if (commError != commErrOk_esp01)
                {
                    commObj->release ();
                }
                else
                {
                    pConnInfo->commObj = commObj;
                }
            }
            ulCommState = commError;
            break;
        default :
            MSGCD (( ERR_UNSUPPORTED_PROT, pConnInfo->ulProtocol ));
            ABORT();
        }

        if (( ulCommState == commErrTasklimit_esp01 )  && ( ulRetry <= RETRYCOUNT ))
        {
            if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
                SLEEP(3000);
            else
                SLEEP(1500);
        }
        else if (commErrStartRequired_esp01 == ulCommState &&  PROT_SHM_EO003 == pConnInfo->ulProtocol &&
                 RTESys_Is64BitPlatform ())
        {
            // Fallback: retry with socket communication
            pConnInfo->ulProtocol = PROT_SOCKET_EO003;
            ConnParam.pszServerNode = localHost;
        }
        else
        {
            retry64Bit = false;
        }
    }

  ulTraceConIDTmp = pConnInfo->ulTraceConID; // - we may need this for tracing

  if ( ulCommState == commErrOk_esp01 )
    {
    // --- evaluate the return values
    pConnInfo->ulMaxCmdDataLen       = ConnParam.ulMaxDataLen -
                                       ConnParam.ulMinReplySize;
    ulMaxCmdDataLen                  = pConnInfo->ulMaxCmdDataLen;
    ulMaxReplyDataLen                = ConnParam.ulMaxDataLen;
    strncpy ( pConnInfo->authenticationAllow,
              ConnParam.authenticationAllow,
              sizeof(pConnInfo->authenticationAllow) - 1);

    pConnInfo->authenticationAllow[sizeof(pConnInfo->authenticationAllow) - 1] = '\0';

    for ( i = 0; i < ulPacketCnt; i++ )
      {
      pConnInfo->pCommPacketList[i] = ConnParam.pCommPacketList[i];
      pSQLPacketList[i] = (PVOID)&ConnParam.pCommPacketList[i]->pDataPart;
      }

    // --- Set connection status.
    pConnInfo->ulConnStatus = CON_CONNECTED;
    }
  else
    {
    sql09c_free_handle  ( pHandles, ulClientRef );
    FREE_MEM ( pConnInfo );
    }

  if ( DbgLevel )
    {
    if ( ulCommState == commErrOk_esp01 )
      {
      MSG1 (( INFO_CL_COMM_CONNECTED, ulTraceConIDTmp, sql03u_get_server_ref(pConnInfo),
              pConnInfo->ulMaxCmdDataLen, ConnParam.ulMinReplySize,
              ConnParam.ulPacketCnt, pSQLPacketList[0], ConnParam.ulPacketCnt > 1 ? pSQLPacketList[1] : NULL ));
      }
    else
      {
      MSG1 (( INFO_CL_COMM_CONNECT_ERR, ulTraceConIDTmp, ulCommState, errText.asCharp () ));
      }
    }


  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static tsp01_CommErr_Enum sql03u_request (ULONG             ulClientRef,
                                          PVOID             pCmdData,
                                          ULONG             ulCmdDataLen,
                                          tsp00_Uint1       RTEFlags,
                                          tsp00_ErrTextc &  errText )
  {
  #undef  MF__
  #define MF__ MOD__"sql03u_request"
  tsp01_CommErr_Enum              ulCommState = commErrOk_esp01;
  PUSER_CONNECT_INFO_REC          pConnInfo;
  ULONG                           ulPacket;
  ULONG                           ulTmp;

  DBGIN;

  // --- get the pointer to the internal communication structure
  if ( !sql09c_get_handle_data  ( pHandles, ulClientRef,
                                  (PVOID*)&pConnInfo,
                                  &ulTmp, &ulTmp ))
    {
    MSG1 (( INFO_CL_COMM_REQUEST_ERR, pConnInfo->ulTraceConID, commErrNotOk_esp01, ERRMSG_COM_ILL_REFERENCE ));
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_ILL_REFERENCE, 0 );
    return ( commErrNotOk_esp01 );
    }

  if (( pConnInfo->ulConnStatus != CON_CONNECTED )  &&
      ( pConnInfo->ulConnStatus != CON_RECEIVED  )  &&
      ( pConnInfo->ulConnStatus != CON_CANCELLED ))
    {
    MSG1 (( INFO_CL_COMM_REQUEST_ERR, pConnInfo->ulTraceConID, commErrNotOk_esp01, ERRMSG_COM_WRONG_CONN_STATE ));
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_WRONG_CONN_STATE, 0 );
    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  //
  // --- find current communication packet containing the
  //     requested command data
  //
  for( ulPacket = 0; ulPacket < pConnInfo->ulPacketCnt; ulPacket++ )
    {
    if (pCmdData == (PVOID)pConnInfo->pCommPacketList[ulPacket]->pDataPart)
      break;
    }

  if ( ulPacket >= pConnInfo->ulPacketCnt )
    {
    MSG1 (( INFO_CL_COMM_REQUEST_ERR, pConnInfo->ulTraceConID, commErrNotOk_esp01, ERRMSG_COM_WRONG_PACKET_ADDR ));
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_WRONG_PACKET_ADDR, 0 );

    DBGOUT;
    return ( commErrNotOk_esp01 );
    }

  // --- check the command packet length
  if (( ulCmdDataLen > pConnInfo->ulMaxCmdDataLen ) ||
      ( ulCmdDataLen % SQL_PACKET_ALIGNMENT ))
    {
    PSZ pszErrTxt = ERRMSG_COM_WRONG_PACKET_LEN;

    if ( ulCmdDataLen % SQL_PACKET_ALIGNMENT )
      ulCommState = commErrNotOk_esp01;
    else
      ulCommState = commErrPacketLimit_esp01;

    eo46BuildErrorStringRC ( errText, pszErrTxt, 0 );
    MSG1 (( INFO_CL_COMM_REQUEST_ERR, pConnInfo->ulTraceConID, ulCommState, pszErrTxt ));

    DBGOUT;
    return ( ulCommState );
    }

  MSG2 (( INFO_CL_COMM_REQUEST, pConnInfo->ulTraceConID, GetCurrentThreadId(), pCmdData, ulCmdDataLen ));

  switch ( pConnInfo->ulProtocol )
     {
    case PROT_SHM_EO003 :
      ulCommState = (tsp01_CommErr_Enum) sql33c_request (&pConnInfo->Shm, ulPacket, 
                                                         ulCmdDataLen, RTEFlags, errText );
      break;

    case PROT_SOCKET_EO003 :
      ulCommState = (tsp01_CommErr_Enum) sql23c_request (&pConnInfo->TCPIP, ulPacket, 
                                                         ulCmdDataLen, RTEFlags, errText );
      break;

    case PROT_SSL_EO003 :
    case PROT_NISSL_EO003 :
    case PROT_NI_EO003 :
      ulCommState = (tsp01_CommErr_Enum) eo03NiRequest (&pConnInfo->NI, ulPacket, 
                                                        ulCmdDataLen, RTEFlags, errText );

      break;
    case PROT_LOCAL_CONTROL_EO003 :
    case PROT_VMTDISPATCH_EO003 :
      ulCommState = (tsp01_CommErr_Enum) pConnInfo->commObj->request (pConnInfo->pCommPacketList[ulPacket]->pDataPart,
                                                                      ulCmdDataLen, 
                                                                      errText);
      break;
    default :
      MSGCD (( ERR_WRONG_PROTOCOL, pConnInfo->ulProtocol ));
      ABORT();
    }

  if ( ulCommState == commErrOk_esp01 )
    pConnInfo->ulConnStatus = CON_REQUESTED;

  if ( DbgLevel )
    {
    if ( ulCommState == commErrOk_esp01 )
      {
      MSG2 (( INFO_CL_COMM_REQUESTED, pConnInfo->ulTraceConID ));
      }
    else
      {
      MSG1 (( INFO_CL_COMM_REQUEST_ERR, pConnInfo->ulTraceConID, ulCommState, errText.asCharp () ));
      }
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static tsp01_CommErr_Enum sql03u_replyavailable (ULONG             ulClientRef,
                                                 tsp00_ErrTextc &  errText )
  {
  #undef  MF__
  #define MF__ MOD__"sql03u_replyavailable"
  tsp01_CommErr_Enum              ulCommState = commErrOk_esp01;
  PUSER_CONNECT_INFO_REC          pConnInfo;
  ULONG                           ulTmp;

  DBGIN;

  // --- get the pointer to the internal communication structure
  if ( !sql09c_get_handle_data  ( pHandles, ulClientRef,
                                  (PVOID*)&pConnInfo,
                                  &ulTmp, &ulTmp ))
    {
    MSG1 (( INFO_CL_COMM_REPLY_AVAIL_ERR, pConnInfo->ulTraceConID, commErrNotOk_esp01, ERRMSG_COM_ILL_REFERENCE ));
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_ILL_REFERENCE, 0 );
    return ( commErrNotOk_esp01 );
    }

  if (( pConnInfo->ulConnStatus != CON_REQUESTED ) &&
      ( pConnInfo->ulConnStatus != CON_CANCELLED ))
    {
    MSG1 (( INFO_CL_COMM_REPLY_AVAIL_ERR, pConnInfo->ulTraceConID, commErrNotOk_esp01, ERRMSG_COM_WRONG_CONN_STATE ));
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_WRONG_CONN_STATE, 0 );
    return ( commErrNotOk_esp01 );
    }

  MSG2 (( INFO_CL_COMM_REPLY_AVAIL, pConnInfo->ulTraceConID, GetCurrentThreadId() ));

  switch ( pConnInfo->ulProtocol )
    {
    case PROT_SHM_EO003 :
      ulCommState = (tsp01_CommErr_Enum) sql33c_replyavailable (&pConnInfo->Shm, errText);
      break;

    case PROT_SOCKET_EO003 :
      ulCommState = (tsp01_CommErr_Enum) sql23c_replyavailable (&pConnInfo->TCPIP, errText);
      break;

    case PROT_SSL_EO003 :
    case PROT_NISSL_EO003 :
    case PROT_NI_EO003 :
       ulCommState = (tsp01_CommErr_Enum) eo03NiSqlReplyAvailable (&pConnInfo->NI, errText);
       break;

    case PROT_LOCAL_CONTROL_EO003 :
    case PROT_VMTDISPATCH_EO003 :
        ulCommState = (tsp01_CommErr_Enum) pConnInfo->commObj->replyAvailable (errText);
        break;
    default :
      MSGCD (( ERR_WRONG_PROTOCOL, pConnInfo->ulProtocol ));
      ABORT();
    }

  if ( DbgLevel )
    {
    if (( ulCommState == commErrOk_esp01 ) || ( ulCommState == commErrWouldBlock_esp01 ))
      {
      MSG2 (( INFO_CL_COMM_REPLY_AVAIL, pConnInfo->ulTraceConID, 
              ulCommState == commErrWouldBlock_esp01 ? "not " : "" ));
      }
    else
      {
      MSG1 (( INFO_CL_COMM_REPLY_AVAIL_ERR, pConnInfo->ulTraceConID, ulCommState, errText.asCharp () ));
      }
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static tsp01_CommErr_Enum sql03u_receive (ULONG             ulClientRef,
                                          PVOID             *ppResData,
                                          tsp00_Int4 &      ulResDataLen,
                                          tsp00_ErrTextc &  errText )
  {
  #undef  MF__
  #define MF__ MOD__"sql03u_receive"
  tsp01_CommErr_Enum              ulCommState = commErrOk_esp01;
  PUSER_CONNECT_INFO_REC          pConnInfo;
  ULONG                           ulTmp;
  PCOMM_PACKET_REC                pResPacket;

  DBGIN;

  // --- get the pointer to the internal communication structure
  if ( !sql09c_get_handle_data  ( pHandles, ulClientRef,
                                  (PVOID*)&pConnInfo,
                                  &ulTmp, &ulTmp ))
    {
    MSG1 (( INFO_CL_COMM_RECEIVE_ERR, pConnInfo->ulTraceConID, commErrNotOk_esp01, ERRMSG_COM_ILL_REFERENCE ));
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_ILL_REFERENCE, 0 );
    return ( commErrNotOk_esp01 );
    }

  if (( pConnInfo->ulConnStatus != CON_REQUESTED ) &&
      ( pConnInfo->ulConnStatus != CON_CANCELLED ))
    {
    MSG1 (( INFO_CL_COMM_RECEIVE_ERR, pConnInfo->ulTraceConID, commErrNotOk_esp01, ERRMSG_COM_WRONG_CONN_STATE ));
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_WRONG_CONN_STATE, 0 );
    return ( commErrNotOk_esp01 );
    }

  MSG2 (( INFO_CL_COMM_RECEIVE, pConnInfo->ulTraceConID, GetCurrentThreadId() ));

  switch ( pConnInfo->ulProtocol )
     {
    case PROT_SHM_EO003 :
      ulCommState = (tsp01_CommErr_Enum) sql33c_receive (&pConnInfo->Shm, &pResPacket,
                                                         (ULONG &)ulResDataLen, errText );
      break;

    case PROT_SOCKET_EO003 :
      ulCommState = (tsp01_CommErr_Enum) sql23c_receive (&pConnInfo->TCPIP, &pResPacket,
                                                         (ULONG &)ulResDataLen, errText );
      break;

    case PROT_SSL_EO003 :
    case PROT_NISSL_EO003 :
    case PROT_NI_EO003 :
      ulCommState = (tsp01_CommErr_Enum) eo03NiReceive (&pConnInfo->NI, &pResPacket,
                                                        (tsp00_Uint &)ulResDataLen, errText );
     break;

    case PROT_LOCAL_CONTROL_EO003 :
    case PROT_VMTDISPATCH_EO003 :
        ulCommState = (tsp01_CommErr_Enum) pConnInfo->commObj->receive ((void**)(&pResPacket), 
                                                                        ulResDataLen, 
                                                                        errText);

        break;
    default :
      MSGCD (( ERR_WRONG_PROTOCOL, pConnInfo->ulProtocol ));
      ABORT();
    }

  if ( ulCommState == commErrOk_esp01 )
    {
    *ppResData              = (PVOID)pResPacket->pDataPart;
    pConnInfo->ulConnStatus = CON_RECEIVED;
    }
  else
    ulResDataLen = 0;

  if ( DbgLevel )
    {
    if ( ulCommState == commErrOk_esp01 )
      {
      MSG2 (( INFO_CL_COMM_RECEIVED, pConnInfo->ulTraceConID, pResPacket->pDataPart, ulResDataLen  ));
      }
    else
     {
      MSG1 (( INFO_CL_COMM_RECEIVE_ERR, pConnInfo->ulTraceConID, ulCommState, errText.asCharp () ));
      }
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static tsp01_CommErr_Enum sql03u_release (ULONG            ulClientRef,
                                          tsp00_ErrTextc & errText )
  {
  #undef  MF__
  #define MF__ MOD__"sql03u_release"
  PUSER_CONNECT_INFO_REC          pConnInfo;
  tsp01_CommErr_Enum              ulCommState = commErrOk_esp01;
  LONG                            rc;
  ULONG                           ulTmp;
  TOSCM_COMM_CLASS                *commObj;
  ULONG                           ulTraceConIDTmp;

  DBGIN;

  // --- get the pointer to the internal communication structure
  if ( !sql09c_get_handle_data  ( pHandles, ulClientRef,
                                  (PVOID*)&pConnInfo,
                                  &ulTmp, &ulTmp ))
    {
    MSG1 (( INFO_CL_COMM_RELEASE_ERR, pConnInfo->ulTraceConID, commErrNotOk_esp01, ERRMSG_COM_ILL_REFERENCE ));
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_ILL_REFERENCE, 0 );
    return ( commErrNotOk_esp01 );
    }

  if ( pConnInfo->ulConnStatus == CON_INITIALIZED )
    {
    MSG1 (( INFO_CL_COMM_RELEASE_ERR, pConnInfo->ulTraceConID, commErrNotOk_esp01, ERRMSG_COM_WRONG_CONN_STATE ));
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_WRONG_CONN_STATE, 0 );
    return ( commErrNotOk_esp01 );
    }

  pConnInfo->ulConnStatus = CON_RELEASED;

  MSG1 (( INFO_CL_COMM_RELEASING_CONN, pConnInfo->ulTraceConID, GetCurrentThreadId() ));

  switch ( pConnInfo->ulProtocol )
    {
    case PROT_SHM_EO003 :
	  /* The 'cancel' is called in order to wake the event task ('cancel' calls 'vcontinue') */
      if (srvEvent_esp01 == pConnInfo->Shm.ulServiceType)
	  {
		ulCommState = (tsp01_CommErr_Enum) sql33c_cancel_dump (&pConnInfo->Shm, SQL_RTE_CANCEL_EO003,
                                                               errText );
	  }
	  if (commErrOk_esp01 == ulCommState)
	  {
		ulCommState = (tsp01_CommErr_Enum)sql33c_release (&pConnInfo->Shm, errText);
	  }
      break;

    case PROT_SOCKET_EO003 :
      ulCommState = (tsp01_CommErr_Enum) sql23c_release (&pConnInfo->TCPIP, errText);
      break;

    case PROT_LOCAL_CONTROL_EO003:
    case PROT_VMTDISPATCH_EO003 :
      commObj = pConnInfo->commObj;
      ulCommState = (tsp01_CommErr_Enum) commObj->release ();
      break;

    case PROT_SSL_EO003 :
    case PROT_NISSL_EO003 :
    case PROT_NI_EO003 :
      ulCommState = (tsp01_CommErr_Enum) eo03NiRelease (&pConnInfo->NI, errText);
      break;

    default :
      MSGCD (( ERR_WRONG_PROTOCOL, pConnInfo->ulProtocol ));
      ABORT();

    }
  sql09c_free_handle  ( pHandles, ulClientRef );

  ulTraceConIDTmp = pConnInfo->ulTraceConID; // - we may need this for tracing

  rc = FREE_MEM ( pConnInfo );

  if ( rc != NO_ERROR )
    {
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_CANT_FREE_MEM, rc );
    ulCommState = commErrNotOk_esp01;
    }

  if ( DbgLevel )
    {
    if ( ulCommState == commErrOk_esp01 )
      {
      MSG1 (( INFO_CL_COMM_CONN_RELEASED, ulTraceConIDTmp ));
      }
    else
      {
      MSG1 (( INFO_CL_COMM_RELEASE_ERR, ulTraceConIDTmp, ulCommState, errText.asCharp () ));
      }
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static tsp01_CommErr_Enum sql03u_cancel (ULONG            ulClientRef,
                                         char            *szSaprouterString,
                                         tsp00_ErrTextc & errText )
  {
  #undef  MF__
  #define MF__ MOD__"sql03u_cancel"
  PUSER_CONNECT_INFO_REC          pConnInfo;
  USER_CONNECT_INFO_REC           ConnInfo;
  tsp01_CommErr_Enum              ulCommState = commErrOk_esp01;
  ULONG                           ulTmp;

  DBGIN;

  // --- get the pointer to the internal communication structure
  if (( !sql09c_lock_handle ( pHandles, ulClientRef ) ) ||
      ( !sql09c_get_handle_data  ( pHandles, ulClientRef, (PVOID*)&pConnInfo, &ulTmp, &ulTmp )))
    {
    sql09c_unlock_handle ( pHandles, ulClientRef );
    MSG1 (( INFO_CL_CANCEL_ERR, pConnInfo->ulTraceConID, commErrNotOk_esp01, ERRMSG_COM_ILL_REFERENCE ));
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_ILL_REFERENCE, 0 );
    return ( commErrNotOk_esp01 );
    }

  memcpy ( &ConnInfo, pConnInfo, sizeof( ConnInfo ));

  if ( ConnInfo.ulConnStatus != CON_REQUESTED )
    {
    sql09c_unlock_handle ( pHandles, ulClientRef );
    MSG1 (( INFO_CL_CANCEL_ERR, pConnInfo->ulTraceConID, commErrNotOk_esp01, ERRMSG_COM_WRONG_CONN_STATE ));
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_WRONG_CONN_STATE, 0 );
    return ( commErrNotOk_esp01 );
    }

  sql09c_unlock_handle ( pHandles, ulClientRef );

  MSG2 (( INFO_CL_CANCEL_REQUEST, pConnInfo->ulTraceConID, GetCurrentThreadId(),
          ulClientRef, sql03u_get_server_ref(pConnInfo) ));

  switch ( ConnInfo.ulProtocol )
    {
    case PROT_SHM_EO003 :
      ulCommState = (tsp01_CommErr_Enum) sql33c_cancel_dump (&ConnInfo.Shm, SQL_RTE_CANCEL_EO003,
                                                             errText );
      break;

    case PROT_SOCKET_EO003 :
      ulCommState = (tsp01_CommErr_Enum) sql23c_cancel_dump (&ConnInfo.TCPIP, SQL_RTE_CANCEL_EO003,
                                                             errText );
      break;

    case PROT_SSL_EO003 :
    case PROT_NISSL_EO003 :
    case PROT_NI_EO003 :
      ulCommState = (tsp01_CommErr_Enum) eo03NiSqlCancelDump (&ConnInfo.NI, szSaprouterString,
                                                              SQL_RTE_CANCEL_EO003, errText );
      break;

    case PROT_LOCAL_CONTROL_EO003 :
    case PROT_VMTDISPATCH_EO003 :
       ulCommState = (tsp01_CommErr_Enum) ConnInfo.commObj->cancel (errText);
       break;

    default :
      MSGCD (( ERR_WRONG_PROTOCOL, ConnInfo.ulProtocol ));
      ABORT();
    }

  pConnInfo->ulConnStatus = CON_CANCELLED;

  if ( DbgLevel )
    {
    if ( ulCommState == commErrOk_esp01 )
      {
      MSG2 (( INFO_CL_CANCEL_REQUESTED, pConnInfo->ulTraceConID ));
      }
    else
      {
      MSG1 (( INFO_CL_CANCEL_ERR, pConnInfo->ulTraceConID, ulCommState, errText.asCharp() ));
      }
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static tsp01_CommErr_Enum sql03u_dump (ULONG            ulClientRef,
                                       tsp00_ErrTextc & errText )
  {
  #undef  MF__
  #define MF__ MOD__"sql03u_dump"
  PUSER_CONNECT_INFO_REC          pConnInfo;
  tsp01_CommErr_Enum              ulCommState = commErrOk_esp01;
  ULONG                           ulTmp;

  DBGIN;

  // --- get the pointer to the internal communication structure
  if ( !sql09c_get_handle_data  ( pHandles, ulClientRef,
                                  (PVOID*)&pConnInfo,
                                  &ulTmp, &ulTmp ))
    {
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_ILL_REFERENCE, 0 );
    return ( commErrNotOk_esp01 );
    }

  if ( pConnInfo->ulConnStatus <= CON_INITIALIZED )
    {
    eo46BuildErrorStringRC ( errText, ERRMSG_COM_WRONG_CONN_STATE, 0 );
    return ( commErrNotOk_esp01 );
    }

  MSG2 (( INFO_CL_DUMP_REQUEST, pConnInfo->ulTraceConID, GetCurrentThreadId(),
           ulClientRef, sql03u_get_server_ref(pConnInfo) ));

  switch ( pConnInfo->ulProtocol )
    {
    case PROT_SHM_EO003 :
      ulCommState = (tsp01_CommErr_Enum) sql33c_cancel_dump (&pConnInfo->Shm, SQL_RTE_DUMP_EO003,
                                                             errText );
      break;

    case PROT_SOCKET_EO003 :
      ulCommState = (tsp01_CommErr_Enum) sql23c_cancel_dump (&pConnInfo->TCPIP, SQL_RTE_DUMP_EO003,
                                                             errText );
      break;

    case PROT_SSL_EO003 :
    case PROT_NISSL_EO003 :
    case PROT_NI_EO003 :
      ulCommState = (tsp01_CommErr_Enum) eo03NiSqlCancelDump (&pConnInfo->NI, NULL, SQL_RTE_DUMP_EO003, 
                                                              errText );
      break;

    default :
      MSGCD (( ERR_WRONG_PROTOCOL, pConnInfo->ulProtocol ));
    }

  DBGOUT;
  return ( ulCommState );
  }

/*------------------------------*/

static LONG sql03u_get_protocol ( PSZ                       pszServerNode,
                                  PSZ                       pszServerDB,
                                  ULONG                     ulServiceType,
                                  PULONG                    pulProtocol,
                                  SAPDB_Bool &              ignoreHostnameInServerCert,
                                  TOSCM_COMMSTRUCTOR_FT * & pfCommstructor,
                                  tsp00_ErrTextc &          errText )

{
#undef  MF__
#define MF__ MOD__"sql03u_get_protocol"
    ULONG                   ulServerNodeLen;
    SQL_NODEIDC             szNodeName;
    LONG                    rc;
    int                     IsSaprouterString = FALSE ;
    SAPDB_Bool              isNISSL = false;
    SAPDB_Bool              isSSL = false;
    tsp01_CommErr_Enum      ulCommState = commErrOk_esp01;
    SAPDB_Bool              optimizeForLocalConnects = false;



    DBGIN;

    ulServerNodeLen = (ULONG)strlen ( pszServerNode );
    /* --- temporary hack to allow proc server communication --- */
    if ((sql03c_constructorPointer != NULL) && (pszServerDB [0] == '$') && (pszServerDB [1] == ':')) {
        *pulProtocol = PROT_VMTDISPATCH_EO003;
        pfCommstructor = sql03c_constructorPointer;
        DBGOUT;
        return ( NO_ERROR );
    }

    if ( ulServerNodeLen != 0 )
    {
        if ( szLocalNodeName[0] == '\0' )
        {
            rc = sql43_get_my_host_name ( szNodeName, sizeof(szNodeName) );

            if (( rc == NO_ERROR ) && ( ulTCPIP_InitResult == TCPIP_INIT_OK ))
            {
                rc = sql43_get_official_node_name ( szNodeName, szLocalNodeName,
                    sizeof(szLocalNodeName));

                if ( rc != NO_ERROR )
                    strcpy ( szLocalNodeName, szNodeName );
            }
            else
                strcpy ( szLocalNodeName, szNodeName );
            DBG3 (( MF__, "Local node: '%s'", szLocalNodeName ));
        }


        ulCommState = RTESec_IsSSLConnection (pszServerNode, 
                                              isSSL, 
                                              isNISSL, 
                                              ignoreHostnameInServerCert, 
                                              errText);

        if (commErrOk_esp01 !=ulCommState)
        {
            DBGOUT;
            return (-1);
        }

        if (!isSSL)
        {
            IsSaprouterString = eo40NiIsSaprouterSring ( pszServerNode ) ;
        }

        if ( !IsSaprouterString && !isNISSL)
        {    
            if (strlen (pszServerNode) >= sizeof (SQL_NODEIDC))
            {
                eo46BuildErrorStringRC ( errText, ERRMSG_COM_SERVER_NODE_NAME_TOO_LONG, 0 );
                DBGOUT;
                return (-1);
            }


            RTE_Nodename             tmpNodeName;
           
            strcpy (tmpNodeName, pszServerNode);
            if (NULL != strchr(tmpNodeName, ':'))
            {
                //port is removed from tmpNodeName 
                memset (strchr(tmpNodeName, ':'), '\0', 1);
            }


            if (!sql43_is_inet_addr(tmpNodeName) && 
                strnicmp(tmpNodeName, "localhost", strlen("localhost")) != 0)
            {
                rc = sql43_get_official_node_name (tmpNodeName, szNodeName, sizeof(szNodeName));

                if ( rc != NO_ERROR )
                    strcpy (szNodeName, tmpNodeName);

                //Optimization if given server node is the local node
                if (optimizeForLocalConnects = (stricmp (szNodeName, szLocalNodeName) == 0))
                {
                    isNISSL = false;
                    isSSL = false;
                }
            }
        }
    }


    if ((IsSaprouterString && !eo40NiIsInitialized ()) || ( isSSL && !RTESec_SAPSSLIsInitialized ()))
    {
        tsp00_Pathc         niTracefileName;

        niTracefileName.Init ();
        if (!eo40NiIsInitialized ())
        {
            eo03NiBuildTracefileName (niTracefileName);
        }

        if (IsSaprouterString) // => !eo40NiIsInitialized ()
        {
            if (commErrOk_esp01 != eo40NiInit(niTracefileName, 0, errText))
            {
                tsp00_ErrTextc  tmpErrText;
                eo40NiFinish (tmpErrText);
                return ( -1 );
            }
        }
        else
        {   // if eo40NiIsInitialized () is true the niTracefileName was not build above. But this
            // does not matter because in this case the trace name is not needed
            if (commErrOk_esp01 != RTESec_SSLInit(niTracefileName, errText))
            {
                return ( -1 );
            }
        }
    }

    if (isNISSL)
    {
        *pulProtocol = PROT_NISSL_EO003;
        DBG3 (( MF__, "Secure NI SAP Network Interface connection (SSL)", 0 ));
    }
    else if (isSSL) 
    {/* it is a SSL connection without SAPRouter String */
        *pulProtocol = PROT_SSL_EO003;
        DBG3 (( MF__, "Secure remote connection (SSL)", 0 ));
    }
    else if ( IsSaprouterString )
    {
        *pulProtocol = PROT_NI_EO003;
        DBG3 (( MF__, "NI SAP Network Interface connection", 0 ));
    }
    else if (ulServerNodeLen != 0 && !optimizeForLocalConnects)
    {
        *pulProtocol = PROT_SOCKET_EO003;
        DBG3 (( MF__, "Remote connection", 0 ));
    }    
    else
    {
        *pulProtocol = PROT_SHM_EO003;
        DBG3 (( MF__, "Local connection", 0 ));
    }

    if ((*pulProtocol == PROT_SOCKET_EO003 ) && ( ulTCPIP_InitResult != TCPIP_INIT_OK ))
    {
        if ( ulTCPIP_InitResult == TCPIP_INIT_WRONG_VERSION )
        {
            MSGD (( ERR_WRONG_TCPIP_VERSION ));
            strcpy ( errText.asCharp (), ERRMSG_WRONG_TCPIP_VERSION );
        }
        else
        {
            MSGD (( ERR_NO_TCPIP_INSTALLED ));
            strcpy ( errText.asCharp (), ERRMSG_NO_TCPIP_INSTALLED );
        }
        return ( -1 );
    }

    if ((*pulProtocol == PROT_SHM_EO003) && (ulServiceType == srvControl_esp01))
    {
        // a local connection to control server is handled
        // by loading the dll
        *pulProtocol = PROT_LOCAL_CONTROL_EO003;
    }

    DBGOUT;
    return ( NO_ERROR );
}

/*------------------------------*/

static VOID _System sql03u_exithandler ( ULONG   ulTermCode )
  {
  #undef  MF__
  #define MF__ MOD__"sql03u_exithandler"


  DBGIN;

  DBG3 (( MF__, "ulTermCode = %d", ulTermCode ));

  sql03u_finish ();

  sql02u_next_exit_func ();

  DBGOUT;

  return;
  }

/*------------------------------*/

//
// for tracing purposes only!  
//
ULONG sql03u_get_server_ref  ( PUSER_CONNECT_INFO_REC  pConnInfo )
  {
  ULONG ulServerRef = (ULONG)-1;

  switch ( pConnInfo->ulProtocol )
    {
    case PROT_SHM_EO003 :
      ulServerRef = pConnInfo->Shm.ulServerRef;
      break;

    case PROT_SOCKET_EO003 :
      ulServerRef = pConnInfo->TCPIP.ulServerRef;
      break;

    case PROT_NISSL_EO003:
    case PROT_SSL_EO003:
    case PROT_NI_EO003 :
      ulServerRef = pConnInfo->NI.ulServerRef;
      break;

    default :
      break;
    }

  return ulServerRef;
  }

/*------------------------------*/

 static BOOL WINAPI sql03u_catch_cancel_sig ( DWORD dwCtrlType )
   {
   #undef  MF__
   #define MF__ MOD__"sql03u_catch_cancel_sig"
   tsp00_ErrTextc          pErrText;

   DBGPAS;

   switch ( dwCtrlType )
     {
     case CTRL_C_EVENT:
       if ( ulCurrClientRef != (ULONG)UNDEF )
         {
         sql03u_cancel ( ulCurrClientRef, NULL, pErrText );
         ulCurrClientRef = (ULONG)UNDEF;
         }
       break;
     }

   return (FALSE);
   }

/*
 * =============================== END ========================================
 */
