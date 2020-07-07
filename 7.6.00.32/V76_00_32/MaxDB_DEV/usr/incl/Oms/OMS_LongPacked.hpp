/*!
 * \file    OMS_LongPacked.hpp
 * \author  PeterG (GoertzP)
 * \brief   OMS: Long Packed
 */

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


#ifndef OMS_LONGPACKED_HPP
#define OMS_LONGPACKED_HPP


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Oms/OMS_AFX.h"
#include "livecachetypes.h" // nocheck


/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#undef  OMS_PACKED_HIGH
#define OMS_PACKED_HIGH(X) (X >> 4)
#undef  OMS_PACKED_LOW
#define OMS_PACKED_LOW(X)  (X & 0x0F)

#undef  BCDPLUS
#define BCDPLUS  0xc
#undef  BCDMINUS
#define BCDMINUS 0xd


/************************************************************************/
/* Class OmsLongPacked                                                  */
/************************************************************************/

class AFX_EXT_CLASS OmsLongPacked {
public:
  void toInt16( OmsTypeInt8 &, OmsTypeUInt8 & );
  void toLongPacked( OmsTypeInt8, OmsTypeInt8, bool );
private:
  inline int getDigit(int);
  inline int getSign();
  unsigned char m_longPacked[15];      
};


/************************************************************************/
/* Implementation of inline methods                                     */
/************************************************************************/

inline int OmsLongPacked::getDigit(int digit) {
  /* TBD: check range
  if (digit < 0) || (digit > 29) {} */
  if (digit & 1) {
    return OMS_PACKED_HIGH(m_longPacked[15-(digit+1)/2]);
  }
  else {
    return OMS_PACKED_LOW (m_longPacked[15-(digit+2)/2]);
  }
}

/*----------------------------------------------------------------------*/

inline int OmsLongPacked::getSign() {
  return OMS_PACKED_LOW(m_longPacked[14]);
}


#endif  /* OMS_LONGPACKED_HPP */

