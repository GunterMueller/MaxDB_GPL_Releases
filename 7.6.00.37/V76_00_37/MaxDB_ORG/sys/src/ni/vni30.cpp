/*!*****************************************************************************
  module:       vni30.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: ni layer
  description:  main-funktion of xbackup and xrestore, analyzing arguments and
                calling dbmcli
  last change:  1999-02-24 14:19
  version:      7.2
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 1999-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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




*******************************************************************************/


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include"hsp100.h"
#include"hni00b.h"
#include"hni32.h"
#include"hni33.h"

#undef global
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>


// -----------------------------------------------------------------------------
// global variables
// -----------------------------------------------------------------------------

char EnvironmentName_DBName[]="DBNAME";
char EnvironmentSymbol_DBName[]=
#if defined(WIN32)
                              "%DBNAME%";
#elif defined(UNIX)
                              "$DBNAME";
#else
    #error xbackup.cpp only coded for NT and UNIX
#endif

char EnvironmentName_InstRoot[]="INSTROOT";
char EnvironmentSymbol_InstRoot[]=
#if defined(WIN32)                            
                               "%INSTROOT%";
#elif defined(UNIX)                           
                               "$INSTROOT";
#else
    #error xbackup.cpp only coded for NT and UNIX
#endif

struct ADbmSrvCommand
{
    char XbackupAction[30];
    char DbmCliAction[30];
    int  MediumRequired;
    char SecondArgument[30];
};

ADbmSrvCommand BackupDbmSrvCommandArray[]=
    {
        {"SAVEDTNCHK", "backup_start", 1, " RECOVERY DATA AUTOIGNORE"},
        {"SAVEDATA"  , "backup_start", 1, " MIGRATION DATA AUTOIGNORE"},
        {"SAVEPGNCHK", "backup_start", 1, " RECOVERY PAGES AUTOIGNORE"},
        {"SAVEPAGES" , "backup_start", 1, " MIGRATION PAGES AUTOIGNORE"},
        {"SAVELOG"   , "backup_start", 1, " LOG"},
        {"AUTOON"    , "autosave_on" , 1, ""},
        {"AUTOOFF"   , "autosave_off", 0, ""},
        {"VERIFY"    , "util_execute", 0, " VERIFY"},
        {""          , ""            , 0, ""}      //dummy, marking end of array
    };

ADbmSrvCommand RestoreDbmSrvCommandArray[]=
    {
        {"SAVEDTNCHK", "recover_start", 1, " DATA"},
        {"SAVEDATA"  , "recover_start", 1, " DATA"},
        {"SAVEPGNCHK", "recover_start", 1, " PAGES"},
        {"SAVEPAGES" , "recover_start", 1, " PAGES"},
        {"SAVELOG"   , "recover_start", 1, " LOG"},
        {"AUTOON"    , "autosave_on"  , 1, ""},
        {"AUTOOFF"   , "autosave_off" , 0, ""},
        {"VERIFY"    , "util_execute" , 0, " VERIFY"},
        {""          , ""             , 0, ""}      //dummy, marking end of array
    };

// declare parameters for xbackup
ni32_DoubleParameter ParamDBName("-d", ni32_AParameter::Optional, EnvironmentName_DBName);
ni32_DoubleParameter ParamInstRoot("-r", ni32_AParameter::Optional, EnvironmentName_InstRoot);
ni32_RestrictedDoubleParameter ParamAction("-a",
                                           ni32_RestrictedDoubleParameter::TempValueList<<"VERIFY"
                                                                                        <<"AUTOOFF"
                                                                                        <<"AUTOON"
                                                                                        <<"SAVELOG"
                                                                                        <<"SAVEPAGES"
                                                                                        <<"SAVEPGNCHK"
                                                                                        <<"SAVEDATA"
                                                                                        <<"SAVEDTNCHK" /* <- last is the default value */
                                                                                        );
ni32_DoubleParameter ParamMediumName("-m");
ni32_SingleParameter ParamVerbose("-v");
ni32_SingleParameter ParamVersion("-V");
ni32_SingleParameter ParamHelp("-h");
ni32_DoubleParameter ParamF("-f");

class XbackupParameters: public ni32_ProgramParameters
{
    ADbmSrvCommand * DbmSrvCommand[2];
    char             CurrentProgramName[20];
    int              IsBackup;
  public:
    XbackupParameters(int argc,const char * const * argv)
        :ni32_ProgramParameters(argc,argv,ni32_ProgramParameters::TempParamList<<&ParamDBName
                                                                               <<&ParamInstRoot
                                                                               <<&ParamAction
                                                                               <<&ParamMediumName
                                                                               <<&ParamVerbose
                                                                               <<&ParamVersion
                                                                               <<&ParamHelp
                                                                               <<&ParamF)
    {
        DbmSrvCommand[0]=RestoreDbmSrvCommandArray;
        DbmSrvCommand[1]=BackupDbmSrvCommandArray;

        #if defined(WIN32)
        const char * BaseName=strrchr(argv[0], '\\');
        const char * AllowedBackupName="backup.exe";    /* PTS 1108841 */
        const char * AllowedRestoreName="restore.exe";  /* PTS 1108841 */
        #elif defined(UNIX)
        const char * BaseName=strrchr(argv[0], '/');
        const char * AllowedBackupName="backup";        /* PTS 1108841 */
        const char * AllowedRestoreName="restore";      /* PTS 1108841 */
        #else
            #error XbackupParameters::XbackupParameters() only coded for WIN32 and UNIX, define one of them
        #endif

        if(0==BaseName)
            BaseName=argv[0];
        else
            BaseName++;

        strncpy(CurrentProgramName, BaseName, 17);
        CurrentProgramName[16]='\0';
        char * h=CurrentProgramName;

        while('\0'!= *h)
        {
            *h=tolower(*h);
            h++;
        }

        #if defined(WIN32)
        if(0==strchr(CurrentProgramName, '.'))
            strcat(CurrentProgramName, ".exe");
        #elif defined(UNIX)
        #else
            #error XbackupParameters::XbackupParameters() only coded for WIN32 and UNIX, define one of them
        #endif

        if(!strcmp(CurrentProgramName, AllowedRestoreName))
            IsBackup=0;
        else
            if(!strcmp(CurrentProgramName, AllowedBackupName))
                IsBackup=1;
            else
            {
                printf("Unknown program name '%s'. Please use program as %s or %s.\n",
                       (const char *)BaseName,
                       (const char *)AllowedBackupName,
                       (const char *)AllowedRestoreName);

                IsBackup=-1;
            }
    }

    virtual int  UsageMessageAvailable(){return 1;}
    const char * UsageMessage()
    {
        static int AllreadyDisplayed=0;
        static char Usage[1024]="\nusage: ";
            
        strcat(Usage,CurrentProgramName);
        strcat(Usage," <options>\n"
                     "\n"
                     "Options are:\n");
        if(IsBackup)
            strcat(Usage, "    -d <dbname>     Name of the database to be saved (default: ");
        else
            strcat(Usage, "    -d <dbname>     Name of the database to be restored/built up (default:\n"
                          "                    ");
                 
        strcat(Usage,EnvironmentSymbol_DBName);
        strcat(Usage,")\n"
                     "    -a <action>     Action to be performed: SAVEDTNCHK (default), SAVEDATA,\n"
                     "                    SAVEPGNCHK, SAVEPAGES, SAVELOG, AUTOON, AUTOOFF, VERIFY.\n"
                     "    -m <mediumname> Name of the medium\n"
                     "    -v              Verbose mode for display of more progress messages\n"
                     "    -h              Help display\n"
                     "    -V              Display of the version information\n"
                     "\n"
                     "    -r <instroot>   Obsolete parameter, supplied for downward compatibility only\n"
                     "    -f <pathname>   Obsolete parameter, supplied for downward compatibility only\n");

        if(0==AllreadyDisplayed++)
            return Usage;
        else
            return "";
    }
    void         OnError(){exit(1);}
    int          ExtendedParameterCheck()
    {
        if(-1==IsBackup)  // unknown name of the program -> exit
            OnError();

        int OtherThenHVVSet=0,
            HVSet=0;
        const ni31_ListElem<ni32_AParameter *> *h=GiveParameterList().First();

        if(ParamHelp.IsSet() || ParamVersion.IsSet())
            HVSet=1;

        while(0!=h)
        {
            if(1==h->GiveInfo()->IsSet()) //parameter is set on command-line, not through environment
            {
                if(&ParamHelp!=(ni32_AParameter *)(*h) && &ParamVersion!=(ni32_AParameter *)(*h) && &ParamVerbose!=(ni32_AParameter *)(*h))
                    OtherThenHVVSet=1;
            }
            h=h->Next();
        }

        if(ParamVersion.IsSet())
        {
            tsp00_Versionc VersionString;
            sp100_GetVersionString("xbackup",s100buildnumber,VersionString);
            printf("%s\n", (const char *)VersionString);
        }

        if(ParamHelp.IsSet())
            printf("%s\n", (const char *)UsageMessage());
        
        if(!OtherThenHVVSet && HVSet)
            exit(0);

        if((!OtherThenHVVSet && !HVSet)                  // no parameter given
           || (OtherThenHVVSet && !ParamDBName.IsSet())  // not only -V or -h or -v is set, in this case dbname is required
          )
        {
            printf("%s\n", (const char *)UsageMessage());
            OnError();
        }

        // get the dbmsrv-command to use for desired xbackup action
        while('\0'!=DbmSrvCommand[IsBackup]->XbackupAction[0]
              && strcmp(DbmSrvCommand[IsBackup]->XbackupAction, ParamAction.GiveValue()))
        {
            DbmSrvCommand[IsBackup]++;
        }

        if('\0'==DbmSrvCommand[IsBackup]->XbackupAction[0])
        {
            printf("Internal Error.\n");
            OnError();
        }
        else
        {
            // check if medium is required for the desired action
            if(DbmSrvCommand[IsBackup]->MediumRequired && !ParamMediumName.IsSet())
            {
                printf("Medium required for action %s.\n", (const char *)DbmSrvCommand[IsBackup]->XbackupAction);
                OnError();
            }
        }

        return 1;
    }

    const ADbmSrvCommand * GiveDbmSrvCommand(){return DbmSrvCommand[IsBackup];}
};

#define MAX_SIZE_OF_COMMANDLINE 2048
char DbmCliCommandLine[MAX_SIZE_OF_COMMANDLINE];

// -----------------------------------------------------------------------------
// PRIVATE FUNCTION main()
// purpose: contains main control-flow-instructions for xbackup 
// -----------------------------------------------------------------------------

int main(int argc,const char ** argv)
{
    // check arguments from current call against declared parameters
    XbackupParameters Parameters(argc, argv);
    Parameters.Check();

    // construct the dbmcli command
    //char DbmCliCommandLine[4096];
    int ReturnCode=1;

    sprintf(DbmCliCommandLine,
            "dbmcli -U c -uUTL -d %s %s%s%s%s",
            ParamDBName.GiveValue(),
            Parameters.GiveDbmSrvCommand()->DbmCliAction,
            Parameters.GiveDbmSrvCommand()->MediumRequired ? " ":"",
            Parameters.GiveDbmSrvCommand()->MediumRequired ? ParamMediumName.GiveValue() : "",
            Parameters.GiveDbmSrvCommand()->SecondArgument);

    if(MAX_SIZE_OF_COMMANDLINE<strlen(DbmCliCommandLine)+1)
        printf("Internal error.\n");
    else
    {
        // start dbmcli, give status-messages if desired
        if(ParamVerbose.IsSet())
            printf("Calling:'%s'\n", (const char *)DbmCliCommandLine);

        tni33_AProcess DbmCli(DbmCliCommandLine);
        
        if(DbmCli.WasStarted())
        {
            ReturnCode=DbmCli.GiveReturnCode(); // exit with same status as dbmcli
            if(ParamVerbose.IsSet())
                printf("Exit-code of dbmcli : %d\n", (int)ReturnCode);
        }
        else
        {
            ReturnCode=9;
            if(ParamVerbose.IsSet())
                printf("Could not start dbmcli, returning with exit-code %d.\n", (int)ReturnCode);
        }
    }

    return ReturnCode;
}
