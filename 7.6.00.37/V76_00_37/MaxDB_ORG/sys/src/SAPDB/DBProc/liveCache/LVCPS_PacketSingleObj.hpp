/*!
 * \file    LVC_PacketSingleObj.hpp
 * \author  MarkusSi
 * \brief   ProcServer packet for operations on a single object.
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

#ifndef __LVCPS_PACKETSINGLEOBJ_HPP
#define __LVCPS_PACKETSINGLEOBJ_HPP

#include "liveCache/LVC_ProcServerInterface.hpp"

/*!
 * \brief Request packet for operations on a single object.
 */
class LVCPS_PacketSingleObj : public LVC_ProcServerPacket<LVCPS_PacketSingleObj> {
public:
  enum {
    CODE  = 100, ///< Packet code.

    msgNewObj     = 0,
    msgGetObj     = 1,	 
    msgGetKeyObj  = 2,
    msgUpdObj     = 3,  
    msgDelObj     = 4,
    msgDelAll     = 5,
    msgLckObj     = 6,
    msgLckUpdObj  = 7,
    msgUnlckObj   = 8,
    msgIsLocked   = 9,
    msgIsHistUsed = 10,
    msgLckSharedObj   = 11,
    msgUnlckSharedObj = 12
  };

  virtual void execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf);

  /// Message type.
  int                      m_type;

  tgg91_TransNo            m_consistentView;
  tgg01_ContainerId        m_containerId;
  bool                     m_isVersion;
  tgg01_OmsVersionContext  m_versionContext; 
  SAPDB_UInt8              m_oid;
  tgg91_PageRef            m_objSeq;

  bool                     m_doLock;
  bool                     m_shared;

  size_t                   m_bodySize;
  size_t                   m_bodyOffset;  

  size_t                   m_keyPos;
  size_t                   m_keyLen;
  size_t                   m_keyOffset;

  bool                     m_contObj;
  bool                     m_reused;
};

#endif  // __LVCPS_PACKETSINGLEOBJ_HPP

