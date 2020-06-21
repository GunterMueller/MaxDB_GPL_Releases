/*!
  -----------------------------------------------------------------------------
  module: vls40bload.cpp
  -----------------------------------------------------------------------------
  responsible:  SteffenS, d033893

  special area: Loader

  description:  implementation of command line Loader access

  version:      7.6.
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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

*/

/*!
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "hls05.h"
#include "gsp00.h"

#include "gsp01.h"
#include "vsp001.h"
#include "gsp09.h"

#include "geo47.h"

#include "heo01.h"      // function to define db root directory
#include "heo02.h"
#include "heo03.h"
#include "hcn13.h"
#include "hcn14.h"
#include "hls30.h"
#include "hls40cli.h"
#include "hls98msg.h"
#include "RunTime/System/RTESys_ProcessID.hpp"
#include "RunTime/System/RTESys_SystemInfo.h"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"  // unicode support

#define LOADEREXTENSION        _T("transport.py")
#if defined(WIN32)
  #define XPYTHON              _T("x_python.exe")
  #define LOADEREXTENSIONDIR   _T("python2.3\\sdb\\loader\\")
  #define SHELL_DELIMITER      _T("\"")
  #define PATHVAR_DELIMITER    _T(";")
  #define PATH_SEPERATOR       _T("\\")
#else
  #define XPYTHON              _T("x_python")
  #define LOADEREXTENSIONDIR   _T("python2.3/sdb/loader/")
  #define SHELL_DELIMITER      _T("")
  #define PATHVAR_DELIMITER    _T(":")
  #define PATH_SEPERATOR       _T("/")
#endif
#define PYTHONLIB  _T("python2.3.zip")

/*!
  -----------------------------------------------------------------------------
  Specification
  -----------------------------------------------------------------------------
*/

/*!
  -----------------------------------------------------------------------------
   function:     ls40_ConvertArguments
  -----------------------------------------------------------------------------
  description:  Convert command line arguments from UCS2 (UNICODE) to UTF8 on
                Windows.

  arguments:    options         [IN]  - 
                argc
                argU

  returnvalue:  ptr to arguments in UTF8
  -----------------------------------------------------------------------------
*/
#ifdef _WIN32
static char**
ls40_ConvertArguments(tls40_Options * options, 
                      int            argc, 
                      wchar_t      * argU[]);
#endif


/*!
  -----------------------------------------------------------------------------
   function:     ls40_Initialize
  -----------------------------------------------------------------------------
  description:  Initializes the structure with all information
                for execution.

  arguments:    opt         [IN]  - structure to initialize

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
static void
ls40_Initialize(tls40_Options* pOptions);


/*!
  -----------------------------------------------------------------------------
   function:     ls40_Usage
  -----------------------------------------------------------------------------
  description:  Prints usage information.

  arguments:    No

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
static void ls40_Usage();


/*!
  -----------------------------------------------------------------------------
  function:     ls40_ShowPrgVersion
  -----------------------------------------------------------------------------
  description:  Print version info of client program to specified outfile.

  arguments:    OutFile     [IN]   - file pointer of file to print to

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
static void
ls40_ShowPrgVersion(FILE *OutFile);


/*!
  -----------------------------------------------------------------------------
  function:     ls40_ShowServerVersion
  -----------------------------------------------------------------------------
  description:  Print version info of loader server.
                To get the version information the function builds a command and
                sends it to the server. Then it 'analyzes' the reply.

  arguments:    options     [IN]    - structure with all information needed

  returnvalue:  PRG_OK              - success
                PRG_CMD_ERROR       - error getting version, error message is written
                PRG_SERVER_ERROR    - fatal server error getting version,
                                      error message is written
  -----------------------------------------------------------------------------
*/
static int
ls40_ShowServerVersion(tls40_Options & options);


/*!
  -----------------------------------------------------------------------------
  function:     ls40_ParseOptions
  -----------------------------------------------------------------------------
  description:  Parses command line.

  arguments:    options     [IN]        - structure to initialize
                argc        [IN]        - command line argument
                argv        [IN]        - command line argument

  returnvalue:  PRG_OK      - success
                PRG_FATAL   - either error in command line or only the help
                              information should be displayed
  -----------------------------------------------------------------------------
*/
static int
ls40_ParseOptions(tls40_Options *options, int argc, char *argv[]);


/*!
  -----------------------------------------------------------------------------
   function:     ls40_ReadUser
  -----------------------------------------------------------------------------

  description:  Reads user name and password hidden from console

  arguments:    options     [IN/OUT]  - structure that holds the user information

  returnvalue:  PRG_OK      - success
                PRG_FATAL   - error reading password
  -----------------------------------------------------------------------------
*/
static int
ls40_ReadUser(tls40_Options *pOptions);

/*!
  -----------------------------------------------------------------------------
   function:     ls40_ReadTargetUser
  -----------------------------------------------------------------------------

  description:  Reads user name and password hidden from console

  arguments:    options     [IN/OUT]  - structure that holds the user information

  returnvalue:  PRG_OK      - success
                PRG_FATAL   - error reading password
  -----------------------------------------------------------------------------
*/
static int
ls40_ReadTargetUser(tls40_Options *pOptions); 


/*!
  -----------------------------------------------------------------------------
  function:     ls40_ServerConnect
  -----------------------------------------------------------------------------
  description:  Connects the client to the loader server.
                The server node to connect to is chosen in the following way:
                1. No server node given in command line -> the local host is used
                or
                2. command option indicates to use the database server node. If there
                   is no db server node given in the command line anyway the local host
                   is used for the loader server.
                or
                3. An own server node is given for the loader server which will
                   be used.

                In case there are more than one installations of db software of the
                same version on the server where to start the loader server the
                user has to give parameter DBROOT in the command line call which will
                be given through the connect to the C-server.

                Additionally the function fills in the max. packet length in the
                option structure.

  arguments:    *options        [IN]    - structure containing all needed information
                                          to connect to loader server

  returnvalue:  PRG_OK      - success
                PRG_FATAL   - could not connect to server; error message is given
  -----------------------------------------------------------------------------
*/
static int ls40_ServerConnect(tls40_Options *pOptions);


static void
ls40_DisconnectServer(tls40_Options *pOptions);

/*!
  -----------------------------------------------------------------------------
  function:     ls40_UserConnect
  -----------------------------------------------------------------------------
  description:  Connect user to db over the loader manager. User name
                and db name are delivered as options to the program.
                In case either the user name or the db name options are not
                specified the function returns simply PRG_OK without doing anything.
                In this case the user must make sure to specify the user name
                and db name in the script file to the loader server.

  arguments:    opt     [IN]        - structure containing all needed information
                                       to logon user

  returnvalue:  PRG_OK              - success
                PRG_CMD_ERROR       - error executing the command, error message is written
                PRG_SERVER_ERROR    - fatal server error executing the command,
                                      error message is written
  -----------------------------------------------------------------------------
*/
static int ls40_UserConnect(tls40_Options &opt);


/*!
  -----------------------------------------------------------------------------
  function:     ls40_ServerCommand
  -----------------------------------------------------------------------------
  description:  Executes the command delivered in command buffer of options
                structure. Checks the answer of server.

  arguments:    opt     [IN]        - structure containing all needed information
                                       to send the command

  returnvalue:  PRG_OK              - success
                PRG_CMD_ERROR       - error executing the command
                PRG_SERVER_ERROR    - fatal server error executing the command,
                                      error message is written
  -----------------------------------------------------------------------------
*/
static int ls40_ServerCommand(tls40_Options *options);


/*!
  -----------------------------------------------------------------------------
  function:     ls40_CheckAnswer
  -----------------------------------------------------------------------------
  description:  Checks the answer of the server after command execution. Removes
                the server answer header from the reply string. Extracts in case
                of error the error number from the reply string and returns it.

  arguments:    opt     [IN]    - structure containing all needed information

  returnvalue:  PRG_OK          - no error on execution of the command
                value > 0       - error executing the command, return value is
                                  the error number returned by the server
                The reply string contains the reply from the server after returning.
  -----------------------------------------------------------------------------
*/
static int ls40_CheckAnswer(tls40_Options & options, tsp00_Int4 ReplyLen);


/*!
  -----------------------------------------------------------------------------
  function:     ls40_CheckForReleaseCmd
  -----------------------------------------------------------------------------
  description:

  arguments:

  returnvalue:
  -----------------------------------------------------------------------------
*/
static SAPDB_Bool
ls40_CheckForReleaseCmd(char* pszCmdBuffer, SAPDB_UInt4 lCmdLen);


/*!
  -----------------------------------------------------------------------------
  function:     ls40_ExecuteTaskBatch
  -----------------------------------------------------------------------------
  description:  Reads the loader server command file, separates commands and
                sends them to the server.

                Reads 32k from command file in a buffer and works on buffer;
                leaves free space of at least ReplyBufferLen byte in packet for
                answer of server
                At the beginning allocates a buffer of size packet size. For every
                command the buffer is initialized.

  arguments:    opt     [IN]        - structure containing all needed information

  returnvalue:  PRG_OK              - success
                PRG_CMD_ERROR       - error executing the command, error message is written
                PRG_SERVER_ERROR    - fatal server error executing the command,
                                      error message is written
  -----------------------------------------------------------------------------
*/
static int ls40_ExecuteTaskBatch(tls40_Options &opt);


/*
  -----------------------------------------------------------------------------
  function:     ls40_ExecuteTaskInteractive
  -----------------------------------------------------------------------------
*/
static int
ls40_ExecuteTaskInteractive(tls40_Options *pOptions);

/*!
  -----------------------------------------------------------------------------
  function:     ls40_ExecuteTaskSteps
  -----------------------------------------------------------------------------
  description: 

  -----------------------------------------------------------------------------
*/
static int
ls40_ExecuteTaskSteps(tls40_Options &options, char *argv[]);


/*!
  -----------------------------------------------------------------------------
  function:     ls40_InitBuffers
  -----------------------------------------------------------------------------

  description:  Initializes used command and reply buffers in options structure.

  arguments:    opt     [IN]        - structure to initialize

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
static void ls40_InitBuffers(tls40_Options & opt);


/*!
  -----------------------------------------------------------------------------
  function:     ls40_IsCmdSeparator
  -----------------------------------------------------------------------------
  description:  Checks if an null terminated buffer (read line from an command
                file) evaluates to a command separator.

                The function changes the length of the buffer in a way that the
                command separator is cut off if it is not the standard separator
                (which is a double slash at the beginning of a line). In case of
                using the standard separator the line length (plen) is set to 0.

                In case the line consists entirely of white spaces and/or control
                characters the line length (plen) is set to 0.

  arguments:    pCmdSep [IN]        - command separator characters
                p       [IN]        - read line to evaluate
                plen    [IN/OUT]    - line length

  returnvalue:  true  = comment
                false = no comment
  -----------------------------------------------------------------------------
*/
static bool
ls40_IsCmdSeparator(tls40_CmdSeparator *pCmdSep,
                    _TCHAR             *p,
                    tsp00_Uint4        &plen);


/*
  -----------------------------------------------------------------------------
  function:     ls40_Substitute
  -----------------------------------------------------------------------------
  description:  Substitutes parameters of the form &<digit> if such parameter
                was defined at the command line when calling the client.

  arguments:    opt           [IN]      - structure holding all information
                pszBuffer     [IN|OUT]  - string where to substitute the
                                          parameters
                lBufferLength [IN|OUT]  - length of buffer, will be
                                          adapted if parameters are found

  returnvalue:  0  = successful substitution or nothing to substitute; the string
                     can be used as command to send to the server
                -1 = substititution creates a string that exceeds the possible
                     length of command strings
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls40_Substitute(tls40_Options &opt, _TCHAR *pszBuffer, tsp00_Longint & lBufferLength);


/*
  -----------------------------------------------------------------------------
  function:     ls40_EmptyStdinBuffer
  -----------------------------------------------------------------------------
  description:  Empties stdin buffer if interactively read command becomes too
                long to fit into available buffer.

  arguments:    pOptions      [IN]      - structure holding all information

  returnvalue:  None
  -----------------------------------------------------------------------------
*/
static void
ls40_EmptyStdinBuffer(tls40_Options* pOptions);


/*
  -----------------------------------------------------------------------------
  function:     ls40_ReadNextArgument
  -----------------------------------------------------------------------------
  description:

  arguments:

  returnvalue:
  -----------------------------------------------------------------------------
*/
static char*
ls40_ReadNextArgument(char *szArgs[],
                      int  &nCurArg,
                      int   nMaxArgs,
                      int   nMode = 0);


/*
  -----------------------------------------------------------------------------
  function:     ls40_CheckArgumentLength
  -----------------------------------------------------------------------------
  description:  Checks the length of arguments read from command line.

  arguments:    pszArgument     [IN]   - argument whose length to check
                pszOptionsChar  [IN]   - option char for error messages
                AllowedLength   [IN]   - limit
                Length          [OUT]  - length of argument

  returnvalue:  true   -  length is within the limit
                false  -  length exceeds allowed length
  -----------------------------------------------------------------------------
*/
inline bool
ls40_CheckArgumentLength(const char*   pszArgument,
                         char*         pszOptionChar,
                         const size_t  AllowedLength,
                         size_t&       Length);

/******************************************************************************
  -----------------------------------------------------------------------------
  START OF IMPLEMENTATION
  -----------------------------------------------------------------------------
******************************************************************************/


/* 
  -----------------------------------------------------------------------------
  function: ls40_ConvertArguments
  -----------------------------------------------------------------------------
*/
#ifdef _WIN32
static char**
ls40_ConvertArguments(tls40_Options*  options, 
                      int             argc,
                      wchar_t*        argU[])
  {
    char **    argv = new char *[argc];
    int nIndex      = 0;


    for (nIndex = 0; nIndex < argc; nIndex++)
    {
        // UCS2 to UTF8 needs max. 4 bytes per character
        argv[nIndex] = new char[(wcslen(argU[nIndex]) * 4) + 1];
        cn13MemcpyUCS2ToUTF8(argv[nIndex], (char *) argU[nIndex], (int)wcslen(argU[nIndex]) * 2);
    } // end for

    return argv;
  } // end ls40_ConvertArguments
#else
  #define ls40_ConvertArguments(a, b, c)  c
#endif


/*
  -----------------------------------------------------------------------------
  function: main
  -----------------------------------------------------------------------------
*/
#ifdef _WIN32
  int wmain (int givenArgc, wchar_t * argU[])
#else
  int main (int givenArgc, char     * argU[])
#endif
{
    int             MainRetCode = PRG_OK;
    tls40_Options    options;

    char**    givenArgv = ls40_ConvertArguments(&options, givenArgc, argU);

    tsp00_CompName c_component;           // initialize connection to Xserver
    strcpy(c_component, C_COMPONENT);
    sqlinit(c_component, &options.bCancelByte_ls40);

    ls40_Initialize(&options);

    MainRetCode = ls40_ParseOptions(&options, givenArgc, givenArgv);

    if (PRG_OK != MainRetCode)
    {
        exit(PRG_FATAL);
    }

    // Connect to loader server;
    // this function fills options->nMaxDataSize und options-session and allocates
    // space for command buffer;
    // in case of error we need to release this connection
    if (PRG_OK == MainRetCode && false == options.bLoaderExtension_ls40)
    {
        MainRetCode = ls40_ServerConnect(&options);
        if (PRG_OK != MainRetCode)
        {
            // we couldn't even log on to the requested server for what reason o'ever
            // ---> suppress any other error output than the one the function already did
            MainRetCode = PRG_SHOW;     // suppress any other error output than the one the function already did
        }
    }
    if ( (MainRetCode == PRG_OK) && (true == options.showServerVersion) && false == options.bLoaderExtension_ls40)
    {
        MainRetCode = ls40_ShowServerVersion(options);
    }
    //*
    //*     User logon
    //*
    if (PRG_OK == MainRetCode && false == options.bLoaderExtension_ls40)
    {
        MainRetCode = ls40_UserConnect(options);
        if (PRG_OK != MainRetCode)
        {
            // we couldn't even log on the user for what reason o'ever
            // ---> suppress any other error output than the one the function already did
            MainRetCode = PRG_SHOW;
        }
    }
    //*    Dispatcher
    if (PRG_OK == MainRetCode)
    {
        if (true == options.bInteractive_ls40)
        {
            MainRetCode = ls40_ExecuteTaskInteractive(&options);
        }
        else if (true == options.bLoaderExtension_ls40)
        {
            MainRetCode = ls40_ExecuteTaskSteps(options, givenArgv);
        }
        else
        {
            MainRetCode = ls40_ExecuteTaskBatch(options);
        }
    }
    //*     Disconnect Server
    ls40_DisconnectServer(&options);

    sqlfinish(false);           // Undo LZU-initialization (sqlinit() )

    if (0 != options.VirtualFile_ls40.vfFileHndl_ls00)
    {
		ls30VFClose(options.VirtualFile_ls40, options.pszReplyBuffer);
    }

    if ( (options.outfile != stdout) && (NULL != options.outfile) )
    {
        fclose (options.outfile);
    }

    exit(MainRetCode);
    return MainRetCode;
}
//  main()


/*
  -----------------------------------------------------------------------------
  function:     ls40_Usage
  -----------------------------------------------------------------------------
*/
static void ls40_Usage()
{
    char szString[4864];
    szString[0] = '\0';
    tsp100_VersionID1 Version;
    sp100_GetVersionID(VersionIDType1_esp100, s100buildnumberEx, &Version );
    sp77sprintf(szString, 4864, "LOADERCLI version %d.%d.%.2d.%.2d, the MaxDB interactive terminal.\n",
                     STATIC_CAST(tsp00_Int4, Version.MajorVersion_sp100),
                     STATIC_CAST(tsp00_Int4, Version.MinorVersion_sp100),
                     STATIC_CAST(tsp00_Int4, Version.CorrLevel_sp100),
                     STATIC_CAST(tsp00_Int4, Version.BuildNumberPrefix_sp100));
    printf(szString);
    szString[0] = '\0';
    sp77sprintf(szString, 4864,
                        " \nUsage:                                      \n"
                        "   loadercli [<options>]                       \n\n"
                        "Options to specify the operating mode:         \n"
                        "   -p  [<package_name>]                        (commandline or interactive transport mode)    \n"
                        "   -b   <file_name>                            (commandfile or session mode (default:stdin))  \n\n"
                        "Options for data resources:                    \n"
//                      "   -U[SRC|TRG][:<port>] <userkey>              Option [SRC|TRG] is only available in commandline mode \n"
                        "   -n[SRC|TRG]<database_computer>[:<PORT>]     (Option [SRC|TRG] is only available in commandline mode(default:localhost)) \n"
                        "   -d[SRC|TRG]<database_name>                  (Option [SRC|TRG] is only available in commandline mode) \n"
                        "   -u[SRC|TRG]<database_user>[,<password>]     (Option [SRC|TRG] is only available in commandline mode) \n"
                        "   -s[SRC|TRG]<database_schema>                (Option [SRC|TRG] is only available in commandline mode( default:databae_user) \n\n" 
//                      "   -n <database_computer>[:<PORT>]             \n"
//                      "   -d <database_name>                          \n"
//                      "   -u <database_user>[,<password>]             \n"
//                      "   -s <database_schema>                        \n"
                        "Options in commandline transport mode:         \n"
//                      "   -p [<package|package_name>]                 (package and package_name ( default <package_name> : TIMESTAMP) \n"
//                      "                                                use a predefinded <package> [DB|USER|SCHEMA|TABLE] or \n"
//                      "                                                    a <package_name> of an generated package in case of direction [TRANSPORT|EXPORT].\n"
//                      "                                                use a <package_name> of an executed  export  in case of direction [IMPORT].    \n"
                        "   -p [<package_name>]                         (package_name ( default <package_name> : TIMESTAMP) \n"
//                      "   -P  <package|TABLE>                         (package_part (default:identical with package value [DB|USER|SCHEMA|TABLE])\n"
//                      "   -t  <EXPORT |IMPORT|TRANSPORT>              (direction)   \n"
//                      "   -T  <EXECUTE|GENERATE>                      (action       (default:EXECUTE))\n"
//                      "   -s  <CATALOG|DATA|PACKAGE>                  (transport streams )   \n"
//                      "   -m  <FILE|PIPE>                             (transport stream  medium)\n"
//                      "   -f  <CSV|FWV><DDL|XML>                      (stream format)\n"
//                      "       <PAGES|RECORDS>                         \n"
//                      "   -F  <PAGE |ROW>                             (stream format for internal processing)\n"
//                      "   -c  <UCS2 |UTF8|ASCII|DYNAMIC>              (stream char  encoding) \n"
//                      "   -l  <TABLE|COLUMN|COLUMNVALUE>              (stream large objects)\n\n"
                        "Options in commandfile and session mode:       \n "        
                        "  Input and output options:                    \n"
                        "   -b  <file_name>                             (use file <file_name> to  input  loader commands (default:stdin))\n"
                        "   -o  <file_name>                             (use file <file_name> for output loader messages (default:stdout))\n"
                        "   -cs <separator>                             (command separator for command input file)\n"
                        "   -ps <number><string>                        (string that substitutes place holder '&<number>'\n"
                        "                                                in command input file (up to 9 parameters possible)\n"
                        "   -E  <number>                                (defines the number of errors after that the client\n"
                        "                                                stops executing (1 = default and 0 = ignore errors)\n"
                        "  Version options:                             \n" 
                        "   -V                                          (show version of loader server)\n"
                        "   -v                                          (show version of loader client)\n"
                        "   -R  <INSTROOT>                              (set  instroot directory (meaningless if omitting option -r))\n\n");
//                      "   -r  [<loader_computer>]                     (loader computer (default localhost);\n"
//                      "                                                remote communication not supported; \n"
//                      "                                                omit this option to use statically linked loader client/server)\n\n");
//                      "Loader tutorial:                                \n\n"
//                      "Task 1. Design, generate and execute a package in interactive mode:\n"
//                      "  loadercli -p                                                     \n\n"
//                      "Task 2. Generate a user package MONA with following parameters:\n"
//                      "  loadercli -p SCHEMA:MONA -t EXPORT  -uSRC MONA,RED -sSRC HOTEL -dSRC HOTELDB -nSRC LOCALHOST -T GENERATE -P TABLE -m FILE -f COMPRESSED -F PAGE -c DYNAMIC -l COLUMNVALUE \n\n"
//                      "Task 3. Execute the MONA package generated in Task 2:                \n"
//                      "  loadercli -p MONA:MONA              -uSRC MONA,RED -sSRC HOTEL   \n\n"
//                      "Task 4. Execute the MONA package command file generated in Task 2:   \n"
//                      "  loadercli -b MONA.prt               -u    MONA,RED -s    HOTEL -d    HOTELDB  -E 0    \n\n"
//                      "Task 5. Design, generate and execute new packages in commandline mode: \n"
//                      "  loadercli -p SCHEMA:MONA -t EXPORT    -uSRC MONA,RED   -dSRC HOTELDB -nSRC LOCALHOST\n"
//                      "  loadercli -p MONA:NINA   -t IMPORT    -uTRG NINA,BLACK -dTRG HOTELDB -nTRG LOCALHOST\n"
//                      "  loadercli -p SCHEMA:NINA -t TRANSPORT -uSRC MONA,RED   -dSRC HOTELDB -nSRC LOCALHOST\n"
//                      "                                        -uTRG NINA,BLACK -dTRG HOTELDB -nTRG LOCALHOST\n\n");
    printf(szString);

    return;
}
//  ls40_Usage()


/*
  -----------------------------------------------------------------------------
   function:     ls40_Initialize
  -----------------------------------------------------------------------------
*/
static void ls40_Initialize(tls40_Options* pOptions)
{
    memset(pOptions, 0, sizeof(tls40_Options));

    pOptions->NodeLoc                   = rncLocal_els40;
    pOptions->lIsoLevel_ls40            = 3;        // This is the default value the Loader server uses
    pOptions->EncryptionProtocol        = epUnknown_els00;
    pOptions->targetEncryptionProtocol  = epUnknown_els00;

    pOptions->dbnode                    = "";
    pOptions->dbname[0] = 0;
    pOptions->dbuser[0] = 0;
    pOptions->dbuserkey[0] = 0;
    pOptions->dbschema[0]               = 0;


    pOptions->targetdbnode              = "";
    pOptions->targetdbname[0] = 0;
    pOptions->targetdbuser[0] = 0;
    pOptions->targetdbuserkey[0] = 0;
    pOptions->targetdbschema[0] = 0;

    pOptions->executionkind[0] = 0;
    pOptions->transformationtype[0] = 0;
    pOptions->transformationpackage[0] = 0;
    pOptions->transformationpart[0] = 0;
    pOptions->transformationstreams[0] = 0;
    pOptions->streammedium[0] = 0;
    pOptions->streamformat[0] = 0;
    pOptions->streamformat_target[0] = 0;
    pOptions->streamcharencoding[0] = 0;
    pOptions->streamlob[0] = 0;
    pOptions->startposition[0] = 0;


    memset(&pOptions->VirtualFile_ls40, 0, sizeof(tls00_VFile));
    pOptions->VirtualFile_ls40.vfFileName_ls00 = new char[6];
    pOptions->VirtualFile_ls40.vfFileName_ls00[0] = 0;
    _tcscpy(pOptions->VirtualFile_ls40.vfFileName_ls00, "stdin");

    pOptions->outfile                 = stdout;
    strcpy(pOptions->CmdSeparator_ls40.csCmdSeparator_ls40, "//");
    pOptions->CmdSeparator_ls40.csDefaultSeparator = true;

    pOptions->ReplyBufferLen          = RPM_REPLY_BUFFER_LEN_CN14 + RPM_ERR_PROTOCOL_SIZE_CN14;

    pOptions->CmdExecError            = 0;
    pOptions->BatchErrorCnt_ls40      = 1;     // default: stop after 1 error

    pOptions->bNoParam_ls40           = true;  // default: no parameters to consider

    tsp00_Int2 i = 0;

    // Initialize to static server (client and server linked together)
    pOptions->bStaticServer_ls40       = true;
    pOptions->bInteractive_ls40        = true;
}
// ls40_Initialize()


/*
  -----------------------------------------------------------------------------
   function:     ls40_InitBuffers
  -----------------------------------------------------------------------------
*/
static void ls40_InitBuffers(tls40_Options & opt)
{

    if (NULL == opt.pszCommBuffer_ls40)
    {
        // First thing to do - allocate space

        //*
        //*     Allocate space for command buffer
        //*
        tsp00_Int4 blen = MAX_COMMAND_LENGTH_LS40;
        if (true == opt.bStaticServer_ls40)
        {
            blen += RPM_PROTOCOL_SIZE_CN14 + sizeof(char);
        }

        opt.pszCommBuffer_ls40 = new _TCHAR[blen];
        memset(opt.pszCommBuffer_ls40, 0, blen);

        //*
        //*     In case of static server we need to leave some space for
        //*     client/server protocol
        //*
        if (true == opt.bStaticServer_ls40)
        {
            opt.pszCmdBuffer = opt.pszCommBuffer_ls40 + RPM_PROTOCOL_SIZE_CN14;
        }
        else
        {
            opt.pszCmdBuffer = opt.pszCommBuffer_ls40;
        }

        //*
        //*     Allocate space for (server) reply buffer
        //*
        opt.pszReplyBuffer = new _TCHAR[opt.ReplyBufferLen];
    }

    //*
    //*     In any case - initialize the buffers
    //*
    if (true == opt.bStaticServer_ls40)
    {
        // Initialize communication protocol
        memset(opt.pszCommBuffer_ls40, 0, RPM_PROTOCOL_SIZE_CN14);
    }
    opt.pszCmdBuffer[0]   = 0;
    opt.CmdBufferLen      = 0;
    opt.pszReplyBuffer[0] = 0;
}
// ls40_InitBuffers()

/*
  -----------------------------------------------------------------------------
  function: ls40_ShowPrgVersion
  -----------------------------------------------------------------------------
*/
static void ls40_ShowPrgVersion(FILE *OutFile)
{
    tsp100_VersionID1 Version;

    sp100_GetVersionID(VersionIDType1_esp100, s100buildnumberEx, &Version );

    fprintf(OutFile, "Version: %d.%d.%.2d.%.2d\n",
                     STATIC_CAST(tsp00_Int4, Version.MajorVersion_sp100),
                     STATIC_CAST(tsp00_Int4, Version.MinorVersion_sp100),
                     STATIC_CAST(tsp00_Int4, Version.CorrLevel_sp100),
                     STATIC_CAST(tsp00_Int4, Version.BuildNumberPrefix_sp100));
}
//  ls40_ShowPrgVersion()


/*
  -----------------------------------------------------------------------------
  function: ls40_ShowServerVersion
  -----------------------------------------------------------------------------
*/
static int ls40_ShowServerVersion(tls40_Options & options)
{
    short rc        = PRG_OK;

    ls40_InitBuffers(options);

    options.CmdBufferLen = 7;
    SAPDB_memcpy(options.pszCmdBuffer, "version", 7);

    rc = ls40_ServerCommand(&options);
    if (PRG_OK == rc)
    {
        fprintf(options.outfile, "Server version information:\n%s\n", options.pszReplyBuffer);
    }
    else if (PRG_CMD_ERROR == rc)
    {
        if (false == options.bSuppressOutput_ls40)
        {
            fprintf(options.outfile, "Error getting server version information: %s\n", options.pszReplyBuffer);
        }
    }
    // else means PRG_SERVER_ERROR error msg is already written by ls40_ServerCommand

    return rc;
}


/*
  -----------------------------------------------------------------------------
   function:     ls40_ReadNextArgument
  -----------------------------------------------------------------------------
*/
static char*
ls40_ReadNextArgument(char *szArgs[], int  &nCurArg, int  nMaxArgs, int  nMode)
{
    int    nCurrent  = nCurArg + 1;
    char * pArgument = NULL;
    if (nCurrent < nMaxArgs)
    {
        if ( (1 == nMode) && ('-' == szArgs[nCurrent][0]) )
        {
            pArgument = &(szArgs[nCurrent][1]);
            nCurArg   = nCurrent;
        }
        else if ( (0 == nMode) && ('-' != szArgs[nCurrent][0]) )
        {
            pArgument = szArgs[nCurrent];
            nCurArg   = nCurrent;
        }
    }
    return pArgument;
}
// ls40_ReadNextArgument()


/*
  -----------------------------------------------------------------------------
   function:     ls40_CheckArgumentLength
  -----------------------------------------------------------------------------
*/
bool
ls40_CheckArgumentLength(const char*   pszArgument,
                         char*         pszOptionChar,
                         const size_t  AllowedLength,
                         size_t&       Length)
{
    const size_t _size = AllowedLength - sizeof(char);
    Length = _tcslen(pszArgument);
    if (Length > _size)
    {
        printf("Value for option -%s too long (maximum: %d characters)\n\n", pszOptionChar, _size);
        return false;
    }
    return true;
}
// ls40_CheckArgumentLength()


/*
  -----------------------------------------------------------------------------
   function:     ls40_MakeArgumentUpperCase
  -----------------------------------------------------------------------------
*/
inline void
ls40_MakeArgumentUpperCase(char* pszArgument)
{
    size_t _len = _tcslen(pszArgument);
    for (char *p = pszArgument; p < pszArgument + _len; *p = _totupper(*p), ++p)
        ;
}
// ls40_MakeArgumentUpperCase()


/*
  -----------------------------------------------------------------------------
  function: ls40_ParseOptions
  -----------------------------------------------------------------------------
*/
static int ls40_ParseOptions(tls40_Options *options, int argc, char *argv[])
{
    int rc = PRG_OK;

    LoaderMessages*  pMsgObj = LoaderMessages::Instance();

    size_t _len = 0;            // reused variable for defining the length of read parameters

    bool  inOptions           = true;
    int   argI                = 0;
    char *pOption             = NULL;
    bool  bReadUser           = false;
    bool  bReadTargetUser     = false;
    bool  bSeparatorSpecified = false;
    char  szErr[MAX_REPLY_LENGTH_LS00];
    char  OptionValue[2048];
    OptionValue[0] = 0;

    char* parameter = NULL;

    // get name of application for prompting in interactive mode
    options->application = argv[0];

    if (1 != argc)
    {
        // iterate parameters
        pOption = ls40_ReadNextArgument(argv, argI, argc, 1);

        while ( (true == inOptions) && ( NULL != pOption ) )
        {
            if (0 == strcmp(pOption, "T"))
            {
                //*
                //*     Loader transportmode: execution 
                //*
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter)
                {
                    options->bInteractive_ls40     = false;
                    options->bLoaderExtension_ls40 = true;
                }
                else
                {
                    if (true == (inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->executionkind), _len)))
                    {
                        if ( (0 == _tcsicmp(parameter, "EXECUTE")) || (0 == _tcsicmp(parameter, "GENERATE")) || 0== 0 )
                        {
                            SAPDB_memcpy(options->executionkind, parameter, _len);

                            options->bInteractive_ls40     = false;
                            options->bLoaderExtension_ls40 = true;
                            strcat(OptionValue, " -T ");
                            strcat(OptionValue, options->executionkind);
                        }
                        else
                        {
                            printf("Possible values for Package Execution kind: GENERATE and EXECUTE \n");
                            inOptions = false;
                        }
                    }
                }
            }
            // loader transport mode
            else if (0 == strcmp(pOption, "t"))
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter)
                {
                    options->bInteractive_ls40     = false;
                    options->bLoaderExtension_ls40 = true;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->transformationtype), _len);
                    if (true == inOptions)
                    {
                        if ( 0 == _tcsicmp(parameter, "TRANSPORT") ||
                             0 == _tcsicmp(parameter, "EXPORT")    ||
                             0 == _tcsicmp(parameter, "IMPORT") )
                        {
                            SAPDB_memcpy(options->transformationtype, parameter, _len);
                            options->bInteractive_ls40     = false;
                            options->bLoaderExtension_ls40 = true;
                            strcat(OptionValue, " -t ");
                            strcat(OptionValue, options->transformationtype);
                        }
                        else
                        {
                            printf("Possible values for Transformation type: <none>, TRANSPORT, EXPORT, IMPORT\n");
                            inOptions = false;
                        }
                    }
                }
            }
            // transformationpackage
            else if (0 == strcmp(pOption, "p"))
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter )
                {
                     options->bInteractive_ls40     = false;
                     options->bLoaderExtension_ls40 = true;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->transformationpackage), _len);
                    if (true == inOptions)
                    {
                        SAPDB_memcpy(options->transformationpackage, parameter, _len);
                        options->bInteractive_ls40     = false;
                        options->bLoaderExtension_ls40 = true;
                        strcat( OptionValue, " -p ");
                        strcat( OptionValue, options->transformationpackage );
                    }
                }
            }
            // transformationpart
            else if (0 == strcmp(pOption, "P"))
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter)
                {
                    printf("Transformation Packagepart missing (P option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->transformationpart), _len);
                    if (true == inOptions)
                    {
                        if ( 0 == _tcsicmp(parameter, "DB")     ||
                             0 == _tcsicmp(parameter, "USER")   ||
                             0 == _tcsicmp(parameter, "SCHEMA") ||
                             0 == _tcsicmp(parameter, "TABLE" ) )
                        {
                            SAPDB_memcpy(options->transformationpart, parameter, _len);
                            options->bInteractive_ls40     = false;
                            options->bLoaderExtension_ls40 = true;
                            strcat( OptionValue, " -P ");
                            strcat( OptionValue, options->transformationpart );
                        }
                        else
                        {
                            printf("Possible values for Transformation Package part: DB, USER, SCHEMA, TABLE\n");
                            inOptions = false;
                        }
                    }
                }
            }
            // transformationstreams
            else if (0 == strcmp(pOption, "s") && options->bLoaderExtension_ls40 == true)
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter )
                {
                     printf("Transformation Streams missing (s option)\n");
                     inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->transformationstreams), _len);
                    if (true == inOptions)
                    {
                        SAPDB_memcpy(options->transformationstreams, parameter, _len);
                        options->bInteractive_ls40     = false;
                        options->bLoaderExtension_ls40 = true;
                        strcat( OptionValue, " -s ");
                        strcat( OptionValue, options->transformationstreams );
                    }
                }
            }
            // streaminitposition
            else if (0 == strcmp(pOption, "i"))
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter )
                {
                     printf("Transformation Streamposition missing (i option)\n");
                     inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->startposition), _len);
                    if (true == inOptions)
                    {
                        SAPDB_memcpy(options->startposition, parameter, _len);
                        options->bInteractive_ls40     = false;
                        options->bLoaderExtension_ls40 = true;
                        strcat( OptionValue, " -i ");
                        strcat( OptionValue, options->startposition );
                    }
                }
            }
            // streammedium
            else if (0 == strcmp(pOption, "m"))
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter )
                {
                    printf("Transformation Medium missing (m option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->streammedium), _len);
                    if (true == inOptions)
                    {
                        if ( 0 == _tcsicmp(parameter, "FILE") ||
                             0 == _tcsicmp(parameter, "PIPE") ||
                             0 == _tcsicmp(parameter, "TAPE") )
                        {
                            SAPDB_memcpy(options->streammedium, parameter, _len);
                            options->bInteractive_ls40     = false;
                            options->bLoaderExtension_ls40 = true;
                            strcat(OptionValue, " -m ");
                            strcat(OptionValue, options->streammedium);
                        }
                        else
                        {
                            printf("Possible values for Transformation Medium: FILE, PIPE, TAPE\n");
                            inOptions = false;
                        }
                    }
                }
            }
            // streamformat
            else if (0 == strcmp(pOption, "f"))
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter )
                {
                    printf("Transformation Format missing (f option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->streamformat), _len);
                    if (true == inOptions)
                    {
                        if ( 0 == _tcsicmp(parameter, "COMPRESSED") ||
                             0 == _tcsicmp(parameter, "FORMATTED")  ||
                             0 == _tcsicmp(parameter, "RECORDS")    ||
                             0 == _tcsicmp(parameter, "PAGES")      ||
                             0 == _tcsicmp(parameter, "CSV")        ||
                             0 == _tcsicmp(parameter, "FWV")        ||
                             0 == _tcsicmp(parameter, "DDL")        ||
                             0 == _tcsicmp(parameter, "XML")           )
                        {
                            SAPDB_memcpy(options->streamformat, parameter, _len);
                            options->bInteractive_ls40     = false;
                            options->bLoaderExtension_ls40 = true;
                            strcat(OptionValue, " -f ");
                            strcat(OptionValue, options->streamformat);
                        }
                        else
                        {
                            printf("Possible values for Transformation Format: RECORDS, PAGES, FORMATTED, COMPRESSED, DDL \n");
                            inOptions = false;
                        } 
                    }
                }
            }
            // streamformat_target
            else if (0 == strcmp(pOption, "F"))
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter )
                {
                    printf("Transformation Target Format missing (F option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->streamformat_target), _len);
                    if (true == inOptions)
                    {
                        if ( 0 == _tcsicmp(parameter, "ROW"  ) ||
                             0 == _tcsicmp(parameter, "PAGE" ) ||
                             0 == _tcsicmp(parameter, "DDL"  ) ||
                             0 == _tcsicmp(parameter, "XML"  ) ||
                             0 == _tcsicmp(parameter, "DTL"  )   )
                        {
                            SAPDB_memcpy(options->streamformat_target, parameter, _len);
                            options->bInteractive_ls40     = false;
                            options->bLoaderExtension_ls40 = true;
                            strcat(OptionValue, " -F ");
                            strcat(OptionValue, options->streamformat_target);
                        }
                        else
                        {
                            printf("Possible values for Transformation Target Format: ROW, PAGES, DDL \n");
                            inOptions = false;
                        }
                    }
                }
            }
            // streamcharencoding
            else if (0 == strcmp(pOption, "c"))
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter )
                {
                    printf("Transformation Char Encoding missing ( c option )\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->streamcharencoding), _len);
                    if (true == inOptions)
                    {
                        if ( 0 == _tcsicmp(parameter, "DYNAMIC") ||
                             0 == _tcsicmp(parameter, "UCS2")    ||
                             0 == _tcsicmp(parameter, "UTF8")    ||
                             0 == _tcsicmp(parameter, "ASCII") )
                        {
                            SAPDB_memcpy(options->streamcharencoding, parameter, _len);
                            options->bInteractive_ls40     = false;
                            options->bLoaderExtension_ls40 = true;
                            strcat(OptionValue, " -c ");
                            strcat(OptionValue, options->streamcharencoding );
                        }
                        else
                        {
                            printf("Possible values for Transformation Char Encoding: DYNAMIC, UCS2, UTF8, ASCII\n");
                            inOptions = false;
                        }
                    }
                }
            }
            // streamlob
            else if (0 == strcmp(pOption, "l"))
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter )
                {
                    printf("Transformation Lob handling missing (l option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->streamlob), _len);
                    if (true == inOptions)
                    {
                        if ( 0 == _tcsicmp(parameter, "TABLE")      ||
                             0 == _tcsicmp(parameter, "COLUMN")     ||
                             0 == _tcsicmp(parameter, "COLUMNVALUE") )
                        {
                            SAPDB_memcpy(options->streamlob, parameter, _len);
                            options->bInteractive_ls40     = false;
                            options->bLoaderExtension_ls40 = true;
                            strcat(OptionValue, " -l ");
                            strcat(OptionValue, options->streamlob);
                        }
                        else
                        {
                            printf("Possible values for Transformation Lob Handling: TABLE, COLUMN, COLUMNVALUE\n");
                            inOptions = false;
                        }
                    }
                }
            }
//xuser
            // sourcedb node
            else if (0 ==   strcmp(pOption, "n")    ||
                     0 == _tcsicmp(pOption, "nSRC" ) )
            {
                options->dbnode = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == options->dbnode)
                {
                    printf("Database node name missing (n[SRC] option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(options->dbnode, pOption, 128, _len);
                    if (true == inOptions)
                    {
                        if ( 0 == _tcsicmp(pOption, "nSRC" )) 
                        {
                             options->bInteractive_ls40     = false;
                             options->bLoaderExtension_ls40 = true;
                        }
                        strcat(OptionValue, " -nSRC ");
                        strcat(OptionValue, options->dbnode);
                    }
                }
            }
            // sourcedb name
            else if (0 ==   strcmp(pOption, "d")  ||
                     0 == _tcsicmp(pOption, "dSRC" ) )
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter)
                {
                    printf("Database name missing (d[SRC] option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->dbname), _len);
                    if (true == inOptions)
                    {
                        if ( 0 == _tcsicmp(pOption, "dSRC" )) 
                        {
                             options->bInteractive_ls40     = false;
                             options->bLoaderExtension_ls40 = true;
                        }
                        strcpy(options->dbname, parameter);
                        ls40_MakeArgumentUpperCase(options->dbname);
                        strcat( OptionValue, " -dSRC " );
                        strcat( OptionValue, options->dbname );
                    }
                }
            }
            // sourcedb user/password
            else if (0 ==   strcmp(pOption, "u") ||
                     0 ==  _tcscmp(pOption, "uSRC" ) )
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter)
                {
                    bReadUser = true;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->dbuser), _len);
                    if (true == inOptions)
                    {
                        options->dbuser[0] = 0;
                        strcpy(options->dbuser, parameter);
                        if ( 0 == _tcsicmp(pOption, "uSRC" )) 
                        {
                             options->bInteractive_ls40     = false;
                             options->bLoaderExtension_ls40 = true;
                        }
                        strcat(OptionValue , " -uSRC " );
                        strcat(OptionValue , options->dbuser);
                    }
                }
            }
            // sourcedb schema or transformation streams
            else if ( 0 ==   strcmp(pOption, "s") && options->bLoaderExtension_ls40 == false )
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter)
                {
                    printf("Database schema name missing (s option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->dbschema), _len);
                    if (true == inOptions)
                    {
                        options->dbschema[0] = 0;
                        strcpy(options->dbschema, parameter);
                        
                        strcat(OptionValue , " -s " );
                        strcat(OptionValue , options->dbschema);
                    }
                }
            }
            else if ( 0 ==  _tcscmp(pOption, "sSRC" ) )
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter)
                {
                    printf("Database schema name missing (s[SRC] option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->dbschema), _len);
                    if (true == inOptions)
                    {
                        options->dbschema[0] = 0;
                        strcpy(options->dbschema, parameter);
                        if ( 0 == _tcsicmp(pOption, "sSRC" )) 
                        {
                             options->bInteractive_ls40     = false;
                             options->bLoaderExtension_ls40 = true;
                        }
                        strcat(OptionValue , " -sSRC " );
                        strcat(OptionValue , options->dbschema);
                    }
                }
            }
            // targetdb node
            else if ( 0 == _tcsicmp(pOption, "nTRG" ) )
            {
                options->targetdbnode = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == options->targetdbnode)
                {
                    printf("Database node name missing (nTRG option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(options->targetdbnode, pOption, 128, _len);
                    if (true == inOptions)
                    {
                        options->bInteractive_ls40     = false;
                        options->bLoaderExtension_ls40 = true;
                        strcat(OptionValue, " -nTRG " );
                        strcat(OptionValue, options->targetdbnode );
                    }
                }
            }
            else if ( (0 == _tcscmp(pOption, "e" )) || (0 ==  _tcscmp(pOption, "eSRC" )) )
            {
                //*
                //*     Encryption protocol
                //*
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter)
                {
                    printf("Encryption protocol missing (e[SRC] option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, 64, _len);
                    if (true == inOptions)
                    {
                        if (0 == _tcsicmp(parameter, pszEncryptionProtName_ls00[epSSL_els00]))
                        {
                            options->EncryptionProtocol = epSSL_els00;
                        }
                        else
                        {
                            printf("Unsupported encryption protocol specified; the only available protocol is SSL\n");
                            inOptions = false;
                        }
                    }
                }
            }
            // targetdb name
            else if ( 0 == _tcsicmp(pOption, "dTRG" ))
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter)
                {
                    printf("Target database name missing (dTRG option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->targetdbname), _len);
                    if (true == inOptions)
                    {
                        strcpy(options->targetdbname, parameter);

                        ls40_MakeArgumentUpperCase(options->targetdbname);
                        options->bInteractive_ls40     = false;
                        options->bLoaderExtension_ls40 = true;
                        strcat(OptionValue, " -dTRG ");
                        strcat(OptionValue, options->targetdbname);
                    }
                }
            }
            // targetdb user/password
            else if ( 0 ==  _tcscmp(pOption, "uTRG" ))
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter)
                {
                    bReadTargetUser = true;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->targetdbuser), _len);
                    if (true == inOptions)
                    {
                        options->targetdbuser[0] = 0;
                        strcpy(options->targetdbuser, parameter);
                        options->bInteractive_ls40     = false;
                        options->bLoaderExtension_ls40 = true;
                        strcat(OptionValue, " -uTRG ");
                        strcat(OptionValue, options->targetdbuser);
                    }
                }
            }
            // targetdb schema
            else if ( 0 ==  _tcscmp(pOption, "sTRG" ) )
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter)
                {
                    printf("Database schema name missing (sTRG option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(options->targetdbschema), _len);
                    if (true == inOptions)
                    {
                        options->targetdbschema[0] = 0;
                        strcpy(options->targetdbschema, parameter);
                        
                        strcat(OptionValue , " -sTRG " );
                        strcat(OptionValue , options->targetdbschema);
                    }
                }
            }
            else if (0 == _tcscmp(pOption, "eTRG" ))
            {
                //*
                //*     target encryption protocol
                //*
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter)
                {
                    printf("Encryption protocol missing (eTRG option)\n");
                    inOptions = false;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, 64, _len);
                    if (true == inOptions)
                    {
                        if (0 == _tcsicmp(parameter, pszEncryptionProtName_ls00[epSSL_els00]))
                        {
                            options->targetEncryptionProtocol = epSSL_els00;
                        }
                        else
                        {
                            printf("Unsupported encryption protocol specified; the only available protocol is SSL\n");
                            inOptions = false;
                        }
                    }
                }
            }
            else if (0 == strcmp(pOption, "r"))
            {
                //*
                //* Loader server node
                //*
                options->repm_node = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == options->repm_node)
                {
                    // lonely -r points to the db server as Loader server node
                    options->NodeLoc = rncDBNode_els40;
                }
                else
                {
                    inOptions = ls40_CheckArgumentLength(options->repm_node, pOption, 128, _len);
                    if (true == inOptions)
                    {
                        options->NodeLoc = rncDifferent_els40;
                    }
                }

                // User wishes to deploy a client server model
                options->bStaticServer_ls40 = false;
            }
            // db root
            else if (0 == strcmp(pOption, "R"))
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL != parameter)
                {
                    // Is the given path name longer than var generate error
                    inOptions = ls40_CheckArgumentLength(parameter, pOption, sizeof(tsp00_Pathc), _len);
                    if (true == inOptions)
                    {
                        SAPDB_memcpy(options->szDBRoot_ls40, parameter, _len);
                        options->szDBRoot_ls40[_len] = '\0';
                    }
                }
            }
            // loader client version
            else if (0 == strcmp(pOption, "v"))
            {
                ls40_ShowPrgVersion(options->outfile);
                inOptions = false;
            }
            // loader server version
            else if (0 == strcmp(pOption, "V"))
            {
                options->showServerVersion = true;
            }
            // loader command separator
            else if (0 == strcmp(pOption, "cs"))
            {
                parameter = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == parameter)
                {
                    printf("Command separator missing (cs option)\n");
                    inOptions = false;
                }
                else
                {
                    bSeparatorSpecified = true;

                    inOptions = ls40_CheckArgumentLength(parameter, pOption, MAX_CMD_SEPARATOR_LENGTH_LS40 + sizeof(char), _len);
                    if (true == inOptions)
                    {
                        SAPDB_memcpy(options->CmdSeparator_ls40.csCmdSeparator_ls40, parameter, _len);
                        options->CmdSeparator_ls40.csCmdSeparator_ls40[_len] = 0;
                        if (MAX_CMD_SEPARATOR_LENGTH_LS40 != _len)
                        {
                            options->CmdSeparator_ls40.csDefaultSeparator = false;
                        }
                        else
                        {
                            if ( ('/' != options->CmdSeparator_ls40.csCmdSeparator_ls40[0]) ||
                                 ('/' != options->CmdSeparator_ls40.csCmdSeparator_ls40[1])  )
                            {
                                options->CmdSeparator_ls40.csDefaultSeparator = false;
                            }
                        }
                    }
                }
            }
            // loader batch mode command file
            else if (0 == strcmp(pOption, "b"))
            {
                inOptions = false;

                // Open file binary for later operations like fseek
                char* pszFileName = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == pszFileName)
                {
                    printf("Input file name missing (b option)\n");
                }
                else
                {
                    if (0 == _tcsicmp(pszFileName, "stdin"))
                    {
                        printf("stdin cannot be used as batch file; omit b option to run Loader in interactive mode\n");
                    }
                    else
                    {
                        delete [] options->VirtualFile_ls40.vfFileName_ls00;    // per default filled with "stdin"

                        // Redirects ptr vfFileName_ls00 to given file name
                        ls30VFInitialize(options->VirtualFile_ls40,
                                         pszFileName,
                                         sp5vf_text,
                                         sp5vf_read,
                                         options->VirtualFile_ls40.vfFileEncoding_ls00);

                        szErr[0] = 0;
                        rc = ls30VFOpen(options->VirtualFile_ls40, szErr);
                        if (rc >= 0)
                        {
                            options->bInteractive_ls40 = false;
                            inOptions = true;
                        }
                        else
                        {
                            printf("Cannot open input file %s\n", options->VirtualFile_ls40.vfFileName_ls00);
                        }
                    }
                }
            }
            // loader max error count
            else if (0 == strcmp(pOption, "E"))
            {
                inOptions = false;          // initialize to error

                char   *pszNumber = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == pszNumber)
                {
                    printf("Number specification missing (E option)\n");
                }
                else
                {
                    bool    bInvalid   = false;
                    size_t  lNumberLen = strlen(pszNumber);

                    if (lNumberLen < 10)
                    {
                        if ( (1 == lNumberLen) && ('0' == pszNumber[0]) )
                        {
                            options->BatchErrorCnt_ls40 = 0;
                            inOptions = true;
                        }
                        else
                        {
                            options->BatchErrorCnt_ls40 = atoi(pszNumber);
                            if (0 == options->BatchErrorCnt_ls40)
                            {
                                bInvalid = true;
                            }
                            else
                            {
                                inOptions = true;
                            }
                        }
                    }
                    else if (lNumberLen == 10)
                    {
                        if (0 < memcmp("2147483647", pszNumber, 10))
                        {
                            options->BatchErrorCnt_ls40 = atoi(pszNumber);
                            if (0 == options->BatchErrorCnt_ls40)
                            {
                                bInvalid = true;
                            }
                            else
                            {
                                inOptions = true;
                            }
                        }
                    }
                    // Is the given value longer than 10 bytes or GT or EQ 2147483647? --> error
                    if (false == inOptions)
                    {
                        if (true == bInvalid)
                        {
                            printf("The value '%s' for the number of errors is not a valid number.\n\n",
                                   pszNumber);
                        }
                        else
                        {
                            printf("Error numbers may not exceed 2147483646\n\n");
                        }
                        ls40_Usage();
                    }
                }   // end if (NULL == pszNumber)
            }
            // output file
            else if ('o' == pOption[0])
            {
                inOptions = false;

                options->pszOutfileName = ls40_ReadNextArgument(argv, argI, argc);
                if (NULL == options->pszOutfileName)
                {
                    printf("Output file name missing (o option)\n");
                }
                else
                {
                    if (0 == _tcsicmp(options->pszOutfileName, "stdout"))
                    {
                        // Nothing to do - user specified default
                        inOptions = true;
                    }
                    else
                    {
                        char mode[2] = {'w'};
                        if (strlen(pOption) > 1)
                        {
                            if ('a' == pOption[1])
                            {
                                mode[0] = 'a';
                            }
                        }

                        options->outfile = fopen(options->pszOutfileName, mode);
                        if (NULL != options->outfile)
                        {
                            inOptions = true;
                        }
                        else
                        {
                            printf("Cannot open output file %s\n", options->pszOutfileName);
                            inOptions = false;
                        }
                    }
                }
            }
            // parameter substitution
            else if ( 0 == strncmp(pOption, "ps",2))
            {
                tsp00_Int4 Index = atoi(&pOption[2]);
                if ( (0 == Index) || (Index > MAX_PARAM_LS40) )
                {
                    printf("Wrong parameter specification\n");
                    inOptions = false;
                }
                else
                {
                    options->bNoParam_ls40 = false;

                    Index -= 1;

                    parameter = ls40_ReadNextArgument(argv, argI, argc);
                    if (NULL == parameter)
                    {
                        inOptions = false;
                    }
                    else
                    {
                        _len = _tcslen(parameter);
                        if (NULL != options->pszParamList_ls40[Index])
                        {
                            // specifying parameters twice not allowed
                            printf("Substitution parameter p%c specified twice\n", pOption[1]);
                            inOptions = false;
                        }
                        else
                        {
                            options->pszParamList_ls40[Index] = new _TCHAR[_len + 1];
                            memcpy(options->pszParamList_ls40[Index], parameter, _len);
                            options->pszParamList_ls40[Index][_len] = '\0';
                        }
                    }
                }
            }
            // suppress output - hidden option
            else if (0 == strcmp(pOption, "x"))
            {
                options->bSuppressOutput_ls40 = true;
            }
            else
            {
                ls40_Usage();
                inOptions = false;
            }

            if (true == inOptions)
            {
                pOption = ls40_ReadNextArgument(argv, argI, argc, 1);
            }
        } // while ( (true == inOptions) && (argI < argc) )


        // Check if special handling of reading users name and password is requested
        if (true == inOptions)
        {
            if ( (true == bReadUser) && (0 == options->dbuser[0])  )
            {
                rc = ls40_ReadUser(options);
                bReadUser = false;
                if (PRG_FATAL == rc)
                {
                    inOptions = false;
                }
                else
                {
                    // Returned without any user information => error. May also be caused by CTRL-C.
                    if (0 == options->dbuser[0] )
                    {
                        printf("User data missing\n");
                        inOptions = false;
                    }
                }
            }
            if ( (true == bReadTargetUser) && (0 == options->targetdbuser[0]) )
            {
                rc = ls40_ReadUser(options);
                bReadTargetUser = false;
                if (PRG_FATAL == rc)
                {
                    inOptions = false;
                }
                else
                {
                    // Returned without any user information => error. May also be caused by CTRL-C.
                    if (0 == options->targetdbuser[0] )
                    {
                        printf("User data missing\n");
                        inOptions = false;
                    }
                }
            }
            //if( true == inOptions && 0 != options->dbuser[0])
            if (true == inOptions)
            {
                if  (0 != options->dbuser[0])
                    ls40_MakeArgumentUpperCase(options->dbuser);

                if (0 != options->targetdbuser[0])
                    ls40_MakeArgumentUpperCase(options->targetdbuser);
            }
        }// end if (true == inOptions)
    } // if ( (1 == argc) || ...

    if ( (true == inOptions) && (argc > (argI + 1)) )
    {
        // There are still plenty of arguments to process but the user made obviously
        // a mistake with one of them - print usage information
        ls40_Usage();
        inOptions = false;
    }

    if (false == inOptions)
    {
        rc = PRG_FATAL;
    }
    else
    {
        // Some additional checks special for interactive mode
        if ( (false == options->bLoaderExtension_ls40) && (true == options->bInteractive_ls40))
        {
            if (stdout != options->outfile)
            {
                // interactive mode and another outfile than stdout does not match
                printf("Interactive mode impossible with an outfile other than stdout\n");
                rc = PRG_FATAL;
            }

            if ( (PRG_OK == rc) && (false == bSeparatorSpecified) )
            {
                // Well, default separator for interactive mode should be used;
                // we mark this case by setting the 2nd character in separator string to binary zero
                options->CmdSeparator_ls40.csCmdSeparator_ls40[1] = 0;
            }
        }
    }

    return rc;
}
//  ls40_ParseOptions()


/*
  -----------------------------------------------------------------------------
  function: ls40_ReadUser
  -----------------------------------------------------------------------------
*/
static int
ls40_ReadUser(tls40_Options *pOptions)
{
    int rc = PRG_OK;

    char         szUser[128];
    szUser[0] = 0;
    tsp00_Namec  szPwd;
    bool         bGiven = false;

    tsp05_RteFileError rteError;

    tsp00_Int4    lCnt = 0;
    tsp00_Longint slen = 0;

    while ( (false == bGiven) && (PRG_OK == rc) && (lCnt < 3) )     // limit: 3 trials
    {
        lCnt++;
        if (0 == slen)
        {
            //*
            //*     Read the user name from stdin
            //*
            cn13fputs(stdout, "Enter user for authorization: ", true);
            cn13fgets(szUser, 99, stdin, true);

            slen = _tcslen(szUser);

            ls05StrTrimRight(szUser, slen, '\n');
            ls05StrTrimRight(szUser, slen, '\r');
            szUser[slen] = 0;
        }   // end if (0 == slen)

        if (slen > 0)
        {
            //*
            //*     Read the password from stdin
            //*
            if (!sqlgetpass("Enter password: ", szPwd, rteError))
            {
                if (vf_ok != rteError.sp5fe_result)
                {
                    printf("Error reading password from console: %s", rteError.sp5fe_text.asCharp());
                }
                rc = PRG_FATAL;
            }
            else
            {
                if (szPwd.length() > 0)
                {
                    bGiven = true;
                    pOptions->dbuser[0] = 0;
                    sp77sprintf(pOptions->dbuser, 128, "%s,%s", szUser, szPwd.asCharp());
                }
            }
        }   // end if (slen > 0)
    }   // end while ( false == bNameGiven)

    return rc;
}
// ls40_ReadUser()

/*
  -----------------------------------------------------------------------------
  function: ls40_ReadTargetUser
  -----------------------------------------------------------------------------
*/
static int
ls40_ReadTargetUser(tls40_Options *pOptions)
{
    int rc = PRG_OK;

    char         szUser[128];
    szUser[0] = 0;
    tsp00_Namec  szPwd;
    bool         bGiven = false;

    tsp05_RteFileError rteError;

    tsp00_Int4    lCnt = 0;
    tsp00_Longint slen = 0;

    while ( (false == bGiven) && (PRG_OK == rc) && (lCnt < 3) )     // limit: 3 trials
    {
        lCnt++;
        if (0 == slen)
        {
            //*
            //*     Read the user name from stdin
            //*
            cn13fputs(stdout, "Enter user for authorization: ", true);
            cn13fgets(szUser, 99, stdin, true);

            slen = _tcslen(szUser);

            ls05StrTrimRight(szUser, slen, '\n');
            ls05StrTrimRight(szUser, slen, '\r');
            szUser[slen] = 0;
        }   // end if (0 == slen)

        if (slen > 0)
        {
            //*
            //*     Read the password from stdin
            //*
            if (!sqlgetpass("Enter password: ", szPwd, rteError))
            {
                if (vf_ok != rteError.sp5fe_result)
                {
                    printf("Error reading password from console: %s", rteError.sp5fe_text.asCharp());
                }
                rc = PRG_FATAL;
            }
            else
            {
                if (szPwd.length() > 0)
                {
                    bGiven = true;
                    pOptions->targetdbuser[0] = 0;
                    sp77sprintf(pOptions->targetdbuser, 128, "%s,%s", szUser, szPwd.asCharp());
                }
            }
        }   // end if (slen > 0)
    }   // end while ( false == bNameGiven)

    return rc;
}
// ls40_ReadTargetUser()


/*
  -----------------------------------------------------------------------------
  function: ls40_ServerConnect
  -----------------------------------------------------------------------------
*/
static int ls40_ServerConnect(tls40_Options *pOptions)
{
    tsp00_ErrTextc   errtext;
    tsp00_Int4       rc = PRG_OK;
    tsp00_NodeIdc    szNode;
    tsp00_DbNamec    szDbName;
    tsp00_VFilenamec szDbRoot;

    if (true == pOptions->bStaticServer_ls40)
    {
        // Use statically linked client/server
        sqlxinit("","");
        ctrlservopen(
                &(pOptions->session),
                pOptions->dbname,
                pOptions->szDBRoot_ls40,
                &pOptions->bCancelByte_ls40,
                true,
                false,
                -1,
                true,
                RTESys_Hostname(),
                RTESys_ProcessID());

        ls40_InitBuffers(*pOptions);

        // Before doing anything else give some state information; this includes the
        // info on the Loader protocol location
        strcpy(pOptions->pszCmdBuffer, "SET");
        pOptions->CmdBufferLen = 3 * sizeof(char);

        rc = ls40_ServerCommand(pOptions);
        if (PRG_OK == rc)
        {
            if ('\0' != pOptions->pszReplyBuffer[0])
            {
                cn13fputs(pOptions->outfile, pOptions->pszReplyBuffer, true);
                cn13fputs(pOptions->outfile, "\n", false);
            }
        }
    }
    else
    {
        switch (pOptions->NodeLoc)       // repm_node-option empty ? --> log on at local host
        {
            case rncLocal_els40:
            {
                pOptions->repm_node = "";                        // use local machine
                break;
            }
            case rncDBNode_els40:
            {
                if ('\0' != pOptions->dbnode[0])
                {
                    pOptions->repm_node = pOptions->dbnode;      // use node running the db for repm server
                }
                else
                {
                    pOptions->repm_node = "";                    // use local machine
                }
                break;
            }
            default:
            {
                break;  // Default is rncDifferent_els40 -> repm_node has already right value
            }
        }

        if ( (0 != pOptions->repm_node[0]) && (0 != _tcsicmp(pOptions->repm_node, "localhost")) )
        {
// We do TEMPORARILY not support remote communication between client and server to prevent vulnerability
// that result from the fact that the server runs as user root. In this case it is necessary to pass username
// and passwort as parameters to the server to make a user change possible.
// Even better would be to not install the server as user root.
// Until this is done the loadercli will not be able to remotely communicate with the server.

            tsp01_CommErr commerr;
            tsp00_NodeId  localnode;
            tsp00_ErrText ErrText;

            sqlhostname(localnode, ErrText, commerr);
            if (0 != commerr)
            {
                // could not get local host name;
                fprintf(pOptions->outfile, "Getting local hostname failed (errno = %d)\n", STATIC_CAST(tsp00_Int4, commerr));
                rc = PRG_FATAL;
            }
            else
            {
                // Make the hostname a c string for comparison
                tsp00_NodeIdc Nodec;
                Nodec.p2c(localnode);
                if (0 != _tcsicmp(Nodec.asCharp(), pOptions->repm_node) )
                {
                    fprintf(pOptions->outfile, "\n** ATTENTION **\n"
                                               "Remote communication not supported (option -r);\n"
                                               "use the local Loader server instead\n\n");
                    rc = PRG_SHOW;      // This is a 'misuse' of a special error message to prevent the client
                                        // from writing too much error messages to stdout/output file
                }
            }
        }

        if (PRG_OK == rc)
        {
            char pszNodeString[NODEID_MXSP00 + sizeof(char)] = "local host";
            if ('\0' != pOptions->repm_node[0])
            {
                if (_tcslen(pOptions->repm_node) > NODEID_MXSP00)
                {
                    rc = PRG_FATAL;
                }
                else
                {
                    strcpy(&pszNodeString[0], pOptions->repm_node);
                }
            }

            if (PRG_OK == rc)
            {
                szDbName.Init();
                szNode.Init();
                szDbRoot.Init();

                if ('\0' != pOptions->repm_node[0])
                {
                    szNode.rawAssign(pOptions->repm_node);
                }

                //*
                //*     Adjust calling parameter at connect to server
                //*
                //* - user may have specified: repm_node, dbnode, dbname, dbroot
                //* - name of loader server host is ALWAYS specified; empty string means <local host>
                //*
                //* 1. if dbnode != repm_node
                //*     ---> dbname (ALWAYS)= leer, dbroot = [<name>]
                //*
                //* 2. if dbnode == repm_node
                //*     - dbname given, no dbroot given     ---> dbname = <name>, dbroot = leer
                //*     - dbname given, dbroot given        ---> dbname = leer,   dbroot = <name>
                //*     - no dbname given, dbroot given     ---> dbname = leer,   dbroot = <name>
                //*     - no dbname given, no dbroot given  ---> dbname = leer,   dbroot = leer

                if (0 != _tcsicmp(pOptions->repm_node, pOptions->dbnode) )
                {
                    if ('\0' != pOptions->szDBRoot_ls40[0])
                    {
                        szDbRoot.rawAssign(pOptions->szDBRoot_ls40.asCharp());
                    }
                }
                else
                {
                    if ('\0' != pOptions->szDBRoot_ls40[0])
                    {
                        szDbRoot.rawAssign(pOptions->szDBRoot_ls40.asCharp());
                    }
                    else
                    {
                        if (NULL != pOptions->dbname)
                        {
                            szDbName.rawAssign(pOptions->dbname);
                        }
                    }
                }

                char pszReply[2*PATH_MXSP00];
                rc = cn14connectRPM(szNode, szDbName, szDbRoot, pszReply, &(pOptions->session), errtext);
                if (PRG_OK == rc)
                {
                    if (false == pOptions->bSuppressOutput_ls40)
                    {
                        fprintf(pOptions->outfile,
                                "Opened connection to Loader SERVER at node %s.\n", pszNodeString);
                        if (NULL != pszReply)
                        {
                            cn13fputs(pOptions->outfile, pszReply, true);
                            cn13fputs(pOptions->outfile, "\n", false);
                        }
                    }
                }
                else
                {
                    if (false == pOptions->bSuppressOutput_ls40)
                    {
                        fprintf(pOptions->outfile, "Connection to Loader SERVER at node %s failed: %s\n",
                                                    pszNodeString, STATIC_CAST(char*,errtext) );
                    }
                    rc = PRG_FATAL;
                }

                fflush(pOptions->outfile);
            }

        }   // end if (PRG_OK == rc)

    }   // end else of if (true == pOptions->bStaticServer_ls40)


    //*
    //*     Get maximum packet length and build command buffer;
    //* depends on static or not
    //*
    if (PRG_OK == rc)
    {
        ls40_InitBuffers(*pOptions);
    }

    return rc;
}
//  ls40_ServerConnect()


/*
  -----------------------------------------------------------------------------
  function:     ls40_DisconnectServer
  -----------------------------------------------------------------------------
*/
static void
ls40_DisconnectServer(tls40_Options *pOptions)
{
    if (NULL != pOptions->session)
    {
        if (true == pOptions->bStaticServer_ls40)
        {
            ls40_InitBuffers(*pOptions);

            sp77sprintf(pOptions->pszCmdBuffer, MAX_COMMAND_LENGTH_LS40, "RELEASE");
            pOptions->CmdBufferLen = _tcslen(pOptions->pszCmdBuffer);
            SAPDB_Int4 rc = ls40_ServerCommand(pOptions);
            if (PRG_CMD_ERROR == rc)
            {
                fprintf(pOptions->outfile, "Could not release user connection\n\n");
            }

            ctrlservclose(pOptions->session);
        }
        else
        {
            cn14release(&pOptions->session);
        }
    }
}
// ls40_DisconnectServer()

/*
  -----------------------------------------------------------------------------
  function:     ls40_UserConnect
  -----------------------------------------------------------------------------
*/
static int ls40_UserConnect(tls40_Options &opt)
{
    int         rc = PRG_OK;

    const char *pszPassword;
    size_t      nUserNameLength;
    SAPDB_Int4  lPos = 0;

    //*
    //* Set Isolation level in any case if different from the default value 3
    //*
    if ( (3 != opt.lIsoLevel_ls40) && (30 != opt.lIsoLevel_ls40) )
    {
        ls40_InitBuffers(opt);

        sp77sprintf(opt.pszCmdBuffer, MAX_COMMAND_LENGTH_LS40, "SET ISOLATION LEVEL %d", opt.lIsoLevel_ls40);
        opt.CmdBufferLen = _tcslen(opt.pszCmdBuffer);
        rc = ls40_ServerCommand(&opt);
        if (PRG_CMD_ERROR == rc)
        {
            fprintf(opt.outfile, "Could not set isolation level: %s\n\n", opt.pszReplyBuffer);
        }
    }

    if ( 0 == opt.dbname[0] )      // db name given ?
    {
        if ( (0 != opt.dbuser[0]) && (false == opt.bSuppressOutput_ls40) )
        {
            fprintf(opt.outfile, "Error connecting to database: database name missing\n");
        }
        return rc;
    }

    ls40_InitBuffers(opt);

    if (0 == opt.dbuser[0])
    {
        // connects only the loader server to db but does not logon user
        // (password and user name missing)
        lPos = sp77sprintf(opt.pszCmdBuffer, MAX_COMMAND_LENGTH_LS40, "USE SERVERDB %s", opt.dbname);
    }
    else
    {
        // Separate user name from password
        pszPassword = strchr (opt.dbuser, ',');
        if (NULL != pszPassword)
        {
            nUserNameLength = pszPassword - opt.dbuser;
            pszPassword++;
        }
        else
        {
            nUserNameLength = strlen(opt.dbuser);
            pszPassword = "";
        }

        if ( 0 == opt.dbschema[0] )
        {
            strncat(opt.dbschema, opt.dbuser, nUserNameLength);
        }

        // Mask DB name with double quotes to make special identifiers (like
        // hyphen -) possible in DB names

        lPos = sp77sprintf(opt.pszCmdBuffer, MAX_COMMAND_LENGTH_LS40, "USE USER \"%.*s\" %s SCHEMA %s SERVERDB \"%s\"",
                                                                      nUserNameLength,
                                                                      opt.dbuser,
                                                                      pszPassword,
                                                                      opt.dbschema,
                                                                      opt.dbname);
    }

    // Append node specification to command
    if ('\0' != opt.dbnode[0])
    {
        lPos += sp77sprintf(opt.pszCmdBuffer + lPos, MAX_COMMAND_LENGTH_LS40 - lPos, " ON %s", opt.dbnode);
        if (epUnknown_els00 != opt.EncryptionProtocol)
        {
            lPos += sp77sprintf(opt.pszCmdBuffer + lPos, MAX_COMMAND_LENGTH_LS40 - lPos,
                                                         " ENCRYPTION %s", pszEncryptionProtName_ls00[opt.EncryptionProtocol]);
        }
    }
   

    opt.CmdBufferLen = _tcslen(opt.pszCmdBuffer);

    rc = ls40_ServerCommand(&opt);
    if (false == opt.bSuppressOutput_ls40)      // output only if requested
    {
        char szTmp[256];
        szTmp[0] = 0;

        char pszNode[1024] = "local host";
        if ('\0' != opt.dbnode[0])
        {
            strcpy(&pszNode[0], opt.dbnode);
        }

        if (PRG_OK == rc)
        {
            if (0 == opt.dbuser[0])
            {
                sprintf(szTmp, "Connection to database %s on %s established.\n", opt.dbname, pszNode);
            }
            else
            {
                sprintf(szTmp, "User %.*s connected to database %s schema %s on %s.\n",
                               nUserNameLength, opt.dbuser, opt.dbname, opt.dbschema, pszNode);
            }
        }
        else if (PRG_CMD_ERROR == rc)
        {
            if (0 == opt.dbuser[0])
            {
                sprintf(szTmp, "Error establishing connection to database %s on %s: %s\n",
                               opt.dbname, pszNode, opt.pszReplyBuffer);
            }
            else
            {
                sprintf(szTmp, "Error connecting user %.*s to database %s on %s: %s\n",
                               nUserNameLength, opt.dbuser, opt.dbname, pszNode, opt.pszReplyBuffer);
            }
        }
        // else means PRG_SERVER_ERROR error msg is already written by ls40_ServerCommand

        if (0 != *szTmp)
        {
            cn13fputs(opt.outfile, szTmp, true);
        }
        fflush(opt.outfile);
    }

    return rc;
}
//  ls40_UserConnect()

/*
  -----------------------------------------------------------------------------
  function:     ls40_IsCmdSeparator
  -----------------------------------------------------------------------------
*/
static SAPDB_Bool
ls40_IsCmdSeparator(tls40_CmdSeparator *pCmdSep,
                    _TCHAR             *p,
                    tsp00_Uint4        &plen)
{
    bool rc = false;

    size_t i = 0;
    if (true == pCmdSep->csDefaultSeparator)
    {
        // We have to deal with 2 different default separators depending on
        // the mode of operation - interactively or batch;
        // - in case of batch mode the default separator is built by 2 adjacent slashes
        //   at the beginning of a line
        // - in case of interactive mode the default separator is simply a line
        //   without a continuation character '/' at the end
        if ('/' == pCmdSep->csCmdSeparator_ls40[1])
        {
            // Batch mode: separator consists of 2 slashes

            if (plen >= DEFAULT_CMD_SEPARATOR_LENGTH_LS40)
            {
                for (i = 0; i < plen; ++i)
                {
                    if ( (0 == isspace(p[i])) && (0 == iscntrl(p[i])) )
                    {
                        if ('/' == p[i])
                        {
                            if ('/' == p[i+1])
                            {
                                rc = true;
                                plen = 0;       // 'Remove' read buffer
                            }
                        }
                        break;
                    }
                }
            }
        }
        else
        {
            // Interactive mode: default separator is a line without continuation character

            if ( (plen <= 1) || ( ('/' != p[plen-2]) && ('/' != p[plen-1]) ) )
            {
                // It is the separator (the last) line: the line does not end with a '/'
                // In case plen <= 1 the buffer is empty (contains only the CR character)
                rc = true;
            }
            else
            {
                // It is not the separator line -> remove the continuation character '/'
                if (plen > 1)
                {
                    if ('/' == p[plen-1])
                    {
                        p[plen-1] = 0;
                        plen -= 1;
                    }
                    else if ('/' == p[plen-2])
                    {
                        p[plen-2] = 0;
                        plen -= 2;
                    }
                }
            }
        }   // end else of if ('/' == pCmdSep->csCmdSeparator_ls40[1])

    }   // end if (true == pOptions->CmdSeparator_ls40.csDefaultSeparator)
    else
    {
        char* _pDest = NULL;
        if (0 == pCmdSep->csCmdSeparator_ls40[1])
        {   // Separator has a length of 1 character
            _pDest = strrchr(p, pCmdSep->csCmdSeparator_ls40[0]);
            if (NULL != _pDest)
            {
                rc = true;
                i  = _pDest + sizeof(char) - p;
            }
        }
        else
        {   // Separator has a length of 2 characters
            _pDest = strrchr(p, pCmdSep->csCmdSeparator_ls40[1]);
            if ( (NULL != _pDest) && (_pDest != p) )    // make sure wanted char pos is NOT the beginnig of p
            {
                _pDest -= sizeof(char);
                if (pCmdSep->csCmdSeparator_ls40[0] == *_pDest)
                {
                    rc = true;
                    i  = (_pDest + 2*sizeof(char)) - p;
                }
            }
        }   // end else of if (0 == pCmdSep->csCmdSeparator_ls40[1])

        if (true == rc)
        {
            // Check if only white spaces follow
            for (i ; ( (i < plen) && (true == rc) ); ++i)
            {
                if ( (0 == isspace(p[i])) && (0 == iscntrl(p[i])) )
                {
                    rc = false;
                }
            }
            if (true == rc)
            {
                plen    = _pDest -p;    // Adapt buffer length
                p[plen] = 0;            // 'Remove' everything behind command separator if only white spaces
            }
        }
    }   // end else of if (true == pCmdSep->csDefaultSeparator)



    // Check if command line is empty if it is not the command separator
    if (SAPDB_FALSE == rc)
    {
        SAPDB_Bool bEmptyLine = SAPDB_TRUE;
        for (i = 0; ( (i < plen) && (SAPDB_TRUE == bEmptyLine) ); ++i)
        {
            if ( (0 == isspace(p[i])) && (0 == iscntrl(p[i])) )
            {
                bEmptyLine = SAPDB_FALSE;
            }
        }
        if (SAPDB_TRUE == bEmptyLine)
        {
            plen = 0;
        }
    }

    return rc;
}
//  ls40_IsCmdSeparator()


/*
  -----------------------------------------------------------------------------
  function:     ls40_Substitute
  -----------------------------------------------------------------------------
*/
static tsp00_Int4
ls40_Substitute(tls40_Options &opt, _TCHAR *pszBuffer, tsp00_Longint & lBufferLength)
{
    tsp00_Int4 rc          = 0;

    _TCHAR        *pszBufStart = pszBuffer;
    tsp00_Longint  lMyBufLen   = lBufferLength;


    tsp00_Int4    lIndex    = 0;
    tsp00_Longint lRestLen  = 0;
    tsp00_Longint lParamLen = 0;

    _TCHAR *pszFound  = NULL;
    bool    bNotFound = true;

    while (NULL != (pszFound = strchr(pszBuffer, '&') ) )
    {
        bNotFound = true;

        if ( 0 != isdigit(pszFound[1]) )            // We treat only parameters with one digit
        {
            _TCHAR pszTemp[2] = {'\0'};
            pszTemp[0] = pszFound[1];
            if (0 != (lIndex = atoi(pszTemp) ) )    // the char following & must be a digit
            {
                lIndex -= 1;                        // lIndex moves by -1, because we start at 0

                if (NULL != opt.pszParamList_ls40[lIndex])  // No parameter defined -> jump over it
                {
                    bNotFound = false;

                    lParamLen = _tcslen(opt.pszParamList_ls40[lIndex]);

                    if ( (lMyBufLen + lParamLen - 3) > MAX_COMMAND_LENGTH_LS40)
                    {
                        rc = -1;                     // entire length of string to generate may not exceed
                        break;                       // allowed length (considering terminating 0, too)
                    }

                    lRestLen = lMyBufLen - ((pszFound - pszBufStart) + 2);

                    SAPDB_memmove(&pszFound[lParamLen], pszFound + 2, lRestLen);
                    SAPDB_memcpy(pszFound, opt.pszParamList_ls40[lIndex], lParamLen);

                    pszBuffer = pszFound + lParamLen;       // move buf ptr to new position behind inserted
                                                            // parameter string
                    lMyBufLen += (lParamLen - 2);           // adapt length

                    pszBufStart[lMyBufLen] = '\0';          // memmove destroys '\0'
                }
            }
        }

        if (true == bNotFound)
        {
            pszBuffer = &pszFound[1];
        }
    }

    if (lMyBufLen != lBufferLength)
    {
        lBufferLength = lMyBufLen;
    }

    return rc;
}
// ls40_Substitute()


/*
  -----------------------------------------------------------------------------
  function:     ls40_ExecuteTaskBatch
  -----------------------------------------------------------------------------
*/
static int
ls40_ExecuteTaskBatch(tls40_Options &opt)
{
    int rc = PRG_OK;

    _TCHAR        *pszBuffer     = new _TCHAR[MAXBUFFERSIZE_LS40];
    tsp00_Uint4    lBufferLen    = 0;

    int            lErrNum       = 0;
    char          *dummy         = NULL;
    bool           _bCmdfound    = false;
    bool           _bCmdSepFound = false;

    //*
    //*     Read command file until eof is reached and process found commands
    //*

    ls40_InitBuffers(opt);      // Initialize buffer
	char szErr[MAX_REPLY_LENGTH_LS00];
	tsp00_Int4 rcREAD = 0;

    while ( (PRG_OK == rc) && (EOF_LS00 != rcREAD) && (FALSE == opt.bCancelByte_ls40) )
    {
        opt.CmdBufferLen = 0;

        //*
        //*     Find rows belonging to command until we find a command separator
        //*     or the end of file is reached or the command becomes too long
        //*
        szErr[0] = 0;
		while ( (PRG_OK == rc) && (0 <= (rcREAD = ls30VFReadLnEx(opt.VirtualFile_ls40,       // read one row in DATA file
                                                                 pszBuffer,
                                                                 szErr,
                                                                 MAXBUFFERSIZE_LS40) ) ) )
        {
            lBufferLen = rcREAD;

            // Sets lBufferLen to 0 if command separator found
            _bCmdSepFound = ls40_IsCmdSeparator(&opt.CmdSeparator_ls40, pszBuffer, lBufferLen);

            // We need to 're'append the newline
            if (0 != lBufferLen)
            {
                memcpy(&pszBuffer[lBufferLen], NEW_LINE_LS00, NEW_LINE_LEN_LS00);
                lBufferLen += NEW_LINE_LEN_LS00;
                pszBuffer[lBufferLen] = 0;
            }

            opt.CmdBufferLen += lBufferLen;
            if (opt.CmdBufferLen > MAX_COMMAND_LENGTH_LS40)
            {
                //*
                //*     Error: command becomes too long
                //* Log the command to file and exit
                //*
                if (false == opt.bSuppressOutput_ls40)      // output only if requested
                {
                    if ('\n' == opt.pszCmdBuffer[opt.CmdBufferLen - lBufferLen - 1])
                    {
                        fprintf(opt.outfile, "%s", opt.pszCmdBuffer);
                    }
                    else
                    {
                        fprintf(opt.outfile, "%s\n", opt.pszCmdBuffer);
                    }
                    fprintf(opt.outfile, "Error processing command; command is too long\n"
                                         "Maximum command length: %d Bytes\n", MAX_COMMAND_LENGTH_LS40);
                }
                rc = PRG_FATAL;
            }
            else
            {
                //*
                //*     Copy command extract to command buffer
                //*
                if (lBufferLen > 0)
                {
                    strcat(opt.pszCmdBuffer, pszBuffer);

                    if (false == _bCmdfound)
                    {
                        _bCmdfound = true;
                    }
                }
                // Finally check if we can send the command to the server
                if ( (true == _bCmdfound) && (true == _bCmdSepFound) )
                {
                    opt.pszCmdBuffer[opt.CmdBufferLen - NEW_LINE_LEN_LS00] = 0;
                    break;
                }
            }
        }   // end while ( (PRG_OK == rc) && ...

        //*
        //*     Check for reading error
        //*
        if ( (PRG_OK == rc) && (rcREAD < 0))
        {
            if (EOF_LS00 != rcREAD)
            {
                if (false == opt.bSuppressOutput_ls40)      // output only if requested
                {
                    fprintf(opt.outfile, "Error reading from command file: %d", lBufferLen);
                }
                rc = PRG_FATAL;
            }
        }

        //*
        //*     Process found command
        //*
        if ( (PRG_OK == rc) && (true == _bCmdfound) )
        {
            //* Remove trailing newline chars
            if ('\n' == opt.pszCmdBuffer[opt.CmdBufferLen - 1])
            {
                opt.CmdBufferLen -= 1;
                if ('\r' == opt.pszCmdBuffer[opt.CmdBufferLen - 1])
                {
                    opt.CmdBufferLen -= 1;
                }
                opt.pszCmdBuffer[opt.CmdBufferLen] = 0;
            }

            //*
            //*     Substitute parameters and log command to outfile
            //*
            if (false == opt.bNoParam_ls40)
            {
                ls40_Substitute(opt, opt.pszCmdBuffer, opt.CmdBufferLen);
            }

            if (false == opt.bSuppressOutput_ls40)      // output only if requested
            {
                cn13fputs(opt.outfile, opt.pszCmdBuffer, true);
                if ('\n' != opt.pszCmdBuffer[opt.CmdBufferLen - 1])
                {
                    cn13fputs(opt.outfile, "\n", false);
                }
            }

            //*
            //*     Send command to server and wait for reply
            //*
            rc = ls40_ServerCommand(&opt);
            if (FALSE == opt.bCancelByte_ls40)      // In case of cancel request LOADERCLI writes special message
            {                                       // at the end of batch processing
                if (PRG_OK == rc)
                {
                    if (false == opt.bSuppressOutput_ls40)      // output only if requested
                    {
                        if ('\0' != opt.pszReplyBuffer[0])
                        {
                            cn13fputs(opt.outfile, "Successfully executed:\n", false);
                            cn13fputs(opt.outfile, opt.pszReplyBuffer, true);
                            cn13fputs(opt.outfile, "\n\n", false);
                        }
                        else
                        {
                            cn13fputs(opt.outfile, "Successfully executed\n\n", false);
                        }
                    }
                }
                else if (PRG_CMD_ERROR == rc)
                {
                    if (false == opt.bSuppressOutput_ls40)      // output only if requested
                    {
                        cn13fputs(opt.outfile, "Error during execution\n-->", false);
                        cn13fputs(opt.outfile, opt.pszReplyBuffer, true);
                        cn13fputs(opt.outfile, "\n\n", false);
                    }

                    ++lErrNum;
                    if ( (0 != opt.BatchErrorCnt_ls40) && (lErrNum == opt.BatchErrorCnt_ls40) )
                    {
                        if (false == opt.bSuppressOutput_ls40)      // output only if requested
                        {
                            char _szTmp[64];
                            _szTmp[0] = 0;
                            sprintf(_szTmp, "Defined maximum number of errors (%d) reached\n", opt.BatchErrorCnt_ls40);
                            cn13fputs(opt.outfile, _szTmp, true);
                        }
                    }
                    else
                    {
                        rc = PRG_OK;
                    }
                }
            }
        }   // end if ( (PRG_OK == rc) && (true == _bCmdfound) )

        _bCmdfound = false;           // reinitialize
        ls40_InitBuffers(opt);      // reinitialize buffer

        fflush(opt.outfile);
    }   // end while ( (PRG_OK == rc) && ( 0 == feof(opt.infile) ) )


    if (NULL != pszBuffer)
    {
        delete [] pszBuffer;
        pszBuffer = NULL;
    }

    // In case of error while processing commands or cancellation print summary to stderr
    char _szTmp[256];
    _szTmp[0] = 0;

    if (FALSE != opt.bCancelByte_ls40)
    {
        sprintf(_szTmp, "\n\nReceived cancel request while processing command file '%s'.\n"
                        "---> See Loader log file for more information\n",
                        opt.VirtualFile_ls40.vfFileName_ls00);
        cn13fputs(stderr, _szTmp, true);
    }
    else if ( (false == opt.bSuppressOutput_ls40) && (0 != lErrNum) )
    {
        sprintf(_szTmp, "\n\n%d error(s) while processing command file '%s'.\n"
                        "---> See Loader log file for more information.\n",
                        lErrNum, opt.VirtualFile_ls40.vfFileName_ls00);
        cn13fputs(stderr, _szTmp, true);
    }
    fflush(opt.outfile);

    return rc;
}
//  ls40_ExecuteTaskBatch()

/*
  -----------------------------------------------------------------------------
  function:     ls40_ExecuteTaskSteps
  -----------------------------------------------------------------------------
*/
static int
ls40_ExecuteTaskSteps(tls40_Options &options, char *argv[])
{
    int rc = PRG_OK;
    
    if ( true == options.bLoaderExtension_ls40 )
    {
        char  OptionValue[2048];
        OptionValue[0] = 0;
        if ( _tcsicmp(options.executionkind ,"" ) != 0 )
        {
             strcat(OptionValue, " -T ");
             strcat(OptionValue, options.executionkind);
        }
        if ( _tcsicmp(options.transformationtype ,"" ) != 0 )
        {
             strcat(OptionValue, " -t ");
             strcat(OptionValue, options.transformationtype);
        }
        if ( _tcsicmp(options.transformationpackage ,"" ) != 0 )
        {
             strcat( OptionValue, " -p ");
             strcat( OptionValue, options.transformationpackage );
        }
        if ( _tcsicmp(options.transformationpart ,"" ) != 0 )
        {
             strcat( OptionValue, " -P ");
             strcat( OptionValue, options.transformationpart );
        }
        if ( _tcsicmp(options.transformationstreams ,"" ) != 0 )
        {
             strcat( OptionValue, " -s ");
             strcat( OptionValue, options.transformationstreams );
        }
        if ( _tcsicmp(options.startposition ,"" ) != 0 )
        {
            strcat( OptionValue, " -i ");
            strcat( OptionValue, options.startposition );
        }
        if ( _tcsicmp(options.streammedium ,"" ) != 0 )
        {
             strcat(OptionValue, " -m ");
             strcat(OptionValue, options.streammedium);
        }
        if ( _tcsicmp(options.streamformat ,"" ) != 0 )
        {
             strcat(OptionValue, " -f ");
             strcat(OptionValue, options.streamformat);
        }
        if ( _tcsicmp(options.streamformat_target ,"" ) != 0 )
        {
             strcat(OptionValue, " -F ");
             strcat(OptionValue, options.streamformat_target);
        }
        if ( _tcsicmp(options.streamcharencoding ,"" ) != 0 )
        {
             strcat(OptionValue, " -c ");
             strcat(OptionValue, options.streamcharencoding );
        }
        if ( _tcsicmp(options.streamlob ,"" ) != 0 )
        {
             strcat(OptionValue, " -l ");
             strcat(OptionValue, options.streamlob);
        }
        if ( _tcsicmp(options.dbnode ,"" ) != 0 )
        {
             strcat(OptionValue, " -nSRC ");
             strcat(OptionValue, options.dbnode);
        }
        if ( _tcsicmp(options.dbname ,"" ) != 0 )
        {
             strcat( OptionValue, " -dSRC " );
             strcat( OptionValue, options.dbname );
        }
        if ( _tcsicmp(options.dbuser ,"" ) != 0 )
        {
             strcat(OptionValue , " -uSRC " );
             strcat(OptionValue , options.dbuser);
        }
        if ( _tcsicmp(options.dbschema ,"" ) != 0 )
        {
             strcat(OptionValue , " -sSRC " );
             strcat(OptionValue , options.dbschema);
        }
        if ( _tcsicmp(options.dbuserkey ,"" ) != 0 )
        {
        }
        if ( _tcsicmp(options.targetdbnode ,"" ) != 0 )
        {
             strcat(OptionValue, " -nTRG " );
             strcat(OptionValue, options.targetdbnode );
        }
        if ( _tcsicmp(options.targetdbname ,"" ) != 0 )
        {
             strcat(OptionValue, " -dTRG ");
             strcat(OptionValue, options.targetdbname);
        }
        if ( _tcsicmp(options.targetdbuser ,"" ) != 0 )
        {
             strcat(OptionValue, " -uTRG ");
             strcat(OptionValue, options.targetdbuser);
        }
        if ( _tcsicmp(options.targetdbschema ,"" ) != 0 )
        {
             strcat(OptionValue, " -sTRG ");
             strcat(OptionValue, options.targetdbschema);
        }
        if ( _tcsicmp(options.targetdbuserkey ,"" ) != 0 )
        {
        }
        const SAPDB_Int4 _lVarSize = 2*PATH_MXSP00;
        tsp01_RteError RteError;
        char           LibPath[_lVarSize + sizeof(char)];
        char           PythonPath[_lVarSize + sizeof(char)];
        char           BinPath[_lVarSize + sizeof(char) + 2048];
        tsp00_Pathc    IndependentBinPath;
        tsp00_Pathc    IndependentLibPath;
        LibPath[0]    = 0;
        PythonPath[0] = 0;
        BinPath[0]    = 0;
        IndependentBinPath.Init();
        IndependentLibPath.Init();
#if defined(WIN32)
        if ( sqlGetIndependentPgmPath(IndependentLibPath, TERM_WITH_DELIMITER_EO01, &RteError) ) 
        {
            sp77sprintf(PythonPath, _lVarSize, "PYTHONPATH=%s%s", IndependentLibPath.asCharp(), PYTHONLIB);
            putenv(PythonPath);
        }
#else
        if ( sqlGetIndependentLibPath(IndependentLibPath, TERM_WITH_DELIMITER_EO01, &RteError) ) 
        {
            sp77sprintf(PythonPath, _lVarSize, "PYTHONPATH=%s%s", IndependentLibPath.asCharp(), PYTHONLIB);
            putenv(PythonPath);
        }
#endif
        if ( !sqlGetIndependentBinPath(IndependentBinPath, TERM_WITH_DELIMITER_EO01, &RteError) )
        {
             BinPath[0] = 0;
             IndependentBinPath.Init();
             printf("Could not open IndependentBin Path %s\n", RteError.RteErrText);
        }
        IndependentLibPath.Init();
        if ( !sqlGetIndependentLibPath (IndependentLibPath, TERM_WITH_DELIMITER_EO01, &RteError) ) 
        {
             LibPath[0] = 0;
             IndependentLibPath.Init();
             printf("Could not open IndependentLib Path %s\n", RteError.RteErrText);
        }
        if ( NULL != IndependentBinPath.asCharp() && NULL != IndependentLibPath.asCharp() )
        {
            Tools_DynamicUTF8String  patterns((SAPDB_UTF8*)" ");
            Tools_DynamicUTF8String  transformation;
            Tools_DynamicUTF8String  xpython;
            Tools_DynamicUTF8String  transport;
            Tools_DynamicUTF8String  indepprogrampath;
            Tools_DynamicUTF8String  indeplibpath;

            indepprogrampath.AssignRaw(REINTERPRET_CAST(unsigned char*, IndependentBinPath.asCharp()),_tcslen(IndependentBinPath.asCharp()));
            indeplibpath.AssignRaw    (REINTERPRET_CAST(unsigned char*, IndependentLibPath.asCharp()),_tcslen(IndependentLibPath.asCharp()));

            xpython    = indepprogrampath;
            xpython   += XPYTHON;
            transport  = indeplibpath;
            transport += LOADEREXTENSIONDIR;
            transport += LOADEREXTENSION;
            BinPath[0] = 0;
            sp77sprintf(BinPath, _lVarSize + 2048, "%s%s%s%s %s%s%s%s %s", 
                        SHELL_DELIMITER,
                        SHELL_DELIMITER, REINTERPRET_CAST(SAPDB_Char*, (CONST_CAST (unsigned char*, xpython  .StrPtr()))), SHELL_DELIMITER,
                        SHELL_DELIMITER, REINTERPRET_CAST(SAPDB_Char*, (CONST_CAST (unsigned char*, transport.StrPtr()))), SHELL_DELIMITER,
                        SHELL_DELIMITER,
                        OptionValue);
            system(BinPath);
            exit(0);
        }
    }// if (true == options.bLoaderExtension_ls40)

    return rc;

}//  ls40_ExecuteTaskSteps()

/*
  -----------------------------------------------------------------------------
  function:     ls40_EmptyStdinBuffer (helper function)
  -----------------------------------------------------------------------------
*/
static void
ls40_EmptyStdinBuffer(tls40_Options* pOptions)
{
    // Coming here we KNOW that the command did not fit into the available buffer;
    // the file read here is ALWAYS stdin;
    // fgets reads without user interaction as long as the new line char is not found,
    // thus emptying the stdin buffer
    if (0 == _tcscmp(pOptions->VirtualFile_ls40.vfFileName_ls00, "stdin") )          // make sure to process only stdin
    {
        SAPDB_UInt4 _len = 0;

        // As we know that the content of pssCmdBuffer can not be processed (cmd is too long)
        // we can use this command buffer to 'empty' the stdin buffer;
        // there is also no need to specify UNICODE upon reading thus no converting will be done
        pOptions->pszCmdBuffer[0] = 0;
        while (NULL != cn13fgets(pOptions->pszCmdBuffer, MAX_COMMAND_LENGTH_LS40, stdin, false))
        {
            _len = _tcslen(pOptions->pszCmdBuffer);
            if ('\n' == pOptions->pszCmdBuffer[_len - 1])
            {
                break;
            }
            pOptions->pszCmdBuffer[0] = 0;
        }
    }
    pOptions->pszCmdBuffer[0] = 0;
    return;
}
// ls40_EmptyStdinBuffer()


/*
  -----------------------------------------------------------------------------
  function:     ls40_ExecuteTaskInteractive
  -----------------------------------------------------------------------------
*/
static int
ls40_ExecuteTaskInteractive(tls40_Options *pOptions)
{
    int rc = PRG_OK;
    SAPDB_Int4 rcREAD = 0;

    SAPDB_UInt4 _lPromptLen = _tcslen("loadercli") + sizeof(char);
    char*       _pszPrompt1 = new char[_lPromptLen + sizeof(char)];
    char*       _pszPrompt2 = new char[_lPromptLen + sizeof(char)];

    sp77sprintf(_pszPrompt1, _lPromptLen + sizeof(char), "%s>", "loadercli");
    sp77sprintf(_pszPrompt2, _lPromptLen + sizeof(char), "%*s->", _lPromptLen - 2, " ");

    _TCHAR      *_pszBuffer     = new _TCHAR[MAX_COMMAND_LENGTH_LS40];
    SAPDB_UInt4  _linelen       = 0;
    char        *_pszDummy      = NULL;
    SAPDB_Bool   _bCmdSepFound  = SAPDB_FALSE;
    SAPDB_Bool   _bCmdFound     = SAPDB_FALSE;


    ls40_InitBuffers(*pOptions);

    // Print prompt
    cn13fputs(pOptions->outfile, _pszPrompt1, true);

    //*
    //*     Read command(s) from stdin and process
    //*
    while ( (false == pOptions->bRelease_ls40) && (PRG_OK == rc) )
    {
        _bCmdFound             = SAPDB_FALSE;
        _linelen               = 0;
        pOptions->CmdBufferLen = 0;

        while ( (PRG_OK == rc) &&
                (NULL   != (_pszDummy = cn13fgets(_pszBuffer, MAX_COMMAND_LENGTH_LS40, stdin, true) ) ) )
        {
            _linelen = strlen(_pszBuffer);

            if (_linelen > sizeof(char)) {
                // Check for end of command
                _bCmdSepFound = ls40_IsCmdSeparator(&pOptions->CmdSeparator_ls40, _pszBuffer, _linelen);

                pOptions->CmdBufferLen += _linelen;
                if (pOptions->CmdBufferLen > MAX_COMMAND_LENGTH_LS40)
                {
                    // Error: command becomes too long; we'll 'misuse' PRG_SHOW here for 'command too long' error
                    ls40_EmptyStdinBuffer(pOptions);
                    rc = PRG_SHOW;
                }
                else
                {
                    //*
                    //*     Copy command extract to command buffer
                    //*
                    strcat(pOptions->pszCmdBuffer, _pszBuffer);
                    if (SAPDB_FALSE == _bCmdFound)
                    {
                        _bCmdFound = SAPDB_TRUE;
                    }
                    // Finally check if we can send the command to the server
                    if ( (SAPDB_TRUE == _bCmdFound) && (SAPDB_TRUE == _bCmdSepFound) )
                    {
                        break;
                    }
                }
            }   // end if (_linelen > sizeof(char))

            // Print prompt
            cn13fputs(pOptions->outfile, _pszPrompt2, true);
        }   // end while ( (PRG_OK == rc) && (NULL != (cn13fgets(...


        if ( (PRG_OK == rc) && (NULL == _pszDummy) )
        {
            int nError = ferror(stdin);
            if (0 != nError)
            {
                fprintf(stderr, "Error reading from stdin: %d", nError);
                rc = PRG_FATAL;
            }
            else
            {
                // Release connection to server
                pOptions->pszCmdBuffer[0] = 0;
                strcat(pOptions->pszCmdBuffer, "exit");
                pOptions->CmdBufferLen = strlen(pOptions->pszCmdBuffer);

                rc = PRG_OK;
            }
        }   // end if ( (PRG_OK == rc) && (NULL == _pszDummy) )

        //*
        //*     Finally process the command
        //*
        if (PRG_OK == rc)
        {
            // Remove newline character
            if (pOptions->pszCmdBuffer[pOptions->CmdBufferLen - 1] == '\n')
            {
                --pOptions->CmdBufferLen;
            }

            // Process command
            rc = ls40_ServerCommand(pOptions);
            if (PRG_OK == rc)
            {
                cn13fputs(pOptions->outfile, "OK\n", false);
                if ('\0' != pOptions->pszReplyBuffer[0])
                {
                    cn13fputs(pOptions->outfile, pOptions->pszReplyBuffer, true);
                    cn13fputs(pOptions->outfile, "\n", false);
                }
            }
            else if (PRG_CMD_ERROR == rc)
            {
                cn13fputs(pOptions->outfile, "ERR ", false);
                cn13fputs(pOptions->outfile, pOptions->pszReplyBuffer, true);
                cn13fputs(pOptions->outfile, "\n", false);
                rc = PRG_OK;
            }

            // Reset Cancel byte if it has been set; Cancel stops a single command in interactive modus
            pOptions->bCancelByte_ls40 = false;
        }

        if (PRG_SHOW == rc)
        {
            // Command was too long to fit into buffer -> print error message
            char szTmp[128];
            szTmp[0] = 0;
            sprintf(szTmp, "Error processing command; command is too long ("
                            "Maximum command length: %d Bytes)\n", MAX_COMMAND_LENGTH_LS40);
            cn13fputs(pOptions->outfile, szTmp, true);

            rc = PRG_OK;
        }

        // Print prompt
        cn13fputs(pOptions->outfile, "\n---\n", true);
        if (false == pOptions->bRelease_ls40)
        {
            cn13fputs(pOptions->outfile, _pszPrompt1, true);
        }

        ls40_InitBuffers(*pOptions);
    }   // end while ( (false  == pOptions->bRelease_ls40) && ...

    return rc;
}
//  ls40_ExecuteTaskInteractive()


/*
  -----------------------------------------------------------------------------
  function: ls40_CheckAnswer
  -----------------------------------------------------------------------------
*/
static int ls40_CheckAnswer(tls40_Options & options, tsp00_Int4 ReplyLen)
{
    short   i               = 0;
    int     nErrorNumber    = PRG_OK;


    if (0 == strncmp(options.pszReplyBuffer, REPMAN_ERR_STR, strlen(REPMAN_ERR_STR)))       // error
    {
        // Skip indicator "err" and read error number. The error is given in
        // the form "E<error number>:" . The first and the last char need to be removed.
        nErrorNumber = atol(&options.pszReplyBuffer[REPLY_HEADER_SIZE_LS40 + 1]);

        options.CmdExecError = (nErrorNumber == 0) ? PRG_FATAL : nErrorNumber;
        nErrorNumber = PRG_CMD_ERROR;
    }

    // Remove answer header in any case
    tsp00_Int4 len = ReplyLen - REPLY_HEADER_SIZE_LS40;

    SAPDB_memmove(options.pszReplyBuffer, &options.pszReplyBuffer[REPLY_HEADER_SIZE_LS40], len);
    options.pszReplyBuffer[len] = '\0';

    return nErrorNumber;
}
//  ls40_CheckAnswer()


/*
  -----------------------------------------------------------------------------
  function: ls40_CheckForReleaseCmd
  -----------------------------------------------------------------------------
*/
static SAPDB_Bool
ls40_CheckForReleaseCmd(char* pszCmdBuffer, SAPDB_UInt4 lCmdLen)
{
    SAPDB_UInt4 i = 0;

    for (0; (i < lCmdLen) && ( (0 != isspace(pszCmdBuffer[i])) || (0 != iscntrl(pszCmdBuffer[i])) ); ++i)
        ;

    return ( (i < lCmdLen) &&
             ( (0 == _tcsnicmp(&pszCmdBuffer[i], "release", 7) ) ||
               (0 == _tcsnicmp(&pszCmdBuffer[i], "bye", 3)     ) ||
               (0 == _tcsnicmp(&pszCmdBuffer[i], "exit", 4)    ) ||
               (0 == _tcsnicmp(&pszCmdBuffer[i], "quit", 4)    )  ) );
}
// ls40_CheckForReleaseCmd()


/*
  -----------------------------------------------------------------------------
  function: ls40_ServerCommand
  -----------------------------------------------------------------------------
*/
static int ls40_ServerCommand(tls40_Options  *options)
{
    tsp00_Int4      rc        = PRG_OK;

    tsp00_ErrTextc  errtext;
    tsp00_Int4      commErr   = CtrlCmdOk_csp09;
    tsp00_Int4      nReplyLen = options->ReplyBufferLen;

    options->bRelease_ls40 = ls40_CheckForReleaseCmd(options->pszCmdBuffer, options->CmdBufferLen);
    if (false == options->bRelease_ls40)
    {
        if (true == options->bStaticServer_ls40)
        {
            // bei statischem betrieb kann das kommando beliebig lang sein, da es einfach
            // als funktionsparameter uebergeben wird

            options->pszCommBuffer_ls40[1] = 1;     // give server a hint to run statically linked

            options->CmdBufferLen += RPM_PROTOCOL_SIZE_CN14;

            commErr = ctrlservcommand(options->session,
                                      options->pszCommBuffer_ls40,
                                      STATIC_CAST(tsp00_Int4, options->CmdBufferLen),
                                      options->pszReplyBuffer,
                                      &nReplyLen,
                                      RPM_REPLY_BUFFER_LEN_CN14);
        }
        else
        {
            commErr = cn14ExecuteLoaderCmd(options->session,
                                           options->pszCommBuffer_ls40,
                                           STATIC_CAST(tsp00_Int4, options->CmdBufferLen),     // IA64
                                           options->pszReplyBuffer,
                                           &nReplyLen,
                                           errtext);
        }

        // return codes of server: CtrlCmdOk_csp09      0
        //                         CtrlCmdErr_csp09     1
        //                         CtrlRelease_csp09    2

        if (CtrlCmdOk_csp09 == commErr)         // no server error; check for command
        {                                       // processing return code
            rc = ls40_CheckAnswer(*options, nReplyLen);
        }
        else if (CtrlRelease_csp09 == commErr)      // relaese command; nothing to do
        {
            ;
        }
        else                                        // server error
        {
            if (false == options->bSuppressOutput_ls40)
            {
                fprintf (options->outfile, "Loader server error (%d : '%s')",
                                           STATIC_CAST(int, commErr),
                                           STATIC_CAST(char*, errtext) );
            }
            rc = PRG_SERVER_ERROR;
        }

    }   // end if (false == options->bRelease_ls40)


    return rc;
}
//  ls40_ServerCommand()

