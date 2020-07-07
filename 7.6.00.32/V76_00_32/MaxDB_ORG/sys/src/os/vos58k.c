/*!
  @file           vos58k.c
  @author         RaymondR
  @brief          vdiaginit
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
#include "geo007_1.h"
#include "gos00k.h"
#include "geo002.h"
#include "heo54k.h"
#include "gos44.h"


/*
 *  DEFINES
 */
#define MOD__  "VOS58KC : "
#define MF__   MOD__"UNDEFINED"

#define MAX_DWORD_VAL           0xFFFFFFFF

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */

/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

VOID vdiaginit ( BOOLEAN         *traceflag,
                 BOOLEAN         *dumpflag,
                 tsp00_VFilename vtracename,
                 INT4            *vtracesize,
                 tsp00_VFilename vdumpname)
  {
  #undef  MF__
  #define MF__ MOD__"vdiaginit"
  PUKT_CTRL_REC      pUKT = THIS_UKT_CTRL;
  INT                i;
  APIRET             rc           = NO_ERROR;
  HANDLE             FileHandle   =  (HANDLE)INVALID_HANDLE_VALUE;
  ULONG              ulWritten;
  BOOLEAN            ok;
  char *             Buffer ;
  ERRORTEXT          szErrText;
  PATHNAME           szPhysTraceFileName;
  ULONG              ulFilePos;
  #if !defined(_WIN32)
   ULONG             Action;
  #endif

  Buffer = (char*)alloca( XPARAM(ulDBPageSize));

  DBGIN_T (pUKT->pCTask->ulTaskIndex);

  DBG3 ((MF__, "[T:0x%03u] traceflag: 0x%08x - dumpflag:  0x%08x - "
                "tracename: 0x%08x - tracesize: 0x%08x - dumpname:  0x%08x   ",
                pUKT->pCTask->ulTaskIndex, traceflag, dumpflag, vtracename, vtracesize, vdumpname));

  pUKT->pCTask->TaskState = TSK_VDIAGINIT_EO00 ;
  kgs.pfTraceFlag         = traceflag;
  kgs.pfDump              = dumpflag;

  eo46CtoP ( vtracename, XPARAM(szTraceDevspace) , sizeof(tsp00_VFilename) );
  eo46CtoP ( vdumpname , XPARAM(szDumpFilename)  , sizeof(tsp00_VFilename)  );

   // --- vdiaginit also does vdevsize for the tracedevspace
  *vtracesize = XPARAM(lTraceDevspaceSize) ;
  vdevsize ( 1 , sp2dt_trace , vtracesize , szErrText, &ok );

  if ( ! ok )
    {
    MSGALL(( ERR_VDEVSIZE_UNSUCCSESSFUL ));
    DBG1  ( (MF__, "Vdevsize of tracedevpace '%s' unsuccsessful, OK = %f", XPARAM(szTraceDevspace), ok ));
    ABORT();
    }

  if ( *vtracesize != XPARAM(lTraceDevspaceSize) )
    {
    MSGALL(( ERR_INCORRECT_TR_DEVSIZE, XPARAM(szTraceDevspace)));
    DBG1  (( MF__, "Size incorrect of tracedevspace: %s, (target: %u - actual: %u)",
             XPARAM(szTraceDevspace), XPARAM(lTraceDevspaceSize), *vtracesize));
    ABORT();
    }

   // --- vdiaginit also initializes the tracedevspace
  memset (Buffer , 0 , XPARAM(ulDBPageSize));

  DBG3 ((MF__, "Openning tracedevice %s", XPARAM(szTraceDevspace) ));

  // --- substitute logical path name parts
  sql44c_subst_log_parts ( szPhysTraceFileName, XPARAM(szTraceDevspace) );

  #if defined(_WIN32)
   FileHandle = (HANDLE)CreateFile( szPhysTraceFileName,
                                   GENERIC_WRITE,
                                   FILE_SHARE_READ   |
                                   FILE_SHARE_WRITE,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL );

   if (FileHandle == (HANDLE)INVALID_HANDLE_VALUE)
     {
     rc = GetLastError();

     if ( rc == ERROR_ACCESS_DENIED )
       MSGALL(( ERR_DEVSPACE_ACCESS_DENIED, XPARAM(szTraceDevspace) ))
     else
       MSGALL(( ERR_CANT_OPEN_TR_DEVSPACE, XPARAM(szTraceDevspace), rc ))

     DBG1  (( MF__, "Cannot open tracedevspace: %s, rc = %d ",
                     XPARAM(szTraceDevspace), rc ));
     ABORT();
     }
  #else
   rc = DosOpen( szPhysTraceFileName,              /* File path name */
                 &FileHandle,                      /* File handle */
                 &Action,                          /* Action taken */
                 0L,                               /* File primary allocation */
                 FILE_NORMAL,                      /* File attribute */
                 OPEN_ACTION_OPEN_IF_EXISTS |      /* Open function type */
                 OPEN_ACTION_FAIL_IF_NEW,
                 OPEN_FLAGS_FAIL_ON_ERROR   |
                 OPEN_SHARE_DENYWRITE       |
                 OPEN_ACCESS_WRITEONLY,
                 NOEABUF);                         /* No extended attributes */

   if ( rc != NO_ERROR )
     {
     MSGALL(( ERR_CANT_OPEN_TR_DEVSPACE, XPARAM(szTraceDevspace), rc ));
     DBG1  (( MF__, "Cannot open tracedevspace: %s, rc = %d ",
                     XPARAM(szTraceDevspace), rc ));
     ABORT();
     }

  #endif

  // --- skip header
  rc = SET_FILE_PTR ( FileHandle, XPARAM(ulDBPageSize), FILE_CURRENT,
                      &ulFilePos, NULL );

  if (( rc != NO_ERROR ) || ( ulFilePos != XPARAM(ulDBPageSize) ))
    {
    MSGALL(( ERR_CANT_SEEK_ON_DEVSPACE, XPARAM(szTraceDevspace), rc ));
    DBG1  (( MF__, "Cannot seek on devspace: %s, (Write Pos: %u - CurrPos: %u), rc = %d",
                   XPARAM(szTraceDevspace), XPARAM(ulDBPageSize), ulFilePos, rc ));
    ABORT();
    }

  for ( i = 0 ; i < XPARAM(lTraceDevspaceSize); i ++ )
    {
    rc = WRITE_FILE ( FileHandle, Buffer, XPARAM(ulDBPageSize), &ulWritten );

    if ( (rc != NO_ERROR) || (ulWritten != XPARAM(ulDBPageSize)) )
      {
      MSGALL(( ERR_CANT_WRITE_TR_DEVSPACE, XPARAM(szTraceDevspace), rc ));
      DBG1  (( MF__, "Cannot write on devspace: %s, (Write Size: %u - Written: %u), rc = %d",
               XPARAM(szTraceDevspace), XPARAM(ulDBPageSize), ulWritten, rc ));
      ABORT();
      }
    }
  DBG3 (( MF__, "Wrote empty blocks, count %d", i));

  rc = CLOSE_FILE ( FileHandle );

  if (rc != NO_ERROR)
    {
    MSGALL(( ERR_CANT_CLOSE_TR_DEVSPACE, XPARAM(szTraceDevspace), rc ));
    DBG1  (( MF__, "Cannot close tracedevspace: %s, rc = %d", XPARAM(szTraceDevspace), rc ));
    ABORT();
    }

  pUKT->pCTask->TaskState = TSK_RUNNING_EO00 ;

  DBGOUT_T (pUKT->pCTask->ulTaskIndex);
  return ;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
