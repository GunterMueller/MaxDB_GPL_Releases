/*!

  \file    hni34.h
  \author  TiloH
  \ingroup ni layer
  \brief   classes handling files, directories and pipes

\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#if !defined(_HNI34_H_)
#define _HNI34_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "hni00b.h"
#include "heo06.h"

#if defined (WIN32)
#include<windows.h>
#elif defined (UNIX)
#include<sys/types.h>
#include<dirent.h>
#else
    #error ni34.h only coded for WIN32 and UNIX (Define one of them.)
#endif


// -----------------------------------------------------------------------------
// class tni34_AFileSystemEntry
// -----------------------------------------------------------------------------

class tni34_AFileSystemEntry
{
  public:
    virtual int Exists();

    virtual int Check();
    virtual int Create();
    virtual int Remove();
    
    const char * GiveName() const;
    int IsGood() const;

  protected:
    tni34_AFileSystemEntry(const char * name=0, int CreateOnCheck=0);
    virtual ~tni34_AFileSystemEntry();

    int SetNameTo(const char *name);
    int ResetNameTo(const char *NewName);

    int IsGoodFlag;
    int CreateOnCheckFlag;

    int IsOpenFlag;

  private:
    char *Name;
};


// -----------------------------------------------------------------------------
// types
// -----------------------------------------------------------------------------

struct tni34_ADirectoryEntry: tni34_AFileSystemEntry
{
    tni34_ADirectoryEntry(const char * name=0);
};


// -----------------------------------------------------------------------------
// class tni34_ADirectory
// -----------------------------------------------------------------------------

//! \brief a class for handling directories
class tni34_ADirectory:public tni34_AFileSystemEntry
{
  public:
    /*! \brief constructor

        constructs an object for the directory specified by Name
        \param Name          [IN] the name of the directory to work with
        \param CreateOnCheck [IN] should the directory be created, if not existing
                                  during any following check for existence
        \param CheckNow      [IN] should the constructor already check for the existence
                                  of the directory */
    tni34_ADirectory(const char * Name, int CreateOnCheck=0, int CheckNow=1);

    //! \brief destructor
    ~tni34_ADirectory();

    /*! is the directory really a directory in the file system?
        \return 1 if the directory is one, 0 otherwise */
    int IsADirectory();

    /*! check if the directory exists and is a directory in the file system?
        \return 1 if the directory exists and is a directory, 0 otherwise */
    int Check();

    /*! create the directory in the file system
        \return 1 if successful, 0 otherwise */
    int Create();

    /*! open the directory for reading
        \return 1 if successful, 0 otherwise */
    int Open();

    /*! read on entry of the directory
        \param NextEntry [OUT] a pointer to the name of the
                               next entry in the directory
        \return 1 if successful, 0 otherwise */
    int Read(const char * & NextEntry);

    /*! remove the directory from the file system
        \return 1 if successful, 0 otherwise */
    int Remove();

  protected:
    tni34_ADirectoryEntry **DirectoryEntries;          //! \todo should be replaced by a vector
    int                     NumberOfDirectoryEntries;
    int                     PlaceForEntries;
    int                     NumberOfCurrentReadEntry;

    int  IncreaseDirectoryEntriesIfNeeded();
    int  AppendDirectoryEntry(const char * EntryName);
    void FreeDirectoryEntrys();
};


// -----------------------------------------------------------------------------
// class tni34_ADataEntry
// -----------------------------------------------------------------------------

class tni34_ADataEntry:public tni34_AFileSystemEntry
{
  public:
    virtual int Open()=0;
    virtual int Read(void *buf, long BytesToRead, long & BytesRead)=0;
    virtual int Write(const void *buf, long BytesToWrite, long & BytesWritten)=0;
    virtual int Close()=0;

    int IsOpen();

    static int               SetTemporaryDataEntryDirectory(const tni34_ADirectory & NewTempDir);
    const tni34_ADirectory & GetTemporaryDataEntryDirectory();

  protected:
    tni34_ADataEntry(const char * name=0, int CreateOnOpen=0, int CreateOnCheck=0);
    
    int CreateOnOpenFlag;

    static tni34_ADirectory * TemporaryDataEntryDirectory;
    virtual int CreateTemporaryName(char *& TempName, const tni34_ADirectory * TempDir=0, int RepeatedRequest=0);
};


// -----------------------------------------------------------------------------
// constants and types
// -----------------------------------------------------------------------------

#if defined (WIN32)
typedef HANDLE tni34_AFileHandle;
const tni34_AFileHandle InvalidFileHandle_ni34=INVALID_HANDLE_VALUE;
#elif defined (UNIX)
typedef int tni34_AFileHandle;
const tni34_AFileHandle InvalidFileHandle_ni34=-1;
#endif


// -----------------------------------------------------------------------------
// class tni34_AFile
// -----------------------------------------------------------------------------

class tni34_AFile:public tni34_ADataEntry
{
  public:
    enum ADirection {FromDisk, ToDiskTruncate, ToDiskAppend};

    tni34_AFile(ADirection Dir=FromDisk,
                int        CreateOnOpen=1,
                int        OpenNow=1,
                int        CreateOnCheck=0,
                int        PrefBufSize=65536,
                int        MaxNumberOfRetries=100000);

    tni34_AFile(const char * name,
                ADirection   Dir=FromDisk,
                int          CreateOnOpen=1,
                int          OpenNow=1,
                int          CreateOnCheck=0,
                int          PrefBufSize=65536);

    ~tni34_AFile();
    
    int IsAFile();

    int Check();
    int Create();
    int Open();
    int Read(void *buf, long BytesToRead, long & BytesRead);
    int Write(const void *buf, long BytesToWrite, long & BytesWritten);
    
    /*!
        @brief flush buffer of this file to disk
        @return 1 if operation succeeded, 0 otherwise
    */
    int flush();
    int Close();
    int Remove();
    int Rename(const char * NameToMoveTo);
    
#if defined (WIN32)
  protected:
    virtual DWORD getShareMode();
#endif

  private:
    ADirection DirectionOfFile;
    int PrefferedBufferSize;

    tni34_AFileHandle HandleForFile;

    int PureQuietCreate(const char * name);
    int PureCreate();
    int PureOpen();
    int PureQuietClose();
};


// -----------------------------------------------------------------------------
// class tni34_AVirtualFile
// -----------------------------------------------------------------------------

class tni34_AVirtualFile:public tni34_ADataEntry
{
  public:
    enum ADirection {FromDisk, ToDiskTruncate, ToDiskAppend};
    enum AOpenType {OpenNow, DoNotOpenNow};

    tni34_AVirtualFile(const char * name,
                       ADirection   Dir=FromDisk,
                       AOpenType    OpenFlag=OpenNow);

    ~tni34_AVirtualFile();

    int Exists();
    int IsAFile();

    int Check();
    int Create();

    int Open();
    int Read(void *buf, long BytesToRead, long & BytesRead);
    int Write(const void *buf, long BytesToWrite, long & BytesWritten);
    int Close();
    int Remove();

    int GetSize(teo00_Uint8 & FileSize);
    int GetDateTime(char Date[9], char Time[7]);
    int GetDateTimeSize(char Date[9], char Time[7], teo00_Uint8 & FileSize);
    
  private:
    ADirection DirectionOfFile;

    tsp00_Int4 HandleForFile;
    int      HandleIsValid;
};


// -----------------------------------------------------------------------------
// class tni34_ATextFile
// -----------------------------------------------------------------------------

class tni34_ATextFile:public tni34_AFile
{
  public:
    tni34_ATextFile(ADirection   Dir=FromDisk,
                    int          CreateOnOpen=1,
                    int          OpenNow=1,
                    int          CreateOnCheck=0,
                    unsigned int EstimatedLineSize=1024,
                    int          PrefBufSize=65536,
                    int          MaxNumberOfRetries=100000);

    tni34_ATextFile(const char * name,
                    ADirection   Dir=FromDisk,
                    int          CreateOnOpen=1,
                    int          OpenNow=1,
                    int          CreateOnCheck=0,
                    unsigned int EstimatedLineSize=1024,
                    int          PrefBufSize=65536);

    ~tni34_ATextFile();

    int Open();

    int ReadLine(const char * & buf);
    int WriteLine(const char * buf, long & BytesWritten);
    int WriteLine(const char * buf);
    
  private:
    char * ReadBuffer;
    size_t SizeOfReadBuffer;
    size_t PositionInReadBuffer;
    size_t EndInReadBuffer;

    int    EndOfFileReached;
    int    AllLinesRead;

    int  ReAllocReadBufferTo(size_t NewSize);
    void MoveStringToStartOfBuffer();

    int FoundEndOfLineForSure(char * & EndOfLine, const char * &FoundDelimiter);
};


// -----------------------------------------------------------------------------
// constants and types
// -----------------------------------------------------------------------------

#if defined (WIN32)
typedef HANDLE tni34_APipeHandle;
const tni34_APipeHandle InvalidPipeHandle_ni34=INVALID_HANDLE_VALUE;
#elif defined (UNIX)
typedef int tni34_APipeHandle;
const tni34_APipeHandle InvalidPipeHandle_ni34=-1;
#endif


// -----------------------------------------------------------------------------
// class tni34_APipe
// -----------------------------------------------------------------------------

class tni34_APipe:public tni34_ADataEntry
{
  public:
    enum ADirection {In, Out};

    tni34_APipe(ADirection Dir,
                int        CreateOnOpen=1,
                int        OpenNow=1,
                int        PrefBufSize=65536,
                int        MaxNumberOfRetries=100000);  //creates a temporary named pipe

    tni34_APipe(const char * name,
                ADirection   Dir,
                int          CreateOnOpen=1,
                int          OpenNow=1,
                int          PrefBufSize=65536);

    ~tni34_APipe();
    
    int IsAPipe();

    int Exists();
    int Check();
    int Create();
    int Open();
    int Read(void *buf, long BytesToRead, long & BytesRead);
    int Write(const void *buf, long BytesToWrite, long & BytesWritten);
    int IsDataAvailable();
    int Close();
    int Remove();
    int BreakAndRemovePipeIfOpened(int & PipeWasRemoved);
 
  private:
    ADirection DirectionOfPipe;
    int PrefferedBufferSize;
    int Created;

    tni34_APipeHandle HandleForPipe;

    int PureQuietCreate(const char * name);
    int PureCreate();
    int PureOpen();
    int PureQuietClose();
};


// -----------------------------------------------------------------------------
// class tni34_AVirtualPipe
// -----------------------------------------------------------------------------

class tni34_AVirtualPipe:public tni34_ADataEntry //TODO: usage of common base class with tni34_AVirtualFile
{
  public:
    enum ADirection {In, Out};
    enum AOpenType {OpenNow, DoNotOpenNow};

    tni34_AVirtualPipe(const char * name,
                       ADirection   Dir,
                       AOpenType    OpenFlag=OpenNow);

    ~tni34_AVirtualPipe();
    
    int IsAPipe();

    int Check();
    int Create();

    int Open();
    int Read(void *buf, long BytesToRead, long & BytesRead);
    int Write(const void *buf, long BytesToWrite, long & BytesWritten);
    int Close();
    int Remove();
    
  private:
    ADirection DirectionOfPipe;

    tsp00_Int4 HandleForPipe;
    int      HandleIsValid;
};


// -----------------------------------------------------------------------------
// inline functions
// -----------------------------------------------------------------------------

inline const char * tni34_AFileSystemEntry::GiveName() const
{
    return Name;
}

inline int tni34_AFileSystemEntry::IsGood() const
{
    return IsGoodFlag;
}

inline int tni34_ADataEntry::IsOpen()
{
    return IsOpenFlag;
}

#endif
