/*


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#ifndef __OMS_TRACE_HPP
#define __OMS_TRACE_HPP

#include <string.h>
#include <stdio.h>
#include "gsp00.h"
#include "Oms/OMS_Defines.h"
#include "Oms/OMS_DbpError.hpp"
#include "Oms/OMS_ContainerHandle.hpp"  // FF 2003-09-23 new FileDir

enum ETraceLevel_co102 {
  omsTrAlways       = 1,
  omsTrError        = 2,
  omsTrInterface    = 4,
  omsTrContainerDir = 8,
  omsTrMemory       = 16,
  omsTrVarObject    = 32,
  omsTrTrans        = 64,
  omsTrSession      = 128,
  omsTrLock         = 256,
  omsTrKeyRange     = 512,
  omsTrStream       = 1024,
  /* the following are for the detail trace only - in DEBUG mode */
  omsTrContext      = 2048,
  omsTrNewObj       = 4096,
  omsTrFlushCache   = 8192,
  omsTrKey          = 16384,
  omsTrVersion      = 32768,
  omsEnumLimit      = 65536
};

class OMS_TraceLevel {
public :
  OMS_TraceLevel() : m_traceLevel(omsTrAlways) {
  }
  bool operator[](tsp00_Uint4 lvl) {
    return ((m_traceLevel & lvl) > 0);
  }
  void disable(ETraceLevel_co102 lvl) {
    m_traceLevel &= ((tsp00_Uint4) lvl ^ 0xFFFFFFFF);
  }
  void enable(ETraceLevel_co102 lvl) {
    m_traceLevel |= (tsp00_Uint4) lvl;
  }
  void enableAll() {
    m_traceLevel = 0xFFFFFFFF;
  }
  bool Set (const char* lvl, bool enable);
private :
  tsp00_Uint4 m_traceLevel;
};


#if !defined(EXTERN_CO102)
#define EXTERN_CO102 extern
#endif

extern OMS_TraceLevel TraceLevel_co102;

class OMS_UnsignedCharBuffer {
public :
  inline OMS_UnsignedCharBuffer(const unsigned char*, size_t);
  inline unsigned char operator[](int) const;
  inline size_t length() const;
private :
  const unsigned char* m_buf;
  size_t               m_length;
};

inline OMS_UnsignedCharBuffer::OMS_UnsignedCharBuffer(const unsigned char* c, size_t length) : m_buf(c), m_length(length) {}

inline unsigned char  OMS_UnsignedCharBuffer::operator[](int ix) const {
  return m_buf[ix];
}

inline size_t OMS_UnsignedCharBuffer::length() const {
  return m_length;
}

class OMS_CharBuffer {
public :
  inline OMS_CharBuffer(const char*, size_t);
  inline OMS_CharBuffer(const unsigned char*, size_t);
  inline char operator[](int) const;
  inline const char* str() const;
  inline size_t length() const;
private :
  const char* m_buf;
  size_t      m_length;
};

inline OMS_CharBuffer::OMS_CharBuffer(const char* c, size_t length) : m_buf(c), m_length(length) {}

inline OMS_CharBuffer::OMS_CharBuffer(const unsigned char* c, size_t length) : m_buf((char*) c), m_length(length) {}

inline const char* OMS_CharBuffer::str() const {
  return m_buf;
}

inline char OMS_CharBuffer::operator[](int ix) const {
  return m_buf[ix];
}

inline size_t OMS_CharBuffer::length() const {
  return m_length;
}

class OMS_TraceStream {
public :
  OMS_TraceStream(char* buffer, int bufSize) : m_buffer(buffer), m_bufSize(bufSize), m_length(0), m_radix(10) {};
  OMS_TraceStream& operator<<(const char*);
  OMS_TraceStream& operator<<(const OMS_UnsignedCharBuffer&);
  OMS_TraceStream& operator<<(char);
  OMS_TraceStream& operator<<(int i)   { return putInt(i); };
  OMS_TraceStream& operator<<(short i) { return putInt(i); }
  OMS_TraceStream& operator<<(long i)  { return putInt(i); };
  OMS_TraceStream& operator<<(unsigned int i)   { return putUInt(i); };
  OMS_TraceStream& operator<<(unsigned short i) { return putUInt(i); }
  OMS_TraceStream& operator<<(unsigned long i)  { return putUInt(i); };
  OMS_TraceStream& operator<<(double);
  OMS_TraceStream& operator<<(const void*);
  OMS_TraceStream& operator<<(const OmsObjectId&);
  OMS_TraceStream& operator<<(const OMS_ObjectId8&);
  OMS_TraceStream& operator<<(const GUID&);
  OMS_TraceStream& operator<<(const OMS_CharBuffer&);
  OMS_TraceStream& operator<<(const OMS_ContainerHandle&); //FF 2003-09-23 new FileDir
  inline int length() const;
private :
  inline void putChar(char);
  OMS_TraceStream& putInt(tsp00_Longint);
  OMS_TraceStream& putUInt(unsigned long);
  char* m_buffer;
  int   m_bufSize;
  int   m_length;
  int   m_radix;
};

inline int OMS_TraceStream::length() const {
  return m_length;
}

inline void OMS_TraceStream::putChar(char c) {
  if (m_length < m_bufSize) {
    m_buffer[m_length] = c;
    ++m_length;
  }
}


#define OMS_TRACE(LVL,SINK,FMT) \
{\
  if (TraceLevel_co102[LVL]) {\
  char buffer[256];\
  OMS_TraceStream s (buffer, sizeof(buffer));\
  s << FMT;\
  SINK->Vtrace (s.length(), (unsigned char*) &buffer[0]);\
  }\
}

#ifdef OMS_DEBUG
#define OMS_DETAIL_TRACE(LVL,SINK,FMT) \
{\
  if (TraceLevel_co102[LVL]) {\
  char buffer[256];\
  OMS_TraceStream s (buffer, sizeof(buffer));\
  s << FMT;\
  SINK->Vtrace (s.length(), (unsigned char*) &buffer[0]);\
  }\
}
#else
#define OMS_DETAIL_TRACE(LVL,SINK,FMT)
#endif

#endif  // __OMS_TRACE_HPP
