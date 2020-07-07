/*!
 * \file    OMS_UnloadedVersionKeyIter.hpp
 * \author  MarkusSi, Roterring
 * \brief   iterator which reads objects from a version which is unloaded
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

#ifndef __OMS_UNLOADEDVERSIONKEYITER_HPP
#define __OMS_UNLOADEDVERSIONKEYITER_HPP

#include "Oms/OMS_KeyIterBase.hpp"

/*----------------------------------------------------------------------*/
/*! Class defines a key iterator over all keyed objects, that have been **
**  created in a version and have been unloaded                         **
**  \since PTS 1119480                                                  */
/*----------------------------------------------------------------------*/
class OMS_UnloadedVersionKeyIter : public OMS_BasisKeyIterBase
{
private :
  /*! Pointer to a local copy of the key of the object, the iterator is  **
  **  currently pointing to.                                             */
  void* m_pCurrKey;
  /*! Pointer to a local copy of the lower boundary key of the intervall */
  void* m_pLowerKey;
  /*! Pointer to a local copy of the upper boundary key of the intervall */
  void* m_pUpperKey;

public :
  /// Constructor
  OMS_UnloadedVersionKeyIter(
    const void*       pStartKey,
    const void*       pLowerKey,
    const void*       pUpperKey,
    OmsIterDirection  direction,
    OMS_Session*      pSession,
    OMS_ContainerEntry* pContainerInfo
    );
  /// Destructor
  ~OMS_UnloadedVersionKeyIter();
  /// returns the key of the object object. (Method overwrites base method)
  void* GetCurrKey() const { return m_pCurrKey; };
  /// cleans the memory
  void DeleteSelf();
  /// increments the iterator
  inline void operator++();
  /// decrements the iterator
  inline void operator--();             

private:
  /// loads the object-container from the kernel into the oms-cache
  inline OmsObjectContainerPtr LoadObj();
};



/*===================================================================================*/
/*! This method increments the iterator. The next object (oid and object frame) is   **
**  read from the kernel. If the object is already stored in the local oms cache,    **
**  then the delivered object frame is discarded otherwise the object frame is       **
**  inserted into the oms cache.                                                     */
/*===================================================================================*/
inline void OMS_UnloadedVersionKeyIter::operator++()
{
  const char* msg = "OMS_UnloadedVersionKeyIter::++ ";

  m_end = false;
  m_pCurrObj = NULL;

  do {
    tsp00_Int2 DBError;
    m_pSession->m_lcSink->VersionNextKey(
      m_pSession->CurrentContext()->VersionContext(),
      m_pContainerInfo->GetFileId(),
      m_pCurrKey, // Key is changed after call
      m_keyLen,
      false,
      m_pUpperKey,
      *REINTERPRET_CAST(SAPDB_UInt8*, &m_currOid),
      DBError);
    if (e_ok == DBError) {
      m_pCurrObj = LoadObj();
      if (NULL != m_pCurrObj) {
        // Object is not marked as deleted
        break;
      }
    }
    else if (e_no_next_object == DBError) {
        m_currOid.setNil();
        m_end = true;
        break;
    }
    else {
        m_pSession->ThrowDBError (DBError, msg, __MY_FILE__, __LINE__);
    }
  } while (true);

  if (m_end){
    OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << ": end reached");
  }
  else {
    OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg 
             << OMS_UnsignedCharBuffer((const unsigned char*)GetCurrKey(), m_keyLen));
  }
}

/*===================================================================================*/
/*! This method decrements the iterator. The prev object (oid and object frame) is   **
**  read from the kernel. If the object is already stored in the local oms cache,    **
**  then the delivered object frame is discarded otherwise the object frame is       **
**  inserted into the oms cache.                                                     */
/*===================================================================================*/
inline void OMS_UnloadedVersionKeyIter::operator--()
{
  const char* msg = "OMS_UnloadedVersionKeyIter::-- ";

  m_end = false;
  m_pCurrObj = NULL;

  do {
    tsp00_Int2 DBError;
    m_pSession->m_lcSink->VersionPrevKey(
      m_pSession->CurrentContext()->VersionContext(),
      m_pContainerInfo->GetFileId(),
      m_pCurrKey, // Key is changed after call
      m_keyLen,
      false,
      m_pLowerKey,
      *REINTERPRET_CAST(SAPDB_UInt8*, &m_currOid),
      DBError);
    if (e_ok == DBError) {
      m_pCurrObj = LoadObj();
      if (NULL != m_pCurrObj) {
        // Object is not marked as deleted
        break;
      }
    }
    else if (e_no_next_object == DBError) {
        m_currOid.setNil();
        m_end  = true;
        break;
    }
    else {
        m_pSession->ThrowDBError (DBError, msg, __MY_FILE__, __LINE__);
    }
  } while (true);

  if (m_end){
    OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << ": end reached");
  }
  else {
    OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg 
             << OMS_UnsignedCharBuffer((const unsigned char*)GetCurrKey(), m_keyLen));
  }
}

/*===================================================================================*/
/*! First this method checks whether the object corresponding to the current oid is  **
**  already stored in the local oms-cache and if not the object is requested from    **
**  the kernel and inserted into the oms cache. If the object is marked as deleted,  **
**  then NULL is returned, otherwise a pointer to the object frame in the oms-cache. */
/*===================================================================================*/
inline OmsObjectContainerPtr OMS_UnloadedVersionKeyIter::LoadObj()
{
  const char* msg = "OMS_UnloadedVersionKeyIter::LoadObj ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  OmsObjectContainerPtr found = NULL;
  if (m_currOid.isTrue()){
    found = m_pSession->m_context->FindObjInContext(&m_currOid);
    if (!found) {
      // Read object from kernel
      found = m_pSession->m_context->GetObjFromLiveCacheBase(m_pContainerInfo->GetGuid(), 
                                                             m_currOid,
                                                             OMS_Context::NoLock);
      if (!found) {
        m_pSession->ThrowDBError (e_object_not_found, msg, m_currOid, __MY_FILE__, __LINE__);
      }
    }

    if (found->DeletedFlag()){
      // Check whether the object has already been deleted locally
      m_pCurrObjDel = found;
      return NULL;
    }
    else {
      m_pCurrObjDel = NULL;
      return found;
    }
  }
  else {
    return NULL;
  }
}

/*===================================================================================*/


#endif