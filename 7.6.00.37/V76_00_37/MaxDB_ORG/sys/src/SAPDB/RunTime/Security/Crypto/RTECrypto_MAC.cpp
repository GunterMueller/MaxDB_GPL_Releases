/*!
  @defgroup     Sec     Security
  @file         RTECrypto_MAC.cpp
  @author       ChristophB

  @brief		this class provides an interface to a Message Authentication Code algorithm

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

#include "SAPDBCommon/SAPDB_string.h"
#include "RunTime/Security/Crypto/RTECrypto_MAC.hpp"

#include "RunTime/Security/Crypto/RTECrypto_MD5.hpp"
#include "RunTime/Security/Crypto/RTECrypto_SHA1.hpp"

RTECrypto_MAC *
RTECrypto_MAC::createInstance( RTECrypto_MAC::Type AlgorithmType, SAPDBMem_IRawAllocator &Allocator )
{
	RTECrypto_MAC *MAC = 0;

	switch( AlgorithmType )
	{
	case MD5:
		MAC = new ( Allocator ) RTECrypto_MD5( );
		break;
	case SHA1:
		MAC = new ( Allocator ) RTECrypto_SHA1( );
		break;
	}

	if( MAC )
		MAC->Allocator = &Allocator;

	return MAC;
}

void
RTECrypto_MAC::destroy( RTECrypto_MAC *MAC )
{
	if( MAC == 0 )
		return;

	SAPDBMem_IRawAllocator *Allocator = MAC->Allocator;

	switch( MAC->AlgorithmType )
	{
	case MD5:
		static_cast <RTECrypto_MD5 *> ( MAC )->~RTECrypto_MD5( );
		break;

	case SHA1:
		static_cast <RTECrypto_SHA1 *> ( MAC )->~RTECrypto_SHA1( );
		break;
	}

    if( Allocator )
		Allocator->Deallocate( MAC );

	MAC = 0;
}
