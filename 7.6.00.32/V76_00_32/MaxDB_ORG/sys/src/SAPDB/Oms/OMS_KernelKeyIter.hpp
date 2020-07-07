/*!
 * \file    OMS_KernelKeyIter.hpp
 * \author  MarkusSi, Roterring
 * \brief   iterator which reads objects from the kernel
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

#ifndef __OMS_KERNELKEYITER_HPP
#define __OMS_KERNELKEYITER_HPP

#include "ggg01.h"
#include "Oms/OMS_KeyIterBase.hpp"
#include "Oms/OMS_Defines.h"


/*----------------------------------------------------------------------*/
/*! Class defines a key iterator over all keyed objects, that have been 
**  created in the kernel.                                              
**  \since PTS 1119480                                                  
*/
class OMS_KernelKeyIter : public OMS_BasisKeyIterBase 
{
private :
  /*! When loading object oids from the kernel this member is set to false. 
  **  Then it is checked whether the objects, corresponding to these oids   
  **  are already stored in the local oms cache and all objects which are    
  **  not in the oms cache are loaded using a mass deref-operation (\see     
  **  OMS_KernelKeyIter::LoadObj). After this operation all objects should   
  **  reside in the cache and the member variable is set to true.            
  */
  bool                m_objectsAreLoaded;

  /*! If true, then the kernel iterator is already empty, but there are      
  **  might some oids left in the local buffer.                              
  */
  bool                 m_allObjReadFromKernel;

  /*! If this variable is set to true, then in every step of the iteration only
  **  a single object is requested from the kernel, and not a whole array. But
  **  in contrast to the mass-operation not only the oid is returned but the 
  **  object itself is directly delivered. This is advantageous if the communication
  **  cost between the liboms and the kernel is low.
  */
  bool                 m_noMassOperation;

  /*! This member contains the index of the current object in the array      
  **  OMS_KernelKeyIter::m_pOid.                                             
  */ 
  int                  m_currIdx;

  /*! This member contains the maximal index of the array m_pOid. If this    
  **  member equals -1, then the buffer is completely empty.                 
  */
  int                  m_maxIdx;

  /*! Maximal size of the array OMS_KernelKeyIter::m_pOid. So this member    
  **  determines the maximal number of oids which can be read from the      
  **  kernel.                                                                
  */
  int m_maxNoOfOid;

  /*! Pointer to the kernel iterator. */
  void               *m_pKBIterator;

  /*! If the direction of the iterator is changed and the kernel iterator is 
  **  not active, then this member is set to the current key of the merge-   
  **  iterator (\see OMS_KernelKeyIter::ChangeDirection) and with this       
  **  information the kernel iterator is restarted.                          
  */
  void               *m_pRestartKey;

  /*! Array to store the oids which are read in a mass-operation from the   
  **  kernel. The size of the array is determined by                         
  **  OMS_KernelKeyIter::m_maxNoOfOid, but the maximal size is restricted 
  **  by the constant OMS_ITER_MAX_OID_BUFFER. The valid entries are stored in the   
  **  buckets 0 to OMS_KernelKeyIter::m_maxIdx. The entry the iterator is    
  **  currently pointing to is given by OMS_KernelClsIter::m_currIdx.        
  **  The entries are always (independent on the direction) stored in        
  **  ascending order, which means a ++ always leads the next larger object. 
  **
  **  To prevent the running of the constructor for every entry in the array,
  **  the memory is allocated using a character type and later on the correctly
  **  typed pointer is assigned to this array.
  */
  OmsTypeUInt8        m_pOid_u[OMS_MASS_OPERATION_CNT];
  OMS_ObjectId8      *m_pOid;

  /*! In this array the Log-Oid of the oids are stored. These oids can be used
  **  to speed up the mass deref as with these oids the object can be accessed
  **  directly in the log instead of starting with the current object version 
  **  and then searching the history chain for the version corresponding to the
  **  current consistent view.
  */
  tgg91_PageRef        m_pObjVers[OMS_MASS_OPERATION_CNT];

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

public :
  /// Constructor
  OMS_KernelKeyIter(
    const void*       pStartKey,
    const void*       pLowerKey,
    const void*       pUpperKey,
    OmsIterDirection  direction,
    OMS_Session*      pSession,
    OMS_ContainerEntry* pContainerInfo,
    int               maxBufferSize
    );
  /// Destructor
  ~OMS_KernelKeyIter();
  /// changes the running direction of the iterator 
  inline bool ChangeDirection (OmsIterDirection direction, void* pRestartKey);  
  /// cleans the memory
  void DeleteSelf();
  /// increments the iterator
  inline void operator++();
  /// decrements the iterator
  inline void operator--();        

private:
  /// delivers the OID of the object the iterator is currently pointing to
  OMS_ObjectId8&        GetCurrOid() { return m_pOid[m_currIdx]; }
  /// loads the object-container from the kernel into the oms-cache
  inline OmsObjectContainer* LoadObj(OmsObjectContainer *pObj);
};


/*===================================================================================*/
/*! This method is called when the running direction of the iterator is changed.     
**  If the iterator is already marked as empty, then the iterator is marked as       
**  active again and if the oid buffer is empty then the restart key is used to      
**  position the iterator. If the iterator was still active, then dependent on the   
**  new running direction either an increment resp. a decrement is executed.           
*/
/*===================================================================================*/
inline bool OMS_KernelKeyIter::ChangeDirection (OmsIterDirection direction, void* pRestartKey)  
{ 
  m_allObjReadFromKernel = false; 

  if (m_end && m_maxIdx != -1){
    // As the buffer is not empty, the m_currIdx member is already pointing to the
    // object which was read last before changing the direction. Therefore nothing
    // had to be done
    m_end = false;
  }
  else {
    if (m_end && m_maxIdx == -1){
      // Buffer is empty and therefore the iterator must be positioned using the
      // given restart key.
      m_pRestartKey = pRestartKey; 
    }

    if (direction == OMS_ASCENDING){
      ++(*this);
    }
    else {
      --(*this);
    }
  }

  return bool(*this);
}


/*===================================================================================*/
/*! This method increments the iterator. First it is checked whether the next object 
**  can be read from the local buffer and if so, only the index to the current       
**  object in this buffer is adapted. If not then a next mass-read to the kernel is  
**  started. The oids of this mass-read are stored in the local buffer               
**  OMS_KernelKeyIter::m_pOid and the corresponding object frames are stored in the  
**  local oms-cache.                                                                 
*/
/*===================================================================================*/
inline void OMS_KernelKeyIter::operator++()
{
  const char* msg = "OMS_KernelKeyIter::++ ";

  do {
    OmsObjectContainer* pObj=NULL;

    if (m_currIdx < m_maxIdx && m_maxIdx != -1){
      // next object can be read from local buffer
      ++m_currIdx;
    }
    else {
      int noOfOid = m_maxNoOfOid; 
      int objHistReadCount; 

      if (!m_allObjReadFromKernel){
        if (m_noMassOperation){
          // Get memory to store the object. 
          pObj = m_pContainerInfo->GetMemory();
        }

        // Get next objects from livecache 
        tgg00_BasisError DBError;
        m_pSession->m_lcSink->NextObjFromKeyIterator (
            m_pSession->m_context->m_consistentView,  // PTS 1127520
            m_pContainerInfo->GetFileId(),
            m_pSession->m_context->VersionContext(),
            m_pKBIterator,
            m_pContainerInfo->GetClassEntry().GetPersistentSize(),
            m_keyLen,
            m_pRestartKey != NULL ? m_pRestartKey : GetCurrKey(),       
            noOfOid,            // changed after call
            REINTERPRET_CAST(SAPDB_UInt8*, m_pOid),
            m_pObjVers,
            m_noMassOperation ? ((unsigned char*) &pObj->m_pobj) + sizeof(void*) : NULL,
            objHistReadCount,
            DBError); 
        m_pRestartKey = NULL;
        m_pSession->IncLogHop(objHistReadCount);
        if (DBError == e_no_next_object){
          // All objects from the kernel are read, but there might be some oids left in the buffer.
          m_allObjReadFromKernel = true;
          if (noOfOid == 0){
            // buffer is empty
            OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << ": end reached");
            m_maxIdx = -1;
            m_end = true;

            if (m_noMassOperation){
              m_pContainerInfo->ChainFree(pObj, 51);
            }
         }
        }
        else if (DBError != e_ok){
          m_pSession->ThrowDBError (DBError, msg, __MY_FILE__, __LINE__);
          return;
        }

        if (noOfOid > 0){
          // There is at least on new oid
          m_maxIdx           = noOfOid - 1;
          m_currIdx          = 0;
          m_objectsAreLoaded = false;
          m_end              = false;
        }
      }
      else {
        // All objects have been read from the kernel and the buffer is completely processed
        OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << ": end reached");
        m_end = true;
      }
    }

    if (!m_end) { 
      m_pCurrObj = LoadObj(pObj);
      if (m_pCurrObj != NULL){
        // Object is not marked as deleted
        OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg 
                 << OMS_UnsignedCharBuffer((const unsigned char*)GetCurrKey(), m_keyLen));
      }
    }
  } while (m_pCurrObj == NULL && m_end == false);
}

/*===================================================================================*/
/*! This method decrements the iterator. First it is checked whether the prev object 
**  can be read from the local buffer and if so, only the index to the current       
**  object in this buffer is adapted. If not then a next mass-read to the kernel is  
**  started. The oids of this mass-read are stored in the local buffer               
**  OMS_KernelKeyIter::m_pOid and the corresponding object frames are stored in the  
**  local oms-cache.                                                                 
*/
/*===================================================================================*/
inline void OMS_KernelKeyIter::operator--()
{
  const char* msg = "OMS_KernelKeyIter::-- ";
 
  do {
    OmsObjectContainer* pObj=NULL;

    if (m_currIdx > 0 && m_maxIdx != -1){
      // Next object can be read from local buffer
      --m_currIdx;
    }
    else {
      int noOfOid = m_maxNoOfOid; 
      int objHistReadCount;

      if (!m_allObjReadFromKernel){
        if (m_noMassOperation){
          // Get memory to store the object. 
          pObj = m_pContainerInfo->GetMemory();
        }

        // Get next objects from livecache 
        tgg00_BasisError DBError;
        m_pSession->m_lcSink->PrevObjFromKeyIterator (
            m_pSession->m_context->m_consistentView,  // PTS 1127520
            m_pContainerInfo->GetFileId(),
            m_pSession->m_context->VersionContext(),
            m_pKBIterator,
            m_pContainerInfo->GetClassEntry().GetPersistentSize(),
            m_keyLen,
            m_pRestartKey != NULL ? m_pRestartKey : GetCurrKey(),       
            noOfOid,    // changed after call
            REINTERPRET_CAST(SAPDB_UInt8*, m_pOid),
            m_pObjVers,
            m_noMassOperation ? ((unsigned char*) &pObj->m_pobj) + sizeof(void*) : NULL,
            objHistReadCount,
            DBError); 
        m_pRestartKey = NULL;
        m_pSession->IncLogHop(objHistReadCount);
        if (DBError == e_no_next_object){
          // All objects from the kernel are read, but there might be some oids left in the buffer.
          m_allObjReadFromKernel = true;
          if (noOfOid == 0){
            // buffer is empty
            OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << ": end reached");
            m_maxIdx = -1;
            m_end = true;

            if (m_noMassOperation){
              m_pContainerInfo->ChainFree(pObj, 52);
            }
          }
        }
        else if (DBError != e_ok){
          m_pSession->ThrowDBError (DBError, msg, __MY_FILE__, __LINE__);
          return;
        }

        if (noOfOid > 0){
          // There is at least on new oid
          m_maxIdx           = noOfOid - 1;
          m_currIdx          = m_maxIdx;
          m_objectsAreLoaded = false;
          m_end              = false;
        }
      }
      else {
        // All objects have been read from the kernel and the buffer is completely processed
        OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg << ": end reached");
        m_end = true;
      }
    }

    if (!m_end) {
      m_pCurrObj = LoadObj(pObj);
      if (m_pCurrObj != NULL){
        // Object is not marked as deleted
        OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg 
                 << OMS_UnsignedCharBuffer((const unsigned char*)GetCurrKey(), m_keyLen));
      }
    }
  } while (m_pCurrObj == NULL && m_end == false);
}

/*===================================================================================*/
/*! This method loads the object frames corresponding to the buffered oids into the  
**  local oms-cache if not already done and it returns the pointer to the current    
**  object frame in the oms-cache.                                                   
**  If the objects are not already loaded then first all oids of those objects which 
**  are not already stored in the oms cache are collected and with all these a       
**  mass-read to the kernel is executed.                                             
**  \return pointer to the object frame in the oms cache of the current object.      
*/
/*===================================================================================*/
inline OmsObjectContainer* OMS_KernelKeyIter::LoadObj(OmsObjectContainer* pObj) 
{ 
  const char* msg = "OMS_KernelKeyIter::LoadObj ";
  OMS_TRACE(omsTrKeyRange, m_pSession->m_lcSink, msg);

  OmsObjectContainerPtr found = NULL;

  if (m_noMassOperation){
    // Object body was already delivered by the create or the ++,-- operator.
    // Check if object does already exist in local cache.
    found = m_pSession->m_context->FindObjInContext(&(GetCurrOid()), /*ignoreGeneration=*/true, /*checkDropped=*/true, /*checkLocallyCreated=*/false);
    if (found == NULL) {
      // Object does not exist locally. 
      // Fill header of object container
      pObj->m_oid    = m_pOid[0];
      pObj->m_objseq = m_pObjVers[0];

      // Insert object into oms-cache
      m_pSession->m_context->PutObjectIntoContext (pObj, m_pContainerInfo->GetContainerHandle());

      return pObj;
    }
    else {
      // Object does already exist, therefore free memory
      m_pContainerInfo->ChainFree(pObj, 53);
     
      if (m_pSession->IsReadUnchangedObjectsActive() && (found->IsNewObject() || m_pSession->m_context->IsVersionOid(found->m_oid))){
        // As object is locally created or was created in an unloaded version, this object is not
        // visible in the 'read-unchanged'-mode
        m_pCurrObjDel = found;
        return NULL;
      }
    }
    m_objectsAreLoaded = false;
  }

  else {
    // Mass derefs from the kernel
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
          found = m_pSession->m_context->FindObjInContext(&(m_pOid[i]), /*ignoreGeneration=*/true, /*checkDropped=*/true, /*checkLocallyCreated=*/false);
          
          if (m_pSession->IsReadUnchangedObjectsActive()){
            if (found && found->IsNewObject()){
              // Objects is not yet committed in the kernel and therefore loading the object from the
              // kernel would result in an error e_wrong_object_state.
              // Remove corresponding oid from the local buffer.
              for (int j=i; j<m_maxIdx-1; ++j){
                m_pOid[j]     = m_pOid[j+1];
                m_pObjVers[j] = m_pObjVers[j+1];
              }
              --m_maxIdx;
            }
            else if (found && found->Changed()){
              // Object which is stored in the cache is changed. Therefore the object must be
              // read again from the kernel.
              found = NULL;
            }
          }
          
          if (!found) {
            m_pLoadOid[noOfLoadObj]     = m_pOid[i];
            m_pObjVers[noOfLoadObj]     = m_pObjVers[i];
            m_ppClassInfo[noOfLoadObj]  = pClassInfo;
            m_pContainerId[noOfLoadObj] = m_pContainerInfo->GetFileId();
            ++noOfLoadObj;
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
                                                            false,  //shared,
                                                            m_ppObjContainer,
                                                            m_pDBError,
                                                            m_ppObj,
                                                            m_ppClassInfo,
                                                            m_pContainerId,
                                                            m_pObjSize);

          if (error) {
            // Search for first error and raise it
            for (int i=0; i<noOfLoadObj; ++i){
              if (m_pDBError[i] != e_ok){
                m_pSession->ThrowDBError (m_pDBError[i], msg, m_pLoadOid[i], __MY_FILE__, __LINE__);
              }
            }
          }
        }
      }
      m_objectsAreLoaded = true;
    }

    if (m_maxIdx >= 0){
      // Get pointer to the current object in the oms cache
      found = m_pSession->m_context->FindObjInContext(&(GetCurrOid()), /*ignoreGeneration*/true);
    }
    else {
      // This might happen in case of read-unchanged-mode if the objects are created and
      // not committed yet.
      return NULL;
    }
  }

  if (found == NULL) {
    m_pSession->ThrowDBError (e_object_not_found, msg, GetCurrOid(), __MY_FILE__, __LINE__);
    return NULL;
  }
  else if (found->DeletedFlag()){
    // Check whether the object has already been deleted locally.
    // Remember the pointer, as it might be needed to determine the key for
    // the next load from the kernel (GetCurrKey)
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
