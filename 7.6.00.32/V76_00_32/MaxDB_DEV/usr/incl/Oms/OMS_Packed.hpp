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
#ifndef __OMS_PACKED_HPP
#define __OMS_PACKED_HPP

#include "Oms/OMS_AFX.h"
#include "livecachetypes.h" // nocheck

#define OMS_PACKED_HIGH(X) (X >> 4)
#define OMS_PACKED_LOW(X)  (X & 0x0F)

#define  BCDPLUS           0xc
#define  BCDMINUS          0xd

class AFX_EXT_CLASS OmsPacked {
public:
  OmsTypeInt64 toInt64_16();
  void   toPacked(OmsTypeInt64, int);
private:
  /* PTS 1107752 */
  int digit15() { return OMS_PACKED_HIGH(m_packed[0]); }
  int digit14() { return OMS_PACKED_LOW (m_packed[0]); }
  int digit13() { return OMS_PACKED_HIGH(m_packed[1]); }
  int digit12() { return OMS_PACKED_LOW (m_packed[1]); }
  int digit11() { return OMS_PACKED_HIGH(m_packed[2]); }
  int digit10() { return OMS_PACKED_LOW (m_packed[2]); }
  int digit9 () { return OMS_PACKED_HIGH(m_packed[3]); }
  int digit8 () { return OMS_PACKED_LOW (m_packed[3]); }
  int digit7 () { return OMS_PACKED_HIGH(m_packed[4]); }
  int digit6 () { return OMS_PACKED_LOW (m_packed[4]); }
  int digit5 () { return OMS_PACKED_HIGH(m_packed[5]); }
  int digit4 () { return OMS_PACKED_LOW (m_packed[5]); }
  int digit3 () { return OMS_PACKED_HIGH(m_packed[6]); }
  int digit2 () { return OMS_PACKED_LOW (m_packed[6]); }
  int digit1 () { return OMS_PACKED_HIGH(m_packed[7]); }
  int sign   () { return OMS_PACKED_LOW (m_packed[7]); }
  unsigned char m_packed[16];      
};

#endif  // __OMS_PACKED_HPP
