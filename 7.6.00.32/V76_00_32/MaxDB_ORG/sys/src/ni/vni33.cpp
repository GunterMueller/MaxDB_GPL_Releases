/*!*****************************************************************************
  module:       vni33.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: ni layer
  description:  implementation of classes for process handling
  last change:  1999-10-04 15:33
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

#include"hni33.h"
#include"hni35.h"

#include<string.h>

#include<ctype.h>
#include<stdio.h>
#if defined (WIN32)
#elif defined (UNIX)
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/types.h>
#include<signal.h>
#include<errno.h>
#else
    #error hni33.cpp only coded for WIN32 and UNIX (Define one of them.)
#endif

// -----------------------------------------------------------------------------
// private function: ni33IsWhiteSpace
//
// description : decides that the argument is a white space character or not
// arguments   : c [in] character to check
// return value: 1 if c is a white space, 0 if it is not
// -----------------------------------------------------------------------------

static int ni33IsWhiteSpace(char c)
{
  return isspace((int)c);
}


// -----------------------------------------------------------------------------
// implementation of class tni33_ArgumentList
// -----------------------------------------------------------------------------


tni33_ArgumentList::tni33_ArgumentList()
    :NumberOfArguments(0),
     LastArgument(0),
     CommandLine(0),
     NumberOfArgumentsOfCurrentCommandLine(0)
{
}

tni33_ArgumentList::~tni33_ArgumentList()
{
    DeleteAll();
}

int tni33_ArgumentList::AddArgument(const char * NewArgument)
{
    return AddArgument(NewArgument, strlen(NewArgument));
}

int tni33_ArgumentList::AddArgument(const char * NewArgument, size_t NewArgumentLength)
{
    int rc=0;

    char * CopyOfNewArgument=new char [NewArgumentLength+1];

    if(0!=CopyOfNewArgument)
    {
        ni31_ListElem<char *> * NewLastArgument;

        strncpy(CopyOfNewArgument, NewArgument, NewArgumentLength);
        CopyOfNewArgument[NewArgumentLength]='\0';

        if(0==LastArgument)
            NewLastArgument=Arguments.Add(CopyOfNewArgument);
        else
            NewLastArgument=Arguments.Add(CopyOfNewArgument, *LastArgument);

        if(0==NewLastArgument)
        {
             ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory);      //not enough memory for the list element pointing to the string
             delete [] CopyOfNewArgument;                                       //last chance to delete the memory reserved above
        }
        else
        {
            LastArgument=NewLastArgument;
            NumberOfArguments++;
            rc=1;
        }
    }
    else
        ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory);

    return rc;
}

void tni33_ArgumentList::DeleteAll()
{
    ni31_ListElem<char *> *h;

    for(h=Arguments.First(); 0!=h; h=h->Next())
        delete [] h->GiveInfo();                //delete the strings the list elements are pointing to

    Arguments.DelAll(); //delete the list elements itself

    if(0!=CommandLine)
    {
        delete [] CommandLine;
        CommandLine=0;
    }

    NumberOfArguments=0;
    LastArgument=0;
    NumberOfArgumentsOfCurrentCommandLine=0;
}

int tni33_ArgumentList::ConstructCommandLine()
{
    int                     rc=1;
    size_t                  Length=0;
    ni31_ListElem<char *> * ArgListElem;

    if(0!=CommandLine)
    {
        delete [] CommandLine;
        CommandLine=0;
    }

    NumberOfArgumentsOfCurrentCommandLine=0;

    for(ArgListElem=Arguments.First(); 0!=ArgListElem; ArgListElem=ArgListElem->Next())
    {
        const char * CurrentArgument=ArgListElem->GiveInfo();
        int FoundASpace=0;

        Length+=strlen(CurrentArgument);    // we will only add something to the argument string

        int FoundBackslashes=0;

        for(; '\0'!=(*CurrentArgument); CurrentArgument++)   //as long as we have not reached the end of the argument string
        {
            if('\\'==(*CurrentArgument)) //a backslash must only masked on NT by a backslash if it (or a row of backslashes) is followed by a '\"'
                FoundBackslashes++;
            else
            {
                if('\"'==(*CurrentArgument)) //if a qoute is 
                    Length+=FoundBackslashes+1; //have to place a backslashes in front of any backslash and one backslash in front of the '\"'
                else
                {
                    if(ni33IsWhiteSpace(*CurrentArgument))
                        FoundASpace=1;

                    #if defined (UNIX)
                    Length+=FoundBackslashes;   //double every backslashe found on UNIX (also if not terminated by a '\"')
                    #elif defined (WIN32)
                    #else
                        #error tni33_ArgumentList::CreateArgumentString() not coded for this operating system!
                    #endif
                }
            
                FoundBackslashes=0; //the row of backslashes was terminated, so other backslashes count for themself
            }
        }

        #if defined (UNIX)
        Length+=FoundBackslashes;   ///double every backslashe found on UNIX (also if at the end of the string)
        #elif defined (WIN32)
        #else
            #error tni33_ArgumentList::CreateArgumentString() not coded for this operating system!
        #endif

        if(FoundASpace)
            Length+=2; //must put an argument with white spaces in quotes

        Length+=1; //put a ' ' between two arguments
    }

    if(0!=Length)
    {
        CommandLine=new char[Length];  //allocate memory for the string, not Length+1 because we have computed space for a ' ' after every argument

        if(0==CommandLine)
        {
            rc=0;
            ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory);
        }
        else
        {
            char * CurrentPosition=CommandLine;

            for(ArgListElem=Arguments.First(); 0!=ArgListElem; ArgListElem=ArgListElem->Next())
            {
                const char * CurrentArgument;
                int FoundASpace=0;
                int FoundBackslashes=0;

                for(CurrentArgument=ArgListElem->GiveInfo(); '\0'!=(*CurrentArgument) && 0==FoundASpace; CurrentArgument++)
                {
                    if(ni33IsWhiteSpace(*CurrentArgument))
                        FoundASpace=1;
                }

                if(FoundASpace)
                    *(CurrentPosition++)='\"';

                for(CurrentArgument=ArgListElem->GiveInfo(); '\0'!=(*CurrentArgument); CurrentArgument++)   //as long as we have not reached the end of the argument string
                {
                    if('\\'==(*CurrentArgument)) //a backslash must only masked on NT by a backslash if it (or a row of backslashes) is followed by a '\"'
                        FoundBackslashes++;
                    else
                    {
                        if('\"'==(*CurrentArgument)) //if a qoute is 
                        {
                            memset(CurrentPosition, '\\', FoundBackslashes);
                            CurrentPosition+=FoundBackslashes;

                            *(CurrentPosition++)='\\';
                        }
                        else
                        {
                            #if defined (UNIX)
                            if(0<FoundBackslashes)
                            {
                                memset(CurrentPosition, '\\', FoundBackslashes);
                                CurrentPosition+=FoundBackslashes;
                            }
                            #elif defined (WIN32)
                            #else
                                #error tni33_ArgumentList::CreateArgumentString() not coded for this operating system!
                            #endif
                        }
            
                        FoundBackslashes=0; //the row of backslashes was terminated, so other backslashes count for themself
                    }

                    *(CurrentPosition++)=(*CurrentArgument); //copy the character itself
                }

                #if defined (UNIX)
                if(0<FoundBackslashes)
                {
                    memset(CurrentPosition, '\\', FoundBackslashes);
                    CurrentPosition+=FoundBackslashes;
                }
                #elif defined (WIN32)
                #else
                    #error tni33_ArgumentList::CreateArgumentString() not coded for this operating system!
                #endif

                if(FoundASpace)
                    *(CurrentPosition++)='\"';

                *(CurrentPosition++)=' ';
            }

            *(CurrentPosition-1)='\0';  //the last arguments ' ' is replaced with the terminating zero

            if(Length-1!=strlen(CommandLine))
            {
               rc=0;
               ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseInternalError));
            }
        }
    }
    else
    {
        CommandLine=new char [1];

        if(0==CommandLine)
        {
            rc=0;
            ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory);
        }
        else
            CommandLine[0]='\0';
    }

    if(rc)
        NumberOfArgumentsOfCurrentCommandLine=NumberOfArguments;

    return rc;
}

const char * tni33_ArgumentList::GiveCommandLine()
{
    if(0==CommandLine || NumberOfArgumentsOfCurrentCommandLine!=NumberOfArguments)
        ConstructCommandLine();

    return CommandLine;
}

const char * tni33_ArgumentList::GiveArgument(int Number)
{
    const char *rc;

    if(0<=Number)
        rc=Arguments[(unsigned)Number]->GiveInfo();
    else
        rc=0;

    return rc;
}


// -----------------------------------------------------------------------------
// implementation of class tni33_ABackgroundProcess
// -----------------------------------------------------------------------------

tni33_ABackgroundProcess::tni33_ABackgroundProcess(const char        * Cmdline,
                                                   const tni34_AFile * StdIn,
                                                   const tni34_AFile * StdOut,
                                                   const int           AppendStdOut,
                                                   const tni34_AFile * StdErr,
                                                   const int           AppendStdErr,
                                                   const bool          DetachConsoleAtWindows)
{
    InitAndStartBackgroundProcess(Cmdline, StdIn, 1, StdOut, AppendStdOut, 1, StdErr, AppendStdErr, 1, DetachConsoleAtWindows);
}

tni33_ABackgroundProcess::tni33_ABackgroundProcess(const char        * Cmdline,
                                                   const tni34_AFile * StdIn,
                                                   const tni34_APipe * StdOut,
                                                   const int           AppendStdOut,
                                                   const tni34_AFile * StdErr,
                                                   const int           AppendStdErr,
                                                   const bool          DetachConsoleAtWindows)
{
    InitAndStartBackgroundProcess(Cmdline, StdIn, 1, StdOut, AppendStdOut, 0, StdErr, AppendStdErr, 1, DetachConsoleAtWindows);
}
    
tni33_ABackgroundProcess::tni33_ABackgroundProcess(const char        * Cmdline,
                                                   const tni34_APipe * StdIn,
                                                   const tni34_AFile * StdOut,
                                                   const int           AppendStdOut,
                                                   const tni34_AFile * StdErr,
                                                   const int           AppendStdErr,
                                                   const bool          DetachConsoleAtWindows)
{
    InitAndStartBackgroundProcess(Cmdline, StdIn, 0, StdOut, AppendStdOut, 1, StdErr, AppendStdErr, 1, DetachConsoleAtWindows);
}
    
tni33_ABackgroundProcess::tni33_ABackgroundProcess(const char        * Cmdline,
                                                   const tni34_APipe * StdIn,
                                                   const tni34_APipe * StdOut,
                                                   const int           AppendStdOut,
                                                   const tni34_AFile * StdErr,
                                                   const int           AppendStdErr,
                                                   const bool          DetachConsoleAtWindows)
{
    InitAndStartBackgroundProcess(Cmdline, StdIn, 0, StdOut, AppendStdOut, 0, StdErr, AppendStdErr, 1, DetachConsoleAtWindows);
}

tni33_ABackgroundProcess::tni33_ABackgroundProcess(tni33_ArgumentList & Arguments,
                                                   const tni34_AFile *  StdIn,
                                                   const tni34_AFile *  StdOut,
                                                   const int            AppendStdOut,
                                                   const tni34_AFile *  StdErr,
                                                   const int            AppendStdErr,
                                                   const bool           DetachConsoleAtWindows)
{
    InitAndStartBackgroundProcess(Arguments, StdIn, 1, StdOut, AppendStdOut, 1, StdErr, AppendStdErr, 1, DetachConsoleAtWindows);
}

void tni33_ABackgroundProcess::InitAndStartBackgroundProcess(const char             * Cmdline,
                                                             const tni34_ADataEntry * StdIn,
                                                             const int                StdInIsFile,
                                                             const tni34_ADataEntry * StdOut,
                                                             const int                AppendStdOut,
                                                             const int                StdOutIsFile,
                                                             const tni34_AFile      * StdErr,
                                                             const int                AppendStdErr,
                                                             const int                StdErrIsFile,
                                                             const bool               DetachConsoleAtWindows)
{
    m_Pid = RTE_UNDEF_OSPID;
    Process=AInvalidProcessHandle_ni33;
    RetCode=InvalidReturnCode_ni33;

    #if defined (WIN32)
    Thread=AInvalidProcessHandle_ni33;
    StandardInHandle=INVALID_HANDLE_VALUE;
    StandardOutHandle=INVALID_HANDLE_VALUE;
    StandardErrorHandle=INVALID_HANDLE_VALUE;
    
    OpenedStandardInHandle=0;
    OpenedStandardOutHandle=0;
    OpenedStandardErrorHandle=0;
    
    StandardInIsFile=StdInIsFile;
    StandardOutIsFile=StdOutIsFile;
    StandardErrorIsFile=StdErrIsFile;

    #elif defined (UNIX)
    #else
        #error  tni33_ABackgroundProcess::tni33_ABackgroundProcess() not coded for this operating system!
    #endif

    if(0==(CommandLine=new char[strlen(Cmdline)+1]))
        ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory);
    else
    {
        strcpy(CommandLine, Cmdline);
        
        #if defined (WIN32)
        SECURITY_ATTRIBUTES SecurityAttrib={sizeof(SECURITY_ATTRIBUTES), 0, TRUE};

        if(0!=StdIn)
        {
            StandardInHandle=CreateFile(StdIn->GiveName(),
                                        GENERIC_READ,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        & SecurityAttrib,
                                        OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL,
                                        NULL);

            OpenedStandardInHandle=1;
        }
        else
            StandardInHandle=GetStdHandle(STD_INPUT_HANDLE);

        if(INVALID_HANDLE_VALUE==StandardInHandle)
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess, CommandLine));
        else
        {
            if(0!=StdOut)
            {
                StandardOutHandle=CreateFile(StdOut->GiveName(),
                                             GENERIC_WRITE,
                                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                                             & SecurityAttrib,
                                             AppendStdOut==0 ?  CREATE_ALWAYS : OPEN_ALWAYS,
                                             FILE_ATTRIBUTE_NORMAL,
                                             NULL);

                OpenedStandardOutHandle=1;

                if(AppendStdOut &&
                   INVALID_HANDLE_VALUE!=StandardOutHandle &&
                   0xFFFFFFFF==SetFilePointer(StandardOutHandle, 0, 0, FILE_END))
                {
                    CloseHandle(StandardOutHandle);
                    StandardOutHandle=INVALID_HANDLE_VALUE;
                }
            }
            else
                StandardOutHandle=GetStdHandle(STD_OUTPUT_HANDLE);

            if(INVALID_HANDLE_VALUE==StandardOutHandle)
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess, CommandLine));
            else
            {
                if(0!=StdErr)
                {
                    StandardErrorHandle=CreateFile(StdErr->GiveName(),
                                                   GENERIC_WRITE,
                                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                   & SecurityAttrib,
                                                   AppendStdOut==0 ?  CREATE_ALWAYS : OPEN_ALWAYS,
                                                   FILE_ATTRIBUTE_NORMAL,
                                                   NULL);

                    OpenedStandardErrorHandle=1;

                    if(AppendStdErr &&
                       INVALID_HANDLE_VALUE!=StandardErrorHandle &&
                       0xFFFFFFFF==SetFilePointer(StandardErrorHandle, 0, 0, FILE_END))
                    {
                        CloseHandle(StandardErrorHandle);
                        StandardErrorHandle=INVALID_HANDLE_VALUE;
                    }
                }
                else
                    StandardErrorHandle=GetStdHandle(STD_ERROR_HANDLE);

                if(INVALID_HANDLE_VALUE==StandardErrorHandle)
                    ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess, CommandLine));
                else
                {
                    STARTUPINFO si={DWORD(sizeof(STARTUPINFO)),
                                    LPTSTR(0),
                                    LPTSTR(0),
                                    LPTSTR(0),
                                    DWORD(0),
                                    DWORD(0),
                                    DWORD(0),
                                    DWORD(0),
                                    DWORD(0),
                                    DWORD(0),
                                    DWORD(0),
                                    STARTF_USESTDHANDLES,
                                    WORD(0),
                                    WORD(0),
                                    LPBYTE(0),
                                    StandardInHandle,
                                    StandardOutHandle,
                                    StandardErrorHandle};
                    PROCESS_INFORMATION pi;

                    if(!CreateProcess(0, CommandLine, 0, 0, TRUE, (DetachConsoleAtWindows?DETACHED_PROCESS:0)|CREATE_NO_WINDOW, 0, 0, &si, &pi))
                    {
                        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess,CommandLine));
                        Process=AInvalidProcessHandle_ni33;
                    }
                    else
                    {
                        Process=pi.hProcess;
                        Thread=pi.hThread;
                        m_Pid = pi.dwProcessId;
                    }        
                }
            }
        }

        AdjustFileHandles();    //regardless if the new process was started or not, the file handles we got, should be adjusted, before further child's are created
        #elif defined (UNIX)

        int argc=0,p=0;
      
        while(CommandLine[p]!=0 && ni33IsWhiteSpace(CommandLine[p]))
            p++;

        while(CommandLine[p]!=0)
        {
            while(CommandLine[p]!=0 && !ni33IsWhiteSpace(CommandLine[p]))
                p++;
            argc++;
            while(CommandLine[p]!=0 && ni33IsWhiteSpace(CommandLine[p]))
                p++;
        }

        if(argc<=0)
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess,CommandLine));
            Process=AInvalidProcessHandle_ni33;
        }
        else
        {
            char **Arguments=new char * [argc+1];

            if(0==Arguments)
            {
                ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory);
                Process=AInvalidProcessHandle_ni33;
            }
            else
            {
                int i=0;
                for(i=0;i<=argc;i++)
                    Arguments[i]=0;

                p=0;
                i=0;

                while(CommandLine[p]!=0 && ni33IsWhiteSpace(CommandLine[p]))
                    p++;

                int p1,ErrorOccured=0;
                
                while(CommandLine[p]!=0 && !ErrorOccured)
                {
                    p1=p;

                    while(CommandLine[p1]!=0 && !ni33IsWhiteSpace(CommandLine[p1]))
                        p1++;

                    if(0==(Arguments[i]=new char [p1-p+1])) 
                    {
                        ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory);
                        Process=AInvalidProcessHandle_ni33;
                        ErrorOccured=1;
                    }
                    else
                    {
                        strncpy(Arguments[i],CommandLine+p,p1-p);
                        Arguments[i++][p1-p]=0;
                        p=p1;
                        while(CommandLine[p]!=0 && ni33IsWhiteSpace(CommandLine[p]))
                            p++;
                    }
                }

                if(!ErrorOccured)
                {
                    fflush(stdout); //empty any buffer used for stdout before forking (otherwise previous output can be duplicated)
                    fflush(stderr); //empty any buffer used for stderr (seems not to be necessary, but should not hurt)

                    if((Process=fork())<0)
                    {
                        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess,CommandLine));
                        Process=AInvalidProcessHandle_ni33;
                    }
                    else
                        if(Process==0) // following code is executed only by the child process
                        {
                            int GotAllFileDescriptors=1;
                            int Desc;

                            if(0!=StdIn)
                            {
                                fclose(stdin);
                                if(0!=(Desc=open(StdIn->GiveName(), O_RDONLY)))
                                {
                                    ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess, CommandLine));
                                    Process=AInvalidProcessHandle_ni33;
                                    GotAllFileDescriptors=0;
                                }
                            }

                            if(GotAllFileDescriptors && 0!=StdOut)
                            {
                                fclose(stdout);

                                if(AppendStdOut)
                                    Desc=open(StdOut->GiveName(), O_WRONLY|O_APPEND);
                                else
                                    Desc=open(StdOut->GiveName(), O_WRONLY|O_TRUNC); 

                                if(1!=Desc)
                                {
                                    ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess, CommandLine));
                                    Process=AInvalidProcessHandle_ni33;
                                    GotAllFileDescriptors=0;
                                }
                            }
                            
                            if(GotAllFileDescriptors && 0!=StdErr)
                            {
                                fclose(stderr);

                                if(AppendStdErr)
                                    Desc=open(StdErr->GiveName(), O_WRONLY|O_APPEND);
                                else
                                    Desc=open(StdErr->GiveName(), O_WRONLY|O_TRUNC); 

                                if(2!=Desc)
                                {
                                    ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess, CommandLine));
                                    Process=AInvalidProcessHandle_ni33;
                                    GotAllFileDescriptors=0;
                                }
                            }
                            if(GotAllFileDescriptors)
                            {
                                char *ProgramName=Arguments[0];
                                
                                execvp(ProgramName,Arguments);

                                //coming to this point means, that an error occurred. Report the reason and end the child-process.
                                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess,CommandLine));
                                exit(1);
                            }
                            else
                                exit(1); // could not change StdIn, StdOut or StdErr for the child as requested, error was allready reported
                        }
                        else {
                            // executed by parent only
                            m_Pid = Process;
                        }
                }        
                        
                for(i=0;i<argc;i++)
                    if(Arguments[i]!=0)
                        delete [] (Arguments[i]);

                delete [] Arguments;
            }
        }
        #else
            #error  tni33_ABackgroundProcess::tni33_ABackgroundProcess not coded for this operating system!
        #endif
    }

    if(AInvalidProcessHandle_ni33==Process)
        State=NotStarted;
    else
        State=Started;

    if(State!=Started)          // do not block the file handles if the process could not be started
        FreeFileHandles();
}

void tni33_ABackgroundProcess::InitAndStartBackgroundProcess(tni33_ArgumentList &     Arguments,
                                                             const tni34_ADataEntry * StdIn,
                                                             const int                StdInIsFile,
                                                             const tni34_ADataEntry * StdOut,
                                                             const int                AppendStdOut,
                                                             const int                StdOutIsFile,
                                                             const tni34_AFile      * StdErr,
                                                             const int                AppendStdErr,
                                                             const int                StdErrIsFile,
                                                             const bool               DetachConsoleAtWindows)
{
    m_Pid = RTE_UNDEF_OSPID;
    Process=AInvalidProcessHandle_ni33;
    RetCode=InvalidReturnCode_ni33;

    #if defined (WIN32)
    Thread=AInvalidProcessHandle_ni33;
    StandardInHandle=INVALID_HANDLE_VALUE;
    StandardOutHandle=INVALID_HANDLE_VALUE;
    StandardErrorHandle=INVALID_HANDLE_VALUE;
    
    OpenedStandardInHandle=0;
    OpenedStandardOutHandle=0;
    OpenedStandardErrorHandle=0;
    
    StandardInIsFile=StdInIsFile;
    StandardOutIsFile=StdOutIsFile;
    StandardErrorIsFile=StdErrIsFile;

    #elif defined (UNIX)
    #else
        #error  tni33_ABackgroundProcess::InitAndStartBackgroundProcess() not coded for this operating system!
    #endif

    if(Arguments.ConstructCommandLine())
    {
        if(0==(CommandLine=new char[strlen(Arguments.GiveCommandLine())+1]))
            ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory);
        else
        {
            strcpy(CommandLine, Arguments.GiveCommandLine());   //make a copy of the command line Windows can play with and which is available later in the object

            #if defined (WIN32)
            SECURITY_ATTRIBUTES SecurityAttrib={sizeof(SECURITY_ATTRIBUTES), 0, TRUE};

            if(0!=StdIn)
            {
                StandardInHandle=CreateFile(StdIn->GiveName(),
                                            GENERIC_READ,
                                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                                            & SecurityAttrib,
                                            OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL,
                                            NULL);

                OpenedStandardInHandle=1;
            }
            else
                StandardInHandle=GetStdHandle(STD_INPUT_HANDLE);

            if(INVALID_HANDLE_VALUE==StandardInHandle)
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess, CommandLine));
            else
            {
                if(0!=StdOut)
                {
                    StandardOutHandle=CreateFile(StdOut->GiveName(),
                                                 GENERIC_WRITE,
                                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                 & SecurityAttrib,
                                                 AppendStdOut==0 ?  CREATE_ALWAYS : OPEN_ALWAYS,
                                                 FILE_ATTRIBUTE_NORMAL,
                                                 NULL);

                    OpenedStandardOutHandle=1;

                    if(AppendStdOut &&
                       INVALID_HANDLE_VALUE!=StandardOutHandle &&
                       0xFFFFFFFF==SetFilePointer(StandardOutHandle, 0, 0, FILE_END))
                    {
                        CloseHandle(StandardOutHandle);
                        StandardOutHandle=INVALID_HANDLE_VALUE;
                    }
                }
                else
                    StandardOutHandle=GetStdHandle(STD_OUTPUT_HANDLE);

                if(INVALID_HANDLE_VALUE==StandardOutHandle)
                    ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess, CommandLine));
                else
                {
                    if(0!=StdErr)
                    {
                        StandardErrorHandle=CreateFile(StdErr->GiveName(),
                                                       GENERIC_WRITE,
                                                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                       & SecurityAttrib,
                                                       AppendStdOut==0 ?  CREATE_ALWAYS : OPEN_ALWAYS,
                                                       FILE_ATTRIBUTE_NORMAL,
                                                       NULL);

                        OpenedStandardErrorHandle=1;

                        if(AppendStdErr &&
                           INVALID_HANDLE_VALUE!=StandardErrorHandle &&
                           0xFFFFFFFF==SetFilePointer(StandardErrorHandle, 0, 0, FILE_END))
                        {
                            CloseHandle(StandardErrorHandle);
                            StandardErrorHandle=INVALID_HANDLE_VALUE;
                        }
                    }
                    else
                        StandardErrorHandle=GetStdHandle(STD_ERROR_HANDLE);

                    if(INVALID_HANDLE_VALUE==StandardErrorHandle)
                        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess, CommandLine));
                    else
                    {
                        STARTUPINFO si={DWORD(sizeof(STARTUPINFO)),
                                        LPTSTR(0),
                                        LPTSTR(0),
                                        LPTSTR(0),
                                        DWORD(0),
                                        DWORD(0),
                                        DWORD(0),
                                        DWORD(0),
                                        DWORD(0),
                                        DWORD(0),
                                        DWORD(0),
                                        STARTF_USESTDHANDLES,
                                        WORD(0),
                                        WORD(0),
                                        LPBYTE(0),
                                        StandardInHandle,
                                        StandardOutHandle,
                                        StandardErrorHandle};
                        PROCESS_INFORMATION pi;

                        if(!CreateProcess(0, CommandLine, 0, 0, TRUE, (DetachConsoleAtWindows?DETACHED_PROCESS:0)|CREATE_NO_WINDOW, 0, 0, &si, &pi))
                        {
                            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess,CommandLine));
                            Process=AInvalidProcessHandle_ni33;
                        }
                        else
                        {
                            Process=pi.hProcess;
                            Thread=pi.hThread;
                            m_Pid = pi.dwProcessId;
                        }        
                    }
                }
            }

            AdjustFileHandles();    //regardless if the new process was started or not, the file handles we got, should be adjusted, before further child's are created
            #elif defined (UNIX)

            int argc=Arguments.GiveNumberOfArguments();

            if(argc<=0)
            {
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess,CommandLine));
                Process=AInvalidProcessHandle_ni33;
            }
            else
            {
                char **Args=new char * [argc+1];

                if(0==Args)
                {
                    ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory);
                    Process=AInvalidProcessHandle_ni33;
                }
                else
                {
                    int i=0,
                        ErrorOccured=0;

                    for(i=0; i<=argc; i++)
                        Args[i]=0;

                    for(i=0; !ErrorOccured && i<argc; i++)
                    {
                        Args[i]=new char [strlen(Arguments.GiveArgument(i))+1];

                        if(0==Args[i])
                        {
                            ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory);
                            Process=AInvalidProcessHandle_ni33;
                            ErrorOccured=1;
                        }
                        else
                            strcpy(Args[i], Arguments.GiveArgument(i));
                    }

                    if(!ErrorOccured)
                    {
                        fflush(stdout); //empty any buffer used for stdout before forking (otherwise previous output can be duplicated)
                        fflush(stderr); //empty any buffer used for stderr (seems not to be necessary, but should not hurt)

                        if((Process=fork())<0)
                        {
                            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess,CommandLine));
                            Process=AInvalidProcessHandle_ni33;
                        }
                        else
                            if(Process==0) // following code is executed only by the child process
                            {
                                int GotAllFileDescriptors=1;
                                int Desc;

                                if(0!=StdIn)
                                {
                                    fclose(stdin);
                                    if(0!=(Desc=open(StdIn->GiveName(), O_RDONLY)))
                                    {
                                        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess, CommandLine));
                                        Process=AInvalidProcessHandle_ni33;
                                        GotAllFileDescriptors=0;
                                    }
                                }

                                if(GotAllFileDescriptors && 0!=StdOut)
                                {
                                    fclose(stdout);

                                    if(AppendStdOut)
                                        Desc=open(StdOut->GiveName(), O_WRONLY|O_APPEND);
                                    else
                                        Desc=open(StdOut->GiveName(), O_WRONLY|O_TRUNC); 

                                    if(1!=Desc)
                                    {
                                        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess, CommandLine));
                                        Process=AInvalidProcessHandle_ni33;
                                        GotAllFileDescriptors=0;
                                    }
                                }
                            
                                if(GotAllFileDescriptors && 0!=StdErr)
                                {
                                    fclose(stderr);

                                    if(AppendStdErr)
                                        Desc=open(StdErr->GiveName(), O_WRONLY|O_APPEND);
                                    else
                                        Desc=open(StdErr->GiveName(), O_WRONLY|O_TRUNC); 

                                    if(2!=Desc)
                                    {
                                        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess, CommandLine));
                                        Process=AInvalidProcessHandle_ni33;
                                        GotAllFileDescriptors=0;
                                    }
                                }
                        
                                if(GotAllFileDescriptors)
                                {
                                    const char *ProgramName=Args[0];
                                
                                    execvp(ProgramName, Args);

                                    //coming to this point means, that an error occurred. Report the reason and end the child-process.
                                    ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateChildProcess,CommandLine));
                                    exit(1);
                                }
                                else
                                    exit(1); // could not change StdIn, StdOut or StdErr for the child as requested, error was allready reported
                            }
                            else {
                                // executed by parent only
                                m_Pid = Process;
                            }
                    }        
                        
                    for(i=0; i<argc; i++)
                        if(0!=Args[i])
                            delete [] (Args[i]);

                    delete [] Args;
                }
            }
            #else
                #error  tni33_ABackgroundProcess::tni33_ABackgroundProcess not coded for this operating system!
            #endif
        }
    }

    if(AInvalidProcessHandle_ni33==Process)
        State=NotStarted;
    else
        State=Started;

    if(State!=Started)          // do not block the file handles if the process could not be started
        FreeFileHandles();
}

int tni33_ABackgroundProcess::AdjustFileHandles()
{
    int rc=1;

    #if defined (WIN32)
    if(OpenedStandardInHandle &&
       INVALID_HANDLE_VALUE!=StandardInHandle &&
       0==SetHandleInformation(StandardInHandle, HANDLE_FLAG_INHERIT, !HANDLE_FLAG_INHERIT))
    {
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotSetHandleInformation, "standard in"));
        rc=0;
    }

    if(OpenedStandardOutHandle &&
       INVALID_HANDLE_VALUE!=StandardOutHandle &&
       0==SetHandleInformation(StandardOutHandle, HANDLE_FLAG_INHERIT, !HANDLE_FLAG_INHERIT))
    {
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotSetHandleInformation, "standard out"));

        rc=0;
    }

    if(OpenedStandardErrorHandle &&
       INVALID_HANDLE_VALUE!=StandardErrorHandle &&
       0==SetHandleInformation(StandardErrorHandle, HANDLE_FLAG_INHERIT, !HANDLE_FLAG_INHERIT))
    {
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotSetHandleInformation, "standard error"));

        rc=0;
    }
    #elif defined (UNIX)
    #else
        #error tni33_ABackgroundProcess::AdjustFileHandles() not coded for this operating system!
    #endif

    return rc;
}

void tni33_ABackgroundProcess::FreeFileHandles()
{
    #if defined (WIN32)
    if(OpenedStandardInHandle && INVALID_HANDLE_VALUE!=StandardInHandle)    //close StandardInHandle also if it is a pipe (we get here only if the processes is dead already)
    {
        CloseHandle(StandardInHandle);
        StandardInHandle=INVALID_HANDLE_VALUE; // don't close it again
    }

    if(StandardOutIsFile && OpenedStandardOutHandle && INVALID_HANDLE_VALUE!=StandardOutHandle)
    {
        CloseHandle(StandardOutHandle);
        StandardOutHandle=INVALID_HANDLE_VALUE; // don't close it again
    }

    if(StandardErrorIsFile && OpenedStandardErrorHandle && INVALID_HANDLE_VALUE!=StandardErrorHandle)
    {
        CloseHandle(StandardErrorHandle);
        StandardErrorHandle=INVALID_HANDLE_VALUE; // don't close it again
    }
    #elif defined (UNIX)
    #else
        #error tni33_ABackgroundProcess::FreeFileHandles() not coded for this operating system!
    #endif
}

tni33_ABackgroundProcess::~tni33_ABackgroundProcess()
{
    if(CommandLine!=0)
        delete [] CommandLine;

    #if defined (WIN32)
    if(AInvalidProcessHandle_ni33!=Process)
        CloseHandle(Process);

    if(AInvalidProcessHandle_ni33!=Thread)
        CloseHandle(Thread);

    if(OpenedStandardInHandle && INVALID_HANDLE_VALUE!=StandardInHandle)
        CloseHandle(StandardInHandle);

    if(OpenedStandardOutHandle && INVALID_HANDLE_VALUE!=StandardOutHandle)
        CloseHandle(StandardOutHandle);

    if(OpenedStandardErrorHandle && INVALID_HANDLE_VALUE!=StandardErrorHandle)
        CloseHandle(StandardErrorHandle);
    
    #elif defined (UNIX)
    #else
        #error tni33_ABackgroundProcess::~tni33_ABackgroundProcess() not coded for this operating system!
    #endif
}

int tni33_ABackgroundProcess::Stop()
{
    // All kill operations are brute force without cleanup !
    int rc=0;

    if(State==Started)
    {
        #if defined (WIN32)
        if(TerminateProcess(Process, 256))  //we use an arbitrary number different from 0 as exit code for the child process, 256 made it, as exit codes on UNIX range only from 0 to 255.
        {
            Sleep(0); //give up time slice, so that terminated process or OS has a chance to use exit code 256 set above

            State=Stopped;
            if(GetExitCodeProcess(Process, &RetCode))
            {
                rc=1;
                CloseHandle(Process);               //TODO: check return code of CloseHandle()
                FreeFileHandles();
                Process=AInvalidProcessHandle_ni33;
            }
            else
            {
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotGetReturnCode, CommandLine));
                RetCode=InvalidReturnCode_ni33;
            }
        }
        else
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotStopProcess, CommandLine));

        #elif defined(UNIX)
	    errno=0;
        if(kill(Process, SIGKILL))
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotStopProcess, CommandLine));
        else
        {
            State=Stopped;
            
            int RCProcess;
            pid_t ReturnedPID;

            do
            {
                errno=0;
                //if(Process==waitpid(Process,&RCProcess,WNOHANG))
                ReturnedPID=waitpid(Process, &RCProcess, 0);    // we hope, that the kill signal will end the process so that the waitpid-function will return, when the OS has done it's work of killing the process
            }
            while(((pid_t)-1)==ReturnedPID && EINTR==errno);    //we are not interested in signals ending waitpid (the kill signal will kill us any day)

            if(Process==ReturnedPID)
            {
                rc=1;
                Process=AInvalidProcessHandle_ni33;
            
                if(WIFEXITED(RCProcess))
                    RetCode=WEXITSTATUS(RCProcess);
                else
                    if(WIFSIGNALED(RCProcess))
                        RetCode=InvalidReturnCode_ni33;
                    else
                        RetCode=InvalidReturnCode_ni33;
            }
            else
            {
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotGetReturnCode, CommandLine));
                RetCode=InvalidReturnCode_ni33;
            }
        }
        #else
            #error tni33_ABackgroundProcess::Stop() was only coded for WIN32 and UNIX (define one of them).
        #endif
    }

    return rc;
}

int tni33_ABackgroundProcess::Wait()
{
    int rc=0;

    if(State==Started)
    {
        #if defined (WIN32)
        if(WAIT_FAILED==WaitForSingleObject(Process,INFINITE))
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseWaitForProcessFailed, CommandLine));
        if(GetExitCodeProcess(Process,&RetCode))
        {
            rc=1;
            CloseHandle(Process);
            FreeFileHandles();
            Process=AInvalidProcessHandle_ni33;
            State=Stopped;
        }
        else
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotGetReturnCode, CommandLine));
            RetCode=InvalidReturnCode_ni33;
        }

        #elif defined (UNIX)
        int RCProcess=0;
        pid_t ReturnedPID;

        do
        {
            errno=0;
            ReturnedPID=waitpid(Process, &RCProcess, 0);
        }
        while(((pid_t)-1)==ReturnedPID && EINTR==errno);    //we are not interested in signals ending waitpid (the kill signal will kill us anyday)

        if(Process==ReturnedPID)
        {
            rc=1;
            Process=AInvalidProcessHandle_ni33;
            State=Stopped;
            
            if(WIFEXITED(RCProcess))
                RetCode=WEXITSTATUS(RCProcess);
            else
                if(WIFSIGNALED(RCProcess))
                    RetCode=InvalidReturnCode_ni33;
                else
                    RetCode=InvalidReturnCode_ni33;
        }
        else
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseWaitForProcessFailed,CommandLine));
            RetCode=InvalidReturnCode_ni33;
        }
        #else
            #error tni33_ABackgroundProcess::Wait() not coded for this operating system!
        #endif
    }
    else
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseProcessNotStarted, CommandLine));

    return rc;
}

int tni33_ABackgroundProcess::IsRunning()
{
    int rc=0;

    if(State==Started)
    {
        #if defined (WIN32)
        if(!GetExitCodeProcess(Process,&RetCode))
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseWaitForProcessFailed,CommandLine));
        else
        {
            if(STILL_ACTIVE==RetCode)
                rc=1;
            else
            {
                CloseHandle(Process);
                FreeFileHandles();
                Process=AInvalidProcessHandle_ni33;
                State=Stopped;
            }
        }

        #elif defined (UNIX)
        int RCProcess;

        pid_t ReturnedPID;
        
        do
        {
            errno=0;
            ReturnedPID=waitpid(Process, &RCProcess, WNOHANG);
        }
        while(((pid_t)-1)==ReturnedPID && EINTR==errno);    //we are not interested in signals ending waitpid (the kill signal will kill us anyday)

        if(0==ReturnedPID)
            rc=1;
        else
            if(Process!=ReturnedPID)
            {
                RetCode=InvalidReturnCode_ni33;
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseWaitForProcessFailed,CommandLine));
            }
            else
            {
                Process=AInvalidProcessHandle_ni33;
                State=Stopped;

                if(WIFEXITED(RCProcess))
                    RetCode=WEXITSTATUS(RCProcess);
                else
                    if(WIFSIGNALED(RCProcess))
                        RetCode=InvalidReturnCode_ni33;
                    else
                        RetCode=InvalidReturnCode_ni33;
            }
        #else
            #error tni33_ABackgroundProcess::Wait() not coded for this operating system!
        #endif
    }

    return rc;
}

void tni33_ABackgroundProcess::WaitForMultipleBackgroundProcesses(int NoProcs,tni33_ABackgroundProcess **Procs)
{
    int i;
    for(i=0;i<NoProcs;i++)
        Procs[i]->Wait();
}


// -----------------------------------------------------------------------------
// implementation of class tni33_AProcess
// -----------------------------------------------------------------------------

tni33_AProcess::tni33_AProcess(const char        * Cmdline,
                               const tni34_AFile * StdIn,
                               const tni34_AFile * StdOut,
                               const int           AppendStdOut,
                               const tni34_AFile * StdErr,
                               const int           AppendStdErr)
    :P(Cmdline, StdIn, StdOut, AppendStdOut, StdErr, AppendStdErr)
{
    if(P.WasStarted())
        P.Wait();
}

tni33_AProcess::tni33_AProcess(tni33_ArgumentList & Arguments,
                               const tni34_AFile *  StdIn,
                               const tni34_AFile *  StdOut,
                               const int            AppendStdOut,
                               const tni34_AFile *  StdErr,
                               const int            AppendStdErr)
    :P(Arguments, StdIn, StdOut, AppendStdOut, StdErr, AppendStdErr)
{
    if(P.WasStarted())
        P.Wait();
}
