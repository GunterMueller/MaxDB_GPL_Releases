/*!---------------------------------------------------------------------
  @file           ven03.c
  @author         JoergM
  @brief          communication / USER

\if EMIT_LICENCE
\endif
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

//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
---------------------------------------------------------------------*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <ctype.h>

#include "gsp01.h"
#include "gsp09.h"
#include "geo003.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "geo03.h"
#include "heo03.h"
#include "geo40.h"
#include "geo401.h"
#include "heo46.h"
#include "geo001.h"
#include "geo57.h"
#include "gen003.h"
#include "gen23.h"
#include "gen33.h"
#include "gen41.h"
#include "gen42.h"
#include "gen43.h"
#ifdef  DEBUG_RTE
#include "gen46.h"
#endif
#include "heo07.h"
#include "RunTime/Security/RTESec_SAPSSL.h"
#include "RunTime/Security/RTESec_SSL.h"
#include "hen40.h"
#include "RunTime/System/RTESys_MicroTime.h"
#include "heo420.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MOD__ "ven03.c:"

#if defined (STATIC_CONNECTIONS)
#define VALID_REF_INL(ref)      (((ref) > 0) && ((ref) <= MAXCONNECTIONS))
#define CONNECTION_AT_INL(ref)  (&sql03_connection [ref])
#define CONNECTION_ARRAY        sql03_connection
#define MAXCONNECTIONS_INL      MAXCONNECTIONS
#else
#define VALID_REF_INL(ref)          \
    (((ref) > 0) && ((ref) <= sql03_connect_pool.connectionCount))
#define CONNECTION_AT_INL(ref)  (&sql03_connect_pool.connection [ref])
#define CONNECTION_ARRAY        sql03_connect_pool.connection
#define MAXCONNECTIONS_INL      (sql03_connect_pool.connectionCount)
#endif

#define SQL_PACKET_ALIGNMENT 8
#define CHECK_PID_EN03 1

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define  ALIGN_BUFFER(_val,_alignment)                                       \
           ((((_val) - 1) / (_alignment) + 1 ) * (_alignment))

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

typedef struct ten03_connect_pool {
    tsp00_Bool          initialized;
    int                 connectionCount;
    connection_info    *connection;
    tsp00_Bool          isMultiThreaded;
    teo07_MutexFunction *createMutex;
    teo07_MutexFunction *destroyMutex;
    teo07_MutexFunction *lockMutex;
    teo07_MutexFunction *unlockMutex;
    teo07_Mutex          mutex;
} ten03_connect_pool;


/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

/* exported functions */
global  void    sql03_split_dbname ( char                   *serverdb,
                                     char                   *servernode );
global void sql03_init_multi_threaded ( teo07_MutexFunction *createMutex,
                                        teo07_MutexFunction *destroyMutex,
                                        teo07_MutexFunction *lockMutex,
                                        teo07_MutexFunction *unlockMutex);

static void           sql03_init ( void );
static void           sql03_finish ( void );

void                (*sql01_init_com)()   = sql03_init;
void                (*sql01_finish_com)() = sql03_finish;

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

static struct sigaction sql03_oldSIGHUP  = { 0 };
static struct sigaction sql03_oldSIGINT  = { 0 };
static struct sigaction sql03_oldSIGPIPE = { 0 };
static struct sigaction sql03_oldSIGTERM = { 0 };
static struct sigaction sql03_oldSIGALRM = { 0 };

static  unsigned                        sql03_remainingAlarmSeconds = 0;
static  time_t                          sql03_oldclock;
static  pid_t                           sql03_pid;
#if defined (STATIC_CONNECTIONS)
static  connection_info                 sql03_connection [ MAXCONNECTIONS ];
#endif
static  connection_info                *sql03_cip;
static  tsp00_Int4                        sql03_receiving;
static  char                            sql03_nodeseparatorlist [] =
    NODESEPARATORLIST;

static ten03_connect_pool sql03_connect_pool = {
            FALSE,      /* initialized          */
            0,          /* connectionCount      */
            NULL,       /* connection           */
            FALSE       /* isMultiThreaded      */
        };

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*------------------------------*/

static  int            sql03_connect (char              *servernode,
                                      tsp00_DbNamec     serverdb,
                                      int               service,
                                      int               packet_cnt,
                                      tsp00_Int4        *reference,
                                      tsp00_Int4        *max_reply_packet_size,
                                      tsp00_Int4        *sql_packet_size,
                                      void              **sql_packet_list,
                                      char              *dbroot,
                                      char              *serverpgm,
                                      connection_VMT    *vmt,
                                      tsp00_ErrTextc     pErrText );

static  int            sql03_release ( tsp00_Int4         reference,
                                       tsp00_ErrTextc   errtext );

static  int            sql03_request ( tsp00_Int4         reference,
                                       void              *packet,
                                       tsp00_Int4         length,
                                       tsp00_Uint1        RTEFlags,
                                       tsp00_ErrTextc     errtext );

static  int            sql03_replyavailable ( tsp00_Int4         reference,
        tsp00_ErrTextc   errtext );

static  int            sql03_receive ( tsp00_Int4     reference,
                                       void       **packetp,
                                       tsp00_Int4    *lengthp,
                                       tsp00_ErrTextc errtext );

static  int            sql03_cancel ( tsp00_Int4      reference,
                                      tsp00_ErrTextc errtext );

static  int            sql03_dump ( connection_info    *cip,
                                    tsp00_ErrTextc      errtext );

static  int            sql03_get_protocol ( char            *Node,
                                            connection_info *cip,
                                            SAPDB_Bool *     ignoreHostnameInServerCert,
                                            tsp00_ErrTextc   ErrTExtC);

static  tsp00_Int4       sql03_find_free_index ( void );
static  char          *sql03_statename ( connection_info *cip);
static  void           sql03_set_signals ( void );
static  void           sql03_reset_signals ( void );
static  void           sql03_set_alarm ( unsigned interval);
static  void           sql03_reset_alarm ( void );
static  SIGNALFUNCTYPE sql03_catch_signal ( int sig );
static  void           sql03_init_connections ( ten03_connect_pool* connectPool,
        int firstIndex,
        int count);
static  void           sql03_finish_connections ( void );
static  int            sql03_init_connect_pool (ten03_connect_pool* connectPool);
static  tsp00_Int4       sql03_alloc_connect ( void );
static  int            sql03_free_connect (int ref, char** msg);

static void en03InitConnParam (char                      *Node,
                               teo003_ConnectParamRecord *pConnParam,
                               connection_info           *cip );


static int en03GetAndCheckConnectionInfo( tsp00_Int4          reference,
        tsp00_Int4          CheckPid,
        connection_info  ** ConnectionInfo,
        char              * CallingFktName,
        tsp00_ErrTextc      pErrText );
/* PTS 1109405 */
static  tsp01_CommErr_Enum en03_aPing( char *         pszServerNode,
                                       tsp00_Versionc szServerVersion,
                                       tsp00_ErrTextc errText );

extern int sqlIsRunTimeComponent();
/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externC tsp01_CommErr_Enum  SqlPingConnect ( char*                        szServerNode,    /* '\0' terminated node or saprouterstring */
                                             char*                        szServerDB,      /* '\0' terminated dbname                  */
                                             tsp00_Int4 VAR_VALUE_REF     reference,      /* connection reference                    */
                                             tsp00_ErrTextc               pErrText )       /* '\0' terminated error string            */
  {
  #undef  MF__
  #define MF__ MOD__"SqlPingConnect"
  tsp01_CommErr_Enum        commError;
  int                       packetSize;
  void*                     packetList;
  tsp00_Int4                max_reply_packet_size;

  DBGIN;

  if ( !sql03_connect_pool.initialized )
      sql03_init();

  if (strlen (szServerDB) >= sizeof (SQL_DBNAMEC))
  {
      eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SERVERDB_NAME_TOO_LONG, 0 );
      DBGOUT;
      return commErrNotOk_esp01;
  }

  commError = (tsp01_CommErr_Enum)sql03_connect ( szServerNode,
                                                  szServerDB,
                                                  (int) srvPing_esp01,
                                                  1,
                                                  reference,
                                                  &max_reply_packet_size,
                                                  &packetSize,
                                                  &packetList,
                                                  NULL,
                                                  NULL,
                                                  NULL,
                                                  pErrText );


  DBGOUT
  return (commError) ;
  }


/*------------------------------*/


externC tsp01_CommErr_Enum SqlDBPing ( tsp00_Int4                         reference,
                                       tsp00_Uint4     VAR_VALUE_REF      sendBufferSize,
                                       tsp00_Uint1     VAR_VALUE_REF      hops,
                                       tsp00_DbNamec   VAR_ARRAY_REF      szServer,
                                       tsp00_Versionc  VAR_ARRAY_REF      szServerVersion,
                                       tsp00_ErrTextc                     pErrText )
{
    tsp01_CommErr_Enum              commState;
    char*                           server;
    char*                           serverVersion;
    connection_info*                cip;
    tsp00_Uint4                     sendLen;

    /* --- get the pointer to the internal communication structure */
    commState = (tsp01_CommErr_Enum)en03GetAndCheckConnectionInfo( reference, CHECK_PID_EN03, &cip, "SqlDBPing", pErrText );

    if ( commState != commErrOk_esp01 )
        return commErrNotOk_esp01;

    if ( *sendBufferSize > cip->ci_max_request_size ) 
        *sendBufferSize = cip->ci_max_request_size;

    *sendBufferSize = ALIGN_BUFFER( *sendBufferSize, SQL_PACKET_ALIGNMENT );

    sendLen =  eo420FillPingPacket ( (char*)cip->ci_packetData[0],
                                     *sendBufferSize, *hops, "PingClient", 0, "!!! SEND DATA !!!" );

    if ( 0 != sendLen )
    {
        commState = (tsp01_CommErr_Enum)sql03_request ( reference, cip->ci_packetData[0], sendLen, RSQL_NORMAL_EO003, pErrText );

        if ( commState == commErrOk_esp01 )
        {
            char*              pResPacket;
            tsp00_Int4         resDataLen;

            commState = (tsp01_CommErr_Enum)sql03_receive ( reference, (void**)&pResPacket, &resDataLen, pErrText );

            if ( commState == commErrOk_esp01 )
            {
                if ( eo420ExtractPingPacket( pResPacket, resDataLen, hops, &server, &serverVersion ) )
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
                    eo46BuildErrorStringRC ( pErrText, ERRMSG_PROTOCOL_ERROR, 0 );
                    commState = commErrNotOk_esp01;
                }
            }
        }
    }
    else
    {
        MSGD (( ERR_PROTOCOL_ERROR, "PING REPLY" ));
        eo46BuildErrorStringRC ( pErrText, ERRMSG_PROTOCOL_ERROR, 0 );
        commState = commErrNotOk_esp01;
    }

    return commState;
}

/*------------------------------*/

externC void SqlPingRelease (tsp00_Int4 reference)
  {
  #undef  MF__
  #define MF__ MOD__"SqlPingRelease"
  tsp00_ErrTextc  pErrText;

  sql03_release ( reference, pErrText );

  return;
  }

  /*------------------------------*/

externC SAPDB_Bool  SqlSSLAvailable     (tsp00_ErrTextc       errText)
{
    tsp00_Pathc         niTracefileName;

    if (RTESec_SAPSSLIsInitialized ())
    {
        return true;
    }

    niTracefileName[0] = '\0';
    if (!eo40NiIsInitialized ())
    {
        eo03NiBuildTracefileName (niTracefileName);
    }

    /* if eo40NiIsInitialized () is true the niTracefileName was not build above. But this
       does not matter because in this case the trace name is not needed */
    if (commErrOk_esp01 != RTESec_SSLInit(niTracefileName, errText))
    {
        return false;
    }

    return true;
}
  /*------------------------------*/

externC SAPDB_Bool  SqlBasicAuthenticationOnly ( tsp00_Int4                   reference,
                                                 tsp00_ErrTextc VAR_ARRAY_REF errText )
{
    int              rc;
    connection_info *cip;

    rc =  en03GetAndCheckConnectionInfo( reference, CHECK_PID_EN03, &cip, "SqlBasicAuthenticationOnly", errText );

    if ( rc != commErrOk_esp01 )
        return(rc);

    return (( '\0' == cip->ci_authentication_allow[0] ) ||
            (   0  == strcmp( cip->ci_authentication_allow, "BASIC" )));
}

/*===============================================================================*/
/* This function is used for calls from C / C++ functions only                   */
/*===============================================================================*/

/* PTS 1105295 */

externC tsp01_CommErr_Enum
SqlAPing ( char                  *pszServerNode,     /* '\0' terminated node or saprouterstring */
           SAPDB_Bool             captureServerCert,
           tsp00_Versionc         szServerVersion,   /* '\0' terminated version from answering server */
           SAPDB_Byte *           pCertificateBuffer,
           SAPDB_UInt4            lenCertificateBuffer,
           tsp00_ErrTextc         pErrText)          /* '\0' terminated error string   */
{
  #undef  MF__
  #define MF__ MOD__"SqlAPing"

    tsp01_CommErr_Enum        commError;

    if ( pszServerNode == NULL || *pszServerNode == '\0' )
    {
        commError = commErrNotOk_esp01;
        strcpy ( pErrText, "missing hostname" );
    }
    else
    {
        SAPDB_Bool              isSaprouterString = false;
        SAPDB_Bool              isNISSL = false;
        SAPDB_Bool              isSSL = false;
        SAPDB_Bool              ignoreHostnameInServerCert = false;

        commError = RTESec_IsSSLConnection (pszServerNode, 
                                            &isSSL, 
                                            &isNISSL, 
                                            &ignoreHostnameInServerCert, 
                                            pErrText);

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

                niTracefileName[0] = '\0';
                if (!eo40NiIsInitialized ())
                {
                    eo03NiBuildTracefileName (niTracefileName);
                }

                if (isSaprouterString || captureServerCert) /* => !eo40NiIsInitialized () */
                {
                    commError = (tsp01_CommErr_Enum)eo40NiInit(niTracefileName, 0, pErrText);
                    if (commErrOk_esp01 != commError)
                    {
                        tsp00_ErrTextc  tmpErrText;
                        eo40NiFinish (tmpErrText);
                    }
                }
                else
                {   /* if eo40NiIsInitialized () is true the niTracefileName was not build above. But this
                       does not matter because in this case the trace name is not needed */
                    commError = RTESec_SSLInit(niTracefileName, pErrText);
                }
            }


            if (commErrOk_esp01 == commError)
            {
                if (isSSL || isSaprouterString)
                {
                    commError = (tsp01_CommErr_Enum) eo03NiXserverPing(pszServerNode, 
                        szServerVersion,
                        pCertificateBuffer,
                        lenCertificateBuffer,
                        isSSL, 
                        isNISSL, 
                        ignoreHostnameInServerCert,
                        captureServerCert,
                        pErrText) ;
                }
                else
                {
                    if (strlen (pszServerNode) >= sizeof (SQL_NODEIDC))
                    {
                        eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SERVER_NODE_NAME_TOO_LONG, 0 );
                        commError = commErrNotOk_esp01;
                    }
                    else
                    {
                        commError = en42pingRequest (pszServerNode,
                            szServerVersion,
                            pCertificateBuffer,
                            lenCertificateBuffer,
                            captureServerCert,
                            pErrText);
                    }
                }
            }

            if (isSSL || isSaprouterString || captureServerCert)
            {
                eo03Finish ();
            }
        }
    }

    DBGOUT;
    return ( commError );
}

/*
 * ===========================================================================
 */

externC tsp01_CommErr_Enum
SqlAConnect ( char                  *szServerNode,      /* '\0' terminated node or saprouterstring */
              tsp00_DbNamec          szServerDB,        /* '\0' terminated dbname                  */
              tsp01_Service_Enum     service,
              tsp00_Int4             packet_cnt,
              tsp00_Int4             *reference,
              tsp00_Int4             *sql_packet_size,
              void                  **sql_packet_list,  /* tsp00_Array<tsp1_packet_ptr, ...> */
              tsp00_ErrTextc         szErrText )        /* '\0' terminated error string   */
{
  #undef  MF__
  #define MF__ MOD__"SqlAConnect"
    tsp01_CommErr_Enum        commError;
    tsp00_Int4                max_reply_packet_size;

    DBGIN;

    DBG3 (( MF__, "pszServerNode '%s'", szServerNode ));
    DBG3 (( MF__, "pszServerDB   '%s'", szServerDB ));

    if (strlen (szServerDB) >= sizeof (SQL_DBNAMEC))
    {
        eo46BuildErrorStringRC (szErrText, ERRMSG_COM_SERVERDB_NAME_TOO_LONG, 0);
        commError = commErrNotOk_esp01;
    }
    else
    {
        commError = (tsp01_CommErr_Enum) sql03_connect ( szServerNode,
            szServerDB,
            (int) service,
            (int) packet_cnt,
            reference,
            &max_reply_packet_size,
            sql_packet_size,
            sql_packet_list,
            NULL, NULL, NULL, szErrText);
    }

    DBGOUT
    return (commError);
}

/*
 * ===========================================================================
 */

externC tsp01_CommErr_Enum
SqlDBConnect ( char                  *szServerNode,      /* '\0' terminated node or saprouterstring */
               tsp00_DbNamec          szServerDB,        /* '\0' terminated dbname                  */
               tsp01_Service_Enum     service,
               tsp00_Int4             packet_cnt,
               tsp00_Int4             *reference,
               tsp00_Int4             *max_reply_packet_size,
               tsp00_Int4             *sql_packet_size,
               void                   **sql_packet_list,  /* tsp00_Array<tsp1_packet_ptr, ...> */
               tsp00_ErrTextc         szErrText )        /* '\0' terminated error string   */
{
  #undef  MF__
  #define MF__ MOD__"SqlDBConnect"
    tsp01_CommErr_Enum        commError;

    DBGIN;

    DBG3 (( MF__, "pszServerNode '%s'", szServerNode ));
    DBG3 (( MF__, "pszServerDB   '%s'", szServerDB ));

    if (strlen (szServerDB) >= sizeof (SQL_DBNAMEC))
    {
        eo46BuildErrorStringRC (szErrText, ERRMSG_COM_SERVERDB_NAME_TOO_LONG, 0);
        commError = commErrNotOk_esp01;
    }
    else
    {
        commError = (tsp01_CommErr_Enum) sql03_connect ( szServerNode,
            szServerDB,
            (int) service,
            (int) packet_cnt,
            reference,
            max_reply_packet_size,
            sql_packet_size,
            sql_packet_list,
            NULL, NULL, NULL, szErrText);
    }

    DBGOUT
    return (commError);
}

/*
 * ===========================================================================
 */

/*ARGSUSED*/ /* pid not used */

    #undef MF__
    #define MF__ MOD__"sqlaconnect"
void sqlaconnect ( tsp00_TaskId           pid,
                   tsp00_NodeId           servernode,       /* tsp00_Array<char, mxsp_c64> */
                   tsp00_DbName           serverdb,         /* tsp00_Array<char, mxsp_c18> */
                   tsp01_Service_Enum     service,
                   tsp00_Int4             packet_cnt,
                   tsp00_Int4           * reference,
                   tsp00_Int4           * sql_packet_size,
                   void                ** sql_packet_list,  /* tsp00_Array<tsp1_packet_ptr, ...> */
                   tsp00_ErrText          errtext,          /* tsp00_Array<char, mxsp_c40> */
                   tsp01_CommErr        * returncode)       /* tsp00_Enum<tsp1_comm_error_Enum, ...> */

{
    tsp00_NodeIdc     szServerNode;
    tsp00_DbNamec     szServerDB;
    tsp00_ErrTextc    pErrText;
    tsp00_Int4        max_reply_packet_size;

    eo46PtoC ( szServerNode, servernode,  sizeof(tsp00_NodeId) );
    eo46PtoC ( szServerDB,   serverdb,    sizeof(tsp00_DbName) );

    DBG3 (( MF__,"node '%.10s' db '%.8s' srv %d pktcnt %d \n",
            szServerNode , szServerDB , (int) service , (int) packet_cnt ))

    *returncode = (tsp1_comm_error) sql03_connect ( szServerNode,
                  szServerDB,
                  (int) service,
                  (int) packet_cnt,
                  reference,
                  &max_reply_packet_size,
                  sql_packet_size,
                  sql_packet_list,
                  NULL,
                  NULL,
                  NULL,
                  pErrText);

    if ( *returncode != commErrOk_esp01 )
        eo46CtoP ( errtext, pErrText, sizeof(tsp00_ErrText) );

    DBG3 (( MF__,"rc %d ref %d pktsiz %d \n",
            (int) *returncode , (int) *reference , (int) *sql_packet_size ))
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sqlarelease"
void sqlarelease (tsp00_Int4 reference)
{
    tsp00_ErrTextc   pErrText;
    int rc;

    DBGIN;
    DBG3 (( MF__,"ref %d \n", reference ))

    rc = sql03_release ( reference , pErrText );

    DBG3 (( MF__,"rc %d\n", rc ))
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"sqlDBRequest"
void sqlDBRequest (  tsp00_Int4        reference,
                     void            * sql_packet_addr,
                     tsp00_Int4        sql_packet_length,
                     SAPDB_Bool        commit,
                     tsp00_ErrText     errtext,
                     tsp01_CommErr   * returncode)
{
    tsp00_ErrTextc    pErrText;

    DBG1 (( MF__,"ref %d lgt %d \n", reference , sql_packet_length ))

    *returncode = (tsp1_comm_error) sql03_request ( reference , sql_packet_addr, sql_packet_length , 
                                                    commit ? RSQL_COMMIT_EO003 : RSQL_NORMAL_EO003, 
                                                    pErrText );

    if ( *returncode != commErrOk_esp01 )
        eo46CtoP ( errtext, pErrText, sizeof(tsp00_ErrText) );

    DBG1 (( MF__,"rc %d \n", (int) *returncode ))
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sqlarequest"
void sqlarequest (  tsp00_Int4        reference,
                    void            * sql_packet_addr,
                    tsp00_Int4        sql_packet_length,
                    tsp00_ErrText     errtext,
                    tsp01_CommErr   * returncode)
{
    tsp00_ErrTextc    pErrText;

    DBG1 (( MF__,"ref %d lgt %d \n", reference , sql_packet_length ))

    *returncode = (tsp1_comm_error) sql03_request ( reference , sql_packet_addr,
                                                    sql_packet_length , 
                                                    RSQL_NORMAL_EO003, pErrText );

    if ( *returncode != commErrOk_esp01 )
        eo46CtoP ( errtext, pErrText, sizeof(tsp00_ErrText) );

    DBG1 (( MF__,"rc %d \n", (int) *returncode ))
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sqlareplyavailable"
void sqlareplyavailable (  tsp00_Int4         reference,
                           tsp00_ErrText      errtext,
                           tsp01_CommErr    * returncode)
{
    tsp00_ErrTextc    pErrText;

    DBG1 (( MF__,"ref %d \n", reference ))

    *returncode = (tsp1_comm_error) sql03_replyavailable ( reference , pErrText);

    if ( *returncode != commErrOk_esp01 )
        eo46CtoP ( errtext, pErrText, sizeof(tsp00_ErrText) );

    DBG1 (( MF__,"rc %d \n", (int) *returncode ))
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sqlareceive"
void sqlareceive ( tsp00_Int4        reference,
                   void            ** res_packet_ptr,
                   tsp00_Int4       * res_packet_length,
                   tsp00_ErrText      errtext,
                   tsp01_CommErr    * returncode)
{
    tsp00_ErrTextc    pErrText;

    DBG1 (( MF__,"ref %d \n", reference ))

    *returncode = (tsp1_comm_error) sql03_receive ( reference , res_packet_ptr,
                  res_packet_length , pErrText);

    if ( *returncode != commErrOk_esp01 )
        eo46CtoP ( errtext, pErrText, sizeof(tsp00_ErrText) );

    DBG1 (( MF__,"rc %d lgt %d \n", (int) *returncode , (int) *res_packet_length ))
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sqlacancel"
void sqlacancel ( tsp00_Int4              reference)
{
    tsp00_ErrTextc    pErrText;

    int rc;

    DBG3 (( MF__,"ref %d \n", reference ))

    rc = sql03_cancel ( reference , pErrText );

    DBG3 (( MF__,"rc %d\n", rc ))
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sqladump"
void    sqladump ( )
{
    tsp00_ErrTextc pErrText;

    DBGIN;

    (void) sql03_dump ( sql03_cip, pErrText );

    DBGOUT;
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"SqlDBDump"
tsp01_CommErr_Enum SqlDBDump (
  tsp00_Int4                         reference,         /* connection reference                          */
  tsp00_ErrTextc VAR_ARRAY_REF       errText )          /* \0' terminated error string                   */
{
    connection_info *cip;
    int             rc;

    rc =  en03GetAndCheckConnectionInfo( reference, CHECK_PID_EN03, &cip, "SqlBasicAuthenticationOnly", errText );

    if ( rc != commErrOk_esp01 )
        return rc;

    return sql03_dump ( cip, errText );
}

/*
 * ===========================================================================
 */

/*ARGSUSED*/ /* pid not used */

    #undef MF__
    #define MF__ MOD__"sql03_xconnect"
void    sql03_xconnect (
    char            *servernode,
    tsp00_DbNamec    serverdb,
    char            *dbroot,
    char            *serverpgm,
    tsp00_Int4        *reference,
    tsp00_Int4        *sql_packet_size,
    void           **sql_packet_list,
    connection_VMT  *vmt,
    tsp00_ErrText      errtext,
    tsp1_comm_error     *returncode)
{
    tsp00_ErrTextc    pErrText;
    tsp00_Int4        max_reply_packet_size;

    DBG3 (( MF__,"node '%.10s' db '%.8s' srv %d \n",
            servernode , serverdb , (int) srvControl_esp01 ));

    if (strlen (serverdb) >= sizeof (SQL_DBNAMEC))
    {
        eo46BuildErrorStringRC (pErrText, ERRMSG_COM_SERVERDB_NAME_TOO_LONG, 0);
        eo46CtoP ( errtext, pErrText, sizeof(tsp00_ErrText) );
        *returncode = (tsp1_comm_error) commErrNotOk_esp01;
        return;
    }

    if (strlen (dbroot) >= sizeof (tsp00_Pathc))
    {
        eo46BuildErrorStringRC (pErrText, ERRMSG_COM_SERVERDBROOT_NAME_TOO_LONG, 0);
        eo46CtoP ( errtext, pErrText, sizeof(tsp00_ErrText) );
        *returncode = (tsp1_comm_error) commErrNotOk_esp01;
        return;
    }

    if (strlen (serverpgm) >= sizeof (tsp00_Pathc))
    {
        eo46BuildErrorStringRC (pErrText, ERRMSG_COM_SERVERPGM_NAME_TOO_LONG, 0);
        eo46CtoP ( errtext, pErrText, sizeof(tsp00_ErrText) );
        *returncode = (tsp1_comm_error) commErrNotOk_esp01;
        return;
    }

    *returncode = (tsp1_comm_error) sql03_connect ( servernode,
                  serverdb,
                  srvControl_esp01,
                  1,
                  reference,
                  &max_reply_packet_size,
                  sql_packet_size,
                  sql_packet_list,
                  dbroot,
                  serverpgm,
                  vmt,
                  pErrText);

    if ( *returncode != commErrOk_esp01 )
        eo46CtoP ( errtext, pErrText, sizeof(tsp00_ErrText) );

    DBG3 (( MF__,"rc %d ref %d pktsiz %d \n",
            (int) *returncode , (int) *reference , (int) *sql_packet_size ))
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sql03_init"
static  void    sql03_init ()
{
    int                 idx;
    connection_info     *cip;

    DBGIN;

    /*
     *  Set sql03_cip to the connection data of the currently active connection.
     *  This information is used by sqladump and sql03_catch_signal.
     */
    sql03_cip = 0;
    sql03_init_connect_pool (&sql03_connect_pool);
#if defined (STATIC_CONNECTIONS)
    for ( idx = 0 ; idx < MAXCONNECTIONS ; idx ++ )
    {
        cip = sql03_connection + idx;
        if ( cip->ci_state )
        {
            MSGCD (( -11600,ERR_TYPE,"COMMUNIC",
                     "ABEND: sql03_init: already initialized before" ));
            sqlabort ();
        }
        SAPDB_memset ( cip , 0 , sizeof(connection_info) );
    }
#endif
    DBGOUT;
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sql03_finish"
static  void    sql03_finish ()
{
    int              idx;
    connection_info *cip;
    tsp00_ErrTextc   pErrText;


    DBGIN;

    for ( idx = 0 ; idx < MAXCONNECTIONS_INL ; idx ++ )
    {
        cip = CONNECTION_AT_INL (idx);
        if ( cip->ci_state != CON_UNUSED_EO003 )
        {
            (void) sql03_release ( (tsp00_Int4) (idx + 1) , pErrText );
        }
        SAPDB_memset ( cip , 0 , sizeof(connection_info) );
    }

    eo03Finish ();

    /*
     *  Set sql03_cip to the connection data of the currently active connection.
     *  This information is used by sqladump and sql03_catch_signal.
     */
    sql03_cip = 0;

    DBGOUT;
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

static void en03InitConnParam (char                      *Node,
                               teo003_ConnectParamRecord *pConnParam,
                               connection_info           *cip )
{
#undef  MF__
#define MF__ MOD__"en03InitConnParam"

    pConnParam->ulPacketCnt      = cip->ci_packet_cnt;
    pConnParam->ulPacketSize     = cip->ci_max_request_size;

    pConnParam->ulClientRef      = cip->ci_my_ref;
    pConnParam->ulServiceType    = cip->ci_service;
    pConnParam->pszServerDB      = cip->ci_peer_dbname;

    pConnParam->pszServerPgm     = cip->ci_serverpgm;
    pConnParam->pszServerNode    = Node;
    pConnParam->pszClientNode    = NULL;     /* comm. xserver/kernel only */
    pConnParam->ulMaxDataLen     = cip->ci_max_data_size;
    pConnParam->ulMinReplySize   = (teo00_Longuint)undef_ceo00;
    pConnParam->pidClientPID     = cip->ci_my_pid;


    pConnParam->pszServerDBRoot  = cip->ci_dbroot;

    pConnParam->ulServerRef                = (teo00_Longuint)undef_ceo00;
    pConnParam->fAcknowledge               = FALSE;
    pConnParam->isSSLoverSAPRouterString   = false;
    pConnParam->isSSL                      = false;
    pConnParam->ignoreHostnameInServerCert = false;
    strcpy( pConnParam->authenticationAllow, "BASIC" );
}

/*
 * ===========================================================================
 */
static int en03GetAndCheckConnectionInfo(tsp00_Int4     reference,
        tsp00_Int4                      CheckPid,
        connection_info               **ConnectionInfo,
        char                           *CallingFktName,
        tsp00_ErrTextc                  pErrText )
{
#undef  MF__
#define MF__ MOD__"en03GetAndCheckConnectionInfo"

    if (!VALID_REF_INL (reference))
    {
        en42FillErrText(pErrText, "%s:%s:%d", CallingFktName, ERRMSG_COM_ILL_REFERENCE, reference);
        MSGD (( -11607, ERR_TYPE, "COMMUNIC","%s: %s: %d",
                CallingFktName, ERRMSG_COM_ILL_REFERENCE , reference ))
        return ( commErrNotOk_esp01 );
    }

    *ConnectionInfo = CONNECTION_AT_INL (reference - 1);

    /*
     *  Check the connection information.
     */
    if ( (*ConnectionInfo)->ci_my_ref != reference )
    {
        en42FillErrText( pErrText,
                         "%s:%s:%d/%d",
                         CallingFktName,
                         ERRMSG_COM_CORRUPTED_INTERNAL_DATA,
                         (*ConnectionInfo)->ci_my_ref,
                         reference );
        MSGALL (( -11607, ERR_TYPE, "COMMUNIC","%s: %s! my %d ref %d",
                  CallingFktName, ERRMSG_COM_CORRUPTED_INTERNAL_DATA,
                  (*ConnectionInfo)->ci_my_ref , reference ))
        return ( commErrNotOk_esp01 );
    }

    /*
     *  The application MUST NOT use a connection in a new process.
     *
     * PTS 1109225 MaO
     * The PID check is decrepitly in particular for Linux without NPTL
     *
     */
#if !(defined(LINUX) && !defined(HAS_NTPL)) && !defined(FREEBSD)
    if ( CheckPid == CHECK_PID_EN03 && (*ConnectionInfo)->ci_my_pid != getpid() )
    {
        en42FillErrText( pErrText,
                         "%s:%s:%d/%d",
                         CallingFktName,
                         ERRMSG_COM_APPLIC_FORKED,
                         (*ConnectionInfo)->ci_my_pid,
                         (int)getpid() );
        MSGALL (( -11607, ERR_TYPE, "COMMUNIC","%s: %s!",
                  CallingFktName, ERRMSG_COM_APPLIC_FORKED))
        return ( commErrNotOk_esp01 );
    }
#endif /* LINUX&&!HAS_NPTL || NMP || FREEBSD */
    return(commErrOk_esp01);
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sql03_connect"
static  int     sql03_connect (char            * servernode,
                               tsp00_DbNamec   anyCaseServerdb, /* PTS 1125559 */
                               int             service,
                               int             packet_cnt,
                               tsp00_Int4      * reference,
                               tsp00_Int4      * max_reply_packet_size,
                               tsp00_Int4      * sql_packet_size,
                               void           ** sql_packet_list,
                               char            * dbroot,
                               char            * serverpgm,
                               connection_VMT  * vmt,
                               tsp00_ErrTextc    pErrText)
{
    int                         rc;
    int                         retry;
    tsp00_Int4                  ref;
    pid_t                       pid;
    time_t                      date;
    connection_info *           cip;
    tsp00_DbNamec               serverdb;
    int                         iChar;
    teo003_ConnectParamRecord   ConnParam;
    SAPDB_Bool                  ignoreHostnameInServerCert;


    DBGIN;

    SAPDB_memset(&serverdb[0], 0, sizeof(tsp00_DbNamec));
    for ( iChar = 0; (0 != anyCaseServerdb[iChar])
                  && (iChar < sizeof(tsp00_DbName)); iChar++ )
    {
        serverdb[iChar] = toupper(anyCaseServerdb[iChar]);
    }

    date = time ( (time_t*)0 );
    *reference  = MAXCONNECTIONS + 5 + date % 100 ; /* some illegal value */
    *sql_packet_size = 0;

    pid = getpid ();
    if ( sql03_pid != pid )
    {
        /*
         * PTS 1109761 BV
         * Don't check PID for some os because threads have different pids.
         */
#if !(defined(LINUX) && !defined(HAS_NTPL)) && !defined(FREEBSD)
        if ( sql03_pid ) /* The first time this is zero. */
        {
            DBG1 (( MF__,"application forked \n" ))
            /*
             *  Clear all connections for re-use.
             *  The connetions will be cleared, not released
             *  (sql03_release notices a change of the pid).
             *  This way, the parent process can proceed in using
             *  its connections, while the child frees its
             *  connection data to allow MAXCONNECTIONS new connections.
             */
            sql03_finish ();
        }
#endif
        sql03_pid = pid ; /* from now on that's the pid for reference. */
    }

    if ( (packet_cnt < 1) || (packet_cnt > MAX_SQL_PACKETS) )
    {
        strcpy(pErrText,"illegal packet count");
        MSGALL (( -11987, ERR_TYPE, "COMMUNIC","ERROR: sqlaconnect: illegal packet count: %d",
                  packet_cnt ))
        return ( commErrPacketLimit_esp01 );
    }

    /*
     *  Select a reference number.
     */
    ref = sql03_alloc_connect (); /* PTS 1113728 fixed wrong limitation to maximum of 8 connections */
    if (!VALID_REF_INL (ref))
    {
        strcpy(pErrText ,ERRMSG_COM_TO_MANY_SESSIONS );
        MSGD (( -11601, ERR_TYPE, "COMMUNIC","sql03_connect: %s (%d allowed)",
                ERRMSG_COM_TO_MANY_SESSIONS , MAXCONNECTIONS_INL ))
        return ( commErrNotOk_esp01 );
    }
    cip = CONNECTION_AT_INL (ref - 1);
    DBG1 (( MF__,"reference %d \n", ref ))

    /*
     *  Set sql03_cip to the connection data of the currently active connection.
     *  This information is used by sqladump and sql03_catch_signal.
     */
    sql03_cip = cip;

    SAPDB_memset ( cip , 0 , sizeof(*cip) );
    cip->ci_connect_time   = date;
    cip->ci_use_count ++;
    cip->ci_state          = CON_CONNECTING_EO003;
    cip->ci_service        = service;
    cip->ci_protocol       = PROT_UNDEF_EO003;
    cip->ci_packet_cnt     = packet_cnt;
    cip->ci_my_pid         = pid;
    cip->ci_my_ref         = ref;
    cip->ci_dbroot         = dbroot;
    cip->ci_serverpgm      = serverpgm;
    cip->ci_vmt            = vmt;
    cip->encryption        = RTE_NO_ENCRYPTION;
    cip->kernelWatchDogPID = RTE_UNDEF_OSPID;
    cip->kernelPID         = RTE_UNDEF_OSPID;
    cip->kernelDied        = false;

    /*
     *  convert pascal names to null terminated strings
     */
    DBG1 (( MF__,"servernode '%s' \n", servernode ))
    DBG1 (( MF__,"serverdb   '%s' \n", serverdb ))

    strcpy(cip->ci_peer_dbname , serverdb   );
    /*
     *  If no node name is given, split database name into node/dbname parts.
     */
    if ( servernode && *servernode == '\0' )
    {
        sql03_split_dbname (cip->ci_peer_dbname , cip->ci_peer_node );
    }

    /*
     *  Determine the protocol to use with the database kernel.
     */
    if (commErrOk_esp01 != sql03_get_protocol (servernode, cip, &ignoreHostnameInServerCert, pErrText ))
    {
        return ( commErrNotOk_esp01 );
    }

    /*
     *  Set signal catching routines for the time of the connect establishment.
     */
    sql03_set_signals ();

    /* --- Initialize connect parameter */
    en03InitConnParam  ( servernode, &ConnParam, cip );

    /*
     *  Try to connect for at least fifteen seconds.
     */
    for ( retry  = 0          ,  rc  = commErrTasklimit_esp01;
            retry <= RETRYCOUNT && rc == commErrTasklimit_esp01;
            retry ++  )
    {
        DBG1 (( MF__,"%s" , retry ? "tasklimit, retrying ...\n" : "trying ...\n" ))

        /*
         *  Call the connect function for the requested protocol.
         */
        switch ( cip->ci_protocol )
        {
        case PROT_SHM_EO003 :
        case PROT_BIGSHM_EO003 :
            sql03_set_alarm ( LOCL_CON_TMO );
            rc = sql33_connect ( cip , pErrText , CONNECTION_ARRAY, MAXCONNECTIONS_INL );
            sql03_reset_alarm ();
            break;

#if SOCKETS
        case PROT_SOCKET_EO003 :
            sql03_set_alarm ( 0 );
            rc = sql23_connect ( cip , pErrText );
            sql03_reset_alarm ();
            break;
#endif
        case PROT_NISSL_EO003 :
            ConnParam.isSSLoverSAPRouterString = true;
        case PROT_SSL_EO003 :
            ConnParam.isSSL = true;
            ConnParam.ignoreHostnameInServerCert = ignoreHostnameInServerCert;
        case PROT_NI_EO003 :
            {
                rc = eo03NiConnect ( &ConnParam, &cip->NI, pErrText );
                strcpy( cip->ci_peer_node, cip->NI.szServerNode ) ;  /* now we know the destination node   */
                /* it is part of the saprouter string */
                if ( rc == commErrOk_esp01 )
                {
                    int idx;
                    for ( idx = 0 ; idx < cip->ci_packet_cnt ; idx ++ )
                    {
                        cip->ci_packet[idx] = (void *)ConnParam.pCommPacketList[idx];
                    }
                    cip->ci_max_data_size        = ConnParam.ulMaxDataLen;
                    cip->ci_min_reply_size       = ConnParam.ulMinReplySize;
                    strncpy ( cip->ci_authentication_allow, 
                              ConnParam.authenticationAllow,
                              sizeof(cip->ci_authentication_allow) - 1 );
                    cip->ci_authentication_allow[sizeof(cip->ci_authentication_allow) - 1] = '\0';
                }
            }
            break;
        case PROT_LOCAL_CONTROL_EO003:
        default :
            if (vmt != NULL) {
                sql03_set_alarm (SOCK_CON_TMO);
                rc = vmt->connect (cip, pErrText);
                sql03_reset_alarm ();
            }
            else
            {
                rc = commErrNotOk_esp01;
                strcpy(pErrText, ERRMSG_UNSUPPORTED_PROTOCOL );
                MSGD (( -11602, ERR_TYPE, "COMMUNIC","sqlaconnect: %s %d",
                        ERRMSG_UNSUPPORTED_PROTOCOL, cip->ci_protocol ));
            }
        }
        if ( rc == commErrTasklimit_esp01 )
            (void) sleep ( RETRYTIME );
    }

    /*
     *  Reset signal catching routines to their old values.
     *  (Some signals remain altered).
     */
    sql03_reset_signals ();

    /*
     *  Set connection state.
     */
    if ( rc == commErrOk_esp01 )
    {
        int idx;
        cip->ci_state = CON_CONNECTED_EO003;
        for ( idx = 0 ; idx < cip->ci_packet_cnt ; idx ++ )
        {
            cip->ci_packetData [idx] = ((char*) cip->ci_packet[idx])
                + RTE_HEADER_SIZE_EO003;
            sql_packet_list [idx] = cip->ci_packetData [idx];
        }
        cip->ci_max_request_size = cip->ci_max_data_size - cip->ci_min_reply_size;
        *sql_packet_size       = cip->ci_max_request_size;
        *max_reply_packet_size = cip->ci_max_data_size;
        *reference = ref;
        DBG1 (( MF__,"pack1 0x%08lx pack2 0x%08lx siz %d \n",
                (long) sql_packet_list[0] , (long) sql_packet_list[1],
                *sql_packet_size ))
    }
    else
    {
        if ( rc == commErrTasklimit_esp01 )
        {
            strcpy(pErrText, "task limit");
            MSGD (( -11987, ERR_TYPE, "COMMUNIC","sqlaconnect: task limit" ))
        }
        cip->ci_connect_time   = 0;
        cip->ci_state          = CON_UNUSED_EO003;
        cip->ci_protocol       = PROT_UNDEF_EO003;
    }

    DBG1 (( MF__,"returning %d ref %d \n", rc , *reference ))
    return ( rc );
}
/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sql03_release"
static  int     sql03_release (
    tsp00_Int4                        reference,
    tsp00_ErrTextc                  pErrText )
{
    int             rc;
    int             pid_changed = 0;
    connection_info     *cip = NULL;


    DBGIN;

    rc =  en03GetAndCheckConnectionInfo( reference, !CHECK_PID_EN03, &cip, "sql03_release", pErrText );
    if ( rc != commErrOk_esp01 )
        return(rc);

    /*
     *  If the application forked, silently clear the connection.
     *  It might still be in use by the parent process.
     *
     *  PTS 1109761 BV
     *  Don't check PID for some os because threads have different pids.
     */
#if !(defined(LINUX) && !defined(HAS_NTPL)) && !defined(FREEBSD)
    pid_changed = ( cip->ci_my_pid != getpid() );
#endif

    /*
     *  Set sql03_cip to the connection data of the currently active connection.
     *  This information is used by sqladump and sql03_catch_signal.
     */
    sql03_cip = cip;

    switch ( cip->ci_protocol )
    {
        char            *errm;

    case PROT_SHM_EO003 :
    case PROT_BIGSHM_EO003 :
        if ( pid_changed )
            rc = sql33_clear ( cip , pErrText, CONNECTION_ARRAY, MAXCONNECTIONS_INL );
        else
		{
			/* The 'cancel' is called in order to wake the event task ('cancel' calls 'vcontinue') */
			if (srvEvent_esp01 == cip->ci_service)
			{
				rc = sql33_cancel ( cip , pErrText );
		    }

			if (commErrOk_esp01 == rc)
			{
			    rc = sql33_release ( cip , pErrText, CONNECTION_ARRAY, MAXCONNECTIONS_INL );
			}
		}

        break;

#if SOCKETS
    case PROT_SOCKET_EO003 :
        if ( pid_changed )
            rc = sql23_clear ( cip , pErrText );
        else
            rc = sql23_release ( cip , pErrText );
        break;
#endif

    case PROT_SSL_EO003 :
    case PROT_NISSL_EO003 :
    case PROT_NI_EO003 :
        rc = eo03NiRelease( &cip->NI, pErrText );
        break;

    default :
        if (cip->ci_vmt != NULL)
        {
            if ( pid_changed )
                rc = cip->ci_vmt->clear ( cip , pErrText );
            else
                rc = cip->ci_vmt->release ( cip , pErrText );
        }
        else
        {
            rc = commErrNotOk_esp01;
            en42FillErrText(pErrText,ERRMSG_UNSUPPORTED_PROTOCOL);
            MSGCD (( -11605,ERR_TYPE,"COMMUNIC",
                     "sqlarelease: unsupported protocol %d", cip->ci_protocol ));
        }
    }

    cip->ci_state = CON_UNUSED_EO003;

    DBG1 (( MF__,"returning %d \n", rc ))
    return ( rc );
}

/*
 * ===========================================================================
 */

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sql03_request"
static  int     sql03_request (tsp00_Int4         reference,
                               void             * packet,
                               tsp00_Int4         length,
                               tsp00_Uint1        RTEFlags,
                               tsp00_ErrTextc     pErrText )
{
    int              rc;
    int              idx;
    rte_header      *header;
    connection_info *cip;
    tsp00_Int4         aligned_length;

    DBGIN;

    rc =  en03GetAndCheckConnectionInfo( reference, CHECK_PID_EN03, &cip, "sql03_request", pErrText );
    if ( rc != commErrOk_esp01 )
        return(rc);

    /*
     *  A request is only allowed after a successful connection
     *  establishment or after a successful receive.
     *  A canceled request has to wait for a cancel confirmation
     *  from the kernel. This is done through a normal receive.
     */
    if ( (cip->ci_state != CON_CONNECTED_EO003) && (cip->ci_state != CON_RECEIVED_EO003 ) )
    {
        en42FillErrText( pErrText,
                         "%s:%s",
                         ERRMSG_COM_WRONG_CONN_STATE,
                         sql03_statename(cip));
        MSGD (( -11608, ERR_TYPE, "COMMUNIC","sql03_request: %s, state is '%s'",
                ERRMSG_COM_WRONG_CONN_STATE , sql03_statename(cip) ))
        return ( commErrNotOk_esp01 );
    }

    /*
     *  The packet length must be within the valid range
     *  and 8-byte aligned.
     */
    if ( (length < 0) || (length > cip->ci_max_request_size))
    {
        en42FillErrText( pErrText,
                         "%s:%d",
                         ERRMSG_COM_WRONG_PACKET_LEN,
                         (int)length );
        MSGD (( -11609, ERR_TYPE, "COMMUNIC","sql03_request: %s: %d max %ld",
                ERRMSG_COM_WRONG_PACKET_LEN , length , cip->ci_max_request_size ))
        return ( commErrNotOk_esp01 );
    }

    if (length & 7)
    {
        if ( cip->ci_service != 4 )
        {
            en42FillErrText( pErrText,
                             "Wrong service %d for length %d",
                             cip->ci_service,
                             length );
            MSGD (( -11609, ERR_TYPE, "COMMUNIC","sql03_request: %s: %d max %ld",
                    ERRMSG_COM_WRONG_PACKET_LEN , length , cip->ci_max_request_size ))
            return ( commErrNotOk_esp01 );
        }
        else
        {
            /* packets from the hserver are not necessarily aligned */
            aligned_length = length + 8 - (length & 7);
        }
    }
    else
    {
        aligned_length = length;
    }

    /*
     *  Check the packet pointer.
     */
    header = NULL;
    for ( idx = 0 ; idx < cip->ci_packet_cnt ; idx ++ )
    {
        if ( cip->ci_packetData [idx] == packet )
        {
            header = cip->ci_packet [idx];
            break;
        }
    }
    if ( header == NULL )
    {
        en42FillErrText( pErrText, "Packet pointer points to NULL");
        MSGD (( -11604, ERR_TYPE, "COMMUNIC","sqlarequest: %s: 0x%08lx",
                ERRMSG_COM_WRONG_PACKET_ADDR, (long) packet ))
        for ( idx = 0 ; idx < cip->ci_packet_cnt ; idx ++ )
        {
            MSGD (( -11604, ERR_TYPE, "COMMUNIC","             allowed pointer      0x%08lx",
                    (long) cip->ci_packetData [idx] ))
        }
        return ( commErrNotOk_esp01 );
    }
    cip->ci_request = header;
    cip->ci_requestData = packet;
    cip->ci_request_lgt = length;
    cip->ci_current_packet = idx;

    /*
     *  Determine the pointer to a following reply.
     */
    if (0 != cip->ci_min_reply_size )
    {
        cip->ci_reply = (rte_header*)((char *)cip->ci_requestData + aligned_length);
        cip->ci_replyData = (char *)cip->ci_requestData + aligned_length
            + RTE_HEADER_SIZE_EO003;
        cip->ci_reply_size = cip->ci_max_data_size - aligned_length;
    }
    else
    {
        cip->ci_reply = header;
        cip->ci_replyData = (char *)cip->ci_requestData;
        cip->ci_reply_size = cip->ci_max_data_size;
    }

    /*
     *  Set sql03_cip to the connection data of the currently active connection.
     *  This information is used by sqladump and sql03_catch_signal.
     */
    sql03_cip = cip;

    /*
     *  Setup the RTE header.
     */
    header->rh_act_send_len     = aligned_length + RTE_HEADER_SIZE_EO003;
    header->rh_max_send_len     = aligned_length + RTE_HEADER_SIZE_EO003;
    header->rh_protocol_id      = RSQL_RTE_PROT_LOCAL_EO003;
    header->rh_mess_class       = RSQL_USER_DATA_REQUEST_EO003;
    header->rh_rte_flags        = RTEFlags;
    header->rh_residual_packets = 0;
    header->rh_sender_ref       = cip->ci_my_ref;
    header->rh_receiver_ref     = cip->ci_peer_ref;
    header->rh_rte_return_code  = 0;
    header->rh_new_swap_type    = sql42_new_swap_type();
    header->rh_filler1          = 0;

#ifdef TOKERNEL
    DBG1 (( MF__,"dumping packet to DUMPFILE \n" ))
    sql46_dump ( "\nTOKERNEL------>", 16 );
    sql46_dump ( (char*) packet , (int) length );
    sql46_dump ( "<---------------", 18 - ((int)length + 13)%16 );
    sql46_dump ( "--TOKERNEL\n", 11 );
#endif
#ifdef  DEBUG_RTE
    sql46_dump_packet ( header );
#endif

    switch ( cip->ci_protocol )
    {
        char            *errm;

    case PROT_SHM_EO003 :
    case PROT_BIGSHM_EO003 :
        rc = sql33_request ( cip , pErrText );
        break;

#if SOCKETS
    case PROT_SOCKET_EO003 :
        rc = sql23_request ( cip , pErrText );
        break;
#endif

    case PROT_SSL_EO003 :
    case PROT_NISSL_EO003 :
    case PROT_NI_EO003 :
        rc = eo03NiRequest  ( &cip->NI, cip->ci_current_packet,
                              aligned_length, RTEFlags, pErrText );
        break;

    default :
        if (cip->ci_vmt != NULL)
        {
            rc = cip->ci_vmt->request (cip, pErrText);
        }
        else
        {
            rc = commErrNotOk_esp01;
            en42FillErrText( pErrText, ERRMSG_UNSUPPORTED_PROTOCOL);
            MSGCD (( -11610,ERR_TYPE,"COMMUNIC",
                     "sql03_request: unsupported protocol %d", cip->ci_protocol ));
        }
    }

    if ( rc == commErrOk_esp01 )
        cip->ci_state = CON_REQUESTED_EO003;

    DBG1 (( MF__,"returning %d\n", rc ))
    return ( rc );
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sql03_replyavailable"
static  int     sql03_replyavailable (
    tsp00_Int4                        reference,
    tsp00_ErrTextc                  pErrText )
{
    int              rc;
    rte_header      *header;
    connection_info *cip;


    DBG1 (( MF__,"ref %d \n", reference ))

    rc =  en03GetAndCheckConnectionInfo( reference, CHECK_PID_EN03, &cip, "sql03_replyavailable", pErrText );
    if ( rc != commErrOk_esp01 )
        return(rc);

    /*
     *  A receive is only allowed after a successful request.
     */
    if ( cip->ci_state != CON_REQUESTED_EO003 )
    {
        en42FillErrText(pErrText,ERRMSG_COM_WRONG_CONN_STATE);
        MSGD (( -11608, ERR_TYPE, "COMMUNIC","sql03_replyavailable: %s, state is '%s'",
                ERRMSG_COM_WRONG_CONN_STATE , sql03_statename(cip) ))
        return ( commErrNotOk_esp01 );
    }

    /*
     *  Set sql03_cip to the connection data of the currently active connection.
     *  This information is used by sqladump and sql03_catch_signal.
     */
    sql03_cip = cip;

    switch ( cip->ci_protocol )
    {
        char            *errm;

    case PROT_SHM_EO003 :
    case PROT_BIGSHM_EO003 :
        rc = sql33_replyavailable ( cip , pErrText );
        break;

    case PROT_SOCKET_EO003 :
        rc = sql23_replyavailable ( cip , pErrText );
        break;

    case PROT_SSL_EO003 :
    case PROT_NISSL_EO003 :
    case PROT_NI_EO003 :
        rc = eo03NiSqlReplyAvailable ( &cip->NI, pErrText );
        break;

    default :
        if (cip->ci_vmt != NULL)
        {
            rc = cip->ci_vmt->replyavailable (cip, pErrText);
        }
        else
        {
            rc = commErrNotOk_esp01;
            en42FillErrText(pErrText,ERRMSG_UNSUPPORTED_PROTOCOL);
            MSGCD (( -11610,ERR_TYPE,"COMMUNIC",
                     "sql03_replyavailable: unsupported protocol %d", cip->ci_protocol ));
        }
    }

    DBG1 (( MF__,"returning %d \n", rc ))
    return ( rc );
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sql03_receive"
static  int     sql03_receive (
    tsp00_Int4                        reference,
    void                            **packetp,
    tsp00_Int4                        *lengthp,
    tsp00_ErrTextc                  pErrText )
{
    int             rc;
    rte_header          *header;
    connection_info     *cip;


    DBG1 (( MF__,"ref %d \n", reference ))
    *packetp = 0;
    *lengthp = 0;

    rc =  en03GetAndCheckConnectionInfo( reference, CHECK_PID_EN03, &cip, "sql03_receive", pErrText );
    if ( rc != commErrOk_esp01 )
        return(rc);

    /*
     *  A receive is only allowed after a successful request.
     */
    if ( cip->ci_state != CON_REQUESTED_EO003 )
    {
        en42FillErrText( pErrText,
                         "%s:%s",
                         ERRMSG_COM_WRONG_CONN_STATE,
                         sql03_statename(cip) );
        MSGD (( -11608, ERR_TYPE, "COMMUNIC","sql03_receive: %s, state is '%s'",
                ERRMSG_COM_WRONG_CONN_STATE , sql03_statename(cip) ))
        return ( commErrNotOk_esp01 );
    }

    /*
     *  Set sql03_cip to the connection data of the currently active connection.
     *  This information is used by sqladump and sql03_catch_signal.
     */
    sql03_cip = cip;

    /*
     *  Set sql03_receiving to indicate that we are waiting for a reply.
     *  This information is used by sql03_catch_signal.
     */
    sql03_receiving = reference;

    switch ( cip->ci_protocol )
    {
    case PROT_SHM_EO003 :
    case PROT_BIGSHM_EO003 :
        rc = sql33_receive ( cip , pErrText );
        break;

#if SOCKETS
    case PROT_SOCKET_EO003 :
        rc = sql23_receive ( cip , pErrText );
        break;
#endif

    case PROT_SSL_EO003 :
    case PROT_NISSL_EO003 :
    case PROT_NI_EO003 :
        {
            tsp00_Longuint            ulCmdDataLen;
            teo003_CommPacketRecord  *pResPacket;
            rc = eo03NiReceive ( &cip->NI, &pResPacket,
                                 &ulCmdDataLen,  pErrText );
            if ( rc == commErrOk_esp01 )
            {
                *lengthp = ulCmdDataLen;
                *packetp = pResPacket->pDataPart;
                DBG1 (( MF__,"NI: pResPacket 0x%08lx lgt %d \n",
                        (long) *packetp , *lengthp ))
            }
        }
        break;

    default :
        if (cip->ci_vmt != NULL)
        {
            rc = cip->ci_vmt->receive (cip, pErrText);
        }
        else
        {
            rc = commErrNotOk_esp01;
            en42FillErrText(pErrText,ERRMSG_UNSUPPORTED_PROTOCOL);
            MSGCD (( -11610,ERR_TYPE,"COMMUNIC",
                     "sql03_receive: unsupported protocol %d", cip->ci_protocol ));
        }
    }

    /*
     *  Clear sql03_receiving to indicate that we don't wait for a reply.
     *  This information is used by sql03_catch_signal.
     */
    sql03_receiving = 0;

    if ( rc == commErrOk_esp01 )
    {
        if ( cip->ci_protocol != PROT_NI_EO003 && 
             cip->ci_protocol != PROT_SSL_EO003 && 
             cip->ci_protocol != PROT_NISSL_EO003)
        {
            /* in case of NI/SSL communication the output parameter are set correctly (see above) */
            header = cip->ci_reply;
            *packetp = cip->ci_replyData;
            *lengthp = header->rh_max_send_len - RTE_HEADER_SIZE_EO003;
            DBG1 (( MF__,"pack 0x%08lx lgt %d \n", (long) *packetp , *lengthp ))

            /*
             *  If the packet itself was ok,
             *  deliver the returncode from the kernel's RTE.
             */
            if ( *lengthp > 0 )
                rc = header->rh_rte_return_code;
            else
                rc = commErrNotOk_esp01 ; /* connection broken */
        }

        cip->ci_state = CON_RECEIVED_EO003;

#ifdef  FROMKERNEL
        DBG1 (( MF__,"dumping packet to DUMPFILE \n" ))
        sql46_dump ( "\nFROMKERNEL---->", 16 );
        sql46_dump ( (char*) (*packetp) , (int) *lengthp );
        sql46_dump ( "<---------------", 18 - ((int)*lengthp + 13)%16 );
        sql46_dump ( "FROMKERNEL\n", 11 );
#endif
#ifdef  DEBUG_RTE
        header = cip->ci_reply;
        sql46_dump_packet ( header );
#endif
#ifdef  TRACE_RETCOD
        DBG1 (( MF__,"sp1r_returncode %d \n",
                ((tsp1_packet*)(header+1))->integer.C_2.sp1_segm.
                integer.C_4.sp1r_returncode ))
#endif
    }
    else
    {
        if ( rc == commErrReleased_esp01 ) rc = commErrNotOk_esp01 ; /* connection broken */
    }

    DBG1 (( MF__,"returning %d\n", rc ))
    return ( rc );
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sql03_cancel"
static  int     sql03_cancel ( tsp00_Int4        reference,
                               tsp00_ErrTextc  pErrText )
{
    int             rc;
    connection_info *cip;

    DBGIN;

    rc =  en03GetAndCheckConnectionInfo( reference, CHECK_PID_EN03, &cip, "sql03_cancel", pErrText );
    if ( rc != commErrOk_esp01 )
        return(rc);

    /*
     *  A cancel is only allowed after a successful request.
     */
    if ( cip->ci_state != CON_REQUESTED_EO003 )
    {
        en42FillErrText(pErrText,ERRMSG_COM_WRONG_CONN_STATE);
        MSGD (( -11608, ERR_TYPE, "COMMUNIC","sql03_cancel: %s, state is '%s'",
                ERRMSG_COM_WRONG_CONN_STATE , sql03_statename(cip) ))
        return ( commErrNotOk_esp01 );
    }

    /*
     *  Set sql03_cip to the connection data of the currently active connection.
     *  This information is used by sqladump and sql03_catch_signal.
     */
    sql03_cip = cip;

    switch ( cip->ci_protocol )
    {
        char    *errm;

    case PROT_SHM_EO003 :
    case PROT_BIGSHM_EO003 :
        sql03_set_alarm ( LOCL_PKT_TMO );
        rc = sql33_cancel ( cip , pErrText );
        break;

#if SOCKETS
    case PROT_SOCKET_EO003 :
        sql03_set_alarm ( SOCK_PKT_TMO );
        rc = sql23_cancel ( cip , pErrText );
        break;
#endif

    case PROT_SSL_EO003 :
    case PROT_NISSL_EO003 :
    case PROT_NI_EO003 :
        rc = (int )eo03NiSqlCancelDump( &cip->NI,
                                        NULL /* PTS */,
                                        SQL_RTE_CANCEL_EO003,
                                        pErrText );
        break;

    default :
        if (cip->ci_vmt != NULL)
        {
            sql03_set_alarm ( SOCK_PKT_TMO );
            rc = cip->ci_vmt->cancel (cip, pErrText);
        }
        else
        {
            rc = commErrNotOk_esp01;
            en42FillErrText(pErrText,ERRMSG_UNSUPPORTED_PROTOCOL);
            MSGCD (( -11610,ERR_TYPE,"COMMUNIC",
                     "sql03_cancel: unsupported protocol %d", cip->ci_protocol ));
        }
    }
    sql03_reset_alarm ();

    DBG1 (( MF__,"returning %d \n", rc ))
    return ( rc );
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sql03_dump"
static  int     sql03_dump ( connection_info    *cip,
                             tsp00_ErrTextc     pErrText )
{
    int             rc;


    DBGIN;

    /*
     *  Use the connection, which was active most recently.
     *  That connection set sql03_cip to its connection data.
     */

    if ( ! cip || cip->ci_state == CON_UNUSED_EO003 )
    {
        en42FillErrText(pErrText,ERRMSG_COM_WRONG_CONN_STATE);
        MSGD (( -11608, ERR_TYPE, "COMMUNIC", "sql03_dump: %s", ERRMSG_COM_WRONG_CONN_STATE ))
        return ( commErrNotOk_esp01 );
    }

    switch ( cip->ci_protocol )
    {
        char    *errm;

    case PROT_SHM_EO003 :
    case PROT_BIGSHM_EO003 :
        sql03_set_alarm ( LOCL_PKT_TMO );
        rc = sql33_dump ( cip , pErrText );
        break;

#if SOCKETS
    case PROT_SOCKET_EO003 :
        sql03_set_alarm ( SOCK_PKT_TMO );
        rc = sql23_dump ( cip , pErrText );
        break;
#endif

    case PROT_SSL_EO003 :
    case PROT_NISSL_EO003 :
    case PROT_NI_EO003 :
        sql03_set_alarm ( SOCK_PKT_TMO );
        rc = eo03NiSqlCancelDump( &cip->NI,
                                  NULL /* PTS */,
                                  SQL_RTE_DUMP_EO003,
                                  pErrText );
        break;

    default :
        if (cip->ci_vmt != NULL)
        {
            sql03_set_alarm ( SOCK_PKT_TMO );
            rc = cip->ci_vmt->dump (cip, pErrText);
        }
        else
        {
            rc = commErrNotOk_esp01;
            en42FillErrText(pErrText,ERRMSG_UNSUPPORTED_PROTOCOL);
            MSGCD (( -11610,ERR_TYPE,"COMMUNIC",
                     "sql03_dump: unsupported protocol %d", cip->ci_protocol ));
        }
    }
    sql03_reset_alarm ();

    DBG1 (( MF__,"returning %d \n", rc ))
    return ( rc );
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sql03_find_free_index"
static  tsp00_Int4    sql03_find_free_index ()
{
    int                             idx;

    DBGIN;

    for ( idx = 0 ; idx < MAXCONNECTIONS_INL ; idx ++ )
    {
        if ( CONNECTION_AT_INL (idx)->ci_state == CON_UNUSED_EO003 )
        {
            /* reference goes from 1 to MAXCONNECTIONS */
            idx ++;
            DBG1 (( MF__,"returning %d\n", idx ))
            return ( (tsp00_Int4) idx );
        }
    }

    DBG1 (( MF__,"returning UNDEF\n" ));
    return ( UNDEF );
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sql03_split_dbname"
global  void    sql03_split_dbname (
    char            *serverdb,
    char            *servernode )
{
    int                     c , lgt;
    char                    *p , *q;


    DBG1 (( MF__,"serverdb '%.19s' \n" , serverdb ))

    for ( p = NULL , lgt = 0;
            (c = sql03_nodeseparatorlist[lgt]) != '\0' ; lgt ++ )
    {
        p = (char *)strchr ( (char *)serverdb , c );
        if ( p != NULL ) break;
    }

    if ( p == NULL )
    {
        DBG1 (( MF__,"no nodeseparator found \n" ))
    }
    else
    {
        DBG1 (( MF__,"nodeseparator '%c' \n" , *p ))
        lgt = p - serverdb;
        strncpy ( servernode , serverdb , lgt );
        servernode[lgt] = '\0';

        for ( q = serverdb , p ++ ; *p ; p ++ )
        {
            *(q ++) = *p;
        }
        *q = '\0';
    }

    DBG1 (( MF__,"servernode '%s' \n" , servernode ))
    DBG1 (( MF__,"serverdb   '%s' \n" , serverdb ))
}

/*
 * ===========================================================================
 */

    #undef MF__
    #define MF__ MOD__"sql03_get_protocol"
static  int    sql03_get_protocol ( char                   *Node,  /* or SAPRouter string */
                                    connection_info        *cip,
                                    SAPDB_Bool *            pIgnoreHostnameInServerCert,
                                    tsp00_ErrTextc          pErrText)
{
    int                 rc;
    int                 lnod;
    int                 lnam;
    FILE *              fp;
    char *              ptr;
    VF_FILENAMEC        file;
    VF_FILENAMEC        dbname;
    SQL_NODEIDC         local_hostname;
    SQL_NODEIDC         local_nodename;
    SQL_NODEIDC         peerNodename;
    char                line [ 256 ];
    int                 IsSaprouterString = FALSE;
    tsp01_CommErr_Enum  ulCommState = commErrOk_esp01;
    SAPDB_Bool          isNISSL = false;
    SAPDB_Bool          isSSL = false;
    SAPDB_Bool          optimizeForLocalConnects = false;
    SQL_NODEIDC         tmpNodeName;
    SAPDB_Char *        port = NULL;



    DBGIN;
    lnod = strlen ( (const char *)Node );
    lnam = strlen ( (const char *)cip->ci_peer_dbname );

    /*
    *  Get my host name.
    */
    SAPDB_memset ( local_hostname , 0 , sizeof(local_hostname) );
    SAPDB_memset ( local_nodename , 0 , sizeof(local_nodename) );

    (void) sql43_get_my_host_name ( (char *)local_hostname ,sizeof(local_hostname) );
    DBG1 (( MF__,"local host '%s' \n", local_hostname ))

        /*
        *  Get my official node name.
        */
        rc = sql43_get_official_node_name ((char *)local_hostname,
                (char *)local_nodename,
                sizeof(local_nodename));
    if (rc)
    {
        strncpy (cip->ci_own_node, local_hostname, sizeof(cip->ci_own_node));
    }
    else
    {
        strncpy (cip->ci_own_node, local_nodename, sizeof(cip->ci_own_node));
    }

    DBG1 (( MF__,"local node '%s' \n", local_nodename ))


    if ( lnod )
    {
        SAPDB_memset ( peerNodename , 0 , sizeof(peerNodename) );

        /*
         *  Get the server's official node name.
         */

        ulCommState = RTESec_IsSSLConnection (Node, &isSSL, &isNISSL, pIgnoreHostnameInServerCert, pErrText);

        if (commErrOk_esp01 !=ulCommState)
        {
            DBGOUT;
            return (-1);
        }

        if (!isSSL)
        {
            IsSaprouterString = eo40NiIsSaprouterSring (Node);
        }

        if (!IsSaprouterString  && !isNISSL)
        {
            if (strlen (Node) >= sizeof (SQL_NODEIDC))
            {
                eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SERVER_NODE_NAME_TOO_LONG, 0 );
                DBGOUT;
                return (-1);
            }

            strcpy (tmpNodeName, Node);
            if (NULL != (port = strchr(tmpNodeName, ':')))
            {
                //port is removed from tmpNodeName 
                memset (port, '\0', 1);
            }


            if (!sql43_is_inet_addr(tmpNodeName) && 
                strncasecmp(tmpNodeName, "localhost", strlen("localhost")) != 0)
            {
                rc = sql43_get_official_node_name (tmpNodeName, (char *)peerNodename, sizeof(peerNodename));

                if ( ! rc )
                {
                    strncpy (cip->ci_peer_node, peerNodename, sizeof(cip->ci_peer_node));
					//May be we have to add a port number
					if (NULL != port)
					{
						if (strlen (cip->ci_peer_node) + strlen (port + 1) + 1 >= sizeof (SQL_NODEIDC))
						{
							eo46BuildErrorStringRC ( pErrText, ERRMSG_COM_SERVER_NODE_NAME_TOO_LONG, 0 );
							DBGOUT;
							return (-1);
						}

						strcat (cip->ci_peer_node, ":");
						strcat (cip->ci_peer_node, port + 1); 
					}
                }
                else
                {
                    strcpy ( cip->ci_peer_node, Node);
                }

                lnod = strlen ( (const char *)cip->ci_peer_node );

                //Optimization if given server node is the local node
                if (optimizeForLocalConnects = (strcmp (cip->ci_peer_node, cip->ci_own_node) == 0))
                {
                    isNISSL = false;
                    isSSL = false;
                }
            }
            else
            {
                strcpy ( cip->ci_peer_node , Node);
            }
        }


        DBG1 (( MF__,"peer node '%s' \n", cip->ci_peer_node ))
    }


    if ((IsSaprouterString && !eo40NiIsInitialized ()) || ( isSSL && !RTESec_SAPSSLIsInitialized ()))
    {
        tsp00_Pathc         niTracefileName;

        niTracefileName[0] = '\0';
        if (!eo40NiIsInitialized ())
        {
            eo03NiBuildTracefileName (niTracefileName);
        }

        if (IsSaprouterString) /* => !eo40NiIsInitialized () */
        {
            if (commErrOk_esp01 != eo40NiInit(niTracefileName, 0, pErrText))
            {
                tsp00_ErrTextc  tmpErrText;
                eo40NiFinish (tmpErrText);
                return ( -1 );
            }
        }
        else
        {   /* if eo40NiIsInitialized () is true the niTracefileName was not build above. But this
               does not matter because in this case the trace name is not needed */
            if (commErrOk_esp01 != RTESec_SSLInit(niTracefileName, pErrText))
            {
                return ( -1 );
            }
        }
    }

    if (isNISSL)
    {
        cip->ci_protocol = PROT_NISSL_EO003;
        DBG1 (( MF__, "Secure NI SAP Network Interface connection (SSL)\n"));
    }
    else if (isSSL) 
    {/* it is a SSL connection without SAPRouter String */
        cip->ci_protocol = PROT_SSL_EO003;
        DBG1 (( MF__, "Secure remote connection (SSL)\n"));
    }
    else if ( IsSaprouterString ) 
    {
        cip->ci_protocol = PROT_NI_EO003 ;
        DBG1 (( MF__,"NI connection \n" ))
    }
    else if (lnod && 
               (!optimizeForLocalConnects || ((ptr = getenv ( "DBSOCKETS" )) && *ptr && !strchr("0Nn",*ptr)))) 
    {
        cip->ci_protocol = PROT_SOCKET_EO003 ;
        DBG1 (( MF__,"remote connection \n" ))
        DBG1 (( MF__, "node '%s' db '%s'\n", cip->ci_peer_node , cip->ci_peer_dbname ))
    }
    else 
    {
        cip->ci_protocol = PROT_BIGSHM_EO003 ;
        DBG1 (( MF__,"local connection \n" ))
    }


    if ((cip->ci_service == srvControl_esp01)
            && (cip->ci_protocol == PROT_BIGSHM_EO003)) {
        cip->ci_protocol = PROT_LOCAL_CONTROL_EO003;
    }
#ifdef  DEBUG_RTE
    switch ( cip->ci_protocol )
    {
    case PROT_SOCKET_EO003 :
        DBG1 (( MF__,"remote C connect \n" ))
        break;

    case PROT_SHM_EO003 :
        DBG1 (( MF__,"local small C connect \n" ))
        break;

    case PROT_BIGSHM_EO003 :
        DBG1 (( MF__,"local C connect \n" ))
        break;

    case PROT_NI_EO003 :
        DBG1 (( MF__,"NI connect \n" ))
        break;

    case PROT_NISSL_EO003 :
        DBG1 (( MF__,"NI SSL connect over SAP router string\n" ))
        break;

    case PROT_SSL_EO003 :
        DBG1 (( MF__,"NI SSL connect \n" ))
        break;

    default :
        MSGD (( -11626, ERR_TYPE, "COMMUNIC", "illegal protocol type %d",
                cip->ci_protocol ))
    }
#endif  /*DEBUG_RTE*/
    return(commErrOk_esp01);
}

/*==========================================================================*/

static  char    *sql03_statename (
    connection_info     *cip )
{
    if ( ! cip ) return ( "no connection" );
    switch ( cip->ci_state )
    {
    case CON_UNUSED_EO003 :     return ( "unused" );
    case CON_CONNECTING_EO003 : return ( "connecting" );
    case CON_CONNECTED_EO003 :  return ( "established" );
    case CON_REQUESTED_EO003 :  return ( "requested" );
    case CON_RECEIVED_EO003 :       return ( "received" );
    case CON_TIMEDOUT_EO003 :       return ( "timed out" );
    case CON_CANCELLED_EO003 :      return ( "aborted" );
    }
    return ( "illegal" );
}

/*==========================================================================*/
/*
  Replace signal handler for given signal. 
  
  If old signal action is provided and not initialized, initialize it with found signal action.
 */
extern void en01replace_signal_handler(int sig, void (*sigHandler)(int), struct sigaction *pOldsa);

/*==========================================================================*/

extern SAPDB_Bool en01restore_old_signal_handler(int sig, struct sigaction *pOldsa);

/*==========================================================================*/

static  void    sql03_set_signals ( )
{
    if (sql03_connect_pool.isMultiThreaded) 
    {
        DBG1 (( MF__,"multi threaded, signals not touched \n" ));
        return;
    }

    DBG1 (( MF__,"setting signals \n" ));

    en01replace_signal_handler(SIGPIPE, SIG_IGN, &sql03_oldSIGPIPE);

    if ( !sqlIsRunTimeComponent() )
    {
        en01replace_signal_handler(SIGINT,  sql03_catch_signal, &sql03_oldSIGINT);
        en01replace_signal_handler(SIGHUP,  sql03_catch_signal, &sql03_oldSIGHUP);
        en01replace_signal_handler(SIGTERM, sql03_catch_signal, &sql03_oldSIGTERM);
    }
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"sql03_reset_signals"
static  void    sql03_reset_signals ( )
{
    if (sql03_connect_pool.isMultiThreaded) 
    {
        DBG1 (( MF__,"multi threaded, signals not touched \n" ))
        return;
    }

    DBG1 (( MF__,"resetting signals \n" ))

    en01restore_old_signal_handler( SIGPIPE, &sql03_oldSIGPIPE);

    if ( !sqlIsRunTimeComponent() )
    {
        /* signal handler for SIGINT is not restored!!! This is a feature not a bug... */
        en01restore_old_signal_handler( SIGHUP, &sql03_oldSIGHUP);
        en01restore_old_signal_handler( SIGTERM, &sql03_oldSIGTERM);
    }
}

/*==========================================================================*/


    #undef MF__
    #define MF__ MOD__"sql03_set_alarm"
static  void    sql03_set_alarm (
    unsigned                interval )
{
    connection_info     *cip;

    if (sql03_connect_pool.isMultiThreaded) 
    {
        DBG1 (( MF__,"multi threaded, alarm not set \n" ))
        return;
    }

    DBG1 (( MF__,"setting alarm %u \n", interval ))

    en01replace_signal_handler(SIGALRM, sql03_catch_signal, &sql03_oldSIGALRM );

    sql03_remainingAlarmSeconds = alarm  ( interval );

    if ( sql03_remainingAlarmSeconds != 0 ) sql03_oldclock = time ( (time_t*)0 );

    DBG1 (( MF__,"oldalarm %u \n", sql03_remainingAlarmSeconds ))

}

/*==========================================================================*/

    #undef MF__
    #define MF__ MOD__"sql03_reset_alarm"
static  void    sql03_reset_alarm ( )
{
    long            interval;


    if (sql03_connect_pool.isMultiThreaded)
    {
        DBG1 (( MF__,"multi threaded, alarm not reset \n" ))
        return;
    }

    DBG1 (( MF__,"resetting alarm \n" ))

    if ( sql03_remainingAlarmSeconds != 0 )
    {
        interval = sql03_remainingAlarmSeconds - (time( (time_t*)0 ) - sql03_oldclock);
        if ( interval <= 0 ) interval = 1;
    }
    else
    {
        interval = 0;
    }

    en01restore_old_signal_handler(SIGALRM, &sql03_oldSIGALRM );

    (void) alarm ( (unsigned) interval );

    DBG1 (( MF__,"alarm %u \n" , (unsigned) interval ))
}

/*==========================================================================*/
extern void en01terminateSelfBySignal(int sig);

/*==========================================================================*/
    #undef MF__
    #define MF__ MOD__"sql03_catch_signal"
static  SIGNALFUNCTYPE  sql03_catch_signal (
    int                     sig )
{
    int              do_exit;
    connection_info *cip;

    /*
     *  Use the connection, which is currently active.
     *  That connection set sql03_cip to its connection data.
     */
    cip = sql03_cip;
    if ( cip && (cip->ci_state == CON_UNUSED_EO003) ) cip = 0;

    switch ( sig )
    {
    case SIGALRM:
        if ( cip ) cip->ci_state = CON_TIMEDOUT_EO003;
        break;

    case SIGINT:
        /*
         *  Normally, when an interrupt signal comes, terminate.
         */
        do_exit = TRUE;

        /*
         *  If waiting for kernel reply, cancel the SQL-command
         *  and don't exit.
         */
        if ( sql03_receiving && cip )
        {
            tsp00_ErrTextc  pErrText;
            (void) sql03_cancel ( cip->ci_my_ref , pErrText );
            do_exit = FALSE;
        }

        /*
         *  If the user supplied an interrupt handler,
         *  call it now and don't exit.
         */
        if ( sql03_oldSIGINT.sa_handler
          && sql03_oldSIGINT.sa_handler  != SIG_ERR
          && sql03_oldSIGINT.sa_handler  != SIG_DFL
          && sql03_oldSIGINT.sa_handler  != SIG_IGN
          && sql03_oldSIGINT.sa_handler  != sql03_catch_signal  )
        {
            sql03_oldSIGINT.sa_handler ( sig );
            do_exit = FALSE;
        }

        if ( do_exit && sql03_oldSIGINT.sa_handler != SIG_IGN )
        {
            en01terminateSelfBySignal(sig);
        }
        break;

    case SIGHUP:
        if ( cip && ((cip->ci_protocol == PROT_SHM_EO003) ||
                     (cip->ci_protocol == PROT_BIGSHM_EO003)) )
        {
            sql33_cleanup ( cip );
        }

        if ( en01restore_old_signal_handler ( sig, &sql03_oldSIGHUP ) )
        {
            kill(getpid(), sig);
        }
        else
        {
            en01terminateSelfBySignal(sig);
        }
        break;

    case SIGTERM:
        if ( cip && ((cip->ci_protocol == PROT_SHM_EO003) ||
                     (cip->ci_protocol == PROT_BIGSHM_EO003)) )
        {
            sql33_cleanup ( cip );
        }

        if ( en01restore_old_signal_handler(sig, &sql03_oldSIGTERM) )
        {
            kill(getpid(), sig);
        }
        else
        {
            en01terminateSelfBySignal(sig);
        }
        break;

    default:
        if ( cip && ((cip->ci_protocol == PROT_SHM_EO003) ||
                     (cip->ci_protocol == PROT_BIGSHM_EO003)) )
        {
            sql33_cleanup ( cip );
        }
        break;
    }
}

/*--------------------------------------------------*/

void sql03_init_multi_threaded (
    teo07_MutexFunction *createMutex,
    teo07_MutexFunction *destroyMutex,
    teo07_MutexFunction *lockMutex,
    teo07_MutexFunction *unlockMutex)
{
    sql03_connect_pool.isMultiThreaded = TRUE;
    sql03_connect_pool.createMutex     = createMutex;
    sql03_connect_pool.destroyMutex    = destroyMutex;
    sql03_connect_pool.lockMutex       = lockMutex;
    sql03_connect_pool.unlockMutex     = unlockMutex;
    createMutex (&sql03_connect_pool.mutex);
}

/*--------------------------------------------------*/

static void sql03_init_connections (
    ten03_connect_pool* connectPool,
    int firstIndex,
    int count)
{
    int i;

    SAPDB_memset (connectPool->connection + firstIndex, '\0',
          count * sizeof (connection_info));
    connectPool->connectionCount = firstIndex + count;
}

/*--------------------------------------------------*/

static void sql03_finish_connections (void)
{
    if (sql03_connect_pool.isMultiThreaded) {
        sql03_connect_pool.destroyMutex (&sql03_connect_pool.mutex);
        sql03_connect_pool.createMutex  = NULL;
        sql03_connect_pool.destroyMutex = NULL;
        sql03_connect_pool.lockMutex    = NULL;
        sql03_connect_pool.unlockMutex  = NULL;
        sql03_connect_pool.isMultiThreaded = FALSE;
    }
    FREE_MEM_EO57( sql03_connect_pool.connection );
    sql03_connect_pool.connection = NULL;
    sql03_connect_pool.connectionCount = 0;
    sql03_connect_pool.initialized = FALSE;
}

/*--------------------------------------------------*/

static int sql03_init_connect_pool (ten03_connect_pool* connectPool)
{
    static const int firstChunk = 8;
    int bytesToAllocate = firstChunk * sizeof (connection_info);

    if (connectPool->connection != NULL) {
        MSGCD (( -11600,ERR_TYPE,"COMMUNIC",
                 "ABEND: sql03_init: already initialized before" ))
        sqlabort ();
    }
    if ( ALLOC_MEM_EO57( (void **)&(connectPool->connection), bytesToAllocate)
            != NO_ERROR_EO001 )
    {
        MSGCD (( -11600,ERR_TYPE,"COMMUNIC",
                 "ABEND: sql03_init: out of memory" ))
        sqlabort ();
    }
    sql03_init_connections (connectPool, 0, firstChunk);
    connectPool->initialized = TRUE;
    return TRUE;
}

/*--------------------------------------------------*/

static tsp00_Bool sql03_realloc_pool (ten03_connect_pool* connectPool)
{
    connection_info *oldMemory;
    connection_info *newMemory;
    int              oldMax = connectPool->connectionCount;
    int              newMax = oldMax * 2;

    if ( ALLOC_MEM_EO57( (void **)&newMemory, newMax * sizeof (connection_info))
            != NO_ERROR_EO001 )
    {
        return FALSE;
    }
    SAPDB_memcpy( newMemory, connectPool->connection, oldMax * sizeof(connection_info) );
    oldMemory = connectPool->connection;
    connectPool->connection = newMemory;

    FREE_MEM_EO57( oldMemory );

    sql03_init_connections (connectPool, oldMax, newMax - oldMax);

    return TRUE;
}

/*--------------------------------------------------*/

static tsp00_Int4 sql03_alloc_connect (void)
{
    tsp00_Int4    result;
    tsp00_Bool     allocOK;

    if (!sql03_connect_pool.initialized) {
        sql03_init_connect_pool (&sql03_connect_pool);
    }
    if (sql03_connect_pool.isMultiThreaded) {
        sql03_connect_pool.lockMutex (&sql03_connect_pool.mutex);
    }
    result = sql03_find_free_index ();
    if (result == UNDEF) {
        allocOK = sql03_realloc_pool (&sql03_connect_pool);
        if (allocOK) {
            result = sql03_find_free_index ();
        }
    }
    if (sql03_connect_pool.isMultiThreaded) {
        sql03_connect_pool.unlockMutex (&sql03_connect_pool.mutex);
    }
    return result;
}

/*--------------------------------------------------*/

static int sql03_free_connect (int ref, char** msg)
{
    int result;
    tsp00_ErrTextc errtext;

    if (sql03_connect_pool.isMultiThreaded) {
        sql03_connect_pool.lockMutex (&sql03_connect_pool.mutex);
    }
    result = sql03_release (ref, errtext);
    if (sql03_connect_pool.isMultiThreaded) {
        sql03_connect_pool.unlockMutex (&sql03_connect_pool.mutex);
    }
    return result;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
