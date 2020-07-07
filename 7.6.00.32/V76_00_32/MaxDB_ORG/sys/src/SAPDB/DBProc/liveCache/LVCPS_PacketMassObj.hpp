/*!
 * \file    LVC_PacketMassObj.hpp
 * \author  MarkusSi
 * \brief   ProcServer packet for mass operations on objects.
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

#ifndef __LVCPS_PACKETMASSOBJ_HPP
#define __LVCPS_PACKETMASSOBJ_HPP

#include "liveCache/LVC_ProcServerInterface.hpp"
#include "Oms/OMS_Defines.h"

/*!
 * \brief Request packet for mass operations on objects.
 */
class LVCPS_PacketMassObj : public LVC_ProcServerPacket<LVCPS_PacketMassObj> {
public:
  enum {
    CODE  = 107, ///< Packet code.

    msgNewObj    = 0,
    msgGetObj    = 1,	 
    msgGetKeyObj = 2,
    msgGetNxtOid = 3,
    msgUpdObj    = 4,  
    msgUpdVarObj = 5,  
    msgDelObj    = 6,
    msgLckObj    = 7,
    msgLckUpdObj = 8,
    msgGetContainer = 9,
    msgGetNxtKeyOid = 10,
    msgGetPrvKeyOid = 11,
    msgLckSharedObj = 12
  };

  virtual void execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf);

  /// Message type.
  int                      m_type;

  tsp00_Int4               m_objCnt;
  tgg91_TransNo            m_consistentView;
  tgg01_ContainerId        m_pContainerId[OMS_MASS_OPERATION_CNT];
  bool                     m_isVersion;
  tgg01_OmsVersionContext  m_versionContext; 
  SAPDB_UInt8              m_pOid[OMS_MASS_OPERATION_CNT];
  tgg91_PageRef            m_pObjSeq[OMS_MASS_OPERATION_CNT];

  bool                     m_doLock;
  bool                     m_shared;

  size_t                   m_pBodySize[OMS_MASS_OPERATION_CNT];
  size_t                   m_pBodyOffset[OMS_MASS_OPERATION_CNT];  

  size_t                   m_pObjSize[OMS_MASS_OPERATION_CNT];
  SAPDB_UInt8              m_pNextOid[OMS_MASS_OPERATION_CNT];

  bool                     m_isKeyed;
  size_t                   m_pKeyPos[OMS_MASS_OPERATION_CNT];
  size_t                   m_pKeyLen[OMS_MASS_OPERATION_CNT];
  size_t                   m_pKeyOffset[OMS_MASS_OPERATION_CNT];

  bool                     m_pContObj[OMS_MASS_OPERATION_CNT];
  bool                     m_pReused[OMS_MASS_OPERATION_CNT];  // PTS 1127661

  bool                     m_clustered;

  tsp00_Int8               m_timeout;
};

#endif  // __LVCPS_PACKETMASSOBJ_HPP

