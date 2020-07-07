/*!
    \file    vcn38.cpp
    \author  TiloH
    \ingroup cn layer
    \brief   main function and core functions of the Backint for MaxDB/
             Backint for Oracle adapter

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
#include <limits.h>
#include <time.h>

#include "SAPDBCommon/SAPDB_string.h"

#include "hcn38.h"
#include "hcn382.h"
#include "hcn383.h"
#include "hcn384.h"
#include "hcn36.h"
#include "hcn385.h"
#include "hcn386.h"

class tcn38_ThisProgramParameters: public ni32_ProgramParameters
{
  public:
    tcn38_ThisProgramParameters(int                            argc,
                                const char * const *           argv,
                                ni31_List<ni32_AParameter *> & ParamList)
        :ni32_ProgramParameters(argc, argv, ParamList),
         CurrentProgramName(argv[0]),
         UsageString(0),
         UsageStringConstructed(0)
    {}

    ~tcn38_ThisProgramParameters()
    {
        if(UsageStringConstructed)
            delete [] UsageString;
    }

    virtual const char * UsageMessage()
    {
        if(0==UsageString)
        {
            static char ConstUsageString[]="usage: backint -u <user_id> [-f <function>] [-t <type>]\n"
                                           "               [-p <par_file>] [-i <in_file>] [-o <out_file>] [-c]\n\n"
                                           "parameters can be:\n"
                                           "  <user_id> : char(16)\n"
                                           "  <function>: backup|restore|inquire|delete\n" 
                                           "  <type>    : file\n"
                                           "  <par_file>: char(255)\n"
                                           "  <in_file> : char(255)\n"
                                           "  <out_file>: char(255)\n";

            if(!strcmp(CurrentProgramName, "backint"))
                UsageString=ConstUsageString;
            else
            {
                char * ProgramName=ConstUsageString+strlen("usage: ");
                char * Options    =ProgramName+strlen("backint");

                *ProgramName='\0';

                size_t i=strlen(ConstUsageString)+strlen(CurrentProgramName)+1+strlen(Options);

                UsageString=new char [i+1];

                if(0==UsageString)
                {
                    *ProgramName='b';
                    UsageString=ConstUsageString;
                }
                else
                {
                    UsageStringConstructed=1;
                    sprintf(UsageString, "%s%s %s", ConstUsageString, CurrentProgramName, Options);
                }
            }
        }

        return UsageString;
    }

    virtual int          UsageMessageAvailable(){return 1;}

  private:
    const char * CurrentProgramName;
    char       * UsageString;
    int          UsageStringConstructed;
};

class tcn38_AParamFileInfo
{
  private:
    static const int DefaultForMaxDelayOfBackintCall;

  public:
    int         NumberOfStagingAreas;
    char *      StagingAreaName[MAX_STAGING_AREAS_CN38];
    teo00_Uint8 StagingAreaSize[MAX_STAGING_AREAS_CN38];

    int    NumberOfFilesPerBackintCall;
    char   PathOfBackint[MAX_BACKINT_PATH_CHARS_CN38];
    char   PathOfBackintParamFile[MAX_BACKINT_PATH_CHARS_CN38];

    char   PathOfBackintInput[MAX_BACKINT_PATH_CHARS_CN38];
    char   PathOfBackintOutput[MAX_BACKINT_PATH_CHARS_CN38];
    char   PathOfBackintErrorOutput[MAX_BACKINT_PATH_CHARS_CN38];

    char   PathOfHistoryFile[MAX_BACKINT_PATH_CHARS_CN38];

    int    MaxDelayOfBackintCall;

    int    UseFinalDirectoryDelimiterOnRestore;

    char   BI_CALLERValue[MAX_BACKINT_PATH_CHARS_CN38];


    tcn38_AParamFileInfo()
      :NumberOfStagingAreas(0),
       NumberOfFilesPerBackintCall(1),
       MaxDelayOfBackintCall(DefaultForMaxDelayOfBackintCall),
       UseFinalDirectoryDelimiterOnRestore(1)
    {
        int i;

        for(i=0; i<MAX_STAGING_AREAS_CN38; i++)
        {
            StagingAreaName[i]=0;
            StagingAreaSize[i]=0;
        }

        PathOfBackint[0]='\0';
        PathOfBackintParamFile[0]='\0';
        
        PathOfBackintInput[0]='\0';
        PathOfBackintOutput[0]='\0';
        PathOfBackintErrorOutput[0]='\0';
        
        PathOfHistoryFile[0]='\0';
        
        BI_CALLERValue[0]='\0';
    }

    ~tcn38_AParamFileInfo()
    {
        int i;

        for(i=0; i<MAX_STAGING_AREAS_CN38; i++)
            if(0!=StagingAreaName[i])
                delete [] StagingAreaName[i];
    }
};

const int tcn38_AParamFileInfo::DefaultForMaxDelayOfBackintCall=10;


struct tcn38_ParamFileEntry
{
  public:
    enum AParamFileEntryType{StagingArea,
                             FilesPerBackintCall,
                             Backint,
                             BRIParamFile,
                             HistoryFile,
                             BRIInput,
                             BRIOutput,
                             BRIError,
                             MaxBRIDelay,
                             UseDirectoryDelimiterOnRestore,
                             BICALLERValue,
                             Last};

    AParamFileEntryType   Type;
    const char          * KeyWords;
    size_t                LengthOfKeyWords;
};

static tcn38_ParamFileEntry ParamFileEntries_cn38[]=
{
    {tcn38_ParamFileEntry::StagingArea,                    "STAGING AREA:",                            0},
    {tcn38_ParamFileEntry::FilesPerBackintCall,            "FILES PER BACKINT CALL:",                  0},
    {tcn38_ParamFileEntry::Backint,                        "BACKINT:",                                 0},
    {tcn38_ParamFileEntry::BRIParamFile,                   "PARAMETERFILE OF BACKINT:",                0},
    {tcn38_ParamFileEntry::HistoryFile,                    "HISTORY FILE:",                            0},
    {tcn38_ParamFileEntry::BRIInput,                       "INPUTFILE FOR BACKINT:",                   0},
    {tcn38_ParamFileEntry::BRIOutput,                      "OUTPUTFILE FOR BACKINT:",                  0},
    {tcn38_ParamFileEntry::BRIError,                       "ERRORFILE FOR BACKINT:",                   0},
    {tcn38_ParamFileEntry::MaxBRIDelay,                    "MAXIMAL DELAY OF BACKINT CALL:",           0},
    {tcn38_ParamFileEntry::UseDirectoryDelimiterOnRestore, "OMIT LAST DIRECTORY DELIMITER ON RESTORE:",0},
    {tcn38_ParamFileEntry::BICALLERValue,                  "BI_CALLER VALUE:",                         0},  //should only be used if an empty/unset BI_CALLER environment variable is problematic
    {tcn38_ParamFileEntry::Last,                           "",                                         0}
};

static int cn38_GetValueNOfEntry(char *       Dest,
                                 size_t       DestLength,
                                 const char * Line,
                                 const char * KeyWords)
{
    int rc=1;

    const char * StartOfValue=cn36_FirstNonWhiteSpaceOf(Line+strlen(KeyWords)); // ignore the keyword and white spaces
    const char * EndOfValue;

    cn36_GetStringBetweenDoubleQuotesOrWhiteSpaces(StartOfValue, EndOfValue);

    if(DestLength<unsigned(EndOfValue-StartOfValue))
    {
        rc=0;   // value is to long
        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::SyntaxErrorValueToLong));
    }
    else
    {
        DestLength=EndOfValue-StartOfValue;     // do not copy to much
        cn36_StrNCpy(Dest, StartOfValue, DestLength);
    }

    return rc;
}

static int cn38_ReadParameterFile(const char *           ParameterFileName,
                                  tcn38_AParamFileInfo & ParamFileInfo,
                                  tcn386_BackintOut    & Output)
{
    int rc=1;

    //init ParamFileEntries_cn38 completely
    int i=0;

    Output.Write("Reading parameter file ", ParameterFileName, ".");

    while(tcn38_ParamFileEntry::Last!=ParamFileEntries_cn38[i].Type)
    {
        ParamFileEntries_cn38[i].LengthOfKeyWords=strlen(ParamFileEntries_cn38[i].KeyWords);
        i++;
    }

    if(0!=ParameterFileName)
    {
        tni34_ATextFile ParamFile(ParameterFileName, tni34_AFile::FromDisk, 0, 1, 0, 1024); 

        if(ParamFile.IsGood())
        {
            const char * Line;

            while(rc && ParamFile.ReadLine(Line) && 0!=Line)
            {
                int TypeFound=0;

                i=0;

                while(!TypeFound && tcn38_ParamFileEntry::Last!=ParamFileEntries_cn38[i].Type)
                {
                    if(0==cn36_StrNUprCmp(Line,
                                          ParamFileEntries_cn38[i].KeyWords,
                                          ParamFileEntries_cn38[i].LengthOfKeyWords))
                    {
                        TypeFound=1;
                    }
                    else
                        i++;
                }

                switch(ParamFileEntries_cn38[i].Type)
                {
                    case tcn38_ParamFileEntry::StagingArea:
                    //if(!strncmp(Line, "staging area:", strlen("staging area:")))
                    {
                        const char * CopyOfLine=Line;

                        const char * NewStagingName=cn36_FirstNonWhiteSpaceOf(CopyOfLine+strlen(ParamFileEntries_cn38[i].KeyWords));
                        const char * EndOfNewStagingName=0;
                        const char * NewStagingSize;
                        const char * EndOfNewStageSize;

                        cn36_GetStringBetweenDoubleQuotesOrWhiteSpaces(NewStagingName, EndOfNewStagingName);    // stage names can be enclosed in double qoutes

                        NewStagingSize=cn36_FirstNonWhiteSpaceOf(cn36_FirstWhiteSpaceOf(EndOfNewStagingName));
                        EndOfNewStageSize=cn36_FirstWhiteSpaceOf(NewStagingSize);

                        if('\0'==NewStagingName[0] || '\0'==NewStagingSize[0])
                        {
                            rc=0;
                            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::SyntaxErrorStage));
                        }
                        else
                        {
                            const char * UnitOfNewSize;
                            const char * EndOfUnitOfNewSize;

                            teo00_Uint8 ISize;
                            teo00_Uint8 UnitFactor=1;

                            if(cn36_StrNConsistsOfDigits(
                                   NewStagingSize,
                                   EndOfNewStageSize-NewStagingSize))
                            {
                                UnitOfNewSize=cn36_FirstNonWhiteSpaceOf(EndOfNewStageSize);
                            }
                            else
                            {
                                UnitOfNewSize=cn36_FirstNonDigitOf(NewStagingSize, EndOfNewStageSize-NewStagingSize);
                                EndOfNewStageSize=UnitOfNewSize;
                            }

                            EndOfUnitOfNewSize=cn36_FirstWhiteSpaceOf(UnitOfNewSize);

                            if(!cn36_StrNToUInt8(ISize,
                                                 NewStagingSize,
                                                 EndOfNewStageSize-NewStagingSize) ||   //could not transform String to Integer
                               0>=ISize)                                                //Size is negativ or 0
                            {
                                rc=0;
                                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::SyntaxErrorSize));
                            }
                            else
                            {
                                if('\0'!=UnitOfNewSize[0])
                                {
                                    if(0==cn36_StrStrNUprCmp("KB", UnitOfNewSize, EndOfUnitOfNewSize-UnitOfNewSize))
                                        UnitFactor=1024;
                                    else
                                        if(0==cn36_StrStrNUprCmp("MB", UnitOfNewSize, EndOfUnitOfNewSize-UnitOfNewSize))
                                            UnitFactor=1024*1024;
                                        else
                                            if(0==cn36_StrStrNUprCmp("GB", UnitOfNewSize, EndOfUnitOfNewSize-UnitOfNewSize))
                                                UnitFactor=1024*1024*1024;
                                            else
                                            {
                                                rc=0;
                                                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::SyntaxErrorSize));
                                            }

                                    if(rc)
                                    {
                                        if(MAX_UINT8_CN36/UnitFactor<ISize) // test for (MAX_UINT8_CN36<ISize*UnitFactor) without risking an overflow by multiplication
                                        {
                                            rc=0;
                                            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::SyntaxErrorSize));
                                        }
                                        else
                                            ISize*=UnitFactor;
                                    }
                                }

                                if(rc)
                                {
                                    char * CopyOfNewStagingName=new char [EndOfNewStagingName-NewStagingName+1];

                                    if(0==CopyOfNewStagingName)
                                    {
                                        rc=0;
                                        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
                                    }
                                    else
                                    {
                                        cn36_StrNCpy(CopyOfNewStagingName, NewStagingName, EndOfNewStagingName-NewStagingName);
                                        ParamFileInfo.StagingAreaName[ParamFileInfo.NumberOfStagingAreas]=CopyOfNewStagingName;
                                        ParamFileInfo.StagingAreaSize[ParamFileInfo.NumberOfStagingAreas++]=ISize;

                                        char ISizeAsString[31];

                                        if(!cn36_UInt8ToStrN(ISizeAsString, sizeof(ISizeAsString), ISize))
                                        {
                                            rc=0;
                                            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
                                        }
                                        else
                                            Output.Write("Using staging area ", CopyOfNewStagingName, " with a size of ", ISizeAsString, " bytes.");
                                    }
                                }
                            }
                        }
                    }
                    break;

                    case tcn38_ParamFileEntry::FilesPerBackintCall:
                    //if(!strncmp(Line, "files per BACKINT call:", strlen("files per BACKINT call:")))
                    {
                        const char * Number=cn36_FirstNonWhiteSpaceOf(Line+strlen(ParamFileEntries_cn38[i].KeyWords));
                        const char * EndOfNumber=cn36_FirstWhiteSpaceOf(Number);

                        if(!cn36_StrNToInt(ParamFileInfo.NumberOfFilesPerBackintCall, Number, EndOfNumber-Number))
                        {
                            rc=0;
                            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::SyntaxErrorNumberFilesPerBRICall));
                        }
                        else
                        {
                            char NumberOfFilesPerBackintCallAsString[31];

                            if(!cn36_IntToStrN(NumberOfFilesPerBackintCallAsString, sizeof(NumberOfFilesPerBackintCallAsString), ParamFileInfo.NumberOfFilesPerBackintCall))
                            {
                                rc=0;
                                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
                            }
                            else
                                Output.Write("Using ", NumberOfFilesPerBackintCallAsString, " file", 1<=ParamFileInfo.NumberOfFilesPerBackintCall?"":"s", " per Backint for Oracle call.");
                        }
                    }
                    break;

                    case tcn38_ParamFileEntry::Backint:
                    //if(!strncmp(Line, "backint:", strlen("backint:")))
                    {
                        rc=cn38_GetValueNOfEntry(ParamFileInfo.PathOfBackint,
                                                 MAX_BACKINT_PATH_CHARS_CN38-1,
                                                 Line,
                                                 ParamFileEntries_cn38[i].KeyWords);

                        if(rc)
                            Output.Write("Using ", ParamFileInfo.PathOfBackint, " as Backint for Oracle.");

                    }
                    break;

                    case tcn38_ParamFileEntry::BRIParamFile:
                    //if(!strncmp(Line, "backint's paramfile:", strlen("backint's paramfile:")))
                    {
                        rc=cn38_GetValueNOfEntry(ParamFileInfo.PathOfBackintParamFile,
                                                 MAX_BACKINT_PATH_CHARS_CN38-1,
                                                 Line,
                                                 ParamFileEntries_cn38[i].KeyWords);

                        if(rc)
                            Output.Write("Using ", ParamFileInfo.PathOfBackintParamFile, " as parameterfile of Backint for Oracle.");
                    }
                    break;

                    case tcn38_ParamFileEntry::HistoryFile:
                    //if(!strncmp(Line, "history file:", strlen("history file:")))
                    {
                        rc=cn38_GetValueNOfEntry(ParamFileInfo.PathOfHistoryFile,
                                                 MAX_BACKINT_PATH_CHARS_CN38-1,
                                                 Line,
                                                 ParamFileEntries_cn38[i].KeyWords);

                        if(rc)
                            Output.Write("Using ", ParamFileInfo.PathOfHistoryFile, " as history file.");
                    }
                    break;

                    case tcn38_ParamFileEntry::BRIInput:
                    {
                        rc=cn38_GetValueNOfEntry(ParamFileInfo.PathOfBackintInput,
                                                 MAX_BACKINT_PATH_CHARS_CN38-1,
                                                 Line,
                                                 ParamFileEntries_cn38[i].KeyWords);
                        
                        if(rc)
                            Output.Write("Using ", ParamFileInfo.PathOfBackintInput, " as input of Backint for Oracle.");
                    }
                    break;

                    case tcn38_ParamFileEntry::BRIOutput:
                    {
                        rc=cn38_GetValueNOfEntry(ParamFileInfo.PathOfBackintOutput,
                                                 MAX_BACKINT_PATH_CHARS_CN38-1,
                                                 Line,
                                                 ParamFileEntries_cn38[i].KeyWords);

                        if(rc)
                            Output.Write("Using ", ParamFileInfo.PathOfBackintOutput, " as output of Backint for Oracle.");
                    }
                    break;
                    
                    case tcn38_ParamFileEntry::BRIError:
                    {
                        rc=cn38_GetValueNOfEntry(ParamFileInfo.PathOfBackintErrorOutput,
                                                 MAX_BACKINT_PATH_CHARS_CN38-1,
                                                 Line,
                                                 ParamFileEntries_cn38[i].KeyWords);

                        if(rc)
                            Output.Write("Using ", ParamFileInfo.PathOfBackintErrorOutput, " as error output of Backint for Oracle.");
                    }
                    break;

                    case tcn38_ParamFileEntry::MaxBRIDelay:
                    {
                        const char * Number=cn36_FirstNonWhiteSpaceOf(Line+strlen(ParamFileEntries_cn38[i].KeyWords));
                        const char * EndOfNumber=cn36_FirstWhiteSpaceOf(Number);

                        if(!cn36_StrNToInt(ParamFileInfo.MaxDelayOfBackintCall, Number, EndOfNumber-Number))
                        {
                            rc=0;
                            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::SyntaxErrorMaxDelayOfBRICall));
                        }
                        else
                        {
                            char MaxDelayAsString[31];

                            if(!cn36_IntToStrN(MaxDelayAsString, sizeof(MaxDelayAsString), ParamFileInfo.MaxDelayOfBackintCall))
                            {
                                rc=0;
                                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
                            }
                            else
                                Output.Write("Using a maximal delay for a Backint for Oracle call of ", MaxDelayAsString, " second", 1==ParamFileInfo.MaxDelayOfBackintCall?".":"s.");
                        }
                    }
                    break;

                    case tcn38_ParamFileEntry::UseDirectoryDelimiterOnRestore:
                    {
                        char Value[3+1];

                        rc=cn38_GetValueNOfEntry(Value,
                                                 3,
                                                 Line,
                                                 ParamFileEntries_cn38[i].KeyWords);

                        if(rc)
                        {
                            if(0==cn36_StrUprCmp("NO", Value))  // do not omit last directory delimiter -> UseFinalDirDelimiter
                                ParamFileInfo.UseFinalDirectoryDelimiterOnRestore=1;
                            else
                                if(0==cn36_StrUprCmp("YES", Value)) // omit last directory delimiter -> !UseFinalDirDelimiter
                                    ParamFileInfo.UseFinalDirectoryDelimiterOnRestore=0;
                                else
                                    rc=0;   //we should get a YES or NO but nothing else
                        }

                        if(!rc)
                            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::SyntaxErrorUseDirectoryDelimiterOnRestore));
                        else
                            Output.Write("Using ", ParamFileInfo.UseFinalDirectoryDelimiterOnRestore?"a":"no", " final delimiter in directory names.");
                    }
                    break;

                    case tcn38_ParamFileEntry::BICALLERValue:
                    {
                        rc=cn38_GetValueNOfEntry(ParamFileInfo.BI_CALLERValue,
                                                 MAX_BACKINT_PATH_CHARS_CN38-1,
                                                 Line,
                                                 ParamFileEntries_cn38[i].KeyWords);

                        if(rc)
                            Output.Write("Using ", ParamFileInfo.BI_CALLERValue, " as value for environment variable BI_CALLER.");

                    }
                    break;

                    default:    //ignore other lines and do nothing
                    break;
                }
            }
        }
        else
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotOpenParameterFile));
        }

        ParamFile.Close();
    }

    if(rc)// check required parameters
    {
        if(ParamFileInfo.NumberOfFilesPerBackintCall > ParamFileInfo.NumberOfStagingAreas)
            ParamFileInfo.NumberOfFilesPerBackintCall=ParamFileInfo.NumberOfStagingAreas;

        if(ParamFileInfo.NumberOfStagingAreas<1)
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::NoStagingAreaDefined));
        }

        if('\0'==ParamFileInfo.PathOfBackint[0])
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::BRINotSpecified));
        }
        else
        {
            tni34_AFile Backint(ParamFileInfo.PathOfBackint, tni34_AFile::FromDisk, 0, 0, 0);

            if(!Backint.IsAFile())
            {
                rc=0;
                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::BackintMustBeAFile));
            }

            //TODO: check that backint is executable
        }
        
        if('\0'==ParamFileInfo.PathOfHistoryFile[0])
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::HistoryNotSpecified));
        }

        if('\0'==ParamFileInfo.PathOfBackintInput[0])
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::BRIInputNotSpecified));
        }

        if('\0'==ParamFileInfo.PathOfBackintOutput[0])
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::BRIOutputNotSpecified));
        }

        if('\0'==ParamFileInfo.PathOfBackintErrorOutput[0])
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::BRIErrorNotSpecified));
        }
    }

    Output.Write("");

    return rc;
}

static int cn38_ReadInputFile(const char *                     InputFileName,
                              const tcn38_AParamFileInfo &     TheParamFileInfo,
                              ni31_List<tcn381_InputEntry *> & TheFileList,
                              const char *                     Function,
                              tcn386_BackintOut              & Output)
{
    int               rc=1;
    int               NumberOfPipes=0;
    int               EndOfFileReached=0;
    char              Buf[1024];
    const char *      Line;
    tni34_ATextFile * InputFile=0;

    Output.Write("Reading input file ", 0==InputFileName?"standard input":InputFileName, ".");

    if(0==InputFileName)
        Line=Buf;
    else
    {
        InputFile=new tni34_ATextFile(InputFileName, tni34_AFile::FromDisk, 0);

        if(0==InputFile || !InputFile->IsGood())
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotOpenInputFile));
        }
    }

    while(rc && !EndOfFileReached)
    {
        if(0==InputFileName)
        {
            int i=0;
            int LineBreak=0;

            while(!LineBreak && !EndOfFileReached && i<1024)
            {
                if(EOF==scanf("%c", Buf+i))
                    EndOfFileReached=1;
                else
                    if('\n'==Buf[i])
                        LineBreak=1;
                    else
                        i++;
            }

            Buf[i]='\0';
        }
        else
        {
            if(!InputFile->ReadLine(Line))
            {
                rc=0;
                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotReadInputFile));
            }
            else
                if(0==Line)
                    EndOfFileReached=1;
        }

        if(rc && !EndOfFileReached)
        {
            if('\0'!=(*cn36_FirstNonWhiteSpaceOf(Line)))
            {
                tcn381_InputEntry *IE=new tcn381_InputEntry(Line, Function);

                if(0!=IE && IE->IsGood && 0!=TheFileList.Add(IE))
                {
                    if(1==IE->IsPipe)        //<- this is not completely correct
                        NumberOfPipes++;

                    if(!strcmp(Function, FunctionStringBackup_cn38))
                        Output.Write("Backing up ", IE->IsPipe?"pipe ":"file ", IE->Name, ".");
                    else
                        if(!strcmp(Function, FunctionStringInquire_cn38))
                        {
                            if(0==IE->Name || 0>=strlen(IE->Name))
                                Output.Write("Inquiring BID ", IE->BID, ".");
                            else
                                Output.Write("Inquiring BID ", IE->BID, " for ", IE->Name, "."); 
                        }
                        else
                            if(!strcmp(Function, FunctionStringRestore_cn38))
                            {
                                if(0==IE->DestinationName || 0<strlen(IE->DestinationName))
                                    Output.Write("Restoring ", IE->Name, " with BID ", IE->BID, ".");
                                else
                                    Output.Write("Restoring ", IE->Name, " with BID ", IE->BID, " into ", IE->DestinationName, ".");
                            }
                }
                else
                {
                    rc=0;
                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
                }
            }
        }
    }

    if(!strcmp(Function, FunctionStringBackup_cn38) ||  // if we make a backup
       !strcmp(Function, FunctionStringRestore_cn38))   // or a restore
    {                                                   
        if(TheParamFileInfo.NumberOfStagingAreas<NumberOfPipes) // the number of pipes must not exceed the number of staging areas
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::NotEnoughStagingAreas));
        }
    }

    if(0!=InputFile)
        delete InputFile;

    TheFileList.Reverse();  //reverse the list so that it has the same order as the entries in the infile

    if(rc)
    {
        char LengthAsString[30];

        if(!cn36_IntToStrN(LengthAsString, sizeof(LengthAsString), TheFileList.Length()))
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
        }
        else
            Output.Write("Found ", LengthAsString, 1==TheFileList.Length()?" entry":" entries", " in the input file.");
    }

    Output.Write("");

    return rc;
}

static int cn38_WriteOutputFile(tcn386_BackintOut              & Output,
                                ni31_List<tcn381_OutputLine *> & TheOutputLineList,
                                const char *                     Function)
{
    int rc=1;
    
    ni31_ListElem<tcn381_OutputLine *> *h;

    Output.Open();

    for(h=TheOutputLineList.First(); rc && 0!=h; h=h->Next())
        rc=Output.Write(h->GiveInfo()->Content);

    if(!Output.Close())
        rc=0;

    return rc;
}

static int cn38_CreateOutputForBackupRestoreAndDelete(ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                                      const char *                     FunctionString,
                                                      ni31_List<tcn381_OutputLine *> & OutputLineList,
                                                      int                              NoErrorSoFar)
{
    int rc=1;

    ni31_ListElem<tcn381_InputEntry *> *h;
    tcn381_OutputLine *OL;

    for(h=TheInputFileList.First(); rc && 0!=h; h=h->Next())
    {
        if(h->GiveInfo()->ProcessedSuccessful && NoErrorSoFar)
        {
            tcn381_OutputLine::SuccessType Type;

            if(!strcmp(FunctionStringBackup_cn38, FunctionString))
                Type=tcn381_OutputLine::Saved;
            else
                if(!strcmp(FunctionStringRestore_cn38, FunctionString))
                    Type=tcn381_OutputLine::Restored;
                else
                    if(!strcmp(FunctionStringDelete_cn38, FunctionString))
                        Type=tcn381_OutputLine::NotDeleted;
                    else
                    {
                        rc=0;
                        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
                    }
            
            if(rc)
            {
                if(tcn381_OutputLine::Saved==Type && h->GiveInfo()->IsPipe) //write number of saved bytes for saved pipes
                {
                    OL=new tcn381_OutputLine(Type,
                                      h->GiveInfo()->BID,
                                      h->GiveInfo()->Name,
                                      h->GiveInfo()->BytesWrittenToDisk);
                }
                else
                    OL=new tcn381_OutputLine(Type, h->GiveInfo()->BID, h->GiveInfo()->Name);
            }
        }
        else
            OL=new tcn381_OutputLine(tcn381_OutputLine::Error, h->GiveInfo()->Name);

        if(0==OL || !OL->IsGood)
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
        }
        else
            OutputLineList.Add(OL);
    }

    OutputLineList.Reverse();   //to get the same order in outfile as in infile

    return rc;
}

static int cn38_DoABackup(const tcn38_AParamFileInfo &     TheParamFileInfo,
                          ni31_List<tcn381_InputEntry *> & TheInputFileList,
                          const char *                     UserID,
                          ni31_List<tcn381_OutputLine *> & TheOutputLineList,
                          tcn386_BackintOut              & Output)
{
    int rc;

    tcn381_APipeToFileConverter Converter[MAX_STAGING_AREAS_CN38];
    int StartedProcesses;

    Output.Write("Starting the backup.");

    rc=cn384_StartPipeToFileConverters(StartedProcesses,
                                       TheInputFileList,
                                       Converter,
                                       TheParamFileInfo.StagingAreaName,
                                       TheParamFileInfo.StagingAreaSize,
                                       Output);

    if(rc)
    {
        int NumberOfRunningProcesses=StartedProcesses,
            MaxNumberOfFilesPerBackintCall=TheParamFileInfo.NumberOfFilesPerBackintCall,
            NumberOfFilesOfBackintCall=0,
            NumberOfAnswersAvailable=0;

        while(rc && NumberOfRunningProcesses>0)
        {
            rc=cn384_WaitForAnswersFromPipeToFileConverters(NumberOfAnswersAvailable,
                                                            NumberOfRunningProcesses,  
                                                            MaxNumberOfFilesPerBackintCall,
                                                            NumberOfFilesOfBackintCall,
                                                            StartedProcesses,
                                                            Converter,
                                                            TheParamFileInfo.MaxDelayOfBackintCall,
                                                            Output);

            if(rc && 0<NumberOfAnswersAvailable) // 0==NumberOfAnswersAvailable if and only if all converters have finished
            {
                rc=cn382_UseBackint(TheInputFileList,
                                    TheParamFileInfo.PathOfBackint,
                                    UserID,
                                    FunctionStringBackup_cn38,
                                    1,
                                    NumberOfFilesOfBackintCall,
                                    TheParamFileInfo.PathOfBackintParamFile,
                                    TheParamFileInfo.PathOfBackintInput,
                                    TheParamFileInfo.PathOfBackintOutput,
                                    TheParamFileInfo.PathOfBackintErrorOutput,
                                    Output,
                                    TheParamFileInfo.BI_CALLERValue);

                //remove saved files and tell readers to go on
                if(rc)
                    rc=cn384_RemoveFilesAndTellConverters(StartedProcesses, Converter);

                if(rc)
                    NumberOfAnswersAvailable-=NumberOfFilesOfBackintCall;
            }
        }

        if(NumberOfRunningProcesses>0)  //all converters have to finish voluntary (and succesful)
        {
            if(0!=rc)
            {
                rc=0;
                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::UnexpectedRunningConverters));
            }
            //else do nothing, error is reported already
        }
    }

    if(!cn384_CleanUpConverters(StartedProcesses, TheInputFileList)) //returns warning if unsuccessful
    {
        rc=0;
        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::UnsuccesfulConverterCleanUp));
    }

    //save files of input list
    if(rc)
        rc=cn382_UseBackint(TheInputFileList,
                            TheParamFileInfo.PathOfBackint,
                            UserID,
                            FunctionStringBackup_cn38,
                            0,
                            0,
                            TheParamFileInfo.PathOfBackintParamFile,
                            TheParamFileInfo.PathOfBackintInput,
                            TheParamFileInfo.PathOfBackintOutput,
                            TheParamFileInfo.PathOfBackintErrorOutput,
                            Output,
                            TheParamFileInfo.BI_CALLERValue);

    //write history
    if(!cn383_WriteInformationToHistoryFile(TheParamFileInfo.PathOfHistoryFile, TheInputFileList, UserID))
    {
        if(rc)  //report an history write error only, if no other error was reported before
        {
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::CouldNotWriteHistory)); 
            rc=0;
        }
    }

    //save own history with backint
    if(rc)
    {
        Output.Write("Backing up history file.");

        ni31_List<tcn381_InputEntry *> HistoryAsFileList;
        
        tcn381_InputEntry h(0, TheParamFileInfo.PathOfHistoryFile);

        HistoryAsFileList.Add(&h);

        rc=cn382_UseBackint(HistoryAsFileList,
                            TheParamFileInfo.PathOfBackint,
                            UserID,
                            FunctionStringBackup_cn38,
                            0,
                            0,
                            TheParamFileInfo.PathOfBackintParamFile,
                            TheParamFileInfo.PathOfBackintInput,
                            TheParamFileInfo.PathOfBackintOutput,
                            TheParamFileInfo.PathOfBackintErrorOutput,
                            Output,
                            TheParamFileInfo.BI_CALLERValue);
    }

    if(!cn38_CreateOutputForBackupRestoreAndDelete(TheInputFileList,
                                                   FunctionStringBackup_cn38,
                                                   TheOutputLineList,
                                                   rc))
    {
        rc=0; //error message is done by function
    }

    Output.Write("Finished the backup ", rc?"":"un", "successfully.");
    Output.Write("");

    return rc;
}

static int cn38_DoAInquire(const tcn38_AParamFileInfo &     TheParamFileInfo,
                           ni31_List<tcn381_InputEntry *> & TheInputFileList,
                           const char *                     UserID,
                           ni31_List<tcn381_OutputLine *> & TheOutputLineList,
                           tcn386_BackintOut              & Output)
{
    int rc=1;
    
    ni31_List<tcn381_InputEntry *> BackUpsInQuestionList;

    Output.Write("Starting the inquire.");

    rc=cn383_GetInformationFromOwnHistory(TheParamFileInfo.PathOfHistoryFile,
                                          TheInputFileList,
                                          UserID,
                                          FunctionStringInquire_cn38,
                                          &BackUpsInQuestionList,
                                          Output);

    char Buf[30];

    if(cn36_IntToStrN(Buf, sizeof(Buf), BackUpsInQuestionList.Length()))
        Output.Write("Found ", Buf, " corresponding entries in the history file.\n");

    if(rc)
    {
        //ask backint for the availability of every piece needed for the inquired backups
        rc=cn382_UseBackint(BackUpsInQuestionList,
                            TheParamFileInfo.PathOfBackint,
                            UserID,
                            FunctionStringInquire_cn38,
                            0,
                            0,
                            TheParamFileInfo.PathOfBackintParamFile,
                            TheParamFileInfo.PathOfBackintInput,
                            TheParamFileInfo.PathOfBackintOutput,
                            TheParamFileInfo.PathOfBackintErrorOutput,
                            Output,
                            TheParamFileInfo.BI_CALLERValue);

        //collect output data
        ni31_ListElem<tcn381_InputEntry *> *hIE, // loops through the input list
                                    *hHE; // loops through the list of historical backups

        tcn381_OutputLine * OutLine;
        ni31_List<tcn381_OutputLine *> OutPutListForInputEntry; 
        int Found;

        for(hIE=TheInputFileList.First(); rc && 0!=hIE; hIE=hIE->Next()) // test every input against all BackupsInQuestion
        {
            Found=0;

            for(hHE=BackUpsInQuestionList.First(); rc && 0!=hHE; hHE=hHE->Next())
            {
                if(hIE->GiveInfo()->EqualsToBIDAndName(*(hHE->GiveInfo())) &&
                   hHE->GiveInfo()->FoundInThisBackintOutput)
                {
                    Found++;

                    if(!strcmp(hIE->GiveInfo()->BID, "#NULL") && '\0'==hIE->GiveInfo()->Name[0])
                        OutLine=new tcn381_OutputLine(tcn381_OutputLine::Found, hHE->GiveInfo()->BID);
                    else
                        OutLine=new tcn381_OutputLine(tcn381_OutputLine::Found, hHE->GiveInfo()->BID, hHE->GiveInfo()->Name);
                    
                    if(0==OutLine || !OutLine->IsGood)
                    {
                        rc=0;
                        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
                    }
                    else
                        OutPutListForInputEntry.Add(OutLine);
                }
            }

            if(0==Found) // if none or no complete backup was found for the current input, tell it to the world
            {
                if('\0'!=hIE->GiveInfo()->Name[0]) //the Interface definition says nothing about this case 
                {
                    OutLine=new tcn381_OutputLine(tcn381_OutputLine::NotFound, hIE->GiveInfo()->Name);

                    if(0==OutLine || !OutLine->IsGood)
                    {
                        rc=0;
                        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
                    }
                    else
                        TheOutputLineList.Add(OutLine);
                }
            }
            else
            {
                //remove duplicate entries if only BID's should be reported
                if(!strcmp(hIE->GiveInfo()->BID, "#NULL") && '\0'==hIE->GiveInfo()->Name[0])
                {
                    ni31_ListElem<tcn381_OutputLine*> *h1,
                                               *h2;

                    for(h1=OutPutListForInputEntry.First(); 0!=h1; h1=h1->Next())
                    {
                        h2=h1;
                        while(0!=h2->Next())
                        {
                            if(!strcmp(h1->GiveInfo()->Content, h2->Next()->GiveInfo()->Content))
                            {
                                delete h2->Next()->GiveInfo();
                                OutPutListForInputEntry.Del(*h2);
                            }
                            else
                                h2=h2->Next();
                        }
                    }
                
                }

                //move the output of the current input line the the output list in the right order
                OutPutListForInputEntry.MoveAllToBeginOf(TheOutputLineList);
            }
        }
    }

    TheOutputLineList.Reverse();    //unsure that we get the same order as in infile and most recent backups first for same BID or same pipe/file name

    ni31_ListElem<tcn381_InputEntry *> *h;

    //free memory allocated in this function
    for(h=BackUpsInQuestionList.First(); 0!=h; h=h->Next())
        delete h->GiveInfo();

    Output.Write("Finished the inquire ", rc?"":"un", "successfully.");
    Output.Write("");

    return rc;
}

static void cn38_CleanUp(ni31_List<tcn381_InputEntry *> & TheInputFileList,
                         ni31_List<tcn381_OutputLine *> & TheOutputLineList)
{
    ni31_ListElem<tcn381_InputEntry *> *h;

    for(h=TheInputFileList.First(); 0!=h; h=h->Next())
        delete (h->GiveInfo());

    ni31_ListElem<tcn381_OutputLine *> *hOL;

    for(hOL=TheOutputLineList.First();  0!=hOL; hOL=hOL->Next())
        delete (hOL->GiveInfo());
}

static int cn38_FoundInformationForAllEntries(ni31_List<tcn381_InputEntry *> & TheInputFileList)
{
    int rc=1;
    ni31_ListElem<tcn381_InputEntry *> *h;
    tcn381_InputEntry * IE;

    for(h=TheInputFileList.First(); rc && 0!=h; h=h->Next())
    {
        IE=h->GiveInfo();

        if((0==IE->StageName || '\0'==IE->StageName[0]) || //the stage name must exists and not be empty
           (IE->IsPipe && 0>=IE->StageSize) ||             //if it's a pipe the StageSize must be greater than 0 
           (!IE->IsPipe && 0!=IE->StageSize) ||            //if it's not a pipe the StageSize must be 0
           0>=IE->NumberOfParts ||                         //more than none data part is required
           IE->ListOfPartialBIDs.IsEmpty())                //more than none data part has also more than none BID
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InsufficientHistoryInformation));
        }
    }
    
    return rc;
}

static int cn38_ShareStagingAreasForInputEntries(ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                                 int                              NumberOfStagingAreas,
                                                 const char * const               StagingAreaName[MAX_STAGING_AREAS_CN38],
                                                 const teo00_Uint8                StagingAreaSize[MAX_STAGING_AREAS_CN38])
{
    int rc=1;

    ni31_ListElem<tcn381_InputEntry *> *h;
    tcn381_InputEntry * IE;

    int NumberOfPipes=0,
        i,
        j;

    for(h=TheInputFileList.First(); 0!=h; h=h->Next())
        if(h->GiveInfo()->IsPipe)
            NumberOfPipes++;

    int StageInUse[MAX_STAGING_AREAS_CN38];

    for(i=0; i<MAX_STAGING_AREAS_CN38; i++)
        StageInUse[i]=0;

    int FoundAllStagesWithSameName=1;

    h=TheInputFileList.First();

    while(FoundAllStagesWithSameName && rc && 0!=h)
    {
        while(0!=h && !h->GiveInfo()->IsPipe)
            h=h->Next();

        if(0!=h)
        {
            tcn381_InputEntry * IE=h->GiveInfo();

            int FoundStageWithSameName=0;

            for(j=0; !FoundStageWithSameName && j<NumberOfStagingAreas; j++)
            {
                if(!StageInUse[j] &&
                   0==strcmp(StagingAreaName[j], IE->StageName) &&
                   StagingAreaSize[j]>=IE->StageSize)
                {
                    if(cn36_StrAlloc(IE->RestoreStageName, strlen(StagingAreaName[j])+20+1))
                    {
                        SAPDB_strcpy(IE->RestoreStageName, StagingAreaName[j]);

                        FoundStageWithSameName=1;
                        StageInUse[j]=1;
                    }
                    else
                        rc=0;
                }
            }
            
            FoundAllStagesWithSameName=FoundStageWithSameName;

            h=h->Next();
        }
    }

    //The pipes were backed up with a certain maximal size, so we need a match of files and available stages,
    //were the size of an available stage is equal or greater than the size of the file assigned to it.
    //If such a match of file sizes to available stages exists, it can be found by ordering file sizes and stage sizes
    //asscending and by assigning file sizes to stages in that order leaving out stages that do not match the current
    //file size

    if(!FoundAllStagesWithSameName) //something has changed in the configuration, so that original stages can not be used anymore
    {
        for(i=0; i<MAX_STAGING_AREAS_CN38; i++) //begin from the start again
            StageInUse[i]=0;

        for(h=TheInputFileList.First(); 0!=h; h=h->Next())  //and clear also the RestoreStageName's
            if(h->GiveInfo()->IsPipe)
                cn36_StrDealloc(h->GiveInfo()->RestoreStageName);

        for(i=0; rc && i<NumberOfPipes; i++)
        {
            tcn381_InputEntry * MinIE=0;
            teo00_Uint8 MinNeededSize=MAX_UINT8_CN36;

            for(h=TheInputFileList.First(); 0!=h; h=h->Next())
            {
                IE=h->GiveInfo();

                if(IE->IsPipe &&                   //only pipes need stages 
                   0==IE->RestoreStageName &&      //no stage assigned to this pipe so far
                   MinNeededSize>=IE->StageSize)   //we look for the minimal needed stage size
                {
                    MinNeededSize=IE->StageSize;
                    MinIE=IE;
                }
            }

            int  MinJ=-1;
            teo00_Uint8 MinFoundSize=MAX_UINT8_CN36;

            for(j=0; j<NumberOfStagingAreas; j++)
            {
                if(!StageInUse[j] &&                    //the stage is not assigned jet
                   0!=StagingAreaName[j] &&             //the stage is useful
                   StagingAreaSize[j]>=MinNeededSize && //the size of the staging area is big enough
                   StagingAreaSize[j]<=MinFoundSize)    //we look for the smallest stage with the above criteria
                {
                    MinJ=j;
                    MinFoundSize=StagingAreaSize[j];
                }
            }

            if(0==MinIE)
            {
                rc=0;
                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::NotEnoughStagingAreas));
            }
            else     
                if(-1==MinJ)
                {
                    rc=0;
                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::NoFittingStagingArea));
                }

            if(rc)
            {
                const char * OriginalFileName=cn36_BaseNameFrom(MinIE->StageName);
            
                if(cn36_StrAlloc(MinIE->RestoreStageName, strlen(StagingAreaName[MinJ])+strlen(OriginalFileName)+20+1))
                {
                    StageInUse[MinJ]=1;

                    SAPDB_strcpy(MinIE->RestoreStageName, StagingAreaName[MinJ]); //there is enough memory for SAPDB_strcpy, because we did a new char [strlen()+strlen()+...]
                    *cn36_BaseNameFrom(MinIE->RestoreStageName)='\0';
                    strcat(MinIE->RestoreStageName, OriginalFileName);
                }
                else
                    rc=0;
            }
        }
    }

    return rc;
}

static int cn38_CheckStagingAreasOfInputEntries(ni31_List<tcn381_InputEntry *> & TheInputFileList)
{
    int rc=1;
    
    ni31_ListElem<tcn381_InputEntry *> * h,*h2;
    tcn381_InputEntry * IE;

    //every pipe must have a restore stage 
    for(h=TheInputFileList.First(); rc && 0!=h; h=h->Next())
    {
        IE=h->GiveInfo();

        if(IE->IsPipe && 0==IE->RestoreStageName)
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::NoStageForPipe));
        }
    }

    //check for duplicate destination stages
    for(h=TheInputFileList.First(); rc && 0!=h; h=h->Next())
    {
        if(h->GiveInfo()->IsPipe)
            for(h2=h->Next(); rc && 0!=h2; h2=h2->Next())
            {
                if(h2->GiveInfo()->IsPipe &&
                   !strcmp(h->GiveInfo()->RestoreStageName, h2->GiveInfo()->RestoreStageName))
                {
                    rc=0;
                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::SameRestoreStage));
                }
            }
    }

    //don't use the destination feature, if it is not necessary
    for(h=TheInputFileList.First(); rc && 0!=h; h=h->Next())
    {
        IE=h->GiveInfo();
        
        if(IE->IsPipe && !strcmp(IE->StageName, IE->RestoreStageName))
        {
            delete [] IE->RestoreStageName;
            IE->RestoreStageName=0;
        }
    }

    return rc;
}

static int cn38_InitConverterObjects(ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                     tcn381_APipeToFileConverter      Converter[MAX_STAGING_AREAS_CN38],
                                     int &                            AnswersAvailable,
                                     int                              UseFinalDirectoryDelimiterOnRestore)
{
    int rc=1,
        i;
    
    ni31_ListElem<tcn381_InputEntry *> * h;
    tcn381_InputEntry * IE;

    AnswersAvailable=0;

    for(h=TheInputFileList.First(), i=0; rc && 0!=h; h=h->Next())
    {
        IE=h->GiveInfo();

        if(IE->IsPipe)
        {
            IE->Converter=&Converter[i];
            Converter[i].State=tcn381_APipeToFileConverter::StageIsFree;

            AnswersAvailable++;

            Converter[i].CompleteStageName=new char [strlen(IE->StageName)+21+1];

            if(0==Converter[i].CompleteStageName)
            {
                rc=0;
                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
            }

            if(rc && 0!=IE->RestoreStageName)
            {
                Converter[i].DestinationStageDirectory=new char [strlen(IE->RestoreStageName)+1];
                Converter[i].CompleteDestinationStage=new char [strlen(IE->RestoreStageName)+1];

                if(0==Converter[i].DestinationStageDirectory || 0==Converter[i].CompleteDestinationStage)
                {
                    rc=0;
                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
                }
            }

            if(rc) // if wo got all the memory we need ...
            {
                SAPDB_strcpy(Converter[i].CompleteStageName, IE->StageName);
                strcat(Converter[i].CompleteStageName, ".");
                Converter[i].EndOfStageName=cn36_TerminatingZeroOf(Converter[i].CompleteStageName);

                if(0!=IE->RestoreStageName)
                {
                    SAPDB_strcpy(Converter[i].DestinationStageDirectory, IE->RestoreStageName);   //copy the complete stage name and determine the directory in it

                    if(UseFinalDirectoryDelimiterOnRestore)
                        *cn36_BaseNameFrom(Converter[i].DestinationStageDirectory)='\0';    //the last / or \ is included in the directory name
                    else
                        *cn36_EndOfDirectory(Converter[i].DestinationStageDirectory)='\0';  //the last / or \ is not included in the directory name (some Backint for Oracle implementations need that)

                    SAPDB_strcpy(Converter[i].CompleteDestinationStage, IE->RestoreStageName);
                    strcat(Converter[i].CompleteDestinationStage, ".");
                    Converter[i].EndOfCompleteDestinationStage=cn36_TerminatingZeroOf(Converter[i].CompleteDestinationStage);
                }
            }

            i++;
        }
        
        if(MAX_STAGING_AREAS_CN38<i)
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::NotEnoughStagingAreas));
        }
    }

    return rc;
}

static int cn38_RemoveFiles(int                              StartedProcesses,
                            ni31_List<tcn381_InputEntry *> & TheInputFileList)
{
    int rc=1;

    ni31_ListElem<tcn381_InputEntry *> *h;
    tcn381_APipeToFileConverter * Converter;

    for(h=TheInputFileList.First(); rc && 0!=h; h=h->Next())
        if(h->GiveInfo()->IsPipe)
        {
            Converter=h->GiveInfo()->Converter;

            if(0!=Converter->Process &&
               tcn381_APipeToFileConverter::ConverterFinishedStage==Converter->State)
            {
                const char * CurrentCompleteStageFileName;

                if(0==Converter->CompleteDestinationStage)
                    CurrentCompleteStageFileName=Converter->CompleteStageName;
                else
                    CurrentCompleteStageFileName=Converter->CompleteDestinationStage;

                tni34_AFile Stage(CurrentCompleteStageFileName, tni34_AFile::ToDiskAppend, 0, 0);

                if(!Stage.IsGood() || !Stage.Exists() || !Stage.Remove())
                {
                    rc=0;
                    TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::StageIsNotExisting));
                }
                else
                {
                    Converter->State=tcn381_APipeToFileConverter::StageIsFree;
                    Converter->InputInfo->RestorePart++;
                }
            }
        }

    return rc;
}

void cn38_AdjustDestinationNamesForFiles(ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                         int                              UseFinalDirectoryDelimiterOnRestore)
{
    int rc=1;

    if(!UseFinalDirectoryDelimiterOnRestore)    //if we have to avoid / or \ at the end of a destination directory for files (some Backint for Oracle do not like it)
    {
        ni31_ListElem<tcn381_InputEntry *> * h;
        tcn381_InputEntry                  * IE;    //used as usual as short cut for h->GiveInfo()

        for(h=TheInputFileList.First(); rc && 0!=h; h=h->Next())
        {
            IE=h->GiveInfo();

            if(!IE->IsPipe &&           //only adjust DestinationNames of Pipes and
               0!=IE->DestinationName)  //only adjust something, if something is there
            {
                if(0==strlen(cn36_BaseNameFrom(IE->DestinationName)))   //if the DestinationName ends with a / on UNIX or a '\' on NT ...
                    *cn36_EndOfDirectory(IE->DestinationName)='\0';     //... end DestinationName in front of that / or '\'
            }
        }                       
    }
}

static int cn38_DoARestore(const tcn38_AParamFileInfo &     TheParamFileInfo,
                           ni31_List<tcn381_InputEntry *> & TheInputFileList,
                           const char *                     UserID,
                           ni31_List<tcn381_OutputLine *> & TheOutputLineList,
                           tcn386_BackintOut              & Output)
{
    int rc;

    Output.Write("Starting the restore.");

    rc=cn383_GetInformationFromOwnHistory(TheParamFileInfo.PathOfHistoryFile,
                                          TheInputFileList,
                                          UserID,
                                          FunctionStringRestore_cn38,
                                          0,
                                          Output);

    if(rc)
        rc=cn38_FoundInformationForAllEntries(TheInputFileList);

    if(rc)
        cn38_AdjustDestinationNamesForFiles(TheInputFileList, TheParamFileInfo.UseFinalDirectoryDelimiterOnRestore);

    if(rc)
        rc=cn38_ShareStagingAreasForInputEntries(TheInputFileList,
                                                 TheParamFileInfo.NumberOfStagingAreas,
                                                 TheParamFileInfo.StagingAreaName,
                                                 TheParamFileInfo.StagingAreaSize);

    if(rc)
        rc=cn38_CheckStagingAreasOfInputEntries(TheInputFileList);

    if(rc)
    {
        tcn381_APipeToFileConverter Converter[MAX_STAGING_AREAS_CN38];

        int End=0,
            TotalyNeededConverters=0,
            EndedConverters=0,
            ConvertersFinishedWork=0,
            StartedConverters=0,
            MaxNumberOfFilesPerBackintCall=TheParamFileInfo.NumberOfFilesPerBackintCall,
            NumberOfFilesPerBackintCall,
            NumberOfAnswersAvailable=0;
        
        rc=cn38_InitConverterObjects(TheInputFileList,
                                     Converter,
                                     TotalyNeededConverters,
                                     TheParamFileInfo.UseFinalDirectoryDelimiterOnRestore);

        NumberOfAnswersAvailable=TotalyNeededConverters;

        if(0==NumberOfAnswersAvailable) // no element of TheInputFileList is a pipe
            End=1;

        if(NumberOfAnswersAvailable<MaxNumberOfFilesPerBackintCall)    // can not use more files, than stages
            MaxNumberOfFilesPerBackintCall=NumberOfAnswersAvailable;   // correct if necessary

        NumberOfFilesPerBackintCall=MaxNumberOfFilesPerBackintCall;

        while(rc && !End)
        {
            if(0<NumberOfAnswersAvailable)
            {
                rc=cn382_UseBackint(TheInputFileList,
                                    TheParamFileInfo.PathOfBackint,
                                    UserID,
                                    FunctionStringRestore_cn38,
                                    1,
                                    NumberOfFilesPerBackintCall,
                                    TheParamFileInfo.PathOfBackintParamFile,
                                    TheParamFileInfo.PathOfBackintInput,
                                    TheParamFileInfo.PathOfBackintOutput,
                                    TheParamFileInfo.PathOfBackintErrorOutput,
                                    Output,
                                    TheParamFileInfo.BI_CALLERValue);

                if(rc)
                    rc=cn384_StartNeededPipeToFileConverters(StartedConverters,
                                                             TheInputFileList,
                                                             Converter,
                                                             TheParamFileInfo.StagingAreaName,
                                                             Output);
            }

            if(rc && 0<(StartedConverters-EndedConverters))
                rc=cn384_TellRunningConverters(StartedConverters, TheInputFileList);

            NumberOfAnswersAvailable-=NumberOfFilesPerBackintCall;

            if(rc)
            {
                rc=cn384_WaitForAnswersFromPipeToFileConverters(NumberOfAnswersAvailable,
                                                                TotalyNeededConverters,
                                                                StartedConverters,
                                                                ConvertersFinishedWork,
                                                                EndedConverters,
                                                                MaxNumberOfFilesPerBackintCall,
                                                                NumberOfFilesPerBackintCall,
                                                                TheInputFileList,
                                                                TheParamFileInfo.MaxDelayOfBackintCall,
                                                                Output);

                //remove restored files and tell converters to go on
                if(rc)
                    rc=cn38_RemoveFiles(StartedConverters, TheInputFileList);
            }

            if(EndedConverters>=TotalyNeededConverters) // use >= instead of == , just to be sure
                End=1;
        }

        if(rc && EndedConverters<TotalyNeededConverters)  //if all is ok so far, all converters should have to finish voluntary at this point
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::UnexpectedRunningConverters));
        }

        if(!cn384_CleanUpConverters(StartedConverters, TheInputFileList)) //returns warning if unsuccessful
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::UnsuccesfulConverterCleanUp));
        }

        //restore files of input list
        if(rc)    
            rc=cn382_UseBackint(TheInputFileList,
                                TheParamFileInfo.PathOfBackint,
                                UserID,
                                FunctionStringRestore_cn38,
                                0,
                                0,
                                TheParamFileInfo.PathOfBackintParamFile,
                                TheParamFileInfo.PathOfBackintInput,
                                TheParamFileInfo.PathOfBackintOutput,
                                TheParamFileInfo.PathOfBackintErrorOutput,
                                Output,
                                TheParamFileInfo.BI_CALLERValue);
    }

    //construct the output
    if(!cn38_CreateOutputForBackupRestoreAndDelete(TheInputFileList,
                                                   FunctionStringRestore_cn38,
                                                   TheOutputLineList,
                                                   rc))
    {
        rc=0; //error message is done by function
    }

    Output.Write("Finished the restore ", rc?"":"un", "successfully.");
    Output.Write("");

    return rc;
}

static int cn38_DoADelete(ni31_List<tcn381_InputEntry *> & TheInputFileList,
                          ni31_List<tcn381_OutputLine *> & TheOutputLineList)
{
    return cn38_CreateOutputForBackupRestoreAndDelete(TheInputFileList,
                                                      FunctionStringDelete_cn38,
                                                      TheOutputLineList,
                                                      1);
}

int main(int argc, const char * const * argv)
{
    int rc;

    tcn385_Backint2ErrorHandler TheErrorHandler;
    TheErrorHandler.SetNewErrorHandler(&TheErrorHandler);

    ni32_DoubleParameter UserIDParameter("-u", ni32_AParameter::Required);
    ni32_RestrictedDoubleParameter FunctionParameter("-f",
                                                     ni32_RestrictedDoubleParameter::TempValueList<<FunctionStringDelete_cn38
                                                                                                  <<FunctionStringRestore_cn38
                                                                                                  <<FunctionStringInquire_cn38
                                                                                                  <<FunctionStringBackup_cn38);
    ni32_RestrictedDoubleParameter TypeParameter("-t", ni32_RestrictedDoubleParameter::TempValueList<<"file");
    ni32_DoubleParameter ParameterFileParameter("-p");
    ni32_DoubleParameter InFileParameter("-i");
    ni32_DoubleParameter OutFileParameter("-o");
    ni32_SingleParameter AttendedModeParameter("-c");

    tcn38_ThisProgramParameters Parameters(argc,
                                           argv,
                                           ni32_ProgramParameters::TempParamList<<&UserIDParameter
                                                                                <<&FunctionParameter
                                                                                <<&TypeParameter
                                                                                <<&ParameterFileParameter
                                                                                <<&InFileParameter
                                                                                <<&OutFileParameter
                                                                                <<&AttendedModeParameter);

    rc=TheErrorHandler.SetOutput(OutFileParameter.IsSet() ? OutFileParameter.GiveValue():0);

    tcn38_AParamFileInfo           TheParamFileInfo;
    ni31_List<tcn381_InputEntry *> TheInputFileList;
    ni31_List<tcn381_OutputLine *> TheOutputLineList;
    
    tcn386_BackintOut Output;

    if(rc)
        rc=Parameters.Check();

    if(rc)
        rc=Output.SetOutputTo(OutFileParameter.IsSet() ? OutFileParameter.GiveValue():0);

    if(rc)
    {
        rc=cn38_ReadParameterFile(ParameterFileParameter.IsSet()?ParameterFileParameter.GiveValue():0,
                                  TheParamFileInfo,
                                  Output);
        if(rc)
        {
            if(rc=cn38_ReadInputFile(InFileParameter.IsSet() ? InFileParameter.GiveValue():0,
                                     TheParamFileInfo,
                                     TheInputFileList,
                                     FunctionParameter.GiveValue(),
                                     Output))
            {
                if(!strcmp(FunctionParameter.GiveValue(), FunctionStringBackup_cn38))
                {
                    rc=cn38_DoABackup(TheParamFileInfo,
                                      TheInputFileList,
                                      UserIDParameter.GiveValue(),
                                      TheOutputLineList,
                                      Output);
                }
                else
                    if(!strcmp(FunctionParameter.GiveValue(), FunctionStringRestore_cn38))
                    {
                        rc=cn38_DoARestore(TheParamFileInfo,
                                           TheInputFileList,
                                           UserIDParameter.GiveValue(),
                                           TheOutputLineList,
                                           Output);
                    }
                    else
                        if(!strcmp(FunctionParameter.GiveValue(), FunctionStringInquire_cn38))
                        {
                            rc=cn38_DoAInquire(TheParamFileInfo,
                                               TheInputFileList,
                                               UserIDParameter.GiveValue(),
                                               TheOutputLineList,
                                               Output);
                        }
                        else
                            if(!strcmp(FunctionParameter.GiveValue(), FunctionStringDelete_cn38))
                            {
                                rc=cn38_DoADelete(TheInputFileList, TheOutputLineList);
                            }
                            else
                            {
                                rc=0;
                                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
                            }

                cn38_WriteOutputFile(Output,
                                     TheOutputLineList,
                                     FunctionParameter.GiveValue());
            }
        }
    }

    cn38_CleanUp(TheInputFileList, TheOutputLineList);

    //adjust rc to the rules of backint
    if(rc)
        rc=0;   // 0 == all OK
    else
        rc=2;   // 2 == error occured

    return rc;
}
