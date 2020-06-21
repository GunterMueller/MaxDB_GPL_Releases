/*******************************************************************************
  module:       hni33.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: ni layer
  description:  classes for process handling
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

#ifndef _HNI33_H_
#define _HNI33_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "hni00b.h"
#include "hni31.h"
#include "hni34.h"

#include "RunTime/System/RTESys_ProcessID.hpp"

#if defined (WIN32)
#include<windows.h>
#elif defined (UNIX)
#include<sys/types.h>
#else
    #error processes.h only coded for WIN32 and UNIX (Define one of them.)
#endif


// -----------------------------------------------------------------------------
// variables and types
// -----------------------------------------------------------------------------

#if defined (WIN32)
typedef HANDLE tni33_AProcessHandle;
typedef DWORD  tni33_AReturnCode;
const tni33_AProcessHandle AInvalidProcessHandle_ni33=INVALID_HANDLE_VALUE;
const tni33_AReturnCode    InvalidReturnCode_ni33    =-1;
#elif defined (UNIX)
typedef pid_t tni33_AProcessHandle;
typedef int   tni33_AReturnCode;
const tni33_AProcessHandle AInvalidProcessHandle_ni33=(pid_t)-1;
const tni33_AReturnCode    InvalidReturnCode_ni33    =-1;
#else
    #error processes.h only coded for WIN32 and UNIX (Define one of them.)
#endif


// -----------------------------------------------------------------------------
// class tni33_ArgumentList
// -----------------------------------------------------------------------------

class tni33_ArgumentList
{
  public:
    tni33_ArgumentList();
    virtual ~tni33_ArgumentList();

    int          AddArgument(const char * NewArgument);
    int          AddArgument(const char * NewArgument, size_t NewArgumentLength);
    void         DeleteAll();
    int          ConstructCommandLine();
    const char * GiveCommandLine();
    const char * GiveArgument(int Number);
    int          GiveNumberOfArguments() const {return NumberOfArguments;}

  private:
      int                     NumberOfArguments;
      ni31_List<char *>       Arguments;
      ni31_ListElem<char *> * LastArgument;
          
      int               NumberOfArgumentsOfCurrentCommandLine;
      char *            CommandLine;
      
};


// -----------------------------------------------------------------------------
// class tni33_ABackgroundProcess
// -----------------------------------------------------------------------------

class tni33_ABackgroundProcess
{
  public:
    tni33_ABackgroundProcess(const char        * Cmdline,
                             const tni34_AFile * StdIn=0,
                             const tni34_AFile * StdOut=0,
                             const int           AppendStdOut=1,
                             const tni34_AFile * StdErr=0,
                             const int           AppendStdErr=1,
                             const bool          DetachConsoleAtWindows=false);

    tni33_ABackgroundProcess(const char        * Cmdline,
                             const tni34_AFile * StdIn,
                             const tni34_APipe * StdOut=0,
                             const int           AppendStdOut=1,
                             const tni34_AFile * StdErr=0,
                             const int           AppendStdErr=1,
                             const bool          DetachConsoleAtWindows=false);
    
    tni33_ABackgroundProcess(const char        * Cmdline,
                             const tni34_APipe * StdIn,
                             const tni34_AFile * StdOut=0,
                             const int           AppendStdOut=1,
                             const tni34_AFile * StdErr=0,
                             const int           AppendStdErr=1,
                             const bool          DetachConsoleAtWindows=false);
    
    tni33_ABackgroundProcess(const char        * Cmdline,
                             const tni34_APipe * StdIn,
                             const tni34_APipe * StdOut,
                             const int           AppendStdOut=1,
                             const tni34_AFile * StdErr=0,
                             const int           AppendStdErr=1,
                             const bool          DetachConsoleAtWindows=false);

    tni33_ABackgroundProcess(tni33_ArgumentList & Arguments,
                             const tni34_AFile *  StdIn=0,
                             const tni34_AFile *  StdOut=0,
                             const int            AppendStdOut=1,
                             const tni34_AFile *  StdErr=0,
                             const int            AppendStdErr=1,
                             const bool           DetachConsoleAtWindows=false);

    virtual ~tni33_ABackgroundProcess();

    int Stop();
    int Wait();
    int WasStarted();
    int IsRunning();
    RTE_OSPid getPid() {return m_Pid;}

    
    tni33_AReturnCode GiveReturnCode();

    static void WaitForMultipleBackgroundProcesses(int NoProcs, tni33_ABackgroundProcess **); 

  private:
    enum AProcessState{NotStarted, Started, Stopped};

    char          *CommandLine;
    tni33_AProcessHandle Process;
    #if defined (WIN32)
    HANDLE         Thread;
    int            OpenedStandardInHandle;   // on NT the process has to supply the child with handles to standard in, out and error
    HANDLE         StandardInHandle;
    int            StandardInIsFile;         // pipes must remain open, files must be closed before they can be used
    int            OpenedStandardOutHandle;
    HANDLE         StandardOutHandle;
    int            StandardOutIsFile;
    int            OpenedStandardErrorHandle;
    HANDLE         StandardErrorHandle;
    int            StandardErrorIsFile;
    #endif
    tni33_AReturnCode RetCode;
    
    AProcessState State;

    void InitAndStartBackgroundProcess(const char             * Cmdline,
                                       const tni34_ADataEntry * StdIn,
                                       const int                StdInIsFile,
                                       const tni34_ADataEntry * StdOut,
                                       const int                AppendStdOut,
                                       const int                StdOutIsFile,
                                       const tni34_AFile      * StdErr,
                                       const int                AppendStdErr,
                                       const int                StdErrIsFile,
                                       const bool               DetachConsoleAtWindows);

    void InitAndStartBackgroundProcess(tni33_ArgumentList &     Arguments,
                                       const tni34_ADataEntry * StdIn,
                                       const int                StdInIsFile,
                                       const tni34_ADataEntry * StdOut,
                                       const int                AppendStdOut,
                                       const int                StdOutIsFile,
                                       const tni34_AFile      * StdErr,
                                       const int                AppendStdErr,
                                       const int                StdErrIsFile,
                                       const bool               DetachConsoleAtWindows);

    int  AdjustFileHandles();
    void FreeFileHandles();
    RTE_OSPid m_Pid;
};


// -----------------------------------------------------------------------------
// class tni33_AProcess
// -----------------------------------------------------------------------------

class tni33_AProcess
{
  private:
    tni33_ABackgroundProcess P;
  public:
    tni33_AProcess(const char        * Cmdline,
                   const tni34_AFile * StdIn=0,
                   const tni34_AFile * StdOut=0,
                   const int           AppendStdOut=1,
                   const tni34_AFile * StdErr=0,
                   const int           AppendStdErr=1);

    tni33_AProcess(tni33_ArgumentList & Arguments,
                   const tni34_AFile *  StdIn=0,
                   const tni34_AFile *  StdOut=0,
                   const int            AppendStdOut=1,
                   const tni34_AFile *  StdErr=0,
                   const int            AppendStdErr=1);

    int WasStarted();
    
    tni33_AReturnCode GiveReturnCode();
};


// -----------------------------------------------------------------------------
// inline functions for class ni33_ABackgroundProcess
// -----------------------------------------------------------------------------


inline tni33_AReturnCode tni33_ABackgroundProcess::GiveReturnCode()
{
    return RetCode;
}

inline int tni33_ABackgroundProcess::WasStarted()
{
    return (NotStarted!=State);
}


// -----------------------------------------------------------------------------
// inline functions for class tni33_AProcess
// -----------------------------------------------------------------------------

inline int tni33_AProcess::WasStarted()
{
    return P.WasStarted();
}

inline tni33_AReturnCode tni33_AProcess::GiveReturnCode()
{
    return P.GiveReturnCode();
}

#endif
