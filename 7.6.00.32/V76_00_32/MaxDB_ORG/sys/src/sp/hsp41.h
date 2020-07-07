
/*******************************************************************************

  module:       hsp41.h

  ------------------------------------------------------------------------------

  responsible:  

  purpose:      Conversions Into VDN-Numbers

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

#ifndef HSP41_H
#define HSP41_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xb5\x8e\x76\x36\x29\xb5\x55\x3d\x32\x52\xf8\x57\x9c\xa6\xb2\x76'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void s41psint (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    integer                              frac,
    tsp00_Int2                           source,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s41plint (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    integer                              frac,
    tsp00_Int4                           source,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s41p4int (
    void                  *  buf,
    tsp00_Int4                           pos,
    tsp00_Int4                           source,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s41p8int (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    integer                              frac,
    tsp00_Longint                        source,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s41psuns (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    integer                              frac,
    tsp00_Int2                           source,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s41pluns (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    integer                              frac,
    tsp00_Int4                           source,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s41psrel (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    integer                              frac,
    tsp00_Shortreal                      source,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s41plrel (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    integer                              frac,
    tsp00_Longreal                       source,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s41pdec (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    integer                              frac,
    tsp00_Decimal         VAR_ARRAY_REF  source,
    integer                              slen,
    integer                              sfrac,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s41pzone (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer                              len,
    integer                              frac,
    tsp00_Zoned           VAR_ARRAY_REF  source,
    integer                              slen,
    integer                              sfrac,
    tsp00_NumError        VAR_VALUE_REF  res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s41pbyte (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer               VAR_VALUE_REF  len,
    void                  *  source,
    tsp00_Int4                           spos,
    integer                              slen,
    pasbool               VAR_VALUE_REF  invalid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void s41p1byte (
    void                  *  buf,
    tsp00_Int4                           pos,
    integer               VAR_VALUE_REF  len,
    void                  *  source,
    tsp00_Int4                           spos,
    integer                              slen,
    pasbool               VAR_VALUE_REF  invalid);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

