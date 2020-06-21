/*!
  @defgroup     Sec     Security
  @file         RTECrypto_SHA1.hpp
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

#ifndef RTECRYPTO_SHA1_HPP
#define RTECRYPTO_SHA1_HPP

#include <string.h>
#include "RunTime/Security/Crypto/RTECrypto_MAC.hpp"


// ===========================================================================

/*
 * If you do not have the SAPDB_Types.h (was ISO standard stdint.h) header file,
 * then you must typdef the following:
 *
 * name          meaning
 * -----------------------------------------------------------
 * SAPDB_UInt4   unsigned 32 bit integer
 * SAPDB_UInt1   unsigned 8 bit integer (i.e., unsigned char)
 * SAPDB_UInt    integer of >= 16 bits
 *
 */

// ===========================================================================

#define SHA1HashSize 20

class RTECrypto_HMACSHA1;
class RTECrypto_SHA1PRNG;
class RTECrypto_EntropyPool;

class RTECrypto_SHA1 : public RTECrypto_MAC
{
friend class RTECrypto_MAC;
friend class RTECrypto_HMACSHA1;
friend class RTECrypto_SHA1PRNG;
friend class RTECrypto_EntropyPool;
private:
	RTECrypto_SHA1( );
	void process( );

	/*
	 *  This structure will hold context information for the SHA-1
	 *  hashing operation
	 */
	SAPDB_UInt4 IntermediateHash[SHA1HashSize / 4]; /* message digest */
	SAPDB_UInt4 LengthLow;                          /* message length in bits */
	SAPDB_UInt4 LengthHigh;                         /* message length in bits */
	SAPDB_UInt  MessageBlockIndex;                  /* index into message block array */
	SAPDB_UInt1 MessageBlock[64];                   /* 512-bit message blocks */
	SAPDB_Bool  Corrupted;                          /* is the message digest corrupted */

public:
	// placement new operator
	void *operator new ( size_t Size, SAPDBMem_IRawAllocator &Allocator )
	{
		return Allocator.Allocate( Size );
	}

	// placement delete operator
	void operator delete ( void *Memory, SAPDBMem_IRawAllocator &Allocator )
	{
		Allocator.Deallocate( Memory );
	}

	// destructor
	~RTECrypto_SHA1( ){	this->reset( ); };

	// get length of MAC
	SAPDB_Int getLength( ){	return SHA1HashSize; }

	/* clean buffers */
	void reset( )
	{
		memset( IntermediateHash, 0, sizeof( IntermediateHash ));
		memset( MessageBlock, 0, sizeof( MessageBlock ));
		LengthLow = 0;
		LengthHigh = 0;
		MessageBlockIndex = 0;
		Corrupted = false;
	}

	// init MAC buffer
	void init( );

	// update MAC buffer
	void update( void *, SAPDB_Int );

	// finalize MAC calculation, provide MAC and clear MAC buffer
	void final( void *, SAPDB_Int );
};

#endif
