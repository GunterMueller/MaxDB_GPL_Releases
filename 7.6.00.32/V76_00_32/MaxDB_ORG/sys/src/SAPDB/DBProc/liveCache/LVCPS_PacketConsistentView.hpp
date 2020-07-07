/*!
 * \file    LVC_PacketConsistentView.hpp
 * \author  MarkusSi
 * \brief   ProcServer packet for handling of consistent views.
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

#ifndef __LVCPS_PACKETCONSISTENTVIEW_HPP
#define __LVCPS_PACKETCONSISTENTVIEW_HPP

#include "liveCache/LVC_ProcServerInterface.hpp"

/*!
 * \brief Request packet for handling of consistent views.
 */
class LVCPS_PacketConsistentView : public LVC_ProcServerPacket<LVCPS_PacketConsistentView> {
public:
  enum {
    CODE  = 106, ///< Packet code.

    msgNew      = 0,
    msgEnd      = 1
  };

  virtual void execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf);

  /// Message type.
  int            m_type;

  tgg91_TransNo  m_consistentView;
};

#endif  // __LVCPS_PACKETCONSISTENTVIEW_HPP

