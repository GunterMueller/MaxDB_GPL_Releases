/*!
 * \file    LVC_SingleFileObjKeyIterator.cpp
 * \author  MarkusS
 * \brief   key range iterator to access non-partitioned key objects
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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg01.h"
#include "gbd900.h"
#include "hbd03.h"
#include "RunTime/RTE_Message.hpp"
#include "liveCache/LVC_ObjKeyIterator.hpp"
#include "liveCache/LVC_Types.hpp"
#include "liveCache/LVC_SingleFileObjKeyIterator.hpp"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_OmsKey.hpp"
#include "FileDirectory/FileDir_OmsKeyPartition.hpp"
#include "Messages/Msg_List.hpp"
#include "SAPDBCommon/Messages/SDBMsg_LiveCache.h"

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/
// New function since: PTS 1134856 
void 
LVC_SingleFileObjKeyIterator::GetNextOIdsWithKey(void            *pRestartKey,
                                                 tgg92_KernelOid *pOId, 
                                                 tsp00_Int4      &noOfOid, 
                                                 void            *pKey,
                                                 int             &KeySize,
                                                 SAPDB_Bool       bInclusive,
                                                 Msg_List         &errorList)
{
  ROUTINE_DBG_MEO00 ("LVC_SingleFileObjKeyIterator::GetNextOIdsWithKey");

  SAPDBERR_ASSERT_STATE (IsValid());

  if (!m_NoMoreOIds)
  {
    tgg00_BasisError      &TrError        = m_Trans.trError_gg00;

    /* create an iterator to access the index tree */
    SAPDB_Bool bStartKeyFound;                          // unused parameter
    LVC_ObjKeyIterator 	ObjKeyIterator (m_Trans, m_ObjKeyFileId, m_KeyLen, bInclusive, 
      pRestartKey ? pRestartKey : &m_LastKey[0], 
      &m_UpperKey, bStartKeyFound, ASCENDING_KEY_ORDER_BD510);

    void* pLastKey = NULL;
    tsp00_Int4 maxNoOfOid = noOfOid;
    noOfOid = 0;
    while (e_ok == TrError && noOfOid < maxNoOfOid 
      && (noOfOid*m_KeyLen + m_KeyLen) < KeySize && !ObjKeyIterator.IsReady())
    {
      /* Append OID to Output */
      memcpy(reinterpret_cast<unsigned char*>(pKey)+noOfOid*m_KeyLen, ObjKeyIterator.GetKeyPtr(), m_KeyLen);
      pOId[noOfOid++] = ObjKeyIterator.GetOId();
      pLastKey = ObjKeyIterator.GetKeyPtr();
      ++ObjKeyIterator;
    }
    if (pLastKey != NULL){
      // Remember key, that has been read last. This is needed as there might be more subsequent
      // invalid objects (objects which do not belong to the consistent view or objects which 
      // have been modified in an unloaded version) than there is space to store oids in the 
      // return buffer. In this case the pRestartKey will never change and this would result in
      // an infinite loop. Therefore in such a case the pRestartKey is set to NULL by the higher 
      // layers and the member m_LastKey is used instead 
      memcpy (&m_LastKey[0], pLastKey, m_KeyLen);
    }

    if (ObjKeyIterator.IsReady()){
      // Iterator has delivered all its data
      m_NoMoreOIds = true;
    }
  }
  KeySize = m_KeyLen;
};

/*---------------------------------------------------------------------------*/

void
LVC_SingleFileObjKeyIterator::GetNextOIds (void             *pRestartKey,
                                           tgg92_KernelOid  *pOId, 
                                           tsp00_Int4       &noOfOid,
                                           SAPDB_Bool        bInclusive,
                                           Msg_List         &errorList)
{
  ROUTINE_DBG_MEO00 ("LVC_SingleFileObjKeyIterator::GetNextOIds");

  SAPDBERR_ASSERT_STATE (IsValid());

  if (m_NoMoreOIds){
    noOfOid = 0;
  }
  else {
    tgg00_BasisError &TrError = m_Trans.trError_gg00;

    if (noOfOid == 1){
      // buffer oids in the class, so not not each call for an oid results in a search in the key-tree
   
      noOfOid = 0;

      ++m_idx;
      if (m_idx < m_maxIdx){
        // There is still an oid in the buffer, which can be returned without creating a new iterator
        pOId[0] = m_pOids[m_idx];
        noOfOid = 1;
      }
      else if (m_allReadFromTree){
        // All oids have been read from the tree and the buffer is empty
        m_NoMoreOIds = true;
      }
      else {
        // create an iterator to access the index tree 
        SAPDB_Bool bStartKeyFound;                          
        LVC_ObjKeyIterator 	ObjKeyIterator (m_Trans, m_ObjKeyFileId, m_KeyLen, bInclusive, 
          pRestartKey ? pRestartKey : &m_LastKey[0], 
          &m_UpperKey, bStartKeyFound, ASCENDING_KEY_ORDER_BD510);

        void* pLastKey = NULL;
        m_maxIdx = 0;
        m_idx    = -1;
        while (e_ok == TrError && m_maxIdx < OID_BUFFER_SIZE && !ObjKeyIterator.IsReady()){
          // Append OID to Output 
          m_pOids[m_maxIdx] = ObjKeyIterator.GetOId();
          pLastKey = ObjKeyIterator.GetKeyPtr();
          ++ObjKeyIterator;
          ++m_maxIdx;
        }

        if (m_maxIdx > 0){
          // At least one object has been inserted into the buffer
          m_idx   = 0;
          pOId[0] = m_pOids[m_idx];
          noOfOid = 1;
        }
 
        if (pLastKey != NULL){
          // Remember key, that has been read last. This is needed as there might be more subsequent
          // invalid objects (objects which do not belong to the consistent view or objects which 
          // have been modified in an unloaded version) than there is space to store oids in the 
          // return buffer. In this case the pRestartKey will never change and this would result in
          // an infinite loop. Therefore in such a case the pRestartKey is set to NULL by the higher 
          // layers and the member m_LastKey is used instead 
          memcpy (&m_LastKey[0], pLastKey, m_KeyLen);
        }

        if (ObjKeyIterator.IsReady()){
          // Iterator has delivered all its data
          m_allReadFromTree = true;
        }          
      }
    }
    else {
      // Prevent infinite loop if buffer-size equals 1 and the object is filtered out
      // in bd90NextObjFromKeyIterator. Otherwise the same object would be read ever
      // and ever again, as the restart-key is not incremented and the 'inclusive'-
      // flag equals true (This flag is set to true only during the creation of the
      // iterator).
      if (noOfOid == 1 && pRestartKey == NULL && bInclusive == true){
        bInclusive = false;
      }

      /* create an iterator to access the index tree */
      SAPDB_Bool bStartKeyFound;                          // unused parameter
      LVC_ObjKeyIterator 	ObjKeyIterator (m_Trans, m_ObjKeyFileId, m_KeyLen, bInclusive, 
        pRestartKey ? pRestartKey : &m_LastKey[0], 
        &m_UpperKey, bStartKeyFound, ASCENDING_KEY_ORDER_BD510);

      void* pLastKey = NULL;
      tsp00_Int4 maxNoOfOid = noOfOid;
      noOfOid = 0;
      while (e_ok == TrError && noOfOid < maxNoOfOid && !ObjKeyIterator.IsReady()){
        /* Append OID to Output */
        pOId[noOfOid++] = ObjKeyIterator.GetOId();
        pLastKey = ObjKeyIterator.GetKeyPtr();
        ++ObjKeyIterator;
      }
      if (pLastKey != NULL){
        // Remember key, that has been read last. This is needed as there might be more subsequent
        // invalid objects (objects which do not belong to the consistent view or objects which 
        // have been modified in an unloaded version) than there is space to store oids in the 
        // return buffer. In this case the pRestartKey will never change and this would result in
        // an infinite loop. Therefore in such a case the pRestartKey is set to NULL by the higher 
        // layers and the member m_LastKey is used instead 
        memcpy (&m_LastKey[0], pLastKey, m_KeyLen);
      }

      if (ObjKeyIterator.IsReady()){
        // Iterator has delivered all its data
        m_NoMoreOIds = true;
      }
    }
  }
}

/*---------------------------------------------------------------------------*/

void
LVC_SingleFileObjKeyIterator::GetPrevOIds (void             *pRestartKey,
                                           tgg92_KernelOid  *pOId, 
                                           tsp00_Int4       &noOfOid,
                                           SAPDB_Bool        bInclusive,
                                           Msg_List         &errorList)
{
  ROUTINE_DBG_MEO00 ("LVC_SingleFileObjKeyIterator::GetPrevOIds");

  SAPDBERR_ASSERT_STATE (IsValid());

  if (m_NoMoreOIds){
    noOfOid = 0;
  }
  else {
    tgg00_BasisError      &TrError        = m_Trans.trError_gg00;

    if (noOfOid == 1){
      // buffer oids in the class, so not not each call for an oid results in a search in the key-tree

      noOfOid = 0;

      --m_idx;
      if (m_idx >= 0){
        // There is still an oid in the buffer, which can be returned without creating a new iterator
        pOId[0] = m_pOids[m_idx];
        noOfOid = 1;
      }
      else if (m_allReadFromTree){
        // All oids have been read from the tree and the buffer is empty
        m_NoMoreOIds = true;
      }
      else {
        SAPDB_Bool bStartKeyFound; // unused parameter
        LVC_ObjKeyIterator 	ObjKeyIterator (m_Trans, m_ObjKeyFileId, m_KeyLen, bInclusive, 
          pRestartKey ? pRestartKey : &m_LastKey[0],
          &m_LowerKey, bStartKeyFound, !ASCENDING_KEY_ORDER_BD510);

        void* pLastKey = NULL;
        m_maxIdx = 0;
        m_idx    = -1;
        while (e_ok == TrError && m_maxIdx < OID_BUFFER_SIZE  && !ObjKeyIterator.IsReady()){
          // Append OID to Output (Fill array starting from the end)
          m_pOids[OID_BUFFER_SIZE-(m_maxIdx+1)] = ObjKeyIterator.GetOId();
          pLastKey = ObjKeyIterator.GetKeyPtr();

          // As bAscendingKeyOrder == !ASCENDING_KEY_ORDER_BD510, operator++ delivers previous object 
          ++ObjKeyIterator;
          ++m_maxIdx;
        }

        if (m_maxIdx > 0){
          // At least one object has been inserted into the buffer
          if (m_maxIdx < OID_BUFFER_SIZE){
            // Buffer is not filled completely, so shift it to the left.
            int i;
            for (i=0; i<m_maxIdx; ++i){
              m_pOids[i] = m_pOids[i+OID_BUFFER_SIZE-m_maxIdx];
            }
            //for (i=m_maxIdx; i<OID_BUFFER_SIZE; ++i){
            //  m_pOids[i].gg92SetNil();
            //}
          }

          m_idx   = m_maxIdx-1;
          pOId[0] = m_pOids[m_idx];
          noOfOid = 1;
        }

        if (pLastKey != NULL){
          // Remember key, that has been read last. This is needed as there might be more subsequent
          // invalid objects (objects which do not belong to the consistent view or objects which 
          // have been modified in an unloaded version) than there is space to store oids in the 
          // return buffer. In this case the pRestartKey will never change and this would result in
          // an infinite loop. Therefore in such a case the pRestartKey is set to NULL by the higher 
          // layers and the member m_LastKey is used instead 
          memcpy (&m_LastKey[0], pLastKey, m_KeyLen);
        }

        if (ObjKeyIterator.IsReady()){
          // Iterator has delivered all its data
          m_allReadFromTree = true;
        }
      }
    }
    else {
      // Prevent infinite loop if buffer-size equals 1 and the object is filtered out
      // in bd90NextObjFromKeyIterator. Otherwise the same object would be read ever
      // and ever again, as the restart-key is not incremented and the 'inclusive'-
      // flag equals true (This flag is set to true only during the creation of the
      // iterator).
      if (noOfOid == 1 && pRestartKey == NULL && bInclusive == true){
        bInclusive = false;
      }

      /* create an iterator to access the index tree */
      SAPDB_Bool bStartKeyFound; // unused parameter
      LVC_ObjKeyIterator 	ObjKeyIterator (m_Trans, m_ObjKeyFileId, m_KeyLen, bInclusive, 
          pRestartKey ? pRestartKey : &m_LastKey[0],
          &m_LowerKey, bStartKeyFound, !ASCENDING_KEY_ORDER_BD510);
      void* pLastKey = NULL;
      tsp00_Int4 maxNoOfOid = noOfOid;
      noOfOid = 0;
		  while (e_ok == TrError && noOfOid < maxNoOfOid && !ObjKeyIterator.IsReady()){
        // Append OID to Output (Fill array starting from the end)
        pOId[maxNoOfOid-(noOfOid+1)] = ObjKeyIterator.GetOId();
        pLastKey = ObjKeyIterator.GetKeyPtr();
        // As bAscendingKeyOrder == !ASCENDING_KEY_ORDER_BD510, operator++ delivers
        // previous object 
        ++noOfOid;
        ++ObjKeyIterator;
      }
      if (pLastKey != NULL){
        // Remember key, that has been read last. This is needed as there might be more subsequent
        // invalid objects (objects which do not belong to the consistent view or objects which 
        // have been modified in an unloaded version) than there is space to store oids in the 
        // return buffer. In this case the pRestartKey will never change and this would result in
        // an infinite loop. Therefore in such a case the pRestartKey is set to NULL by the higher 
        // layers and the member m_LastKey is used instead 
        memcpy (&m_LastKey[0], pLastKey, m_KeyLen);
      }

      if (noOfOid < maxNoOfOid){
        // If array is not filled completely, then shift the entries in the array 
        // to the left side
        int i;
        for (i=0; i<noOfOid; ++i){
          pOId[i] = pOId[i+maxNoOfOid-noOfOid];
        }
        for (i=noOfOid; i<maxNoOfOid; ++i){
          pOId[i].gg92SetNil();
        }
      }

      if (ObjKeyIterator.IsReady()){
        // Iterator has delivered all its data
        m_NoMoreOIds = true;
      }
    }
  }
}

/*---------------------------------------------------------------------------*/
LVC_SingleFileObjKeyIterator::LVC_SingleFileObjKeyIterator 
                             (tgg00_TransContext     &Trans,
                              Msg_List               &errorList,
                              tgg00_FileId           &ObjFileId,
                              tsp00_Int4              KeyLen,
                              void                   *pStartKey,
                              void                   *pLowerKey,          
                              void                   *pUpperKey,         
                              SAPDBMem_IRawAllocator &Allocator,
                              bool                    bAscendingKeyOrder) 
:
LVC_AbstractObjKeyIterator (Trans, errorList, ObjFileId, KeyLen, pLowerKey, pUpperKey, bAscendingKeyOrder),	 
m_Allocator                (Allocator),
m_ObjKeyFileIndex          (0),           // = index first key file 
m_ObjFileNo                (ObjFileId.fileDirFileId_gg00()),
m_CheckNumber              (m_StaticCheckNumber), 
m_NoMoreOIds               (false),
m_allReadFromTree          (false),
m_idx                      (-1),
m_maxIdx                   (0)
{
    tgg00_BasisError      &TrError        = m_Trans.trError_gg00;
    FileDir_IDirectory &IFDC  = FileDir_IDirectory::Instance(m_Trans.trIndex_gg00);
    FileDir_OmsKey     fd_smart;
    FileDir_ReturnCode  fd_rc = IFDC.GetFile(m_Trans.trTaskId_gg00, m_ObjFileNo, fd_smart, errorList);
    if (FileDir_Okay != fd_rc) {
      TrError = e_file_not_found;
      errorList.AppendNewMessage (Msg_List(
        Msg_List::Error,
        SDBMSG_LIVECACHE_LVC_ITERATOR_ON_REMOVED_FILE,
        Msg_Arg("FILENO",m_ObjFileNo.GetUInt8())
        ));
      RTE_Message (errorList);
      return;
    }

    /* fill oid queue with new OIds from the index tree */
    FileDir_FileNo KeyFileno = fd_smart.GetKeyPartition(m_ObjKeyFileIndex + 1);
    FileDir_OmsKeyPartition   fd_keypart_smart;
    fd_rc = IFDC.GetFile(m_Trans.trTaskId_gg00, KeyFileno, fd_keypart_smart, errorList);
    if (FileDir_Okay != fd_rc) {
      TrError = e_wrong_obj_file_id;
      errorList.AppendNewMessage (Msg_List(
        Msg_List::Error,
        SDBMSG_LIVECACHE_LVC_ITERATOR_ON_REMOVED_FILE,
        Msg_Arg("FILENO",m_ObjFileNo.GetUInt8())
        ));
      RTE_Message (errorList);
      return;
    }     
    fd_keypart_smart.BuildFileId(m_ObjKeyFileId);
}

/*---------------------------------------------------------------------------*/
// PTS 1119480
void LVC_SingleFileObjKeyIterator::ChangeDirection (void*     pRestartKey,
                                                    bool      bAscendingKeyOrder,
                                                    Msg_List &errorList)
{
  m_NoMoreOIds         = false;
  m_bAscendingKeyOrder = bAscendingKeyOrder;
  m_ObjKeyFileIndex    = 0;
}

/*---------------------------------------------------------------------------*/

const tsp00_Int4  LVC_SingleFileObjKeyIterator::m_StaticCheckNumber = 816;

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
