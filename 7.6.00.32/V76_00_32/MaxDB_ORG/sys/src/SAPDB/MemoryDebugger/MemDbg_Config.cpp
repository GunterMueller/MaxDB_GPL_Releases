/*!
 * \file    MemDbg_Config.cpp
 * \author  IvanS
 * \brief   Memory debugger configuration.
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

#include "MemoryDebugger/MemDbg_Config.hpp"
#include "MemoryDebugger/MemDbg_Header.hpp"

/*-----------------------------------------------------*/

int MemDbg_Config::computeDbgSpace(size_t size) const
{
  if (m_flags & FLG_FENCE) {
    // we need 16B for the fences
    return (size > MemDbg_Header::MAX_SIZE) ? 32 : 16;
  } else if ((m_flags & FLG_PACK) || m_noleaks) {
    // we need just 8B for header
    return (size > MemDbg_Header::MAX_SIZE) ? 24 : 8;
  } else {
    return 0;
  }
}

/*-----------------------------------------------------*/
