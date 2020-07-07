/*!
 * \file    OMS_DbpBase.hpp
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

#ifndef __OMS_DBPBASE_HPP
#define __OMS_DBPBASE_HPP

#include "Oms/OMS_DbpError.hpp"
#include "Oms/OMS_Defines.h"

typedef void  (*DbpPROC)();

#if defined(WIN32)
typedef HANDLE DbpHandle;
#else
typedef void* DbpHandle;
#endif

struct IUnknown;
struct IliveCacheSink;

/*!
 * \brief Base class for SQL-Handle and OMS-Handle.
 *
 * \ingroup oms
 */
class AFX_EXT_CLASS DbpBase {
public :
  DbpBase();
  DbpBase(IUnknown * pSink);

  /// Causes a break point, if running in the livecache proxy (Win NT only).
  void           dbpDebugBreak();
  /// Drops a message file.
  void           dbpDropMessageFile(const char* fn);

  /// Checks for existence of a file or directory.
  void           dbpExistsFile(const char* fn, bool& doesExist, bool& IsDirectory);

  /// Unloads a shared library.
  void dbpFreeLibrary (DbpHandle hd);

  /// Gets the DBROOT dependent "sap" path (libraries, shared libraries...).
  void           dbpGetDbrootSapPath(char* path, int bufSize);

  /// Determines the address of a funtcion from a shared library.
  DbpPROC dbpGetProcAddress(DbpHandle hd, const char *pProcName);

  /// Get path to directory where message files are stored.
  void           dbpGetWorkingDirectory(char* path, int bufSize);

  /// Loads a shared library.
  DbpHandle      dbpLoadLibrary(const char* path, bool loadGlobal);

  /// Writes an error message into the kernel diag file.
  void           dbpOpError( const char*fmt, ... );
  /// Writes an error message into the kernel diag file from varargs.
  void           dbpOpErrorVarg( const char*fmt,va_list args );

  /// Writes a message into the kernel diag file.
  void           dbpOpMsg( const char*fmt, ... );
  /// Writes a message into the kernel diag file, from varargs.
  void           dbpOpMsgVarg( const char*fmt, va_list args );

  /// Writes a message into the kernel trace file.
  void           dbpTrace( const char*fmt, ... );
  /// Writes a message into the kernel trace file, from varargs.
  void           dbpTraceVarg( const char*fmt, va_list args );

  /// Opens a message file.
  OmsTypeInt4    dbpOpenMessageFile (const char* fn, unsigned long fileSize, bool doAppend);

  /// Write a variable length null terminated string into a message file.
  void           dbpWriteMessageFile(OmsTypeInt4 fileHandle, const char*fmt, ... );
  /// Write a variable length null terminated string into a message file.
  void           dbpWriteMessageFileVarg(OmsTypeInt4 fileHandle, const char*fmt, va_list args );
  /// Write a variable length, null terminated string (without format transformation) into a message file.
  void           dbpWriteMessageFileConst(OmsTypeInt4 fileHandle, const char* msg );

  /// Closes a message file.
  void           dbpCloseMessageFile(OmsTypeInt4 fileHandle);

  /// Flushes buffered messages into a message file.
  void           dbpSyncMessageFile (OmsTypeInt4 fileHandle);
  /// Flushes buffered messages into a message file.
  void           dbpSyncMessageFile (const char* fn);

  /// Get time in microseconds (for calculating elapsed times).
  static OmsTypeInt8  dbpMicroTime();
  /// Get file information.
  static bool         dbpStatFile(
      const char  *fn,
      OmsTypeInt8 &ctime,
      OmsTypeInt8 &fsize);
  /// Format file time like ctime.
  static void         dbpFormatTime(
      char        *buffer,
      size_t      bufsize,
      OmsTypeInt8 time);

protected:
  void           dbpAdviseLcSink (IliveCacheSink* pSink);
# if defined (SQLCLS_IMPL)
  const IliveCacheSink * getLcSink() const {return m_lcSink;} /* PTS 1130629, PG */
# endif
private :
  /// Underlying communication sink.
  IliveCacheSink* m_lcSink;
  /// Helper method
  bool formatOutput( char *, int, char *, va_list ); /* PTS 1127735, PG */
};

#endif  // __OMS_DBPBASE_HPP
