/*!
 * \file    OMS_VersionDictionary.hpp
 * \author  MarkusSi
 * \brief   OMS context.
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

#ifndef __OMS_VERSIONDICTIONARY_HPP
#define __OMS_VERSIONDICTIONARY_HPP

#include "Oms/OMS_ContextDef.hpp"
#include "Oms/OMS_UnloadableVersionList.hpp"
#include "Oms/OMS_Defines.h"                     
#include "Oms/OMS_LibOmsInterfaceInstance.hpp"    

class OMS_VersionDictionary {
public :
  /// Iterator for the version dictionary  
  /*! 
  ** This class implements an iterator to read the version dictionary.
  **
  **  To prevent lock contention, it is possible to release the lock after
  **  processing each slot. So time-expensive operation can be executed without 
  **  the danger of lock-contention.
  **  
  **  Example of how to use the iterator
  **  \code
  **  iter.Reset(EXCLUSIVE);
  **  while (true){
  **    pContext = iter.GetFirstInSlot();  // Release lock on old slot (if one is hold) and
  **                                       // acquire lock on new slot 
  **    if (!pContext) break;
  **    while (pContext){
  **      pNext = iter.GetNextInSlot();
  **      // Do anything while holding a lock
  **      pContext = pNext;
  **    }  
  **    iter.ReleaseSlot();                // Release lock on old slot    
  **    // Do anything without holding a lock
  **  }
  **  \endcode
  **
  ** \attention Do not drop a version while iterating over the version dictionary
  **    with this operator. 
  ** \attention Even if the access is synchronized, the iterator might not return a 'snapshort'
  **   as always only a single lock on the current slot is hold while iterating over the entries
  **   and not a global lock on all slots. This is done because of performance and as it is not
  **   considered as so important to get a real 'snapshot'.
  */
  class Iter {
  private:
    /// Slot of the hashbucket, where the iterator is currently positioned to. 
    int                            m_slot;
    /// How should the entries be locked while iteration over them?
    OMS_LockMode                   m_lockMode;
    /// Indicator, whether currently a lock on a chain is hold.
    bool                           m_locked;
    /// The version the iteration is currently pointing to.
    OMS_Context                   *m_pCurr;
    /*! Pointer, to identify the iterator in the kernel, if the ProcServer is active.
    **  \attention Do not deref this pointer, it is only useful as parameter to call
    **  ProcServer-methods
    */
    void                          *m_pIter;

    /// Enters the region (either reader-writer of critical section) which belongs to the current slot  
    void EnterRgn();
    /// Leaves the region (either reader-writer of critical section) which belongs to the current slot  
    void LeaveRgn();

    Iter(const Iter&);                  ///< Not allowed, therefore private
    Iter& operator=(const Iter &iter);  ///< Not allowed, therefore private
  
  public :
    /// Default-constructor
    Iter();
    Iter(OMS_LockMode lockMode);

    /// Destructor
    /*! 
    ** The iterator is destructed. If it still holds some locks, then these will be
    ** released.
    */
    ~Iter();

    /// Resets an iterator.
    /*!
    **  Starts an iterator without positioning the iterator onto the first element
    **  and without aquiring a lock. 
    */  
    void Reset(OMS_LockMode lockMode);

    /// Position the iterator onto the first element in the next non-empty slot.
    /*!
    **  Position the iterator onto the first element in the next non-empty slot. A lock
    **  for this slot is aquired and is hold.
    **
    **  \return Pointer to the version the iterator is currently pointing, if a version
    **          was found; otherwise NULL (this means the iterator has reached its end)
    */
    OMS_Context* GetFirstInSlot();

    /// Position the iterator onto the next element in the same slot
    /*! Position the iterator onto the next element in the same slot, if a next element
    **  exists. If no next element is found, then the lock of the current slot is released.
    **  
    **  \return Pointer to the version the iterator is currently pointing, if a version
    **          was found; otherwise NULL (this means all versions of this slot have been read)
    */
    OMS_Context* GetNextInSlot();

    /// The current slot is released and the lock on this slot is freed
    /*!
    **  The current slot is released and the lock on this slot is freed. 
    **  If a GetNextInSlot is called after a ReleaseSlot this will lead to an
    **  error. To continue with the iteration in the next slot, GetFirstInSlot
    **  must be called first.
    */
    void ReleaseSlot();

    /// Stops an iterator
    /*! 
    **  Stops an iterator. This function is needed, if an iterator should stop processing
    **  without having run to the end. In this case the iterator might hold a lock which must
    **  be released to prevent lock contention.
    **  A call of this method set the iterator to invalid mode. Therefore it is not possible 
    **  to continue iterating after having called 'stop'.
    */
    void Stop();
  };

  /// Constructor
  OMS_VersionDictionary();

  /// Returns the version corrsponding to the specified version-id
  OMS_Context* FindVersion  (const OmsVersionId&);

  /// Inserts a new version in the directory
  /*!
  ** Inserts a new version in the directory.
  **
  ** \param versionId [in] Name of the version (key in the search-structure), 
  **                       which should be inserted
  ** \param pContext  [in] Pointer to the version, which should be inserted
  **
  ** \return If a version with the same name already exists, then the error code
  **         e_OMS_duplicate_name is returned; otherwise e_ok
  */
  tgg00_BasisError InsertVersion(const OmsVersionId& versionId, OMS_Context* pContext); 

  /// Deletes a version out of the directory
  /*!
  ** Deletes a version out of the directory.
  ** 
  ** \param versionId [in] Name of the version, which which should be deleted
  **
  ** \return If no version with the specified name is found, then e_unknown_version
  **         is returned; otherwise e_ok
  */
  tgg00_BasisError DropVersion  (const OmsVersionId& versionId);
 
  void             MarkUnloadable(OMS_Context* pContext);
  void             MarkNotUnloadable(IliveCacheSink* lcSink, OMS_Context* pContext);
  void             Shutdown();
  bool             UnloadOldestVersion(IliveCacheSink* lcSink);

  /// Determines and returns the hash-slot of a given version-id  
  int              GetSingleLockId(const OmsVersionId &versionId) const;

  /// Determines and returns the hash-slots of all given version-ids
  /*! 
  ** Determines and returns the hash-slots of all given version-ids.
  **
  ** \param ppVersionId [in]  Array with the given version-ids.
  ** \param size        [in,out] in: number of version-ids in the array ppVersionId
  **                             out: number of slot-ids returned in pLockId
  **  \param pLockId    [out] Array with the hash-slots which corresponds to the given version-ids
  **
  ** \attention The entries in the array pLockId are sorted in ascending order
  ** to prevent deadlocks. Therefore does not change this sequence manually!
  */
  void             GetMultiLockIds(const OmsVersionId **ppVersionId, int &size, int  *pLockId) const;
  
  /// Returns all lock-ids of the version-dictionary
  void             GetAllLockIds(int *pLockId, int &size) const;

  /// Are reader-writer-locks or critical section used for synchonization
  bool             UseRWLocks() const { return m_useRWLocks; }

  bool             NextDiagEntry (IliveCacheSink* pLcSink, bool first, char* pDiagEntry, int size, short &error); // PTS 1127508


private :
  enum { VDIR_START_CNT = 1 };
  enum { VDIR_SIZE = 17 };

  /// Compute the hash-slot for a given version-id
#if defined WIN32 && defined AMD64
  inline __int64   HashValue (const OmsVersionId&) const;
#else
  inline int       HashValue (const OmsVersionId&) const;
#endif
  /// Return the offset of the lock-ids.
  /*! 
  ** In the version-dictionary coding it is assumed that the lock-ids start with
  ** the number zero, but in the kernel these lock-ids might be mapped to other 
  ** ids. Therefore this offset is needed.
  */
  inline int       GetVDirStartCnt() const;
  inline int       GetVDirSize()     const;

  /// Does the own session own a lock on the given slot
  bool isLockedByOwn(int slot) const;   

  /// Hash-array. Collisions are resolved by chaining 
  OMS_Context           *m_pVersionDictionary[VDIR_SIZE];
  /// Datastructure which contains all versions which could be unloaded
  OMS_UnloadableVersionList m_UnloadableVersions;
  /// Flag which indicates whether to use reader-writer-locks or critical sections for synchonization
  bool                   m_useRWLocks;
  /*! Pointer to an iterator. This iterator is used to remember the state, if an 
  **  ude-server is active and the version dictionary is read from an other process. */
  Iter                  *m_pIter;
  /// Iterator to read the versions currently in the dictionary.
  Iter                   m_iter;

  friend void OMS_LibOmsInterfaceInstance::AdviseKernelInterface(
    LVC_KernelInterface& knlInterface, 
    bool isKernel,
    bool isUDEServer); // Initializes the variable m_useRWLocks
  friend class Iter;
  friend void OMS_Context::DeleteSelf();
};

/*---------------------------------------------------------------------------*/

inline int OMS_VersionDictionary::GetVDirStartCnt() const {
  return VDIR_START_CNT; 
}

/*---------------------------------------------------------------------------*/

inline int OMS_VersionDictionary::GetVDirSize() const {
  return VDIR_SIZE; 
}

/*---------------------------------------------------------------------------*/

#if defined WIN32 && defined AMD64
// this is to avoid compiler bug in msc 14.00 rc1
inline __int64 OMS_VersionDictionary::HashValue (const OmsVersionId &versionId) const{
#else
inline int OMS_VersionDictionary::HashValue (const OmsVersionId &versionId) const{
#endif
  unsigned long sum = 0;
  for (int i=0; i<sizeof(OmsVersionId); ++i){
    sum ^= ((sum << 5) ^ versionId[i]);
  }
  return (sum % VDIR_SIZE);
}

#endif  // __OMS_VERSIONDICTIONARY_HPP
