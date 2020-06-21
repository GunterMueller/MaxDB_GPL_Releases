/*!
  @defgroup     Sec     Security
  @file         RTECrypto_RNG.hpp
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

#ifndef RTECRYPTO_RNG_HPP
#define RTECRYPTO_RNG_HPP

#include "SAPDBCommon/SAPDB_Types.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

class RTECrypto_RNG
{
public:
	enum Type {
		SysRNG = 1,
		SHA1PRNG
	};

	// create new RNG object
	static RTECrypto_RNG *createInstance( RTECrypto_RNG::Type, SAPDBMem_IRawAllocator & );

	// destroy new RNG object
	static void destroy( RTECrypto_RNG * );

	// get next bytes
	virtual SAPDB_Bool nextBytes( void *, SAPDB_Int ) = 0;

	// note: you must either
	// - set a an initial seed to get a identical sequence of pseudo random numbers or
	// - set an entropy source to get true random numbers

	// set initial seed
	virtual SAPDB_Bool setSeed( void *, SAPDB_Int ) = 0;

protected:
	SAPDBMem_IRawAllocator *Allocator;
	Type RNGType;
};

#endif
