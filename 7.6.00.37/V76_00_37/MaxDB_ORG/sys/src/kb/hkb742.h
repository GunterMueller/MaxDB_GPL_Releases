/*!******************************************************************************

  @file         hkb742.h

  ------------------------------------------------------------------------------

  @author       ThomasA

  @brief        KB_ParallelJoin

\if EMIT_LICENCE

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

\endif

********************************************************************************/

#ifndef HKB742_H
#define HKB742_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x13\xed\x15\x26\x84\x7a\x4f\xf7\x32\xcf\xf1\x8d\x3e\x49\x84\x85'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "ggg07.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void k742CloseIterator (
    tgg00_VoidPtr                        pIter);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tgg00_VoidPtr k742CreateInvIterator (
    tgg00_MessBlock       VAR_VALUE_REF  m,
    tgg00_SelectFieldsParam  VAR_VALUE_REF  sel,
    tgg07_select_param    VAR_VALUE_REF  selrec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k742DestroyIterator (
    tgg00_VoidPtr                        pIter);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tgg00_BasisError k742Next (
    tgg00_VoidPtr                        pIter);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k742OpenIterator (
    tgg00_VoidPtr                        pIter,
    tgg00_SelectFieldsParam  VAR_VALUE_REF  sel,
    tgg07_select_param    VAR_VALUE_REF  selrec,
    tgg00_BdInvSet        VAR_ARRAY_REF  bdInvSet);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

