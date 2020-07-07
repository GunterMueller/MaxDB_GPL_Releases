/*!
    @file           Tools_PipeCall.hpp
    @author         BerndV
    @brief          special main function for tools - Specification

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

#ifndef _Tools_PipeCall_HPP_
#define _Tools_PipeCall_HPP_


/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
 */
/*! Size hint for pipe buffer */
#define SAPDB_PARAMETER_PIPE_SIZE    256
/*! Identifier leading character for overlong parameter */
#define SAPDB_PART_OF_LONG_PARAMETER '!'
/*! Identifier of last part or short parameter */
#define SAPDB_LAST_PART_OF_PARAMETER ':'
/*! Identifier of end of parameter transmission */
#define SAPDB_END_OF_PARAMETERS      '.'
/*! Option to select parameter transfer via pipe */
#define SAPDB_OPT_PIPE "-SDB_PARAMSVIAPIPE"
#if defined(WIN32)
/*! Format string for creating named pipe. '%ld' is replaced by process id of caller */
#define SAPDB_NAMED_ARGUMENT_PIPE "\\\\.\\pipe\\arg%ld"
#endif

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include "ToolsCommon/Tools_Array.hpp"

/*!
    @brief     PipeMain class
*/
class Tools_PipeCall {

public:
  /*! 
    @ brief call type
   */
 typedef enum {
    CallSynchron,
    CallAsynchron
  } CallType;

  /*! 
    @ brief mode for reading arguments
   */
 typedef enum {
    Dummy                    = 0x001,
    ReadXUser                = 0x002,  // read user/pwd (if not specified) from XUser if 
                                       // a userkey is specified (-U) or known by argument

    PromptForUser            = 0x004,  // prompt user/pwd if not specified
    PromptForUserOnRequest   = 0x008,  // prompt user/pwd if empty -u is specified

    PromptForDBName          = 0x010,  // prompt database name not specified
    PromptForDBNameOnRequest = 0x020,  // prompt database name if empty -n is specified

    PromptForServer          = 0x040,  // prompt for server name if not specified
    PromptForServerOnRequest = 0x080   // prompt for server name if empty -n is specified

 } ReadArgMode;

  /*! 
    @ brief argument vector type
   */
 typedef Tools_Array<char *> ArgvType;

  /*! @brief CallProgram

      @param  szProgram - command line to call
      @param  oCallType - type of call CallSynchron/CallAsynchron
      @param  pStdout   - Standard out file for new process
      @param  pStderr   - Standard error file for new process

      @return -1 in case of errors or exit code of program
   */
  static int CallProgram
    ( const char * szProgram,
      CallType     oCallType,
      const char * pStdout = NULL,
      const char * pStderr = NULL);

  /*! @brief Read the arguments

      @param  argcIn       - argument counter input
      @param  argvIn       - argument vector input
      @param  argcOut      - argument counter output
      @param  argvOut      - argument vector output
      @param  oReadArgMode - argument reading mode
      @param  pXUserKey    - Defaulr XUserKey to use


      @return 0 on success / -1 on errors
   */
  static int ReadArguments
    ( int          argcIn,
      char **      argvIn,
      int &        argcOut,
      char ** &    argvOut,
      unsigned int oReadArgMode = Dummy,
      const char * pXUserKeyIn  = NULL);

  /*! @brief Free the arguments

      This function frees the data structures  deliverd by ReadArguments

      @param  argc      - argument counter
      @param  argv      - argument vector
   */
  static void FreeArguments
    ( int     argc,
      char ** argv );

  /*! Get the error text

      @return pointer to error text
   */
  static const char * ErrorReason
    ( )
  { return &szError[0]; }
  static const char * ErrorReason
    ( int & nErrnoOut )
  { nErrnoOut = nErrno;
    return &szError[0]; }

private:

  /*! @brief parse commandline
   */
  static void ParseCommandline
    ( const char * szSource, 
      char       * szTarget, 
      int        & argc,
      ArgvType   & argv);

  /*! @brief Execute the program
   */
  static int ExecuteProgram
    ( int        & argc,
      ArgvType   & argv,
      CallType     oCallType,
      const char * pStdout = NULL,
      const char * pStderr = NULL);

  /*! @brief save error reason
   */
  static void SaveError
    ( const char * szText, 
      const char * szSource, 
      int          nLine,
      int          nErrnoIn );

  /*! @brief read value from stdin
   */
  static char * ReadValues
    ( const char * szDataPrompt, 
      const char * szHiddenPrompt,
      int          nLoopIn );

  /*! @brief read usr,pwd from xuser
   */
  static char * ReadXUserData
    ( const char * pXUserKey );


  /*! @brief add a option to a array
   */
  static int AddOption
    ( const char * pData,
      int &        argcOut,
      char ** &    argvOut );

  /*! @brief add a option to a array
   */
  static int AddOption
    ( const char * pData,
      int &        argc,
      ArgvType &   argv );

  /*! @brief free internal arguments
   */
  static void FreeArguments
    ( int      argc,
      ArgvType argv );

  /*! @brief redirect stdout/stderr
   */
  static int RedirectFiles 
     ( const char * pStdout,
       const char * pStderr,
       int        & nStdout,
       int        & nStderr,
       FILE      *& hStdout,
       FILE      *& hStderr);
  
  /*! @brief redirect stdout/stderr
   */
  static void RedirectFiles 
     ( int         nStdout,
       int         nStderr,
       FILE      * hStdout,
       FILE      * hStderr);

  static char szError[1024];
  static int  nErrno;
}; // end class Tools_PipeMain

#endif /* _Tools_PipeMain_HPP_ */
