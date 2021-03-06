
/*******************************************************************************

  module:       hta01_1.h

  ------------------------------------------------------------------------------

  responsible:  ElkeZ

  purpose:      Test_Procedures

  lastChanged:  2003-02-11

  generatedBy:  $TOOL/bin/ptocProto.py

  ------------------------------------------------------------------------------

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

********************************************************************************/

#ifndef HTA01_1_H
#define HTA01_1_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x38\x6b\xbd\x8e\x68\xa0\xca\xf7\x86\x8f\x3f\x5b\x14\x8a\x4e\xa1'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "vsp001.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void t01packet (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tsp1_packet_ptr          packet_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01packet_part (
    tgg00_Debug_Param        debug,
    tsp1_part             &  packet_part);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01segment (
    tgg00_Debug_Param        debug,
    tsp1_segment          &  segm);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

