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

#ifndef __LVCPS_PACKETCONSOLEMSG_HPP
#define __LVCPS_PACKETCONSOLEMSG_HPP

#include "liveCache/LVC_ProcServerInterface.hpp"

/*!
 * \brief Request packet for ConsoleMsg.
 */
class LVCPS_PacketConsoleMsg : public LVC_ProcServerPacket<LVCPS_PacketConsoleMsg> {
public:
  enum {
    CODE  = 10, ///< Packet code.

    msgConsole	    = 0,	   ///< ConsoleMsg
    msgOpMsg        = 1,     ///< dbpOpMsg
    msgOpError      = 2,     ///< dbpOpError
    msgHError       = 3,
    msgSetError     = 4,
    msgVTrace       = 5
  };

  virtual void execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf);

  /// Size of the message in bytes.
  size_t  m_length;
  /// Message type.
  int     m_type;

  HRESULT          m_hError;
  tgg00_BasisError m_errorNo;
  tsp00_CodeType   m_codeType;
  // variable-sized text data follow
};

#endif  // __LVCPS_PACKETCONSOLEMSG_HPP

