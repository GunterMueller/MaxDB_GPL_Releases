/*!
    @file     SAPDBAlgo_Comparator.hpp
    @ingroup  SAPDBAlgo
    @author   DirkT
    @brief    SAPDBAlgo_IComparator Interface to be used with SAPDBAlgo_QuickSort and SAPDBAlgo_FunnelMerge
    @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef SAPDBALGO_COMPARATOR
#define SAPDBALGO_COMPARATOR


/*!
    @class          SAPDBAlgo_IComparator
    @brief          Comparator interface necessary for SAPDBAlgo_QuickSort, SAPDBAlgo_FunnelMerge and others
 */
template <class TData>
class SAPDBAlgo_IComparator
{
public:
    /*!
       @brief   IsEqual
       @param   A [in]
       @param   B [in]
       @return  TRUE if A equals B, else FALSE
    */
    virtual bool IsEqual( TData const& A, TData const& B ) = 0;

    /*!
       @brief   IsPredecessor
       @param   A [in]
       @param   B [in]
       @return  TRUE if A preceds B (e.g. A<B), else FALSE (e.g. A=B or A>B). Must return FALSE if equal otherwise SAPDBAlgo_QuickSort may crash!
    */
    virtual bool IsPredecessor( TData const& A, TData const& B ) = 0;
};

#endif
