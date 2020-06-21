/*!
  @defgroup     Sec     Security
  @file         RTECrypto_MD5.cpp
  @author       ChristophB

  @brief		this class provides the implementation of MD5 algorithm

\if EMIT_LICENCE

  ==========================================================================

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

 ============================================================================

\endif
*/

#include "RunTime/Security/Crypto/RTECrypto_MD5.hpp"

/*===========================================================================*
 *                          LOCAL FUNCTIONS
 *===========================================================================*/

union {
	SAPDB_UInt4 Value;
	SAPDB_UInt1 Array[4];
} SwapType;

inline void
byteSwap( SAPDB_UInt4 *Data, SAPDB_Int Count )
{
/* not nessacary on windows/intel */
#if defined _M_IX86 || defined _M_AMD64 || defined _M_IA64
	return;
#endif

/* not nessacary on unix/linux on intel (except HP IPF) */
#if ( defined __i386 ||defined __amd64 || defined __x86_64 || defined __ia64 ) && (! defined __hpux ) 
	return;
#endif

	/* not nessacary on a little endian cpu */
	SwapType.Value = 1;
	if( SwapType.Array[0] == 1 )
		return;

	do {
		SAPDB_UInt1 *pByte = reinterpret_cast <SAPDB_UInt1 *>( Data );

		*Data =
		( static_cast <SAPDB_UInt4>( pByte[3] ) << 24 ) |
		( static_cast <SAPDB_UInt4>( pByte[2] ) << 16 ) |
		( static_cast <SAPDB_UInt4>( pByte[1] ) <<  8 ) |
		( static_cast <SAPDB_UInt4>( pByte[0] ));

		Data++;
	} while( --Count );
}

/* the four core functions */

/* - F1 is optimized somewhat */
/* #define F1(x, y, z) (x & y | ~x & z) */

/*! define */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
/*! define */
#define F2(x, y, z) F1(z, x, y)
/*! define */
#define F3(x, y, z) (x ^ y ^ z)
/*! define */
#define F4(x, y, z) (y ^ (x | ~z))

/*! this is the central step in the MD5 algorithm. */
#define MD5STEP(f,w,x,y,z,in,s) (w += f(x,y,z) + in, w = (w<<s | w>>(32-s)) + x)

/*!
  @brief transformation
  The core of the MD5 algorithm, this alters an existing MD5 hash to
  reflect the addition of 16 longwords of new data.  MD5Update blocks
  the data and converts bytes into longwords for this routine.
  @param buf [out] result
  @param in [in] input
  @return none
 */
inline void
MD5Transform( SAPDB_UInt4 buf[4], SAPDB_UInt4 const in[16] )
{
	register SAPDB_UInt4 a, b, c, d;

	a = buf[0];
	b = buf[1];
	c = buf[2];
	d = buf[3];

	MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
	MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
	MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
	MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
	MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
	MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
	MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
	MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
	MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
	MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
	MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
	MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
	MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
	MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
	MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
	MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

	MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
	MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
	MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
	MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
	MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
	MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
	MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
	MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
	MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
	MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
	MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
	MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
	MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
	MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
	MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
	MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

	MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
	MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
	MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
	MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
	MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
	MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
	MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
	MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
	MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
	MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
	MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
	MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
	MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
	MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
	MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
	MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

	MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
	MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
	MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
	MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
	MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
	MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
	MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
	MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
	MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
	MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
	MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
	MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
	MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
	MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
	MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
	MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
}

/* ============================================================================ */

RTECrypto_MD5::RTECrypto_MD5( )
{
	Allocator = 0;
	AlgorithmType = MD5; 
	this->reset( );
}

void
RTECrypto_MD5::init( )
{
	this->reset( );

	State[0] = 0x67452301;
	State[1] = 0xefcdab89;
	State[2] = 0x98badcfe;
	State[3] = 0x10325476;

	Count[0] = 0;
	Count[1] = 0;
}

void
RTECrypto_MD5::update( void *InputData, SAPDB_Int Length )
{
	SAPDB_UInt4 Value;

	/* update byte count */
	Value = Count[0];
	if(( Count[0] = Value + Length ) < Value )
		Count[1]++;	/* carry from low to high */

	Value = 64 - ( Value & 0x3f );	/* space available in In (at least 1) */
	if( Value > static_cast <SAPDB_UInt> ( Length ))
	{
		memcpy(( SAPDB_UInt1 *)In + 64 - Value, InputData, Length );
		return;
	}

	/* first chunk is an odd size */
	memcpy(( SAPDB_UInt1 *)In + 64 - Value, InputData, Value );
	byteSwap( In, 16 );
	MD5Transform( State, In );
	InputData = static_cast <void *> ( static_cast <SAPDB_UInt1 *>( InputData ) + Value );
	Length -= Value;

	/* process data in 64-byte chunks */
	while( Length >= 64) {
		memcpy( In, InputData, 64);
		byteSwap( In, 16);
		MD5Transform( State, In);
		InputData = static_cast <void *> ( static_cast <SAPDB_UInt1 *>( InputData ) + 64 );
		Length -= 64;
	}

	/* handle any remaining bytes of data. */
	memcpy( In, InputData, Length );
}

void
RTECrypto_MD5::final ( void *OutputData, SAPDB_Int Length )
{
	/* number of bytes in In */
	SAPDB_Int Cnt = Count[0] & 0x3f;
	SAPDB_UInt1 *p = reinterpret_cast <SAPDB_UInt1 *>( In ) + Cnt;

	/* set the first char of padding to 0x80, there is always room. */
	*p++ = 0x80;

	/* bytes of padding needed to make 56 bytes (-8..55) */
	Cnt = 56 - 1 - Cnt;

	if( Cnt < 0 )
	{
		/* padding forces an extra block */
		memset (p, 0, Cnt + 8 );
		byteSwap( In, 16 );
		MD5Transform( State, In);
		p = reinterpret_cast <SAPDB_UInt1 *> ( In );
		Cnt = 56;
	}

	memset( p, 0, Cnt );
	byteSwap( In, 14 );

	/* append length in bits and transform */
	In[14] = Count[0] << 3;
	In[15] = Count[1] << 3 | Count[0] >> 29;
	MD5Transform( State, In );

	byteSwap( State, 4 );

	if( Length > sizeof( State ))
		Length = sizeof( State );

	memcpy( OutputData, State, Length );

	/* in case it is sensitive */
	this->reset( );
	return;
}
