/*!
 * \file    LVCPS_LiveCacheSink.cpp
 * \author  MarkusSi
 * \brief   LiveCache Interface used by DB-Procedures in ProcServer mode.
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

#include "liveCache/LVC_ProcServerInterface.hpp"
#include "liveCache/LVC_ProcServerExecutor.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "DBProc/liveCache/LVCPS_LiveCacheSink.hpp"
#include "DBProc/liveCache/LVCPS_PacketConsoleMsg.hpp"
#include "DBProc/liveCache/LVCPS_PacketSingleObj.hpp"
#include "DBProc/liveCache/LVCPS_PacketContainer.hpp"
#include "DBProc/liveCache/LVCPS_PacketVersion.hpp"
#include "DBProc/liveCache/LVCPS_PacketSchema.hpp"
#include "DBProc/liveCache/LVCPS_PacketRegion.hpp"
#include "DBProc/liveCache/LVCPS_PacketSubtrans.hpp"
#include "DBProc/liveCache/LVCPS_PacketConsistentView.hpp"
#include "DBProc/liveCache/LVCPS_PacketMassObj.hpp"
#include "DBProc/liveCache/LVCPS_PacketIterator.hpp"
#include "DBProc/liveCache/LVCPS_PacketSequence.hpp"
#include "DBProc/liveCache/LVCPS_PacketFile.hpp"
#include "DBProc/liveCache/LVCPS_PacketABAPStream.hpp"
#include "DBProc/liveCache/LVCPS_PacketTasking.hpp"
#include "DBProc/liveCache/LVCPS_PacketSQL.hpp"
#include "DBProc/liveCache/LVCPS_PacketVersionDict.hpp"
#include "Oms/OMS_DbpError.hpp"
#include "Oms/OMS_Defines.h"
#include "Oms/OMS_CallbackInterface.hpp"

/*---------------------------------------------------------------------------*/

LVCPS_LiveCacheSink::~LVCPS_LiveCacheSink()
{
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::QueryInterface(const IID &iid,void **ppv)
{
  if (memcmp (&iid, &IID_IUnknown, sizeof (IID)) == 0)
    *ppv = (void *) this;
  else 
    if (memcmp (&iid, &IID_IliveCacheSink, sizeof (IID)) == 0)
    *ppv = (void *) this;
  else
  {
    *ppv = 0;
    return E_NOINTERFACE;
  }
  this->AddRef();
  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::ABAPRead( 
    tgg01_StreamDescriptor& descriptor,
    short&                  error)
{
  size_t                  packetSize = 0;
  size_t                  objectSize = 0;  // TODO !!!
  LVCPS_PacketABAPStream *packet     = NULL;

  // TODO

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              = LVCPS_PacketABAPStream::msgRead;
  //packet->m_dataOffset        = m_interf->getOffsetForObjectRead(buf, objectSize);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerABAPStreamRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  // TODO
  error      = replyPacket->m_error;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::ABAPWrite(
    tgg01_StreamDescriptor& descriptor,
    short                 & error)
{
  // TODO
  size_t                  packetSize = 0;
  // size_t                  objectSize = row_size * row_count;  // TODO !!!
  LVCPS_PacketABAPStream *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              = LVCPS_PacketABAPStream::msgWrite;
  //packet->m_tabHandle         = *TabHandle;
  //packet->m_rowSize           = row_size;
  //packet->m_rowCount          = row_count;
  //packet->m_dataOffset        = m_interf->getOffsetForObjectRead(lpv, objectSize);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  error   = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::AllocPacket( 
    unsigned char  **p,
    tsp00_Int4      *sz)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::CloseVersion( 
    short        *lpb_error)
{
  size_t               packetSize = 0;
  LVCPS_PacketVersion *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketVersion::msgClose;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpb_error  = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::ConsoleMsg( 
    tsp00_Int4      buflen,
    unsigned char  *lpv)
{
  size_t maxSize;
  LVCPS_PacketConsoleMsg *packet;
  m_interf->preparePacket(packet, maxSize);
  packet->m_size = sizeof(LVCPS_PacketConsoleMsg) + buflen;
  if (packet->m_size > maxSize) {
    // TODO: maybe error, since truncated?
    buflen -= packet->m_size - maxSize;
    packet->m_size = maxSize;
  }
  packet->m_length = buflen;
  packet->m_type = LVCPS_PacketConsoleMsg::msgConsole;
  memcpy(packet + 1, lpv, buflen);
  m_interf->send(packet->m_size);
  m_interf->receiveSimpleReply();
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::CreateVersion( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpVersionContext,
    short          *lpb_error)
{
  size_t               packetSize = 0;
  LVCPS_PacketVersion *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketVersion::msgCreate;
  packet->m_consistentView    = *reinterpret_cast<tgg91_TransNo*>(lpConsistentView);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerVersionRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *((tgg01_OmsVersionContext*)lpVersionContext) = replyPacket->m_versionContext;
  *lpb_error                                    = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::CreateKeyRangeIterator(
    tgg91_TransNo            &consistentView,
    tgg00_FileId             &objFile,
    tgg01_OmsVersionContext  *lpVersionContext,
    void                    **pKeyRangeIterator,
    tsp00_Int4                objBodySize,
    size_t                    keyLen,
    const void               *pStartKey,              
    const void               *pLowerKey,             
    const void               *pUpperKey,              
    tsp00_Int4               &noOfOid,
    SAPDB_UInt8               *pResultOId,  
    tgg91_PageRef            *pObjSeq,
    unsigned char            *pBody,
    tsp00_Int4               &LogReadAccesses,
    tgg00_BasisError         &e,
    OmsIterDirection          direction)
{
  size_t                 packetSize = 0;
  LVCPS_PacketIterator  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  if (sizeof(LVCPS_PacketIterator) + 3*keyLen > packetSize){
    throw (DbpError(e_buffer_too_small, "Buffer two small for package LVCPS_PacketIterator"));
  }
  packet->m_type              = LVCPS_PacketIterator::msgCreateKeyIter;
  packet->m_size              = sizeof(LVCPS_PacketIterator) + 3*keyLen;
  packet->m_cnt               = noOfOid;
  packet->m_consistentView    = consistentView;
  packet->m_containerId       = objFile;
  if (lpVersionContext != NULL){
    packet->m_versionContext  = *lpVersionContext;
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  packet->m_keyLen            = keyLen;
  packet->m_direction         = direction;
  // Copy the keys just behind the regular data of the package.
  memcpy((unsigned char*)(packet + 1),            pStartKey, keyLen);
  memcpy((unsigned char*)(packet + 1) + keyLen,   pLowerKey, keyLen);
  memcpy((unsigned char*)(packet + 1) + 2*keyLen, pUpperKey, keyLen);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerIteratorRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  noOfOid            = replyPacket->m_cnt; 
  consistentView     = replyPacket->m_consistentView;
  memcpy(&pResultOId[0],  &replyPacket->m_pOid[0],        noOfOid * sizeof(pResultOId[0])); 
  memcpy(&pObjSeq[0],     &replyPacket->m_pObjSeq[0],     noOfOid * sizeof(pObjSeq[0])); 
  *pKeyRangeIterator = replyPacket->m_pIterator;
  LogReadAccesses    = replyPacket->m_logHistRead;
  e                  = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::StartUnloadedVersionIter(
    tgg91_TransNo           &consistentView,
    tgg01_OmsVersionContext *lpVersionContext,
    tgg00_FileId            &objFile,
    size_t                   keyLen,
    void                    *pStartKey,            
    const void              *pLowerKey,
    const void              *pUpperKey,
    size_t                   ObjBodySize,           
    void                    *pObjBody,             
    SAPDB_UInt8              &ResultOId, 
    short                   &e,
    OmsIterDirection        direction)             
{
  size_t                 packetSize = 0;
  size_t                 objectSize = 0;
  LVCPS_PacketIterator  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  if (sizeof(LVCPS_PacketIterator) + 3*keyLen > packetSize){
    throw (DbpError(e_buffer_too_small, "Buffer two small for package LVCPS_PacketIterator"));
  }
  packet->m_type              = LVCPS_PacketIterator::msgStartUnloadedVersionIter;
  packet->m_size              = sizeof(LVCPS_PacketIterator) + 3*keyLen;
  packet->m_cnt               = 1;
  packet->m_consistentView    = consistentView;
  packet->m_containerId       = objFile;
  if (lpVersionContext != NULL){
    packet->m_versionContext  = *lpVersionContext;
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  packet->m_keyLen            = keyLen;
  packet->m_bodySize          = ObjBodySize;
  packet->m_bodyOffset        = m_interf->getOffsetForObjectRead(pObjBody, objectSize);
  packet->m_direction         = direction;
  // Copy the keys just behind the regular data of the package.
  memcpy((unsigned char*)(packet + 1),            pStartKey, keyLen);
  memcpy((unsigned char*)(packet + 1) + keyLen,   pLowerKey, keyLen);
  memcpy((unsigned char*)(packet + 1) + 2*keyLen, pUpperKey, keyLen);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerIteratorRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  consistentView = replyPacket->m_consistentView;
  ResultOId      = *reinterpret_cast<SAPDB_UInt8*>(replyPacket->m_pOid[0]); 
  e              = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::DeleteAllObj( 
    tgg91_TransNo&  consistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionFile,
    int            &lpDeleted,
    short          &lpb_error,
    SAPDB_UInt8     &errorOid)

{
  size_t                 packetSize = 0;
  LVCPS_PacketSingleObj *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSingleObj::msgDelAll;
  packet->m_consistentView    =  consistentView;
  packet->m_containerId       = *reinterpret_cast<tgg01_ContainerId*>(lpContainerId);
  if (lpVersionFile != NULL){
    packet->m_versionContext  = *reinterpret_cast<tgg01_OmsVersionContext*>(lpVersionFile);
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerSingleObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  consistentView = replyPacket->m_consistentView;
  lpDeleted      = replyPacket->m_counter;
  errorOid       = replyPacket->m_oid;
  lpb_error      = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::DeleteObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionContext,
    unsigned char  *lpoid,
    unsigned char  *lpobjseq,
    bool            reused,    // PTS 1127661
    bool            contObj,   // PTS 1124278
    short          *lpb_error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketSingleObj *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSingleObj::msgDelObj;
  packet->m_consistentView    = *reinterpret_cast<tgg91_TransNo*>(lpConsistentView);
  packet->m_containerId       = *reinterpret_cast<tgg01_ContainerId*>(lpContainerId);
  if (lpVersionContext != NULL){
    packet->m_versionContext  = *reinterpret_cast<tgg01_OmsVersionContext*>(lpVersionContext);
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  packet->m_oid               = *reinterpret_cast<SAPDB_UInt8*>(lpoid);
  packet->m_objSeq            = *reinterpret_cast<tgg91_PageRef*>(lpobjseq);
  packet->m_reused            = reused;
  packet->m_contObj           = contObj;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpb_error = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/
// PTS 1121449
HRESULT LVCPS_LiveCacheSink::DeleteObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    bool                    *pReused, // PTS 1127661
    bool                    *pContObj,// PTS 1124278
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  size_t                 packetSize = 0;
  LVCPS_PacketMassObj   *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketMassObj::msgDelObj;
  packet->m_objCnt            = cnt;
  packet->m_consistentView    = *pConsistentView;
  for (int i=0; i<cnt; ++i)
    packet->m_pContainerId[i] = *(ppContainerId[i]);
  if (pVersionContext != NULL){
    packet->m_versionContext  = *reinterpret_cast<tgg01_OmsVersionContext*>(pVersionContext);
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  memcpy(&packet->m_pOid[0], pOid, cnt * sizeof(SAPDB_UInt8)); 
  memcpy(&packet->m_pObjSeq[0], pObjSeq, cnt * sizeof(packet->m_pObjSeq[0]));
  memcpy(&packet->m_pReused[0], pReused, cnt * sizeof(packet->m_pReused[0]));
  memcpy(&packet->m_pContObj[0], pContObj, cnt * sizeof(packet->m_pContObj[0]));
  packet->m_clustered = clustered;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerMassObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  errCnt   = replyPacket->m_errCnt;
  memcpy(&pDBError[0], &replyPacket->m_pDBError[0], cnt * sizeof(pDBError[0]));

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::DestroyKeyRangeIterator(
  void* pKeyRangeIterator)
{
  size_t                 packetSize = 0;
  LVCPS_PacketIterator  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              = LVCPS_PacketIterator::msgDestroyKeyIter;
  packet->m_pIterator         = pKeyRangeIterator;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  m_interf->receiveSimpleReply();

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::EndConsistentView( 
    unsigned char  *lpConsistentView,
    short          *lpb_error)
{
  size_t                      packetSize = 0;
  LVCPS_PacketConsistentView *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketConsistentView::msgEnd;
  packet->m_consistentView    = *reinterpret_cast<tgg91_TransNo*>(lpConsistentView);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerConsistentViewRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *((tgg91_TransNo*)lpConsistentView)  = replyPacket->m_consistentView;
  *lpb_error                           = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::EnterCriticalSection(
  short regionId)
{
  size_t              packetSize = 0;
  LVCPS_PacketRegion *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type        =  LVCPS_PacketRegion::msgEnter;
  packet->m_regionId    = regionId;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  m_interf->receiveSimpleReply();

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::GetObj( 
    tgg91_TransNo                 &consistentView,
    tgg01_ContainerId             &containerId, 
    const tgg01_OmsVersionContext *pVersionContext,
    const SAPDB_UInt8              &oid,
    bool                           doLock,
    bool                           shared,
    tgg91_PageRef                 &objSeq,
    tgg91_TransNo                 &updTransId,
    size_t                         bodySize,
    unsigned char                 *pBody,
    size_t                        &bodyLen,
    tsp00_Int4                    &histLogCount,
    tgg00_BasisError              &e) 
{
  size_t                 packetSize = 0;
  LVCPS_PacketSingleObj *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSingleObj::msgGetObj;
  packet->m_consistentView    = consistentView;
  packet->m_containerId       = containerId;
  if (pVersionContext != NULL){
    packet->m_versionContext  = *const_cast<tgg01_OmsVersionContext*>(pVersionContext);
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  packet->m_oid               = oid;
  packet->m_doLock            = doLock;
  packet->m_shared            = shared;
  packet->m_objSeq            = objSeq;
  packet->m_bodySize          = bodySize;
  packet->m_bodyOffset        = m_interf->getOffsetForObjectRead(pBody, bodySize);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerSingleObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  consistentView = replyPacket->m_consistentView;
  containerId    = replyPacket->m_containerId;
  objSeq         = replyPacket->m_objSeq;
  updTransId     = replyPacket->m_updTransId;
  bodyLen        = replyPacket->m_bodyLen;
  histLogCount   = replyPacket->m_counter;
  e              = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/
// PTS 1120478 
HRESULT LVCPS_LiveCacheSink::GetObjects(
    tgg91_TransNo                 &consistentView,
    tgg01_ContainerId             *pContainerId, 
    const tgg01_OmsVersionContext *pVersionContext,
    tsp00_Int4                     noOfOid,
    const SAPDB_UInt8              *pOid,
    tgg91_PageRef                 *pObjVers,
    tgg91_TransNo                 *pUpdTransId,
    size_t                        *pBodySize,
    bool                           doLock,
    bool                           shared,
    unsigned char                **ppObj,
    tsp00_Int4                    &histLogCount,
    tsp00_Int4                    &errorCount,
    tgg00_BasisError              *e)
{
  size_t                 packetSize = 0;
  LVCPS_PacketMassObj   *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketMassObj::msgGetObj;
  packet->m_consistentView    =  consistentView;
  memcpy(&packet->m_pContainerId[0], pContainerId, noOfOid * sizeof(packet->m_pContainerId[0]));
  if (pVersionContext != NULL){
    packet->m_versionContext  = *pVersionContext;
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  packet->m_objCnt = noOfOid;
  memcpy(&packet->m_pOid[0], pOid, noOfOid * sizeof(SAPDB_UInt8)); 
  memcpy(&packet->m_pObjSeq[0], pObjVers, noOfOid * sizeof(packet->m_pObjSeq[0]));
  memcpy(&packet->m_pBodySize[0], pBodySize, noOfOid * sizeof(packet->m_pBodySize[0]));
  packet->m_doLock = doLock;
  packet->m_shared = shared;
  for(int i=0; i<noOfOid; ++i)
    packet->m_pBodyOffset[i] =  m_interf->getOffsetForObjectRead(ppObj[i], pBodySize[i]);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerMassObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  consistentView = replyPacket->m_consistentView;
  memcpy(&pContainerId[0], &replyPacket->m_pContainerId[0], noOfOid * sizeof(pContainerId[0]));
  memcpy(&pObjVers[0],     &replyPacket->m_pObjSeq[0],      noOfOid * sizeof(pObjVers[0])); 
  memcpy(&pUpdTransId[0],  &replyPacket->m_pUpdTransId[0],  noOfOid * sizeof(pUpdTransId[0])); 
  histLogCount   = replyPacket->m_counter;
  errorCount     = replyPacket->m_errCnt;
  memcpy(&e[0], &replyPacket->m_pDBError[0], noOfOid * sizeof(e[0]));

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::GetObjWithKey( 
    unsigned char  *lpConsistentView,
    unsigned char  *lptree,
    unsigned char  *lpVersionContext,
    size_t          keylen,
    unsigned char  *lpkey,
    bool            doLock,
    bool            shared,
    size_t          bodysize,
    unsigned char  *lpbody,
    SAPDB_UInt8     *lpoid,
    unsigned char  *lpobjseq,
    tgg91_TransNo  *pUpdTransId,
    int            *hist_log_count,
    short          *lpb_error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketSingleObj *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSingleObj::msgGetKeyObj;
  packet->m_consistentView    = *reinterpret_cast<tgg91_TransNo*>(lpConsistentView);
  packet->m_containerId       = *reinterpret_cast<tgg01_ContainerId*>(lptree);
  if (lpVersionContext != NULL){
    packet->m_versionContext  = *reinterpret_cast<tgg01_OmsVersionContext*>(lpVersionContext);
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  packet->m_keyLen            =  keylen;
  packet->m_keyOffset         =  m_interf->getOffsetForObjectWrite(lpkey, keylen);
  packet->m_doLock            =  doLock;
  packet->m_shared            =  shared;
  packet->m_bodySize          =  bodysize;
  packet->m_bodyOffset        =  m_interf->getOffsetForObjectRead(lpbody, bodysize);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerSingleObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *((tgg91_TransNo*)lpConsistentView) = replyPacket->m_consistentView;
  *lpoid                              = replyPacket->m_oid;
  *((tgg91_PageRef*)lpobjseq)         = replyPacket->m_objSeq;
  *pUpdTransId                        = replyPacket->m_updTransId;
  *hist_log_count                     = replyPacket->m_counter;
  *lpb_error                          = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/
// PTS 1122194
HRESULT LVCPS_LiveCacheSink::GetObjectsWithKey( 
    tgg91_TransNo            *pConsistentView,
    tgg01_ContainerId        *pContainerId,
    tgg01_OmsVersionContext  *pVersionContext,
    size_t                   *pKeyLen,
    tsp00_Int4                noOfObj,
    unsigned char           **ppKey,
    bool                      doLock,
    bool                      shared,
    size_t                   *pBodySize,
    unsigned char           **ppBody,
    SAPDB_UInt8               *pOid,
    tgg91_PageRef            *pObjSeq,
    tgg91_TransNo            *pUpdTransId,
    tsp00_Int4               &histLogCnt,
    tsp00_Int4               &errorCnt,
    tgg00_BasisError         *pDBError)
{
  size_t                 packetSize = 0;
  LVCPS_PacketMassObj   *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketMassObj::msgGetKeyObj;
  packet->m_objCnt            = noOfObj;
  packet->m_consistentView    = *pConsistentView;
  memcpy(&packet->m_pContainerId[0], pContainerId, noOfObj * sizeof(packet->m_pContainerId[0]));
  if (pVersionContext != NULL){
    packet->m_versionContext  = *pVersionContext;
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  memcpy(&packet->m_pKeyLen[0], &pKeyLen[0], noOfObj * sizeof(packet->m_pKeyLen[0]));
  int i;
  for(i=0; i<noOfObj; ++i)
    packet->m_pKeyOffset[i] =  m_interf->getOffsetForObjectWrite(ppKey[i], pKeyLen[i]);
  packet->m_doLock = doLock;
  packet->m_shared = shared;
  memcpy(&packet->m_pBodySize[0], pBodySize, noOfObj * sizeof(packet->m_pBodySize[0]));
  for(i=0; i<noOfObj; ++i)
    packet->m_pBodyOffset[i] =  m_interf->getOffsetForObjectRead(ppBody[i], pBodySize[i]);
  
  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerMassObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  memcpy(pConsistentView, &replyPacket->m_consistentView, sizeof(tgg91_TransNo));
  memcpy(&pOid[0],        &replyPacket->m_pOid[0],        noOfObj * sizeof(SAPDB_UInt8)); 
  memcpy(&pObjSeq[0],     &replyPacket->m_pObjSeq[0],     noOfObj * sizeof(pObjSeq[0])); 
  memcpy(&pUpdTransId[0], &replyPacket->m_pUpdTransId[0], noOfObj * sizeof(pUpdTransId[0])); 
  histLogCnt   = replyPacket->m_counter;
  errorCnt     = replyPacket->m_errCnt;
  memcpy(&pDBError[0], &replyPacket->m_pDBError[0], noOfObj * sizeof(pDBError[0]));

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::HResultError( 
  HRESULT h_error)
{
  size_t                  maxSize = 0;
  LVCPS_PacketConsoleMsg *packet  = NULL;

  // Build request package
  m_interf->preparePacket(packet, maxSize);
  packet->m_type              = LVCPS_PacketConsoleMsg::msgHError;
  packet->m_hError            = h_error;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  m_interf->receiveSimpleReply();

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::IsLocked( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8     *lpoid,
    bool            shared,
    short          *lpb_error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketSingleObj *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSingleObj::msgIsLocked;
  packet->m_containerId       = *reinterpret_cast<tgg01_ContainerId*>(lpContainerId);
  packet->m_oid               = *lpoid;
  packet->m_shared            =  shared;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpb_error  = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::LeaveCriticalSection(
  short regionId)
{
  size_t              packetSize = 0;
  LVCPS_PacketRegion *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type        =  LVCPS_PacketRegion::msgLeave;
  packet->m_regionId    = regionId;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  m_interf->receiveSimpleReply();

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::LockObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    SAPDB_UInt8     *lpoid,
    unsigned char  *lpobjseq,
    short          *lpb_error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketSingleObj *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSingleObj::msgLckObj;
  packet->m_consistentView    = *reinterpret_cast<tgg91_TransNo*>(lpConsistentView);
  packet->m_containerId       = *reinterpret_cast<tgg01_ContainerId*>(lpContainerId);
  packet->m_oid               = *lpoid;
  packet->m_objSeq            = *reinterpret_cast<tgg91_PageRef*>(lpobjseq);
  
  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpb_error = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

// PTS 1121449
HRESULT LVCPS_LiveCacheSink::LockObjMass(
    tsp00_Int4              &cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    tsp00_Int8              &timeout,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  size_t                 packetSize = 0;
  LVCPS_PacketMassObj   *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketMassObj::msgLckObj;
  packet->m_objCnt            =  cnt;
  packet->m_consistentView    = *pConsistentView;
  for (int i=0; i<cnt; ++i)
    packet->m_pContainerId[i] = *(ppContainerId[i]);
  memcpy(&packet->m_pOid[0], pOid, cnt * sizeof(SAPDB_UInt8)); 
  memcpy(&packet->m_pObjSeq[0], pObjSeq, cnt * sizeof(packet->m_pObjSeq[0]));
  packet->m_timeout           = timeout;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerMassObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  cnt      = replyPacket->m_objCnt;
  timeout  = replyPacket->m_timeout;
  errCnt   = replyPacket->m_errCnt;
  memcpy(&pDBError[0], &replyPacket->m_pDBError[0], cnt * sizeof(pDBError[0]));

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::MultiPurpose( 
    unsigned char   MessType,
    unsigned char   MessType2,
    tsp00_Int4     *lpbuflen,
    unsigned char  *lpv,
    short          *lpb_error)
{
  switch (MessType) {
  case m_show:
    {
      size_t maxSize;
      LVCPS_PacketConsoleMsg *packet;
      m_interf->preparePacket(packet, maxSize);
      size_t buflen = *lpbuflen;
      packet->m_size = sizeof(LVCPS_PacketConsoleMsg) + buflen;
      if (packet->m_size > maxSize) {
        // TODO: maybe error, since truncated?
        buflen -= packet->m_size - maxSize;
        packet->m_size = maxSize;
      }
      packet->m_length = buflen;
      packet->m_type = (MessType2 == mm_log) ? 
        LVCPS_PacketConsoleMsg::msgOpMsg : LVCPS_PacketConsoleMsg::msgOpError;
      memcpy(packet + 1, lpv, buflen);
      m_interf->send(packet->m_size);
      m_interf->receiveSimpleReply();
      return S_OK;
    }
  }

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::NewConsistentView( 
    bool            isOmsVersion,             /* PTS 1130354 */
    unsigned char  *lpConsistentView,
    short          *lpb_error)
{
  size_t                      packetSize = 0;
  LVCPS_PacketConsistentView *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketConsistentView::msgNew;
  packet->m_consistentView    = *reinterpret_cast<tgg91_TransNo*>(lpConsistentView);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerConsistentViewRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *((tgg91_TransNo*)lpConsistentView)  = replyPacket->m_consistentView;
  *lpb_error                           = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::NewObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionContext,
    size_t          KeyPos,
    size_t          KeyColSize,
    unsigned char  *KeyColumn,
    bool            contObj,    // PTS 1124278
    SAPDB_UInt8     *lpoid,
    unsigned char  *lpobjseq,
    tgg91_TransNo  *pUpdTransId, 
    short          *lpb_error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketSingleObj *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSingleObj::msgNewObj;
  packet->m_consistentView    = *reinterpret_cast<tgg91_TransNo*>(lpConsistentView);
  packet->m_containerId       = *reinterpret_cast<tgg01_ContainerId*>(lpContainerId);
  if (lpVersionContext != NULL){
    packet->m_versionContext  = *reinterpret_cast<tgg01_OmsVersionContext*>(lpVersionContext);
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  packet->m_keyPos            =  KeyPos;
  packet->m_keyLen            =  KeyColSize; 
  packet->m_keyOffset         =  m_interf->getOffsetForObjectWrite(KeyColumn, KeyColSize);
  packet->m_contObj           =  contObj;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerSingleObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpoid                       = replyPacket->m_oid;
  *((tgg91_PageRef*)lpobjseq)  = replyPacket->m_objSeq;
  *pUpdTransId                 = replyPacket->m_updTransId;
  *lpb_error                   = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/
// PTS 1121449
HRESULT LVCPS_LiveCacheSink::NewObjMass( 
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    size_t                  *pKeyPos,
    size_t                  *pKeyColSize,
    unsigned char          **ppKeyColumn, 
    bool                    *pContObj,    // PTS 1124278
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    tgg91_TransNo           *pUpdTransId,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  size_t                 packetSize = 0;
  LVCPS_PacketMassObj   *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketMassObj::msgNewObj;
  packet->m_objCnt            =  cnt;
  packet->m_consistentView    = *pConsistentView;
  int i;
  for (i=0; i<cnt; ++i)
    packet->m_pContainerId[i] = *ppContainerId[i];
  if (pVersionContext != NULL){
    packet->m_versionContext  = *reinterpret_cast<tgg01_OmsVersionContext*>(pVersionContext);
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  if (pKeyPos == NULL || pKeyColSize == NULL || ppKeyColumn == NULL)
    packet->m_isKeyed = false;
  else
    packet->m_isKeyed = true;
  if (packet->m_isKeyed){
    memcpy(&packet->m_pKeyPos[0], pKeyPos, cnt * sizeof(packet->m_pKeyPos[0]));
    memcpy(&packet->m_pKeyLen[0], pKeyColSize, cnt * sizeof(packet->m_pKeyLen[0])); 
    for(i=0; i<cnt; ++i)
      packet->m_pKeyOffset[i] =  m_interf->getOffsetForObjectWrite(ppKeyColumn[i], pKeyColSize[i]);
  }
  memcpy(&packet->m_pContObj[0], pContObj, cnt * sizeof(packet->m_pContObj[0]));

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerMassObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  memcpy(&pOid[0],        &replyPacket->m_pOid[0],        cnt * sizeof(pOid[0]));
  memcpy(&pObjSeq[0],     &replyPacket->m_pObjSeq[0],     cnt * sizeof(pObjSeq[0]));
  memcpy(&pUpdTransId[0], &replyPacket->m_pUpdTransId[0], cnt * sizeof(pUpdTransId[0]));
  memcpy(&pDBError[0],    &replyPacket->m_pDBError[0],    cnt * sizeof(pDBError[0]));
  errCnt = replyPacket->m_errCnt;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::NextObjFromKeyIterator (
    const tgg91_TransNo     &ConsistentView,  // PTS 1127520
    tgg01_ContainerId       &ObjFile,
    tgg01_OmsVersionContext *VersionContext,
    void                    *pKeyIterator,
    tsp00_Int4               objBodySize,
    size_t                   KeyLen,
    void                    *pRestartKey, 
    tsp00_Int4              &NoOfOId,
    SAPDB_UInt8              *pResultOId, 
    tgg91_PageRef           *pObjVers,
    unsigned char           *pBody,
    tsp00_Int4              &objHistReadCount,
    tgg00_BasisError        &e)
{
  size_t                 packetSize = 0;
  LVCPS_PacketIterator  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  if (sizeof(LVCPS_PacketIterator) + KeyLen > packetSize){
    throw (DbpError(e_buffer_too_small, "Buffer two small for package LVCPS_PacketIterator"));
  }
  packet->m_type              = LVCPS_PacketIterator::msgGetNxtKey;
  packet->m_size              = sizeof(LVCPS_PacketIterator) + KeyLen;
  packet->m_cnt               = NoOfOId;
  packet->m_consistentView    = ConsistentView;  // PTS 1127520
  packet->m_containerId       = ObjFile;
  if (VersionContext != NULL){
    packet->m_versionContext  = *VersionContext;
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  packet->m_keyLen            = KeyLen;
  packet->m_pIterator         = pKeyIterator;
  // Copy the key just behind the regular data of the package.
  memcpy((unsigned char*)(packet + 1), pRestartKey, KeyLen);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerIteratorRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  NoOfOId              = replyPacket->m_cnt; 
  memcpy(&pResultOId[0],  &replyPacket->m_pOid[0],        NoOfOId * sizeof(pResultOId[0])); 
  memcpy(&pObjVers[0],    &replyPacket->m_pObjSeq[0],     NoOfOId * sizeof(pObjVers[0])); 
  objHistReadCount     = replyPacket->m_logHistRead;
  e                    = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::VersionNextKey(
    tgg01_OmsVersionContext  *VersionContext,
    const tgg01_ContainerId  &ObjFile,
    void                     *pKey,
    size_t                    KeyLen,
    bool                      Inclusive,
    const void               *pStopKey,
    SAPDB_UInt8               &Oid,
    tgg00_BasisError         &e)
{
  size_t                 packetSize = 0;
  LVCPS_PacketIterator  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  if (sizeof(LVCPS_PacketIterator) + 2*KeyLen > packetSize){
    throw (DbpError(e_buffer_too_small, "Buffer two small for package LVCPS_PacketIterator"));
  }
  packet->m_type              = LVCPS_PacketIterator::msgGetVersionNxtKey;
  packet->m_size              = sizeof(LVCPS_PacketIterator) + 2*KeyLen;
  packet->m_cnt               = 1;
  packet->m_containerId       = ObjFile;
  if (VersionContext != NULL){
    packet->m_versionContext  = *VersionContext;
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  packet->m_keyLen            = KeyLen;
  packet->m_inclusive         = Inclusive;
  // Copy the keys just behind the regular data of the package.
  memcpy((unsigned char*)(packet + 1),          pKey,     KeyLen);
  memcpy((unsigned char*)(packet + 1) + KeyLen, pStopKey, KeyLen);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerIteratorRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  memcpy(pKey, (unsigned char*)(replyPacket+1), KeyLen);
  Oid        = *reinterpret_cast<SAPDB_UInt8*>(replyPacket->m_pOid[0]); 
  e          = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::PrevObjFromKeyIterator (
    const tgg91_TransNo     &ConsistentView,  // PTS 1127520
    tgg01_ContainerId       &ObjFile,
    tgg01_OmsVersionContext *VersionContext,
    void                    *pKeyIterator,
    tsp00_Int4               objBodySize,
    size_t                   KeyLen,
    void                    *pRestartKey, 
    tsp00_Int4              &NoOfOId,
    SAPDB_UInt8              *pResultOId, 
    tgg91_PageRef           *pObjVers,
    unsigned char           *pBody,
    tsp00_Int4              &objHistReadCount,
    tgg00_BasisError        &e)
{
  size_t                 packetSize = 0;
  LVCPS_PacketIterator  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  if (sizeof(LVCPS_PacketIterator) + KeyLen > packetSize){
    throw (DbpError(e_buffer_too_small, "Buffer two small for package LVCPS_PacketIterator"));
  }
  packet->m_type              = LVCPS_PacketIterator::msgGetPrvKey;
  packet->m_size              = sizeof(LVCPS_PacketIterator) + KeyLen;
  packet->m_cnt               = NoOfOId;
  packet->m_consistentView    = ConsistentView;  // PTS 1127520
  packet->m_containerId       = ObjFile;
  if (VersionContext != NULL){
    packet->m_versionContext  = *VersionContext;
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  packet->m_keyLen            = KeyLen;
  packet->m_pIterator         = pKeyIterator;
  // Copy the key just behind the regular data of the package.
  memcpy((unsigned char*)(packet + 1), pRestartKey, KeyLen);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerIteratorRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  NoOfOId          = replyPacket->m_cnt; 
  memcpy(&pResultOId[0],  &replyPacket->m_pOid[0],        NoOfOId * sizeof(pResultOId[0])); 
  memcpy(&pObjVers[0],    &replyPacket->m_pObjSeq[0],     NoOfOId * sizeof(pObjVers[0])); 
  objHistReadCount = replyPacket->m_logHistRead;
  e                = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::VersionPrevKey(
    tgg01_OmsVersionContext  *VersionContext,
    const tgg01_ContainerId  &ObjFile,
    void                     *pKey,
    size_t                    KeyLen,
    bool                      Inclusive,
    const void               *pStopKey,
    SAPDB_UInt8               &Oid,
    tgg00_BasisError         &e)
{
  size_t                 packetSize = 0;
  LVCPS_PacketIterator  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  if (sizeof(LVCPS_PacketIterator) + 2*KeyLen > packetSize){
    throw (DbpError(e_buffer_too_small, "Buffer two small for package LVCPS_PacketIterator"));
  }
  packet->m_size              = sizeof(LVCPS_PacketIterator) + 2*KeyLen;
  packet->m_type              = LVCPS_PacketIterator::msgGetVersionPrvKey;
  packet->m_cnt               = 1;
  packet->m_containerId       = ObjFile;
  if (VersionContext != NULL){
    packet->m_versionContext  = *VersionContext;
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  packet->m_keyLen            = KeyLen;
  packet->m_inclusive         = Inclusive;
  // Copy the keys just behind the regular data of the package.
  memcpy((unsigned char*)(packet + 1),          pKey,     KeyLen);
  memcpy((unsigned char*)(packet + 1) + KeyLen, pStopKey, KeyLen);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerIteratorRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  memcpy(pKey, (unsigned char*)(replyPacket+1), KeyLen);
  Oid        = *reinterpret_cast<SAPDB_UInt8*>(replyPacket->m_pOid[0]); 
  e          = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::CreateContainer(
    tsp00_Int4           SchemaId,
    GUID                *lpGUID,
    tsp00_KnlIdentifier *lpClassName,
    size_t               size,
    OmsContainerNo       ContainerNo,
    size_t               keyPos,          // PTS 1122540
    size_t               keyLen,          // PTS 1122540
    bool                 keyPartitioned,  // PTS 1122540
    bool                 useCachedKeys,
    bool                 isVarObject,
    bool                 useInternalTrans,
    bool                 noWait,
    unsigned char       *lpContainerId,
    short               *lpb_error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketContainer *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type            =  LVCPS_PacketContainer::msgCreateContainer;
  packet->m_schemaId        =  SchemaId;
  packet->m_guid            = *lpGUID;
  packet->m_className       = *lpClassName;
  packet->m_containerNo     =  ContainerNo;
  packet->m_size            =  size;
  packet->m_keyPos          =  keyPos;
  packet->m_keyLen          =  keyLen;
  packet->m_keyPartitioned  =  keyPartitioned;
  packet->m_useCachedKeys   =  useCachedKeys;
  packet->m_isVarObject     =  isVarObject;
  packet->m_useInternalTrans = useInternalTrans;
  packet->m_noWait          = noWait;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerContainerRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *((tgg01_ContainerId*)lpContainerId) = replyPacket->m_containerId;
  *lpb_error                           = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::ResetVersion( 
    unsigned char *lpVersionContext,
    short          &error)
{
  size_t               packetSize = 0;
  LVCPS_PacketVersion *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketVersion::msgReset;
  packet->m_versionContext    = *reinterpret_cast<tgg01_OmsVersionContext*>(lpVersionContext);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  error  = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::SetError( 
    short                 errorno,
    tsp00_Int4            buflen,
    const unsigned char  *lpv,
    tsp00_CodeType        codeType )
{
  size_t                  maxSize = 0;
  LVCPS_PacketConsoleMsg *packet  = NULL;

  // Build request package
  m_interf->preparePacket(packet, maxSize);
  packet->m_type              = LVCPS_PacketConsoleMsg::msgSetError;
  packet->m_size = sizeof(LVCPS_PacketConsoleMsg) + buflen;
  if (packet->m_size > maxSize) {
    throw;  // TODO
  }
  packet->m_errorNo           = errorno;
  packet->m_length            = buflen;
  packet->m_codeType          = codeType;
  memcpy((unsigned char*)(packet+1), lpv, buflen);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  m_interf->receiveSimpleReply();

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::SQLRequest( 
    tsp00_Int4      buflen,
    unsigned char  *lpv)
{
  size_t           maxSize = 0;
  LVCPS_PacketSQL *packet  = NULL;

  // Build request package
  m_interf->preparePacket(packet, maxSize);
  packet->m_type     = LVCPS_PacketSQL::msgRequest;
  packet->m_length   = buflen;
  packet->m_offset   = m_interf->getOffsetForObjectRead(lpv, buflen);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  m_interf->receiveSimpleReply();

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::SubtransCommit( 
  short  *lpb_error)
{
  size_t              packetSize = 0;
  LVCPS_PacketRegion *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type        =  LVCPS_PacketSubtrans::msgCommit;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpb_error   = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::SubtransRollback( 
  short  *lpb_error)
{
  size_t              packetSize = 0;
  LVCPS_PacketRegion *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type        =  LVCPS_PacketSubtrans::msgRollback;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpb_error   = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::SubtransStart( 
  short  *lpb_error)
{
  size_t              packetSize = 0;
  LVCPS_PacketRegion *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type        =  LVCPS_PacketSubtrans::msgStart;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpb_error   = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::UnlockObj( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8     *lpoid,
    short          *lpb_error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketSingleObj *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSingleObj::msgUnlckObj;
  packet->m_containerId       = *reinterpret_cast<tgg01_ContainerId*>(lpContainerId);
  packet->m_oid               = *lpoid;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerSingleObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *((tgg01_ContainerId*)lpContainerId) = replyPacket->m_containerId;
  *lpb_error                           = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::UnlockSharedObj( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8     *lpoid,
    short          *lpb_error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketSingleObj *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSingleObj::msgUnlckSharedObj;
  packet->m_containerId       = *reinterpret_cast<tgg01_ContainerId*>(lpContainerId);
  packet->m_oid               = *lpoid;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerSingleObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *((tgg01_ContainerId*)lpContainerId) = replyPacket->m_containerId;
  *lpb_error                           = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::UpdateObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionContext,
    unsigned char  *lpoid,
    unsigned char  *lpobjseq,
    size_t          bodysize,
    unsigned char  *lpbody,
    size_t          keyPos,            // PTS 1122540
    size_t          keyLen,            // PTS 1122540
    bool            contObj,           // PTS 1124278
    bool            reused,            // PTS 1127661
    short          *lpb_error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketSingleObj *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSingleObj::msgUpdObj;
  packet->m_consistentView    = *reinterpret_cast<tgg91_TransNo*>(lpConsistentView);
  packet->m_containerId       = *reinterpret_cast<tgg01_ContainerId*>(lpContainerId);
  if (lpVersionContext != NULL){
    packet->m_versionContext  = *reinterpret_cast<tgg01_OmsVersionContext*>(lpVersionContext);
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  packet->m_oid               = *reinterpret_cast<SAPDB_UInt8*>(lpoid);
  packet->m_objSeq            = *reinterpret_cast<tgg91_PageRef*>(lpobjseq);
  packet->m_bodySize          =  bodysize;
  packet->m_bodyOffset        =  m_interf->getOffsetForObjectWrite(lpbody, bodysize);
  packet->m_keyPos            =  keyPos;
  packet->m_keyLen            =  keyLen;
  packet->m_contObj           =  contObj;
  packet->m_reused            =  reused;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerSingleObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *((tgg01_ContainerId*)lpContainerId) = replyPacket->m_containerId;
  *((tgg91_PageRef*)lpobjseq)          = replyPacket->m_objSeq;
  *lpb_error                           = replyPacket->m_DBError;

  return S_OK;
}

/*----------------------------------------------------------------------*/
// PTS 1121449
HRESULT LVCPS_LiveCacheSink::UpdateObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    size_t                  *pBodySize,
    unsigned char          **ppBody,
    size_t                  *pKeyPos,       // PTS 1122540
    size_t                  *pKeyLen,       // PTS 1122540
    bool                    *pReused,       // PTS 1127661
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  int                    i = 0;
  size_t                 packetSize = 0;
  LVCPS_PacketMassObj   *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketMassObj::msgUpdObj;
  packet->m_objCnt            = cnt;
  packet->m_consistentView    = *pConsistentView;
  for (i=0; i<cnt; ++i)
    packet->m_pContainerId[i] = *ppContainerId[i];
  if (pVersionContext != NULL){
    packet->m_versionContext  = *reinterpret_cast<tgg01_OmsVersionContext*>(pVersionContext);
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  memcpy(&packet->m_pOid[0], &pOid[0], cnt * sizeof(SAPDB_UInt8)); 
  memcpy(&packet->m_pObjSeq[0], &pObjSeq[0], cnt * sizeof(packet->m_pObjSeq[0]));
  memcpy(&packet->m_pBodySize[0], &pBodySize[0], cnt * sizeof(packet->m_pBodySize[0]));
  for(i=0; i<cnt; ++i)
    packet->m_pBodyOffset[i] =  m_interf->getOffsetForObjectWrite(ppBody[i], pBodySize[i]);
  if (pKeyPos == NULL || pKeyLen == NULL)
    packet->m_isKeyed = false;
  else
    packet->m_isKeyed = true;
  if (packet->m_isKeyed){
    memcpy(&packet->m_pKeyPos[0], &pKeyPos[0], cnt * sizeof(packet->m_pKeyPos[0]));
    memcpy(&packet->m_pKeyLen[0], &pKeyLen[0], cnt * sizeof(packet->m_pKeyLen[0]));
  }
  memcpy(&packet->m_pReused[0], &pReused[0], cnt * sizeof(packet->m_pReused[0]));  // PTS 1127661
  packet->m_clustered = clustered;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerMassObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  for (i=0; i<cnt; ++i)
    *ppContainerId[i] = replyPacket->m_pContainerId[i];
  memcpy(&pObjSeq[0], &replyPacket->m_pObjSeq[0], cnt * sizeof(pObjSeq[0]));
  memcpy(&pDBError[0], &replyPacket->m_pDBError[0], cnt * sizeof(pDBError[0]));
  errCnt     = replyPacket->m_errCnt;

	return S_OK;
}


/*----------------------------------------------------------------------*/
// PTS 1121449
HRESULT LVCPS_LiveCacheSink::UpdateVarObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    size_t                  *pObjSize,
    size_t                  *pBodySize,
    unsigned char          **ppBody,
    SAPDB_UInt8              *pNextOid,
    bool                    *pContObj,  // PTS 1124278
    bool                    *pReused,   // PTS 1127661
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  int                    i          = 0;
  size_t                 packetSize = 0;
  LVCPS_PacketMassObj   *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketMassObj::msgUpdVarObj;
  packet->m_objCnt            = cnt;
  packet->m_consistentView    = *pConsistentView;
  for (i=0; i<cnt; ++i)
    packet->m_pContainerId[i] = *ppContainerId[i];
  if (pVersionContext != NULL){    
    packet->m_versionContext  = *reinterpret_cast<tgg01_OmsVersionContext*>(pVersionContext);
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  memcpy(&packet->m_pOid[0], &pOid[0], cnt * sizeof(SAPDB_UInt8)); 
  memcpy(&packet->m_pObjSeq[0], &pObjSeq[0], cnt * sizeof(packet->m_pObjSeq[0]));
  memcpy(&packet->m_pObjSize[0], &pObjSize[0], cnt * sizeof(packet->m_pObjSize[0]));
  memcpy(&packet->m_pBodySize[0], &pBodySize[0], cnt * sizeof(packet->m_pBodySize[0]));
  for(i=0; i<cnt; ++i)
    packet->m_pBodyOffset[i] =  m_interf->getOffsetForObjectWrite(ppBody[i], pBodySize[i]);
  memcpy(&packet->m_pNextOid[0], &pNextOid[0], cnt * sizeof(SAPDB_UInt8));
  memcpy(&packet->m_pContObj[0], &pContObj[0], cnt * sizeof(bool));
  memcpy(&packet->m_pReused[0], &pReused[0], cnt * sizeof(packet->m_pReused[0]));  // PTS 1127661

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerMassObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  for (i=0; i<cnt; ++i)
    *ppContainerId[i] = replyPacket->m_pContainerId[i];
  memcpy(&pObjSeq[0], &replyPacket->m_pObjSeq[0], cnt * sizeof(pObjSeq[0]));
  memcpy(&pDBError[0], &replyPacket->m_pDBError[0], cnt * sizeof(pDBError[0]));
  errCnt     = replyPacket->m_errCnt;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::Vtrace( 
    tsp00_Int4      buflen,
    unsigned char  *lpv)
{
  size_t                  maxSize = 0;
  LVCPS_PacketConsoleMsg *packet  = NULL;

  // Build request package
  m_interf->preparePacket(packet, maxSize);
  packet->m_type    = LVCPS_PacketConsoleMsg::msgVTrace;
  packet->m_size    = sizeof(LVCPS_PacketConsoleMsg) + buflen;
  if (packet->m_size > maxSize) {
    throw; // TODO
  }
  packet->m_length  = buflen;
  memcpy((unsigned char*)(packet+1), lpv, buflen);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  m_interf->receiveSimpleReply();

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::GetDefaultContext(
    void         **p, 
    pasbool      **pCancelByte,
    bool          &optimizedStreamCommunication, /* PTS 1130281 */
    tsp00_TaskId  &taskId /* PTS 1110315 */)
{
  *p = m_pDefaultContext;
  *pCancelByte = m_pCancelByte;
  optimizedStreamCommunication = false; /* TODO */
  taskId = 1 /* TODO: session ID */;
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::SetDefaultContext(
  void* p)
{
  m_pDefaultContext = p;
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::OpenFile(
    char        *lpFileName,
    tsp00_Uint4  FileSize,
    boolean      doAppend, 
    tsp00_Int4  *FileHandle,
    short       *lpb_error)
{
  size_t             packetSize = 0;
  LVCPS_PacketFile  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  int len = strlen(lpFileName);
  if (sizeof(LVCPS_PacketFile) + len + 1 > packetSize){
    throw (DbpError(e_buffer_too_small, "Buffer two small for package LVCPS_PacketFile"));
  }
  packet->m_type              = LVCPS_PacketFile::msgOpen;
  packet->m_size              = sizeof(LVCPS_PacketFile) + len + 1;
  packet->m_fileSize          = FileSize;
  packet->m_doAppend          = doAppend;
  // Copy filename just behind the package
  strcpy((char*)(this+1), lpFileName);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerFileRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *FileHandle = replyPacket->m_fileHandle;
  *lpb_error  = replyPacket->m_error;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::WriteFile(
    tsp00_Int4 FileHandle,
    char      *msg,
    short     *lpb_error) 
{
  size_t             packetSize = 0;
  LVCPS_PacketFile  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  int len = strlen(msg);
  if (sizeof(LVCPS_PacketFile) + len + 1 > packetSize){
    throw (DbpError(e_buffer_too_small, "Buffer two small for package LVCPS_PacketFile"));
  }
  packet->m_type              = LVCPS_PacketFile::msgWrite;
  packet->m_size              = sizeof(LVCPS_PacketFile) + len + 1;
  packet->m_fileHandle        = FileHandle;
  // Copy msg just behind the package
  strcpy((char*)(this+1), msg);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpb_error  = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::CloseFile(
    tsp00_Int4  FileHandle,
    short      *lpb_error)
{
  size_t             packetSize = 0;
  LVCPS_PacketFile  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              = LVCPS_PacketFile::msgClose;
  packet->m_size              = sizeof(LVCPS_PacketFile);
  packet->m_fileHandle        = FileHandle;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpb_error  = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::SyncFile(
    char       *lpFileName,
    tsp00_Int4  FileHandle,
    short      *lpb_error)
{
  size_t             packetSize = 0;
  LVCPS_PacketFile  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  int len = strlen(lpFileName);
  if (sizeof(LVCPS_PacketFile) + len + 1 > packetSize){
    throw (DbpError(e_buffer_too_small, "Buffer two small for package LVCPS_PacketFile"));
  }
  packet->m_type              = LVCPS_PacketFile::msgWrite;
  packet->m_size              = sizeof(LVCPS_PacketFile) + len + 1;
  packet->m_fileHandle        = FileHandle;
  // Copy msg just behind the package
  strcpy((char*)(this+1), lpFileName);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpb_error  = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::GetDateTime(
    tsp00_Date* Date,
    tsp00_Time* Time)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::GetClock(
    tsp00_Int4* Sec,
    tsp00_Int4* MSec)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::DropVersion( 
    unsigned char  *lpVersionContext,
    short          *lpb_error)
{
  size_t               packetSize = 0;
  LVCPS_PacketVersion *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketVersion::msgDrop;
  packet->m_versionContext    = *reinterpret_cast<tgg01_OmsVersionContext*>(lpVersionContext);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpb_error  = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::CreateSchema(
    const OmsTypeWyde *lpSchemaName,
    tsp00_Int4        *lpSchemaId,
    short             *lpb_error)
{
  size_t               packetSize = 0;
  LVCPS_PacketSchema *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSchema::msgCreate;
  memcpy(&packet->m_schemaName[0], lpSchemaName, sizeof(packet->m_schemaName));

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerSchemaRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpSchemaId = replyPacket->m_schemaId;
  *lpb_error  = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::ExistsSchema(
    const OmsTypeWyde *lpSchemaName,
    tsp00_Int4        *lpSchemaId,
    short             *lpb_error)
{
  size_t              packetSize = 0;
  LVCPS_PacketSchema *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSchema::msgExists;
  memcpy(&packet->m_schemaName[0], lpSchemaName, sizeof(packet->m_schemaName));

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerSchemaRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpSchemaId = replyPacket->m_schemaId;
  *lpb_error  = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::DropSchema(
    tsp00_Int4  SchemaId,
    short      *lpb_error)
{
  size_t              packetSize = 0;
  LVCPS_PacketSchema *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type      =  LVCPS_PacketSchema::msgDrop;
  packet->m_schemaId  = SchemaId;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpb_error  = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::GetSchemaName(
    tsp00_Int4    SchemaId,
    OmsTypeWyde  *lpSchemaName,
    short        *lpb_error)
{
  size_t              packetSize = 0;
  LVCPS_PacketSchema *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type      =  LVCPS_PacketSchema::msgGetName;
  packet->m_schemaId  = SchemaId;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerSchemaRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  memcpy(lpSchemaName, &replyPacket->m_schemaName[0], sizeof(replyPacket->m_schemaName));
  *lpb_error  = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::ExistsFile(
    char    *lpFileName,
    bool    &existsFile,
    bool    &isDirectory,
    short   &error)
{
  size_t             packetSize = 0;
  LVCPS_PacketFile  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  int len = strlen(lpFileName);
  if (sizeof(LVCPS_PacketFile) + len + 1 > packetSize){
    throw (DbpError(e_buffer_too_small, "Buffer two small for package LVCPS_PacketFile"));
  }
  packet->m_type              = LVCPS_PacketFile::msgExists;
  packet->m_size              = sizeof(LVCPS_PacketFile) + len + 1;
  // Copy msg just behind the package
  strcpy((char*)(this+1), lpFileName);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerFileRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  existsFile  = replyPacket->m_existsFile;
  isDirectory = replyPacket->m_isDirectory;
  error       = replyPacket->m_error;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::DropFile(
    char  *lpFileName,
    short &error) 
{
  size_t             packetSize = 0;
  LVCPS_PacketFile  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  int len = strlen(lpFileName);
  if (sizeof(LVCPS_PacketFile) + len + 1 > packetSize){
    throw (DbpError(e_buffer_too_small, "Buffer two small for package LVCPS_PacketFile"));
  }
  packet->m_type              = LVCPS_PacketFile::msgDrop;
  packet->m_size              = sizeof(LVCPS_PacketFile) + len + 1;
  // Copy msg just behind the package
  strcpy((char*)(this+1), lpFileName);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  error  = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::GetWorkingDirectory(
    tsp00_Pathc &pathName,
    short       &error) 
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::GetDbrootSapPath(
    tsp00_Pathc &pathName,
    short       &error)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::LoadLibraryEx( 
    const char    *path, 
    bool           loadGlobal,
    HANDLE        *hd,
    tsp00_ErrText &errtext,
    int            errtext_size)

{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::FreeLibrary( 
    HANDLE         hd,
    bool          &ok,
    tsp00_ErrText &errtext,
    int            errtext_size)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::DropContainer(
    OmsSchemaHandle  schema,
    const GUID       &guid,
    OmsContainerNo    cno,
    tgg00_BasisError &error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketContainer *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              = LVCPS_PacketContainer::msgDrop;
  packet->m_schemaId          = schema;
  packet->m_guid              = guid;
  packet->m_containerNo       = cno;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  error  = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::GetContainerInfo(
    OMS_ContainerHandle &containerHandle, //PTS 1124356 FF 2003-09-23 new FileDir
    GUID            &guid,
    OmsSchemaHandle &schema,
    OmsContainerNo  &cno,
    short           &error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketContainer *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type            = LVCPS_PacketContainer::msgGetInfo;
  packet->m_containerHandle = containerHandle;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerContainerRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  guid       = replyPacket->m_guid;
  schema     = replyPacket->m_schema;
  cno        = replyPacket->m_containerNo;
  error      = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::IsObjHistoryUsed( 
    const SAPDB_UInt8  &oid,
  	bool              ignoreVersions,  /*PTS 1130354*/
    bool              &isUsed,
    tgg00_BasisError  &e,
  	OmsTypeUInt8      &OldestTransNo)  /*PTS 1130354*/
{
  size_t                 packetSize = 0;
  LVCPS_PacketSingleObj *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              = LVCPS_PacketSingleObj::msgIsHistUsed;
  packet->m_oid               = oid;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  isUsed = replyPacket->m_state;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::Reschedule()
{
  size_t               packetSize = 0;
  LVCPS_PacketTasking *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketTasking::msgReschedule;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  m_interf->receiveSimpleReply();

  return S_OK;
}

/*----- PTS 1132163 ---------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::Sleep(
    const OmsTypeUInt1 sleepTime)
{
  // todo
  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::LockUpdObj( 
    unsigned char   *lpConsistentView,
    unsigned char   *lpContainerId,
    SAPDB_UInt8      *lpoid,
    unsigned char   *lpobjseq,
    short           *lpb_error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketSingleObj *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSingleObj::msgLckUpdObj;
  packet->m_consistentView    = *reinterpret_cast<tgg91_TransNo*>(lpConsistentView);
  packet->m_containerId       = *reinterpret_cast<tgg01_ContainerId*>(lpContainerId);
  packet->m_oid               = *lpoid;
  packet->m_objSeq            = *reinterpret_cast<tgg91_PageRef*>(lpobjseq);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerSingleObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *((tgg01_ContainerId*)lpContainerId) = replyPacket->m_containerId;
  *((tgg91_PageRef*)lpobjseq)          = replyPacket->m_objSeq;
  *lpb_error                           = replyPacket->m_DBError;

  return S_OK;
}

/*----------------------------------------------------------------------*/
// PTS 1121449
HRESULT LVCPS_LiveCacheSink::LockUpdObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  int                    i          = 0;
  size_t                 packetSize = 0;
  LVCPS_PacketMassObj   *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketMassObj::msgLckUpdObj;
  packet->m_objCnt            =  cnt;
  packet->m_consistentView    = *pConsistentView;
  for (i=0; i<cnt; ++i)
    packet->m_pContainerId[i] = *(ppContainerId[i]);
  memcpy(&packet->m_pOid[0], pOid, cnt * sizeof(SAPDB_UInt8)); 
  memcpy(&packet->m_pObjSeq[0], pObjSeq, cnt * sizeof(packet->m_pObjSeq[0]));
  packet->m_clustered = clustered;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerMassObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  for (i=0; i<cnt; ++i)
    *ppContainerId[i] = replyPacket->m_pContainerId[i];
  memcpy(&pObjSeq[0], &replyPacket->m_pObjSeq[0], cnt * sizeof(pObjSeq[0]));
  memcpy(&pDBError[0], &replyPacket->m_pDBError[0], cnt * sizeof(pDBError[0]));
  errCnt   = replyPacket->m_errCnt;

	return S_OK;
}

/*----------------------------------------------------------------------*/
// PTS 1130435
HRESULT LVCPS_LiveCacheSink::LockSharedObjMass(
    tsp00_Int4              &cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjSeq,
    tsp00_Int8              &timeout,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  size_t                 packetSize = 0;
  LVCPS_PacketMassObj   *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketMassObj::msgLckSharedObj;
  packet->m_objCnt            =  cnt;
  packet->m_consistentView    = *pConsistentView;
  for (int i=0; i<cnt; ++i)
    packet->m_pContainerId[i] = *(ppContainerId[i]);
  memcpy(&packet->m_pOid[0], pOid, cnt * sizeof(SAPDB_UInt8)); 
  memcpy(&packet->m_pObjSeq[0], pObjSeq, cnt * sizeof(packet->m_pObjSeq[0]));
  packet->m_timeout           = timeout;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerMassObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  cnt      = replyPacket->m_objCnt;
  timeout  = replyPacket->m_timeout;
  errCnt   = replyPacket->m_errCnt;
  memcpy(&pDBError[0], &replyPacket->m_pDBError[0], cnt * sizeof(pDBError[0]));

	return S_OK;
}

/*----------------------------------------------------------------------*/
// PTS 1130435
HRESULT LVCPS_LiveCacheSink::LockSharedObj(
    unsigned char  * lpConsistentView,
    unsigned char  * lpContainerId,
    SAPDB_UInt8     * lpoid,
    unsigned char  * lpobjseq,
    short          * lpb_error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketSingleObj *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketSingleObj::msgLckSharedObj;
  packet->m_consistentView    = *reinterpret_cast<tgg91_TransNo*>(lpConsistentView);
  packet->m_containerId       = *reinterpret_cast<tgg01_ContainerId*>(lpContainerId);
  packet->m_oid               = *lpoid;
  packet->m_objSeq            = *reinterpret_cast<tgg91_PageRef*>(lpobjseq);
  
  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *lpb_error = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::Signal(
  tsp00_TaskId TaskId, 
  tsp00_Bool   ok)
{
  size_t               packetSize = 0;
  LVCPS_PacketTasking *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type   =  LVCPS_PacketTasking::msgSignal;
  packet->m_taskId = TaskId;
  packet->m_ok     = ok;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  m_interf->receiveSimpleReply();

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::Wait(
  tsp00_Bool &ok)
{
  size_t               packetSize = 0;
  LVCPS_PacketTasking *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type   =  LVCPS_PacketTasking::msgWait;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  ok  = replyPacket->m_state;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::ExistsContainer(
    tsp00_Int4            schemaId,
    const GUID           &guid,
    OmsContainerNo        containerNo,
    size_t                objectSize,
    size_t                keyLen,
    tgg01_ContainerId    &containerId,
    bool                 &useCachedKeys,
    bool                 &partitionedKeys,
    short                &error) // PTS 1110334
{
  size_t                 packetSize = 0;
  LVCPS_PacketContainer *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              = LVCPS_PacketContainer::msgExists;
  packet->m_schemaId          = schemaId;
  packet->m_guid              = guid;
  packet->m_containerNo       = containerNo;
  packet->m_size              = objectSize;
  packet->m_keyLen            = keyLen;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerContainerRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  memcpy(&containerId, &replyPacket->m_containerId, sizeof(tgg01_ContainerId));
  useCachedKeys = replyPacket->m_useCachedKeys;
  partitionedKeys = replyPacket->m_partitionedKeys;
  error         = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::LockContainer(
  const tgg00_FileId &containerId,
  short              &error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketContainer *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              = LVCPS_PacketContainer::msgLock;
  packet->m_containerId       = containerId;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  error = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::GetSessionHeapUsage(
  tsp00_8ByteCounter& heapUsage)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::EvalCallStatistics(
    bool            AllocatorOnly,
    OmsMonitorData& MonitorData)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::SimCtlDumpToHtml(
    class OmsHandle           &handle,
    SAPDB_UInt8                obj,
    class OmsSimDumpCollector &str)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::SimCtlSetObjectName(
    class OmsHandle  &handle,
    SAPDB_UInt8       obj,
    const char       *name)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::SimCtlGetObjectName(
    class OmsHandle				&handle,
    SAPDB_UInt8            obj,
    const char					**name)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::SimCtlDestroyHandleCB(
    class OmsHandle			     &handle,
    struct IliveCacheSink		**session)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::SimCtlGetHTMLRef(
    class OmsHandle  &handle,
    SAPDB_UInt8       obj,
    const char      **data,
    int               flags)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::IsInRegion(
      int    regionId,
      bool  &inRegion,
      short &error) 
{
  size_t              packetSize = 0;
  LVCPS_PacketRegion *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type        =  LVCPS_PacketRegion::msgIsInRegion;
  packet->m_regionId    = regionId;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  inRegion = replyPacket->m_state;
  error    = replyPacket->m_DBError;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::AttachSequence(
    const tsp00_KnlIdentifier &sequenceName,
    tgg00_Surrogate           &sequenceId,
    tgg00_SeqInfo             &sequenceInfo,
    short                     &sqlError)
{
  size_t                 packetSize = 0;
  LVCPS_PacketSequence  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              = LVCPS_PacketSequence::msgAttachSeq;
  packet->m_sequenceName      = sequenceName;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerSequenceRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  sequenceId          = replyPacket->m_sequenceId; 
  sequenceInfo        = replyPacket->m_sequenceInfo; 
  sqlError            = replyPacket->m_error;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::NextVal(
    const tgg00_Surrogate &sequenceId,
    tgg00_SeqInfo         &sequenceInfo,   
    tsp00_Uint4           &nextVal,
    short                 &sqlError)
{
  size_t                 packetSize = 0;
  LVCPS_PacketSequence  *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              = LVCPS_PacketSequence::msgNextVal;
  packet->m_sequenceId        = sequenceId;
  packet->m_sequenceInfo      = sequenceInfo;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerSequenceRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  sequenceInfo = replyPacket->m_sequenceInfo;
  nextVal      = replyPacket->m_nextVal; 
  sqlError     = replyPacket->m_error;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::IsUnicodeInstance(
      bool  &isUnicodeInstance,
      short &sqlError)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

int LVCPS_LiveCacheSink::GetSinkType()
{
  return 2;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::GetSqlSessionContext (
  void** p,
  tsp00_TaskId  &taskId)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::SetSqlSessionContext (
  void* p)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::NextOids(
    tgg91_TransNo           &ConsistentView,
    tgg00_FileId            &ContainerId,
    tgg01_OmsVersionContext *pVersionContext,
    bool                     bOnlyHdrVarObjects,
    SAPDB_UInt8               CurrOid,
    tsp00_Int4              &noOfOid,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef           *pObjVers,
    tsp00_Int4              &LogReadAccesses,
    tgg00_BasisError        *pb_error)
{
  size_t                 packetSize = 0;
  LVCPS_PacketIterator   *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketIterator::msgGetNxtOid;
  packet->m_cnt               =  noOfOid;
  packet->m_consistentView    =  ConsistentView;
  packet->m_containerId       =  ContainerId;
  if (pVersionContext != NULL){
    packet->m_versionContext  = *pVersionContext;
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  packet->m_startOid          = CurrOid;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerIteratorRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  noOfOid         = replyPacket->m_cnt; 
  ConsistentView  = replyPacket->m_consistentView;
  memcpy(&pOid[0],        &replyPacket->m_pOid[0],        noOfOid * sizeof(pOid[0])); 
  memcpy(&pObjVers[0],    &replyPacket->m_pObjSeq[0],     noOfOid * sizeof(pObjVers[0])); 
  LogReadAccesses = replyPacket->m_logHistRead;
  *pb_error       = replyPacket->m_DBError;

	return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::GetContainerId(		
    tgg01_ContainerId              *pcontainerId, 
    const tgg01_OmsVersionContext  *pVersionContext,
    tsp00_Int4                      noOfOid,
    const SAPDB_UInt8               *pOid,
    tgg00_BasisError               *pDBError)
{
  size_t                 packetSize = 0;
  LVCPS_PacketMassObj   *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type              =  LVCPS_PacketMassObj::msgGetContainer;
  packet->m_objCnt            =  noOfOid;
  if (pVersionContext != NULL){
    packet->m_versionContext  = *pVersionContext;
    packet->m_isVersion       = true;
  }
  else {
    packet->m_isVersion       = false;
  }
  memcpy(&packet->m_pOid[0], pOid, noOfOid * sizeof(SAPDB_UInt8)); 

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerMassObjRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  memcpy(&pcontainerId[0], &packet->m_pContainerId[0], noOfOid * sizeof(packet->m_pContainerId[0]));
  memcpy(&pDBError[0], &replyPacket->m_pDBError[0], noOfOid * sizeof(pDBError[0]));

	return S_OK;
}

/*---------------------------------------------------------------------------*/
HRESULT STDMETHODCALLTYPE LVCPS_LiveCacheSink::SetTransactionComment(
  tsp00_Int2         commentLength,
  const OmsTypeWyde *pComment,
  short             &sqlError)
{ return S_OK; }

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::VersionDictInsert(
    const OmsVersionId  &versionId,
    const OMS_Context   *pVersionContext,
    tgg00_BasisError    &error) 
{
  size_t                   packetSize = 0;
  LVCPS_PacketVersionDict *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type            = LVCPS_PacketVersionDict::msgInsert;
  memcpy(&packet->m_versionId[0], &versionId, sizeof(OmsVersionId));
  packet->m_pVersionContext = const_cast<OMS_Context*>(pVersionContext);

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerVersionDictRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  error = replyPacket->m_error;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::VersionDictFind(
    const OmsVersionId   &versionId,
    OMS_Context         **ppVersionContext) 
{
  size_t                   packetSize = 0;
  LVCPS_PacketVersionDict *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type       = LVCPS_PacketVersionDict::msgFind;
  memcpy(&packet->m_versionId[0], &versionId, sizeof(OmsVersionId));

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerVersionDictRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *ppVersionContext = replyPacket->m_pVersionContext;
  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::VersionDictDrop(
    const OmsVersionId  &versionId,
    tgg00_BasisError    &error) 
{
  size_t                   packetSize = 0;
  LVCPS_PacketVersionDict *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type       = LVCPS_PacketVersionDict::msgDrop;
  memcpy(&packet->m_versionId[0], &versionId, sizeof(OmsVersionId));

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerVersionDictRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  error = replyPacket->m_error;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::VersionDictShutdown() 
{
  size_t                   packetSize = 0;
  LVCPS_PacketVersionDict *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type       = LVCPS_PacketVersionDict::msgShutdown;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  m_interf->simpleReply(0);

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::VersionDictUnloadOldestVersion(
  bool &unloaded) 
{
  size_t                   packetSize = 0;
  LVCPS_PacketVersionDict *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type       = LVCPS_PacketVersionDict::msgUnload;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerErrorStateRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  unloaded = replyPacket->m_state;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::VersionDictMarkNotUnloadable(
    OMS_Context *pVersionContext)
{
  size_t                   packetSize = 0;
  LVCPS_PacketVersionDict *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type            = LVCPS_PacketVersionDict::msgMarkNotUnloadable;
  packet->m_pVersionContext = pVersionContext;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  m_interf->simpleReply(0);

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::VersionDictMarkUnloadable(
    OMS_Context *pVersionContext)
{
  size_t                   packetSize = 0;
  LVCPS_PacketVersionDict *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type               = LVCPS_PacketVersionDict::msgMarkUnloadable;
  packet->m_pVersionContext    = pVersionContext;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  m_interf->simpleReply(0);

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::VersionDictCreateIter(
  OMS_LockMode   lockMode, 
  void         **ppIter, 
  OMS_Context  **ppContext)
{
  size_t                   packetSize = 0;
  LVCPS_PacketVersionDict *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type               = LVCPS_PacketVersionDict::msgCreateIter;
  packet->m_lockMode           = lockMode;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerVersionDictRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *ppIter    = replyPacket->m_pIter;
  *ppContext = replyPacket->m_pVersionContext;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::VersionDictGetNext(
  void         *pIter, 
  OMS_Context **ppContext)
{
  size_t                   packetSize = 0;
  LVCPS_PacketVersionDict *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type               = LVCPS_PacketVersionDict::msgGetNext;
  packet->m_pIter              = pIter;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  const LVC_ProcServerVersionDictRep *replyPacket = NULL;
  m_interf->receiveReply(replyPacket);

  // Process reply packet
  *ppContext = replyPacket->m_pVersionContext;

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::VersionDictDestructIter(
  void   *pIter)
{
  size_t                   packetSize = 0;
  LVCPS_PacketVersionDict *packet     = NULL;

  // Build request package
  m_interf->preparePacket(packet, packetSize);
  packet->m_type               = LVCPS_PacketVersionDict::msgDestructIter;
  packet->m_pIter              = pIter;

  // Send request package
  m_interf->send(packet->m_size); 

  // Wait for reply
  m_interf->simpleReply(0);

  return S_OK;
}

/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::IsTestKnlBadAllocActive(bool &active)
{
  // TODO
  active = false;
  return S_OK;
}
/*---------------------------------------------------------------------------*/

HRESULT LVCPS_LiveCacheSink::ResetMonitor()
{
  // TODO
  return S_OK;
}


