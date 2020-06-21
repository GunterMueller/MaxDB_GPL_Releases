/****************************************************************************

  module      : LVC_ObjKeyIterator.hpp

  -------------------------------------------------------------------------

  responsible : d025251

  special area: 
  description : 


  last changed: 2001-07-05  18:40
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

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


#ifndef LVC_OBJKEYITERATOR_HPP
#define LVC_OBJKEYITERATOR_HPP

#include "gbd300.h"
#include "gbd500.h"
#include "gbd510.h"
#include "gbd600.h"
#include "ggg00.h"
#include "ggg92.h"

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

   class:        LVC_ObjKeyIterator
   description:  This class implements a key range iterator for object files
   
	             

*******************************************************************************/

class LVC_ObjKeyIterator
{
public:

	
	/*!-------------------------------------------------------------------------
    function:     IsReady()
    description:  displays if there are any more OId in the scope of the iterator
		  
	arguments:    none
	return value: (SAPDB_Bool) true if there are more oids in the iterator scope which can be 
	                           reached via the operator ++
    --------------------------------------------------------------------------*/
	SAPDB_Bool	IsReady()
	{
		return m_RecordIterator.bd510GetRecord() == 0;
	}

	/*!-------------------------------------------------------------------------
    function:     GetOId()
    description:  dereferences the iterator,
	
	The OId of the object at which the iterator is currently pointing is retuirned 
	  
	arguments:    none
	return value: (tgg92_KernelOid) OId of the object at which the iterator is currently pointing
    --------------------------------------------------------------------------*/
	tgg92_KernelOid	GetOId()
	{
		tgg00_RecPtr  pRec = m_RecordIterator.bd510GetRecord();
		
		if ( NULL == pRec )
		{
			m_InvCurrent.curr_trans->trError_gg00 = e_no_next_invkey;
			return tgg92_KernelOid();
		}
		else
		{
			tsp00_KeyPtr          pOId;
			cbd450_InvListArray   InvListArray (false /* ConsistentReadSupport */, pRec);
			tsp00_Int2            AuxOIdLen;
            tgg92_KernelOid       auxOid; // PTS 1112239 UH 2001-10-22

			InvListArray.bd450GetPrimKey (FIRST_PRIM_KEY_INDEX_BD450, pOId, AuxOIdLen);
			
			SAPDBERR_ASSERT_STATE (AuxOIdLen == sizeof(tgg92_KernelOid));	

            memcpy (&auxOid, pOId, sizeof(tgg92_KernelOid)); // PTS 1112239 UH 2001-10-22

            return auxOid; // PTS 1112239 UH 2001-10-22
		}
	}

	/*!-------------------------------------------------------------------------
    function:     GetKeyPtr()
    description:  dereferences the iterator,
	
	The OId of the object at which the iterator is currently pointing is retuirned 
	  
	arguments:    none
	return value: (tgg92_KernelOid) OId of the object at which the iterator is currently pointing
    --------------------------------------------------------------------------*/
	void * GetKeyPtr()
	{
        tsp00_KeyPtr pKey;
        tsp00_Int2   keyLength;
        
        if ( ! m_RecordIterator.bd510GetKey(pKey, keyLength) )
		{
			m_InvCurrent.curr_trans->trError_gg00 = e_no_next_invkey;
			return NULL;
		}
		else
		{
			SAPDBERR_ASSERT_STATE (keyLength == m_KeyLen);	
			return pKey;
		}
	}

	/*!-------------------------------------------------------------------------
	function:     operator ++();
	description:  moves the iterator to the next object in the key range

		   
	arguments:    none
	return value: none
    --------------------------------------------------------------------------*/
	void operator ++()
	{
	   ++m_RecordIterator;
	}

	/*!-------------------------------------------------------------------------
	function:     constructor()
	description:  
		   
	arguments:    none
	return value: none
    --------------------------------------------------------------------------*/
	LVC_ObjKeyIterator (tgg00_TransContext     &Trans,
                      tgg00_FileId           &ObjKeyFileId,
                      tsp00_Int4              KeyLen,
                      SAPDB_Bool              bIncludeStartKey,
                      void                   *pStartKey,
                      void                   *pStopKey,
                      SAPDB_Bool             &bStartKeyFound,
                      SAPDB_Bool              bAscendingKeyOrder = ASCENDING_KEY_ORDER_BD510)  // PTS 1119480
	:
	m_InvCurrent     (Trans, ObjKeyFileId,
	                  tbd_node_request::fromConst(nr_for_read),
	                  tgg00_MessType::fromConst(m_select),
	                  tgg00_VtraceType::fromConst(b03get),
		              ! IGNORE_INV_ERROR_BD300),
	m_LeafNode       (m_InvCurrent, tbd_node_request::fromConst(nr_for_read)),
	m_Tree           (m_LeafNode),
	m_RecordIterator (m_Tree, tsp00_BytePtr (pStartKey), KeyLen, tsp00_BytePtr (pStopKey), 
	                  KeyLen, bAscendingKeyOrder, bIncludeStartKey, bStartKeyFound),
  m_KeyLen         (KeyLen)
	{}

private: // member
         cbd300_InvCurrent     m_InvCurrent;
		 cbd600_Node           m_LeafNode;
		 cbd500_Tree           m_Tree;
		 cbd510_RecordIterator m_RecordIterator;
		 const tsp00_Int4      m_KeyLen;
};

/*!*****************************************************************************

   endclass: LVC_ObjKeyIterator

*******************************************************************************/

#endif  /* LVC_OBJKEYITERATOR_HPP */
