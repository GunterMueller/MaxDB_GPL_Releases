/*!*****************************************************************************
  module:       vni34.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: ni layer
  description:  implementing classes handling files, directories and pipes
  last change:  1999-10-04 15:32
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

//
//   FreeBSD portions added by Kai Mosebach, 
//   For more informations see : http://www.komadev.de/sapdb
//



*******************************************************************************/


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "hni34.h"
#include "hni35.h"

#include <string.h> // must be before definition of _XOPEN_SOURCE_EXTENDED

#if defined (WIN32)
    #include <direct.h>
    #include <windows.h>
#elif defined (UNIX)
    #if defined (OSF1)
        #define _XOPEN_SOURCE_EXTENDED
    #endif
    #include <fcntl.h>
    #include <sys/uio.h>
    #include <unistd.h>
    #include <poll.h>
    #include <errno.h>
#else
    #error vni34.cpp was only coded for WIN32 and UNIX (define one of them).
#endif

#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>


// -----------------------------------------------------------------------------
// file local functions and constants
// -----------------------------------------------------------------------------

static const char * ErrorMsgToCString(tsp00_C40 ErrorText)
{
    static char CString[41];
    int Size=40;

    if(Size>sizeof(tsp00_C40))
        Size=sizeof(tsp00_C40);

    strncpy(CString, ErrorText, Size);
    CString[Size]='\0';

    return CString;
}

#if defined (WIN32)
const char * PathDelimiter_ni34="\\";
#elif defined (UNIX)
const char * PathDelimiter_ni34="/";
#else
    #error vni34.cpp is only coded for WIN32 and UNIX (define one of them)
#endif


// -----------------------------------------------------------------------------
// member functions of class tni34_AFileSystemEntry
// -----------------------------------------------------------------------------

tni34_AFileSystemEntry::tni34_AFileSystemEntry(const char *name, int CreateOnCheck)
  : IsGoodFlag(1),
    CreateOnCheckFlag(CreateOnCheck),
    IsOpenFlag(0)
{
    SetNameTo(name);
}

tni34_AFileSystemEntry::~tni34_AFileSystemEntry()
{
    if(0!=Name)
        delete [] Name;
}

int tni34_AFileSystemEntry::Exists()
{
    int rc=1;

    #if defined (WIN32)
    struct _stat s_stat;
    if(-1==_stat(Name,&s_stat))
        IsGoodFlag=rc=0;
    #elif defined (UNIX)
    struct stat s_stat;
    if(-1==stat(Name,&s_stat))
        IsGoodFlag=rc=0;
    #else
        #error tni34_AFileSystemEntry::Exists() only coded for WIN32 and UNIX (Define one of them.)
    #endif

    /*  TODO: if this works with files, pipes and directories it can replace the code above
    tsp05_RteFileInfo  Info;
    tsp05_RteFileError Error;
    
    sqlfinfoc(Name, &Info, &Error);

    if(!Info.sp5fi_exists)
        rc=0;
    */

    return rc;
}

int tni34_AFileSystemEntry::Check()
{
    return 0;
}

int tni34_AFileSystemEntry::Create()
{
    return 0;
}

int tni34_AFileSystemEntry::Remove()
{
    return 0;
}

int tni34_AFileSystemEntry::SetNameTo(const char *name)
{
    int rc=1;

    if(0!=name)
    {
        Name=new char[strlen(name)+1];

        if(0==Name)
        {
            ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory.Set());
            IsGoodFlag=rc=0;
        }
        else
            strcpy(Name, name);
    }
    else
        Name=0;

    return rc;
}

int tni34_AFileSystemEntry::ResetNameTo(const char *NewName)
{
    if(0!=Name)
        delete [] Name;

    return SetNameTo(NewName);
}

// -----------------------------------------------------------------------------
// member functions of class tni34_ADirectoryEntry
// -----------------------------------------------------------------------------

tni34_ADirectoryEntry::tni34_ADirectoryEntry(const char * name)
:tni34_AFileSystemEntry(name)
{
}


// -----------------------------------------------------------------------------
// member functions of class tni34_ADirectory
// -----------------------------------------------------------------------------

tni34_ADirectory::tni34_ADirectory(const char *name, int CreateOnCheck, int CheckNow)
    :tni34_AFileSystemEntry(0, CreateOnCheck),  //do not put name to the base class as it is (must be processed before, see below)
     DirectoryEntries(0),
     NumberOfDirectoryEntries(0),
     PlaceForEntries(0),
     NumberOfCurrentReadEntry(0)
{
    if(0!=name)
    {
        char * CorrectedName=new char[strlen(name)+1];

        if(0==CorrectedName)
        {
            ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory.Set());
            IsGoodFlag=0;
        }
        else
        {
            size_t LengthOfPathDelimiter=strlen(PathDelimiter_ni34);
            size_t Position=strlen(name);

            strcpy(CorrectedName, name);    //we now have a copy of name to play with            

            while(LengthOfPathDelimiter<Position && 0==strcmp(CorrectedName+(Position-LengthOfPathDelimiter), PathDelimiter_ni34))  // "<" because at least one path delimiter is allowed ("\", "/" are paths)
            {
                Position-=LengthOfPathDelimiter;
                CorrectedName[Position]='\0';
            }

            if(SetNameTo(CorrectedName))
            {
                if(CheckNow)
                    Check();
            }

            delete [] CorrectedName;
        }   
    }
}

tni34_ADirectory::~tni34_ADirectory()
{
    FreeDirectoryEntrys();
}

int tni34_ADirectory::IsADirectory()
{
    int rc;

    #if defined (WIN32)
    struct _stat s_stat;
    if(-1!=_stat(GiveName(),&s_stat) && 0!=(s_stat.st_mode & _S_IFDIR))
    #elif defined (UNIX)
    struct stat s_stat;
    if(-1!=stat(GiveName(),&s_stat) && 0!=(s_stat.st_mode & S_IFDIR))
    #else
        #error tni34_ADirectory::IsADirectory() was only coded for UNIX and WIN32 (define one of them).
    #endif
        rc=1;
    else
        IsGoodFlag=rc=0;

    return rc;
}

int tni34_ADirectory::Check()
{
    int rc;

    if(Exists())
    {
        rc=IsADirectory();

        if(0==rc)
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseExistsAndIsNoDirectory, GiveName()));
    }
    else
        if(CreateOnCheckFlag)
            rc=Create();
        else
        {
            IsGoodFlag=rc=0;
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseDirectoryDoesNotExist, GiveName()));
        }

    return rc;
}

int tni34_ADirectory::Create()
{
    int rc=1;

    #if defined (WIN32)
    if(_mkdir(GiveName()))
        rc=0;
    #elif defined (UNIX)
    mode_t OldFileCreationMask=umask(S_IROTH|S_IWOTH);  // dont let the file creation mask be in our way

    if(mkdir(GiveName(), S_IRWXU|S_IRWXG))
        rc=0;

    umask(OldFileCreationMask); // try to leave the file creation mask unchanged
    #else
        #error tni34_ADirectory::Create was only coded for WIN32 and UNIX (define one of them).
    #endif

    if(!rc)
    {
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateDirectory,GiveName()));
        IsGoodFlag=0;
    }

    return rc;
}

int tni34_ADirectory::Open()
{
    int rc=1;

    FreeDirectoryEntrys();

    #if defined (WIN32)
    WIN32_FIND_DATA CurrentOSEntry;
    size_t          DirectoryNameLength=strlen(GiveName());
    char *          NamesToFind=new char [DirectoryNameLength+3];

    if(0==NamesToFind)
    {
        ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory.Set());
        IsGoodFlag=rc=0;
    }
    else
    {
        strcpy(NamesToFind, GiveName());
        strcpy(NamesToFind+DirectoryNameLength, "\\*");
        
        HANDLE DirectoryHandle=FindFirstFile(NamesToFind, &CurrentOSEntry);
        
        delete [] NamesToFind;

        if(INVALID_HANDLE_VALUE==DirectoryHandle)
        {

    #elif defined (UNIX)
    const dirent * CurrentOSEntry;

    {
        DIR *DirectoryHandle=opendir(GiveName());

        if(0==DirectoryHandle)
        {

    #else 
        #error tni34_ADirectory::Open() was only coded for WIN32 and UNIX (define one of them).
    #endif

            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotOpenDirectory, GiveName()));
            IsGoodFlag=rc=0;
        }
        else
        {
            int FoundAll=0;

            #if defined (WIN32)
            rc=AppendDirectoryEntry(CurrentOSEntry.cFileName); //on Windows opening the search returns already the first file name
            #elif defined (UNIX)
            #else 
                #error tni34_ADirectory::Open() was only coded for WIN32 and UNIX (define one of them).
            #endif
            
            while(rc && !FoundAll)
            {
                #if defined (WIN32)
                if(!FindNextFile(DirectoryHandle, &CurrentOSEntry))
                {
                    if(ERROR_NO_MORE_FILES==GetLastError())

                #elif defined (UNIX)
                errno=0;
                CurrentOSEntry=readdir(DirectoryHandle);

                if(0==CurrentOSEntry)
                {
                    if(0==errno)

                #else 
                    #error tni34_ADirectory::Open() was only coded for WIN32 and UNIX (define one of them).
                #endif
                        FoundAll=1;
                    else
                    {
                        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotReadInDirectory, GiveName()));
                        IsGoodFlag=rc=0;
                    }
                }
                else
                {
                    #if defined (WIN32)
                    rc=AppendDirectoryEntry(CurrentOSEntry.cFileName);
                    #elif defined (UNIX)
                    rc=AppendDirectoryEntry(CurrentOSEntry->d_name);
                    #else 
                        #error tni34_ADirectory::Open() was only coded for WIN32 and UNIX (define one of them).
                    #endif
                }
            }

            #if defined (WIN32)
            FindClose(DirectoryHandle);
            #elif defined (UNIX)
            closedir(DirectoryHandle);
            #else 
                #error tni34_ADirectory::Open() was only coded for WIN32 and UNIX (define one of them).
            #endif
        }
    }

    return rc;
}

int tni34_ADirectory::Read(const char * & NextEntry)
{
    int rc=0;

    NextEntry=0;

    if(0!=DirectoryEntries)
    {
        if(0<=NumberOfCurrentReadEntry &&
           NumberOfCurrentReadEntry<NumberOfDirectoryEntries)
        {
            NextEntry=DirectoryEntries[NumberOfCurrentReadEntry++]->GiveName();
            rc=1;
        }
        else
            if(NumberOfCurrentReadEntry==NumberOfDirectoryEntries)
            {
                NumberOfCurrentReadEntry++;
                rc=1;
            }
    }

    return rc;
}

int tni34_ADirectory::Remove()
{
    int rc=1;

    #if defined (WIN32)
    if(_rmdir(GiveName()))
    #elif defined (UNIX)
    if(rmdir(GiveName()))
    #else
        #error tni34_ADirectory::Remove() was only coded for WIN32 and UNIX (define one of them).
    #endif
    {
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotRemoveDirectory,GiveName()));
        IsGoodFlag=rc=0;
    }

    return rc;
}

int tni34_ADirectory::IncreaseDirectoryEntriesIfNeeded()
{
    int rc=1;

    if(NumberOfDirectoryEntries==PlaceForEntries) //if all slots in the array are used up
    {
        int                     NewSize=PlaceForEntries;
        tni34_ADirectoryEntry **NewEntryField=0;
        
        if(0==NewSize)
            NewSize=20; //get an initial array with 20 entries
        else
            NewSize*=2; //get a new array of twice the size

        NewEntryField=new tni34_ADirectoryEntry * [NewSize];

        if(0==NewEntryField)
        {
            ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory.Set());
            IsGoodFlag=rc=0;
        }
        else
        {
            if(0<NumberOfDirectoryEntries) //if there are any old entries
            {
                memcpy(NewEntryField, DirectoryEntries, NumberOfDirectoryEntries*sizeof(tni34_ADirectoryEntry *)); //copy them
                delete [] DirectoryEntries;                                                                        //free the space of their old array
            }

            PlaceForEntries=NewSize;
            DirectoryEntries=NewEntryField;
        }
    }

    return rc;
}

int tni34_ADirectory::AppendDirectoryEntry(const char * EntryName)
{
    int rc=IncreaseDirectoryEntriesIfNeeded();

    if(rc)
    {
        DirectoryEntries[NumberOfDirectoryEntries]=new tni34_ADirectoryEntry(EntryName);

        if(0==DirectoryEntries[NumberOfDirectoryEntries])
        {
            ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory.Set());
            IsGoodFlag=rc=0;
        }
        else
        {
            if(!DirectoryEntries[NumberOfDirectoryEntries]->IsGood())
                IsGoodFlag=rc=0;
            else
                NumberOfDirectoryEntries++;
        }
    }

    return rc;
}

void tni34_ADirectory::FreeDirectoryEntrys()
{
    if(0!=DirectoryEntries)
    {
        for(int i=0; i<NumberOfDirectoryEntries; i++)
            delete DirectoryEntries[i];

        delete [] DirectoryEntries;

        DirectoryEntries=0;
    }

    NumberOfDirectoryEntries=0;
    PlaceForEntries=0;
    NumberOfCurrentReadEntry=0;
}


// -----------------------------------------------------------------------------
// member functions of class tni34_ADataEntry
// -----------------------------------------------------------------------------

static tni34_ADirectory TemporaryDataEntryDirectory_cn353(P_tmpdir , 0, 0);
tni34_ADirectory * tni34_ADataEntry::TemporaryDataEntryDirectory=0;

tni34_ADataEntry::tni34_ADataEntry(const char * name, int CreateOnOpen, int CreateOnCheck)
  : tni34_AFileSystemEntry(name, CreateOnCheck),
    CreateOnOpenFlag(CreateOnOpen)
{
}

int tni34_ADataEntry::SetTemporaryDataEntryDirectory(const tni34_ADirectory & NewTempDir)
{
    int rc=1;

    if(0!=TemporaryDataEntryDirectory)
        delete TemporaryDataEntryDirectory;
    
    TemporaryDataEntryDirectory=new tni34_ADirectory(NewTempDir.GiveName(), 0, 0);

    if(0==TemporaryDataEntryDirectory)
    {
        rc=0;
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::OutOfMemory));
    }
    else
        if(!NewTempDir.IsGood())
            rc=0;
  
    return rc;
}

int tni34_ADataEntry::CreateTemporaryName(char * &                 TempName,
                                          const tni34_ADirectory * TempDir,
                                          int RepeatedRequest)
{
    int rc=1;

    static unsigned long AttemptNumber=0;
    static time_t        AttemptTime=0;
    time_t CurrentTime=time(0);

    if(0==TemporaryDataEntryDirectory)
    {
        tni34_ADirectory StandardTempDir(P_tmpdir , 0, 0);  //P_tmpdir should be defined in stdio.h

        if(StandardTempDir.IsGood())
            rc=SetTemporaryDataEntryDirectory(StandardTempDir);
    }

    if(rc)
    {
        const char * Version;
        const char   VersionDelimiter='.';
        const char   AttemptDelimiter='-';
        int          VersionConsistsOfDigits=1;
        size_t       BaseNameLength;
        size_t Length=0;            

        if(RepeatedRequest && 0!=TempName)
        {
            BaseNameLength=strlen(TempName);
            Version=strrchr(TempName, VersionDelimiter);

            if(0==Version || 0!=strstr(Version, PathDelimiter_ni34)) // no '.' or a '\\' (or '/' on UNIX) after the '.' 
                Version="";
            else
            {
                const char *h;

                BaseNameLength-=strlen(Version);

                for(h=(++Version); '\0'!=(*h) && VersionConsistsOfDigits; h++)
                    if(!isdigit(*h))
                        VersionConsistsOfDigits=0;
            }
        }

        if(!RepeatedRequest || 0==TempName || '\0'==TempName[0] || !VersionConsistsOfDigits)    //we have to start from beginning with the temporary name
        {
            char         TimeAsString[21];      //20 decimal digits should work for the next 3170979 million years ;) and long but TODO use real string
            char         AttemptAsString[21];   //should work for a long but TODO use real string
            const char * TempDirName;
            int          TempDirEndsWithDelimiter=1;
            const char * TempPrefix="temp";

            if(AttemptTime!=CurrentTime)    //different attempts for different files/pipes in same second get different AttamptNumbers
            {
                AttemptNumber=0;
                AttemptTime=CurrentTime;
            }
            else
                AttemptNumber++;

            if(0==TempDir)                              //if no directory is given...
                TempDir=TemporaryDataEntryDirectory;    //use the default directory for temporary files/pipes

            TempDirName=TempDir->GiveName();    //just an abbreviation

            Length=strlen(TempDirName);         //the name we construct starts with the directory for temporary files/pipes
        
            if(strlen(TempDirName)>strlen(PathDelimiter_ni34) &&                //if the TempDir not already ends with an PathDelimiter ...
               0!=strcmp(PathDelimiter_ni34, TempDirName+(strlen(TempDirName)-strlen(PathDelimiter_ni34))))
            {
                TempDirEndsWithDelimiter=0;                                     //...be prepared to insert one
                Length+=strlen(PathDelimiter_ni34);                             //count the space for that delimiter 
            }

            Length+=strlen(TempPrefix);         //the name then contains the hard coded prefix "temp" to allow easier identification as temp files/pipes by humans

            sprintf(TimeAsString, "%ld", long(AttemptTime));    //convert the AttemptTime to a string
            Length+=strlen(TimeAsString);                       //count the space needed for that

            sprintf(AttemptAsString, "%lu", (unsigned long)AttemptNumber);  //convert the AttemptNumber to a string
            Length+=(1+strlen(AttemptAsString));                            //count the space needed for that

            if(0!=TempName)             //any old name?
                delete [] TempName;     //free it's memory  -> calling functions must take care of that

            TempName=new char[Length+1];//get needed amount of memory for the name

            if(0==TempName)
            {
                rc=0;
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::OutOfMemory));
            }
            else    //so lets build the name
                sprintf(TempName, "%s%s%s%s%c%s", TempDir->GiveName(), TempDirEndsWithDelimiter?"":PathDelimiter_ni34, TempPrefix, TimeAsString, AttemptDelimiter, AttemptAsString);
        }
        else
        {
            int    NewVersionAsInt;
            char   NewVersion[31];
            char * NewTempName;
        
            if(0==strlen(Version))              //was there no version in the string?
                NewVersionAsInt=0;              //let's start with 0
            else
                NewVersionAsInt=1+atoi(Version);//otherwise transform it to an integer and increment it by 1

            sprintf(NewVersion, "%d", NewVersionAsInt); //convert it back to an integer

            Length=BaseNameLength+1+strlen(NewVersion); //we need place for the unchanged base, a '.' and the new version number as string

            NewTempName=new char[Length+1]; //get needed amount of memory for the name

            if(0==NewTempName)
            {
                rc=0;
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::OutOfMemory));
            }
            else
            {
                strncpy(NewTempName, TempName, BaseNameLength);                                             //copy the base part
                sprintf(NewTempName+BaseNameLength, "%c%s", char(VersionDelimiter), NewVersion);  //append a '.' and the new version number as string
            
                delete [] TempName;     //free the old name (it is unequal 0) -> calling functions must take care of that
                TempName=NewTempName;   //return the newly constructed temporary name
            }
        }
    }

    return rc;
}

// -----------------------------------------------------------------------------
// member functions of class tni34_AFile
// -----------------------------------------------------------------------------

tni34_AFile::tni34_AFile(ADirection Dir,
                         int CreateOnOpen,
                         int OpenNow,
                         int CreateOnCheck,
                         int PrefBufSize,
                         int MaxNumberOfRetries)
  : tni34_ADataEntry(0, CreateOnOpen, CreateOnCheck),
    DirectionOfFile(Dir),
    PrefferedBufferSize(PrefBufSize),
    HandleForFile(InvalidFileHandle_ni34)
{
    char *TempName=0;

    int i=0,
        Created,
        TempNameOK=1;

    do
    {
        if(CreateTemporaryName(TempName, 0, i++))   //get a temporary name, use no special directory and handle the repeated flag through i
            Created=PureQuietCreate(TempName);
        else
            TempNameOK=0;
    }
    while(TempNameOK && !Created && i<MaxNumberOfRetries); // i<MaxNumberOfRetries -> MaxNumberOfRetries processes can try to create a temporary file in the same second

    if(Created)
    {
        if(ResetNameTo(TempName))
        {
            if(OpenNow)
                if(!PureOpen())
                {
                    Remove();
                    IsGoodFlag=0;
                }
        }
        else
        {
            PureQuietClose(); // TODO: should be PureQuietRemove, same for temporary Pipes
            IsGoodFlag=0;
        }
    }
    else
    {
        IsGoodFlag=0;

        if(TempNameOK)
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateFile, TempName));

        ResetNameTo("");    //try to simulate a name
    }

    if(0!=TempName)
        delete [] TempName;
}

tni34_AFile::tni34_AFile(const char *name,
                         ADirection Dir,
                         int CreateOnOpen,
                         int OpenNow,
                         int CreateOnCheck,
                         int PrefBufSize)
  : tni34_ADataEntry(name, CreateOnOpen, CreateOnCheck),
    DirectionOfFile(Dir),
    PrefferedBufferSize(PrefBufSize),
    HandleForFile(InvalidFileHandle_ni34)
{
    if(OpenNow)
        Open();
}

tni34_AFile::~tni34_AFile()
{
    Close();
}

int tni34_AFile::IsAFile()
{
    int rc;

    #if defined (WIN32)
    struct _stat s_stat;
    if(-1!=_stat(GiveName(),&s_stat) && 0!=(s_stat.st_mode & _S_IFREG))
    #elif defined (UNIX)
    struct stat s_stat;
    if(-1!=stat(GiveName(),&s_stat) && 0!=(s_stat.st_mode & S_IFREG))
    #else
        #error File::IsAFile() not coded for this operating system
    #endif
        rc=1;
    else
        IsGoodFlag=rc=0;

    return rc;
}

int tni34_AFile::Check()
{
    int rc;

    if(Exists())
    {
        rc=IsAFile();

        if(0==rc)
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseExistsAndIsNoFile, GiveName()));
    }
    else
        if(CreateOnCheckFlag)
            rc=Create();
        else
        {
            IsGoodFlag=rc=0;
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseFileDoesNotExist, GiveName()));
        }

   return rc;
}

int tni34_AFile::Create()
{
    int rc=0; // TODO : other checks Close(), Remove(), ...???

    if(InvalidFileHandle_ni34==HandleForFile)
        rc=PureCreate();

    return rc;
}

int tni34_AFile::Open()
{
    int rc=0;

    if(Exists())
    {
        if(IsAFile())
            rc=PureOpen();
        else
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseExistsAndIsNoFile,GiveName()));
    }
    else
    {
        if(CreateOnOpenFlag)
        {
            if(Create())
                rc=PureOpen();
        }
        else
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseFileDoesNotExist,GiveName()));
            IsGoodFlag=0;
        }
    }

    return rc;
}

int tni34_AFile::Read(void *buf, long BytesToRead, long & BytesRead)
{
    int rc=1;

    BytesRead=0;

    while(0<BytesToRead && 1==rc)
    {
        #if defined (WIN32)
        DWORD dwBytesReadNow;

        if(ReadFile(HandleForFile, buf, DWORD(BytesToRead), &dwBytesReadNow, 0))
        {
            if(0==dwBytesReadNow)
                rc=-1;
            else
            {
                BytesRead+=long(dwBytesReadNow);
                BytesToRead-=long(dwBytesReadNow);
            }
        }
        else
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotReadInFile, GiveName()));
            IsGoodFlag=rc=0;
        }

        #elif defined (UNIX)
        ssize_t RCOf_read=read(HandleForFile, buf, size_t(BytesToRead));
    
        if(-1==RCOf_read)
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotReadInFile, GiveName()));
            IsGoodFlag=rc=0;
        }
        else
            if(0==RCOf_read)
                rc=-1;
            else
            {
                BytesRead+=long(RCOf_read); // rc=1; already done
                BytesToRead-=long(RCOf_read);
            }

        #else
            #error tni34_AFile::Read() only coded for WIN32 and UNIX (Define one of them.)
        #endif
    }

    return rc;
}

int tni34_AFile::Write(const void *buf, long BytesToWrite, long &BytesWritten)
{
    int rc=1;

    #if defined (WIN32)
    DWORD dwBytesWritten;
    int   RCOfWriteFile=WriteFile(HandleForFile, buf, DWORD(BytesToWrite), &dwBytesWritten, 0);

    if(!RCOfWriteFile || BytesToWrite!=long(dwBytesWritten))
    {
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotWriteInFile,GiveName()));
        IsGoodFlag=rc=0;
        BytesWritten=0;
    }
    else
    {
        BytesWritten=long(dwBytesWritten);

        if(0==dwBytesWritten)
            rc=-1;
    }

    #elif defined (UNIX)
    ssize_t RCOf_write=write(HandleForFile, buf, size_t(BytesToWrite));

    if(-1==RCOf_write || RCOf_write!=size_t(BytesToWrite))
    {
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotWriteInFile,GiveName()));
        IsGoodFlag=rc=0;
        BytesWritten=0;
    }
    else
    {
        BytesWritten=long(RCOf_write);

        if(0==RCOf_write)
            rc=-1;
    }
    #else
        #error tni34_AFile::Write() only coded for WIN32 and UNIX (Define one of them.)
    #endif

    return rc;
}

int tni34_AFile::flush() {

    int rc = 1;

    if( HandleForFile != InvalidFileHandle_ni34 ) {
        #if defined (WIN32)
        if( !FlushFileBuffers(HandleForFile) ) {
            rc = 0;
        }
        #elif defined (UNIX)
        if( -1 != fsync(HandleForFile) ) {
            rc = 0;
        }
        #else
            #error tni34_AFile::flush() only coded for WIN32 and UNIX (Define one of them.)
        #endif

        if( 0 == rc )
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotFlushFile,GiveName()));
    }
    return (IsGoodFlag=rc);
}

int tni34_AFile::Close()
{
    int rc=PureQuietClose();

    if(!rc)
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCloseFile,GiveName()));

    return rc;
}

int tni34_AFile::Remove()
{
    int rc=Close();
        
    if(rc)
    {
        if(remove(GiveName()))
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotRemoveFile, GiveName()));
            IsGoodFlag=rc=0;
        }
    }

    return rc;
}

int tni34_AFile::Rename(const char * NameToMoveTo)
{
    int rc=Close();

    if(rc)
    {
        if(0!=rename(GiveName(), NameToMoveTo))
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotMoveFile, GiveName(), NameToMoveTo));
            IsGoodFlag=rc=0;
        }
    }

    return rc;
}

int tni34_AFile::PureQuietCreate(const char * name)
{
    int rc=1;

    /*
    tsp05_RteFileError FError;
    sqlfopenc(name, SP5VF_BINARY, SP5VF_WRITE, SP5BK_UNBUFFERED, &HandleForFile, &FError);

    if(vf_ok==FError.sp5fe_result)
    {
        sqlfclosec(HandleForFile, SP5VF_CLOSE_NORMAL, &FError);

        if(vf_ok==FError.sp5fe_result)
        {
            HandleForFile=InvalidFileHandle_ni34;
            HandleIsValid=0;
        }
        else
            rc=0;
    }
    else
        rc=0;
    */

    #if defined (WIN32)
    HandleForFile=CreateFile(name,
                             GENERIC_READ|GENERIC_WRITE,
                             getShareMode(),
                             0,
                             CREATE_NEW,
                             FILE_ATTRIBUTE_NORMAL,
                             0);
    #elif defined (UNIX)
    mode_t OldFileCreationMask=umask(S_IROTH|S_IWOTH);  // dont let the file creation mask be in our way

    HandleForFile=open(name, O_RDWR|O_CREAT|O_EXCL, S_IFREG|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);

    umask(OldFileCreationMask); // try to leave the file creation mask unchanged
    #else
        #error tni34_AFile::PureQuitCreate() only coded for WIN32 and UNIX (define one of them).
    #endif
        
    if(InvalidFileHandle_ni34==HandleForFile)
        rc=0;
    else
    {
        #if defined (WIN32)
        if(!CloseHandle(HandleForFile))
	    #elif defined (UNIX)
        if(-1==close(HandleForFile))
        #else
            #error tni34_AFile::PureQuitCreate() only coded for WIN32 and UNIX (define one of them).
        #endif
            rc=0;
        else
            HandleForFile=InvalidFileHandle_ni34;
    }

    return (IsGoodFlag=rc);
}

int tni34_AFile::PureCreate()
{
    int rc=PureQuietCreate(GiveName());

    if(!rc)
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateFile, GiveName()));

    return rc;
}

#if defined (WIN32)
DWORD tni34_AFile::getShareMode() {
    return 0;
}
#endif

int tni34_AFile::PureOpen()
{
    int rc=Close();

    if(rc)
    {   // InvalidFileHandle_ni34==HandleForFile and IsGoodFlag==1 holds, because of  the Close()-call above!
        #if defined (WIN32)

        switch(DirectionOfFile)
        {
            case FromDisk:
                HandleForFile=CreateFile(GiveName(),
                                         GENERIC_READ,
                                         getShareMode(),
                                         0,
                                         OPEN_EXISTING,
                                         FILE_ATTRIBUTE_NORMAL,
                                         0);
                break;
            case ToDiskTruncate:
                HandleForFile=CreateFile(GiveName(),
                                         GENERIC_WRITE,
                                         getShareMode(),
                                         0,
                                         TRUNCATE_EXISTING,
                                         FILE_ATTRIBUTE_NORMAL,
                                         0);
                break;
            case ToDiskAppend:
                HandleForFile=CreateFile(GiveName(),
                                         GENERIC_WRITE,
                                         getShareMode(),
                                         0,
                                         OPEN_EXISTING,
                                         FILE_ATTRIBUTE_NORMAL,
                                         0);
                break;
            default:
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnknownFileType));
        }

        if(InvalidFileHandle_ni34==HandleForFile)
            rc=0;
        else
        {
            if(ToDiskAppend==DirectionOfFile)
            {
                if(0xFFFFFFFF==SetFilePointer(HandleForFile, 0, 0, FILE_END))
                {
                    ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotSetPositionInFile));
                    Close();
                    rc=0;
                }
            }
        }

        #elif defined (UNIX)
        int oflag;

        switch(DirectionOfFile)
        {
            case ToDiskTruncate:
                oflag=O_WRONLY|O_TRUNC;
                break;
            case ToDiskAppend:
                oflag=O_WRONLY|O_APPEND;
                break;
            case FromDisk:
                oflag=O_RDONLY;
                break;
            default:
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnknownFileType));
                rc=0;
        }

        if(rc)
        {
            HandleForFile=open(GiveName(), oflag);

            if(InvalidFileHandle_ni34==HandleForFile)
                rc=0;
        }
        #else
            #error tni34_AFile::Open() only coded for WIN32 and UNIX (Define one of them.)
        #endif

        if(0==rc)
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotOpenFile,GiveName()));
            IsGoodFlag=0;
        }
        else
            IsOpenFlag=1;
    }

    return rc;
}

int tni34_AFile::PureQuietClose()
{
    int rc=1;

    if(HandleForFile!=InvalidFileHandle_ni34)
    {
        #if defined (WIN32)
        if(DirectionOfFile==ToDiskTruncate || DirectionOfFile==ToDiskAppend)
            if(!FlushFileBuffers(HandleForFile))
                rc=0;

        if(!CloseHandle(HandleForFile))
        #elif defined (UNIX)
        if(close(HandleForFile))
        #else
            #error  tni34_AFile::Close() only coded for WIN32 and UNIX (Define one of them.)
        #endif
            rc=0;
        else
        {
            HandleForFile=InvalidFileHandle_ni34;
            IsOpenFlag=0;
        }
    }

    return (IsGoodFlag=rc);
}


// -----------------------------------------------------------------------------
// member functions of class tni34_AVirtualFile
// -----------------------------------------------------------------------------

tni34_AVirtualFile::tni34_AVirtualFile(const char *name,
                                       ADirection Dir,
                                       AOpenType OpenFlag)
    :tni34_ADataEntry(name),
    DirectionOfFile(Dir),
    HandleForFile(tsp00_Int4(0)),
    HandleIsValid(0)
{
    if(OpenNow==OpenFlag)
        Open();
}

tni34_AVirtualFile::~tni34_AVirtualFile()
{
    if(HandleIsValid)
        Close();
}

int tni34_AVirtualFile::Exists()
{
    int rc;

    tsp05_RteFileInfo  Info;
    tsp05_RteFileError Error;
    
    sqlfinfoc(GiveName(), &Info, &Error);

    if(Info.sp5fi_exists)
        rc=1;
    else
        IsGoodFlag=rc=0;

    return rc;
}

int tni34_AVirtualFile::IsAFile()
{
    int rc;

    tsp05_RteFileInfo  Info;
    tsp05_RteFileError Error;
    
    sqlfinfoc(GiveName(), &Info, &Error);

    if(Info.sp5fi_exists && vf_t_file==Info.sp5fi_media_kind)
        rc=1;
    else
        IsGoodFlag=rc=0;

    return rc;
}

int tni34_AVirtualFile::Check()
{
    return 0;
}

int tni34_AVirtualFile::Create()
{
    return 0;
}

int tni34_AVirtualFile::Open()
{
    int rc=1;

    if(!HandleIsValid)
    {
        tsp05_RteFileMode    FileMode;

        switch(DirectionOfFile)
        {
            case FromDisk:
                FileMode.becomes (sp5vf_read);
                break;
            case ToDiskTruncate:
                FileMode.becomes (sp5vf_write);
                break;
            case ToDiskAppend:
                FileMode.becomes (sp5vf_append);
                break;
            default:
            {
                rc=0;
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnknownFileType));
            }
        }

        if(rc)
        {
            tsp05_RteFileError FError;

            sqlfopenc(GiveName(), sp5vf_binary, FileMode, sp5bk_unbuffered, &HandleForFile, &FError);

            if(vf_ok!=FError.sp5fe_result)
            {
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotOpenVirtualFile,
                                                               GiveName(),
                                                               ErrorMsgToCString(FError.sp5fe_text)));
                rc=0;
            }
        }

        if(rc)
        {
            IsOpenFlag=1;
            HandleIsValid=1;
        }
        else
        {
            IsGoodFlag=0;
        }
    }

    return rc;
}

int tni34_AVirtualFile::Read(void *buf, long BytesToRead, long & BytesRead)
{
    int rc=1;

    if(HandleIsValid)
    {
        tsp00_Longint tspBytesReadNow;
        tsp05_RteFileError Error;

        BytesRead=0;
        sqlfreadc(HandleForFile, buf, tsp00_Longint(BytesToRead), &tspBytesReadNow, &Error);

        switch(Error.sp5fe_result)
        {
            case vf_ok:
                BytesRead=long(tspBytesReadNow);
                break;
            case vf_eof:
                if(0==tspBytesReadNow)
                    rc=-1;                              //0==BytesRead
                else
                    BytesRead=long(tspBytesReadNow);    //rc=1, ignore vf_eof if we could read more than 0 bytes 
                break;
            default:
            {
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotReadInVirtualFile,
                                                               GiveName(),
                                                               ErrorMsgToCString(Error.sp5fe_text)));
                IsGoodFlag=rc=0;
            }
        }
    }
    else
    {
        IsGoodFlag=rc=0;
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnreadableVirtualFile,
                                                       GiveName()));
    } 

    return rc;
}

int tni34_AVirtualFile::Write(const void *buf, long BytesToWrite, long & BytesWritten)
{
    int rc=1;

    if(HandleIsValid)
    {
        tsp05_RteFileError Error;

        sqlfwritec(HandleForFile, buf, tsp00_Longint(BytesToWrite), &Error);

        if(vf_ok==Error.sp5fe_result)
            BytesWritten=BytesToWrite;
        else
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotWriteInVirtualFile,
                                                           GiveName(),
                                                           ErrorMsgToCString(Error.sp5fe_text)));
            IsGoodFlag=rc=0;
            BytesWritten=0;
        }
    }
    else
    {
        IsGoodFlag=rc=0;
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnwritableVirtualFile,
                                                       GiveName()));
    }            

    return rc;
}

int tni34_AVirtualFile::Close()
{
    int rc=1;

    if(HandleIsValid)
    {
        tsp05_RteFileError FError;

        sqlfclosec(HandleForFile, sp5vf_close_normal, &FError);

        if(vf_ok==FError.sp5fe_result)
        {
            HandleIsValid=0;
            IsOpenFlag=0;
        }
        else
        {
            rc=0;
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCloseVirtualFile,
                                                           GiveName(),
                                                           ErrorMsgToCString(FError.sp5fe_text)));
        }
    }

    return rc;
}

int tni34_AVirtualFile::Remove()
{
    int rc=Close();
        
    if(rc)
    {
        tsp05_RteFileError FError;

        sqlferasec(GiveName(), &FError);

        if(vf_ok!=FError.sp5fe_result)
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotRemoveVirtualFile,
                                      GiveName(),
                                      ErrorMsgToCString(FError.sp5fe_text)));
            IsGoodFlag=rc=0;
        }
    }

    return rc;
}

int tni34_AVirtualFile::GetSize(teo00_Uint8 & FileSize)
{
    char Date[9];
    char Time[7];

    return GetDateTimeSize(Date, Time, FileSize);
}
int tni34_AVirtualFile::GetDateTime(char Date[9], char Time[7])
{
    teo00_Uint8 FileSize;

    return GetDateTimeSize(Date, Time, FileSize);
}

int tni34_AVirtualFile::GetDateTimeSize(char Date[9], char Time[7], teo00_Uint8 & FileSize)
{
    int rc=1;

    tsp05_RteFileInfo  Info;
    tsp05_RteFileError Error;
    
    sqlfinfoc(GiveName(), &Info, &Error);

    if(vf_ok==Error.sp5fe_result &&
       Info.sp5fi_exists &&
       vf_t_file==Info.sp5fi_media_kind)
    {
        /* old code from 6.2 and 7.1
        if(0!=Info.sp5fi_size_x)
        {
            rc=0;
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotGetSizeOfVirtualFile,
                                                           GiveName(),
                                                           "File is to large"));
        }
        else
            FileSize=teo00_Uint8(Info.sp5fi_size);
        */
        
        FileSize=Info.sp5fi_size64; // new code starting with 7.2
        
        strncpy(Date, Info.sp5fi_date_modified, 8);
        Date[8]='\0';
        strncpy(Time, Info.sp5fi_time_modified, 6);
        Time[6]='\0';
    }
    else
    {
        rc=0;

        const char * Explanation=0;

        if(vf_ok!=Error.sp5fe_result)
            Explanation=ErrorMsgToCString(Error.sp5fe_text);
        else
            if(!Info.sp5fi_exists)
                Explanation="It does not exist.";
            else
                if(vf_t_file!=Info.sp5fi_media_kind)
                    Explanation="It is no file.";
                    
        if(0!=Explanation)
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotGetSizeOfVirtualFile,
                                                           GiveName(),
                                                           Explanation));
        else
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseInternalError));
    }

    return rc;
}


// -----------------------------------------------------------------------------
// member functions of class tni34_ATextFile
// -----------------------------------------------------------------------------

static const char szOSLineDelimiterNT_ni34[3]={13, 10, '\0'};
static const char szOSLineDelimiterUNIX_ni34[2]={'\n','\0'};

#if defined (WIN32)
static const char  * szOSLineDelimiter_ni34=szOSLineDelimiterNT_ni34;
#elif defined (UNIX)
static const char * szOSLineDelimiter_ni34=szOSLineDelimiterUNIX_ni34;
#else
    #error vni34.cpp was only coded for WIN32 and UNIX (define one of them).
#endif

static const size_t MaxOSLineDelimiterLength=strlen(szOSLineDelimiterNT_ni34);

tni34_ATextFile::tni34_ATextFile(ADirection Dir,
                                 int CreateOnOpen,
                                 int OpenNow,
                                 int CreateOnCheck,
                                 unsigned int EstimatedLineSize,
                                 int PrefBufSize,
                                 int MaxNumberOfRetries)
  :tni34_AFile(Dir, CreateOnOpen, OpenNow, CreateOnCheck, PrefBufSize, MaxNumberOfRetries),
   ReadBuffer(0),
   SizeOfReadBuffer(0),
   PositionInReadBuffer(0),
   EndInReadBuffer(0),
   EndOfFileReached(0),
   AllLinesRead(0)
{
    ReAllocReadBufferTo(EstimatedLineSize+sizeof(szOSLineDelimiter_ni34)); //space for: line+delimiter+'\0' !
}

tni34_ATextFile::tni34_ATextFile(const char *name,
                                 ADirection Dir,
                                 int CreateOnOpen,
                                 int OpenNow,
                                 int CreateOnCheck,
                                 unsigned int EstimatedLineSize,
                                 int PrefBufSize)
  :tni34_AFile(name, Dir, CreateOnOpen, OpenNow, CreateOnCheck, PrefBufSize),
   ReadBuffer(0),
   SizeOfReadBuffer(0),
   PositionInReadBuffer(0),
   EndInReadBuffer(0),
   EndOfFileReached(0),
   AllLinesRead(0)
{
    ReAllocReadBufferTo(EstimatedLineSize+sizeof(szOSLineDelimiter_ni34)); //space for: line+delimiter+'\0' !
}

tni34_ATextFile::~tni34_ATextFile()
{
    if(0!=ReadBuffer)
        delete [] ReadBuffer;
}

int tni34_ATextFile::Open()
{
    int rc=tni34_AFile::Open();

    PositionInReadBuffer=0;
    EndInReadBuffer=0;
    EndOfFileReached=0;
    AllLinesRead=0;

    if(rc)
    {
        if(0!=ReadBuffer)
            ReadBuffer[0]='\0';
        else
            rc=ReAllocReadBufferTo(1024+sizeof(szOSLineDelimiter_ni34));
    }

    return rc;
}

int tni34_ATextFile::ReadLine(const char * & buf)
{
    int rc=1;

    char * DeLim;
    const char * FoundDelimiter;
    int EndOfLineFound=0;

    while(rc && !EndOfLineFound && !AllLinesRead)
    {
        EndOfLineFound=FoundEndOfLineForSure(DeLim, FoundDelimiter);

        if(!EndOfLineFound)
        {
            if(EndOfFileReached)
                AllLinesRead=1; // if we get here -> we have read all from the file and all from the buffer (ReadBuffer[CurrentPositionInReadBuffer]=='\0')
            else                // line is not completly in ReadBuffer and something could be read from the file -> try to get something from the file
            {
                //move info to start of buffer
                MoveStringToStartOfBuffer(); // does nothing if 0==CurrentPositionInReadBuffer

                //fill the buffer
                size_t BytesToReadAsSizeT=SizeOfReadBuffer-EndInReadBuffer-sizeof(szOSLineDelimiter_ni34);  // left space for delimiter+'\0' !

                if(0>=BytesToReadAsSizeT)   //no space in buffer left
                {
                    rc=ReAllocReadBufferTo(2*SizeOfReadBuffer);    //double the space
                    BytesToReadAsSizeT=SizeOfReadBuffer-EndInReadBuffer-sizeof(szOSLineDelimiter_ni34); // left space for delimiter+'\0' !
                }

                if(rc)
                {
                    if(BytesToReadAsSizeT>LONG_MAX)
                    {
                        rc=0;
                        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseTextLineTooLong, GiveName()));
                    }
                    else
                    {
                        long BytesToRead=long(BytesToReadAsSizeT);
                        long BytesReadNow;

                        if(!Read(ReadBuffer+EndInReadBuffer, BytesToRead, BytesReadNow))
                            rc=0;
                        else
                        {
                            long i;

                            for(i=0; i<BytesReadNow; i++)
                            {
                                if('\0'==ReadBuffer[EndInReadBuffer+i]) //if the newly read bytes contain a 0-byte
                                    ReadBuffer[EndInReadBuffer+i]=' ';  //replace it with a space (otherwise with current usage of C-string-functions the file would "end" on every 0-byte)
                            }

                            EndInReadBuffer+=BytesReadNow;
                            ReadBuffer[EndInReadBuffer]='\0';
                
                            if(BytesReadNow<BytesToRead)    //if we have found the end of the file
                            {
                                EndOfFileReached=1;
                                strcpy(ReadBuffer+EndInReadBuffer, szOSLineDelimiter_ni34); //be sure we have a line delimiter at the end
                                EndInReadBuffer+=strlen(szOSLineDelimiter_ni34);
                            }
                        }
                    }
                }
            }
        }
    }        
    
    if(rc && !AllLinesRead)
    {
        DeLim[0]='\0';
        buf=ReadBuffer+PositionInReadBuffer;
        PositionInReadBuffer=DeLim-ReadBuffer+strlen(FoundDelimiter);   //the next line starts behind the found delimiter
    }
    else
        buf=0;

    return rc;
}
    
int tni34_ATextFile::WriteLine(const char * buf, long & BytesWritten)
{
    int rc=1;

    BytesWritten=0;

    long   BytesWrittenLine;
    size_t BytesToWriteAsSizeT=strlen(buf);
    long   BytesToWrite;

    if(BytesToWriteAsSizeT>LONG_MAX)
    {
        rc=0;
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseTextLineTooLong));
    }
    else
    {
        BytesToWrite=long(BytesToWriteAsSizeT);    

        if(!Write(buf, BytesToWrite, BytesWrittenLine))
            rc=0;
    
        BytesWritten+=BytesWrittenLine;
    }

    if(rc)
    {
        long BytesWrittenDelimiter;
        BytesToWrite=long(strlen(szOSLineDelimiter_ni34));

        if(!Write(szOSLineDelimiter_ni34, BytesToWrite, BytesWrittenDelimiter))
            rc=0;

        BytesWritten+=BytesWrittenDelimiter;
    }

    return rc;
}

int tni34_ATextFile::WriteLine(const char * buf)
{
    long BytesWritten;

    return WriteLine(buf, BytesWritten);
}

int tni34_ATextFile::ReAllocReadBufferTo(size_t NewSize)
{
    int rc=1;

    if(SizeOfReadBuffer<NewSize)
    {
        char * h=new char[NewSize+1];

        if(0==h)
        {
            ErrorHandler_ni35->Report(tni35_AStandardError::OutOfMemory.Set());
            IsGoodFlag=rc=0;
        }
        else
        {
            if(0!=ReadBuffer)
            {
                strcpy(h, ReadBuffer+PositionInReadBuffer); //just copy what is realy needed
                delete [] ReadBuffer;
            }
            else
                h[0]='\0';

            ReadBuffer=h;
            SizeOfReadBuffer=NewSize;

            EndInReadBuffer-=PositionInReadBuffer;  //adjust the variables describing the string in the buffer
            PositionInReadBuffer=0;
        }
    }

    return rc;
}

void tni34_ATextFile::MoveStringToStartOfBuffer()
{
    if(PositionInReadBuffer>0)
    {
        size_t h=PositionInReadBuffer,  // step size (size of one memcpy step)
               s=PositionInReadBuffer,  // current source
               d=0;                     // current destination

        while(s!=EndInReadBuffer)
        {
            if(s+h>EndInReadBuffer) // next step would copy from behind '\0'
                h=EndInReadBuffer-s;
                
            memcpy((void *)(ReadBuffer+d), (void *)(ReadBuffer+s), h);
            
            d+=h;
            s+=h;
        }

        ReadBuffer[d]='\0'; //"copy" the trailing '\0'

        EndInReadBuffer-=PositionInReadBuffer;  //adjust the variables describing the string in the buffer
        PositionInReadBuffer=0;
    }
}

int tni34_ATextFile::FoundEndOfLineForSure(char * & EndOfLine, const char * &FoundDelimiter)
{
    char * NTDel=strstr(ReadBuffer+PositionInReadBuffer, szOSLineDelimiterNT_ni34);
    char * UNIXDel=strstr(ReadBuffer+PositionInReadBuffer, szOSLineDelimiterUNIX_ni34);

    int FoundWhat=0;    // 0==no delimiter, 1==NT delimiter, 2==UNIX delimiter

    if(0!=NTDel && 0!=UNIXDel)
    {
        if((NTDel-ReadBuffer)<=(UNIXDel-ReadBuffer))
            FoundWhat=1;
        else
            FoundWhat=2;
    }
    else
        if(EndOfFileReached)
        {
            if(0!=NTDel)
                FoundWhat=1;
            else
                if(0!=UNIXDel)
                    FoundWhat=2;
        }
        else
        {
            if(0!=NTDel && (NTDel-ReadBuffer)+MaxOSLineDelimiterLength<=EndInReadBuffer)
                FoundWhat=1;
            else
                if(0!=UNIXDel && (UNIXDel-ReadBuffer)+MaxOSLineDelimiterLength<=EndInReadBuffer)
                    FoundWhat=2;
        }
    
    switch(FoundWhat)
    {
        case 1:
        {
            EndOfLine=NTDel;
            FoundDelimiter=szOSLineDelimiterNT_ni34;
        }break;

        case 2:
        {
            EndOfLine=UNIXDel;
            FoundDelimiter=szOSLineDelimiterUNIX_ni34;
        }break;

        default:
        {
            EndOfLine=0;
            FoundDelimiter=0;
        }break;
    }

    return (0!=EndOfLine);
}

// -----------------------------------------------------------------------------
// member functions of class tni34_APipe
// -----------------------------------------------------------------------------

tni34_APipe::tni34_APipe(ADirection Dir,
                         int CreateOnOpen,
                         int OpenNow,
                         int PrefBufSize,
                         int MaxNumberOfRetries)
  : tni34_ADataEntry(0, CreateOnOpen, 0), // (no name, create on open, do not create on check)
    DirectionOfPipe(Dir),
    PrefferedBufferSize(PrefBufSize),
    HandleForPipe(InvalidPipeHandle_ni34),
    Created(0)
{
    int i=0,
        TempNameOK=1;

    tni34_ADirectory *TempPipeDir=0;
    char *TempName=0;

    #if defined (WIN32)
    tni34_ADirectory NTPipeDir("\\\\.\\pipe\\", 0, 0);
    TempPipeDir=&NTPipeDir;
    #elif defined (UNIX)
    #else
        #error tni34_APipe::tni34_APipe() was only coded for UNIX and WIN32 (define one of them)
    #endif

    do
    {
        if(CreateTemporaryName(TempName, TempPipeDir, i++)) //get a temporary name, in TempPipeDir and handle the repeated flag through i
            PureQuietCreate(TempName);  //try to create the pipe
        else
            TempNameOK=0;
    }
    while(TempNameOK && !Created && i<MaxNumberOfRetries);  // i<MaxNumberOfRetries -> MaxNumberOfRetries processes can try to create a temporary file in the same second

    if(Created)
    {
        if(ResetNameTo(TempName))
        {
            if(OpenNow)
                if(!PureOpen()) // pipe exists no further check is necessary
                {
                    Remove();
                    IsGoodFlag=0;
                }
        }
        else
        {
            PureQuietClose();
            IsGoodFlag=0; 
        }
    }
    else
    {
        IsGoodFlag=0;
        
        if(TempNameOK)
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreateFile, TempName));

        ResetNameTo("");    //try to simulate a name
    }

    if(0!=TempName)
        delete [] TempName;
}

tni34_APipe::tni34_APipe(const char *name,
                         ADirection Dir,
                         int CreateOnOpen,
                         int OpenNow,
                         int PrefBufSize)
    :tni34_ADataEntry(name, CreateOnOpen),
    DirectionOfPipe(Dir),
    PrefferedBufferSize(PrefBufSize),
    HandleForPipe(InvalidPipeHandle_ni34),
    Created(0)
{
    if(OpenNow)
        Open();
}

tni34_APipe::~tni34_APipe()
{
    if(Created)
        Remove(); //cannot use the return code of Remove() in a destructor
}

int tni34_APipe::IsAPipe()
{
    int rc;

    #if defined (WIN32)

    //struct _stat s_stat;
    //if(-1!=_stat(GiveName(),&s_stat) && 0!=(s_stat.st_mode & _S_IFIFO))

    const char *Name=GiveName();
    size_t l=strlen(Name);

    if(l<10 || Name[0]!='\\' || Name[1]!='\\')              // at least "\\.\pipe\x" -> 10 chars, starting with "\\"
        Name=0;
    else
    {
        const char * KeyWordPipe=strchr(Name+2, '\\');
        l-=KeyWordPipe-Name;

        if(l<7 ||                                       // at least "\pipe\x" -> 7 chars, starting with "\pipe\"
            'P'!=toupper(KeyWordPipe[1]) ||
            'I'!=toupper(KeyWordPipe[2]) ||
            'P'!=toupper(KeyWordPipe[3]) ||
            'E'!=toupper(KeyWordPipe[4]) ||
            '\\'!=KeyWordPipe[5])
            Name=0;
    }

    if(0!=Name && Exists())
    #elif defined (UNIX)
    struct stat s_stat;
    if(-1!=stat(GiveName(),&s_stat) && 0!=(s_stat.st_mode & S_IFIFO))
    #else
        #error tni34_APipe::IsAPipe() was only coded for WIN32 and UNIX, define one of them.
    #endif
        rc=1;
    else
        IsGoodFlag=rc=0;

    /*
    tsp05_RteFileInfo  Info;
    tsp05_RteFileError Error;
    
    sqlfinfoc(GiveName(), &Info, &Error);

    if(Info.sp5fi_exists && vf_t_pipe==Info.sp5fi_media_kind)
        rc=1;
    else
        IsGoodFlag=rc=0;
    */

    return rc;
}

int tni34_APipe::Exists()
{
    int rc=1;

    #if defined (WIN32)
    if(!WaitNamedPipe(GiveName(), 1))
        if(ERROR_SEM_TIMEOUT!=GetLastError()) // if no instance exists ERROR_FILE_NOT_FOUND should be reported by GetLastError()
            IsGoodFlag=rc=0;

    //struct _stat s_stat;
    //if(-1==_stat(Name,&s_stat))
    //    IsGoodFlag=rc=0;

    #elif defined (UNIX)
    struct stat s_stat;
    if(-1==stat(GiveName(), &s_stat))
        IsGoodFlag=rc=0;
    #else
        #error tni34_APipe::Exists() only coded for WIN32 and UNIX (Define one of them.)
    #endif

    /*
    tsp05_RteFileInfo  Info;
    tsp05_RteFileError Error;
    
    sqlfinfoc(GiveName(), &Info, &Error);

    if(!Info.sp5fi_exists)
        IsGoodFlag=rc=0;
    */

    return rc;
}

int tni34_APipe::Check()
{
    int rc;

    if(Exists())
    {
        rc=IsAPipe();

        if(0==rc)
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseExistsAndIsNoPipe, GiveName()));
    }
    else
        if(CreateOnCheckFlag)
            rc=Create();
        else
        {
            IsGoodFlag=rc=0;
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CasePipeDoesNotExist, GiveName()));
        }

   return rc;
}

int tni34_APipe::Create()
{
    int rc;
    
    if(InvalidPipeHandle_ni34==HandleForPipe)
        rc=PureCreate();
    else
        rc=0;

    return rc;
}

int tni34_APipe::Open()
{
    int rc=0;

    if(Created)         //if pipe was already created by this object -> open it now
        rc=PureOpen();
    else
    {
        if(Exists())
        {
            if(IsAPipe())
                rc=PureOpen();
            else
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseExistsAndIsNoPipe, GiveName()));
        }
        else
        {
            if(CreateOnOpenFlag)
            {
                if(PureQuietCreate(GiveName())) //try to create the pipe
                    rc=PureOpen();
                else
                    if(Exists()) // maybe an other process just openend the pipe as owner
                    {
                        if(IsAPipe())
                            rc=PureOpen();
                        else
                        {
                            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseExistsAndIsNoPipe, GiveName()));
                            IsGoodFlag=0;
                        }
                    }
                    else
                    {
                        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreatePipe, GiveName()));
                        IsGoodFlag=0;
                    }
            }
            else
            {
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CasePipeDoesNotExist, GiveName()));
                IsGoodFlag=0;
            }
        }
    }

    return rc;
}

int tni34_APipe::Read(void *buf, long BytesToRead, long & BytesRead)
{
    int rc=1;

    /*
    tsp00_Longint tspBytesReadNow;
    tsp05_RteFileError Error;

    BytesRead=0;
    sqlfreadc(HandleForPipe, buf, tsp00_Longint(BytesToRead), &tspBytesReadNow, &Error);

    switch(Error.sp5fe_result)
    {
        case vf_ok:
            BytesRead=long(tspBytesReadNow);

            if(tspBytesReadNow!=BytesToRead)
                rc=-1;
            break;
        case vf_eof:
            BytesRead=0;
            rc=-1;
            break;
        default:
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotReadInPipe, GiveName()));
            IsGoodFlag=rc=0;
        }
    }
    */

    BytesRead=0;

    while(BytesToRead>0 && 1==rc)
    {
        #if defined (WIN32)
        DWORD dwBytesReadNow;
        
        if(ReadFile(HandleForPipe, (char *)buf+BytesRead, DWORD(BytesToRead), &dwBytesReadNow, 0))
        {
            if(0==dwBytesReadNow)
                rc=-1;
            else
            {
                BytesRead+=long(dwBytesReadNow);
                BytesToRead-=long(dwBytesReadNow);
            }
        }
        else
        {
            DWORD LastError=GetLastError();

            if(ERROR_BROKEN_PIPE==LastError || ERROR_PIPE_NOT_CONNECTED==LastError)
                rc=-1;
            else
            {
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotReadInPipe,GiveName()));
                IsGoodFlag=rc=0;
            }
        }

        #elif defined (UNIX)
        ssize_t RCOf_read=read(HandleForPipe, (char *)buf+BytesRead, size_t(BytesToRead));

        if(-1==RCOf_read)
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotReadInPipe,GiveName()));
            IsGoodFlag=rc=0;
        }
        else
            if(0==RCOf_read)
                rc=-1;
            else
            {
                BytesRead+=long(RCOf_read);
                BytesToRead-=long(RCOf_read);
            }
        #else
            #error tni34_APipe::Read() only coded for WIN32 and UNIX (Define one of them.)
        #endif
    }

    return rc;
}

int tni34_APipe::Write(const void *buf, long BytesToWrite, long &BytesWritten)
{
    int rc=1;

    /*
    tsp05_RteFileError Error;

    sqlfwritec(HandleForPipe, buf, tsp00_Longint(BytesToWrite), &Error);

    if(vf_ok==Error.sp5fe_result)
        BytesWritten=BytesToWrite;
    else
    {
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotWriteInPipe, GiveName()));
        IsGoodFlag=rc=0;
        BytesWritten=0;
    }
    */

    #if defined (WIN32)
    DWORD dwBytesWritten;
    int RCOfWriteFile=WriteFile(HandleForPipe,buf,DWORD(BytesToWrite),&dwBytesWritten,0);
    
    if(!RCOfWriteFile)
    {
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotWriteInPipe,GiveName()));
        IsGoodFlag=rc=0;
        BytesWritten=long(0);
    }
    else
    {
        BytesWritten=long(dwBytesWritten);

        if(0==dwBytesWritten)
            rc=-1;
    }

    #elif defined (UNIX)
    ssize_t RCOf_write=write(HandleForPipe,buf,size_t(BytesToWrite));
    
    if(-1==RCOf_write)
    {
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotWriteInPipe,GiveName()));
        IsGoodFlag=rc=0;
        BytesWritten=0;
    }
    else
    {
        BytesWritten=long(RCOf_write);

        if(0==RCOf_write)
            rc=-1;
    }
    #else
        #error tni34_APipe::Write() only coded for WIN32 and UNIX (Define one of them.)
    #endif

    return rc;
}

int tni34_APipe::IsDataAvailable()
{
    int rc=0;

    #if defined (WIN32)
    DWORD Available;
    if(!PeekNamedPipe(HandleForPipe, NULL, 0, NULL, &Available, NULL))
    {
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotReadInPipe,GiveName()));
        IsGoodFlag=rc=0;
    }
    else
    {
        if(0<Available)
            rc=1;
    }
    #elif defined (UNIX)
    struct pollfd fds;

    fds.fd=HandleForPipe;
    fds.events=POLLIN;
        
    int rcOfPoll=poll(&fds, 1, 0);

    if(-1==rcOfPoll)
    {
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotReadInPipe,GiveName()));
        IsGoodFlag=rc=0;
    }
    else
    {
        if(0<rcOfPoll && (fds.revents & POLLIN))
            rc=1;
    }
    #else
        #error tni34_APipe::IsDataAvailable() was only coded for WIN32 and UNIX, define one of them.
    #endif

    return rc;
}

int tni34_APipe::Close()
{
    int rc=PureQuietClose();

    if(!rc)
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotClosePipe,GiveName()));

    return rc;
}

int tni34_APipe::BreakAndRemovePipeIfOpened(int & PipeWasRemoved)
{
    int rc=Close(),
        PipeIsOpened=1;

    PipeWasRemoved=0;

    #if defined (WIN32)
    if(!WaitNamedPipe(GiveName(), 1))
    {
        DWORD err=GetLastError();

        if(ERROR_SEM_TIMEOUT!=err) // ERROR_SEM_TIMEOUT is set if a pipe exists but no ConnectNamedPipe() is done by the pipe server
            PipeIsOpened=0;        // if no instance exists ERROR_FILE_NOT_FOUND should be reported by GetLastError()
    }

    if(PipeIsOpened)
    {
        switch(DirectionOfPipe)
        {
            case In:
                HandleForPipe=CreateFile(GiveName(), GENERIC_READ, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
                break;
            case Out:
                HandleForPipe=CreateFile(GiveName(), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
                break;
            default:
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnknownPipeType));
        }

        if(HandleForPipe==InvalidPipeHandle_ni34)
        {
            IsGoodFlag=rc=0;
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotConnectToPipe,GiveName())); 
        }
        else
            IsOpenFlag=1;

        if(!CloseHandle(HandleForPipe))
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotClosePipe, GiveName()));
            rc=0;
        }
        else
        {
            HandleForPipe=InvalidPipeHandle_ni34;
        }
    }
    //else: nobody has created the pipe -> nothing to break

    #elif defined (UNIX)
    errno=0;

    switch(DirectionOfPipe)
    {
        case In:
            HandleForPipe=open(GiveName(), O_RDONLY | O_NONBLOCK);
        break;

        case Out:
        {
            int SecondHandleForPipe=open(GiveName(), O_RDONLY | O_NONBLOCK);  /* open the pipe for reading first, to asure the open for writing goes through */

            HandleForPipe=open(GiveName(), O_WRONLY | O_NONBLOCK);

            if(InvalidPipeHandle_ni34!=SecondHandleForPipe)
                close(SecondHandleForPipe);
        }
        break;

        default:
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnknownPipeType));
    }

    //try to delete the pipe to prevent further opens on the pipe from now
    if(Exists())
    {
        unlink(GiveName());
        PipeWasRemoved=1;
    }

    if(InvalidPipeHandle_ni34!=HandleForPipe)
    {
        char Buf[2];
        int NumberOfBytesToUse=0;

        ssize_t op;

        switch(DirectionOfPipe)
        {
            case In:
            {
                int MaxWaitForDataInPipe=10;
            
                NumberOfBytesToUse=1;

                do
                {
                    errno=0;

                    op=read(HandleForPipe, Buf, NumberOfBytesToUse);

                    if(op!=NumberOfBytesToUse)
                    {
                        sleep(1);
                        MaxWaitForDataInPipe--;
                    }
                }
                while(-1==op && errno==EAGAIN && 0<MaxWaitForDataInPipe);
            }
            break;

            case Out:
                op=write(HandleForPipe, Buf, NumberOfBytesToUse);
            break;

            default:
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnknownPipeType));
        }
            
        if(NumberOfBytesToUse!=op)
            rc=0;

        if(0!=close(HandleForPipe))
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotClosePipe, GiveName()));
            rc=0;
        }
    }
    else    // else TODO: distinguish between real Error and "not found/not open", until then -> return rc==1;
    {
        IsGoodFlag=rc=0;
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotConnectToPipe, GiveName()));
    }
    

    #else
        #error tni34_APipe::BreakAndRemovePipeIfOpened() is only coded for WIN32 and UNIX (define one of them).
    #endif

    return rc;
}

int tni34_APipe::Remove()
{
    int rc=Close();

    if(rc)
    {
        #if defined (WIN32)
        if(Created)
        {
            if(rc=CloseHandle(HandleForPipe))
                HandleForPipe=InvalidPipeHandle_ni34;
        }
        #elif defined (UNIX)
        rc=!unlink(GiveName());
        #else
            #error tni34_APipe::Remove() was only coded for WIN32 and UNIX, define one of them.
        #endif
        if(rc)
        {
            Created=0;
            rc=1;
        }
        else
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotRemovePipe,GiveName()));
            IsGoodFlag=0;
        }
    }

    return rc;
}

int tni34_APipe::PureQuietCreate(const char * name)
{   
    int rc=1;

    #if defined (WIN32)
    switch(DirectionOfPipe)
    {
        case In:
            HandleForPipe=CreateNamedPipe(name,
                                          PIPE_ACCESS_INBOUND,
                                          PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT,
                                          1,
                                          PrefferedBufferSize,
                                          PrefferedBufferSize,
                                          NMPWAIT_WAIT_FOREVER,
                                          NULL);
            break;
        case Out:
            HandleForPipe=CreateNamedPipe(name,
                                          PIPE_ACCESS_OUTBOUND,
                                          PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT,
                                          1,
                                          PrefferedBufferSize,
                                          PrefferedBufferSize,
                                          NMPWAIT_WAIT_FOREVER,
                                          NULL);
            break;
        default:
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnknownPipeType));
            HandleForPipe=InvalidPipeHandle_ni34;
    }

    if(InvalidPipeHandle_ni34==HandleForPipe)
        rc=0;

    #elif defined (UNIX)
    mode_t OldFileCreationMask=umask(S_IROTH|S_IWOTH);  // dont let the file creation mask be in our way
    #ifndef FREEBSD
    // mknod = ok, mkfifo = fail
    if(mknod(name, S_IFIFO|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP, 0))
        rc=0;
    #else
    if(mkfifo(name, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP))
        rc=0;
    #endif // FREEBSD

    umask(OldFileCreationMask); // try to leave the file creation mask unchanged
    #else
        #error tni34_APipe::PureQuietCreate() only coded for WIN32 and UNIX (Define one of them.)
    #endif
    
    return (IsGoodFlag=Created=rc);
}

int tni34_APipe::PureCreate()
{
    int rc=PureQuietCreate(GiveName());
    
    if(!rc)
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCreatePipe, GiveName()));

    return rc;
}

int tni34_APipe::PureOpen()
{
    int rc=Close();

    if(rc)
    {
        #if defined (WIN32)
        if(Created)
        {
            if(!ConnectNamedPipe(HandleForPipe,0) && GetLastError()!=ERROR_PIPE_CONNECTED)
                rc=0;
        }
        else
            switch(DirectionOfPipe)
            {
                case In:
                    HandleForPipe=CreateFile(GiveName(),GENERIC_READ,FILE_SHARE_WRITE,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
                    break;
                case Out:
                    HandleForPipe=CreateFile(GiveName(),GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
                    break;
                default:
                    ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnknownPipeType));
            }

        #elif defined (UNIX)
        switch(DirectionOfPipe)
        {
            case In:
                HandleForPipe=open(GiveName(),O_RDONLY);
                break;
            case Out:
                HandleForPipe=open(GiveName(),O_WRONLY);
                break;
            default:
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnknownPipeType));
        }
        #else
            #error tni34_APipe::Connect() only coded for WIN32 and UNIX (Define one of them.)
        #endif

        if(HandleForPipe==InvalidPipeHandle_ni34 || !rc)  // !rc is for errors with created NT-pipes 
        {
            IsGoodFlag=rc=0;
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotConnectToPipe,GiveName())); 
        }
        else
            IsOpenFlag=1; //rc==1 and IsGoodFlag==1 hold already;
    }
   
    return rc;
}

int tni34_APipe::PureQuietClose()
{
    int rc;

    if(IsOpen())
    {
        #if defined (WIN32)
        if(Created)
        {
            if(Out==DirectionOfPipe)
            {
                if(FlushFileBuffers(HandleForPipe))
                    rc=DisconnectNamedPipe(HandleForPipe);
                else
                    rc=0;
            }
            else
                rc=DisconnectNamedPipe(HandleForPipe);
        }
        else
            rc=CloseHandle(HandleForPipe);
        #elif defined (UNIX)
        rc=!close(HandleForPipe);
        #else
            #error tni34_APipe::PureQuietClose() only coded for WIN32 and UNIX (Define one of them.)
        #endif
        if(rc)
        {
            IsOpenFlag=0;
            #if defined (WIN32)
            if(!Created)
                HandleForPipe=InvalidPipeHandle_ni34;
            #elif defined (UNIX)
            HandleForPipe=InvalidPipeHandle_ni34;
            #else
                #error tni34_APipe::PureQuietClose() only coded for WIN32 and UNIX (Define one of them.)
            #endif
        }
    }
    else
        rc=1;

    return (IsGoodFlag=rc);
}


// -----------------------------------------------------------------------------
// member functions of class tni34_AVirtualPipe
// -----------------------------------------------------------------------------

tni34_AVirtualPipe::tni34_AVirtualPipe(const char *name,
                                       ADirection Dir,
                                       AOpenType OpenFlag)
    :tni34_ADataEntry(name),
    DirectionOfPipe(Dir),
    HandleForPipe(tsp00_Int4(0)),
    HandleIsValid(0)
{
    if(OpenNow==OpenFlag)
        Open();
}

tni34_AVirtualPipe::~tni34_AVirtualPipe()
{
    if(HandleIsValid)
        Close();
}

int tni34_AVirtualPipe::IsAPipe()
{
    int rc;

    tsp05_RteFileInfo  Info;
    tsp05_RteFileError Error;
    
    sqlfinfoc(GiveName(), &Info, &Error);

    if(Info.sp5fi_exists && vf_t_pipe==Info.sp5fi_media_kind)
        rc=1;
    else
        IsGoodFlag=rc=0;

    return rc;
}

int tni34_AVirtualPipe::Check()
{
    return 0;
}

int tni34_AVirtualPipe::Create()
{
    return 0;
}

int tni34_AVirtualPipe::Open()
{
    int rc=1;

    if(!HandleIsValid)
    {
        tsp05_RteFileMode    FileMode;

        switch(DirectionOfPipe)
        {
            case In:
                FileMode.becomes (sp5vf_read);
                break;
            case Out:
                FileMode.becomes (sp5vf_write);
                break;
            default:
            {
                rc=0;
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnknownPipeType));
            }
        }

        if(rc)
        {
            tsp05_RteFileError FError;

            sqlfopenc(GiveName(), sp5vf_binary, FileMode, sp5bk_unbuffered, &HandleForPipe, &FError);

            if(vf_ok!=FError.sp5fe_result)
            {
                rc=0;
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotOpenVirtualPipe,
                                                               GiveName(),
                                                               ErrorMsgToCString(FError.sp5fe_text)));
            }
        }

        if(rc)
        {
            IsOpenFlag=1;
            HandleIsValid=1;
        }
        else
        {
            IsGoodFlag=0;
        }
    }

    return rc;
}

int tni34_AVirtualPipe::Read(void *buf, long BytesToRead, long & BytesRead)
{
    int rc=1;

    if(HandleIsValid)
    {
        tsp00_Longint tspBytesReadNow;
        tsp05_RteFileError Error;

        BytesRead=0;
        sqlfreadc(HandleForPipe, buf, tsp00_Longint(BytesToRead), &tspBytesReadNow, &Error);

        switch(Error.sp5fe_result)
        {
            case vf_ok:
                BytesRead=long(tspBytesReadNow);
                break;
            case vf_eof:
                if(0==tspBytesReadNow)
                    rc=-1;                              //0==BytesRead
                else
                    BytesRead=long(tspBytesReadNow);    //1==rc, ignore vf_eof if we could read more than 0 bytes
                break;
            default:
            {
                ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotReadInVirtualPipe,
                                                               GiveName(),
                                                               ErrorMsgToCString(Error.sp5fe_text)));
                IsGoodFlag=rc=0;
            }
        }
    }
    else
    {
        IsGoodFlag=rc=0;
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnreadableVirtualPipe,
                                                       GiveName()));
    }

    return rc;
}

int tni34_AVirtualPipe::Write(const void *buf, long BytesToWrite, long & BytesWritten)
{
    int rc=1;

    if(HandleIsValid)
    {
        tsp05_RteFileError Error;

        sqlfwritec(HandleForPipe, buf, tsp00_Longint(BytesToWrite), &Error);

        if(vf_ok==Error.sp5fe_result)
            BytesWritten=BytesToWrite;
        else
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotWriteInVirtualPipe,
                                                           GiveName(),
                                                           ErrorMsgToCString(Error.sp5fe_text)));
            IsGoodFlag=rc=0;
            BytesWritten=0;
        }
    }
    else
    {
        IsGoodFlag=rc=0;
        ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseUnwritableVirtualPipe,
                                                       GiveName()));
    }            

    return rc;
}

int tni34_AVirtualPipe::Close()
{
    int rc=1;

    if(HandleIsValid)
    {
        tsp05_RteFileError FError;

        sqlfclosec(HandleForPipe, sp5vf_close_normal, &FError);

        if(vf_ok==FError.sp5fe_result)
        {
            HandleIsValid=0;
            IsOpenFlag=0;
        }
        else
        {
            rc=0;
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotCloseVirtualPipe,
                                                           GiveName(),
                                                           ErrorMsgToCString(FError.sp5fe_text)));
        }

    }

    return rc;
}

int tni34_AVirtualPipe::Remove()
{
    int rc=Close();
        
    if(rc)
    {
        tsp05_RteFileError FError;
        sqlferasec(GiveName(), &FError);

        if(vf_ok!=FError.sp5fe_result)
        {
            ErrorHandler_ni35->Report(tni35_AStandardError(tni35_AStandardError::CaseCouldNotRemoveVirtualPipe,
                                                           GiveName(),
                                                           ErrorMsgToCString(FError.sp5fe_text)));
            IsGoodFlag=rc=0;
        }
    }

    return rc;
}
