/*!
 * \file    LVC_PacketIterator.cpp
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

#include "SAPDB/DBProc/liveCache/LVCPS_PacketIterator.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDB/Oms/OMS_DbpError.hpp"

void LVCPS_PacketIterator::execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf)
{
  tgg00_BasisError  DBError;  

  switch (m_type) {
    //--------------------------------------------------------------
    case msgGetNxtOid: {
      tsp00_Int4     histLogCnt = 0;
      SAPDB_UInt8    pOid[OMS_MASS_OPERATION_CNT];
      tgg91_PageRef  pObjSeq[OMS_MASS_OPERATION_CNT];

      sink->NextOids(m_consistentView,
                     m_containerId, 
                     m_isVersion ? &m_versionContext : NULL, 
                     true, // bOnlyHdrVarObjects
                     m_startOid,
                     m_cnt,
                     (SAPDB_UInt8*)&pOid[0], &pObjSeq[0],
                     histLogCnt,
                     &DBError); 

      interf->iteratorReply(m_cnt, &m_consistentView, 
                            &pOid[0], &pObjSeq[0],
                            histLogCnt, NULL, 0, NULL, DBError);
      break;
    }

    //--------------------------------------------------------------
    case msgGetNxtKey: {
      tsp00_Int4     histLogCnt = 0;
      SAPDB_UInt8    pOid[OMS_MASS_OPERATION_CNT];
      tgg91_PageRef  pObjSeq[OMS_MASS_OPERATION_CNT];

      sink->NextObjFromKeyIterator (m_consistentView,  // PTS 1127520
                     m_containerId, 
                     m_isVersion ? &m_versionContext : NULL, 
                     m_pIterator,
                     0, // objBodySize
                     m_keyLen, (unsigned char*)(this + 1) /*startKey*/,
                     m_cnt,
                     (SAPDB_UInt8*)&pOid[0], &pObjSeq[0],
                     NULL,  // pBody
                     histLogCnt,
                     DBError); 

      interf->iteratorReply(m_cnt, NULL, 
                            &pOid[0], &pObjSeq[0],
                            histLogCnt, NULL, 0, NULL, DBError);
      break;
    }

    //--------------------------------------------------------------
    case msgGetPrvKey: {
      tsp00_Int4     histLogCnt = 0;
      SAPDB_UInt8    pOid[OMS_MASS_OPERATION_CNT];
      tgg91_PageRef  pObjSeq[OMS_MASS_OPERATION_CNT];

      sink->PrevObjFromKeyIterator (m_consistentView, // PTS 1127520
                     m_containerId, 
                     m_isVersion ? &m_versionContext : NULL, 
                     m_pIterator,
                     0, // objBodySize
                     m_keyLen, (unsigned char*)(this + 1) /*startKey*/,
                     m_cnt,
                     (SAPDB_UInt8*)&pOid[0], &pObjSeq[0],
                     NULL,  // pBody
                     histLogCnt,
                     DBError); 

      interf->iteratorReply(m_cnt, NULL, 
                            &pOid[0], &pObjSeq[0],
                            histLogCnt, NULL, 0, NULL, DBError);
      break;
    }

    //--------------------------------------------------------------
    case msgGetVersionNxtKey: {
      SAPDB_UInt8  oid;

      sink->VersionNextKey ( 
                     m_isVersion ? &m_versionContext : NULL, 
                     m_containerId,
                     (unsigned char*)(this + 1) /*startKey*/, m_keyLen, 
                     m_inclusive,
                     ((unsigned char*)(this + 1)) + m_keyLen /*stopKey*/,
                     oid, 
                     DBError); 

      interf->iteratorReply(1, NULL, 
                            &oid, NULL,
                            0, NULL, m_keyLen, (unsigned char*)(this + 1), DBError);
      break;
    }

    //--------------------------------------------------------------
    case msgGetVersionPrvKey: {
      tsp00_Int4     histLogCnt = 0;
      SAPDB_UInt8    oid;

      sink->VersionPrevKey ( 
                     m_isVersion ? &m_versionContext : NULL, 
                     m_containerId,
                     (unsigned char*)(this + 1) /*startKey*/, m_keyLen, 
                     m_inclusive,
                     ((unsigned char*)(this + 1)) + m_keyLen /*stopKey*/,
                     oid, 
                     DBError); 

      interf->iteratorReply(1, NULL, 
                            &oid, NULL,
                            0, NULL, m_keyLen, (unsigned char*)(this + 1), DBError);
      break;
    }

    //--------------------------------------------------------------
    case msgCreateKeyIter: {
      tsp00_Int4     histLogCnt = 0;
      SAPDB_UInt8    pOid[OMS_MASS_OPERATION_CNT];
      tgg91_PageRef  pObjSeq[OMS_MASS_OPERATION_CNT];

      sink->CreateKeyRangeIterator (
                     m_consistentView,
                     m_containerId, 
                     m_isVersion ? &m_versionContext : NULL, 
                     &m_pIterator,
                     0,  // objBodySize
                     m_keyLen, 
                     (unsigned char*)(this + 1)                /*startKey*/, 
                     ((unsigned char*)(this + 1)) +   m_keyLen /*lowerKey*/,
                     ((unsigned char*)(this + 1)) + 2*m_keyLen /*upperKey*/,
                     m_cnt,
                     &pOid[0], &pObjSeq[0],
                     NULL,  // pBody
                     histLogCnt,
                     DBError,
                     m_direction); 

      interf->iteratorReply(m_cnt, &m_consistentView, 
                            &pOid[0], &pObjSeq[0],
                            histLogCnt, m_pIterator, 0, NULL, DBError);
      break;
    }

    //--------------------------------------------------------------
    case msgStartUnloadedVersionIter: {
      void          *pBody      = NULL;
      SAPDB_UInt8    oid;

      interf->getObjectForRead(pBody,  m_bodyOffset);

      sink->StartUnloadedVersionIter (
                     m_consistentView,
                     m_isVersion ? &m_versionContext : NULL, 
                     m_containerId,
                     m_keyLen, 
                     (unsigned char*)(this + 1)                /*startKey*/, 
                     ((unsigned char*)(this + 1)) +   m_keyLen /*lowerKey*/,
                     ((unsigned char*)(this + 1)) + 2*m_keyLen /*upperKey*/,
                     m_bodySize, (unsigned char*)pBody,
                     oid, 
                     DBError,
                     m_direction); 

      interf->iteratorReply(1, &m_consistentView, 
                            &oid, NULL,
                            0, NULL, 0, NULL, DBError);
      break;
    }

    //--------------------------------------------------------------
    case msgDestroyKeyIter: {
      sink->DestroyKeyRangeIterator ( m_pIterator );

      interf->simpleReply(0);
      break;
    }             

    //--------------------------------------------------------------
    //default: {
    //  throw(DbpError(e_case_not_found, "LVCPS_PacketIterator::execute"));
    //}
  }
}
