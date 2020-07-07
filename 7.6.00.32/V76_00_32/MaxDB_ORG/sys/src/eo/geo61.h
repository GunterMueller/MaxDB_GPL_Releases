/*!
  @file           geo61.h
  @author         JoergM
  @special area   Simple Kernel TRACE
  @brief          Structures and Classes used by simple Kernel Trace
  @see            example.html ...

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



\endif
*/



#ifndef GEO61_H
#define GEO61_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include <stdlib.h>
#include <string.h> /* NULL */
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifndef externC
#if !defined(__cplusplus)
#define externC
#else
#define externC extern "C"
#endif
#endif /* externC */

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

#if defined(__cplusplus)

/*! @name TrackBuffer API Classes */
 /*@(*/
/*
  Class: teo61_TrackBufferEntry

  description: Element of the Track Buffer
 */
class teo61_TrackBufferEntry
{
private:
    teo61_TrackBufferEntry *m_prev_TBeo61;
    teo61_TrackBufferEntry *m_next_TBeo61;
	tsp00_Int4              m_len_TBeo61; // 0 on overflow
	union
	{
		void                   *m_msg_TBeo61;
		tsp00_Int4             m_overflow_TBeo61;
	} m_u_TBeo61;

	inline void SetAsLast( teo61_TrackBufferEntry *priv );

public:
    friend class teo61_TrackBuffer;
};
/* EndClass: teo61_TrackBufferEntry */

inline void teo61_TrackBufferEntry::SetAsLast( teo61_TrackBufferEntry *prev )
{
	m_prev_TBeo61 = prev;
	m_next_TBeo61 = NULL;
	m_len_TBeo61 = 0;
	m_u_TBeo61.m_overflow_TBeo61 = 0;
}

/*!
  @class          teo61_TrackBuffer
   @brief          The TrackBuffer implements the low cost track buffer

 */

class teo61_TrackBuffer
{
private:
    void                   *m_storage_TBeo61;
	tsp00_Int4             m_storage_size_TBeo61;
	tsp00_Int4             m_storage_remaining_TBeo61;
    teo61_TrackBufferEntry *m_head_TBeo61;
    teo61_TrackBufferEntry *m_tail_TBeo61;
    teo61_TrackBufferEntry *m_next_next_TBeo61;

	void                    SetLast( teo61_TrackBufferEntry *priv );
public:
/*!
   @brief          Constructor of a TrackBuffer instance
   @param          size [in] Number of Bytes to allocate for the TrackBuffer
   @return         not appliable


         Normally this is called once per UKT Task.

 */

inline teo61_TrackBuffer( tsp00_Int4 size );

/*!
   @brief          Destructor of a TrackBuffer instance
   @return         not appliable


         Normally never called from UKT Task.

 */

inline ~teo61_TrackBuffer( );

inline tsp00_Bool IsValid() const;

/*!
   @brief          Reset the TrackBuffer
   @return         none


         This function is simple removing the reference to the first element.
         After this call the TraceStack is empty. The storage is still allocated

 */

inline void eo61_Empty( );

/*!
   @brief          Insert a message line in the TrackBuffer
   @param          msg [in] Pointer to the message
   @param          len [in] Length of the message
   @return         none


         This function is used to copy a message into the TrackBuffer and connecting
         its pointers.

 */

inline void eo61_Insert( void *msg, tsp00_Int4 len );

/*!
   @brief          Remove the last message line in the TrackBuffer
   @param          msg [in] Pointer to the message
   @param          len [in] Length of the message
   @return         none


         This function is used to pop the last content out of the TrackBuffer

 */

inline void eo61_RemoveLast( );

/*!
   @brief          Returns the current first element in TrackBuffer.
   @return         FALSE if no element in TrackBuffer
      TRUE  if first element returned

 */

inline tsp00_Bool eo61_FirstLine( void **pVtrace, tsp00_Int4 *pLen );

/*!
   @brief          Returns the next element in TrackBuffer.
   @return         FALSE if no element in TrackBuffer
      TRUE  if first element returned

 */

inline tsp00_Bool eo61_NextLine( void **pVtrace, tsp00_Int4 *pLen );

/*!
   @brief          Returns the number of elements not been added
   @return         number of elements that not have been added


           Each Insert that was not able to be added is counted. Each RemoveLast
           decrements the count, so that this will not persist.

 */

inline tsp00_Int4 eo61_OverflowCount( );

};


inline teo61_TrackBuffer::teo61_TrackBuffer( tsp00_Int4 size )
{
	m_storage_TBeo61 = NULL; 
	m_head_TBeo61 = NULL;
	m_tail_TBeo61 = NULL;
	m_storage_remaining_TBeo61 = 0;
	m_next_next_TBeo61 = NULL;

	if ( size > 0 )
	{
        /* PTS 1110403 */
		m_storage_TBeo61 = malloc(size + sizeof(teo61_TrackBufferEntry));
		if ( m_storage_TBeo61 != NULL )
		{
			m_storage_size_TBeo61 = size;
			eo61_Empty();
		}
	}
}

inline teo61_TrackBuffer::~teo61_TrackBuffer( )
{
    /* PTS 1110403 */
	free(m_storage_TBeo61);
	m_storage_TBeo61 = 0;
}

inline tsp00_Bool teo61_TrackBuffer::IsValid() const
{
	return (tsp00_Bool)(m_storage_TBeo61 != NULL); 
}

inline void teo61_TrackBuffer::eo61_Insert(
void *msg, 
tsp00_Int4 len )
{
	if ( msg != NULL && len > 0 )
	{
		teo61_TrackBufferEntry *ptr = m_tail_TBeo61;

		if ( ptr->m_u_TBeo61.m_overflow_TBeo61 > 0 )
		{
			ptr->m_u_TBeo61.m_overflow_TBeo61++;
		}
		else
		{
			if ( (len + (tsp00_Int4)sizeof(teo61_TrackBufferEntry)) 
					<= m_storage_remaining_TBeo61 )
			{
				ptr->m_u_TBeo61.m_msg_TBeo61 = (void *)(ptr + 1);
				ptr->m_len_TBeo61 = len;
				SAPDB_MemCopyNoCheck( (void *)(ptr + 1), msg, len);
#define LEN_ALIGN(_l,_a) (_a)*(((_l)+(_a)-1)/(_a))
				ptr = (teo61_TrackBufferEntry *) ( 
					((char *)(ptr+1)) 
						+ LEN_ALIGN(len, sizeof(teo61_TrackBufferEntry))
												 );
				m_tail_TBeo61->m_next_TBeo61 = ptr;										
				ptr->SetAsLast(m_tail_TBeo61);
				m_tail_TBeo61 = ptr;
				m_storage_remaining_TBeo61 = 
				 m_storage_size_TBeo61 - 
					(tsp00_Int4)((char *)(ptr+1) - (char *)m_head_TBeo61);
			}
			else
			{
				ptr->m_u_TBeo61.m_overflow_TBeo61 = 1;
			}
		}
	}
}

inline void teo61_TrackBuffer::eo61_RemoveLast( )
{
	teo61_TrackBufferEntry *ptr = m_tail_TBeo61;

	if ( ptr->m_u_TBeo61.m_overflow_TBeo61 > 0 )
	{
		ptr->m_u_TBeo61.m_overflow_TBeo61--;
	}
	else
	{
		ptr = ptr->m_prev_TBeo61;
		if ( ptr )
		{
			m_tail_TBeo61 = ptr;
			ptr->SetAsLast(ptr->m_prev_TBeo61);
			m_storage_remaining_TBeo61 = 
				(tsp00_Int4)((char *)(ptr+1) - (char *)m_head_TBeo61);
		}
	}
}

inline void teo61_TrackBuffer::eo61_Empty( )
{
	teo61_TrackBufferEntry *ptr = 
		(teo61_TrackBufferEntry *)m_storage_TBeo61;

	ptr->SetAsLast(NULL);
	m_head_TBeo61 = m_tail_TBeo61 = ptr;
	m_storage_remaining_TBeo61 = m_storage_size_TBeo61;
	m_next_next_TBeo61 = NULL;
}

inline tsp00_Bool teo61_TrackBuffer::eo61_FirstLine(
void **pMsg,
tsp00_Int4  *pLen )
{
	tsp00_Bool result = (tsp00_Bool)0;
	teo61_TrackBufferEntry *ptr = m_head_TBeo61;

	if ( pMsg && pLen && ptr )
	{
		if ( ptr->m_len_TBeo61 > 0 )
		{
			*pMsg = ptr->m_u_TBeo61.m_msg_TBeo61;
			*pLen = ptr->m_len_TBeo61;
			m_next_next_TBeo61 = ptr->m_next_TBeo61;
			result = (tsp00_Bool)1;
		}
	}
	return result;
}

inline tsp00_Bool teo61_TrackBuffer::eo61_NextLine( 
void      **pMsg,
tsp00_Int4  *pLen )
{
	tsp00_Bool result = (tsp00_Bool)0;
	teo61_TrackBufferEntry *ptr = m_next_next_TBeo61;

	if ( pMsg && pLen && ptr )
	{
		if ( ptr->m_len_TBeo61 > 0 )
		{
			*pMsg = ptr->m_u_TBeo61.m_msg_TBeo61;
			*pLen = ptr->m_len_TBeo61;
			m_next_next_TBeo61 = ptr->m_next_TBeo61;
			result = (tsp00_Bool)1;
		}
	}
	return result;
}

inline tsp00_Int4 teo61_TrackBuffer::eo61_OverflowCount( )
{
	tsp00_Int4 count = 0;
	teo61_TrackBufferEntry *ptr = m_tail_TBeo61;

	if ( ptr )
	{
		if ( ptr->m_len_TBeo61 == 0 )
		{
			count = ptr->m_u_TBeo61.m_overflow_TBeo61;
		}
	}
	return count;
}

/*@)*/

#endif


#endif  /* GEO61_H */
