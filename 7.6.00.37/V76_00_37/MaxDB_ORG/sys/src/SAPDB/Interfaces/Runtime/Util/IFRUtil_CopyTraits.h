/*!
  @file           IFRUtil_CopyTraits.h
  @author         D039759
  @ingroup        IFR_Util
  @brief          Helper for safe assignment and copy construction.
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#ifndef IFRUTIL_COPYTRAITS_H
#define IFRUTIL_COPYTRAITS_H

#include "Interfaces/Runtime/IFR_Types.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

IFR_BEGIN_NAMESPACE

struct IFRUtil_TrueType
{};

struct IFRUtil_FalseType
{};


/**
 * Copy traits.
 * These traits permit safe copying. The default implementation does a simple
 * assignment, which is ok for all types that don't need the extra check.
 */
template <class T>
class IFRUtil_CopyTraits
{
public:
    /**
     * The argument type of the default constructor.
     */
#if !defined(AIX) && !defined(LINUX) && !defined(OSF1) && !defined(HPUX)
    typedef typename T ConstructorType;
#else
    typedef T ConstructorType;
#endif
    typedef IFRUtil_TrueType IsCopyConstructible;
    typedef IFRUtil_TrueType IsDefaultConstructible;

    /**
     * Safe copying. Copies the value of @c source to @c target, if
     * @c check is @c true, and sets @c check to false if there
     * is an out-of-memory condition raised.
     */
    static inline void copy(T& target, const T& source, IFR_Bool& check)
    {
        if(check) {
            target = source;
        }
        return;
    }

    static inline ConstructorType init_helper (SAPDBMem_IRawAllocator* allocator, const T& source)
    {
        return ConstructorType(source);
    }
};

IFR_END_NAMESPACE

#endif
