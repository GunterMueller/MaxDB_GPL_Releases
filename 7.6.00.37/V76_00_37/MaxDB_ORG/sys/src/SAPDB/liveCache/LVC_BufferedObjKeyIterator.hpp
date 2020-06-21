/*!
 * \file    LVC_BufferedObjKeyIterator.hpp
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


*****************************************************************************/


#ifndef LVC_BUFFEREDOBJKEYITERATOR_HPP
#define LVC_BUFFEREDOBJKEYITERATOR_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "liveCache/LVC_KeyOIdQueue.hpp"

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!*****************************************************************************

   class:        LVC_BufferedObjKeyIterator
   description:  This class implements a key range iterator for object files
   
	             

*******************************************************************************/

class LVC_BufferedObjKeyIterator
{
public:

	
	/*!-------------------------------------------------------------------------
    function:     GetOId()
    description:  dereferences the iterator,
	
	The OId of the object at which the iterator is currently pointing is retuirned 
	  
	arguments:    none
	return value: (tgg92_KernelOid) OId of the object at which the iterator is currently pointing
    --------------------------------------------------------------------------*/
	tgg92_KernelOid	GetOId() const
	{
		/* dont call this method if there is nothing to fetch */
		SAPDBERR_ASSERT_STATE (!m_pKeyOIdQueue->IsEmpty());
		
		return m_pKeyOIdQueue->GetTopOId();
	}

	/*!-------------------------------------------------------------------------
    function:     GetKeyPtr()
    description:  dereferences the iterator,
	
	The Key of the object at which the iterator is currently pointing is returned 
	  
	arguments:    none
	return value: (void *) Key of the object at which the iterator is currently pointing
    --------------------------------------------------------------------------*/
	void *	GetKeyPtr() const
	{
        /* keys can only be deliverd if they are stored */
		SAPDBERR_ASSERT_STATE (m_bStoreKeys);

		/* dont call this method if there is nothing to fetch */
		SAPDBERR_ASSERT_STATE (!m_pKeyOIdQueue->IsEmpty());

		return m_pKeyOIdQueue->GetTopKeyPtr();
	}

	/*!-------------------------------------------------------------------------
	function:     IsReady()
	description:  returns true if the end of the iterator scope is reached
		   
	arguments:    none
	return value: (SAPDB_Bool) true if the end of the iterator scope is reached
    --------------------------------------------------------------------------*/
    SAPDB_Bool  IsReady()
	{
		return m_pKeyOIdQueue->IsEmpty();
	}

	/*!-------------------------------------------------------------------------
	function:     GotoNextObject();
	description:  moves the iterator to the next object in the key range

		   
	arguments:    none
	return value: none
    --------------------------------------------------------------------------*/
    void GotoNextObject (tgg00_TransContext     &Trans,
                       const FileDir_FileNo &ObjFileNo,
                       tsp00_Int4              KeyLen,
                       void                   *pStopKey,
                       SAPDB_Bool              bInclusive,
                       Msg_List               &errorList);

 /*!-------------------------------------------------------------------------
	function:     GotoPrevObject();
	description:  moves the iterator to the previous object in the key range

		   
	arguments:    none
	return value: none
  New function since: PTS 1119480
  --------------------------------------------------------------------------*/
	void GotoPrevObject (tgg00_TransContext     &Trans,
                       const FileDir_FileNo &ObjFileNo,
                       tsp00_Int4              KeyLen,
                       void                   *pStopKey,
                       SAPDB_Bool              bInclusive,
                       Msg_List               &errorList);

	/*!-------------------------------------------------------------------------
	function:     Restart();
	description:  clears the current buffer and reloads the buffer starting at the given key and with given direction

		   
	arguments:    none
	return value: none
  New function since: PTS 1119480
  --------------------------------------------------------------------------*/
    void Restart (tgg00_TransContext&     Trans,    
        Msg_List               &errorList,
        const FileDir_FileNo   &ObjFileNo,
        tsp00_Int4              KeyLen,
        void*                   pRestartKey,
        void*                   pLowerKey,
        void*                   pUpperKey,
        bool                    bAscendingKeyOrder); 

  /*!-------------------------------------------------------------------------
	function:     IsValid;
	description:  returns true if the iterator fulfills some basic requirements of an iterator

          This function can be used to validate an iterator after a cast
				  from a void pointer. Thus this function reperesnts a simple
				  type check.
		   
	arguments:    none
	return value: (SAPDB_Bool) true if the iterator fulfills some basic requirements
    --------------------------------------------------------------------------*/
    SAPDB_Bool    IsValid()
	{
		return (m_CheckNumber == m_StaticCheckNumber);
	}

	/*!-------------------------------------------------------------------------
	function:     constructor()
	description:  
		   
	arguments:    none
	return value: none
    --------------------------------------------------------------------------*/
	LVC_BufferedObjKeyIterator (tgg00_TransContext     &Trans,
                                Msg_List               &errorList,
                                const FileDir_FileNo   &ObjFileNo,
		                        tsp00_Int4              ObjKeyFileIndex,
		                        tsp00_Int4	            KeyLen,
		                        void		           *pStartKey,
		                        void                   *pLowerKey,   // PTS 1119480
		                        void                   *pUpperKey,   // PTS 1119480
		                        SAPDB_Bool              bStoreKeys,
		                        SAPDBMem_IRawAllocator &Allocator,
                                bool                    bAscendingKeyOrder = true);  // PTS 1119480
	
	/*!-------------------------------------------------------------------------
	function:     destructor()
	description:  releases all resources and mark the iterator as destroyed via the checknumber
		   
	arguments:    none
	return value: none
    --------------------------------------------------------------------------*/
	~LVC_BufferedObjKeyIterator()
	{
	   	SAPDBERR_ASSERT_STATE (IsValid());

		m_CheckNumber = 0;
		destroy (m_pKeyOIdQueue, m_Allocator);
	}

private: // member

	SAPDBMem_IRawAllocator  &m_Allocator;
    tgg00_FileId             m_ObjKeyFileId;
	tsp00_Key     		     m_NextStartKey;
	LVC_KeyOIdQueue         *m_pKeyOIdQueue;
	tsp00_Int4               m_ObjKeyFileIndex;
	tsp00_Int4               m_CheckNumber;
	SAPDB_Bool               m_NoMoreOIds;
	SAPDB_Bool               m_bStoreKeys;

	static const tsp00_Int4  m_StaticCheckNumber;
};

/*!*****************************************************************************

   endclass: LVC_BufferedObjKeyIterator

*******************************************************************************/

#endif  /* LVC_BUFFEREDOBJKEYITERATOR_HPP */
