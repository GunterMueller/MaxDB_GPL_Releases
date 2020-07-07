/*!
  @file           heo15.h
  @author         JoergM
  @special area   Message File Output Module (for buffered trace I/O)
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



#ifndef HEO15_H
#define HEO15_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "geo00_0.h"
#include "heo07.h"
#include "RunTime/System/RTESys_IO.h"

/*
 * Bufferspace may be found in Shared Memory, so LZU can dump it even after crash of kernel
 */
/* PTS 1105164 Increased size due to '1GByte Messagefiles...' */
#define MESSAGE_FILE_PAGE_MXEO15  (64*1024)
#define INVALID_BUFFER_EO15      ((tsp00_Int4)0)
typedef tsp00_Byte teo15_Buffer[MESSAGE_FILE_PAGE_MXEO15];
#define UNLIMITED_FILE_SIZE_EO15 ((teo15_Position)0)

typedef tRTESys_IOPosition teo15_Position;
typedef tRTESys_IOHandle   teo15_Handle;

# define INVALID_HANDLE_EO15    INVALID_HANDLE_RTESYS_IO

typedef struct
{
    /* If == 0 currently unused
       if != 0 currently used m_used times */
    tsp00_Int4       m_Used;

    /* If == 0 running normal
       if != 0 terminating due to shutdown or crash */
    tsp00_Int4       m_Terminating;

    /* if == 0 write into first buffer
       if == 1 write into second buffer */
    tsp00_Int4       m_CurrentBuffer;

    /* number of bytes written to CurrentBuffer */
    teo15_Position   m_BytesStored;

    /* number of bytes written to second Buffer */
    teo15_Position   m_BytesToFlush;

    /*
     * If != 0file size is limited...
     * Write restarts at beginning of file and is terminated by END_LINE_EO15
     */
    teo15_Position   m_MaxFileSize;

    /*
     * If file size is limited it is important to know the next write position...
     */
    teo15_Position   m_NextWritePosition;

    /* Cycle Counter */
    tsp00_Int4       m_CycleCounter;

    /* Number of Bytes written into file since first file open */
    teo15_Position   m_TotalBytesWritten;

    /* Hostfilenumber */
    teo15_Handle     m_HostFileNo;

    /* The two buffers */
    teo15_Buffer     m_Buffer[2];

    /* Filename used (always found in working directory) */
    tsp00_Pathc      m_Filename;

    /* Number of threads running inside critical region */
    tsp00_Int4       m_BusyCount;

    /* Critical Section for managing of BufferSpace */
    teo07_Mutex      m_Mutex;

} teo15MessageBufferSpace;


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*! @name RTE Internal Interface */
 /*@(*/
/*!
   @brief          RTE Internal - Used inside RTE code to setup memory space
   @param          NumberOfBuffers [in] Number of concurrent message files
   @param          BufferArray [in] Pointer to the buffer array
   @return         none


     It must be called before any message file function is called. It is done during
     setup of the global kernel RTE shared memory section.  
     NOTE - not part of user interface....

 */

externC void       eo15InitMessageBufferSpace( const tsp00_Int4  NumberOfBuffers,
                                               teo15MessageBufferSpace *BufferArray);

/*!
   @brief          RTE Internal - Used inside RTE code to dump contents in case crash
   @param          NumberOfBuffers [in] Number of concurrent message files
   @param          BufferArray [in] Pointer to the buffer array
   @return         none


     This function is called in case of a UNIX crash or NT fatal exception. 
     In UNIX the kernel child process crashed containing the thread responsible for dumping
     the message file contents. Since the buffers are placed in the shared memory segments
     prepared and accessible by the parent kernel process, this call allows to complete the
     message files. 
     Under NT the files are still open, even in a case of a crash and the exception handler
     simple has to close them and prevent tasks from reusing the message files during crash.
     NOTE - not part of user interface....

 */

externC void eo15CompleteMessageFiles( const tsp00_Int4 NumberOfBuffers,
                                      teo15MessageBufferSpace *BufferArray);

/*!
   @brief          RTE Internal - Used inside RTE code to flush contents in case of SHUTDOWN
   @return         none


     This function is called in case of a successfull SHUTDOWN of database. It performs a close
     on each still open Message file, so DCOM threads are allowed to forget the call to 
     eo15CloseMessageFile.
     NOTE - not part of user interface....

 */

externC void eo15FlushMessageFiles( void );
/*@)*/

/*! @name MessageFile */
 /*@(*/

/*!
   @brief          Open a message file for overwriting
   @param          Filename [in] The file name
   @param          MaxFileSize [in] The maximum size of the file. If 0 the file size is unlimited.  If not 0 writing restarts at beginning of file after MaxFileSize was reached.
   @param          doNotInit [in] Do not reset file size on open, if not necessary
   @param          ReturnStatus [out] vf_ok: Open successfull, vf_notok: Open failed
   @return         The index of the message file (NOT a file handle!!!)
              In case of error, INVALID_BUFFER_EO15(0) is returned.


     It is called before any write to the message file takes place.
     If file already exists and is not still opened the file is overwritten! 
     So be carefull, choosing the filename.
     A second open to the same file, simply increases a reference count. 
     MaxFileSize is ignored in such a case.
     For each open, a close must be issued.
     For NT only: The file is always created with FILE_SHARE_READ flag set.

 */

externC tsp00_Int4 eo15OpenMessageFile( const tsp00_Char                 * Filename, 
                                        const teo15_Position               MaxFileSize, 
                                        bool                               doNotInit,
                                        tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus );
/*!
   @brief          Get an index for a named MassageFile
   @param          Filename [in] The file name
   @return         The index of the message file (NOT a file handle!!!)
              In case file is unknown, INVALID_BUFFER_EO15(0) is returned.


     It can be called to find the message file index corresponding to an open message file.
     If file is not opened, INVALID_BUFFER_EO15(0) is returned, even if file exists on disk.

 */

externC tsp00_Int4 eo15GetMessageFileIndex( const tsp00_Char             * Filename );

/*!
   @brief          Check for existence of file or directory
   @param          Filename [in] The file/directory name
   @param          DoesExist [out] Set to true if file or directory was found, false if not.
   @param          IsDirectory [out] Set to true to indicate directory was found, false if not.
   @param          ReturnStatus [out] vf_ok: Valid information returned, vf_notok: Call failed
   @return         true if path leads to existing file or directory
              false if no file or directory exists at given path


     Called to test for existence of file or directory. 
     Sets DoesExist to true if file or directory exists, false otherwise or if error occured.
     If DoesExist is false IsDirectory is always set to false.
     NOTE: You must inspect ReturnCode to make sure information is valid!
     Use sqlerrs() to get retrieve complete error information.

 */

externC void eo15CheckExist( const tsp00_Char * Filename,
                             bool  VAR_VALUE_REF DoesExist,
                             bool  VAR_VALUE_REF IsDirectory,
                             tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus );

/*!
   @brief          Write a variable length 'C string' to a message file
   @param          FileIndex [in] The file index returned by eo15OpenMessageFile
   @param          Message [in] The message to be written.
   @param          ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_notok if failed, vf_noseek if seek failed )
   @return         none


     This expects a 0 terminated 'C String' as input. The message written is not flushed
     immediatly, but stored in internal buffers. Flushing takes place on eo15CloseMessageFile
     and in eo15CopyMessageFile implicitly. 
     NOTE - If length of message exceeds MaxFileSize, only the last 'piece' of the message will
     be visible.

 */

externC void       eo15WriteMessageFile( const tsp00_Int4  FileIndex,
                                         const tsp00_Char *Message,
                                         tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus );

/*
  Function:     eo15SyncMessageFile
  description:  Flush buffered messages to a message file using index
    
    This flushes the content of the buffers without closing the file. Not really needed for
    small files, since flushing takes place automatically before eo15CloseMessageFile and 
    eo15CopyMessageFile.

  NOTE: Using this after each write, will reduce performance of the database dramatically...

  arguments:
    Filename [in] The file name used for eo15OpenMessageFile
    FileIndex [in] The file index returned by eo15OpenMessageFile
    ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_notok if failed, vf_noseek if seek failed )
    
  return value: none
 */
externC void eo15SyncMessageFile( const tsp00_Int4 FileIndex,
                                  tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus );

/*!
   @brief          Flush buffered messages to a message file using file name
   @param          Filename [in] The file name used for eo15OpenMessageFile
   @param          FileIndex [in] The file index returned by eo15OpenMessageFile
   @param          ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_notok if failed, vf_noseek if seek failed )
   @return         none


     This does the same, but allows to specify file name instead of index.

 */

externC void eo15SyncNamedMessageFile( const tsp00_Char * Filename,
                                       tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus );

/*!
   @brief          Close a message file
   @param          FileIndex [in] The file index returned by eo15OpenMessageFile
   @param          ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_notok if failed, vf_noseek if seek failed )
   @return         none


     This flushes the content of the buffers and allows the index to be used again by a 
     different eo15OpenMessageFile call.
     If file was opened more than once, the reference count is decreased but the file
     is kept open.
     But always the file is implicitly synchronized as if eo15SyncMessageFile() was called.

 */

externC void       eo15CloseMessageFile( const tsp00_Int4 FileIndex,
                                         tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus );

/*!
   @brief          Copy a message file
   @param          FileIndex [in] The file index returned by eo15OpenMessageFile
   @param          CopyFileName [in] The name of the copy file
   @param          ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_notok if failed, vf_noseek if seek failed )
   @return         none


     This copies the current content of the message file indexed by FileIndex into a snapshot
     file given by name. The original file is untouched. If snapshot file already exists, it is
     overwritten. So be carfull choosing the file name!

 */

externC void       eo15CopyMessageFile( const tsp00_Int4  FileIndex,
                                        const tsp00_Char *CopyFileName,
                                        tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus );

/*!
   @brief          Remove a message file
   @WARNING        This is a direct call which is allowed to remove all files named...
   @param          FileName [in] The file name of the file to be removed
   @param          ReturnStatus [out] Status enumeration ( vf_ok if remove is done, vf_notok if remove failed
   @return         none


     This tries to remove a named MessageFile. If File is still open, it will not be removed
     and an error is returned instead. If file does not exist, vf_notok is returned.

 */

externC void       eo15RemoveMessageFile( const tsp00_Char *RemoveFileName,
                                        tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus );


/*@)*/

#endif  /* HEO15_H */
