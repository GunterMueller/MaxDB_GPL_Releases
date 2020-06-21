/*!
  @file           vos11.c
  @author         RaymondR
  @brief          XPARAM Functions
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




//
//  MODULE - XPARAM Functions ( Common Part )
//


//
//  INCLUDE FILES
//
#include "gos00.h"
#include "heo00.h"
#include "heo46.h"

#if defined (KERNEL)
# include "gos00k.h"            /* nocheck */  
#endif

#include "geo007_1.h"
#include "geo007_2.h"
#include "heo11.h"
#include "heo11_1.h"
#include "RunTime/RTE_EnvironmentVariables.h"
#include "RunTime/System/RTESys_Time.h"

//
//  DEFINES
//

#define MOD__       "VOS11CC : "
#define MF__        MOD__"UNDEFINED"

#define         XP_PAGE_SIZE ( sizeof( XP_KEY_TYPE ) + sizeof( XP_VALUE_REC ) )
#define PREV            0
#define ACT             1
#define NEXT            2

#if defined(_WIN32)
 #define FILE_EXISTED   1
 #define FILE_CREATED   2
#endif

#define MAX_OPEN_PARAMS    3
#define NUM_START_PARAMS 200
#define NUM_NEW_PARAMS    50

typedef struct  xp_data_struc
  {
    PATHNAME szXParamFile;
    HANDLE hXParamFile ;
    char *Xparams ;
    ULONG ulOpenMode ; /* PTS 1104765 */
    ULONG ulActParam ;
    ULONG ulLastParam ;
    ULONG ulMaxParams ;
    char *RefXparams ;
    ULONG ulRefSize ;
    BOOLEAN fFileIsNew ;
  } XP_DATA_STRUC ;

static XP_DATA_STRUC  XpData [ MAX_OPEN_PARAMS ] ;

/*! opening a parameter fail may file when another process is currently accessing the file */
/*! If this case is detected, just sleep for a given interval and try again */
#define NUMBER_OF_ATTEMPTS_ON_OPEN_ 600
/*! the sleep interval between open attempts in 100 milliseconds  */
#define WAIT_INTERVAL_ON_OPEN_      5
//
//  MACROS
//
#ifdef DEBUG_RTE
static char _TmpStr[500 + 1];

#define _PToC( _Str, _Len)  \
            (eo46PtoC( _TmpStr, _Str, min(_Len, 500)), _TmpStr)
#endif

//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
//  EXTERNAL VARIABLES
//


//
//  EXPORTED VARIABLES
//


//
//  LOCAL VARIABLES
//


//
//  LOCAL FUNCTION PROTOTYPES
//

static XP_HFILE GetFreeXparamStrucSlot ( VOID ) ;

static VOID     OpenXParamFile  (   XP_PHFILE       plXParamFd,
                                    ERRORTEXT       acErrorText,
                                    PBOOLEAN        pbOk,
                                    PSZ             pszXParamFile,
                                    tsp00_Int4        OpenMode            );

static VOID     CloseXParamFile (   LONG            lXParamFileID,
                                    BOOLEAN         fFlush,
                                    ERRORTEXT       acErrorText,
                                    PBOOLEAN        pbOk                );
//
// ========================== GLOBAL FUNCTIONS ================================
//


VOID    sql11c_OpenXParam   (   tsp00_Pathc acXParamFile,
                                XP_PHFILE   plXParamFd,
                                ERRORTEXT   acErrorText,
                                tsp00_Int4    OpenMode,
                                PBOOLEAN    pbOk            )
{
    #undef  MF__
    #define MF__ MOD__"sql11c_OpenXParam"


    DBGIN;

    // initialize outgoing parms
    *pbOk = TRUE;
    memset (( char* ) acErrorText, ' ', sizeof ( ERRORTEXT ));

    OpenXParamFile ( plXParamFd, acErrorText, pbOk, acXParamFile, OpenMode );

    DBGOUT;
}


/*------------------------------*/

VOID    sql11c_CloseXParam  (   XP_HFILE    lXParamFd,
                                BOOLEAN     fFlush,
                                ERRORTEXT   acErrorText,
                                PBOOLEAN    pbOk            )
{
    #undef  MF__
    #define MF__ MOD__"sql11c_CloseXParam"


    DBGIN;

    // initialize outgoing parms
    *pbOk = TRUE;
    memset (( char* ) acErrorText, ' ', sizeof ( ERRORTEXT ));


    // close XPARAM file
    CloseXParamFile (lXParamFd, fFlush, acErrorText, pbOk );

    DBGOUT;
}


/*==========================================================================*/

VOID    sql11c_SeqBinReadXParam ( XP_HFILE        lXParamFd,
                                  char           *buf ,
                                  tsp00_Int4      BytesToRead ,
                                  tsp00_Int4     *pBytesRead,
                                  ERRORTEXT       acErrorText,
                                  XP_PRETURNCODE  pucXParamReturnCode )

{
  APIRET rc = READ_FILE ( XpData [ lXParamFd ] . hXParamFile ,
                          buf, (ULONG )BytesToRead , (ULONG *)pBytesRead );

  if ( rc )
    { DBG1 (( MF__, ERRMSG_READ_ERROR ));
      MSGCD (( ERR_READ_ERROR ));
      sql46c_build_error_string ( acErrorText, ERRMSG_READ_ERROR, 0 );
      *pucXParamReturnCode = XP_NOT_OK;
    }
  else
  if ( *pBytesRead < BytesToRead )
    { *pucXParamReturnCode = XP_EOF;
      DBG1 (( MF__, ERRMSG_EOF_REACHED ));
      sql46c_build_error_string ( acErrorText, ERRMSG_EOF_REACHED, 0 );
    }
  else
    *pucXParamReturnCode = XP_OK;

  return  ;
}

/*------------------------------*/

VOID    sql11c_SeqBinWriteXParam ( XP_HFILE        lXParamFd,
                                   char           *buf ,
                                   tsp00_Int4      BytesToWrite ,
	                               tsp00_Int4     *pBytesWritten,
                                   ERRORTEXT       acErrorText,
                                   XP_PRETURNCODE  pucXParamReturnCode )

{
  APIRET rc = WRITE_FILE ( XpData [ lXParamFd ] . hXParamFile ,
                           buf, (ULONG )BytesToWrite , (ULONG *)pBytesWritten );

  if ( rc || *pBytesWritten != BytesToWrite )
    {
      *pucXParamReturnCode = XP_NOT_OK;
      DBG1 (( MF__, ERRMSG_WRITE_ERROR ));
      MSGCD (( ERR_WRITE_ERROR ));
      sql46c_build_error_string ( acErrorText, ERRMSG_EOF_REACHED, 0 );
    }
  else
    { *pucXParamReturnCode = XP_OK;
      XpData[lXParamFd].ulActParam   = BytesToWrite / XP_PAGE_SIZE ;
      XpData[lXParamFd].ulLastParam += XpData[lXParamFd].ulActParam;
    }

  return  ;
}

//
// ========================== LOCAL FUNCTIONS =================================
//

static XP_HFILE       GetFreeXparamStrucSlot ( VOID )

{
    XP_HFILE        lXParamFileID ;
    for ( lXParamFileID = 0; lXParamFileID < MAX_OPEN_PARAMS &&
          XpData[ lXParamFileID ].hXParamFile != (HANDLE) NULL ;
          lXParamFileID++ )
       {;}

    if ( XpData [ lXParamFileID ]. hXParamFile != (HANDLE )NULL )
        lXParamFileID = -1 ;

    return ( lXParamFileID ) ;
}



/*------------------------------*/


static VOID OpenXParamFile  (   XP_PHFILE   plXParamFd,
                                ERRORTEXT   acErrorText,
                                PBOOLEAN    pbOk,
                                PSZ         pszXParamFile,
                                tsp00_Int4    OpenMode       )
{
    #undef  MF__
    #define MF__ MOD__"OpenXParamFile"


    HANDLE          hXParamFile;
    PATHNAME        szPhysXParamFileName;
    SAPDB_UInt4     sizeOfPhysXParamFileName = sizeof(szPhysXParamFileName);
    ULONG           ulActionTaken;
/*
    XP_VALUE_REC    rcXParamHeaderValue;
    XP_RETURNCODE   ucXParamReturnCode;
*/
    APIRET          rc = NO_ERROR;
    ULONG           lXParamFileID;
    BOOLEAN         bCreate = ( OpenMode & XP_OPEN_CREATE_SP11 ) == XP_OPEN_CREATE_SP11 ;
    BOOLEAN         bOverwr = bCreate && (OpenMode & XP_OPEN_BIN_SP11) ==XP_OPEN_BIN_SP11;

    SAPDB_UInt attempts;
    SAPDB_Bool dummyFlag = false;

    DBGIN;

    // initialize return vars
    *pbOk       = TRUE;
    *plXParamFd = UNDEF;
    memset (( char* ) acErrorText, ' ', sizeof ( ERRORTEXT ));

    DBG3 (( MF__, "Open xparam file: %s", pszXParamFile ));

    lXParamFileID = GetFreeXparamStrucSlot () ;

    if ( lXParamFileID < 0 )
      {
        *pbOk = FALSE;
        return ;
      }

    XpData[ lXParamFileID ].ulActParam = 0 ;
    XpData[ lXParamFileID ].ulLastParam = 0 ;
    XpData[ lXParamFileID ].ulOpenMode = OpenMode ;
    XpData[ lXParamFileID ].Xparams    = (char *)NULL ;
    strcpy ( XpData [ lXParamFileID ].szXParamFile, pszXParamFile )  ;

    RTE_ExpandEnvVars ( pszXParamFile, szPhysXParamFileName, &sizeOfPhysXParamFileName );

    // open or create xparm file
    #if defined(_WIN32)
        /* make several attempts to open file: it may be open by a dbmsrv    */
        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1138580 */
        for(attempts=0;attempts < NUMBER_OF_ATTEMPTS_ON_OPEN_;attempts++)   
        {
            if ( bCreate ) {
                hXParamFile = (HANDLE)CreateFile(szPhysXParamFileName,
                                                GENERIC_READ | GENERIC_WRITE,
                                                FILE_SHARE_READ,
                                                NULL,
                                        bOverwr ? CREATE_ALWAYS : CREATE_NEW ,
                                                FILE_ATTRIBUTE_NORMAL |
                                                FILE_FLAG_WRITE_THROUGH,
                                                NULL);
                ulActionTaken = FILE_CREATED;
            }
            if (!bCreate || hXParamFile==(HANDLE)INVALID_HANDLE_VALUE) {
                hXParamFile = (HANDLE)CreateFile(szPhysXParamFileName,
                                                GENERIC_READ | GENERIC_WRITE,
                                                FILE_SHARE_READ,
                                                NULL,
                                                OPEN_EXISTING,
                                                FILE_ATTRIBUTE_NORMAL  |
                                                FILE_FLAG_WRITE_THROUGH ,
                                                NULL);
                ulActionTaken = FILE_EXISTED;
            }
            if (hXParamFile==(HANDLE)INVALID_HANDLE_VALUE) rc = GetLastError();
            else rc = NO_ERROR;

            /* If (and only if!) the file is open by another process, wait a little while and try again. */
            if( rc == ERROR_SHARING_VIOLATION )
            {
                RTESys_Timeout(&dummyFlag, WAIT_INTERVAL_ON_OPEN_+1);
            }
            else
            {
                break;
            }
        }
    #else
      if ( bCreate ) {
        rc = DosOpen ( szPhysXParamFileName,
                       &hXParamFile,
                       &ulActionTaken,
                       0,                   //1781L,
                       FILE_NORMAL,
             bOverwr ? OPEN_ACTION_CREATE_IF_NEW    |
                       OPEN_ACTION_REPLACE_IF_EXISTS
                     : OPEN_ACTION_CREATE_IF_NEW    |
                       OPEN_ACTION_OPEN_IF_EXISTS,
                       OPEN_FLAGS_WRITE_THROUGH     | // Open mode of the file
                       OPEN_FLAGS_FAIL_ON_ERROR     |
                       OPEN_SHARE_DENYWRITE         |
                       OPEN_ACCESS_READWRITE,
                       NOEABUF );                     // No extended attributes
      }
      else {
        rc = DosOpen ( szPhysXParamFileName,
                       &hXParamFile,
                       &ulActionTaken,
                       0,                   //1781L,
                       FILE_NORMAL,
                       OPEN_ACTION_OPEN_IF_EXISTS   |
                       OPEN_ACTION_FAIL_IF_NEW,
                       OPEN_FLAGS_WRITE_THROUGH     | // Open mode of the file
                       OPEN_FLAGS_FAIL_ON_ERROR     |
                       OPEN_SHARE_DENYWRITE         |
                       OPEN_ACCESS_READWRITE,
                       NOEABUF );                     // No extended attributes
      }
    #endif
    if ( rc ) {
        DBG1 (( MF__, "ERROR[%d]:"ERRMSG_ACCESS_XPARAM_FILE, rc ));
#define  XERR_ACCESS_XPARAM_FILE                       11559, ERR_TYPE,"XPARAM  ","Could not access xparam file '%s', rc = %d"
        MSGCD (( XERR_ACCESS_XPARAM_FILE, szPhysXParamFileName, rc ));
        sql46c_build_error_string ( acErrorText, ERRMSG_ACCESS_XPARAM_FILE, rc );
        *pbOk = FALSE;

        DBGOUT;

        return;
    }
    else {

      XpData [ lXParamFileID ] . hXParamFile = hXParamFile;
      XpData[ lXParamFileID ].fFileIsNew = ulActionTaken != FILE_EXISTED ;

      XpData [ lXParamFileID ].ulMaxParams = 0 ;
      XpData [ lXParamFileID ].Xparams = NULL ;

      *plXParamFd = lXParamFileID;

      DBGOUT;
      return;
    }
}



/*------------------------------*/

static BOOLEAN IsParamToFlush ( XP_DATA_STRUC *XpDataStruc )
{
    #undef  MF__
    #define MF__ MOD__"IsParamToFLush"

    BOOLEAN fFlush ;

    DBGIN;

    fFlush = XpDataStruc->Xparams     != ( char *) NULL &&
            (XpDataStruc->ulOpenMode & XP_OPEN_READONLY_SP11) != XP_OPEN_READONLY_SP11 &&
             XpDataStruc->ulLastParam > 1 ;

    if ( fFlush )
      { ULONG i,j ;

        fFlush = XpDataStruc->RefXparams == ( char *) NULL ;

        //  remove deleted Parameters

        for ( j = i = 0; i< XpDataStruc->ulLastParam ; i++ )
          { char *ActParam =XpDataStruc->Xparams + XP_PAGE_SIZE * i ;
            if ( !IS_DEL_KEY(ActParam) )
              { if ( i != j )
                  SAPDB_memcpy (XpDataStruc->Xparams + XP_PAGE_SIZE * j,
                          ActParam, XP_PAGE_SIZE ) ;
                  if ( !fFlush )
                    { char *OldParam = XpDataStruc->RefXparams + XP_PAGE_SIZE * j ;
                      if ( IS_XP_TYPE_EQ( OldParam, ActParam ) )
                        { XP_VALUE_REC ParamVal1, ParamVal2 ;
                          SAPDB_memcpy( &ParamVal1, OldParam + sizeof(XP_KEY_TYPE), sizeof(XP_VALUE_REC) ) ;                          
                          SAPDB_memcpy( &ParamVal2, ActParam + sizeof(XP_KEY_TYPE), sizeof(XP_VALUE_REC) ) ;
                          if ( !IS_XP_VALUE_EQ(ParamVal1, ParamVal2) )
                            fFlush = TRUE ;  
                        }
                      else
                        fFlush = TRUE ;  
                    }
                  j++ ;
              }
           }
        XpDataStruc->ulLastParam = j ;

        if ( !fFlush && XpDataStruc->ulRefSize > 0 &&
             XpDataStruc->ulRefSize != (XpDataStruc->ulLastParam * XP_PAGE_SIZE) )
           fFlush = TRUE ;  

#ifdef OLD 
        if ( XpDataStruc->RefXparams != ( char *) NULL && 
             XpDataStruc->ulRefSize == (XpDataStruc->ulLastParam * XP_PAGE_SIZE) )
          { 
            fFlush = memcmp ( XpDataStruc->Xparams, 
                              XpDataStruc->RefXparams, XpDataStruc->ulRefSize ) != 0 ;

          DBG3 (( MF__, "memcmp: paramfile is %s to flush", fFlush ? "" : "NOT" ));
          }
#endif
      }

    DBG3 (( MF__, "return: paramfile is %s to flush", fFlush ? "" : "NOT" ));
    DBGOUT;

    return ( fFlush ) ;
}

#define RESET_PARAM_CHANGED_FLAG 1

/*------------------------------*/

static VOID CloseXParamFile (   LONG        lXParamFileID ,
                                BOOLEAN     fFlush,
                                ERRORTEXT   acErrorText,
                                PBOOLEAN    pbOk            )
{
    #undef  MF__
    #define MF__ MOD__"CloseXParamFile"


    DBGIN;

    *pbOk = lXParamFileID > -1 && lXParamFileID < MAX_OPEN_PARAMS ;
    if ( *pbOk )
      { 
      /* the code that was executed if fFlush_buffer=true operated on the contents of the xparam file. */
      /* Because these have changed and the function is currently never called with fFlush_buffer=true, this case is forbidden */
      if(fFlush)  
        MSGD (( -11114, ERR_TYPE, "XPARAM  ", "fFlush = true no longer supported for sql11c_close_xparam_file()")) ;

        /* ***** close XPARAN file ***** */

        CLOSE_FILE ( XpData [ lXParamFileID ] . hXParamFile );
        if ( XpData [ lXParamFileID ] .ulLastParam <= 1 &&
             XpData [ lXParamFileID ] .fFileIsNew )
           unlink ( XpData [ lXParamFileID ] . szXParamFile )  ;

        if ( XpData [ lXParamFileID ] . Xparams != ( char *) NULL )
          FREE_MEM ( (PVOID)XpData [ lXParamFileID ] . Xparams);
        if ( XpData [ lXParamFileID ] . RefXparams != ( char *) NULL )
          FREE_MEM ( (PVOID)XpData [ lXParamFileID ] . RefXparams);

        memset ( &XpData [ lXParamFileID ] , 0, sizeof ( XP_DATA_STRUC )) ;
        DBG3 (( MF__, "XPARAM file closed" ));
    }
    DBGOUT;
}

//
// =============================== END ========================================
//
