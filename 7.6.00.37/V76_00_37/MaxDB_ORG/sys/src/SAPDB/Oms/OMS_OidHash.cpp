/*!
 * \file    OMS_OidHash.cpp
 * \author  MarkusSi, Roterring
 * \brief   Hash to access the objects stored in the liboms cache via their oid
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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


#include "Oms/OMS_OidHash.hpp"
#include "Oms/OMS_ContainerEntry.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_VarObjInfo.hpp"
#include "Oms/OMS_DumpInterface.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_DbpError.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "geo573.h"

class OMS_OidHashDumpInfo {
public:
  void*                 m_this;
  OmsObjectContainerPtr m_hashnext;
  OMS_ObjectId8         m_oid;
  tgg91_PageRef         m_objseq;
  unsigned char         m_state;
  unsigned char         m_filler;
  tsp00_Uint4           m_beforeImages;
  OMS_ContainerHandle   m_containerHandle; // FF 2003-09-23 new FileDir
  tsp00_Int4            m_slot;
};

/*----------------------------------------------------------------------*/
// Implementation OMS_OidHash
/*----------------------------------------------------------------------*/

OMS_OidHash::OMS_OidHash () 
  : m_headentries(0), 
    m_arrayentries(0),
    m_headMask(0),     // PTS 1118855 
    m_count(0),    // PTS 1118855
    m_maxCount(0), // PTS 1118855
    m_arrayMask(0),
    m_headShiftCnt(0),
    m_headcurr(NULL),
    m_headArray(NULL),
    m_context(NULL),
    m_maxLen(0)
{
}
// end constructor

/*----------------------------------------------------------------------*/

OMS_OidHash::~OMS_OidHash () 
{
  if(m_context){
    HashFree();	
    m_context->deallocate(m_headArray);
    m_headArray = NULL;
    m_context = NULL;
  }
}
// end destructor

/*===========================================================================*/
/*! The method removes all objects which belong to the specified containers.   
**  The corresponding objects containers in the context cache are returned to 
**  the free-list.  */
/*===========================================================================*/
void OMS_OidHash::Clean(OMS_ContainerEntry *pDel) 
{
  // removes all objects of dropped containers from OID Hash
  OmsObjectContainerPtr* prev;
  OmsObjectContainerPtr  curr;
  OmsObjectContainerPtr  toFree;
  tsp00_Int4             arrayIndex, headIndex;
  if (m_count > 0) {  
    for (arrayIndex = 0; arrayIndex < m_arrayentries; ++arrayIndex){
      for (headIndex = 0; headIndex < OMS_HASH_ARRAY_CHUNK_BUCKET_CNT; headIndex++) {
        prev = &(m_headArray[arrayIndex][headIndex]);
        curr = *prev;
        while (NULL != curr) {
          OMS_ContainerEntry* pContainerInfo = curr->GetContainerInfoNoCheck();
          if (pContainerInfo == pDel) {
            DecCount();    
            toFree = curr;
            *prev = curr->GetNext();
            curr  = curr->GetNext();

            // PTS 1117571
            if (pContainerInfo->UseCachedKeys() && pContainerInfo->GetClassEntry().IsKeyedObject()) {
              // Delete object out of cached key structure  
              pContainerInfo->VersionDelKey(toFree);
            }
            //// PTS 1132362: The number is already adapted when the container is dropped
            //// (see method OMS_ContainerDirectory::DropPhysicalContainer)
            //if (toFree->IsNewObject() && !toFree->ReplacedFlag()){
            //  OMS_DETAIL_TRACE(omsTrNewObj, m_context->GetLcSink,
            //    "OMS_OidHash::Clean : dec flush: " << toFree->m_oid 
            //    << ", class: " << pContainerInfo->GetClassEntry().GetClassName());
            //  m_context->DecNewObjectsToFlush(toFree);
            //}
            pContainerInfo->ChainFree(toFree, 61);
          }
          else {
            prev = curr->GetNextAddr();
            curr = curr->GetNext();
          }
        }
      }
    }
  }
}


/*===========================================================================*/
/*! The method removes all objects from the hash. The corresponding objects 
**  containers in the context cache are returned to the free-list.
**  If required it adapts the hash size. Rehashing is required if 
**  the current hash size is unequel to the DEFAULT_OMS_HASH_SIZE. Rehashing 
**  will reduce the size to the DEFAULT_OMS_HASH_SIZE.
** \param adaptOidHash If this parameter equals true, it is checked whether 
**  a rehash should be done. */
/*===========================================================================*/
void OMS_OidHash::Clear(bool adaptOidHash) 
{ 
  tsp00_Int4  i,j;
  tsp00_Int4 listCount  = 0;
  tsp00_Int4 entryCount = 0;
  OmsObjectContainerPtr curr;

  if (m_count > 0) {  // PTS 1120585
    for (i = 0; i < m_arrayentries; ++i){
      for (j = 0; j < OMS_HASH_ARRAY_CHUNK_BUCKET_CNT; ++j) {
        curr = m_headArray[i][j];    
        if (NULL != curr) {
          m_headArray[i][j] = NULL;
          ++listCount;
          do {
            ++entryCount;
            if (curr->VarObjFlag()) {

              REINTERPRET_CAST(OMS_VarObjInfo*, &curr->m_pobj)->freeVarObj(m_context);
            }
  #ifdef USE_SYSTEM_ALLOC_CO13
            OmsObjectContainerPtr toFree = curr;
            OMS_ContainerEntry* containerInfo = curr->GetContainerInfoNoCheck(m_context);
            curr  = curr->GetNext();
            containerInfo->ChainFree(toFree, 62);
  #else
            curr  = curr->GetNext();
  #endif
          }while(curr != NULL);
        }   
      }
    }
  }

  // PTS 1117571
  // reset search structure for cached keys
  m_context->GetContainerDir().ClearCachedKeys();

  // In 'ResetCount' the oid buffer might get changed (rehash).
  ResetCount(adaptOidHash);
}


/*===========================================================================*/
/*! The method creates an empty hash with the given size for the context s.
**  For restrictions on the size see method (see OMS_OidHash::HashResize) 
**  \param c Pointer to the context
**  \param sz Intended size (number of buckets) of the hash array */
/*===========================================================================*/
void OMS_OidHash::Create (OMS_Context* c, const tsp00_Int4 sz) 
{
  if(!m_context)
  {
    m_context = c;
    m_headArray = reinterpret_cast<OmsObjectContainerPtr**>( m_context->allocate(OMS_HASH_ARRAY_SIZE*sizeof(OmsObjectContainerPtr**)));
    memset(m_headArray, 0, OMS_HASH_ARRAY_SIZE*sizeof(OmsObjectContainerPtr**)); 
    HashResize(sz, false/*DoRehash*/);
  }
}


/*===========================================================================*/
/*! The method checks that there is no loop in the current hash chain starting 
**  at the given object. 
**  \param curr object container to start check from
**  \return Number of entries found in this chain starting from the given object
**  \exception DbpError (error code -9) A loop is found.*/ 
/*===========================================================================*/
int OMS_OidHash::CheckChain(OmsObjectContainerPtr curr) 
{
  int cnt = 1;
  OmsObjectContainerPtr p = curr->GetNext();
  while (p != NULL) {
    ++cnt;
    if (p == curr) {
      OMS_Globals::Throw(e_OMS_duplicate_key, "Loop in Oms Cache", __MY_FILE__, __LINE__);
    }
    p = p->GetNext();
  }
  return cnt;
}

/*===========================================================================*/

void OMS_OidHash::Dump(OMS_DumpInterface& dumpObj) const
{
  OMS_OidHashDumpInfo oidHashDumpInfo;

  dumpObj.SetDumpLabel(LABEL_OMS_OID_CACHE);
  if (m_count > 0) {  // PTS 1120585
    for (tsp00_Int4 i = 0; i < m_arrayentries; i++) {
      for(tsp00_Int4 j = 0; j < OMS_HASH_ARRAY_CHUNK_BUCKET_CNT; j++) { 
        oidHashDumpInfo.m_slot = j;
        OmsObjectContainerPtr curr = m_headArray[i][j];
        while (NULL != curr)
        {
          SAPDB_MemCopyNoCheck(&oidHashDumpInfo.m_hashnext, curr,
            sizeof(oidHashDumpInfo) -
            sizeof(oidHashDumpInfo.m_this) -
            sizeof(oidHashDumpInfo.m_slot));
          dumpObj.Dump(&oidHashDumpInfo, sizeof(oidHashDumpInfo));
          curr = curr->GetNext();
        }
      }
    }
  }
}


/*===========================================================================*/
/*! The method checks that there are no loop in any hash chains and if the 
**  number of objects in the hash equals the value stored in the member m_count.
**  If an error is found an exception -9 is thrown. */ 
/*===========================================================================*/
void OMS_OidHash::HashCheck() 
{
  int sum = 0;
  for (tsp00_Int4 i = 0; i < m_arrayentries; i++) {
    for(tsp00_Int4 j = 0; j < OMS_HASH_ARRAY_CHUNK_BUCKET_CNT; j++) { 
      if (m_headArray[i][j] != NULL) {
        int cnt = CheckChain (m_headArray[i][j]);
        sum += cnt;
      }
    }
  }
  if (sum != m_count){
    OMS_Globals::Throw(e_OMS_system_error, "Number of entries does not match member m_count", __MY_FILE__, __LINE__);
  }
}

/*===========================================================================*/

inline void OMS_OidHash::HashFree () 
{
  if (NULL != m_context)
  {
    int oldArrayentries = m_arrayentries;
    m_headentries = 0;
    m_arrayentries = 0;
    m_headShiftCnt = 0;
    m_arrayMask   = 0;
    m_headMask    = 0;

    ResetCount(/*adaptOidHash*/false);

    for (tsp00_Int4 i = 0; i < oldArrayentries; i++) {
      m_context->deallocate(m_headArray[i]);
      m_headArray[i] = NULL;
    }
  }
}

/*===========================================================================*/
/*! The method removes the object with the given OID out of the hash. The 
**  object container is not removed from the context cache! After the operation
**  the member OMS_OidHash::m_headcurr is pointing to the bucket from
**  which the object was deleted. 
**  \param key OID of the object to be deleted
**  \param updateKeyCache If parameter equals true, then the key structure,
**         which is used for cached keys, is updated. */
/*===========================================================================*/
bool OMS_OidHash::HashDelete (const OMS_ObjectId8& key, bool updateKeyCache)
{
  OmsObjectContainerPtr* prev = HeadPtr(key);
  OmsObjectContainerPtr  curr = *prev;
  while (curr) {
    if (curr->m_oid == key) {
      if (curr->VarObjFlag()) {
        OMS_VarObjInfo* objInfo;
        objInfo = REINTERPRET_CAST(OMS_VarObjInfo*, (&curr->m_pobj));
        objInfo->freeVarObj(m_context);
      }

      if (updateKeyCache)
      {
        // PTS 1117571
        OMS_ContainerEntry* containerInfo = curr->GetContainerInfoNoCheck();
        if (containerInfo->UseCachedKeys() && containerInfo->GetClassEntry().IsKeyedObject()) {
          // Delete object out of cached key structure  
          containerInfo->VersionDelKey(curr);
        }
      }

      *prev = curr->GetNext();
      DecCount();   // PTS 1118855
      return true;
    }
    else {
      prev = curr->GetNextAddr();
      curr = curr->GetNext();
    }
  }
#ifdef _ASSERT_OMS
  HashCheck();
#endif
  return false;
};


/*===========================================================================*/
/*! The method resizes the hash array with the given size (see 
**  OMS_OidHash::HashInit for restrictions on the size) and if requested
**  the old entries are rehashed into the new hash array. It initializes  
**  the hash array and all other members of the class. 
**  The size of the hash array may vary from the given size in the following
**  cases:
**  - The size is not a power of 2: The used size is the next larger number
**    which is a power of 2.
**  - The size is equal or less than zero: The used size is determined by the
**    constant DEFAULT_OMS_HASH_SIZE.
**  - The size lies outside the interval DEFAULT_OMS_HASH_SIZE and 
**    MAXIMAL_OMS_HASH_SIZE: The corresponding boundary of the interval is used. 
**  \param newHeadEntries Intended size of the new hash array
**  \param rehash If this parameter equals false, then no rehashing is done.
**         The rehashing can only be skipped, if the cache is completely  
**         reset afterwards anyway.  
*/
/*===========================================================================*/
void OMS_OidHash::HashResize(int newHeadEntries, bool rehash)
{
  if( newHeadEntries == m_headentries){
    // Same size, nothing to do
    return;
  }

  // calculate new hash values
  tsp00_Int4 size = OMS_HASH_ARRAY_CHUNK_BUCKET_CNT;
  int newArrayentries = 1;
  int newHeadShiftCnt = 0;
  while (newHeadEntries > size && (newHeadEntries<<1) <= MAXIMAL_OMS_HASH_SIZE){
    size <<= 1;
    newArrayentries *= 2;
    ++newHeadShiftCnt;
  }
  if( size == m_headentries){
    // Same size, nothing to do
    return;
  }
  newHeadEntries = size;

  // first allocate memory if required
  if(newArrayentries > m_arrayentries){
    for (int arrayIndex=m_arrayentries; arrayIndex < newArrayentries; ++arrayIndex)
    {
      m_headArray[arrayIndex] = REINTERPRET_CAST(OmsObjectContainerPtr*, m_context->allocate(sizeof(OmsObjectContainerPtr) * OMS_HASH_ARRAY_CHUNK_BUCKET_CNT ));
      memset(m_headArray[arrayIndex], 0, sizeof(OmsObjectContainerPtr) * OMS_HASH_ARRAY_CHUNK_BUCKET_CNT);  
    }
  }
  // memory allocation succesful, start to store entries in tmp list

  OmsObjectContainerPtr tmpListEntry = NULL; // temporary list for rehash
  float avgChainLen = 0;  // Only for analysing purpose

  // store entries in tmp list if rehashing is requested and there are entries in the old hash 
  if (rehash && m_count > 0){
    int nonEmptySlots   = 0;
    m_context->GetSession()->IncRehash();
    for (tsp00_Int4 arrayIndex = 0; arrayIndex < m_arrayentries; ++arrayIndex) {
      for (tsp00_Int4 headIndex = 0; headIndex < OMS_HASH_ARRAY_CHUNK_BUCKET_CNT; ++headIndex) {
        OmsObjectContainerPtr curr = m_headArray[arrayIndex][headIndex];
        if (curr != NULL){
          m_headArray[arrayIndex][headIndex] = NULL;
          ++nonEmptySlots;
          while (NULL != curr){
            OmsObjectContainerPtr pInsert = curr;
            curr = curr->GetNext();
            pInsert->m_hashnext = tmpListEntry;
            tmpListEntry = pInsert;
          }
        }
      }
    }
    avgChainLen = (float) m_count / (float) (nonEmptySlots + 1);
  }
  else if(!rehash && m_count > 0){ // rehash not required but hash not empty, clear hash to ensure consistent state
    for (tsp00_Int4 arrayIndex = 0; arrayIndex < m_arrayentries; ++arrayIndex){
      memset(m_headArray[arrayIndex], 0, sizeof(OmsObjectContainerPtr) * OMS_HASH_ARRAY_CHUNK_BUCKET_CNT);  
    }
  }

  // initialize hash with new size, set hash members to new values
  int oldArrayentries = m_arrayentries;
  m_headentries = newHeadEntries;
  m_arrayentries = newArrayentries;
  m_headShiftCnt = newHeadShiftCnt;
  m_arrayMask   = newArrayentries-1;
  m_headMask    = (OMS_HASH_ARRAY_CHUNK_BUCKET_CNT-1)<<newHeadShiftCnt;

  m_headcurr = NULL;
  m_count    = 0;
  m_maxCount = 0;
  m_maxLen   = 0;

  // PTS 1117571
  // reset search structure for cached keys 
  m_context->GetContainerDir().ClearCachedKeys();
  
  // rehash if rehashing is requested and there are entries in the tmp list 
  if (rehash && tmpListEntry != NULL){
    OmsObjectContainerPtr curr = tmpListEntry;
    while (NULL != curr){
      OmsObjectContainerPtr pInsert = curr;
      curr = curr->GetNext();
      HashInsert(pInsert);
    }
  }

  // rehash successful, deallocate memory if required
  if(oldArrayentries > m_arrayentries){
    for (int arrayIndex=(oldArrayentries-1); arrayIndex >= m_arrayentries; --arrayIndex)
    {
       m_context->deallocate(m_headArray[arrayIndex]);
       m_headArray[arrayIndex] = NULL;
    }
  }
}


/*===========================================================================*/
/*! The method removes all objects which are not locked. The corresponding  
**  object containers which are stored in the context cache are returned to 
**  the free-list. */
/*===========================================================================*/
void OMS_OidHash::RemoveUnlockedObjFromCache()
{
  OmsObjectContainerPtr* prev           = NULL;
  OmsObjectContainerPtr  curr           = NULL;
  OmsObjectContainerPtr  toFree         = NULL;
  OMS_ContainerEntry*    pContainerInfo = NULL;

  if (m_count > 0) {  // PTS 1120585
    for (tsp00_Int4 arrayIndex=0; arrayIndex < m_arrayentries; ++arrayIndex)
      for (tsp00_Int4 headIndex = 0; headIndex < OMS_HASH_ARRAY_CHUNK_BUCKET_CNT; ++headIndex) {
        prev       = &(m_headArray[arrayIndex][headIndex]);
        curr       = m_headArray[arrayIndex][headIndex];
        m_headArray[arrayIndex][headIndex] = NULL;
        while (NULL != curr) {
          toFree = curr;
          curr   = curr->GetNext();
          // Keep objects which are locked (either exclusive or shared) in the cache.
          if (toFree->LockedFlag() || toFree->LockedSharedFlag()) 
          {
            *prev              = toFree;
            toFree->SetNext(NULL);
            prev               = toFree->GetNextAddr();
          }
          else {
            pContainerInfo = toFree->GetContainerInfo(__MY_FILE__, __LINE__);

            // PTS 1117571
            if (pContainerInfo->UseCachedKeys() && pContainerInfo->GetClassEntry().IsKeyedObject()) {
              // Delete object out of cached key structure
              pContainerInfo->VersionDelKey(toFree);
            }
            DecCount();   // PTS 1118855
            pContainerInfo->ChainFree(toFree, 63);
          }
        }
    }
  }
}


/*===========================================================================*/
/*! The method resets the hash array. The corresponding object containers are
**  not freed! If required it adapts the hash size. Rehashing is required if 
**  the current hash size is unequel to the DEFAULT_OMS_HASH_SIZE. Rehashing 
**  will reduce the size to the DEFAULT_OMS_HASH_SIZE.
** \param adaptOidHash If this parameter equals true, it is checked whether 
**        a rehash should be done. */
/*===========================================================================*/
void OMS_OidHash::SetEmpty(bool adaptOidHash)
{
  tsp00_Int4 arrayIndex;
  if (m_count > 0) {  // PTS 1120585
    for (arrayIndex = 0; arrayIndex < m_arrayentries; ++arrayIndex){
      memset(m_headArray[arrayIndex], 0, sizeof(OmsObjectContainerPtr) * OMS_HASH_ARRAY_CHUNK_BUCKET_CNT);  
    }
  }
  ResetCount(adaptOidHash);

  // PTS 1117571
  // reset search structure for cached keys 
  m_context->GetContainerDir().ClearCachedKeys();
}

/*===========================================================================*/
  
void OMS_OidHash::ChangeMaxHashChainLen(int len)
{ 
  m_context->GetSession()->MaxHashChainLen(len); 
}


/*===========================================================================*/
/*===========================================================================*/
  
OMS_ClusteredOidHashEntry* OMS_ClusteredOidHash::GetNewEntry()
{
  return reinterpret_cast<OMS_ClusteredOidHashEntry*>(m_pContext->allocate(sizeof(OMS_ClusteredOidHashEntry)));
}

/*===========================================================================*/

void OMS_ClusteredOidHash::Clear()
{
  for (int slot = 0; slot < MAX_HASH_SIZE; ++slot) {
    OMS_ClusteredOidHashEntry *pEntry = m_ppHead[slot];
    m_ppHead[slot] = NULL;
    while (pEntry){
      OMS_ClusteredOidHashEntry *pNext = pEntry->m_pNext;
      m_pContext->deallocate(pEntry);
      pEntry = pNext;
    }
  }
}



