/*!================================================================
module:        OMS_Trace.cpp

responsible:   ThomasA

special area:  OMS

description:   Oms Tracing   

see:

Copyright (c) 1998-2005 SAP AG 



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




===================================================================*/

#include "Oms/OMS_Trace.hpp"
#include "Oms/OMS_Oid.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include <string.h>
#include "hsp77.h"      // PTS 1125307

/*
Trace Levels
*/

struct CTraceLevel {
  ETraceLevel_co102 m_lvl;
  const char*       m_desc;
};

static CTraceLevel traceLevelsList[] = {
  omsTrError, "OMS_ERROR",
  omsTrInterface, "OMS_INTERFACE",
  omsTrContainerDir, "OMS_CONTAINERDIR",
  omsTrMemory, "OMS_MEMORY",
  omsTrVarObject, "OMS_VAROBJECT",
  omsTrSession, "OMS_SESSION",
  omsTrLock, "OMS_LOCK",
  omsTrKeyRange , "OMS_KEYRANGE",
  omsTrStream , "OMS_STREAM",
  /* the following are for the detail trace only - in DEBUG mode */
  omsTrContext, "OMS_CONTEXT",
  omsTrNewObj, "OMS_NEWOBJ",
  omsTrFlushCache, "OMS_FLUSHCACHE",
  omsTrKey, "OMS_KEY",
  omsTrVersion, "OMS_VERSION",
  omsEnumLimit, ""
};

OMS_TraceLevel TraceLevel_co102;

/*----------------------------------------------------------------------*/

OMS_TraceStream& OMS_TraceStream::operator<<(const char* msg) 
{
  int length = (int)strlen(msg);
  if (m_length + length < m_bufSize) {
    SAPDB_MemCopyNoCheck(m_buffer + m_length, msg, length);
    m_length += length;
  }
  return *this;
}

/*----------------------------------------------------------------------*/

OMS_TraceStream& OMS_TraceStream::operator<<(const OMS_UnsignedCharBuffer& uchar) 
{
  *this << "x'";
  for (size_t ix = 0; ix < uchar.length(); ++ix) {
    int  h = uchar[ix];
    char c;
    if ((h >> 4) < 10) {
      c = '0' + (h >> 4);
    }
    else {
      c = 'A' + (h >> 4) - 10;
    }
    this->putChar(c);
    h = h & 0xF;
    if (h < 10) {
      c = '0' + h;
    }
    else {
      c = 'A' + h - 10;
    }
    this->putChar(c);
  }
  *this << "'";
  return *this;
}

/*----------------------------------------------------------------------*/

OMS_TraceStream& OMS_TraceStream::putInt(tsp00_Longint val) 
{
  if (0 == val)
  {
      this->putChar('0');
      return *this;
  }
  if (val < 0) {
    this->putChar('-');
    val = -val;
  }
  char tmpBuf[22];
  int  digit;
  tmpBuf[21] = 0;
  char* pCurr = &tmpBuf[20];
  while (val > 0) {
    digit = val % m_radix;
    val   /= m_radix;
    if (digit > 9) {
      *pCurr = 'A' + (digit - 10);
    }
    else {
      *pCurr = '0' + digit;
    }
    --pCurr;
  }
  ++pCurr;
  return *this << pCurr;
}

/*----------------------------------------------------------------------*/

OMS_TraceStream& OMS_TraceStream::putUInt(unsigned long val) 
{
  if (0 == val)
  {
      this->putChar('0');
      return *this;
  }
  char tmpBuf[22];
  int  digit;
  tmpBuf[21] = 0;
  char* pCurr = &tmpBuf[20];
  while (val > 0) {
    digit = val % m_radix;
    val   /= m_radix;
    if (digit > 9) {
      *pCurr = 'A' + (digit - 10);
    }
    else {
      *pCurr = '0' + digit;
    }
    --pCurr;
  }
  ++pCurr;
  return *this << pCurr;
}

/*----------------------------------------------------------------------*/

OMS_TraceStream& OMS_TraceStream::operator<<(char c)
{
  this->putChar(c);
  return *this;
}

/*----------------------------------------------------------------------*/

OMS_TraceStream& OMS_TraceStream::operator<<(double d)
{
  char tmpBuf[40];
  sprintf(tmpBuf, "%f", d);                     // PTS 1125307
  sp77sprintf(tmpBuf, sizeof(tmpBuf),"%f", d);  // PTS 1125307
  return *this << tmpBuf;
}

/*----------------------------------------------------------------------*/

OMS_TraceStream& OMS_TraceStream::operator<<(const void* p)
{
  char tmpBuf[64];
  //sprintf(tmpBuf, "%p", p);                   // PTS 1125307
  sp77sprintf(tmpBuf, sizeof(tmpBuf),"%p", p);  // PTS 1125307
  return *this << tmpBuf;
}

/*----------------------------------------------------------------------*/

OMS_TraceStream& OMS_TraceStream::operator<<(const OmsObjectId& oid)
{
  return *this << "OID : " << oid.getPno() << "." << oid.getPagePos() << "(vers " << oid.getGeneration() << ")";
}
/*----------------------------------------------------------------------*/

OMS_TraceStream& OMS_TraceStream::operator<<(const OMS_ObjectId8& oid)
{
  return *this << "OID : " << oid.getPno() << "." << oid.getPagePos() << "(vers " << oid.getGeneration() << ")";
}

/*----------------------------------------------------------------------*/

OMS_TraceStream& OMS_TraceStream::operator<<(const GUID& guid)
{
  char tmpBuf[64];
  //sprintf (tmpBuf, "guid : %8X-%4X-%4X-%1X%1X%1X%1X%1X%1X%1X%1X", guid.Data1, guid.Data2, guid.Data3,
  //  guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5],
  //  guid.Data4[6], guid.Data4[7]);                     // PTS 1125307
  sp77sprintf (tmpBuf, sizeof(tmpBuf),"guid : %8X-%4X-%4X-%1X%1X%1X%1X%1X%1X%1X%1X", guid.Data1, guid.Data2, guid.Data3,
    guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5],
    guid.Data4[6], guid.Data4[7]);                       // PTS 1125307
  return *this << tmpBuf;
}

/*----------------------------------------------------------------------*/

OMS_TraceStream& OMS_TraceStream::operator<<(const OMS_CharBuffer& v)
{
  if (m_length + (int)v.length() < m_bufSize) {
    SAPDB_MemCopyNoCheck(m_buffer + m_length, v.str(), v.length());
    m_length += v.length();
  }
  return *this;
}

/*----------------------------------------------------------------------*/

OMS_TraceStream& OMS_TraceStream::operator<<(const OMS_ContainerHandle& contFileNo)
{
  return *this << "ContainerFileNo: "
               << OMS_UnsignedCharBuffer(reinterpret_cast<const unsigned char*>(&contFileNo), sizeof(OMS_ContainerHandle));
}

/*----------------------------------------------------------------------*/

bool OMS_TraceLevel::Set (const char* pLvl, bool enable)
{
  void (OMS_TraceLevel::* pAction)(ETraceLevel_co102);
  pAction = (enable) ? &OMS_TraceLevel::enable : &OMS_TraceLevel::disable;
  int idx  = 0;
  do {
    if (0 == strcmp(pLvl, traceLevelsList[idx].m_desc)) {
      (this->*pAction)(traceLevelsList[idx].m_lvl);
      return true;
    }
    ++idx;
  }
  while (omsEnumLimit != traceLevelsList[idx].m_lvl);

  return false;
}