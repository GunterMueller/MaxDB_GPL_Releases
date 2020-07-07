/*!
 * \file    OMS_KeyDesc.hpp
 * \author  MarkusSi, Roterring
 * \brief   Description of the key of persistent objects
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

#ifndef __OMS_KEYDESC_HPP
#define __OMS_KEYDESC_HPP


class OMS_KeyDesc {        // PTS 1122540
private:
  size_t   m_pos; 
  size_t   m_len;       

public:
  OMS_KeyDesc()
    : m_pos(0)
    , m_len(0)
  {}

  OMS_KeyDesc(size_t pos, size_t len)
    : m_pos(pos)
    , m_len(len)
  {}

  OMS_KeyDesc(const OMS_KeyDesc &keyDesc){
    m_pos           = keyDesc.m_pos;
    m_len           = keyDesc.m_len;
  }

  bool   IsKeyed()       const { return m_len > 0;       }
  size_t GetLen()        const { return m_len;           }
  size_t GetPos()        const { return m_pos;           }
};

static bool operator == (const OMS_KeyDesc &k1, const OMS_KeyDesc &k2)
{
  return k1.GetLen() == k2.GetLen() && k1.GetPos() == k2.GetPos();
}

static bool operator != (const OMS_KeyDesc &k1, const OMS_KeyDesc &k2)
{
  return k1.GetLen() != k2.GetLen() || k1.GetPos() != k2.GetPos();
}

#endif  // __OMS_KEYDESC_HPP
