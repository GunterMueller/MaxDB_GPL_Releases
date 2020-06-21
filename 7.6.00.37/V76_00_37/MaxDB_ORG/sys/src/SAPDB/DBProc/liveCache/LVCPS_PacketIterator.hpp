/*!
 * \file    LVC_PacketIterator.hpp
 * \author  MarkusSi
 * \brief   ProcServer packet for iterators.
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

#ifndef __LVCPS_PACKETITERATOR_HPP
#define __LVCPS_PACKETITERATOR_HPP

#include "liveCache/LVC_ProcServerInterface.hpp"
#include "Oms/OMS_Defines.h"
#include "Oms/OMS_IterDirection.hpp"

/*!
 * \brief Request packet for Iterators.
 */
class LVCPS_PacketIterator : public LVC_ProcServerPacket<LVCPS_PacketIterator> {
public:
  enum {
    CODE  = 108, ///< Packet code.

    msgGetNxtOid                = 0,
    msgGetNxtKey                = 1,
    msgGetPrvKey                = 2,
    msgGetVersionNxtKey         = 3,
    msgGetVersionPrvKey         = 4,
    msgCreateKeyIter            = 5,
    msgDestroyKeyIter           = 6,
    msgStartUnloadedVersionIter = 7
  };

  virtual void execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf);

  /// Message type.
  int                      m_type;

  tsp00_Int4               m_cnt;
  tgg91_TransNo            m_consistentView;
  tgg01_ContainerId        m_containerId;
  bool                     m_isVersion;
  tgg01_OmsVersionContext  m_versionContext; 

  SAPDB_UInt8              m_startOid;

  tsp00_Int4               m_bodySize;
  tsp00_Int4               m_bodyOffset;

  void                    *m_pIterator;

  OmsIterDirection         m_direction;
  bool                     m_inclusive;

  tsp00_Int4               m_keyLen;
  // Variable sized data of the keys follows:
  // case 1: 
  //   -startkey
  //   -stopkey
  // case 2:
  //   -startKey
  //   -lowerKey  
  //   -upperKey
};

#endif  // __LVCPS_PACKETITERATOR_HPP

