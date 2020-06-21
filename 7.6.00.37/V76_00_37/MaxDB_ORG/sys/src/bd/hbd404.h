/****************************************************************************

  module      : hbd404.h

  -------------------------------------------------------------------------

  author      : AlexanderK
  responsible : UweH

  special area: Indexhandling
  description : description ...


  last changed: 1999-10-05  14:57
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


#ifndef HBD404_H
#define HBD404_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"
#include "gbd300.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externCpp void					  
bd404AggrFuncOnIndex (cbd300_InvCurrent       &InvCurrent,
					  tbd_current_tree        &AuxCurrent,
					  tsp00_KeyPtr             pStartSecKey, 
					  tsp00_Int2              &StartSecKeyLen, 
					  tsp00_KeyPtr             pStopSecKey, 
					  tsp00_Int2               StopSecKeyLen,
					  tsp00_KeyPtr             pStartPrimKey,
					  tsp00_Int2               StartPrimKeyLen,
					  tsp00_KeyPtr             pStopPrimKey,
					  tsp00_Int2               StopPrimKeyLen,
					  tgg00_AggrDescrRec      &AggrFieldDescr,
					  tgg00_SelectFieldsParam &SelectParam,
					  tgg00_StackDesc         &StackDesc,
					  tgg00_Rec               &AuxBuffer, 
					  tsp00_Buf               &ResultBuffer, 
					  bool                     bDistinctAccessExists, 
					  bool                     bOneListOnly,
					  bool                     bWarnOfNullValues);

/*---------------------------------------------------------------------------*/


#endif  /* HBD404_H */
