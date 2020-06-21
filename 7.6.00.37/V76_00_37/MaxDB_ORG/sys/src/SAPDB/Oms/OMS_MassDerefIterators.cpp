/*!
 * \file    OMS_MassDerefIterators.cpp
 * \author  MarkusSi
 * \brief   Iterators for mass deref
 */

/*

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



*/

#include "Oms/OMS_MassDerefIterators.hpp"
#include "Oms/OMS_DbpError.hpp"
#include "Oms/OMS_Globals.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"


void OmsDeref::throwDbpError(long errorNo, const char* msg, OmsObjectId &oid, const char* pFile, unsigned int line){
  OMS_ObjectId8 oid8(oid);
  OMS_Globals::Throw((short)errorNo, msg, oid8, pFile, line);
}


void OmsDerefArray::throwDbpError(long errorNo, const char* msg, OmsObjectId &oid, const char* pFile, unsigned int line){
  OMS_ObjectId8 oid8(oid);
  OMS_Globals::Throw((short)errorNo, msg, oid8, pFile, line);
}


void OmsDerefForUpd::throwDbpError(long errorNo, const char* msg, OmsObjectId &oid, const char* pFile, unsigned int line){
  OMS_ObjectId8 oid8(oid);
  OMS_Globals::Throw((short)errorNo, msg, oid8, pFile, line);
}


void OmsDerefArrayForUpd::throwDbpError(long errorNo, const char* msg, OmsObjectId &oid, const char* pFile, unsigned int line){
  OMS_ObjectId8 oid8(oid);
  OMS_Globals::Throw((short)errorNo, msg, oid8, pFile, line);
}


void convertHex(const unsigned char* in, size_t inLen, char* out, size_t outLen)
{
  // Check if output array is large enough
  if ((inLen+1)*2 > outLen){
    memset(out, 'x', outLen);
  }

  // Loop over all characters and transform them into hexadecimal 
  int outCnt = 0;
  out[outCnt++] = 'x';
  for (size_t inCnt=0; inCnt<inLen; ++inCnt){
    // First consider leading byte
    int  h = in[inCnt] >> 4;
    out[outCnt++] = (h<10 ? ('0' + h) : ('A' + h - 10));

    // Then consider tailing byte
    h = in[inCnt] & 0xF;
    out[outCnt++] = (h<10 ? ('0' + h) : ('A' + h - 10));
  }
  out[outCnt] = 0;
}


void OmsDerefKeyArray::throwDbpError(long errorNo, const char* msg, const unsigned char* pKey, size_t keyLen, const char* pFile, unsigned int line){
  char buffer[100];
  size_t len = strlen(msg);
  memset(buffer, ' ', 100);
  if (len < 100)
    SAPDB_MemCopyNoCheck(&buffer[0], pKey, keyLen);
  convertHex(pKey, keyLen, &buffer[len+1], 100-(len+1));
  OMS_Globals::Throw((short)errorNo, &buffer[0], pFile, line);
}


void OmsDerefKeyArrayForUpd::throwDbpError(long errorNo, const char* msg, const unsigned char* pKey, size_t keyLen, const char* pFile, unsigned int line){
  char buffer[100];
  size_t len = strlen(msg);
  memset(buffer, ' ', 100);
  if (len < 100)
    SAPDB_MemCopyNoCheck(&buffer[0], pKey, keyLen);
  convertHex(pKey, keyLen, &buffer[len+1], 100-(len+1));
  OMS_Globals::Throw((short)errorNo, &buffer[0], pFile, line);
}

