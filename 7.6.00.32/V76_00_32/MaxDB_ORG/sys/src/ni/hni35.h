/*!*****************************************************************************
  module:       hni35.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: ni layer
  description:  classes handling errors from vni32.cpp, vni33.cpp, vni34.cpp
  last change:  1999-10-04 15:29
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

#ifndef _HNI35_H_
#define _HNI35_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include"hni00b.h"

#if defined (WIN32)
    #include <windows.h>
#elif defined (UNIX)
#else
    #error error.h only coded for WIN32 and UNIX (Define one of them.)
#endif

#include <sys/types.h>

// -----------------------------------------------------------------------------
// types
// -----------------------------------------------------------------------------

#if defined (WIN32)
typedef DWORD tni35_ErrorID;
#elif defined (UNIX)
typedef int tni35_ErrorID;
#else
    #error error.h only coded for WIN32 and UNIX (Define one of them.)
#endif

 
// -----------------------------------------------------------------------------
// constants
// -----------------------------------------------------------------------------

#if defined (WIN32)
    const tni35_ErrorID NoError_ni35=ERROR_SUCCESS;
#elif defined (UNIX)
    #if defined (ESUCCESS)
        const tni35_ErrorID NoError_ni35=ESUCCESS;
    #else
        const tni35_ErrorID NoError_ni35=0;
    #endif
#else
    #error tni35_AError errors.cpp only coded for WIN32 and UNIX (Define one of them.)
#endif

const int ErrorInfoBufferLength_ni35=128;
const int ErrorMsgBufferLength_ni35=256;
const int SystemErrorMsgBufferLength_ni35=128;


// -----------------------------------------------------------------------------
// class tni35_AError
// -----------------------------------------------------------------------------

class tni35_AError
{
  public:
    explicit tni35_AError(int set=1);
    tni35_AError(const char *ErrInfo, int set=1);
    virtual ~tni35_AError();

    tni35_AError & Set();
    tni35_AError & Set(const char *ErrInfo);
    tni35_AError & SetStrN(const char *ErrInfo, size_t ErrInfoLength);

    virtual void         FillErrorMsgBuffer() const;
    virtual const char * GiveErrorMsgBuffer() const;

  protected:
    static char ErrorMsgBuffer[ErrorMsgBufferLength_ni35];

    static  const char * GiveErrorInfoBuffer();

  private:  
    static char ErrorInfoBuffer[ErrorInfoBufferLength_ni35];
};


// -----------------------------------------------------------------------------
// class tni35_AErrorHandler
// -----------------------------------------------------------------------------

class tni35_AErrorHandler
{
  public:
    virtual void Report(const tni35_AError & Err);
    virtual void reportString(const char * string)=0;

    /*!
        @brief set this error handler as new error handler
        @return previous error handler
    */
    tni35_AErrorHandler* SetAsCurrentErrorHandler();

    static void SetNewErrorHandler(tni35_AErrorHandler * ANewErrorHandler);

  protected:
    static void SetErrorHandlerToDefault();
};

class tni35_ASimpleErrorHandler: public tni35_AErrorHandler
{
  public:
    virtual void reportString(const char * string);
};

// -----------------------------------------------------------------------------
// global variable
// -----------------------------------------------------------------------------

extern tni35_AErrorHandler * ErrorHandler_ni35;


// -----------------------------------------------------------------------------
// class tni35_AStandardError
// -----------------------------------------------------------------------------

class tni35_AStandardError:public tni35_AError
{
  public:
    enum StandardErrorCase
    {
        CaseNoError,

        CaseOutOfMemory,

        CaseCouldNotCreateFile,
        CaseCouldNotOpenFile,
        CaseCouldNotCloseFile,
        CaseCouldNotRemoveFile,
        CaseCouldNotMoveFile,
        CaseCouldNotReadInFile,
        CaseCouldNotWriteInFile,
        CaseCouldNotFlushFile,
        CaseFileDoesNotExist,
        CaseExistsAndIsNoFile,
        CaseUnknownFileType,
        CaseCouldNotSetPositionInFile,

        CaseTextLineTooLong,

        CaseCouldNotOpenVirtualFile,
        CaseCouldNotReadInVirtualFile,
        CaseUnreadableVirtualFile,
        CaseCouldNotWriteInVirtualFile,
        CaseUnwritableVirtualFile,
        CaseCouldNotCloseVirtualFile,
        CaseCouldNotRemoveVirtualFile,
        CaseCouldNotGetSizeOfVirtualFile,

        CaseCouldNotCreateDirectory,
        CaseCouldNotOpenDirectory,
        CaseCouldNotReadInDirectory,
        CaseCouldNotRemoveDirectory,
        CaseExistsAndIsNoDirectory,
        CaseDirectoryDoesNotExist,

        CaseCouldNotCreatePipe,
        CaseCouldNotConnectToPipe,
        CaseCouldNotClosePipe,
        CaseCouldNotRemovePipe,
        CaseCouldNotReadInPipe,
        CaseCouldNotWriteInPipe,
        CaseExistsAndIsNoPipe,
        CasePipeDoesNotExist,
        CaseUnknownPipeType,
        CaseCouldNotFlushPipe,

        CaseCouldNotCreateChildProcess,
        CaseWaitForProcessesFailed,
        CaseWaitForProcessFailed,
        CaseCouldNotGetReturnCode,
        CaseProcessNotStarted,
        CaseCouldNotStopProcess,
        CaseCouldNotSetHandleInformation,

        CaseCouldNotOpenVirtualPipe,
        CaseCouldNotReadInVirtualPipe,
        CaseUnreadableVirtualPipe,
        CaseCouldNotWriteInVirtualPipe,
        CaseUnwritableVirtualPipe,
        CaseCouldNotCloseVirtualPipe,
        CaseCouldNotRemoveVirtualPipe,

        CaseInternalError,
        CaseUndefinedError
    };

    enum ACheckOSError {IsSystemError, IsNoSystemError};
    enum AErrorInfoType{NoInfo, NoneOrString, NoneOrTwoStrings};

    tni35_AStandardError(StandardErrorCase StErrCase, int set=1);
    tni35_AStandardError(StandardErrorCase StErrCase, const char *ErrInfo, int set=1);
    tni35_AStandardError(StandardErrorCase StErrCase, const char *ErrInfo, const char*ErrInfo2, int set=1);

    ~tni35_AStandardError();

    tni35_AError & Set();
    tni35_AError & Set(const char *ErrInfo);
    tni35_AError & Set(const char *ErrInfo, const char *ErrInfo2);

    virtual void FillErrorMsgBuffer() const;

    static tni35_AStandardError OutOfMemory;

    struct StandardErrorDescription // must be public for initialization of static member StandardErrorToText
    {
        StandardErrorCase Case;
        ACheckOSError     CheckOSFlag;
        AErrorInfoType    InfoType;
        const char *      PlainErrorText;
        const char *      ExtendedErrorText;
    };

  private:
    static char SystemErrorMsgBuffer[SystemErrorMsgBufferLength_ni35];

    static StandardErrorDescription StandardErrorToText[];
    
    tni35_ErrorID     LastSystemError;

    const StandardErrorDescription * ThisError;
    
    void GetLastSystemError();
    void SetThisError(StandardErrorCase CaseToSet);

    static char ErrorInfoBuffer2[ErrorInfoBufferLength_ni35];
};

#endif
