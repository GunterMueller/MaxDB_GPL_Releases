/*!
  @defgroup     Sec     Security
  @file         RTECrypto_SHA1PRNG.hpp
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

	X9.82 RFC4986

\endif
*/

#ifndef RTECRYPTO_SHA1PRNG_HPP
#define RTECRYPTO_SHA1PRNG_HPP

#include "RunTime/Security/Crypto/RTECrypto_RNG.hpp"
#include "RunTime/Security/Crypto/RTECrypto_HMACSHA1.hpp"

class RTECrypto_RNG;
class RTECrypto_EntropyPool;

class RTECrypto_SHA1PRNG : public RTECrypto_RNG
{
friend class RTECrypto_RNG;
friend class RTECrypto_EntropyPool;
private:
	enum InitializationType
	{
		Uninitialized = 0,
		Seeded,
		EntropyPoolUsed
	};

	InitializationType Initialization;

	/* K and V are together the state of the PRNG */
	SAPDB_UInt4 K[SHA1HashSize / 4];	// updated each time data were read
	SAPDB_UInt4 V[SHA1HashSize / 4];	// updated each time SHA1HashLength data were read 

	RTECrypto_HMACSHA1 Context;

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

	/* constructor */
	RTECrypto_SHA1PRNG( );

	/* initialize PRNG state */
	inline void initState( void *, SAPDB_Int );

	/* add entropy to PRNG state */
	inline void addEntropy( void *, SAPDB_Int );

public:
	/* destructor */
	~RTECrypto_SHA1PRNG( );

	// get next bytes
	SAPDB_Bool nextBytes( void *, SAPDB_Int );

	// note: you must either
	// - set a an initial seed to get a identical sequence of pseudo random numbers or
	// - set an entropy source to get true random numbers

	// set initial sees
	SAPDB_Bool setSeed( void *, SAPDB_Int );
};

#endif
