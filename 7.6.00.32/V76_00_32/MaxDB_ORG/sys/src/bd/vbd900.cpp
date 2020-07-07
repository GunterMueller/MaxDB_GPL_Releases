/****************************************************************************

  module      : vbd900.cpp

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: BD_Object_Constants_And_Types
  description : 


  version     : 7.2.0.0
  last changed: 2001-03-02  13:11
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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gbd900.h"
#include "gbd910.h"
 
#include "hgg01.h"
#include "hgg01_1.h"
#include "hgg06.h"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"

#if COMPILEMODE_MEO00 >= SLOW_MEO00 
#include "hta99.h"
#endif

#include "KernelCommon/Kernel_OpMsg.hpp"        /* PTS 1114951 FF 2002-04-19 */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define HUNDERT_PERCENT   100

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

static void
bd900_NewDumpPage (tgg00_VfFileref      &HostFile,
				   tsp00_Int4            BufferSize,
				   tsp00_Page           &Buffer,
				   tsp00_Int4           &OutPno,
				   tsp00_Int4           &OutPos,
				   tsp00_VfReturn       &HostError,
				   tsp00_ErrText        &ErrText,
				   const tsp00_Int2     &DumpCode,  
				   char *                DumpMark,
                   tgg00_BasisError      MoveError)
{

	/* start at a new dump page */
	g01new_dump_page (HostFile, Buffer, OutPno, OutPos, HostError, ErrText);
    if (vf_ok != HostError) return;
	
	SAPDB_RangeMove( __FILE__, 1,    
		DUMPMARKSIZE_BD900, BufferSize, DumpMark, 1,
		Buffer, OutPos, DUMPMARKSIZE_BD900 , MoveError);
    if ( e_ok == MoveError )
    {
        OutPos += DUMPMARKSIZE_BD900;

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &DumpCode, sizeof(DumpCode));
        OutPos         += sizeof(DumpCode);
    }
}

/*===========================================================================*
 *  IMPLEMENTATION OF METHODS DECLARED IN gbd900.h                           * 
 *===========================================================================*/



// end of vbd900.cpp
