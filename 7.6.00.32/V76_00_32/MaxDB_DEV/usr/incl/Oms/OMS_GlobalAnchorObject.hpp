/*!
 * \file    OMS_AllocatorInterface.hpp
 * \author  IvanS
 * \brief   OMS allocator interface
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

#ifndef __OMS_GLOBALANCHOROBJECT_HPP
#define __OMS_GLOBALANCHOROBJECT_HPP

#include "Oms/OMS_AFX.h"
#include "Oms/OMS_Defines.h"
#include "Oms/OMS_AllocatorInterface.hpp"
#include "livecachetypes.h" // nocheck

class OMS_RWLock;
class OmsHandle;
class OMS_GlobalAnchorItem;

/*!
 * \brief global anchor scope base class.
 *
 * This class defines an global anchor object.
 *
 */

class AFX_EXT_CLASS OmsGlobalAnchorScopeBase {
public:
  /*!
   * \brief Get reference to global shared memory allocator.
   *
   *  \return reference to OmsAllocatorInterface
   */
  OmsAllocatorInterface &getAllocator();

  /*!
   * \brief Check if global shared memory area is valid.
   *
   *  \return if \c true, memory area is valid, if \c false, memory area is invalid
   */
  bool isValid();
protected:
  /*!
   * \brief Create new global anchor object.
   *
   * Get the global anchor item from kernel and share lock the corresponding R/W lock.
   *
   * \param h OMS handle.
   * \param areaId Id of the app area (user-defined).
   * \param anchorId Id of the global anchor (user-defined).
   * \param anchorClassSize Size of anchor class.
   * \param anchorClassId Id that identifies the anchor class.
   */
  OmsGlobalAnchorScopeBase(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 anchorId, size_t anchorClassSize, OmsTypeInt4 anchorClassId);

  /*!
   * \brief Destruct global anchor object.
   *
   * Unlocks the corresponding R/W lock. 
   * 
   */
  ~OmsGlobalAnchorScopeBase();

  /*!
   * \brief Get the global anchor pointer.
   *
   * Get the entry pointer to the global shared memory
   * 
   */
  void *getAnchorPtr();

  /*!
   * \brief Set the global anchor pointer.
   *
   * Set the entry pointer to the global shared memory
   * 
   * \param p Entry pointer to the anchor class.
   * \param anchorClassSize Size of the anchor class.
   * \param anchorClassId Id of the anchor class.
   */
  void setAnchorPtr(void *p, size_t anchorClassSize, OmsTypeInt4 anchorClassId);

  /*!
   * \brief Set the anchor name (user defined).
   *
   * \param name Anchor name.
   *
   */
  void setAnchorName(const OmsTypeWyde *anchorName);

  /*!
   * \brief Lock the corresponding R/W lock.
   *
   * Lock the corresponding R/W lock, upgrade (from share access to exclusive access) or downgrade is possible
   * 
   */
  void lock(bool exclusive);

private:
  enum {ANCHOR_LOCK_AREA = -1};             // lock area for global anchor objects
  OMS_RWLock *m_pLock;                      // pointer to the corresponding R/W lock
  OMS_GlobalAnchorItem *m_pAnchorItem;   // pointer to the global anchor item
  OmsTypeInt4 m_areaId;                     // Id of the app area
  OmsTypeInt4 m_anchorId;                   // Id of the global anchor

  //private copy constructor, assignment operator and new operator
  //this methods are not allowed for a anchor scope
  OmsGlobalAnchorScopeBase(const OmsGlobalAnchorScopeBase& anchorScope);
  void operator =(const OmsGlobalAnchorScopeBase& anchorScope);
  void *operator new(size_t size);
};


/*!
 * \brief User-specific global anchor scope template.
 *
 * This class defines an global anchor scope.
 *
 */
template<class Anchor>
class OmsGlobalAnchorScope : public OmsGlobalAnchorScopeBase {
public:

   /*!
   * \brief Create new global anchor object.
   *
   * Get the global anchor item from kernel and share lock the corresponding R/W lock.
   *
   * \param h OMS handle,
   * \param areaId Id of the app area (user-defined),
   * \param anchorId Id of anchor object (user-defined),
   */
  OmsGlobalAnchorScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 anchorId) 
    : OmsGlobalAnchorScopeBase(h, areaId, anchorId, sizeof(Anchor), Anchor::ID)
  {}

  /*!
   * \brief Create new global anchor object.
   *
   * Get the global anchor item from kernel and share lock the corresponding R/W lock.
   * 
   * It also creates a new Anchor class if it not exists. First the share lock is unlocked and then
   * exclusive locked. Then the object is created using the GSM-Allocator. After creation the exclusive 
   * lock is unlocked and then share locked.
   * The template methods support anchor constructors with up to four parameters.
   *
   * \param h OMS handle,
   * \param areaId Id of the app area (user-defined),
   * \param anchorId Id of the anchor object (user-defined).
   * \param anchorName Name of the anchor instance (user-defined).
   */
  OmsGlobalAnchorScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 anchorId, const OmsTypeWyde *anchorName) 
    : OmsGlobalAnchorScopeBase(h, areaId, anchorId, sizeof(Anchor), Anchor::ID)
  {
     while(!getAnchorPtr()){
       lock(true/*exclusive*/);
       if(getAnchorPtr() == NULL){
         Anchor *pAnchor = NULL;
         pAnchor = new(getAllocator().omsAllocate(sizeof(Anchor))) Anchor(getAllocator());
         setAnchorPtr(reinterpret_cast<void *>(pAnchor),sizeof(Anchor), Anchor::ID);
         setAnchorName(anchorName);
       }
       lock(false/*share*/);
     }
  }
  template <class P1>
  OmsGlobalAnchorScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 anchorId, const OmsTypeWyde *anchorName, P1 p1) 
    : OmsGlobalAnchorScopeBase(h, areaId, anchorId, sizeof(Anchor), Anchor::ID)
  {
     while(!getAnchorPtr()){
       lock(true/*exclusive*/);
       if(getAnchorPtr() == NULL){
         Anchor *pAnchor = NULL;
         pAnchor = new(getAllocator().omsAllocate(sizeof(Anchor))) Anchor(getAllocator(),p1);
         setAnchorPtr(reinterpret_cast<void *>(pAnchor),sizeof(Anchor), Anchor::ID);
         setAnchorName(anchorName);
       }
       lock(false/*share*/);
     }
  }
  template <class P1,class P2>
  OmsGlobalAnchorScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 anchorId, const OmsTypeWyde *anchorName, P1 p1, P2 p2) 
    : OmsGlobalAnchorScopeBase(h, areaId, anchorId, sizeof(Anchor), Anchor::ID)
  {
     while(!getAnchorPtr()){
       lock(true/*exclusive*/);
       if(getAnchorPtr() == NULL){
         Anchor *pAnchor = NULL;
         pAnchor = new(getAllocator().omsAllocate(sizeof(Anchor))) Anchor(getAllocator(),p1,p2);
         setAnchorPtr(reinterpret_cast<void *>(pAnchor),sizeof(Anchor), Anchor::ID);
         setAnchorName(anchorName);
       }
       lock(false/*share*/);
     }
  }
  template <class P1,class P2,class P3>
  OmsGlobalAnchorScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 anchorId, const OmsTypeWyde *anchorName, P1 p1, P2 p2, P3 p3) 
    : OmsGlobalAnchorScopeBase(h, areaId, anchorId, sizeof(Anchor), Anchor::ID)
  {
     while(!getAnchorPtr()){
       lock(true/*exclusive*/);
       if(getAnchorPtr() == NULL){
         Anchor *pAnchor = NULL;
         pAnchor = new(getAllocator().omsAllocate(sizeof(Anchor))) Anchor(getAllocator(),p1,p2,p3);
         setAnchorPtr(reinterpret_cast<void *>(pAnchor),sizeof(Anchor), Anchor::ID);
         setAnchorName(anchorName);
       }
       lock(false/*share*/);
     }
  }
  template <class P1,class P2,class P3,class P4>
  OmsGlobalAnchorScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 anchorId, const OmsTypeWyde *anchorName, P1 p1, P2 p2, P3 p3, P4 p4) 
    : OmsGlobalAnchorScopeBase(h, areaId, anchorId, sizeof(Anchor), Anchor::ID)
  {
     while(!getAnchorPtr()){
       lock(true/*exclusive*/);
       if(getAnchorPtr() == NULL){
         Anchor *pAnchor = NULL;
         pAnchor = new(getAllocator().omsAllocate(sizeof(Anchor))) Anchor(getAllocator(),p1,p2,p3,p4);
         setAnchorPtr(reinterpret_cast<void *>(pAnchor),sizeof(Anchor), Anchor::ID);
         setAnchorName(anchorName);
       }
       lock(false/*share*/);
     }
  }

  /*!
   * \brief Destruct global anchor object.
   *
   * Unlocks the corresponding R/W lock. 
   * 
   */
  ~OmsGlobalAnchorScope() {}

  /*!
   * \brief Get the global anchor pointer.
   * 
   * \return anchor pointer as entry to the global shared memory,
   * 
   */
  Anchor *getAnchor() {
    Anchor *pAnchor = reinterpret_cast<Anchor*>( getAnchorPtr());
    return pAnchor;
  }

  /*!
   * \brief Delete the global anchor object.
   *
   * First the share lock is unlocked and then exclusive locked. Then the
   * anchor object is deleted using the GSM-Allocator. Afterwards the exclusive 
   * lock is unlocked and then share locked. 
   * 
   */
  void deleteAnchor() {
    OmsAllocatorInterface &alloc = getAllocator();
    lock(true/*exclusive*/);
    alloc.omsDeallocate(getAnchorPtr());
    setAnchorPtr(NULL,0,0);
    if(alloc.omsCheckAllocatorEmpty() == false)
      // allocator is not empty, memory leak /*TODO*/
    lock(false/*share*/);
  }
};

#endif  // __OMS_GLOBALANCHOROBJECT_HPP
