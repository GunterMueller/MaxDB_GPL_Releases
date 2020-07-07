/*!******************************************************************************

  @file         hkb501_1.h

  ------------------------------------------------------------------------------

  @author       FerdiF

  @brief        KB_ObjShare_lock

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

\endif

********************************************************************************/

#ifndef HKB501_1_H
#define HKB501_1_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "SAPDB/FileDirectory/FileDir_FileNo.hpp"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal pasbool k501GetShareLockInfo (
    tgg00_TransContext    &Trans,
    tsp00_Addr            &contextAddr,
    FileDir_FileNo        &fileno,
    tgg92_KernelOid       &oid,
    tgg91_TransNo         &transno,
    tsp00_TaskId          &taskidOfTrans,
    pasbool               &isExclusive,
    pasbool               &isHolder);

#endif

