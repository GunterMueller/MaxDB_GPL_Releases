/*!
 * \file    LVC_MultiFileObjKeyIterator.hpp
 * \author  MarkusS
 * \brief   iterator to merge the different files of partitioned keys 
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


#ifndef LVC_MULTIFILEOBJKEYITERATOR_HPP
#define LVC_MULTIFILEOBJKEYITERATOR_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "liveCache/LVC_BufferedObjKeyIterator.hpp"
#include "liveCache/LVC_AbstractObjKeyIterator.hpp"
#include "ggg200.h"
#include "ggg92.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define STORE_KEYS true

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

typedef  LVC_BufferedObjKeyIterator*   LVC_BufferedObjKeyIteratorPtr;

/*---------------------------------------------------------------------------*/

class LVC_ComparatorForIterators : public cgg200ComparisonOperator <LVC_BufferedObjKeyIteratorPtr>
{
public:

   /*!
     function: gg200Compare

     description:
       Compares two elements.

     returns:
       'l_less' if elem1 < elem2,
       'l_greater' if elem1 > elem2,
       'l_equal' if elem1 == elem2
    */
   virtual tsp00_LcompResult_Enum 
	   gg200Compare (const LVC_BufferedObjKeyIteratorPtr &pObjKeyIterator1,
	                 const LVC_BufferedObjKeyIteratorPtr &pObjKeyIterator2)
   {
	    int result = memcmp (pObjKeyIterator1->GetKeyPtr(), pObjKeyIterator2->GetKeyPtr(), m_KeyLen);
      if (result < 0)	return (m_bAscendingKeyOrder ? l_less : l_greater);

		/* duplicate keys are not allowed in the liveCache */
	   	SAPDBERR_ASSERT_STATE (result != 0);
		
      return (m_bAscendingKeyOrder ? l_greater : l_less);
   }

   LVC_ComparatorForIterators (SAPDB_UInt KeyLen, SAPDB_Bool bAscendingKeyOrder)
	   :m_KeyLen (KeyLen)
     ,m_bAscendingKeyOrder (bAscendingKeyOrder)
   {}

   void SetDirection (SAPDB_Bool bAscendingKeyOrder) 
   { m_bAscendingKeyOrder = bAscendingKeyOrder; }

private:
	SAPDB_UInt m_KeyLen;
  SAPDB_Bool m_bAscendingKeyOrder;
};
			
/*!*****************************************************************************

   class:        LVC_MultiFileObjKeyIterator
   description:  This class implements a key range iterator for object files
   
*******************************************************************************/

class LVC_MultiFileObjKeyIterator : public LVC_AbstractObjKeyIterator, 
                                    public cgg200_MergeSource <LVC_BufferedObjKeyIteratorPtr>
{
public:
  /*!-------------------------------------------------------------------------
  function:     GetNextOIdsWithKey()
  description:  see LVC_AbstractObjKeyIterator::GetNextOIds
  New function since: PTS 1134856 
  --------------------------------------------------------------------------*/
  virtual void GetNextOIdsWithKey(void            *pRestartKey,
                                  tgg92_KernelOid *pOId, 
                                  tsp00_Int4      &noOfOid, 
                                  void            *pKey,
                                  int             &KeySize,
                                  SAPDB_Bool       bInclusive,
                                  Msg_List        &errorList) 
  {
    int maxNoOfOId = noOfOid;
    noOfOid    = 0;
    while (!IsEmpty() && noOfOid < maxNoOfOId && (noOfOid*m_KeyLen + m_KeyLen) < KeySize){
      memcpy(reinterpret_cast<unsigned char*>(pKey)+noOfOid*m_KeyLen, (*m_MergeIterator)->GetKeyPtr(), m_KeyLen);
      pOId[noOfOid++] = (*m_MergeIterator)->GetOId();
      ++m_MergeIterator;
    }
    KeySize = m_KeyLen;
  };

  /*!-------------------------------------------------------------------------
	function:     GetNextOIds;
	description:  see LVC_AbstractObjKeyIterator::GetNextOIds
  --------------------------------------------------------------------------*/
  virtual void GetNextOIds (void            *pRestartKey,
                            tgg92_KernelOid *pOId, 
                            tsp00_Int4      &noOfOid,
                            SAPDB_Bool       bInclusive,
                            Msg_List        &errorList) 
  {
    int maxNoOfOId = noOfOid;
    noOfOid    = 0;
    while (!IsEmpty() && noOfOid < maxNoOfOId){
      pOId[noOfOid++] = (*m_MergeIterator)->GetOId();
      ++m_MergeIterator;
    }
  }

	/*!-------------------------------------------------------------------------
	function:     GetNextOIds;
	description:  see LVC_AbstractObjKeyIterator::GetNextOIds
  --------------------------------------------------------------------------*/
  virtual void GetPrevOIds (void            *pRestartKey,
                            tgg92_KernelOid *pOId, 
                            tsp00_Int4      &noOfOid,
                            SAPDB_Bool       bInclusive,
                            Msg_List        &errorList) 
{
    int maxNoOfOId = noOfOid;
    noOfOid    = 0;
    while (!IsEmpty() && noOfOid < maxNoOfOId){
      pOId[maxNoOfOId-(noOfOid+1)] = (*m_MergeIterator)->GetOId();
      ++noOfOid;
      // As bAscendingKeyOrder == !ASCENDING_KEY_ORDER_BD510, operator++ delivers
      // previous object 
      ++m_MergeIterator;
    }


    if (noOfOid < maxNoOfOId){
      // Shift entries in the array to the beginning of the array
      int i;
      for (i=0; i<noOfOid; ++i){
        pOId[i] = pOId[i+maxNoOfOId-noOfOid];
      }
      for (i=noOfOid; i<maxNoOfOId; ++i){
        pOId[i].gg92SetNil();
      }
    }
  }

	/*!-------------------------------------------------------------------------
	function:     GetNextOIds;
	description:  see LVC_AbstractObjKeyIterator::GetNextOIds
  --------------------------------------------------------------------------*/
	virtual void ChangeDirection(void* pRestartKey, SAPDB_Bool bAscendingKeyOrder, Msg_List &errorList)  
	{
    for (SAPDB_UInt ObjKeyFileIndex=0; ObjKeyFileIndex<m_NumObjKeyFiles; ++ObjKeyFileIndex){
      m_ppBufferdObjKeyIterator[ObjKeyFileIndex]->Restart(m_Trans,
                                                          errorList, 
                                                          m_ObjFileId.fileDirFileId_gg00(),
                                                          m_KeyLen,
                                                          pRestartKey,
                                                         &m_LowerKey,
                                                         &m_UpperKey,
                                                          bAscendingKeyOrder); 
    }
    SAPDB_UInt WorkingSpaceSize = m_NumObjKeyFiles * sizeof(cgg200_MergeNode<LVC_BufferedObjKeyIteratorPtr>);

    m_MergeComperator.SetDirection(bAscendingKeyOrder);
    m_MergeIterator.gg200Assign(*this, m_pMergeWorkingSpace, WorkingSpaceSize);

    m_bAscendingKeyOrder = bAscendingKeyOrder;
	}

	/*!-------------------------------------------------------------------------
	function:     IsEmpty()
	description:  returns true if the end of the iterator scope is reached
		   
	arguments:    none
	return value: (SAPDB_Bool) true if the end of the iterator scope is reached
  --------------------------------------------------------------------------*/
  virtual SAPDB_Bool  IsEmpty()
	{
		return m_MergeIterator.IsReady();
	}	

	/*!-------------------------------------------------------------------------
	function:     constructor()
	description:  
		   
	arguments:    none
	return value: none
  --------------------------------------------------------------------------*/
	LVC_MultiFileObjKeyIterator (tgg00_TransContext      &Trans,
                                 Msg_List                &errorList, 
    		                     tgg00_FileId            &ObjFileId,
                                 tsp00_Int4               NumObjKeyFiles,
		                         tsp00_Int4	              KeyLen,
		                         void		             *pStartKey,
		                         void		             *pLowerKey,           // PTS 1119480
		                         void		             *pUpperKey,           // PTS 1119480
		                         SAPDBMem_IRawAllocator  &Allocator,
                                 SAPDB_Bool                 bAscendingKeyOrder)  // PTS 1119480
	:LVC_AbstractObjKeyIterator (Trans, errorList, ObjFileId, KeyLen, pLowerKey, pUpperKey, bAscendingKeyOrder),
   m_Allocator       (Allocator),
	 m_NumObjKeyFiles  (NumObjKeyFiles),
	 m_MergeComperator (KeyLen, bAscendingKeyOrder),
   m_MergeIterator   (m_MergeComperator)	 
	{
		newarray(m_ppBufferdObjKeyIterator, m_NumObjKeyFiles, m_Allocator);
		
		/* check if memory allocation succeeded */
		if (NULL == m_ppBufferdObjKeyIterator){
			Trans.trError_gg00 = e_no_more_memory;
			return;
		}
		
    for (SAPDB_UInt ObjKeyFileIndex=0; ObjKeyFileIndex<m_NumObjKeyFiles; ++ObjKeyFileIndex)
		{
        m_ppBufferdObjKeyIterator[ObjKeyFileIndex]= new (m_Allocator)  
            LVC_BufferedObjKeyIterator  (Trans,
            errorList,
            m_ObjFileId.fileDirFileId_gg00(),
            ObjKeyFileIndex, KeyLen, 
            pStartKey, pLowerKey, pUpperKey,
            STORE_KEYS, Allocator, bAscendingKeyOrder);
			
			/* check if memory allocation succeeded */
			if (NULL == m_ppBufferdObjKeyIterator[ObjKeyFileIndex]){
				Trans.trError_gg00 = e_no_more_memory;
				return;
			}
		}

    SAPDB_UInt WorkingSpaceSize = m_NumObjKeyFiles * sizeof(cgg200_MergeNode<LVC_BufferedObjKeyIteratorPtr>);
    m_pMergeWorkingSpace        = Allocator.Allocate (WorkingSpaceSize);

		if (NULL == m_pMergeWorkingSpace){
			Trans.trError_gg00 = e_no_more_memory;
			return;
		}

		m_MergeIterator.gg200Assign(*this, m_pMergeWorkingSpace, WorkingSpaceSize);
	}

	/*!-------------------------------------------------------------------------
	function:     destructor()
	description:  
		   
	arguments:    none
	return value: none
  --------------------------------------------------------------------------*/
  ~LVC_MultiFileObjKeyIterator()
	{
    for (SAPDB_UInt ObjKeyFileIndex=0; ObjKeyFileIndex<m_NumObjKeyFiles; ++ObjKeyFileIndex){
      destroy (m_ppBufferdObjKeyIterator[ObjKeyFileIndex], m_Allocator);
    }
    destroyarray(m_ppBufferdObjKeyIterator, m_NumObjKeyFiles, m_Allocator);
                 m_Allocator.Deallocate (m_pMergeWorkingSpace);
	}

	/*!-------------------------------------------------------------------------
    function: gg200GetListCount

    description: Returns the number of lists that should be merged.
  --------------------------------------------------------------------------*/
  virtual tsp00_Uint4 gg200GetListCount()
	{
		return m_NumObjKeyFiles;
	}

	/*!-------------------------------------------------------------------------
     function: gg200GetCurrentListElement

     description:
     Yields the current element from the specified list,
     if there is one.

     arguments:
       target   [out]  Target where element should be copied to.
       numList  [in]   Specifies the list from which to take
                       the element. Counting starts at 0.

     returns:        'True', if and only if an element has been
                     supplied.
  --------------------------------------------------------------------------*/
  virtual bool gg200GetCurrentListElement (unsigned int                  ObjKeyFileIndex,
	                                        LVC_BufferedObjKeyIteratorPtr &pObjKeyIterator)
  {
		pObjKeyIterator = m_ppBufferdObjKeyIterator[ObjKeyFileIndex];
    return (!pObjKeyIterator->IsReady()); 
	}

	/*!-------------------------------------------------------------------------
     function: gg200GotoNextListElement

     description:
     Next element on the list becomes current element.

     arguments:
     numList  [in]   Specifies the list from which to take
       the element. Counting starts at 0.

  --------------------------------------------------------------------------*/
  virtual void gg200GotoNextListElement (unsigned int ObjKeyFileIndex)
  {
    Msg_List ErrorList;
    if (m_bAscendingKeyOrder) {
        m_ppBufferdObjKeyIterator[ObjKeyFileIndex]->GotoNextObject(m_Trans, 
            m_ObjFileId.fileDirFileId_gg00(),
            m_KeyLen, &m_UpperKey, true, ErrorList);
    }
    else {
        m_ppBufferdObjKeyIterator[ObjKeyFileIndex]->GotoPrevObject(m_Trans, 
            m_ObjFileId.fileDirFileId_gg00(),
            m_KeyLen, &m_LowerKey, true, ErrorList);
    }
	}

	/*-------------------------------------------------------------------------*/

private: // member

	LVC_BufferedObjKeyIteratorPtr     *m_ppBufferdObjKeyIterator;
	SAPDBMem_IRawAllocator            &m_Allocator;
	SAPDB_UInt                         m_NumObjKeyFiles;
	void                              *m_pMergeWorkingSpace;         
	gg200MergeIterator <LVC_BufferedObjKeyIteratorPtr, LVC_ComparatorForIterators> m_MergeIterator; 
    LVC_ComparatorForIterators         m_MergeComperator; 	
};

/*!*****************************************************************************

   endclass: LVC_MultiFileObjKeyIterator

*******************************************************************************/


#endif  /* LVC_MULTIFILEOBJKEYITERATOR_HPP */
