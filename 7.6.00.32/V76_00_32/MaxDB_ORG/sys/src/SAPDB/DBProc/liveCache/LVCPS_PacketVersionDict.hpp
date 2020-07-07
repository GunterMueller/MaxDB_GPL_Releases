/*!
 * \file    LVCPS_PacketVersionDict.hpp
 * \author  MarkusSi
 * \brief   ProcServer packet for the handling of the version dictionary.
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

#ifndef __LVCPS_PACKETVERSIONDICT_HPP
#define __LVCPS_PACKETVERSIONDICT_HPP

#include "liveCache/LVC_ProcServerInterface.hpp"
#include "livecachetypes.h"  /* nocheck */  
#include "Oms/OMS_Defines.h"

class OMS_Context;

/*!
 * \brief Request packet for version dictionary.
 */
class LVCPS_PacketVersionDict : public LVC_ProcServerPacket<LVCPS_PacketVersionDict> {
public:
  enum {
    CODE  = 114, ///< Packet code.

    msgDrop               = 0,
    msgFind               = 1,	 
    msgInsert             = 2,
    msgShutdown           = 3,
    msgUnload             = 4,	 
    msgMarkNotUnloadable  = 5,
    msgMarkUnloadable     = 6,
    msgCreateIter         = 7,
    msgGetNext            = 8,
    msgDestructIter       = 9
  };

  virtual void execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf);

  /// Message type.
  int            m_type;

  OmsVersionId   m_versionId;
  OMS_Context   *m_pVersionContext;
  void          *m_pIter;
  OMS_LockMode   m_lockMode;
};

#endif  // __LVCPS_PACKETVERSIONDICT_HPP

