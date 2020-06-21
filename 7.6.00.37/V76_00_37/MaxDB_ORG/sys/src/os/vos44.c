/*!
  @file           vos44.c
  @author         RaymondR
  @brief          miscellaneous device/file functions
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

#if defined (KERNEL)
# include "gos00k.h" /* nocheck */
#endif

#include "geo007_1.h"
#include "geo007_2.h"
#include "gos44.h"

#include "SAPDBCommon/SAPDB_string.h"

/*
 *  DEFINES
 */
#define MOD__  "VOS44C : "
#define MF__   MOD__"UNDEFINED"

#if defined(_WIN32)
 #define TAPE_ID_STR          "TAPE"
 #define TAPE_ID_LEN          (sizeof(TAPE_ID_STR) - 1)

 #define CONOUT_ID_STR        "CONOUT$"
 #define CONIN_ID_STR         "CONIN$"
#endif

#define PIPE_ID_STR          "PIPE\\"
#define PIPE_ID_LEN          (sizeof(PIPE_ID_STR) - 1)

#define SKIP_NODE(_out,_in)  {_out = (_in[0] == '\\' && _in[1] == '\\') ?   \
                                        strchr( &_in[2], '\\') : NULL;      \
                              _out = (_out != NULL && _out[0] != '\0' ) ?   \
                                        _out + 1 : NULL; }

#define CON_ID_STR            "CON"
#define CON_ID_LEN            (sizeof(CON_ID_STR) - 1)
#define PRN_ID_STR            "PRN"
#define PRN_ID_LEN            (sizeof(PRN_ID_STR) - 1)
#define COM_ID_STR            "COM"
#define COM_ID_LEN            (sizeof(COM_ID_STR) - 1)
#define LPT_ID_STR            "LPT"
#define LPT_ID_LEN            (sizeof(LPT_ID_STR) - 1)
#define NUL_ID_STR            "NUL"
#define NUL_ID_LEN            (sizeof(NUL_ID_STR) - 1)

#if defined(_WIN32)
  #define FILESIZELOW  InfoBuf.nFileSizeLow
  #define FILESIZEHIGH InfoBuf.nFileSizeHigh
  #define FILEATTR     InfoBuf.dwFileAttributes
#else
  #define FILESIZELOW  InfoBuf.cbFile
  #define FILESIZEHIGH 0
  #define FILEATTR     InfoBuf.attrFile
#endif

#define ERROR_OPEN_FILE_FOR_READ  "Can't open (read) File: "
#define ERROR_OPEN_FILE_FOR_WRITE "Can't open (write) File: "
#define ERROR_READ_FILE           "Can't read File: "
#define ERROR_WRITE_FILE          "Can't write File: "

/*
 * EXTERNAL VARIABLES
 */


/*
 * LOCAL VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */

static void os44SetFileCopyError        ( char                *FileName,
                                          char                *RteTxt  ,
                                          int                  OsErrorCode,
                                          tsp01_RteError      *RteError ) ;


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

tsp00_Int sql44c_get_dev_type_by_filename ( char * pszName )
  {
  #undef  MF__
  #define MF__ MOD__"sql44c_get_dev_type_by_filename"
  tsp00_Int DevType   = DT_OTHER;
  PATHNAME  szName;
  CHAR      szNum[5]  = "";
  LONG      lNum;
  PSZ       pszID;
  #if defined(_WIN32)
   CHAR     cAny     = ' ';
  #endif

  DBGPAS;

  // --- Convert diagnostic file name to uppercase
  strncpy (szName, pszName, sizeof (PATHNAME) - 1);
  szName[sizeof(PATHNAME)-1] = '\0';
  strupr(szName);

   //
   // --- TAPE
   //
   SKIP_NODE( pszID, szName);  // --- try to skip node name

   if (( pszID != NULL ) &&
       (!strncmp( TAPE_ID_STR, pszID, TAPE_ID_LEN)))
     {
     lNum = sscanf ( pszID + TAPE_ID_LEN,
                     "%4[0123456789]%c", szNum, &cAny );

     if ( lNum == 1 )
       {
       DevType = DT_TAPE;
       DBG3 ((MF__, "DevType:  TAPE"));
       return (DevType);
       }
     }

   //
   // --- DRIVE
   //
   if (( szName[0] == '\\' ) && ( szName[1] == '\\' ) &&
       ( szName[2] == '.'  ) && ( szName[3] == '\\' ) &&
        isalpha( szName[4] ) && ( szName[5] == ':'  ) &&
       ( szName[6] == '\0' ))
     {
     DevType = DT_DRIVE;
     DBG3 ((MF__, "DevType:  DRIVE"));
     return (DevType);
     }

   //
   // --- CONSOLE
   //
   if (!strcmp( CONOUT_ID_STR, szName ))
     {
     DevType = DT_CONOUT;
     DBG3 ((MF__, "DevType:  CONSOLE"));
     return (DevType);
     }

   if (!strcmp( CONIN_ID_STR, szName ))
     {
     DevType = DT_CONIN;
     DBG3 ((MF__, "DevType:  CONSOLE"));
     return (DevType);
     }
  //
  // --- PIPE ( '\\<nodename>\PIPE\<pipename>' )
  //
  SKIP_NODE( pszID, szName);  // --- try to skip node name

  if (( pszID != NULL ) &&
      (!strncmp( PIPE_ID_STR, pszID, PIPE_ID_LEN)))
    {
    DevType = DT_PIPE;
    DBG3 ((MF__, "DevType:  PIPE"));
    return (DevType);
    }


  //
  // --- CONSOLE
  //
  if (( !strncmp (CON_ID_STR, szName, CON_ID_LEN) ) &&
      ((( szName[CON_ID_LEN] == '.' ) && ( szName[CON_ID_LEN + 1] == '\0')) ||
       (( szName[CON_ID_LEN] == ':' ) && ( szName[CON_ID_LEN + 1] == '\0')) ||
       ( szName[CON_ID_LEN] == '\0' ) ))
    {
    DevType = DT_CON;
    DBG3 ((MF__, "DevType:  CONSOLE"));
    return (DevType);
    }

  //
  // --- PRINTER
  //
  if (( !strncmp (PRN_ID_STR, szName, PRN_ID_LEN) ) &&
      ((( szName[PRN_ID_LEN] == '.' ) && ( szName[PRN_ID_LEN + 1] == '\0')) ||
       (( szName[PRN_ID_LEN] == ':' ) && ( szName[PRN_ID_LEN + 1] == '\0')) ||
       ( szName[PRN_ID_LEN] == '\0' ) ))
    {
    DevType = DT_PRN;
    DBG3 ((MF__, "DevType:  PRN"));
    return (DevType);
    }

  //
  // --- NUL
  //
  if (( !strncmp (NUL_ID_STR, szName, NUL_ID_LEN) ) &&
      (( szName[NUL_ID_LEN] == '.' ) ||
       (( szName[NUL_ID_LEN] == ':' ) && ( szName[NUL_ID_LEN + 1] == '\0')) ||
       ( szName[NUL_ID_LEN] == '\0' ) ))
    {
    DevType = DT_NUL;
    DBG3 ((MF__, "DevType:  NUL"));
    return (DevType);
    }

  //
  // --- COM-Port
  //
  if (!strncmp( COM_ID_STR, szName, COM_ID_LEN ))
    {
    sscanf ( szName + COM_ID_LEN, "%4[0123456789]", szNum );

    lNum = (LONG)strlen ( szNum );

    if (( lNum > 0 ) &&
        ((( szName[COM_ID_LEN + lNum]   == '.'  ) &&
          ( szName[COM_ID_LEN + lNum + 1] == '\0' )) ||
         (( szName[COM_ID_LEN + lNum]   == ':'  ) &&
          ( szName[COM_ID_LEN + lNum + 1] == '\0' )) ||
         ( szName[COM_ID_LEN + lNum]   == '\0' )))
      {
      DevType = DT_COMx;
      DBG3 ((MF__, "DevType:  DT_COMx"));
      return (DevType);
      }
    }

  //
  // --- LPT-Port
  //
  if (!strncmp( LPT_ID_STR, szName, LPT_ID_LEN ))
    {
    sscanf ( szName + LPT_ID_LEN, "%4[0123456789]", szNum );

    lNum = (ULONG)strlen ( szNum );

    if (( lNum > 0 ) &&
        ((( szName[LPT_ID_LEN + lNum]   == '.'  ) &&
          ( szName[LPT_ID_LEN + lNum + 1] == '\0' )) ||
         (( szName[LPT_ID_LEN + lNum]   == ':'  ) &&
          ( szName[LPT_ID_LEN + lNum + 1] == '\0' )) ||
         ( szName[LPT_ID_LEN + lNum]   == '\0' )))
      {
      DevType = DT_LPTx;
      DBG3 ((MF__, "DevType:  DT_LPTx"));
      return (DevType);
      }
    }

  return (DevType);
  }

/*------------------------------*/

/*
 *
 * sql44c_subst_log_parts
 *
 * Example:
 *    input : %DBROOT%\xpath
 *    output: d:\vdn\usr\xpath
 *
 */

tsp00_Bool sql44c_subst_log_parts ( char*  pszOutName,
                                    char*  pszInName )
  {
  #undef  MF__
  #define MF__ MOD__"sql44c_subst_log_parts"

  SAPDB_Char                            *szVarName;
  PSZ                                    pszResult;
  PCHAR                                  pch1stPercent;
  PCHAR                                  pch2ndPercent;
  size_t                                 cbLength;
  size_t                                 cbOutLength;

  DBGIN;

  /*
   * Initialization
   */
  cbOutLength   = 0;
  pch2ndPercent = pszInName - 1;
  pch1stPercent = strchr (pszInName, '%');             /* beginning of env.var.*/

  while (pch1stPercent)
    {
    cbLength     = pch1stPercent - pch2ndPercent - 1;
    cbOutLength += cbLength;

    if (cbOutLength > sizeof (PATHNAME))
      {
      SAPDB_strncpy_zero_terminated (pszOutName, pszInName, sizeof (PATHNAME)-(cbOutLength-cbLength));
      return (FALSE);
      }

    strncpy (pszOutName, pch2ndPercent+1, cbLength);
    pszOutName  += cbLength;

    pch2ndPercent  = strchr (pch1stPercent+1, '%');    /* end of env. var.*/

    if (pch2ndPercent)
      {
      /*
       * environment variable found:
       * scan environment for result string
       */

      cbLength = pch2ndPercent - pch1stPercent - 1;
      szVarName = (SAPDB_Char *)alloca(cbLength+1);
      SAPDB_strncpy_zero_terminated(szVarName,pch1stPercent+1,cbLength+1);
      strupr  (szVarName);

      GETENV ( szVarName, &pszResult );

      if (pszResult != NULL)
        {
        DBG3 ((MF__, "EnvVar: '%s' = '%s'", szVarName, pszResult));
        /*
         * replace environment variable by result string
         */
        cbLength     = strlen (pszResult);
        cbOutLength += cbLength;

        if (cbOutLength > sizeof(PATHNAME))
          {
          SAPDB_strncpy_zero_terminated (pszOutName, pszInName, sizeof (PATHNAME)-(cbOutLength-cbLength) );
          return (FALSE);
          }

        strcpy (pszOutName, pszResult);
        pszOutName  += cbLength;
        }
      else
        {
        DBG3 ((MF__, "Environment variable '%s' no found", szVarName));
        /*
         * environment variable not set:
         * take %....% as part of filename
         */
        cbLength    += 2;
        cbOutLength += cbLength;

        if (cbOutLength > sizeof(PATHNAME))
          {
          SAPDB_strncpy_zero_terminated (pszOutName, pszInName, sizeof (PATHNAME)-(cbOutLength-cbLength) );
          return (FALSE);
          }

        strncpy (pszOutName, pch1stPercent, cbLength);
        pszOutName  += cbLength;
        }
        /*
         * search for next environment variable
         */
        pch1stPercent = strchr (pch2ndPercent+1, '%');
      }
    else
      {
      /*
       * % was not matched by another %
       * take %.... as part of filename
       */
      cbOutLength  += strlen (pch1stPercent);

      if (cbOutLength > sizeof(PATHNAME))
        {
        SAPDB_strncpy_zero_terminated (pszOutName, pszInName, sizeof (PATHNAME)-(cbOutLength-strlen (pch1stPercent)) );
        return (FALSE);
        }


      strcpy (pszOutName, pch1stPercent);
      pch1stPercent = 0;
      }
    }
  /*
   * copy remaining characters
   */
  if (pch2ndPercent)
    {
    cbOutLength  += strlen (pch2ndPercent+1);

    if (cbOutLength > sizeof(PATHNAME))
      {
      SAPDB_strncpy_zero_terminated (pszOutName, pszInName, sizeof (PATHNAME)-(cbOutLength-strlen (pch2ndPercent+1)) );
      return (FALSE);
      }

    strcpy (pszOutName, pch2ndPercent+1);
    }

  DBGOUT;

  return (TRUE);
  }

/*------------------------------*/

// --- Gets the filemode of the node specified by fname
tsp00_Int sql44c_get_file_info ( char *  pszFName,
                                 tsp00_Int *  plAtt,
                                 tsp00_Uint * pFSizeLow,
                                 tsp00_Uint * pFSizeHigh )
  {
  #undef  MF__
  #define MF__  MOD__"sql44_get_file_info"

  #if defined(_WIN32)
   BY_HANDLE_FILE_INFORMATION InfoBuf;
   HANDLE                     FileHandle;
  #else
   ULONG                      InfoLevel;      /* Data required */
   FILESTATUS3                InfoBuf;        /* File info buffer */
   ULONG                      InfoBufSize;    /* Data buffer size */
  #endif
  APIRET                       rc = NO_ERROR;  /* Return code */

  DBGIN;

  *pFSizeLow  = 0;
  *pFSizeHigh = 0;

  #if defined(_WIN32)
   FileHandle = CreateFile(pszFName,
                           GENERIC_READ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           NULL,
                           OPEN_EXISTING,
                           0,  //... file attributes seem to be ignored
                           NULL);
   if (FileHandle == INVALID_HANDLE_VALUE)
     rc = GetLastError();
   else
     {
     if (!GetFileInformationByHandle(FileHandle, &InfoBuf))
       rc = GetLastError();
     CloseHandle(FileHandle);
     }

   // --- remove some flags
   FILEATTR &= ~FILE_ARCHIVE;

  #else
   InfoLevel   = 1;       /* Indicate that Level 1 information is desired   */
   InfoBufSize = sizeof(FILESTATUS3);

   //
   // On successful return, the Level 1 directory
   // information is in the InfoBuf buffer
   //
   rc = DosQueryPathInfo(pszFName, InfoLevel, &InfoBuf, InfoBufSize);

   // --- remove the archive flag
   FILEATTR &= ~FILE_ARCHIVE;
  #endif


  if (rc == ERROR_FILE_NOT_FOUND)
    {
    *plAtt      = FILE_NORMAL;
    *pFSizeLow  = 0;
    *pFSizeHigh = 0;
    }
  else if (rc != 0)
    {
    *plAtt = UNDEF;
    }
  else if ((FILEATTR & FILE_DIRECTORY) == FILE_DIRECTORY)
    {
    *plAtt      = FILEATTR;
    *pFSizeLow  = 0;
    *pFSizeHigh = 0;
    }
  else
    {
    *plAtt      = FILEATTR;
    *pFSizeLow  = ((ULONG) FILESIZELOW);
    *pFSizeHigh = ((ULONG) FILESIZEHIGH);
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

bool    os44Filecopy ( char              *pszSrcName,
                       char              *pszDestName,
                       tsp01_RteError    *RteError )
  {
  #undef  MF__
  #define MF__ MOD__"os44Filecopy"

  APIRET          rc = NO_ERROR;  /* Return code */
  HANDLE          hSrcFile, hDestFile;
  CHAR            Buf [ MAXBUFFERLENGTH ] ;
  LONG            ulNumRead, ulNumWritten;

  memset(RteError, 0, sizeof(tsp01_RteError) ) ;
  RteError->RteErrCode = RTE_NO_ERROR_SP01 ;
  RteError->OsErrCode  = OS_NO_ERROR_SP01  ;

  hSrcFile = (HANDLE)CreateFile(pszSrcName,
                               GENERIC_READ,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);

  if (hSrcFile==(HANDLE)INVALID_HANDLE_VALUE) rc = GetLastError();

  if ( rc != NO_ERROR ) 
    {
      MSGD ((ERR_OPEN, pszSrcName, rc )) ;
      os44SetFileCopyError ( pszSrcName, ERRMSG_VF_OPEN, rc, RteError ) ;
    }
  else
    {
    hDestFile = (HANDLE)CreateFile(pszDestName,
                                  GENERIC_READ | GENERIC_WRITE,
                                  0,
                                  NULL,
                                  CREATE_ALWAYS ,
                                  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                  (HANDLE)hSrcFile);
  
    if (hDestFile==(HANDLE)INVALID_HANDLE_VALUE) rc = GetLastError();

    if ( rc != NO_ERROR ) 
      { 
      MSGD ((ERR_OPEN, pszDestName, rc )); 
      os44SetFileCopyError ( pszDestName, ERRMSG_VF_OPEN, rc, RteError ) ;
      CLOSE_FILE ( hSrcFile  );
      }
    }

  if ( rc == NO_ERROR ) 
    {
    do
      {
      rc = READ_FILE ( hSrcFile ,Buf, sizeof ( Buf ), &ulNumRead );
      if ( rc == NO_ERROR)
        {
        rc = WRITE_FILE ( hDestFile, Buf, ulNumRead, &ulNumWritten );
        if ( rc == NO_ERROR)
          { 
          DBG1 (( MF__, "Bytes Read / Written", ulNumRead, ulNumWritten )); }
        else
          { MSGD ((ERR_WRITE, rc ))
            os44SetFileCopyError ( pszDestName, ERRMSG_VF_SEEK, rc, RteError ) ;
          }
        }
      else
        {
          MSGD ((ERR_READ, rc )) ;
          os44SetFileCopyError ( pszSrcName, ERRMSG_VF_READ, rc, RteError ) ;
        }
      }
    while ( rc == NO_ERROR && ulNumRead > 0 && ulNumRead == ulNumWritten ) ;

    if ( rc == NO_ERROR )
    { 
      FILETIME        SrcCreationTime, SrcLastAccessTime, SrcLastWriteTime ;
      if ( GetFileTime( (HANDLE )hSrcFile,&SrcCreationTime, &SrcLastAccessTime, &SrcLastWriteTime) )
        SetFileTime( (HANDLE )hDestFile, &SrcCreationTime, &SrcLastAccessTime, &SrcLastWriteTime) ;
    }

    CLOSE_FILE ( hSrcFile  );
    CLOSE_FILE ( hDestFile );
    }

  if ( rc != NO_ERROR ) 
      DBG1 (( MF__, "return ERROR[%d]", rc ))

  DBGOUT;
  return ( rc == NO_ERROR);
  }


//
// ========================== LOCAL FUNCTIONS =================================
//

static void os44SetFileCopyError ( char             *FileName,
                                   char             *RteTxt  ,
                                   int               OsErrorCode,
                                   tsp01_RteError   *RteError )
{
  RteError->OsErrCode = OsErrorCode ;
  sqlos_errtext_by_errcode   ( RteError->OsErrCode,
                               RteError->OsErrText,
                               sizeof ( RteError->OsErrText) ) ;
  
  RteError->RteErrCode = !RTE_ERROR_SP01 ;
  eo46_rte_errtext_with_filename ( RteTxt,
                                   FileName,
                                   RteError->RteErrText ,
                                   sizeof( RteError->RteErrText ) );
}

/*
 * ========================== LOCAL FUNCTIONS =================================
 */
