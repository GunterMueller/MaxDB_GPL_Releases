/*!
 * \file    OMS_ObjectContainerDef.hpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   Object frame, which is used to store the object in the cache of the liboms
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
#ifndef __OMS_OBJECTCONTAINERDEF_HPP
#define __OMS_OBJECTCONTAINERDEF_HPP

#include "Oms/OMS_AbstractObject.hpp"
#include "ggg91.h"
#include "Oms/OMS_ContainerHandle.hpp"  // FF 2003-09-23 new FileDir

class OmsObjectContainer;
class OMS_ContainerEntry;
class OMS_ClassEntry;
class OMS_Context;

typedef OmsObjectContainer* OmsObjectContainerPtr; 

/// Object Container for the storage in the local cache.  
class OmsObjectContainer{ 
  friend class OmsHandle;
  friend class OMS_OidHash;

public:
  /*! State of an object container */
  enum ct_state {
    /*! This flag is used to mark an object for being stored to the kernel. 
    ** This can be caused either by an explicit call of omsStore or when an object is newly 
    ** created. The flag is reset if the object is flushed to the kernel or when it is 
    ** deleted before it has been flushed. */
    CN_STORE = 1, 
    /*! This flag indicates whether an object is locked in the kernel. So every time a lock 
    ** is requested from the kernel this flag is set, too. The advantage of this local 
    ** bookkeeping is, that it is possible to check whether a lock is acquired on the object 
    ** without asking the kernel. 
    ** \attention The flag does not always reflect the status in the kernel. If the flag is
    ** set, then the object is locked in the kernel, but if the flag is not set, then there
    ** is not guarantee, that it is not locked in the kernel. Therefore if a guarantee is
    ** needed the kernel must be asked. A inconsistency might arise in the following 
    ** scenario:
    ** \code
    **   h.omsDeRefForUpd(oid, dolock=true);
    **   h.omsRelease(oid);
    **   h.omsDeRef(oid);
    ** \endcode
    */
    CN_LOCKED = 2, 
    /*! Flag is set if an object is deleted. In this case the flag CN_STORE is reset to initial. */
    CN_DELETE = 4, 
    /*! CN_NEW : This flag is only used in conjunction with before images and there it means, 
    ** that the corresponding object has been created in a subtransaction. If such an object is
    ** found in a rollback the corresponding object in the cache must be deleted. */
    CN_NEW = 8, 
    /*! This flag is set for objects with variable length */
    CN_VAROBJ = 16, 
    /*! Flag is set if object is newly created */
    CN_IS_NEW = 32,
    /*! Flag is set if an keyed object is deleted and a new keyed object with the same key is
    **  created in the same transaction. In this case the oid is reused, but the generation
    **  counter of the new oid is incremented. The frame in which the deleted object is stored
    **  is marked as replaced.
    **  \since PTS 1125361 */
    CN_REPLACED = 64,
    /*! This flag is set, if the object reuses the oid of a deleted object. 
    ** \since PTS 1127661 */
    CN_REUSED   = 128
  };

  /*! Additional state of an object container */
  enum ct_ver_state { 
    /*! This flag is set when a version object is copied into the local cache. */
    CN_VER_NEW = 1, 
    /*! This flag is set after a version object has been inserted into the structure for newly 
    ** created objects in versions (OMS_ContainerEntry::m_newVersionObjects) */
    CN_VER_LINKED = 2,
    /*! This flag indicates whether an object is locked in shared mode in the kernel. 
    ** So every time a lock is requested from the kernel this flag is set, too. The advantage 
    ** of this local bookkeeping is, that it is possible to check whether a lock is acquired 
    ** on the object without asking the kernel. 
    ** \attention The flag does not always reflect the status in the kernel. If the flag is
    ** set, then the object is locked in the kernel, but if the flag is not set, then there
    ** is not guarantee, that it is not locked in the kernel. Therefore if a guarantee is
    ** needed the kernel must be asked. A inconsistency might arise in the following 
    ** scenario:
    ** \code
    **   h.omsDeRefSharedLock(oid, dolock=true);
    **   h.omsRelease(oid);
    **   h.omsDeRef(oid);
    ** \endcode
    */
    CN_LOCKED_SHARED = 4,
    /*! This flag is used to generate an entry for the before-image list which shows, that
    **  a container has been dropped. The user-data of the object container is empty.
    **  The container handle (8 Byte) is stored in the variable m_oid.
    */
    CN_DROP_CONTAINER = 8,
    /*! This flag is used to generate an entry for the before-image list which shows, that
    **  a container has been created. The user-data of the object container is empty.
    **  The container handle (8 Byte) is stored in the variable m_oid.
    */
    CN_CREATE_CONTAINER = 16,
    /*! The frame does not own a data area */
    CN_HEADER_ONLY = 32
  };

private:
  /*! Link to next object in a hashlist. This links is either used to link together 
  ** data-objects belonging to the same hash value, freelist objects with the same size, 
  ** before-images belonging to the same subtransaction level ... */
  OmsObjectContainerPtr m_hashnext;
#if !defined(BIT64)
  SAPDB_UInt4           m_filler;
#endif

public:
  /*! Object identifier of the object stored in this container */
  OMS_ObjectId8         m_oid;
  tgg91_PageRef         m_objseq;

private:
  /*! Bit array which indicates the state of the object. See OmsObjectContainer::ct_state
  ** for a description of the possible states. */ 
  unsigned char         m_state;
  /*! Bit array which indicates the state of the object in a version. See 
  ** OmsObjectContainer::ct_ver_state for a description of the possible states. */
  unsigned char         m_verstate;


  /*! If frame is used to store a normal object, then the variables m_beforeImages and
  **  m_revisionNumber are filled.
  **  Otherwise if the frame is used as before-image, then a pointer to the corresponding 
  ** object in the oms-cache is stored in m_pOriginalObject. */
  union {
    struct {
      /*! Bit-array which indicates, on which level of sub-transaction there exists  
      **  a before-image for this object */
      tsp00_Uint4           m_beforeImages;
      /*! The last 4 bytes of the updateTransId. 
      **  On 64B platforms there was a gab (because of alignment of the following pointer) and 
      **  to keep the header size the same only 4 bytes of the 6 are used. */
      tsp00_Uint4           m_revisionNumber;
    } x;
    /*! Pointer to the object-frame in the oms-cache which corresponds to the before-image
    **  which is stored in this frame */
    OmsObjectContainer     *m_pOriginalObject;
  } y;

public:
  /*! Pointer to the class container entry which corresponds to object stored in this container. */
  OMS_ContainerEntry   *m_pContainerInfo; 
  /*! User data. At the beginning of this chunk the vtable-pointer is stored. Beside of this the structure is unknown; 
  **  only the size of this chunk is known. */
  OmsAbstractObject     m_pobj;

  inline OmsObjectContainer*  GetNext();
  inline OmsObjectContainer** GetNextAddr();
  inline void                 SetNext(OmsObjectContainer*);

  inline OmsObjectContainer*  GetOriginalObject() const                { return y.m_pOriginalObject; }
  inline void                 SetOriginalObject(OmsObjectContainer* p) { y.m_pOriginalObject = p;    }

  inline tsp00_Uint4          GetBeforeImageMask() const               { return y.x.m_beforeImages;    }
  inline void                 SetBeforeImageMask(tsp00_Uint4 m)        { y.x.m_beforeImages = m;       }

  unsigned int                GetRevisionNumber() const                      { return y.x.m_revisionNumber;           }
  void                        SetRevisionNumber(tsp00_Uint4 revisionNumber)  { y.x.m_revisionNumber = revisionNumber; }
  void                        SetRevisionNumber(const tgg91_TransNo &updTransId);

  inline void                 InitializeForFreeList(int location);
  inline void                 InitializeForAllocator(int location);
  inline void                 CheckFreeListState();
  inline OmsObjectContainer*  GetNextFreeList();
  inline void                 SetNextFreeList(OmsObjectContainer*);

  void                        PrintError(const char* msg, const OmsObjectContainer* p) const;  
  bool                        CheckConsistency() const;

  static tsp00_Int4 headerSize(){
    return (sizeof (OmsObjectContainer) - sizeof (OmsAbstractObject));
  }

  bool DeletedFlag() const {
    return ((m_state & OmsObjectContainer::CN_DELETE) > 0);
  }

  bool existBeforeImage (int subtransLvl, bool inVersion) const
  {
    if (0 == subtransLvl) {
      return true;
    }
    if ((!inVersion) && (1 == subtransLvl)) {
      return true;
    }
    return ((GetBeforeImageMask() & (1 << (subtransLvl - 1))) > 0);
  }

  inline OMS_ContainerEntry* GetContainerInfo(const char *pFile, unsigned int line);
  inline OMS_ContainerEntry* GetContainerInfoNoCheck();

  inline bool IsContainerAssigned() const
  {
    return m_pContainerInfo != NULL;
  }

  unsigned char GetState() const
  {
    return m_state;
  }

  inline void Init();
  inline void InitObjContainer(const OMS_ClassEntry& classEntry);

  void InitVerNew(size_t ofs)
  {
    void **ptr = (void**) (((char*) this) + ofs);
    ptr[0] = ptr[1] = NULL;
    m_verstate |= OmsObjectContainer::CN_VER_NEW;
  }


  bool IsNewObject() const
  {
    return ((m_state & OmsObjectContainer::CN_IS_NEW) > 0);
  }

  bool IsDropContainer() const
  {
    return ((m_verstate & OmsObjectContainer::CN_DROP_CONTAINER) > 0);
  }

  bool IsCreateContainer() const
  {
    return ((m_verstate & OmsObjectContainer::CN_CREATE_CONTAINER) > 0);
  }

  bool IsHeaderOnly() const
  {
    return ((m_verstate & OmsObjectContainer::CN_HEADER_ONLY) > 0);
  }

  bool IsVerNewObject() const
  {
    return ((m_verstate & OmsObjectContainer::CN_VER_NEW) > 0);
  }

  bool IsVerNewRegistered() const
  {
    return ((m_verstate & OmsObjectContainer::CN_VER_LINKED) > 0);
  }

  void UnmarkVerNewObject()
  {
    m_verstate = 0;
  }

  bool LockedFlag() const
  {
    return ((m_state & OmsObjectContainer::CN_LOCKED) > 0);
  }

  bool LockedSharedFlag() const
  {
    return ((m_verstate & OmsObjectContainer::CN_LOCKED_SHARED) > 0);
  }

  void MarkAsNew()
  {
    m_state |= OmsObjectContainer::CN_IS_NEW;
  }

  void MarkDeleted()
  {
    m_state &= (OmsObjectContainer::CN_STORE ^ 0xFF);
    m_state |= (OmsObjectContainer::CN_DELETE);
  }

  void MarkDropContainer()
  {
    m_verstate |= OmsObjectContainer::CN_DROP_CONTAINER;
  }

  void MarkCreateContainer()
  {
    m_verstate |= OmsObjectContainer::CN_CREATE_CONTAINER;
  }

  void MarkHeaderOnly()
  {
    m_verstate |= OmsObjectContainer::CN_HEADER_ONLY;
  }

  void MarkLocked()
  {
    m_state |= OmsObjectContainer::CN_LOCKED;
  }

  void MarkLockedShared()
  {
    m_verstate |= OmsObjectContainer::CN_LOCKED_SHARED;
  }

  void MarkNew()
  {
    m_state |= OmsObjectContainer::CN_NEW;
  }

  void MarkReplaced()  // PTS 1125361
  {
    m_state |= OmsObjectContainer::CN_REPLACED;
  }

  void MarkReused()  // PTS 1127661
  {
    m_state |= OmsObjectContainer::CN_REUSED;
  }

  void MarkVerNewRegistered()
  {
    m_verstate |= OmsObjectContainer::CN_VER_LINKED;
  }

  void MarkStored()
  {
    m_state |= OmsObjectContainer::CN_STORE;
  }

  void MarkVarObj() 
  {
    m_state |= OmsObjectContainer::CN_VAROBJ;
  }

  bool NewFlag() const
  {
    return ((m_state & OmsObjectContainer::CN_NEW) > 0);
  }

  void RemoveStoredFlag() 
  {
    m_state &= (OmsObjectContainer::CN_STORE ^ 0xFF);
  }

  bool ReplacedFlag() const   // PTS 1125361
  {
    return ((m_state & OmsObjectContainer::CN_REPLACED) > 0);
  }
  
  bool ReusedFlag() const  // PTS 1127661
  {
    return ((m_state & OmsObjectContainer::CN_REUSED) > 0);
  }

  void setBeforeImage (int subtransLvl) 
  {
    SetBeforeImageMask(GetBeforeImageMask() | (1 << (subtransLvl-1))); 
  }

  static void store (OmsHandle& h, OmsAbstractObject* o) 
  {
    OmsObjectContainerPtr p = containerPtr(o);
    p->m_state |= OmsObjectContainer::CN_STORE;
  }

  void SetContainerInfo(OMS_ContainerEntry *pContainerInfo) 
  {
    m_pContainerInfo = pContainerInfo;
  }

  bool StoredFlag() const {
    return ((m_state & OmsObjectContainer::CN_STORE) > 0);
  }

  void UnmarkReplaced()  // PTS 1125361
  {
    m_state &= (OmsObjectContainer::CN_REPLACED ^ 0xFF);
  }
  
  void UnmarkReused()  // PTS 1127661
  {
    m_state &= (OmsObjectContainer::CN_REUSED ^ 0xFF);
  }

  bool VarObjFlag() const {
    return ((m_state & OmsObjectContainer::CN_VAROBJ) > 0);
  }

  bool Changed() const{
    return ((m_state & (CN_STORE | CN_LOCKED | CN_DELETE)) > 0);
  }

  static OmsObjectContainerPtr containerPtr(OmsAbstractObject* pobj) 
  {
    return (OmsObjectContainerPtr) (((unsigned char*) pobj) - ((sizeof(OmsObjectContainer)) - sizeof (OmsAbstractObject))); 
  }

  void Copy (OMS_Context* pContext, OmsObjectContainerPtr pBeforeImage, size_t ObjectSize);

  /*!
   * \brief Compare object data in this object and before image.
   *
   * This method compares object data in the object with those in before image,
   * except vtable. This is used in simulator to check for not stored updated
   * objects.
   *
   * \param pContext context,
   * \param pBeforeImage before image,
   * \param ObjectSize object size excluding vtable (persistent size).
   *
   * \return 0, if data match, !=0, if data differ.
   */
  int Compare (OMS_Context* pContext, OmsObjectContainerPtr pBeforeImage, size_t ObjectSize);
};

/*----------------------------------------------------------------------*/

inline OmsObjectContainer* OmsObjectContainer::GetNext()
{
//#ifdef _ASSERT_OMS
  CheckConsistency();
//#endif

  return m_hashnext;
}

/*----------------------------------------------------------------------*/

inline void OmsObjectContainer::SetNext(OmsObjectContainer *p)
{
//#ifdef _ASSERT_OMS
  CheckConsistency();
//#endif

  m_hashnext = p;
}

/*----------------------------------------------------------------------*/

inline bool OmsObjectContainer::CheckConsistency() const
{
  unsigned int pattern1 = 0xfdfdfdfd;
  unsigned int pattern2 = 0xadadadad;

  tsp00_Uint4 t = *reinterpret_cast<const tsp00_Uint4*>(this);

  if (t == pattern1){
    PrintError("Illegal pattern 'fd' found.", this);
    return false;
  }
  else if (t == pattern2){
    PrintError("Illegal pattern 'ad' found.", this);
    return false;
  }

  return true;
}

/*----------------------------------------------------------------------*/

#endif  // __OMS_OBJECTCONTAINERDEF_HPP
