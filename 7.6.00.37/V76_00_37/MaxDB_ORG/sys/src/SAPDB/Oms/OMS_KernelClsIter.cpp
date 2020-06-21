/*!
 * \file    OMS_KernelClsIter.cpp
 * \author  MarkusSi, Roterring
 * \brief   implementation of an oid-iterator for objects created in the kernel
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

#include "Oms/OMS_KernelClsIter.hpp"
#include "Oms/OMS_HResult.hpp"


/*===================================================================================*/
/*! Construtor.                                                                      
**  In this method the arrays to store the buffered oids are allocated and iterator  
**  is positioned onto the first object. When positioning the first oids are already 
**  read and stored in the array. Additionally, the objects corresponding to these   
**  oids are loaded into the oms-cache, too.                                         
**  \param pSession  pointer to the session                                          
**  \param pContainerInfo pointer to the container info of the corresponding class   
**  \param maxBufferSize Number of oids which can be buffered locally.               
*/
/*************************************************************************************/
OMS_KernelClsIter::OMS_KernelClsIter( OMS_Session        *pSession,
                                      OMS_ContainerEntry *pContainerInfo,
                                      int                 maxBufferSize ) 
 : m_pSession(pSession)
 , m_pContainerInfo(pContainerInfo)
 , m_allObjReadFromKernel(false)
 , m_end(false)                
 , m_objectsAreLoaded(false)   
 , m_pCurrObj(NULL)  
 , m_refCnt(1)
 , m_currIdx(0)                
 , m_maxIdx(-1)                 
 , m_maxNoOfOid(maxBufferSize) 
 , m_pOid(NULL)                
{
  const char* msg = "OMS_KernelClsIter::Ctor ";

  if (m_maxNoOfOid < 1){
    m_pSession->ThrowDBError (e_invalid_iterator, msg, __MY_FILE__, __LINE__);
  }

  if (m_maxNoOfOid > OMS_MASS_OPERATION_CNT){
    m_maxNoOfOid = OMS_MASS_OPERATION_CNT;
  }

  m_isVarObjContainer = m_pContainerInfo->GetClassEntry().IsVarObject();

  // Assign the correctly typed pointers to the untyped arrays
  m_pLoadOid = reinterpret_cast<OMS_ObjectId8*>(&m_pLoadOid_u[0]);
  m_pOid = reinterpret_cast<OMS_ObjectId8*>(&m_pOid_u[0]);

  // Initialize first element of m_pOid with 0 as this element is used as starting 
  // point for reading from kernel
  m_pOid[0].setNil();

  for (int i=0; i<m_maxNoOfOid; ++i){
    m_pObjVers[i].gg91SetNilRef();
  }

  // Position Iterator onto the first oid
  ++(*this);
}

/*************************************************************************************/

void OMS_KernelClsIter::DeleteSelf()
{
  const char* msg = "OMS_KernelClsIter::DeleteSelf ";

  this->~OMS_KernelClsIter();
  m_pSession->deallocate(this);
}

/*************************************************************************************/

OMS_KernelClsIter::~OMS_KernelClsIter()
{}
