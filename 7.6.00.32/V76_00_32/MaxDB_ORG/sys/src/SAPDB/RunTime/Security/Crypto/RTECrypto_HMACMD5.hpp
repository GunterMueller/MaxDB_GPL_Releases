/*!
  @defgroup     Sec     Security
  @file         RTECrypto_HMACMD5.hpp
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

#ifndef RTECRYPTO_HMACMD5_HPP
#define RTECRYPTO_HMACMD5_HPP

#include <sys/types.h>
#include <string.h>
#include "RunTime/Security/Crypto/RTECrypto_HMAC.hpp"
#include "RunTime/Security/Crypto/RTECrypto_MD5.hpp"

class RTECrypto_HMACMD5 : public RTECrypto_HMAC
{
friend class RTECrypto_HMAC;
private:
	/* constructor */
	RTECrypto_HMACMD5( ){ AlgorithmType = MD5; }

	RTECrypto_MD5 InnerContext;
	RTECrypto_MD5 OuterContext;

public:
	/* placement new */
	void *operator new ( size_t Size, SAPDBMem_IRawAllocator &Allocator )
	{
		return Allocator.Allocate( Size );
	}

	/* placement delete */
	void operator delete ( void *Memory, SAPDBMem_IRawAllocator &Allocator )
	{
		Allocator.Deallocate( Memory );
	}

	/* destructor */
	~RTECrypto_HMACMD5( ){ };

	/* get length of hased value */
	SAPDB_Int getLength( ){	return MD5HashSize; }

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
