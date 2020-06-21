/*!
  \file     DBMP2F_Main.cpp
  \author   TiloH
  \ingroup  pipe2file
  \brief    main file and parameter handling for pipe2file
  
  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include <stdio.h>

#include "hcn36.h"
#include "hni32.h"

#include "SAPDB/DBM/Pipe2File/DBMP2F_TransferController.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_CompareController.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_Pipe.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_File.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_VersionedFile.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_Null.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_Test.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_Event.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_ReactionDelay.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_ReactionWait.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_ReactionSignalDone.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_ReactionRemoveFile.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_ReactionShowSpeed.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_ReactionMessage.hpp"


// -----------------------------------------------------------------------------
// class DBMP2F_Parameters
// -----------------------------------------------------------------------------

class DBMP2F_Parameters: public ni32_ProgramParameters
{
  public:
    DBMP2F_Parameters(int                              argc,
                      const char * const             * argv,
                      ni32_RestrictedDoubleParameter & TheDirection,
                      ni32_DoubleParameter           & TheFileName,
                      ni32_DoubleParameter           & ThePipeName,
                      ni32_DoubleParameter           & TheFileSize,
                      ni32_SingleParameter           & TheNoWait,
                      ni32_SingleParameter           & TheHelp,
                      ni31_List<ni32_AParameter *>   & ParamList);

    ~DBMP2F_Parameters();

    virtual const char * UsageMessage();
    int UsageMessageAvailable();

    int ExtendedParameterCheck();

  private:
    const char                     * CurrentProgramName;
    char                           * UsageString;
    int                              UsageStringConstructed;
    int                              ArgC;
    ni32_RestrictedDoubleParameter & Direction;
    ni32_DoubleParameter           & FileName;
    ni32_DoubleParameter           & PipeName;
    ni32_DoubleParameter           & FileSize;
    ni32_SingleParameter           & NoWait;
    ni32_SingleParameter           & Help;
};


// -----------------------------------------------------------------------------
// class DBMP2F_ControllerConstructor
// -----------------------------------------------------------------------------

class DBMP2F_ControllerConstructor
{
  public:
    DBMP2F_ControllerConstructor(ni32_RestrictedDoubleParameter Direction,
                                 unsigned int                   BufferSize,
                                 const char *                   FileName,
                                 teo00_Uint8                    FileSizeAsInt,
                                 const char *                   PipeName,
                                 int                            ReadOnly,
                                 int                            DelayTime,
                                 teo00_Uint8                    DelayPoint,
                                 int                            DoNotWait,
                                 int                            RemoveFiles,
                                 int                            NumberOfWaits,
                                 teo00_Uint8                    WaitAfter,
                                 int                            ShowSpeed);

    int                 IsGood();
    DBMP2F_Controller * GiveConstructedController();

    static const char * const Directions[];

  private:
    DBMP2F_Controller * ConstructedController;
    int IsGoodFlag;
};


// -----------------------------------------------------------------------------
// member functions DBMP2F_Parameters
// -----------------------------------------------------------------------------

DBMP2F_Parameters::DBMP2F_Parameters(int                              argc,
                                     const char * const             * argv,
                                     ni32_RestrictedDoubleParameter & TheDirection,
                                     ni32_DoubleParameter           & TheFileName,
                                     ni32_DoubleParameter           & ThePipeName,
                                     ni32_DoubleParameter           & TheFileSize,
                                     ni32_SingleParameter           & TheNoWait,
                                     ni32_SingleParameter           & TheHelp,
                                     ni31_List<ni32_AParameter *>   & ParamList)
    :ni32_ProgramParameters(argc, argv, ParamList),
     CurrentProgramName(argv[0]),
     UsageString(0),
     UsageStringConstructed(0),
     ArgC(argc),
     Direction(TheDirection),
     FileName(TheFileName),
     PipeName(ThePipeName),
     FileSize(TheFileSize),
     NoWait(TheNoWait),
     Help(TheHelp)
{
}

DBMP2F_Parameters::~DBMP2F_Parameters()
{
    if(UsageStringConstructed)
        delete [] UsageString;
}

const char * DBMP2F_Parameters::UsageMessage()
{
    if(0==UsageString)
    {
        static char ConstUsageString[]=//"usage: pipe2file <options> [-d file2pipe|pipe2file|test2pipe] [-s <max_file_size>]\n"
                                       "usage: pipe2file [-d file2pipe|pipe2file|test2pipe|compare] <options>\n"
                                       "\n"
                                       "    <options>:\n"
                                       "        -f <file_name>                                      (name of the file)\n"
                                       "        -p <pipe_name>                                      (name of the pipe)\n"
                                       "        -s <max_file_size>\n"
                                       "        -b <buffer_size>\n"
                                       "        -nowait\n"
                                       "        -readonly\n"
                                       "        -removefiles\n"
                                       "        -delay <delay_time> <delay_after>\n"
                                       "        -wait <number_waits> <after_bytes>\n"
                                       "        -showpseed\n"
                                       "\n"
                                       "    <max_file_size>:\n"
                                       "        An integer specifying the maximal size of the files processed. Only if\n"
                                       "        a maximal file size is specified, consecutive numbers are added to the\n"
                                       "        <file_name>.\n"
                                       "\n"
                                       "    <buffer_size>:\n"
                                       "        Size in bytes of the internal buffer used by pipe2file to store the\n"
                                       "        transfered data between reading and writing\n";

        const char * RelativCurrentProgramName=cn36_BaseNameFrom(/*FileNameInPath(*/CurrentProgramName);

        if(0==RelativCurrentProgramName || !strcmp(RelativCurrentProgramName, "pipe2file"))
            UsageString=ConstUsageString;
        else
        {
            char * ProgramName=ConstUsageString+strlen("usage: ");
            char * Options    =ProgramName+strlen("pipe2file");

            *ProgramName='\0';

            size_t Length=strlen(ConstUsageString)+strlen(RelativCurrentProgramName)+1+strlen(Options);

            UsageString=new char [Length+1];

            if(0==UsageString)
            {
                *ProgramName='p';
                UsageString=ConstUsageString;
            }
            else
            {
                UsageStringConstructed=1;
                sprintf(UsageString, "%s%s%s", ConstUsageString, RelativCurrentProgramName, Options);
            }
        }
    }

    return UsageString;
}

int DBMP2F_Parameters::UsageMessageAvailable(){return 1;}

int DBMP2F_Parameters::ExtendedParameterCheck()
{
    int rc=1;

    if(0==strcmp(Direction.GiveValue(), DBMP2F_ControllerConstructor::Directions[0]) &&  //"pipe2file"
       !PipeName.IsSet())
    {
        rc=0;
    }

    if(0==strcmp(Direction.GiveValue(), DBMP2F_ControllerConstructor::Directions[1]) &&  //"file2pipe"
       !FileName.IsSet())
    {
        rc=0;
    }

    if(0==strcmp(Direction.GiveValue(), DBMP2F_ControllerConstructor::Directions[2]))    //"test2pipe"
    {
        if(!PipeName.IsSet() || !FileSize.IsSet())
            rc=0;
        else
            NoWait.TakeValue("-nowait");    // the "-nowait" is not necessary
    }

    if(0==strcmp(Direction.GiveValue(), DBMP2F_ControllerConstructor::Directions[3]) &&  //"compare"
       (!PipeName.IsSet() || !FileName.IsSet()))
    {
        rc=0;
    }

    if(1>=ArgC || Help.IsSet())
        rc=0;

    if(!rc)
        fprintf(stderr, "%s\n", (const char *)UsageMessage());

    return rc;
}

int DBMP2F_Main_ReportOutOfMemory()
{
    fprintf(stderr, "Out of memory.\n");
    return 0;
}


// -----------------------------------------------------------------------------
// member functions DBMP2F_ControllerConstructor
// -----------------------------------------------------------------------------

const char * const DBMP2F_ControllerConstructor::Directions[]={"pipe2file", "file2pipe", "test2pipe", "compare", ""};

DBMP2F_ControllerConstructor::DBMP2F_ControllerConstructor(ni32_RestrictedDoubleParameter Direction,
                                                           unsigned int                   BufferSize,
                                                           const char *                   FileName,
                                                           teo00_Uint8                    FileSizeAsInt,
                                                           const char *                   PipeName,
                                                           int                            ReadOnly,
                                                           int                            DelayTime,
                                                           teo00_Uint8                    DelayPoint,
                                                           int                            DoNotWait,
                                                           int                            DoRemoveFiles,
                                                           int                            NumberOfWaits,
                                                           teo00_Uint8                    WaitAfter,
                                                           int                            ShowSpeed)
  :IsGoodFlag(1),
  ConstructedController(0)
{
    const char *    DirectionString;
    int             DirectionNumber;
    DBMP2F_Device * InitialDevice1=0;
    DBMP2F_Device * InitialDevice2=0;
    DBMP2F_Event  * DelayEvent=0;
    DBMP2F_Event  * EndOfSubDeviceEvent=0;
    DBMP2F_Event  * WaitEvent=0;
    DBMP2F_Event  * ShowSpeedEvent=0;
    DBMP2F_Event  * SourcesDifferEvent=0;

    if(Direction.IsSet())
        DirectionString=Direction.GiveValue();
    else
        DirectionString=Directions[0];

    DirectionNumber=0;

    while('\0'!=Directions[DirectionNumber][0] && 0!=strcmp(Directions[DirectionNumber], DirectionString))
        DirectionNumber++;

    if('\0'==Directions[DirectionNumber][0])
        DirectionNumber=0;

    if((!DoNotWait || DoRemoveFiles) && (0==DirectionNumber || 1==DirectionNumber || 3==DirectionNumber)) //file are involved and we should wait after a file or it should be removed
    {
        EndOfSubDeviceEvent=new DBMP2F_Event(DBMP2F_Event::AEndOfSubDevice);

        if(0==EndOfSubDeviceEvent)
            IsGoodFlag=DBMP2F_Main_ReportOutOfMemory();
    }

    if(IsGoodFlag && 0!=EndOfSubDeviceEvent)
    {
        DBMP2F_Reaction * Wait=      DoNotWait?0:(new DBMP2F_ReactionWait());
        DBMP2F_Reaction * SignalDone=DoNotWait?0:(new DBMP2F_ReactionSignalDone());
        DBMP2F_Reaction * RemoveFile=DoRemoveFiles?(new DBMP2F_ReactionRemoveFile()):0;

        if((!DoNotWait && 0==Wait) ||
           (!DoNotWait && !EndOfSubDeviceEvent->AddReaction(Wait)) ||
           (!DoNotWait && 0==SignalDone) ||
           (!DoNotWait && !EndOfSubDeviceEvent->AddReaction(SignalDone)) ||
           (DoRemoveFiles && 0==RemoveFile) ||
           (DoRemoveFiles && !EndOfSubDeviceEvent->AddReaction(RemoveFile)))
        {
            IsGoodFlag=DBMP2F_Main_ReportOutOfMemory();
        }
    }

    if(IsGoodFlag)
    {
        switch(DirectionNumber)
        {
            case 0: //"pipe2file"
            {
                if(0!=PipeName && '\0'!=PipeName[0])
                    InitialDevice1=new DBMP2F_Pipe(PipeName, DBMP2F_DeviceDirection::Read);
                else
                    IsGoodFlag=0;

                if(0!=InitialDevice1)
                {
                    if(ReadOnly)
                        InitialDevice2=new DBMP2F_Null();
                    else
                        if(0!=FileName && '\0'!=FileName[0])
                        {
                            if(0==FileSizeAsInt)
                                InitialDevice2=new DBMP2F_File(FileName, DBMP2F_DeviceDirection::Write);
                            else
                                InitialDevice2=new DBMP2F_VersionedFile(FileName, DBMP2F_DeviceDirection::Write, FileSizeAsInt, EndOfSubDeviceEvent);
                        }
                        else
                            IsGoodFlag=0;   //we should not get here
                }
            }
            break;

            case 1: //"file2pipe"
            {
                if(0!=FileName && '\0'!=FileName[0])
                {
                    if(0==FileSizeAsInt)
                        InitialDevice1=new DBMP2F_File(FileName, DBMP2F_DeviceDirection::Read);
                    else
                        InitialDevice1=new DBMP2F_VersionedFile(FileName, DBMP2F_DeviceDirection::Read, FileSizeAsInt, EndOfSubDeviceEvent);
                }
                else
                    IsGoodFlag=0;

                if(0!=InitialDevice1)
                {
                    if(ReadOnly)
                        InitialDevice2=new DBMP2F_Null();
                    else
                        if(0!=PipeName && '\0'!=PipeName[0])
                            InitialDevice2=new DBMP2F_Pipe(PipeName, DBMP2F_DeviceDirection::Write);
                        else
                            IsGoodFlag=0;   //we should not get here
                }
            }
            break;

            case 2: //"test2pipe"
            {
                InitialDevice1=new DBMP2F_Test(DBMP2F_DeviceDirection::Read, FileSizeAsInt);

                if(0!=InitialDevice1)
                {
                    if(ReadOnly)
                        InitialDevice2=new DBMP2F_Null();
                    else
                        if(0!=PipeName && '\0'!=PipeName[0])
                            InitialDevice2=new DBMP2F_Pipe(PipeName, DBMP2F_DeviceDirection::Write);
                        else
                            IsGoodFlag=0;   //we should not get here
                }
            }
            break;

            case 3: //"compare"
            {
                if(0!=FileName && '\0'!=FileName[0])
                {
                    if(0==FileSizeAsInt)
                        InitialDevice1=new DBMP2F_File(FileName, DBMP2F_DeviceDirection::Read);
                    else
                        InitialDevice1=new DBMP2F_VersionedFile(FileName, DBMP2F_DeviceDirection::Read, FileSizeAsInt, EndOfSubDeviceEvent);
                }
                else
                    IsGoodFlag=0;

                if(0!=InitialDevice1)
                {
                    if(0!=PipeName && '\0'!=PipeName[0])
                        InitialDevice2=new DBMP2F_Pipe(PipeName, DBMP2F_DeviceDirection::Read);
                    else
                        IsGoodFlag=0;
                }
            }
            break;

            default:
                IsGoodFlag=0;
        }
    }

    if(IsGoodFlag && (0==InitialDevice1 || 0==InitialDevice2))
    {
        IsGoodFlag=0;
        fprintf(stderr, "Out of memory.\n");
    }

    if(IsGoodFlag && (!InitialDevice1->IsGood() || !InitialDevice2->IsGood()))
        IsGoodFlag=0;

    if(IsGoodFlag && 0!=DelayTime)
    {
        DelayEvent=new DBMP2F_Event(DBMP2F_Event::ADelay);

        if(0==DelayEvent)
            IsGoodFlag=DBMP2F_Main_ReportOutOfMemory();
        else
        {
            DBMP2F_Reaction * Delay=new DBMP2F_ReactionDelay(DelayTime);

            if(0==Delay || !DelayEvent->AddReaction(Delay))
                IsGoodFlag=DBMP2F_Main_ReportOutOfMemory();
        }
    }

    if(IsGoodFlag && 0!=NumberOfWaits)
    {
        WaitEvent=new DBMP2F_Event(DBMP2F_Event::AWait);

        if(0==WaitEvent)
            IsGoodFlag=DBMP2F_Main_ReportOutOfMemory();
        else
        {
            DBMP2F_Reaction * Wait=      new DBMP2F_ReactionWait();
            DBMP2F_Reaction * SignalDone=new DBMP2F_ReactionSignalDone();

            if(0==Wait ||
               !WaitEvent->AddReaction(Wait) ||
               0==SignalDone ||
               !WaitEvent->AddReaction(SignalDone))
            {
                IsGoodFlag=DBMP2F_Main_ReportOutOfMemory();
            }
        }
    }

    if(IsGoodFlag && ShowSpeed)
    {
        ShowSpeedEvent=new DBMP2F_Event(DBMP2F_Event::AShowSpeed);

        if(0==ShowSpeedEvent)
            IsGoodFlag=DBMP2F_Main_ReportOutOfMemory();
        else
        {
            DBMP2F_Reaction * ShowSpeedReaction=new DBMP2F_ReactionShowSpeed();

            if(0==ShowSpeedReaction ||
               !ShowSpeedEvent->AddReaction(ShowSpeedReaction))
            {
                IsGoodFlag=DBMP2F_Main_ReportOutOfMemory();
            }
        }
    }

    if(IsGoodFlag && 3==DirectionNumber) // && !Quiet.IsSet()
    {
        SourcesDifferEvent=new DBMP2F_Event(DBMP2F_Event::AMessage);

        if(0==SourcesDifferEvent)
            IsGoodFlag=DBMP2F_Main_ReportOutOfMemory();
        else
        {
            DBMP2F_Reaction * DifferMessage=new DBMP2F_ReactionMessage("The sources differ.");

            if(0==DifferMessage ||
               !SourcesDifferEvent->AddReaction(DifferMessage))
            {
                IsGoodFlag=DBMP2F_Main_ReportOutOfMemory();
            }
        }
    }

    if(IsGoodFlag && (0==DirectionNumber || 1==DirectionNumber || 2==DirectionNumber))
        ConstructedController=new DBMP2F_TransferController(BufferSize, InitialDevice1, InitialDevice2, DelayTime, DelayPoint, DelayEvent, NumberOfWaits, WaitAfter, WaitEvent, ShowSpeedEvent);

    if(IsGoodFlag && 3==DirectionNumber)
        ConstructedController=new DBMP2F_CompareController(BufferSize, InitialDevice1, InitialDevice2, DelayTime, DelayPoint, DelayEvent, NumberOfWaits, WaitAfter, WaitEvent, ShowSpeedEvent, SourcesDifferEvent);

    if(IsGoodFlag)
    {
        if(0==ConstructedController)
        {
            IsGoodFlag=0;
            fprintf(stderr, "Out of memory.\n");
        } 
        else
            if(!ConstructedController->IsGood())
                IsGoodFlag=0;
    }
}

int DBMP2F_ControllerConstructor::IsGood()
{
    return IsGoodFlag;
}

DBMP2F_Controller * DBMP2F_ControllerConstructor::GiveConstructedController()
{
    return ConstructedController;
}


// -----------------------------------------------------------------------------
// function main
// -----------------------------------------------------------------------------

int main(int argc, char ** argv)
{
    int rc=1;

    ni32_RestrictedDoubleParameter Direction("-d",
                                             ni32_RestrictedDoubleParameter::TempValueList
                                             <<DBMP2F_ControllerConstructor::Directions[0]   //"pipe2file"
                                             <<DBMP2F_ControllerConstructor::Directions[1]   //"file2pipe"
                                             <<DBMP2F_ControllerConstructor::Directions[2]   //"test2pipe"
                                             <<DBMP2F_ControllerConstructor::Directions[3]); //"compare"
    ni32_DoubleParameter  FileName("-f");
    ni32_DoubleParameter  PipeName("-p");
    ni32_DoubleParameter  FileSize("-s");
    ni32_DoubleParameter  BufferSizeParameter("-b");
    ni32_SingleParameter  ReadOnly("-readonly");
    ni32_SingleParameter  NoWait("-nowait");
    ni32_SingleParameter  RemoveFiles("-removefiles");
    ni32_SingleParameter  Help("-h");
    ni32_SingleParameter  ShowSpeed("-showspeed");

    ni32_TrippleParameter DelayAfter("-delay");
    ni32_TrippleParameter Wait("-wait");

    DBMP2F_Parameters              Parameters(argc,
                                              argv,
                                              Direction,
                                              FileName,
                                              PipeName,
                                              FileSize,
                                              NoWait,
                                              Help,
                                              ni32_ProgramParameters::TempParamList<<&Direction
                                                                                   <<&FileName
                                                                                   <<&PipeName
                                                                                   <<&FileSize
                                                                                   <<&BufferSizeParameter
                                                                                   <<&DelayAfter
                                                                                   <<&ReadOnly
                                                                                   <<&RemoveFiles
                                                                                   <<&ShowSpeed
                                                                                   <<&NoWait
                                                                                   <<&Wait
                                                                                   <<&Help);

    if(!Parameters.Check())
        rc=0;
    else
    {
        unsigned int BufferSize=64*1024;    //default buffer size of 64 KByte
        teo00_Uint8  FileSizeAsUInt=0;

        int          DelayTime=0;
        teo00_Uint8  DelayPoint=0;

        int          NumberOfWaits=0;
        teo00_Uint8  WaitAfter=0;

        if(BufferSizeParameter.IsSet() && !cn36_StrToUInt(BufferSize, BufferSizeParameter.GiveValue()))
            rc=0;

        if(rc && FileSize.IsSet() && !cn36_StrToUInt8(FileSizeAsUInt, FileSize.GiveValue()))
            rc=0;

        if(rc && DelayAfter.IsSet())
        {
            const char * DelayTimeAsString;
            const char * DelayPointAsString;

            DelayAfter.GiveValues(DelayTimeAsString, DelayPointAsString);

            if(!cn36_StrToInt(DelayTime, DelayTimeAsString) ||
               !cn36_StrToUInt8(DelayPoint, DelayPointAsString))
            {
                rc=0;
            }
        }

        if(rc && Wait.IsSet())
        {
            const char * NumberOfWaitsAsString;
            const char * WaitAfterAsString;

            Wait.GiveValues(NumberOfWaitsAsString, WaitAfterAsString);

            if(!cn36_StrToInt(NumberOfWaits, NumberOfWaitsAsString) ||
               !cn36_StrToUInt8(WaitAfter, WaitAfterAsString))
            {
                rc=0;
            }
        }

        if(rc)
        {
            DBMP2F_ControllerConstructor ControllerConstructor(Direction,
                                                               BufferSize,
                                                               FileName.GiveValue(),
                                                               FileSizeAsUInt,
                                                               PipeName.GiveValue(),
                                                               ReadOnly.IsSet(),
                                                               DelayTime,
                                                               DelayPoint,
                                                               NoWait.IsSet(),
                                                               RemoveFiles.IsSet(),
                                                               NumberOfWaits,
                                                               WaitAfter,
                                                               ShowSpeed.IsSet());

            if(ControllerConstructor.IsGood())
                rc=ControllerConstructor.GiveConstructedController()->DoTransfer();
            else
                rc=0;

            delete ControllerConstructor.GiveConstructedController();
        }
    }

    //adjust return code of the program 0==OK, 1=ERROR, 2=Mismatch during compare
    if(1==rc)
        rc=0;
    else
        if(0==rc)
            rc=1;

    return rc;
}
