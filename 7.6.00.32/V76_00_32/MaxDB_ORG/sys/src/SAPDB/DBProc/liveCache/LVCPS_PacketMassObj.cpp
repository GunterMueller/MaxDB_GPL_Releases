/*!
 * \file    LVC_PacketMassObj.cpp
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

#include "SAPDB/DBProc/liveCache/LVCPS_PacketMassObj.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDB/Oms/OMS_DbpError.hpp"

void LVCPS_PacketMassObj::execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf)
{
  tsp00_Int4        errCnt;
  tgg00_BasisError  pDBError[OMS_MASS_OPERATION_CNT];  

  switch (m_type) {
    //--------------------------------------------------------------
    case msgNewObj: {
      void              *ppKey[OMS_MASS_OPERATION_CNT];
      tgg01_ContainerId *ppContainerId[OMS_MASS_OPERATION_CNT];
      tgg91_TransNo      pUpdTransId[OMS_MASS_OPERATION_CNT];

      for (int i=0; i<m_objCnt; ++i){
        ppContainerId[i] = &m_pContainerId[i];
        if (m_isKeyed){
          // Convert offset of key into corresponding address in shared memory
          interf->getObjectForRead(ppKey[i], m_pKeyOffset[i]);     
        }
      }

      sink->NewObjMass(m_objCnt,
                       &m_consistentView, 
                       &ppContainerId[0], 
                       m_isVersion ? (unsigned char*)&m_versionContext : NULL, 
                       m_isKeyed ? &m_pKeyPos[0] : NULL, 
                       m_isKeyed ? &m_pKeyLen[0] : NULL, 
                       m_isKeyed ? (unsigned char**)&ppKey[0] : NULL, 
                       &m_pContObj[0],
                       &m_pOid[0], &m_pObjSeq[0], &pUpdTransId[0],
                       errCnt, &pDBError[0]); 

      interf->massObjReply(m_objCnt, NULL, NULL, &m_pOid[0],
                           &m_pObjSeq[0], &pUpdTransId[0], 0, 0, errCnt, &pDBError[0]);
      break;
    }

    //--------------------------------------------------------------
    case msgGetObj: {
      tsp00_Int4         histLogCnt = 0;
      void              *ppBody[OMS_MASS_OPERATION_CNT];
      tgg91_TransNo      pUpdTransId[OMS_MASS_OPERATION_CNT];

      for (int i=0; i<m_objCnt; ++i){
        interf->getObjectForWrite(ppBody[i], m_pBodyOffset[i]);
      }

      sink->GetObjects(m_consistentView, 
                       &m_pContainerId[0], 
                       m_isVersion ? &m_versionContext : NULL, 
                       m_objCnt,
                       &m_pOid[0], &m_pObjSeq[0], &pUpdTransId[0],
                       &m_pBodySize[0], m_doLock, m_shared, (unsigned char**)&ppBody[0],
                       histLogCnt, errCnt, &pDBError[0]); 

      interf->massObjReply(m_objCnt, &m_consistentView, &m_pContainerId[0], &m_pOid[0],
                           &m_pObjSeq[0], &pUpdTransId[0], histLogCnt, 0, errCnt, &pDBError[0]);
      break;
    }

    //--------------------------------------------------------------
    case msgDelObj: {
      tgg01_ContainerId *ppContainerId[OMS_MASS_OPERATION_CNT];

      for (int i=0; i<m_objCnt; ++i){
        ppContainerId[i] = &m_pContainerId[i];
      }

      sink->DeleteObjMass(m_objCnt,
                          &m_consistentView, 
                          &ppContainerId[0], 
                          m_isVersion ? (unsigned char*)&m_versionContext : NULL, 
                          &m_pOid[0], &m_pObjSeq[0], 
                          &m_pReused[0], &m_pContObj[0], m_clustered,
                          errCnt, &pDBError[0]); 

      interf->massObjReply(m_objCnt, NULL, NULL, NULL,
                           NULL, NULL, 0, 0, errCnt, &pDBError[0]);
      break;
    }

    //--------------------------------------------------------------
    case msgGetKeyObj: {
      tsp00_Int4         histLogCnt = 0;
      void              *ppKey[OMS_MASS_OPERATION_CNT];
      void              *ppBody[OMS_MASS_OPERATION_CNT];
      tgg91_TransNo      pUpdTransId[OMS_MASS_OPERATION_CNT];

      for (int i=0; i<m_objCnt; ++i){
        interf->getObjectForRead(ppKey[i],  m_pKeyOffset[i]);
        interf->getObjectForWrite(ppBody[i], m_pBodyOffset[i]);
      }

      sink->GetObjectsWithKey(&m_consistentView, 
                              &m_pContainerId[0], 
                              m_isVersion ? &m_versionContext : NULL, 
                              &m_pKeyLen[0],
                              m_objCnt,
                              (unsigned char**)&ppKey[0],
                              m_doLock,  m_shared,
                              &m_pBodySize[0], (unsigned char**)&ppBody[0],
                              &m_pOid[0], &m_pObjSeq[0], &pUpdTransId[0],
                              histLogCnt, errCnt, &pDBError[0]); 

      interf->massObjReply(m_objCnt, &m_consistentView, NULL, &m_pOid[0],
                           &m_pObjSeq[0], &pUpdTransId[0], histLogCnt, 0, errCnt, &pDBError[0]);
      break;
    }

    //--------------------------------------------------------------
    case msgLckObj: {
      tgg01_ContainerId *ppContainerId[OMS_MASS_OPERATION_CNT];

      int i;
      for (i=0; i<m_objCnt; ++i){
        ppContainerId[i] = &m_pContainerId[i];
      }

      sink->LockObjMass(m_objCnt,
                        &m_consistentView, 
                        &ppContainerId[0], 
                        &m_pOid[0], &m_pObjSeq[0], 
                        m_timeout,
                        errCnt, &pDBError[0]); 

      for (i=0; i<m_objCnt; ++i) {
        m_pContainerId[i] = *ppContainerId[i];
      }
      interf->massObjReply(m_objCnt, NULL, &m_pContainerId[0], NULL,
                           NULL, NULL, 0, m_timeout, errCnt, &pDBError[0]);
      break;
    }

    //--------------------------------------------------------------
    case msgUpdObj: {
      void              *ppBody[OMS_MASS_OPERATION_CNT];
      tgg01_ContainerId *ppContainerId[OMS_MASS_OPERATION_CNT];

      int i;
      for (i=0; i<m_objCnt; ++i){
        interf->getObjectForRead(ppBody[i], m_pBodyOffset[i]);
        ppContainerId[i] = &m_pContainerId[i];
      }

      sink->UpdateObjMass(m_objCnt,
                         &m_consistentView, 
                         &ppContainerId[0], 
                         m_isVersion ? (unsigned char*)&m_versionContext : NULL, 
                         &m_pOid[0], &m_pObjSeq[0], 
                         &m_pBodySize[0], (unsigned char**)&ppBody[0],
                         m_isKeyed ? &m_pKeyPos[0] : NULL, 
                         m_isKeyed ? &m_pKeyLen[0] : NULL, 
                         &m_pReused[0],  // PTS 1127661
                         m_clustered,
                         errCnt, &pDBError[0]); 

      for (i=0; i<m_objCnt; ++i){
        m_pContainerId[i] = *ppContainerId[i];
      }
      interf->massObjReply(m_objCnt, NULL, &m_pContainerId[0], NULL,
                           &m_pObjSeq[0], NULL, 0, 0, errCnt, &pDBError[0]);
      break;
    }

    //--------------------------------------------------------------
    case msgUpdVarObj: {
      void              *ppBody[OMS_MASS_OPERATION_CNT];
      tgg01_ContainerId *ppContainerId[OMS_MASS_OPERATION_CNT];

      int i;
      for (i=0; i<m_objCnt; ++i){
        interf->getObjectForRead(ppBody[i], m_pBodyOffset[i]);
        ppContainerId[i] = &m_pContainerId[i];
      }

      sink->UpdateVarObjMass(m_objCnt,
                             &m_consistentView, 
                             &ppContainerId[0], 
                             m_isVersion ? (unsigned char*)&m_versionContext : NULL, 
                             &m_pOid[0], &m_pObjSeq[0], 
                             &m_pObjSize[0],
                             &m_pBodySize[0], (unsigned char**)&ppBody[0],
                             &m_pNextOid[0],
                             &m_pContObj[0],
                             &m_pReused[0],   // PTS 1127661
                             errCnt, &pDBError[0]); 

      for (i=0; i<m_objCnt; ++i){
        m_pContainerId[i] = *ppContainerId[i];
      }
      interf->massObjReply(m_objCnt, NULL, &m_pContainerId[0], NULL,
                           &m_pObjSeq[0], NULL, 0, 0, errCnt, &pDBError[0]);
      break;
    }

    //--------------------------------------------------------------
    case msgLckUpdObj: {
      tgg01_ContainerId *ppContainerId[OMS_MASS_OPERATION_CNT];

      int i;
      for (i=0; i<m_objCnt; ++i){
        ppContainerId[i] = &m_pContainerId[i];
      }

      sink->LockUpdObjMass(m_objCnt,
                           &m_consistentView, 
                           &ppContainerId[0], 
                           &m_pOid[0], &m_pObjSeq[0], 
                           m_clustered,
                           errCnt, &pDBError[0]); 

      for (i=0; i<m_objCnt; ++i){
        m_pContainerId[i] = *ppContainerId[i];
      }
      interf->massObjReply(m_objCnt, NULL, &m_pContainerId[0], NULL,
                           &m_pObjSeq[0], NULL, 0, 0, errCnt, &pDBError[0]);
      break;
    }

    //--------------------------------------------------------------
    case msgGetContainer: {

      sink->GetContainerId(&m_pContainerId[0], 
                           m_isVersion ? &m_versionContext : NULL, 
                           m_objCnt,
                           &m_pOid[0],
                           &pDBError[0]); 

      interf->massObjReply(m_objCnt, NULL, &m_pContainerId[0], NULL,
                           NULL, NULL, 0, 0, 0, &pDBError[0]);
      break;
    }

    //--------------------------------------------------------------
    case msgLckSharedObj: {
      tgg01_ContainerId *ppContainerId[OMS_MASS_OPERATION_CNT];

      int i;
      for (i=0; i<m_objCnt; ++i){
        ppContainerId[i] = &m_pContainerId[i];
      }

      sink->LockSharedObjMass(m_objCnt,
                        &m_consistentView, 
                        &ppContainerId[0], 
                        &m_pOid[0], &m_pObjSeq[0], 
                        m_timeout,
                        errCnt, &pDBError[0]); 

      for (i=0; i<m_objCnt; ++i) {
        m_pContainerId[i] = *ppContainerId[i];
      }
      interf->massObjReply(m_objCnt, NULL, &m_pContainerId[0], NULL,
                           NULL, NULL, 0, 0, errCnt, &pDBError[0]);
      break;
    }
                   
    //--------------------------------------------------------------
    //default: {
    //  throw(DbpError(e_case_not_found, "LVCPS_PacketMassObj::execute"));
    //}
  }
}
