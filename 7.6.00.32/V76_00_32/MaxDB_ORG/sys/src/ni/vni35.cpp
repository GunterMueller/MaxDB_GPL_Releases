/*!*****************************************************************************
  module:       vni35.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: ni layer
  description:  implementing classes handling errors from vni32.cpp, vni33.cpp, vni34.cpp
  last change:  1999-10-04 15:30
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

#include "hni35.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#if defined (WIN32)
    #include <windows.h>
#elif defined (UNIX)
    #include <errno.h>
#else
    #error vni35.cpp only coded for WIN32 and UNIX (Define one of them.)
#endif


// -----------------------------------------------------------------------------
// constants
// -----------------------------------------------------------------------------

char tni35_AError::ErrorInfoBuffer[ErrorInfoBufferLength_ni35]={'\0',};
char tni35_AError::ErrorMsgBuffer[ErrorMsgBufferLength_ni35]={'\0',};

static const char LineDelimiter_ni35[]="\n";
#if defined (WIN32)
static const char szOSLineDelimiter_ni35[3]={13, 10, '\0'};
#elif defined (UNIX)
static const char szOSLineDelimiter_ni35[2]={'\n','\0'};
#else
    #error vni34a.cpp was only coded for WIN32 and UNIX (define one of them).
#endif

// -----------------------------------------------------------------------------
// member functions of class tni35_AError
// -----------------------------------------------------------------------------

tni35_AError::tni35_AError(int set)
{   
    if(set)
        Set();
}

tni35_AError::tni35_AError(const char *ErrInfo, int set)
{    
    if(set)
        Set(ErrInfo);
}

tni35_AError::~tni35_AError()
{
}

tni35_AError & tni35_AError::Set()
{
    ErrorInfoBuffer[0]='\0';
    
    return *this;
}

tni35_AError & tni35_AError::Set(const char *ErrInfo)
{
    if(0!=ErrInfo)
    {
        strncpy(ErrorInfoBuffer,ErrInfo,ErrorInfoBufferLength_ni35-1);
        ErrorInfoBuffer[ErrorInfoBufferLength_ni35-1]='\0';
    }
    else
        ErrorInfoBuffer[0]='\0';

    return *this;
}

tni35_AError & tni35_AError::SetStrN(const char *ErrInfo, size_t ErrInfoLength)
{
    if(0!=ErrInfo)
    {
        if(ErrInfoLength>ErrorInfoBufferLength_ni35-1)
            ErrInfoLength=ErrorInfoBufferLength_ni35-1;

        strncpy(ErrorInfoBuffer, ErrInfo, ErrInfoLength);
        ErrorInfoBuffer[ErrInfoLength]='\0';
    }
    else
        ErrorInfoBuffer[0]='\0';

    return *this;
}

const char * tni35_AError::GiveErrorInfoBuffer(void)
{
    return ErrorInfoBuffer;
}

void tni35_AError::FillErrorMsgBuffer() const
{
    strncpy(ErrorMsgBuffer, ErrorInfoBuffer, ErrorMsgBufferLength_ni35-1);
    ErrorMsgBuffer[ErrorMsgBufferLength_ni35-1]='\0';
}

const char * tni35_AError::GiveErrorMsgBuffer(void) const
{
    return ErrorMsgBuffer;
}


// -----------------------------------------------------------------------------
// member of class tni35_AErrorHandler
// -----------------------------------------------------------------------------

tni35_ASimpleErrorHandler ASimpleErrorHandler_ni35;
tni35_AErrorHandler * ErrorHandler_ni35=&ASimpleErrorHandler_ni35;

void tni35_AErrorHandler::Report(const tni35_AError & Err)
{
    Err.FillErrorMsgBuffer();
    reportString(Err.GiveErrorMsgBuffer());
}

tni35_AErrorHandler* tni35_AErrorHandler::SetAsCurrentErrorHandler()
{
    tni35_AErrorHandler* oldHandler(ErrorHandler_ni35);
    ErrorHandler_ni35=this;
    return oldHandler;
}

void tni35_AErrorHandler::SetNewErrorHandler(tni35_AErrorHandler * ANewErrorHandler)
{
    ErrorHandler_ni35=ANewErrorHandler;
}

void tni35_AErrorHandler::SetErrorHandlerToDefault()
{
    ErrorHandler_ni35=&ASimpleErrorHandler_ni35;
}

// -----------------------------------------------------------------------------
// member of class tni35_ASimpleErrorHandler
// -----------------------------------------------------------------------------

void tni35_ASimpleErrorHandler::reportString(const char * string)
{
    fprintf(stderr, "%s\n", (const char *)string);
}

// -----------------------------------------------------------------------------
// member of class tni35_AStandardError
// -----------------------------------------------------------------------------

tni35_AStandardError::tni35_AStandardError(StandardErrorCase StErrCase,
                                           int set)
    :tni35_AError(0),               // do not set basis error, to avoid doing the same twice
    LastSystemError(NoError_ni35)
{
    SetThisError(StErrCase);
    
    if(set)
        Set();                  // will set basis error to!
}

tni35_AStandardError::tni35_AStandardError(StandardErrorCase StErrCase,
                                           const char *ErrInfo,
                                           int set)
    :tni35_AError(ErrInfo, 0),
    LastSystemError(NoError_ni35)
{
    SetThisError(StErrCase);
    
    if(set)
        Set(ErrInfo);
}

tni35_AStandardError::tni35_AStandardError(StandardErrorCase StErrCase,
                                           const char *ErrInfo,
                                           const char *ErrInfo2,
                                           int set)
    :tni35_AError(ErrInfo, 0),
    LastSystemError(NoError_ni35)
{
    SetThisError(StErrCase);

    if(set)
        Set(ErrInfo, ErrInfo2);
}

tni35_AStandardError::~tni35_AStandardError()
{
}

void tni35_AStandardError::GetLastSystemError()
{
    #if defined (WIN32)
    LastSystemError=GetLastError();
    #elif defined (UNIX)
    LastSystemError=errno;
    #else
        #error tni35_AError::GetLastSystemError only coded for WIN32 and UNIX (Define one of them.)
    #endif

    char MsgPart1[40]; // 40 chars are enough space for "SystemError "+an int+", "

    sprintf(MsgPart1, "System error %d; ", (int)LastSystemError);
    strncpy(SystemErrorMsgBuffer, MsgPart1, SystemErrorMsgBufferLength_ni35-1);
    SystemErrorMsgBuffer[SystemErrorMsgBufferLength_ni35-1]='\0';

    char * EndOfSystemErrorMsgBuffer=SystemErrorMsgBuffer+strlen(SystemErrorMsgBuffer);
    
    if(SystemErrorMsgBufferLength_ni35-1>strlen(SystemErrorMsgBuffer))
    {
        size_t FreeChars=SystemErrorMsgBufferLength_ni35-1-strlen(SystemErrorMsgBuffer);

        #if defined (WIN32)
        if(0==FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                            0,
                            LastSystemError,
                            0,
                            EndOfSystemErrorMsgBuffer,
                            DWORD(FreeChars),
                            0))
        {
            *EndOfSystemErrorMsgBuffer='\0';
        }
        else
        {
            size_t SystemMsgLength=strlen(EndOfSystemErrorMsgBuffer);
            size_t OSLineDelimiterLength=strlen(szOSLineDelimiter_ni35);

            if(OSLineDelimiterLength<=SystemMsgLength &&    // if FormatMessage puts an newline at the end of the message ...
               !strcmp(EndOfSystemErrorMsgBuffer+SystemMsgLength-OSLineDelimiterLength, szOSLineDelimiter_ni35))
            {
                *(EndOfSystemErrorMsgBuffer+SystemMsgLength-OSLineDelimiterLength)='\0';    // ... remove it
            }
        }
        #elif defined (UNIX)
        errno=0;
        char * MsgPart2=strerror(LastSystemError);
        if(errno==EINVAL)
        {
            *EndOfSystemErrorMsgBuffer='\0';
            MsgPart2=0;
        }

        if(MsgPart2!=0)
            strncat(SystemErrorMsgBuffer, MsgPart2, FreeChars);
        #else
            #error tni35_AError::GetLastSystemError() only coded for WIN32 and UNIX (Define one of them.)
        #endif

        if('\0'== *EndOfSystemErrorMsgBuffer)
            strncat(SystemErrorMsgBuffer," explanation not available.", FreeChars);

        SystemErrorMsgBuffer[SystemErrorMsgBufferLength_ni35-1]=0;
    }
}


tni35_AError & tni35_AStandardError::Set()
{
    if(IsSystemError==ThisError->CheckOSFlag) 
        GetLastSystemError();                   // get the last system error before any other action can destroy the right value
  
    tni35_AError::Set();

    ErrorInfoBuffer2[0]='\0';

    return *this;
}

tni35_AError & tni35_AStandardError::Set(const char *ErrInfo)
{
    if(IsSystemError==ThisError->CheckOSFlag)
        GetLastSystemError();                   // get the last system error before any other action can destroy the right value

    tni35_AError::Set(ErrInfo);

    ErrorInfoBuffer2[0]='\0';

    return *this;
}

tni35_AError & tni35_AStandardError::Set(const char *ErrInfo, const char * ErrInfo2)
{
    if(IsSystemError==ThisError->CheckOSFlag)
        GetLastSystemError();                   // get the last system error before any other action can destroy the right value

    tni35_AError::Set(ErrInfo);

    if(0!=ErrInfo2)
    {
        strncpy(ErrorInfoBuffer2, ErrInfo2, ErrorInfoBufferLength_ni35-1);
        ErrorInfoBuffer2[ErrorInfoBufferLength_ni35-1]='\0';

        //delete trailing white spaces
        char * p=ErrorInfoBuffer2+strlen(ErrorInfoBuffer2);
        while(p!=ErrorInfoBuffer2 && isspace(*(--p)))
            *p='\0';
    }
    else
        ErrorInfoBuffer2[0]='\0';

    return *this;
}

void tni35_AStandardError::FillErrorMsgBuffer(void) const
{
    AErrorInfoType RealType=ThisError->InfoType;

    switch(ThisError->InfoType) //change to NoInfo if extended string or strings are not available or to long
    {
        case NoneOrString:
            if('\0'==GiveErrorInfoBuffer()[0] ||
               0==ThisError->ExtendedErrorText ||
               ErrorMsgBufferLength_ni35-1 < strlen(ThisError->ExtendedErrorText)+strlen(GiveErrorInfoBuffer())-2)
            {
                RealType=NoInfo;
            }
            break;
      
        case NoneOrTwoStrings:
            if('\0'==GiveErrorInfoBuffer()[0] || // do not test for ErrorInfoBuffer2 (in worst case we print an empty string "")
               0==ThisError->ExtendedErrorText || 
               ErrorMsgBufferLength_ni35-1 < strlen(ThisError->ExtendedErrorText)+strlen(GiveErrorInfoBuffer())+strlen(ErrorInfoBuffer2)-4)
            {
                RealType=NoInfo;
            }
            break;

        case NoInfo:
        default:
            break;
    }

    switch(RealType)
    {
        case NoneOrString:  //InfoBuffer contains a string
            sprintf(ErrorMsgBuffer, ThisError->ExtendedErrorText, GiveErrorInfoBuffer());
        break;

        case NoneOrTwoStrings:  //InfoBuffer contains a string (ErrorInfoBuffer2 contains at least an empty message)
            sprintf(ErrorMsgBuffer, ThisError->ExtendedErrorText, GiveErrorInfoBuffer(), ErrorInfoBuffer2);
        break;

        case NoInfo:
        default:
            strncpy(ErrorMsgBuffer, ThisError->PlainErrorText, ErrorMsgBufferLength_ni35-1);
            ErrorMsgBuffer[ErrorMsgBufferLength_ni35-1]='\0';
        break;
    }

    if(IsSystemError==ThisError->CheckOSFlag)
    {
        char * EndOfMessage=ErrorMsgBuffer+strlen(ErrorMsgBuffer);
        size_t FreeChars=ErrorMsgBufferLength_ni35-1-(EndOfMessage-ErrorMsgBuffer); // ErrorMsgBufferLength_ni35-1>=(EndOfMessage-ErrorMsgBuffer) assured by insertion to the buffer

        if(FreeChars>strlen(LineDelimiter_ni35)+strlen("()")) // enough space for newline+'('+"x"+')'
        {
            strcpy(EndOfMessage, LineDelimiter_ni35);
            EndOfMessage+=strlen(LineDelimiter_ni35);
            FreeChars-=strlen(LineDelimiter_ni35);

            strcpy(EndOfMessage, "(");
            EndOfMessage+=strlen("(");
            FreeChars-=strlen("(");

            strncpy(EndOfMessage, SystemErrorMsgBuffer, FreeChars-strlen(")")); //0<FreeChars-strlen(")")) should hold because of last if(...)
            EndOfMessage[FreeChars-strlen(")")]='\0';                           //be sure that the string is terminated
            EndOfMessage+=strlen(EndOfMessage);
            
            strcpy(EndOfMessage, ")"); //FreeChars>=strlen(")") holds already
        }
    }
}

void tni35_AStandardError::SetThisError(StandardErrorCase CaseToSet)
{
    if(StandardErrorToText[int(CaseToSet)].Case==CaseToSet)
        ThisError=StandardErrorToText+int(CaseToSet);
    else
    {
        #if defined (DEBUG_NI)
        printf("Warning: unexpected behaviour in tni35_AStandardError::SetThisError()\n");
        #endif
        ThisError=StandardErrorToText;
        while(ThisError->Case!=CaseToSet && ThisError->Case!=CaseUndefinedError)
            ThisError++;
    }
}


// -----------------------------------------------------------------------------
// constants
// -----------------------------------------------------------------------------

tni35_AStandardError tni35_AStandardError::OutOfMemory(tni35_AStandardError::CaseOutOfMemory, 0);
 
char tni35_AStandardError::SystemErrorMsgBuffer[SystemErrorMsgBufferLength_ni35]={'\0',};

tni35_AStandardError::StandardErrorDescription tni35_AStandardError::StandardErrorToText[]=
{
    {CaseNoError,
     tni35_AStandardError::IsNoSystemError,
     NoneOrString,
     "All works fine.",
     "%s"},


    {CaseOutOfMemory,
     tni35_AStandardError::IsNoSystemError,
     NoInfo,
     "Out of memory.", 0 },


    {CaseCouldNotCreateFile,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not create file.",
     "Can not create file '%s'."},

    {CaseCouldNotOpenFile,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not open file.",
     "Can not open file '%s'."},

    {CaseCouldNotCloseFile,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not close file.",
     "Can not close file '%s'."},

    {CaseCouldNotRemoveFile,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not remove file.",
     "Can not remove file '%s'."},

    {CaseCouldNotMoveFile,
     tni35_AStandardError::IsSystemError,
     NoneOrTwoStrings,
     "Cannot move a file.",
     "Can not move file '%s' to '%s'."},

    {CaseCouldNotReadInFile,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not read in file.",
     "Can not read in file '%s'."},

    {CaseCouldNotWriteInFile,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not write in file.",
     "Can not write in file '%s'."},

    {CaseCouldNotFlushFile,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not flush file.",
     "Can not flush file '%s'."},

    {CaseFileDoesNotExist,
     tni35_AStandardError::IsNoSystemError,
     NoneOrString,
     "A file does not exists.",
     "The file '%s' does not exists."},

    {CaseExistsAndIsNoFile,
     tni35_AStandardError::IsNoSystemError,
     NoneOrString,
     "Expected a file and found something else.",
     "'%s' exists, but it is not a file."},

    {CaseUnknownFileType,
     tni35_AStandardError::IsNoSystemError,
     NoInfo,
     "Unknown type of file specified.",
     0},

    {CaseCouldNotSetPositionInFile,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not change position in file.",
     "Can not change position in file '%s'."},


    {CaseTextLineTooLong,
     tni35_AStandardError::IsNoSystemError,
     NoneOrString,
     "A text file contains a line, which is too long for processing.",
     "The text file '%s' contains a line, which is too long for processing."},


    {CaseCouldNotOpenVirtualFile,
     tni35_AStandardError::IsNoSystemError,
     NoneOrTwoStrings,
     "Can not open a file.",
     "Can not open file '%s' (%s)."},

    {CaseCouldNotReadInVirtualFile,
     tni35_AStandardError::IsNoSystemError,
     NoneOrTwoStrings,
     "Can not read from a file.",
     "Can not read from file '%s' (%s)."},

    {CaseUnreadableVirtualFile,
     tni35_AStandardError::IsNoSystemError,
     NoneOrString,
     "A file was not opened for reading.",
     "The file '%s' was not opened for reading."},

    {CaseCouldNotWriteInVirtualFile,
     tni35_AStandardError::IsNoSystemError,
     NoneOrTwoStrings,
     "Can not write to a file.",
     "Can not write to file '%s' (%s)."},

    {CaseUnwritableVirtualFile,
     tni35_AStandardError::IsNoSystemError,
     NoneOrString,
     "A file was not opened for writing.",
     "The file '%s' was not opened for writing."},

    {CaseCouldNotCloseVirtualFile,
     tni35_AStandardError::IsNoSystemError,
     NoneOrTwoStrings,
     "Can not close a file.",
     "Can not close file '%s' (%s)."},

    {CaseCouldNotRemoveVirtualFile,
     tni35_AStandardError::IsNoSystemError,
     NoneOrTwoStrings,
     "Could not remove a file.",
     "Could not remove file '%s' (%s)."},
     
    {CaseCouldNotGetSizeOfVirtualFile,
     tni35_AStandardError::IsNoSystemError,
     NoneOrTwoStrings,
     "Could not determine the size of a file.",
     "Could not determine the size of file '%s' (%s)."},


    {CaseCouldNotCreateDirectory,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not create directory.",
     "Can not create directory '%s'."},

    {CaseCouldNotOpenDirectory,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not open directory.",
     "Can not open directory '%s'."},

    {CaseCouldNotReadInDirectory,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not read directory.",
     "Can not read directory '%s'."},

    {CaseCouldNotRemoveDirectory,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not remove directory.",
     "Can not remove directory '%s'."},

    {CaseExistsAndIsNoDirectory,
     tni35_AStandardError::IsNoSystemError,
     NoneOrString,
     "Expected a directory and found something else.",
     "'%s' exists, but it's not a directory."},

    {CaseDirectoryDoesNotExist,
     tni35_AStandardError::IsNoSystemError,
     NoneOrString,
     "A directory does not exists.",
     "The directory '%s' does not exists."},


    {CaseCouldNotCreatePipe,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not create pipe.",
     "Can not create pipe '%s'."},

    {CaseCouldNotConnectToPipe,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not connect to a pipe.",
     "Can not connect to pipe '%s'."},

    {CaseCouldNotClosePipe,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not close a pipe.",
     "Can not close pipe '%s'."},

    {CaseCouldNotRemovePipe,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not remove a pipe.",
     "Can not remove the pipe '%s'."},

    {CaseCouldNotReadInPipe,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not read in pile.",
     "Can not read in pipe '%s'."},

    {CaseCouldNotWriteInPipe,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not write in pipe.",
     "Can not write in pipe '%s'."},

    {CaseExistsAndIsNoPipe,
     tni35_AStandardError::IsNoSystemError,
     NoneOrString,
     "Expected a pipe and found something else.",
     "'%s' exists, but it is not a pipe."},

    {CasePipeDoesNotExist,
     tni35_AStandardError::IsNoSystemError,
     NoneOrString,
     "A pipe does not exists.",
     "'%s' does not exists."},

    {CaseUnknownPipeType,
     tni35_AStandardError::IsNoSystemError,
     NoInfo,
     "Unknown type of pipe specified.",
     0},

    {CaseCouldNotFlushPipe,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not flush a pipe.",
     "Can not flush pipe '%s'."}, 


    {CaseCouldNotCreateChildProcess,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Could not create child process.",
     "Could not create child-process '%s'."},

    {CaseWaitForProcessesFailed,
     tni35_AStandardError::IsSystemError,
     NoInfo,
     "Wait for processes failed.",
     0},

    {CaseWaitForProcessFailed,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Wait for a process failed.",
     "Wait for process %s failed"},

    {CaseCouldNotGetReturnCode,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Can not get a process return code.",
     "Can not get the return code of process '%s'."},

    {CaseProcessNotStarted,
     tni35_AStandardError::IsNoSystemError,
     NoneOrString,
     "A process was not started.",
     "The process '%s' is not running."},

    {CaseCouldNotStopProcess,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Could not stop a process.",
     "Could not stop the process '%s'."},

    {CaseCouldNotSetHandleInformation,
     tni35_AStandardError::IsSystemError,
     NoneOrString,
     "Could not change handle information.",
     "Could not change handle information for %s handle."},
     

    {CaseCouldNotOpenVirtualPipe,
     tni35_AStandardError::IsNoSystemError,
     NoneOrTwoStrings,
     "Can not open a pipe.",
     "Can not open pipe '%s' (%s)."},

    {CaseCouldNotReadInVirtualPipe,
     tni35_AStandardError::IsNoSystemError,
     NoneOrTwoStrings,
     "Can not read from a pipe.",
     "Can not read from pipe '%s' (%s)."},

    {CaseUnreadableVirtualPipe,
     tni35_AStandardError::IsNoSystemError,
     NoneOrString,
     "A pipe was not opened for reading.",
     "The pipe '%s' was not opened for reading."},

    {CaseCouldNotWriteInVirtualPipe,
     tni35_AStandardError::IsNoSystemError,
     NoneOrTwoStrings,
     "Can not write to a pipe.",
     "Can not write to pipe '%s' (%s)."},

    {CaseUnwritableVirtualPipe,
     tni35_AStandardError::IsNoSystemError,
     NoneOrString,
     "A pipe was not opened for writing.",
     "The pipe '%s' was not opened for writing."},

    {CaseCouldNotCloseVirtualPipe,
     tni35_AStandardError::IsNoSystemError,
     NoneOrTwoStrings,
     "Can not close a pipe.",
     "Can not close pipe '%s' (%s)."},

    {CaseCouldNotRemoveVirtualPipe,
     tni35_AStandardError::IsNoSystemError,
     NoneOrTwoStrings,
     "Could not remove a pipe.",
     "Could not remove pipe '%s' (%s)."},


    {CaseInternalError,
     tni35_AStandardError::IsNoSystemError,
     NoInfo,
     "An internal error occured.",
     0},


    {CaseUndefinedError,
     tni35_AStandardError::IsNoSystemError,
     NoInfo,
     "An unspecified error occured.",
     0}
};

char tni35_AStandardError::ErrorInfoBuffer2[ErrorInfoBufferLength_ni35]={'\0',};
