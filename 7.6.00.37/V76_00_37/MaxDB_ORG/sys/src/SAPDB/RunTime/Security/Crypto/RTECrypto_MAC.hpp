/*!
  @defgroup     Sec     Security
  @file         RTECrypto_MAC.hpp
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

#ifndef RTECRYPTO_MAC_HPP
#define RTECRYPTO_MAC_HPP

#include "SAPDBCommon/SAPDB_Types.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

class RTECrypto_MAC {
public:
	enum Type {
		MD5 = 1,
		SHA1
	};

	// create new MAC object
	static RTECrypto_MAC *createInstance( RTECrypto_MAC::Type, SAPDBMem_IRawAllocator & );

	// destroy new MAC object
	static void destroy( RTECrypto_MAC * );

	// get length of MAC
	virtual SAPDB_Int getLength( ) = 0;

	// init MAC buffer
	virtual void init( ) = 0;

	// update MAC buffer
	virtual void update( void *, SAPDB_Int ) = 0;

	// finalize MAC calculation, provide MAC and clear MAC buffer
	virtual void final ( void *, SAPDB_Int ) = 0;

protected:
	Type AlgorithmType;
	SAPDBMem_IRawAllocator *Allocator;
};

#endif
