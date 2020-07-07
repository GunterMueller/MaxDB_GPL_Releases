/*!
  @defgroup     Sec     Security
  @file         RTECrypto_HMACMD5.cpp
  @author       ChristophB

  @brief		this class provides the implementation of HMAC-MD5 algorithm

\if EMIT_LICENCE

  ==========================================================================

  This file implements the keyed hash algorith HMAC-MD5 as descriped in
  RFC 2101 and RFC 2202.

  Sources:
  [RFC1321]  Rivest, R., "The MD5 Message-Digest Algorithm",
             RFC 1321, April 1992.

  [RFC 2101] Krawczyk, H., Bellare, M., and R. Canetti,
             "HMAC: Keyed-Hashing for Message Authentication",
             RFC 2104, February 1997.

  [RFC 2202] Cheng, P., Glenn, R.,
             "Test Cases for HMAC-MD5 and HMAC-SHA-1",
             RFC 2202, September 1997.
 
  ==========================================================================

\endif
*/

#include "RunTime/Security/Crypto/RTECrypto_HMACMD5.hpp"

/* size of data hashed in one step */
#define MD5BlockSize 64

/* init( ) */
void
RTECrypto_HMACMD5::init( void *Key, SAPDB_Int Length )
{
	SAPDB_UInt1 PaddedKey[MD5BlockSize];    /* padded key */

	/* if key longer than MD5BlockSize bytes reset to MD5( key ) */
	if( Length > MD5BlockSize )
	{
		InnerContext.init( );
		InnerContext.update( Key, Length );
		InnerContext.final( PaddedKey, MD5HashSize );
		Key = PaddedKey;
		Length = MD5HashSize;
	}

	/* pad the key for inner digest */
	SAPDB_Int i;
	for( i = 0; i < Length; i++ )
		PaddedKey[i] = reinterpret_cast<SAPDB_UInt1 *>( Key )[i] ^ 0x36;

	while( i < MD5BlockSize )
		PaddedKey[i++] = 0x36;

	/* prepare calculation of inner digest */
	InnerContext.init( );
	InnerContext.update( PaddedKey, MD5BlockSize );

	/* pad the key for outer digest */
	for( i = 0; i < MD5BlockSize; i++ )
		PaddedKey[i] ^= (0x36 ^ 0x5c);

	/* prepare calculation of outer digest */
	OuterContext.init( );
	OuterContext.update( PaddedKey, MD5BlockSize );

	/* clean up derived key */
	memset( PaddedKey, 0, MD5BlockSize );
}

/* final( ) */
void
RTECrypto_HMACMD5::final( void *OutputData, SAPDB_Int Length )
{
	SAPDB_UInt1 Digest[MD5HashSize];

	/* finish inner MD5 */
	InnerContext.final( Digest, MD5HashSize );

	/* finish outer MD5 */
	OuterContext.update( Digest, MD5HashSize );
	OuterContext.final( Digest, MD5HashSize );

	/* truncate output data */
	if( Length > MD5HashSize )
		Length = MD5HashSize;

	memcpy( OutputData, Digest, Length );
}
