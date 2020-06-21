/*!
 * \file    OMS_InMemVersionKeyIter.cpp
 * \author  MarkusSi, Roterring
 * \brief   iterator which reads objects from a version which resides in memory
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


#include "Oms/OMS_InMemVersionKeyIter.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*===================================================================================*/
/*! Construtor.                                                                      **
**  In this method an iterator on the avl-tree, which stores the objects is created  **
**  and it is positioned on the first object. As the avl-tree might also store       **
**  objects which are not created in a version when the cached key feature is        **
**  switched on, these objects have to be skipped.                                   **
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
OMS_InMemVersionKeyIter::OMS_InMemVersionKeyIter
(
 const void*       pStartKey,
 const void*       pLowerKey,
 const void*       pUpperKey,
 OmsIterDirection  direction,
 OMS_Session*      pSession,
 OMS_ContainerEntry* pContainerInfo
 )
 : OMS_BasisKeyIterBase(pSession, pContainerInfo)
 , m_pLowerKey(NULL)
 , m_pUpperKey(NULL)
 , m_iter(pContainerInfo->VersionIterStart((unsigned char*)pStartKey, direction))
{
  const char* msg = "OMS_InMemVersionKeyIter::Ctor ";

  if (pLowerKey){
    m_pLowerKey = pSession->allocate(m_keyLen);
    SAPDB_MemCopyNoCheck(m_pLowerKey, pLowerKey, m_keyLen);
  }
  if (pUpperKey){
    m_pUpperKey = pSession->allocate(m_keyLen);
    SAPDB_MemCopyNoCheck(m_pUpperKey, pUpperKey, m_keyLen);
  }

  if (m_pContainerInfo->UseCachedKeys()){
    // Iterator might point to an object which was not created in a version
    // (see cached keys). Therefore if needed search for first object which 
    // was created in a version.
    while (m_iter){
      OmsObjectContainer* pCurrObject = m_pContainerInfo->VersionGetInfoFromNode(m_iter());          
      if (m_pSession->CurrentContext()->IsVersionOid(pCurrObject->m_oid)){
        break;
      }
      else {
        if (!m_iter.IsValid()){  
          m_pSession->ThrowDBError (e_invalid_iterator, msg, __MY_FILE__, __LINE__);
        }   
        if (direction == OMS_ASCENDING){
          ++m_iter;
          // Check if upper bound is reached
          if (pUpperKey && m_iter){
            if (memcmp(*m_iter(), m_pUpperKey, m_keyLen) > 0){
              m_end = false;
              break;
            }
          }
        }
        else {
          --m_iter;
          // Check if lower bound is reached
          if (pLowerKey && m_iter){
            if (memcmp(*m_iter(), m_pLowerKey, m_keyLen) < 0){
              m_end = false;
              break;
            }
          }
        }
      }
    }
  }

  // Check whether iterator has reached its end.
  m_end = (m_end || !bool(m_iter));

  if (!m_end) {
    // Check object, delivered by the kernel, against the boundaries
    if (pLowerKey){
      if (memcmp(*m_iter(), m_pLowerKey, m_keyLen) < 0){
        m_end = true;
      }
    }
    if (pUpperKey){
      if (memcmp(*m_iter(), m_pUpperKey, m_keyLen) > 0){
        m_end = true;
      }
    }
  }

  if (m_end){
    OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << ": end reached");
  }
  else {
    // Check if object is already marked as deleted locally and if so
    // select next object
    m_pCurrObj = LoadObj();
    if (NULL != m_pCurrObj) {
      OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << OMS_UnsignedCharBuffer(*m_iter(), m_keyLen));
    }
    else {
      if (direction == OMS_ASCENDING){
        ++(*this);
      }
      else {
        --(*this);
      }
    }
  }
}

/*===================================================================================*/

void OMS_InMemVersionKeyIter::DeleteSelf()
{
  const char* msg = "OMS_InMemVersionKeyIter::DeleteSelf ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  this->~OMS_InMemVersionKeyIter();
  m_pSession->deallocate(this);
}

/*===================================================================================*/

OMS_InMemVersionKeyIter::~OMS_InMemVersionKeyIter()
{
  const char* msg = "OMS_InMemVersionKeyIter::Dtor ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  if (NULL != m_pLowerKey){ 
    m_pSession->deallocate(m_pLowerKey);
    m_pLowerKey = NULL;
  }

  if (NULL != m_pUpperKey){
    m_pSession->deallocate(m_pUpperKey);
    m_pUpperKey = NULL;
  }
}

/*===================================================================================*/
