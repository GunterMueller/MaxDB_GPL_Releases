/*!
  @defgroup     Sec     Security
  @file         RTECrypto_HMACSHA1.cpp
  @author       ChristophB

  @brief		this class provides the implementation of HMAC-SHA-1 algorithm

\if EMIT_LICENCE

  ==========================================================================

  This file implements the keyed hash algorith HMAC-SHA-1 as descriped in
  RFC 2202.

  Sources:
  [FIPS 180-1] "Secure Hash Standard", United States of American,
               National Institute of Science and Technology, Federal
               Information Processing Standard (FIPS) 180-1, April 1993.

  [RFC 2202]   Cheng, P., Glenn, R.,
               "Test Cases for HMAC-MD5 and HMAC-SHA-1",
               RFC 2202, September 1997.

  ==========================================================================

\endif
*/

#include "RunTime/Security/Crypto/RTECrypto_HMACSHA1.hpp"

/* size of data hashed in one step */
#define SHA1BlockSize 64

/* init( ) */
void
RTECrypto_HMACSHA1::init( void *Key, SAPDB_Int Length )
{
	SAPDB_UInt1 PaddedKey[SHA1BlockSize];    /* padded key */

	/* if key longer than SHA1BlockSize bytes reset to SHA1( key ) */
	if( Length > SHA1BlockSize )
	{
		InnerContext.init( );
		InnerContext.update( Key, Length );
		InnerContext.final( PaddedKey, SHA1HashSize );
		Key = PaddedKey;
		Length = SHA1HashSize;
	}

	/* pad the key for inner digest */
	SAPDB_Int i;
	for( i = 0; i < Length; i++ )
		PaddedKey[i] = reinterpret_cast<SAPDB_UInt1 *>( Key )[i] ^ 0x36;

	while( i < SHA1BlockSize )
		PaddedKey[i++] = 0x36;

	/* prepare calculation of inner digest */
	InnerContext.init( );
	InnerContext.update( PaddedKey, SHA1BlockSize );

	/* pad the key for outer digest */
	for( i = 0; i < SHA1BlockSize; i++ )
		PaddedKey[i] ^= (0x36 ^ 0x5c);

	/* prepare calculation of outer digest */
	OuterContext.init( );
	OuterContext.update( PaddedKey, SHA1BlockSize );

	/* clean up derived key */
	memset( PaddedKey, 0, SHA1BlockSize );
}

/* final( ) */
void
RTECrypto_HMACSHA1::final( void *OutputData, SAPDB_Int Length )
{
	SAPDB_UInt1 Digest[SHA1HashSize];

	/* finish inner SHA1 */
	InnerContext.final( Digest, SHA1HashSize );

	/* finish outer SHA1 */
	OuterContext.update( Digest, SHA1HashSize );
	OuterContext.final( Digest, SHA1HashSize );

	/* truncate output data */
	if( Length > SHA1HashSize )
		Length = SHA1HashSize;

	memcpy( OutputData, Digest, Length );
}
