/****************************************************************************

  module      : Converter_ArchiveContainer.cpp

  -------------------------------------------------------------------------

  responsible : henrik

  author:       henrik

  special area: Converter

  description :

  see also    :

  copyright:    Copyright (c) 2003-2004 SAP AG

  -------------------------------------------------------------------------

  ========== licence begin  GPL
  Copyright (c) 2003-2004 SAP AG

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



#include "Converter/Converter_ArchiveContainer.hpp"
#include "FreeBlockManagement/FBM_IManager.hpp"


/*!*****************************************************************************

   class: Converter_ArchiveContainer

*******************************************************************************/
SAPDB_Bool Converter_ArchiveContainer::Append
    (Converter_LeafPage&      leafpage,
     Converter_PageFrameAllocator&  framealloc)
{

//    SAPDBERR_ASSERT_STATE (m_AddressingMode == leafpage.ReadPageRecoveryMode().GetAddressingMode());

    // enlarge page vector if necessary:
    if (m_PageVector.IsFull())
        if (!m_PageVector.Reserve(m_PageVector.GetSize() + m_SizeDelta))
            return false;


    Converter_Version        ConvVersion  = leafpage.ReadVersion(); 
    Converter_ParentReference parent      = leafpage.ReadParentReference();
    Data_PageNo              basepageno   = leafpage.ReadBasePageNo();
    Data_PageRecoveryMode    rec          = leafpage.ReadPageRecoveryMode();

    Converter_PageEntryIterator start = leafpage.Begin();
    Converter_PageEntryIterator end = leafpage.End();

    // get new page from frame control:
    Converter_LeafPage newLeaf = Converter_LeafPage(framealloc.New( false ), ConvVersion, parent, basepageno, rec);

    IOMan_BlockAddress dummy; 

    while (start != end)
    {
        if (start.EntryInUse())
        {
            IOMan_BlockAddress blockAddress = start.GetBlockAddress();
            if (RTE_VolumeAccessModeSequential == FBM_IManager::Instance().GetVolMode(blockAddress.GetDeviceNo())) // ...
            {
                newLeaf.ReserveSpecifiedEntry(start.GetPosition());
                newLeaf.WriteBlockAddress(start.GetPosition(), blockAddress, dummy);
            }
        }
        ++start;
    }
    m_PageVector.InsertEnd(newLeaf);

    return true;
}
