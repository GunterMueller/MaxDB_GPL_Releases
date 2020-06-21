/*!
  @defgroup     Sec     Security
  @file         RTECrypto_SHA1PRNG.cpp
  @author       ChristophB

  @brief		this class provides a SHA-1 Pseudo Random Number Generator
                as descriped in RFC 4986 refering to ANSI X9.82

	Sources:

	[RFC 4986]	 Eastlake, D. 3rd, Schiller, J., Crocker, S.,
	             "Randomness Requirements for Security",
                 RFC 4986, June 2005.


\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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
*/

#include <string.h>
#include "RunTime/Security/Crypto/RTECrypto_SHA1PRNG.hpp"
#include "RunTime/Security/Crypto/RTECrypto_EntropyPool.hpp"

RTECrypto_SHA1PRNG::RTECrypto_SHA1PRNG( )
{
	Allocator = 0;
	RNGType = RTECrypto_RNG::SHA1PRNG;

	Initialization = Uninitialized;

	memset( V, 0, SHA1HashSize );
	memset( K, 0, SHA1HashSize );
}

RTECrypto_SHA1PRNG::~RTECrypto_SHA1PRNG( )
{
	memset( V, 0, SHA1HashSize );
	memset( K, 0, SHA1HashSize );
}

inline void
RTECrypto_SHA1PRNG::initState( void *Data, SAPDB_Int Length )
{
	// initial setup of PRNG state
	memset( V, 1, SHA1HashSize );
	memset( K, 0x00000000, SHA1HashSize );

	addEntropy( Data, Length );
}

inline void
RTECrypto_SHA1PRNG::addEntropy( void *Data, SAPDB_Int Length )
{
	SAPDB_UInt1 ByteNull = 0;
	SAPDB_UInt1 ByteOne  = 1;

	// 1st HMAC operation
	Context.init( K, sizeof( K ));
	Context.update( V, sizeof( V ));
	Context.update( &ByteNull, 1 ); 
	Context.update( Data, Length );
	Context.final( K, sizeof( K ));

	// 2nd HMAC operation
	Context.init( K, sizeof( K ));
	Context.update( V, sizeof( V ));
	Context.final( V, sizeof( V ));

	// 3rd HMAC operation
	Context.init( K, sizeof( K ));
	Context.update( V, sizeof( V ));
	Context.update( &ByteOne, 1 ); 
	Context.update( Data, Length );
	Context.final( K, sizeof( K ));

	// 4th HMAC operation
	Context.init( K, sizeof( K ));
	Context.update( V, sizeof( V ));
	Context.final( V, sizeof( V ));
}

// set seed to generate an indentical sequence with the given seed
SAPDB_Bool
RTECrypto_SHA1PRNG::setSeed( void *SeedData, SAPDB_Int SeedLength )
{
	// do not add external entropy
	Initialization = Seeded;

	// initialize PRNG state
	initState( SeedData, SeedLength );
	return true;
}

SAPDB_Bool
RTECrypto_SHA1PRNG::nextBytes( void *OutputData, SAPDB_Int Length )
{
	if( Initialization == Uninitialized )
	{
		Initialization = EntropyPoolUsed;
		
		SAPDB_UInt1 EntropyBuffer[SHA1HashSize];
		RTECrypto_EntropyPool::getInstance( ).nextBytes( EntropyBuffer, SHA1HashSize );
		initState( EntropyBuffer, SHA1HashSize );
		memset( EntropyBuffer, 0, SHA1HashSize );
	}

	SAPDB_UInt1 ByteNull = 0;
	SAPDB_Int Remain = Length;

	SAPDB_Int Index = 0;
	while( Remain >= 0 )
	{
		Context.init( K, sizeof( K ));
		Context.update( V, sizeof( V ));
		Context.final( V, sizeof( V ));

		memcpy(
		static_cast<SAPDB_UInt1 *>( OutputData ) + Index, V,
		( Remain > SHA1HashSize ? SHA1HashSize : Remain ));

		Remain -= SHA1HashSize;
		Index += SHA1HashSize;
	}

	// 1st HMAC operation
	Context.init( K, sizeof( K ));
	Context.update( V, sizeof( V ));
	Context.update( &ByteNull, 1 ); 
	Context.final( K, sizeof( K ));
	
	// 2nd HMAC operation
	Context.init( K, sizeof( K ));
	Context.update( V, sizeof( V ));
	Context.final( V, sizeof( V ));

	return true;
}
