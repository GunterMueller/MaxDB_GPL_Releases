/*!
 * \file    LVC_BufferedObjKeyIterator.cpp
 * \author  MarkusS
 * \brief   buffered iterator for key range iterator
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
#include "liveCache/LVC_BufferedObjKeyIterator.hpp"
#include "liveCache/LVC_ObjKeyIterator.hpp"
#include "liveCache/LVC_Types.hpp"

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

void
LVC_BufferedObjKeyIterator::GotoNextObject (tgg00_TransContext    &Trans,
                                            const FileDir_FileNo &ObjFileNo,
                                            tsp00_Int4             KeyLen,
                                            void                  *pUpperKey,
                                            SAPDB_Bool             bInclusive,
                                            Msg_List              &errorList)
                                           
{
  ROUTINE_DBG_MEO00 ("LVC_BufferedObjKeyIterator::operator ++()");

  SAPDBERR_ASSERT_STATE (IsValid());

  if (!m_pKeyOIdQueue->IsEmpty())
    m_pKeyOIdQueue->DelTopEntry();

  if (m_pKeyOIdQueue->IsEmpty() && !m_NoMoreOIds)
  {
    tgg00_BasisError      &TrError        = Trans.trError_gg00;

    // create an iterator to access the index tree and
    // position Iterator on object with the smallest key
    SAPDB_Bool bStartKeyFound;                          // unused parameter
    LVC_ObjKeyIterator 	ObjKeyIterator (Trans, m_ObjKeyFileId, KeyLen, bInclusive, 
        &m_NextStartKey, pUpperKey, bStartKeyFound, ASCENDING_KEY_ORDER_BD510);
				
    int i=0;
		while (e_ok == TrError)
		{
			if (ObjKeyIterator.IsReady())
			{
				m_NoMoreOIds = true;
				break;
			}
			else
			{
				if(m_pKeyOIdQueue->IsFull())
				{
					/* prepare start key for next iteration */
					memcpy (&m_NextStartKey, ObjKeyIterator.GetKeyPtr(), KeyLen);
					break;
				}
				else
				{	
					/* insert oid into buffer */
					if (m_bStoreKeys)
						m_pKeyOIdQueue->Append (ObjKeyIterator.GetOId(), ObjKeyIterator.GetKeyPtr());
					else
						m_pKeyOIdQueue->Append (ObjKeyIterator.GetOId());
					++ObjKeyIterator;
				}
			}
		}
	}
}

/*---------------------------------------------------------------------------*/

void
LVC_BufferedObjKeyIterator::GotoPrevObject (tgg00_TransContext    &Trans,
                                            const FileDir_FileNo &ObjFileNo,
                                            tsp00_Int4             KeyLen,
                                            void                  *pLowerKey,
                                            SAPDB_Bool             bInclusive,
                                            Msg_List              &errorList)
{
	ROUTINE_DBG_MEO00 ("LVC_BufferedObjKeyIterator::operator --()");

   	SAPDBERR_ASSERT_STATE (IsValid());

	if (!m_pKeyOIdQueue->IsEmpty())
         m_pKeyOIdQueue->DelTopEntry();

	if (m_pKeyOIdQueue->IsEmpty() && !m_NoMoreOIds)
	{
		tgg00_BasisError      &TrError        = Trans.trError_gg00;

    // create an iterator to access the index tree and
    // position Iterator on object with the largest key
    SAPDB_Bool bStartKeyFound; // unused parameter
    LVC_ObjKeyIterator 	ObjKeyIterator (Trans, m_ObjKeyFileId, KeyLen, bInclusive, 
      &m_NextStartKey, pLowerKey, bStartKeyFound, !ASCENDING_KEY_ORDER_BD510);

    while (e_ok == TrError)
    {
      if (ObjKeyIterator.IsReady())
      {
        m_NoMoreOIds = true;
        break;
      }
      else
      {
        if(m_pKeyOIdQueue->IsFull())
        {
          /* prepare start key for next iteration */
          memcpy (&m_NextStartKey, ObjKeyIterator.GetKeyPtr(), KeyLen);
          break;
        }
        else
        {	
          /* insert oid into buffer */
          if (m_bStoreKeys)
            m_pKeyOIdQueue->Append (ObjKeyIterator.GetOId(), ObjKeyIterator.GetKeyPtr());
          else
            m_pKeyOIdQueue->Append (ObjKeyIterator.GetOId());

          // As bAscendingKeyOrder == !ASCENDING_KEY_ORDER_BD510, operator++ delivers
          // previous object 
          ++ObjKeyIterator;
        }
      }
    }
  }
}

/*---------------------------------------------------------------------------*/

LVC_BufferedObjKeyIterator::LVC_BufferedObjKeyIterator (tgg00_TransContext     &Trans,
                                                        Msg_List               &errorList,
                                                        const FileDir_FileNo   &ObjFileNo,
                                                        tsp00_Int4              ObjKeyFileIndex,
				                 						tsp00_Int4	            KeyLen,
								                 		void		           *pStartKey,
                										void                   *pLowerKey,    // PTS 1119480
                										void                   *pUpperKey,    // PTS 1119480
                										SAPDB_Bool              bStoreKeys,
										                SAPDBMem_IRawAllocator &Allocator,
                                                        bool                    bAscendingKeyOrder)   // PTS 1119480
:
m_Allocator            (Allocator),
m_ObjKeyFileIndex      (ObjKeyFileIndex),
m_CheckNumber          (m_StaticCheckNumber), 
m_bStoreKeys           (bStoreKeys),
m_NoMoreOIds           (false)
{
  memcpy (&m_NextStartKey, pStartKey,  KeyLen);
  const SAPDB_UInt cNumQueueElements = 20;

  tgg00_BasisError   &TrError = Trans.trError_gg00;
  FileDir_IDirectory &IFDC    = FileDir_IDirectory::Instance(Trans.trIndex_gg00);
  FileDir_OmsKey      fd_smart;

  FileDir_ReturnCode  fd_rc = IFDC.GetFile(Trans.trTaskId_gg00, ObjFileNo, fd_smart, errorList);
  if (FileDir_Okay != fd_rc) {
      TrError = e_file_not_found;
      errorList.AppendNewMessage (Msg_List(
          Msg_List::Error,
          SDBMSG_LIVECACHE_LVC_ITERATOR_ON_REMOVED_FILE,
          Msg_Arg("FILENO",ObjFileNo.GetUInt8())
          ));
      RTE_Message (errorList);
      return;
  }

  /* fill oid queue with new OIds from the index tree */
  FileDir_FileNo KeyFileno = fd_smart.GetKeyPartition(m_ObjKeyFileIndex + 1);
  FileDir_OmsKeyPartition    fd_keypart_smart;
  fd_rc = IFDC.GetFile(Trans.trTaskId_gg00, KeyFileno, fd_keypart_smart, errorList);
  if (FileDir_Okay != fd_rc) {
      TrError = e_wrong_obj_file_id;
      errorList.AppendNewMessage (Msg_List(
          Msg_List::Error,
          SDBMSG_LIVECACHE_LVC_ITERATOR_ON_REMOVED_FILE,
          Msg_Arg("FILENO",KeyFileno.GetUInt8())
          ));
      RTE_Message (errorList);
      return;
  }     
  fd_keypart_smart.BuildFileId(m_ObjKeyFileId);

  if (bStoreKeys)
    m_pKeyOIdQueue = new (m_Allocator) LVC_KeyOIdQueue (cNumQueueElements, KeyLen , m_Allocator);
  else
    m_pKeyOIdQueue = new (m_Allocator) LVC_KeyOIdQueue (cNumQueueElements, 0 , m_Allocator);

  /* check if memory allocation succeeded */
  if (NULL == m_pKeyOIdQueue) 
  {
    Trans.trError_gg00 = e_no_more_memory;
    return;
  }

  if (bAscendingKeyOrder){  // PTS 1119480
    /* set iterator onto first object */
    GotoNextObject (Trans, ObjFileNo, KeyLen, pUpperKey, true, errorList);
  }
  else {
    /* set iterator onto last object */
    GotoPrevObject (Trans, ObjFileNo, KeyLen, pLowerKey, true, errorList);
  }
}

/*---------------------------------------------------------------------------*/
// PTS 1119480
void LVC_BufferedObjKeyIterator::Restart (tgg00_TransContext&     Trans,    
                                          Msg_List               &errorList,
                                          const FileDir_FileNo   &ObjFileNo,
                                          tsp00_Int4              KeyLen,
                                          void*                   pRestartKey,
                                          void*                   pLowerKey,
                                          void*                   pUpperKey,
                                          bool                    bAscendingKeyOrder)
{
  memcpy (&m_NextStartKey, pRestartKey,  KeyLen);
  m_NoMoreOIds = false;
	m_pKeyOIdQueue->Reset();

  if (bAscendingKeyOrder){  
    /* set iterator onto first object */
	  GotoNextObject (Trans, ObjFileNo, KeyLen, pUpperKey, false, errorList);
  }
  else {
	  /* set iterator onto last object */
	  GotoPrevObject (Trans, ObjFileNo, KeyLen, pLowerKey, false, errorList);
  }
}

/*---------------------------------------------------------------------------*/

const tsp00_Int4  LVC_BufferedObjKeyIterator::m_StaticCheckNumber = 815;

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
