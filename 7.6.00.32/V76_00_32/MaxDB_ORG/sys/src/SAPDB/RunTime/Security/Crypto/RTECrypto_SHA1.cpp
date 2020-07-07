/*!
  @defgroup     Sec     Security
  @file         RTECrypto_SHA1.cpp
  @author       ChristophB

  @brief		this class provides the implementation of SHA1 algorithm

\if EMIT_LICENCE

  ==========================================================================
	
	Description:
	This file implements the Secure Hashing Algorithm 1 as
	defined in FIPS PUB 180-1 published April 17, 1995.

	Many of the variable names in this code, especially the
	single character names, were used because those were the names
	used in the publication.

	The SHA-1, produces a 160-bit message digest for a given
	data stream.  It should take about 2**n steps to find a
	message with the same digest as a given message and
	2**(n/2) to find any two messages with the same digest,
	when n is the digest size in bits.  Therefore, this
	algorithm can serve as a means of providing a
	"fingerprint" for a message.
	
	Portability Issues:
	SHA-1 is defined in terms of 32-bit "words".  This code
	uses <stdint.h> (included via "sha1.h" to define 32 and 8
	bit unsigned integer types.  If your C compiler does not
	support 32 bit unsigned integers, this code is not
	appropriate.

	Caveats:
	SHA-1 is designed to work with messages less than 2^64 bits
	long.  Although SHA-1 allows a message digest to be generated
	for messages of any number of bits less than 2^64, this
	implementation only works with messages with a length that is
	a multiple of the size of an 8-bit character.

	Sources:
	[FIPS 180-1] "Secure Hash Standard", United States of American,
                National Institute of Science and Technology, Federal
                Information Processing Standard (FIPS) 180-1, April
                1993.

	[RFC 3174]	Eastlake, D. 3rd, "US Secure Hash Algorithm 1 (SHA1)",
                RFC 3174, September 2001.

  ==========================================================================

\endif
*/

#include "RunTime/Security/Crypto/RTECrypto_SHA1.hpp"
#include <string.h>

/* define the SHA1 circular left shift macro */
#define SHA1CircularShift(cnt,w) (((w) << (cnt)) | ((w) >> (32 - (cnt))))

void
RTECrypto_SHA1::process( )
{
	/* constants defined in SHA-1 */
    const SAPDB_UInt4 K[] =
	{
		0x5a827999,
		0x6ed9eba1,
		0x8f1bbcdc,
		0xca62c1d6
	};

    SAPDB_Int           t;              /* loop counter  */
    SAPDB_UInt4      Temp;              /* temporary word value  */
    SAPDB_UInt4      W[80];             /* word sequence */
    SAPDB_UInt4      A, B, C, D, E;     /* word buffers */

    /* initialize the first 16 words in the array W */
    for( t = 0; t < 16; t++ )
    {
        W[t] = MessageBlock[t * 4] << 24;
        W[t] |= MessageBlock[t * 4 + 1] << 16;
        W[t] |= MessageBlock[t * 4 + 2] << 8;
        W[t] |= MessageBlock[t * 4 + 3];
    }

    for( t = 16; t < 80; t++ )
       W[t] = SHA1CircularShift( 1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);

    A = IntermediateHash[0];
    B = IntermediateHash[1];
    C = IntermediateHash[2];
    D = IntermediateHash[3];
    E = IntermediateHash[4];

    for( t = 0; t < 20; t++ )
    {
        Temp = SHA1CircularShift( 5, A ) +
		       (( B & C ) | (( ~B ) & D )) + E + W[t] + K[0];
        E = D;
        D = C;
        C = SHA1CircularShift( 30, B );

        B = A;
        A = Temp;
    }

    for( t = 20; t < 40; t++ )
    {
        Temp = SHA1CircularShift( 5, A ) + ( B ^ C ^ D ) + E + W[t] + K[1];
        E = D;
        D = C;
        C = SHA1CircularShift( 30, B);
        B = A;
        A = Temp;
    }

    for( t = 40; t < 60; t++ )
    {
		Temp = SHA1CircularShift( 5, A ) +
		       (( B & C ) | ( B & D ) | ( C & D )) + E + W[t] + K[2];
        E = D;
        D = C;
        C = SHA1CircularShift( 30, B );
        B = A;
        A = Temp;
    }

    for( t = 60; t < 80; t++ )
    {
        Temp = SHA1CircularShift( 5, A ) + ( B ^ C ^ D ) + E + W[t] + K[3];
        E = D;
        D = C;
        C = SHA1CircularShift( 30, B );
        B = A;
        A = Temp;
    }

    IntermediateHash[0] += A;
    IntermediateHash[1] += B;
    IntermediateHash[2] += C;
    IntermediateHash[3] += D;
    IntermediateHash[4] += E;

    MessageBlockIndex = 0;
}

RTECrypto_SHA1::RTECrypto_SHA1( )
{
	AlgorithmType = SHA1;
	Allocator = 0;
	this->reset( );
}

void
RTECrypto_SHA1::init( )
{
	this->reset( );
	IntermediateHash[0] = 0x67452301;
    IntermediateHash[1] = 0xefcdab89;
    IntermediateHash[2] = 0x98badcfe;
    IntermediateHash[3] = 0x10325476;
    IntermediateHash[4] = 0xc3d2e1f0;
}

void
RTECrypto_SHA1::update( void *InputData, SAPDB_Int Length )
{
	const SAPDB_UInt1 *MessageArray = reinterpret_cast <const SAPDB_UInt1 *>( InputData );
	if( Length == 0 )
        return; // true;

    if( MessageArray == 0 )
        return; // false;

    if( Corrupted )
         return; // false;

	while( Length-- && !Corrupted )
    {
		MessageBlock[MessageBlockIndex++] = ( *MessageArray & 0xff );
		LengthLow += 8;
		if( LengthLow == 0 )
		{
			LengthHigh++;

			/* message is too long */
			if( LengthHigh == 0 )
				Corrupted = true;
		}

		if( MessageBlockIndex == 64 )
			this->process( );

		MessageArray++;
    }

	return; // true;
}

void
RTECrypto_SHA1::final( void *OutputData, SAPDB_Int Length )
{
	SAPDB_UInt1 MessageDigest[20];
    SAPDB_Int i;

    if ( Corrupted )
        return; // false;
  
     /*
	 *  Check to see if the current message block is too small to hold
	 *  the initial padding bits and length.  If so, we will pad the
	 *  block, process it, and then continue padding into a second
	 *  block.
	 */
	if( MessageBlockIndex > 55 )
	{
		MessageBlock[MessageBlockIndex++] = 0x80;
		while( MessageBlockIndex < 64 )
			MessageBlock[MessageBlockIndex++] = 0;

		this->process( );
		while( MessageBlockIndex < 56 )
			MessageBlock[MessageBlockIndex++] = 0;
	}
	else
	{
		MessageBlock[MessageBlockIndex++] = 0x80;
		while( MessageBlockIndex < 56 )
			MessageBlock[MessageBlockIndex++] = 0;
	}

	/* store the message length as the last 8 octets */
	MessageBlock[56] = LengthHigh >> 24;
	MessageBlock[57] = LengthHigh >> 16;
	MessageBlock[58] = LengthHigh >> 8;
	MessageBlock[59] = LengthHigh;
	MessageBlock[60] = LengthLow >> 24;
	MessageBlock[61] = LengthLow >> 16;
	MessageBlock[62] = LengthLow >> 8;
	MessageBlock[63] = LengthLow;
		
	this->process( );
 
	/* prepare for output */
	for( i = 0; i < SHA1HashSize; ++i )
		MessageDigest[i] = IntermediateHash[i >> 2] >> 8 * ( 3 - ( i & 0x03 ));

	if( Length > sizeof( MessageDigest ))
		Length = sizeof( MessageDigest );

	memcpy( OutputData, MessageDigest, Length );

	/* message may be sensitive, clear it out */
	this->reset( );
	return;
}
