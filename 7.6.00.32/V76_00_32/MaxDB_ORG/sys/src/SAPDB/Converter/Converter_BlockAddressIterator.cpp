/*!*****************************************************************************

  module:       Converter_BlockAddressIterator

  ------------------------------------------------------------------------------

  responsible:  Henrik

  author:       Henrik

  special area: Converter

  description:  see .hpp

  see also:

  Copyright (c) 2000-2004 SAP AG

  ------------------------------------------------------------------------------

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

*******************************************************************************/


#include "Converter/Converter_BlockAddressIterator.hpp"

/*******************************************************************************

  class: Converter_BlockAddressIterator

*******************************************************************************/

Converter_BlockAddressIterator::Converter_BlockAddressIterator(Converter_LeafPage &leafPage):
    m_LeafPage(leafPage),
    m_PageEntryIter(leafPage.Begin()),
    m_CurPageEntryIter(leafPage.Begin()),
    m_PageEntryEndIter(leafPage.End())
{
    //search first
    while ((m_PageEntryIter != m_PageEntryEndIter) &&
           (!m_PageEntryIter.EntryInUse() || m_PageEntryIter.GetPackedBlockAddress().IsReserved()))
    {
        ++m_PageEntryIter;
    }

};


SAPDB_Bool
Converter_BlockAddressIterator::hasMoreElements(){

    if (m_PageEntryIter != m_PageEntryEndIter)
    {
        return SAPDB_TRUE;
    }
    else
    {
        return SAPDB_FALSE;
    }
};

IOMan_BlockAddress
Converter_BlockAddressIterator::getNextElement() {

    m_CurPageEntryIter = m_PageEntryIter;

    //search next
    ++m_PageEntryIter;
    while ((m_PageEntryIter != m_PageEntryEndIter) &&
           (!m_PageEntryIter.EntryInUse() || m_PageEntryIter.GetPackedBlockAddress().IsReserved()))
    {
        ++m_PageEntryIter;
    }

    if (m_CurPageEntryIter != m_PageEntryEndIter)
    {
        return m_CurPageEntryIter.GetBlockAddress();
    }
    else
    {
        return IOMan_BlockAddress();  // invalid
    }
};


Converter_PageEntryNo
Converter_BlockAddressIterator::GetPosition() const
{
    return m_CurPageEntryIter.GetPosition();
};

