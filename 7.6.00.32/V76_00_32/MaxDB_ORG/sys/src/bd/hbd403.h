/****************************************************************************

  module      : hbd403.h

  -------------------------------------------------------------------------

  author      : AlexanderK
  responsible : UweH

  special area: Indexhandling
  description : ...


  last changed: 1999-09-06  19:15
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#ifndef HBD403_H
#define HBD403_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


externCpp void
bd403CountPrimKeysInInvLists (cbd300_InvCurrent &InvCurrent, 
							  tsp00_KeyPtr       pStartSecKey,
							  tsp00_Int4         StartSecKeyLen, 
							  tsp00_KeyPtr       pStopSecKey,
							  tsp00_Int4         StopSecKeyLen,
							  tsp00_KeyPtr       pStartPrimKey,
							  tsp00_Int4         StartPrimKeyLen,
							  tsp00_KeyPtr       pStopPrimKey,
							  tsp00_Int4         StopPrimKeyLen,
							  tsp00_Int4        &NumPrimKeys);

/*---------------------------------------------------------------------------*/

externCpp void
bd403MergePrimKeysInInvLists (cbd300_InvCurrent  &InvCurrent,
							  cbd300_InvCurrent  &DestCurrent,
							  tsp00_KeyPtr        pStartSecKey,
							  tsp00_Int4          StartSecKeyLen, 
							  tsp00_KeyPtr        pStopSecKey,
							  tsp00_Int4          StopSecKeyLen,
							  tsp00_KeyPtr        pStartPrimKey,
							  tsp00_Int4          StartPrimKeyLen,
							  tsp00_KeyPtr        pStopPrimKey,
							  tsp00_Int4          StopPrimKeyLen,
							  tsp00_Int4         &NumPrimKeys);

#endif  /* HBD403_H */
