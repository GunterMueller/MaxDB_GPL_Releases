/*!
 * \file    LVC_PacketVersion.hpp
 * \author  MarkusSi
 * \brief   ProcServer packet for operations on versions.
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

#ifndef __LVCPS_PACKETVERSION_HPP
#define __LVCPS_PACKETVERSION_HPP

#include "liveCache/LVC_ProcServerInterface.hpp"

/*!
 * \brief Request packet for operations on a single object.
 */
class LVCPS_PacketVersion : public LVC_ProcServerPacket<LVCPS_PacketVersion> {
public:
  enum {
    CODE  = 101, ///< Packet code.

    msgCreate      = 0,
    msgClose       = 1,	 
    msgReset       = 2,
    msgDrop        = 3  
  };

  virtual void execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf);

  /// Message type.
  int                      m_type;

  tgg91_TransNo            m_consistentView;
  tgg01_OmsVersionContext  m_versionContext; 
};

#endif  // __LVCPS_PACKETVERSION_HPP

