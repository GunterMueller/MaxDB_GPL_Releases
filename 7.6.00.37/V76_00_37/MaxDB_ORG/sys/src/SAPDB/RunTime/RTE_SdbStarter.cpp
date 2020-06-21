/*!
  @file           RTE_SdbStarter.cpp
  @author         JoergM
  @brief          DBM RunTime: Start a given program as if logged on as SdbOwner
  
  It also contains the code for RTE_CallSdbStarter and the program code for sdbstarttest

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#include <stdio.h>

#include "heo01.h" /* needed only to check for 'Berlin' development environment
                      eo01_IsEnvVarDoNotUseIndepPathSet() */
#include "RunTime/RTE_SdbStarter.h"
#include "Messages/Msg_List.hpp"

static int dontCheckOwnerAndMode = 0;
static int beQuiet = 0;
static int beVerbose = 0;
static int orgArgc;
static char **orgArgv;
static int sdbUserId = -1;
static int sdbGroupId = -1;

/*---------------------------------------*/

static void ShowArguments(int argc, char **argv)
{
    int iArg;

    if ( argv[argc] )
    {
        fprintf(stderr, "Got %d arguments, but argv is not 0 terminated '%s'\n", argc, argv[argc]);
    }
    else
    {
        fprintf(stderr, "Got %d arguments and argv is 0 terminated\n", argc);
    }
    for ( iArg = 0; iArg < argc; iArg++ )
    {
        fprintf(stderr, "argv[%d]:'%s'\n", iArg, argv[iArg]);
    }
}

/*---------------------------------------*/

static void Usage(char const *msg=0)
{
    if ( msg )
    {
        fprintf(stderr, "%s\n", msg);
    }

    if ( beVerbose )
    {
        fprintf(stderr, "Argument list\n");
        ShowArguments(orgArgc, orgArgv);
    }

    fprintf(stderr, "Syntax: " SDBSTARTER_PROGRAM " program [{<arguments>}]\n");
    fprintf(stderr, "Usage: Start given program for parent directory with\n");
    fprintf(stderr, "       argument list -starter followed by given arguments\n");
    fprintf(stderr, "       The started program will be started as SdbOwner and\n");
    fprintf(stderr, "       with supementary groups reinitialized for SdbOwner\n");
}

#if defined(WIN32)

int main(int argc, char *argv[])
{
    orgArgc = argc;
    orgArgv = &argv[0];

    Usage("Tool for UNIX only");
    return(1);
}

#else

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <grp.h>
#include "RunTime/RTE_IniFileHandling.h"
#include "SAPDBCommon/SAPDB_Names.h"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_saveUNIXcalls.h"
#include "RunTime/RTE_UNIXAuthenticate.hpp"
#include "RunTime/System/RTESys_IO.h"

/* for sdbstarter program directory restrict to read+execute for sdbOwner (allow 0202000 for aix...) */
#define SDBSTARTER_DIRECTORY_MODE  (S_IFDIR|S_ISGID|S_IRUSR|S_IXUSR)

/* sdbstarter program must be setuid root with execute only for sdbadmin group */
#define SDBSTARTER_PROGRAM_MODE    (S_IFREG|S_ISUID|S_IRUSR|S_IXUSR|S_IXGRP)

/* for called program directory restrict to read+execute for sdbOwner (allow 0202000 for aix...) */
#define CALLED_DIRECTORY_MODE      (S_IFDIR|S_ISGID|S_IRUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)

/* called program must be setuid sdbowner setgid sdbadmin with execute only for owner and group */
#define CALLED_PROGRAM_MODE        (S_IFREG|S_ISUID|S_ISGID|S_IRUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)

/* Mode mask to protect agains 'strange' mode definitions... */
/* i.e. AIX adds to directories to 0242000 instead of 040000... 0200000 is not defined in sys/mode.h... */
#define MODE_TYPE_MASK (S_IFMT|S_ISUID|S_ISGID|S_IRWXU|S_IRWXG|S_IRWXO)

/*
  Check ownership against given user id , group id and minimum wanted mode.
  The modeWanted is the minimum restriction enforced. I.e. if a read access for world is granted, this
  is not really a problem... */

typedef char ModeMaskAsString[11]; /* 11 == strlen("drwxrwxrwx") + 1]; */

static void BuildModeMask(int mode, ModeMaskAsString &output)
{
    memset(&output[0], '-', sizeof(ModeMaskAsString)-1);
    output[sizeof(ModeMaskAsString)-1] = 0;

    if ( S_ISFIFO(mode) )
        output[0] = 'p';
    if ( S_ISCHR(mode) )
        output[0] = 'c';
    if ( S_ISBLK(mode) )
        output[0] = 'b';
    if ( S_ISDIR(mode) )
        output[0] = 'd';
    if ( S_ISLNK(mode) )
        output[0] = 'l';

    if ( mode & S_IRUSR )
        output[1] = 'r';
    if ( mode & S_IWUSR )
        output[2] = 'w';
    if ( mode & S_IXUSR )
        output[3] = 'x';
    if ( (mode & (S_IXUSR|S_ISUID)) == (S_IXUSR|S_ISUID) )
        output[3] = 's';
    if ( (mode & (S_IXUSR|S_ISUID)) == (S_ISUID) )
        output[3] = 'S';

    if ( mode & S_IRGRP )
        output[4] = 'r';
    if ( mode & S_IWGRP )
        output[5] = 'w';
    if ( mode & S_IXGRP )
        output[6] = 'x';
    if ( (mode & (S_IXGRP|S_ISGID)) == (S_IXGRP|S_ISGID) )
        output[6] = 's';
    if ( (mode & (S_IXGRP|S_ISGID)) == (S_ISGID) )
        output[6] = 'l';

    if ( mode & S_IROTH )
        output[7] = 'r';
    if ( mode & S_IWOTH )
        output[8] = 'w';
    if ( mode & S_IXOTH )
        output[9] = 'x';
}

static SAPDB_Bool CheckOwnership(char const *absolutePath, int userWanted, int groupWanted, int modeWanted)
{
    if ( dontCheckOwnerAndMode ) return true;

    struct stat statusBuffer;
    if ( RTE_save_stat(absolutePath, &statusBuffer) )
    {
        fprintf(stderr, "Status of absolute path %s not determined\n", absolutePath);
        return false;
    }

    if (  ((int)statusBuffer.st_uid)  != userWanted
      ||  ((int)statusBuffer.st_gid)  != groupWanted
      ||  (((int)statusBuffer.st_mode)&(~modeWanted)&MODE_TYPE_MASK) != 0 )
    {
        ModeMaskAsString wantedModeMask;
        ModeMaskAsString gotModeMask;

        BuildModeMask((int)modeWanted, wantedModeMask);
        BuildModeMask((int)statusBuffer.st_mode, gotModeMask);

        fprintf(stderr, "Bad path '%s'\n"
                        " Wanted (UserId %5d, GroupId %5d, Mode %6o '%s')\n"
                        " Got    (UserId %5d, GroupId %5d, Mode %6o '%s')\n",
                absolutePath,
                userWanted,
                groupWanted,
                modeWanted,
                &wantedModeMask[0],
                (int)statusBuffer.st_uid,
                (int)statusBuffer.st_gid,
                (int)statusBuffer.st_mode,
                &gotModeMask[0] );
        return false;
    }

    if ( beVerbose )
    {
        fprintf(stderr, "Good %s Wanted (%d,%d,%o) got (%d,%d,%o)\n",
                absolutePath,
                userWanted,
                groupWanted,
                modeWanted,
                (int)statusBuffer.st_uid,
                (int)statusBuffer.st_gid,
                (int)statusBuffer.st_mode );
    }

    return true;
}

/*
   Trim tail of directory path and strip it from all '/'
 */
static void TrimDirectoryPath(char *directoryPath)
{
    int lengthOfPath = (int)strlen(directoryPath);
    while ( lengthOfPath > 0 )
    {
        if ( directoryPath[lengthOfPath-1] == '/' )
        {
            --lengthOfPath;
            directoryPath[lengthOfPath] = 0;
        }
        else
        {
            break;
        }
    }

    if ( lengthOfPath == 0 )
    {
        /* root directory was trimmed to empty string... */
        strcpy( directoryPath, "/");
    }
}

/*
  This implements 'access(path, X_OK) in exactly the way unix works...
  First the user is tested, on match only the user priveledge counts
  Next the grouo is tested, on match only the group priveledge counts
  Now all gropus contain in process current group list are tested. On any match only group counts.
  At last the world preveledge is tested.

  NOTE: this is not thread save since the user id, group id and group list membership could be modified by other
  threads...
 */
static SAPDB_Bool IsAccessible(char const *path)
{
    struct stat statusBuffer;

    if ( 0 != RTE_save_stat(path, &statusBuffer) )
    {
        return false;
    }

    if ( statusBuffer.st_uid == RTE_save_geteuid() )
    {
        return ( (statusBuffer.st_mode & S_IXUSR) == S_IXUSR );
    }

    if ( statusBuffer.st_gid == RTE_save_getegid() )
    {
        return ( (statusBuffer.st_mode & S_IXGRP) == S_IXGRP );
    }

    int iGroup;
    gid_t groupList[NGROUPS_MAX];
    int nGroups = getgroups(NGROUPS_MAX, groupList);

    for ( iGroup = 0; iGroup < nGroups; iGroup++ )
    {
        if ( statusBuffer.st_gid == groupList[iGroup] )
        {
            return ( (statusBuffer.st_mode & S_IXGRP) == S_IXGRP );
        }
    }

    return ( (statusBuffer.st_mode & S_IXOTH) == S_IXOTH );
}

/*
  Try to change to given directory and return absolute path if possible. CurrentWorkingDirectory is preserved...
  NOTE: this is not thread save since chdir() in multiple threads would fail to produce what you want...
 */
static SAPDB_Bool SaveGetAbsoluteFromRelativePath(char const *relativePath, RTE_Path &absolutePath)
{
    SAPDB_Bool result;
    RTE_Path currentWorkingDirectory;
    /* Save original current working directory */
    if ( !RTE_save_getcwd(&currentWorkingDirectory[0], sizeof(RTE_Path)-1) )
    {
        beVerbose = 1;
        Usage("Current working directory does not exist or path is too long");
        exit(51);
    }

    /* try to change to relative path */
    if ( 0 == chdir ( relativePath ) )
    {
        if ( RTE_save_getcwd ( &absolutePath[0], sizeof(RTE_Path)-1 ) )
        {
            result = true;
        }
        else
        {
            if ( beVerbose )
            {
                fprintf(stderr, "Failed to get cwd for '%s'\n", relativePath);
            }
            result = false;
        }
    }
    else
    {
        if ( beVerbose )
        {
            fprintf(stderr, "Failed to chdir('%s')\n", relativePath);
        }
        result = false;
    }

    /* Switch back to original current working directory */
    if ( chdir(currentWorkingDirectory) )
    {
        beVerbose = 1;
        fprintf(stderr,
                "Changing back to current working directory %s failed\n",
                &currentWorkingDirectory[0]);
        Usage();
        exit(52);
    }

    if ( beVerbose )
    {
        fprintf(stderr, "Got absolute path %s for relative path %s\n", &absolutePath[0], relativePath);
    }

    return result;
}

/*
   Get absolute path for program
 */
static SAPDB_Bool GetAbsolutePath( char const *programPath,
                                   RTE_Path &absolutePath )
{
    if ( strlen(programPath) >= sizeof(RTE_Path) )
    {
        fprintf(stderr, "Path too long: %s\n", programPath);
        return false;
    }

    {
        char const *slashPlusProgramFilename = strrchr(programPath, '/');

        if ( slashPlusProgramFilename ) /* relative or absolute path */
        {
            RTE_Path tmpAbsolutePath;
            strncpy( &tmpAbsolutePath[0], programPath, slashPlusProgramFilename-programPath );

            tmpAbsolutePath[slashPlusProgramFilename-programPath] = 0;

            TrimDirectoryPath(&tmpAbsolutePath[0]);

            if ( SaveGetAbsoluteFromRelativePath(&tmpAbsolutePath[0], absolutePath) )
            {
                TrimDirectoryPath(&absolutePath[0]);

                strcat(&absolutePath[0], slashPlusProgramFilename);

                if ( IsAccessible(&absolutePath[0]) )
                {
                    return true;
                }
                else
                {
                    fprintf(stderr, "Absolute path %s from relative path %s not accessible\n",
                        &absolutePath[0],
                        &tmpAbsolutePath[0]);
                }
            }
            else
            {
                fprintf(stderr, "Cannot change to relative path %s\n", &tmpAbsolutePath[0]);
            }
            return false;
        }
    }

    char *PathFromEnvironment = getenv("PATH");

    if ( !PathFromEnvironment )
    {
        fprintf(stderr, "PATH not set\n");
        return false;
    }

    char *PathCopy = strdup(PathFromEnvironment);
    PathFromEnvironment = PathCopy;

    int FileNameLen = (int)strlen(programPath);
    char *pathElement;

    do 
    {
        int PathNameLen;

        if ( 0 != (pathElement = strchr(PathFromEnvironment, ':')) )
        {
            *pathElement = '\0'; /* seperate one dir, if pathElement == 0 handle last dir in path */
        }

        PathNameLen = (int)strlen ( PathFromEnvironment );

        if ( PathNameLen > 0 
          && (PathNameLen + FileNameLen) < (sizeof(RTE_Path)-1) )
        {
            RTE_Path tmpAbsolutePath;
            if ( *PathFromEnvironment == '/' )
            {
                strcpy(&tmpAbsolutePath[0], PathFromEnvironment);
            }
            else
            {
                if ( !SaveGetAbsoluteFromRelativePath(PathFromEnvironment, tmpAbsolutePath) )
                {
                    tmpAbsolutePath[0] = 0;
                    if ( beVerbose )
                    {
                        fprintf(stderr, "Failed to get path from '%s'\n", PathFromEnvironment);
                    }
                }
            }

            if ( 0 != tmpAbsolutePath[0] )
            {
                TrimDirectoryPath(&tmpAbsolutePath[0]);

                tmpAbsolutePath[strlen(&tmpAbsolutePath[0])+1] = 0;
                tmpAbsolutePath[strlen(&tmpAbsolutePath[0])] = '/';

                strncat(&tmpAbsolutePath[0], programPath, sizeof(RTE_Path)-1);

                if ( IsAccessible(&tmpAbsolutePath[0]) )
                {
                    free(PathCopy);
                    strcpy(&absolutePath[0], &tmpAbsolutePath[0]);
                    return true;
                }
                else if ( beVerbose )
                {
                    fprintf(stderr, "Failed access to '%s'\n", &tmpAbsolutePath[0]);
                }
            }
        }

        if ( pathElement )
        {
            PathFromEnvironment = (pathElement+1);
        }
    } while ( pathElement ) ;

    fprintf(stderr, "Found no absolute path for %s\n", programPath);
    return false;
}

/*
   Get absolute path for parent directory of sdbstarter program
 */
static SAPDB_Bool GetAbsoluteParentPath(char const *sdbstarterPath, RTE_Path &parentDirectoryPath)
{
    RTE_Path absolutePath;
    RTE_Path absoluteParentPath;

    if ( !GetAbsolutePath(sdbstarterPath, absolutePath) )
    {
        fprintf(stderr, "No absolute path for: %s\n", sdbstarterPath);
        return false;
    }

    if ( strlen(&absolutePath[0]) <= strlen(SDBSTARTER_PROGRAM) )
    {
        fprintf(stderr, "Absolute path too short: %s\n", &absolutePath[0]);
        return false;
    }

    if ( strcmp(SDBSTARTER_PROGRAM, &absolutePath[strlen(&absolutePath[0])-strlen(SDBSTARTER_PROGRAM)]) )
    {
        fprintf(stderr, "Wrong program called: %s\n", &absolutePath[0]);
        return false;
    }

    /* own program must be setuid root with execute only for sdbadmin group */
    if ( !CheckOwnership(&absolutePath[0], 0, sdbGroupId, SDBSTARTER_PROGRAM_MODE) )
    {
        beVerbose = 1;
        Usage("Wrong ownership for own absolute path");
        exit(53);
    }

    absolutePath[strlen(&absolutePath[0])-strlen(SDBSTARTER_PROGRAM)] = 0;

    if ( !SaveGetAbsoluteFromRelativePath(&absolutePath[0], parentDirectoryPath) )
    {
        fprintf(stderr, "Cannot get absolute path for %s\n", &absolutePath[0]);
        return false;
    }

    TrimDirectoryPath(&parentDirectoryPath[0]);

    /* for own directory restrict to read+execute for sdbOwner */
    if ( !CheckOwnership(&parentDirectoryPath[0], sdbUserId, sdbGroupId, SDBSTARTER_DIRECTORY_MODE) )
    {
        beVerbose = 1;
        Usage("Wrong ownership for own directory absolute path");
        exit(54);
    }

    strcat(&absolutePath[0], "/..");

    if ( !SaveGetAbsoluteFromRelativePath(&absolutePath[0], parentDirectoryPath) )
    {
        fprintf(stderr, "Cannot get absolute path for %s\n", &absolutePath[0]);
        return false;
    }

    TrimDirectoryPath(&parentDirectoryPath[0]);

    /* for program directory allow everything, but no write access... */
    if ( !CheckOwnership(&parentDirectoryPath[0], sdbUserId, sdbGroupId, CALLED_DIRECTORY_MODE) )
    {
        beVerbose = 1;
        Usage("Wrong ownership for parent directory absolute path");
        exit(55);
    }

    return true;
}

/*
   Get absolute path for given path and absoulte path of directory which contains program
 */
static SAPDB_Bool GetAbsolutePathAndDirectory(char const *programPath,
                                              RTE_Path &absolutePath,
                                              RTE_Path &absoluteDirectoryPath)
{
    if ( !GetAbsolutePath(programPath, absolutePath) )
    {
        fprintf(stderr, "No absolute path for: %s\n", programPath);
        return false;
    }

    /* called program must be setuid sdbowner setgid sdbadmin with execute only for owner and group */
    if ( !CheckOwnership(&absolutePath[0], sdbUserId, sdbGroupId, CALLED_PROGRAM_MODE) )
    {
        fprintf(stderr, "Wrong ownership program to call absolute path\n");
        return false;
    }

    int lengthOfPath = (int)strlen(&absolutePath[0]);

    if ( absolutePath[lengthOfPath-1] == '/' )
    {
        fprintf(stderr, "Bad termination of absolute path: %s\n", &absolutePath[0]);
        return false;
    }

    /* Copy absolute path and trim filename and '/' */
    strcpy(&absoluteDirectoryPath[0], &absolutePath[0]);

    while ( lengthOfPath > 0 )
    {
        if ( absoluteDirectoryPath[lengthOfPath-1] != '/' )
        {
            --lengthOfPath;
            absoluteDirectoryPath[lengthOfPath] = 0;
        }
        else
        {
            break;
        }
    }

    TrimDirectoryPath(&absoluteDirectoryPath[0]);

    return true;
}
/*
   Use argv_ZERO to find directory where sdbstarter was found and use programToCall to start the right program
 */
char *FindExecutable(char *argv_ZERO, char *programToCall)
{
    RTE_Path ownParentDirectoryPath;
    static RTE_Path callPath;
    RTE_Path callDirectoryPath;

    if ( beVerbose )
    {
        fprintf(stderr, "Searching absolute parent path for %s\n", argv_ZERO);
    }

    if ( !GetAbsoluteParentPath(argv_ZERO, ownParentDirectoryPath) )
    {
        beVerbose = 1;
        Usage("Own absolute parent path not determined");
        exit(56);
    }

    if ( beVerbose )
    {
        fprintf(stderr, "Absolute path for %s : %s\n", argv_ZERO, &ownParentDirectoryPath[0]);
    }

    if ( beVerbose )
    {
        fprintf(stderr, "Getting absolute path and parent directory for %s\n", programToCall);
    }

    if ( !GetAbsolutePathAndDirectory(programToCall, callPath, callDirectoryPath) )
    {
        beVerbose = 1;
        Usage("Program absolute path not determined");
        exit(57);
    }

    if ( beVerbose )
    {
        fprintf(stderr, "Absolute path and parent directory for %s : %s : %s\n", 
                programToCall,
                &callPath[0],
                &callDirectoryPath[0] );
    }

    if ( strcmp(ownParentDirectoryPath, callDirectoryPath) )
    {
        beVerbose = 1;
        fprintf(stderr,
                "Directory mismatch: own parent directory %s call directory %s\n",
                ownParentDirectoryPath,
                callDirectoryPath);
        Usage();
        exit(58);
    }

    if ( beVerbose )
    {
        fprintf(stderr, "Absolute path '%s' accepted\n", &callPath[0] );
    }

    return &callPath[0];
}


/*
  saved version of strdup that does allow a NULL pointer for end of argument list and exits if copy fails
 */
char *saveStrdup(char const *stringArg)
{
    char *copiedArg;
    if ( !stringArg )
    {
        copiedArg = (char *)0;
    }
    else
    {
        copiedArg = strdup(stringArg);
        if ( !copiedArg )
        {
            beVerbose = 1;
            Usage("Failed to copy argument");
            exit(59);
        }
    }
    return copiedArg;
}

#ifdef COMPILE_SDBSTARTER_PROGRAM

/*
  The given argument is copied into new argument vector.
  All first arguments with 'option introducer '-' are skipped.
  i.e. '-verbose' switches on verbosity inside this tool
  The first non optional argument is the program to execute. It is expected in the
  directory above the directory where this program was called.
  The first argument of the called program is '-sdbstarter'.
  The rest of argument list is copied directly as new argument vector.
 */
static void BuildNewArgumentVector(int argc, char *argv[], int &newArgc, char ** &newArgv)
{
    if ( argc < 2 )
    {
        Usage();
        exit(60);
    }

    newArgv = new char *[argc+1];

    if ( !newArgv )
    {
        beVerbose = 1;
        Usage("Failed to allocate new argument vector\n");
        exit(61);
    }

    int iArg = 1;

    /* Skipping sdbstarter local options */
    while ( iArg < argc 
         && *argv[iArg] == '-' )
    {
        if ( !strcmp(argv[iArg], "-verbose") )
        {
            beVerbose = 1;
        }
        else if ( !strcmp(argv[iArg], "-nocheck") )
        {
            dontCheckOwnerAndMode = 1;
        }
        else if ( !strcmp(argv[iArg], "-quiet") )
        {
            beQuiet = 1;
        }
        ++iArg;
    }

    if ( iArg >= argc )
    {
        beVerbose = 1;
        Usage("Missing program argument\n");
        exit(62);
    }

    newArgv[0] = saveStrdup(FindExecutable(argv[0], argv[iArg]));
    newArgv[1] = saveStrdup("-" SDBSTARTER_PROGRAM);

    int iNewArg = 2;

    /* Copies including zero... */
    for ( iArg++;
          iArg < argc;
          iArg++ )
    {
        newArgv[iNewArg++] = saveStrdup(argv[iArg]);
    }
    newArgv[iNewArg] = 0;

    newArgc = iNewArg;
}

/*---------------------------------------*/

int main(int argc, char *argv[])
{
    int newArgc;
    char **newArgv;

    orgArgc = argc;
    orgArgv = &argv[0];

    Msg_List errList;

    if ( !RTE_GetDefaultSapdbUserAndGroupIds( sdbUserId,
                                              sdbGroupId,
                                              errList) )
    {
        fprintf(stderr, "Failed to get sdb owner and group id\n");
        RTE_Message(errList, MSG_DIAG_CONSOLE);
        exit(63);
    }

    BuildNewArgumentVector(argc, argv, newArgc, newArgv);

    if ( beVerbose )
    {
        fprintf(stderr,"Got arguments:\n");
        ShowArguments(argc, &argv[0]);
        fprintf(stderr,"New arguments:\n");
        ShowArguments(newArgc, newArgv);
    }

    if ( !RTE_SetUserAndGroupId(sdbUserId, sdbGroupId, errList) )
    {
        fprintf(stderr, "Failed to change to sdb owner and group id\n");
        RTE_Message(errList, MSG_DIAG_CONSOLE);
        exit(64);
    }
    else if ( beVerbose )
    {
        fprintf(stderr, "Successfull changed to sdb owner and group id %d:%d\n", sdbUserId, sdbGroupId);
    }

    errno = 0;
    extern char **environ;

    if ( -1 == RTE_save_execve(newArgv[0], newArgv, environ) )
    {
        fprintf(stderr, "Failed to execve %s: error code %d:%s\n", newArgv[0], errno, strerror(errno));
        ShowArguments(newArgc, newArgv);
    }
    else
    {
        /* if execve did not return -1 errno may be wrong... */
        fprintf(stderr, "Failed to execve %s! errno ?%d?\n", newArgv[0], errno);
        ShowArguments(newArgc, newArgv);
    }

    /* normally execve does not return... */
    return 42;
}

#endif /* COMPILE_SDBSTARTER_PROGRAM */

extern "C" void RTE_CallSdbStarter(int  &argc,
                                   char *argv[],
                                   SAPDB_Int4 &callerUid,
                                   SAPDB_Int4 &callerGid,
                                   SAPDB_Bool &isAdministrator)
{
    Msg_List                errList;
    tsp00_Pathc             newWorkingDirectory;
    tsp01_RteError          rteError;
    uid_t                   euid;
    gid_t                   egid;


    isAdministrator = true;

    /* PTS 1124659 Installation fails to start x_server from root:sys owned working directory */
    if ( !sqlGetIndependentWrkPath ( newWorkingDirectory,
                                     false,
                                     &rteError ) )
    {
        fprintf(stderr,
                "Failed to get indep data work path:%s [%d:%s]\n",
                rteError.RteErrText, rteError.OsErrCode, rteError.OsErrText);
        exit(78);
    }
 
    if ( 0 != chdir ( &newWorkingDirectory[0] ) )
    {
        fprintf(stderr,
                "Failed to change to indep data work path:%s [%d]\n",
                &newWorkingDirectory[0],
                errno);
        exit(79);
    }

    if ( !RTE_GetDefaultSapdbUserAndGroupIds( sdbUserId,
                                              sdbGroupId,
                                              errList) )
    {
        fprintf(stderr, "Failed to get sdb owner and group id\n");
        RTE_Message(errList, MSG_DIAG_CONSOLE);
        exit(65);
    }

    callerUid = (SAPDB_Int4)RTE_save_getuid();
    callerGid = (SAPDB_Int4)RTE_save_getgid();

    euid = RTE_save_geteuid();
    egid = RTE_save_getegid();

    if ( euid != 0 && (euid != sdbUserId || egid != sdbGroupId))
    {
        fprintf(stderr, "Must be called by root or effective user %d effective group %d\n",
            sdbUserId, sdbGroupId );
        exit(66);
    }

    int iArg;
    if ( argc > 4 && !strcmp(argv[1], "-" SDBSTARTER_PROGRAM) )
    {
        if ( beVerbose )
        {
            fprintf(stderr, "Called by " SDBSTARTER_PROGRAM "\n");
        }

        callerUid = (SAPDB_Int4)strtol(argv[2], 0, 10);
        callerGid = (SAPDB_Int4)strtol(argv[3], 0, 10);
        isAdministrator = ( 'A' == *argv[4] );

        for ( iArg = 5; iArg < argc; iArg++ )
        {
            argv[iArg-4] = argv[iArg];
        }
        argv[iArg-4] = 0;
        argc -= 4;
        return;
    }

    /* Superuser does not have to call 'sdbstarter' and this way bypasses directory checks completely... */
    if ( callerUid == 0 )
    {
        SAPDB_Char OwnernameBuffer[256];
        RTE_IniFileResult  ok;
        tsp00_ErrTextc errtext;


        isAdministrator = true;

        /* root user must become effectiv root user before it can set to 'any' user/group combination! */
        RTE_GetInstallationConfigString( SAPDB_SAPDBOWNER_KEY,
                                         OwnernameBuffer,
                                         (SAPDB_Int4)sizeof(OwnernameBuffer),
                                         errtext,
                                         ok );
        if ( SAPDB_INIFILE_RESULT_OK != ok)
        {
            fprintf(stderr, "Failed to lookup sdb owner user name: %s\n", errtext.asCharp ());
            exit (80);
        }

        RTE_save_setuid(0);
        /* initialize group list */
        initgroups(&OwnernameBuffer[0], egid);
        RTE_save_setgid(egid);
        RTE_save_setuid(euid);

        return;
    }

    /* Berlin development make environ does have severe problems to get rights right ;-) */
    if ( eo01_IsEnvVarDoNotUseIndepPathSet() )
    {
        isAdministrator = RTE_VerifyAsSapdbAdministrator(errList);
        return;
    }

    /* Check for backward compatibility on existing 'starter' directory */
    RTE_Path ownProgramPath;
    RTE_Path tempPath;
    RTE_Path starterDirectoryPath;
    RTE_Path starterProgramPath;

    if ( !GetAbsolutePath(argv[0], ownProgramPath) )
    {
        fprintf(stderr, "No absolute path for: %s\n", argv[0]);
        exit(67);
    }

    /* Copy absolute path and trim filename and '/' */
    strcpy(&tempPath[0], &ownProgramPath[0]);

    int lengthOfDirectoryPath = (int)strlen(&tempPath[0]);

    while ( lengthOfDirectoryPath > 0
         && tempPath[lengthOfDirectoryPath-1] != '/' )
    {
        --lengthOfDirectoryPath;
        tempPath[lengthOfDirectoryPath] = 0;
    }

    if ( lengthOfDirectoryPath <= 0 )
    {
        fprintf(stderr, "No directory found in:%s for %s\n", ownProgramPath, argv[0]);
        exit(68);
    }

    if ( lengthOfDirectoryPath+strlen(SDBSTARTER_DIRECTORY) >= sizeof(RTE_Path) )
    {
        fprintf(stderr, "Path %s" SDBSTARTER_DIRECTORY " too long\n", tempPath);
        exit(69);
    }

    /* add starter directory to path */
    strcat(&tempPath[0], SDBSTARTER_DIRECTORY);

    struct stat statusBuffer;
    if ( RTE_save_stat(&tempPath[0], &statusBuffer) )
    {
        RTE_SystemRc rc;
        rc = RTESys_GetLastError();
        if ( RTE_SYSTEMRC_FILE_NOT_FOUND == rc )
        {
            isAdministrator = RTE_VerifyAsSapdbAdministrator(errList);
            return; /* backward compatibility... dont do anything if SDBSTARTER directory does not exist */
        }
    }

    /* 'starter' directory exists... Now all checks are necessary.... */

    /* calling program must be setuid sdbowner setgid sdbadmin with execute only for owner and group and world */
    if ( !CheckOwnership(&ownProgramPath[0], sdbUserId, sdbGroupId, CALLED_PROGRAM_MODE) )
    {
        fprintf(stderr, "Wrong ownership for own program absolute path\n");
        exit(70);
    }

    if ( !GetAbsolutePath(&tempPath[0], starterDirectoryPath) )
    {
        fprintf(stderr, "No absolute path for start directory: %s\n", starterDirectoryPath);
        exit(71);
    }

    /* Starter directory must allow only read+execute for sdbowner */
    if ( !CheckOwnership(&starterDirectoryPath[0], sdbUserId, sdbGroupId, SDBSTARTER_DIRECTORY_MODE) )
    {
        fprintf(stderr, "Wrong ownership for starter directory absolute path\n");
        exit(72);
    }

    /* Copy absolute path and trim filename and '/' */
    strcpy(&tempPath[0], &starterDirectoryPath[0]);
    if ( (strlen(&tempPath[0])+strlen("/" SDBSTARTER_PROGRAM)) > sizeof(RTE_Path) )
    {
        fprintf(stderr, "Path %s/" SDBSTARTER_PROGRAM " too long\n", tempPath);
        exit(73);
    }

    strcat(&tempPath[0], "/" SDBSTARTER_PROGRAM);

    if ( !GetAbsolutePath(&tempPath[0], starterProgramPath) )
    {
        fprintf(stderr, "No absolute path for start directory: %s\n", starterDirectoryPath);
        exit(74);
    }

    /* Starter program must be setuid root and allow only execute for sdbgroup */
    if ( !CheckOwnership(&starterProgramPath[0], 0, sdbGroupId, SDBSTARTER_PROGRAM_MODE) )
    {
        fprintf(stderr, "Wrong ownership for starter program absolute path\n");
        exit(75);
    }

    errno = 0;
    extern char **environ;
    int newArgc = argc+4;
    if ( beVerbose )
    {
        ++newArgc;
    }
    if ( dontCheckOwnerAndMode )
    {
        ++newArgc;
    }
    char **newArgv = new char *[newArgc+1];
    if ( !newArgv )
    {
        fprintf(stderr, "Allocation of argument vector failed\n");
        exit(76);
    }
    newArgv[0] = saveStrdup(&starterProgramPath[0]);
    int iNewArg = 1;
    if ( beVerbose )
    {
        newArgv[iNewArg++] = saveStrdup("-verbose");
    }
    if ( dontCheckOwnerAndMode )
    {
        newArgv[iNewArg++] = saveStrdup("-nocheck");
    }
    if ( beQuiet )
    {
        newArgv[iNewArg++] = saveStrdup("-quiet");
    }
    newArgv[iNewArg++] = saveStrdup(&ownProgramPath[0]);

    /* copy callerUid and callerGid */
    {
        char idString[16];
        sql47_itoa(callerUid, idString, sizeof(idString));
        newArgv[iNewArg++] = saveStrdup(idString);
        sql47_itoa(callerGid, idString, sizeof(idString));
        newArgv[iNewArg++] = saveStrdup(idString);
        newArgv[iNewArg++] = saveStrdup(isAdministrator ? "A" : "U");
    }

    for ( iArg = 1; iArg < argc; iArg++ )
    {
        newArgv[iNewArg++] = saveStrdup(argv[iArg]);
    }
    newArgv[iNewArg] = 0;

    if ( -1 == RTE_save_execve(newArgv[0], newArgv, environ) )
    {
        fprintf(stderr, "Failed to execve %s: error code %d:%s\n", newArgv[0], errno, strerror(errno));
        ShowArguments(newArgc, newArgv);
    }
    else
    {
        /* if execve did not return -1 errno may be wrong... */
        fprintf(stderr, "Failed to execve %s! errno ?%d?\n", newArgv[0], errno);
        ShowArguments(newArgc, newArgv);
    }

    /* normally execve does not return... */
    exit(77);
}

#ifdef COMPILE_SDBTEST_PROGRAM

void DumpIdsAndGroups()
{
    fprintf(stderr, "real uid %d real gid %d effective uid %d effective group id %d\n",
                RTE_save_getuid(),
                RTE_save_getgid(),
                RTE_save_geteuid(),
                RTE_save_getegid() );
    {
        gid_t groupList[NGROUPS_MAX];

        int iGroup;
        int nGroups = getgroups(NGROUPS_MAX, groupList);

        if ( nGroups > 0 )
        {
            fprintf(stderr, "Member of groups: ");
            for ( iGroup = 0; iGroup < nGroups; iGroup++ )
            {
                fprintf(stderr, "%d ", groupList[iGroup]);
            }
            fprintf(stderr, "\n");
        }
        else
        {
            fprintf(stderr, "Not member in any additional groups\n");
        }
    }
}

/*---------------------------------*/

void StartProtected(int &argc, char **argv)
{
    if ( !beQuiet )
    {
        fprintf(stderr, "Argument vector before RTE_CallSdbStarter\n");
        ShowArguments(argc, &argv[0]);
        DumpIdsAndGroups();
    }

    SAPDB_Int4 callerUid;
    SAPDB_Int4 callerGid;
    SAPDB_Bool isAdministrator;

    /* all around this is output only... */
    RTE_CallSdbStarter(argc, argv, callerUid, callerGid, isAdministrator);
    /* all around this is output only... */

    fprintf(stderr, "Caller Uid %d Caller Gid %d Caller is %s sdb administrator\n",
        callerUid, callerGid, isAdministrator ? "a" : "NO" );
    if ( !beQuiet )
    {
        fprintf(stderr, "Argument vector after RTE_CallSdbStarter\n");
        ShowArguments(argc, &argv[0]);
        DumpIdsAndGroups();
    }
}

/*---------------------------------*/

int main(int argc, char *argv[])
{
    fprintf(stderr, "\n\nStarting sdbstarter test program (should be visible twice...)\n\n");

    if ( argc > 1 )
    {
        if ( !strcmp("-verbose", argv[1]) )
        {
            beVerbose = 1;
        }
        else if ( !strcmp("-nocheck", argv[1]) )
        {
            dontCheckOwnerAndMode = 1;
        }
        else if ( !strcmp("-quiet", argv[1]) )
        {
            beQuiet = 1;
        }
    }

    if ( argc > 2 )
    {
        if ( !strcmp("-verbose", argv[2]) )
        {
            beVerbose = 1;
        }
        else if ( !strcmp("-nocheck", argv[2]) )
        {
            dontCheckOwnerAndMode = 1;
        }
        else if ( !strcmp("-quiet", argv[2]) )
        {
            beQuiet = 1;
        }
    }

    if ( argc > 3 )
    {
        if ( !strcmp("-verbose", argv[3]) )
        {
            beVerbose = 1;
        }
        else if ( !strcmp("-nocheck", argv[3]) )
        {
            dontCheckOwnerAndMode = 1;
        }
        else if ( !strcmp("-quiet", argv[3]) )
        {
            beQuiet = 1;
        }
    }

    StartProtected(argc, argv);

    if ( !beQuiet )
    {
        fprintf(stderr, "Now finally executing sdbstarter test program\n");
    }

    fprintf(stderr, "--------------\n");
    fprintf(stderr, "Hello world...\n");
    fprintf(stderr, "--------------\n");

    return 0;
}

#endif /* COMPILE_SDBTEST_PROGRAM */

#endif /* WIN32/UNIX */
