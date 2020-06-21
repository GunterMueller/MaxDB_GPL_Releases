/*!
 * \file    OMS_VersionClsIter.hpp
 * \author  MarkusSi, Roterring
 * \brief   implementation of an oid-iterator to read objects created in a version
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


#ifndef _OMS_VERSIONCLSITER_HPP
#define _OMS_VERSIONCLSITER_HPP

#include "Oms/OMS_Oid.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_Session.hpp"


/*===================================================================================*/
/*! Class defines an iterator which iterates over all objects, that belong to the    
**  given class and container and which are currently stored in an non-unloaded      
**  version.                                                                         
**  \since PTS 1120478                                                               
*/  
class OMS_VersionClsIter {

private:
  /// Pointer to the version in which the iterator was created
  OMS_Session         *m_pSession;
  /// How many OmsObjByClsIterBase instances are refering to this instance   
  int                  m_refCnt;
  /// Pointer to the object frame in the oms-cache the iterator is currently pointing to
  OmsObjectContainer*  m_pCurrObj;
  OMS_List<OmsObjectContainer>::Iter m_iterator;

public:
  /// Constructor
  inline OMS_VersionClsIter(OMS_Session *pSession, OMS_ContainerEntry *pContainerEntry);
  /// Destructor
  inline ~OMS_VersionClsIter(){};

  /// Cleanup
  inline void DeleteSelf();
  /// returns a pointer to the frame of the object the iterator is currently pointing to
  OmsObjectContainer* GetCurrObj () const
  { return m_pCurrObj; }
  /// returns the oid of the object the iterator is currently pointing to
  const OMS_ObjectId8&  GetCurrOid () const
  { return m_pCurrObj ? m_pCurrObj->m_oid : OMS_Globals::m_globalsInstance->m_nilOid; }
  /// yields true, if the iterator has not reached its end
  operator bool () const
  { return m_pCurrObj != NULL; }
  /// increments the kernel iterator
  inline void operator++ ();

  /// Increase reference counting
  void IncRefCnt()
  { ++m_refCnt; }
  /// Decrease reference counting
  int DecRefCnt()
  { return --m_refCnt; }
};



/*===================================================================================*/
/*! Construtor.                                                                      
**  In this method the iterator is created and it is positioned onto the first       
**  object.                                                                          
**  \param pContainerEntry pointer to the container info    
*/
/*************************************************************************************/
inline OMS_VersionClsIter::OMS_VersionClsIter(OMS_Session        *pSession,
                                              OMS_ContainerEntry *pContainerEntry)
: m_pSession(pSession)
, m_refCnt(1)
, m_pCurrObj(NULL)
, m_iterator()
{
  const char* msg = "OMS_VersionClsIter::Ctor ";

  m_iterator.Reset(pContainerEntry->GetNewVersionObjects());
  do {
    m_pCurrObj = m_iterator();
    ++m_iterator;
    //m_pCurrObj = const_cast<OmsObjectContainer*>
    //           (m_pSession->m_context->m_newObjCache.nextObject(m_pContainerInfo->GetContainerHandle(),
    //                                                            m_pContainerInfo->GetObjectSize(), 
    //                                                            m_pCurrObj));
  } while (m_pCurrObj && (m_pCurrObj->DeletedFlag() || m_pCurrObj->m_objseq.gg91IsDummyRef()));
}

/*===================================================================================*/

inline void OMS_VersionClsIter::DeleteSelf()
{
  const char* msg = "OMS_VersionClsIter::DeleteSelf ";

  this->~OMS_VersionClsIter();
  m_pSession->deallocate(this);
}

/*===================================================================================*/

inline void OMS_VersionClsIter::operator++ ()
{
  const char* msg = "OMS_VersionClsIter::++ ";

  do {
    m_pCurrObj = m_iterator();
    ++m_iterator;
    //m_pCurrObj = const_cast<OmsObjectContainer*>
    //             (m_pSession->m_context->m_newObjCache.nextObject(m_pContainerInfo->GetContainerHandle(),
    //                                                              m_pContainerInfo->GetObjectSize(), 
    //                                                              m_pCurrObj));
  } while (m_pCurrObj && (m_pCurrObj->DeletedFlag() || m_pCurrObj->m_objseq.gg91IsDummyRef()));
}

/*===================================================================================*/

#endif  // _OMS_VERSIONCLSITER_HPP
