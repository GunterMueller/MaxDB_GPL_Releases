/*!
 * \file    OMS_OidHash.hpp
 * \author  MarkusSi, Roterring
 * \brief   Datastructure to handle accesses to the persistent objects in the local cache.
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
#ifndef __OMS_OIDHASH_HPP
#define __OMS_OIDHASH_HPP

#include "Oms/OMS_ObjectContainerDef.hpp"
#include "ggg00.h"

/// Default resp. maximal possible size of the (adaptable) hash array 
/// in the class OMS_OidHash
#define DEFAULT_OMS_HASH_SIZE (16*OMS_HASH_ARRAY_CHUNK_BUCKET_CNT)
#define MAXIMAL_OMS_HASH_SIZE (OMS_HASH_ARRAY_SIZE*OMS_HASH_ARRAY_CHUNK_BUCKET_CNT)

class OMS_DumpInterface;

/// Implements access structure for objects in the local context cache
class OMS_OidHash {
  friend class OmsHandle;
  friend class OmsObjByClsIterBase;
  /*! Current number of buckets in the hash-array. This number can be changed dynamically 
  ** (see OMS_OidHash::HashResize). */
  tsp00_Int4             m_arrayentries;
  tsp00_Int4             m_headentries;
  /*! Bit-mask to filter the relevant bit positions of the hash value depending on the 
  ** size of the hash array 
  ** \since PTS 1118855 */
  tsp00_Int4             m_arrayMask;
  tsp00_Int4             m_headShiftCnt;
  tsp00_Int4             m_headMask;      
  /*! Number of entries in the cache 
  ** \since PTS 1118855 */
  tsp00_Int4             m_count;
  /*! Maximal number of entries in the cache since the last reset of the cache 
  ** \since PTS 1118855 */
  tsp00_Int4             m_maxCount;
  /*! Maximal length of a hash chain for this instance so far */
  tsp00_Int4             m_maxLen;
  /*! Backward-pointer to the context, in which the instance was created */
  OMS_Context*       m_context;
  /*! Hash array with pointers to the object container. The components of the OID are
  ** used to compute the hash value. */
  OmsObjectContainerPtr** m_headArray;
  //OmsObjectContainerPtr* m_head;
  /*! Pointer to the bucket in the hash array which corresponds to the hash value of the 
  ** current object. This information can be used to prevent several computations of the 
  ** hash value for the same OID. */
  OmsObjectContainerPtr* m_headcurr;

public :
  OMS_OidHash ();
  ~OMS_OidHash ();
  /// Removes all objects out of the hash and out of the context cache
  void                         Clear(bool adaptOidHash = true);
  tgg00_BasisError             ClearForOpenVersion();
  ///// Removes all objects of dropped containers out of the hash and out of the context cache.
  //void                         Clean();
  /// Removes all objects belonging to the given container
  void                         Clean(OMS_ContainerEntry *pDel); 
  /// Creates an empty hash with the given size for the context s.
  void                         Create (OMS_Context* s, const tsp00_Int4 sz = DEFAULT_OMS_HASH_SIZE);
  void                         Dump(OMS_DumpInterface& dumpObj) const;
  /// Returns the current number of hash buckets.  
  inline tsp00_Int4            GetHashSize() const;
  /// Removes the object with the given OID from the hash.
  bool                         HashDelete     (const OMS_ObjectId8& key, bool updateKeyCache);
  /// Searches the hash for the object identified by the given OID.
  inline OmsObjectContainerPtr HashFind       (const OMS_ObjectId8* k, bool ignoreGeneration=false);
  /// Frees the memory allocated by the hash array.
  inline void                  HashFree       ();
  /// Inserts an object into the hash. 
  inline void                  HashInsert     (OmsObjectContainer* h);
  /// Inserts an object into the bucket to which the member OMS_OidHash::m_headcurr is currently pointing.
  inline void                  HashSlotInsert (OmsObjectContainer* h);
  /// Removes all objects which are not locked and which do not have before-images out of the hash and out of the cache.
  void                         RemoveUnlockedObjFromCache();
  /// Reactive objects which were replaced by the now rollbacked object version.
  inline OmsObjectContainerPtr ReactivateReplacedVersion(OmsObjectContainer* p);
  /// Resets the hash array.
  void                         SetEmpty       (bool adaptOidHash = true);

  /// /name Methods for the adaptation of the hash.
  //@{
  /// Increases number of entries in the hash and if necessary adapts the hash size.
  inline void                  IncCount       ();                        
  /// Decreases number of entries in the hash.
  inline void                  DecCount       ();                          
  /// Resets the number of entries to zero and adapts the hash array if required.
  inline void                  ResetCount     (bool adaptOidHash);  
  /// Changes the size of the hash array and if requested rehashes the current entries
  void                         HashResize     (int newHeadEntries, bool rehash = true); // PTS 1118855
  //@}

  /// /name Check Routines.
  //@{
  /// Checks for loops in the current hash chain.
  int                          CheckChain     (OmsObjectContainerPtr curr);
  /// Checks for loops in any hash chain.
  void                         HashCheck      ();
  //@}

  class OidIter {
    friend class OMS_OidHash;
    OMS_OidHash*           m_hash;
    int                    m_headIndex;
    int                    m_arrayIndex;
    OmsObjectContainerPtr  m_curr;
  public :
    inline OidIter(OMS_OidHash*);
    inline OidIter(const OidIter&);
    inline ~OidIter();
    inline void operator++();
    inline void operator++(int);
    inline operator bool() const;
    inline OmsObjectContainerPtr operator()() const;
  };
  friend class OMS_OidHash::OidIter;
  inline OidIter               First ();

private :
  /*! Computation of the hash bucket for a given OID. The current hash bucket is stored
  ** in the variable m_headcurr, so that later accesses to the same OID can be executed
  ** without the recomputation of the hash value. 
  ** \attention The hash-function has to ensure, that objects, which are not created  
  ** in a version, and which have the same pno and pagePos, have the same hash value 
  ** independend of the value of the generation.
  */
  OmsObjectContainerPtr* HeadPtr (const OMS_ObjectId8& key) {
    unsigned long keyValue = key.omsHashValue(); 
    tsp00_Uint4 arrayIndex = keyValue & m_arrayMask;
    tsp00_Uint4 headIndex = (keyValue & m_headMask)>>m_headShiftCnt;
    m_headcurr = &(m_headArray[arrayIndex][headIndex]);  
    return m_headcurr;
  }

  // ChangeMaxHashChainLen is implemented as a separate method to ensure that
  // MaxHashChainLen is inline. (Without this split OMS_Session and OMS_Context would
  // have to be included into this file to allow inlining)
  void ChangeMaxHashChainLen(int len);
  inline void MaxHashChainLen(int len){
    if (len > m_maxLen){
      m_maxLen = len;
      ChangeMaxHashChainLen(len);
    }
  }
};


class OMS_ClusteredOidHashEntry;
class OMS_ClusteredOidHash;


class OMS_ClusteredOidHashEntry
{
public:
  OMS_ClusteredOidHashEntry(int page)
  : m_page(page)
  , m_pNext(NULL)
  , m_pObj(NULL)
  {}

  int                        m_page;
  OmsObjectContainer        *m_pObj;
  OMS_ClusteredOidHashEntry *m_pNext;
};


class OMS_ClusteredOidHash
{
public:
  class Iter
  {
  private:
    OMS_ClusteredOidHash      *m_pHash;
    int                        m_slot;
    OMS_ClusteredOidHashEntry *m_pEntry;
    OmsObjectContainer        *m_pObj;

  public :
    
    inline Iter(OMS_ClusteredOidHash *pHash);
    inline Iter(const Iter&);
    inline bool Next();
    inline operator bool() const;
    inline OmsObjectContainer* operator()() const;
  };

  inline OMS_ClusteredOidHash(OMS_Context *pContext);
  inline ~OMS_ClusteredOidHash() {Clear();}
  void         Clear();
  inline Iter  First ();
  inline void  Insert(OmsObjectContainer *pObj);


private:
  enum { MAX_HASH_SIZE = 1000 };

  inline int                 Hash(const OMS_ObjectId8 &oid);
  OMS_ClusteredOidHashEntry* GetNewEntry();

  OMS_ClusteredOidHashEntry *m_ppHead[MAX_HASH_SIZE];
  OMS_Context               *m_pContext;

  friend class Iter;
};

/*----------------------------------------------------------------------*/
/* Implementation of inline methods of class OMS_OidHash              */
/*----------------------------------------------------------------------*/

inline tsp00_Int4 OMS_OidHash::GetHashSize() const
{
  return m_headentries;
}

/*===========================================================================*/
/*! The method inserts an object container into the hash. The counter of entries   
**  in the hash is increased and thereby it is checked whether a rehash should be
**  started. After the execution of the method, the member 
**  OMS_OidHash::m_headcurr is pointing to the bucket into which the insert 
**  was done. 
**  \param h Object container of the object to be inserted */
/*===========================================================================*/
inline void OMS_OidHash::HashInsert(OmsObjectContainer* h) 
{
  h->m_hashnext = *HeadPtr(h->m_oid);
	*m_headcurr     = h;
  IncCount();        // PTS  1118855
#ifdef _ASSERT_OMS
  HashCheck();
#endif
}

/*===========================================================================*/
/*! The method inserts an object container into the hash. Instead of computing
**  the bucket using the hash-function, the bucket to which the member 
**  OMS_OidHash::m_headcurr is pointing, is used.
**  The counter of entries in the hash is increased and thereby it is checked 
**  whether a rehash should be started.
**  \attention There is NO check, that this bucket is the right one for 
**  the given object! 
**  \param h Object container of the object to be inserted */
/*===========================================================================*/
inline void OMS_OidHash::HashSlotInsert (OmsObjectContainer* h) 
{
	h->m_hashnext = *m_headcurr;
	*m_headcurr     = h;
  IncCount();         // PTS  1118855
#ifdef _ASSERT_OMS
  HashCheck();
#endif
}

/*===========================================================================*/
/*! This method increases the counter of the current entries in the hash and 
**  if necessary it adapts the hash size. Rehashing is considered necessary, 
**  if the number of entries is larger than twice the size of the hash array 
**  (The hash chains have a mean length of 2). 
**  Rehashing will double the size of the hash array. 
** \since PTS 1118855 */
/*===========================================================================*/
inline void OMS_OidHash::IncCount()
{  
  ++m_count;

  if (m_count > m_maxCount){
    m_maxCount = m_count;
  }

  if (m_count > (m_headentries << 1) && (m_headentries << 1) <= MAXIMAL_OMS_HASH_SIZE){
    HashResize(m_headentries << 1);
  }
}


/*===========================================================================*/
/*! This method decreases the counter of the current entries in the hash. This
**  information is needed for the dynamical adaptation of the hash size.
** \since PTS 1118855 */
/*===========================================================================*/
inline void OMS_OidHash::DecCount()
{  
  --m_count;
}


/*! This method resets the number of entries to zero and if required it adapts 
**  the hash size. Rehashing is required if the current hash size is unequel 
**  to the DEFAULT_OMS_HASH_SIZE. Rehashing will reduce the size to the 
**  DEFAULT_OMS_HASH_SIZE. 
**  \param adaptOidHash If this parameter equals false, then it is not checked
**         whether an adaptation is necessary.
*/
/*===========================================================================*/
inline void OMS_OidHash::ResetCount(bool adaptOidHash)
{ 
  m_headcurr = NULL;
  m_count    = 0;
  m_maxCount = 0;
  m_maxLen   = 0;
  // Check if rehash is required.
  if ( adaptOidHash && m_headentries != DEFAULT_OMS_HASH_SIZE){
    HashResize(DEFAULT_OMS_HASH_SIZE, false/*DoRehash*/);
  }
}

/*----------------------------------------------------------------------*/

inline OMS_OidHash::OidIter OMS_OidHash::First ()
{
  OidIter iter(this);
  for( iter.m_arrayIndex=0; iter.m_arrayIndex < m_arrayentries ; ++iter.m_arrayIndex){
    /* PTS 1122885 FF 2003-07-03 if there is no element within the OidHash don't iterate */
    for (iter.m_headIndex = 0; (0 < m_count) && (iter.m_headIndex < OMS_HASH_ARRAY_CHUNK_BUCKET_CNT); ++iter.m_headIndex) {
      iter.m_curr = this->m_headArray[iter.m_arrayIndex][iter.m_headIndex];
      if (NULL != iter.m_curr) 
      {
        break;
      }
    }
    if (NULL != iter.m_curr) 
    {
      break;
    }
  } 
  return iter;
}

/*----------------------------------------------------------------------*/

inline OMS_OidHash::OidIter::OidIter(OMS_OidHash* h) : m_hash(h), m_curr(NULL), m_headIndex(-1), m_arrayIndex(-1) 
{
}

/*----------------------------------------------------------------------*/

inline OMS_OidHash::OidIter::OidIter(const OidIter& iter)
{
  *this = iter;
}

/*----------------------------------------------------------------------*/

inline OMS_OidHash::OidIter::~OidIter()
{
}

/*----------------------------------------------------------------------*/

inline void OMS_OidHash::OidIter::operator++() 
{
  if (NULL != m_curr) {
    m_curr = m_curr->GetNext();
  }
  if (NULL == m_curr) {
    //++m_arrayIndex;
    ++m_headIndex;
    while(m_arrayIndex < m_hash->m_arrayentries){
      while (m_headIndex < OMS_HASH_ARRAY_CHUNK_BUCKET_CNT) {
        m_curr = ((m_hash->m_headArray[m_arrayIndex])[m_headIndex]);
        if (NULL != m_curr) 
        {
          break;
        }
        ++m_headIndex;
      }
      if (NULL != m_curr) 
      {
        break;
      }
      ++m_arrayIndex;
      m_headIndex = 0;
    }
  }
}

/*----------------------------------------------------------------------*/

inline void OMS_OidHash::OidIter::operator++(int dummy) 
{
  operator++();
}

/*----------------------------------------------------------------------*/

inline OMS_OidHash::OidIter::operator bool() const 
{
  return (m_curr != NULL);
}

/*----------------------------------------------------------------------*/

inline OmsObjectContainerPtr OMS_OidHash::OidIter::operator()() const 
{
  return m_curr;
}

/*===========================================================================*/
/*! This method searches the hash for the object identified by the given OID.
**  While traversing the hash chain, the length of the hash chain is cheched
**  against the current maximal hash chain length and if necessary this
**  monitoring information is updated (PTS 1118855).
**  \param k OID of the object to lookup
**  \param ignoreGeneration If true, then the object with the given pageNo and pagePos
**         and with the 'replace-flag' = false is returned (ignoring the generation).
**         If false, then the object with exaclty the same oid (incl. the generation)
**         is returned.
**  \return a pointer to the object container if object is found; otherwise NULL */
/*===========================================================================*/
inline OmsObjectContainerPtr OMS_OidHash::HashFind (const OMS_ObjectId8* k, bool ignoreGeneration) 
{
	OmsObjectContainer* curr = *HeadPtr(*k);
  int cnt = 1;  // PTS 1118855

  if (ignoreGeneration){  // PTS 1125361
	  while (curr){
      if (!curr->ReplacedFlag() && curr->m_oid.equalExceptGeneration(*k)){
        MaxHashChainLen(cnt);  // PTS 1118855
			  return curr;
      }
      else {
			  curr = curr->GetNext();
        ++cnt;  // PTS 1118855
      }
    }
  }
  else {
    while (curr){
      if (curr->m_oid == *k){
        MaxHashChainLen(cnt);  // PTS 1118855
			  return curr;
      }
      else {
			  curr = curr->GetNext();
        ++cnt;  // PTS 1118855
      }
    }
  }

  MaxHashChainLen(cnt);  // PTS 1118855
	return NULL;
}


/*===========================================================================*/
/*! When a new keyed object is created with the same key as an other object, which was 
**  deleted in the same transaction, then the old one is marked as replaced. Therefore 
**  during the rollback of the 'NEW' such objects must be reactivated.
**  \param p Pointer to the frame of the object which should be rolled back.
**  \return Pointer to the object, if an object has been found and reactivated; 
**          otherwise NULL 
**  \since PTS 1125361
*/
/*===========================================================================*/
inline OmsObjectContainerPtr OMS_OidHash::ReactivateReplacedVersion(OmsObjectContainer* p)
{
  OmsObjectContainer* curr = *HeadPtr(p->m_oid);

	while (curr){
    if (curr->ReplacedFlag() && curr->m_oid.isDirectPredOf(p->m_oid)){
      curr->UnmarkReplaced();
			return curr;
    }
    else {
			curr = curr->GetNext();
    }
  }
  return NULL;
}







/*----------------------------------------------------------------------*/
/* ----  Implementation of OMS_ClusteredOidHash                     --- */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/

inline OMS_ClusteredOidHash::OMS_ClusteredOidHash(
  OMS_Context *pContext)
: m_pContext(pContext)
{
  for (int slot = 0; slot < MAX_HASH_SIZE; ++slot) {
    m_ppHead[slot] = NULL;
  }
}

/*----------------------------------------------------------------------*/

inline int OMS_ClusteredOidHash::Hash(const OMS_ObjectId8 &oid)
{
  return oid.getPno() % MAX_HASH_SIZE;
}

/*----------------------------------------------------------------------*/

inline void OMS_ClusteredOidHash::Insert(OmsObjectContainer *pObj)
{
  int slot = Hash(pObj->m_oid);
  OMS_ClusteredOidHashEntry *pEntry = m_ppHead[slot];
  while (pEntry){
    if (pEntry->m_page == pObj->m_oid.getPno()){
      // Append entry to already existing page-chain
      pObj->SetNext(pEntry->m_pObj);
      pEntry->m_pObj   = pObj;
      return;
    }
    else {
      pEntry = pEntry->m_pNext;
    }
  }

  // No entry for this page was found, so create a new one
  pEntry           = GetNewEntry();
  pEntry->m_pNext  = m_ppHead[slot];
  m_ppHead[slot]   = pEntry;  

  pEntry->m_page = pObj->m_oid.getPno();
  pEntry->m_pObj = pObj;
  pObj->SetNext(NULL);
}

/*----------------------------------------------------------------------*/

inline OMS_ClusteredOidHash::Iter OMS_ClusteredOidHash::First ()
{
  Iter iter(this);
  return iter;
}

/*----------------------------------------------------------------------*/
/* ----  Implementation of OMS_ClusteredOidHash::Iter               --- */
/*----------------------------------------------------------------------*/

inline OMS_ClusteredOidHash::Iter::Iter(
  OMS_ClusteredOidHash  *pHash) 
{
  m_pHash  = pHash;
  m_pObj   = NULL;
  m_pEntry = NULL;
  m_slot   = 0; 

  // Position iterator on the first entry
  for (m_slot = 0; m_slot < OMS_ClusteredOidHash::MAX_HASH_SIZE; ++m_slot) {
    m_pEntry = m_pHash->m_ppHead[m_slot];
    if (m_pEntry != NULL) {
      m_pObj = m_pEntry->m_pObj;
      return;
    }
  }
}

/*----------------------------------------------------------------------*/

inline OMS_ClusteredOidHash::Iter::Iter(const Iter& iter) 
{
  *this = iter;
}

/*----------------------------------------------------------------------*/

inline bool OMS_ClusteredOidHash::Iter::Next() 
{
  m_pObj = m_pObj->GetNext();
  if (m_pObj != NULL){
    return false;
  }

  m_pEntry = m_pEntry->m_pNext;
  if (m_pEntry){
    m_pObj = m_pEntry->m_pObj;
    return true;
  }

  ++m_slot;
  while (m_slot < OMS_ClusteredOidHash::MAX_HASH_SIZE){
    m_pEntry = m_pHash->m_ppHead[m_slot];
    if (m_pEntry){
      m_pObj = m_pEntry->m_pObj;
      return true;
    }
    ++m_slot;
  }

  // Iterator is empty
  m_pObj = NULL;
  return false;
}

/*----------------------------------------------------------------------*/

inline OMS_ClusteredOidHash::Iter::operator bool() const 
{
  return (m_pObj != NULL);
}

/*----------------------------------------------------------------------*/

inline OmsObjectContainer* OMS_ClusteredOidHash::Iter::operator()() const
{
  return m_pObj;
}


/*----------------------------------------------------------------------*/



#endif // __OMS_OIDHASH_HPP
