/*!======================================================================

  @ingroup      Sec
  @file         RTESec_HMACMD5.h
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

#ifndef RTESEC_HMACMD5_H
#define RTESEC_HMACMD5_H RTESEC_HMACMD5_H

/*
 * The following sample C source code calls the source code in the MD5
 * specification [MD5] and is derived from the source code in [HMAC].
 * It is needed by the SCRAM source code in the next section.
 *
 * The client may call HMACMD5Precalc() to save the intermediate
 * HMAC result for later use in HMACMD5Import().  This can be used
 * by a CRAM-MD5 [CRAM-MD5] or a SCRAM-MD5 [SCRAM-MD5] client to save
 * an intermediate result suitable for use with any server without
 * saving the plain-text passphrase.
 */

/*===========================================================================*
 *  INCLUDES                                                               *
 *===========================================================================*/
#include "gsp01.h"
#include "RunTime/RTE_Types.h"
#include "RunTime/Security/RTESec_MD5.h"

#define HMACMD5_SIZE 16

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*! intermediate MD5 context */
typedef struct HMACMD5_CTX_s {
	MD5_CTX ictx;
	MD5_CTX octx;
} HMACMD5_CTX;

/*! intermediate HMAC state values stored in network byte order (Big Endian) */
typedef struct HMACMD5_STATE_s {
	MD5_UINT4 istate[4];
	MD5_UINT4 ostate[4];
} HMACMD5_STATE;

/*===========================================================================*
 *  PROTOTYPES                                                               *
 *===========================================================================*/

/*! One step hmac computation digest may be same as text or key */
void HMACMD5 (const unsigned char *text, int text_len,
               const unsigned char *key, int key_len,
               unsigned char digest[HMACMD5_SIZE]);

/*! create context from key */
externC void HMACMD5Init (HMACMD5_CTX *hmac, const unsigned char *key, int key_len);

/*! precalculate intermediate state from key */
externC void HMACMD5Precalc (HMACMD5_STATE *hmac, const unsigned char *key, int key_len);

/*! initialize context from intermediate state */
externC void HMACMD5Import (HMACMD5_CTX *hmac, HMACMD5_STATE *state);

#define HMACMD5Update(hmac,text,text_len) \
	RTESec_MD5Update (&(hmac)->ictx, (text), (text_len))

/*! finish hmac from intermediate result. Intermediate result is erased. */
externC void HMACMD5Final (unsigned char digest[HMACMD5_SIZE], HMACMD5_CTX *hmac);

#endif
