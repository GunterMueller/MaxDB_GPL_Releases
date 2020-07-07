/*!
  @defgroup     Sec     Security
  @file         RTECrypto_SysRNG.hpp
  @author       ChristophB

  @brief		this class provides a System Random Number Generator
 
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

#ifndef RTECRYPTO_SYSRNG_HPP
#define RTECRYPTO_SYSRNG_HPP

#include <sys/types.h>
#include "RunTime/Security/Crypto/RTECrypto_RNG.hpp"

#ifdef WIN32
#include <windows.h>

typedef BOOL (WINAPI *RTLGENRANDOM)( PVOID, ULONG );
#endif

class RTECrypto_RNG;

class RTECrypto_SysRNG : public RTECrypto_RNG
{
friend class RTECrypto_RNG;
private:

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
	RTECrypto_SysRNG( );

#ifdef WIN32
	HMODULE hAdvapi;
	RTLGENRANDOM fRtlGenRandom;
#else
	SAPDB_Int fdDevURandom;
#endif

public:
	/* destructor */
	~RTECrypto_SysRNG( );

	// get next bytes
	SAPDB_Bool nextBytes( void *, SAPDB_Int );

	// note:
	// you cannot seed the system random number generator

	// set initial sees
	SAPDB_Bool setSeed( void *, SAPDB_Int ) { return false; };
};

#endif
