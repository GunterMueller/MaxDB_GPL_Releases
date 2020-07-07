/*!========================================================================

  @file         RTETask_LegacyQueueElementFreeList.h
  @ingroup      Runtime
  @author       RaymondR

  @brief        Access to the dobly linked queue element free list for 
                legacy code

  @since        2003-10-01  9:36
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/


#ifndef RTETASK_LEGACYQUEUEELEMENTFREELIST_H
#define RTETASK_LEGACYQUEUEELEMENTFREELIST_H

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


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
@brief         Get legacy queue element from free list
*/
externC DLQ_ELEMENT*         GetElementFromFreeList();

/*!
@brief         Put legacy queue element to free list
*/
externC void PutElementToFreeList( DLQ_ELEMENT* pQueueElement );

#endif  /* RTETASK_LEGACYQUEUEELEMENTFREELIST_H */
