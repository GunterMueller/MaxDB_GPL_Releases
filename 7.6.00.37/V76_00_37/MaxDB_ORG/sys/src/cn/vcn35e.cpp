/*******************************************************************************
  module:       vcn35e.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  class for controling errors, log writers and configuration
                variables in the DBMServer
  last change:  2002-04-02 19:37
  version:      7.4.3
  see also:
  ------------------------------------------------------------------------------
                         Copyright (c) 2002-2005 SAP AG


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



*******************************************************************************/

// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "gcn00.h"
#include "hcn20.h"
#include "hcn35e.h"
#include "hcn36.h"
#include "hcn42.h"
#include "hcn51.h"
#include "hcn53.h"

#include "ToolsCommon/Tools_List.hpp"
#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBuffer.hpp"

#include <stdlib.h>
#include <time.h>


// -----------------------------------------------------------------------------
// member functions tcn35e_DBMBackupControllerLogWriter
// -----------------------------------------------------------------------------

tcn35e_DBMBackupControllerLogWriter tcn35e_DBMBackupControllerLogWriter::BasicLogWriter_cn35e(0, 0, 0);

const char * tcn35e_DBMBackupControllerLogWriter::ParameterNameDBMEBLSize="DBM_EBLSIZE";
const int    tcn35e_DBMBackupControllerLogWriter::MaxPrefixChars=256+1;
const int    tcn35e_DBMBackupControllerLogWriter::CharsPerPrefixLevel=4;
const int    tcn35e_DBMBackupControllerLogWriter::MaxPrefixLevels=(tcn35e_DBMBackupControllerLogWriter::MaxPrefixChars-1)/tcn35e_DBMBackupControllerLogWriter::CharsPerPrefixLevel;
const char * tcn35e_DBMBackupControllerLogWriter::LogFileActionStart="------------------ Start of a new external backup tool action ------------------";
const char * tcn35e_DBMBackupControllerLogWriter::LogFileActionEnd=  "------------------ end of the external backup tool action ----------------------";
const char * tcn35e_DBMBackupControllerLogWriter::LineDelimiter="\n";

tcn35e_DBMBackupControllerLogWriter::tcn35e_DBMBackupControllerLogWriter(const char * TheNameOfProtFile,
                                                                         const char * TheNameOfLogFile,
                                                                         const char * TheWishedNumberOfActions)
   :ClearProtFile(1),
    WishedNumberOfActionsInLogFile(13),
    NameOfProtFile(0),
    NameOfLogFile(0),
    CurrentPrefixLevel(0),
    LevelPrefix(0),
    IsGoodFlag(1),
    DelayedNewline(0),
    useTimeInPrefix(false),
    ProtStream(0),
    LogStream(0)
{
    CurrentTime[0]='\0';

    if(0!=TheNameOfProtFile && !cn36_StrAllocCpy(NameOfProtFile, TheNameOfProtFile))
        IsGoodFlag=0;

    if(0!=TheNameOfLogFile && !cn36_StrAllocCpy(NameOfLogFile, TheNameOfLogFile))
        IsGoodFlag=0;

    if(!cn36_StrAlloc(LevelPrefix, MaxPrefixChars))
        IsGoodFlag=0;
    else
        memset(LevelPrefix, '\0', MaxPrefixChars);  //initialize with a lot of terminating zeros

    if(0!=TheWishedNumberOfActions)
    {
        if(0==cn36_StrUprCmp("NONE", TheWishedNumberOfActions))
            WishedNumberOfActionsInLogFile=-1;
        else
        {
            if('\0'!=TheWishedNumberOfActions[0] &&
               cn36_StrConsistsOfDigits(TheWishedNumberOfActions) &&
               0<=atoi(TheWishedNumberOfActions))
            {
                WishedNumberOfActionsInLogFile=atoi(TheWishedNumberOfActions);
            }
        }
    }
}

tcn35e_DBMBackupControllerLogWriter::~tcn35e_DBMBackupControllerLogWriter()
{
    cn36_StrDealloc(NameOfProtFile);
    cn36_StrDealloc(NameOfLogFile);
    cn36_StrDealloc(LevelPrefix);

    if(0!=ProtStream || 0!=LogStream)
        CloseStream();
}

void tcn35e_DBMBackupControllerLogWriter::setProperty(MessageModifier Modifier)
{
    switch(Modifier)
    {
        case UsePrefix:
        case NoPrefix: useTimeInPrefix=false; break;
        case UsePrefixAndDateTime: useTimeInPrefix=true; break;
    }
}

int tcn35e_DBMBackupControllerLogWriter::Write(const char * Message, MessageModifier Modifier)
{
    int rc=0;

    if(IsGoodFlag && 0!=NameOfProtFile && '\0'!=NameOfProtFile[0])
    {
        if(SubALevelFromPrefix==Modifier || SubALevelFromPrefixAndEndParagraph==Modifier)
        {
            if(CurrentPrefixLevel>0)
            {
                LevelPrefix[(CurrentPrefixLevel-1)*CharsPerPrefixLevel]='\0';
                CurrentPrefixLevel--;
            }

            DelayedNewline=0;   //any previous delayed newlines for paragraphs should not be used after a SubALevel
        }

        if(OpenStream())
        {
            rc=PrintDelayedNewLineIfNecessary();

            if(rc)
            {
                bool resetUseTimeInPrefix=false;

                constructCurrentTimeString();

                if(false==useTimeInPrefix && Modifier==UsePrefixAndDateTime)
                {
                    useTimeInPrefix=true;
                    resetUseTimeInPrefix=true;
                }

                if(JustDateTime==Modifier)
                    rc=PrintTimeIfNecessary(Modifier);
                else
                {
                    const char * NextNewLine;
                    const char * CurrentPos=Message;
                    char         FormatString[30];  // should be enough for "%s%.<length>s", with length a number of type int
                    const size_t LengthOfLineDelimiter=strlen(LineDelimiter);
                    size_t       CharactersToPrint=0;
                    const char * CurrentPrefix=(NoPrefix==Modifier?"":LevelPrefix);

                    NextNewLine=strstr(CurrentPos, LineDelimiter);

                    //but remember that there was one

                    while(0!=NextNewLine)   //there is a new line in the string
                    {
                        if(!PrintTimeIfNecessary(Modifier))
                            rc=0;

                        CharactersToPrint=(NextNewLine+LengthOfLineDelimiter-CurrentPos);

                        if(isCurrentTimeStringEmpty())
                            sprintf(FormatString, "%%s%%s%%.%ds", (int)CharactersToPrint);  // we prepare for a sprintfn
                        else
                            sprintf(FormatString, "%%s%%s %%.%ds", (int)CharactersToPrint); // we prepare for a sprintfn

                        CharactersToPrint+=strlen(CurrentPrefix);
                        CharactersToPrint+=(isCurrentTimeStringEmpty()?0:1);
                        CharactersToPrint+=strlen(giveCurrentTimeString());

                        //write into dbm.ebp
                        if((int)CharactersToPrint>fprintf(ProtStream, FormatString, CurrentPrefix, giveCurrentTimeString(), CurrentPos))
                            rc=0;

                        //write into dbm.ebl
                        if(0!=LogStream)
                            fprintf(LogStream, FormatString, CurrentPrefix, giveCurrentTimeString(), CurrentPos);

                        //write into the DBA History
                        if(isCurrentTimeStringEmpty())
                            sprintf(FormatString, "%%s%%s%%.%ds", int(NextNewLine-CurrentPos));     // we prepare the same sprintfn without the newline
                        else
                            sprintf(FormatString, "%%s%%s %%.%ds", int(NextNewLine-CurrentPos));    // we prepare the same sprintfn without the newline

                        tcn53_DBAAction::writeDetail(FormatString, (NoPrefix==Modifier && 0==(NextNewLine-CurrentPos))?"":LevelPrefix, giveCurrentTimeString(), CurrentPos);

                        CurrentPos=NextNewLine+LengthOfLineDelimiter;
                        NextNewLine=strstr(CurrentPos, LineDelimiter);
                    }

                    if('\0'!=CurrentPos[0]) //something after the last new line
                    {
                        if(!PrintTimeIfNecessary(Modifier))
                            rc=0;

                        //write into dbm.ebp
                        CharactersToPrint=strlen(CurrentPrefix)+strlen(giveCurrentTimeString())+(isCurrentTimeStringEmpty()?0:1)+strlen(CurrentPos);

                        if(isCurrentTimeStringEmpty())
                            SAPDB_strcpy(FormatString, "%s%s%s");
                        else
                            SAPDB_strcpy(FormatString, "%s%s %s");

                        if((int)CharactersToPrint>fprintf(ProtStream, FormatString, CurrentPrefix, giveCurrentTimeString(), CurrentPos))
                            rc=0;

                        //write into dbm.ebl
                        if(0!=LogStream)
                            fprintf(LogStream, FormatString, CurrentPrefix, giveCurrentTimeString(), CurrentPos);

                        //write into the DBA History
                        tcn53_DBAAction::writeDetail(FormatString, (NoPrefix==Modifier && 0==strlen(CurrentPos))?"":LevelPrefix, giveCurrentTimeString(), CurrentPos);
                    }
                }

                if(resetUseTimeInPrefix)
                    useTimeInPrefix=false;
            }

            CloseStream();
            }

        if(AddALevelToPrefix==Modifier && CurrentPrefixLevel<MaxPrefixLevels)
        {
            memset(LevelPrefix+CurrentPrefixLevel*CharsPerPrefixLevel, ' ', CharsPerPrefixLevel);
            CurrentPrefixLevel++;
        }

        if(EndParagraph==Modifier || SubALevelFromPrefixAndEndParagraph==Modifier)
            DelayedNewline=1;
    }

    return rc;
}

int tcn35e_DBMBackupControllerLogWriter::Write(MessageModifier Modifier)
{
    int rc=1;

    if(SubALevelFromPrefix==Modifier || SubALevelFromPrefixAndEndParagraph==Modifier)
    {
        if(CurrentPrefixLevel>0)
        {
            LevelPrefix[(CurrentPrefixLevel-1)*CharsPerPrefixLevel]='\0';
            CurrentPrefixLevel--;
        }

        DelayedNewline=0;   //any previous delayed newlines for paragraphs should not be used after a SubALevel
    }

    if(AddALevelToPrefix==Modifier && CurrentPrefixLevel<MaxPrefixLevels)
    {
        memset(LevelPrefix+CurrentPrefixLevel*CharsPerPrefixLevel, ' ', CharsPerPrefixLevel);
        CurrentPrefixLevel++;
    }

    if(JustDateTime==Modifier)
        rc=Write("", JustDateTime);

    if(EndParagraph==Modifier || SubALevelFromPrefixAndEndParagraph==Modifier)
        DelayedNewline=1;

    return rc;
}

int tcn35e_DBMBackupControllerLogWriter::Write(tni34_AFile & TheFileToMove, const char * FileDescription)
{
    int rc=1;
    const char * FileDescriptionToUse="a file";

    TheFileToMove.Close();

    if(TheFileToMove.IsGood())
    {
        if(0!=FileDescription && '\0'!=FileDescription[0])
            FileDescriptionToUse=FileDescription;

        sprintf(MsgBuf_cn36, "Copying %s to this file.\n", FileDescriptionToUse);
        Write(MsgBuf_cn36);
        sprintf(MsgBuf_cn36, "---------- Begin of %s (%s)----------\n", FileDescriptionToUse, TheFileToMove.GiveName());
        Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

        tni34_ATextFile SrcFile(TheFileToMove.GiveName(), tni34_AFile::FromDisk, 0, 0); //do not create on open, do not open now

        if(SrcFile.IsGood())
        {   
            if(SrcFile.Open())
            {
                const char * Line;
                char       * LinePlusNewLine=0;
                size_t       LengthOfLine=0;
                size_t       SpaceInLinePlusNewLine=0;
                size_t       LengthOfNewLine=strlen("\n");

                while(rc && SrcFile.ReadLine(Line) && 0!=Line)
                {
                    LengthOfLine=strlen(Line);

                    if(LengthOfLine+LengthOfNewLine>SpaceInLinePlusNewLine)            //is more place needed then available?
                    {
                        SpaceInLinePlusNewLine=LengthOfLine+LengthOfNewLine;

                        if(!cn36_StrAlloc(LinePlusNewLine, SpaceInLinePlusNewLine))    //than get more
                            rc=0;
                    }

                    if(rc)
                    {
                        SAPDB_strcpy(LinePlusNewLine, Line);
                        SAPDB_strcpy(LinePlusNewLine+LengthOfLine, "\n"); //LinePlusNewLine is now Line+"\n"

                        if(!Write(LinePlusNewLine))
                            rc=0;// report this error after the LogFile was closed, because it will be reported to the LogFile !
                    }
                }

                cn36_StrDealloc(LinePlusNewLine); //last chance for freeing this memory
            }
            else
                rc=0;
        }
        else
            rc=0;

        sprintf(MsgBuf_cn36, "---------- End of %s (%s)----------\n", FileDescriptionToUse, TheFileToMove.GiveName());
        Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefix);
    }
    else
        rc=0;

    return rc;
}

int tcn35e_DBMBackupControllerLogWriter::IsGood()
{
    return IsGoodFlag;
}

void tcn35e_DBMBackupControllerLogWriter::ReinitLogWriter()
{
    ClearProtFile=1;
    CurrentPrefixLevel=0;
    DelayedNewline=0;
    useTimeInPrefix=false;

    if(IsGoodFlag)
        memset(LevelPrefix, '\0', MaxPrefixChars);
}

int tcn35e_DBMBackupControllerLogWriter::OpenStream()
{
    if(ClearProtFile)
    {
        //truncate dbm.ebp
        ProtStream=fopen(NameOfProtFile, "w");   //TODO replace with tni34_AFile
        ClearProtFile=0;

        //shorten dbm.ebl for one section
        if(0<=WishedNumberOfActionsInLogFile && 0!=NameOfLogFile && '\0'!=NameOfLogFile[0]) //negativ WishedNumberOfActionsInLogFile means don't use it
        {   
            tsp00_Int4         LogFile;
            tsp05_RteFileError RTEFileError;
            tsp00_Pathc        FileName;
            Tools_List<char *> OldContent;
            int                CurrentActionsInLogFile=0;

            FileName.rawAssign(NameOfLogFile);

            //read the file
            sqlfopenc(FileName, sp5vf_text, sp5vf_read, sp5bk_buffered, &LogFile, &RTEFileError);

            if(vf_ok==RTEFileError.sp5fe_result)
            {
                int    AllOK=1;
                int    ReachedEndOfFile=0;
                char * Line=0;
                size_t LineLength=10;

                if(cn36_StrAlloc(Line, LineLength))
                {
                    while(AllOK && !ReachedEndOfFile)
                    {
                        AllOK=ReadNextLine(LogFile, Line, LineLength, ReachedEndOfFile);

                        if(AllOK && !ReachedEndOfFile)
                        {
                            char * CopyOfLine=0;
            
                            if(!cn36_StrAllocCpy(CopyOfLine, Line) || !OldContent.push_back(CopyOfLine))
                                AllOK=0;

                            if(0==strcmp(Line, LogFileActionStart))
                                CurrentActionsInLogFile++;
                        }
                    }

                    cn36_StrDealloc(Line);
                }

                sqlfclosec(LogFile, sp5vf_close_normal, &RTEFileError);
            }

            //write the shortened version back (if reading failed write nothing back but try to write the LogFileActionStart-string)
            LogStream=fopen(NameOfLogFile, "w");

            if(0!=LogStream)
            {
                int                          i=CurrentActionsInLogFile-(WishedNumberOfActionsInLogFile-1);
                Tools_List<char *>::iterator p=OldContent.begin();

                if(0<i) //something to delete
                {
                    for(;0<i; i--)
                    {
                        for(; p!=OldContent.end() && 0!=strcmp((*p), LogFileActionStart); ++p);
                        ++p; //jump over the action start string
                    }

                    for(; p!=OldContent.end() && 0!=strcmp((*p), LogFileActionStart); ++p); //and now search for the next action (which must be copied)
                }

                for(; p!=OldContent.end(); ++p)
                    fprintf(LogStream, "%s\n", *p); //this will just fail for instance, if there is not enough disk space (it's only a log file)

                for(p=OldContent.begin(); p!=OldContent.end(); ++p) //free the memory not freed by the list itself
                    cn36_StrDealloc(*p);

                fprintf(LogStream, "%s%s", LogFileActionStart, "\n\n");
            }
        }
        else
            LogStream=0;
    }
    else
    {
        ProtStream=fopen(NameOfProtFile, "a");

        if(0<=WishedNumberOfActionsInLogFile && 0!=NameOfLogFile && '\0'!=NameOfLogFile[0]) //negativ WishedNumberOfActionsInLogFile means don't use it
            LogStream=fopen(NameOfLogFile, "a");
        else
            LogStream=0;
    }

    return (0!=ProtStream);
}

void tcn35e_DBMBackupControllerLogWriter::CloseStream()
{
    if(0!=ProtStream)
    {
        fflush(ProtStream);
        fclose(ProtStream);
    
        ProtStream=0;
    }

    if(0!=LogStream)
    {
        fflush(LogStream);
        fclose(LogStream);
    
        LogStream=0;
    }
}

int tcn35e_DBMBackupControllerLogWriter::PrintTimeIfNecessary(MessageModifier Modifier)
{
    int rc=1;

    if(NoPrefix!=Modifier)
    {
        if((0==CurrentPrefixLevel && AddALevelToPrefix==Modifier) ||
           (JustDateTime==Modifier)                                  )
        {
            time_t ltime ;
            char   szDatTim[30];

            time(&ltime);   // Get current date and time,
            strftime(szDatTim, sizeof(szDatTim), "%Y-%m-%d %H:%M:%S", localtime(&ltime));   //convert according to timezone and make string.

            //write into dbm.ebp
            if((int)strlen(szDatTim)+(int)strlen("\n")>fprintf(ProtStream, "%s\n", szDatTim))
                rc=0;

            //write into dbm.ebl
            if(0!=LogStream)
                fprintf(LogStream, "%s\n", szDatTim);

            //write into the DBA History
            tcn53_DBAAction::writeDetail("%s", szDatTim);   //writeDetail() is appending the \n for itself
        }
    }

    return rc;
}

int tcn35e_DBMBackupControllerLogWriter::PrintDelayedNewLineIfNecessary()
{
    int rc=1;

    if(DelayedNewline)  //if another paragraph ended one line ago on the same level
    {                   //separate the paragraphs by a newline
        DelayedNewline=0;
       
        //write into dbm.ebp
        if((int)strlen(LineDelimiter)>fprintf(ProtStream, "%s", LineDelimiter))  
            rc=0;

        //write into dbm.ebl
        if(0!=LogStream)
            fprintf(LogStream, "%s", LineDelimiter);

        //write into the DBA History
        tcn53_DBAAction::writeDetail("%s", LineDelimiter);
    }

    return rc;
}

void tcn35e_DBMBackupControllerLogWriter::constructCurrentTimeString()
{
    time_t ltime;

    time(&ltime);   // get current date and time,
    strftime(CurrentTime, sizeof(CurrentTime), "%Y-%m-%d %H:%M:%S", localtime(&ltime));   //convert according to timezone and make string
}

bool tcn35e_DBMBackupControllerLogWriter::isCurrentTimeStringEmpty() const
{
    return (!useTimeInPrefix || 0==strlen(CurrentTime));
}

const char * tcn35e_DBMBackupControllerLogWriter::giveCurrentTimeString() const
{
    if(useTimeInPrefix)
        return CurrentTime;
    else
        return "";
}

int tcn35e_DBMBackupControllerLogWriter::ReadNextLine(tsp00_Int4 & File, char *& Line, size_t & LineLength, int & ReachedEndOfFile)
{
    int                rc=1;
    tsp00_Longint      BytesRead=0;
    tsp05_RteFileError FileError;
    int                LineIncomplete=1;
    size_t             Used=0;

    while(rc && LineIncomplete)
    {
        sqlfreadc(File, Line+Used, LineLength-Used, &BytesRead, &FileError);

        if(vf_ok!=FileError.sp5fe_result)
        {
            LineIncomplete=0;   //no more data to read, end while loop

            if(vf_eof==FileError.sp5fe_result)
                ReachedEndOfFile=1;
            else
                rc=0;
        }
        else
        {
            if(sp5vfw_no_warning==FileError.sp5fe_warning)
                LineIncomplete=0; // we could read a complete line into Line without errors or warnings, end the while loop
            else
                if(sp5vfw_no_eol_found==FileError.sp5fe_warning)   //line was to big for current buffer
                {
                    char * BiggerLine=0;

                    if(!cn36_StrAlloc(BiggerLine, 2*LineLength))    //allocate more memory
                        rc=0;
                    else
                    {
                        SAPDB_strcpy(BiggerLine, Line);   //copy the part of the line read so far
                    
                        cn36_StrDealloc(Line);         //forget Line and us BiggerLine as Line
                        Line=BiggerLine;
                        LineLength*=2;

                        Used=strlen(BiggerLine);
                    }
                }
                else    //we don't know about other warnings, report them as errors
                {
                    LineIncomplete=0;   //no more data to read, end while loop
                    rc=0;
                }
        }
    }

    return rc;
}

// -----------------------------------------------------------------------------
// member functions tcn35e_DBMBackupControllerErrorHandler
// -----------------------------------------------------------------------------

tcn35e_DBMBackupControllerErrorHandler tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e(0, 0, &tcn35e_DBMBackupControllerLogWriter::BasicLogWriter_cn35e);

tcn35e_DBMBackupControllerErrorHandler::APhaseToDBMErrorMap tcn35e_DBMBackupControllerErrorHandler::ErrorMap[11]=
{
    {CommonError,        ERR_CN00},
    {ToolCheck,          ERR_TOOLCHK_CN00},
    {MediumCheck,        ERR_MEDIUMCHK_CN00},
    {PrepareOperation,   ERR_PREPARE_CN00},
    {DBRequest,          ERR_DBREQ_CN00},
    {ToolRequest,        ERR_TOOLREQ_CN00},
    {OperationCheck,     ERR_OPCHK_CN00},
    {PostOperation,      ERR_POSTOP_CN00},
    {BackupOperation,    ERR_BACKUPOP_CN00},
    {CleanUp,            ERR_CLEANUP_CN00},
    {MediumNotFound,     ERR_MENOTFOUND_CN00},
};

tcn35e_DBMBackupControllerErrorHandler::tcn35e_DBMBackupControllerErrorHandler(char                               * TheReplyBuffer,
                                                                               int                                * TheReplyBufferLength,
                                                                               tcn35d_DBBackupControllerLogWriter * TheLogWriter)
   :ReplyData(TheReplyBuffer),
    ReplyLen(TheReplyBufferLength),
    LastError(OK_CN00),
    ReplyAlreadyConstructed(0),
    LogWriter(TheLogWriter)
{
}

tcn35e_DBMBackupControllerErrorHandler::~tcn35e_DBMBackupControllerErrorHandler()
{
    //nothing to do so far
}


void tcn35e_DBMBackupControllerErrorHandler::SetReplyArrayTo(char * TheNewReplyBuffer,
                                                             int  * TheNewReplyBufferLength)
{
    ReplyData=TheNewReplyBuffer;
    ReplyLen =TheNewReplyBufferLength;

    LastError=OK_CN00;
    AnErrorWasReported=0;
    ReplyAlreadyConstructed=0;

    SetPhaseTo(CommonError);
}

void tcn35e_DBMBackupControllerErrorHandler::AnswerCNError( _TCHAR * szBuffer, int * pnDataLen, const tcn00_Error nError, const char * szInfo)
{
    int i=0;
    int j=0;

    size_t length=(0==szInfo)?0:strlen(szInfo);
    bool   infoDoesEndWithNewLine=(0<length && szInfo[length-1]=='\n');

    struct
    {
        tcn00_Error   nValue;
        const _TCHAR* szText;
    }errorArray[] = ERROR_TO_TEXT_CN00;

    while((errorArray[i].nValue != ERR_UNKNOWN_CN00) && (errorArray[i].nValue != nError))
        i++;

    _stprintf(szBuffer,
              _T("%s%s%d%s%s%s%s%s"),
              (OK_CN00==nError) ? ANSWER_OK_CN00 : ANSWER_ERR_CN00,
              LINE_SEPSTRING_CN00,
              nError,
              TOKEN_SEPSTRING_CN00,
              errorArray[i].szText,
              LINE_SEPSTRING_CN00,
              0==szInfo? "" : szInfo,
              (0==szInfo || '\0'==szInfo[0] || infoDoesEndWithNewLine)? "" : LINE_SEPSTRING_CN00);    // only print a second line separating sting if we realy have a second line

    *pnDataLen=int(_tcslen(szBuffer));

    LogWriter->Write("Constructed the following reply:\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

    sprintf(MsgBuf_cn36, "%s%s", ((nError == OK_CN00) ? ANSWER_OK_CN00 : ANSWER_ERR_CN00), LINE_SEPSTRING_CN00);
    LogWriter->Write(MsgBuf_cn36);
    sprintf(MsgBuf_cn36, "%d%s%s%s", nError, TOKEN_SEPSTRING_CN00, errorArray[i].szText, LINE_SEPSTRING_CN00);
    LogWriter->Write(MsgBuf_cn36);
    sprintf(MsgBuf_cn36, "%s\n", (szInfo == NULL ? "" : szInfo));
    LogWriter->Write(MsgBuf_cn36);
    LogWriter->Write(tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
}

void tcn35e_DBMBackupControllerErrorHandler::Report(const tni35_AError & Err)
{
    tni35_AErrorHandler::Report(Err);
}

void tcn35e_DBMBackupControllerErrorHandler::reportString(const char * string)
{
    LastError=ErrorMap[CurrentErrorPhase].TheDBMError;

    sprintf(MsgBuf_cn36, "Have encountered error %d:\n", int(LastError));
    LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

    if(0!=string)
    {
        LogWriter->Write(string);

        size_t length=strlen(string);
        if(0>=length || string[length-1]!='\n')
            LogWriter->Write("\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
    }
    LogWriter->Write(tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

    if(0!=ReplyData && 0!=ReplyLen && !ReplyAlreadyConstructed)
    {
        AnswerCNError(ReplyData, ReplyLen, LastError, string);
        ReplyAlreadyConstructed=1;
    }

    AnErrorWasReported=1;
}

void tcn35e_DBMBackupControllerErrorHandler::reportMessageList(const DBMSrvMsg_Error & err)
{
    DBMSrvInfrReply_ReplyBuffer tempBuf;

    if(tempBuf.isValid())
    {
        tempBuf.getReply().startWithMessageList(err);
        reportString(tempBuf.getReply().getStartOfReplyText());
    }
    else
        Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseOutOfMemory));
}

tcn00_Error tcn35e_DBMBackupControllerErrorHandler::GiveLastError()
{
    return LastError;
}


// -----------------------------------------------------------------------------
// class tcn35e_DBMBackupError
// -----------------------------------------------------------------------------

tcn35e_DBMBackupError::tcn35e_DBMBackupError(DBMBackupErrorCase ErrCase)
:tcn35d_DBBackupError()
{
    SetThisError(ErrCase);
    DBCode=0;
    Set();
}

tcn35e_DBMBackupError::tcn35e_DBMBackupError(DBMBackupErrorCase ErrCase, const char *ErrInfo)
:tcn35d_DBBackupError()
{
    SetThisError(ErrCase);
    DBCode=0;
    Set(ErrInfo);
}

tcn35e_DBMBackupError::tcn35e_DBMBackupError(DBMBackupErrorCase ErrCase, int TheDBCode, const char * TheSQLErrText)
:tcn35d_DBBackupError()
{
    SetThisError(ErrCase);
    DBCode=TheDBCode;

    if(0!=TheSQLErrText)
    {
        const char *EndOfText=cn36_StartOfTrailingWhiteSpaces(TheSQLErrText);

        SetStrN(TheSQLErrText, EndOfText-TheSQLErrText);
    }
    else
        Set("");
}

tcn35e_DBMBackupError::tcn35e_DBMBackupError(DBMBackupErrorCase ErrCase, int TheDBCode, int ErrInfo)
:tcn35d_DBBackupError()
{
    SetThisError(ErrCase);
    DBCode=TheDBCode;
    SetI(ErrInfo);
}

tcn35e_DBMBackupError::tcn35e_DBMBackupError(DBMBackupErrorCase ErrCase, int ErrInfo)
:tcn35d_DBBackupError()
{
    SetThisError(ErrCase);
    DBCode=0;
    SetI(ErrInfo);
}

void tcn35e_DBMBackupError::SetThisError(int CaseToSet)
{
    if(DBMBackupErrorToText[CaseToSet].Case==CaseToSet)
        ThisError=DBMBackupErrorToText+CaseToSet;
    else
    {
        ThisError=DBMBackupErrorToText;

        while(ThisError->Case!=CaseToSet && ThisError->Case!=CaseCouldNotGetNameOfToolProtFile)
            ThisError++;

        if(CaseToSet!=ThisError->Case)
            ThisError=GiveDescriptionOfInternalError();
    }
}

void tcn35e_DBMBackupError::FillErrorMsgBuffer() const
{
    FillErrorMsgBuffer(*ThisError);
}

void tcn35e_DBMBackupError::FillErrorMsgBuffer(const DBBackupErrorDescription &ErrDesc) const
{
    size_t LengthOfExtendedErrorMsg;

    if(ErrDesc.InfoType==None ||
       ErrDesc.InfoType==AString ||
       ErrDesc.InfoType==AIntCode)
    {
        tcn35d_DBBackupError::FillErrorMsgBuffer(ErrDesc);
    }
    else
    {
        switch(ErrDesc.InfoType)
        {
            case AIntAndADBCode:
            case ADBCodeAndAInt:
                LengthOfExtendedErrorMsg=strlen(ThisError->ExtendedErrorText)-4+MaximalCharsForAnInt+MaximalCharsForAnInt;
            break;
            case ADBCodeAndAString:
                LengthOfExtendedErrorMsg=strlen(ThisError->ExtendedErrorText)-4+MaximalCharsForAnInt+strlen(GiveErrorInfoBuffer());
            break;
            default:
                ErrorHandler_ni35->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
        }

        if(ErrorMsgBufferLength_ni35-1 >= LengthOfExtendedErrorMsg)
        {
            switch(ErrDesc.InfoType)
            {
                case AIntAndADBCode:
                    sprintf(ErrorMsgBuffer, ThisError->ExtendedErrorText, IntCode, DBCode);
                break;
                case ADBCodeAndAInt:
                    sprintf(ErrorMsgBuffer, ThisError->ExtendedErrorText, DBCode, IntCode);
                break;
                case ADBCodeAndAString:
                    sprintf(ErrorMsgBuffer, ThisError->ExtendedErrorText, DBCode, GiveErrorInfoBuffer());
                break;
                default:
                    ErrorHandler_ni35->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            }
        }
        else
            cn36_StrNCpy(ErrorMsgBuffer, ThisError->PlainErrorText, ErrorMsgBufferLength_ni35-1);
    }
}

tcn35d_DBBackupError::DBBackupErrorDescription tcn35e_DBMBackupError::DBMBackupErrorToText[]=
{
    {CaseIllegalBlockingFactor,
     AString,
     "The blocking factor of the medium is incorrect.",
     "The blocking factor (%s) of the medium can not be used."},

    {CaseIllegalBlockingFactorParameter,
     AString,
     "The kernel's blocking factor for backup and restore is incorrect.",
     "The kernel's blocking factor for backup and restore (%s) can not be used."},

    {CaseIllegalPageSize,
     AString,
     "The page size of the database is illegal.",
     "The page size of the database (%s) is illegal."},


    {CaseIllegalStageFileSize,
     ADBCodeAndAString,
     "A size of a stage file is not as expected.",
     "A size of %d byte for the stage file %s is not correct."},

    {CaseInsufficientHisoryData,
     AString,
     "Could not find all the needed data in the backup history files.",
     "Could not find all the needed data in the backup history files (e.g. for '%s')."},

    {CaseStageFileMissing,
     AString,
     "A stage file is missing.",
     "The stage file '%s' is missing."},

    {CaseStageChangedInSize,
     AString,
     "A stage file was changed in size.",
     "The stage file '%s' was changed in size."},

    {CaseStageChangedInDateTime,
     AString,
     "Date or time of a stage file was changed.",
     "Date or time of the stage file '%s' was changed."},

    {CaseCanNotReadLabel,
     AString,
     "Can not read the label of a stage file.",
     "Can not read the label of stage file '%s'."},

    {CaseStageChangedInLabel,
     AString,
     "The label of a stage file has changed.",
     "The label of a stage file '%s' has changed."},


    {CaseIncorrectPagesTransfered,
     AString,
     "The database kernel provided an incorrect value for Pages Transfered.",
     "The database kernel provided the incorrect value '%s' for Pages Transfered."},

    {CaseNoPagesTransfered,
     None,
     "The database did not return a value for Pages Transfered.",
     0},


    {CaseBackupToolAndDBFailed,
     AIntAndADBCode,
     "The backup tool and the database request failed.",
     "The backup tool failed with %d as sum of exit codes. The database request failed with error %d."},

    {CaseBackupToolKilled,
     AIntAndADBCode,
     "The backup tool was killed.",
     "The backup tool was killed with %d as sum of exit codes. The database request ended with code %d."},

    {CaseBackupToolFailedAndDBKernelRequestCanceled,
     AIntAndADBCode,
     "The backup tool failed and the database request was canceled.",
     "The backup tool failed with %d as sum of exit codes. The database request was canceled and ended with error %d."},

    {CaseBackupToolKilledAndDBKernelRequestCanceled,
     AIntAndADBCode,
     "The backup tool was killed and the database request was canceled.",
     "The backup tool was killed with %d as sum of exit codes. The database request was canceled and ended with error %d."},

    {CaseDBKernelRequestCanceled,
     ADBCodeAndAInt,
     "The database request was canceled.",
     "The database request was canceled and ended with error %d. The backup tool ended with %d as sum of exit codes."},

    {CaseCancelDBKernelRequestFailed,
     AIntCode,
     "A cancel of the database request failed.",
     "A cancel of the database request failed. The backup tool ended with %d as sum of exit codes."},

    {CaseBackupToolFailedAndCancelDBKernelRequestFailed,
     AIntCode,
     "The backup tool failed and a cancel of the database request failed.",
     "The backup tool failed with %d as sum of exit codes. A cancel of the database request failed."},

    {CaseBackupToolKilledAndCancelDBKernelRequestFailed,
     AIntCode,
     "The backup tool was killed and a cancel of the database request failed.",
     "The backup tool was killed with %d as sum of exit codes. A cancel of the database request failed."},


    {CasePipe2FileFailed,
     AIntCode,
     "Pipe2file failed.",
     "Pipe2file failed with %d as exit code."},

    {CaseBackupToolAndPipe2FileFailed,
     AIntAndADBCode,
     "The backup tool and pipe2file failed.",
     "The backup tool failed with %d as sum of exit codes. Pipe2file failed with %d as exit code."},

    {CasePipe2FileKilled,
     AIntCode,
     "Pipe2file was killed.",
     "Pipe2file was killed. The backup tool ended with %d as sum of exit codes."},

    {CaseBackupToolFailedAndPipe2FileKilled,
     AIntCode,
     "The backup tool failed and pipe2file was killed.",
     "The backup tool failed with %d as sum of exit codes and pipe2file was killed."},

    {CaseBackupToolAndPipe2FileKilled,
     AIntCode,
     "The backup tool and pipe2file were killed.",
     "The backup tool was killed with %d as sum of exit codes. Pipe2file was killed too."},

    {CaseStageVerifyFailed,
     AString,
     "The backup of a stage file could not be verified successfully.",
     "The backup of the stage file '%s' could not be verified successfully."},


    {CaseCouldNotMovePipe2FileOutputToToolProtFile,
     AString,
     "Could not move output of pipe2file to the log file.",
     "Could not move output of pipe2file from file %s to log file."},


    {CaseSQLError,
     ADBCodeAndAString,
     "The database was unable to fulfill a request.",
     "The database was unable to fulfill a request (%d, %s)."},

    {CaseCN00Error,
     AIntCode,
     "A baisc error occured.",
     "The error %d occured."},


    {CaseNoMediumFound,
     None,
     "Unable to determine backup medium.",
     0},


    {CaseCouldNotGetNameOfToolHistFile,
     AIntCode,
     "Could not determine the name of the external backup history file.",
     "Could not determine the name of the external backup history file (Error %d)."},

    {CaseCouldNotGetInternalBackupIDs,
     None,
     "Could not get internal backup ID assigned by the database kernel.",
     0},

    {CaseCouldNotGetExternalBackupIDs,
     None,
     "Could not get external backup ID's from the backup tool.",
     0},

    {CaseCouldNotWriteToToolHistFile,
     AString,
     "Could not write to external backup history file.",
     "Could not write to external backup history file %s."},

    {CaseNoLogPageInformation,
     None,
     "Have no correct log page information from database kernel.",
     0},


    {CaseCouldNotGetNameOfBackupHistory,
     AIntCode,
     "Could not determine the name of the backup history file.",
     "Could not determine the name of the backup history file (Error %d)."},

    {CaseCouldNotGetNameOfToolLogFile,
     None,
     "Could not determine name of external backup log file.",
     0},

    {CaseCouldNotGetNameOfToolProtFile,
     None,
     "Could not determine name of external backup protocol file.",
     0},
};


// -----------------------------------------------------------------------------
// member functions of class tcn35e_ConfigVariable
// -----------------------------------------------------------------------------

tcn35e_ConfigVariable::tcn35e_ConfigVariable(char *TheName, char *TheValue)
{
    Name=TheName;
    Value=TheValue;
}

tcn35e_ConfigVariable::~tcn35e_ConfigVariable()
{
    cn36_StrDealloc(Name);
    cn36_StrDealloc(Value);
}

const char * tcn35e_ConfigVariable::GiveName()
{
    return Name;
}

const char * tcn35e_ConfigVariable::GiveValue()
{
    return Value;
}

int tcn35e_ConfigVariable::SetValue(const char * NewValue)
{
    return cn36_StrAllocCpy(Value, NewValue);
}

// -----------------------------------------------------------------------------
// member functions of class tcn35e_ConfigurationVariableHandler
// -----------------------------------------------------------------------------

const char * tcn35e_ConfigurationVariableHandler::MagicStringExportVariables="Set_Variable_";

tcn35e_ConfigurationVariableHandler::tcn35e_ConfigurationVariableHandler()
:ConfigVarListFilled(0)
{
}

tcn35e_ConfigurationVariableHandler::~tcn35e_ConfigurationVariableHandler()
{
    //free the memory used by the list
    while(0!=ConfigVarList.First())
    {
        delete(ConfigVarList.First()->GiveInfo());
        ConfigVarList.Del();
    }
}

int tcn35e_ConfigurationVariableHandler::GetConfigurationVariable(const tcn35d_BackupInfo *  BackupInfo,
                                                                  const char              *  VariableName,
                                                                  char                    *& FileEnvVar)
{
    int rc=1;

    cn36_StrDealloc(FileEnvVar);

    if(!ConfigVarListFilled)
        rc=FillConfigVarList(BackupInfo->DBName());

    if(0!=cn36_StrNUprCmp(VariableName, MagicStringExportVariables, strlen(MagicStringExportVariables)) ||  //VariableName does not start with "Set_Variable_" or ...
       0>=strlen(VariableName+strlen(MagicStringExportVariables)) ||                                        //... it is only "Set_Variable_" or ...
       !cn36_StrConsistsOfDigits(VariableName+strlen(MagicStringExportVariables)))                          //... it is "Set_Variable_<something>" but <something> is not a number
    {
        tsp00_DbNamec tspDBName;
        tsp00_Namec   tspVariableName;

        tspDBName.rawAssign(BackupInfo->DBName());
        tspVariableName.rawAssign(VariableName);

        cn51DBMConfigValue CFGValue(tspDBName, tspVariableName);

        if(!cn36_StrAllocCpy(FileEnvVar, CFGValue))
            rc=0;
    }

    if(rc)  //all ok
    {
        ni31_ListElem<tcn35e_ConfigVariable *> *ListElem;
        int FoundInList=0;

        for(ListElem=ConfigVarList.First(); rc && !FoundInList && 0!=ListElem; ListElem=ListElem->Next())   //check against the complete list of "Set_Variable_..."-variables
            if(0==cn36_CompareEnvVarNames(VariableName, ListElem->GiveInfo()->GiveName()))    //if we found the variable in the list
            {
                if(0==FileEnvVar || 0>=strlen(FileEnvVar)) //not found a variable <VariableName> as such in dbm.cfg
                {
                    if(!cn36_StrAllocCpy(FileEnvVar, ListElem->GiveInfo()->GiveValue()))  //take the value from the "Set_Variable_..."-section
                        rc=0;
                }
                else    //already found variable <VariableName> as such in dbm.cfg (not only as "Set_Variable_... <VariableName>=...")
                {
                    if(!ListElem->GiveInfo()->SetValue(FileEnvVar))  //overwrite the value in the list with the value found already (and prevent an export with the wrong value)
                        rc=0;
                }

                FoundInList=1; //end the loop
            }
    }

    return rc;
}

const ni31_List<tcn35e_ConfigVariable *> & tcn35e_ConfigurationVariableHandler::ConfigurationVariableToExportList()
{
    return ConfigVarList;
}

void tcn35e_ConfigurationVariableHandler::ReinitConfigVarHandler()
{
    ConfigVarListFilled=0;

    //free the memory used by the list
    while(0!=ConfigVarList.First())
    {
        delete(ConfigVarList.First()->GiveInfo());
        ConfigVarList.Del();
    }
}

int tcn35e_ConfigurationVariableHandler::FillConfigVarList(const char * DBName)
{
    int rc=1;

    cn51DBMConfigValueIterator oIterator(DBName);

    char   CurrentName[30];
    size_t StartOfSuffix;
    int i;

    cn36_StrNCpy(CurrentName, MagicStringExportVariables, 20);
    StartOfSuffix=strlen(CurrentName);

    for(i=0; rc && i<=100; i++)
    {
        sprintf(CurrentName+StartOfSuffix, "%d", (int)i);
        oIterator.Goto(CurrentName);

        if(!oIterator.IsEnd())
        {
            const char * CFGValueString=oIterator.GetCurrentValueAsString();
            const char * EqualSign=strchr(CFGValueString, '=');

            if(0!=EqualSign)
            {
                char * CopyOfFoundValue=0;
                char * CopyOfFoundName=0;

                if(cn36_StrNAllocCpy(CopyOfFoundName, CFGValueString, EqualSign-CFGValueString) &&
                   cn36_StrAllocCpy(CopyOfFoundValue, EqualSign+1))
                {
                    int FoundInList=0;
                    ni31_ListElem<tcn35e_ConfigVariable *> *ListElem=ConfigVarList.First();

                    for(; !FoundInList && 0!=ListElem; ListElem=ListElem->Next())
                    {
                        if(0==cn36_CompareEnvVarNames(CopyOfFoundName, ListElem->GiveInfo()->GiveName()))
                            FoundInList=1;
                    }

                    if(FoundInList)
                    {
                        cn36_StrDealloc(CopyOfFoundValue);  //if found variable already in the list throw away the new values
                        cn36_StrDealloc(CopyOfFoundName);
                    }
                    else
                    {
                        tcn35e_ConfigVariable * NewConfigVar=new tcn35e_ConfigVariable(CopyOfFoundName, CopyOfFoundValue);

                        if(0==NewConfigVar || 0==ConfigVarList.Add(NewConfigVar))
                        {
                            rc=0;
                            ErrorHandler_ni35->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseOutOfMemory));

                            if(0!=NewConfigVar)
                                delete NewConfigVar;
                            else
                            {
                                 cn36_StrDealloc(CopyOfFoundValue);  //0==NewConfigVar last chance to free that memory
                                 cn36_StrDealloc(CopyOfFoundName);
                            }
                        }
                    }
                }
                else
                {
                    rc=0;
                    cn36_StrDealloc(CopyOfFoundValue);
                    cn36_StrDealloc(CopyOfFoundName);
                }
            }
        }
    }

    ConfigVarListFilled=1;  //do not read the List again
    ConfigVarList.Reverse();//reverses the order of the list

    return rc;
}


// -----------------------------------------------------------------------------
// class tcn35e_TheDBMBackupControllerFactory
// -----------------------------------------------------------------------------

int tcn35e_TheDBMBackupControllerFactory::RunDirectory(char                                   *& TheRunDirectory,
                                                       const char                             *  TheDBName,
                                                       tcn35e_DBMBackupControllerErrorHandler *  ErrorHandler,
                                                       tcn35e_DBMBackupControllerLogWriter    *  LogWriter)
{
    int rc=0;
    tcn002_XpValueString    Value;
    tsp00_DbNamec tDBName;

    tDBName.rawAssign(TheDBName);

    DBMSrvMsg_Error oParamGetMsg;
    if(cn20XParamGetValue(tDBName, PAN_RUNDIR, Value, oParamGetMsg))
        rc=cn36_StrAllocCpy(TheRunDirectory, (const char *)Value);
    else
    {
        tcn00_Error XParamRC=oParamGetMsg.ID();
        ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseCN00Error, int(XParamRC)));
        LogWriter->Write("Could not determine the run directory of the database.\n");
    }

    return rc;
}

int tcn35e_TheDBMBackupControllerFactory::MediumInfo(const char                             *& MediumName,
                                                     int                                     & NumberOfPipes,
                                                     char                                   *  ThePipeNames[MAX_NUMBER_OF_DATAPIPES_CN35D],
                                                     char                                   *  TheMediumTypes[MAX_NUMBER_OF_DATAPIPES_CN35D],
                                                     long                                    & BytesPerBlock,
                                                     long                                    & PagesPerBlock,
                                                     long                                    & BytesPerPage,
                                                     long                                      TheCountInPages[MAX_NUMBER_OF_DATAPIPES_CN35D],
                                                     const char                             *  TheDBName,
                                                     const tcn003_Medium                    *  pMedium,
                                                     const tcn003_MediaGroup                *  pMediaGroup,
                                                     tcn35e_DBMBackupControllerErrorHandler &  ErrorHandler)
{
    int                   rc=1;
    const tcn003_Medium * pSingleMedium;
    tsp00_DbNamec         tDBName;
                
    tDBName.rawAssign(TheDBName);

    PagesPerBlock=0;

    if(0!=pMedium) //only one medium
    {
        pSingleMedium=pMedium;
        MediumName=pMedium->szName;
        NumberOfPipes=1;
    }
    else
        if(0!=pMediaGroup && 0!=pMediaGroup->pFirst) // we have a group of media with at least one element
        {
            pSingleMedium=pMediaGroup->pFirst;
            MediumName=pMediaGroup->szName;

            //determine the number of media in the group
            NumberOfPipes=0;

            while(0!=pSingleMedium)
            {
                NumberOfPipes++;
                pSingleMedium=pSingleMedium->pNext;
            }

            pSingleMedium=pMediaGroup->pFirst; //go back to the first medium of the group
        }
        else
        {
            rc=0;
            ErrorHandler.SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::MediumNotFound);
            ErrorHandler.Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseNoMediumFound));
            ErrorHandler.SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::CommonError);
        }


    if(rc) //now get the pages per block of the medium or of the first medium of a group
    {
        if(!cn36_StrToLong(PagesPerBlock, pSingleMedium->szBlocksize) ||
           0>PagesPerBlock)
        {
            rc=0;
            ErrorHandler.Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseIllegalBlockingFactor, pSingleMedium->szBlocksize));
        }
        else
            if(0==PagesPerBlock)    //if medium has block size 0, use the DBM Server default (db kernel parameter BACKUP_BLOCK_CNT is no longer existent starting with 7.6.00)
                cn36_StrToLong(PagesPerBlock, ME_DEFAULT_BLOCKSIZE_CN003);
    }

    int i;

    for(i=0; i<MAX_NUMBER_OF_DATAPIPES_CN35D; i++)  //init even if 0==rc
        ThePipeNames[i]=TheMediumTypes[i]=0;

    if(rc) // now get copies of the pipe names
    {
        for(i=0; rc && i<NumberOfPipes; i++, pSingleMedium=pSingleMedium->pNext)
        {
            if(!cn36_StrAllocCpy(ThePipeNames[i], pSingleMedium->szLocation) ||
               !cn36_StrAllocCpy(TheMediumTypes[i], pSingleMedium->szType)      )
            {
                rc=0;
            }

            cn36_StrToLong(TheCountInPages[i], pSingleMedium->szSize);
        }
    }

    if(rc)
    {
        tsp00_DbNamec tDBName;
        tcn002_XpValueString    szPageSize;

        tDBName.rawAssign(TheDBName);

        DBMSrvMsg_Error oParamGetMsg;
        if(!cn20XParamGetValue(tDBName, PAN_PAGE_SIZE, szPageSize, oParamGetMsg))
        {
            tcn00_Error RCOfGetValue=oParamGetMsg.ID();
            ErrorHandler.Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseCN00Error, int(RCOfGetValue)));
            rc=0;
        }
        else
        {
            if(!cn36_StrToLong(BytesPerPage, (char *)szPageSize) ||
               0>=BytesPerPage)
            {
                rc=0;
                ErrorHandler.Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseIllegalPageSize, szPageSize));
            }
            else
                BytesPerBlock=BytesPerPage*PagesPerBlock;
        }
    }

    return rc;
}

int tcn35e_TheDBMBackupControllerFactory::BackupHistories(char                                  *& BackupHistory,
                                                          char                                  *& ExternalBackupHistory,
                                                          const char                            *  DBName,
                                                          tcn35e_DBMBackupControllerErrorHandler & ErrorHandler)
{
    int rc=1;

    tsp00_DbNamec tDBName;
    tDBName.rawAssign(DBName);

    tsp00_Pathc tFileName;

    tcn00_Error Err=cn42GetFileName(tDBName, FGET_BACKEBF_CN42, tFileName);

    if(OK_CN00==Err)
        rc=cn36_StrAllocCpy(ExternalBackupHistory, (char *)tFileName);   //  0==ExternalBackupHistory, if cn36_StrAllocCpy fails
    else
    {
        rc=0;
        ErrorHandler.Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseCouldNotGetNameOfToolHistFile, int(Err)));
    }

    if(rc)
    {
        Err=cn42GetFileName(tDBName, FGET_BACKHIST_CN42, tFileName);

        if(OK_CN00==Err)
            rc=cn36_StrAllocCpy(BackupHistory, (char *)tFileName);   // 0==PathOfBackupHistory, if cn36_StrAllocCpy fails
        else
        {
            rc=0;
            ErrorHandler.Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseCouldNotGetNameOfBackupHistory, int(Err)));
        }
    }

    return rc;
}
