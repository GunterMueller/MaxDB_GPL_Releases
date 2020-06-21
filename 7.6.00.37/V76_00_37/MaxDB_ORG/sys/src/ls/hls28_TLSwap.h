/*! 
  -----------------------------------------------------------------------------
 
  module: hls28_TLSwap.h
 
  -----------------------------------------------------------------------------
 
  responsible:  SteffenS

  special area: Replication Server
 
  description:  Definition of exported functions for swap on pages for TABLELOAD
			    for single table
                
  version:      7.4.

  -----------------------------------------------------------------------------
 
  Copyright (c) 2000-2005 SAP AG
 
  -----------------------------------------------------------------------------



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




*/
#ifndef HLS28_TLSWAP
#define HLS28_TLSWAP

#include "gls00.h"

/*
  -----------------------------------------------------------------------------
 
  function:     ls28SwapHeader
  -----------------------------------------------------------------------------

  description:  Swaps all values in special TE/TL file page header.

  attention:    Modifies directly the values delivered with the pointer.

  arguments:    SourceSwapKind  [IN]     - Swap kind while tableextracting
                Header          [IN|OUT] - pointer; structure with values to swap

  returnvalue:  No
  -----------------------------------------------------------------------------
*/
void
ls28SwapHeader(tsp00_SwapKind SourceSwapKind, tls26_DescPageHeader *pHeader);



/*
  -----------------------------------------------------------------------------
 
  function:     ls28SwapPageValues
  -----------------------------------------------------------------------------

  description:  Swaps all values (which need to be swapped) on a number of db page

  arguments:    pszBuffer  [IN]  - pointer to first page (char ptr)
                lPageCount [IN]  - count of page to be processed
				lPageSize  [IN]  - size of single page
				pTLRec     [IN]  - structure holding information for
                                    executing TABLELOAD for a single table
  returnvalue:  No
  -----------------------------------------------------------------------------
*/
void
ls28SwapPageValues(tsp00_Addr      pszBuffer,
				   tsp00_Int4      lPageSize,
				   tls00_TloadRec *pTLRec);


/*
  -----------------------------------------------------------------------------
  function:     ls28SwapTableDescription
  -----------------------------------------------------------------------------
*/
void
ls28SwapTableDescription(tsp7_table_description & TD,
		  			     tsp00_SwapKind_Enum      SSwap);



/*
  -----------------------------------------------------------------------------
  function:     ls28SwapIndexDescription
  -----------------------------------------------------------------------------
*/
void
ls28SwapIndexDescription(tsp7_index_description & ID,
		  			     tsp00_SwapKind_Enum      SSwap);

#endif