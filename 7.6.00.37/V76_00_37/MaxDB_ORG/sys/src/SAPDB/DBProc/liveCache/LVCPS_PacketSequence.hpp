/*!
 * \file    LVC_PacketSequence.hpp
 * \author  MarkusSi
 * \brief   ProcServer packet for handling of sequences.
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

#ifndef __LVCPS_PACKETSEQUENCE_HPP
#define __LVCPS_PACKETSEQUENCE_HPP

#include "liveCache/LVC_ProcServerInterface.hpp"

/*!
 * \brief Request packet for handling of sequences.
 */
class LVCPS_PacketSequence : public LVC_ProcServerPacket<LVCPS_PacketSequence> {
public:
  enum {
    CODE  = 109, ///< Packet code.

    msgAttachSeq   = 0,
    msgNextVal     = 1
  };

  virtual void execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf);

  /// Message type.
  int            m_type;

  tsp00_KnlIdentifier   m_sequenceName;
  tgg00_Surrogate       m_sequenceId;
  tgg00_SeqInfo         m_sequenceInfo;

};

#endif  // __LVCPS_PACKETSEQUENCE_HPP

