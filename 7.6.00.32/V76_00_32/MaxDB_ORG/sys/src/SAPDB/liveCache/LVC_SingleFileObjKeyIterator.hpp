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


*****************************************************************************/


#ifndef LVC_SINGLEFILEOBJKEYITERATOR_HPP
#define LVC_SINGLEFILEOBJKEYITERATOR_HPP

 

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "liveCache/LVC_AbstractObjKeyIterator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "liveCache/LVC_KeyOIdQueue.hpp"


/*!*****************************************************************************
   class:        LVC_SingleFileObjKeyIterator
   description:  This class implements a key range iterator for object files 
*******************************************************************************/
class LVC_SingleFileObjKeyIterator : public LVC_AbstractObjKeyIterator
{
public:
  /*!-------------------------------------------------------------------------
	function:     IsEmpty()
	description:  returns true if the end of the iterator scope is reached
		   
	arguments:    none
	return value: (SAPDB_Bool) true if the end of the iterator scope is reached
    --------------------------------------------------------------------------*/
  virtual SAPDB_Bool IsEmpty() 
	{
		return m_NoMoreOIds;
	}

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
                                  Msg_List        &errorList);

   /*!-------------------------------------------------------------------------
	function:     GetNextOIds();
	description:  see LVC_AbstractObjKeyIterator::GetNextOIds()
  --------------------------------------------------------------------------*/
	virtual void GetNextOIds(void          *pRestartKey, 
                           tgg92_KernelOid *pOId, 
                           tsp00_Int4      &noOfOid, 
                           SAPDB_Bool       bInclusive,
                           Msg_List        &errorList);

	/*!-------------------------------------------------------------------------
	function:     GetPrevOIds();
	description:  see LVC_AbstractObjKeyIterator::GetPrevOIds()
  --------------------------------------------------------------------------*/
	virtual void GetPrevOIds(void            *pRestartKey, 
                           tgg92_KernelOid *pOId, 
                           tsp00_Int4      &noOfOid, 
                           SAPDB_Bool       bInclusive,
                           Msg_List        &errorList);

	/*!-------------------------------------------------------------------------
	function:     ChangeDirection();
	description:  see LVC_AbstractObjKeyIterator::ChangeDirection()
  --------------------------------------------------------------------------*/
	virtual void ChangeDirection(void *m_pRestartKey, SAPDB_Bool bAscendingKeyOrder, Msg_List &errorList);

	/*!-------------------------------------------------------------------------
	function:     constructor()
	description:  
  --------------------------------------------------------------------------*/
	LVC_SingleFileObjKeyIterator (tgg00_TransContext     &Trans,
                                Msg_List               &errorList,
                                tgg00_FileId           &ObjFileId,
                                tsp00_Int4              KeyLen,
                                void                   *pStartKey,
                                void                   *pLowerKey,   
                                void                   *pUpperKey,       
                                SAPDBMem_IRawAllocator &Allocator,
                                bool                    bAscendingKeyOrder); 

	/*!-------------------------------------------------------------------------
	function:     destructor()
	description:  releases all resources and mark the iterator as destroyed via the checknumber
  --------------------------------------------------------------------------*/
	~LVC_SingleFileObjKeyIterator()
	{
	  SAPDBERR_ASSERT_STATE (IsValid());

		m_CheckNumber = 0;
	}

  /*!-------------------------------------------------------------------------*/

private: // member

  enum { OID_BUFFER_SIZE = 20 };

    SAPDBMem_IRawAllocator  &m_Allocator;
    tgg00_FileId             m_ObjKeyFileId;
    tsp00_Int4               m_ObjKeyFileIndex;
    FileDir_FileNo           m_ObjFileNo;
    tsp00_Int4               m_CheckNumber;
    SAPDB_Bool               m_NoMoreOIds;
    tsp00_Key                m_LastKey;             // PTS 1119480

    // If only a single objects is requested from the above layers (no mass-operation), then
    // a bunch of oids is read from the key-tree and buffered in the array m_pOids. 
    // Subsequent calls will read from this buffer and so the search in the tree for each single
    // request is prevented. m_idx points to the current position in this array and m_maxIdx
    // equals the number of objects which are stored in the buffer.
    // If all objects have been read from the key-tree, then m_allReadFromTree is set to 'true',
    // and after all objects have been read from the buffer, then m_NoMoreOIds is set to 'true'.
    SAPDB_Bool               m_allReadFromTree;
    tsp00_Int4               m_idx;
    tsp00_Int4               m_maxIdx;
    tgg92_KernelOid          m_pOids[OID_BUFFER_SIZE];


	static const tsp00_Int4  m_StaticCheckNumber;
};

/*!*****************************************************************************

   endclass: LVC_SingleFileObjKeyIterator

*******************************************************************************/


#endif  /* LVC_SINGLEFILEOBJKEYITERATOR_HPP */
