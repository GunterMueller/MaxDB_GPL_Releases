
/*******************************************************************************

  module:       hbd72.h

  ------------------------------------------------------------------------------

  responsible:  JuergenP

  purpose:      filestatistic

  lastChanged:  2003-01-31

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

#ifndef HBD72_H
#define HBD72_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x73\x9b\xa4\x0d\x75\x10\x51\xe1\x18\x90\xf8\x33\x06\x6b\xfe\xb8'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void bd72SampleMultiColumnStatistic (
    tgg00_MessBlock       &  MessBlock,
    tbd_current_tree      &  Current,
    tbd_current_tree      &  TempCurrent,
    tsp00_Int4               NumberOfSampleLeaves,
    tsp00_Int4            &  NumberOfLeaves,
    tsp00_Int4            &  NumberOfRecords,
    tgg00_ColumnDistinctValues  &  DistinctValues);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd72InitSampleInfo (
    tgg00_FileId          &  tree,
    tgg00_SampleInfo      &  info,
    tbd_stat_aux_vars     &  stat_aux_vars);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd72SampleInfoEpilog (
    tgg00_SampleInfo      &  info,
    pasbool                  scol_only,
    tbd_stat_aux_vars     &  stat_aux_vars);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

