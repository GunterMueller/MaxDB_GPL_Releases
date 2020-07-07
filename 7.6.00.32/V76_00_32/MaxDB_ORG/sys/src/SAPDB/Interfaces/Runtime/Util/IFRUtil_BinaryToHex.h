/*!
  @file           IFRUtil_BinaryToHex.h
  @author         D039759
  @ingroup        Util
  @brief          Binary to Hex dump conversion needed by ODBC.
  @see

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (C) 2001-2005 SAP AG

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
*/
#ifndef IFRUTIL_BINARYTOHEX_H
#define IFRUTIL_BINARYTOHEX_H

#include "hsp78_0.h"

#ifdef IFR_NAMESPACE
namespace IFR_NAMESPACE {
#endif


tsp78ConversionResult IFRUtil_HexToBinary(void *destbuffer,
                                          tsp00_Uint4 destbufferlengthinbytes,
                                          tsp00_Uint4 *destbyteswritten,
                                          const tsp77encoding *sourceencoding,
                                          const void *srcbuffer,
                                          tsp00_Uint4  srcbufferlengthinbytes,
                                          tsp00_Uint4 *srcbytesparsed);
                                          
tsp78ConversionResult IFRUtil_BinaryToHex(const tsp77encoding *targetencoding,
                                          void *destbuffer,
                                          tsp00_Uint4 destbufferlengthinbytes,
                                          tsp00_Uint4 *destbyteswritten,
                                          const void *srcbuffer,
                                          tsp00_Uint4  srcbufferlengthinbytes,
                                          tsp00_Uint4 *srcbytesparsed);


#ifdef IFR_NAMESPACE
};
#endif

#endif
