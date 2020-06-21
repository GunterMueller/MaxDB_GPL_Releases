/*!
  @file           IFRUtil_Pair.h
  @author         D039759
  @ingroup        IFR_Util
  @brief          Pair implementation, for map

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
#ifndef IFRUTIL_PAIR_H
#define IFRUTIL_PAIR_H

#include <stdio.h>
#include <assert.h>

#include "Interfaces/Runtime/IFR_Types.h"

IFR_BEGIN_NAMESPACE

/**
 * @ingroup IFR_Util
 * @brief Pair type, similar to STL.
 *
 * This pair type is used in associative containers to associate the first
 * element somehow to the second.
 */
template <class T1, class T2>
struct IFRUtil_Pair
{
    typedef T1 FirstType;   //!< The type of the first element.
    typedef T2 SecondType;  //!< The type of the second element.
    
    T1 first;               //!< The first element.
    T2 second;              //!< The second element.
    
    /**
     * Default constructor, call the default 
     * constructors of <code>first</code> and 
     * <code>second</code>.
     */
    inline IFRUtil_Pair()
    :first(T1()),
     second(T2())
    {}

    /**
     * Constructor.
     * @param a Initialization value for <code>first</code>.
     * @param b Initialization value for <code>second</code>.
     */
    inline IFRUtil_Pair(const T1& a, const T2& b) 
    :first(a),
     second(b)
    {}
};

/**
 * @ingroup ifrcore
 * @brief Function class which does select the first element of a pair.
 */
template <class Pair>
struct IFRUtil_Select1st
{
    /**
     * The pair type used as argzument. This must have a <code>first</code> member,
     * and must define the nested type <code>FirstType</code>, which must be the
     * type of <code>first</code>.
     */
    typedef Pair ArgumentType;
    typedef IFR_TYPENAME Pair::FirstType ResultType;  //!< The result type.
    
    /**
     * Retrieves the 1st element of a pair.
     * @param x The pair.
     * @return <code>x.first</code>
     */
    inline const ResultType& operator()(const Pair& x) const 
    {
        return x.first;
    }
};  
    
IFR_END_NAMESPACE

#endif
