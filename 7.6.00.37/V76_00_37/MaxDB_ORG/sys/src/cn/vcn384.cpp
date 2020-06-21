/*!
    \file    vcn384.cpp
    \author  TiloH
    \ingroup cn layer
    \brief   implementation of Backint for MaxDB functions for handling the
             pipe2file converter programs

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2006 SAP AG

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

#include "SAPDBCommon/SAPDB_string.h"

#include "hcn36.h"
#include "hcn384.h"
#include "hcn385.h"

static const int MaxCmdLineLength_cn384=2048;

int cn384_StartPipeToFileConverters(int                            & StartedProcesses,
                                    ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                    tcn381_APipeToFileConverter      Converter[MAX_STAGING_AREAS_CN38],
                                    const char * const               StagingAreaName[MAX_STAGING_AREAS_CN38],
                                    const teo00_Uint8                StagingAreaSize[MAX_STAGING_AREAS_CN38],
                                    tcn386_BackintOut              & Output)
{
    int rc=1;
    ni31_ListElem<tcn381_InputEntry *> *h=TheInputFileList.First();
    char CmdLine[MaxCmdLineLength_cn384+1];

    Output.Write("Starting pipe2file program(s).");

    for(StartedProcesses=0; rc && h!=0; h=h->Next())
    {
        if(h->GiveInfo()->IsPipe)
        {
            Converter[StartedProcesses].InPipe=new tni34_APipe(tni34_APipe::Out, 1, 0);
            Converter[StartedProcesses].OutPipe=new tni34_APipe(tni34_APipe::In, 1, 0);
            Converter[StartedProcesses].CompleteStageName=new char [strlen(StagingAreaName[StartedProcesses])+21+1];

            h->GiveInfo()->StageName=new char [strlen(StagingAreaName[StartedProcesses])+1];
            
            if(0==Converter[StartedProcesses].InPipe || !Converter[StartedProcesses].InPipe->IsGood() ||
               0==Converter[StartedProcesses].OutPipe || !Converter[StartedProcesses].OutPipe->IsGood() ||
               0==Converter[StartedProcesses].CompleteStageName ||
               0==h->GiveInfo()->StageName)
            {
                rc=0;
                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));

                if(0!=Converter[StartedProcesses].InPipe)
                {
                    delete Converter[StartedProcesses].InPipe;
                    Converter[StartedProcesses].InPipe=0;
                }

                if(0!=Converter[StartedProcesses].OutPipe)
                {
                    delete Converter[StartedProcesses].OutPipe;
                    Converter[StartedProcesses].OutPipe=0;
                }

                if(0!=Converter[StartedProcesses].CompleteStageName)
                {
                    delete [] Converter[StartedProcesses].CompleteStageName;
                    Converter[StartedProcesses].CompleteStageName=0;
                }
                
                if(0!=h->GiveInfo()->StageName)
                {
                    delete [] h->GiveInfo()->StageName;
                    h->GiveInfo()->StageName=0;
                }
            }
            else
            {
                char * CurrentPosition=CmdLine;

                SAPDB_strcpy(Converter[StartedProcesses].CompleteStageName, StagingAreaName[StartedProcesses]);
                strcat(Converter[StartedProcesses].CompleteStageName, ".");

                Converter[StartedProcesses].EndOfStageName=Converter[StartedProcesses].CompleteStageName+strlen(Converter[StartedProcesses].CompleteStageName);
                sprintf(Converter[StartedProcesses].EndOfStageName, "%d", int(h->GiveInfo()->NumberOfParts));
                
                SAPDB_strcpy(CmdLine, "pipe2file -d pipe2file -f ");
                strcat(CmdLine, StagingAreaName[StartedProcesses]);
                strcat(CmdLine, " -p ");
                strcat(CmdLine, h->GiveInfo()->Name);
                strcat(CmdLine, " -s ");

                CurrentPosition=cn36_TerminatingZeroOf(CmdLine);

                if(!cn36_UInt8ToStrN(CurrentPosition, MaxCmdLineLength_cn384-(CurrentPosition-CmdLine), StagingAreaSize[StartedProcesses]))
                {
                    rc=0;
                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
                }
                else
                {
                    Converter[StartedProcesses].Process=new tni33_ABackgroundProcess(CmdLine, Converter[StartedProcesses].InPipe, Converter[StartedProcesses].OutPipe);
                
                    if(0==Converter[StartedProcesses].Process)
                    {
                        rc=0;
                        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
                    }
                    else
                        if(!Converter[StartedProcesses].Process->WasStarted())
                            rc=0;
                
                    if(0==rc)
                    {
                        delete Converter[StartedProcesses].InPipe;
                        Converter[StartedProcesses].InPipe=0;

                        delete Converter[StartedProcesses].OutPipe;
                        Converter[StartedProcesses].OutPipe=0;

                        delete [] Converter[StartedProcesses].CompleteStageName;
                        Converter[StartedProcesses].CompleteStageName=0;
                    }
                    else
                    {
                        Converter[StartedProcesses].WasRunning=1;
                        Converter[StartedProcesses].InputInfo=h->GiveInfo();

                        h->GiveInfo()->Converter=&(Converter[StartedProcesses]);
                        SAPDB_strcpy(h->GiveInfo()->StageName, StagingAreaName[StartedProcesses]);
                        h->GiveInfo()->StageSize=StagingAreaSize[StartedProcesses];

                        Converter[StartedProcesses].State=tcn381_APipeToFileConverter::ConverterIsWorking;

                        if(!Converter[StartedProcesses].InPipe->Open())
                            rc=0;

                        if(!Converter[StartedProcesses].OutPipe->Open())
                            rc=0;
                        
                        StartedProcesses++;
                    }
                }
            }
        }
    }

    Output.Write("");

    return rc;
}

int cn384_StartNeededPipeToFileConverters(int                            & StartedProcesses,
                                          ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                          tcn381_APipeToFileConverter      Converter[MAX_STAGING_AREAS_CN38],
                                          const char * const               StagingAreaName[MAX_STAGING_AREAS_CN38],
                                          tcn386_BackintOut              & Output)
{
    int rc=1;
    ni31_ListElem<tcn381_InputEntry *> *h;
    tcn381_InputEntry *IE;
    char CmdLine[MaxCmdLineLength_cn384+1];

    int FirstRealEntry=1;

    for(h=TheInputFileList.First(); rc && 0!=h; h=h->Next())
    {
        IE=h->GiveInfo();

        if(IE->IsPipe &&
           IE->FoundInThisBackintOutput &&
           0==IE->Converter->Process)
        {
            if(FirstRealEntry)
            {
                Output.Write("Starting pipe2file program(s).");
                FirstRealEntry=0;
            }

            IE->Converter->InPipe=new tni34_APipe(tni34_APipe::Out, 1, 0);
            IE->Converter->OutPipe=new tni34_APipe(tni34_APipe::In, 1, 0);
            
            if(0==IE->Converter->InPipe || !IE->Converter->InPipe->IsGood() ||
               0==IE->Converter->OutPipe || !IE->Converter->OutPipe->IsGood())
            {
                rc=0;
                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
            }
            else
            {   
                char * EndOfCmdLine;

                SAPDB_strcpy(CmdLine, "pipe2file -d file2pipe -f ");

                if(0==IE->RestoreStageName)
                    strcat(CmdLine, IE->StageName);
                else
                    strcat(CmdLine, IE->RestoreStageName);

                strcat(CmdLine, " -p ");

                if(0==IE->DestinationName)
                    strcat(CmdLine, IE->Name);
                else
                    strcat(CmdLine, IE->DestinationName);
                
                strcat(CmdLine, " -s ");

                EndOfCmdLine=cn36_TerminatingZeroOf(CmdLine);
                if(!cn36_UInt8ToStrN(EndOfCmdLine, MaxCmdLineLength_cn384-(EndOfCmdLine-CmdLine), IE->StageSize))
                {
                    rc=0;
                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
                }
                else
                {

                    IE->Converter->Process=new tni33_ABackgroundProcess(CmdLine, IE->Converter->InPipe, IE->Converter->OutPipe);
                
                    if(0==IE->Converter->Process)
                    {
                        rc=0;
                        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
                    }
                    else
                        if(!IE->Converter->Process->WasStarted())
                            rc=0;

                    if(0==rc)
                    {
                        delete IE->Converter->InPipe;
                        IE->Converter->InPipe=0;

                        delete IE->Converter->OutPipe;
                        IE->Converter->OutPipe=0;

                        if(0!=IE->Converter->Process)
                        {
                            delete IE->Converter->Process;
                            IE->Converter->Process=0;
                        }
                    }
                    else
                    {
                        IE->Converter->WasRunning=1;
                        IE->Converter->InputInfo=h->GiveInfo();

                        if(!IE->Converter->InPipe->Open())
                            rc=0;

                        if(!IE->Converter->OutPipe->Open())
                            rc=0;

                        StartedProcesses++;

                        IE->Converter->State=tcn381_APipeToFileConverter::ConverterIsWorking;
                    }
                }
            }
        }
    }

    if(0==FirstRealEntry)
        Output.Write("");

    return rc;
}

static int cn384_CheckConverterForNewAnswer(tcn381_APipeToFileConverter & Converter,
                                            int                         & NewAnswer,
                                            int                         & FinishedNow)
{
    int rc=1;

    NewAnswer=0;
    FinishedNow=0;

    if(0!=Converter.Process) // can be 0 at the beginning of a restore
        if(Converter.WasRunning)
            if(Converter.Process->IsRunning())
            {
                if(tcn381_APipeToFileConverter::ConverterIsWorking==Converter.State)
                    if(Converter.OutPipe->IsDataAvailable())
                    {
                        char answer;
                        long BytesRead;
                        int  ReadRC=Converter.OutPipe->Read(&answer, 1, BytesRead);

                        if(!ReadRC || BytesRead!=1)
                        {
                            if(-1!=ReadRC || 0!=BytesRead)  //set not rc=0, if at End of Pipe (-1==ReadRC && 0==BytesRead)
                                rc=0;
                        }
                        else
                            if('d'==answer)
                            {
                                Converter.State=tcn381_APipeToFileConverter::ConverterFinishedStage;
                                NewAnswer=1;
                            }
                    }
            }
            else
            {
                Converter.WasRunning=0;
                if(0!=Converter.Process->GiveReturnCode())
                {
                    rc=0;
                    Converter.InputInfo->ProcessedSuccessful=0;
                }
                FinishedNow=1;
            }

    return rc;
}

int cn384_WaitForAnswersFromPipeToFileConverters(int                         & NumberOfAnswersAvailable,
                                                 int                         & NumberOfRunningProcesses,
                                                 int                         & NumberOfFilesPerBackintCall,
                                                 int                         & NumberOfFilesOfNextBackintCall,
                                                 int                           StartedProcesses,
                                                 tcn381_APipeToFileConverter   Converter[MAX_STAGING_AREAS_CN38],
                                                 unsigned                      MaxDelayOfBackintCall,
                                                 tcn386_BackintOut           & Output)
{
    int rc=1,
        i,
        NewAnswerFound,
        FinishedNow;

    unsigned CurrentDelay=0;

    Output.Write("Waiting for creation of temporary files.");

    if(NumberOfAnswersAvailable<NumberOfFilesPerBackintCall) // if we have enough answer use these -> avoid use of one single converter
    {
        do
        {
            // check the converters for existence and new answers
            for(i=0; rc && i<StartedProcesses; i++)
            {
                FinishedNow=NewAnswerFound=0;

                rc=cn384_CheckConverterForNewAnswer(Converter[i], NewAnswerFound, FinishedNow);
            
                if(NewAnswerFound)
                    NumberOfAnswersAvailable++;

                if(FinishedNow)
                    NumberOfRunningProcesses--;
            }

            //can not get more answers than running converters
            if(NumberOfFilesPerBackintCall>NumberOfRunningProcesses)
                NumberOfFilesPerBackintCall=NumberOfRunningProcesses;

            if(rc &&                                                                // sleep only, if all is ok and
               NumberOfAnswersAvailable<NumberOfFilesPerBackintCall &&              //             if not enough answers and
               0<NumberOfRunningProcesses &&                                        //             if something is still running and
               (0>=NumberOfAnswersAvailable || CurrentDelay<MaxDelayOfBackintCall)) //             if not waited to long for more than one answer
            {
                cn36_Sleep(1);        // sleep one second
                
                if(0<NumberOfAnswersAvailable)  // if we have already the first answer:
                    CurrentDelay++;             // count that second
            }
        }
        while(rc &&                                                                 // all ok and
              NumberOfAnswersAvailable<NumberOfFilesPerBackintCall &&               // not enough answers and
              0<NumberOfRunningProcesses &&                                         // there can be a new answer and
              (0>=NumberOfAnswersAvailable || CurrentDelay<MaxDelayOfBackintCall)); // (no answer available or delay after first answer is not to big)
    }

    NumberOfFilesOfNextBackintCall=NumberOfAnswersAvailable;        // every answer corresponds to one available stage file, use them in next backint call

    if(NumberOfFilesOfNextBackintCall>NumberOfFilesPerBackintCall)  // but, do not use more then NumberOfFilesPerBackintCall
        NumberOfFilesOfNextBackintCall=NumberOfFilesPerBackintCall; // stage files at once

    if(rc)
    {
        char Buf[30];

        if(cn36_IntToStrN(Buf, sizeof(Buf), NumberOfAnswersAvailable))
            Output.Write(Buf, " temporary file", 1==NumberOfAnswersAvailable?" is":"s are", " available for backup.");
    }

    return rc;
}

int cn384_WaitForAnswersFromPipeToFileConverters(int &                            NumberOfAnswersAvailable,
                                                 int                              TotalyNeededConverters,
                                                 int                              StartedConverters,
                                                 int &                            ConvertersFinishedWork,
                                                 int &                            EndedConverters,
                                                 int &                            MaxNumberOfFilesPerBackintCall,
                                                 int &                            NumberOfFilesOfNextBackintCall,
                                                 ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                                 unsigned                         MaxDelayOfBackintCall,
                                                 tcn386_BackintOut              & Output)
{
    int rc=1;

    int NewAnswerFound,
        FinishedNow;
    
    ni31_ListElem<tcn381_InputEntry *> * h;

    unsigned CurrentDelay=0;
    unsigned Pipe2FilesFinishedNow=0;
    unsigned Pipe2FilesAfterLastFile=0;

    Output.Write("Checking pipe2file program(s).");

    if(NumberOfAnswersAvailable<MaxNumberOfFilesPerBackintCall || 0==MaxNumberOfFilesPerBackintCall)
    {
        do
        {
            for(h=TheInputFileList.First(); rc && 0!=h; h=h->Next())
                if(h->GiveInfo()->IsPipe)
                {
                    tcn381_APipeToFileConverter *Converter=h->GiveInfo()->Converter;

                    FinishedNow=NewAnswerFound=0;

                    rc=cn384_CheckConverterForNewAnswer(*Converter, NewAnswerFound, FinishedNow);

                    if(NewAnswerFound)
                    {
                        if(1+Converter->InputInfo->RestorePart==Converter->InputInfo->NumberOfParts)    // if the converter has answered for the last part
                        {
                            ConvertersFinishedWork++;                                                   // the converter has done its work
                            Pipe2FilesAfterLastFile++;
                        }
                        else
                            NumberOfAnswersAvailable++;                                                 // otherwise the converter is available for his next parts
                    }

                    if(FinishedNow)
                    {
                        Pipe2FilesFinishedNow++;
                        EndedConverters++;

                        if(1+Converter->InputInfo->RestorePart<Converter->InputInfo->NumberOfParts)
                            ConvertersFinishedWork++;
                    }
                }

            int MaximalAvailableConverters=TotalyNeededConverters-ConvertersFinishedWork;

            if(MaxNumberOfFilesPerBackintCall>MaximalAvailableConverters)
                MaxNumberOfFilesPerBackintCall=MaximalAvailableConverters;

            if(rc &&
               NumberOfAnswersAvailable<MaxNumberOfFilesPerBackintCall &&
               (0>=NumberOfAnswersAvailable || CurrentDelay<MaxDelayOfBackintCall))
            {
                cn36_Sleep(1);

                if(0<NumberOfAnswersAvailable)  // if we have already the first answer:
                    CurrentDelay++;             // count that second
            }
            else
                if(0==MaximalAvailableConverters && EndedConverters<TotalyNeededConverters)
                    cn36_Sleep(1);    // sleep also, if only waiting for end of finished pipe2file converter processes
        }
        while(rc &&
              (NumberOfAnswersAvailable<MaxNumberOfFilesPerBackintCall) &&
              (0>=NumberOfAnswersAvailable || CurrentDelay<MaxDelayOfBackintCall));
    }

    NumberOfFilesOfNextBackintCall=NumberOfAnswersAvailable;        // every answer corresponds to one available stage file, use them in next backint call

    if(NumberOfFilesOfNextBackintCall>MaxNumberOfFilesPerBackintCall)  // but, do not use more then NumberOfFilesPerBackintCall
        NumberOfFilesOfNextBackintCall=MaxNumberOfFilesPerBackintCall; // stage files at once

    if(rc)
    {
        char Buf[30];

        if(0<NumberOfAnswersAvailable && cn36_IntToStrN(Buf, sizeof(Buf), NumberOfAnswersAvailable))
            Output.Write("Transfer of ", Buf, " temporary file", 1==NumberOfAnswersAvailable?"":"s", " has ended.");

        if(0<Pipe2FilesAfterLastFile && cn36_IntToStrN(Buf, sizeof(Buf), Pipe2FilesAfterLastFile))
            Output.Write(Buf, " pipe2file program", 1==Pipe2FilesAfterLastFile?" has":"s have", " transfered last part of a pipe.");

        if(0<Pipe2FilesFinishedNow && cn36_IntToStrN(Buf, sizeof(Buf), Pipe2FilesFinishedNow))
            Output.Write(Buf, " pipe2file program", 1==Pipe2FilesFinishedNow?" has":"s have", " finished work.");
    }

    return rc;
}

int cn384_RemoveFilesAndTellConverters(int                         StartedProcesses,
                                       tcn381_APipeToFileConverter Converter[MAX_STAGING_AREAS_CN38])
{
    //this function must be used only in backup mode 
    int rc=1;
    int i;

    for(i=0; rc && i<StartedProcesses; i++)
    {
        if(tcn381_APipeToFileConverter::BackintIsWorking==Converter[i].State)
        {
            tni34_AVirtualFile Stage(Converter[i].CompleteStageName,
                                     tni34_AVirtualFile::ToDiskAppend,
                                     tni34_AVirtualFile::DoNotOpenNow);

            if(Stage.IsGood())
            {
                if(Stage.Exists())
                {
                    teo00_Uint8 StageSize;

                    if(Stage.GetSize(StageSize))
                    {
                        if(MAX_UINT8_CN36-StageSize>=Converter[i].InputInfo->BytesWrittenToDisk)
                            Converter[i].InputInfo->BytesWrittenToDisk+=StageSize;
                        else
                        {
                            rc=0;
                            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::TooManyDataTroughPipe));
                        }

                        if(!Stage.Remove())
                            rc=0;

                        if(rc)  // could determine size of stage file and remove the stage file
                        {
                            Converter[i].State=tcn381_APipeToFileConverter::StageIsFree;

                            long BytesWritten;
                            char Command[10];
                            
                            sprintf(Command, "n\n");

                            if(!Converter[i].InPipe->Write(Command, (int) strlen(Command), BytesWritten)) // tell converter program to go on
                                rc=0;
                            else
                                Converter[i].State=tcn381_APipeToFileConverter::ConverterIsWorking;
                        }
                    }
                    else
                        rc=0;
                }
                else
                {
                    rc=0;
                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::StageIsNotExisting));
                }
            }
            else
            {
                rc=0;
                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
            }
        }
    }

    return rc;
}

int cn384_TellRunningConverters(int                              StartedProcesses,
                                ni31_List<tcn381_InputEntry *> & TheInputFileList)
{
    //this function must be used in restore mode only
    int rc=1;

    long BytesWritten;
    char Command[10];
    
    ni31_ListElem<tcn381_InputEntry *> *h;
    tcn381_APipeToFileConverter * Converter;

    for(h=TheInputFileList.First(); rc && 0!=h; h=h->Next())
        if(h->GiveInfo()->IsPipe)
        {
            Command[0]='\0';
            Converter=h->GiveInfo()->Converter;

            if(tcn381_APipeToFileConverter::BackintIsWorking==Converter->State) // there is a file waiting for this converter
            {
                if(Converter->WasRunning)
                    sprintf(Command, "n\n");
            }
            else
                if(Converter->WasRunning && 
                   Converter->InputInfo->RestorePart==Converter->InputInfo->NumberOfParts)
                {
                    sprintf(Command, "l\n");
                    Converter->InputInfo->RestorePart++; // -> avoid sending a secound "l\n" to the process, which could and up with an error
                }

            if('\0'!=Command[0])
            {
                if(!Converter->InPipe->Write(Command, 2, BytesWritten)) 
                    rc=0;
                else
                    Converter->State=tcn381_APipeToFileConverter::ConverterIsWorking;
            }
        }

    return rc;
}

int cn384_CleanUpConverters(int StartedProcesses,
                            ni31_List<tcn381_InputEntry *> & TheInputFileList)
{
    int rc=1;

    ni31_ListElem<tcn381_InputEntry *> * h;
    tcn381_APipeToFileConverter * Converter;

    for(h=TheInputFileList.First(); rc && 0!=h; h=h->Next())
    {
        Converter=h->GiveInfo()->Converter;
        
        if(h->GiveInfo()->IsPipe && 0!=Converter && 0!=Converter->Process)
        {
            //stop the process if necessary
            if(Converter->Process->IsRunning())
                if(!Converter->Process->Stop())
                    rc=0;

            //remove staging files if they exist
            const char * CurrentCompleteStageFileName;

            if(0==Converter->CompleteDestinationStage)
                CurrentCompleteStageFileName=Converter->CompleteStageName;
            else
                CurrentCompleteStageFileName=Converter->CompleteDestinationStage;

            tni34_AFile Stage(CurrentCompleteStageFileName,
                              tni34_AFile::ToDiskAppend,
                              0,
                              0);

            if(!Stage.IsGood() || (Stage.Exists() && !Stage.Remove()))
                rc=0;

            //remove pipes and processes
            if(0!=Converter->InPipe)
                delete Converter->InPipe;

            if(0!=Converter->OutPipe)
                delete Converter->OutPipe;

            if(0!=Converter->Process)
                delete Converter->Process;
        }
    }

    return rc;
}
