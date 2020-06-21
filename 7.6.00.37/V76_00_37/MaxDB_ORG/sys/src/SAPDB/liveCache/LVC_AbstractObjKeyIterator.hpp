/*!
 * \file    LVC_AbstractObjKeyIterator.hpp
 * \author  MarkusS
 * \brief   abstract interface for a key iterator
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


#ifndef LVC_ABSTRACTOBJKEYITERATOR_HPP
#define LVC_ABSTRACTOBJKEYITERATOR_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define CHECK_NUMBER 4711


/*!*****************************************************************************

   class:        LVC_AbstractObjKeyIterator
   description:  This class implements a key range iterator for object files
   
*******************************************************************************/

class LVC_AbstractObjKeyIterator
{
public:
    
    /*!-------------------------------------------------------------------------
    function:     GetObjFileId()
    description:  returns the id of the file the iterator is working on

    arguments:    none
    return value: (tgg00_FileId) id of the file the iterator is working on
    --------------------------------------------------------------------------*/
	tgg00_FileId   GetObjFileId()
	{
		return m_ObjFileId;
	}
	
	/*!-------------------------------------------------------------------------
	function:     GetNextOIdsWithKey()
	description:  reads the next larger oid and returns it
                
	arguments:  pRestartKey [in]  the iterator starts reading from this key
                pOId        [out] array to store the oids which have been read
                noOfOid     [in/out] in: maximal number of oids which can be read
                                  (=size of array pOId).
                                  out: number of oids which have been read.
                pKey        [out] array to store keys which have been read
                KeySize     [in/out] in: size of buffer available
                                  out: size of one key entry
                bIncluse    [in]  should object with key = restartkey be included
                                  in the return array
	return value: none

    New function since: PTS 1134856 
    --------------------------------------------------------------------------*/
    virtual void GetNextOIdsWithKey(void            *pRestartKey,
                                    tgg92_KernelOid *pOId, 
                                    tsp00_Int4      &noOfOid, 
                                    void            *pKey,
                                    int             &KeySize,
                                    SAPDB_Bool       bInclusive,
                                    Msg_List        &errorList) = 0;

	/*!-------------------------------------------------------------------------
	function:     GetNextOIds()
	description:  reads the next largest oids and return them in an array.
                
	arguments:    pRestartKey [in]  the iterator starts reading from this key
                pOId        [out] array to store the oids which have been read
                noOfOid     [in/out] in: maximal number of oids which can be read
                                  (=size of array pOId).
                                  out: number of oids which have been read.
                bIncluse    [in]  should object with key = restartkey be included
                                  in the return array

	return value: none

  New function since: PTS 1119480
  --------------------------------------------------------------------------*/
	virtual void GetNextOIds(void          *pRestartKey,
                           tgg92_KernelOid *pOId, 
                           tsp00_Int4      &noOfOid, 
                           SAPDB_Bool       bInclusive,
                           Msg_List        &errorList) = 0;

	/*!-------------------------------------------------------------------------
	function:     GetPrevOIds()
	description:  reads the next smallest oids and return them in an array.
                
	arguments:    pRestartKey [in]  the iterator starts reading from this key
                pOId        [out] array to store the oids which have been read
                noOfOid     [in/out] in: maximal number of oids which can be read
                                  (=size of array pOId).
                                  out: number of oids which have been read.
                bIncluse    [in]  should object with key = restartkey be included
                                  in the return array

	return value: none

  New function since: PTS 1119480
  --------------------------------------------------------------------------*/
	virtual void GetPrevOIds(void            *pRestartKey,
                           tgg92_KernelOid *pOId, 
                           tsp00_Int4      &noOfOid, 
                           SAPDB_Bool       bInclusive,
                           Msg_List        &errorList) = 0;

	/*!-------------------------------------------------------------------------
	function:     AscendingKeyOrder()
	description:  
  
	arguments:    none
	return value: (SAPDB_Bool) true: if iterator is currently running in ascending key order;
                             false: otherwise
  New function since: PTS 1119480
  --------------------------------------------------------------------------*/
  virtual SAPDB_Bool AscendingKeyOrder()
  {
    return m_bAscendingKeyOrder;
  }

	/*!-------------------------------------------------------------------------
	function:     ChangeDirection()
	description:  repositions the iterator on the specified key
   
	arguments:    pRestartKey        [in] key of the object the iterator should be positioned on
                bAscendingKeyOrder [in] the new direction the iterator should run

	return value: none

  New function since: PTS 1119480
  --------------------------------------------------------------------------*/
  virtual void ChangeDirection(void* pRestartKey, SAPDB_Bool bAscendingKeyOrder, Msg_List &errorList) = 0;

  /*!-------------------------------------------------------------------------
	function:     IsValid()
	description:  returns true if the iterator fulfills some basic requirements of an iterator

          This function can be used to validate an iterator after a cast
          from a void pointer. Thus this function represents a simple
				  type check.
		   
	arguments:    none
	return value: (SAPDB_Bool) true if the iterator fulfills some basic requirements
    --------------------------------------------------------------------------*/
    virtual SAPDB_Bool  IsValid()
	{
		return (m_CheckNumber == CHECK_NUMBER);
	}

    /*!-------------------------------------------------------------------------
	function:     IsEmpty()
	description:  returns true if the end of the iterator scope is reached
		   
	arguments:    none
	return value: (SAPDB_Bool) true if the end of the iterator scope is reached
    --------------------------------------------------------------------------*/
    virtual SAPDB_Bool  IsEmpty() = 0;

	/*!-------------------------------------------------------------------------
	function:     constructor()
	description:  
		   
	arguments:    none
	return value: none
    --------------------------------------------------------------------------*/
	LVC_AbstractObjKeyIterator (tgg00_TransContext     &Trans,
                                Msg_List               &errorList,
		                        tgg00_FileId           &ObjFileId,
		                        tsp00_Int4              KeyLen,	
		                        void                   *pLowerKey,         // PTS 1119480
		                        void                   *pUpperKey,         // PTS 1119480
                                SAPDB_Bool              bAscendingKeyOrder) // PTS 1119480
  :
	m_KeyLen               (KeyLen),
	m_ObjFileId            (ObjFileId),
	m_Trans                (Trans),
	m_CheckNumber          (CHECK_NUMBER),
  m_bAscendingKeyOrder   (bAscendingKeyOrder)        // PTS 1119480     
	{
		memcpy (&m_LowerKey, pLowerKey,  m_KeyLen);      // PTS 1119480
		memcpy (&m_UpperKey, pUpperKey,  m_KeyLen);      // PTS 1119480
	}

	/*!-------------------------------------------------------------------------
	function:     destructor()
	description:  releases all resources and mark the iterator as destroyed via the checknumber
		   
	arguments:    none
	return value: none
    --------------------------------------------------------------------------*/
	virtual ~LVC_AbstractObjKeyIterator(){};

protected: // member
	tsp00_Int4               m_KeyLen;
  tsp00_Key                m_LowerKey;            // PTS 1119480
  tsp00_Key                m_UpperKey;            // PTS 1119480
	tgg00_FileId             m_ObjFileId;
	tgg00_TransContext      &m_Trans; 
	tsp00_Int4               m_CheckNumber;
  SAPDB_Bool               m_bAscendingKeyOrder;  // PTS 1119480

};

/*!*****************************************************************************

   endclass: LVC_AbstractObjKeyIterator

*******************************************************************************/

#endif  /* LVC_ABSTRACTOBJKEYITERATOR_HPP */
