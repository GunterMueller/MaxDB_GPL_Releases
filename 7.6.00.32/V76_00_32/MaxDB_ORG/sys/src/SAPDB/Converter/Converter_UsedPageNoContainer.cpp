/****************************************************************************

  module      : Converter_UsedPageNoContainer.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  author:       TillL

  special area: Converter

  description :

  see also    :

  copyright:    Copyright (c) 2001-2004 SAP AG

  -------------------------------------------------------------------------

  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end

*****************************************************************************/



#include "Converter/Converter_UsedPageNoContainer.hpp"



 /*!*****************************************************************************

   class: Converter_UsedPageNoContainer

*******************************************************************************/
SAPDB_Bool Converter_UsedPageNoContainer::Append
    (const Converter_LeafPage&      leafpage,
     Converter_PageFrameAllocator&  framealloc,
     Converter_Version              version)
{

    SAPDBERR_ASSERT_STATE (m_AddressingMode == leafpage.ReadPageRecoveryMode().GetAddressingMode());

    // check whether actual bitmap page is full:
    if (m_PageVector.IsEmpty() || m_PageVector[m_PageVector.GetSize()-1].IsFull())
    {
        // enlarge page vector if necessary:
        if (m_PageVector.IsFull())
            if (!m_PageVector.Reserve(m_PageVector.GetSize() + m_SizeDelta))
                return false;

        // get new page from frame control:
        m_PageVector.InsertEnd(framealloc.New( false ));

        Converter_PageEntryCount bitcount = leafpage.GetEntryCount();
        m_PageVector[m_PageVector.GetSize()-1].InitializeFrame(version, bitcount, m_AddressingMode);
    }

    BitMapPageIterator SlotIterator = m_PageVector[m_PageVector.GetSize()-1].End();
    SlotIterator.WriteDataPageNo(leafpage.ReadBasePageNo());
    SlotIterator.WriteBitMap(leafpage.GetInUseFlagsBlock());

    m_PageVector[m_PageVector.GetSize()-1].IncFilledSlotCount();
    return true;
}
// -----------------------------------------------------------------------------




