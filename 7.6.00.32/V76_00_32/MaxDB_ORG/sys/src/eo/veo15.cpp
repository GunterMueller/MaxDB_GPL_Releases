/*!
  @file           veo15.cpp
  @author         JoergM
  @special area   Message Output Module (for buffered Message I/O)
  @brief          Contains Message buffering for use inside the kernel
PTS 1103824
  @see            example.html ...

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



/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/

#include "gsp00.h"
#include "heo07.h"
#include "heo15.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "geo57.h"
#include "geo001.h"
#include "hsp77.h" // sp77sprintf()
#if defined(_WIN32)
#include "gos00k.h" /* nocheck */
#else
#include "hen40.h" /* nocheck */
#endif

#include "RunTime/System/RTESys_IO.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/Threading/RTEThread_KernelThread.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_MessageList.hpp"

#include <ctype.h>
/*===========================================================================*
*  DEFINES                                                                  *
*===========================================================================*/

#ifdef TEST
#include <malloc.h>
#undef ALLOC_MEM_EO57
#define ALLOC_MEM_EO57(_pBuf,_s) \
    ( (*_pBuf = malloc(_s)) != NULL ? NO_ERROR_EO001 : ERROR_NOT_ENOUGH_MEMORY_EO001 )
#undef FREE_MEM_EO57
#define FREE_MEM_EO57(_Buf) free(_Buf)

int DbgPrint = 0;
#define TEST_PRINT(fmt) printf(fmt);
#define TEST_PRINT1(fmt,arg1) printf(fmt,arg1);
#define TEST_PRINT3(fmt,arg1,arg2,arg3) printf(fmt,arg1,arg2,arg3);
#define TEST_DPRINT3(fmt,arg1,arg2,arg3) if ( DbgPrint ) printf(fmt,arg1,arg2,arg3);
#else
#define TEST_PRINT(fmt)
#define TEST_PRINT1(fmt,arg1)
#define TEST_PRINT3(fmt,arg1,arg2,arg3)
#define TEST_DPRINT3(fmt,arg1,arg2,arg3)
#endif /* TEST */

#define END_LINE_EO15       "----- Last Line Written ----- Cycle:" // followed by max 12 digits
#define CLR_LINE_MATCH_EO15 "===== Last Line"
#define CLR_LINE_EO15 CLR_LINE_MATCH_EO15   " in File. More Information above!"
#define FIRST_LINE_EO15     "===== Current Cycle:"
#define CRASH_MARK_EO15     "===== WARNING: Crash may reproduce information   "

/* Size used for eo15_CopyFile */
# define COPY_SIZE_EO15 ((16*1024)/sizeof(long))

/*===========================================================================*
*  MACROS                                                                   *
*===========================================================================*/

#if !defined(_WIN32)
extern "C" void vabort(bool);
#  define ABORT()                    vabort(true)
#endif /* _WIN32 */

#define WORKER_STACK_EO15 (64*1024)
/*===========================================================================*
*  LOCAL VARIABLES                                                          *
*===========================================================================*/
#if defined(_WIN32)
#define USE_DYNAMIC_MESSAGE_BUFFER
#else
#undef USE_DYNAMIC_MESSAGE_BUFFER
#endif

#ifdef USE_DYNAMIC_MESSAGE_BUFFER

  static teo07_Mutex                eo15_BufferMapMutex;
  static teo15MessageBufferSpace ** eo15_BufferMap; // Each entry in this list is filled on demand
  static tsp00_Int4                 eo15_BufferMapSize;

#  define MAP_INCREASE_AMOUNT_EO15 64 /* If eo15_BuffsListMapSize must ever get increased, use this size */
#  define MESSAGE_BUFFER_PTR(_i)     (eo15_BufferMap[_i])
#  define NUMBER_OF_MESSAGE_BUFFERS   eo15_BufferMapSize

#else

// Due to usage of a shared memory segment, the number of buffers is fixed...
  static teo15MessageBufferSpace *  eo15_BufferArray;
#  define MESSAGE_BUFFER_PTR(_i)    (eo15_BufferArray+_i)
#  define NUMBER_OF_MESSAGE_BUFFERS   eo15_NumberOfBuffers

#endif /* USE_DYNAMIC_MESSAGE_BUFFER */

static tsp00_Int4                 eo15_NumberOfBuffers; // Maximum number of buffers

static teo15_Buffer               eo15_ClearBuffer; // a buffer filled with spaces
static teo15_Buffer               eo15_ExpandBuffer; // a buffer filled with zeros
static tsp00_Int4                 eo15_CycleOverhead; // extra bytes needed for cycle head and tyil
static bool eo15_ExpandMessageFiles = false;
/*===========================================================================*
*  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
*===========================================================================*/
/*---------------------------------------------------------------------------*/
static void eo15_MapAndLockMessageFile( const tsp00_Char *    Filename,
                                       tsp00_Int4           &FileIndex,
                                       tsp00_VfReturn_Param &ReturnStatus );
/*---------------------------------------------------------------------------*/
static void eo15_SearchAndLockMessageFile( const tsp00_Char *    Filename,
                                       tsp00_Int4           &FileIndex,
                                       bool                     &isFree,
                                       tsp00_VfReturn_Param &ReturnStatus );
/*---------------------------------------------------------------------------*/
static void eo15ShowLastOsError( const tsp00_Char *    Routine,
                                 const tsp00_Char *    Filename );
  

/*---------------------------------------------------------------------------*/
#define LAST_ERROR_MESSAGE_SIZE_EO15 64
typedef char teo15_LastErrorMessage[LAST_ERROR_MESSAGE_SIZE_EO15];

/*---------------------------------------------------------------------------*/

class Worker : public RTEThread_KernelThread
{
public:
    /*!
    @description    Startup a new task worker

                    This call creates the worker thread, that is listening on its
                    worker queue and does blocking calls instead of tasks. This 
                    routine is called once during kernel initialization, where no 
                    task is running.

    @param          stackSize   [in] - Stack size in bytes.
    @return value   true:   Thread could be started
                    false:  Start failed
    */
    static SAPDB_Bool BeginThread( SAPDB_UInt4            stackSize,
                                   SAPDBErr_MessageList   &messageList );



    /*!
    @description    Wakeup thread to start execution 
    */
    static SAPDB_Bool Continue( SAPDBErr_MessageList   &messageList );

private:
    /*!
       @brief          Constructor
     */
    Worker()  : RTEThread_KernelThread("MessageOutputWorker") {}

    /*!
    @description    The created thread runs the code of the methode 'ThreadMain'
    */
    virtual SAPDB_Int4  KernelThreadMain();

    /*!
    @description    Wait for a wakeup call
    */
    void Wait();
    
  /*!
    @description    Destroy object
    */
   virtual void  DestroyObject() { this->~Worker();
                                   RTEMem_RteAllocator::Instance().Deallocate(this);
                                   m_pInstance = 0; }

private:
    static Worker*    m_pInstance;
};

/*---------------------------------------------------------------------------*/

Worker* Worker::m_pInstance = 0;

class ceo15_LockMutex
{
public:
  ceo15_LockMutex(teo07_Mutex &Mutex) : m_Mutex(Mutex) { sqlbeginmutex(&Mutex); }
  ~ceo15_LockMutex() { sqlendmutex(&m_Mutex); }
private:
  teo07_Mutex &m_Mutex;
};

/*---------------------------------------------------------------------------*/

class ceo15_MessageFile
{
private:
  teo15MessageBufferSpace &m_Space;

  bool eo15Expand( const teo15_Position CurrentEndOfFilePosition,
                   tsp00_VfReturn_Param &ReturnStatus );

  bool eo15GetCompleteEndLine( const tsp00_Byte *  stringToMatch,
                               const teo15_Position currentPosition,
                               tsp00_Byte *inputBuffer,
                               tsp00_Int4 bytesToParse,
                               const tsp00_Byte * &nextMatch,
                               teo15_Position &NextWritePosition,
                               teo15_Position &lastNonSpacePosition,
                               tsp00_Int4 &CycleCounter );

  void eo15FindCycleInfo( teo15_Position EndOfFilePosition,
                          tsp00_Int4     &CurrentCycleCounter,
                          teo15_Position &NextWritePosition,
                          tsp00_VfReturn_Param &ReturnStatus );

public:
  static bool eo15CheckIndex(const tsp00_Int4 FileIndex)
  {
#ifdef USE_DYNAMIC_MESSAGE_BUFFER
    return ( (FileIndex > 0)
          && (FileIndex <= NUMBER_OF_MESSAGE_BUFFERS)
          && (MESSAGE_BUFFER_PTR(FileIndex-1) != NULL) );
#else
    return ( (FileIndex > 0)
          && (FileIndex <= NUMBER_OF_MESSAGE_BUFFERS) );
#endif
  }

  ceo15_MessageFile(teo15MessageBufferSpace &Space)
    : m_Space(Space)
  {}

  void eo15Open( /* PTS 1113575 */
    const teo15_Position MaxFileSize,
    bool doNotInit,
    tsp00_VfReturn_Param &ReturnStatus );

#if !defined(_WIN32)
  // This is not not called reguarly... Only if reopen is needed after crash.
  // In such a case the old m_HostFileNo is invalid. We override it simply.
  void eo15Reopen( tsp00_VfReturn_Param &ReturnStatus )
  {
    if ( eo15IsUsed() )
    {
      // Reset refrence count
      m_Space.m_Used = 1;
#ifdef TEST
      // Test program must simulate 'close' done by the crash
      RTESys_IOClose ( m_Space.m_HostFileNo, ReturnStatus );
      m_Space.m_HostFileNo = INVALID_HANDLE_EO15;
#endif /* TEST */
      RTESys_IOChangeMode( m_Space.m_Filename.asCharp(), RTESys_IOReadWrite, ReturnStatus );
      if ( vf_ok != ReturnStatus )
      {
        eo15ShowLastOsError("ChangeMode(ReadWrite)", m_Space.m_Filename.asCharp() );
      }
#ifdef TEST
      TEST_PRINT1("ChangeMode(ReadWrite) for reopen of %s\n", m_Space.m_Filename.asCharp() )
#endif /* TEST */
      RTESys_IOOpen ( m_Space.m_HostFileNo,
        m_Space.m_Filename.asCharp(),
        RTESys_IOWriteOnly,
        false,
        DEFAULT_SECURITY_RTESYS_IO,
        ReturnStatus );
      if ( vf_ok != ReturnStatus )
      {
        eo15ShowLastOsError("ReOpen", m_Space.m_Filename.asCharp() );
      }
    }
  }
#endif /* WIN32 */

  // Writing tries to store in current buffer. If no more space, first it is checked
  // that the second buffer is not in use already. But if , it is forced to be flushed.
  // Then the m_BytesStored counter is copied to m_BytesToFlush and the current buffer
  // is switched. The new buffer is used to store the message.
  void eo15Write(const char *Message, tsp00_VfReturn_Param &ReturnStatus );
  /* Flush both buffers */
  void eo15Sync( tsp00_VfReturn_Param &ReturnStatus )
  {
    if ( eo15IsUsed() )
    {
TEST_PRINT("Flush first\n")
      eo15Flush( ReturnStatus);
      m_Space.m_BytesToFlush  = m_Space.m_BytesStored;
      m_Space.m_CurrentBuffer = 1 - m_Space.m_CurrentBuffer;
      m_Space.m_BytesStored   = 0;
TEST_PRINT("Flush second\n")
      eo15Flush( ReturnStatus);
    }
  }

  // PTS 1109999
  void eo15OpenAgain(const teo15_Position MaxFileSize,
                                          bool doNotInit,
                                          tsp00_VfReturn_Param &ReturnStatus );

  void eo15AddRef()
  {
    ++m_Space.m_Used;
  }

  bool eo15IsUsed() const
  {
    return ( m_Space.m_Used > 0 );
  }

  bool eo15Release()
  {
    --m_Space.m_Used;
    if ( m_Space.m_Used < 0 )
    {
      m_Space.m_Used = 0;
    }
    return (m_Space.m_Used == 0);
  }

  void eo15Close( tsp00_VfReturn_Param &ReturnStatus )
  {
    if ( eo15IsUsed() )
    {
      eo15Sync( ReturnStatus );
      if ( eo15Release() )
      {
        RTESys_IOClose( m_Space.m_HostFileNo , ReturnStatus );
        if ( vf_ok != ReturnStatus )
        {
          eo15ShowLastOsError("Close", m_Space.m_Filename.asCharp() );
        }
        RTESys_IOChangeMode( m_Space.m_Filename.asCharp(), RTESys_IOReadOnly, ReturnStatus );
        if ( vf_ok != ReturnStatus )
        {
          eo15ShowLastOsError("ChangeMode(ReadOnly)", m_Space.m_Filename.asCharp() );
        }
#ifdef TEST
      TEST_PRINT1("ChangeMode(ReadOnly) on %s\n", m_Space.m_Filename.asCharp() )
#endif /* TEST */
        m_Space.m_HostFileNo = INVALID_HANDLE_EO15;
      }
    }
  }

  void eo15Copy(const char *CopyFileName, tsp00_VfReturn_Param &ReturnStatus)
  {
    eo15Sync(ReturnStatus);
    RTESys_IOCopyFile( m_Space.m_Filename.asCharp(), CopyFileName, ReturnStatus, eo15ShowLastOsError);
  }

  void eo15Flush( tsp00_VfReturn_Param &ReturnStatus );

  bool eo15Lock( tsp00_VfReturn_Param &ReturnStatus );

  void eo15Unlock();

  bool eo15CompareFilename(const char *Filename) const
  {
    return ( eo15IsUsed()
      && (strcmp(m_Space.m_Filename.asCharp(), Filename) == 0) );
  }

  void eo15SetTerminating( )
  {
    m_Space.m_Terminating++;
  }
};

/*---------------------------------------------------------------------------*/
// Function: ceo15_MessageFile::eo15Expand
// Description: Expand file to given file size
//
// If file already exists only the extension of the file is filled with spaces
// If file is just created the complete file is filled with spaces
// If file was written with 'cycle' mode, the "last line in file" information
// is overwritten with overwritten automatically when reached...
//
// Arguments: CurrentEndOfFilePosition [in] Currently known size of file
//            ReturnStatus [out] if expand failed, set to vf_notok, else set to vf_ok
// Return value: true if expand wroked, false if expand failed
//
bool ceo15_MessageFile::eo15Expand( const teo15_Position CurrentEndOfFilePosition,
                                    tsp00_VfReturn_Param &ReturnStatus )
{
    ReturnStatus = vf_ok;

    if ( UNLIMITED_FILE_SIZE_EO15 == m_Space.m_MaxFileSize
      || !eo15_ExpandMessageFiles )
    {
        return true;
    }

    teo15_Position TargetPosition = CurrentEndOfFilePosition;
    teo15_Position CurrentPosition = 0; // dummy

    RTESys_IOSeek( m_Space.m_HostFileNo,
                   TargetPosition,
                   RTESys_IOSeekSet,
                   CurrentPosition,
                   ReturnStatus );
    if ( vf_ok != ReturnStatus )
    {
        eo15ShowLastOsError("Expand(seek)", m_Space.m_Filename.asCharp() );
        return false;
    }

    while ( TargetPosition < m_Space.m_MaxFileSize )
    {
        teo15_Position BytesWritten;
        teo15_Position BytesToWrite;

        BytesToWrite = m_Space.m_MaxFileSize - TargetPosition;

        if ( BytesToWrite > (teo15_Position)sizeof(teo15_Buffer) )
        {
            BytesToWrite = (teo15_Position)sizeof(teo15_Buffer);
        }

        RTESys_IOWrite( m_Space.m_HostFileNo,
                       &eo15_ExpandBuffer[sizeof(teo15_Buffer)-BytesToWrite],
                        BytesToWrite,
                        BytesWritten,
                        ReturnStatus );
        if ( vf_ok != ReturnStatus )
        {
            eo15ShowLastOsError("Expand(Write)", m_Space.m_Filename.asCharp() );
            return false;
        }

        TargetPosition += BytesWritten;
    }

    return true;
}

/*---------------------------------------------------------------------------*/
// Function: eo15GetCompleteEndLine
// Description: Find the end line with the current cycle information
// Return true only if complete line was found
// This routine uses nextMatch to identify what to do
//
// Initial it must be NULL or point to stringToMatch
//
// After string is matched, it points to zero byte at end of string to match
// In this mode, it tries to read the cycle counter (allowing leading blanks)
// After the CycleCounter is read the '\n' folling the cycle counter completes the
// scan.
//
// During parsing the lastNonSpacePosition is continously mode up to date.
//
// Whenever the matching failed, the nextMatch is reset to point to begin of stringToMatch
//
// Arguments: stringToMatch [in] the line to search
//            currentPosition [in] position of buffer in file
//            inputBuffer [in] the buffer read from current position
//            bytesToParse [in] number of bytes in buffer to parse
//            nextMatch [inout] the pointer for the next character to match
//                              if complete line was found it points to 0 byte
//            NextWritePosition [out] set only if a match start was found
//            lastNonSpacePosition [out] set only if a non space character was found
//            CycleCounter [out] set if line is completely parsed
//
// Return value: true if line was found (matchStartPosition and CycleCounter valid), false if not
bool ceo15_MessageFile::eo15GetCompleteEndLine( const tsp00_Byte *  stringToMatch,
                                                const teo15_Position currentPosition,
                                                tsp00_Byte *inputBuffer,
                                                tsp00_Int4 bytesToParse,
                                                const tsp00_Byte * &nextMatch,
                                                teo15_Position &NextWritePosition,
                                                teo15_Position &lastNonSpacePosition,
                                                tsp00_Int4 &CycleCounter )
{
    tsp00_Int4 bytesParsed = 0;

    if ( !nextMatch )
    {
        nextMatch = stringToMatch;
    }

    while ( bytesParsed < bytesToParse )
    {
        tsp00_Byte parsedByte = inputBuffer[bytesParsed];

        if ( ' ' != parsedByte )
        {
            lastNonSpacePosition = currentPosition + bytesParsed;
        }

        if ( 0 == *nextMatch )
        {
            switch( parsedByte )
            {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                if ( -1 == CycleCounter )
                {
                    CycleCounter = 0;
                }
                else
                {
                    CycleCounter *= 10;
                }
                CycleCounter += (parsedByte - '0');
                break;

            default:
                if ( -1 != CycleCounter )
                {
                    return true;
                }
                else
                {
                    nextMatch = stringToMatch;
                }
                break;
            }
        }

        if ( 0 != *nextMatch )
        {
            if ( parsedByte == *nextMatch )
            {
                if ( stringToMatch == nextMatch )
                {
                    NextWritePosition = currentPosition + bytesParsed;
                    CycleCounter = -1;
                }

                ++nextMatch;
            }
            else
            {
                nextMatch = stringToMatch;
            }
        }

        ++bytesParsed;
    }

    return false;
}

/*---------------------------------------------------------------------------*/
// Function: ceo15_MessageFile::eo15FindCycleInfo
// Description: Find the cycle information in current file
//
// If file already exists and was written in cycle mode, the content depends if
// a cycle was ever completed. After the first cycle is completed, the end of the
// file is marked, and the first line of file gets the current cycle information.
//
// The file is read blockwise using the two output buffers, which are unsued if called.
// Each block is scanned for the current write position and the header line is used to
// setup cycle count. If it is a cycle line, it got its special format, otherwise the
// cycle count is still zero.
//
// Arguments: EndOfFilePosition [in] Currently known size of file
//            CurrentCycleCounter [out] Number of cycles the file already was written
//            NextWritePosition [out] Position where the next write operation will start
//            ReturnStatus [out] returns vf_ok if cycle information completed, or I/O error
//
void ceo15_MessageFile::eo15FindCycleInfo( teo15_Position EndOfFilePosition,
                                           tsp00_Int4     &CurrentCycleCounter,
                                           teo15_Position &NextWritePosition,
                                           tsp00_VfReturn_Param &ReturnStatus )
{
    teo15_Position TargetPosition = 0;
    teo15_Position CurrentPosition; // dummy

    RTESys_IOSeek( m_Space.m_HostFileNo,
                   (teo15_Position)0,
                   RTESys_IOSeekSet,
                   CurrentPosition,
                   ReturnStatus );
    if ( vf_ok != ReturnStatus )
    {
        eo15ShowLastOsError("FindCycle(Seek0)", m_Space.m_Filename.asCharp() );
        return;
    }

    tsp00_Int4 CurrentBuffer = 0;
    const tsp00_Byte *nextMatch = 0;
    teo15_Position lastNonSpacePosition = 0;

    NextWritePosition = 0;

    for ( CurrentBuffer = 0; ; CurrentBuffer = 1 - CurrentBuffer )
    {
        teo15_Position BytesToParse = 0;
        tsp00_Byte *inputBuffer = (tsp00_Byte *)&((m_Space.m_Buffer[CurrentBuffer])[0]);

        RTESys_IORead( m_Space.m_HostFileNo,
                       inputBuffer,
                       sizeof(teo15_Buffer),
                       BytesToParse,
                       ReturnStatus );

        if ( vf_eof == ReturnStatus )
        {
            NextWritePosition = lastNonSpacePosition;
            CurrentCycleCounter = 0;
            ReturnStatus = vf_ok;
            break;
        }

        if ( vf_ok != ReturnStatus )
        {
            eo15ShowLastOsError("FindCycle(Read)", m_Space.m_Filename.asCharp() );
            break;
        }

        if ( eo15GetCompleteEndLine( (const SAPDB_Byte *)END_LINE_EO15,
                                     TargetPosition,
                                     inputBuffer,
                                     (tsp00_Int4)BytesToParse,
                                     nextMatch,
                                     NextWritePosition,
                                     lastNonSpacePosition,
                                     CurrentCycleCounter ) )
        {
            break;
        }

        TargetPosition += sizeof(teo15_Buffer);
    }
}

/*---------------------------------------------------------------------------*/

void ceo15_MessageFile::eo15Open(const teo15_Position MaxFileSize,
                                 bool doNotInit,
                                 tsp00_VfReturn_Param &ReturnStatus )
{
    ROUTINE_DBG_MEO60 (_T("ceo15_MessageFile::eo15Open"));

    SAPDB_MemFillNoCheck( &m_Space.m_Buffer[0], 0, 2 * sizeof(teo15_Buffer) );

    if ( MaxFileSize != UNLIMITED_FILE_SIZE_EO15 )
    {
        m_Space.m_MaxFileSize = ALIGN_EO00(MaxFileSize, MESSAGE_FILE_PAGE_MXEO15) + eo15_CycleOverhead;
    }
    else
    {
        m_Space.m_MaxFileSize = UNLIMITED_FILE_SIZE_EO15;
    }

    bool IsDirectory = false;
    bool DoesExist = false;

    eo15CheckExist( m_Space.m_Filename.asCharp(),
                    DoesExist,
                    IsDirectory,
                    ReturnStatus );
    if ( vf_ok == ReturnStatus )
    {
        if ( IsDirectory )
        {
            MSGD(( ERR_TRACEIO_OS_ERROR, "Open", m_Space.m_Filename.asCharp(), "found directory" ));

TEST_PRINT("eo15CheckExist found directory\n")
            ReturnStatus = vf_notok;
        }
    }
TEST_PRINT("eo15CheckExist returned\n")

    if ( vf_ok != ReturnStatus )
	{
        eo15ShowLastOsError("Open(CheckExistence)", m_Space.m_Filename.asCharp());
		return;
    }

    // If file exists and is forced to be initilized, it is removed first
    if ( !doNotInit && DoesExist )
    {
        RTESys_IOChangeMode( m_Space.m_Filename.asCharp(), RTESys_IOReadWrite, ReturnStatus );
#ifdef TEST
        TEST_PRINT1("ChangeMode(ReadWrite) for unlink of %s\n", m_Space.m_Filename.asCharp() )
#endif /* TEST */
        RTESys_IOUnlink( m_Space.m_Filename.asCharp(), ReturnStatus );
        if ( vf_ok != ReturnStatus )
        {
        	eo15ShowLastOsError( "Open(Unlink)",
                                      m_Space.m_Filename.asCharp());
        }
        DoesExist = false;
    }

    // initialize file if it does not exist or initilization is wanted
    bool initializeFile = ( !doNotInit || !DoesExist );

    teo15_Position EndOfFilePosition = 0;
    teo15_Position NextWritePosition = 0;
    tsp00_Int4     CycleCounter = 0;

    if ( DoesExist )
    {
        RTESys_IOChangeMode( m_Space.m_Filename.asCharp(), RTESys_IOReadWrite, ReturnStatus );
        if ( vf_ok != ReturnStatus )
        {
            eo15ShowLastOsError("ChangeMode(ReadWrite) for open of %s", m_Space.m_Filename.asCharp() );
        }
#ifdef TEST
        TEST_PRINT1("ChangeMode(ReadWrite) on %s\n", m_Space.m_Filename.asCharp() )
#endif /* TEST */
    }

    RTESys_IOOpen (  m_Space.m_HostFileNo,
                     m_Space.m_Filename.asCharp(),
                     RTESys_IOReadWrite,
                     initializeFile,
                     DEFAULT_SECURITY_RTESYS_IO,
                     ReturnStatus );
    if ( vf_ok != ReturnStatus )
    {
		eo15ShowLastOsError("Open", m_Space.m_Filename.asCharp());
        return;
    }

    if ( !initializeFile )
    {
        // Find current file size
        RTESys_IOSeek( m_Space.m_HostFileNo,
                       (teo15_Position)0,
                       RTESys_IOSeekEnd,
                       EndOfFilePosition,
                       ReturnStatus );
        if ( vf_ok != ReturnStatus )
        {
            eo15ShowLastOsError("Open(SeekEnd)", m_Space.m_Filename.asCharp());
        }
        else
        {
            if ( UNLIMITED_FILE_SIZE_EO15  == m_Space.m_MaxFileSize )
            {
                NextWritePosition = EndOfFilePosition;
            }
            else
            {
                if ( EndOfFilePosition > m_Space.m_MaxFileSize )
                {
/*                    MSGD(( WRN_TRACEIO_CANNOT_SHRINK,
                           EndOfFilePosition,
                           MaxFileSize,
                           m_Space.m_Filename.asCharp() ));
 */
                    RTESys_IOClose(m_Space.m_HostFileNo, ReturnStatus);
                    if ( vf_ok != ReturnStatus )
                    {
        				eo15ShowLastOsError( "Open(CloseShrink)",
                                                  m_Space.m_Filename.asCharp());
                        m_Space.m_HostFileNo = INVALID_HANDLE_EO15;
                        return;
                    }

                    this->eo15Open(MaxFileSize, false, ReturnStatus );
                    return;
                }

                eo15FindCycleInfo( EndOfFilePosition,
                                   CycleCounter,
                                   NextWritePosition,
                                   ReturnStatus );
            }
        }
    }

    if ( vf_ok == ReturnStatus
      && eo15Expand( EndOfFilePosition, ReturnStatus ) )
    {
        eo15AddRef();
        m_Space.m_CurrentBuffer = 0;
        m_Space.m_BytesStored = 0;
        m_Space.m_BytesToFlush = 0;
        m_Space.m_NextWritePosition = NextWritePosition;
        m_Space.m_CycleCounter = CycleCounter;
        m_Space.m_TotalBytesWritten = 0;
    }
    else
    {
        tsp00_VfReturn_Param CloseReturnStatus = vf_ok;

        RTESys_IOClose( m_Space.m_HostFileNo,
                        CloseReturnStatus);
        if ( vf_ok != CloseReturnStatus )
        {
        	eo15ShowLastOsError( "Open(Close)",
                                      m_Space.m_Filename.asCharp());
        }
        m_Space.m_HostFileNo = INVALID_HANDLE_EO15;
    }
}

/*---------------------------------------------------------------------------*/

void ceo15_MessageFile::eo15OpenAgain(const teo15_Position MaxFileSize,
                                 bool doNotInit,
                                 tsp00_VfReturn_Param &ReturnStatus )
{
    ROUTINE_DBG_MEO60 (_T("ceo15_MessageFile::eo15OpenAgain"));

    ReturnStatus = vf_ok;
    teo15_Position alignedMaxFileSize;
    if ( MaxFileSize != UNLIMITED_FILE_SIZE_EO15 )
    {
        alignedMaxFileSize = ALIGN_EO00(MaxFileSize, MESSAGE_FILE_PAGE_MXEO15) + eo15_CycleOverhead;
    }
    else
    {
        alignedMaxFileSize = UNLIMITED_FILE_SIZE_EO15;
    }

    if ( (m_Space.m_MaxFileSize == alignedMaxFileSize) && doNotInit )
    {
        eo15AddRef();
        return;
    }

    RTESys_IOClose( m_Space.m_HostFileNo, ReturnStatus );
    m_Space.m_HostFileNo = INVALID_HANDLE_EO15;
    m_Space.m_MaxFileSize = alignedMaxFileSize;
    RTESys_IOChangeMode( m_Space.m_Filename.asCharp(), RTESys_IOReadWrite, ReturnStatus );
#ifdef TEST
    TEST_PRINT1("ChangeMode(ReadWrite) for unlink(for reopen) of %s\n", m_Space.m_Filename.asCharp() )
#endif /* TEST */
    RTESys_IOUnlink( &m_Space.m_Filename[0], ReturnStatus );
    if ( vf_ok != ReturnStatus )
    {
      eo15ShowLastOsError("Unlink", &m_Space.m_Filename[0] );
    }

    SAPDB_MemFillNoCheck( &m_Space.m_Buffer[0], 0, 2 * sizeof(teo15_Buffer) );

    if ( vf_ok == ReturnStatus )
    {
        RTESys_IOOpen ( m_Space.m_HostFileNo,
                        &m_Space.m_Filename[0],
                        RTESys_IOWriteOnly,
                        true,
                        DEFAULT_SECURITY_RTESYS_IO,
                        ReturnStatus );
        if ( vf_ok == ReturnStatus )
        {
            if ( eo15Expand( 0, ReturnStatus ) )
            {
                eo15AddRef();
                m_Space.m_CurrentBuffer = 0;
                m_Space.m_BytesStored = 0;
                m_Space.m_BytesToFlush = 0;
                m_Space.m_NextWritePosition = 0;
                m_Space.m_CycleCounter = 0;
                m_Space.m_TotalBytesWritten = 0;
            }
            else
            {
                tsp00_VfReturn_Param CloseReturnStatus = vf_ok;

                RTESys_IOClose( m_Space.m_HostFileNo,
                                CloseReturnStatus );
                if ( vf_ok != CloseReturnStatus )
                {
        	        eo15ShowLastOsError( "OpenAgain(Close)",
                                              m_Space.m_Filename.asCharp());
                }
                m_Space.m_HostFileNo = INVALID_HANDLE_EO15;
            }
        }
        else
        {
            eo15ShowLastOsError("OpenAgain", &m_Space.m_Filename[0]);
        }
    }
}

/*===========================================================================*
*  PUBLIC FUNCTIONS (CODE)                                                  *
*===========================================================================*/

/*---------------------------------------------------------------------------*/
//
// Initialize the message buffer space due the number of preconfigured buffers
//
externC void eo15InitMessageBufferSpace(const tsp00_Int4 NumberOfBuffers,
                                        teo15MessageBufferSpace *BufferArray)
{
  ROUTINE_DBG_MEO60 (_T("eo15InitMessageBufferSpace"));
/* PTS 1106793 */

  MSGD(( INFO_TRACEIO_MAX_MESSAGE_FILES, NumberOfBuffers ));

  /* PTS 1109715 */
  if ( NumberOfBuffers <= 0 )
  {
#ifdef USE_DYNAMIC_MESSAGE_BUFFER
    eo15_BufferMap = NULL;
    eo15_BufferMapSize = 0;
#else
    eo15_BufferArray = NULL;
#endif
    eo15_NumberOfBuffers = 0;
    MSGD(( INFO_TRACEIO_DISABLED ));
    return;
  }

  {
        SAPDB_Char *paramValue;
        SAPDB_Char *errText;
        if ( RTEConf_ParameterAccessGetString((SAPDB_UTF8 *)PAN_EXPAND_COM_TRACE, &paramValue, &errText) )
        {
            eo15_ExpandMessageFiles = ( strcmp("YES", paramValue) == 0 );
        }
        else
        {
            eo15_ExpandMessageFiles = false;
        }
  }

#ifdef USE_DYNAMIC_MESSAGE_BUFFER
  sqlcreatemutex( &eo15_BufferMapMutex );

  if ( NO_ERROR_EO001 != ALLOC_MEM_EO57 ((void **)&eo15_BufferMap,
                    sizeof(teo15MessageBufferSpace *) * MAP_INCREASE_AMOUNT_EO15 ) )
  {
    eo15_BufferMap = NULL;
    eo15_BufferMapSize = 0;
    eo15_NumberOfBuffers = 0;
    MSGD(( INFO_TRACEIO_DISABLED_NOMEM ));
    return;
  }
  eo15_BufferMapSize = MAP_INCREASE_AMOUNT_EO15;
  eo15_NumberOfBuffers = NumberOfBuffers;
  SAPDB_MemFillNoCheck(eo15_BufferMap, 0, sizeof(teo15MessageBufferSpace *) * MAP_INCREASE_AMOUNT_EO15);
#else

  {
    int i;

    eo15_BufferArray     = BufferArray;
    eo15_NumberOfBuffers = NumberOfBuffers;

    for ( i = 0; i < eo15_NumberOfBuffers; i++ )
    {
      eo15_BufferArray[i].m_Used = 0;
      eo15_BufferArray[i].m_Terminating = 0;
      eo15_BufferArray[i].m_BusyCount = 0;
      sqlcreatemutex( &(eo15_BufferArray[i].m_Mutex) );
    }
  }

#endif

  SAPDBErr_MessageList   messageList;

  if ( !Worker::BeginThread( WORKER_STACK_EO15, messageList ))
      RTE_Crash(messageList);

  MSGD(( INFO_TRACEIO_ENABLED ));

  SAPDB_MemFillNoCheck(eo15_ClearBuffer, ' ', sizeof(teo15_Buffer));
  SAPDB_MemFillNoCheck(eo15_ExpandBuffer, ' ', sizeof(teo15_Buffer));
#define EXPAND_LINE_LENGTH 128
  for ( int iBuffer = (EXPAND_LINE_LENGTH-1); iBuffer < (int)sizeof(teo15_Buffer); iBuffer += EXPAND_LINE_LENGTH )
  {
      eo15_ExpandBuffer[iBuffer] = '\n';
  }

  eo15_CycleOverhead = (SAPDB_Int4)(strlen(CLR_LINE_EO15) + strlen(FIRST_LINE_EO15) + 14); // 12 digits + line delimiter
}
/*---------------------------------------------------------------------------*/
externC void eo15CompleteMessageFiles( const tsp00_Int4, teo15MessageBufferSpace *)
{
  ROUTINE_DBG_MEO60 (_T("eo15CompleteMessageFiles"));

  if ( eo15_NumberOfBuffers <= 0 )
  {
    return;
  }

  //
  // Called after crash of kernel! Ignores any locking!!!!
  //
  tsp00_VfReturn_Param ReturnStatus;

#ifdef USE_DYNAMIC_MESSAGE_BUFFER
  ceo15_LockMutex Lock( eo15_BufferMapMutex );
#endif

  teo15MessageBufferSpace *ActiveBuffer;
  for ( int i = 0; (i < NUMBER_OF_MESSAGE_BUFFERS)
               && ((ActiveBuffer = MESSAGE_BUFFER_PTR(i)) != NULL) ; i++ )
  {
    ceo15_MessageFile TheMessageFile( *ActiveBuffer );

    TheMessageFile.eo15SetTerminating();
#if !defined(_WIN32)
    // For WIN32 reopen is never needed...
    TheMessageFile.eo15Reopen(ReturnStatus);
#endif
    TheMessageFile.eo15Close(ReturnStatus); // This automatically flushes
  }
}
/*---------------------------------------------------------------------------*/
externC void eo15FlushMessageFiles( void )
{
  ROUTINE_DBG_MEO60 (_T("eo15FlushMessageFiles"));

  if ( eo15_NumberOfBuffers <= 0 )
  {
    return;
  }

  //
  // Called after a successfull SHUTDOWN. Resets any locking and flushes by doing forgotten close.
  //
  tsp00_VfReturn_Param ReturnStatus;

#ifdef USE_DYNAMIC_MESSAGE_BUFFER
  ceo15_LockMutex Lock( eo15_BufferMapMutex );
#endif

  teo15MessageBufferSpace *ActiveBuffer;
  for ( int i = 0; (i < NUMBER_OF_MESSAGE_BUFFERS)
               && ((ActiveBuffer = MESSAGE_BUFFER_PTR(i)) != NULL) ; i++ )
  {
    ActiveBuffer->m_Terminating = 0;
    ActiveBuffer->m_BusyCount = 0;

    sqlcreatemutex( &(ActiveBuffer->m_Mutex) );

    ceo15_MessageFile TheMessageFile( *ActiveBuffer );

    TheMessageFile.eo15Close(ReturnStatus);
  }
}
/*---------------------------------------------------------------------------*/
externC tsp00_Int4 eo15OpenMessageFile( const tsp00_Char *    Filename,
                                       const teo15_Position  MaxFileSize,
                                       bool doNotInit,
                                       tsp00_VfReturn_Param &ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("eo15OpenMessageFile"));

  tsp00_Int4 FileIndex;
  bool            isFreeSlot;

  eo15_SearchAndLockMessageFile( Filename, FileIndex, isFreeSlot, ReturnStatus );
  if ( vf_ok != ReturnStatus )
  {
    return INVALID_BUFFER_EO15;
  }

  if ( !isFreeSlot )
  {
    ceo15_MessageFile TheMessageFile( *MESSAGE_BUFFER_PTR(FileIndex-1) );
    TheMessageFile.eo15OpenAgain(MaxFileSize, doNotInit, ReturnStatus);
    TheMessageFile.eo15Unlock();
TEST_PRINT("Ok. Reopened\n")
    return FileIndex;
  }

  if ( INVALID_BUFFER_EO15 != FileIndex )
  {
    /* Got a new buffer place */
    ceo15_MessageFile TheMessageFile( *MESSAGE_BUFFER_PTR(FileIndex-1) );

    TheMessageFile.eo15Open(MaxFileSize, doNotInit, ReturnStatus);
    if ( vf_ok == ReturnStatus )
    {
      TheMessageFile.eo15Unlock();
      return FileIndex;
    }
    else
    {
      MSGD((ERR_TRACEIO_OPEN_FAILED, Filename, "Check OS error code"));
      MESSAGE_BUFFER_PTR(FileIndex-1)->m_Filename.rawAssign("");
      TheMessageFile.eo15Unlock();
      return INVALID_BUFFER_EO15;
    }
  }
  else
  {
    MSGD((ERR_TRACEIO_OPEN_FAILED, Filename, "No more internal buffer"));
  }
  ReturnStatus = vf_notok;
  return INVALID_BUFFER_EO15;
}
/*---------------------------------------------------------------------------*/
// PTS 1105164
externC tsp00_Int4 eo15GetMessageFileIndex( const tsp00_Char *    Filename )
{
  ROUTINE_DBG_MEO60 (_T("eo15GetMessageFileIndex"));

  tsp00_VfReturn_Param ReturnStatus = vf_ok;
  tsp00_Int4 FileIndex = INVALID_BUFFER_EO15;

  eo15_MapAndLockMessageFile( Filename, FileIndex, ReturnStatus );

  if ( vf_ok != ReturnStatus )
  {
    MSGD((ERR_TRACEIO_GET_INDEX_FAILED, Filename == NULL ? "<NULL>" : Filename, "invalid file name"));
  }
  else
  {
      if ( INVALID_BUFFER_EO15 == FileIndex )
      {
           ceo15_MessageFile TheMessageFile( *MESSAGE_BUFFER_PTR(FileIndex) );

           TheMessageFile.eo15Unlock();
      }
   }
   return FileIndex;
}

/*---------------------------------------------------------------------------*/

// PTS 1105164
externC void eo15CheckExist( const tsp00_Char * Filename,
                            bool  &DoesExist,
                            bool  &IsDirectory,
                            tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("eo15CheckExist"));

#if defined(_WIN32)
  DWORD Attributes = GetFileAttributes(Filename);

  DoesExist = ( Attributes != 0xFFFFFFFF );

  if ( DoesExist )
  {
    ReturnStatus = vf_ok;
    IsDirectory = ( (Attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY );
  }
  else
  {
    ReturnStatus = (GetLastError() == ERROR_FILE_NOT_FOUND ? vf_ok : vf_notok);
    IsDirectory = false;
  }
#else
  struct stat testBuf;

  // stat follows symbolic links...
  DoesExist = ( stat( Filename, &testBuf ) == 0 );

  if ( DoesExist )
  {
    ReturnStatus = vf_ok;
    IsDirectory = ( (testBuf.st_mode & S_IFDIR) == S_IFDIR );
  }
  else
  {
    ReturnStatus = ( errno == ENOENT ? vf_ok : vf_notok );
    IsDirectory = false;
  }
#endif

  return;
}

/*---------------------------------------------------------------------------*/

externC void eo15WriteMessageFile( const tsp00_Int4  FileIndex,
                                  const tsp00_Char *Message,
                                  tsp00_VfReturn_Param &ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("eo15WriteMessageFile"));

  if ( !ceo15_MessageFile::eo15CheckIndex(FileIndex) )
  {
    MSGD((ERR_TRACEIO_BAD_FILEINDEX, "Write", FileIndex ));
    ReturnStatus = vf_notok;
    return;
  }

  ceo15_MessageFile TheMessageFile( *MESSAGE_BUFFER_PTR(FileIndex-1) );

  if ( !TheMessageFile.eo15Lock(ReturnStatus) )
  {
    return;
  }

  if ( !TheMessageFile.eo15IsUsed() )
  {
    MSGD((WRN_TRACEIO_BAD_FILEINDEX, "Write(UnUsed)", FileIndex ));
  }
  else
  {
    TheMessageFile.eo15Write(Message, ReturnStatus);
  }

  TheMessageFile.eo15Unlock();
}
/*---------------------------------------------------------------------------*/
// PTS 1105164
externC void eo15SyncMessageFile( const tsp00_Int4 FileIndex,
                                 tsp00_VfReturn_Param &ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("eo15SyncMessageFile"));

  if ( !ceo15_MessageFile::eo15CheckIndex(FileIndex) )
  {
    MSGD((ERR_TRACEIO_BAD_FILEINDEX, "Sync", FileIndex ));
    ReturnStatus = vf_notok;
    return;
  }

  ceo15_MessageFile TheMessageFile( *MESSAGE_BUFFER_PTR(FileIndex-1) );

  if ( !TheMessageFile.eo15Lock(ReturnStatus) )
  {
    return;
  }

  if ( !TheMessageFile.eo15IsUsed() )
  {
    MSGD((WRN_TRACEIO_BAD_FILEINDEX, "Sync(UnUsed)", FileIndex ));
  }
  else
  {
    TheMessageFile.eo15Sync(ReturnStatus);
  }

  TheMessageFile.eo15Unlock();
}
/*---------------------------------------------------------------------------*/
// Alternative Interface using Filename instead of FileIndex
// Locking prevents the index from becoming invalid in between, but make reuse of
// other interface function impossible.
externC void eo15SyncNamedMessageFile( const tsp00_Char *Filename,
                                      tsp00_VfReturn_Param &ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("eo15SyncNamedMessageFile"));

  tsp00_Int4 FileIndex;

  eo15_MapAndLockMessageFile( Filename, FileIndex, ReturnStatus );

  if ( vf_ok != ReturnStatus )
  {
    MSGD((ERR_TRACEIO_CHECK_FILENAME, Filename == NULL ? "<NULL>" : Filename, "not opened"));
    return;
  }

  if ( !ceo15_MessageFile::eo15CheckIndex(FileIndex) )
  {
    MSGD((ERR_TRACEIO_BAD_FILEINDEX, "SyncNamed", FileIndex ));
    ReturnStatus = vf_notok;
    return;
  }

  ceo15_MessageFile TheMessageFile( *MESSAGE_BUFFER_PTR(FileIndex-1) );

  if ( !TheMessageFile.eo15IsUsed() )
  {
    MSGD((WRN_TRACEIO_BAD_FILEINDEX, "SyncNamed(UnUsed)", FileIndex ));
  }
  else
  {
    TheMessageFile.eo15Sync(ReturnStatus);
  }

  TheMessageFile.eo15Unlock();
}
/*---------------------------------------------------------------------------*/
externC void eo15CloseMessageFile( const tsp00_Int4 FileIndex,
                                  tsp00_VfReturn_Param &ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("eo15CloseMessageFile"));

  if ( !ceo15_MessageFile::eo15CheckIndex(FileIndex) )
  {
    MSGD((WRN_TRACEIO_BAD_FILEINDEX, "Close", FileIndex ));
    ReturnStatus = vf_notok;
    return;
  }

  ceo15_MessageFile TheMessageFile( *MESSAGE_BUFFER_PTR(FileIndex-1) );

  if ( !TheMessageFile.eo15Lock(ReturnStatus) )
  {
    return;
  }

  if ( !TheMessageFile.eo15IsUsed() )
  {
    MSGD((WRN_TRACEIO_BAD_FILEINDEX, "Close(UnUsed)", FileIndex ));
  }
  else
  {
    TheMessageFile.eo15Close(ReturnStatus);
  }

  TheMessageFile.eo15Unlock();
}
/*---------------------------------------------------------------------------*/
externC void eo15CopyMessageFile( const tsp00_Int4 FileIndex,
                                 const char *CopyFileName,
                                 tsp00_VfReturn_Param &ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("eo15CopyMessageFile"));

  if ( !ceo15_MessageFile::eo15CheckIndex(FileIndex) )
  {
    MSGD((ERR_TRACEIO_BAD_FILEINDEX, "CopyFile", FileIndex ));
    ReturnStatus = vf_notok;
    return;
  }

  ceo15_MessageFile TheMessageFile( *MESSAGE_BUFFER_PTR(FileIndex-1) );

  if ( !TheMessageFile.eo15Lock(ReturnStatus) )
  {
    return;
  }

  if ( !TheMessageFile.eo15IsUsed() )
  {
    MSGD((ERR_TRACEIO_BAD_FILEINDEX, "CopyFile(UnUsed)", FileIndex ));
    ReturnStatus = vf_notok;
  }
  else
  {
    TheMessageFile.eo15Copy(CopyFileName, ReturnStatus);
  }

  TheMessageFile.eo15Unlock();
}
/*---------------------------------------------------------------------------*/
// PTS 1105164
externC void       eo15RemoveMessageFile( const tsp00_Char *RemoveFileName,
                                          tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("eo15RemoveMessageFile"));

  tsp00_Int4 FileIndex;
  bool isFreeSlot = true;

  eo15_SearchAndLockMessageFile( RemoveFileName, FileIndex, isFreeSlot, ReturnStatus );
  if ( vf_ok == ReturnStatus )
  {
      if ( isFreeSlot )
      {
           RTESys_IOChangeMode( RemoveFileName, RTESys_IOReadWrite, ReturnStatus );
           RTESys_IOUnlink(RemoveFileName, ReturnStatus);
           if ( vf_ok != ReturnStatus )
           {
             eo15ShowLastOsError("Unlink", RemoveFileName );
           }
      }
      else
      {
         MSGD((ERR_TRACEIO_BAD_FILEINDEX, "RemoveFile", FileIndex ));
         ReturnStatus = vf_notok;
      }

      if ( isFreeSlot )
      {
            /* PTS 1113575 */
          MESSAGE_BUFFER_PTR(FileIndex-1)->m_Filename.rawAssign("");
      }

      ceo15_MessageFile TheMessageFile( *MESSAGE_BUFFER_PTR(FileIndex-1) );

      TheMessageFile.eo15Unlock();
  }
}
/*===========================================================================*
*  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
*===========================================================================*/
void ceo15_MessageFile::eo15Write(const char *Message, tsp00_VfReturn_Param &ReturnStatus )
{
    ROUTINE_DBG_MEO60 (_T("ceo15_MessageFile::eo15Write"));

    tsp00_Int4 LengthInBytes = (tsp00_Int4)strlen(Message);

    if ( m_Space.m_MaxFileSize != UNLIMITED_FILE_SIZE_EO15 )
    {
        if ( (teo15_Position)( m_Space.m_NextWritePosition
          + m_Space.m_BytesStored
          + LengthInBytes
          + strlen(CLR_LINE_EO15))
          >= m_Space.m_MaxFileSize )
        {
            if ( m_Space.m_BytesToFlush > 0 )
            {
                // Workerthread failed to do its job, so we do it... Otherwise we would be blocked anyhow
                TEST_PRINT("Doing worker threads job...\n")
                eo15Flush( ReturnStatus );
            }
            m_Space.m_BytesToFlush = m_Space.m_BytesStored;
            m_Space.m_CurrentBuffer = 1 - m_Space.m_CurrentBuffer;
            m_Space.m_BytesStored = 0;
        }
    }

    if ( (m_Space.m_BytesStored + LengthInBytes) <= MESSAGE_FILE_PAGE_MXEO15 )
    {
        SAPDB_MemCopyNoCheck( &(m_Space.m_Buffer[m_Space.m_CurrentBuffer][m_Space.m_BytesStored]), Message, LengthInBytes );
        m_Space.m_BytesStored += LengthInBytes;
    }
    else
    {
        do {
            if ( m_Space.m_BytesToFlush > 0 )
            {
                // Workerthread failed to do its job, so we do it... Otherwise we would be blocked anyhow
                TEST_PRINT("Doing worker threads job...\n")
                eo15Flush( ReturnStatus );
            }

            m_Space.m_BytesToFlush = m_Space.m_BytesStored;
            m_Space.m_CurrentBuffer = 1 - m_Space.m_CurrentBuffer;
            if ( LengthInBytes > MESSAGE_FILE_PAGE_MXEO15 )
            {
                SAPDB_MemCopyNoCheck( &(m_Space.m_Buffer[m_Space.m_CurrentBuffer][0]), Message, MESSAGE_FILE_PAGE_MXEO15 );
                m_Space.m_BytesStored  = MESSAGE_FILE_PAGE_MXEO15;
                Message       += MESSAGE_FILE_PAGE_MXEO15;
                LengthInBytes -= MESSAGE_FILE_PAGE_MXEO15;
            }
            else
            {
                SAPDB_MemCopyNoCheck( &(m_Space.m_Buffer[m_Space.m_CurrentBuffer][0]), Message, LengthInBytes );
                m_Space.m_BytesStored  = LengthInBytes;
                LengthInBytes = 0;
            }

            SAPDBErr_MessageList   messageList;

            if (!Worker::Continue( messageList ) )
                RTE_Crash(messageList);

        } while ( LengthInBytes != 0 );
    }
}
/*---------------------------------------------------------------------------*/
void ceo15_MessageFile::eo15Flush( tsp00_VfReturn_Param &ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("ceo15_MessageFile::eo15Flush"));

  if ( m_Space.m_BytesToFlush > 0 )
  {
//    TEST_PRINT1("Flushing %ld bytes...\n", m_Space.m_BytesToFlush)
      if ( m_Space.m_MaxFileSize != UNLIMITED_FILE_SIZE_EO15 )
      {
        char Cycle[16];
        teo15_Position BytesWritten; // write result
        teo15_Position NextPosition; // seek result

        if ( (teo15_Position)(m_Space.m_BytesToFlush
          + m_Space.m_NextWritePosition
          + strlen(CLR_LINE_EO15))
          >= m_Space.m_MaxFileSize )
        {
          RTESys_IOSeek( m_Space.m_HostFileNo,
            m_Space.m_NextWritePosition,
            RTESys_IOSeekSet,
            NextPosition,
            ReturnStatus );
          if ( ReturnStatus != vf_ok )
          {
            eo15ShowLastOsError("Flush(1st Seek)", m_Space.m_Filename.asCharp() );
            return;
          }

          RTESys_IOWrite( m_Space.m_HostFileNo,
            CLR_LINE_EO15,
              (tRTESys_IOPosition)strlen(CLR_LINE_EO15),
            BytesWritten,
            ReturnStatus );
          if ( ReturnStatus != vf_ok )
          {
            eo15ShowLastOsError("Flush(Write CLR)", m_Space.m_Filename.asCharp() );
            return;
          }

		  RTESys_IOWrite( m_Space.m_HostFileNo,
			  eo15_ClearBuffer,
			  m_Space.m_MaxFileSize - m_Space.m_NextWritePosition - strlen(CLR_LINE_EO15),
			  BytesWritten,
			  ReturnStatus );
          if ( ReturnStatus != vf_ok )
          {
            eo15ShowLastOsError("Flush(Write SPACES)", m_Space.m_Filename.asCharp() );
            return;
          }

          RTESys_IOSeek( m_Space.m_HostFileNo,
            (teo15_Position)0,
            RTESys_IOSeekSet,
            NextPosition,
            ReturnStatus );
          if ( ReturnStatus != vf_ok )
          {
            eo15ShowLastOsError("Flush(2nd Seek)", m_Space.m_Filename.asCharp() );
            return;
          }

          RTESys_IOWrite( m_Space.m_HostFileNo,
            FIRST_LINE_EO15,
            (tRTESys_IOPosition)strlen(FIRST_LINE_EO15),
            BytesWritten,
            ReturnStatus );
          if ( ReturnStatus != vf_ok )
          {
            eo15ShowLastOsError("Flush(Write FIRST)", m_Space.m_Filename.asCharp() );
            return;
          }

          m_Space.m_CycleCounter++;
          sprintf(Cycle, "%-12d\n", m_Space.m_CycleCounter);

          RTESys_IOWrite( m_Space.m_HostFileNo,
            Cycle,
            (tRTESys_IOPosition)strlen(Cycle),
            BytesWritten,
            ReturnStatus );
          if ( ReturnStatus != vf_ok )
          {
            eo15ShowLastOsError("Flush(Write Cycle)", m_Space.m_Filename.asCharp() );
            return;
          }

          m_Space.m_NextWritePosition = (teo15_Position)(strlen(FIRST_LINE_EO15) + strlen(Cycle));
        }
        else
        {
          RTESys_IOSeek( m_Space.m_HostFileNo,
            m_Space.m_NextWritePosition,
            RTESys_IOSeekSet,
            NextPosition,
            ReturnStatus );
          if ( ReturnStatus != vf_ok )
          {
            eo15ShowLastOsError("Flush(1st Seek)", m_Space.m_Filename.asCharp() );
            return;
          }
        }

        if ( m_Space.m_Terminating == 1
          && m_Space.m_BusyCount != 0 )
        {
          char EndOfLine[3];
          sprintf(EndOfLine, "\n");

          RTESys_IOWrite( m_Space.m_HostFileNo,
            CRASH_MARK_EO15,
            (tRTESys_IOPosition)strlen(CRASH_MARK_EO15),
            BytesWritten,
            ReturnStatus );
          if ( ReturnStatus != vf_ok )
          {
            eo15ShowLastOsError("Flush(Write Mark)", m_Space.m_Filename.asCharp() );
            return;
          }
          RTESys_IOWrite( m_Space.m_HostFileNo,
            EndOfLine,
            (tRTESys_IOPosition)strlen(EndOfLine),
            BytesWritten,
            ReturnStatus );
          if ( ReturnStatus != vf_ok )
          {
            eo15ShowLastOsError("Flush(Write EOL)", m_Space.m_Filename.asCharp() );
            return;
          }
          /* Crash mark only written once!!! */
          ++(m_Space.m_Terminating);
          m_Space.m_NextWritePosition += (teo15_Position)(strlen(CRASH_MARK_EO15) + strlen(EndOfLine));
        }

        RTESys_IOWrite( m_Space.m_HostFileNo,
          m_Space.m_Buffer[1 - m_Space.m_CurrentBuffer],
          m_Space.m_BytesToFlush,
          BytesWritten,
          ReturnStatus );
        if ( ReturnStatus != vf_ok )
        {
          eo15ShowLastOsError("Flush(Write Buffer)", m_Space.m_Filename.asCharp() );
          return;
        }

        m_Space.m_TotalBytesWritten += BytesWritten;
        m_Space.m_NextWritePosition += BytesWritten;

        RTESys_IOWrite( m_Space.m_HostFileNo,
          END_LINE_EO15,
          (tRTESys_IOPosition)strlen(END_LINE_EO15),
          BytesWritten,
          ReturnStatus );
        if ( ReturnStatus != vf_ok )
        {
          eo15ShowLastOsError("Flush(Write END)", m_Space.m_Filename.asCharp() );
          return;
        }

        sprintf(Cycle, "%-12d", m_Space.m_CycleCounter);

        RTESys_IOWrite( m_Space.m_HostFileNo,
          Cycle,
          (tRTESys_IOPosition)strlen(Cycle),
          BytesWritten,
          ReturnStatus );
        if ( ReturnStatus != vf_ok )
        {
          eo15ShowLastOsError("Flush(Write Cycle)", m_Space.m_Filename.asCharp() );
          return;
        }
      }
      else
      {
    TEST_PRINT("File unlimited\n")
          teo15_Position BytesWritten; // write result

          if ( m_Space.m_Terminating == 1
            && m_Space.m_BusyCount != 0 )
          {
    TEST_PRINT("Terminating\n")
            char EndOfLine[3];
            sprintf(EndOfLine, "\n");

            RTESys_IOWrite( m_Space.m_HostFileNo,
              CRASH_MARK_EO15,
              (tRTESys_IOPosition)strlen(CRASH_MARK_EO15),
              BytesWritten,
              ReturnStatus );
            if ( ReturnStatus != vf_ok )
            {
              eo15ShowLastOsError("Flush(Write Mark)", m_Space.m_Filename.asCharp() );
              return;
            }

            RTESys_IOWrite( m_Space.m_HostFileNo,
              EndOfLine,
              (tRTESys_IOPosition)strlen(EndOfLine),
              BytesWritten,
              ReturnStatus );
            if ( ReturnStatus != vf_ok )
            {
              eo15ShowLastOsError("Flush(Write EOL)", m_Space.m_Filename.asCharp() );
              return;
            }
            /* Crash mark only written once!!! */
            ++(m_Space.m_Terminating);
          }

    TEST_PRINT1("Bytes to flush: 0x%lx\n", (long)m_Space.m_BytesToFlush)
    TEST_PRINT1("Write from 0x%lx\n", &(m_Space.m_Buffer[1- m_Space.m_CurrentBuffer]))
    TEST_PRINT1("Writing '%29.29s'\n", &(m_Space.m_Buffer[1- m_Space.m_CurrentBuffer]))
          RTESys_IOWrite( m_Space.m_HostFileNo,
            m_Space.m_Buffer[1 - m_Space.m_CurrentBuffer],
            m_Space.m_BytesToFlush,
            BytesWritten,
            ReturnStatus );
          if ( ReturnStatus != vf_ok )
          {
            eo15ShowLastOsError("Flush(Write Buffer)", m_Space.m_Filename.asCharp() );
            return;
          }

          m_Space.m_TotalBytesWritten += BytesWritten;
          m_Space.m_NextWritePosition += BytesWritten;
        }

        m_Space.m_BytesToFlush = 0;
    }
    else
    {
//      TEST_PRINT("Flushing not needed...\n")
    }
}

/*---------------------------------------------------------------------------*/
bool ceo15_MessageFile::eo15Lock( tsp00_VfReturn_Param &ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("ceo15_MessageFile::eo15Lock"));

  if ( m_Space.m_Terminating > 0 )
  {
    MSGD((INFO_TRACEIO_LOCK_TERMINATING, m_Space.m_Filename.asCharp() ));
    ReturnStatus = vf_notok;
    return false;
  }
    // PTS 11054990
  sqlbeginmutex( &(m_Space.m_Mutex) );
  ++(m_Space.m_BusyCount);

  ReturnStatus = vf_ok;
  return true;
}

/*---------------------------------------------------------------------------*/
void ceo15_MessageFile::eo15Unlock()
{
  ROUTINE_DBG_MEO60 (_T("ceo15_MessageFile::eo15Unlock"));

  --(m_Space.m_BusyCount);
  sqlendmutex( &(m_Space.m_Mutex) );
}
/*---------------------------------------------------------------------------*/

SAPDB_Bool Worker::BeginThread( SAPDB_UInt4            stackSize,
                                SAPDBErr_MessageList   &messageList )
{
    if ( m_pInstance )
        return true;   //  instance already created!

    m_pInstance  = new (RTEMem_RteAllocator::Instance()) Worker();

    if ( 0 == m_pInstance )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_MALLOC_FAILED_FOR, "MessageOutputWorker" );
        return false;
    }

    if ( m_pInstance->Create( Worker::Normal, stackSize,
                              messageList ) != Worker::NoError )
    {
        m_pInstance->DestroyObject();
        m_pInstance = 0;
        return false;
    }


    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool Worker::Continue( SAPDBErr_MessageList   &messageList )
{
    if ( !m_pInstance )
        return true;   //  ignore call instance does not exist!

    if ( m_pInstance->Resume ( messageList ) != Worker::NoError )
        return false;

    return true;
}

/*---------------------------------------------------------------------------*/

void Worker::Wait()
{
    SAPDBErr_MessageList   messageList;

    if ( Suspend ( messageList ) != Worker::NoError )
        RTE_Crash(messageList);

    return;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int4  Worker::KernelThreadMain()
{
#ifndef HPUX
    // aCC compiler complains:
    // Warning 495: "veo15.cpp", line 1032 # The linkage directive is ignored
    // for an object or function declared static.
    ROUTINE_DBG_MEO60 (_T("eo15DumpMessageFile"));
#endif
    int FileIndex;
    tsp00_VfReturn_Param ReturnStatus;

    while ( eo15_NumberOfBuffers > 0 )
    {
      for ( FileIndex = 1; FileIndex <= NUMBER_OF_MESSAGE_BUFFERS; FileIndex++ )
      {
#ifdef USE_DYNAMIC_MESSAGE_BUFFER
        ceo15_LockMutex Lock( eo15_BufferMapMutex );

        if ( MESSAGE_BUFFER_PTR(FileIndex-1) == NULL )
          continue;
#endif /* USE_DYNAMIC_MESSAGE_BUFFER */

        ceo15_MessageFile TheMessageFile( *MESSAGE_BUFFER_PTR(FileIndex-1) );

        if ( !TheMessageFile.eo15Lock(ReturnStatus) )
        {
          continue;
        }

        if ( TheMessageFile.eo15IsUsed() )
        {
//  TEST_PRINT1("Worker trys to flush File %d\n", FileIndex)
          TheMessageFile.eo15Flush( ReturnStatus );
        }

//  TEST_PRINT("Worker unlocks\n")
        TheMessageFile.eo15Unlock();
      }

//  TEST_PRINT("Worker suspends itself\n")      
      Wait();
    }

    return 0;
}

/*---------------------------------------------------------------------------*/
static void eo15_MapAndLockMessageFile( const tsp00_Char *    Filename,
                                       tsp00_Int4           &FileIndex,
                                       tsp00_VfReturn_Param &ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("eo15_MapAndLockMessageFile"));

  FileIndex = INVALID_BUFFER_EO15;

  if ( !RTESys_IOCheckFilename(Filename) )
  {
    ReturnStatus = vf_notok;
  }
  else
  {

    tsp00_Int4 SearchIndex;
    // Inspect all buffers to find first free and verify against doublicated filename
    for ( SearchIndex = 1; SearchIndex <= NUMBER_OF_MESSAGE_BUFFERS; SearchIndex++ )
    {
#ifdef USE_DYNAMIC_MESSAGE_BUFFER
      ceo15_LockMutex Lock(eo15_BufferMapMutex);

      if ( MESSAGE_BUFFER_PTR(SearchIndex-1) == NULL )
        continue;
#endif

      ceo15_MessageFile TheMessageFile( *MESSAGE_BUFFER_PTR(SearchIndex-1) );

      if ( !TheMessageFile.eo15Lock(ReturnStatus) )
      {
        break;
      }

      // Leave loop if filename is found (and return valid index!)
      if ( TheMessageFile.eo15IsUsed()
        && TheMessageFile.eo15CompareFilename(Filename) )
      {
        FileIndex = SearchIndex;
        break;
      }

      TheMessageFile.eo15Unlock();
    }
  }
}
/*---------------------------------------------------------------------------
  Function: eo15_SearchAndLockMessageFile
  Description: The given file name is searched and locked in the message buffer space

  PTS 1113575 Each entry in the message buffer space is always locked and the filename
  is setup even before trying to open the file. If open fails, the filename is cleared
  from slot and the slot is unlocked. This way two parallel open are allowed to collide
  even on the same messaage file.
 */
static void eo15_SearchAndLockMessageFile( const tsp00_Char * Filename,
                                       tsp00_Int4           &ResultFileIndex,
                                       bool                      &isFreeSlot,
                                       tsp00_VfReturn_Param &ReturnStatus )
{
  ROUTINE_DBG_MEO60 (_T("eo15_SearchAndLockMessageFile"));

  ResultFileIndex = INVALID_BUFFER_EO15;
  isFreeSlot = false;
  ReturnStatus = vf_ok;

  if ( eo15_NumberOfBuffers <= 0 )
  {
    ReturnStatus = vf_notok;
    return;
  }

  if ( !RTESys_IOCheckFilename(Filename) )
  {
    ReturnStatus = vf_notok;
    return;
  }

#ifdef USE_DYNAMIC_MESSAGE_BUFFER
  ceo15_LockMutex Lock( eo15_BufferMapMutex );
#endif

  // Inspect all buffers to find first free and verify against doublicated filename
  tsp00_Int4 FileIndex;
  tsp00_Int4 NewFileIndex = INVALID_BUFFER_EO15;

  for ( FileIndex = 1, NewFileIndex = INVALID_BUFFER_EO15;
        FileIndex <= NUMBER_OF_MESSAGE_BUFFERS;
        FileIndex++ )
  {
#ifdef USE_DYNAMIC_MESSAGE_BUFFER
    if ( MESSAGE_BUFFER_PTR(FileIndex-1) == NULL )
    {
      if ( INVALID_BUFFER_EO15 == NewFileIndex )
      {
        NewFileIndex = FileIndex; // First free buffer index is memorized
      }
      continue;
    }
#endif

    ceo15_MessageFile TheMessageFile( *MESSAGE_BUFFER_PTR(FileIndex-1) );

    if ( !TheMessageFile.eo15Lock(ReturnStatus) )
    {
      MSGD((ERR_TRACEIO_OPEN_FAILED, Filename, "buffer lock failed"));
      return;
    }

    if ( !TheMessageFile.eo15IsUsed()
      && (INVALID_BUFFER_EO15 == NewFileIndex) )
    {
      NewFileIndex = FileIndex; // First free buffer index is memorized
    }

    // Leave loop if filename is duplicated (but return valid index!)
    if ( TheMessageFile.eo15CompareFilename(Filename) )
    {
      break;
    }

    // Keep only first free buffer locked....
    if ( NewFileIndex != FileIndex )
    {
      TheMessageFile.eo15Unlock();
    }
  }

  isFreeSlot = ( FileIndex == (NUMBER_OF_MESSAGE_BUFFERS+1)  );

  // If Filename is already in use, clear lock on first free buffer and return old index
  if ( !isFreeSlot )
  {
    // release not needed but locked buffer
    if ( INVALID_BUFFER_EO15 != NewFileIndex )
    {
      ceo15_MessageFile( *MESSAGE_BUFFER_PTR(NewFileIndex-1) ).eo15Unlock();
    }
  }
  else
  {
#ifdef USE_DYNAMIC_MESSAGE_BUFFER
    if ( INVALID_BUFFER_EO15 == NewFileIndex )
    {
        teo15MessageBufferSpace **NewMap;

        // Reallocate Dynamic Buffer Map
        if ( NO_ERROR_EO001 != ALLOC_MEM_EO57( (void **)&NewMap, sizeof(teo15MessageBufferSpace **)
                          * ( NUMBER_OF_MESSAGE_BUFFERS + MAP_INCREASE_AMOUNT_EO15 ) ) )
        {
          MSGD((ERR_TRACEIO_OPEN_FAILED, Filename, "Extending BufferMap failed"));
          ReturnStatus = vf_notok;
          return;
        }

        SAPDB_MemCopyNoCheck(NewMap, eo15_BufferMap, sizeof(teo15MessageBufferSpace **) * NUMBER_OF_MESSAGE_BUFFERS );
        SAPDB_MemFillNoCheck(NewMap + NUMBER_OF_MESSAGE_BUFFERS, 0, sizeof(teo15MessageBufferSpace **) * MAP_INCREASE_AMOUNT_EO15 );
        NewFileIndex = FileIndex = NUMBER_OF_MESSAGE_BUFFERS + 1;
        isFreeSlot = true;
        teo15MessageBufferSpace **OldMap = eo15_BufferMap;
        eo15_BufferMap = NewMap;
        NUMBER_OF_MESSAGE_BUFFERS = NUMBER_OF_MESSAGE_BUFFERS + MAP_INCREASE_AMOUNT_EO15;
        FREE_MEM_EO57(OldMap);
    }

    if ( MESSAGE_BUFFER_PTR(NewFileIndex-1) == NULL )
    {
      if ( NO_ERROR_EO001 != ALLOC_MEM_EO57( (void **)&MESSAGE_BUFFER_PTR(NewFileIndex-1),
                                              sizeof(teo15MessageBufferSpace)) )
      {
        MSGD((ERR_TRACEIO_OPEN_FAILED, Filename, "Out of memory"));
        ReturnStatus = vf_notok;
        return;
      }

      if ( NewFileIndex > eo15_NumberOfBuffers )
      {
        MSGD(( WRN_TRACEIO_MAXIMUM_EXCEEDED, NewFileIndex, eo15_NumberOfBuffers ));
      }

      sqlcreatemutex( &(MESSAGE_BUFFER_PTR(NewFileIndex-1)->m_Mutex) );
      MESSAGE_BUFFER_PTR(NewFileIndex-1)->m_Used = 0;
      MESSAGE_BUFFER_PTR(NewFileIndex-1)->m_Terminating = 0;
      MESSAGE_BUFFER_PTR(NewFileIndex-1)->m_BusyCount = 0;
      ceo15_MessageFile( *MESSAGE_BUFFER_PTR(NewFileIndex-1) ).eo15Lock(ReturnStatus);
    }
#endif

            /* PTS 1113575 */
    if ( ceo15_MessageFile::eo15CheckIndex(NewFileIndex) )
    {
        MESSAGE_BUFFER_PTR(NewFileIndex-1)->m_Filename.rawAssign(Filename);
    }
  }

  ResultFileIndex = ( isFreeSlot ? NewFileIndex : FileIndex );
}

/*---------------------------------------------------------------------------
  Function: eo15ShowLastOsError
  Description: build an error message containing the given routine and file name and write it using MSGD()

 */
static void eo15ShowLastOsError( const tsp00_Char *    Routine,
                                 const tsp00_Char *    Filename )
{
    char                  LastErrorMessage[256];
    RTESys_ErrorMessage   SysErrorMessage;
    RTE_SystemRc lastError = RTESys_GetLastError();

    sp77sprintf( &LastErrorMessage[0], sizeof(LastErrorMessage), "[%d] %s", (int)lastError, RTESys_StrError(lastError, SysErrorMessage) );
    MSGD(( ERR_TRACEIO_OS_ERROR, 
            Routine, 
            Filename == NULL ? "" : Filename, 
            &LastErrorMessage[0] ));
}

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/
