/*!========================================================================
  @ingroup      Sec
  @file         RTESec_HMACMD5.c
  @author       ChristophB

  @brief

  @since        2004-08-18  16:34
  @sa

  ==========================================================================

  \if EMIT_LICENCE


	hmac-md5.c -- HMAC MD5 Keyed-Hashing by Chris Newman
	derived from RFC 2104 by H. Krawczyk, M. Bellare, R.Canetti

	The following sample C source code calls the source code in the MD5
	specification [MD5] and is derived from the source code in [HMAC].
	It is needed by the SCRAM source code in the next section.

	The client may call HMACMD5Precalc() to save the intermediate
	HMAC result for later use in HMACMD5Import().  This can be used
	by a CRAM-MD5 [CRAM-MD5] or a SCRAM-MD5 [SCRAM-MD5] client to save
	an intermediate result suitable for use with any server without
	saving the plain-text passphrase.

  \endif
 ============================================================================*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "RunTime/Security/RTESec_MD5.h"
#include "RunTime/Security/RTESec_HMACMD5.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/* MD5 block size */
#define BLOCK_SIZE 64

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#ifdef HAS_BIGENDIAN_BYTEORDER
static unsigned int HToNInt (unsigned int val) {
	unsigned char *i;
	unsigned char i0;
	unsigned char i1;

	i = (unsigned char *)(&val);

	i0 = i[0];
	i1 = i[1];
	i[0] = i[3];
	i[1] = i[2];
	i[3] = i0;
	i[2] = i1;

	return (val);
}
#define NToHInt(val) HToNInt (val)

#elif defined HAS_LITTLEENDIAN_BYTEORDER

#define NToHInt(val) (val)
#define HToNInt(val) (val)

#else

#error undefined BYTEORDER

#endif

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

void HMACMD5Init(HMACMD5_CTX *hmac, const unsigned char *key, int key_len)
{
	unsigned char k_pad[BLOCK_SIZE];    /* padded key */
	int i;

	/* if key longer than BLOCK_SIZE bytes reset to MD5 (key) */
	if (key_len > BLOCK_SIZE) {
		RTESec_MD5Init (&hmac->ictx);
		RTESec_MD5Update (&hmac->ictx, key, key_len);
		RTESec_MD5Final (k_pad, &hmac->ictx);
		key = k_pad;
		key_len = HMACMD5_SIZE;
	}

	/* XOR padded key with inner pad value */
	for (i = 0; i < key_len; i++) {
		k_pad[i] = key[i] ^ 0x36;
	}

	while (i < BLOCK_SIZE) {
		k_pad[i++] = 0x36;
	}

	/* Begin inner MD5 */
	RTESec_MD5Init(&hmac->ictx);
	RTESec_MD5Update(&hmac->ictx, k_pad, BLOCK_SIZE);

	/* XOR padded key with outer pad value */
	for (i = 0; i < BLOCK_SIZE; ++i) {
		k_pad[i] ^= (0x36 ^ 0x5c);
	}

	/* Begin outer MD5 */
	RTESec_MD5Init(&hmac->octx);
	RTESec_MD5Update(&hmac->octx, k_pad, BLOCK_SIZE);

	/* clean up workspace */
	memset(k_pad, 0, BLOCK_SIZE);
}

void HMACMD5Final (unsigned char digest[HMACMD5_SIZE], HMACMD5_CTX *hmac)
{
	/* finish inner MD5 */
	RTESec_MD5Final(digest, &hmac->ictx);

	/* finish outer MD5 */
	RTESec_MD5Update(&hmac->octx, digest, HMACMD5_SIZE);
	RTESec_MD5Final(digest, &hmac->octx);

	/* RTESec_MD5Final zeros context */
}

void HMACMD5 (const unsigned char *text, int text_len,
               const unsigned char *key, int key_len,
               unsigned char digest[HMACMD5_SIZE])
{
	HMACMD5_CTX hmac;

	HMACMD5Init (&hmac, key, key_len);
	HMACMD5Update (&hmac, text, text_len);
	HMACMD5Final (digest, &hmac);
}

void HMACMD5Precalc (HMACMD5_STATE *ctx,
					   const unsigned char *pass, int passlen)
{
	HMACMD5_CTX hctx;

	if (passlen == 0) passlen = strlen((const char *) pass);
		HMACMD5Init (&hctx, pass, passlen);

	ctx->istate[0] = HToNInt(hctx.ictx.state[0]);
	ctx->istate[1] = HToNInt(hctx.ictx.state[1]);
	ctx->istate[2] = HToNInt(hctx.ictx.state[2]);
	ctx->istate[3] = HToNInt(hctx.ictx.state[3]);
	ctx->ostate[0] = HToNInt(hctx.octx.state[0]);
	ctx->ostate[1] = HToNInt(hctx.octx.state[1]);
	ctx->ostate[2] = HToNInt(hctx.octx.state[2]);
	ctx->ostate[3] = HToNInt(hctx.octx.state[3]);
	memset(&hctx, 0, sizeof (hctx));
}


void HMACMD5Import (HMACMD5_CTX *hctx, HMACMD5_STATE *ctx)
{
	hctx->ictx.state[0] = NToHInt(ctx->istate[0]);
	hctx->ictx.state[1] = NToHInt(ctx->istate[1]);
	hctx->ictx.state[2] = NToHInt(ctx->istate[2]);
	hctx->ictx.state[3] = NToHInt(ctx->istate[3]);
	hctx->octx.state[0] = NToHInt(ctx->ostate[0]);
	hctx->octx.state[1] = NToHInt(ctx->ostate[1]);
	hctx->octx.state[2] = NToHInt(ctx->ostate[2]);
	hctx->octx.state[3] = NToHInt(ctx->ostate[3]);
	hctx->ictx.count[0] = hctx->octx.count[0] = BLOCK_SIZE << 3;
	hctx->ictx.count[1] = hctx->octx.count[1] = 0;
}
