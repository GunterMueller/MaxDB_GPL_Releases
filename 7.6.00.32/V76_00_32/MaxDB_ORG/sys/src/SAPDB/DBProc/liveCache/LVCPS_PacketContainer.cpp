/*!
 * \file    LVC_PacketVersion.cpp
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

#include "SAPDB/DBProc/liveCache/LVCPS_PacketContainer.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDB/Oms/OMS_DbpError.hpp"

void LVCPS_PacketContainer::execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf)
{
  tsp00_Int2               DBError         = 0;

  switch (m_type) {
    //--------------------------------------------------------------
    case msgCreateContainer: {
      tgg01_ContainerId containerId;
      sink->CreateContainer(m_schemaId, &m_guid, &m_className, m_size,
                            m_containerNo, 
                            m_keyPos, m_keyLen, 
                            m_keyPartitioned, 
                            m_useCachedKeys,
                            m_isVarObject,
                            m_useInternalTrans,
                            m_noWait,
                            (unsigned char*)&containerId,
                            &DBError);

      interf->containerReply(&containerId, NULL, 0, 0, false, false, 0,
                             DBError);
      break;
    }

    //--------------------------------------------------------------
    case msgExists: {
      bool useCachedKeys   = false;
      bool partitionedKeys = false;
      tgg01_ContainerId containerId;
      sink->ExistsContainer(m_schemaId, m_guid, m_containerNo, m_size, m_keyLen,
                            containerId, useCachedKeys, partitionedKeys, DBError);

      interf->containerReply(&containerId, NULL, 0, 0, useCachedKeys, partitionedKeys, 0,
                             DBError);
      break;
    }

    //--------------------------------------------------------------
    case msgLock: {
      sink->LockContainer(m_containerId,DBError);

      interf->errorStateReply(false, DBError);
      break;
    }


    //--------------------------------------------------------------
    case msgGetInfo: {
      sink->GetContainerInfo(m_containerHandle, m_guid, 
                             m_schemaId, m_containerNo,
                             DBError);

      interf->containerReply(NULL, &m_guid, m_schemaId, m_containerNo, false, false, 0,
                             DBError);
     break;
    }
    
    //--------------------------------------------------------------
    case msgDrop: {
     sink->DropContainer(m_schemaId, m_guid, m_containerNo,
                         DBError);

     interf->errorStateReply(false, DBError);
     break;
    }
    
    //--------------------------------------------------------------
    //default: {
    //  throw(DbpError(e_case_not_found, "LVCPS_PacketContainer::execute"));
    //}
  }
}
