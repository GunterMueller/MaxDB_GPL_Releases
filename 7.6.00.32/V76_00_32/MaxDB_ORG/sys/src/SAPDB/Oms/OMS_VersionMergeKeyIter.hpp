/*!
 * \file    OMS_VersionMergeKeyiter.hpp
 * \author  MarkusSi, Roterring
 * \brief   iterator which merges the two version iterators
 */

/*
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


#ifndef __OMS_VERSIONMERGEKEYITER_HPP
#define __OMS_VERSIONMERGEKEYITER_HPP

#include "Oms/OMS_KeyIterBase.hpp"
#include "Oms/OMS_UnloadedVersionKeyIter.hpp"
#include "Oms/OMS_InMemVersionKeyIter.hpp"

/*----------------------------------------------------------------------*/
/*! Class defines a key iterator over all keyed objects, that have      **
**  been created in a version. This is a merge of                       **
**  OMS_InMemVersionKeyIter and OMS_UnloadedVersionKeyIter              **
**  \since PTS 1119480                                                  */
/*----------------------------------------------------------------------*/
class OMS_VersionMergeKeyIter : public OMS_MergeKeyIterBase
{
private :
  /*! If both iterators are pointing to the object with identical key, then    **
  **  this variable is set to true; else false                                 */
  bool                        m_cmpEqual;
  /*! If this variable equals true, then the iterator for objects of unloaded  **
  **  versions is active and must be considered when determine the next object */
  bool                        m_unloadedVersionActive;
  /*! If this variable equals true, then the iterator for objects of normal    **
  **  versions is active and must be considered when determine the next object */
  bool                        m_inMemVersionActive;
  /*! Pointer to the iterator for unloaded version. If this pointer equals     **
  **  NULL then either there is no unloaded version or there are no objects    **
  **  within the given boundary.                                               */
  OMS_UnloadedVersionKeyIter* m_pUnloadedVersionIter;
  /*! Pointer to the iterator for normal version. If this pointer equals       **
  **  NULL then either no version is active or there are no objects within the **
  **  given boundary.                                                          */
  OMS_InMemVersionKeyIter*    m_pInMemVersionIter;

public :
  /// Constructor
  OMS_VersionMergeKeyIter(
    const void*       pStartKey,
    const void*       pLowerKey,
    const void*       pUpperKey,
    OmsIterDirection  direction,
    OMS_Session*      pSession,
    OMS_ContainerEntry* pContainerInfo
    );
  /// Destructor
  ~OMS_VersionMergeKeyIter();
  /// increments the iterator
  inline void operator++();
  /// decrements the iterator
  inline void operator--(); 
  /// cleans the memory
  void DeleteSelf();
  /// changes the running direction of the iterator
  inline bool ChangeDirection(OmsIterDirection direction);
};


/*===================================================================================*/
/*! This method increments the merge iterator. To achieve this, first the current    **
**  iterator (the iterator which contains the current object) is incremented and     **
**  then it is checked which of both iterators delivers the next largest object and  **
**  this iterator is defined as the new current iterator.                            **
**  As it is possible that both iterators are pointing to the same object, it might  **
**  be necessary to increment both iterators. As the iterator for 'normal' objects   **
**  contains the newer object, the object is always taken from this one, if both     **
**  contain the same object.                                                         */
/*===================================================================================*/
inline void OMS_VersionMergeKeyIter::operator++()
{
  const char* msg = "OMS_VersionMergeKeyIter::++ ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  // Increment iterator which delivers the current object. If both iterators
  // are pointing to the same object, then an increment on both is necessary.
  // If iterator is empty after the operation, then set the iterator inactive.
  ++(*m_pCurrIterator);
  if (!(*m_pCurrIterator)){
    if (m_pCurrIterator == m_pInMemVersionIter){
      m_inMemVersionActive = false;
    }
    else {
      m_unloadedVersionActive = false;
    }
    m_pCurrIterator = NULL;
  }
  if (m_cmpEqual){
    ++(*m_pUnloadedVersionIter);
    if (!(*m_pUnloadedVersionIter)){
      m_unloadedVersionActive = false;
    }
    m_cmpEqual = false;
  }

  // Determine next object 
  if (m_unloadedVersionActive == true && m_inMemVersionActive == true){
    // Get smallest object either from unloaded or in-memory iterator
    int cmpResult = memcmp(m_pInMemVersionIter->GetCurrKey(),
                           m_pUnloadedVersionIter->GetCurrKey(),
                           m_keyLen);
    if (cmpResult == 0){
      m_pCurrIterator = m_pInMemVersionIter;
      m_cmpEqual      = true;
    }
    else if (cmpResult < 0){
      m_pCurrIterator = m_pInMemVersionIter;
   }
    else {
      m_pCurrIterator = m_pUnloadedVersionIter;
    }
  }
  else if (m_inMemVersionActive == true) {
    // Only in-memory iterator is non-empty
    m_pCurrIterator = m_pInMemVersionIter;
  }
  else if (m_unloadedVersionActive == true) {
    // Only version iterator is non-empty
    m_pCurrIterator = m_pUnloadedVersionIter;
  }
  else {
    // Iterators are empty
    m_pCurrIterator = NULL;
  }
}

/*===================================================================================*/
/*! This method decrements the merge iterator. To achieve this, first the current    **
**  iterator (the iterator which contains the current object) is decremented and     **
**  then it is checked which of both iterators delivers the next smallest object and **
**  this iterator is defined as the new current iterator.                            **
**  As it is possible that both iterators are pointing to the same object, it might  **
**  be necessary to decrement both iterators. As the iterator for 'normal' objects   **
**  contains the newer object, the object is always taken from this one, if both     **
**  contain the same object.                                                         */
/*===================================================================================*/
inline void OMS_VersionMergeKeyIter::operator--()
{
  const char* msg = "OMS_VersionMergeKeyIter::-- ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  // Decrement iterator which delivers the current object. If both iterators
  // are pointing to the same object, then an decrement on both is necessary. 
  // If iterator is empty after the operation, then set the iterator inactive.
  --(*m_pCurrIterator);
  if (!(*m_pCurrIterator)){
    if (m_pCurrIterator == m_pInMemVersionIter){
      m_inMemVersionActive = false;
    }
    else {
      m_unloadedVersionActive = false;
    }
    m_pCurrIterator = NULL;
  }
  if (m_cmpEqual){
    --(*m_pUnloadedVersionIter);
    if (!(*m_pUnloadedVersionIter)){
      m_unloadedVersionActive = false;
    }
    m_cmpEqual = false;
  }

  // Determine previous object 
  if (m_unloadedVersionActive == true && m_inMemVersionActive == true){
    // Get largest object either from unloaded or in-memory iterator
    int cmpResult = memcmp(m_pInMemVersionIter->GetCurrKey(),
                           m_pUnloadedVersionIter->GetCurrKey(),
                           m_keyLen);
    if (cmpResult == 0){
      m_pCurrIterator = m_pInMemVersionIter;
      m_cmpEqual      = true;
    }
    else if (cmpResult > 0){
      m_pCurrIterator = m_pInMemVersionIter;
    }
    else {
      m_pCurrIterator = m_pUnloadedVersionIter;
   }
  }
  else if (m_inMemVersionActive == true) {
    // Only in-memory iterator is non-empty
    m_pCurrIterator = m_pInMemVersionIter;
  }
  else   if (m_unloadedVersionActive == true) {
    // Only version iterator is non-empty
    m_pCurrIterator = m_pUnloadedVersionIter;
  }
  else {
    // Iterators are empty
    m_pCurrIterator = NULL;
  }
}

/*===================================================================================*/
/*! This method changes the direction the iterator is running. If the new direction  **
**  equals OMS_DESCENDING, then the method first decrements both iterator (if they   **
**  are active) and then compares the current objects to determine the largest one   **
**  and the corresponding iterator is then defined as new current iterator.          **
**  The handling is analogous if the new direction equals OMS_ASCENDING.             **
**  \param direction new direction the iterator should run                           **
**  \return true if there is at least one of the iterators active after the change   **
**          of the direction.                                                        */
/*===================================================================================*/
inline bool OMS_VersionMergeKeyIter::ChangeDirection(OmsIterDirection direction)
{
  const char* msg = "OMS_VersionMergeKeyIter::ChangeDirection ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  if (m_pInMemVersionIter != NULL){
    m_inMemVersionActive = true;
    if (direction == OMS_DESCENDING){
      --(*m_pInMemVersionIter);
    }
    else {
      ++(*m_pInMemVersionIter);
    }
    if (!(*m_pInMemVersionIter)){
      m_inMemVersionActive = false;
    }
  }


  if (m_pSession->CurrentContext()->IsUnloaded()){
    if (m_pUnloadedVersionIter != NULL){
      m_unloadedVersionActive = true;
      if (direction == OMS_DESCENDING){
        --(*m_pUnloadedVersionIter);
      }
      else {
        ++(*m_pUnloadedVersionIter);
      }
      if (!(*m_pUnloadedVersionIter)){
        m_unloadedVersionActive = false;
      }
    }
  }

  if (m_inMemVersionActive && m_unloadedVersionActive){
    int cmpResult = memcmp(m_pInMemVersionIter->GetCurrKey(),
                           m_pUnloadedVersionIter->GetCurrKey(),
                           m_keyLen);
    if (  cmpResult <= 0 && direction == OMS_ASCENDING
       || cmpResult >= 0 && direction == OMS_DESCENDING) {
      m_pCurrIterator = m_pInMemVersionIter;
    }
    else {
      m_pCurrIterator = m_pUnloadedVersionIter;
    }
  }
  else if (m_inMemVersionActive){
    m_pCurrIterator = m_pInMemVersionIter;
  }
  else if (m_unloadedVersionActive){
    m_pCurrIterator = m_pUnloadedVersionIter;
  }
  else {
    m_pCurrIterator = NULL;
  }

  return (m_inMemVersionActive || m_unloadedVersionActive);
}


#endif