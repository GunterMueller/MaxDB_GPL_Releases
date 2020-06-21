/*!
 * \file    OMS_BeforeImageList.cpp
 * \author  MarkusSi, Roterring
 * \brief   Handling of the beforeImages of subtransactions
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
#include "Oms/OMS_BeforeImageList.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_DumpInterface.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Oms/OMS_ContainerHandle.hpp"

class OMS_BeforeImageListDumpInfo 
{
public:
  OmsObjectContainerPtr m_this;
  OmsObjectContainerPtr m_hashNext;
  OMS_Context*          m_context;
  OMS_ContainerEntry*   m_containerInfo;
  OMS_ObjectId8         m_oid;
  tsp00_Int2            m_subtransLevel;
  unsigned char         m_state;
  bool                  m_filler1;
  tsp00_Int4            m_filler2;
};


OMS_BeforeImageList::OMS_BeforeImageList () : m_session(NULL) 
{
  int ix;
  for (ix = 0; ix < OMS_BEFOREIMAGE_MAX_SUBTRANS_LEVEL; ix++) {
    m_beforeImages[ix] = NULL;
  }
}

/*----------------------------------------------------------------------*/

OMS_BeforeImageList::~OMS_BeforeImageList ()
{

}

/*----------------------------------------------------------------------*/

void OMS_BeforeImageList::Dump(OMS_DumpInterface& dumpObj) const
{
  OMS_BeforeImageListDumpInfo beforeImageDumpInfo;
  OmsObjectContainerPtr pBeforeImage;
  bool isFirst = true;
  int  subtransLvl = m_session->CurrentSubtransLevel();
  while (subtransLvl > 0) {
    beforeImageDumpInfo.m_subtransLevel = subtransLvl;
    pBeforeImage = m_beforeImages[subtransLvl-1];
    while (pBeforeImage) {
      if (isFirst)
      {
        isFirst = false;
        dumpObj.SetDumpLabel(LABEL_OMS_BEFORE_IMAGE);
      }
      beforeImageDumpInfo.m_this     = pBeforeImage;
      beforeImageDumpInfo.m_hashNext = pBeforeImage->GetNext();
      beforeImageDumpInfo.m_context = *REINTERPRET_CAST(OMS_Context**, &pBeforeImage->m_pobj);
      OMS_ContainerEntry* pContainerInfo = 
        pBeforeImage->GetContainerInfoNoCheck();   // PTS 1127338
      if (pContainerInfo != NULL && !pContainerInfo->IsDropped())
        beforeImageDumpInfo.m_containerInfo = pContainerInfo;
      else
        beforeImageDumpInfo.m_containerInfo = NULL;
      beforeImageDumpInfo.m_state = pBeforeImage->GetState();
      beforeImageDumpInfo.m_oid   = pBeforeImage->m_oid;
      dumpObj.Dump(&beforeImageDumpInfo, sizeof(beforeImageDumpInfo));
      pBeforeImage   = pBeforeImage->GetNext();
    }
    subtransLvl--; 
  }
}

/*----------------------------------------------------------------------*/

void OMS_BeforeImageList::init (OMS_Session* s)
{
  m_session = s;
}

/*----------------------------------------------------------------------*/

void OMS_BeforeImageList::freeBeforeImages (int level)
{
  OmsObjectContainerPtr pBeforeImage;
  OmsObjectContainerPtr pToFree;
  OMS_ContainerEntry   *pContainerInfo;
  OMS_Context          *pContext;
  int                   subtransLvl = m_session->CurrentSubtransLevel();
  while (subtransLvl > level-1) {
    pBeforeImage                  = m_beforeImages[subtransLvl-1];
    m_beforeImages[subtransLvl-1] = NULL;
    while (pBeforeImage) {
      pContext       = *REINTERPRET_CAST(OMS_Context**, &pBeforeImage->m_pobj);
      pContainerInfo = pBeforeImage->GetContainerInfoNoCheck();   // PTS 1127338
      pToFree        = pBeforeImage;
      pBeforeImage   = pBeforeImage->GetNext();

      if (pToFree->IsDropContainer() || pToFree->IsCreateContainer() || pToFree->NewFlag()){
        pContext->GetContainerDir().ChainFree(pToFree, /*size=*/sizeof(void*), 1);
      }
      else {
        pContainerInfo->ChainFree(pToFree, 2);
      }
    }
    subtransLvl--; 
  }
}

/*----------------------------------------------------------------------*/

void OMS_BeforeImageList::removeContext (OMS_Context* pContext) 
{
  OmsObjectContainerPtr  pBeforeImage;
  OmsObjectContainerPtr* pPrev;
  OmsObjectContainerPtr  pToFree;
  OMS_ContainerEntry*    pContainerInfo;
  OMS_Context*           pCurrContext;
  int                    subtransLvl = m_session->CurrentSubtransLevel();
  while (subtransLvl > 0) {
    pBeforeImage = m_beforeImages[subtransLvl-1];
    pPrev        = &m_beforeImages[subtransLvl-1];
    while (NULL != pBeforeImage) {
      pCurrContext = *REINTERPRET_CAST(OMS_Context**, &pBeforeImage->m_pobj);
      if (pCurrContext == pContext) {
        pContainerInfo = pBeforeImage->GetContainerInfoNoCheck();  // PTS 1127338
        pToFree        = pBeforeImage;
        pBeforeImage   = pBeforeImage->GetNext();
        *pPrev         = pBeforeImage;

        if (pToFree->IsDropContainer() || pToFree->IsCreateContainer() || pToFree->NewFlag()){
          pContext->GetContainerDir().ChainFree(pToFree, /*size=*/sizeof(void*), 3);
        }
        else {
          pContainerInfo->ChainFree(pToFree, 4);
        }
      }
      else {
        pPrev        = pBeforeImage->GetNextAddr();
        pBeforeImage = pBeforeImage->GetNext();
      }
    }
    subtransLvl--; 
  } 
}

/*----------------------------------------------------------------------*/

void OMS_BeforeImageList::subtransCommit (int wantedSubtransLvl) 
{
  OmsObjectContainerPtr pBeforeImage, p;
  OmsObjectContainerPtr found;
  OMS_Context*          pContext;

  int         subtransLvl = m_session->CurrentSubtransLevel();
  tsp00_Uint4 mask = (1 == wantedSubtransLvl) ? 0 : (0xFFFFFFFF >> (OMS_BEFOREIMAGE_MAX_SUBTRANS_LEVEL - wantedSubtransLvl + 1));
  const bool version = m_session->InVersion() || OMS_Globals::m_globalsInstance->InSimulator();
  
  while (subtransLvl >= wantedSubtransLvl) {
    
    OMS_DETAIL_TRACE(omsTrFlushCache, m_session->m_lcSink,
                "OMS_BeforeImageList::subtransCommit : commit subtransLvl=" << 
                subtransLvl);

    subtransLvl--;
    pBeforeImage                = m_beforeImages[subtransLvl];
    m_beforeImages[subtransLvl] = NULL;

    OmsObjectContainerPtr* pInsert = NULL;
    if (wantedSubtransLvl >= 2) {
      pInsert = &m_beforeImages[wantedSubtransLvl-2];
    }

    while (pBeforeImage) {
      p                  = pBeforeImage;
      pBeforeImage       = pBeforeImage->GetNext();
      pContext            = *reinterpret_cast<OMS_Context**>(&p->m_pobj);

      if (p->IsDropContainer()){
        // frame represents a DropContainer.
        OMS_ContainerHandle *handle = reinterpret_cast<OMS_ContainerHandle*>(&p->m_oid);
        OMS_DETAIL_TRACE(omsTrFlushCache, m_session->m_lcSink,
          "OMS_BeforeImageList::subtransCommit : commit drop container : "  << *handle);
        pContext->GetContainerDir().CommitDropContainer(handle);

        if (pInsert){
          p->SetNext(*pInsert);
          *pInsert   = p;
          pInsert    = p->GetNextAddr();
        }
      }
      else if (p->IsCreateContainer()){
        // frame represents a CreateContainer.
        OMS_ContainerHandle *handle = reinterpret_cast<OMS_ContainerHandle*>(&p->m_oid);
        OMS_DETAIL_TRACE(omsTrFlushCache, m_session->m_lcSink,
          "OMS_BeforeImageList::subtransCommit : commit create container : " << *handle);
        pContext->GetContainerDir().CommitCreateContainer(handle);

        if (pInsert){
          p->SetNext(*pInsert);
          *pInsert   = p;
          pInsert    = p->GetNextAddr();
        }
      }
      else {
        // frame represents an operation on a persistent object
        found = pContext->FindObjInContext(&p->m_oid, /*ignoreGeneration=*/false, /*checkDropped=*/false);
        if (found) {
          if (found != p->GetOriginalObject()){
            OMS_Globals::Throw(e_invalid_oid, "Object does not match before-image (overflow of generation)", found->m_oid, __FILE__, __LINE__);
          }

          OMS_DETAIL_TRACE(omsTrFlushCache, m_session->m_lcSink,
            "OMS_BeforeImageList::subtransCommit : commit object : " << found->m_oid 
            << ", class: " << found->GetContainerInfoNoCheck()->GetClassEntry().GetClassName());

          found->SetBeforeImageMask(found->GetBeforeImageMask() & mask);

          if (pInsert && !found->existBeforeImage(wantedSubtransLvl-1, version)) {
            p->SetNext(*pInsert);
            *pInsert      = p;
            pInsert       = p->GetNextAddr();
            found->setBeforeImage (wantedSubtransLvl-1);
          }
          else {
            if (p->NewFlag()){
              // release memory of the before-image (before-image consists only of a frame-header!!)
              pContext->GetContainerDir().ChainFree(p, /*size=*/sizeof(void*), 5);
            }
            else {
              // release memory of a regular frame
              p->GetContainerInfoNoCheck()->ChainFree(p, 6);
            }
          }
        }
        else {
          if (!p->GetContainerInfoNoCheck()->IsDropped()){ // PTS 1127338 condition added 
            // object must have been release by application, i.e. omsRelease of an
            // object, that has been read for update without omsStore, omsDelete
            DbpBase opMsg(m_session->m_lcSink);
            opMsg.dbpOpError("OMS_BeforeImageList::subtransRollback: Try to rollback object which does not exist anymore in the cache. oid=%d.%d(%d)",
                             p->m_oid.getPno(), p->m_oid.getPagePos(), p->m_oid.getGeneration()); 

            if (p->NewFlag()){
              // release memory of the before-image (before-image consists only of a frame-header!!)
              pContext->GetContainerDir().ChainFree(p, /*size=*/sizeof(void*), 7);
            }
            else {
              // release memory of a regular frame
              p->GetContainerInfoNoCheck()->ChainFree(p, 8);
            }
          }
        }
      }
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_BeforeImageList::subtransRollback (int wantedSubtransLvl)
{
  OmsObjectContainerPtr pBeforeImage, p;

  int         subtransLvl = m_session->CurrentSubtransLevel();
  tsp00_Uint4 mask        = (1 == wantedSubtransLvl) 
                            ? 0 
                            : (0xFFFFFFFF >> (OMS_BEFOREIMAGE_MAX_SUBTRANS_LEVEL - wantedSubtransLvl + 1));
  
  while (subtransLvl >= wantedSubtransLvl) {

    OMS_DETAIL_TRACE(omsTrFlushCache, m_session->m_lcSink,
              "OMS_BeforeImageList::subtransRollback : rollback subtransLvl=" << 
              subtransLvl);

    subtransLvl--;
    pBeforeImage                = m_beforeImages[subtransLvl];
    m_beforeImages[subtransLvl] = NULL;

    while (pBeforeImage) {
      p                  = pBeforeImage;
      pBeforeImage       = pBeforeImage->GetNext();

      // Pointer to the context was stored in the vtable-field of the frame
      OMS_Context*  pContext  = *reinterpret_cast<OMS_Context**>(&p->m_pobj);

      if (p->IsDropContainer()){
        // frame represents a DropContainer.
        // ContainerHandle is stored in the oid-field of the frame
        OMS_ContainerHandle *handle = reinterpret_cast<OMS_ContainerHandle*>(&p->m_oid);
        OMS_DETAIL_TRACE(omsTrFlushCache, m_session->m_lcSink,
          "OMS_BeforeImageList::subtransRollback : rollback drop container : " << *handle);

        pContext->GetContainerDir().RollbackDropContainer(handle);
        pContext->GetContainerDir().ChainFree(p, /*size=*/sizeof(void*), 9);
      }
      else if (p->IsCreateContainer()){
        // frame represents a DropContainer.
        // ContainerHandle is stored in the oid-field of the frame
        OMS_ContainerHandle *handle = reinterpret_cast<OMS_ContainerHandle*>(&p->m_oid);
        OMS_DETAIL_TRACE(omsTrFlushCache, m_session->m_lcSink,
          "OMS_BeforeImageList::subtransRollback : rollback create container : " << *handle);

        pContext->GetContainerDir().RollbackCreateContainer(handle);
        pContext->GetContainerDir().ChainFree(p, /*size=*/sizeof(void*), 10);
      }
      else {
        // frame represents an operation on a persistent object
        OMS_ContainerEntry* pContainerInfo = p->GetContainerInfoNoCheck();   // PTS 1127338

        OmsObjectContainerPtr found = pContext->FindObjInContext (&p->m_oid, /*ignoreGeneration=*/false, /*checkDropped=*/false);
        if (found){
          if (found != p->GetOriginalObject()){
            OMS_Globals::Throw(e_invalid_oid, "Object does not match before-image (overflow of generation)", found->m_oid, __FILE__, __LINE__);
          }

          if (p->NewFlag()) {
            // Before-image indicates, that the object was newly created in the subtransaction. 
            // So the rollback must delete the object completely,
            
            if (pContext->IsVersionOid(found->m_oid)){
              pContext->DecNewObjectsToFlush(found);

              OMS_DETAIL_TRACE(omsTrNewObj | omsTrFlushCache, m_session->m_lcSink,
                "OMS_BeforeImageList::subtransRollback : rollback new object(version) : dec flush: " << found->m_oid 
                << ", class: " << pContainerInfo->GetClassEntry().GetClassName());

              if (pContainerInfo->GetClassEntry().IsKeyedObject()) {
                  // Remove the entry in the key-tree (version)
                  pContainerInfo->VersionDelKey(found);
              }
            }
            else {
              OmsObjectContainerPtr pReactivated = NULL;
              if (found->ReusedFlag()){
                // Reactivate 'replaced' version if one exists   PTS 1125361
                pReactivated = pContext->ReactivateReplacedVersion(p);

                if (pReactivated){
                  OMS_DETAIL_TRACE(omsTrNewObj, m_session->m_lcSink,
                    "OMS_BeforeImageList::subtransRollback : reactivate object : " << pReactivated->m_oid);
                }
              }
              
              // If reactivated object is also marked as new, then the number of objects to 
              // flush remain unchanged
              if (!(pReactivated && pReactivated->IsNewObject())){
                pContext->DecNewObjectsToFlush(found);

                OMS_DETAIL_TRACE(omsTrNewObj | omsTrFlushCache, m_session->m_lcSink,
                  "OMS_BeforeImageList::subtransRollback : rollback new object : dec flush: " << found->m_oid 
                  << ", class: " << pContainerInfo->GetClassEntry().GetClassName());
              }
              else{
                OMS_DETAIL_TRACE(omsTrFlushCache, m_session->m_lcSink,
                  "OMS_BeforeImageList::subtransRollback : rollback new object : " << found->m_oid 
                  << ", class: " << pContainerInfo->GetClassEntry().GetClassName());
              }

              if (pContainerInfo->GetClassEntry().IsKeyedObject()) {
                // Adapt the key-tree
                if ((pContainerInfo->UseCachedKeys())
                  && (found == pContainerInfo->VersionFindKey(pContainerInfo->GetClassEntry().GetKeyPtr(found)))){
                  if (pReactivated != NULL){
                    // Redirect the pointer in the key tree to the formerly replaced object
                    pContainerInfo->VersionReplaceOrAddKeyContainerPtr(pReactivated);
                  }
                  else {
                    // Remove the entry in the key-tree (cached-keys)
                    pContainerInfo->VersionDelKey(found);
                  }
                }
              }
            }

            // remove object out of the cache
            pContext->DeleteObjInContext(found, pContainerInfo);

            // release memory of the before-image (before-image is only a frame-header!!)
            pContext->GetContainerDir().ChainFree(p, /*size=*/sizeof(void*), 11);
          }
          else {
            // Before-image indicates, that the object was modified in the subtransaction.

            // Restores the field which where used to store the pointer to the object in the cache
            p->SetBeforeImageMask(found->GetBeforeImageMask() & mask);
            p->SetRevisionNumber(found->GetRevisionNumber());

            if (found->IsNewObject() && ! p->IsNewObject()) {
              pContext->DecNewObjectsToFlush(found);

              OMS_DETAIL_TRACE(omsTrNewObj | omsTrFlushCache, m_session->m_lcSink,
                "OMS_BeforeImageList::subtransRollback : rollback modified object : dec flush: " << found->m_oid 
                << ", class: " << pContainerInfo->GetClassEntry().GetClassName());
            }
            else
            {
              OMS_DETAIL_TRACE(omsTrFlushCache, m_session->m_lcSink,
                "OMS_BeforeImageList::subtransRollback : rollback modified object : " << found->m_oid 
                << ", class: " << pContainerInfo->GetClassEntry().GetClassName());
            }

            // Copy object body into the old frame.
            found->Copy (pContext, p, pContainerInfo->GetClassEntry().GetTransientSize());  

            if (pContainerInfo->GetClassEntry().IsKeyedObject()
              && (pContainerInfo->UseCachedKeys() || pContext->IsVersionOid(found->m_oid))
              && !found->DeletedFlag()){
              // Adjust the key-tree, so that the entry is pointing to the right object
              pContainerInfo->VersionReplaceOrAddKeyContainerPtr(found);
            }

            // restore vtable ptr 
            *REINTERPRET_CAST(void**, &found->m_pobj) = pContainerInfo->GetClassEntry().GetVTblPtr();
          
            // Release the memory of the before-image
            pContainerInfo->ChainFree(p, 12);
          }
        }
        else {
          if (!pContainerInfo->IsDropped()){ // PTS 1127338 condition added 
            // object must have been release by application, i.e. omsRelease of an
            // object, that has been read for update without omsStore, omsDelete
            DbpBase opMsg(m_session->m_lcSink);
            opMsg.dbpOpError("OMS_BeforeImageList::subtransRollback: Try to rollback object which does not exist anymore in the cache. oid=%d.%d(%d)",
                             p->m_oid.getPno(), p->m_oid.getPagePos(), p->m_oid.getGeneration()); 

            if (p->NewFlag()){
              // release memory of the before-image (before-image consists only of a frame-header!!)
              pContext->GetContainerDir().ChainFree(p, /*size=*/sizeof(void*), 13);
            }
            else {
              // release memory of a regular frame
              pContainerInfo->ChainFree(p, 14);
            }
          }
        }
      }
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_BeforeImageList::checkStores()
{
  OmsObjectContainerPtr pBeforeImage = m_beforeImages[0];
  while (pBeforeImage) {
    OmsObjectContainerPtr p = pBeforeImage;
    pBeforeImage            = pBeforeImage->GetNext();

    if (  !p->IsCreateContainer()
       && !p->IsDropContainer()
       && !p->NewFlag()) {
      OMS_Context* context        = *REINTERPRET_CAST(OMS_Context**, &p->m_pobj);
      OmsObjectContainerPtr found = context->FindObjInContext(&p->m_oid, /*ignoreGeneration=*/false, /*checkDropped=*/false);
      if (NULL != found) {
        if (found != p->GetOriginalObject()){
          OMS_Globals::Throw(e_invalid_oid, "Object does not match before-image (overflow of generation)", found->m_oid, __FILE__, __LINE__);
        }

        OMS_ContainerEntry* pContainerInfo = p->GetContainerInfoNoCheck();   
        if (!found->StoredFlag() && !found->DeletedFlag() &&
          found->Compare(context, p, pContainerInfo->GetClassEntry().GetPersistentSize()) != 0) {
          // ERROR: not stored, but modified
          const char *className = pContainerInfo->GetClassEntry().GetClassName();
          OMS_TRACE(omsTrAlways, m_session->m_lcSink,
            "OMS_BeforeImageList::checkStores: " << found->m_oid 
            << ", class: " << className << " not stored but modified");

          // quick hack, TODO: remove MultiPurpose later?
          struct _DATA {
            OMS_ObjectId8 oid;
            const char  *name;

            _DATA(OMS_ObjectId8 &o, const char *n)
              : oid(o), name(n)
            {
            }
          };
          _DATA data(found->m_oid, className);
          short err;
          tsp00_Int4 datasize = sizeof(data);
          m_session->m_lcSink->MultiPurpose(m_verify, mm_object, &datasize,
            reinterpret_cast<unsigned char*>(&data), &err);
        }
      } 
      else {
        // Should not occur.
        // object must have been release by application, i.e. omsRelease of an
        // object, that has been read for update without omsStore, omsDelete
      }
    }
  }
}

/*----------------------------------------------------------------------*/

void OMS_BeforeImageList::insertBeforeImage (OmsObjectContainerPtr pObj, int subtransLvl)
{
  if (subtransLvl > 0) {
    OMS_ContainerEntry* pContainerInfo = pObj->GetContainerInfo(__MY_FILE__, __LINE__);
    OMS_DETAIL_TRACE(omsTrFlushCache, m_session->m_lcSink,
          "OMS_BeforeImageList::insertBeforeImage : subtransLvl=" << subtransLvl << " : " << pObj->m_oid 
          << ", class: " << pContainerInfo->GetClassEntry().GetClassName());
  
#ifdef _ASSERT_OMS
    // Check that there is not already an entry with the same oid in the same subtrans-level.
    // (If the oid is the same, then at least the corresponding contexts must be different. This
    // might happen if the same object is modified in different versions.
    {
      OmsObjectContainerPtr p = m_beforeImages[subtransLvl-1];
      while (p) {
        OMS_Context *pContext = *reinterpret_cast<OMS_Context**>(&p->m_pobj);
        _OMSASSERT (m_session, pObj->m_oid != p->m_oid || pContext != m_session->m_context);
        p = p->GetNext();
      }
    }
#endif

    // Allocate memory for the before-image and copy content of original object into the before-image
    OmsObjectContainer* pBeforeImage   = pContainerInfo->GetMemory(pObj->IsVerNewObject());   
    SAPDB_MemCopyNoCheck(pBeforeImage, pObj, pContainerInfo->GetClassEntry().GetTransientSize());  
    
    // Insert frame into list of before-images for the current sub-transaction level
    pBeforeImage->SetNext(m_beforeImages[subtransLvl-1]);
    pObj->setBeforeImage (subtransLvl);

    m_beforeImages[subtransLvl-1] = pBeforeImage;

    if (pObj->VarObjFlag()) {
      OMS_VarObjInfo* objInfo = REINTERPRET_CAST (OMS_VarObjInfo*, &pObj->m_pobj);
      objInfo->incRefCnt();
    }

    /* use the vtable ptr of the before image to store the context the object is from */
    *REINTERPRET_CAST(OMS_Context**, (&pBeforeImage->m_pobj)) = m_session->m_context;

    // Remember the pointer to the current object in the oid-cache.
    // This pointer is needed during a subtrans-commit to determine whether the object, which is
    // read from the oid-hash is the object for which this before-image was constructed. 
    // A mismatch might occur, if in a sub-transaction an object with the same key is created and 
    // deleted more than 255 times. (Overflow in the generation counter of the oid.)
    pBeforeImage->SetOriginalObject(pObj);
  }
}

/*----------------------------------------------------------------------*/

void OMS_BeforeImageList::insertNewBeforeImage (OmsObjectContainerPtr pObj, OMS_ContainerEntry* pContainerInfo, int subtransLvl) 
{
  if (subtransLvl > 0) {
    OMS_DETAIL_TRACE(omsTrFlushCache, m_session->m_lcSink,
          "OMS_BeforeImageList::insertNewBeforeImage : subtransLvl=" << subtransLvl << " : " << pObj->m_oid 
          << ", class: " << pContainerInfo->GetClassEntry().GetClassName());

#ifdef _ASSERT_OMS
    // Check that there is not already an entry with the same oid in the same subtrans-level.
    // (If the oid is the same, then at least the corresponding contexts must be different. This
    // might happen if the same object is modified in different versions.
    {
      OmsObjectContainerPtr p = m_beforeImages[subtransLvl-1];
      while (p) {
        OMS_Context *pContext = *reinterpret_cast<OMS_Context**>(&p->m_pobj);
        _OMSASSERT (m_session, pObj->m_oid != p->m_oid || pContext != m_session->m_context);
        p = p->GetNext();
      }
    }
#endif

    // Allocate memory for before-image. As this before image corresponds to a newly created object, 
    // no information from the body is needed, because in case of an rollback the object is completely
    // removed from the cache.
    OmsObjectContainerPtr pBeforeImage = m_session->m_context->GetContainerDir().GetMemory(/*size=*/sizeof(void*));
    SAPDB_MemCopyNoCheck(pBeforeImage, pObj, OmsObjectContainer::headerSize());  

    // Indicate, that this before image corresponds to a newly created object.
    pBeforeImage->MarkNew();
    pObj->setBeforeImage (subtransLvl);

    // Insert frame into list of before-images for the current sub-transaction level
    pBeforeImage->SetNext(m_beforeImages[subtransLvl-1]);
    m_beforeImages[subtransLvl-1] = pBeforeImage;

    // Use the vtable ptr of the before image to store the context the object is from 
    *REINTERPRET_CAST(OMS_Context**, (&pBeforeImage->m_pobj)) = m_session->m_context;

    // Remember the pointer to the current object in the oid-cache.
    // This pointer is needed during a subtrans-commit to determine whether the object, which is
    // read from the oid-hash is the object for which this before-image was constructed. 
    // A mismatch might occur, if in a sub-transaction an object with the same key is created and 
    // deleted more than 255 times. (Overflow in the generation counter of the oid.)
    pBeforeImage->SetOriginalObject(pObj);
  }
}

/*----------------------------------------------------------------------*/

void OMS_BeforeImageList::insertDropContainerBeforeImage (const OMS_ContainerHandle *pContainerHandle,
                                                          OMS_ContainerEntry        *pContainerEntry,
                                                          int                        subtransLvl) 
{
  if (subtransLvl > 1) {
    OMS_DETAIL_TRACE(omsTrFlushCache, m_session->m_lcSink,
      "OMS_BeforeImageList::insertDropContainerBeforeImage : subtransLvl=" << subtransLvl << " : " << *pContainerHandle);

    // Get empty frame
    OmsObjectContainerPtr pBeforeImage = m_session->m_context->GetContainerDir().GetMemory(/*size=*/sizeof(void*));

    // Set flags in the frame. 
    pBeforeImage->MarkDropContainer();
    // The container handle (8Byte) is stored in the field m_oid.
    SAPDB_MemCopyNoCheck(reinterpret_cast<OMS_ContainerHandle*>(&pBeforeImage->m_oid),
                         pContainerHandle,
                         sizeof(OMS_ContainerHandle));
    
    // Insert the entry into the before image chain
    pBeforeImage->SetNext(m_beforeImages[subtransLvl-1]);
    m_beforeImages[subtransLvl-1] = pBeforeImage;

    // use the vtable ptr of the before image to store the context the object is from
    *reinterpret_cast<OMS_Context**>(&pBeforeImage->m_pobj) = m_session->m_context;
  }
}

/*----------------------------------------------------------------------*/

void OMS_BeforeImageList::insertCreateContainerBeforeImage (const OMS_ContainerHandle *pContainerHandle,
                                                            OMS_ContainerEntry        *pContainerEntry,
                                                            int                        subtransLvl) 
{
  if (subtransLvl > 1) {  
    OMS_DETAIL_TRACE(omsTrFlushCache, m_session->m_lcSink,
      "OMS_BeforeImageList::insertCreateContainerBeforeImage : subtransLvl=" << subtransLvl << " : " << *pContainerHandle);

    // Get empty frame 
    OmsObjectContainerPtr pBeforeImage = m_session->m_context->GetContainerDir().GetMemory(/*size=*/sizeof(void*));

    // Set flags in the frame. 
    pBeforeImage->MarkCreateContainer();
    // The container handle (8Byte) is stored in the field m_oid.
    SAPDB_MemCopyNoCheck(reinterpret_cast<OMS_ContainerHandle*>(&pBeforeImage->m_oid),
                         pContainerHandle,
                         sizeof(OMS_ContainerHandle));
    
    // Insert the entry into the before image chain
    pBeforeImage->SetNext(m_beforeImages[subtransLvl-1]);
    m_beforeImages[subtransLvl-1] = pBeforeImage;

    // use the vtable ptr of the before image to store the context the object is from
    *reinterpret_cast<OMS_Context**>(&pBeforeImage->m_pobj) = m_session->m_context;
  }
}

