/*!
  @defgroup     Sec     Security
  @file         RTECrypto_RNG.cpp
  @author       ChristophB

  @brief		this class provides an interface to a Pseudo Random Number Generator

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

#include "RunTime/Security/Crypto/RTECrypto_RNG.hpp"
#include "RunTime/Security/Crypto/RTECrypto_SysRNG.hpp"
#include "RunTime/Security/Crypto/RTECrypto_SHA1PRNG.hpp"

RTECrypto_RNG *
RTECrypto_RNG::createInstance( RTECrypto_RNG::Type RNGType, SAPDBMem_IRawAllocator &Allocator )
{
	RTECrypto_RNG *RNG = 0;
	switch( RNGType )
	{
	case SysRNG:
		RNG = new ( Allocator ) RTECrypto_SysRNG( );
		break;
	case SHA1PRNG:
		RNG = new ( Allocator ) RTECrypto_SHA1PRNG( );
		break;
	}

	if( RNG )
		RNG->Allocator = &Allocator;

	return RNG;
}

void
RTECrypto_RNG::destroy( RTECrypto_RNG *RNG )
{
	if( RNG == 0 )
		return;

	SAPDBMem_IRawAllocator *Allocator = RNG->Allocator;

	switch( RNG->RNGType )
	{
	case SysRNG:
		static_cast <RTECrypto_SysRNG *> ( RNG )->~RTECrypto_SysRNG( );
		break;
	case SHA1PRNG:
		static_cast <RTECrypto_SHA1PRNG *> ( RNG )->~RTECrypto_SHA1PRNG( );
		break;
	}

    if( Allocator )
		Allocator->Deallocate( RNG );

	RNG = 0;
}
