/*!
 * \file    LVC_PacketContainer.hpp
 * \author  MarkusSi
 * \brief   ProcServer packet for operations on containers.
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

#ifndef __LVCPS_PACKETCONTAINER_HPP
#define __LVCPS_PACKETCONTAINER_HPP

#include "liveCache/LVC_ProcServerInterface.hpp"
#include "Oms/OMS_ContainerHandle.hpp"  //PTS 1124356 FF 2003-09-23 new FileDir

/*!
 * \brief Request packet for operations on containers.
 */
class LVCPS_PacketContainer : public LVC_ProcServerPacket<LVCPS_PacketContainer> {
public:
  enum {
    CODE  = 102, ///< Packet code.

    msgCreateContainer  = 0,
    msgExists           = 1,	 
    msgGetInfo          = 2,
    msgDrop             = 3,
    msgSync             = 4,
    msgLock             = 5
  };

  virtual void execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf);

  /// Message type.
  int                      m_type;

  OmsSchemaHandle          m_schemaId;
  GUID                     m_guid;
  tsp00_KnlIdentifier      m_className;
  size_t                   m_size;
  OmsContainerNo           m_containerNo;
  OMS_ContainerHandle      m_containerHandle;    //PTS 1124356 FF 2003-09-23 new FileDir
  tgg00_FileId             m_containerId;
  size_t                   m_keyPos;
  size_t                   m_keyLen;
  bool                     m_keyPartitioned;
  bool                     m_useCachedKeys;
  bool                     m_isVarObject;
  bool                     m_useInternalTrans;
  bool                     m_noWait;
};

#endif  // __LVCPS_PACKETCONTAINER_HPP

