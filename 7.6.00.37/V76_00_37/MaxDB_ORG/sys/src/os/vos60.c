/*!
  @file           vos60.c
  @author         RaymondR
  @brief          message output
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
#include "gos00.h"
#include "heo00.h"
#include "heo46.h"
#include "hsp77.h"

#if defined (KERNEL)
# include "gos00k.h"                    /* nocheck */  
# include "hsp100.h"                    /* nocheck */
# include	"RunTime/RTE_Speed.h"       /* nocheck */
# include "geo67.h"                     /* nocheck */
# include "Messages/Msg_OutputKnlDiag.h" /* nocheck */
# include "SAPDB/RunTime/RTE_DiagPipe.h" /* nocheck */
#endif

#include "geo007_1.h"
#include "geo007_2.h"
#include "gos44.h"
#include "heo01.h"
#include "geo00_2.h"
#include "RunTime/System/RTESys_Time.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageOutput.h"


#include           <stdio.h>
#include           <stdarg.h>

/*
 *  DEFINES
 */

#define MOD__  "VOS60C : "
#define MF__   MOD__"UNDEFINED"
#define KILO_BYTES    1024

#define WRITE_TO_CONSOLE              0x01
#define WRITE_TO_DIAGFILE             0x02
#define WRITE_TO_EVENTLOG             0x04
#define WRITE_TO_APPL_DIAG            0x08
#define WRITE_TO_MSG_BOX              0x10
#define WRITE_TO_GUI_CONSOLE          0x20
#define WRITE_TO_ERR_DIAGFILE         0x40

                                      // --- Don't include 'WRITE_TO_MSG_BOX'
#define WRITE_TO_ALL                  WRITE_TO_CONSOLE      |  \
                                      WRITE_TO_DIAGFILE     |  \
                                      WRITE_TO_EVENTLOG     |  \
                                      WRITE_TO_APPL_DIAG    |  \
                                      WRITE_TO_GUI_CONSOLE  |  \
                                      WRITE_TO_ERR_DIAGFILE

// --- set default output devices
#if defined (WINCOMPO)
 #define DEFAULT_OUTPUT_DEVICE        WRITE_TO_MSG_BOX
#elif defined (USER)
 #define DEFAULT_OUTPUT_DEVICE        WRITE_TO_APPL_DIAG
#elif defined (SERVER) || defined (COMPO) || defined (KERNEL)
 #define DEFAULT_OUTPUT_DEVICE        WRITE_TO_CONSOLE
#else
 #define DEFAULT_OUTPUT_DEVICE        0
#endif


#define EVENT_SEVERITY_INFORMATIONAL  ((DWORD)0x40000000L)
#define EVENT_SEVERITY_WARNING        ((DWORD)0x80000000L)
#define EVENT_SEVERITY_ERROR          ((DWORD)0xC0000000L)


#if defined (SERVER)
 #define EVENT_CATEGORY               SQLMSG_CATEGORY_XSERVER
#elif defined (KERNEL)
 #define EVENT_CATEGORY               kgs.dwEventCategory
#else
 #define EVENT_CATEGORY               SQLMSG_CATEGORY_CLIENT
#endif


#define DF_RENAME_OLD_DIAGFILE        0x01
#define DF_APPEND_DIAG_OUTPUT         0x02


/*
 *  MACROS
 */
#define  ENTER_CRIT_SEC()                                                    \
           if (!CritSecInitialized) { CritSecInitialized = TRUE;             \
             InitializeCriticalSection(&CritSec); }                          \
           EnterCriticalSection(&CritSec); CritSecCount++;

#define  EXIT_CRIT_SEC()                                                     \
           if (CritSecCount) { CritSecCount--;                               \
             LeaveCriticalSection(&CritSec); }

#define  DELETE_CRIT_SEC()                                                   \
            { DeleteCriticalSection(&CritSec); \
              CritSecInitialized = FALSE; }

/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */
typedef struct diag_file_record
  {
  HANDLE    FileHandle;
  PATHNAME  szDiagFileName;
  PATHNAME  szPhysDiagFileName;
  BOOL      fStartOfWriteCycle;
  ULONG     ulBegOfWrtCycle;
  ULONG     ulWrtPos;
  ULONG     ulEndOfWrtCycle;
  } DIAG_FILE_REC;

typedef DIAG_FILE_REC  *PDIAG_FILE_REC;


/*
 * EXTERNAL VARIABLES
 */

/*
 *  EXPORTED VARIABLES
 */



/*
 * LOCAL VARIABLES
 */
#if defined(_WIN32)
static HANDLE           hEventSource        = NULL;
static PTOKEN_USER      pTU                 = NULL;
static CHAR             szEventLogTitle[80 + 1];
#endif

static BOOL             fUserDiagFileEnabled = FALSE;
static tsp00_Pathc      szUserDiagFileName;
static BOOL             fMessOutputDisabled  = FALSE;
static CRITICAL_SECTION CritSec;
static BOOL             CritSecInitialized   = FALSE;
static INT              CritSecCount         = 0;
static DIAG_FILE_REC    dfDiagFile           = { (HANDLE)INVALID_HANDLE_VALUE,
                                                 "", "", FALSE,
                                                 0, 0, 0 };
static DIAG_FILE_REC    dfUtilDiagFile       = { (HANDLE)INVALID_HANDLE_VALUE,
                                                 "", "", FALSE,
                                                 0, 0, 0 };
static DIAG_FILE_REC    dfEventFile          = { (HANDLE)INVALID_HANDLE_VALUE,
                                                 "", "", FALSE,
                                                 0, 0, 0 };
static HANDLE           hfErrDiag            = (HANDLE)INVALID_HANDLE_VALUE;
static PMSGFUNCTYPE     pGUIMessageFunc      = NULL;
static ULONG            ulOutput             = DEFAULT_OUTPUT_DEVICE;


/*
 * LOCAL FUNCTION PROTOTYPES
 */

#if defined(_WIN32)
 LONG sql60_write_event_log   ( char *                pszHeader,
                                const char *          pszMsg,
                                char *                pszEventLogTitle,
                                LONG                  lMsgID,
                                HANDLE                hEventSource,
                                ULONG                 ulEventType );
#endif

LONG  sql60_get_cons_handle   ( PHANDLE               phfCon );
LONG  sql60_open_error_diag   ( char *                   pszDiagFileName,
                                PSECURITY_ATTRIBUTES  pSA,
                                HANDLE                *FileHandle );
LONG  sql60_create_diag       ( char *                   pszHeaderInfo,
                                char *                   pszDiagFileName,
                                ULONG                 ulDiagOptions,
                                ULONG                 ulDiagSize,
                                PSECURITY_ATTRIBUTES  pSA,
                                PDIAG_FILE_REC        pDiagInfo,
                                BOOL                  fWriteThrough );
LONG  sql60_write_con         ( char *                   pszHeader,
                                char *                   pszMsg );
LONG  sql60_write_GUI_con     ( char *                   pszHeader,
                                char *                   pszMsg );
LONG  sql60_write_mess_box    ( LONG                  lMsgID,
                                ULONG                 ulEventType,
                                const char *          pszMsg );
LONG  sql60_write_diag        ( char *                pszHeader,
                                const char *          pszMsg,
                                ULONG                 ulMsgLen,
                                PDIAG_FILE_REC        pDiagInfo );
LONG  sql60_write_appl_diag   ( char *                pszHeader,
                                const char *          pszMsg );
LONG  sql60_init_diag_file    ( char *                   pszHeaderInfo,
                                ULONG                 ulDiagOptions,
                                ULONG                 ulDiagSize,
                                PDIAG_FILE_REC        pDiagInfo );
LONG  sql60_write_error_diag  ( char *                pszHeader,
                                const char *          pszMsg,
                                HANDLE                FileHandle );
VOID  sql60_deliver_msg       ( ULONG                 ulOutputRequest,
                                LONG                  lMsgID,
                                ULONG                 ulEventType,
                                const char *          pszLabel,
                                const char *          pszFormatStr,
                                va_list               VarArgs );
LONG  sql60_emergency_write   ( const char *          pszMsg,
                                LONG                  lMsgID,
                                const char *          pszLabel,
                                ULONG                 ulEventType );
LONG  sql60_user_diag_opt     ( BOOL                  *pfEnabled,
                                char *                pszUserDiagName,
                                LONG                  lMxUserDiagName );


LONG  sql60_open_add_diag_file  ( char *                  pszFileName,
			                      		  ULONG                ulFileSize,
                                  PSECURITY_ATTRIBUTES pSA,
                    			        PDIAG_FILE_REC       pdfDiagnoseFile,
                                        BOOL                 fEventFile);
								      		
VOID  sql60_close_diagnose_f    ( PDIAG_FILE_REC       pdfDiagnoseFile ) ;

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

LONG sql60_install_event_source ( char *   pszNodeName,
                                  char *   pszEventLogSource,
                                  char *   pszEventFile,
                                  char *   pszCategoryFile,
                                  ULONG ulCategories) 
  {
  #undef  MF__
  #define MF__ MOD__"sql60_install_event_source"
  LONG                    rc          = NO_ERROR;
  REG_ENTRY_REC           RegistryEntries[4];
  DWORD                   dwTypesSupported = 0;
  DWORD                   dwCategoryCount  = 0;
  PATHNAME                szKey;
  ULONG                   ulEntries        = 0;


  DBGIN;

  //
  // --- insert message dll entry into the eventlog subkey of
  //     registry database
  //
  strcpy ( szKey, REG_SK_EVENT_LOG );
  strcat ( szKey, "\\" );
  strcat ( szKey, pszEventLogSource );

  ulEntries = 0;
  RegistryEntries[ulEntries].pszValueName = REG_VN_EV_TYPES;
  RegistryEntries[ulEntries].pValue       = &dwTypesSupported;
  RegistryEntries[ulEntries].ulValueSize  = sizeof (DWORD);
  RegistryEntries[ulEntries].ulValueType  = REG_DWORD;
  RegistryEntries[ulEntries].rc           = NO_ERROR;
  ulEntries++;

  if ( pszCategoryFile != NULL)
    {
    RegistryEntries[ulEntries].pszValueName = REG_VN_CATEGORY_COUNT;
    RegistryEntries[ulEntries].pValue       = &dwCategoryCount;
    RegistryEntries[ulEntries].ulValueSize  = sizeof (DWORD);
    RegistryEntries[ulEntries].ulValueType  = REG_DWORD;
    RegistryEntries[ulEntries].rc           = NO_ERROR;
    ulEntries++;
    }

  sql50_reg_get_service_values ( pszNodeName, szKey, ulEntries, RegistryEntries );

  if ( pszCategoryFile != NULL)
    {
    if ( dwCategoryCount < ulCategories )
      dwCategoryCount = ulCategories;
    }

  dwTypesSupported = dwTypesSupported      |
                     EVENTLOG_ERROR_TYPE   |
                     EVENTLOG_WARNING_TYPE |
                     EVENTLOG_INFORMATION_TYPE;

  ulEntries = 0;
  RegistryEntries[ulEntries].pszValueName = REG_VN_MSG_FILE;
  RegistryEntries[ulEntries].pValue       = pszEventFile;
  RegistryEntries[ulEntries].ulValueSize  = (ULONG)strlen(pszEventFile) + 1;
  RegistryEntries[ulEntries].ulValueType  = REG_EXPAND_SZ;
  ulEntries++;

  RegistryEntries[ulEntries].pszValueName = REG_VN_EV_TYPES;
  RegistryEntries[ulEntries].pValue       = &dwTypesSupported;
  RegistryEntries[ulEntries].ulValueSize  = sizeof (DWORD);
  RegistryEntries[ulEntries].ulValueType  = REG_DWORD;
  ulEntries++;

  if ( pszCategoryFile != NULL)
    {
    RegistryEntries[ulEntries].pszValueName = REG_VN_CATEGORY_FILE;
    RegistryEntries[ulEntries].pValue       = pszCategoryFile;
    RegistryEntries[ulEntries].ulValueSize  = (ULONG)strlen(pszCategoryFile) + 1;
    RegistryEntries[ulEntries].ulValueType  = REG_EXPAND_SZ;
    ulEntries++;

    RegistryEntries[ulEntries].pszValueName = REG_VN_CATEGORY_COUNT;
    RegistryEntries[ulEntries].pValue       = &dwCategoryCount;
    RegistryEntries[ulEntries].ulValueSize  = sizeof (DWORD);
    RegistryEntries[ulEntries].ulValueType  = REG_DWORD;
    ulEntries++;
    }

  rc = sql50_reg_put_service_values ( pszNodeName, szKey, ulEntries,
                                      RegistryEntries );

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sql60_remove_event_source ( char *  pszNodeName,
                                 char *  pszEventLogSource )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_remove_event_source"
  LONG                    rc          = NO_ERROR;
  PATHNAME                szKey;

  DBGIN;

  strcpy ( szKey, REG_SK_EVENT_LOG );
  strcat ( szKey, "\\" );
  strcat ( szKey, pszEventLogSource );

  rc = sql50_reg_del_service_key ( NULL, szKey );

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sql60_open_event_log ( char *    pszEventLogTitle,
                            char *    pszEventLogSource )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_open_event_log"
  LONG         rc          = NO_ERROR;

  DBGIN;


  #if defined(_WIN32)
   #if defined (KERNEL) || defined (SERVER)
    // --- get user information
    rc = sql49c_get_token_information ( &pTU, NULL, NULL );

    if ( rc != NO_ERROR )
     pTU = NULL;
   #endif

   szEventLogTitle[sizeof(szEventLogTitle) - 1] = '\0';
   strncpy ( szEventLogTitle, pszEventLogTitle, sizeof(szEventLogTitle) - 4 );

   hEventSource = RegisterEventSource( NULL, pszEventLogSource );

   if ( hEventSource == NULL )
     {
     rc = GetLastError();
     sql60_msg_all ( ERR_CANT_OPEN_EVENT_LOG, rc );
     }
   else
     ulOutput |= WRITE_TO_EVENTLOG;

  #endif

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sql60_reopen_event_log ( char *    pszEventLogTitle,
                              char *    pszEventLogSource )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_reopen_event_log"
  LONG         rc  = NO_ERROR;
  HANDLE       hTempEventSource;

  DBGIN;

  #if defined(_WIN32)
   #if defined (KERNEL) || defined (SERVER)
    if ( pTU == NULL )
      {
      // --- get user information
      rc = sql49c_get_token_information ( &pTU, NULL, NULL );

      if ( rc != NO_ERROR )
        pTU = NULL;
      }
   #endif

   hTempEventSource = hEventSource;

   szEventLogTitle[sizeof(szEventLogTitle) - 1] = '\0';
   strncpy ( szEventLogTitle, pszEventLogTitle, sizeof(szEventLogTitle) - 4 );

   hEventSource = RegisterEventSource( NULL, pszEventLogSource );

   if ( hEventSource == NULL )
     {
     rc = GetLastError();
     sql60_msg_all ( ERR_CANT_OPEN_EVENT_LOG, rc );

     if ( pTU != NULL )
       FREE_MEM (pTU);
     }
   else
     {
     ulOutput |= WRITE_TO_EVENTLOG;
     
     if ( hTempEventSource != NULL )
       DeregisterEventSource( hTempEventSource );
     }

  #endif

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/


LONG sql60_open_diag_file ( char *                  pszDiagFileName,
                            ULONG                ulDiagSize,
                            PSECURITY_ATTRIBUTES pSA )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_open_diag_file"
  LONG                  rc          = NO_ERROR;
  ULONG                 ulDiagDevType;

  DBGIN;

  if ( ulOutput & WRITE_TO_DIAGFILE )
    {
    // --- already open!
    DBGOUT;
    return ( NO_ERROR );
    }

  if ( pszDiagFileName[0] == '\0' )
    {
    sql60_msg_all ( ERR_WRONG_DIAG_FILE_NAME, pszDiagFileName );
    DBGOUT;
    return ( ERROR_INVALID_PARAMETER );
    }

  if ( ulDiagSize == 0 )
    {
    sql60_msg_all ( ERR_WRONG_DIAG_FILE_SIZE, pszDiagFileName, ulDiagSize );
    DBGOUT;
    return ( ERROR_INVALID_PARAMETER );
    }


  // --- get device type  (Console, LPTx, COMx, PRN ...)
  ulDiagDevType = sql44c_get_dev_type_by_filename (pszDiagFileName);

  switch (ulDiagDevType)
    {
    case DT_OTHER:
      ulDiagSize *= KILO_BYTES;

      rc = sql60_create_diag ( BIG_HEADER_INFO_LINE,
                               pszDiagFileName,
                               DF_RENAME_OLD_DIAGFILE,
                               ulDiagSize,
                               pSA,
                               &dfDiagFile,
                               false );

      if ( rc == NO_ERROR )
        ulOutput |= WRITE_TO_DIAGFILE;
      break;

    default:
      sql60_msg_all ( ERR_WRONG_DIAG_FILE_NAME, pszDiagFileName );
      DBGOUT;
      rc = ERROR_INVALID_PARAMETER;
      break;
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sql60_open_util_diag_file ( char *                  pszUtilDiagFileName,
                                 ULONG                ulDiagSize,
                                 PSECURITY_ATTRIBUTES pSA )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_open_util_diag_file"
  LONG                  rc    = NO_ERROR;

  DBGIN;

  rc = sql60_open_add_diag_file ( pszUtilDiagFileName,
								  ulDiagSize, pSA, &dfUtilDiagFile, false ) ;

  DBGOUT;
  return ( rc );
  }


/*------------------------------*/

LONG sql60_open_event_file ( char *                  pszEventFileName,
                             ULONG                ulFileSize,
                             PSECURITY_ATTRIBUTES pSA )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_open_event_file"
  LONG                  rc    = NO_ERROR;

  DBGIN;

  DELETE_FILE( pszEventFileName );

  rc = sql60_open_add_diag_file ( pszEventFileName,
								  ulFileSize, pSA, &dfEventFile, true ) ;

  DBGOUT;
  return ( rc );
  }


/*------------------------------*/

LONG sql60_open_error_diag_file ( char *                  pszDiagFileName,
                                  PSECURITY_ATTRIBUTES pSA )

  {
  #undef  MF__
  #define MF__ MOD__"sql60_open_error_diag_file"
  LONG                  rc    = NO_ERROR;
  ULONG                 ulDiagDevType;

  DBGIN;

  if ( ulOutput & WRITE_TO_ERR_DIAGFILE )
    {
    // --- already open!
    DBGOUT;
    return ( rc );
    }

  if ( pszDiagFileName[0] == '\0' )
    {
    sql60_msg_all ( ERR_WRONG_DIAG_FILE_NAME, pszDiagFileName );
    DBGOUT;
    return ( ERROR_INVALID_PARAMETER );
    }

  // --- get device type  (Console, LPTx, COMx, PRN ...)
  ulDiagDevType = sql44c_get_dev_type_by_filename (pszDiagFileName);

  switch (ulDiagDevType)
    {
    case DT_OTHER:
      rc = sql60_open_error_diag ( pszDiagFileName, pSA, &hfErrDiag );

      if ( rc == NO_ERROR )
        ulOutput |= WRITE_TO_ERR_DIAGFILE;

      break;

    default:
      sql60_msg_all ( ERR_WRONG_DIAG_FILE_NAME, pszDiagFileName );
      DBGOUT;
      rc = ERROR_INVALID_PARAMETER;
      break;
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

VOID sql60_get_diag_filename ( char * *ppszPhysDiagFileName )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_get_diag_filename"

  DBGPAS;

  if ( dfDiagFile.szPhysDiagFileName[0] != '\0' )
    *ppszPhysDiagFileName = dfDiagFile.szPhysDiagFileName;
  else
    *ppszPhysDiagFileName = NULL;

  return;
  }

/*------------------------------*/

VOID sql60_enable_GUI_console ( PMSGFUNCTYPE pMessageFunc )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_open_GUI_console"

  DBGPAS;

  pGUIMessageFunc  = pMessageFunc;
  ulOutput        |= WRITE_TO_GUI_CONSOLE;

  return;
  }

/*------------------------------*/

VOID sql60_enable_default_device ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_enable_default_device"

  DBGPAS;

  ulOutput |= DEFAULT_OUTPUT_DEVICE;

  return;
  }

/*------------------------------*/

VOID sql60_enable_message_box ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_enable_message_box"

  DBGPAS;

  ulOutput |= WRITE_TO_MSG_BOX;

  return;
  }

/*------------------------------*/

VOID sql60_enable_console ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_enable_console"

  DBGPAS;

  ulOutput |= WRITE_TO_CONSOLE;

  return;
  }

/*------------------------------*/

VOID sql60_enable_message_output ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_enable_message_output"

  DBGPAS;

  fMessOutputDisabled = FALSE;

  return;
  }

/*------------------------------*/

VOID sql60_close_event_log ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_close_event_log"

  DBGPAS;

  #if defined(_WIN32)
   if ( hEventSource != NULL )
     {
     if ( pTU != NULL )
       FREE_MEM (pTU);

     DeregisterEventSource( hEventSource );
     hEventSource  = NULL;
     ulOutput     &= ~WRITE_TO_EVENTLOG;
     }
  #endif
  }

/*------------------------------*/

VOID sql60_close_util_diag_file (VOID)
  {
  #undef  MF__
  #define MF__ MOD__"sql60_close_util_diag_file"

  DBGPAS;

  sql60_close_diagnose_f ( &dfUtilDiagFile ) ;

  return;
  }

/*------------------------------*/

VOID sql60_close_event_file (VOID)
  {
  #undef  MF__
  #define MF__ MOD__"sql60_close_event_diag_file"

  DBGPAS;

  sql60_close_diagnose_f ( &dfEventFile ) ;

  return;
  }

/*------------------------------*/

VOID sql60_close_diag_file (VOID)
  {
  #undef  MF__
  #define MF__ MOD__"sql60_close_diag_file"
 
  DBGPAS;

  ulOutput                        &= ~WRITE_TO_DIAGFILE;

  sql60_close_diagnose_f ( &dfDiagFile ) ;

  return;
  }



/*------------------------------*/

VOID sql60_close_error_diag_file (VOID)
  {
  #undef  MF__
  #define MF__ MOD__"sql60_close_error_diag_file"
  HANDLE      hfTmpDiag;

  DBGPAS;
  
  ENTER_CRIT_SEC();

  if ( hfErrDiag != (HANDLE) INVALID_HANDLE_VALUE )
    {
    hfTmpDiag                = dfDiagFile.FileHandle;
    ulOutput                &= ~WRITE_TO_ERR_DIAGFILE;
    EXIT_CRIT_SEC();

    SLEEP(0);
    CLOSE_FILE (hfTmpDiag);
    }
  else
    EXIT_CRIT_SEC();

  return;
  }

/*------------------------------*/

VOID sql60_disable_GUI_console( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_disable_GUI_console"

  DBGPAS;

  ulOutput &= ~WRITE_TO_GUI_CONSOLE;

  return;
  }

/*------------------------------*/

VOID sql60_disable_message_box ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_disable_message_box"

  DBGPAS;

  ulOutput &= ~WRITE_TO_MSG_BOX;

  return;
  }

/*------------------------------*/

VOID sql60_disable_console ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_disable_console"

  DBGPAS;

  ulOutput &= ~WRITE_TO_CONSOLE;

  return;
  }

/*------------------------------*/


VOID sql60_disable_default_device ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_disable_default_device"

  DBGPAS;

  ulOutput &= ~DEFAULT_OUTPUT_DEVICE;

  return;
  }

/*------------------------------*/

VOID sql60_disable_message_output ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_disable_message_output"

  DBGPAS;

  fMessOutputDisabled = TRUE;

  return;
  }

/*------------------------------*/

VOID _System  sql60_int_err_msg_prio7 ( const char *  pszFormatStr, ... )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_int_err_msg_prio7"
  va_list               args;
  CHAR                  szMsg   [MAX_MSG_LINE_LEN];

  DBGPAS;

  va_start(args, pszFormatStr);

  ENTER_CRIT_SEC();
  _vsnprintf(szMsg, sizeof(szMsg) - 1, pszFormatStr, args );
  EXIT_CRIT_SEC();

  va_end(args);

  sql60_msg_all ( ERR_INT_ERR_MSG_PRIO7, szMsg );

  return;
  }

/*------------------------------*/

VOID _System sql60_msg_all ( LONG  lMsgID,
                             ULONG ulEventType,
                             const char * pszLabel,
                             const char * pszFormatStr, ...)
  {
  #undef  MF__
  #define MF__ MOD__"sql60_msg_all"
  va_list     args;
  ULONG       ulOutputRequest;

  DBGIN;

  ulOutputRequest = WRITE_TO_ALL;

  if ( ulEventType == INFO_TYPE )
    ulOutputRequest = WRITE_TO_ALL & ~WRITE_TO_MSG_BOX;

  if (( ulEventType != IERR_TYPE ) &&
      ( ulEventType != ERR_TYPE ))
    {
    ulOutputRequest = ulOutputRequest & ~WRITE_TO_ERR_DIAGFILE; 
    }

  va_start ( args, pszFormatStr);
#ifdef NEW_DIAG_FILES
  Msg_OutputToKnlDiagVArgs( lMsgID,ulEventType,pszLabel,pszFormatStr,args);
#endif
#ifndef NO_OLD_DIAG_FILES
  sql60_deliver_msg ( ulOutputRequest, lMsgID, ulEventType, pszLabel, pszFormatStr, args );
#endif
  va_end ( args );

  DBGOUT;
  return;
  }

/*------------------------------*/

VOID _System sql60_msg_box ( LONG  lMsgID,
                             ULONG ulEventType,
                             const char * pszLabel,
                             const char * pszFormatStr, ...)
  {
  #undef  MF__
  #define MF__ MOD__"sql60_msg_box"
  va_list     args ;

  DBGIN;

  va_start ( args, pszFormatStr);
  sql60_deliver_msg ( WRITE_TO_MSG_BOX, lMsgID, ulEventType, pszLabel, pszFormatStr, args );
  va_end ( args );

  DBGOUT;
  return;
  }

/*------------------------------*/

VOID _System sql60_msg_con_diag ( LONG  lMsgID,
                                  ULONG ulEventType,
                                  const char * pszLabel,
                                  const char * pszFormatStr, ...)
  {
  #undef  MF__
  #define MF__ MOD__"sql60_msg_con_diag"
  va_list     args;
  ULONG       ulOutputRequest;

  DBGIN;

  ulOutputRequest = WRITE_TO_CONSOLE     |
                    WRITE_TO_GUI_CONSOLE |
                    WRITE_TO_APPL_DIAG   |
                    WRITE_TO_DIAGFILE;

  if (( ulEventType == IERR_TYPE ) ||
      ( ulEventType == ERR_TYPE ))
    {
    ulOutputRequest |= WRITE_TO_ERR_DIAGFILE;
    }

  va_start ( args, pszFormatStr);
#ifdef NEW_DIAG_FILES
  Msg_OutputToKnlDiagVArgs( lMsgID,ulEventType,pszLabel,pszFormatStr,args);
#endif
#ifndef NO_OLD_DIAG_FILES
  sql60_deliver_msg ( ulOutputRequest, lMsgID, ulEventType, pszLabel, pszFormatStr, args );
#endif
  va_end ( args );

  DBGOUT;
  return;
  }

/*------------------------------*/

VOID _System sql60_msg_diag ( LONG  lMsgID,
                              ULONG ulEventType,
                              const char * pszLabel,
                              const char * pszFormatStr, ...)
  {
  #undef  MF__
  #define MF__ MOD__"sql60_msg_diag"
  va_list     args;
  ULONG       ulOutputRequest;

  DBGIN;

  ulOutputRequest = WRITE_TO_APPL_DIAG   |
                    WRITE_TO_DIAGFILE;

  if (( ulEventType == IERR_TYPE ) ||
      ( ulEventType == ERR_TYPE ))
    {
    ulOutputRequest |= WRITE_TO_ERR_DIAGFILE;
    }

  va_start ( args, pszFormatStr);
#ifdef NEW_DIAG_FILES
  Msg_OutputToKnlDiagVArgs( lMsgID,ulEventType,pszLabel,pszFormatStr,args);
#endif
#ifndef NO_OLD_DIAG_FILES
  sql60_deliver_msg ( ulOutputRequest, lMsgID, ulEventType, pszLabel, pszFormatStr, args );
#endif
  va_end ( args );


  DBGOUT;
  return;
  }

/*------------------------------*/

VOID _System sql60_msg_con ( LONG  lMsgID,
                             ULONG ulEventType,
                             const char * pszLabel,
                             const char * pszFormatStr, ...)
  {
  #undef  MF__
  #define MF__ MOD__"sql60_msg_con"
  va_list     args;
  ULONG       ulOutputRequest;

  DBGIN;

  ulOutputRequest = WRITE_TO_CONSOLE     |
                    WRITE_TO_GUI_CONSOLE;

  if (( ulEventType == IERR_TYPE ) ||
      ( ulEventType == ERR_TYPE ))
    {
    ulOutputRequest |= WRITE_TO_ERR_DIAGFILE;
    }

  va_start ( args, pszFormatStr);
  sql60_deliver_msg ( ulOutputRequest, lMsgID, ulEventType, pszLabel, pszFormatStr, args );
  va_end ( args );

  DBGOUT;
  return;
  }

/*------------------------------*/

VOID _System sql60_msg_event_log ( LONG  lMsgID,
                                   ULONG ulEventType,
                                   const char * pszLabel,
                                   const char * pszFormatStr, ...)
  {
  #undef  MF__
  #define MF__ MOD__"sql60_msg_event_log"
  va_list     args;
  ULONG       ulOutputRequest;

  DBGIN;

  ulOutputRequest = WRITE_TO_EVENTLOG;

  va_start ( args, pszFormatStr);
  sql60_deliver_msg ( ulOutputRequest, lMsgID, ulEventType, pszLabel, pszFormatStr, args );
  va_end ( args );

  DBGOUT;
  return;
  }

/*------------------------------*/

VOID _System sql60_msg_con_event_log ( LONG  lMsgID,
                                       ULONG ulEventType,
                                       const char * pszLabel,
                                       const char * pszFormatStr, ...)
  {
  #undef  MF__
  #define MF__ MOD__"sql60_msg_con_event_log"
  va_list     args;
  ULONG       ulOutputRequest;

  DBGIN;

  ulOutputRequest = WRITE_TO_CONSOLE     |
                    WRITE_TO_GUI_CONSOLE |
                    WRITE_TO_EVENTLOG;

  va_start ( args, pszFormatStr);
  sql60_deliver_msg ( ulOutputRequest, lMsgID, ulEventType, pszLabel, pszFormatStr, args );
  va_end ( args );

  DBGOUT;
  return;
  }

/*------------------------------*/
#ifndef NO_OLD_DIAG_FILES
VOID _System sql60_msg_prio_x ( ULONG ulPrio,
                                LONG  lMsgID,
                                ULONG ulEventType,
                                const char *   pszLabel,
                                const char *   pszFormatStr, ...)
  {
  #undef  MF__
  #define MF__ MOD__"sql60_msg_prio_x"
  ULONG       ulOutputRequest = 0;
  BOOL        fWriteToDiagFile;
  va_list     args;

  DBGIN;

  fWriteToDiagFile = ulOutput & (WRITE_TO_APPL_DIAG |
                                 WRITE_TO_DIAGFILE  |
                                 WRITE_TO_ERR_DIAGFILE);

  if ((ulPrio == 0) || (ulPrio == 6))
    ulOutputRequest |= WRITE_TO_EVENTLOG;

  if ((ulPrio == 0) || (ulPrio == 1) ||
      (ulPrio == 6) || (ulPrio == 7) || !fWriteToDiagFile )
    {
    ulOutputRequest |= WRITE_TO_CONSOLE     |
                       WRITE_TO_GUI_CONSOLE;
    }

  if ( fWriteToDiagFile )
    {
    ulOutputRequest |= WRITE_TO_APPL_DIAG |
                       WRITE_TO_DIAGFILE;
    }

  if (( ulEventType == IERR_TYPE ) ||
      ( ulEventType == ERR_TYPE ))
    {
    ulOutputRequest |= WRITE_TO_ERR_DIAGFILE;
    }


  va_start ( args, pszFormatStr);

  sql60_deliver_msg ( ulOutputRequest, lMsgID, ulEventType, pszLabel, pszFormatStr, args );

  va_end ( args );

  DBGOUT;
  return;
  }
#endif
/*------------------------------*/

VOID 
sql60_strt_msg_wrt_cycle
( VOID )
{
  #undef  MF__
  #define MF__ MOD__"sql60_strt_msg_wrt_cycle"
  DBGIN;
#ifdef NEW_DIAG_FILES  
  Msg_StartupPhaseCompleted();
#endif
  dfDiagFile.fStartOfWriteCycle = TRUE;
  DBGOUT;

  return;
  }

/*------------------------------*/

LONG sql60_write_to_util_diag ( const char *    pszMsg,
                                ULONG           ulMsgLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_strt_msg_wrt_cycle"
  LONG        rc = NO_ERROR;
  CHAR        szHeader [MAX_DIAG_HEADER_LEN];
  DBGIN;

  sql60_create_diag_header ( 0, 0, NULL, time_diag_header, szHeader );

  rc = sql60_write_diag ( szHeader, pszMsg, ulMsgLen, &dfUtilDiagFile );

  if ( rc != NO_ERROR )
    {
    sql60_close_util_diag_file ();
    sql60_msg_all ( ERR_WRITING_DIAG_FILE, dfUtilDiagFile.szDiagFileName , rc );
    }


  DBGOUT;
  return ( rc );
  }


/*------------------------------*/

LONG sql60_write_to_event_file ( const char *    pszMsg,
                                 ULONG           ulMsgLen )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_write_to_event_file"
  LONG        rc = NO_ERROR;
  CHAR        szHeader [MAX_DIAG_HEADER_LEN];
  DBGIN;

  if ( dfEventFile.FileHandle != (HANDLE)INVALID_HANDLE_VALUE )
    {
    *szHeader = '\0' ;
/*    sql60_create_diag_header ( 0, 0, NULL, time_diag_header, szHeader ); */

    rc = sql60_write_diag ( szHeader, pszMsg, ulMsgLen, &dfEventFile );

    if ( rc != NO_ERROR )
      {
      sql60_close_event_file ();
      sql60_msg_all ( ERR_WRITING_DIAG_FILE, dfEventFile.szDiagFileName , rc );
      }
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

VOID sql60_reset_crit_section( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_reset_crit_section"

  DBGPAS;

  EXIT_CRIT_SEC();

  return;
  }

/*------------------------------*/

VOID sql60_release_resources( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_release_resources"

  DBGPAS;

  DELETE_CRIT_SEC();

  return;
  }

/*
 * ==================================================================
 */
#if defined(KERNEL)
externC void 
vksprintf ( tsp00_Addr pszMsg, const tsp00_Addr pszFormatStr, ...)
{
#undef  MF__
#define MF__ MOD__"vksprintf"
  va_list     args ;

  DBGIN;

  ENTER_CRIT_SEC();

  va_start ( args, pszFormatStr);

  vsprintf ( pszMsg, pszFormatStr , args );

  va_end ( args );

  EXIT_CRIT_SEC();

  DBGOUT;

  return;
}
#endif

/*------------------------------*/

VOID sql60_create_diag_header ( LONG                 lMsgID,
                                ULONG                ulEventType,
                                const char *         pszLabel,
                                DIAG_HEADER_TYPE     HeaderType,
                                char *               pszOutBuffer ) // - use 'MAX_DIAG_HEADER_LEN'
                                                                    //   as minimum buffer size
  {
  #undef  MF__
  #define MF__ MOD__"sql60_create_diag_header"
  RTESys_Timestamp timeBuffer;
  CHAR          szTid[12];
  char *           pszTid;
  ULONG         ulTid  = 0;
  static CHAR   szPid[12];
  static char *    pszPid = szPid;
  static ULONG  ulPid  = 0;
  char *           pszType;
  char *           pszLongType;

  DBGIN;

  if ( ulPid == 0 )
    {
    GETPROCESSID(&ulPid);

    szPid[0] = '0';
    szPid[1] = 'x';
    _ultoa ( ulPid, szPid + 2, 16 );
    strupr ( szPid + 2 );

    if ( strlen(szPid) > 10 )
      pszPid = szPid + 2;
    else
      pszPid = szPid;
    }

  switch ( ulEventType )
    {
    case IERR_TYPE:
      //pszType     = "IER";
      pszType     = "ERR";
      pszLongType = IERR_LONG_DESCRIPTION;
      break;
    case ERR_TYPE:
      pszType     = "ERR";
      pszLongType = ERR_LONG_DESCRIPTION;
      break;
    case WRN_TYPE:
      pszType     = "WRN";
      pszLongType = WRN_LONG_DESCRIPTION;
      break;
    case INFO_TYPE:
      pszType     = "";
      pszLongType = INFO_LONG_DESCRIPTION;
      break;
    default:
      pszType     = "???";
      pszLongType = "???";
      break;
    }

  switch ( HeaderType )
    {
    case small_diag_header:
      #if defined(_WIN32)
        wsprintf (pszOutBuffer, SMALL_HEADER_FORMAT_STR, lMsgID, pszLongType);
      #else
        ENTER_CRIT_SEC();
        sprintf (pszOutBuffer, SMALL_HEADER_FORMAT_STR, lMsgID, pszLongType);
        EXIT_CRIT_SEC();
      #endif
      break;

    case event_log_header:
      #if defined(_WIN32)

       ulTid = THREADID;
       szTid[0] = '0';
       szTid[1] = 'x';
       _ultoa ( ulTid, szTid + 2, 16 );
       strupr ( szTid + 2 );

       if ( strlen(szTid) > 10 )
         pszTid = szTid + 2;
       else
         pszTid = szTid;

       ENTER_CRIT_SEC();
       wsprintf( pszOutBuffer,
                 E_LOG_HEADER_FORMAT_STR,
                 pszLongType,
                 pszTid,
                 pszPid,
                 lMsgID );

       DBG3((MF__, "Header = '%s'", pszOutBuffer));
       EXIT_CRIT_SEC();
      #endif
     break;

    case big_diag_header:
      /*
        * Print Date and Time
        */
       ulTid = THREADID;
       szTid[0] = '0';
       szTid[1] = 'x';
       _ultoa ( ulTid, szTid + 2, 16 );
       strupr ( szTid + 2 );

       if ( strlen(szTid) > 10 )
         pszTid = szTid + 2;
       else
         pszTid = szTid;

        RTESys_FillProtocolTimeStamp(timeBuffer);
        ENTER_CRIT_SEC();
        wsprintf( pszOutBuffer,
                  BIG_HEADER_FORMAT_STR,
                  &timeBuffer[0],
                  pszTid,
                  pszType,
                  lMsgID,
                  pszLabel );

        DBG3((MF__, "Header = '%s'", pszOutBuffer));
        EXIT_CRIT_SEC();
      break;

    case time_diag_header:
      /*
        * Print Date and Time
        */
        RTESys_FillProtocolTimeStamp(timeBuffer);
        ENTER_CRIT_SEC();
        wsprintf( pszOutBuffer, "%s ", &timeBuffer[0] );
        DBG3((MF__, "Header = '%s'", pszOutBuffer));
        EXIT_CRIT_SEC();
      break;

    case big_user_diag_header:
      /*
       * Print Date and Time
       */
        RTESys_FillProtocolTimeStamp(timeBuffer);
        wsprintf( pszOutBuffer,
                  BIG_HEADER_FORMAT_STR,
                  &timeBuffer[0],
                  pszPid,
                  pszType,
                  lMsgID,
                  pszLabel );

        DBG3((MF__, "Header = '%s'", pszOutBuffer));
      break;
    }

  DBGOUT;
  return;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

STATIC LONG sql60_open_add_diag_file ( char *                  pszFileName,
                                       ULONG                ulFileSize,
                                       PSECURITY_ATTRIBUTES pSA,
                                       PDIAG_FILE_REC       pdfDiagFile,
                                       BOOL                 fEventFile )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_open_add_diag_file"
  LONG                  rc    = NO_ERROR;
  ULONG                 ulDiagDevType;

  DBGIN;


  if ( pszFileName[0] == '\0' )
    {
    sql60_msg_all ( ERR_WRONG_DIAG_FILE_NAME, pszFileName );
    DBGOUT;
    return ( ERROR_INVALID_PARAMETER );
    }

  if ( ulFileSize == 0 )
    {
    sql60_msg_all ( ERR_WRONG_DIAG_FILE_SIZE, pszFileName, ulFileSize );
    DBGOUT;
    return ( ERROR_INVALID_PARAMETER );
    }


  // --- get device type  (Console, LPTx, COMx, PRN ...)
  ulDiagDevType = sql44c_get_dev_type_by_filename (pszFileName);

  switch (ulDiagDevType)
    {
    case DT_OTHER:
      ulFileSize                       *= KILO_BYTES;
      pdfDiagFile->fStartOfWriteCycle  = FALSE;

      rc = sql60_create_diag ( fEventFile ? EVENT_FILE_INFO_LINE : TIME_HEADER_INFO_LINE,
                               pszFileName,
                               DF_APPEND_DIAG_OUTPUT,
                               ulFileSize,
                               pSA,
                               pdfDiagFile,
                               false /* PTS 1105348 WRITE_THROUGH not needed ... fEventFile */ );

      if ( rc != NO_ERROR )
         sql60_close_diagnose_f ( pdfDiagFile ) ;

      break;

    default:
      sql60_msg_all ( ERR_WRONG_DIAG_FILE_NAME, pszFileName );
      DBGOUT;
      rc = ERROR_INVALID_PARAMETER;
      break;
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

STATIC VOID sql60_close_diagnose_f ( PDIAG_FILE_REC        pDiagnoseFile )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_close_diagnose_f"
  HANDLE      hfTmpDiag;

  DBGPAS;

  ENTER_CRIT_SEC();

  if ( pDiagnoseFile->FileHandle != (HANDLE) INVALID_HANDLE_VALUE )
    {
    hfTmpDiag                        = pDiagnoseFile->FileHandle;
    EXIT_CRIT_SEC();

    SLEEP(0);
    CLOSE_FILE (hfTmpDiag);
    pDiagnoseFile->FileHandle            = (HANDLE)INVALID_HANDLE_VALUE;
    pDiagnoseFile->szPhysDiagFileName[0] = '\0';
    pDiagnoseFile->szDiagFileName[0]     = '\0';
    pDiagnoseFile->fStartOfWriteCycle    = FALSE;
    pDiagnoseFile->ulBegOfWrtCycle       = 0;
    pDiagnoseFile->ulWrtPos              = 0;
    pDiagnoseFile->ulEndOfWrtCycle       = 0;
    }
  else
    EXIT_CRIT_SEC();

  return;
  }

/*------------------------------*/

static VOID  sql60_deliver_msg ( ULONG    ulOutputRequest,
                                 LONG     lMsgID,
                                 ULONG    ulEventType,
                                 const char *      pszLabel,
                                 const char *      pszFormatStr,
                                 va_list  VarArgs )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_deliver_msg"
  CHAR                  szHeader [MAX_DIAG_HEADER_LEN];
  CHAR                  szMsg [MAX_MSG_LINE_LEN];
  LONG                  rc                = NO_ERROR;
  BOOL                  fMessageDelivered = FALSE;

  DBGIN;

  ENTER_CRIT_SEC();
  // --- Build up message text string, use stack buffer
  _vsnprintf(szMsg, sizeof(szMsg) - 1, pszFormatStr, VarArgs );

  EXIT_CRIT_SEC();

  if (( szMsg[0] == '\0' ) || ( fMessOutputDisabled == TRUE ))
    {    
    DBGOUT;
    return; // --- there is nothing to do!
    }

#ifdef KERNEL
  if ( ulEventType == ERR_TYPE || ulEventType == IERR_TYPE )
  {
      eo67InsertMessageEvent( (SAPDB_Int4)lMsgID
                            , (const char *)szMsg );
  }
#endif /* KERNEL */
  //
  // --- write to event log
  //
  if ((( ulOutput        & WRITE_TO_EVENTLOG ) &&
       ( ulOutputRequest & WRITE_TO_EVENTLOG )) ||
      (  ulOutput       == WRITE_TO_EVENTLOG ))
    {
    sql60_create_diag_header ( lMsgID, ulEventType, pszLabel, event_log_header, szHeader );

    rc = sql60_write_event_log ( szHeader, szMsg, szEventLogTitle, lMsgID,
                                 hEventSource, ulEventType );
    if ( rc != NO_ERROR )
      {
      ulOutput     &= ~WRITE_TO_EVENTLOG;
      DeregisterEventSource( hEventSource );
      hEventSource  = NULL;

      sql60_msg_all ( ERR_WRITING_EVENT_LOG, rc );
      DBG1 ((MF__, "Cannot write to the event log, rc = %d", rc ));
      }
    else
      fMessageDelivered = TRUE;
    }

  //
  // --- write to message box
  //
  if ((( ulOutput        & WRITE_TO_MSG_BOX ) &&
       ( ulOutputRequest & WRITE_TO_MSG_BOX )) ||
      (  ulOutput       == WRITE_TO_MSG_BOX ))
    {
    sql60_write_mess_box ( lMsgID, ulEventType, szMsg );

    fMessageDelivered = TRUE;
    }

  //
  // --- write to the console or GUI console
  //
  if ((( ulOutput        & WRITE_TO_CONSOLE ) &&
       ( ulOutputRequest & WRITE_TO_CONSOLE )) ||
      (  ulOutput       == WRITE_TO_CONSOLE ))
    {
    sql60_create_diag_header ( lMsgID, ulEventType, pszLabel, small_diag_header, szHeader );

    rc = sql60_write_con ( szHeader, szMsg );

    if ( rc != NO_ERROR )
      {
      ulOutput &= ~WRITE_TO_CONSOLE;

      sql60_msg_all ( ERR_WRITING_CON_MSG, rc );
      DBG1 ((MF__, "Cannot write to console, rc = %d", rc ));
      }
    else
      fMessageDelivered = TRUE;
    }
  else if ((( ulOutput        & WRITE_TO_GUI_CONSOLE ) &&
            ( ulOutputRequest & WRITE_TO_GUI_CONSOLE )) ||
           (  ulOutput       == WRITE_TO_GUI_CONSOLE ))
    {
    sql60_create_diag_header ( lMsgID, ulEventType, pszLabel, small_diag_header, szHeader );

    rc = sql60_write_GUI_con ( szHeader, szMsg );

    if ( rc != NO_ERROR )
      {
      ulOutput &= ~WRITE_TO_GUI_CONSOLE;

      sql60_msg_all ( ERR_WRITING_GUI_CON_MSG, rc );
      DBG1 ((MF__, "Cannot write to GUI console, rc = %d", rc ));
      }
    else
      fMessageDelivered = TRUE;
    }

  if (( ulOutput        &  WRITE_TO_ERR_DIAGFILE ) &&
      ( ulOutputRequest &  WRITE_TO_ERR_DIAGFILE ))
    {
    // ---  Create Message Header
    sql60_create_diag_header ( lMsgID, ulEventType, pszLabel, big_diag_header, szHeader );

    rc = sql60_write_error_diag ( szHeader, szMsg, hfErrDiag );

    if ( rc == NO_ERROR )
      fMessageDelivered = TRUE;
    }

  //
  // --- write to the diag file or application diag file
  //
  if ((( ulOutput        & WRITE_TO_DIAGFILE ) &&
       ( ulOutputRequest & WRITE_TO_DIAGFILE )) ||
      (  ulOutput       == WRITE_TO_DIAGFILE ))
    {
    // ---  Create Message Header

    sql60_create_diag_header ( lMsgID, ulEventType, pszLabel, big_diag_header, szHeader );

    rc = sql60_write_diag ( szHeader, szMsg, (ULONG)strlen(szMsg), &dfDiagFile );
    if ( rc != NO_ERROR )
      {
      ulOutput &= ~WRITE_TO_DIAGFILE;

      sql60_close_diag_file ();
      sql60_msg_all ( ERR_WRITING_DIAG_FILE, dfDiagFile.szDiagFileName , rc );
      DBG1 ((MF__, "Cannot write to the diagnostic file, rc = %d", rc ));
      ABORT();
      }
    else
      fMessageDelivered = TRUE;
    }
  else if ((( ulOutput        & WRITE_TO_APPL_DIAG ) &&
            ( ulOutputRequest & WRITE_TO_APPL_DIAG )) ||
           (  ulOutput       == WRITE_TO_APPL_DIAG ))
    {
    // ---  Create Message Header
    sql60_create_diag_header ( lMsgID, ulEventType, pszLabel, big_user_diag_header, szHeader );

    rc = sql60_write_appl_diag ( szHeader, szMsg );

    if ( rc != NO_ERROR )
      ulOutput &= ~WRITE_TO_APPL_DIAG;
    else
      fMessageDelivered = TRUE;
    }

  if ( fMessageDelivered == FALSE )
    rc = sql60_emergency_write ( szMsg, lMsgID, pszLabel, ulEventType );

  DBGOUT;
  return;
  }

/*------------------------------*/

STATIC LONG sql60_get_cons_handle ( PHANDLE phfCon )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_get_cons_handle"
  LONG         rc = NO_ERROR;

  DBGIN;

  #if defined(_WIN32)
   #if defined (KERNEL)
    *phfCon = (HANDLE)INVALID_HANDLE_VALUE;
   #else
    *phfCon = (HANDLE)GetStdHandle(STD_OUTPUT_HANDLE);

    if (*phfCon == (HANDLE)INVALID_HANDLE_VALUE )
      {
      rc = GetLastError();
      DBG1 ((MF__, "Cannot access CONSOLE, rc = %d ", rc ));
      sql60_msg_all ( ERR_CANT_ACC_CONSOLE, rc );
      }
    else
      rc = NO_ERROR;
   #endif
  #else
   #if defined (KERNEL) && defined ( NEW_CONSOLE )
    *phfCon = (HANDLE)INVALID_HANDLE_VALUE;
   #else
    *phfCon = STDOUT;
   #endif
  #endif

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

STATIC LONG sql60_create_diag ( char *                   pszHeaderInfo,
                                char *                   pszDiagFileName,
                                ULONG                 ulDiagOptions,
                                ULONG                 ulDiagSize,
                                PSECURITY_ATTRIBUTES  pSA,
                                PDIAG_FILE_REC        pDiagInfo,
                                BOOL                  fWriteThrough )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_create_diag"
  LONG         rc;
  ULONG         ulFileType;
  PATHNAME      szPhysOldDiagFileName;
  PUCHAR        pucExt;
  #if !defined(_WIN32)
   ULONG        ulDeviceAttr;
   ULONG        Action;
   ULONG        ulOpenFlags;
   ULONG        ulOpenMode;
  #endif

  DBGIN;

  DBG3 ((MF__, "Open diagfile '%s' ", pDiagInfo->szPhysDiagFileName));

  // --- substitute logical path name parts
  sql44c_subst_log_parts ( pDiagInfo->szPhysDiagFileName, pszDiagFileName );
  strcpy ( pDiagInfo->szDiagFileName, pszDiagFileName );

  if ( DF_RENAME_OLD_DIAGFILE & ulDiagOptions )
    {
    strcpy ( szPhysOldDiagFileName, pDiagInfo->szPhysDiagFileName );
    pucExt = strrchr ( szPhysOldDiagFileName, '.' );

    if ( pucExt != NULL )
      *pucExt = '\0';

    strcat ( szPhysOldDiagFileName, KNLDIAG_EXT_OLD );

    // --- delete old diagfile - ignore the return code
    // --- backup diagfile - ignore the return code
    DELETE_FILE( szPhysOldDiagFileName );

    rc = MOVE_FILE  ( pDiagInfo->szPhysDiagFileName, szPhysOldDiagFileName );

    if (( ERROR_FILE_NOT_FOUND != rc ) && ( NO_ERROR != rc ))
      {
      sql60_msg_all (ERR_MOVE_DIAG_FILE, rc);
      }
    }

  #if defined(_WIN32)
   pDiagInfo->FileHandle = (HANDLE)CreateFile( pDiagInfo->szPhysDiagFileName,
                                              GENERIC_READ | GENERIC_WRITE,
                                              FILE_SHARE_READ,
                                              pSA,
                                              OPEN_ALWAYS,
                                              ( fWriteThrough 
                                                ? FILE_FLAG_WRITE_THROUGH 
                                                : FILE_ATTRIBUTE_NORMAL),
                                              NULL );

   if ( pDiagInfo->FileHandle == (HANDLE)INVALID_HANDLE_VALUE )
     rc = GetLastError();
   else
     rc = NO_ERROR;
  #else
   ulOpenFlags = OPEN_ACTION_OPEN_IF_EXISTS |
                 OPEN_ACTION_CREATE_IF_NEW;
   ulOpenMode  = OPEN_FLAGS_FAIL_ON_ERROR   |     /* Open mode of the file */
                 OPEN_SHARE_DENYWRITE       |
                 OPEN_ACCESS_READWRITE;
   rc = DosOpen( pDiagInfo->szPhysDiagFileName,   /* File path name */
                 &pDiagInfo->FileHandle,          /* File handle */
                 &Action,                         /* Action taken */
                 0,                               /* File primary allocation */
                 FILE_NORMAL,                     /* File attribute */
                 ulOpenFlags,
                 ulOpenMode,
                 NOEABUF );                       /* No extended attributes */
  #endif

  if (rc == ERROR_SHARING_VIOLATION)
    {
    pDiagInfo->FileHandle = (HANDLE)INVALID_HANDLE_VALUE;
    sql60_msg_all ( ERR_DIAG_FILE_IN_USE, pszDiagFileName, rc );
    DBG1 ((MF__, "File '%s' is already used by another process, rc = %u", pszDiagFileName, rc ));
    }
  else if ( rc == ERROR_ACCESS_DENIED )
    {
    pDiagInfo->FileHandle = (HANDLE)INVALID_HANDLE_VALUE;
    sql60_msg_all ( ERR_ACCESS_DENIED_ON_XX, pszDiagFileName );
    DBG1 (( MF__, ERRMSG_VF_ACCESS_DENIED ));
    }
  else if (rc != NO_ERROR)
    {
    pDiagInfo->FileHandle = (HANDLE)INVALID_HANDLE_VALUE;
    sql60_msg_all ( ERR_CANT_OPEN_DIAG_FILE, pszDiagFileName, rc );
    DBG1 ((MF__, "Cannot open diagnostic file: %s, rc = %d", pszDiagFileName, rc ));
    }
  else
    {
    #if defined(_WIN32)
     ulFileType = GetFileType( (HANDLE)pDiagInfo->FileHandle );

     if ( ulFileType != FILE_TYPE_DISK )
       {
       rc = ERROR_INVALID_PARAMETER;
       CLOSE_FILE( pDiagInfo->FileHandle );
       pDiagInfo->FileHandle = (HANDLE)INVALID_HANDLE_VALUE;
       sql60_msg_all ( ERR_WRONG_DIAG_FILE_NAME, pszDiagFileName);
       DBG1 ((MF__, "Wrong diagnostic file name: '%s'", pszDiagFileName));
       }
    #else
     // --- query handle type
     rc = DosQueryHType(*FileHandle, &ulFileType, &ulDeviceAttr);

     ulFileType &= 7;     // -- extract the handle class

     if (rc != NO_ERROR)
       {
       rc = ERROR_INVALID_PARAMETER;
       CLOSE_FILE( pDiagInfo->FileHandle );
       pDiagInfo->FileHandle = (HANDLE)INVALID_HANDLE_VALUE;
       sql60_msg_all ( ERR_QUERY_FILE_TYPE, pszDiagFileName, rc );
       DBG1 ((MF__, "Query file-type error for '%s'; rc = %d", pszDiagFileName, rc));
       }
     else if ( ulFileType != HANDTYPE_FILE )
       {
       rc = ERROR_INVALID_PARAMETER;
       CLOSE_FILE( pDiagInfo->FileHandle );
       pDiagInfo->FileHandle = (HANDLE)INVALID_HANDLE_VALUE;
       sql60_msg_all ( ERR_WRONG_DIAG_FILE_NAME, pszDiagFileName);
       DBG1 ((MF__, "Wrong diagnostic file name: '%s'", pszDiagFileName));
       }
    #endif
    }

  if (rc == NO_ERROR)
    {
    rc = sql60_init_diag_file( pszHeaderInfo, ulDiagOptions,
                               ulDiagSize, pDiagInfo );
    }


  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

STATIC LONG sql60_init_diag_file ( char *             pszHeaderInfo,
                                   ULONG           ulDiagOptions,
                                   ULONG           ulDiagSize,
                                   PDIAG_FILE_REC  pDiagInfo )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_init_diag_file"
  CHAR                        szBuff [(MAX_MSG_LINE_LEN + 2) * 10];
  char *                         pszBuff;
  char *                         pszFound;
  ULONG                       ulLocal;
  ULONG                       ulWritten;
  ULONG                       ulHeaderInfoSize;
  APIRET                      rc = NO_ERROR;
  INT                         i;
  LONG                        lPos;
  ULONG                       ulBytesRead;
  ULONG                       ulBytes;
  ULONG                       ulMovedBytes    = 0;
  BOOL                        fAppendPosFound = FALSE;
  #if defined(_WIN32)
   BY_HANDLE_FILE_INFORMATION InfoBuf;
  #endif

  DBGIN;

  pDiagInfo->ulEndOfWrtCycle = ulDiagSize -
                               sizeof (END_OF_DIAG_WRITE_CYCLE) - 1;


  if ( DF_APPEND_DIAG_OUTPUT & ulDiagOptions )
    {
    if ((GetFileInformationByHandle((HANDLE)pDiagInfo->FileHandle,&InfoBuf)) &&
        ( InfoBuf.nFileSizeLow == ulDiagSize ))
      {
      if ( pszHeaderInfo != NULL )
        pDiagInfo->ulBegOfWrtCycle = (ULONG)strlen(pszHeaderInfo);

      pDiagInfo->ulWrtPos           = 0;
      pDiagInfo->fStartOfWriteCycle = FALSE;
      ulBytes                       = sizeof(szBuff) - 1;
      pszBuff                       = szBuff;
      szBuff[sizeof(szBuff) - 1 ]   = '\0';

      do
        {
        rc = READ_FILE(pDiagInfo->FileHandle, pszBuff, ulBytes, &ulBytesRead);
        pszBuff[ulBytesRead] = '\0';

        if (( pszFound = strstr( szBuff, BEGIN_OF_DIAG_WRITE_CYCLE )) != NULL )
          {
          pDiagInfo->ulBegOfWrtCycle = pDiagInfo->ulWrtPos - ulMovedBytes +
                                       (ULONG)(pszFound - szBuff)        +
                                       sizeof (BEGIN_OF_DIAG_WRITE_CYCLE) - 1;
          }

        if (( pszFound = strstr( szBuff, CURR_DIAG_WRITE_POS )) != NULL )
          {
          fAppendPosFound      = TRUE;
          pDiagInfo->ulWrtPos += (ULONG)(pszFound - szBuff) - ulMovedBytes;
          }
        else
          {
          ulMovedBytes = min( MAX_MSG_LINE_LEN, ulBytesRead );
          SAPDB_memmove(szBuff, pszBuff + ulBytesRead - ulMovedBytes, ulMovedBytes );

          pDiagInfo->ulWrtPos += ulBytesRead;

          ulBytes = sizeof(szBuff) - 1 - ulMovedBytes;
          pszBuff = szBuff + ulMovedBytes;
          }
        }
      while (( rc              == NO_ERROR ) &&
             ( ulBytesRead     != 0 )        &&
             ( fAppendPosFound == FALSE ));

      if (( rc != NO_ERROR ) && ( rc != ERROR_HANDLE_EOF ))
        {
        CLOSE_FILE( pDiagInfo->FileHandle );
        pDiagInfo->FileHandle = (HANDLE)INVALID_HANDLE_VALUE;

        sql60_msg_all ( ERR_READING_DIAG_FILE, pDiagInfo->szDiagFileName, rc );
        DBGOUT;
        return ( rc );
        }
      else
        SET_FILE_PTR( pDiagInfo->FileHandle, pDiagInfo->ulWrtPos,
                      FILE_BEGIN, &ulLocal, NULL );
      }
    }

  if ( fAppendPosFound == FALSE )
    {
    SET_FILE_PTR( pDiagInfo->FileHandle, 0, FILE_BEGIN, &ulLocal, NULL );

    if ( pszHeaderInfo != NULL )
      {
      ulHeaderInfoSize = (ULONG)strlen(pszHeaderInfo);
      rc = WRITE_FILE ( pDiagInfo->FileHandle, pszHeaderInfo,
                        ulHeaderInfoSize, &ulWritten );
      }
    else
      ulHeaderInfoSize = 0;

    ulDiagSize                -= ulHeaderInfoSize;
    pDiagInfo->ulBegOfWrtCycle = ulHeaderInfoSize;
    pDiagInfo->ulWrtPos        = ulHeaderInfoSize;

    // --- fill up the diagnostic file with blank characters
    if (rc == NO_ERROR)
      {
      memset (szBuff, ' ', sizeof(szBuff));

      for ( i = 1; i <= 10; i++)
        {
        lPos = (MAX_MSG_LINE_LEN + 2) * i;
        szBuff [ lPos - 1 ] = '\n';
        szBuff [ lPos - 2 ] = '\r';
        }

      while ((ulDiagSize > 0) && (rc == NO_ERROR))
        {
        rc = WRITE_FILE (pDiagInfo->FileHandle, szBuff, min(ulDiagSize, sizeof(szBuff)),
                        &ulWritten);
        ulDiagSize -= min(ulDiagSize, sizeof(szBuff));
        }
      }

    // --- write the info line of the write cycle end
    if (rc == NO_ERROR)
      {
      lPos = sizeof (END_OF_DIAG_WRITE_CYCLE) - 1;
      rc   = SET_FILE_PTR( pDiagInfo->FileHandle, -lPos,
                           FILE_END, &ulLocal, NULL );

      if ( rc == NO_ERROR )
        {
        rc = WRITE_FILE (pDiagInfo->FileHandle, END_OF_DIAG_WRITE_CYCLE,
                         sizeof(END_OF_DIAG_WRITE_CYCLE) - 1, &ulWritten);

        #if defined(_WIN32)
         SetEndOfFile( (HANDLE)pDiagInfo->FileHandle );
        #endif

        if (rc == NO_ERROR)
          rc = SET_FILE_PTR( pDiagInfo->FileHandle, pDiagInfo->ulBegOfWrtCycle,
                             FILE_BEGIN, &ulLocal, NULL );
        }
      }

    if ( rc != NO_ERROR )
      {
      CLOSE_FILE( pDiagInfo->FileHandle );
      pDiagInfo->FileHandle = (HANDLE)INVALID_HANDLE_VALUE;

      sql60_msg_all ( ERR_WRITING_DIAG_FILE, pDiagInfo->szDiagFileName, rc );
      DBG1 ((MF__, "Cannot write to the diagnostic file, rc = %d ", rc ));
      }
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

STATIC LONG sql60_open_error_diag ( char *                   pszDiagFileName,
                                    PSECURITY_ATTRIBUTES  pSA,
                                    HANDLE                *FileHandle )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_open_error_diag"
  LONG          rc;
  ULONG         ulFileType;
  ULONG         ulLocal;
  ULONG         ulWritten;
  CHAR          szOpenLine[MAX_MSG_LINE_LEN + 1];
  PATHNAME      szPhysDiagFileName;
  RTESys_Timestamp timeBuffer;
  RTESys_Timestamp timeBufferGMT;
#ifdef KERNEL
  tsp00_Versionc   currentVersion;
  SAPDB_Char	speed[RTE_MAXSPEEDLEN + 1];	
#endif

  DBGIN;

  sql44c_subst_log_parts ( szPhysDiagFileName, pszDiagFileName );

  DBG3 ((MF__, "Open diagfile '%s' ", szPhysDiagFileName));

   if ((*FileHandle = (HANDLE)CreateFile( szPhysDiagFileName,
                                         GENERIC_READ      |
                                         GENERIC_WRITE,
                                         FILE_SHARE_READ,
                                         pSA,
                                         OPEN_ALWAYS,
                                         FILE_ATTRIBUTE_NORMAL,
                                         NULL )) == (HANDLE)INVALID_HANDLE_VALUE)
     {
     rc = GetLastError();
     }
   else
     rc = NO_ERROR;

  if (rc == ERROR_SHARING_VIOLATION)
    {
    *FileHandle = (HANDLE)INVALID_HANDLE_VALUE;
    sql60_msg_all ( ERR_DIAG_FILE_IN_USE, pszDiagFileName, rc );
    DBG1 ((MF__, "File '%s' is already used by another process, rc = %u", pszDiagFileName, rc ));
    }
  else if ( rc == ERROR_ACCESS_DENIED )
    {
    *FileHandle = (HANDLE)INVALID_HANDLE_VALUE;
    sql60_msg_all ( ERR_ACCESS_DENIED_ON_XX, pszDiagFileName );
    DBG1 (( MF__, ERRMSG_VF_ACCESS_DENIED ));
    }
  else if (rc != NO_ERROR)
    {
    *FileHandle = (HANDLE)INVALID_HANDLE_VALUE;
    sql60_msg_all ( ERR_CANT_OPEN_DIAG_FILE, pszDiagFileName, rc );
    DBG1 ((MF__, "Cannot open diagnostic file: %s, rc = %d", pszDiagFileName, rc ));
    }
  else
    {
     ulFileType = GetFileType( (HANDLE)*FileHandle );

     if ( ulFileType != FILE_TYPE_DISK )
       {
       rc = ERROR_INVALID_PARAMETER;
       CLOSE_FILE( *FileHandle );
       sql60_msg_all ( ERR_WRONG_DIAG_FILE_NAME, pszDiagFileName);
       DBG1 ((MF__, "Wrong diagnostic file name: '%s'", pszDiagFileName));
       }
    }

  if ( rc == NO_ERROR )
    {
    rc = SET_FILE_PTR( *FileHandle, 0, FILE_END, &ulLocal, NULL );

    if ( rc == NO_ERROR )
      {
      if ( ulLocal == 0 )
        {
        // --- write open message
        rc = WRITE_FILE ( *FileHandle, BIG_HEADER_INFO_LINE,
                           sizeof (BIG_HEADER_INFO_LINE) - 1, &ulWritten);
        }
        RTESys_FillProtocolTimeStamp(timeBuffer);
        RTESys_FillProtocolGMTTimeStamp(timeBufferGMT);
#ifdef KERNEL
		/* PTS 1110621 */
		memset (speed, 0, RTE_MAXSPEEDLEN + 1);
		strcpy (speed, RTE_GetSpeed ());
		if (!strcmp (speed, RTE_FAST) )
		{
			strcpy (speed, " ");
		}
		else
		{
			strcat (speed, " ");
		}

		sp100_GetVersionString( speed, 
	    						s100buildnumber,
		    					&currentVersion[0]);
		wsprintf( szOpenLine,
                  ERR_FILE_STARTING_LINE,
                  &timeBuffer[0], &timeBufferGMT[0], &currentVersion[0] );
#else
        wsprintf( szOpenLine,
                  ERR_FILE_STARTING_LINE,
                  &timeBuffer[0], &timeBufferGMT[0], "" );
#endif

      // --- write header-info-line
      rc = WRITE_FILE ( *FileHandle, szOpenLine,
                        (ULONG)strlen(szOpenLine), &ulWritten);
      }

    if ( rc != NO_ERROR )
      {
      sql60_msg_all ( ERR_WRITING_ERROR_DIAG_FILE, rc );
      DBG1 ((MF__, "Cannot write to the error diagnostic file, rc = %d ", rc ));
      }
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

#ifdef KERNEL
void RTE_WriteDatabaseStopTime()
{
    LONG  rc;
    ULONG ulLocal;
    ULONG ulWritten;
    RTESys_Timestamp timeBuffer;
    RTESys_Timestamp timeBufferGMT;
    tsp00_Versionc   currentVersion;
    SAPDB_Char       speed[RTE_MAXSPEEDLEN + 1];	
    CHAR             szOpenLine[MAX_MSG_LINE_LEN + 1];

    rc = SET_FILE_PTR( hfErrDiag, 0, FILE_END, &ulLocal, NULL );

    if ( rc == NO_ERROR )
    {
        RTESys_FillProtocolTimeStamp(timeBuffer);
        RTESys_FillProtocolGMTTimeStamp(timeBufferGMT);
		/* PTS 1110621 */
		memset (speed, 0, RTE_MAXSPEEDLEN + 1);
		strcpy (speed, RTE_GetSpeed ());
		if (!strcmp (speed, RTE_FAST) )
		{
			strcpy (speed, " ");
		}
		else
		{
			strcat (speed, " ");
		}

		sp100_GetVersionString( speed, 
	    						s100buildnumber,
		    					&currentVersion[0]);
		wsprintf( szOpenLine,
                  ERR_FILE_STOPPING_LINE,
                  &timeBuffer[0],
                  &timeBufferGMT[0],
                  &currentVersion[0] );
        // --- write header-info-line
        (void)WRITE_FILE ( hfErrDiag,
                           szOpenLine,
                           (ULONG)strlen(szOpenLine),
                           &ulWritten);
    }
}
#endif

/*------------------------------*/

#if defined(_WIN32)
 STATIC LONG sql60_write_event_log ( char *     pszHeader,
                                     const char * pszMsg,
                                     char *     pszEventLogTitle,
                                     LONG    lMsgID,
                                     HANDLE  hEventSource,
                                     ULONG   ulEventType )
   {
   #undef  MF__
   #define MF__ MOD__"sql60_write_event_log"
   APIRET       rc = NO_ERROR;
   LPCTSTR      lpszStrings[ 3 ];
   static BOOL  fLogFull  = FALSE;
   DWORD        dwMsgID;
     
   DBGIN;

   lpszStrings[0] = pszEventLogTitle;
   lpszStrings[1] = pszHeader;
   lpszStrings[2] = pszMsg;

   switch ( ulEventType )
     {
     case  ERR_TYPE:  ulEventType = EVENTLOG_ERROR_TYPE;       break;
     case  WRN_TYPE:  ulEventType = EVENTLOG_WARNING_TYPE;     break;
     case  IERR_TYPE: ulEventType = EVENTLOG_ERROR_TYPE;       break;
     case  INFO_TYPE: ulEventType = EVENTLOG_INFORMATION_TYPE; break;
     default:         ulEventType = EVENTLOG_INFORMATION_TYPE; break;
     }

   if ( hEventSource != NULL )
     {
     dwMsgID = EVENT_SEVERITY_INFORMATIONAL | (DWORD)((lMsgID < 0) ? - lMsgID : lMsgID);

     if ( ReportEvent( hEventSource,
                       (WORD) ulEventType,
                       (WORD) EVENT_CATEGORY,
                       dwMsgID,
                       (pTU == NULL ) ? NULL : pTU->User.Sid,
                       3,
                       0,
                       lpszStrings,
                       NULL) == FALSE )
       {
       rc = GetLastError();
       }

     if ( rc == ERROR_LOG_FILE_FULL )
       {
       if ( fLogFull == FALSE )
         {
         fLogFull = TRUE;
         sql60_msg_all ( ERR_EVENT_LOG_FULL, rc );
         DBG1 ((MF__, "event log is full, rc = %d", rc ));
         }
       rc = NO_ERROR;
       }
     else if ( rc == NO_ERROR )
       fLogFull = FALSE;
     }

   DBGOUT;
   return ( rc );
   }
#endif

/*------------------------------*/

STATIC LONG sql60_write_con ( char *    pszHeader,
                              char *    pszMsg )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_write_con"
  APIRET               rc     = NO_ERROR;
  static HANDLE        hfCon  = (HANDLE)INVALID_HANDLE_VALUE;
  CHAR                 szTmpBuf  [MAX_MSG_LINE_LEN + 1];
  ULONG                ulWritten;

  DBGIN;

  strcpy  (szTmpBuf, pszHeader);
  strncat (szTmpBuf, pszMsg, MAX_MSG_LINE_LEN - strlen(pszHeader) - 3);
  szTmpBuf[MAX_MSG_LINE_LEN - 2] = '\0';
  strcat (szTmpBuf, "\r\n");


  if ( hfCon == (HANDLE)INVALID_HANDLE_VALUE )
    sql60_get_cons_handle ( &hfCon );

  if ( hfCon != (HANDLE)INVALID_HANDLE_VALUE )
    {
    rc = WRITE_FILE ( hfCon, szTmpBuf, (ULONG)strlen(szTmpBuf), &ulWritten );

    if ( rc != NO_ERROR )
      hfCon = (HANDLE)INVALID_HANDLE_VALUE;
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

STATIC LONG sql60_write_GUI_con ( char *    pszHeader,
                                  char *    pszMsg )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_write_con"
  APIRET    rc = NO_ERROR;
  CHAR      szTmpBuf  [MAX_MSG_LINE_LEN + 1];

  DBGIN;

  strcpy  (szTmpBuf, pszHeader);
  strncat (szTmpBuf, pszMsg, MAX_MSG_LINE_LEN - strlen(pszHeader) - 3);
  szTmpBuf[MAX_MSG_LINE_LEN - 2] = '\0';
  strcat (szTmpBuf, "\r\n");

  DBG3 ((MF__, szTmpBuf));

  rc = pGUIMessageFunc ( szTmpBuf );

  DBGOUT;
  return (rc);
  }

/*------------------------------*/

STATIC LONG sql60_write_mess_box ( LONG   lMsgID,
                                   ULONG  ulEventType,
                                   const char *pszMsg )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_write_mess_box"
  APIRET    rc = NO_ERROR;
  CHAR      szTmpBuf  [MAX_MSG_LINE_LEN + 1];
  #if defined(_WIN32)
   UINT     fuStyle;
  #else
   ULONG    fsStyle;
  #endif

  DBGIN;

  if ( lMsgID != 0 )
  {
      _snprintf ( szTmpBuf, MAX_MSG_LINE_LEN, "\n%s  \n\nMessageID: %d\n", pszMsg, lMsgID );
  }
  else
  {
      _snprintf ( szTmpBuf, MAX_MSG_LINE_LEN, "\n%s  \n", pszMsg );
  }

  #if defined(_WIN32)
   switch ( ulEventType )
     {
     case WRN_TYPE :
       MessageBeep( MB_ICONEXCLAMATION );
       fuStyle = MB_OK | MB_ICONEXCLAMATION;
       break;
     case INFO_TYPE:
       MessageBeep( MB_ICONHAND );
       fuStyle = MB_OK | MB_ICONINFORMATION;
       break;
     default:
       MessageBeep( MB_ICONHAND );
       fuStyle = MB_OK | MB_ICONSTOP;
       break;
     }

   MessageBox( HWND_DESKTOP, szTmpBuf, (char *)"", fuStyle);

  #else
   switch ( ulEventType )
     {
     case WRN_TYPE :
       MessageBeep( MB_ICONEXCLAMATION );
       fuStyle = MB_OK | MB_ICONEXCLAMATION;
       break;
     case INFO_TYPE:
       MessageBeep( MB_ICONHAND );
       fuStyle = MB_OK | MB_ICONINFORMATION;
       break;
     default:
       MessageBeep( MB_ICONHAND );
       fuStyle = MB_OK | MB_ICONSTOP;
       break;
     }

   MessageBox( HWND_DESKTOP, szTmpBuf, (char *)"", fuStyle);
  #endif


  DBGOUT;
  return (rc);
  }

/*------------------------------*/

LONG sql60_write_diag ( char *          pszHeader,
                               const char *    pszMsg,
                               ULONG           ulMsgLen,
                               PDIAG_FILE_REC  pDiagInfo )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_write_diag"
  #define  MSG_WPOS_INFO_SIZE    (sizeof ( "\r\n" ) - 1           + \
                                  sizeof ( CURR_DIAG_WRITE_POS)  - 1)
  char *           pszBuffer;
  char *           pszTmp;
  ULONG            ulOutputLen;
  ULONG            ulTmp;
  ULONG            ulLen;
  ULONG            ulHeaderLen;
  ULONG            ulWritten = 0;
  APIRET           rc        = NO_ERROR;

  DBGIN;

  if ( pszHeader )
    {
    ulHeaderLen = (ULONG)strlen(pszHeader);

    if ( ulHeaderLen > MAX_DIAG_HEADER_LEN )
      {
      ulHeaderLen = MAX_DIAG_HEADER_LEN;
      pszHeader[ulHeaderLen] = '\0';  // --- truncate header line
      }

    ulOutputLen = ulMsgLen + ulHeaderLen + MSG_WPOS_INFO_SIZE;
    }
  else
    {
    ulHeaderLen = 0;
    ulOutputLen = ulMsgLen + MSG_WPOS_INFO_SIZE;
    }

  pszBuffer = ALLOCA( ulOutputLen + 1 );

  if ( pszBuffer == NULL )
    {
    DBGOUT;
    return (ERROR_NOT_ENOUGH_MEMORY);
    }

  // --- enter critical section
  ENTER_CRIT_SEC();

  if (pDiagInfo->FileHandle != (HANDLE) INVALID_HANDLE_VALUE)
    {
    if ( pDiagInfo->fStartOfWriteCycle == TRUE )
      {
      pDiagInfo->fStartOfWriteCycle = FALSE;
      rc = WRITE_FILE (pDiagInfo->FileHandle, BEGIN_OF_DIAG_WRITE_CYCLE,
                       sizeof (BEGIN_OF_DIAG_WRITE_CYCLE) - 1, &ulWritten);

      pDiagInfo->ulWrtPos        += ulWritten;
      pDiagInfo->ulBegOfWrtCycle  = pDiagInfo->ulWrtPos;
      }

    // --- end of the write cycle area reached?
    if (( rc == NO_ERROR ) &&
        (pDiagInfo->ulWrtPos + ulOutputLen > pDiagInfo->ulEndOfWrtCycle))
      {
      ulLen = pDiagInfo->ulEndOfWrtCycle - pDiagInfo->ulWrtPos;

      if ( ulOutputLen < ulLen )
        {
        pszTmp = ALLOCA( ulLen + 1 );

        if ( pszTmp == NULL )
          {
          DBGOUT;
          return (ERROR_NOT_ENOUGH_MEMORY);
          }
        }
      else
        pszTmp = pszBuffer;

      memset (pszTmp, ' ', ulLen );

      // --- overwrite old lines
      rc = WRITE_FILE (pDiagInfo->FileHandle, pszTmp, ulLen, &ulWritten);

      // --- set the filepointer to the begin of the write cycle area
      if (rc == NO_ERROR)
        rc = SET_FILE_PTR( pDiagInfo->FileHandle, pDiagInfo->ulBegOfWrtCycle,
                           FILE_BEGIN, &pDiagInfo->ulWrtPos, NULL );
      }

    // --- write the output line and the write-position-info-line
    if (rc == NO_ERROR)
      {
      // --- build the output line
      //
      if ( ulHeaderLen != 0 )
        {
        strcpy (pszBuffer, pszHeader );
        strncat (pszBuffer, pszMsg, ulMsgLen );
        pszBuffer[ulHeaderLen + ulMsgLen] = '\0';
        }
      else
        {
        strncpy (pszBuffer, pszMsg, ulMsgLen );
        pszBuffer[ulMsgLen] = '\0';
        }

#ifdef KERNEL
      {
          SAPDB_UInt4 lengthOfMessage = strlen(pszBuffer)+1;
          SAPDB_Char *messageForDiagPipe = (SAPDB_Char *)alloca(lengthOfMessage+1);
          sp77sprintf(messageForDiagPipe,lengthOfMessage+1,"%s\n",pszBuffer);
          RTE_WriteToDiagPipe( messageForDiagPipe, lengthOfMessage );
      }
#endif
      strcat  (pszBuffer, "\r\n"CURR_DIAG_WRITE_POS );

      ulOutputLen = (ULONG)strlen( pszBuffer );

      rc = WRITE_FILE (pDiagInfo->FileHandle, pszBuffer, ulOutputLen,  &ulWritten);
      }
    // --- calculate the write position for the next incomming output line
    pDiagInfo->ulWrtPos += ulWritten - (sizeof(CURR_DIAG_WRITE_POS) - 1);

    // --- set the write position for the next incomming output line
    if (rc == NO_ERROR)
      rc = SET_FILE_PTR( pDiagInfo->FileHandle, pDiagInfo->ulWrtPos,
                         FILE_BEGIN, &ulTmp, NULL );

    #if defined(FLUSH_BUFFER)
     // --- Write Buffer to Disk
     if (rc == NO_ERROR)
       {
       #if defined(_WIN32)
        if (!FlushFileBuffers((HANDLE)pDiagInfo->FileHandle))
          rc = GetLastError();
       #else
        rc = DosResetBuffer(pDiagInfo->FileHandle);
       #endif
       }
    #endif
    }

  // --- exit critical section
  EXIT_CRIT_SEC();

  DBGOUT;
  return (rc);
  }

/*------------------------------*/

STATIC LONG sql60_write_error_diag ( char *       pszHeader,
                                     const char * pszMsg,
                                     HANDLE       FileHandle )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_write_error_diag"
  APIRET               rc = NO_ERROR;
  CHAR                 szTmpBuf [MAX_MSG_LINE_LEN + 1];
  ULONG                ulWritten;

  DBGIN;

  strcpy  (szTmpBuf, pszHeader);
  strncat (szTmpBuf, pszMsg, MAX_MSG_LINE_LEN - strlen(pszHeader) - 3);
  szTmpBuf[MAX_MSG_LINE_LEN - 2] = '\0';
  strcat (szTmpBuf, "\r\n");


  if ( FileHandle != (HANDLE)INVALID_HANDLE_VALUE )
    rc = WRITE_FILE ( FileHandle, szTmpBuf, (ULONG)strlen(szTmpBuf), &ulWritten );

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

STATIC LONG sql60_write_appl_diag ( char *          pszHeader,
                                    const char *    pszMsg )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_write_appl_diag"
  FILE         *fpUser;
  tsp00_Pathc  szPhysUserDiagFileName;
  static BOOL  fInit = TRUE;
  char         pszUserDiagFileEnabled[4];
  APIRET       rc = NO_ERROR;

  DBGIN;

  if ( fInit == TRUE )
  {
    fInit = FALSE;

    //
    // --- write a diagfile?
    //
	if ( sqlGetEnvironment( USERDIAG_ENV_VAR, pszUserDiagFileEnabled, sizeof(pszUserDiagFileEnabled)) )
    {
      if ( ! strcmp ( "YES", strupr ( pszUserDiagFileEnabled )))
      {
        tsp01_RteError  RteError ;
		 
        fUserDiagFileEnabled = TRUE;

        /* PTS 1106789*/
        if ( sqlGetIndependentWrkPath (szUserDiagFileName, TERM_WITH_DELIMITER_EO01, &RteError) )
          strcat ( szUserDiagFileName,  USER_DIAGFILE_NAME );
        else
          strcpy ( szUserDiagFileName, USER_DIAGFILE_NAME );
      }
      else
        fUserDiagFileEnabled = FALSE;
    }
    else
    {
      rc = sql60_user_diag_opt ( &fUserDiagFileEnabled,
                                 szUserDiagFileName,
                                 sizeof(szUserDiagFileName) - 1 );

      if ( rc != NO_ERROR )
        fUserDiagFileEnabled = FALSE;
    }
  }

  if ( fUserDiagFileEnabled  == FALSE )
    return ( NO_ERROR );;

  // --- substitute logical path name parts
  sql44c_subst_log_parts ( szPhysUserDiagFileName, szUserDiagFileName );

  fpUser = fopen ( szPhysUserDiagFileName, "a");

  if ( fpUser == NULL )
    fpUser = fopen ( USER_DIAGFILE_NAME, "a");

  if ( fpUser != NULL )
    {
    fprintf ( fpUser, "%s%s\n", pszHeader, pszMsg );
    fclose ( fpUser );
    }

  DBGOUT;
  return ( NO_ERROR );
  }

/*------------------------------*/

STATIC LONG sql60_emergency_write ( const char *  pszMsg,
                                    LONG   lMsgID,
                                    const char *  pszLabel,
                                    ULONG  ulEventType )
  {
  #undef  MF__
  #define MF__ MOD__"sql60_emergency_write"
  APIRET        rc        = NO_ERROR;
  static HANDLE hEventSrc = NULL;
  FILE          *fpUser;
  PATHNAME      szPhysUserDiagFileName;
  CHAR          szHeader [MAX_DIAG_HEADER_LEN];

  DBGIN;

  #if defined(_WIN32)
   if ( hEventSrc == NULL )
     hEventSrc = RegisterEventSource( NULL, EVENT_SOURCE_NAME );

   if ( hEventSrc != NULL )
     {
     sql60_create_diag_header ( lMsgID, ulEventType, pszLabel, event_log_header, szHeader );

     rc = sql60_write_event_log ( szHeader, pszMsg,
                                  LAST_CHANCE_INFO,
                                  lMsgID,
                                  hEventSrc,
                                  ulEventType );
     if ( rc != NO_ERROR )
       {
       DeregisterEventSource( hEventSrc );
       hEventSrc  = NULL;
       }
     else
       {
       DBGOUT;
       return ( NO_ERROR );
       }
     }
  #endif

  // --- write to appldiag first
  sql44c_subst_log_parts ( szPhysUserDiagFileName, FULL_USER_DIAGFILE_NAME );

  fpUser = fopen ( szPhysUserDiagFileName, "a");

  if ( fpUser == NULL )
    fpUser = fopen ( USER_DIAGFILE_NAME, "a");

  if ( fpUser != NULL )
    {
    // ---  Create Message Header
    sql60_create_diag_header ( lMsgID, ulEventType, pszLabel, big_user_diag_header, szHeader );

    fprintf ( fpUser, "%s%s\n", szHeader, pszMsg );
    fclose ( fpUser );
    }


  DBGOUT;
  return ( NO_ERROR );
  }

 /*------------------------------*/

STATIC LONG sql60_user_diag_opt ( BOOL *pfEnabled,
                                  char *   pszUserDiagName,
                                  LONG  lMxUserDiagName )
   {
   #undef  MF__
   #define MF__ MOD__"sql60_user_diag_opt"
   APIRET           rc = NO_ERROR;
   REG_ENTRY_REC    RegistryEntries[2];

   DBGPAS;

   RegistryEntries[0].pszValueName = REG_VN_DIAG_ENABLED;
   RegistryEntries[0].pValue       = pfEnabled;
   RegistryEntries[0].ulValueSize  = sizeof(DWORD);
   RegistryEntries[0].ulValueType  = REG_DWORD;

   RegistryEntries[1].pszValueName = REG_VN_DIAG_NAME;
   RegistryEntries[1].pValue       = pszUserDiagName;
   RegistryEntries[1].ulValueSize  = lMxUserDiagName + 1;
   RegistryEntries[1].ulValueType  = REG_SZ;

   rc = sql50_reg_get_applic_values ( NULL, HKEY_CURRENT_USER, "", 2,
                                      RegistryEntries );

   if (( rc != NO_ERROR ) && ( rc != ERROR_FILE_NOT_FOUND ))
     {
     MSGCD (( ERR_READ_USER_DIAG_OPT_VALS, rc ));
     return ( rc );
     }

/* PTS 1106789*/
   // Usage of DBROOT is forbidden! Update registry entry if DBROOT found in it
   if ( memcmp(pszUserDiagName, "%DBROOT%", strlen("%DBROOT%") ) == 0 )
   {
     tsp00_Pathc DiagFileName ;
     tsp01_RteError  RteError ;
     char *DiagFileNamePos = strrchr ( pszUserDiagName, PATH_DELIMITER_EO01 ) ;

     if ( DiagFileNamePos )
         strcpy( DiagFileName, DiagFileNamePos+1 ) ;
     else
         strcpy( DiagFileName, USER_DIAGFILE_NAME ) ;

     if ( sqlGetIndependentWrkPath ( pszUserDiagName, TERM_WITH_DELIMITER_EO01, &RteError ) )
     {
         strcat ( pszUserDiagName, DiagFileName ) ;
         RegistryEntries[0].pszValueName = REG_VN_DIAG_NAME;
         RegistryEntries[0].pValue       = pszUserDiagName;
         RegistryEntries[0].ulValueSize  = (ULONG)strlen(pszUserDiagName);
         RegistryEntries[0].ulValueType  = REG_SZ;
         rc = sql50_reg_put_applic_values ( NULL, NULL, HKEY_CURRENT_USER, "", "", NULL, 1, RegistryEntries );
     }
   }

   return ( rc );
   }

/* --------------------------------------------------------------------- */
                           
void os60MessageOutputCall( SAPDB_Int4         msgOutputMask,
                            SAPDB_Int4         msgNumber, 
                            SAPDB_Int4         msgType, 
                            SAPDB_Char const * msgLabel, 
                            SAPDB_Char const * msgFormat,
                            va_list            msgArguments )
{
    SAPDB_UInt4 outputRequestMask;

    outputRequestMask = msgOutputMask;

    switch ( outputRequestMask )
    {
        case SAPDB_MSG_TO_ALL:                  // ----[ MSGALL = All possible outputs
            outputRequestMask &= ~(SAPDB_WRITE_TO_MSG_BOX); // ---[ Suppress message box output
            switch ( msgType )
            {
                case SAPDB_MESSAGE_TYPE_INFO:
                case SAPDB_MESSAGE_TYPE_WARNING:
                    outputRequestMask &= ~(SAPDB_WRITE_TO_ERR_DIAGFILE);
                    break;
                default:
                    break;
            }
            break;

        case SAPDB_MSG_TO_CONSOLE_AND_DIAGFILE: // ----[ MSGCD = All possible consoles and diagnostic files
        case SAPDB_MSG_TO_CONSOLE:              // ----[ MSGC  = All possible consoles
        case SAPDB_MSG_TO_DIAGFILE:             // ----[ MSGD  = All possible diagnostic files
            switch( msgType )
            {
                case SAPDB_MESSAGE_TYPE_ERROR:
                case SAPDB_MESSAGE_TYPE_INTERNAL_ERROR:
                    outputRequestMask |= (SAPDB_WRITE_TO_ERR_DIAGFILE);
                    break;
                default:
                    break;
            }
            break;

        case SAPDB_MSG_TO_MESSAGE_BOX:          // ----[ MSGCD = Message to message box
        case SAPDB_MSG_TO_EVENTLOG:             // ----[ MSGEL = Message to NT Event Log
        case SAPDB_MSG_TO_CONSOLE_AND_EVENTLOG: // ----[ MSGCEL = Message to all possible consoles and NT Event Log
        default:
            break;
    }

#ifdef NEW_DIAG_FILES
    Msg_OutputToKnlDiagVArgs(msgNumber,msgType,msgLabel,msgFormat,msgArguments);
#endif
#ifndef NO_OLD_DIAG_FILES
    sql60_deliver_msg ( outputRequestMask, msgNumber, msgType, msgLabel, msgFormat, msgArguments );
#endif
}

/*
 * =============================== END ========================================
 */
