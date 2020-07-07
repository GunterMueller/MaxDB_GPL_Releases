/*!---------------------------------------------------------------------
  @file           RTE_StartDatabase.cpp
  @author         JoergM
  @brief          DBM: Start a SAPDB Instance

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
#include "geo00_2.h"
#  if defined(WIN32)
   typedef DWORD PID;
   typedef DWORD APIRET;
#define PROGRAM_NAME "strt.exe"
#else
#include "RunTime/RTE_saveUNIXcalls.h" /* nocheck */
#include "RunTime/RTE_UNIXAuthenticate.hpp" /* nocheck */
#define PROGRAM_NAME "start"
#endif
#include "RunTime/RTE_ActiveDatabase.hpp"
#include "RunTime/RTE_DBRegister.hpp"
#include "RunTime/RTE_GetDBRootOfDB.h"
#include "heo01.h"

/*-------------------------------------------*/
SAPDB_Bool CallRightProgram( char const *programToCall,
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

#if defined(WIN32)
    if ( !stricmp(dbroot, szDbroot) )
#else
    if ( !strcmp(dbroot, szDbroot) )
#endif
    {
        return true;
    }

#ifdef _WIN32
    sprintf(szPath, "\"\"%s\\pgm\\%s\"", dbroot, programToCall);
#else
    sprintf(szPath, "%s/bin/%s", dbroot, programToCall);
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
    int nExit = system(szPath);
    if (nExit == -1)
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

//--------------------------------------------------------

static void Usage(SAPDB_Bool beVerbose)
{
#ifdef ALLOW_OWN_EXECUTABLE
    printf("Usage: x_start DBName [-<speed>] [-x <executable>] [additional arguments for executable]\n");
#else
    printf("Usage: x_start DBName [-<speed>] [additional arguments for executable]\n");
#endif
    if ( beVerbose )
    {
        printf("Start the database instance DBName with optional parameter for speed, start mode and arguments\n");
        printf("The program is equivalent to 'dbmcli' command db_online, but needs no authentification\n"); 
        printf("To start only into admin mode, the extra argument -admin has to be given!\n");
        printf("Possible value for <speed> are fast,quick,slow and test\n");
#ifdef ALLOW_OWN_EXECUTABLE
        printf("other values for <speed> must be combined with an own executable\n");
#endif
        printf("Special handling for additional arguments...\n");
        printf("  -i or -admin allows to suppress automatic restart into online mode\n");
        printf("  -l allows to start kernel in lower priority mode\n");
        printf("  -D, -v, -V or -verbose allows to get the startup output even if successfully started\n\n");
        printf("Example: x_start SAPDB                            start with default speed (fast)\n");
        printf("         x_start SAPDB -slow                      start with 'slow' speed\n");
        printf("         x_start SAPDB -test -admin               start into 'admin' mode using 'test' speed\n");
#ifdef ALLOW_OWN_EXECUTABLE
        printf("         x_start SAPDB -own -x kernel_try -admin  start using special executable for own speed\n");
#endif
    }
}

//--------------------------------------------------------

SAPDB_Bool ParseOptions( const int    argc,
                         char       * argv[],
                         SAPDB_Char * &givenDbname,
                         SAPDB_Char const * &speedWanted,
#ifdef ALLOW_OWN_EXECUTABLE
                         SAPDB_Char * &databaseProgramGiven,
#endif
                         SAPDB_Bool   &beVerbose,
                         SAPDB_Bool   &adminMode,
                         SAPDB_Bool   &lowPriority,
                         int          &numAdditionalArgs,
                         char **      &AdditionalArgs,
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

#ifdef ALLOW_OWN_EXECUTABLE
    SAPDB_Bool waitForProgram = false;
#endif
    SAPDB_Bool waitForDbname = true;

    for ( numAdditionalArgs = argc-1; numAdditionalArgs > 0; --numAdditionalArgs )
    {
        SAPDB_Char *checkedArgument = argv[argc-numAdditionalArgs];

        if ( '-' == checkedArgument[0] )
        {
            // check for well known speeds
                 if ( !strcmp("-fast", checkedArgument) )
            {
                if ( speedWanted )
                {
                    errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                    RTEINFO_ARGUMENT_COLLISION,
                                                    speedWanted,
                                                    checkedArgument );
                    return false;
                }
                speedWanted = "fast";
            }
            else if ( !strcmp("-quick", checkedArgument) )
            {
                if ( speedWanted )
                {
                    errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                    RTEINFO_ARGUMENT_COLLISION,
                                                    speedWanted,
                                                    checkedArgument );
                    return false;
                }
                speedWanted = "quick";
            }
            else if ( !strcmp("-slow", checkedArgument) )
            {
                if ( speedWanted )
                {
                    errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                    RTEINFO_ARGUMENT_COLLISION,
                                                    speedWanted,
                                                    checkedArgument );
                    return false;
                }
                speedWanted = "slow";
            }
            else if ( !strcmp("-test", checkedArgument) )
            {
                if ( speedWanted )
                {
                    errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                    RTEINFO_ARGUMENT_COLLISION,
                                                    speedWanted,
                                                    checkedArgument );
                    return false;
                }
                speedWanted = "test";
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

            // check for admin mode option
            else if ( !strcmp("-admin", checkedArgument) 
                   || !strcmp("-i", checkedArgument) )
            {
                adminMode = true;
            }

            // check for admin mode option
            else if ( !strcmp("-l", checkedArgument) )
            {
                lowPriority = true;
            }

#ifdef ALLOW_OWN_EXECUTABLE
            // check for own program option
            else if ( checkedArgument[1] == 'x' )
            {
                speedWanted = "own";
                waitForProgram = ( 0 != checkedArgument[2] );
                if ( !waitForProgram )
                {
                    databaseProgramGiven = &checkedArgument[2];
                }
            }
#endif
            // check for end of parsable options option
            else if ( checkedArgument[1] == '-' ) 
            {
                --numAdditionalArgs;
                break;
            }

            else
            {
                errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                                RTEINFO_ARGUMENT_UNEXPECTED,
                                                checkedArgument );
                return false;
            }
        }
#ifdef ALLOW_OWN_EXECUTABLE
        else if ( waitForProgram )
        {
            databaseProgramGiven = checkedArgument;
        }
#endif
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

    if ( numAdditionalArgs > 0 )
    {
        AdditionalArgs = &argv[argc-numAdditionalArgs];
    }

    return true;
}

//-----------------
#if defined(WIN32)
extern "C" void sql60_enable_console();
#endif

int main(int argc, char *argv[])
{
    int result = 0;
    RTE_DBName dbname;
    SAPDB_Char *givenDbname = 0;
    RTE_ActiveDatabase * dbInstance = 0;
    SAPDB_Char const *speedWanted = 0;
#ifdef ALLOW_OWN_EXECUTABLE
    SAPDB_Char *databaseProgramGiven = 0;
#endif
    SAPDB_Bool beVerbose = false;
    SAPDB_Bool adminMode = false;
    SAPDB_Bool lowPriority = false;
    int numAdditionalArgs = 0;
    char **AdditionalArgs = 0;
    SAPDBErr_MessageList errList;
#if defined(WIN32)
    sql60_enable_console();
#endif

    if ( !ParseOptions( argc,
                        argv,
                        givenDbname,
                        speedWanted,
#ifdef ALLOW_OWN_EXECUTABLE
                        databaseProgramGiven,
#endif
                        beVerbose,
                        adminMode,
                        lowPriority,
                        numAdditionalArgs,
                        AdditionalArgs,
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

#ifndef WIN32
    if ( !RTE_SetSapdbOwnerAndGroupId(errList) )
    {
        RTE_Message(errList, MSG_DIAG_CONSOLE);
        return 1;
    }
#endif

    if ( !CallRightProgram(PROGRAM_NAME, argc, argv, dbname) )
    {
        return 1;
    }

    dbInstance = RTE_DBRegister::Instance().GetActiveDatabase(dbname, errList);
    if ( !dbInstance )
    {
        RTE_Message(errList, MSG_DIAG_CONSOLE);
        return 1;
    }

    // speedFast is set by default, so the active speed must be set only if quick or slow is desired

    RTE_SpeedInfo & speed = speedFast;

    if ( !speedWanted )
    {
        speedWanted = "fast";
    }
    else
    {
        if ( !strcmp(speedWanted, "fast") )
        {
            speed = speedFast;
        }
        else if ( !strcmp(speedWanted, "quick") )
        {
            speed = speedQuick;
        }
        else if ( !strcmp(speedWanted, "slow") )
        {
            speed = speedSlow;
        }
        else if ( !strcmp(speedWanted, "test") )
        {
            speed = speedTest;
        }
        else
        {
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEINFO_ARGUMENT_MISSING,
                                            "-speed" );
        }
    }

#ifdef ALLOW_OWN_EXECUTABLE
    if ( !databaseProgramGiven )
    {
        if ( speedWanted )
        {
            errList = SAPDBErr_MessageList( RTE_CONTEXT,
                                            RTEINFO_ARGUMENT_COLLISION,
                                            "-speed",
                                            "-x executable" );
        }
        else
        {
            speed = RTE_SpeedInfo((const unsigned char *)speedWanted, 5, false , databaseProgramGiven);
        }
    }
#endif

    if ( beVerbose )
    {
        printf("Starting database %s speed %s into state %s with priority %s\n",
                    dbname,
                    speedWanted,
                    adminMode ? "ADMIN" : "ONLINE",
                    lowPriority ? "LOW" : "NORMAL" 
              );
    }
    if ( !dbInstance->Start(speed, errList, numAdditionalArgs, AdditionalArgs, !adminMode, lowPriority) )
    {
        RTE_Message(errList, MSG_DIAG_CONSOLE);
        result = 1;
    }
    else
    {
        if ( beVerbose )
        {
            RTE_Message(errList, MSG_DIAG_CONSOLE);
        }

        const RTE_Database::DBStatusInfo* pStatus;
        pStatus = dbInstance->GetDBStatus(errList);
        if ( !pStatus )
        {
            RTE_Message(errList, MSG_DIAG_CONSOLE);
            result = 1;
        }
        else
        {
            printf("OK\n");

            if ( (adminMode 
               && pStatus->statusId != SERVER_ADMIN_EO00)
             ||  (!adminMode
               && pStatus->statusId != SERVER_ONLINE_EO00
               && pStatus->statusId != SERVER_STANDBY_EO00) )
            {
                printf( "Warning: Expected database state was %s\n", adminMode ? "ADMIN" : "ONLINE");
            }
            printf("Database %s started to status %s\n", dbname, pStatus->statusName );
        }

        result = 0;
    }

    delete dbInstance;

    return result;
}
