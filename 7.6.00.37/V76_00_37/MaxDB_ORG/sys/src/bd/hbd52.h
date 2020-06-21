
/*******************************************************************************

  module:       hbd52.h

  ------------------------------------------------------------------------------

  responsible:  JuergenP

  purpose:      branchhandling

  lastChanged:  1999-07-30

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

#ifndef HBD52_H
#define HBD52_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\020\215$o\240\277R\202\355\253\243rda\360\277'
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

externPascal void bd52GetSepKey (
    tbd_nodeptr           &  nptr,
    tsp00_Int4               RecIndex,
    tgg00_Lkey            &  SepKey,
    tgg00_TransContextPtr     t);

externPascal void bd52PutSeparatorIntoNode (
    tbd_nodeptr           &  nptr,
    tsp00_Int4               RecIndex,
    tbd00_Separator       &  Separator,
    tgg00_TransContextPtr     t);

externPascal void bd52PutSepLeafCount (
    tgg00_RecPtr             pRec,
    tsp00_Int4               LeafCount);

externPascal tsp00_PageNo bd52SubtreePno (
    tbd_nodeptr           &  nptr,
    tsp00_Int4               RecIndex);


#endif

