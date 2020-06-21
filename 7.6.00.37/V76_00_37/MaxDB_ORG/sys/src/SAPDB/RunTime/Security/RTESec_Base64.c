/*!
  @defgroup     Sec     Security
*/
/*!
  @ingroup      Sec
  @file         RTESec_Base64.c
  @author       ChristophB

  @brief        

  @since        2004-08-18  13:43
  @sa           


  =======================================================================

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
 
 ============================================================================*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

 /*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

/* base64 encoding table derived from RFC 1113 */
static const char enc[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

/* base64 decoding table */
static const char dec[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x3f,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
	0x3c, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
	0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
	0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
	0x17, 0x18, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
	0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

static int enc3to4 (unsigned char in[3], unsigned char out[4], int len);
static int dec3from4 (unsigned char in[4], unsigned char out[3]);

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

int Base64Enc (void *in, void *out, int len) {
	int i, cnt;

	/* calc len of encoded text */
	cnt = len / 3;
	if (len % 3)
		cnt++;

	if (out == 0)
		return (4 * cnt);

	for (i = 0; i < cnt; i++)
		if (i + 1 == cnt && len % 3 != 0)
			enc3to4 ((unsigned char *) in + 3 * i, (unsigned char *) out + 4 * i, len % 3);
		else
			enc3to4 ((unsigned char *) in + 3 * i, (unsigned char *) out + 4 * i, 3);

	return (4 * i);
}

int Base64Dec (void *in, void *out, int size) {
	int i, cnt, len;

	/* find len of encoded text */
	for (i = 0; *((unsigned char *) in + i) != '=' && *((unsigned char *) in + i) != '\0'; i++);

	/* calc len of binary text */
	cnt = i / 4;
	len = 3 * cnt;

	if (i % 4) {
		len += dec3from4 ((unsigned char *) in + 4 * cnt, 0);
		cnt++;
	}

	if (out == 0)
		return (len);

	/* do not copy out if specified size is to small */
	if (size != 0 && size < len)
		return 0;

	len = 0;
	for (i = 0; i < cnt; i++)
		len += dec3from4 ((unsigned char *) in + 4 * i, (unsigned char *) out + 3 * i);

	return (len);
}

/*===========================================================================*
 *                          LOCAL FUNCTIONS
 *===========================================================================*/
static int enc3to4 (unsigned char in[3], unsigned char out[4], int len) {
	out[0] = enc[in[0] >> 2];
	out[1] = enc[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];

	if (len > 1)
		out[2] = enc[((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)];
	else
		out[2] = (unsigned char) '=';

	if (len > 2)
		out[3] = enc[in[2] & 0x3f];
	else
		out[3] = (unsigned char) '=';

	return 4;
}

static int dec3from4 (unsigned char in[4], unsigned char out[3]) {
	int len;

	len = 
	(in[0] == '=') ? 0 :
	(in[1] == '=') ? 0 :
	(in[2] == '=') ? 1 :
	(in[3] == '=') ? 2 :
	3;

	if (out == 0)
		return (len);	

	out[0] = (unsigned char) (dec[in[0]] << 2 | dec[in[1]] >> 4);
	out[1] = (unsigned char) (dec[in[1]] << 4 | dec[in[2]] >> 2);
	out[2] = (unsigned char) (((dec[in[2]] << 6) & 0xc0) | dec[in[3]]);

	return (len);
}
