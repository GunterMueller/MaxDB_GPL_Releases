/*!
 * \file    OMS_UnloadedVersionKeyIter.cpp
 * \author  MarkusSi, Roterring
 * \brief   iterator to read objects from a version which is unloaded
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

#include "Oms/OMS_UnloadedVersionKeyIter.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"


/*===================================================================================*/
/*! Construtor.                                                                      **
**  In this method an iterator to the objects of the unloaded version is created in  **
**  the kernel and the first of the requested objects is read (oid as well as the    **
**  object frame itself). If the object is already stored in the local oms cache,    **
**  then the delivered object frame is discarded otherwise the object frame is       **
**  inserted into the oms cache.                                                     **
**  As the boundaries of the interval are checked inside the class, local copies of  **
**  the corresponding keys are created.                                              **
**  \param pStartKey pointer to the key which should be used to position the         **
**                   iterator after the creation. Currently this key must equal      **
**                   either the lower or the upper key.                              **
**  \param pLowerKey pointer to the key which represents the lower boundary of the   **
**                   interval the iterator should read from. The interval boundary   **
**                   is inclusive.                                                   **
**  \param pUpperKey pointer to the key which represents the upper boundary of the   **
**                   interval the iterator should read from. The interval boundary   **
**                   is inclusive.                                                   **
**  \param direction the direction (OMS_ASCENDING or OMS_DESCENDING) of the iterator **
**  \param pSession  pointer to the session                                          **
**  \param pContainerInfo pointer to the container info of the corresponding class   */
/*************************************************************************************/
OMS_UnloadedVersionKeyIter::OMS_UnloadedVersionKeyIter
(
 const void*       pStartKey,
 const void*       pLowerKey,
 const void*       pUpperKey,
 OmsIterDirection  direction,
 OMS_Session*      pSession,
 OMS_ContainerEntry* pContainerInfo
 )
 : OMS_BasisKeyIterBase(pSession, pContainerInfo)
 , m_pCurrKey(NULL)
 , m_pLowerKey(NULL)
 , m_pUpperKey(NULL)
{
  const char* msg = "OMS_UnloadedVersionKeyIter::Ctor ";

  // Create local copies of keys
  m_pCurrKey = m_pSession->allocate(m_keyLen);
  SAPDB_MemCopyNoCheck(m_pCurrKey, pStartKey, m_keyLen);
  m_pLowerKey = m_pSession->allocate(m_keyLen);
  if (pLowerKey){
    SAPDB_MemCopyNoCheck(m_pLowerKey, pLowerKey, m_keyLen);
  }
  else {
    memset(m_pLowerKey, 0, m_keyLen);
  }
  m_pUpperKey = m_pSession->allocate(m_keyLen);
  if (pUpperKey){
    SAPDB_MemCopyNoCheck(m_pUpperKey, pUpperKey, m_keyLen);
  }
  else {
    memset(m_pUpperKey, 0xff, m_keyLen);
  }

  // Reserve buffer for reading of the object
  OmsObjectContainer *pObjBuf = m_pContainerInfo->GetMemory();

  tsp00_Int2 DBError;
  m_pSession->m_lcSink->StartUnloadedVersionIter(
      m_pSession->m_context->m_consistentView,
      m_pSession->CurrentContext()->VersionContext(),
      m_pContainerInfo->GetFileId(),
      m_keyLen,
      m_pCurrKey,            // Key is changed after call if necessary
      m_pLowerKey,
      m_pUpperKey,
      m_pContainerInfo->GetClassEntry().GetPersistentSize(),           
      ((unsigned char*) &pObjBuf->m_pobj) + sizeof(void*),
      *REINTERPRET_CAST(SAPDB_UInt8*, &m_currOid),            
      DBError,
      direction);  
  if (e_ok == DBError) {
    m_pCurrObj = LoadObj();
    if (NULL == m_pCurrObj) {
      // Object is might be marked as deleted locally, in this case read next object 
      if (direction == OMS_ASCENDING) {
        ++(*this);
      } else {
        --(*this);
      }
    }
  }
  else {
    if (e_no_next_object == DBError) {
      m_currOid.setNil();
      m_end = true;
    }
    else {
      m_pSession->ThrowDBError (DBError, msg, __MY_FILE__, __LINE__);
    }
  }

  if (m_end){
    OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << ": end reached");
  }
  else {
    OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg 
             << OMS_UnsignedCharBuffer((const unsigned char*)GetCurrKey(), m_keyLen));
  }
  
  m_pContainerInfo->ChainFree(pObjBuf, 91);
}


/*===================================================================================*/

void OMS_UnloadedVersionKeyIter::DeleteSelf(){
  const char* msg = "OMS_UnloadedVersionKeyIter::DeleteSelf ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  this->~OMS_UnloadedVersionKeyIter();
  m_pSession->deallocate(this);
}

/*===================================================================================*/

OMS_UnloadedVersionKeyIter::~OMS_UnloadedVersionKeyIter()
{
  const char* msg = "OMS_UnloadedVersionKeyIter::Dtor ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  if (m_pCurrKey){
    m_pSession->deallocate(m_pCurrKey);
    m_pCurrKey = NULL;
  }

  if (m_pLowerKey){
    m_pSession->deallocate(m_pLowerKey);
    m_pLowerKey = NULL;
  }

  if (m_pUpperKey){
    m_pSession->deallocate(m_pUpperKey);
    m_pUpperKey = NULL;
  }
}

/*===================================================================================*/
