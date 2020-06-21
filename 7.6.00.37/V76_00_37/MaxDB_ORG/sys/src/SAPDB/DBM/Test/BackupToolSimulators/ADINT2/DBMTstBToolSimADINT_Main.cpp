/*!
    \file    DBMTstBToolSimADINT_Main.cpp
    \author  TiloH
    \ingroup ADINT2 simulator for tests
    \brief   implementing the main function for the simulator program

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2006 SAP AG

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

#include "hcn36.h"
#include "hni33.h"
#include "ToolsCommon/Parsers/ToolsParsers_ProgramParameterParser.hpp"

int main(int argc, char ** argv)
{
    bool         rc=true;
    const char * syntax="(-b|-r) -n <name> [-o <outfile>] [-c <blocksize>] -p <pipename> |"
                        "(-q|-d) -n <name> [-o <outfile>]";

    printf("\n");
    printf("+-----------------------------------------------------------------------------+\n");
    printf("|                                                                             |\n");
    printf("|           TiloH's ADINT2 Simulator, Copyright (c) 2004-2006 SAP AG          |\n");
    printf("|                                                                             |\n");
    printf("+-----------------------------------------------------------------------------+\n");
    printf("TDI0074X: The license is valid until >Thu Jun  1 01:00:00 2006<.\n");

    printf("\nGot the following arguments:");
    for(int i=0; i<argc; i++)
        printf(" %s", argv[i]);
    printf("\n");

    ToolsParsers_ProgramParameterParser parser(argc, argv, syntax);

    const ToolsParsers_ParseResult & parseResult=parser.parse();

    if(ToolsParsers_ParseResult::NoError!=parseResult.getResultCode())
    {
        if(parseResult.foundSyntaxError())
            printf("adint2 %s\n", syntax);
        else
            printf("Could not parse parameters\n");

       rc=false;
    }
    else
    {
        char       * dataDir=0;
        const char * adaopt=cn36_GetEnv("ADA_OPT");

        cn36_StrAllocCpy(dataDir, 0==adaopt?"":adaopt);

        if('\0'!=dataDir[0])
            *(cn36_BaseNameFrom(dataDir))='\0'; // dataDir is now a path including a separator or empty

        if(parseResult.getKeyword("-b").isSet())
        {
            if(!parseResult.getVariable("<pipename>").isSet())
                rc=false;
            else
            {
                char cmdLinePipe2File[2048];

                sprintf(
                    cmdLinePipe2File,
                    "pipe2file -d pipe2file -nowait -p %s -f %sBackupADSM_%s",
                    parseResult.getVariable("<pipename>").getValue(),
                    dataDir,
                    parseResult.getVariable("<name>").getValue());

                tni33_AProcess pipe2file(cmdLinePipe2File);

                if(!pipe2file.WasStarted())
                    printf("Could not start '%s'.\n", cmdLinePipe2File);
                else
                    printf("'%s' ended with return code %d.\n", cmdLinePipe2File, (int)pipe2file.GiveReturnCode());

                /* Read from "stdin": */
                char buffer[1024];
                int i=0;
                int character;

                do
                {
                    character=getchar();

                    if(EOF!=character && '\n'!=character)
                        buffer[i++]=(char)character;
                }
                while(i<1024 && EOF!=character && '\n'!=character);

                buffer[i]='\0';

                printf("adint2: Got a '%s'\n", buffer);

                if(0!=cn36_StrUprCmp(buffer, "COMMIT "))
                    rc=false;
            }
        }

        if(parseResult.getKeyword("-q").isSet())
        {
            tni34_ADirectory dir('\0'==dataDir[0]?".":dataDir);
            const char * nextEntry=0;
            const char * backupNamePart=parseResult.getVariable("<name>").getValue();
            size_t       ignoreLength=0;

            if(0<strlen(backupNamePart) && backupNamePart[strlen(backupNamePart)-1]=='*') //ignore any suffixed '*'
                ignoreLength=1;

            printf("\n");
            printf("Query available backups on TSM server >BERD3000116311A<:\n");
            printf("Savename                                                        [Sessions used]\n");
            printf("===============================================================================\n");
            printf("");


            if(dir.Open())
            {
                int i=0;

			    while(dir.Read(nextEntry) && 0!=nextEntry)
                {
                    if(0==strncmp("BackupADSM_", nextEntry, strlen("BackupADSM_")) &&
                       0<strlen(nextEntry+strlen("BackupADSM_")) &&
                       0==strncmp(backupNamePart, nextEntry+strlen("BackupADSM_"), strlen(backupNamePart)-ignoreLength) &&
                       0<strlen(nextEntry+strlen("BackupADSM_")+strlen(backupNamePart)-ignoreLength))
                    {
                        int sizeEntry=strlen(nextEntry+strlen("BackupADSM_"));

                        i++;
                        //savename
			            printf(nextEntry+strlen("BackupADSM_"));

                        //sessions used
                        if(sizeEntry<70)
                        {
                            for(int j=70-sizeEntry; j>0; j--)
                                printf(" ");
                        }
                        else
                            printf(" ");

                        printf("[1]\n");
                    }
                }

                if(0==i)
                    printf("Could not find any matching files in directory '%s'\n", dir.GiveName());
            }
            else
                printf("Could not open directory '%s'.\n", dir.GiveName());
        }

        if(parseResult.getKeyword("-r").isSet())
        {
            char                       cmdLinePipe2File[2048];
            tni33_ABackgroundProcess * pipe2FileProcs [100];
            int                        numberOfRunningPipe2Files=0;
            int                        i;

            for(i=0; i<1; i++)
                pipe2FileProcs[i]=0;

			for(i=0; i<1; i++)
			{
                sprintf(
                    cmdLinePipe2File,
                    "pipe2file -d file2pipe -nowait -p %s -f %sBackupADSM_%s",
                    parseResult.getVariable("<pipename>").getValue(),
                    dataDir,
                    parseResult.getVariable("<name>").getValue());

                pipe2FileProcs[i]=new tni33_ABackgroundProcess(cmdLinePipe2File);

                if(pipe2FileProcs[i]->WasStarted())
                    numberOfRunningPipe2Files++;
			}

            while(0<numberOfRunningPipe2Files)
            {
                cn36_Sleep(3);

                numberOfRunningPipe2Files=0;

                for(i=0; i<1; i++)
                {
                    if(pipe2FileProcs[i]->IsRunning())
                        numberOfRunningPipe2Files++;
                }
            }

            for(i=0; i<1; i++)
            {
                if(0!=pipe2FileProcs[i]->GiveReturnCode())
                    rc=false;
            }
        }

        if(parseResult.getKeyword("-d").isSet())
        {
            tni34_ADirectory dir('\0'==dataDir[0]?".":dataDir);
            const char * nextEntry=0;
            const char * backupNamePart=parseResult.getVariable("<name>").getValue();
            size_t       ignoreLength=0;

            if(0<strlen(backupNamePart) && backupNamePart[strlen(backupNamePart)-1]=='*') //ignore any suffixed '*'
                ignoreLength=1;

            if(dir.Open())
            {
                int i=0;

			    while(dir.Read(nextEntry) && 0!=nextEntry)
                {
                    if(0==strncmp("BackupADSM_", nextEntry, strlen("BackupADSM_")) &&
                       0<strlen(nextEntry+strlen("BackupADSM_")) &&
                       0==cn36_StrNUprCmp(backupNamePart, nextEntry+strlen("BackupADSM_"), strlen(backupNamePart)-ignoreLength) &&
                       0<strlen(nextEntry+strlen("BackupADSM_")+strlen(backupNamePart)-ignoreLength))
                    {
                        char * absoluteFileName=0;

                        cn36_StrAlloc(absoluteFileName, strlen(dataDir)+strlen(nextEntry));
                        sprintf(absoluteFileName, "%s%s", dataDir, nextEntry);

                        tni34_AFile file(absoluteFileName, tni34_AFile::FromDisk, 0, 0);

                        if(file.IsAFile())
                        {
                            i++;
                            file.Remove();
                        }

                        cn36_StrDealloc(absoluteFileName);
                    }
                }

                if(0==i)
                    printf("Could not find any matching files in directory '%s'\n", dir.GiveName());
            }
            else
                printf("Could not open directory '%s'.\n", dir.GiveName());
        }

        cn36_StrDealloc(dataDir);
    }

    if(rc)
        printf("\nTDI00074: ADINT2 Simulator has ended successful.\n\n");

    return rc?0:2;
}
