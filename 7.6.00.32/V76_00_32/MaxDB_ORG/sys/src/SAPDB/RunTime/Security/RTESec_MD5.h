/*!======================================================================

  @ingroup      Sec
  @file         RTESec_MD5.h
  @author       ChristophB

  @brief

  @since        2004-08-18  13:43
  @sa


  =======================================================================

  \if EMIT_LICENCE

    This code implements the MD5 message-digest algorithm.
    The algorithm is due to Ron Rivest.  This code was
    written by Colin Plumb in 1993, no copyright is claimed.
    This code is in the public domain; do with it what you wish.

    Equivalent code is available from RSA Data Security, Inc.
    This code has been tested against that, and is equivalent,
    except that you don't need to include two pages of legalese
    with every copy.

    To compute the message digest of a chunk of bytes, declare an
    MD5Context structure, pass it to MD5Init, call MD5Update as
    needed on buffers full of bytes, and then call MD5Final, which
    will fill a supplied 16-byte array with the digest.

    Changed so as no longer to depend on Colin Plumb's `usual.h' header
    definitions; now uses stuff from dpkg's config.h.
     - Ian Jackson <ijackson@nyx.cs.du.edu>.

  \endif

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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
  ----------------------------------------------------------------------------*/

#ifndef RTESEC_MD5_H
#define RTESEC_MD5_H RTESEC_MD5_H

/*===========================================================================*
 *  INCLUDES                                                               *
 *===========================================================================*/
#include "gsp01.h"
#include "RunTime/RTE_Types.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if \
defined _WIN32 && (defined _M_IX86 || defined _M_IA64) || \
defined linux && (defined i386 || defined ia64 || defined __x86_64__) || \
defined __osf__

#undef HAS_BIGENDIAN_BYTEORDER
#define HAS_LITTLEENDIAN_BYTEORDER HAS_LITTLEENDIAN_BYTEORDER

#else

#define HAS_BIGENDIAN_BYTEORDER HAS_BIGENDIAN_BYTEORDER
#undef HAS_LITTLEENDIAN_BYTEORDER

#endif

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

typedef unsigned int MD5_UINT4;
typedef unsigned char MD5_UINT1;

struct MD5Context {
	MD5_UINT4 state[4];
	MD5_UINT4 count[2];
	MD5_UINT4 in[16];
};

typedef struct MD5Context MD5_CTX;

/*===========================================================================*
 *  PROTOTYPES                                                               *
 *===========================================================================*/

void RTESec_MD5Init (struct MD5Context *);
void RTESec_MD5Update (struct MD5Context *, MD5_UINT1 const *, unsigned int);
void RTESec_MD5Final (MD5_UINT1 digest[16], struct MD5Context *);

#endif
