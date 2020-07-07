/*!
    @file           Tools_PipeCall.hpp
    @author         BerndV
    @brief          special calling function for tools - Implementation

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
*/

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include <stdio.h>   // sprintf
#include <string.h>  // SAPDB_strcpy
#include <fcntl.h>   // O_TEXT
#include <stdlib.h>  // atoi
#include <errno.h>   // errno
#ifdef _WIN32
  #include <io.h>      // pipe, write, read
  #include <process.h> // spawnlp, wait 
#  include <windows.h>
#else
  #include <unistd.h>  // pipe, write, read, fork, execlp
  #include <sys/wait.h> // waitpid
#endif

#include "heo02.h"
#include "heo13.h"
#include "heo06.h"
#include "hsp02.h"

#include "SAPDBCommon/SAPDB_string.h"
#include "ToolsCommon/Tools_PipeCall.hpp"

/* 
  -----------------------------------------------------------------------------
  os dependent definitions
  -----------------------------------------------------------------------------
*/
#define READ     0
#define WRITE    1

#define OPT_USER     "-u"
#define OPT_USERKEY  "-U"
#define OPT_DBNAME   "-d"
#define OPT_SERVER   "-n"
#define OPT_BREAK    "-c"
#define OPT_TPSYSID  "-tpi"
#define OPT_TPCONN   "-tpc"
#define OPT_TPPROF   "-tpp"

#define OLD_SAPDB_END_OF_PARAMETERS "EOPIPE"

#define OPT_CHAR     '-'

#define TXT_MSG_ENTERUSER   "Enter user name: "
#define TXT_MSG_ENTERPWD    "Enter password: "
#define TXT_MSG_ENTERDBNAME "Enter database name: "
#define TXT_MSG_ENTERSERVER "Enter node name: "


/* 
  -----------------------------------------------------------------------------
  os dependent definitions
  -----------------------------------------------------------------------------
*/
#ifdef _WIN32
  #define CREATE_PIPE_STRING(s_,h_) sprintf(s_, SAPDB_NAMED_ARGUMENT_PIPE, (long)GetCurrentProcessId() )
  #define HPROCESS                  intptr_t
  #define OPEN_PIPE(hdl)            openPipe(hdl)
                                    int openPipe(HANDLE &pipeHandle)
                                    {
                                        char pipeName[60];
                                        CREATE_PIPE_STRING(pipeName, "dummy");
                                        pipeHandle = CreateNamedPipe( pipeName,
                                                                      PIPE_ACCESS_OUTBOUND,
                                                                      PIPE_TYPE_BYTE     |
                                                                      PIPE_READMODE_BYTE |
                                                                      PIPE_WAIT,
                                                                      1,
                                                                      SAPDB_PARAMETER_PIPE_SIZE,
                                                                      SAPDB_PARAMETER_PIPE_SIZE,
                                                                      INFINITE,
                                                                      NULL );
                                        if ( INVALID_HANDLE_VALUE == pipeHandle )
                                        {
                                            return -1;
                                        }

                                        return 0;
                                    }
#define SPAWN_PROCESS(prog, arg0, pipe, bWait)  spawnlp( bWait?P_NOWAIT:P_DETACH, prog, arg0, SAPDB_OPT_PIPE, pipe, NULL )
  #define WAIT_PROCESS(exit,proc)   cwait( exit, proc, WAIT_CHILD );
  #define WEXITSTATUS(a)            a
#else
  #define HPROCESS                  int
  #define OPEN_PIPE(a)              pipe(a)
  #define SPAWN_PROCESS(prog, arg0, pipe, bWait)  mySpawn(prog,pipe, bWait)
                                    int mySpawn (const char * prog, const char * pipe, bool bWait) {
                                      int hProcess = fork();
                                      if(hProcess==0) {
                                        if (!bWait) {
                                          int hProcess2 = fork();
                                          if(hProcess2==0) {
                                            if (execlp(prog, prog, SAPDB_OPT_PIPE, pipe, NULL )  == -1 ) {
                                              exit( 1 );
                                            } // end if
                                          } else {
                                            exit(0);
                                          } // end if
                                        } else {
                                          if (execlp(prog, prog, SAPDB_OPT_PIPE, pipe, NULL )  == -1 ) {
                                            exit( 1 );
                                          } // end if
                                        } // end if
                                      } // end if
                                      return hProcess;
                                    } // mySpawn
  #define WAIT_PROCESS(exit,proc)   waitpid( proc, exit, 0);
  #define CREATE_PIPE_STRING(s_,h_) sprintf(s_, "%d", h_[READ])
#endif


/* 
  -----------------------------------------------------------------------------
  static members
  -----------------------------------------------------------------------------
*/
char Tools_PipeCall::szError[1024];
int  Tools_PipeCall::nErrno = 0;

/*
 * static function Tools_PipeCall::CallProgram
 */
int Tools_PipeCall::CallProgram
  ( const char               * szProgram,
    Tools_PipeCall::CallType   oCallType ,
    const char *               pStdout,
    const char *               pStderr)
{
  char     * szTarget = new char[strlen(szProgram) + 1];
  ArgvType   argv;
  int        argc;

  if (szTarget == NULL) {
    SaveError("Memory allcocation error", __FILE__, __LINE__, errno);
    return -1;
  } // end if

  ParseCommandline(szProgram, szTarget, argc, argv);

  if (argc == 0) {
    delete [] szTarget;
    SaveError("No program found", __FILE__, __LINE__, 0);
    return -1;
  } // end if

  int nRc = ExecuteProgram(argc, argv, oCallType, pStdout, pStderr);

  delete [] szTarget;

  return nRc;
} // end Tools_PipeCall::CallProgram

/*
 * static function Tools_PipeCall::ParseCommandline
 */
void Tools_PipeCall::ParseCommandline
  ( const char * szSource, 
    char *       szTarget, 
    int        & argc,
    ArgvType   & argv)
{
  size_t nLength   = strlen(szSource);
  size_t nSource   = 0;
  size_t nTarget   = 0;
  bool   bInToken  = false;
  bool   bInString = false;
  bool   bIgnore   = false;

  argc = 0;
  argv.clear();

  for (nSource = 0; nSource < nLength; ++nSource) {
    switch (szSource[nSource]) {
      case '\\':
        if (bIgnore) {
          if (!bInToken) {
            bInToken = true;
            argv[argc] = &szTarget[nTarget];
            ++argc;
          } // end if
          szTarget[nTarget++] = '\\';
          szTarget[nTarget++] = szSource[nSource];
          bIgnore = false;
        } else {
          bIgnore = true;
        } // end if
        break;
      case '"':
        if (bIgnore) {
          if (!bInToken) {
            bInToken = true;
            argv[argc] = &szTarget[nTarget];
            ++argc;
          } // end if
          szTarget[nTarget++] = szSource[nSource];
          bIgnore = false;
        } else {
          bInString = !bInString;
        } // end if
        break;
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        if (bIgnore) {
          if (!bInToken) {
            bInToken = true;
            argv[argc] = &szTarget[nTarget];
            ++argc;
          } // end if
          szTarget[nTarget++] = '\\';
          bIgnore = false;
        } // end if
        if(!bInString) {
          szTarget[nTarget++] = '\0';
          bInToken = false;
        } else {
          szTarget[nTarget++] = szSource[nSource];
        } // end if
        break;
      default:
        if (!bInToken) {
          bInToken = true;
          argv[argc] = &szTarget[nTarget];
          ++argc;
        } // end if
        if (bIgnore) {
          szTarget[nTarget++] = '\\';
          bIgnore = false;
        } // end if
        szTarget[nTarget++] = szSource[nSource];
        break;
    } // end switch
  } // end if
  
  szTarget[nTarget++] = '\0';
} // end Tools_PipeCall::ParseCommandline

/*
 * static function Tools_PipeCall::ExecuteProgram
 */
int Tools_PipeCall::ExecuteProgram
    ( int                      & argc,
      ArgvType                 & argv,
      Tools_PipeCall::CallType   oCallType,
      const char *               pStdout,
      const char *               pStderr)
{
#if defined(WIN32)
  HANDLE hPipe;
#else
  int  hPipe[2];
#endif

  // Open a set of pipes 
  if( OPEN_PIPE( hPipe ) == -1) {
    SaveError("Can not create pipe", __FILE__, __LINE__, errno);
    return -1;
  } // end if

  // redirect output
  int         nStdout;
  int         nStderr;
  FILE      * hStdout;
  FILE      * hStderr;
  if (RedirectFiles(pStdout, pStderr, nStdout, nStderr, hStdout, hStderr) == -1) {
    RedirectFiles(nStdout, nStderr, hStdout, hStderr);
    return -1;
  } // end if

  char szPipe[60];
  // create pipe string
  CREATE_PIPE_STRING(szPipe, hPipe);

  HPROCESS  hProzess;
  // start process
  char * arg0 = NULL;
#if defined(WIN32)
  arg0 = new char[strlen(argv[0]) + 3];
  if (arg0 == NULL) {
    SaveError("Memory allcocation error", __FILE__, __LINE__, errno);
    return -1;
  } // end if
  sprintf(arg0, "\"%s\"", argv[0]);
#endif 

  if( ( hProzess = SPAWN_PROCESS(  argv[0], arg0, szPipe, (oCallType == CallSynchron)) ) == -1 ) {

    SaveError("Can not spawn process", __FILE__, __LINE__, errno);
#if !defined(WIN32)
    close( hPipe[READ] );
    close( hPipe[WRITE] );
#endif
    RedirectFiles(nStdout, nStderr, hStdout, hStderr);
	if (arg0 != NULL) delete [] arg0;
    return -1;
  } // end if

  RedirectFiles(nStdout, nStderr, hStdout, hStderr);

#if defined(WIN32)
  ConnectNamedPipe(hPipe, 0);
  DWORD bytesWritten = 0;
#endif

  // send parameters
  int  nIndex = 0;
  char szParameter[SAPDB_PARAMETER_PIPE_SIZE];
  char *argument;
  int argLength;

  for (nIndex = 1;nIndex < argc; ++nIndex) {
      argument = argv[nIndex];
      argLength = (int)strlen(argument);
      while ( argLength >= (SAPDB_PARAMETER_PIPE_SIZE-1) )
      {
          szParameter[0] = SAPDB_PART_OF_LONG_PARAMETER;
          SAPDB_memcpy(&szParameter[1], argument, SAPDB_PARAMETER_PIPE_SIZE-1);
          argLength -= (SAPDB_PARAMETER_PIPE_SIZE-1);
#if defined(WIN32)
          WriteFile(hPipe, szParameter, SAPDB_PARAMETER_PIPE_SIZE, &bytesWritten, 0);
#else
          write( hPipe[WRITE], szParameter, SAPDB_PARAMETER_PIPE_SIZE);
#endif
          argument  += (SAPDB_PARAMETER_PIPE_SIZE-1);
      }

      szParameter[0] = SAPDB_LAST_PART_OF_PARAMETER;
      SAPDB_memcpy(&szParameter[1], argument, argLength);
      memset(&szParameter[argLength+1], 0, SAPDB_PARAMETER_PIPE_SIZE - (argLength+1));
#if defined(WIN32)
      WriteFile(hPipe, szParameter, SAPDB_PARAMETER_PIPE_SIZE, &bytesWritten, 0);
#else
      write( hPipe[WRITE], szParameter, SAPDB_PARAMETER_PIPE_SIZE);
#endif
  } // end for
  szParameter[0] = SAPDB_END_OF_PARAMETERS;
  memset(&szParameter[1], 0, SAPDB_PARAMETER_PIPE_SIZE - 1);
#if defined(WIN32)
  WriteFile(hPipe, szParameter, SAPDB_PARAMETER_PIPE_SIZE, &bytesWritten, 0);
  CloseHandle(hPipe);
#else
  write( hPipe[WRITE], szParameter, SAPDB_PARAMETER_PIPE_SIZE);

  close( hPipe[READ] );
  close( hPipe[WRITE] );
#endif

  int  nExit = 0;

  if (oCallType == CallSynchron) {
    WAIT_PROCESS( &nExit, hProzess );
  } // end if

  if (arg0 != NULL) delete [] arg0;
  return WEXITSTATUS(nExit);
} //  end Tools_PipeCall::ExecuteProgram

/*
 * static function Tools_PipeCall::ReadArguments
 */
int Tools_PipeCall::ReadArguments
    ( int          argcIn,
      char **      argvIn,
      int &        argcOut,
      char ** &    argvOut,
      unsigned int oReadArgMode,
      const char * pXUserKeyIn )
{
  int  nRc      = 0;

  bool bUserPwd = false;
  bool bDBName  = false;
  bool bServer  = false;

  bool bXUser   = false;

  bool bTPArgs  = false;

  bool bBreak   = false;
  bool bCopyArg = false;

  const char * pXUserKey = pXUserKeyIn;

  argcOut = 0;

  int        argcPass1;
  ArgvType   argvPass1;

  int        argcPass2;
  ArgvType   argvPass2;

  char     * pArgument = NULL;

  argcPass1 = 0;
  argvPass1.clear();
  argcPass2 = 0;
  argvPass2.clear();

  int nIndex  = 0;

  // pipe analysis
  for (nIndex = 0; (nIndex < argcIn) && (nRc == 0); ++nIndex) {
    if (strcmp(SAPDB_OPT_PIPE, argvIn[nIndex]) == 0) {
      ++nIndex;
      if (nIndex < argcIn) {
        char szParameter[SAPDB_PARAMETER_PIPE_SIZE + 1];
        char *completeParameter = 0;
        char *tmpCompleteParameter;
        long completeLength = 0;

        // Convert passed string handle to integer handle.
        int  hPipe = atoi( argvIn[nIndex] );

#if defined(WIN32)
        HANDLE hNamedPipe = INVALID_HANDLE_VALUE;
        DWORD  bytesIn;
        hNamedPipe = CreateFile( argvIn[nIndex],
                                 GENERIC_READ,
                                 FILE_SHARE_READ|FILE_SHARE_WRITE,
                                 0,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL );

        while (1) {
          if ( INVALID_HANDLE_VALUE != hNamedPipe ) {
            if( !ReadFile( hNamedPipe, szParameter, SAPDB_PARAMETER_PIPE_SIZE, &bytesIn, 0 ) ) {
              break;
            } // end if
          } else {
            // compatibility mode, maybe it is an unamed pipe
            if(read(hPipe, szParameter, SAPDB_PARAMETER_PIPE_SIZE) < 0 )  {
                break;
            } // end if
          } // end if
#else
        while(read(hPipe, szParameter, SAPDB_PARAMETER_PIPE_SIZE) >= 0) {
#endif
          int iParam;
          for ( iParam = 1; iParam < SAPDB_PARAMETER_PIPE_SIZE; iParam++ ) {
            if ( szParameter[iParam] == 0 )
                break;
          } // end for

          switch( szParameter[0] )
          {
          case SAPDB_PART_OF_LONG_PARAMETER:
              if ( iParam != SAPDB_PARAMETER_PIPE_SIZE )
              {
                  SaveError("Invalid long parameter found", __FILE__, __LINE__, iParam);
                  nRc = -1;
                  break;
              }

              tmpCompleteParameter = completeParameter;
              completeParameter = new char [completeLength+SAPDB_PARAMETER_PIPE_SIZE-1];
              if ( !completeParameter )
              {
                  SaveError("Memory allcocation error", __FILE__, __LINE__, errno);
                  nRc = -1;
                  break;
              }
              if ( tmpCompleteParameter )
              {
                  SAPDB_memcpy(completeParameter, tmpCompleteParameter, completeLength);
                  delete [] tmpCompleteParameter;
              }
              SAPDB_memcpy(&completeParameter[completeLength], &szParameter[1], SAPDB_PARAMETER_PIPE_SIZE-1);
              completeLength += (SAPDB_PARAMETER_PIPE_SIZE - 1);
              break;

          case SAPDB_LAST_PART_OF_PARAMETER:
              if ( !completeParameter )
              {
                  nRc = AddOption(&szParameter[1], argcPass1, argvPass1);
              }
              else
              {
                  tmpCompleteParameter = completeParameter;
                  completeParameter = new char [completeLength+SAPDB_PARAMETER_PIPE_SIZE];
                  if ( !completeParameter )
                  {
                      SaveError("Memory allcocation error", __FILE__, __LINE__, errno);
                      nRc = -1;
                      break;
                  }

                  SAPDB_memcpy(completeParameter, tmpCompleteParameter, completeLength);
                  delete [] tmpCompleteParameter;

                  if ( iParam > 1 )
                  {
                      SAPDB_memcpy(&completeParameter[completeLength], &szParameter[1], iParam-1);
                  }
                  completeParameter[completeLength + iParam - 1] = 0;

                  nRc = AddOption(completeParameter, argcPass1, argvPass1);
                  delete [] completeParameter;
                  completeParameter = 0;
                  completeLength = 0;
              }
              break;
          case SAPDB_END_OF_PARAMETERS:
              break;
          default:
              if (strcmp(szParameter, OLD_SAPDB_END_OF_PARAMETERS) != 0) {
                  nRc = AddOption(szParameter, argcPass1, argvPass1);
              } // end if
              break;

          } // end switch
          if ( nRc == -1 || SAPDB_END_OF_PARAMETERS == szParameter[0] || strcmp(szParameter, OLD_SAPDB_END_OF_PARAMETERS) == 0)
          {
              break;
          }
        } // end while
#if defined(WIN32)
        if (hNamedPipe != INVALID_HANDLE_VALUE) CloseHandle(hNamedPipe);
#endif
      } else {
        SaveError("Pipe handle not found", __FILE__, __LINE__, errno);
        nRc = -1;
      } // end if

    } else {
      nRc = AddOption(argvIn[nIndex], argcPass1, argvPass1);
    } // end if
  } // end for

  // special argument reading XUser, user/pwd, server, dbname
  if (nRc == 0) {
    bUserPwd = ((oReadArgMode & PromptForUser)   != 0);
    bDBName  = ((oReadArgMode & PromptForDBName) != 0);
    bServer  = ((oReadArgMode & PromptForServer) != 0);

    bXUser  = ((oReadArgMode & ReadXUser)       != 0);
  } // end if

  bool bXUserOptionFound(false);

  // copy from tmp variables to out parameters
  if (nRc == 0) {
    argcPass2 = 0;
    nIndex = 0;
    while ((nIndex < argcPass1) && (nRc == 0)) { 

      bCopyArg = true;
      // check for special arguments
      if ((strcmp(argvPass1[nIndex], OPT_USER) == 0) && !bBreak) {
        // true because option -u found
        bUserPwd = bUserPwd || ((oReadArgMode & PromptForUserOnRequest)   != 0);
        // back to false if user/pwd found
        bUserPwd = bUserPwd && (((nIndex + 1) == argcPass1) || (((nIndex + 1) < argcPass1) && (argvPass1[nIndex+1][0] == OPT_CHAR)));
        bXUser   = bXUser   && (((nIndex + 1) == argcPass1) || (((nIndex + 1) < argcPass1) && (argvPass1[nIndex+1][0] == OPT_CHAR)));
        // skip option
        bCopyArg = !bUserPwd;
      } else if (((strcmp(argvPass1[nIndex], OPT_TPSYSID) == 0) ||
                  (strcmp(argvPass1[nIndex], OPT_TPCONN ) == 0) ||
                  (strcmp(argvPass1[nIndex], OPT_TPPROF ) == 0)     ) && !bBreak) {
        // check for tp arguments this will supress the access to the default xuserkey
        bTPArgs = true;
      } else if ((strcmp(argvPass1[nIndex], OPT_DBNAME) == 0) && !bBreak) {
        // true because option -d found
        bDBName = bDBName || ((oReadArgMode & PromptForDBNameOnRequest)   != 0);
        // back to false if dbname found
        bDBName = bDBName && (((nIndex + 1) == argcPass1) || (((nIndex + 1) < argcPass1) && (argvPass1[nIndex+1][0] == OPT_CHAR)));
        // skip option
        bCopyArg = !bDBName;
      } else if ((strcmp(argvPass1[nIndex], OPT_SERVER) == 0) && !bBreak) {
        // true because option -n found
        bServer = bServer || ((oReadArgMode & PromptForServerOnRequest)   != 0);
        // back to false if server found
        bServer = bServer && (((nIndex + 1) == argcPass1) || (((nIndex + 1) < argcPass1) && (argvPass1[nIndex+1][0] == OPT_CHAR)));
        // skip option
        bCopyArg = !bServer;
      } else if ((strcmp(argvPass1[nIndex], OPT_USERKEY) == 0) && !bBreak) {
        // save user key
        bXUserOptionFound = true;
        if ((bXUser) && ((nIndex + 1) < argcPass1) && (argvPass1[nIndex+1][0] != OPT_CHAR)) {
          pXUserKey = argvPass1[nIndex+1];
        } // end if
        // skip option
        //bCopyArg = !bXUser; // copy both -U and <UKEY> to output
      } else if ((strcmp(argvPass1[nIndex], OPT_BREAK) == 0) && !bBreak) {
        bBreak = true;
      } // end if

      if (bCopyArg) {
        nRc = AddOption(argvPass1[nIndex], argcPass2, argvPass2);
      } // end if
      ++nIndex;
      
    } // end while
  } // end if

  // allocat output array
  if (nRc == 0) {
    argcOut = 0;
    argvOut = new char * [argcPass2 + 6];
    if (argvOut == NULL) {
      SaveError("Memory allcocation error", __FILE__, __LINE__, errno);
      nRc = -1;
    } else {
      nRc = AddOption(argvPass2[0], argcOut, argvOut);
    } // end if
  } // end if

  // search XUser data
  char* convertedUserData(NULL);
  if (nRc == 0 && bXUser && !(bTPArgs && (pXUserKey == pXUserKeyIn))) {
    if ((pXUserKey != NULL) && (strlen(pXUserKey) > 0)) {
      convertedUserData = ReadXUserData(pXUserKey);
      if (convertedUserData != NULL && !bXUserOptionFound ) {
        bUserPwd = false;
        nRc = AddOption(OPT_USER, argcOut, argvOut);
        if (nRc == 0) {
          nRc = AddOption(convertedUserData, argcOut, argvOut);
          delete[] convertedUserData;
          convertedUserData = NULL;
        } // end if
      }
      else if (convertedUserData == NULL && pXUserKey != pXUserKeyIn) {
        // only error if key not default
        nRc = -1;
      } // end if
    } // end if
  } // end if

  // prompt for server
  if (nRc == 0 && bServer) {
    nRc = AddOption(OPT_SERVER, argcOut, argvOut);
    if (nRc == 0) {
      pArgument = ReadValues(TXT_MSG_ENTERSERVER, NULL, 3);
      if (pArgument != NULL) {
        nRc = AddOption(pArgument, argcOut, argvOut);
        delete [] pArgument;
      } else {
        nRc = -1;
      } // end if
    } // end if
  } // end if

  // prompt for database name
  if (nRc == 0 && bDBName) {

    nRc = AddOption(OPT_DBNAME, argcOut, argvOut);
    if (nRc == 0) {
      pArgument = ReadValues(TXT_MSG_ENTERDBNAME, NULL, 3);
      if (pArgument != NULL) {
        nRc = AddOption(pArgument, argcOut, argvOut);
        delete [] pArgument;
      } else {
        nRc = -1;
      } // end if
    } // end if
  } // end if

  // prompt for user name
  if (nRc == 0 && bUserPwd) {
    nRc = AddOption(OPT_USER, argcOut, argvOut);
    if (nRc == 0) {
      pArgument = ReadValues(TXT_MSG_ENTERUSER, TXT_MSG_ENTERPWD, 3);
      if (pArgument != NULL) {
        nRc = AddOption(pArgument, argcOut, argvOut);
        delete [] pArgument;
      } else {
        nRc = -1;
      } // end if
    } // end if
  } // end if

  // copy pass2-array to output
  if (nRc == 0) {
    for (nIndex = 1; (nIndex < argcPass2) && (nRc == 0); ++nIndex) {
      // special handling for xuser data
      if( convertedUserData != NULL && strcmp(argvPass2[nIndex], OPT_USERKEY) == 0 ) {
        nRc = AddOption(OPT_USER, argcOut, argvOut);
        if( nRc == 0 ) {
          ++nIndex;
          nRc = AddOption(convertedUserData, argcOut, argvOut);
          delete[] convertedUserData;
          convertedUserData = NULL;
        }
      }
      else 
        nRc = AddOption(argvPass2[nIndex], argcOut, argvOut);
    } // end for
  } // end if

  // delete arrays
  FreeArguments(argcPass1, argvPass1);
  FreeArguments(argcPass2, argvPass2);

  return nRc;
} // end Tools_PipeCall::ReadArguments

/*
 * static function Tools_PipeCall::SaveError
 */
void Tools_PipeCall::SaveError
    ( const char * szText, 
      const char * szSource, 
      int          nLine,
      int          nErrnoIn )
{
  if (nErrnoIn != 0) {
    sprintf(szError, "%s - %s (%s,%d)", szText, strerror(nErrnoIn), szSource, nLine);
  } else {
    sprintf(szError, "%s (%s,%d)", szText, szSource, nLine);
  } // end if
  nErrno = nErrnoIn;
} // end Tools_PipeCall::SaveError

/*
 * static function Tools_PipeCall::ReadValues
 */
char * Tools_PipeCall::ReadValues
    ( const char * szDataPrompt, 
      const char * szHiddenPrompt,
      int          nLoopIn )
{
  char        szData[SAPDB_PARAMETER_PIPE_SIZE+1] = "";
  int         nLoop          = 0;
  char      * pData          = NULL;
  tsp05_RteFileError rteError;
  tsp00_Namec szHidden;

  szHidden.Init();

    while (nLoop < nLoopIn) {
      if (szDataPrompt != NULL) {
        printf(szDataPrompt);
        fgets(szData, SAPDB_PARAMETER_PIPE_SIZE, stdin);
        int nLen = (int) strlen(szData) - 1;
        while ((nLen >= 0) && ((szData[nLen] == '\r') || (szData[nLen] == '\n'))) {
          szData[nLen] = 0;
        } // end while

      } // end if
      if (szHiddenPrompt != NULL) {
        if (!sqlgetpass(szHiddenPrompt, szHidden, rteError)) {
          tsp00_C40c szRTEError;
          szRTEError.p2c(rteError.sp5fe_text);
          SaveError(szRTEError, __FILE__, __LINE__, rteError.sp5fe_result);
          break;
        } // end if
      } // end if

      if ((szDataPrompt != NULL) && (szHiddenPrompt != NULL) && 
          (strlen(szData) > 0)   && (strlen(szHidden) > 0)) {
        pData = new char [strlen(szData) + strlen(szHidden) + 2];
        if (pData != NULL) {
          SAPDB_strcpy(pData, szData);
          strcat(pData, ",");
          strcat(pData, szHidden);
        } else {
          SaveError("Memory allcocation error", __FILE__, __LINE__, errno);
        } // end if
        break;
      } else if ((szDataPrompt != NULL) && (strlen(szData) > 0)) {
        pData = new char [strlen(szData) + 1];
        if (pData != NULL) {
          SAPDB_strcpy(pData, szData);
        } else {
          SaveError("Memory allcocation error", __FILE__, __LINE__, errno);
        } // end if
        break;
      } // end if
      ++nLoop;
    } // end while

  if ((nLoop == nLoopIn) && (pData == NULL)) {
    SaveError("No Input given", __FILE__, __LINE__, 0);
  } // end if
  return pData;
} // end Tools_PipeCall::ReadValues

/*
 * static function Tools_PipeCall::ReadXUserData
 */
char * Tools_PipeCall::ReadXUserData
    ( const char * pXUserKey )
{
  char              * pData = NULL;
  tsp4_xuser_record   recUser;
  tsp00_ErrText       spError;
  tsp00_Bool          bOk;
 
  memset(recUser.xu_key, ' ', sizeof(recUser.xu_key));
  strncpy(recUser.xu_key, pXUserKey, strlen(pXUserKey));
  sqlgetuser (&recUser, NULL, spError, &bOk);

  if (bOk) {
    tsp00_Name           spClear;
    tsp00_Namec          szPwd;
    tsp00_KnlIdentifierc szUser;

    s02appldecrypt(spClear, recUser.xu_password);

    szPwd.p2c(spClear);
    szUser.p2c(recUser.xu_user);

    pData = new char [strlen(szUser) + strlen(szPwd) + 2];
    if (pData != NULL) {
      SAPDB_strcpy(pData, szUser);
      strcat(pData, ",");
      strcat(pData, szPwd);
    } else {
      SaveError("Memory allcocation error", __FILE__, __LINE__, errno);
    } // end if

  } else {
    tsp00_ErrTextc szRTEError;
    szRTEError.p2c(spError);
    SaveError(szRTEError, __FILE__, __LINE__, 0);
  } // end if

  return pData;
} // end Tools_PipeCall::ReadXUserData

/*
 * static function Tools_PipeCall::AddOption
 */
int Tools_PipeCall::AddOption
    ( const char * pData,
      int &        argc,
      char ** &    argv ) 
{
  char * pArgument =  new char [strlen(pData) + 1];

  if (pArgument != NULL) {
    SAPDB_strcpy(pArgument, pData);
    argv[argc++] = pArgument;
  } else {
    SaveError("Memory allcocation error", __FILE__, __LINE__, errno);
    return -1;
  } // end if
  return 0;
} // end Tools_PipeCall::AddOption

/*
 * static function Tools_PipeCall::AddOption
 */
int Tools_PipeCall::AddOption
    ( const char * pData,
      int &        argc,
      ArgvType &   argv ) 
{
  char * pArgument =  new char [strlen(pData) + 1];

  if (pArgument != NULL) {
    SAPDB_strcpy(pArgument, pData);
    argv[argc++] = pArgument;
  } else {
    SaveError("Memory allcocation error", __FILE__, __LINE__, errno);
    return -1;
  } // end if
  return 0;
} // end Tools_PipeCall::AddOption

/*
 * static function Tools_PipeCall::FreeArguments
 */
void Tools_PipeCall::FreeArguments
    ( int     argc,
      char ** argv )
{
  if (argv != NULL) {
    for (int nIndex = 0; nIndex < argc; ++nIndex) {
      if (argv[nIndex]  != 0) delete [] argv[nIndex];
    } // end if
  if (argv  != 0) delete [] argv;
  } // end if
} // end Tools_PipeCall::FreeArguments

/*
 * static function Tools_PipeCall::FreeArguments
 */
void Tools_PipeCall::FreeArguments
    ( int     argc,
      ArgvType argv )
{
  for (int nIndex = 0; nIndex < argc; ++nIndex) {
    if (argv[nIndex]  != 0) delete [] argv[nIndex];
  } // end if
} // end Tools_PipeCall::FreeArguments

int Tools_PipeCall::RedirectFiles 
   ( const char * pStdout,
     const char * pStderr,
     int        & nStdout,
     int        & nStderr,
     FILE      *& hStdout,
     FILE      *& hStderr)
{
  nStdout = 0;
  nStderr = 0;
  hStdout = NULL;
  hStderr = NULL;

  if (pStdout != NULL) {

    hStdout = fopen(pStdout, "w");

    if (hStdout == NULL) {
      SaveError("Can not open stdout", __FILE__, __LINE__, errno);
      return -1;
    } // end if

    nStdout = dup(fileno(stdout));

    if (nStdout == -1) {
      SaveError("Can not duplicate stdout", __FILE__, __LINE__, errno);
      return -1;
    } // end if

    if (dup2( fileno( hStdout ), fileno(stdout) ) == -1 ) {
      SaveError("Can not reopen stdout", __FILE__, __LINE__, errno);
      return -1;
    } // end if

  } // end if

  if (pStderr != NULL) {

    if ((pStdout != NULL) && (strcmp(pStderr, pStdout) == 0) ){
      hStderr = hStdout;
    } else {
      hStderr = fopen(pStderr, "w");
    } // end if

    if (hStderr == NULL) {
      SaveError("Can not open stderr", __FILE__, __LINE__, errno);
      return -1;
    } // end if

    nStderr = dup(fileno(stderr));

    if (nStderr == -1) {
      SaveError("Can not duplicate stderr", __FILE__, __LINE__, errno);
      return -1;
    } // end if

    if (dup2( fileno( hStderr ), fileno(stderr) ) == -1 ) {
      SaveError("Can not reopen stderr", __FILE__, __LINE__, errno);
      return -1;
    } // end if

  } // end if

  return 0;
} // end if

void Tools_PipeCall::RedirectFiles 
   ( int         nStdout,
     int         nStderr,
     FILE      * hStdout,
     FILE      * hStderr)
{
  if (nStdout != 0) {
    fflush( stdout  );
    dup2( nStdout, fileno(stdout));
    close(nStdout);
  } // end if

  if (nStderr != 0) {
    fflush( stderr  );
    dup2( nStderr, fileno(stderr));
    close(nStderr);
  } // end if

  if (hStdout != NULL) {
    fclose( hStdout );
  } // end if

  if ((hStderr != NULL) && (hStderr != hStdout)) {
    fclose( hStderr );
  } // end if
} // end if
