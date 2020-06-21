/*!
 * \file    LVCPS_PacketVersionDict.cpp
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

#include "SAPDB/DBProc/liveCache/LVCPS_PacketVersionDict.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDB/Oms/OMS_DbpError.hpp"
#include "liveCache/LVC_LibOmsInterface.hpp"

void LVCPS_PacketVersionDict::execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf)
{
  tsp00_Int2  DBError         = 0;

  switch (m_type) {
    //--------------------------------------------------------------
    case msgDrop: {
      OMS_LibOmsInterface *pInterface = LVC_LibOmsInterface::Instance();
      pInterface->VersionDictDrop(m_versionId, DBError);

      interf->versionDictReply(NULL, DBError, NULL);
      break;
    }

    //--------------------------------------------------------------
    case msgFind: {
      OMS_Context *pContext = NULL;
      OMS_LibOmsInterface *pInterface = LVC_LibOmsInterface::Instance();
      pInterface->VersionDictFind(m_versionId, &pContext);
      
      interf->versionDictReply(pContext, e_ok, NULL);
      break;
    }
    
    //--------------------------------------------------------------
    case msgInsert: {
      OMS_LibOmsInterface *pInterface = LVC_LibOmsInterface::Instance();
      pInterface->VersionDictInsert(m_versionId, m_pVersionContext, DBError);

      interf->versionDictReply(NULL, DBError, NULL);
      break;
    }
    
    //--------------------------------------------------------------
    case msgShutdown: {
      OMS_LibOmsInterface *pInterface = LVC_LibOmsInterface::Instance();
      pInterface->VersionDictShutdown();

      interf->simpleReply(0);
      break;
    }
    
    //--------------------------------------------------------------
    case msgUnload: {
      bool unloaded;
      OMS_LibOmsInterface *pInterface = LVC_LibOmsInterface::Instance();
      unloaded = pInterface->VersionDictUnloadOldestVersion();

      interf->errorStateReply(unloaded, 0);
      break;
    }
    
    //--------------------------------------------------------------
    case msgMarkNotUnloadable: {
      OMS_LibOmsInterface *pInterface = LVC_LibOmsInterface::Instance();
      pInterface->VersionDictMarkNotUnloadable(m_pVersionContext);

      interf->simpleReply(0);
      break;
    }
    
    //--------------------------------------------------------------
    case msgMarkUnloadable: {
      OMS_LibOmsInterface *pInterface = LVC_LibOmsInterface::Instance();
      pInterface->VersionDictMarkUnloadable(m_pVersionContext);

      interf->simpleReply(0);
      break;
    }
    
    //--------------------------------------------------------------
    case msgCreateIter: {
      OMS_LibOmsInterface *pInterface = LVC_LibOmsInterface::Instance();
      pInterface->VersionDictCreateIter(m_lockMode, &m_pIter, &m_pVersionContext);

      interf->versionDictReply(m_pVersionContext, 0, m_pIter);
      break;
    }
    
    //--------------------------------------------------------------
    case msgGetNext: {
      OMS_LibOmsInterface *pInterface = LVC_LibOmsInterface::Instance();
      pInterface->VersionDictGetNext(m_pIter, &m_pVersionContext);

      interf->versionDictReply(m_pVersionContext, 0, NULL);
      break;
    }

    //--------------------------------------------------------------
    case msgDestructIter: {
      OMS_LibOmsInterface *pInterface = LVC_LibOmsInterface::Instance();
      pInterface->VersionDictDestructIter(m_pIter);

      interf->simpleReply(0);
      break;
    }
    
   //--------------------------------------------------------------
    //default: {
    //  throw(DbpError(e_case_not_found, "LVCPS_PacketVersionDict::execute"));
    //}
  }
}
