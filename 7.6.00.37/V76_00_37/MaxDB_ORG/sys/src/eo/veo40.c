/*!
  @file           veo40.c
  @author         JoergM
  @special area   TCP/IP routines via SAP NI ( network interface )
  @brief          description ...
  @see            example.html ...

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

#if ! defined STUB_MODULE
#define IMPLEMENTATION_MODULE
#endif

/*===========================================================================*/
#if defined IMPLEMENTATION_MODULE || defined STUB_MODULE
/*===========================================================================*/
/*--------------------------------------------------------------------*/
/* SAP includes                                                       */
/*--------------------------------------------------------------------*/
/* The following platforms already use the 6.20 Unicode NI libraries */
#define SAPwithUNICODE 

#ifdef WIN32
  #define SAPonNT
#endif

#ifdef _IBMR2
  #define SAPonRS6000
#endif

#if PA11 || PA20W
  #define SAPonHPPA
#endif

#if defined(OSF1)
  #define SAPonALPHA
#endif

#if defined (HP_IA64)
  #define SAPonHPPA
  #define SAPonHPIA64
#endif

#if (defined(SVR4) && defined(SUN)) || defined(SOLARIS)
  #define SAPonSUN
#endif

#if defined(LINUX)
#define SAPonLIN
#endif

#if defined(FREEBSD)
#define SAPonFREEBSD
/* defined somewhere else ... */
#endif

/* UNICODE */
#include "sap/700/saptype.h"	/* nocheck */
#include "sap/700/saptypeb.h"	/* nocheck */
#include "sap/700/saptypec.h"	/* nocheck */
#include "sap/700/sapuc.h"		/* nocheck */
#include "sap/700/sapuc2.h"		/* nocheck */
      
/* NI */
#include "sap/700/err.h"		/* nocheck */
#include "sap/700/nixx.h"		/* nocheck */
#include "sap/700/dptrace.h"	/* nocheck */
   
/*===========================================================================*/
#endif /* IMPLEMENTATION_MODULE || STUB_MODULE */
/*===========================================================================*/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* externC is defined in SAP-Includes (sapuc.h) too   */
#undef externC
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo007_2.h"
#include "heo420.h"
#include "geo40.h"
#include "heo46.h"
#include "heo47.h"
#include "geo60.h"
#include "SAPDBCommon/SAPDB_string.h"

#define MOD__ "veo40.c:"

/*===========================================================================*/
#ifdef IMPLEMENTATION_MODULE
/*===========================================================================*/
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	__inline
#endif

#ifdef DEBUG_RTE
#define DBGNI(_arg) {printf("%s", MOD__); printf _arg ; printf("\n");}
#else
#define DBGNI(_arg)
#endif

/*
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    SAP Unicode is realized as UTF16. So a single unicode character may need  
    a four byte representation. In case of unicode the datatype SAP_UC is
    a short, i.e sizeof (SAP_UC) == 2.
    Therefore in case of unicode the following define is not quite exact. But we
    assume that we have not to deal with characters that need a four byte 
    representation.
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */ 

#define LENGTH_OF_UNICODE_CHARACTER		2
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

externC FILE *tf ; /* SAP global trace/error file descriptor */
SAPDB_Bool              is_NI_initialized = false;

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/
static void
eo40NiBuildErrorString  ( tsp00_ErrTextc VAR_ARRAY_REF  pErrText,
                          char                         *ErrorMsgHeader,
                          SAPRETURN                     SapRc );

static void *
eo40NiDupHandle         ( void                         *NiHandle );

static void
eo40GetServiceAsString  ( unsigned short               *Service,
                          SAP_UC                       *szService);

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

#endif /* IMPLEMENTATION_MODULE */


#if defined IMPLEMENTATION_MODULE

/*===========================================================================*/
externC int
eo40NiIsLoaded()
{
  return(true);
}

externC int
eo40NiIsAvailable()
{
  return(TRUE);
}

externC int
eo40NiIsInitialized()
{
  return is_NI_initialized;
}

/*===========================================================================*/

#define CTRC_TRUNC      1

externC tsp00_Uint
eo40NiInit        ( tsp00_Pathc     VAR_ARRAY_REF  TraceFile,
                    int                            TraceLevel,  /* obsolete */
                    tsp00_ErrTextc  VAR_ARRAY_REF  ErrText )
{
    SAPRETURN             SapRc ;
    tsp00_Uint            ulCommState = commErrOk_esp01;
    SAP_UC *			pTraceFileNameUC;
    SAP_UC				nullByte = {'\0'};

    pTraceFileNameUC = (SAP_UC *) alloca ((strlen (TraceFile) + 1) * LENGTH_OF_UNICODE_CHARACTER);
    A7sToUcs (pTraceFileNameUC, TraceFile);
    tf = (FILE *)CTrcInit (pTraceFileNameUC[0] != nullByte ? pTraceFileNameUC : NULL ,
        CTRC_TRUNC, NI_TRACE_ERRORS_ONLY_EO40);
    SapRc = NiInit();

    if ( SapRc == SAP_O_K)
    {
        ulCommState = commErrOk_esp01;
        is_NI_initialized = true;
    }
    else
    {
        ulCommState = commErrNotOk_esp01 ;
        eo40NiBuildErrorString ( ErrText, ERRMSG_NICOM_INIT_ERROR, SapRc );
    }
    return ( ulCommState ) ;
}

/*===========================================================================*/
#define ERRMSG_NICOM_GET_SD_ERROR "Can't get sd from NiHdl:"

externC tsp00_Uint
eo40NiHandleToSocket        ( void                         *NiHandle ,
                              int                          *Sd,
                              tsp00_ErrTextc  VAR_ARRAY_REF  ErrText )
{
  tsp00_Uint              ulCommState;
  SAPRETURN               SapRc ;

  SapRc = NiHdlToSock 	 (*(NI_HDL *)NiHandle, Sd ) ;

  if ( SapRc == SAP_O_K)
    ulCommState = commErrOk_esp01 ;
  else
  {
    ulCommState = commErrNotOk_esp01 ;
    eo40NiBuildErrorString ( ErrText, ERRMSG_NICOM_GET_SD_ERROR, SapRc );
    ErrTrace(tf);
  }

  return ( ulCommState );

}


/*===========================================================================*/

externC tsp00_Uint
eo40NiDataAvailable   ( void                         *NiHandle ,
                        tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{
  #undef  MF__
  #define MF__ MOD__"eo40NiDataAvailable"
  tsp00_Uint              ulCommState;
  SAPRETURN               SapRc ;

  SapRc = NiPeek ( *(NI_HDL *)NiHandle, 0 ) ;

  switch (SapRc)
  {
    case SAP_O_K    : ulCommState = commErrOk_esp01 ;        break ;
    case NIETIMEOUT : ulCommState = commErrWouldBlock_esp01; break ;
    default         : ulCommState = commErrNotOk_esp01;
                      eo40NiBuildErrorString ( pErrText,
                                               ERRMSG_NICOM_PEEK_ERROR, SapRc );
                      ErrTrace(tf);
                      break ;
  }
  return ( ulCommState );
}

/*===========================================================================*/

int eo40NiIsValidHandle      ( void                     *NiHandle )
{
  return ( NiHandle != NULL && *(NI_HDL *)NiHandle != NI_INVALID_HDL ) ;
}


/*===========================================================================*/
externC tsp00_Uint
eo40NiClose     (void **        NiHandle)
{
  tsp00_Uint              ulCommState = commErrOk_esp01;
  SAPRETURN               SapRc ;

  if ( eo40NiIsValidHandle ( *NiHandle) )
  {
    SapRc = NiCloseHandle (**(NI_HDL **)NiHandle) ;
    free ( *NiHandle ) ;
    *NiHandle = NULL ;
  }

  return ( ulCommState );
}

/*===========================================================================*/

externC tsp00_Uint
eo40NiFinish(tsp00_ErrTextc VAR_ARRAY_REF   pErrText)
{
  SAPRETURN               SapRc;
  tsp00_Uint              ulCommState = commErrOk_esp01;

  if (is_NI_initialized)
  {
    NiCloseAll ();
    SapRc = NiExit();
    if ( SapRc  == SAP_O_K )
        ulCommState = commErrOk_esp01 ;
    else
    {
        ulCommState = commErrNotOk_esp01 ;
        eo40NiBuildErrorString ( pErrText, ERRMSG_NICOM_FINISH_ERROR, SapRc );
        ErrTrace(tf);
    }
  }

  is_NI_initialized = false;
  return ( ulCommState );
}

/*===========================================================================*/

externC tsp00_Uint
eo40NiListen    ( void                        **NiHandle,
                  unsigned short               *Service,
                  tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{
  tsp00_Uint              CommState = commErrOk_esp01;
  SAPRETURN               SapRc ;
  NI_HDL                  ListenHandle ;
  SAPDB_Char			  szService[6 * LENGTH_OF_UNICODE_CHARACTER];	/* Maximal 6 unicode character   */
										     							/* (we suppose that an charakter */
																		/* needs maximal 2 bytes)        */
  eo40GetServiceAsString (Service, (SAP_UC *)szService);


  SapRc = NiListen ((SAP_UC *)szService, &ListenHandle) ;

  if ( SapRc == SAP_O_K)
  {
    *NiHandle = eo40NiDupHandle( &ListenHandle ) ;
    CommState = *NiHandle ? commErrOk_esp01 : commErrNotOk_esp01 ;
  }
  else
  {
    eo40NiBuildErrorString ( pErrText, ERRMSG_NICOM_LISTEN_ERROR, SapRc );
    CommState = commErrNotOk_esp01 ;
    ErrTrace(tf);
  }
  return ( CommState );
}

/*===========================================================================*/
externC tsp00_Uint
eo40NiAccept    ( void                        *NiListenHandle,
                  void **                      NiCommHandle,
                  char                        *ServerNode,
                  int                          MaxServerNodeLen,
                  tsp00_ErrTextc VAR_ARRAY_REF ErrText )
{
  tsp00_Uint              CommState = commErrOk_esp01;
  SAPRETURN               SapRc ;
  NI_HOSTADR              hostaddr ;
  NI_HDL                  NiHandle ;

  SapRc = NiAccept (*(NI_HDL *)NiListenHandle, NI_BLOCK, &NiHandle, &hostaddr );

  if ( SapRc == SAP_O_K)
  {
      SAP_UC *pHostNameUC = NiAddrToHost (&hostaddr);

      if ( pHostNameUC == NULL )
          pHostNameUC = NiAdrToStr (&hostaddr);

      if ( pHostNameUC != NULL )
      {
          SAP_A7	* pHostName;

          pHostName = (SAP_A7 *) alloca (strlenU (pHostNameUC) + 1);

          UcnToA7n(pHostName, pHostNameUC, (int)(strlenU (pHostNameUC)));
          memset (pHostName + strlenU (pHostNameUC), '\0', 1);

          strncpy ( ServerNode, (SAPDB_Char *)pHostName, MaxServerNodeLen ) ;
          ServerNode [ MaxServerNodeLen -1 ] = '\0' ;
      }

      *NiCommHandle = eo40NiDupHandle( &NiHandle ) ;
      CommState = *NiCommHandle ? commErrOk_esp01 : commErrNotOk_esp01 ;
  }
  else
  {
      CommState = commErrNotOk_esp01 ;
      eo40NiBuildErrorString ( ErrText, ERRMSG_NICOM_ACCEPT_ERROR, SapRc );
      ErrTrace(tf);
  }

  return ( CommState );
}



/*===========================================================================*/

tsp00_Uint
eo40NiSend ( void                         *SendHandle,
             char                         *pData,
             int                           Len,
             tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{
  tsp00_Uint              CommState = commErrOk_esp01;
  SAPRETURN               SapRc ;
  NI_HDL                  NiHandle = *((NI_HDL *)SendHandle ) ;
  int                     BytesSend;

  for (BytesSend = 0;
       Len > 0 && CommState == commErrOk_esp01 ;
       pData += BytesSend, Len -= BytesSend )
  {
    SapRc = NiRawWrite (NiHandle, (SAP_BYTE *)pData, Len, NI_BLOCK, &BytesSend);
    if ( SapRc != SAP_O_K)
    {
      eo40NiBuildErrorString ( pErrText, ERRMSG_NICOM_SOCKET_SND_ERROR, SapRc );
      CommState = commErrNotOk_esp01 ;
      ErrTrace(tf);
    }
  }
  return ( CommState );
}

/*===========================================================================*/
tsp00_Uint
eo40NiReceive ( void                         *RecvHandle,
                char                         *pData,
                tsp00_Uint                    ulMaxDataLen,
                tsp00_Uint                   *BytesReceived,
                tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{
  #undef  MF__
  #define MF__ MOD__"eo40NiReceive"

  tsp00_Uint              CommState = commErrOk_esp01;
  SAPRETURN               SapRc ;
  SAP_INT                 readLen;
  NI_HDL                  NiHandle = *((NI_HDL *)RecvHandle ) ;

  SapRc = NiRawRead (NiHandle, (SAP_BYTE *)pData, ulMaxDataLen, NI_BLOCK, &readLen);

  DBGNI(( "header: wanted %d bytes, read %d, SapRc = %d",
           BytesRemaining, *BytesReceived, SapRc ));

  if ( SapRc != SAP_O_K )
  {
      eo40NiBuildErrorString ( pErrText, ERRMSG_NICOM_REC_ERROR, SapRc );
      CommState = commErrNotOk_esp01 ;
      ErrTrace(tf);
  }

  *BytesReceived = readLen;
  return ( CommState );
}

/*===========================================================================*/
tsp00_Uint
eo40NiConnectToServer( teo40_NiConnectInfoRecord    *pNIConnInfo,
                       tsp00_ErrTextc VAR_ARRAY_REF  ErrText )
{
  SAPRETURN      SapRc ;
  tsp00_Uint     CommState = commErrOk_esp01;
  NI_HDL         NiHandle ;
  SAP_UC *		 pSapRouterString;		
  SAPDB_Char	 szService[6 * LENGTH_OF_UNICODE_CHARACTER]; /* Maximal 6 unicode character   */
														     /* (we suppose that an charakter */
														     /* needs maximal 2 bytes)        */

  eo40GetServiceAsString (&pNIConnInfo->usServicePort, (SAP_UC *)szService);

  pSapRouterString = (SAP_UC *) alloca ((strlen (pNIConnInfo->szSapRouterString) + 1) * LENGTH_OF_UNICODE_CHARACTER);
  A7sToUcs (pSapRouterString, pNIConnInfo->szSapRouterString);

  SapRc = NiRawConnect (pSapRouterString, (SAP_UC *)szService, NI_BLOCK, &NiHandle);
  if ( SapRc == SAP_O_K )
  {
      pNIConnInfo->NiHandle = eo40NiDupHandle( &NiHandle ) ;
      CommState = pNIConnInfo->NiHandle ? commErrOk_esp01 : commErrNotOk_esp01 ;
  }
  else
  {
    eo40NiBuildErrorString ( ErrText, ERRMSG_NICOM_CONNECT_ERROR, SapRc );
    CommState = commErrNotOk_esp01 ;
    ErrTrace(tf);
  }
  return ( CommState ) ;
}


/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

/*===========================================================================*/

#define ERR_NIEINTERN           "internal error"
#define ERR_NIEHOST_UNKNOWN     "unknown Host"
#define ERR_NIESERV_UNKNOWN     "unknown Service"
#define ERR_NIESERV_USED        "Service in use"
#define ERR_NIETIMEOUT          "timeout"
#define ERR_NIECONN_BROKEN      "connection broken"
#define ERR_NIECONN_REFUSED     "connection refused"

#define ERR_NIEINVAL            "invalid parameter"
#define ERR_NIEPING             "ping received"
#define ERR_NIECONN_PENDING     "NIECONN_PENDING"
#define ERR_NIEVERSION          "bad version"

#define ERR_NIEROUT_HOST_UNKNOWN "ROUT: unknown host"
#define ERR_NIEROUT_SERV_UNKNOWN "ROUT: unknown service"
#define ERR_NIEROUT_PERM_DENIED  "ROUT: perm. denied"
#define ERR_NIEROUT_CONN_REFUSED "ROUT: conn. refused"
#define ERR_NIEROUT_CONN_BROKEN  "ROUT: conn. broken"
#define ERR_NIEROUT_INTERN       "ROUT: intern. error"
#define ERR_NIEROUT_VERSION      "ROUT: bad version"
#define ERR_NIEROUT_CANCELED     "ROUT: comm. canceled"

#define ERR_NIEROUT_SHUTDOWN     "ROUT: shutdown"
#define ERR_NIEROUT_INFO_DENIED  "ROUT: info denied"
#define ERR_NIEROUT_OVERFLOW     "ROUT: client overflow"
#define ERR_NIEROUT_MODE_DENIED  "ROUT: talkmode forbidden"
#define ERR_NIEROUT_NOCLIENT     "ROUT: client missing"
#define ERR_NIEROUT_EXTERN       "ROUT: ext. lib error"
#define ERR_NIEROUT_SNC_FAILURE  "ROUT: SNC Error"

#define ERR_NI_UNKNOWN_RC        "NI: RC="
/*===========================================================================*/

static void
eo40NiBuildErrorString ( tsp00_ErrTextc VAR_ARRAY_REF ErrTextC,
                         char                        *ErrorMsgHeader,
                         SAPRETURN                    SapRc )
{
  char *NiErrorString ;
  char UnknownRC[sizeof(ERR_NI_UNKNOWN_RC) + 8];
  int  ErrorHeaderLen  = (int)strlen(ErrorMsgHeader) ;
  int  ErrorTrailerLen = sizeof(tsp00_ErrTextc) - ErrorHeaderLen - 1 ;

  switch ( SapRc )
  {
    case NIEHOST_UNKNOWN     : NiErrorString = ERR_NIEHOST_UNKNOWN     ; break ;
    case NIESERV_UNKNOWN     : NiErrorString = ERR_NIESERV_UNKNOWN     ; break ;
    case NIESERV_USED        : NiErrorString = ERR_NIESERV_USED        ; break ;
    case NIETIMEOUT          : NiErrorString = ERR_NIETIMEOUT          ; break ;
    case NIECONN_BROKEN      : NiErrorString = ERR_NIECONN_BROKEN      ; break ;
    case NIEINVAL            : NiErrorString = ERR_NIEINVAL            ; break ;
    case NIECONN_REFUSED     : NiErrorString = ERR_NIECONN_REFUSED     ; break ;
    case NIEPING             : NiErrorString = ERR_NIEPING             ; break ;
    case NIECONN_PENDING     : NiErrorString = ERR_NIECONN_PENDING     ; break ;
    case NIEVERSION          : NiErrorString = ERR_NIEVERSION          ; break ;
    case NIEINTERN           : NiErrorString = ERR_NIEINTERN           ; break ;

    case NIEROUT_HOST_UNKNOWN: NiErrorString = ERR_NIEROUT_HOST_UNKNOWN; break ;
    case NIEROUT_SERV_UNKNOWN: NiErrorString = ERR_NIEROUT_SERV_UNKNOWN; break ;
    case NIEROUT_PERM_DENIED : NiErrorString = ERR_NIEROUT_PERM_DENIED ; break ;
    case NIEROUT_CONN_REFUSED: NiErrorString = ERR_NIEROUT_CONN_REFUSED; break ;
    case NIEROUT_CONN_BROKEN : NiErrorString = ERR_NIEROUT_CONN_BROKEN ; break ;
    case NIEROUT_INTERN      : NiErrorString = ERR_NIEROUT_INTERN      ; break ;
    case NIEROUT_VERSION     : NiErrorString = ERR_NIEROUT_VERSION     ; break ;
    case NIEROUT_CANCELED    : NiErrorString = ERR_NIEROUT_CANCELED    ; break ;

    case NIEROUT_SHUTDOWN    : NiErrorString = ERR_NIEROUT_SHUTDOWN    ; break ;
    case NIEROUT_INFO_DENIED : NiErrorString = ERR_NIEROUT_INFO_DENIED ; break ;
    case NIEROUT_OVERFLOW    : NiErrorString = ERR_NIEROUT_OVERFLOW    ; break ;
    case NIEROUT_MODE_DENIED : NiErrorString = ERR_NIEROUT_MODE_DENIED ; break ;
    case NIEROUT_NOCLIENT    : NiErrorString = ERR_NIEROUT_NOCLIENT    ; break ;
    case NIEROUT_EXTERN      : NiErrorString = ERR_NIEROUT_EXTERN      ; break ;
    case NIEROUT_SNC_FAILURE : NiErrorString = ERR_NIEROUT_SNC_FAILURE ; break ;

    default                  : strcpy(UnknownRC, ERR_NI_UNKNOWN_RC) ;
                               sql47_itoa(SapRc, UnknownRC  + sizeof(ERR_NI_UNKNOWN_RC),
                                          sizeof(UnknownRC) - sizeof(ERR_NI_UNKNOWN_RC) ) ;
                               NiErrorString = UnknownRC               ; break ;
  }

  strcpy( ErrTextC, ErrorMsgHeader ) ;

  if ( ErrorTrailerLen > 0 )
  {
    ErrTextC [ ErrorHeaderLen ] = ',';
    strncpy( &ErrTextC [ ErrorHeaderLen + 1] , NiErrorString, ErrorTrailerLen ) ;
  }

}

/*===========================================================================*/

static void *
eo40NiDupHandle ( void *NiHandle )
{
  void *NiDupHandle = malloc ( sizeof(NI_HDL) ) ;

  if ( NiDupHandle )
    SAPDB_memcpy( NiDupHandle, NiHandle, sizeof(NI_HDL) ) ;

  return ( NiDupHandle ) ;
}

/*===========================================================================*/
static void
eo40GetServiceAsString ( unsigned short *Service,
                         SAP_UC          *szService)
{
  unsigned short NiServiceNo ;
  char           szServiceAsChar[6];

  if ( !Service || *Service == 0 )
  {
    NiServiceNo = DEFAULT_PORT_SERVICE_NI72_EO003 ;
    if (Service)
      *Service = NiServiceNo; /* give used ServiceNo back */
  }
  else
    NiServiceNo = *Service ;

  sql47_itoa( ( int )NiServiceNo, szServiceAsChar, 6);

  A7sToUcs (szService, szServiceAsChar);
}

/*===========================================================================*/

#if defined(LINUX)
/* On LINUX the dladdr() is contained in libdl which is normally not used during link step. */
/* Since dladdr() is only needed in Stack back trace routine of R/3 which is never called here */
/* there is no reason to have a usefull implementation for it. So we put a dummy here, that satisfies the linker */
/* Structure containing information about object searched using `dladdr'.  */

/* copied from /usr/include/dlfcn.h ... */
typedef struct
{
  const char *dli_fname;      /* File name of defining object.  */
  void *dli_fbase;              /* Load address of that object.  */
  const char *dli_sname;      /* Name of nearest symbol.  */
  void *dli_saddr;              /* Exact value of nearest symbol.  */
} Dl_info;

/* Fill in *INFO with the following information about ADDRESS.
   Returns 0 iff no shared object's segments contain that address.  */
int dladdr (const void *__address, Dl_info *__info)
{
    return 0;
}
#endif /* LINUX */

/*===========================================================================*/

#endif /* IMPLEMENTATION_MODULE */


#ifdef STUB_MODULE
/*===========================================================================*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <string.h>
#include    "RunTime/Security/RTESec_SAPSSL.h" /* nocheck */
#include    "RunTime/System/RTESys_SharedLib.h" /* nocheck */
#include    "gip00.h" /* nocheck */


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#if defined (_WIN32)
#define SAPNI_FILENAME "sapni"
#else
#define SAPNI_FILENAME "libsapni"
#endif
/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

/*
    EXTERNAL VARIABLES
*/
#if defined (_WIN32)
extern HANDLE       hModule;
#endif

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

RTE_SharedLibHandle hSAPNI = RTE_ILLEGAL_SHARED_LIB_HANDLE;

typedef int (*eo40NiIsLoaded_t)();
static eo40NiIsLoaded_t fpeo40NiIsLoaded = 0;
   
typedef int (*eo40NiIsAvailable_t)();
static eo40NiIsAvailable_t fpeo40NiIsAvailable = 0;

typedef int (*eo40NiIsInitialized_t)();
static eo40NiIsInitialized_t fpeo40NiIsInitialized = 0;

typedef tsp00_Uint (*eo40NiInit_t)( tsp00_Pathc VAR_ARRAY_REF, int, tsp00_ErrTextc VAR_ARRAY_REF );
static eo40NiInit_t fpeo40NiInit = 0;

typedef tsp00_Uint (*eo40NiHandleToSocket_t) ( void *, int *, tsp00_ErrTextc VAR_ARRAY_REF );
static  eo40NiHandleToSocket_t fpeo40NiHandleToSocket = 0;

typedef tsp00_Uint (*eo40NiDataAvailable_t) ( void *, tsp00_ErrTextc VAR_ARRAY_REF );
static eo40NiDataAvailable_t fpeo40NiDataAvailable = 0;

typedef int (*eo40NiIsValidHandle_t) ( void * );
static eo40NiIsValidHandle_t fpeo40NiIsValidHandle = 0;

typedef tsp00_Uint (*eo40NiClose_t) ( void ** );
static eo40NiClose_t fpeo40NiClose  = 0;

typedef tsp00_Uint (*eo40NiFinish_t) (tsp00_ErrTextc VAR_ARRAY_REF);
static eo40NiFinish_t fpeo40NiFinish = 0;

typedef tsp00_Uint (*eo40NiListen_t) ( void **, unsigned short *, tsp00_ErrTextc VAR_ARRAY_REF );
static eo40NiListen_t fpeo40NiListen = 0;

typedef tsp00_Uint (*eo40NiAccept_t) ( void *, void **, char *, int, tsp00_ErrTextc VAR_ARRAY_REF );
static eo40NiAccept_t fpeo40NiAccept = 0;

typedef tsp00_Uint (*eo40NiSend_t) ( void *, char *, int, tsp00_ErrTextc VAR_ARRAY_REF );
static eo40NiSend_t fpeo40NiSend = 0;

typedef tsp00_Uint (*eo40NiReceive_t) ( void *, char *, tsp00_Uint, tsp00_Uint *, tsp00_ErrTextc VAR_ARRAY_REF );
static eo40NiReceive_t fpeo40NiReceive = 0;

typedef tsp00_Uint (*eo40NiConnectToServer_t) ( teo40_NiConnectInfoRecord *, tsp00_ErrTextc VAR_ARRAY_REF );
static eo40NiConnectToServer_t fpeo40NiConnectToServer = 0;


/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static void
eo40NiUnLoadLib ()
{
    tsp00_ErrTextc          tmpErrText;

    if (hSAPNI != RTE_ILLEGAL_SHARED_LIB_HANDLE)
    {
        RTESys_UnloadSharedLibrary(hSAPNI, tmpErrText, sizeof(tsp00_ErrTextc) - 1);
    }

    fpeo40NiIsLoaded = 0;
    fpeo40NiIsAvailable = 0;
    fpeo40NiIsInitialized = 0;
    fpeo40NiInit = 0;
    fpeo40NiHandleToSocket = 0;
    fpeo40NiDataAvailable = 0;
    fpeo40NiIsValidHandle = 0;
    fpeo40NiClose  = 0;
    fpeo40NiFinish = 0;
    fpeo40NiListen = 0;
    fpeo40NiAccept = 0;
    fpeo40NiSend = 0;
    fpeo40NiReceive = 0;
    fpeo40NiConnectToServer = 0;

    /* UnLoad SSL Interface */
    RTESec_UnLoadSSLFunctions ();

    hSAPNI = RTE_ILLEGAL_SHARED_LIB_HANDLE;
}

/*===========================================================================*/

static RTE_SharedLibHandle
eo40NiLoadLib (tsp00_ErrTextc    errText)
{
    tsp00_Pathc             szDllPath;
    tsp00_Pathc             dllPath;
    int                     bLoadSymbols;
    tsp01_RteError          UpdateError;
    tsp01_RteError          RteError;
    RTE_Path                fullDllPath;
#if defined (_WIN32)
	SAPDB_Bool              updateSucceeded = false;
    DWORD                   dwLen;
    SAPDB_Bool              dummy = false;
#endif
    bLoadSymbols = ( hSAPNI == RTE_ILLEGAL_SHARED_LIB_HANDLE ) ? 1 : 0;

    if (bLoadSymbols == 0)
    {
        return ( hSAPNI );
    }

#if defined (_WIN32)
	/* Manipulate Path variable, so that the dynamic libraries from SAP could be found in <IndepLib> */
    /* Ignore any errors for the moment because there are applications (e.g. SQLStudio) which do not care for the 'IndependentLibPath'. */
	updateSucceeded = sqlUpdateNiLibPathEnvironment(&dummy, &UpdateError);

	/* On Windows try first to load the SAPNI from the path of the DLL or executable respectively */
    if ( hModule != INVALID_HANDLE_VALUE )
    {
        dwLen = GetModuleFileName(hModule, szDllPath, sizeof(szDllPath) - 1);
    }
    else
    {
        dwLen = GetModuleFileName(NULL, szDllPath, sizeof(szDllPath) - 1);
    }

    if (dwLen > 0) 
    {
        char *Ptr;

        Ptr = strrchr ((char *) szDllPath, '\\');
        if (Ptr != 0) 
        {
            Ptr++;
            *Ptr = '\0';
            strcpy (dllPath, szDllPath);
            strcat( szDllPath, SAPNI_FILENAME );
            RTESys_LoadSharedLibrary (szDllPath, 
                RTE_LoadSymbolsLocal, 
                errText,
                sizeof(tsp00_ErrTextc),
                fullDllPath, 
                &hSAPNI);
        }
    }
#endif

    /* now try indep program or indep library path respectively */
    if ( hSAPNI == RTE_ILLEGAL_SHARED_LIB_HANDLE )
    {
#if defined (_WIN32)
		//try to extend the DLL search path with the 'IndependentLibPath'
	    if (!updateSucceeded)
	    {
            strcpy ( errText, "cannot load SAPNI library" );
            return RTE_ILLEGAL_SHARED_LIB_HANDLE;
	    }

        if (sqlGetIndependentPgmPath(szDllPath, TERM_WITH_DELIMITER_EO01, &RteError)) 
#else
        if (sqlGetIndependentLibPath(szDllPath, TERM_WITH_DELIMITER_EO01, &RteError)) 
#endif
        {
            strcat( szDllPath, SAPNI_FILENAME );
            RTESys_LoadSharedLibrary (szDllPath, 
                                      RTE_LoadSymbolsLocal, 
                                      errText,
                                      sizeof(tsp00_ErrTextc) - 1,
                                      fullDllPath, 
                                      &hSAPNI);

        }
        else
        {
            strcpy ( errText, "cannot load SAPNI library" );
        }
    }
#if defined (_WIN32)
    else   /* => dwlen > 0!, i.e. sapni was found in the directory of the DLL (e.g. ODBC driver) or executable ... */
    {
		/* to find the other DLL's from Walldorf (UNICODE) extend the search path also with the path of the */
		/* DLL or executable determined above. */
        SAPDB_UInt4             envBufferLengthNew;
        SAPDB_Char *            pEnvBuffer;
        SAPDB_Char *            oldPath;

        oldPath = getenv(LDLIBPATH_IP00);
        envBufferLengthNew = (SAPDB_UInt4)(strlen (LDLIBPATH_IP00) + 1 + strlen (oldPath) + 1 + strlen (dllPath) + 1);
        pEnvBuffer = (SAPDB_Char *)alloca (envBufferLengthNew);
        if (NULL != pEnvBuffer)
        {
            strcpy (pEnvBuffer, LDLIBPATH_IP00);
            strcat (pEnvBuffer, "=");
            strcat (pEnvBuffer, oldPath);
            strcat (pEnvBuffer, ";");
            strcat (pEnvBuffer, dllPath);
            putenv (pEnvBuffer);
        }
    }
#endif

    if ( hSAPNI == RTE_ILLEGAL_SHARED_LIB_HANDLE )
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;


    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiIsLoaded",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiIsLoaded)))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }
   
    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiIsAvailable",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiIsAvailable)))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }



    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiIsInitialized",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiIsInitialized)))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiInit",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiInit) ))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiHandleToSocket",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiHandleToSocket) ))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiDataAvailable",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiDataAvailable) ))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiIsValidHandle",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiIsValidHandle) ))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiClose",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiClose) ))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiFinish",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiFinish) ))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiListen",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiListen) ))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiAccept",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiAccept) ))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiSend",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiSend) ))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiReceive",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiReceive) ))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }

    if (!RTESys_GetProcAddressFromSharedLibrary (hSAPNI,
                                                 "eo40NiConnectToServer",
                                                 errText,
                                                 sizeof(tsp00_ErrTextc),
                                                 (RTE_SharedLibProcAddress *)(&fpeo40NiConnectToServer) ))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }

    /* Load SSL Interface */
    if (!RTESec_LoadSSLFunctions (errText))
    {
        eo40NiUnLoadLib ();
        return RTE_ILLEGAL_SHARED_LIB_HANDLE;
    }

    return ( hSAPNI );
}


/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/
#if 0
externC int
eo40NiIsSaprouterSring   (  char                  *StrToCheck )
{
  return ( fpeo40NiIsSaprouterSring ( StrToCheck ) );
}

externC int
eo40NiExtractDBNodeFromSaprouterSring ( char                         *SaprouterString,
                                        tsp00_NodeIdc  VAR_ARRAY_REF  ServerNode  )
{
  return ( fpeo40NiExtractDBNodeFromSaprouterSring ( SaprouterString, ServerNode ) );
}

externC int
eo40NiExtractServiceFromSaprouterSring ( char                        *SaprouterString,
                                         unsigned short              *Service )
{
  return ( fpeo40NiExtractServiceFromSaprouterSring ( SaprouterString, Service ) );
}
#endif

externC int
eo40NiIsLoaded()
{
  return(hSAPNI != RTE_ILLEGAL_SHARED_LIB_HANDLE);
}
   
externC int
eo40NiIsAvailable()
{
  RTE_SharedLibHandle   hLib;
  tsp00_ErrTextc        errText;

  hLib = eo40NiLoadLib (errText);
  if (hLib == RTE_ILLEGAL_SHARED_LIB_HANDLE)
    return 0;

  return( fpeo40NiIsAvailable () );
}

externC int
eo40NiIsInitialized()
{
    if (0 != fpeo40NiIsInitialized)
    {
        return (fpeo40NiIsInitialized ());
    }
    else
    {
        return false;
    }
}

externC tsp00_Uint
eo40NiInit        ( tsp00_Pathc     VAR_ARRAY_REF  TraceFile,
                    int                            TraceLevel,
                    tsp00_ErrTextc  VAR_ARRAY_REF  ErrText )
{
  RTE_SharedLibHandle hLib;

  if (eo40NiIsInitialized ())
  {
      return commErrOk_esp01;
  }

  hLib = eo40NiLoadLib (ErrText);
  if (hLib == RTE_ILLEGAL_SHARED_LIB_HANDLE) 
  {
    return commErrNotOk_esp01;
  }

  return ( fpeo40NiInit ( TraceFile, TraceLevel, ErrText ) );
}

externC tsp00_Uint
eo40NiHandleToSocket        ( void                         *NiHandle ,
                              int                          *Sd,
                              tsp00_ErrTextc  VAR_ARRAY_REF  ErrText )
{
    if (0 != fpeo40NiHandleToSocket)
    {
        return ( fpeo40NiHandleToSocket ( NiHandle, Sd, ErrText ));
    }
    else
    {
        eo46BuildErrorStringRC ( ErrText, ERRMSG_NI_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
 }

externC tsp00_Uint
eo40NiDataAvailable   ( void                         *NiHandle ,
                        tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{
    if (0 != fpeo40NiDataAvailable)
    {
        return ( fpeo40NiDataAvailable ( NiHandle, pErrText ) );
    }
    else
    {
        eo46BuildErrorStringRC ( pErrText, ERRMSG_NI_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC int
eo40NiIsValidHandle      ( void                   *NiHandle )
{
    if (0 != fpeo40NiIsValidHandle)
    {
        return ( fpeo40NiIsValidHandle ( NiHandle ) );
    }
    else
    {
        return false;
    }
}

externC tsp00_Uint
eo40NiClose     ( void                         **NiHandle)
{
    if (0 != fpeo40NiClose)
    {
        return ( fpeo40NiClose (NiHandle) );
    }
    else
    {
        return commErrNotOk_esp01;
    }
}

externC tsp00_Uint
eo40NiFinish(tsp00_ErrTextc VAR_ARRAY_REF   pErrText)
{
    if (0 != fpeo40NiFinish)
    {
        tsp01_CommErr_Enum      commState = commErrOk_esp01;

        commState = fpeo40NiFinish (pErrText);
        eo40NiUnLoadLib ();
        return (commState);
    }
    else
    {
        eo46BuildErrorStringRC ( pErrText, ERRMSG_NI_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC tsp00_Uint
eo40NiListen    ( void                         **NiHandle,
                  unsigned short                *Service,
                  tsp00_ErrTextc VAR_ARRAY_REF   pErrText )
{
    if (0 != fpeo40NiListen)
    {
        return (fpeo40NiListen ( NiHandle, Service, pErrText ));
    }
    else
    {
        eo46BuildErrorStringRC ( pErrText, ERRMSG_NI_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC tsp00_Uint
eo40NiAccept    ( void                          *NiListenHandle,
                  void                         **NiCommHandle,
                  char                          *ServerNode,
                  int                            MaxServerNodeLen,
                  tsp00_ErrTextc VAR_ARRAY_REF   pErrText )
{
    if (0 != fpeo40NiAccept)
    {
        return (fpeo40NiAccept ( NiListenHandle, NiCommHandle, ServerNode, MaxServerNodeLen, pErrText ));
    }
    else
    {
        eo46BuildErrorStringRC ( pErrText, ERRMSG_NI_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC tsp00_Uint
eo40NiSend              ( void                         *SendHandle,
                          char                         *pData,
                          int                           Len,
                          tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{
    if (0 != fpeo40NiSend)
    {
        return (fpeo40NiSend ( SendHandle, pData, Len, pErrText ));
    }
    else
    {
        eo46BuildErrorStringRC ( pErrText, ERRMSG_NI_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC tsp00_Uint
eo40NiReceive ( void                         *RecvHandle,
                char                         *pData,
                tsp00_Uint                    ulMaxDataLen,
                tsp00_Uint                   *BytesReceived,
                tsp00_ErrTextc VAR_ARRAY_REF  pErrText )
{
    if (0 != fpeo40NiReceive)
    {
        return (fpeo40NiReceive ( RecvHandle, pData, ulMaxDataLen,  BytesReceived, pErrText));
    }
    else
    {
        eo46BuildErrorStringRC ( pErrText, ERRMSG_NI_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

externC tsp00_Uint
eo40NiConnectToServer   ( teo40_NiConnectInfoRecord    *pNIConnInfo,
                          tsp00_ErrTextc VAR_ARRAY_REF  ErrText )
{
    if (0 != fpeo40NiConnectToServer)
    {
        return (fpeo40NiConnectToServer ( pNIConnInfo, ErrText ));
    }
    else
    {
        eo46BuildErrorStringRC ( ErrText, ERRMSG_NI_FUNCTION_NOT_LOADED, 0);
        return commErrNotOk_esp01;
    }
}

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#endif /* STUB_MODULE */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
