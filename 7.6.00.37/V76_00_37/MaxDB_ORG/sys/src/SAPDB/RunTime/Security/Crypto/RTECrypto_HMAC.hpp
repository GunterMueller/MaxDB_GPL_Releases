/*!
  @defgroup     Sec     Security
  @file         RTECrypto_HMAC.hpp
  @author       ChristophB

  @brief		this class provides an interface to a Keyed Hash algorithm

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

#ifndef RTECRYPTO_HMAC_HPP
#define RTECRYPTO_HMAC_HPP

#include "SAPDBCommon/SAPDB_Types.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

class RTECrypto_HMAC {
public:
	enum Type {
		MD5 = 1,
		SHA1
	};

	// create new MAC object
	static RTECrypto_HMAC *createInstance( RTECrypto_HMAC::Type, SAPDBMem_IRawAllocator & );

	// destroy new HMAC object
	static void destroy( RTECrypto_HMAC * );

	// get length of Hash
	virtual SAPDB_Int getLength( ) = 0;

	// init hash buffer, needs key
	virtual void init( void *, SAPDB_Int ) = 0;

	// update hash buffer
	virtual void update( void *, SAPDB_Int ) = 0;

	// finalize Hash calculation, provide hash and clear hash buffer
	virtual void final( void *, SAPDB_Int ) = 0;

	// access to intermediate state may help to implement CRAM or SCRAM algorithms;

	// precalculate intermediate state from key
	virtual void precalc( void *, SAPDB_Int ) = 0;

	// initialize context from intermediate state
	virtual void import( void*, SAPDB_Int ) = 0;

private:
	SAPDBMem_IRawAllocator *Allocator;

protected:
	Type AlgorithmType;
};

#endif
