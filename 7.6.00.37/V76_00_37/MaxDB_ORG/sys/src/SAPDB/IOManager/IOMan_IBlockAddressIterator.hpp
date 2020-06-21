/*!
    @file           IOMan_IBlockAddressIterator.hpp
    @author         Henrik
    @ingroup        IOManagement
    @brief          

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

#ifndef IOMAN_IBLOCKADDRESSITERATOR_HPP
#define IOMAN_IBLOCKADDRESSITERATOR_HPP


#include "SAPDBCommon/SAPDB_Types.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"

/*!
    @class  IOMan_IBlockAddressIterator
    @brief  general interface for IOMan_BlockAddress iterators
 */

class IOMan_IBlockAddressIterator
{
public:

    /*!
      @brief  check for more valid elements in iterator set 
      @return 
    */
    virtual SAPDB_Bool hasMoreElements() = 0;

    /*!
      @brief  get next element from iterator set
      @return IOMan_BlockAddress
    */
    virtual IOMan_BlockAddress getNextElement() = 0;

};

#endif // IOMAN_IBLOCKADDRESSITERATOR_HPP
