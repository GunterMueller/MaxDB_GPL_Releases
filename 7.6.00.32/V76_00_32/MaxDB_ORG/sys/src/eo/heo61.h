/*!
  @file           heo61.h
  @author         JoergM
  @special area   Simple Kernel TRACE
  @brief          TRACE stack functions for kernel tasks

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



#ifndef HEO61_H
#define HEO61_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "geo47.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

 /*! @name TrackBuffer */
  /*@(*/

/*@)*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*! @name TrackBuffer API Functions */
 /*@(*/
/*!
   @brief          Allocate a TrackBuffer with a given size
   @param          size [in] Number of bytes to allocate for track buffer
   @return         FALSE if failed
      TRUE  if succeeded


         This function is used to allocate a track buffer for usage inside
         a specific task.

 */

externC tsp00_Bool vtrack_alloc ( tsp00_Int4 size );

/*!
   @brief          Free a previously allocated TrackBuffer
   @return         none


         This function is used to free a track buffer allocated for the calling
         task. If no track buffer was allocated, nothing is done.

 */

externC void vtrack_free( );

/*!
   @brief          Insert an element in the TrackBuffer
   @param          vtrace [in] Pointer to TrackBuffer content to add
   @param          len [in] Size of content in bytes
   @return         none


         This function is used to copy the message in the TrackBuffer and
         doing the pointer updates. If no more space is available a special
         overflow element is added, that contiains an overflow counter...

 */

externC void vtrack_insert ( void *vtrace, tsp00_Int4 len );

/*!
   @brief          RemoveLast element in the TrackBuffer
   @return         none


         This function is used to remove the last element in the track buffer.
         If last element is an overflow element, its counter is decreased.

 */

externC void vtrack_remove_last ( );

/*!
   @brief          Clear all entries in TrackBuffer
   @return         none


         This function is simple removing the reference to the first element.
         After this call the TrackBuffer is empty.

 */

externC void vtrack_empty( );

/*!
   @brief          Return the first element in the TrackBuffer
   @param          pMsg [out] Pointer to pointer to message
   @param          pLen [out] Pointer to size of message line
   @return         FALSE if no elements
      TRUE  if valid first element returned


         This function is used to return a pointer to the first internally stored 
         message to the caller. The first element is the first element ever stored
         or the first element stored after the last vtrack_Empty() call.

 */

externC tsp00_Bool vtrack_first ( void **pVtrace, tsp00_Int4 *pLen );

/*!
   @brief          Return the next element in the TrackBuffer
   @param          pVtrace [out] Pointer to pointer to message
   @param          pLen [out] Pointer to size of message line
   @return         FALSE if no more elements
      TRUE  if valid element returned


         This function is used to return a pointer to the internally stored 
         message to the caller. If no more message is stored, FALSE is returned
         and the values are set to NULL and 0 respectivly.
         The internal state of the track buffer changes, so subsequent calls to
         this function return followon messages.

 */

externC tsp00_Bool vtrack_next ( void **pVtrace, tsp00_Int4 *pLen );

/*!
   @brief          Return the counter of the overflow element if overflow
  occured.
   @return         number of overflows occured


         This function is used to return the number of overflows stored in the 
         TrackBuffer. A overflow occures if no more space is in the track buffer
         for a given message. A overflow can be removed by vtrack_remove_last
         like a normal message. A overflow count is maintained to keep track of the
         number of overflows that occured. This way only the message gets lost, but
         the number of messages stored is always correct.

 */

externC tsp00_Int4 vtrack_check ( );

/*!
   @brief          Return the first element of the first task that has a TrackBuffer
   @param          pMsg [out] Pointer to pointer to message
   @param          pLen [out] Pointer to size of message line
   @param          pId [out] Pointer to TaskId
   @return         FALSE if no valid task/element returned
      TRUE  if valid first element or first task returned


         This function is used to return a pointer to the first internally stored 
         message of the first task to the caller. This call works like vtrack_first
         but does not necessary return an element stored by the caller.

 */

externC tsp00_Bool vtrack_dump_first ( void **pVtrace, tsp00_Int4 *pLen, tsp00_TaskId *pId );

/*!
   @brief          Return the next element in TrackBuffer of given Task, skipping to next task if necessary
   @param          pMsg [out] Pointer to pointer to message
   @param          pLen [out] Pointer to size of message line
   @param          pId [inout] Pointer to TaskId (updated on success)
   @return         FALSE if no valid task/element returned
      TRUE  if valid first element or first task returned


         This function is used to return a pointer to the next internally stored 
         message of the first task to the caller. This call works like vtrack_next
         but does not stop before all task track buffers are dumped.

 */

externC tsp00_Bool vtrack_dump_next ( void **pVtrace, tsp00_Int4 *pLen, tsp00_TaskId *pId );

/*!
   @brief          Return the count of overflows (if any) occured in task specific TrackBuffer.
   @param          id [in] The Task Id for which TrackBuffer is tested
   @return         number of overflows occured for task specific TrackBuffer


         This function is used to return the number of overflows stored in the 
         TrackBuffer for a specific task. A overflow occures if no more space is in 
         the track buffer for a given message. A overflow can be removed by 
         vtrack_remove_last like a normal message. A overflow count is maintained 
         to keep track of the number of overflows that occured. This way only the message 
         gets lost, but the number of messages stored is always correct.

 */

externC tsp00_Int4 vtrack_dump_check ( tsp00_TaskId id );

/*@)*/

/*! @name Examples */
 /*@(*/

/*!
  Example: TrackBuffer usage example

  description:
      vtrack_alloc vtrack_free vtrack_insert vtrack_remove_last

  code:

#define MY_MESSAGE(_s) (_s),(strlen(_s)+1)
		int
	task_main()
	{
		vtrack_alloc(1000);

		vtrack_insert(MY_MESSAGE("main calling sub1"));
		sub1();
		vtrack_empty();
		vtrack_insert(MY_MESSAGE("main calling sub2"));
		sub2();

		vtrack_free();
		return 0;
	}

	void
	sub1()
	{
		{ // First interesting section
			vtrack_insert(MY_MESSAGE("sub1 calling sub2 first time"));
			sub2();
			vtrack_remove_last( );
		}
		{ // Second interesting section
			vtrack_insert(MY_MESSAGE("sub1 calling sub2 second time"));
			sub2();
			vtrack_remove_last( );
		}
		return;
	}

	void
	sub2() // Passing of to interesting points...
	{
		vtrack_insert(MY_MESSAGE("sub2 successfully called"));
		error_call();
		vtrack_remove_last( );
		return;
	}

	void
	error_call()
	{
	tsp00_Bool ok;
	tsp00_Int4 cnt;
	void *msg;
	tsp00_Int4 len;

		cout << _T("Error BIG PROBLEM... Traceback follows:") << endl;
		for ( ok = vtrack_first( &msg, &len ); 
			  ok != FALSE; 
			  ok = vtrack_next( &msg, &len ) )
		{
			cout << (char *)msg << endl;
		}
		cnt = vtrack_check( );
		if ( cnt > 0 )
		{
			cout << "Sorry OVERFLOW in TrackBuffer. Last " 
				 << cnt 
				 << " messages not stored"
				 << endl;
		}
	}
	
	Output

		Error Traceback...
		main
		sub1_1
		sub2_1
		sub2_2
		Error Traceback...
		main
		sub1_2
		sub2_1
		sub2_2		
 */

/*!
   @brief          vtrack_first,vtrack_next
   @code           
    
    To do it in order use a simple loop:
    
    tsp00_Bool ok;
    void *msg;
    tsp00_Int4 len;
    tsp00_Int4 cnt;
    
            cout << _T("Error BIG PROBLEM... Traceback follows:") << endl;
            for ( ok = vtrack_first( &msg, &len ); 
                      ok != FALSE; 
                      ok = vtrack_next( &msg, &len ) )
            {
                cout << (char *)msg << endl;
            }
    
            cnt = vtrack_check( );
            if ( cnt > 0 )
            {
                    cout << "Sorry OVERFLOW in TrackBuffer. Last " 
                             << cnt 
                             << " messages not stored"
                             << endl;
            }
    
    @endcode        

                         Shows how a track buffer content is dumped in order.
                         NOTE: Content of TrackBuffer is opaque. For this example
                         strings have been inserted...

 */


/*!
   @brief          vtrack_dump_first,vtrack_dump_next
   @code           
    
    To do it in order use a simple loop:
    
    WARNING: This example is not multi thread save! To make it multi thread save
                     enclose it in a critical section or make sure no task is writing in
                     its TrackBuffer anymore (should be true during trace writer dump...).
    
    tsp00_Bool ok;
    void *msg;
    tsp00_Int4 len;
    tsp00_Int4 cnt;
    tsp00_TaskId last_id;
    tsp00_TaskId id;
    
            cout << "TRACKBUFFER DUMP FOLLOWS:" << endl;
            ok = vtrack_dump_first( &msg, &len, &id );
            while ( ok != FALSE )
            {
                    last_id = id;
    
                    cout << "----------------------------------" << endl;
                    cout << "Dumping Trackbuffer for Task T" << (long)last_id << endl;
    
                    do {
                            cout << (char *)msg << endl;
                            ok = vtrack_dump_next( &msg, &len, &id ) )
                    } while ( ok != FALSE && id == last_id );
    
                    cnt = vtrack_dump_check( last_id );
                    if ( cnt > 0 )
                    {
                            cout << "Sorry OVERFLOW in TrackBuffer. Last " 
                                     << cnt 
                                     << " messages not stored"
                                     << endl;
                    }
            }
    
    
    @endcode        

                         Shows how all track buffer contents are dumped in order.
                         NOTE: Content of TrackBuffer is opaque. For this example
                         strings have been inserted...

 */


/*@)*/

#endif  /* HEO61_H */
