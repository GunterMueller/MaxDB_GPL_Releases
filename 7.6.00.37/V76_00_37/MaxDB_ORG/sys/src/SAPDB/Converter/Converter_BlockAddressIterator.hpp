/*!
  @file           Converter_BlockAddressIterator.hpp
  @author         Henrik
  @ingroup        Converter
  @brief
*/


/*
  ========== licence begin  GPL
  Copyright (c) 2000-2004 SAP AG

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
*/

#ifndef CONVERTER_BLOCKADDRESSITERATOR_HPP
#define CONVERTER_BLOCKADDRESSITERATOR_HPP

#include "IOManager/IOMan_IBlockAddressIterator.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "Converter/Converter_LeafPage.hpp"


class Converter_BlockAddressIterator : public IOMan_IBlockAddressIterator
{

public:

    /*! @name Constructors and initialization */

    /*!
       @brief          constructor
       @param          leafPage
       @return         none
     */
    Converter_BlockAddressIterator(Converter_LeafPage &leafPage);

    SAPDB_Bool hasMoreElements(void);

    IOMan_BlockAddress getNextElement(void);

    Converter_PageEntryNo GetPosition() const;

    const Converter_LeafPage           &m_LeafPage;
    Converter_PageEntryIterator         m_PageEntryIter;
    Converter_PageEntryIterator         m_CurPageEntryIter;
    Converter_PageEntryIterator         m_PageEntryEndIter;
};

#endif
