/*!---------------------------------------------------------------------
  @file           RTE_StopDatabase.cpp
  @author         JoergM
  @brief          DBM: Stop a SAPDB Instance

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
---------------------------------------------------------------------*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Types.hpp"


#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"
#  if defined(WIN32)
// gos00.h
   /*! typedef */
   typedef DWORD PID;
   /*! typedef */
   typedef DWORD APIRET;
/*! define */
#include <io.h>
#define PROGRAM_NAME_STOP "stop.exe"
#define PROGRAM_NAME_DBMSTOP "dbmstop.exe"
#define DUP _dup
#define DUP2 _dup2
#define CLOSE _close
#else
#include "RunTime/RTE_UNIXAuthenticate.hpp" /* nocheck */
/*! define */
#define PROGRAM_NAME_STOP "stop"
#define PROGRAM_NAME_DBMSTOP "dbmstop"
#define DUP dup
#define DUP2 dup2
#define CLOSE close
#endif
#include "RunTime/RTE_ActiveDatabase.hpp"
#include "RunTime/RTE_DBRegister.hpp"
#include "RunTime/RTE_GetDBRootOfDB.h"
#include "heo01.h"


/*!
  @brief call the right program
  Call the depend program using dbname to find dependend path
  @param gracefully [in]
  @param argc [in]
  @param argv [in]
  @param dbname [in]
  @return result
 */
SAPDB_Bool CallRightProgram( SAPDB_Bool gracefully,
                             int    argc, 
                             char * argv[],
                             char * dbname )
{
    tsp01_RteError  aRteError;
    tsp00_Pathc     szExecPath;
    tsp00_Pathc     szDbroot;

    RTE_Path dbroot;

    if ( !RTE_GetDBRootOfDB( dbname,
                             &dbroot[0],
                             sizeof(RTE_Path) ) )
    {
		RTE_Message(SAPDBErr_MessageList( RTE_CONTEXT,
                                          SAPDBErr_MessageList::Error,
                                          0,
                                          "Failed to get installation root for database %s",
                                          1,
                                          (const char *)dbname ), MSG_DIAG_CONSOLE);
        return false;
    }

    char szPath  [1024 + 1];

    if ( !sqlGetMyModuleFilename ( argv[0], szPath, 1024, &aRteError) )
    {
		RTE_Message(SAPDBErr_MessageList( RTE_CONTEXT,
                                          SAPDBErr_MessageList::Error,
                                          0,
                                          "Failed to get module filename for %s",
                                          1,
                                          (const char *)argv[0] ), MSG_DIAG_CONSOLE);
        return false;
    }

    szExecPath.rawAssign(szPath);
    sqlGetDBrootFromExecPath(szExecPath, szDbroot, 0);

    char *pCurrentExeName = strrchr( szPath,PATH_DELIMITER_EO01 ) + 1;

/* with http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1139731 the behaviour has changed: when the gracefully flag is set (default),     */
/* the program dbmstop is called, which sends a SHUTDOWN command to the kernel. */
    const char *programToCall = gracefully ? PROGRAM_NAME_DBMSTOP : PROGRAM_NAME_STOP;
#if defined(WIN32)
    if ( !stricmp(dbroot, szDbroot) && !stricmp(pCurrentExeName,programToCall) )
#else
    if ( !strcmp(dbroot, szDbroot)  && !strcmp(pCurrentExeName,programToCall) )
#endif
    {
        return true;
    }

#ifdef _WIN32
    sprintf(szPath, "\"\"%s\\pgm\\%s\"", dbroot, programToCall);
#else
    sprintf(szPath, "%s/pgm/%s", dbroot, programToCall);
#endif

    for (int i = 1; i < argc; ++i) 
    {
        strcat(szPath, " ");
        if (strchr(argv[i], ' ') != NULL) strcat(szPath, "\"");
        strcat(szPath, argv[i]);
        if (strchr(argv[i], ' ') != NULL) strcat(szPath, "\"");
    } // end for
#ifdef _WIN32
    strcat(szPath, "\"");
#endif
    int old = DUP(1);   // save stdout
    CLOSE(1);           // and close it (suppress output of executed program)

    int nExit = system(szPath);

    DUP2( old,1 );      // restore stdout

    if (nExit == 0)
    {
        printf("OK\n");
    }
    else if (nExit == -1)
    {
		RTE_Message(SAPDBErr_MessageList( RTE_CONTEXT,
                                          SAPDBErr_MessageList::Error,
                                          0,
                                          "Failed to call right program %s",
                                          1,
                                          (const char *)&szPath[0] ), MSG_DIAG_CONSOLE);
    } else {
         exit(nExit);
    } // end if
    return false;
} // CallRightProgram

/*!
  @brief program usage
  @param beVerbose [in] if set report all options
  @return none
 */
static void Usage(SAPDB_Bool beVerbose)
{
    printf("Usage: x_stop DBName [-force] [-dump]\n");
    if ( beVerbose )
    {
        printf("Stop the runing database instance DBName with optional parameter for supressing gracefully shutdown before stop\n");
        printf("The program is equivalent to 'dbmcli' command db_offline, but needs no authentification\n"); 
        printf("Special handling for additional arguments...\n");
        printf("  -force allows to suppress automatic shutdown (like dbmcli command db_stop)\n");
        printf("  -dump  enforce creation of kernel dump during stop\n");
        printf("Example: x_stop SAPDB                 stop the running instance gracefully\n");
        printf("         x_stop SAPDB -force          stop the running instance without implicit shutdown\n");
        printf("         x_stop SAPDB -force -dump    stop the running instance with dump and without shutdown\n");
    }
}

/*!
  @brief parse options
  @param argc [in]
  @param argv [in]
  @param givenDbname [out]
  @param beVerbose [out]
  @param wantDump [out]
  @param gracefully [out]
  @param errList [out]
  @return result
 */
SAPDB_Bool ParseOptions( const int    argc,
                         char       * argv[],
                         SAPDB_Char * &givenDbname,
                         SAPDB_Bool   &beVerbose,
                         SAPDB_Bool   &wantDump,
                         SAPDB_Bool   &gracefully,
                         SAPDBErr_MessageList &errList )
{
    SAPDB_Bool paramOk = false;
    if ( argc < 2 )
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEINFO_ARGUMENT_MISSING,
                                        "dbname" );
        return false;
    }

    SAPDB_Bool waitForDbname = true;

    for ( int numAdditionalArgs = argc-1; numAdditionalArgs > 0; --numAdditionalArgs )
    {
        SAPDB_Char *checkedArgument = argv[argc-numAdditionalArgs];

        if ( '-' == checkedArgument[0] )
        {
            // check for well known speeds
            if ( !strcmp("-force", checkedArgument) )
            {
                gracefully = false;
            }
            else if ( !strcmp("-p", checkedArgument)
                   || !strcmp("-dump", checkedArgument) )
            {
                wantDump = true;
            }

            // check for '-d' introducer for database name
            else if ( !strcmp("-d", checkedArgument) )
            {
                if ( givenDbname )
                {
                    errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                    RTEINFO_ARGUMENT_COLLISION,
                                                    givenDbname,
                                                    checkedArgument );
                    return false;
                }
            }

            // check for usage option
            else if ( !strcmp("-h", checkedArgument) )
            {
                beVerbose = true;
                return false;
            }
            else if ( !strcmp("-?", checkedArgument) )
            {
                beVerbose = true;
                return false;
            }

            // check for verbosity option
            else if ( checkedArgument[1] == 'v' 
                   || checkedArgument[1] == 'V'
                   || ( checkedArgument[1] == 'D' 
                     && ( checkedArgument[2] == 0 
                       || isdigit(checkedArgument[2]) 
                        )
                      )
                    )
            {
                beVerbose = true;
            }
            else if ( !strcmp("-verbose", checkedArgument) )
            {
                beVerbose = true;
            }

            else
            {
                errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                RTEINFO_ARGUMENT_UNEXPECTED,
                                                checkedArgument );
                return false;
            }
        }
        else if ( waitForDbname )
        {
            givenDbname = checkedArgument;
        }
        else
            break;
    }

    if ( !givenDbname )
    {
        errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                        RTEINFO_ARGUMENT_MISSING,
                                        "DbName" );
        return false;
    }

    return true;
}

//-----------------
#if defined(WIN32)
extern "C" void sql60_enable_console();
#endif

/*!
  @brief dbstop main program call
  @param argc [in]
  @param argv [in]
  @return exit code
 */
int main(int argc, char *argv[])
{
    int result = 0;
    RTE_DBName dbname;
    SAPDB_Char *givenDbname = 0;
    RTE_ActiveDatabase * dbInstance = 0;
    SAPDB_Bool beVerbose = false;
    SAPDB_Bool wantDump = false;
    SAPDB_Bool gracefully = true;
    SAPDBErr_MessageList errList;
#if defined(WIN32)
    sql60_enable_console();
#endif

    if ( !ParseOptions( argc,
                        argv,
                        givenDbname,
                        beVerbose,
                        wantDump,
                        gracefully,
                        errList ) )
    {
        RTE_Message(errList, MSG_DIAG_CONSOLE);
        Usage(beVerbose);
        return 1;
    }

    if ( beVerbose )
    {
        printf("Arguments\n");
        int iArg;
        for ( iArg = 0; iArg < argc; iArg++ )
        {
            printf("%d:%s\n", iArg, argv[iArg]);
        }
    }

#ifndef WIN32
    if ( !RTE_SetSapdbOwnerAndGroupId(errList) )
    {
        RTE_Message(errList, MSG_DIAG_CONSOLE);
        return 1;
    }
#endif

    if ( givenDbname )
    {
        if ( strlen(givenDbname) > (sizeof(dbname)-1) )
        {
            printf("Given dbNane %s too long\n", givenDbname );
            Usage(false);
            return 1;
        }

        SAPDB_Int iName;
        for ( iName = 0; *givenDbname; ++iName )
        {
            dbname[iName] = toupper(*givenDbname++);
        }
        dbname[iName] = 0;
    }

    if ( !CallRightProgram(gracefully, argc, argv, dbname) )
    {
        return 1;
    }

    dbInstance = RTE_DBRegister::Instance().GetActiveDatabase(dbname, errList);
    if ( !dbInstance )
    {
        RTE_Message(errList, MSG_DIAG_CONSOLE);
        return 1;
    }

    if ( beVerbose )
    {
        printf("Stopping database %s %s %s dump\n",
                    dbname,
                    gracefully ? "gracefully" : "forced",
                    wantDump ? "with" : "without" 
              );
    }

    if ( !dbInstance->Stop(errList, 90, wantDump, gracefully) )
    {
        RTE_Message(errList, MSG_DIAG_CONSOLE);
        result = 1;
    }
    else
    {
        printf("OK\n");

        if ( beVerbose )
        {
            RTE_Message(SAPDBErr_MessageList(RTE_CONTEXT,RTEINFO_STOP_SUCCESSFUL,dbname), MSG_DIAG_CONSOLE);
        }

        result = 0;
    }

    delete dbInstance;

    return result;
}
