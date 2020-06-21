/*!
 * \file    LVC_PacketSingleObj.cpp
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

#include "SAPDB/DBProc/liveCache/LVCPS_PacketSingleObj.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDB/Oms/OMS_DbpError.hpp"

void LVCPS_PacketSingleObj::execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf)
{
  tsp00_Int2 DBError    = 0;

  switch (m_type) {
    //--------------------------------------------------------------
    case msgNewObj: {
      void         *pKey = NULL;
      tgg91_TransNo updTransId;

      interf->getObjectForRead(pKey, m_keyOffset);

      sink->NewObj((unsigned char*)&m_consistentView, 
                   (unsigned char*)&m_containerId, 
                   m_isVersion ? (unsigned char*)&m_versionContext : NULL, 
                   m_keyPos, m_keyLen, (unsigned char*)pKey , m_contObj,
                   &m_oid, (unsigned char*)&m_objSeq, &updTransId, &DBError); 

      interf->singleObjReply(NULL, NULL, m_oid,
                             m_objSeq, &updTransId, 0, 0, DBError);
      break;
    }

    //--------------------------------------------------------------
    case msgGetObj: {
      void          *pBody      = NULL;
      size_t         bodyLen    = 0;
      tsp00_Int4     histLogCnt = 0;
      tgg91_TransNo  updTransId;

      interf->getObjectForWrite(pBody, m_bodyOffset);

      sink->GetObj(m_consistentView, m_containerId, 
                   m_isVersion ? &m_versionContext : NULL,
                   m_oid, m_doLock, m_shared, m_objSeq, updTransId, m_bodySize, 
                   (unsigned char*)pBody, bodyLen, 
                   histLogCnt, DBError);

      interf->singleObjReply(&m_consistentView, &m_containerId, m_oid,
                             m_objSeq, &updTransId, bodyLen, histLogCnt, DBError);
      break;
    }
    
    //--------------------------------------------------------------
    case msgLckObj: { 
      sink->LockObj((unsigned char*)&m_consistentView,
                    (unsigned char*)&m_containerId,
                    &m_oid,
                    (unsigned char*)&m_objSeq,
                    &DBError);

      interf->errorStateReply(false, DBError);
      break;
    }
  
    //--------------------------------------------------------------
    case msgUpdObj: {
      void *pBody = NULL;
      interf->getObjectForRead(pBody, m_bodyOffset);

      sink->UpdateObj((unsigned char*)&m_consistentView,
                      (unsigned char*)&m_containerId,
                      m_isVersion ? (unsigned char*)&m_versionContext : NULL,
                      (unsigned char*)&m_oid,
                      (unsigned char*)&m_objSeq,
                      m_bodySize, 
                      (unsigned char*)pBody,
                      m_keyPos,
                      m_keyLen,
                      m_contObj,
                      m_reused,
                      &DBError);

      interf->singleObjReply(NULL, &m_containerId, m_oid,
                             m_objSeq, NULL, 0, 0, DBError);
      break;
    }
    
    //--------------------------------------------------------------
    case msgDelObj: {
      sink->DeleteObj((unsigned char*)&m_consistentView,
                      (unsigned char*)&m_containerId,
                      m_isVersion ? (unsigned char*)&m_versionContext : NULL,
                      (unsigned char*)&m_oid,
                      (unsigned char*)&m_objSeq,
                      m_reused,
                      m_contObj,
                      &DBError);

      interf->errorStateReply(false, DBError);
      break;
    }
    
    //--------------------------------------------------------------
    case msgLckUpdObj: {
      sink->LockUpdObj((unsigned char*)&m_consistentView,
                       (unsigned char*)&m_containerId,
                       &m_oid,
                       (unsigned char*)&m_objSeq,
                       &DBError);

      interf->singleObjReply(NULL, &m_containerId, m_oid,
                             m_objSeq, NULL, 0, 0, DBError);
      break;
    }
    
    //--------------------------------------------------------------
    case msgGetKeyObj: {
      tsp00_Int4  histLogCnt = 0;
      void       *pKey       = NULL;
      void       *pBody      = NULL;
      tgg91_TransNo updTransId;

      interf->getObjectForRead(pKey, m_keyOffset);
      interf->getObjectForWrite(pBody, m_bodyOffset);

      sink->GetObjWithKey((unsigned char*)&m_consistentView,
                          (unsigned char*)&m_containerId,
                          m_isVersion ? (unsigned char*)&m_versionContext : NULL,
                          m_keyLen, (unsigned char*)pKey, m_doLock, m_shared,
                          m_bodySize, (unsigned char*)pBody, &m_oid,
                          (unsigned char*)&m_objSeq, &updTransId,
                          &histLogCnt, &DBError);

      interf->singleObjReply(&m_consistentView, NULL, m_oid,
                             m_objSeq, &updTransId, 0, histLogCnt, DBError);    
      break;
    }

    //--------------------------------------------------------------
    case msgDelAll: {
      tsp00_Int4  noOfDeleted = 0;

      sink->DeleteAllObj(m_consistentView,
                         (unsigned char*)&m_containerId,
                         m_isVersion ? (unsigned char*)&m_versionContext : NULL,
                         noOfDeleted, DBError, m_oid);

      interf->singleObjReply(&m_consistentView, NULL, m_oid,
                             m_objSeq, NULL, 0, noOfDeleted, DBError);    
      break;
    }

    //--------------------------------------------------------------
    case msgUnlckObj: {
      sink->UnlockObj((unsigned char*)&m_containerId,
                      &m_oid, &DBError);

      interf->singleObjReply(NULL, &m_containerId, m_oid,
                             m_objSeq, NULL, 0, 0, DBError);    
      break;
    }

    //--------------------------------------------------------------
    case msgUnlckSharedObj: {
      sink->UnlockSharedObj((unsigned char*)&m_containerId,
                            &m_oid, &DBError);

      interf->singleObjReply(NULL, &m_containerId, m_oid,
                             m_objSeq, NULL, 0, 0, DBError);    
      break;
    }

    //--------------------------------------------------------------
    case msgIsLocked: {
      sink->IsLocked((unsigned char*)&m_containerId,
                      &m_oid, m_shared, &DBError);

      interf->errorStateReply(false, DBError);    
      break;
    }

    //--------------------------------------------------------------
    case msgIsHistUsed: {
      bool isUsed;
			OmsTypeUInt8   OldestTransNo;   /*  PTS 1130354 */
      sink->IsObjHistoryUsed(m_oid, false /*ignoreVersions*/, isUsed, DBError, OldestTransNo);
        
      interf->errorStateReply(isUsed, DBError);    
      break;
    }

    //--------------------------------------------------------------
    case msgLckSharedObj: { 
      sink->LockSharedObj((unsigned char*)&m_consistentView,
                    (unsigned char*)&m_containerId,
                    &m_oid,
                    (unsigned char*)&m_objSeq,
                    &DBError);

      interf->errorStateReply(false, DBError);
      break;
    }

    //--------------------------------------------------------------
    //default: {
    //  throw(DbpError(e_case_not_found, "LVCPS_PacketSingleObj::execute"));
    //}
  }
}
