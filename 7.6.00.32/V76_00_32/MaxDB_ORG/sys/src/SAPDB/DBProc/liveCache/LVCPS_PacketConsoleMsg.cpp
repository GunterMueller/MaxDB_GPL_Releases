/*!
 * \file    LVC_ProcServerInterface.hpp
 * \author  IvanS
 * \brief   ProcServer packet for messages.
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

#include "DBProc/liveCache/LVCPS_PacketConsoleMsg.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"

void LVCPS_PacketConsoleMsg::execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf)
{
  tsp00_Int2 DBError = 0;
  tsp00_Int4 size;

  switch (m_type) {
  case msgConsole:
    sink->ConsoleMsg(m_length, reinterpret_cast<unsigned char*>(this + 1));
    break;
  case msgOpMsg:
    size = m_length;
    sink->MultiPurpose (m_show, mm_log, &size, 
      reinterpret_cast<unsigned char*>(this + 1), &DBError);
    break;
  case msgOpError:
    size = m_length;
    sink->MultiPurpose (m_show, mm_nil, &size, 
      reinterpret_cast<unsigned char*>(this + 1), &DBError);
    break;
  case msgSetError:
    sink->SetError(m_errorNo, m_length, 
                   reinterpret_cast<unsigned char*>(this + 1),
                   m_codeType);
    break;
  case msgHError:
    sink->HResultError(m_hError);
    break;
  case msgVTrace:
    sink->Vtrace(m_length,
                 reinterpret_cast<unsigned char*>(this + 1));
    break;
  }
  interf->simpleReply(DBError);
}
