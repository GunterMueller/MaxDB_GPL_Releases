/*!
 * \file    OMS_KeyIterBase.hpp
 * \author  MarkusSi, Roterring
 * \brief   base classes for key iterators
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

#ifndef __OMS_KEYITERBASE_HPP
#define __OMS_KEYITERBASE_HPP

#include "Oms/OMS_IterDirection.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_ContainerEntryDef.hpp"


/*----------------------------------------------------------------------*/
/*! Basis class for all iterators                                       **
**  \since PTS 1119480                                                  */
/*----------------------------------------------------------------------*/
class OMS_KeyIterBase   
{
protected:
  /*! Length of the key. */
  size_t                m_keyLen;
  /*! Pointer to the corresponding session */
  OMS_Session*          m_pSession;
  /*! Pointer to the container information. */ 
  OMS_ContainerEntry*   m_pContainerInfo;

public:
  /// Constructor
  OMS_KeyIterBase (OMS_Session*        pSession,
                   OMS_ContainerEntry* pContainerInfo)
  : m_keyLen(pContainerInfo->GetClassEntry().GetKeyDesc().GetLen())
  , m_pSession(pSession)
  , m_pContainerInfo(pContainerInfo) 
  {}

  /// returns true is iterator is not empty; otherwise false 
  virtual operator bool() const = 0;
  /// increments the iterator
  virtual void operator++() = 0;
  /// decrements the iterator
  virtual void operator--() = 0; 
  /// returns a pointer to the current key
  virtual void* GetCurrKey() const = 0;
  /// returns a pointer to the current object
  virtual OmsObjectContainer* GetCurrObj(bool noCheck = false) const = 0;
  /// cleanup
  virtual void DeleteSelf() = 0;
};



/*----------------------------------------------------------------------*/
/*! Basis class for all basis iterators, which means, iterators which   **
**  corresponds to a certain data source like e.g. kernel, in memory    **
**  version, or unloaded version                                        **
**  \since PTS 1119480                                                  */
/*----------------------------------------------------------------------*/
class OMS_BasisKeyIterBase : public OMS_KeyIterBase
{
protected:
  /*! Variable is set to true if iterator is empty. */
  bool                  m_end;
  /*! Pointer to the current object. If the delete flag is set, then the     **
  **  pointer equals NULL. (\see OMS_BasisKeyIterBase::m_pCurrObjDel)        */
  OmsObjectContainer*   m_pCurrObj;
  /*! Pointer to the current object, if the delete flag is set.              **
  **  (\see OMS_BasisKeyIterBase::m_pCurrObj)                                */
  OmsObjectContainer*   m_pCurrObjDel;
  /*! Oid of the current object. */
  OMS_ObjectId8         m_currOid;

public:
  /// Constructor
  OMS_BasisKeyIterBase
  ( OMS_Session*  pSession,
    OMS_ContainerEntry* pContainerInfo
  )
  : OMS_KeyIterBase (pSession, pContainerInfo)
  , m_end(false)
  , m_pCurrObj(NULL)
  , m_pCurrObjDel(NULL)
  { m_currOid.setNil(); }
  
  /// returns true is iterator is not empty; otherwise false 
  operator bool() const  { return !m_end; }

  /// returns a pointer to the current object
  /*! This method returns a pointer to the current object frame in the local **
  **  oms cache.                                                             **
  **  \param noCheck If true the pointer to the frame is returned even if    **
  **         the object is marked as deleted; otherwise the pointer is only  **
  **         returned if it is not marked as deleted.                        */
  OmsObjectContainer* GetCurrObj(bool noCheck = false) const { 
    if (m_pCurrObj != NULL) 
      return m_pCurrObj; 
    else if (m_pCurrObjDel != NULL && noCheck == true) 
      return m_pCurrObjDel;
    else 
      return NULL;
  }

  /// returns a pointer to the current key
  /*! This method returns a pointer to the key of the current object. The    **
  **  key is returned even if the object is marked as deleted in the local   **
  **  oms cache.                                                             */
  void* GetCurrKey() const {
    OmsObjectContainer* pObj = GetCurrObj(true);
    return pObj != NULL ? m_pContainerInfo->GetClassEntry().GetKeyPtr(pObj) : NULL; 
  }
};




/*----------------------------------------------------------------------*/
/*! Basis class for all merge iterators, which means, iterators which   **
**  merge the sorted streams of two other iterators into a single       **
**  sorted stream.                                                      **
**  \since PTS 1119480                                                  */
/*----------------------------------------------------------------------*/
class OMS_MergeKeyIterBase : public OMS_KeyIterBase
{
protected:
  /*! Pointer to the iterator which contains the object, the iterator **
  **  is currently pointing to.                                       */
  OMS_KeyIterBase*             m_pCurrIterator;

public:
  /// Constructor
  OMS_MergeKeyIterBase
  ( OMS_Session*        pSession,
    OMS_ContainerEntry* pContainerInfo
  )
  : OMS_KeyIterBase(pSession, pContainerInfo)
  , m_pCurrIterator(NULL)
  {}
 
  /// returns true is iterator is not empty; otherwise false 
  operator bool() const                         
  { return m_pCurrIterator != NULL; }

  /// returns a pointer to the current object
  OmsObjectContainer* GetCurrObj(bool noCheck = false) const 
  { return m_pCurrIterator->GetCurrObj(); }

  /// returns a pointer to the current key
  void* GetCurrKey() const 
  { return m_pCurrIterator->GetCurrKey(); }

protected:
  /// changes the direction the iterator is currently running
  virtual bool ChangeDirection(OmsIterDirection NewDirection) = 0;
};

#endif
