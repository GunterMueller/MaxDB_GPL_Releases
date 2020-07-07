/*!
 * \file    OMS_DbpBase.cpp
 * \author  ThomasA
 * \brief   Defines Functions used by SQL-Class and OMS
 */
/*

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



*/


/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/


#include "Oms/OMS_DbpBase.hpp"
#include "Oms/OMS_Globals.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "RunTime/RTE_IInterface.hpp"
#include "hsp77.h"
#include "SAPDBCommon/SAPDB_UTF8Basis.hpp"

/*===========================================================================*
*  DEFINES                                                                  *
*===========================================================================*/


DbpBase::DbpBase() : m_lcSink(NULL) 
{
}

/*----------------------------------------------------------------------*/

DbpBase::DbpBase(IUnknown * pSink)  
{
  if (pSink){
    m_lcSink = reinterpret_cast<IliveCacheSink*>(pSink);
  }
  else {
    m_lcSink = OMS_Globals::GetCurrentLcSink();
  }
}

/*----------------------------------------------------------------------*/

void DbpBase::dbpAdviseLcSink (IliveCacheSink* pSink)
{
  m_lcSink = pSink;
}

/*----------------------------------------------------------------------*/

/*!
 * Causes a break point, if running in the livecache proxy (Win NT only).
 */
void DbpBase::dbpDebugBreak()
{
#if defined(WIN32)
  //DebugBreak();
#endif
}

/*----------------------------------------------------------------------*/

/*!
 * This tries to drop a named MessageFile. If the file does not exist or is
 * still open, it will not be dropped and an error is thrown instead.
 *
 * \param fn the file name of the file to be dropped
 */
void DbpBase::dbpDropMessageFile(const char* fn)
{
  tgg00_BasisError DBError;
  m_lcSink->DropFile (CONST_CAST(char*, fn), DBError);
  if (0 != DBError) {
    OMS_Globals::Throw(DBError, "DbpBase::dbpDropMessageFile", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

/*!
 * Called to test for existence of file or directory.
 * Sets doesExist to \c true if file or directory exists, \c false otherwise
 * or if error occured. If doesExist is \c false isDirectory is always set 
 * to \c false.
 *
 * \param fn           the file/directory name,
 * \param doesExist    set to \c true if file or directory was found, \c false if not,
 * \param isDirectory  set to \c true to indicate directory was found, \c false if not.
 */
void DbpBase::dbpExistsFile(const char* fn, bool& doesExist, bool& isDirectory) 
{
  tgg00_BasisError DBError;
  m_lcSink->ExistsFile (CONST_CAST(char*, fn), doesExist, isDirectory, DBError);
  if (0 != DBError) {
    OMS_Globals::Throw(DBError, "DbpBase::dbpExistsFile", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

/*!
 * Unloads a shared library.
 *
 * \param hd handle to the shared library.
 */
void DbpBase::dbpFreeLibrary (DbpHandle hd) 
{
  tsp00_ErrText errtext;
  bool        ok;
  HRESULT     hr;
  hr = m_lcSink->FreeLibrary(hd, ok, errtext, sizeof(tsp00_ErrText)-1 );
  //  if (!sqlFreeLibrary(hd, errtext, sizeof(errtext) -1)) {
  if (!ok) {
    OMS_Globals::Throw(e_lib_not_released, errtext, __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

/*!
 * Gets the DBROOT dependent "sap" path (libraries, shared libraries...).
 *
 * \param path buffer for the path,
 * \param bufSize the size of the path buffer provided by the caller.
 */
void DbpBase::dbpGetDbrootSapPath(char* path, int bufSize)
{
  tgg00_BasisError DBError;
  tsp00_Pathc      pathBuf; 
  m_lcSink->GetDbrootSapPath (pathBuf, DBError);
  if (0 != DBError) {
    OMS_Globals::Throw(DBError, "DbpBase::dbpGetDbrootSapPath", __MY_FILE__, __LINE__);
  }
  int len = (int)strlen(pathBuf);
  if (len < bufSize) {
    SAPDB_MemCopyNoCheck(path, &pathBuf[0], len + 1);
  }
  else {
    OMS_Globals::Throw(e_directory_not_found, "DbpBase::dbpGetDbrootSapPath", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

/*!
 * Determines the address of a funtcion from a shared library.
 *
 * \param hd handle to the shared library provided by dbpLoadLibrary,
 * \param pProcName name of the function.
 *
 * \return a pointer to the function.
 */
DbpPROC DbpBase::dbpGetProcAddress(DbpHandle hd, const char *pProcName) 
{
  tsp00_ErrText errtext;
  DbpPROC     p;
  p = (DbpPROC)RTE_IInterface::Instance().GetProcAddress( (RTE_SharedLibHandle)hd,
                                                           pProcName,
                                                           REINTERPRET_CAST(char*, &errtext[0]),
                                                           sizeof(errtext-1));
  if (NULL == p) {
    OMS_Globals::Throw(e_proc_address_not_found, errtext, __MY_FILE__, __LINE__);
  }
  return p;
}

/*----------------------------------------------------------------------*/

/*!
 * This returns the path to the directory that is used for all relatively
 * specified message files. It should be identical to the WorkingDirectory
 * of the database.
 * 
 * \param path the path to the root of all relatively specified message files,
 * \param bufSize the size of the path buffer provided by the caller.
 */
void DbpBase::dbpGetWorkingDirectory(char* path, int bufSize) 
{
  tgg00_BasisError DBError;
  tsp00_Pathc      pathBuf; 
  m_lcSink->GetWorkingDirectory (pathBuf, DBError);
  if (0 != DBError) {
    OMS_Globals::Throw(DBError, "DbpBase::dbpGetWorkingDirectory", __MY_FILE__, __LINE__);
  }
  int len = (int)strlen(pathBuf);
  if (len < bufSize) {
    SAPDB_MemCopyNoCheck(path, &pathBuf[0], len + 1);
  }
  else {
    OMS_Globals::Throw(e_directory_not_found, "DbpBase::dbpGetWorkingDirectory", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

/*!
 * Loads a shared library.
 *
 * \param path the name of the library to be loaded. The name can
 *  include the path but it should no include the fileextension.
 * \param loadGlobal if \c true, load with 'global' scope of symbol-resolution,
 *  if \c false, load with 'local' scope of symbol-resolution.
 *
 * \return handle for the loaded library.
 */
DbpHandle DbpBase::dbpLoadLibrary(const char* path, bool loadGlobal)
{
  DbpHandle       hd;
  tsp00_ErrText     errtext;
  m_lcSink->LoadLibraryEx(path, loadGlobal, &hd, errtext, sizeof(tsp00_ErrText)-1 );
  return hd;
}

/*----------------------------------------------------------------------*/

/*!
 * Writes an error message into the kernel diag file.
 * 
 * \param fmt a format specification (like printf).
 */
void DbpBase::dbpOpError( const char*fmt, ... )
{
  /* PTS 1127735, PG: re-written */
  va_list args;
  va_start (args, fmt);
  dbpOpErrorVarg(fmt, args);
  va_end (args);
}

/*----------------------------------------------------------------------*/

/*!
 * Writes an error message into the kernel diag file from varargs.
 * 
 * \param fmt a format specification (like printf),
 * \param args argument list.
 */
void DbpBase::dbpOpErrorVarg( const char*fmt, va_list args )
{
  /* PTS 1127735, PG: re-written */

  char        buffer[256];
  tsp00_Int2  DBError;
  tsp00_Int4  BufLen;
  HRESULT     hr;
  bool        formatOK = false;
  char        locFmt[256];
  int         maxFmtLen = sizeof(locFmt) - 1;

  if ((int)strlen(fmt) > maxFmtLen) {
    strncpy( locFmt, fmt, maxFmtLen );
    locFmt[maxFmtLen] = 0;
  }
  else {
    strcpy( locFmt, fmt );
  }
  formatOK = formatOutput( buffer, sizeof(buffer), locFmt, args );

  if (!formatOK) {
    /* %w replaced by %s */
    sp77vsprintf( buffer, sizeof(buffer), locFmt, args );
  }

  BufLen = (tsp00_Int4)strlen(buffer);
  hr = m_lcSink->MultiPurpose( m_show, mm_nil, &BufLen, 
                               (unsigned char*) &buffer[0], &DBError );
}

/*----------------------------------------------------------------------*/

/*!
 * Opens a message file. If \c doAppend is \c true and the file already exists,
 * it is opened but writing takes place at end of file. \c MaxFileSize is ignored
 * in such a case. If \c doAppend is \c false or the file does not exist, a file
 * is created. If the file already exists and it is not opened it is overwritten!
 * So be careful choosing the filename.
 *
 * A second open to the same file, simply increases a reference count.
 * \c MaxFileSize is ignored in such a case.
 *
 * For each open, a close must be issued.
 *
 * \param fn the file name.
 * \param fileSize the maximum size of the file. If 0 the file size is ignored. 
 *  \c fileSize has no meaning, if the file already exists and is not overwritten.
 *  If not 0, writing restarts at the beginning of the file after \c MaxFileSize
 *  was reached.
 * \param doAppend if \c true: if the file already exist it is opened, but writing
 *  takes place at end of file. If \c false: if the file already exists, it is 
 *  overwritten.
 *
 * \return the index of the message file (NOT a file handle!!!)
 */
OmsTypeInt4 DbpBase::dbpOpenMessageFile (const char* fn, unsigned long fileSize, bool doAppend) 
{
  tsp00_Int4 FileHandle;
  tgg00_BasisError DBError;
  m_lcSink->OpenFile (CONST_CAST(char*, fn), fileSize, doAppend, &FileHandle, &DBError);
  if (0 != DBError) {
    OMS_Globals::Throw(DBError, "DbpBase::dbpOpenMessageFile", __MY_FILE__, __LINE__);
  }
  return FileHandle;
}

/*----------------------------------------------------------------------*/

/*!
 * Writes a message into the kernel diag file.
 *
 * \param fmt a format specification (like printf).
 */
void DbpBase::dbpOpMsg( const char*fmt, ... ) 
{
  /* PTS 1127735, PG: re-written */
  va_list args;
  va_start (args, fmt);
  dbpOpMsgVarg(fmt, args);
  va_end (args);
}

/*----------------------------------------------------------------------*/

/*!
 * Writes a message into the kernel diag file, from varargs.
 *
 * \param fmt a format specification (like printf),
 * \param args argument list.
 */
void DbpBase::dbpOpMsgVarg( const char*fmt, va_list args )
{
  /* PTS 1127735, PG: re-written */

  char        buffer[256];
  tsp00_Int2  DBError;
  tsp00_Int4  BufLen;
  HRESULT     hr;
  bool        formatOK = false;
  char        locFmt[256];
  int         maxFmtLen = sizeof(locFmt) - 1;

  if ((int)strlen(fmt) > maxFmtLen) {
    strncpy( locFmt, fmt, maxFmtLen );
    locFmt[maxFmtLen] = 0;
  }
  else {
    strcpy( locFmt, fmt );
  }
  formatOK = formatOutput( buffer, sizeof(buffer), locFmt, args );

  if (!formatOK) {
    /* %w replaced by %s */
    sp77vsprintf( buffer, sizeof(buffer), locFmt, args );
  }

  BufLen = (tsp00_Int4)strlen(buffer);
  hr = m_lcSink->MultiPurpose( m_show, mm_log, &BufLen, 
                               (unsigned char*) &buffer[0], &DBError );
}

/*----------------------------------------------------------------------*/

/*!
 * Writes a message into the kernel trace file.
 *
 * \param fmt a format specification (like printf).
 */
void DbpBase::dbpTrace( const char*fmt, ... ) 
{
  /* PTS 1127735, PG: re-written */
  va_list args;
  va_start (args, fmt);
  dbpTraceVarg(fmt, args);
  va_end (args);
}

/*----------------------------------------------------------------------*/

/*!
 * Writes a message into the kernel trace file, from varargs.
 *
 * \param fmt a format specification (like printf),
 * \param args argument list.
 */
void DbpBase::dbpTraceVarg( const char*fmt, va_list args ) 
{
  /* PTS 1127735, PG: re-written */

  char  buffer[256];
  bool  formatOK = false;
  char  locFmt[256];
  int   maxFmtLen = sizeof(locFmt) - 1;

  if ((int)strlen(fmt) > maxFmtLen) {
    strncpy( locFmt, fmt, maxFmtLen );
    locFmt[maxFmtLen] = 0;
  }
  else {
    strcpy( locFmt, fmt );
  }
  formatOK = formatOutput( buffer, sizeof(buffer) - 4, locFmt, args );

  if (!formatOK) {
    /* %w replaced by %s */
    sp77vsprintf( buffer, sizeof(buffer) - 4, locFmt, args );
  }

  m_lcSink->Vtrace ((tsp00_Int4)strlen(buffer), (unsigned char*) &buffer[0]);
}


/*----------------------------------------------------------------------*/

/*!
 * Closes a message file.
 *
 * This flushes the content of the buffers and allows the index to be used again 
 * by a different dbpOpenMessageFile call.
 *
 * If the file was opened more than once, the reference count is decreased but 
 * the file is kept open.
 *
 * The file is always implicitly synchronized as if dbpSyncMessageFile() was called.
 *
 * \param fileHandle the file index returned by dbpOpenMessageFile.
 */
void DbpBase::dbpCloseMessageFile(OmsTypeInt4 fileHandle) 
{
  tgg00_BasisError DBError;
  m_lcSink->CloseFile (fileHandle, &DBError);
  if (0 != DBError) {
    OMS_Globals::Throw(DBError, "DbpBase::dbpCloseMessageFile", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

/*
 * This flushes the content of the buffers without closing the file. Not 
 * really needed for small files, since flushing takes place automatically 
 * before dbpCloseMessageFile.
 *
 * \note Using this after each write, will reduce performance of the database
 *    dramatically...
 *
 * \param fileHandle the file index returned by dbpOpenMessageFile.
 */
void DbpBase::dbpSyncMessageFile (OmsTypeInt4 fileHandle)
{
  tgg00_BasisError DBError;
  m_lcSink->SyncFile (NULL, fileHandle, &DBError);
  if (0 != DBError) {
    OMS_Globals::Throw(DBError, "DbpBase::dbpSyncMessageFile", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

/*
 * This flushes the content of the buffers without closing the file. Not 
 * really needed for small files, since flushing takes place automatically 
 * before dbpCloseMessageFile.
 *
 * \note Using this after each write, will reduce performance of the database
 *    dramatically...
 *
 * \param fn the name of the message file as specified in dbpOpenMessageFile.
 */
void DbpBase::dbpSyncMessageFile (const char* fn)
{
  tgg00_BasisError DBError;
  m_lcSink->SyncFile (CONST_CAST(char*, fn), 0, &DBError);
  if (0 != DBError) {
    OMS_Globals::Throw(DBError, "DbpBase::dbpSyncMessageFile", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

/*!
 * Write a variable length null terminated string into a message file.
 * This expects a 0 terminated 'C String' as input. The message written is
 * not flushed immediatly, but stored in internal buffers. Flushing takes
 * place on dbpCloseMessageFile. The length of the message is restricted 
 * to 255 bytes.
 *
 * \note if the length of the message file exceeds MaxFileSize, only the
 *  last 'piece' of the message file will be visible.
 * 
 * \param fileHandle the file index returned by dbpOpenMessageFile,
 * \param fmt        format of the message to be written.
 */
void DbpBase::dbpWriteMessageFile(OmsTypeInt4 fileHandle, const char*fmt, ... )
{
  char        buffer[256];
  tsp00_Int2  DBError;
  tsp00_Int4  BufLen;

  va_list args;
  va_start (args, fmt);
  sp77vsprintf ( buffer, sizeof(buffer), fmt, args );
  BufLen = (tsp00_Int4)strlen(buffer);
  m_lcSink->WriteFile (fileHandle, buffer, &DBError);
  va_end (args);
  if (0 != DBError) {
    OMS_Globals::Throw(DBError, "DbpBase::dbpWriteMessageFile", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

/*!
 * Write a variable length null terminated string into a message file.
 * The message written is not flushed immediatly, but stored in internal
 * buffers. Flushing takes place on dbpCloseMessageFile. The length of 
 * the message is restricted to 255 bytes.
 *
 * \note if the length of the message file exceeds MaxFileSize, only the
 *  last 'piece' of the message file will be visible.
 * 
 * \param fileHandle the file index returned by dbpOpenMessageFile,
 * \param fmt        message format string,
 * \param args       argument list.
 */
void DbpBase::dbpWriteMessageFileVarg(OmsTypeInt4 fileHandle, const char*fmt, va_list args )
{
  char        buffer[256];
  tsp00_Int2  DBError;
  tsp00_Int4  BufLen;

  sp77vsprintf ( buffer, sizeof(buffer), fmt, args );
  BufLen = (tsp00_Int4)strlen(buffer);
  m_lcSink->WriteFile (fileHandle, buffer, &DBError);
  if (0 != DBError) {
    OMS_Globals::Throw(DBError, "DbpBase::dbpWriteMessageFileVarg", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

/*!
 * Write a variable length null terminated string into a message file.
 * The message written is not flushed immediatly, but stored in internal
 * buffers. Flushing takes place on dbpCloseMessageFile. 
 * There is no limitation on the length of the message is restricted to 255 bytes.
 *
 * \note The routine itself does not impose any restriction on the length 
 *  but if run in ude-server mode the package size might be a limitation of
 *  the length.
 *  If the length of the message file exceeds MaxFileSize, only the
 *  last 'piece' of the message file will be visible.
 * 
 * \param fileHandle the file index returned by dbpOpenMessageFile,
 * \param msg        message to be written 
 */
void DbpBase::dbpWriteMessageFileConst(OmsTypeInt4 fileHandle, const char* msg )
{
  tsp00_Int2  DBError;

  m_lcSink->WriteFile (fileHandle, const_cast<char*>(msg), &DBError);
  if (0 != DBError) {
    OMS_Globals::Throw(DBError, "DbpBase::dbpWriteMessageFileConst", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

/*!
 * Get time in microseconds. This routine can be used to calculate elapsed
 * time by subtracting the values returned by this function before and
 * after execution of the measured block.
 *
 * \return time in microseconds.
 */
OmsTypeInt8  DbpBase::dbpMicroTime()
{
  return OMS_Globals::GetKernelInterface()->GetMicroTime();
}

/*----------------------------------------------------------------------*/

/*!
 * Get file information. You can use dbpFormatTime to convert the returned
 * time into ASCII format.
 *
 * \param fn file name,
 * \param ctime change time (out),
 * \param fsize file size (out).
 */
bool         DbpBase::dbpStatFile(
    const char  *fn,
    OmsTypeInt8 &ctime,
    OmsTypeInt8 &fsize)
{
  SAPDB_UInt8 time;
  bool success = OMS_Globals::GetKernelInterface()->StatFile(fn, time,
    reinterpret_cast<SAPDB_UInt8&>(fsize));
  ctime = (OmsTypeInt8) time;
  return success;
}

/*----------------------------------------------------------------------*/

/*!
 * Format time into ASCII format.
 *
 * \param buffer destination buffer,
 * \param bufsize buffer size,
 * \param time time returned by dbpStatFile().
 */
void         DbpBase::dbpFormatTime(
      char        *buffer,
      size_t      bufsize,
      OmsTypeInt8 time)
{
  OMS_Globals::GetKernelInterface()->FormatTime(buffer, bufsize, time);
}

/*----------------------------------------------------------------------*/

/*!
 * Format 
 *
 * \...
 */
#if defined(LINUX) && ( defined(S390X) || defined(X86_64)|| defined(SDBonPPC64) )
bool DbpBase::formatOutput( char * buffer, int bufSize, char * fmt, va_list xargs ) {
  va_list args;
  __va_copy(args,xargs);
#else
bool DbpBase::formatOutput( char * buffer, int bufSize, char * fmt, va_list args ) {
#endif

  char   c;
  int    argCnt = 0;
  int    cntr   = 0;
  char * perPtr = NULL;
  int    perPos = -2;
  int    startPos    = 0;
  int    curWritePos = 0;
  int    charSize    = 0;
  int           d = 0;
  void *        p = NULL;
  char *        s = NULL;
  unsigned int  u = 0;
  OmsTypeWyde * w = NULL;
  char   locBuf[256];
  int    locBufSize  = sizeof(locBuf);
  int    locWritePos = 0;
  int    locBufLen   = 0;
  int    locReadPos  = 0;
  int    strLen      = 0;
  bool   notSuppFmt  = false;
  bool   uniCat      = true;
  bool   uniLc       = true;
  SAPDB_UTF8 * pUTF8 = NULL;


  /* check for not supported formats */

  perPtr = strchr( fmt, '%' );
  while (perPtr) {
    argCnt++;
    c = *(perPtr + 1);
    switch (c) {
      case 'd' :
      case 'p' :
      case 's' :
      case 'u' :
      case 'w' :
          break;
      default :
        notSuppFmt = true;
    }
    if (notSuppFmt) {
      break;
    }
    else {
      perPtr = strchr( perPtr + 1, '%' );
    }
  }


  /* in case of not supported formats, replace %w by %s and return false */

  if (notSuppFmt) {
    perPtr = strstr( fmt, "%w" );
    while (perPtr) {
      *(perPtr + 1) = 's';
      perPtr = strstr( perPtr + 1, "%w" );
    }
    return false;
  }


  /* everything fine, build string */

  uniCat = OMS_Globals::KernelInterfaceInstance->IsUnicodeInstance();
  uniLc  = omsIsUnicodeInstance();

  for ( int argCntr = 0; argCntr < argCnt; argCntr++ ) {
    startPos  += perPos + 2;
    perPos     = strcspn( fmt + startPos, "%" );
    if (perPos > 0) {
      if (perPos < bufSize - 1 - curWritePos) {
        strncpy( buffer + curWritePos, fmt + startPos, perPos );
        curWritePos += perPos;
      }
      else {
        if (bufSize - 1 - curWritePos > 0) {
          strncpy( buffer + curWritePos, fmt + startPos, bufSize - 1 - curWritePos);
        }
        curWritePos = bufSize - 1;
        break;
      }
    }
    memset( locBuf, '\0', sizeof(locBuf) );
    switch (*(fmt + startPos + perPos + 1)) {
      case 'd' :
        d = va_arg( args, int );
        sp77sprintf( locBuf, sizeof(locBuf), "%d", d );
        break;
      case 'p' :
        p = va_arg( args, void * );
        sp77sprintf( locBuf, sizeof(locBuf), "%p", p );
        break;
      case 's' :
        s = va_arg( args, char * );
        if (uniLc) {
          pUTF8       = REINTERPRET_CAST( SAPDB_UTF8 *, s );
          charSize    = (int)SAPDB_UTF8Basis::CharacterSize(pUTF8);
          locWritePos = 0;
          strLen      = (int)strlen(s);
          while ( (0 < charSize) && 
                  (charSize <= locBufSize - 1 - locWritePos) &&
                  (charSize <= strLen - locWritePos) ) {
            for ( cntr = 0; cntr < charSize; cntr++ ) {
              locBuf[locWritePos] = (char)(*pUTF8);
              ++locWritePos;
              ++pUTF8;
            }
            charSize = (int)SAPDB_UTF8Basis::CharacterSize(pUTF8);
          }
          locBuf[locWritePos] = 0;
        }
        else {
          if (uniCat) {
            sp77sprintfUnicode( sp77encodingUTF8, locBuf, sizeof(locBuf), "%=s",
                                sp77encodingAscii, s );
          }
          else {
            sp77sprintf( locBuf, sizeof(locBuf), "%s", s );
          }
        }
        break;
      case 'u' :
        u = va_arg( args, unsigned int );
        sp77sprintf( locBuf, sizeof(locBuf), "%u", u );
        break;
      case 'w' :
        w = va_arg( args, OmsTypeWyde * );
        if (uniLc) {
          sp77sprintfUnicode( sp77encodingUTF8, locBuf, sizeof(locBuf), "%=s",
                              sp77encodingUCS2Native, w );
        }
        else {
          int loByte = 0, hiByte = 0, srcPos = 0, tmpWritePos = 0;
          char tmpBuf[256];
          char intBuf[6];
          if (sp77encodingUCS2Native == sp77encodingUCS2) {
            loByte = 1;
          }
          else {
            hiByte = 1;
          }
          SAPDB_Byte * pSource = REINTERPRET_CAST( SAPDB_Byte *, w );
          while ( (tmpWritePos < sizeof(tmpBuf) - 1) &&
                  !((0 == *pSource) && (0 == *(pSource + 1))) ) {
            if (0 != pSource + hiByte) {
              sp77sprintf( intBuf, sizeof(intBuf), "%d", *(w + srcPos) );
              if (strlen(intBuf) <= sizeof(tmpBuf) - 1 - tmpWritePos) {
                strncpy( &tmpBuf[tmpWritePos], intBuf, strlen(intBuf) );
                tmpWritePos += strlen(intBuf);
              }
              else {
                break;
              }
            }
            else {
              tmpBuf[tmpWritePos] = *(pSource + loByte);
              tmpWritePos += 1;
            }
            srcPos  += 1;
            pSource += 2;
          }
          tmpBuf[tmpWritePos] = 0;
          if (uniCat) {
            sp77sprintfUnicode( sp77encodingUTF8, locBuf, sizeof(locBuf), "%=s",
                                sp77encodingAscii, tmpBuf );
          }
          else {
            strcpy( locBuf, tmpBuf );
          }
        }
    }
    pUTF8      = REINTERPRET_CAST( SAPDB_UTF8 *, locBuf );
    locReadPos = 0;
    charSize   = (int)SAPDB_UTF8Basis::CharacterSize(pUTF8 + locReadPos);
    locBufLen  = (int)strlen(locBuf);
    while ( (0 < charSize) &&
            (charSize <= bufSize - 1 - curWritePos) &&
            (charSize <= locBufLen - locReadPos) ) {
      for ( cntr = 0; cntr < charSize; cntr++ ) {
        buffer[curWritePos] = (char)(*(pUTF8 + locReadPos));
        ++curWritePos;
        ++locReadPos;
      }
      charSize = (int)SAPDB_UTF8Basis::CharacterSize(pUTF8 + locReadPos);
    }
  }
  startPos += perPos + 2;

  if ( (*(fmt + startPos) != 0) && (curWritePos < bufSize - 1) ) {
    if ((int)strlen(fmt + startPos) < bufSize - 1 - curWritePos) {
      strncpy( buffer + curWritePos, fmt + startPos, strlen(fmt + startPos) );
      curWritePos += strlen(fmt + startPos);
    }
    else {
      if (bufSize - 1 - curWritePos > 0) {
        strncpy( buffer + curWritePos, fmt + startPos, bufSize - 1 - curWritePos );
      }
      curWritePos = bufSize - 1;
    }
  }

  buffer[curWritePos] = 0;

  return true;

}

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/
