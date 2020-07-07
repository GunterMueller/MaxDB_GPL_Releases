/*!
 * \file    OMS_InMemVersionKeyIter.hpp
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

#ifndef __OMS_INMEMVERSIONKEYITER_HPP
#define __OMS_INMEMVERSIONKEYITER_HPP

#include "Oms/OMS_KeyIterBase.hpp"

/*----------------------------------------------------------------------*/
/*! Class defines a key iterator over all keyed objects, that have      **
**  been created in a version which is not unloaded                     **
**  \since PTS 1119480                                                  */
/*----------------------------------------------------------------------*/
class OMS_InMemVersionKeyIter : public OMS_BasisKeyIterBase
{
private :
  /*! Pointer to a local copy of the lower boundary key of the intervall */
  void*   m_pLowerKey;    
  /*! Pointer to a local copy of the upper boundary key of the intervall */
  void*   m_pUpperKey;
  /*! Iterator which is used to read the objects from the AVL-tree       */
  cgg250AvlTree<unsigned char*, OMS_ContainerEntry,OMS_Context>::Iterator m_iter;

public :
  /// Constructor
  OMS_InMemVersionKeyIter(
    const void*       pStartKey,
    const void*       pLowerKey,
    const void*       pUpperKey,
    OmsIterDirection  direction,
    OMS_Session*  pSession,
    OMS_ContainerEntry* pContainerInfo
    );
  /// Destructor
  ~OMS_InMemVersionKeyIter();
  /// cleans the memory
  void DeleteSelf();
  /// increments the iterator
  inline void operator++();
  /// decrements the iterator
  inline void operator--();      

private:
  /// loads the object-container from the kernel into the oms-cache
  inline OmsObjectContainer* LoadObj();
};



/*===================================================================================*/
/*! This method increments the iterator. As the avl-tree might also store            **
**  objects which are not created in a version, when the cached key feature is       **
**  switched on, these objects have to be skipped.                                   */
/*===================================================================================*/
inline void OMS_InMemVersionKeyIter::operator++()
{
  const char* msg = "OMS_InMemVersionKeyIter::++ ";

  m_pCurrObj = NULL;

  do {
    if (!m_iter.IsValid()){  
      m_pSession->ThrowDBError (e_invalid_iterator, msg, __MY_FILE__, __LINE__);
    }
    m_end = false;
    ++m_iter;

    if (m_pContainerInfo->UseCachedKeys()){
      // If cached keys are used, then there might be entries in the structure
      // m_index which do not belong to objects which are newly created in
      // a version. Therefore for the iteration consider only those objects 
      // which belong to a version.
      while (m_iter){
        OmsObjectContainer* pCurrObject = m_pContainerInfo->VersionGetInfoFromNode(m_iter());          
        if (m_pSession->CurrentContext()->IsVersionOid(pCurrObject->m_oid)){
          break;
        }
        else{
          if (!m_iter.IsValid()){  
            m_pSession->ThrowDBError (e_invalid_iterator, msg, __MY_FILE__, __LINE__);
          }
          ++m_iter;
          if (m_pUpperKey && m_iter){
            if (memcmp(*m_iter(), m_pUpperKey, m_keyLen) > 0){
              m_end = true;
              break;
            }
          }
        }
      }
    } 

    if (m_iter){
      // Check if object is already marked as deleted locally
      m_pCurrObj = LoadObj();
    }
    else {
      m_pCurrObj = NULL;
    }

    // Check whether iterator has reached its end.
    m_end = (m_end || !bool(m_iter));

    // Check that object lies in between specified boundaries.
    if (!m_end && m_pUpperKey != NULL){
      if (memcmp(*m_iter(), m_pUpperKey, m_keyLen) > 0){
        m_end = true;
      }
    }
  }while (m_pCurrObj == NULL && !m_end);

  if (m_end){
    OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << ": end reached");
  }
  else {
    OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << OMS_UnsignedCharBuffer(*m_iter(), m_keyLen));
  }
}

/*===================================================================================*/
/*! This method decrements the iterator. As the avl-tree might also store            **
**  objects which are not created in a version, when the cached key feature is       **
**  switched on, these objects have to be skipped.                                   */
/*===================================================================================*/
inline void OMS_InMemVersionKeyIter::operator--()
{
  const char* msg = "OMS_InMemVersionKeyIter::-- ";

  m_pCurrObj = NULL;

  do {
    if (!m_iter.IsValid()){  
      m_pSession->ThrowDBError (e_invalid_iterator, msg, __MY_FILE__, __LINE__);
    }
    m_end = false;
    --m_iter;

    if (m_pContainerInfo->UseCachedKeys()){
      // If cached keys are used, then there might be entries in the structure
      // m_index which do not belong to objects which are newly created in
      // a version. Therefore for the iteration consider only those objects 
      // which belong to a version.
      while (m_iter){
        OmsObjectContainer* pCurrObject = m_pContainerInfo->VersionGetInfoFromNode(m_iter());          
        if (m_pSession->CurrentContext()->IsVersionOid(pCurrObject->m_oid)){
          break;
        }
        else{
          if (!m_iter.IsValid()){  
            m_pSession->ThrowDBError (e_invalid_iterator, msg, __MY_FILE__, __LINE__);
          }
          --m_iter;
          if (m_pLowerKey && m_iter){
            if (memcmp(*m_iter(), m_pLowerKey, m_keyLen) < 0){
              m_end = true;
              break;
            }
          }
        }
      }
    } 

    if (m_iter){
      // Check if object is already marked as deleted locally
      m_pCurrObj = LoadObj();
    }
    else {
      m_pCurrObj = NULL;
    }

    // Check whether iterator has reached its end.
    m_end = (m_end || !bool(m_iter));

    // Check that object lies in between specified boundaries.
    if (!m_end && m_pLowerKey != NULL){
      if (memcmp(*m_iter(), m_pLowerKey, m_keyLen) < 0){
        m_end = true;
      }
    }
  }while (m_pCurrObj == NULL && !m_end);


  if (m_end){
    OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << ": end reached");
  }
  else {
    OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << OMS_UnsignedCharBuffer(*m_iter(), m_keyLen));
  }
}

/*===================================================================================*/
/*! This method returns a pointer to the object frame in the local oms-cache. As     **
**  the object is created in a version and the corresponding version is not unloaded **
**  the object must reside in the local oms-cache.                                   */
/*===================================================================================*/
inline OmsObjectContainerPtr OMS_InMemVersionKeyIter::LoadObj() 
{
  const char* msg = "OMS_InMemVersionKeyIter::LoadObj ";

  unsigned char* const* p = m_iter();
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << OMS_UnsignedCharBuffer(*p, m_keyLen)); 

  OmsObjectContainerPtr found 
    = REINTERPRET_CAST(OmsObjectContainer*, CONST_CAST(unsigned char*, (*p) + m_keyLen  - m_pContainerInfo->GetClassEntry().GetTransientSize()));  

  if (found->DeletedFlag()){
    m_pCurrObjDel = found;
    return NULL;
  }
  else {
    m_pCurrObjDel = NULL;
    return found;
  }
}

/*===================================================================================*/

#endif