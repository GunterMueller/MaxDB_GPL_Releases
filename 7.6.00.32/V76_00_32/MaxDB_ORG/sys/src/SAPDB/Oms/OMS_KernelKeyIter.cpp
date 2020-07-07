/*!
 * \file    OMS_KernelKeyIter.cpp
 * \author  MarkusSi, Roterring
 * \brief   iterator which reads objects from the kernel
 */

/*

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


#include "Oms/OMS_KernelKeyIter.hpp"


/*===================================================================================*/
/*! Construtor.                                                                      
**  In this method the arrays to store the buffered oids are allocated and the kernel
**  iterator is created. When creating the kernel the first oids are already read    
**  from the kernel and stored in the array. Additionally, the objects corresponding 
**  to these oids are loaded into the oms-cache, too.                                
**  \param pStartKey pointer to the key which should be used to position the         
**                   iterator after the creation. Currently this key must equal      
**                   either the lower or the upper key.                              
**  \param pLowerKey pointer to the key which represents the lower boundary of the   
**                   interval the iterator should read from. The interval boundary   
**                   is inclusive.                                                   
**  \param pUpperKey pointer to the key which represents the upper boundary of the   
**                   interval the iterator should read from. The interval boundary   
**                   is inclusive.                                                   
**  \param direction the direction (OMS_ASCENDING or OMS_DESCENDING) of the iterator 
**  \param pSession  pointer to the session                                          
**  \param pContainerInfo pointer to the container info of the corresponding class   
**  \param maxBufferSize Number of oids which can be buffered locally.               
*/
/*************************************************************************************/
OMS_KernelKeyIter::OMS_KernelKeyIter
(
 const void*       pStartKey,
 const void*       pLowerKey,
 const void*       pUpperKey,
 OmsIterDirection  direction,
 OMS_Session*      pSession,
 OMS_ContainerEntry* pContainerInfo,
 int               maxBufferSize
 )
 : OMS_BasisKeyIterBase(pSession, pContainerInfo)
 , m_maxNoOfOid(maxBufferSize)
 , m_pKBIterator(NULL)
 , m_pRestartKey(NULL)
 , m_currIdx(0)
 , m_maxIdx(-1)
 , m_objectsAreLoaded(false)
 , m_allObjReadFromKernel(false)
 , m_pOid(NULL)
{
  const char* msg = "OMS_KernelKeyIter::Ctor ";

  m_noMassOperation = !OMS_Globals::m_globalsInstance->InProcServer();  // Use Mass-Operations only if UDE-Server is active

  if (m_noMassOperation == true){
    m_maxNoOfOid = 1;
  }
  else {
    if (m_maxNoOfOid < 1){
      m_pSession->ThrowDBError (e_invalid_iterator, msg, __MY_FILE__, __LINE__);
    }

    if (m_maxNoOfOid > OMS_MASS_OPERATION_CNT){
      m_maxNoOfOid = OMS_MASS_OPERATION_CNT;
    }
  }

  // Assign the correctly typed pointers to the untyped arrays
  m_pLoadOid = reinterpret_cast<OMS_ObjectId8*>(&m_pLoadOid_u[0]);
  m_pOid = reinterpret_cast<OMS_ObjectId8*>(&m_pOid_u[0]);

  OmsObjectContainer* pObj=NULL;
  if (m_noMassOperation){
    // Get memory to store the object if no mass-operations are active. 
    pObj = m_pContainerInfo->GetMemory();
  }

  // Create iterator in kernel and get first object from livecache 
  tgg00_BasisError DBError;
  int noOfOid = m_maxNoOfOid;
  int objHistReadCount;
  m_pSession->m_lcSink->CreateKeyRangeIterator(
      m_pSession->m_context->m_consistentView,
      m_pContainerInfo->GetFileId(),
      m_pSession->m_context->VersionContext(),
      &m_pKBIterator,
      m_pContainerInfo->GetClassEntry().GetPersistentSize(),
      m_keyLen,
      pStartKey,
      pLowerKey,
      pUpperKey,
      noOfOid,   // changed after call
      REINTERPRET_CAST(SAPDB_UInt8*, m_pOid),
      m_pObjVers,
      m_noMassOperation ? ((unsigned char*) &pObj->m_pobj) + sizeof(void*) : NULL,
      objHistReadCount,
      DBError,
      direction);
  m_pSession->IncLogHop(objHistReadCount);
  if (DBError == e_no_next_object) {
    // All objects from the kernel are read, but there might be some oids left in the buffer
    m_allObjReadFromKernel = true;
    if (noOfOid == 0){
      OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << ": end reached");
      m_maxIdx = -1;
      m_end = true;

      if (m_noMassOperation){
        m_pContainerInfo->ChainFree(pObj, 54);
      }
    }
  }
  else if (DBError != e_ok){
    m_pSession->ThrowDBError(DBError, msg, __MY_FILE__, __LINE__);
    return;
  }
  
  if (noOfOid > 0){
    // There is at least one new oid
    m_maxIdx = noOfOid - 1;
    m_currIdx  = ( direction == OMS_ASCENDING ? 0 : m_maxIdx);
  }

  if (!m_end){
    m_pCurrObj = LoadObj(pObj);
    if (m_pCurrObj != NULL){
      // Object is not marked as deleted
      OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg 
               << OMS_UnsignedCharBuffer((const unsigned char*)GetCurrKey(), m_keyLen));
    }
    else if (direction == OMS_ASCENDING){
      // If the current object is marked as deleted, then m_pCurrObj = NULL. 
      // In this case look for next non-deleted object.
      ++(*this);
    }
    else {
      --(*this);
    }
  }
}

/*===================================================================================*/

void OMS_KernelKeyIter::DeleteSelf()
{
  const char* msg = "OMS_KernelKeyIter::DeleteSelf ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  this->~OMS_KernelKeyIter();
  m_pSession->deallocate(this);
}

/*===================================================================================*/

OMS_KernelKeyIter::~OMS_KernelKeyIter()
{
  const char* msg = "OMS_KernelKeyIter::Dtor ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  if (NULL != m_pKBIterator){
    m_pSession->m_lcSink->DestroyKeyRangeIterator(m_pKBIterator);
    m_pKBIterator = NULL;
  }
}

/*===================================================================================*/


