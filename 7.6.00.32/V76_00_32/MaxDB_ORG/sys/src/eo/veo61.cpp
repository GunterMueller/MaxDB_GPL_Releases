/*!
  @file           veo61.cpp
  @author         JoergM
  @special area   Simple Kernel TRACE
  @brief          Wrapper Funktions and Allocation for RTE C_Code
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo61.h"
#include "heo61.h"
#include "geo00_0.h"
#include "heo00.h"
#include "geo50_0.h"
#include <new.h>
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#if defined(_WIN32)
#define CURRENT_TASK() THIS_UKT_CTRL->pCTask
#else
#define CURRENT_TASK() THIS_UKT_CTRL->curr_task
#endif

#define NUM_OF_TASKS()  KGS->ulNumOfTasks
#define ID_TO_TASK(_id) (KGS->pFirstTaskCtrl + ((_id) - 1))

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externC tsp00_Bool vtrack_alloc( tsp00_Int4 size )
{
	teo61_TrackBuffer *pTB;
    /* PTS 1110400 */
	void *buffer = malloc(sizeof(teo61_TrackBuffer));
	pTB = new (buffer) teo61_TrackBuffer( size );
    CURRENT_TASK()->pTrackBuffer = pTB;
	return  pTB && pTB->IsValid();
}

externC void vtrack_free( )
{
	teo61_TrackBuffer *pTB;
	
	pTB = (teo61_TrackBuffer *)CURRENT_TASK()->pTrackBuffer;
	if ( pTB )
	{
        /* PTS 1110400 */
        pTB->~teo61_TrackBuffer();
		free(pTB);
		CURRENT_TASK()->pTrackBuffer = NULL;
	}
}

externC void vtrack_insert( void *vtrace, tsp00_Int4 len )
{
	teo61_TrackBuffer *pTB;
	
	pTB = (teo61_TrackBuffer *)CURRENT_TASK()->pTrackBuffer;
	if ( pTB )
	{
		pTB->eo61_Insert( vtrace, len );
	}
}

externC void vtrack_remove_last( )
{
	teo61_TrackBuffer *pTB;

	pTB = (teo61_TrackBuffer *)CURRENT_TASK()->pTrackBuffer;
	if ( pTB )
	{
		pTB->eo61_RemoveLast( );
	}
}

externC void vtrack_empty( )
{
	teo61_TrackBuffer *pTB;
	
	pTB = (teo61_TrackBuffer *)CURRENT_TASK()->pTrackBuffer;
	if ( pTB )
	{
	    pTB->eo61_Empty();
	}
}

externC tsp00_Bool vtrack_first( void **pVtrace, tsp00_Int4 *pLen )
{
	teo61_TrackBuffer *pTB;

	pTB = (teo61_TrackBuffer *)CURRENT_TASK()->pTrackBuffer;
	if ( pTB )
	{
	    return pTB->eo61_FirstLine( pVtrace, pLen );
	}
	else
	{
		return (tsp00_Bool)0;
	}
}


externC tsp00_Bool vtrack_next( void **pVtrace, tsp00_Int4 *pLen )
{
	teo61_TrackBuffer *pTB;
	
	pTB = (teo61_TrackBuffer *)CURRENT_TASK()->pTrackBuffer;
	if ( pTB )
	{
	    return pTB->eo61_NextLine( pVtrace, pLen);
	}
	else
	{
		return (tsp00_Bool)0;
	}
}

externC tsp00_Int4 vtrack_check( )
{
	tsp00_Int4 count = 0;
	teo61_TrackBuffer *pTB;
	
	pTB = (teo61_TrackBuffer *)CURRENT_TASK()->pTrackBuffer;
	if ( pTB )
	{
	    count = pTB->eo61_OverflowCount( );
	}

	return count;
}

externC tsp00_Bool vtrack_dump_first( void **pVtrace, tsp00_Int4 *pLen, tsp00_TaskId *pId )
{
	tsp00_Bool found = 0;
	tsp00_TaskId id;
	teo61_TrackBuffer *pTB;

	for ( id = 1; id <= (tsp00_TaskId)NUM_OF_TASKS() 
		       && found == ((tsp00_Bool)0); id++ )
	{
		pTB = (teo61_TrackBuffer *)ID_TO_TASK(id)->pTrackBuffer;
		if ( pTB )
		{
			found = pTB->eo61_FirstLine( pVtrace, pLen );
		}
	}

	if ( found != ((tsp00_Bool)0) )
	{
		*pId = id;
	}
	return found;
}


externC tsp00_Bool vtrack_dump_next( void **pVtrace, tsp00_Int4 *pLen, tsp00_TaskId *pId )
{
	tsp00_Bool found = 0;
	tsp00_TaskId id;
	teo61_TrackBuffer *pTB;

	id = *pId;
	pTB = (teo61_TrackBuffer *)ID_TO_TASK(id)->pTrackBuffer;
	if ( pTB )
	{
		found = pTB->eo61_NextLine( pVtrace, pLen );
	}

	if ( found != ((tsp00_Bool)0) )
	{
		*pId = id;
	}
	else
	{
		for ( id++; id <= (tsp00_TaskId)NUM_OF_TASKS() 
			       && found == ((tsp00_Bool)0); id++ )
		{
			pTB = (teo61_TrackBuffer *)ID_TO_TASK(id)->pTrackBuffer;
			if ( pTB )
			{
				found = pTB->eo61_FirstLine( pVtrace, pLen );
			}
		}
	}
	if ( found != ((tsp00_Bool)0) )
	{
		*pId = id;
	}
	return found;
}

externC tsp00_Int4 vtrack_dump_check( tsp00_TaskId id )
{
	tsp00_Int4 count = 0;
	teo61_TrackBuffer *pTB;

	if ( id > 0 || id <= (tsp00_TaskId)NUM_OF_TASKS() )
	{
		pTB = (teo61_TrackBuffer *)ID_TO_TASK(id)->pTrackBuffer;
		if ( pTB )
		{
		    count = pTB->eo61_OverflowCount( );
		}
	}

	return count;
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
