/*!
  @defgroup     Sec     Security
  @file         RTECrypto_MD5.hpp
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

#ifndef RTECRYPTO_MD5_HPP
#define RTECRYPTO_MD5_HPP

#include <sys/types.h>
#include <string.h>
#include "RunTime/Security/Crypto/RTECrypto_MAC.hpp"

#define MD5HashSize 16

class RTECrypto_HMACMD5;

class RTECrypto_MD5 : public RTECrypto_MAC
{
friend class RTECrypto_MAC;
friend class RTECrypto_HMACMD5;
private:
	/* constructor */
	RTECrypto_MD5( );

	SAPDB_UInt4 State[ MD5HashSize / 4 ];
	SAPDB_UInt4 Count[2];
	SAPDB_UInt4 In[16];

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
	~RTECrypto_MD5( ){ this->reset( ); }

	/* get length of hased value */
	SAPDB_Int getLength( ){	return MD5HashSize; }

	/* clean buffers */
	void reset( )
	{
		memset( State, 0, sizeof( State ));
		memset( Count, 0, sizeof( Count ));
		memset( In, 0, sizeof( In ));
	};

	/* start hashing */
	void init( );

	/* hash buffer */
	void update( void *, SAPDB_Int );

	/* finalize hashing and fill buffer */
	void final( void *, SAPDB_Int );
};

#endif
