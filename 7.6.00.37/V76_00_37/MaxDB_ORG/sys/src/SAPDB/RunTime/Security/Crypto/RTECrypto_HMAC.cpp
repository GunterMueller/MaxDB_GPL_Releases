/*!
  @defgroup     Sec     Security
  @file         RTECrypto_HMAC.cpp
  @author       ChristophB

  @brief		this class provides an interface to a Keaed Hash algorithm

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
#include "RunTime/Security/Crypto/RTECrypto_HMAC.hpp"
#include "RunTime/Security/Crypto/RTECrypto_HMACMD5.hpp"
#include "RunTime/Security/Crypto/RTECrypto_HMACSHA1.hpp"

RTECrypto_HMAC *
RTECrypto_HMAC::createInstance( RTECrypto_HMAC::Type AlgorithmType, SAPDBMem_IRawAllocator &Allocator )
{
	RTECrypto_HMAC *HMAC = 0;
	switch( AlgorithmType )
	{
	case MD5:
		HMAC = new ( Allocator ) RTECrypto_HMACMD5( );
		break;
	case SHA1:
		HMAC = new ( Allocator ) RTECrypto_HMACSHA1( );
		break;
	}

	if( HMAC )
		HMAC->Allocator = &Allocator;

	return HMAC;
}

void
RTECrypto_HMAC::destroy( RTECrypto_HMAC *HMAC )
{
	if( HMAC == 0 )
		return;

	SAPDBMem_IRawAllocator *Allocator = HMAC->Allocator;

	switch( HMAC->AlgorithmType )
	{
	case MD5:
		static_cast <RTECrypto_HMACMD5 *> ( HMAC )->~RTECrypto_HMACMD5( );
		break;

	case SHA1:
		static_cast <RTECrypto_HMACSHA1 *> ( HMAC )->~RTECrypto_HMACSHA1( );
		break;
	}

    if( Allocator )
		Allocator->Deallocate( HMAC );

	HMAC = 0;
}