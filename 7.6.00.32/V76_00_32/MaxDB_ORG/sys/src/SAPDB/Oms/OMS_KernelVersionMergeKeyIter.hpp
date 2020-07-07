/*!
 * \file    OMS_KernelVersionMergeKeyIter.hpp
 * \author  MarkusSi, Roterring
 * \brief   iterator which merges the kernel iterator and the version iterator
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

#ifndef __OMS_KERNELVERSIONMERGEKEYITER_HPP
#define __OMS_KERNELVERSIONMERGEKEYITER_HPP

#include "Oms/OMS_KeyIterBase.hpp"
#include "Oms/OMS_VersionMergeKeyIter.hpp"
#include "Oms/OMS_KernelKeyIter.hpp"  


/*----------------------------------------------------------------------*/
/*! Class defines a key iterator over all keyed objects.                
**  This is a merge of OMS_VersionMergeKeyIter and OMS_KernelKeyIter.   
**  \since PTS 1119480                                                  
*/
/*----------------------------------------------------------------------*/
class OMS_KernelVersionMergeKeyIter : public OMS_MergeKeyIterBase
{
private:
  /*! If this variable equals true, then the iterator for versions is active  
  **  and must be considered when determine the next object.                  
  */
  bool                     m_versionActive;

  /*! If this variable equals true, then the iterator for objects in the      
  **  kernel is active and must be considered when determine the next object. 
  */
  bool                     m_kernelActive;

  /*! The direction the iterator is currently running.  */
  OmsIterDirection         m_direction;

  /*! Reference counter. If the iterator OmsObjByKeyIterBase is copied, then  
  **  only the pointer to an instance of type OMS_KernelVersionMergeKeyIter   
  **  is copied. This counter keeps track of how many pointers are currently  
  **  pointing to this instance. A destruction of the iterator is only        
  **  allowed, if the counter equals 0.                                       
  */
  int                      m_refCnt;

  /*! Pointer to the iterator for version objects. If this pointer equals     
  **  NULL then either no version is active or there are no objects within    
  **  the given boundary.                                                     
  */
  OMS_VersionMergeKeyIter* m_pVersionIterator;

  /*! Pointer to the iterator for kernel objects. If this pointer equals NULL   
  **  then either no version is active or there are no objects within the        
  **  given boundary.                                                         
  */
  OMS_KernelKeyIter*       m_pKernelIterator;
  
public :
  /// Constructor
  OMS_KernelVersionMergeKeyIter(
    const void*       pStartKey,
    const void*       pLowerKey,
    const void*       pUpperKey,
    OmsIterDirection  direction,
    OMS_Session*      pSession,
    OMS_ContainerEntry* pContainerInfo,
    int               maxBufferSize
    );
  /// Destructor
  ~OMS_KernelVersionMergeKeyIter();
  /// increments the iterator
  inline void operator++();
  /// decrements the iterator
  inline void operator--();  
  /// cleans the memory
  void DeleteSelf();

  /// increments the reference counter
  void IncRefCnt()  { ++m_refCnt;                    }
  /// decrements the reference counter and returns the current count
  int  DecRefCnt()  { --m_refCnt;  return m_refCnt;  }

  OMS_Session*        GetSession()       const { return m_pSession;       }
  OMS_ContainerEntry* GetContainerInfo() const { return m_pContainerInfo; }

private:
  /// changes the running direction
  inline bool ChangeDirection(OmsIterDirection direction);
};


/*===================================================================================*/
/*! This method increments the merge iterator. To achieve this, first the current    
**  iterator (the iterator which contains the current object) is incremented and     
**  then it is checked which of both iterators delivers the next largest object and  
**  this iterator is defined as the new current iterator.                            
*/
/*===================================================================================*/
inline void OMS_KernelVersionMergeKeyIter::operator++()
{
  const char* msg = "OMS_KernelVersionMergeKeyIter::++ ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);
    
  if (m_direction == OMS_ASCENDING){
    ++(*m_pCurrIterator);
    if (!(*m_pCurrIterator)){
      if(m_pCurrIterator == m_pKernelIterator){
        m_kernelActive = false;
      }
      else {
        m_versionActive = false;
      }
      m_pCurrIterator = NULL;
    }
  }
  else {
    // There is change in the direction.  
    // As the iterators are currently pointing to objects which are equal or 
    // larger than the current object it is necessary to increase all iterators
    // so that the next largest object can be found.
    ChangeDirection(OMS_ASCENDING);
  }
  
  // Determine next object 
  if (m_kernelActive  == true && m_versionActive == true){
    // Get smallest object either from kernel or from version
    int cmpResult = memcmp(m_pKernelIterator->GetCurrKey(),
                           m_pVersionIterator->GetCurrKey(),
                           m_keyLen);
    if (cmpResult <= 0){
      m_pCurrIterator = m_pKernelIterator;
    }
    else {
      m_pCurrIterator = m_pVersionIterator;
    }
  }
  else if (m_kernelActive  == true) {
    // Only kernel iterator is non-empty
    m_pCurrIterator = m_pKernelIterator;
  }
  else if (m_versionActive == true) { 
    // Only version iterator is non-empty
    m_pCurrIterator = m_pVersionIterator;
  }
  else {
    // Iterators are empty
    m_pCurrIterator = NULL;
  }

  m_direction = OMS_ASCENDING;
}

/*===================================================================================*/
/*! This method decrements the merge iterator. To achieve this, first the current    
**  iterator (the iterator which contains the current object) is decremented and     
**  then it is checked which of both iterators delivers the next largest object and  
**  this iterator is defined as the new current iterator.                            
*/
/*===================================================================================*/
inline void OMS_KernelVersionMergeKeyIter::operator--()
{
  const char* msg = "OMS_KernelVersionMergeKeyIter::-- ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  if (m_direction == OMS_DESCENDING){
    --(*m_pCurrIterator);
    if (!(*m_pCurrIterator)){
      if(m_pCurrIterator == m_pKernelIterator){
        m_kernelActive = false;
      }
      else {
        m_versionActive = false;
      }
      m_pCurrIterator = NULL;
    }
  }
  else {
    // There is change in the direction. then decrease all iterators 
    // As the iterators are currently pointing to objects which are equal or 
    // smaller than the current object it is necessary to decrease all iterators
    // so that the next smallest object can be found.
    ChangeDirection(OMS_DESCENDING);
  }

  // Determine previous object 
  if (m_kernelActive  == true && m_versionActive == true){
    // Get largest object either from kernel or from version
    int cmpResult = memcmp(m_pKernelIterator->GetCurrKey(),
                           m_pVersionIterator->GetCurrKey(),
                           m_keyLen);
    if (cmpResult >= 0){
      m_pCurrIterator = m_pKernelIterator;
    }
    else {
      m_pCurrIterator = m_pVersionIterator;
    }
  }
  else if (m_kernelActive  == true) {
    // Only kernel iterator is non-empty
    m_pCurrIterator = m_pKernelIterator;
  }
  else  if (m_versionActive == true) { 
    // Only version iterator is non-empty
    m_pCurrIterator = m_pVersionIterator;
  }
  else {
    // Iterators are empty
    m_pCurrIterator = NULL;
  }
  m_direction = OMS_DESCENDING;
}

/*===================================================================================*/
/*! This method changes the direction the iterator is running. If the new direction  
**  equals OMS_DESCENDING, then the method first decrements both iterator (if they   
**  are active) and then compares the current objects to determine the largest one   
**  and the corresponding iterator is then defined as new current iterator.          
**  The handling is analogous if the new direction equals OMS_ASCENDING.             
**  \param direction new direction the iterator should run                           
**  \return true if there is at least one of the iterators active after the change   
**          of the direction.                                                        
*/
/*===================================================================================*/
inline bool OMS_KernelVersionMergeKeyIter::ChangeDirection(OmsIterDirection direction)
{
  const char* msg = "OMS_KernelVersionMergeKeyIter::ChangeDirection ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  if (m_pKernelIterator != NULL){
    m_kernelActive = m_pKernelIterator->ChangeDirection(direction, GetCurrKey());
  }
  
  if (m_pSession->InVersion() && m_pVersionIterator != NULL){
    m_versionActive = m_pVersionIterator->ChangeDirection(direction);
  }


  if (m_kernelActive && m_versionActive){
    int cmpResult = memcmp(m_pKernelIterator->GetCurrKey(),
                           m_pVersionIterator->GetCurrKey(),
                           m_keyLen);
    if (  cmpResult <= 0 && direction == OMS_ASCENDING
       || cmpResult >= 0 && direction == OMS_DESCENDING) {
      m_pCurrIterator = m_pKernelIterator;
    }
    else {
      m_pCurrIterator = m_pVersionIterator;
    }

  }
  else if (m_kernelActive){
    m_pCurrIterator = m_pKernelIterator;
  }
  else if (m_versionActive){
    m_pCurrIterator = m_pVersionIterator;
  }
  else {
    m_pCurrIterator = NULL;
  }

  return (m_kernelActive || m_versionActive);
}

/*===================================================================================*/

#endif