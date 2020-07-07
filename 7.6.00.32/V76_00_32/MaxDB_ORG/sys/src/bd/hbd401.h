/****************************************************************************

  module      : hbd401.h

  -------------------------------------------------------------------------

  responsible : UweH

  special area: Inverted Lists Accesses And Statistics
  description : 


  last changed: 1999-08-11  20:45
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


#ifndef HBD401_H
#define HBD401_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externCpp void 
bd401CalculatePageCount (cbd300_InvCurrent   &InvCurrent,
						tsp00_KeyPtr         pStartKey,
						tsp00_Int4           StartKeyLen,
						tsp00_KeyPtr         pStopKey,
						tsp00_Int4           StopKeyLen,
						const bool           bCountKeys,
						tsp00_Int4          &PageCnt,
						tsp00_Int4          &AllPageCnt,
						tsp00_Int4          &PrimKeyCnt);

/*---------------------------------------------------------------------------*/

externCpp void 
bd401GetFromInvTree (cbd300_InvCurrent          &InvCurrent,
					 tsp00_KeyPtr               pSecKey,
					 tsp00_Int4                 SecKeyLen,
					 tsp00_KeyPtr               pStartPrimKey,
					 tsp00_Int2                 StartPrimKeyLen,
					 tsp00_KeyPtr               pStopPrimKey,
					 tsp00_Int4                 StopPrimKeyLen,
                     tsp00_KeyPtr               pCurrPrimKey,
                     tsp00_Int2                 &CurrPrimKeyLen,
					 bool                       bIncludeStartKey,
					 bool   	                bCountOnly,
                     tgg00_SelectFieldsParam   *pSelectFields,
                     tgg00_StackDesc           *pStackDescription,
					 tgg00_LockReqMode          WantedLock, 
					 tgg00_KeylistBuf           &KeyListBuf,
 					 tsp00_Int4                 &NumPrimKeys,
					 tsp00_Int4                 &ListLen);

/*---------------------------------------------------------------------------*/

externCpp void 
bd401InvStatistics (cbd300_InvCurrent &InvCurrent,
					bool               bWithSelectivity,
					tgg00_SampleInfo  &StatInfo);

/*---------------------------------------------------------------------------*/

externCpp void
bd401CountClusteredPages(
    cbd300_InvCurrent     &  InvCurrent,
    tsp00_Int4            &  LeafPageCount,
    tsp00_Int4            &  ClusteredBlockCount);

/*---------------------------------------------------------------------------*/

externCpp void
bd401StatisticsUpdate (cbd300_InvCurrent &InvCurrent,
					   tsp00_Int4        &NumLeaves,
					   tsp00_Int4        &AvgPrimKeyCountPerPage,
					   const bool         bOnlyCountLeaves);     /* PTS 1107153 AK 17/07/2000 */

/*---------------------------------------------------------------------------*/

#endif  /* HBD401_H */
