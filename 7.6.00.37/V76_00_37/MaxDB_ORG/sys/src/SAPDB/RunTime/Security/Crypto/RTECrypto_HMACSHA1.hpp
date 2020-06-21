/*!
  @defgroup     Sec     Security
  @file         RTECrypto_HMACSHA1.hpp
  @author       ChristophB

  @brief		this class provides the implementation of HMAC SHA-1 algorithm

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

#ifndef RTECRYPTO_HMACSHA1_HPP
#define RTECRYPTO_HMACSHA1_HPP

#include <sys/types.h>
#include <string.h>
#include "RunTime/Security/Crypto/RTECrypto_HMAC.hpp"
#include "RunTime/Security/Crypto/RTECrypto_SHA1.hpp"

class RTECrypto_SHA1PRNG;

class RTECrypto_HMACSHA1 : public RTECrypto_HMAC
{
friend class RTECrypto_HMAC;
friend class RTECrypto_SHA1PRNG;
private:
	/* constructor */
	RTECrypto_HMACSHA1( ){ AlgorithmType = SHA1; }

	RTECrypto_SHA1 InnerContext;
	RTECrypto_SHA1 OuterContext;

public:
	/* placement new */
	void *operator new( size_t Size, SAPDBMem_IRawAllocator &Allocator )
	{
		return Allocator.Allocate( Size );
	}

	/* placement delete */
	void operator delete( void *Memory, SAPDBMem_IRawAllocator &Allocator )
	{
		Allocator.Deallocate( Memory );
	}

	/* destructor */
	~RTECrypto_HMACSHA1( ){ }

	/* get length of hased value */
	SAPDB_Int getLength( ){	return SHA1HashSize; }

	/* clean buffers */
	void reset( )
	{
		InnerContext.reset( );
		OuterContext.reset( );
	}

	/* start hashing */
	void init( void *, SAPDB_Int );

	/* hash buffer */
	void update( void *InputData, SAPDB_Int Length )
	{
		InnerContext.update( InputData, Length );
	}

	/* finalize hashing and fill buffer */
	void final( void *, SAPDB_Int );

	void precalc( void *, SAPDB_Int ){ }

	void import( void*, SAPDB_Int ){ }
};

#endif
