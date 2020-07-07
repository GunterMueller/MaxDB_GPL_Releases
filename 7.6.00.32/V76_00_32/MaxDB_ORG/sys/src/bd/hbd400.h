/****************************************************************************

  module      : hbd400.h

  -------------------------------------------------------------------------

  responsible : UweH

  special area: Inverted Lists
  description : Add / Delete


  last changed: 1999-03-08  18:43
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1998-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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


#ifndef HBD400_H
#define HBD400_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	inline
#endif

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externCpp void 
bd400AddToInvTree (cbd300_InvCurrent &InvCurrent,
                   tsp00_KeyPtr       SecKey,
				   tsp00_Int4         SecKeyLen,
                   tsp00_KeyPtr       PrimKey,
				   tsp00_Int4         PrimKeyLen,
                   bool              &SecKeyFound);

/*---------------------------------------------------------------------------*/

externCpp void 
bd400DelFromInvTree (cbd300_InvCurrent       &InvCurrent,
                     tsp00_KeyPtr             SecKey,
				     tsp00_Int4               SecKeyLen,
                     tsp00_KeyPtr             PrimKey,
				     tsp00_Int4               PrimKeyLen,
					 bool                     unlock,
                     tbd00_DelInvReason_Param callReason);

/*---------------------------------------------------------------------------*/

externCpp void 
bd400DropInvTree (cbd300_InvCurrent &InvCurrent);

/*---------------------------------------------------------------------------*/

externCpp void 
bd400TestUniqueInv (cbd300_InvCurrent &InvCurrent,
                    tsp00_KeyPtr       SecKey,
				    tsp00_Int4         SecKeyLen,
                    tsp00_KeyPtr       PrimKey,
				    tsp00_Int4         PrimKeyLen);

/*---------------------------------------------------------------------------*/

externCpp void 
bd400Verify (cbd300_InvCurrent  &InvCurrent, 
			 bool			 	 bIsCold,
             bool                bWithExtendedCheck,
			 tsp00_Int4         &NumPages,
             tgg00_IndexCounter &counter);

/*---------------------------------------------------------------------------*/

externCpp void 
bd400ExtractIndex ( 
           cbd300_InvCurrent      &invCurrent,
           const tsp00_VFilename  &hostFileName );

/*===========================================================================*
 *  FUNCTION PROTOTYPES OMS KEY MANAGEMENT                                   *
 *===========================================================================*/

externCpp void 
bd400AddToOmsInvTree (cbd300_InvCurrent &InvCurrent,
                      tsp00_KeyPtr       pSecKey,
				      tsp00_Int4         SecKeyLen,
                      tsp00_KeyPtr       pPrimKey,
				      tsp00_Int4         PrimKeyLen,
				      tsp00_Int4         FoundPrimKeySize,
                      tsp00_KeyPtr       pFoundPrimKey,
				      tsp00_Int4        &FoundPrimKeyLen);

/*---------------------------------------------------------------------------*/

externCpp void 
bd400DelFromOmsInvTree (cbd300_InvCurrent &InvCurrent,
                        tsp00_KeyPtr       pSecKey,
                        tsp00_Int4         SecKeyLen,
                        tsp00_KeyPtr       pPrimKey,
                        tsp00_Int4         PrimKeyLen,
                        tsp00_Int4         FoundPrimKeySize,
                        tsp00_KeyPtr       pFoundPrimKey,
                        tsp00_Int4         FoundPrimKeyToCompareSize,
                        tsp00_Int4        &FoundPrimKeyLen);

/*---------------------------------------------------------------------------*/

externCpp void 
bd400GetFromOmsInvTree (cbd300_InvCurrent   &InvCurrent,
					    tsp00_KeyPtr         pSecKey,
					    tsp00_Int4           SecKeyLen,
					    tsp00_Int4           PrimKeySize,
					    tsp00_KeyPtr         pPrimKey,
						tsp00_Int4          &PrimKeyLen);

/*---------------------------------------------------------------------------*/
   
#endif  /* HBD400_H */
