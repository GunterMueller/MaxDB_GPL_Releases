/*!
 * \file    OMS_KernelClsIter.hpp
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


#ifndef __OMS_KERNELCLSITER_HPP
#define __OMS_KERNELCLSITER_HPP

#include "Oms/OMS_Oid.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_ContainerEntryDef.hpp"
#include "Oms/OMS_Defines.h"


/*===================================================================================*/
/*! Class defines an iterator which iterates over all objects, that belong to the    
**  given class and container and which are currently stored in the kernel.          
**  This includes objects which are created in the kernel and objects belonging to   
**  an unloaded version. The iterator returns first those objects which are created  
**  in the kernel and if no such objects are left, then it looks for objects of      
**  unloaded versions                                                                
**  \since PTS 1120478                                                                 
*/
class OMS_KernelClsIter {

private:
  /*! Variable is set to true if iterator is empty. */
  bool                 m_end;

  /*! When loading object oid's from the kernel this member is set to false. 
  **  Then it is checked whether the objects, corresponding to these oid's   
  **  are already stored in the local oms cache and all objects which are    
  **  not in the oms cache are loaded using a mass deref-operation (\see     
  **  OMS_KernelClsIter::LoadObj). After this operation all objects should   
  **  reside in the cache and the member variable is set to true.            
  */
  bool                 m_objectsAreLoaded;

  /*! If true, then the kernel iterator is already empty, but there are      
  **  might some oids left in the local buffer.                              
  */
  bool                 m_allObjReadFromKernel;

  /// Is the iterator running on a container which contains variable sized objects 
  bool                 m_isVarObjContainer;

  /*! This member contains the index of the current object in the array      
  **  OMS_KernelClsIter::m_pOid.                                             
  */ 
  int                  m_currIdx;

  /*! This member contains the maximal index of the array m_pOid.  */
  int                  m_maxIdx;

  /*! Maximal size of the array OMS_KernelClsIter::m_pOid. So this member    
  **  determines the maximal number of oid's which can be read from the      
  **  kernel.                                                                
  */
  int                  m_maxNoOfOid;

  /*! How many OmsObjByClsIterBase instances are refering to this instance   
  */
  int                  m_refCnt;

  /*! Pointer to the object in the local oms cache which corresponds to the  
  **  oid the iterator is currently pointing to                              
  */
  OmsObjectContainer*  m_pCurrObj;

  /*! Pointer to the current session */
  OMS_Session*         m_pSession;

  /*! Pointer to the class meta data  */
  OMS_ContainerEntry*  m_pContainerInfo;

  /*! Array to store the oid's which are read in a mass-operation from the   
  **  kernel. The size of the array is determined by                         
  **  OMS_KernelClsIter::m_maxNoOfOid, but the maximal size is restricted 
  **  by the constant OMS_ITER_MAX_OID_BUFFER. The valid entries are stored in the   
  **  buckets 0 to OMS_KernelClsIter::m_maxIdx. The entry the iterator is    
  **  currently pointing to is given by OMS_KernelClsIter::m_currIdx.        
  **
  **  To prevent the running of the constructor for every entry in the array,
  **  the memory is allocated using a character type and later on the correctly
  **  typed pointer is assigned to this array.
  */
  OmsTypeUInt8        m_pOid_u[OMS_MASS_OPERATION_CNT ];
  OMS_ObjectId8      *m_pOid;

  /*! In this array the Log-Oid of the oids are stored. These oids can be used
  **  to speed up the mass deref as with these oids the object can be accessed
  **  directly in the log instead of starting with the current object version 
  **  and then searching the history chain for the version corresponding to the
  **  current consistent view.
  */
  tgg91_PageRef       m_pObjVers[OMS_MASS_OPERATION_CNT];

  /*! 
  ** \name Arrays which are needed for the mass deref
  */
  //@{
  // Prevent running the constructor for every entry of the array
  //OmsObjectId          m_pLoadOid      [OMS_MASS_OPERATION_CNT];
  OmsTypeUInt8           m_pLoadOid_u    [OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8         *m_pLoadOid;
  tgg00_BasisError       m_pDBError      [OMS_MASS_OPERATION_CNT];
  OmsObjectContainer    *m_ppObjContainer[OMS_MASS_OPERATION_CNT];
  OmsAbstractObject     *m_ppObj         [OMS_MASS_OPERATION_CNT];
  OMS_ClassEntry        *m_ppClassInfo   [OMS_MASS_OPERATION_CNT];
  tgg01_ContainerId      m_pContainerId  [OMS_MASS_OPERATION_CNT];
  size_t                 m_pObjSize      [OMS_MASS_OPERATION_CNT];
  tgg91_TransNo          m_pUpdTransId   [OMS_MASS_OPERATION_CNT];
  //@}

public:
  /// Constructor
  OMS_KernelClsIter(OMS_Session *pSession, OMS_ContainerEntry *pContainerInfo, int maxBufferSize);
  /// Destructor
  ~OMS_KernelClsIter();

  /// Cleanup
  void DeleteSelf();
  /// returns a pointer to the frame of the object the iterator is currently pointing to
  OmsObjectContainer* GetCurrObj () const
  { return m_end ? NULL : m_pCurrObj; }
  /// returns the oid of the object the iterator is currently pointing to
  const OMS_ObjectId8&  GetCurrOid () const
  { return m_end ? OMS_Globals::m_globalsInstance->m_nilOid : m_pOid[m_currIdx]; }
  void SetCurrOid(OMS_ObjectId8 &oid)
  { m_pOid[m_currIdx] = oid; }
  /// yields true, if the iterator has not reached its end
  operator bool () const
  { return !m_end; }
  /// increments the kernel iterator
  inline void operator++ ();

  /// Increase reference counting
  void IncRefCnt()
  { ++m_refCnt; }
  /// Decrease reference counting
  int DecRefCnt()
  { return --m_refCnt; }

private:
  // loads the objects, corresponding to the oids in the buffer, into the local oms-cache
  inline OmsObjectContainer* LoadObj();
};



/*===================================================================================*/
/*! This method loads the object frames corresponding to the buffered oids into the  
**  local oms-cache if not already done and it returns the pointer to the current    
**  object frame in the oms-cache.                                                   
**  If the objects are not already loaded then first all oids of those objects which 
**  are not already stored in the oms cache are collected and with all these a       
**  mass-deref to the kernel is executed.                                            
**  \return pointer to the object frame in the oms cache of the current object.      
*/
/*===================================================================================*/
inline OmsObjectContainer* OMS_KernelClsIter::LoadObj ()
{
  const char* msg = "OMS_KernelClsIter::LoadObj ";
  
  OmsObjectContainerPtr found = NULL;

  if (!OMS_Globals::m_globalsInstance->InProcServer() || m_isVarObjContainer){
    // InProc case
    if (! m_isVarObjContainer ){
      m_pSession->IncDeref();
      found = m_pSession->m_context->FindObjInContext(&(GetCurrOid()), true /*ignoreGeneration*/);
      if (found == NULL) {
          found = m_pSession->m_context->GetObjFromLiveCacheBase(m_pContainerInfo->GetGuid(), GetCurrOid(), OMS_Context::NoLock, &m_pObjVers[m_currIdx]);
      }
      else {
        // Maybe the frame is reused and therefore the oid which is stored in the local
        // buffer is different from the oid belonging to the object which was found
        // in the cache => Adapt the oid in the local buffer
        SetCurrOid(found->m_oid);
      }
    }
    else {
      // Read variable sized object  // PTS 1126047
      m_pSession->IncLoadVarObj();
      const OMS_ObjectId8& varOid = GetCurrOid();
      try {
        found = m_pSession->m_context->FindVarObjInContext(varOid, VarObjNoLock, 0, NULL);
      } catch (DbpError &e){
        if (e.m_errorNo == e_object_not_found){
          // Exception is thrown, if object is stored in the local cache and marked as deleted
          
          DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
          if (pCBInterface){
            pCBInterface->dbpCaughtError(e);
          }

          return NULL;
        }
        else {
          throw;
        }
      }
      if (NULL == found) {
        size_t objSize = m_pSession->m_context->GetVarObjFromLiveCacheBase(varOid, false);
        found = m_pSession->m_context->LoadVarObject(varOid, VarObjNoLock, objSize, m_pSession->m_currVarObjChunk, NULL);
        m_pSession->m_currVarObjChunk.Clear();
      }
      else {
        // Maybe the frame is reused and therefore the oid which is stored in the local
        // buffer is different from the oid belonging to the object which was found
        // in the cache => Adapt the oid in the local buffer
        SetCurrOid(found->m_oid);
      }
    }
    m_objectsAreLoaded = false;
  }
  else {
    // ProcServer case
    if (!m_objectsAreLoaded){
      OMS_ClassEntry *pClassInfo = &m_pContainerInfo->GetClassEntry();

      int i = 0;
      while (i<=m_maxIdx){
        int noOfLoadObj = 0;
        // Deref oids in portion of size OMS_MASS_OPERATION_CNT from the kernel
        while (noOfLoadObj < OMS_MASS_OPERATION_CNT && i<=m_maxIdx){
          // Check whether object are already stored in local oms-cache; 
          // if not, remember them in an array
          m_pSession->IncDeref();
          found = m_pSession->m_context->FindObjInContext(&(m_pOid[i]), true /*ignoreGeneration*/);
          if (!found) {
            m_pLoadOid[noOfLoadObj]     = m_pOid[i];
            m_pObjVers[noOfLoadObj]     = m_pObjVers[i];
            m_ppClassInfo[noOfLoadObj]  = pClassInfo;
            m_pContainerId[noOfLoadObj] = m_pContainerInfo->GetFileId();
            ++noOfLoadObj;
          }
          else {
            // Maybe the frame is reused and therefore the oid which is stored in the local
            // buffer is different from the oid belonging to the object which was found
            // in the cache => Adapt the oid in the local buffer
            m_pOid[i] = found->m_oid;
          }

          ++i;
        }

        if (noOfLoadObj > 0) {
          // Read all objects which are not already stored in the local oms-cache
          int error 
            = m_pSession->m_context->LoadObjsFromLiveCacheBase(noOfLoadObj,
                                                            m_pLoadOid,
                                                            m_pObjVers,
                                                            m_pUpdTransId,
                                                            false,  //doLock,
                                                            false,  //shared
                                                            m_ppObjContainer,
                                                            m_pDBError,
                                                            m_ppObj,
                                                            m_ppClassInfo,
                                                            m_pContainerId,
                                                            m_pObjSize);

          if (error) {
            for (int j=0; j<noOfLoadObj; ++j){
              if (m_pDBError[j] != e_ok){
                m_pSession->ThrowDBError (m_pDBError[j], msg, m_pLoadOid[j], __MY_FILE__, __LINE__);
              }
            }
          }
        }
      }
      m_objectsAreLoaded = true;
    }
    // Get pointer to the current object in the oms cache
    m_pSession->IncDeref();
    found = m_pSession->m_context->FindObjInContext(&GetCurrOid(), true /*ignoreGeneration*/);
  }

  if (found == NULL){
    m_pSession->ThrowDBError (e_object_not_found, msg, GetCurrOid(), __MY_FILE__, __LINE__);
    return NULL;
  }
  else if (found->DeletedFlag()) {
    // Check whether the object has already been deleted locally.
    return NULL;
  }
  else {
    return found;
  }
}

/*===================================================================================*/
/*! This method increments the iterator. First it is checked whether the next object 
**  can be read from the local buffer and if so, only the index to the current       
**  object in this buffer is adapted. If not then a next mass-read to the kernel is  
**  started. The oids of this mass-read are stored in the local buffer               
**  OMS_KernelClsIter::m_pOid and the corresponding object frames are stored in the  
**  local oms-cache.                                                                 
*/
/*===================================================================================*/
inline void OMS_KernelClsIter::operator++ ()
{
  const char* msg = "OMS_KernelClsIter::++ ";

  do {
    if (m_currIdx < m_maxIdx && m_maxIdx != -1){
      // Next object can be read from the buffer
      ++m_currIdx;
    }
    else {
      tsp00_Int4 noOfOid = m_maxNoOfOid; 

      if (!m_allObjReadFromKernel){
        m_pSession->IncDerefIter_LC(); // PTS 1107819 TA 13/09/2000 

        // Read next oids from kernel
        tgg00_BasisError DBError;
        tsp00_Int4 LogReadAccesses;
        OMS_HResult hr = m_pSession->m_lcSink->NextOids (
            m_pSession->m_context->m_consistentView,
            m_pContainerInfo->GetFileId(),
            m_pSession->m_context->VersionContext(),
            true,   //  bOnlyHdrVarObjects,
            (SAPDB_UInt8&)GetCurrOid(), 
            noOfOid,
            (SAPDB_UInt8*)m_pOid, 
            m_pObjVers,
            LogReadAccesses,
            &DBError);
        m_pSession->IncLogHopIter(LogReadAccesses);
        if (e_no_next_object == DBError) {
          // All objects from the kernel are read, but there might be some oids left in the buffer. 
          m_allObjReadFromKernel = true;
          if (noOfOid == 0) {
            m_maxIdx = -1;
            m_end    = true;
          }
        }
        else if (DBError != 0) {
            m_pSession->ThrowDBError (DBError, msg, __MY_FILE__, __LINE__);
            return;
        }
        
        if (noOfOid > 0){
          // There is at least one new oid
          m_maxIdx  = noOfOid - 1;
          m_currIdx = 0;
          m_objectsAreLoaded = false;
        }
      }
      else {
        // All objects have been read from the kernel and the buffer is empty
        m_end = true;
      }
    }

    // Load object into the cache and check whether it is marked as deleted
    m_pCurrObj = (m_end ? NULL : LoadObj());

  } while (m_pCurrObj == NULL && !m_end);
}


#endif  // __OMS_KERNELCLSITER_HPP
