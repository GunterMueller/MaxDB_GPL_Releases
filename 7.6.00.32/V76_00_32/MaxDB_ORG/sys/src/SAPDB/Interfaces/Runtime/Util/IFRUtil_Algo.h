/*!
  @file           IFRUtil_Algo.h
  @author         D039759
  @ingroup        IFR_Util
  @brief          Utility template functions that do not fit elsewhere.
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
#ifndef IFRUTIL_ALGO_H
#define IFRUTIL_ALGO_H

#include "Interfaces/Runtime/IFR_Types.h"

IFR_BEGIN_NAMESPACE

/**
 * Value exchange. Exchanges two values creating an intermediate temporary.
 * @param a First Value.
 * @param b Second Value.
 */
template <class Value>
inline void IFRUtil_Swap(Value& a, Value& b)
{
    Value tmp=a;
    a=b;
    b=tmp;
    return;
}

/**
 * Lower Bound. Performs a binary search in a sorted range <code>[first, second)</code>.
 * Both <code>first</code> and <code>last</code> must have the '-' and
 * the '+' operation (similar to pointer arithmetic).
 * @param first The first iterator.
 * @param last The second iterator.
 * @param value The value to look for.
 * @return An iterator pointing to the element or the element that is
 * less than or equal to the searched, or <code>last</code> if the
 * element is not found.
 */
template <class Iterator, class Type>
inline Iterator IFRUtil_LowerBound(Iterator first, Iterator last, const Type& value)
{
    IFR_ptrdiff_t len = (IFR_ptrdiff_t) (last - first);
    IFR_ptrdiff_t half;
    Iterator middle;
    
    while(len > 0) {
        half = len >> 1;
        middle = first + half;
        if((Type)(*middle) < value) {
            first=middle;
            ++first;
            len = len - half - 1;
        } else {
            len = half;
        }
    }
    return first;
}

IFR_END_NAMESPACE

#endif  
