/*!*****************************************************************************

  module:       Log_QueueRingBuffer.cpp

  ------------------------------------------------------------------------------

  author      : TillL
  responsible : UweH

  special area: Logging

  description:  see .hpp

  see also:

  ------------------------------------------------------------------------------

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


  *****************************************************************************/


#include "Logging/Log_QueueRingBuffer.hpp"


// dummy implementation of frame allocator:
Log_FrameAllocator* Log_FrameAllocator::m_Instance = 0;


/*******************************************************************************

   class: Log_QueueRingBuffer

*******************************************************************************/
SAPDB_Bool Log_QueueRingBuffer::Initialize
    (EntryCount                     count)
{
    Log_Page                page;
    Kernel_Page::PageFrame  frame;
    QueueEntry              entry(GetAllocator());
    Iterator                iter;
 
    for (EntryCount i=0; i<count; i++)
    {
        // -------------------------------------
        // get new frame and initialize it with default values
        // -------------------------------------
        page.SetFrame(m_FrameAlloc.New());
        if (!page.IsAssigned())                // alloc failed
        {
            Delete();
            return false;
        }
        
        page.InitializeFrame(QueueID(), Log_IOSequenceNo());
        entry.SetFrame(page.GetFrame());
        
        // -------------------------------------
        // insert entry
        // -------------------------------------
        if (i == 0)
            iter = Insert(entry);
        else
            iter = InsertBehind(iter, entry);

        if (!iter.IsValid())                    // alloc failed
        {
            Delete();
            return false;
        }
    }
    return true;
}
// -----------------------------------------------------------------------------
void Log_QueueRingBuffer::Delete()
{
    Iterator pos = Anchor();
    while (!IsEmpty()) 
    {   
        Delete(pos);    
    }
}
// -----------------------------------------------------------------------------
void Log_QueueRingBuffer::Delete
    (Iterator&                      pos)
{
    SAPDBERR_ASSERT_STATE(pos.IsValid());
    Kernel_Page::PageFrame frame = (*pos).GetPage().GetFrame();
    m_FrameAlloc.Free(frame);
    List::Delete(pos);
}
// -----------------------------------------------------------------------------
