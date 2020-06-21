/*!---------------------------------------------------------------------
  @file           RTESys_IO.c
  @author         RobinW, JoergM
  @brief          platform-independent file I/O routines
  @see            


                Contains simple routines for opening files, reading from them etc..
                The routines work on all platforms. They were extracted from veo15.cpp
                in order to make them available outside the kernel, too.

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
---------------------------------------------------------------------*/



/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/

#include "RunTime/System/RTESys_IO.h"
#include "geo00_0.h"
#include "geo007_1.h"
#include "hsp77.h" /* sp77sprintf() */
#if !defined(_WIN32)
# include "RunTime/RTE_saveUNIXcalls.h" /* nocheck */
#endif
/*===========================================================================*
*  DEFINES                                                                  *
*===========================================================================*/

#define TEST_PRINT(fmt)
#define TEST_PRINT1(fmt,arg1)
#define TEST_PRINT3(fmt,arg1,arg2,arg3)
#define TEST_DPRINT3(fmt,arg1,arg2,arg3)

                                                
# define COPY_SIZE_RTESYS_IO ((16*1024)/sizeof(long))

/*===========================================================================*
*  MACROS                                                                   *
*===========================================================================*/

/*===========================================================================*
*  LOCAL VARIABLES                                                          *
*===========================================================================*/

/*===========================================================================*
*  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
*===========================================================================*/

/*===========================================================================*
*  PUBLIC FUNCTIONS (CODE)                                                  *
*===========================================================================*/

externC RTE_SystemRc RTESys_GetLastError()
{
#if defined(WIN32)
    return GetLastError();
#else
    return errno;
#endif
}

/* ----------------------------- */

externC SAPDB_Char *RTESys_StrError(RTE_SystemRc errCode, RTESys_ErrorMessage spaceToFill)
{
#if defined(_WIN32)
    LPVOID lpMsgBuf;
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );
    strncpy(&spaceToFill[0], (char *)lpMsgBuf, sizeof(RTESys_ErrorMessage)-1);
    LocalFree(lpMsgBuf);
#elif defined(LINUX) || defined(FREEBSD) || defined(AIX)
    if ( strerror_r(errCode, &spaceToFill[0], sizeof(RTESys_ErrorMessage)-1) )
    {
        sp77sprintf(&spaceToFill[0], sizeof(RTESys_ErrorMessage), "unknown error %d", (SAPDB_Int)errCode);
    }
#else
    strncpy(&spaceToFill[0], RTE_save_strerror(errCode), sizeof(RTESys_ErrorMessage)-1);
#endif
    spaceToFill[sizeof(RTESys_ErrorMessage)-1] = 0;
    return &spaceToFill[0];
}

/*---------------------------------------------------------------------------*/

/* PTS 1105164 */
externC void RTESys_IOCheckExist( const tsp00_Char * Filename,
                            bool  *DoesExist,
                            bool  *IsDirectory,
                            tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("RTESys_IOCheckExist"));

#if defined(_WIN32)
  DWORD Attributes = GetFileAttributes(Filename);
  
  *DoesExist = ( Attributes != 0xFFFFFFFF );
  
  if ( *DoesExist )
  {
    *ReturnStatus = vf_ok;
    *IsDirectory = ( (Attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY );
  }
  else
  {
    *ReturnStatus = (RTESys_GetLastError() == ERROR_FILE_NOT_FOUND ? vf_ok : vf_notok);
    *IsDirectory = false;
  }
#else
  struct stat testBuf;
  
  /* stat follows symbolic links... */
  *DoesExist = ( RTE_save_stat( Filename, &testBuf ) == 0 );
  
  if ( *DoesExist )
  {
    *ReturnStatus = vf_ok;
    *IsDirectory = ( testBuf.st_mode == S_IFDIR );
  }
  else
  {
    *ReturnStatus = ( RTESys_GetLastError() == ENOENT ? vf_ok : vf_notok );
    *IsDirectory = false;
  }
#endif
  
  return;
}

/*---------------------------------------------------------------------------*/
/* PTS 1105164 */
externC void RTESys_IOGetWorkingDirectory( tsp00_Pathc          WorkingDirectory,
                                     tsp00_VfReturn_Param *ReturnStatus )
{
    /* PTS 1114436 */
  static tsp00_Pathc currentWorkingDirectory; /* never changed after first getcwd... */
  static int         wasSet = 0;

  ROUTINE_DBG_MEO60 (_T("RTESys_IOGetWorkingDirectory"));

  if ( 1 == wasSet )
  {
      *ReturnStatus = vf_ok;
  }
  else
  {
      *ReturnStatus = ( 
#if defined(_WIN32)
        GetCurrentDirectory(sizeof(tsp00_Pathc), &currentWorkingDirectory[0]) == 0xFFFFFFFF 
#else
        RTE_save_getcwd(&currentWorkingDirectory[0], sizeof(tsp00_Pathc)) == NULL 
#endif
        ? vf_notok : vf_ok );
  }
  if ( vf_ok == *ReturnStatus )
  {
      wasSet = 1;
      strcpy( &WorkingDirectory[0], &currentWorkingDirectory[0]);
  }
}
/*===========================================================================*
*  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
*===========================================================================*/

/*---------------------------------------------------------------------------*/

externC void RTESys_IOOpen ( tRTESys_IOHandle     *FileHandle,
                        const tsp00_Char     *Path,
                        const tRTESys_IOOpenMode  FileMode,
                        const bool            DoCreate,
                        tRTESys_IOSecurityAttrib  Security,
                        tsp00_VfReturn_Param *ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("RTESys_IOOpen"));

#if defined(_WIN32)
  DWORD OpenMode;
  DWORD ShareMode;
  switch( FileMode )
  {
  case RTESys_IOReadWrite:
  case RTESys_IOReadWriteShared:
  case RTESys_IOReadWriteExclusive:
    OpenMode = GENERIC_READ|GENERIC_WRITE;
    break;
  case RTESys_IOReadOnly:
  case RTESys_IOReadOnlyShared:
  case RTESys_IOReadOnlyExclusive:
    OpenMode = GENERIC_READ;
    break;
  case RTESys_IOWriteOnly:
  case RTESys_IOWriteOnlyShared:
  case RTESys_IOWriteOnlyExclusive:
    OpenMode = GENERIC_WRITE;
    break;
  default:
    OpenMode = GENERIC_READ|GENERIC_WRITE;
    break;
  }
  switch( FileMode )
  {
  case RTESys_IOReadWrite:
  case RTESys_IOReadOnly:
  case RTESys_IOWriteOnly:
    ShareMode = FILE_SHARE_READ;
    break;
  case RTESys_IOReadWriteShared:
  case RTESys_IOReadOnlyShared:
  case RTESys_IOWriteOnlyShared:
    ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    break;
  case RTESys_IOReadOnlyExclusive:
  case RTESys_IOReadWriteExclusive:
  case RTESys_IOWriteOnlyExclusive:
    ShareMode = 0;
    break;
  default:
    ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    break;
  }
  *FileHandle = CreateFile( Path,
    OpenMode,
    ShareMode,
    Security,
    ( DoCreate ? OPEN_ALWAYS : OPEN_EXISTING ),
    FILE_ATTRIBUTE_NORMAL, /* WRITE_THROUGH would only be needed to protect againt system crash */
    INVALID_HANDLE_VALUE);
  if ( *FileHandle == INVALID_HANDLE_VALUE )
#else
  int OpenMode;
  switch( FileMode )
  {
  case RTESys_IOReadWrite:
  case RTESys_IOReadWriteShared:
  case RTESys_IOReadWriteExclusive:
    OpenMode = O_RDWR;
    break;
  case RTESys_IOReadOnly:
  case RTESys_IOReadOnlyShared:
  case RTESys_IOReadOnlyExclusive:
    OpenMode = O_RDONLY;
    break;
  case RTESys_IOWriteOnly:
  case RTESys_IOWriteOnlyShared:
  case RTESys_IOWriteOnlyExclusive:
    OpenMode = O_WRONLY;
    break;
  default:
    OpenMode = O_RDWR;
    break;
  }
  if ( DoCreate )
  {
      *FileHandle = RTE_save_openCreate( Path, OpenMode | O_CREAT, ( Security == 0 ? 0666 : Security ) );
  }
  else
  {
      *FileHandle = RTE_save_open( Path, OpenMode);
  }
  if ( *FileHandle < 0 )
#endif
  {
    *ReturnStatus = vf_notok;
  }
  else
  {
    *ReturnStatus = vf_ok;
  }
}

/*---------------------------------------------------------------------------*/

externC void RTESys_IOClose ( const tRTESys_IOHandle     FileHandle,
                         tsp00_VfReturn_Param      *ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("RTESys_IOClose"));

#if defined(_WIN32)
  BOOL Closed;

  Closed = CloseHandle( FileHandle );
  if ( !Closed )
#else
  if ( RTE_save_close( FileHandle ) == -1 )
#endif
  {
    *ReturnStatus = vf_notok;
  }
  else
  {
    *ReturnStatus = vf_ok;
  }
}

/*---------------------------------------------------------------------------*/

externC void RTESys_IOChangeMode ( const SAPDB_Char *filePath,
                                   tRTESys_IOOpenMode newMode,
                                   tsp00_VfReturn_Param      *ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("RTESys_IOChangeMode"));

#if defined(_WIN32)
  DWORD currentAttributes;
  DWORD newAttributes;
  currentAttributes = GetFileAttributes(filePath);
  
  if ( ((DWORD)0xFFFFFFFF) == currentAttributes )
  {
      *ReturnStatus = (RTESys_GetLastError() == ERROR_FILE_NOT_FOUND ? vf_ok : vf_notok);
  }
  else
  {
      *ReturnStatus = vf_ok;
      newAttributes = currentAttributes;

      switch( newMode ) {
      case RTESys_IOReadOnly:
          if ( FILE_ATTRIBUTE_READONLY != ( FILE_ATTRIBUTE_READONLY & currentAttributes) )
          {
              newAttributes = (currentAttributes | FILE_ATTRIBUTE_READONLY);
          }
          break;
      case RTESys_IOReadWrite:
          if ( FILE_ATTRIBUTE_READONLY == ( FILE_ATTRIBUTE_READONLY & currentAttributes) )
          {
              newAttributes = (currentAttributes & ~FILE_ATTRIBUTE_READONLY);
          }
          break;
      case RTESys_IOWriteOnly:
          /* Windows does not support explicit WriteOnly mode */
          if ( FILE_ATTRIBUTE_READONLY == ( FILE_ATTRIBUTE_READONLY & currentAttributes) )
          {
              newAttributes = (currentAttributes & ~FILE_ATTRIBUTE_READONLY);
          }
          break;          
      default:
          break;
      }

      if ( newAttributes != currentAttributes )
      {
          *ReturnStatus = ( SetFileAttributes(filePath, newAttributes) ? vf_ok : vf_notok );
      }
  }
#else
  struct stat statBuf;
  mode_t      newAttributes;

  if ( 0 != RTE_save_stat( filePath, &statBuf ) )
  {
      *ReturnStatus = (RTESys_GetLastError() == ENOENT ? vf_ok : vf_notok);
  }
  else
  {
      *ReturnStatus = vf_ok;
      newAttributes = statBuf.st_mode;

      switch( newMode ) {
      case RTESys_IOReadOnly:
          if ( 0 != ( (S_IWUSR|S_IWGRP|S_IWOTH) & statBuf.st_mode ) )
          {
              /* ReadOnly means no write access for anybody and at least read acccess for user */
              newAttributes = ((~(S_IWUSR|S_IWGRP|S_IWOTH)) & statBuf.st_mode );
          }
          newAttributes |= S_IRUSR;
          break;
      case RTESys_IOReadWrite:
          /* Read write for user */
          newAttributes |= (S_IRUSR|S_IWUSR);
          break;
      case RTESys_IOWriteOnly:
          if ( 0 != ( (S_IRUSR|S_IRGRP|S_IROTH) & statBuf.st_mode ) )
          {
              /* WriteOnly means no read access for anybody and at least write acccess for user */
              newAttributes = ((~(S_IRUSR|S_IRGRP|S_IROTH)) & statBuf.st_mode );
          }
          newAttributes |= S_IWUSR;
          break;          
      default:
          break;
      }

      if ( newAttributes != statBuf.st_mode )
      {
          *ReturnStatus = ( (0 == RTE_save_chmod(filePath, newAttributes)) ? vf_ok : vf_notok );
      }
  }
#endif
}

/*---------------------------------------------------------------------------*/

externC void RTESys_IOWrite ( const tRTESys_IOHandle    FileHandle,
                         const void           *Buffer,
                         const tRTESys_IOPosition  WantedSizeInBytes,
                         tRTESys_IOPosition       *WrittenSizeInBytes,
                         tsp00_VfReturn_Param *ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("RTESys_IOWrite"));
TEST_DPRINT3("Writing to %d 0x%lx (0x%lx bytes)\n", FileHandle, Buffer, WantedSizeInBytes )
#if defined(_WIN32)
  DWORD Wanted = (DWORD)WantedSizeInBytes;
  DWORD Written;
#else
  size_t Wanted = WantedSizeInBytes;
  ssize_t Written;
#endif

  *WrittenSizeInBytes = 0;
  do {
#if defined(_WIN32)
  
    if ( !WriteFile( FileHandle,
      ((char *)Buffer)+*WrittenSizeInBytes,
      Wanted,
      &Written,
      NULL) )
    {
      break;
    }
    
    if ( Written > 0 )
    {
      *WrittenSizeInBytes += Written;
      Wanted -= Written;
    }
#else
    Written = RTE_save_write( FileHandle, ((char *)Buffer)+*WrittenSizeInBytes, Wanted );

    /* PTS 1109920 */
    if ( (Written == (ssize_t)-1) )
    {
        break;
    }

    if ( Written > 0 )
    {
      *WrittenSizeInBytes += Written;
      Wanted -= Written;
    }
#endif
  } while ( Wanted > 0 );
  
  if ( *WrittenSizeInBytes != WantedSizeInBytes )
  {
    *ReturnStatus = vf_notok;
  }
  else
  {
    *ReturnStatus = vf_ok;
  }
}

/*---------------------------------------------------------------------------*/

externC void RTESys_IORead ( const tRTESys_IOHandle      FileHandle,
                        void                   *Buffer,
                        const tRTESys_IOPosition    WantedSizeInBytes,
                        tRTESys_IOPosition         *ReceivedSizeInBytes,
                        tsp00_VfReturn_Param   *ReturnStatus )
{
    ROUTINE_DBG_MEO60 (_T("RTESys_IORead"));

#if defined(_WIN32)
    DWORD BytesToRead = (DWORD)WantedSizeInBytes;
    DWORD ReceivedBytes;
    bool  ReadOk;

    ReadOk = ( ReadFile( FileHandle,
                Buffer,
                BytesToRead,
                &ReceivedBytes,
                NULL) != 0 );
  
    if ( ReadOk )
    {
        *ReceivedSizeInBytes = ReceivedBytes;
    }
  
    if ( !ReadOk )
    {
        if ( RTESys_GetLastError() == ERROR_HANDLE_EOF 
          || RTESys_GetLastError() == ERROR_BROKEN_PIPE )
        {
            ReadOk = true;
            *ReceivedSizeInBytes = 0;
        }
    }
 
    if ( !ReadOk )
#else
    *ReceivedSizeInBytes = RTE_save_read( FileHandle, Buffer, WantedSizeInBytes );
  
    if ( *ReceivedSizeInBytes < 0 )
#endif
    {
        *ReturnStatus = vf_notok;
    }
    else
    {
        if ( *ReceivedSizeInBytes == 0 )
        {
            *ReturnStatus = vf_eof;
        }
        else
        {
            *ReturnStatus = vf_ok;
        }
    }
}

/*---------------------------------------------------------------------------*/

externC void RTESys_IOSeek ( const tRTESys_IOHandle      FileHandle,
                        const tRTESys_IOPosition    Position,
                        const tRTESys_IOSeekMode    SeekMode,
                        tRTESys_IOPosition         *NewPosition,
                        tsp00_VfReturn_Param   *ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("RTESys_IOSeek"));

#if defined(_WIN32)
  DWORD whence;
  LONG lowPosition = (LONG)(Position & 0xFFFFFFFFU);
  LONG highPosition = (LONG)(Position >> 32);
  DWORD lowNewPosition;

  switch ( SeekMode )
  {
  case RTESys_IOSeekSet:
    whence = FILE_BEGIN;
    break;
  case RTESys_IOSeekEnd:
    whence = FILE_END;
    break;
  case RTESys_IOSeekCurrent:
    whence = FILE_CURRENT;
    break;
  default:
    whence = FILE_BEGIN;
    break;
  }
  lowNewPosition = SetFilePointer( FileHandle,
    lowPosition,
    &highPosition,
    whence);
  if ( ((DWORD)-1) != lowNewPosition )
  {
     *NewPosition = lowNewPosition + ( ((tRTESys_IOPosition)highPosition) << 32 );
  }
  else
  {
     *NewPosition = (tRTESys_IOPosition)-1;
  }
#else
  int whence;
  switch ( SeekMode )
  {
  case RTESys_IOSeekSet:
    whence = SEEK_SET;
    break;
  case RTESys_IOSeekEnd:
    whence = SEEK_END;
    break;
  case RTESys_IOSeekCurrent:
    whence = SEEK_CUR;
    break;
  default:
    whence = SEEK_SET;
    break;
  }
  *NewPosition = RTE_save_lseek( FileHandle,
    Position,
    whence);
#endif
  if ( ((tRTESys_IOPosition)-1) == *NewPosition )
  {
    *ReturnStatus = vf_noseek;
  }
  else
  {
    *ReturnStatus = vf_ok;
  }
}

/*---------------------------------------------------------------------------*/

externC void RTESys_IOCopyFile( const tsp00_Char                   *Original,
                                const tsp00_Char                   *Destination,
                                tsp00_VfReturn_Param               *ReturnStatus,
                                RTESys_IOTraceOutputFuncFuncType    traceOutputFunc )
{
  RTESys_IOCopyFileWithDestinationAtribute(Original,Destination,-1,ReturnStatus,traceOutputFunc );
}

/*---------------------------------------------------------------------------*/
externC void RTESys_IOCopyFileWithDestinationAtribute
                              ( const tsp00_Char                   *Original,
                                const tsp00_Char                   *Destination,
                                SAPDB_Int                           DestinationAttribute,
                                tsp00_VfReturn_Param               *ReturnStatus,
                                RTESys_IOTraceOutputFuncFuncType    traceOutputFunc )
{
#if defined(_WIN32)
  ROUTINE_DBG_MEO60 (_T("RTESys_IOCopyFile"));

  SetFileAttributes( Destination,GetFileAttributes(Destination) & ~FILE_ATTRIBUTE_READONLY);

  *ReturnStatus = ( CopyFile( Original, Destination, false ) ? vf_ok : vf_notok );

  if(DEFAULT_ATTRIBUTE_RTESYS_IO != DestinationAttribute)
  {
    SetFileAttributes( Destination,DestinationAttribute );
  }

#else
  tRTESys_IOPosition BytesWritten;
  tRTESys_IOPosition BytesRead;
  long Buffer[COPY_SIZE_RTESYS_IO];
  tRTESys_IOHandle OriginalFd;
  tRTESys_IOHandle DestinationFd;
  tsp00_VfReturn_Param IgnoredError; /* close errors of temporary files are ignored here */
  SAPDB_Bool DestinationAttributeSetOnOpen = false;  

  ROUTINE_DBG_MEO60 (_T("RTESys_IOCopyFile"));

  RTESys_IOOpen( &OriginalFd, Original, RTESys_IOReadOnly, false, DEFAULT_SECURITY_RTESYS_IO, ReturnStatus );
  if ( vf_ok == *ReturnStatus )
  {
    if( DEFAULT_ATTRIBUTE_RTESYS_IO == DestinationAttribute )
    {
      struct stat orgBuf;
      if ( 0 == RTE_save_stat(Original, &orgBuf) )
      {
        DestinationAttribute = orgBuf.st_mode;
      }
      else
      {
        DestinationAttribute = DEFAULT_SECURITY_RTESYS_IO;
      }
    }
    RTESys_IOOpen( &DestinationFd, Destination, RTESys_IOWriteOnly, true, DestinationAttribute, ReturnStatus );
    if ( vf_ok != *ReturnStatus )
    {

      RTESys_IOOpen( &DestinationFd, Destination, RTESys_IOWriteOnly, false, DEFAULT_SECURITY_RTESYS_IO, ReturnStatus );
      if ( vf_ok != *ReturnStatus )
      {
        if(traceOutputFunc)
            (*traceOutputFunc)("Open(Destination)", Destination );
        RTESys_IOClose( OriginalFd, &IgnoredError );
        return;
      }
    }
    else
    {
        DestinationAttributeSetOnOpen = true;
    } 
    
    do {
      RTESys_IORead( OriginalFd, Buffer, COPY_SIZE_RTESYS_IO, &BytesRead, ReturnStatus );
      if ( vf_ok == *ReturnStatus )
      {
        RTESys_IOWrite( DestinationFd, Buffer, BytesRead, &BytesWritten, ReturnStatus );
        if ( vf_ok != *ReturnStatus )
        {
          if(traceOutputFunc)
              (*traceOutputFunc)("Write(Destination)", Destination );
        }
      }
      else if ( vf_eof != *ReturnStatus ) /* 1110223 */
      {
        if(traceOutputFunc)
            (*traceOutputFunc)("Read(Original)", Original );
      }
    } while ( vf_ok == *ReturnStatus && 0 != BytesRead );
    RTESys_IOClose( OriginalFd, &IgnoredError );
    RTESys_IOClose( DestinationFd, &IgnoredError );

    if ( *ReturnStatus == vf_eof && BytesRead == 0 )
    {
      *ReturnStatus = vf_ok;
    }
    if(!DestinationAttributeSetOnOpen) 
    {   
      if ( vf_ok == *ReturnStatus )
      {
          /* Ignore any errors if mode setting fails... */
          struct stat orgBuf;
          if ( 0 == RTE_save_stat(Original, &orgBuf) )
          {
              if ( 0 != RTE_save_chmod(Destination, orgBuf.st_mode) )
              {
                  if(traceOutputFunc)
                      (*traceOutputFunc)("Ignoring bad chmod(Destination)", Destination );
              }
          }
          else
          {
              if(traceOutputFunc)
                  (*traceOutputFunc)("Ignoring bad stat(Original)", Original );
          }
      }
    }
  }
  else
  {
    if(traceOutputFunc)
        (*traceOutputFunc)("Open(Original)", Original );
  }
#endif
}

/*---------------------------------------------------------------------------*/

externC void RTESys_IOUnlink ( const tsp00_Char                 *FileName,
                           tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("RTESys_IOUnlink"));

#if defined(_WIN32)
    *ReturnStatus = ( DeleteFile(FileName) ? vf_ok : vf_notok );
#else
    *ReturnStatus = ( RTE_save_unlink(FileName) == 0 ? vf_ok : vf_notok );
#endif /* WIN32 */
}

/*---------------------------------------------------------------------------*/

externC bool RTESys_IOCheckFilename(const tsp00_Char *Filename)
{
  bool ReturnStatus = true;

  if ( (Filename == NULL) || (*Filename == '\0') || strlen(Filename) > sizeof(tsp00_Path) )
  {
    ReturnStatus = false;

    if ( Filename == NULL )
    {
      MSGD((ERR_TRACEIO_CHECK_FILENAME, "<NULL>", "invalid file name"));
    } else if ( *Filename == '\0' ) {
      MSGD((ERR_TRACEIO_CHECK_FILENAME, "<Empty>", "zero length file name"));
    } else {
      MSGD((ERR_TRACEIO_CHECK_FILENAME, Filename, "file name too long"));
    }
  }

  return ReturnStatus;
}

/*---------------------------------------------------------------------------*/

externC void RTESys_IORename(const tsp00_Char *OldFileName,
                             const tsp00_Char *NewFileName,
                             tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus)
{
    ROUTINE_DBG_MEO60 (_T("RTESys_IORename"));

#if defined(_WIN32)
    *ReturnStatus = ( MoveFile(OldFileName, NewFileName) ? vf_ok : vf_notok );
#else
    *ReturnStatus = ( RTE_save_rename(OldFileName, NewFileName) == 0 ? vf_ok : vf_notok );
#endif /* WIN32 */
}
/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/
