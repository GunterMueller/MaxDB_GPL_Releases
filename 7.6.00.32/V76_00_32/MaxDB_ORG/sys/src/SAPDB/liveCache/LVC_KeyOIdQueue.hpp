/****************************************************************************

  module      : LVC_KeyOIdQueue.hpp

  -------------------------------------------------------------------------

  responsible : d025251

  special area: 
  description : 


  last changed: 2001-07-16  15:58
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


#ifndef LVC_KEYOIDQUEUE_HPP
#define LVC_KEYOIDQUEUE_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg92.h"

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

   class:        LVC_KeyOIdQueue
   description:  This class implements a buffer to accomodate a fixed number of
                 key OId pairs 

*******************************************************************************/

class LVC_KeyOIdQueue
{

public:

	LVC_KeyOIdQueue (SAPDB_UInt               NumElements,
		             SAPDB_UInt               KeyLen,
		             SAPDBMem_IRawAllocator  &Allocator)
	:
	m_NumElements       (NumElements),
	m_NumUsedElements   (0),
	m_KeyLen            (KeyLen),
	m_ElementSize       (ALIGN_8BYTE_EO00 (sizeof (tgg92_KernelOid) + KeyLen)),
	m_FirstElementIndex (0),
	m_Allocator         (Allocator)
	{
	   m_Buffer = (SAPDB_Byte *) Allocator.Allocate (NumElements * m_ElementSize);	
	}

	/*!-------------------------------------------------------------------------
	function:     destructor ();
	description:  release the allocated memory
		   
	arguments:    none
	return value: none
    --------------------------------------------------------------------------*/
    ~LVC_KeyOIdQueue ()
	{
	   m_Allocator.Deallocate(m_Buffer);
	}

	/*!-------------------------------------------------------------------------
	function:     IsEmpty ();
	description:  displays if the queue contains any element
		   
	arguments:    none
	return value: (SAPDB_Bool) returns true if the queue contains any element
    --------------------------------------------------------------------------*/
    SAPDB_Bool IsEmpty() const
	{
	   return(m_NumUsedElements == 0);
	}

    /*!-------------------------------------------------------------------------
	function:     IsFull ();
	description:  displays if the queue contains any free element
		   
	arguments:    none
	return value: (SAPDB_Bool) returns true if the queue contains no free element
    --------------------------------------------------------------------------*/
    SAPDB_Bool IsFull() const
	{
	   return (m_NumUsedElements == m_NumElements);
	}

    /*!-------------------------------------------------------------------------
	function:     Append ();
	description:  appends the input oid to the queue
		   
	arguments:    none
	return value: (SAPDB_Bool) returns true if the queue could still accomodate the new OId
    --------------------------------------------------------------------------*/
    void Append(const tgg92_KernelOid &OId)
	{
		SAPDBERR_ASSERT_STATE (!IsFull());

		++m_NumUsedElements;

		SAPDB_UInt ElementIndex = m_FirstElementIndex + m_NumUsedElements - 1;
		if (ElementIndex >= m_NumElements)
          ElementIndex -= m_NumElements; 

		memcpy (m_Buffer + ElementIndex*m_ElementSize, &OId, sizeof(tgg92_KernelOid));
	}

    /*!-------------------------------------------------------------------------
	function:     Append ();
	description:  appends the input oid to the queue
		   
	arguments:    none
	return value: (SAPDB_Bool) returns true if the queue could still accomodate the new OId
    --------------------------------------------------------------------------*/
    void Append(const tgg92_KernelOid &OId,
		        void                  *KeyPtr)
	{
		SAPDBERR_ASSERT_STATE (!IsFull());
		SAPDBERR_ASSERT_STATE (m_KeyLen > 0);

		++m_NumUsedElements;

		SAPDB_UInt ElementIndex = m_FirstElementIndex + m_NumUsedElements - 1;
		if (ElementIndex >= m_NumElements)
          ElementIndex -= m_NumElements; 

		memcpy (m_Buffer + ElementIndex*m_ElementSize, &OId, sizeof(tgg92_KernelOid));
		memcpy (m_Buffer + ElementIndex*m_ElementSize + sizeof(tgg92_KernelOid), KeyPtr, m_KeyLen);
	}

	/*!-------------------------------------------------------------------------
	function:     GetTopOId ();
	description:  returns first OId in queue
		   
	arguments:    none
	return value: (tgg92_KernelOid) first OId in queue
    --------------------------------------------------------------------------*/
    tgg92_KernelOid GetTopOId()
	{
		SAPDBERR_ASSERT_STATE (!IsEmpty());
		return * ((tgg92_KernelOid *) (m_Buffer + m_FirstElementIndex * m_ElementSize));
	}

    /*!-------------------------------------------------------------------------
	function:     GetTopKeyPtr ();
	description:  returns pointer to the first key in queue
		   
	arguments:    none
	return value: (void*) pointer to the first key in queue
    --------------------------------------------------------------------------*/
    void* GetTopKeyPtr()
	{
		SAPDBERR_ASSERT_STATE (!IsEmpty());
		SAPDBERR_ASSERT_STATE (m_KeyLen > 0);
		return (m_Buffer + (m_FirstElementIndex * m_ElementSize) + sizeof (tgg92_KernelOid));
	}

	/*!-------------------------------------------------------------------------
	function:     DelTopEntry ();
	description:  removes first OId in queue
		   
	arguments:    none
	return value: none
    --------------------------------------------------------------------------*/
    void DelTopEntry()
	{
		SAPDBERR_ASSERT_STATE (!IsEmpty());
		--m_NumUsedElements;

		if (m_FirstElementIndex < m_NumElements-1)
         ++m_FirstElementIndex; 
		else
		  m_FirstElementIndex = 0;
    }

	/*!-------------------------------------------------------------------------
	function:     Reset ();
	description:  Initializes the Queue
		   
	arguments:    none
	return value: none
  --------------------------------------------------------------------------*/
  void Reset()
	{
		m_NumUsedElements = 0;

		  m_FirstElementIndex = 0;
  }

  /*------------------------------------------------------------------------*/

private: //members
	const SAPDB_UInt        m_NumElements;
	const SAPDB_UInt        m_KeyLen;
	const SAPDB_UInt        m_ElementSize;
	SAPDB_UInt              m_NumUsedElements;
	SAPDB_UInt              m_FirstElementIndex;
	SAPDB_Byte *            m_Buffer;
	SAPDBMem_IRawAllocator &m_Allocator;
};

/*!*****************************************************************************

   endclass: LVC_KeyOIdQueue

*******************************************************************************/

#endif  /* LVC_KEYOIDQUEUE_HPP */
