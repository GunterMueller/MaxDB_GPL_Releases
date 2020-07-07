/*!
  @file           hbd402.h
  @author         AlexanderK
  @author         UweH
  @author         TorstenS
  @ingroup        Index
  @brief          Selects

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
*/
#ifndef HBD402_H
#define HBD402_H

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"


externCpp void
bd402NextFromInvTree (
    cbd300_InvCurrent   &InvCurrent,
    bool                 bIncludeStartSecKey,
    tsp00_KeyPtr         pSecKey,
    tsp00_Int2          &SecKeyLen);

/*---------------------------------------------------------------------------*/

externCpp void
bd402SearchIndexForQualifiedRecords (
    cbd300_InvCurrent&          InvCurrent,
    tgg00_FileId&               PrimFileId,
    tsp00_KeyPtr                pCurrSecKey,
    tgg00_KeyLen&               currSecKeyLen,
    const tsp00_KeyPtr          pStopSecKey,
    const tsp00_Int4            stopSecKeyLen,
    const tsp00_KeyPtr          pFrozenStartPrimKey,
    const tsp00_Int4            frozenStartPrimKeyLen,
    const tsp00_KeyPtr          pStopPrimKey,
    const tsp00_Int4            stopPrimKeyLen,
    tsp00_KeyPtr                pCurrPrimKey,
    tgg00_KeyLen&               currPrimKeyLen,
    const tsp00_Int4            RecBufSize,
    tsp00_MoveObjPtr            pRecBuf,
    tgg00_SelectFieldsParam&    SelectParam,
    tgg00_StackDesc&            StackDesc,
    tgg00_LockReqMode&          GrantedLock,
    bool&                       bPrimaryFileErr);


/*---------------------------------------------------------------------------*/

externCpp void
bd402SearchResultFileForQualifiedRecords (
    cbd300_InvCurrent        &InvCurrent,
    tgg00_FileId             &PrimFileId,
    tsp00_KeyPtr              pStartKey,
    tgg00_KeyLen             &StartKeyLen,
    tsp00_Int4                RecBufSize,
    tsp00_MoveObjPtr          pRecBuf,
    tgg00_SelectFieldsParam  &SelectParam,
    tgg00_StackDesc          &StackDesc,
    tgg00_LockReqMode        &GrantedLock);

/*---------------------------------------------------------------------------*/


#endif  /* HBD402_H */
