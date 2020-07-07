/*!
    \file    vcn382.cpp
    \author  TiloH
    \ingroup cn layer
    \brief   implementation of functions for handling Backint for Oracle
  
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

#include "hcn36.h"
#include "hcn382.h"
#include "hcn385.h"

static tcn36_DateTimeFormat DateTimeFormatForOutput_cn382("yyyy-mm-dd HH:MM:SS");

static int cn382_WriteALineToBackintInput(tni34_ATextFile & BackintInput,
                                          const char *      TheLine,
                                          int &             FilesWrittenToInput)
{
    long BytesWritten;
    int rc=BackintInput.WriteLine(TheLine, BytesWritten);

    if(rc)
        FilesWrittenToInput++;
    else
        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotWriteBackintInput));

    return rc;
}

static int cn382_CreateBackintInput(ni31_List<tcn381_InputEntry *> & TheFileList,
                                    tni34_ATextFile &                BackintInput,
                                    const char *                     Function,
                                    int                              ForPipes,
                                    int                              MaxNumberOfFilesPerBackintCall,
                                    int &                            FilesWrittenToInput)
{
    int rc=1;

    FilesWrittenToInput=0;

    ni31_ListElem<tcn381_InputEntry *> *hBackup;
    ni31_ListElem<char *> *hPart;
    const char * StageName;
    int          StageSuffix;
    const char * BID;
    char         Buf[1024];

    tcn381_InputEntry *IE; //a shortcut for hBackup->GiveInfo();

    for(hBackup=TheFileList.First(); rc && 0!=hBackup; hBackup=hBackup->Next()) //loop through file list
    {
        IE=hBackup->GiveInfo();
        StageName=IE->StageName;
        StageSuffix=0;

        if(!strcmp(Function, FunctionStringInquire_cn38)) // if it is for inquire
        {
            for(hPart=IE->ListOfPartialBIDs.First(); rc && 0!=hPart; hPart=hPart->Next(), StageSuffix++)
            {
                BID=hPart->GiveInfo();

                if(IE->IsPipe)
                    sprintf(Buf, "%s %s.%d", BID, StageName, StageSuffix);
                else
                    sprintf(Buf, "%s %s", BID, StageName);

                rc=cn382_WriteALineToBackintInput(BackintInput, Buf, FilesWrittenToInput);
            }

            //for handling all partial BIDs of a pipe 
            IE->FoundPartInThisBackintOutput=new int[IE->NumberOfParts];

            if(0==IE->FoundPartInThisBackintOutput)
            {
                rc=0;
                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
            }
            else
            {
                int i;

                for(i=0; i<IE->NumberOfParts; i++)
                    IE->FoundPartInThisBackintOutput[i]=0;
            }
        }
        else
            if(!strcmp(Function, FunctionStringBackup_cn38)) // if we are doing a backup
            {
                if(ForPipes)
                {
                    if(IE->IsPipe &&
                       tcn381_APipeToFileConverter::ConverterFinishedStage==IE->Converter->State &&
                       FilesWrittenToInput<MaxNumberOfFilesPerBackintCall)
                    {
                        sprintf(IE->Converter->EndOfStageName, "%d", int(IE->NumberOfParts));

                        if(!cn382_WriteALineToBackintInput(BackintInput, IE->Converter->CompleteStageName, FilesWrittenToInput))
                            rc=0; // error message is given by function
                        else
                            IE->Converter->State=tcn381_APipeToFileConverter::BackintIsWorking;
                    }
                }
                else
                {
                    // handle normal files
                    if(!IE->IsPipe)
                    {
                        rc=cn382_WriteALineToBackintInput(BackintInput,
                                                          IE->Name,
                                                          FilesWrittenToInput);
                    }
                }
            }
            else
                if(!strcmp(Function, FunctionStringRestore_cn38)) // if we are doing a restore
                {
                    if(ForPipes)
                    {
                        if(IE->IsPipe &&
                           tcn381_APipeToFileConverter::StageIsFree==IE->Converter->State &&
                           FilesWrittenToInput<MaxNumberOfFilesPerBackintCall &&
                           IE->RestorePart<IE->NumberOfParts)
                        {
                            sprintf(IE->Converter->EndOfStageName, "%d", int(IE->RestorePart));
                            
                            sprintf(Buf, "%s %s", IE->ListOfPartialBIDs[IE->RestorePart]->GiveInfo(),
                                                      IE->Converter->CompleteStageName);

                            if(0!=IE->Converter->EndOfCompleteDestinationStage)
                            {
                                sprintf(IE->Converter->EndOfCompleteDestinationStage, "%d", int(IE->RestorePart)); //we need this name for removal of the file

                                if(0<strlen(IE->Converter->DestinationStageDirectory))                                              //if we have a destination directory...
                                    sprintf(cn36_TerminatingZeroOf(Buf), " %s", IE->Converter->DestinationStageDirectory);  //...use the destination feature of backint
                            } 
                            
                            if(!cn382_WriteALineToBackintInput(BackintInput, Buf, FilesWrittenToInput))
                                rc=0; // error message is given by function
                            else
                                IE->Converter->State=tcn381_APipeToFileConverter::BackintIsWorking;
                        }
                    }
                    else
                    {
                        if(!IE->IsPipe)
                        {
                            if(0==IE->DestinationName)
                                sprintf(Buf, "%s %s", IE->BID, IE->Name);
                            else
                                sprintf(Buf, "%s %s %s", IE->BID, IE->Name, IE->DestinationName);
                            
                            rc=cn382_WriteALineToBackintInput(BackintInput, Buf, FilesWrittenToInput);
                        }
                    }
                }
                else
                {
                    rc=0;
                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
                }

        IE->FoundInThisBackintOutput=0; // entry not found in output so far
    }

    return rc;
}

static int cn382_CallBackint(const char *        PathOfBackint,
                             const char *        UID,
                             const char *        Function,
                             const char *        PathOfBackintParamFile,
                             const tni34_AFile & InFile,
                             const tni34_AFile & OutFile,
                             const tni34_AFile & ErrFile,
                             tcn386_BackintOut & Output)
{
    int rc=1;

    tcn36_DateTimeString   CurrentDateTime;
    char                 * CurrentDateTimeAsString=CurrentDateTime.CreateDateTimeString(DateTimeFormatForOutput_cn382);

    Output.Write("Calling Backint for Oracle at ", CurrentDateTimeAsString, ".");
    cn36_StrDealloc(CurrentDateTimeAsString);

    tni33_ArgumentList Arguments;

    if(!Arguments.AddArgument(PathOfBackint) ||                     //"...backint"
       !Arguments.AddArgument("-u") ||                              //"-u" 
       !Arguments.AddArgument(UID) ||                               //"<UID>"
       !Arguments.AddArgument("-f") ||                              //"-f"
       !Arguments.AddArgument(Function) ||                          //"backup|inquire|restore"
       !Arguments.AddArgument("-t") ||                              //"-t"
       !Arguments.AddArgument("file"))                              //"file"
    {
        rc=0;
    }
    else
    {
        if('\0'!=PathOfBackintParamFile[0])
        {
            if(!Arguments.AddArgument("-p") ||                      //"-p"
               !Arguments.AddArgument(PathOfBackintParamFile))      //"<ParameterFileOfBackintForOracle>
            {
                rc=0;
            }
        }

        if(rc)
            if(!Arguments.AddArgument("-i") ||                      //"-i"
               !Arguments.AddArgument(InFile.GiveName()) ||         //"<infile>"
               !Arguments.AddArgument("-c"))                        //"-c"
            {
                rc=0;
            }
    }

    if(rc)
    {
        char * Buf=0;
        
        cn36_StrAlloc(Buf, strlen(PathOfBackint)+
                           strlen(" -u ")+
                           strlen(UID)+
                           strlen(" -f ")+
                           strlen(Function)+
                           strlen(" -t file")+
                           (('\0'==PathOfBackintParamFile[0])?0:(strlen(" -p ")+strlen(PathOfBackintParamFile)))+
                           strlen(" -i ")+
                           strlen(InFile.GiveName())+
                           strlen(" -c"));

        if(0!=Buf)
        {
            sprintf(Buf, "%s -u %s -f %s -t file%s%s -i %s -c",
                    PathOfBackint,
                    UID,
                    Function,
                    ('\0'==PathOfBackintParamFile[0])?"":" -p ",
                    ('\0'==PathOfBackintParamFile[0])?"":PathOfBackintParamFile,
                    InFile.GiveName());

            Output.Write("Calling '", Buf, "' .");

            cn36_StrDealloc(Buf);
        }
    }
    else
        Output.Write("Out of memory during construction of parameter list.");

    if(rc)
    {
        tni33_AProcess BackIntProc(Arguments, 0, &OutFile, 0, &ErrFile, 0);

        if(!BackIntProc.WasStarted())
        {
            Output.Write("Could not start Backint for Oracle successfully.");

            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotStartBackint));
        }
        else
        {
            char retCodeAsString[30];
            sprintf(retCodeAsString, "%d", (int)BackIntProc.GiveReturnCode());

            CurrentDateTime.SetTo();
            CurrentDateTimeAsString=CurrentDateTime.CreateDateTimeString(DateTimeFormatForOutput_cn382);

            Output.Write("Backint for Oracle ended at ", CurrentDateTimeAsString, " with return code ", retCodeAsString, ".");
            cn36_StrDealloc(CurrentDateTimeAsString);


            if(0==strcmp(FunctionStringInquire_cn38, Function))                         //if Function=="inquire"...#
            {
                if(0>BackIntProc.GiveReturnCode() || 2<BackIntProc.GiveReturnCode())    //return code can be 0,1 or 2 (with 2 being a harmless concession for some existing Backint for Oracle implementations)
                    rc=0;
            }
            else
            {
	            if(0!=BackIntProc.GiveReturnCode() && 1!=BackIntProc.GiveReturnCode())  //...check the return code (must be 0 or 1 for success)
                    rc=0;
            }

            if(!rc)
                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::BackintWasUnsuccessful));
        }
    }
    
    return rc;
}

static int cn382_ReadBIDs(ni31_List<tcn381_InputEntry *> & TheFileList,
                          int                              ForPipes,
                          const char *                     FunctionString,
                          tni34_ATextFile &                BackintOutput)
{
    int rc=1;

    const char *Line;
    ni31_ListElem<tcn381_InputEntry *> *h;
    tcn381_InputEntry *IE;

    const char *KeyWordForFunction;

    // determine the right keyword for the function
    if(!strcmp(FunctionString, FunctionStringBackup_cn38))
        KeyWordForFunction="#SAVED";
    else
        if(!strcmp(FunctionString, FunctionStringInquire_cn38))
            KeyWordForFunction="#BACKUP";
        else
            if(!strcmp(FunctionString, FunctionStringRestore_cn38))
                KeyWordForFunction="#RESTORED";

    //for every line
    while(BackintOutput.ReadLine(Line) && Line!=0)
    {
        const char *KeyWord=cn36_FirstNonWhiteSpaceOf(Line);
        const char *EndOfKeyWord=cn36_FirstWhiteSpaceOf(KeyWord);

        if(!cn36_StrStrNCmp(KeyWordForFunction, KeyWord, size_t(EndOfKeyWord-KeyWord)))
        {
            const char *BID=cn36_FirstNonWhiteSpaceOf(EndOfKeyWord);
            const char *EndOfBID=cn36_FirstWhiteSpaceOf(BID);
            const char *FileName=cn36_FirstNonWhiteSpaceOf(EndOfBID);
            const char *EndOfFileName=cn36_FirstWhiteSpaceOf(FileName);

            if(EndOfBID[0]!='\0' && FileName[0]!='\0') //found BID and Name behind the keyword
            {
                if(EndOfBID-BID>16) //is the BID a BID? this is needed for the StrNCpy calls
                {
                    rc=0;
                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::SyntaxErrorInBackintOutput));
                }
                else
                {
                    const char *CurrentName;

                    for(h=TheFileList.First(); rc && h!=0; h=h->Next()) // for all files from the list
                    {
                        IE=h->GiveInfo(); // just a shortcut to the current tcn381_InputEntry

                        if(!strcmp(FunctionString, FunctionStringBackup_cn38) ||
                           !strcmp(FunctionString, FunctionStringRestore_cn38)) // if we do a backup or restore
                        {
                            if(IE->IsPipe)
                                CurrentName=IE->Converter->CompleteStageName; //if it's a pipe, we saved only a part with backint
                            else
                                CurrentName=IE->Name; //if it's a file, we saved it at once with backint

                            if(ForPipes==IE->IsPipe &&                                            //if IE is from the rigth type
                               !cn36_StrStrNCmp(CurrentName, FileName, EndOfFileName-FileName))//the name we have to look for is equal the found name
                            {
                                if(0!=IE->FoundInThisBackintOutput ||         //was this file already found?
                                   (IE->IsPipe &&
                                    tcn381_APipeToFileConverter::BackintIsWorking!=IE->Converter->State)) //the file should not be saved by backint
                                {
                                    rc=0;
                                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::BackintProcessedWrongFile));
                                }
                                else
                                {
                                    if(!strcmp(FunctionString, FunctionStringBackup_cn38))
                                        rc=IE->SetBID(BID, EndOfBID-BID);

                                    IE->FoundInThisBackintOutput=1;
                                }
                            }
                        }
                        else
                            if(!strcmp(FunctionString, FunctionStringInquire_cn38))
                            {
                                if(IE->IsPipe)
                                {
                                    int SuffixNumber;

                                    if(!strncmp(FileName, IE->StageName, strlen(IE->StageName)) &&
                                       '.'==FileName[strlen(IE->StageName)] &&
                                       cn36_StrNToInt(SuffixNumber, FileName+strlen(IE->StageName)+1, EndOfFileName-FileName-strlen(IE->StageName)-1) &&
                                       0<=SuffixNumber &&
                                       IE->NumberOfParts>SuffixNumber)
                                    {
                                        char SuffixAsString[21];
                                        sprintf(SuffixAsString, "%d", SuffixNumber);

                                        if(!cn36_StrStrNCmp(SuffixAsString, FileName+strlen(IE->StageName)+1, EndOfFileName-FileName-strlen(IE->StageName)-1) &&
                                           !cn36_StrStrNCmp(IE->ListOfPartialBIDs[SuffixNumber]->GiveInfo(), BID, EndOfBID-BID))
                                        {
                                            IE->FoundPartInThisBackintOutput[SuffixNumber]=1;//test BID !!!!
                                        }
                                    }
                                }
                                else
                                {
                                    if(!cn36_StrStrNCmp(IE->StageName, FileName, EndOfFileName-FileName) &&
                                       !cn36_StrStrNCmp(IE->BID, BID, EndOfBID-BID))
                                    {
                                        IE->FoundInThisBackintOutput=1;
                                    }
                                }
                            }
                            else
                            {
                                rc=0;
                                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
                            }
                    }
                }
            }
        }
    }

    if(!strcmp(FunctionString, FunctionStringBackup_cn38) ||
       !strcmp(FunctionString, FunctionStringRestore_cn38))
    {
        //check that every file was processed successfully
        for(h=TheFileList.First(); h!=0; h=h->Next())
        {
            IE=h->GiveInfo();

            if((0==ForPipes && !IE->IsPipe) ||
               (1==ForPipes && IE->IsPipe && tcn381_APipeToFileConverter::BackintIsWorking==IE->Converter->State))
            {
                if(!IE->FoundInThisBackintOutput)
                {
                    IE->ProcessedSuccessful=0;
                    rc=0;
                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::BackintProcessedUnsuccessful));
                }
            }
        }
    }
    else
        if(!strcmp(FunctionString, FunctionStringInquire_cn38))
        {
            int FoundAllParts,
                i;

            for(h=TheFileList.First(); rc && h!=0; h=h->Next()) // for all files from the list
                if(h->GiveInfo()->IsPipe)
                {
                    for(FoundAllParts=1, i=0; FoundAllParts && i<h->GiveInfo()->NumberOfParts; i++)
                        if(!h->GiveInfo()->FoundPartInThisBackintOutput[i])
                            FoundAllParts=0;

                    if(FoundAllParts)
                        h->GiveInfo()->FoundInThisBackintOutput=1;
                }
        }

    return rc;
}

int cn382_CopyBackintForOracleOutputToOwnOutput(tni34_ATextFile   * BackintOutput,
                                                tni34_ATextFile   * BackintError,
                                                tcn386_BackintOut & Output)
{
    int rc=1;

    Output.Open();

    if(rc)
    {
        char * CompleteLine=0;
        tni34_ATextFile * BackintFile[2]={BackintOutput, BackintError};
        const char * Line;
        const char * Prefix[2]={"Backint for Oracle output: ", "Backint for Oracle error output: "};
        int i;

        for(i=0; i<2; i++)
            if(BackintFile[i]->Exists() && BackintFile[i]->Open())
            {    
                while(BackintFile[i]->IsGood() && BackintFile[i]->ReadLine(Line) && Line!=0)
                {
                    if(0!=CompleteLine)
                    {
                        delete [] CompleteLine;
                        CompleteLine=0;
                    }

                    CompleteLine=new char[strlen(Prefix[i])+strlen(Line)+1];

                    if(0==CompleteLine)
                    {
                        rc=0;
                        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
                    }
                    else
                    {
                        sprintf(CompleteLine, "%s%s", Prefix[i], Line);

                        if(!Output.Write(CompleteLine))
                        {
                            rc=0;
                            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotWriteToOutFile));
                        }
                    }
                }

                BackintFile[i]->Close();
            }

        if(0!=CompleteLine)
            delete [] CompleteLine;
    }

    Output.Close();

    return rc;
}

int cn382_SetBIEnvironmentVariables(int FirstBackintForOracleCall, const char * BI_CALLERValue)
{
    int rc=1;

    const char * BI_CALLER="BI_CALLER";
    const char * ExpectedValueOfBI_CALLER=BI_CALLERValue;
    const char * BI_REQUEST="BI_REQUEST";
    const char * ExpectedValueOfBI_REQUEST="OLD";

    const char * ValueOfBI_CALLER=cn36_GetEnv(BI_CALLER);
    const char * ValueOfBI_REQUEST=cn36_GetEnv(BI_REQUEST);

    if((0==ValueOfBI_CALLER && 0<strlen(ExpectedValueOfBI_CALLER)) ||                   //if BI_CALLER is not set but should be set
       (0!=ValueOfBI_CALLER && 0!=strcmp(ValueOfBI_CALLER, ExpectedValueOfBI_CALLER)))  //or if it is set but not equal to BI_CALLERValue (default: "")
    {
        rc=cn36_PutEnv(BI_CALLER, ExpectedValueOfBI_CALLER);    //try to export "BI_CALLER=<BI_CALLERValue" to the environment
    }

    if(rc)
    {
        //find out which value must be set for BI_REQUEST
        if(FirstBackintForOracleCall &&             //if it will be the first call of Backint for Oracle by this program
           (0==ValueOfBI_REQUEST ||                 //and BI_REQUEST was not set
            0==strcmp(ValueOfBI_REQUEST, "NEW")))   //or was set to "NEW" ("BI_REQUEST=NEW") by the parent process
        {
            ExpectedValueOfBI_REQUEST="NEW";    //the value to set is "NEW"
        }                                       //else stay with "OLD" as set above (e.g. if parent process has already set it to "OLD")

        if(0==ValueOfBI_REQUEST ||                                  //if BI_REQUEST was not set
           0!=strcmp(ValueOfBI_REQUEST, ExpectedValueOfBI_REQUEST)) //or is not equal to the expected value ("NEW" or "OLD")
        {
            rc=cn36_PutEnv(BI_REQUEST, ExpectedValueOfBI_REQUEST);                                      //try to export "BI_REQUEST=<Expectedvalue>"
        }
    }

    return rc;
}

int cn382_UseBackint(ni31_List<tcn381_InputEntry *> & TheInputFileList,
                     const char *                     PathOfBackint,
                     const char *                     UserID,
                     const char *                     FunctionString,
                     int                              ForPipes,
                     int                              MaxNumberOfFilesPerBackintCall,
                     const char *                     PathOfBackintParamFile,
                     const char *                     InFileName,
                     const char *                     OutFileName,
                     const char *                     ErrFileName,
                     tcn386_BackintOut              & Output,
                     const char *                     BI_CALLERValue)
{
    int rc=1;

    static int FirstBackintForOracleCall=1;

    //create/truncate the input file
    tni34_ATextFile * BackintInput;
    
    if('\0'==InFileName[0])
        BackintInput=new tni34_ATextFile(tni34_AFile::ToDiskTruncate);
    else
        BackintInput=new tni34_ATextFile(InFileName, tni34_AFile::ToDiskTruncate);

    if(0==BackintInput)
    {
        rc=0;
        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
    }
    else
    {
        if(BackintInput->IsGood())
        {
            //write all necessary data to the input file
            int NumberOfFilesWrittenToInput;

            if(cn382_CreateBackintInput(TheInputFileList,
                                        *BackintInput,
                                        FunctionString,
                                        ForPipes,
                                        MaxNumberOfFilesPerBackintCall,
                                        NumberOfFilesWrittenToInput))
            {
                if(!BackintInput->Close())
                {
                    rc=0;
                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotCloseBackintInput));
                }
                else
                    if(0<NumberOfFilesWrittenToInput) //there is something to do for backint
                    {
                        //create/truncate the output file
                        tni34_ATextFile * BackintOutput=0;
                        tni34_ATextFile * BackintError=0;
                
                        if('\0'==OutFileName[0])
                            BackintOutput=new tni34_ATextFile(tni34_AFile::FromDisk);
                        else
                            BackintOutput=new tni34_ATextFile(OutFileName, tni34_AFile::FromDisk);

                        if('\0'==ErrFileName[0])
                            BackintError=new tni34_ATextFile(tni34_AFile::FromDisk);
                        else
                            BackintError=new tni34_ATextFile(ErrFileName, tni34_AFile::FromDisk);

                        if(0==BackintOutput ||
                           !BackintOutput->IsGood() ||
                           0==BackintError ||
                           !BackintError->IsGood())
                        {
                            rc=0;
                            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
                        }
                        else
                        {
                            if(!BackintOutput->Close())
                            {
                                rc=0;
                                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotCreateBackintOutput));
                            }

                            if(!BackintError->Close())
                            {
                                rc=0;
                                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotCreateBackintErrorOutput));
                            }

                            if(rc)
                            {
                                rc=cn382_SetBIEnvironmentVariables(FirstBackintForOracleCall, BI_CALLERValue);

                                FirstBackintForOracleCall=0;
                            }

                            if(rc)
                            {
                                // now execute backint
                                rc=cn382_CallBackint(PathOfBackint,
                                                     UserID,
                                                     FunctionString,
                                                     PathOfBackintParamFile,
                                                     *BackintInput,
                                                     *BackintOutput,
                                                     *BackintError,
                                                     Output);

                                //analyse the results of the backint call (if there are any)
                                if(!BackintOutput->Exists())
                                {
                                    rc=0;
                                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::BackintOutputVanished));
                                }
                                else
                                    if(!BackintOutput->Open())
                                    {
                                        rc=0;
                                        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotOpenBackintOutput));
                                    }

                                if(rc)
                                {
                                    rc=cn382_ReadBIDs(TheInputFileList,
                                                      ForPipes,
                                                      FunctionString,
                                                      *BackintOutput);

                                    if(!BackintOutput->Close())
                                    {
                                        rc=0;
                                        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotCloseBackintOutput));
                                    }
                                }

                                cn382_CopyBackintForOracleOutputToOwnOutput(BackintOutput, BackintError, Output);

                                if(BackintOutput->Exists() && !BackintOutput->Remove()) //clean up the output
                                {
                                    rc=0;
                                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotRemoveBackintOutput));
                                }

                                if(BackintError->Exists() && !BackintError->Remove()) //clean up the error output
                                {
                                    rc=0;
                                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotRemoveBackintErrorOutput));
                                }
                            }
                        }

                        if(0!=BackintOutput)
                            delete BackintOutput;

                        if(0!=BackintError)
                            delete BackintError;
                    }
            }
            else
                rc=0;
  
            if(BackintInput->Exists() && !BackintInput->Remove()) //clean up the input
            {
                rc=0;
                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotRemoveBackintInput));
            }
        }
        else
            rc=0;

        delete BackintInput;
    }

    Output.Write("");

    return rc;
}
