/*!
  @file           vos59k.c
  @author         RaymondR
  @brief          vexec
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


/*
 *  DEFINES
 */
#define MOD__  "VOS59KC : "
#define MF__   MOD__"UNDEFINED"

#define MX_EXTENSIONS_LIST_SIZE \
        ( sizeof (ExtensionList) / sizeof (*ExtensionList) )

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


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
 static PCHAR ExtensionList [] = { ".BAT", ".CMD", ".COM", ".PIF", ".EXE" };
#else
 static PCHAR ExtensionList [] = { ".EXE", ".COM", ".CMD", ".BAT" };
#endif


/*
 * LOCAL FUNCTION PROTOTYPES
 */
#if !defined(_WIN32)
 static APIRET sql59k_get_path ( PATHNAME pszFullPathName,
                                 PSZ      pszFileName );
#endif

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

VOID vexec  ( tsp00_ExecArgLine     command,
              tsp00_ExecReturn     *exec_ret,
              tsp00_ErrText         err_text )
  {
  #undef  MF__
  #define MF__ MOD__"vexec"
  CHAR                  szCommand[ sizeof ( tsp00_ExecArgLine ) + 1 ];
  PSZ                   pszCommand;
  #if defined(_WIN32)
    static STARTUPINFO  StartupInfo;
    PROCESS_INFORMATION ProcessInfo;
  #else
    CHAR                szParameter [ sizeof ( tsp00_ExecArgLine ) + 1 ];
    CHAR                szPgmTitle[60 + 1];
    PATHNAME            szPgmName,
                        szIconFile,
                        szFullyQualifiedPgmName;
    PSZ                 pszParameter,
                        pszIconFile = NULL;
    STARTDATA           stdataSession;
    ULONG               idSession,
                        ulStrLen;
    PID                 idProcess;
    UCHAR               ObjBuf[100];
    APIRET              rc = NO_ERROR;
  #endif

  DBGIN;

  eo46PtoC ( szCommand,  command, sizeof(tsp00_ExecArgLine) );

  DBG3 (( MF__, "szCommand : %s ", szCommand ));

  // --- skip leading blanks
  for ( pszCommand = szCommand; *pszCommand == ' '; pszCommand++ ) { ; }

  #if defined(_WIN32)

    StartupInfo.cb = sizeof(StartupInfo);
    if (!CreateProcess(NULL, pszCommand, NULL, NULL, FALSE,
                       CREATE_NEW_PROCESS_GROUP,
                       NULL, NULL, &StartupInfo, &ProcessInfo)) {
      //retry command execution with CMD.EXE /C ...
      char szExecLine[sizeof(tsp00_ExecArgLine)+1 + sizeof(CMD_FILENAME)
                                    + sizeof(CMD_COMMAND_PARAMETER)];
      strcpy(szExecLine, CMD_FILENAME);
      strcat(szExecLine, " ");
      strcat(szExecLine, CMD_COMMAND_PARAMETER);
      strcat(szExecLine, " ");
      strcat(szExecLine, pszCommand);
      if (!CreateProcess(NULL, szExecLine, NULL, NULL, FALSE,
                         CREATE_NEW_PROCESS_GROUP,
                         NULL, NULL, &StartupInfo, &ProcessInfo)) {
        *exec_ret = ex_notok;
        DBGOUT;
        return;
      }
    }
    *exec_ret = ex_ok;
    CloseHandle(ProcessInfo.hProcess);
    CloseHandle(ProcessInfo.hThread);
  #else
    //
    // --- get program name and parameters
    //
    pszParameter = strchr (pszCommand, ' ');

    if (pszParameter != NULL)
      {
      ulStrLen = min (pszParameter - pszCommand, sizeof (szPgmName) - 1);

      // --- skip leading blanks
      for (; *pszParameter == ' '; pszParameter++ ) { ; }
      }
    else
      ulStrLen = sizeof (szPgmName) - 1;

    strncpy (szPgmName, pszCommand, ulStrLen );
    szPgmName[ulStrLen] = '\0';

    // --- get the fully-qualified file name of the program to be started
    if (sql59k_get_path ( szFullyQualifiedPgmName, szPgmName) != NO_ERROR)
      {
      //
      // --- unkown program; it might be an internal command or
      //     program without an extention.
      //     Use CMD.EXE to start this command or program
      //
      if (sql59k_get_path ( szFullyQualifiedPgmName, CMD_FILENAME ) != NO_ERROR)
        {
        // --- cmd.exe not found!
        *exec_ret = ex_notok;
        sql46c_build_error_string ( err_text, ERRMSG_CMD_FILE_NOT_FOUND, 0 );
        MSGALL(( ERR_CMD_FILE_NOT_FOUND ));
        DBG3  (( MF__,  "CMD.EXE not found" ));

        return;
        }

      // --- build icon file name
      pszIconFile = szIconFile;
      strcpy (szIconFile, kgs.szDBRoot);
      if (szIconFile[strlen(szIconFile) - 1] == '\\')
        strcat (szIconFile, DB_KERN_ICON_NAME);
      else
        strcat (szIconFile, "\\"DB_KERN_ICON_NAME);

      // --- build parameter string
      strcpy  (szParameter, CMD_COMMAND_PARAMETER);
      strncat (szParameter, pszCommand,
               sizeof (szParameter) - 1 - sizeof (CMD_COMMAND_PARAMETER) - 1);
      pszParameter = szParameter;
      }

    // --- set window title
    szPgmTitle[ sizeof (szPgmTitle) - 1] = '\0';
    strncpy (szPgmTitle, kgs.szServerDB, sizeof (szPgmTitle) - 1);
    strncat (szPgmTitle, " : ", sizeof (szPgmTitle) - 1 - strlen (szPgmTitle));
    strncat (szPgmTitle, szPgmName,
             sizeof (szPgmTitle) - 1 - strlen (szPgmTitle));


    stdataSession.Length        = sizeof(STARTDATA);
    stdataSession.Related       = SSF_RELATED_INDEPENDENT;
    stdataSession.FgBg          = SSF_FGBG_FORE;
    stdataSession.TraceOpt      = SSF_TRACEOPT_NONE;
    stdataSession.PgmTitle      = szPgmTitle;
    stdataSession.PgmName       = szFullyQualifiedPgmName;
    stdataSession.PgmInputs     = pszParameter;
    stdataSession.TermQ         = 0;
    stdataSession.Environment   = NULL;
    stdataSession.InheritOpt    = SSF_INHERTOPT_SHELL;
    stdataSession.SessionType   = SSF_TYPE_DEFAULT;
    stdataSession.IconFile      = pszIconFile;
    stdataSession.PgmHandle     = 0;
    stdataSession.PgmControl    = SSF_CONTROL_VISIBLE;
    stdataSession.InitXPos      = 0;
    stdataSession.InitYPos      = 0;
    stdataSession.InitXSize     = 100;
    stdataSession.InitYSize     = 100;
    stdataSession.Reserved      = 0;
    stdataSession.ObjectBuffer  = ObjBuf;
    stdataSession.ObjectBuffLen = sizeof(ObjBuf);

    DBG3 (( MF__, "szPgmTitle             : %s ", szPgmTitle ));
    DBG3 (( MF__, "szFullyQualifiedPgmName: %s ", szFullyQualifiedPgmName ));
    DBG3 (( MF__, "pszParameter           : %s ", pszParameter ? pszParameter
                                                               : "< NO >" ));

    // --- execute the program or command
    rc = DosStartSession (&stdataSession, &idSession, &idProcess);

    if (( rc != NO_ERROR ) && ( rc != ERROR_SMG_START_IN_BACKGROUND ))
      {
      *exec_ret = ex_notok;
      sql46c_build_error_string ( err_text, ERRMSG_EXEC_COMMAND, rc );
      MSGALL(( ERR_CANNOT_EXEC_FILE, szFullyQualifiedPgmName, rc ));
      DBG3  (( MF__,  "Cannot execute file %s, rc = %d",
                      szFullyQualifiedPgmName, rc ));
      }
    else
      {
      *exec_ret = ex_ok;
      }

  #endif

  DBGOUT;
  return;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

#if !defined(_WIN32)
 static APIRET sql59k_get_path ( PATHNAME pszFullPathName,
                                 PSZ      pszFileName )
   {
   #undef  MF__
   #define MF__ MOD__"sql59k_get_path"
   ULONG         ulIndex;
   PSZ           pszSlash;
   PSZ           pszBackSlash;
   PSZ           pszPoint;
   PSZ           pszFile;
   APIRET        rc         = NO_ERROR;
   LONG          lAtt       = 0;
   ULONG         ulTmp      = 0;

   #if defined(_WIN32)
    PSZ          pszTail   = NULL;
   #endif


   DBGIN;

   DBG3 (( MF__, "%d extensions to search", MX_EXTENSIONS_LIST_SIZE ));

   pszSlash     = strrchr ( pszFileName, '/' );
   pszBackSlash = strrchr ( pszFileName, '\\' );

   if ( pszBackSlash > pszSlash )
     pszSlash = pszBackSlash;

   if ( pszSlash && *( pszSlash + 1 ))
     pszFile = pszSlash + 1;
   else
     pszFile = pszFileName;

   pszPoint = strrchr ( pszFile, '.' );

   if ( pszPoint && *( pszPoint + 1 ) == '\0' )
     {
     *pszPoint = '\0';
     pszPoint  = NULL;
     }

   for ( ulIndex = 0; ulIndex < MX_EXTENSIONS_LIST_SIZE; ulIndex++ )
     {
     if ( !pszPoint || ulIndex )
       {
       // --- no extension or next loop
       strcat ( pszFile, ExtensionList [ ulIndex ] );
       pszPoint = strrchr ( pszFile, '.' );
       }

     if ( pszSlash )
       {
       DBG3 ((MF__, "Trying to open %s...", pszFileName));

       rc = sql44c_get_file_info ( pszFileName, &lAtt, &ulTmp, &ulTmp );

       if (( rc == NO_ERROR ) && ( !(lAtt &= FILE_DIRECTORY) ))
         {
         strcpy ( pszFullPathName, pszFileName );

         DBGOUT;
         return ( NO_ERROR );
         }
       }
     else
       {
       DBG3 ((MF__, "Looking for %s...", pszFile));

       #if defined(_WIN32)
        if ( SearchPath ( NULL, pszFile, ExtensionList [ ulIndex ],
                          sizeof(PATHNAME) - 1, pszFullPathName, &pszTail))
          {
          DBGOUT;
          return ( NO_ERROR );
          }
       #else
        rc = DosSearchPath ( SEARCH_IGNORENETERRS   |
                             SEARCH_ENVIRONMENT     |
                             SEARCH_CUR_DIRECTORY,
                             "PATH",
                             pszFile,
                             pszFullPathName,
                             sizeof(PATHNAME) - 1);
        if (rc == NO_ERROR)
          {
          //
          // --- 'DosSearchPath' BUG FIX:
          //     check if the found name is a directory name in curr. directory
          //
          rc = sql44c_get_file_info ( pszFullPathName, &lAtt, &ulTmp, &ulTmp );

          if (( rc == NO_ERROR ) && ( !(lAtt &= FILE_DIRECTORY) ))
            {
            DBGOUT;
            return ( NO_ERROR );
            }
          }
       #endif

       DBG3 ((MF__, "not found, rc=%d", rc));
       }

     if ( pszPoint )
       *pszPoint = '\0';

     }

   DBGOUT;
   return ( rc );
   }
#endif

/*
 * =============================== END ========================================
 */
