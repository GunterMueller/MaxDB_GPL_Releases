/*!
   @file DevTrace_Trace.hpp

   @brief C++ tracing (streaming) of types in SAPDB
\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#ifndef SAPDBTRACE_STREAMING_HPP
#define SAPDBTRACE_STREAMING_HPP

#include "gsp00.h"
#include "ggg00.h"
#include "ggg04.h"
#include "ggg07.h"
#include "SAPDB/SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_OStream.hpp"

class SQLMan_Identifier;
class Catalog_ObjectId;

// void *
SAPDB_OStream & operator<< (SAPDB_OStream &, void *);

// tgg00_BdInvRange
SAPDB_OStream & operator<< (SAPDB_OStream &, const tgg00_BdInvRange &);

// tgg07_StratEnum
SAPDB_OStream & operator<< (SAPDB_OStream &, const tgg07_StratEnum &);

//bool
SAPDB_OStream & operator<< (SAPDB_OStream &, const bool);

// tgg04_Distinct
SAPDB_OStream & operator<< (SAPDB_OStream &, const tgg04_Distinct &);

namespace SAPDBTrace {
// tgg00_BasisError
class BasisError : public SAPDB_Manipulator
{
private:
    const tsp00_Int2 m_BasisError;
public:
    BasisError (tsp00_Int2 err) : m_BasisError(err) {}
    virtual ~BasisError () {}
    virtual SAPDB_OStream& ToStream (SAPDB_OStream& os) const;
};
}

// tgg04_CacheType
SAPDB_OStream & operator<< (SAPDB_OStream &, const tgg04_CacheType &);

// tgg00_MessType
SAPDB_OStream & operator<< (SAPDB_OStream &, const tgg00_MessType &);

// tgg00_MessType2
SAPDB_OStream & operator<< (SAPDB_OStream &, const tgg00_MessType2 &);

// tgg00_DateTimeFormat
SAPDB_OStream & operator<< (SAPDB_OStream &, const tgg00_DateTimeFormat &);

// tgg00_IndexMode
SAPDB_OStream & operator<< (SAPDB_OStream &, const tgg00_IndexMode &);

// tgg00_InvListKind
SAPDB_OStream & operator<< (SAPDB_OStream &, const tgg00_InvListKind &);

// tgg00_LockReqMode
SAPDB_OStream & operator<< (SAPDB_OStream &, const tgg00_LockReqMode &);

// tgg00_StackOpType
SAPDB_OStream & operator<< (SAPDB_OStream &, const tgg00_StackOpType &);

// tgg00_StackOpFunc
SAPDB_OStream & operator<< (SAPDB_OStream &, const tgg00_StackOpFunc &);

// tsp00_LcompResult
SAPDB_OStream & operator<< (SAPDB_OStream &, const tsp00_LcompResult &);

// SQLMan_Identifier
SAPDB_OStream & operator<< (SAPDB_OStream &, const SQLMan_Identifier &);

// Catalog_ObjectId
SAPDB_OStream & operator<< (SAPDB_OStream &, const Catalog_ObjectId &);

#endif // SAPDBTRACE_STREAMING_HPP


