
/*******************************************************************************

  module:       hsp40.h

  ------------------------------------------------------------------------------

  responsible:  

  purpose:      Conversions from VDN-Numbers

  lastChanged:  2002-12-16

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

#ifndef HSP40_H
#define HSP40_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x16\x2b\x02\xdb\xa4\x7b\xad\xd4\x25\x97\x5c\x61\x6f\x1b\x1b\xdc'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void s40check (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40gsint (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    tsp00_Int2            VAR_VALUE_REF  dest,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40gsuns (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    tsp00_Int2            VAR_VALUE_REF  dest,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40glint (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    tsp00_Int4            VAR_VALUE_REF  dest,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40g4int (
    void                  *  buf,
    tsp00_Int4                           pos,
    tsp00_Int4            VAR_VALUE_REF  dest,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40g8int (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    tsp00_Longint         VAR_VALUE_REF  dest,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40gluns (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    tsp00_Int4            VAR_VALUE_REF  dest,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40gsrel (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    tsp00_Shortreal       VAR_VALUE_REF  dest,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40glrel (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    tsp00_Longreal        VAR_VALUE_REF  dest,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40gdec (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    tsp00_Decimal         VAR_ARRAY_REF  dest,
    integer                              dlen,
    integer                              dfrac,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40gzone (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    integer                              frac,
    tsp00_Zoned           VAR_ARRAY_REF  dest,
    integer                              dlen,
    integer                              dfrac,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40glzon (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    integer                              frac,
    tsp00_Zoned           VAR_ARRAY_REF  dest,
    integer                              dlen,
    integer                              dfrac,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40glszo (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    integer                              frac,
    tsp00_Zoned           VAR_ARRAY_REF  dest,
    integer                              dlen,
    integer                              dfrac,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40gtszo (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    integer                              frac,
    tsp00_Zoned           VAR_ARRAY_REF  dest,
    integer                              dlen,
    integer                              dfrac,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s40gbyte (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    void                  *  dest,
    tsp00_Int4                           dpos,
    integer                              dlen,
    pasbool               VAR_VALUE_REF  truncated);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif
