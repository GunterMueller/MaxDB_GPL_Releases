/*!
  @file           IFR_FetchChunk.cpp
  @author         ThomasS
  @ingroup        IFR_Fetch
  @brief          Handles a piece of a resultset
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
#include "SAPDB/Interfaces/Runtime/IFR_FetchChunk.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_Part.h"



//======================================================================
// IFR_FetchChunk
//======================================================================
#define DBUG_FETCHCHUNK_METHOD_ENTER(x, y) DBUG_CONTEXT_METHOD_ENTER(x, y, m_replypacket.getLock())

//----------------------------------------------------------------------
IFR_FetchChunk::IFR_FetchChunk (int type, 
                                int absoluteStartRow,
                                IFRPacket_ReplyPacket& replyPacket,
                                int recordSize,
                                int maxRows,
                                int rowsInResultSet,
                                IFR_FetchChunk *previousChunk,
                                IFR_Bool& memory_ok,
                                IFR_UInt4 *fetchRowsCounter,
                                IFR_UInt8 *dataReadSizeCounter)
:IFRUtil_RuntimeItem(*(replyPacket.getLock())),
 m_Type (type),
 m_RecordSize (recordSize),
 m_RowsInResultSet (rowsInResultSet),
 m_AbsoluteStartRow (absoluteStartRow),
 m_MaxRows (maxRows),
 m_First (false),
 m_Last (false),
 m_ReplySegment (replyPacket),
 m_replypacket(replyPacket)
{
    if(memory_ok) {

        m_CursorClosed = m_ReplySegment.isResultSetClosed();

        IFRPacket_DataPart dp;
        m_ReplySegment.getPart(dp);
        dp.setCurrentRecord (0);
        dp.setRecordSize (recordSize);
        if(dp.isValid()) {
            *fetchRowsCounter += dp.getPartArguments();
            *dataReadSizeCounter += dp.getBufferLength();
            if(previousChunk) {
                IFR_FetchChunkDataPart *op = previousChunk->getCopiedData();
                if(op->Length() == dp.Length()) {
                    IFR_Byte *data = (IFR_Byte *)op->GetRawPart();
                    op->Invalidate();
                    memcpy(data, dp.GetRawPart(), sizeof(tsp1_part_header) + dp.Length());
                    SAPDBMem_IRawAllocator& tmpallocator=m_replypacket.getLock()->allocator;
                    new (&m_copieddata) IFR_FetchChunkDataPart(data, 
                                                               0, 
                                                               recordSize, 
                                                               dp.getEncoding(),
                                                               tmpallocator);
                    
                }
            }
            // fall through if old data part hadn't the same size
            if(m_copieddata.GetRawPart() == 0) {
                IFR_size_t copysize=sizeof(tsp1_part_header) + dp.Length();
                SAPDBMem_IRawAllocator& tmpallocator=m_replypacket.getLock()->allocator;
                IFR_Byte *bsource=(IFR_Byte*) dp.GetRawPart();
                IFR_Byte *bdest = (IFR_Byte *) IFR_ALLOCATOR(tmpallocator).Allocate(copysize);
                if(bdest == 0) {
                    memory_ok = false;
                    return;
                }
                memcpy(bdest, bsource, copysize);
                new (&m_copieddata) IFR_FetchChunkDataPart(bdest, 
                                                           0, 
                                                           recordSize, 
                                                           dp.getEncoding(),
                                                           tmpallocator);
            }
        }
    }
    m_replypacket.releaseLock(); 
}

//----------------------------------------------------------------------
IFR_FetchChunk::IFR_FetchChunk(PIn_ReplySegment& replysegment,
                               IFRPacket_ReplyPacket& replypacket,
                               IFR_Int4 maxrows,
                               IFR_Bool& memory_ok,
                               IFR_UInt4 *fetchRowsCounter,
                               IFR_UInt8 *dataReadSizeCounter)
:IFRUtil_RuntimeItem(*(replypacket.getLock())),
 m_Type(IFR_TYPE_FIRST),
 m_RecordSize(-1),
 m_RowsInResultSet(-1),
 m_AbsoluteStartRow(1),
 m_MaxRows(maxrows),
 m_First(false),
 m_Last(false),
 m_ReplySegment(replysegment, replypacket.getEncoding()),
 m_replypacket(replypacket)
{
    if(memory_ok) {

        m_CursorClosed = m_ReplySegment.isResultSetClosed();

        IFRPacket_DataPart dp;
        m_ReplySegment.getPart(dp);
        dp.setCurrentRecord (0);
        dp.setRecordSize (-1);
        if(dp.isValid()) {
           *fetchRowsCounter += dp.getPartArguments();
           *dataReadSizeCounter += dp.getBufferLength();
            // fall through if old data part hadn't the same size
            if(m_copieddata.GetRawPart() == 0) {
                IFR_size_t copysize=sizeof(tsp1_part_header) + dp.Length();
                SAPDBMem_IRawAllocator& tmpallocator=m_replypacket.getLock()->allocator;
                IFR_Byte *bsource=(IFR_Byte*) dp.GetRawPart();
                IFR_Byte *bdest = (IFR_Byte *) IFR_ALLOCATOR(tmpallocator).Allocate(copysize);
                if(bdest == 0) {
                    memory_ok = false;
                    return;
                }
                memcpy(bdest, bsource, copysize);
                new (&m_copieddata) IFR_FetchChunkDataPart(bdest, 
                                                           0, 
                                                           -1, 
                                                           dp.getEncoding(),
                                                           tmpallocator);
            }
        }
    }
    m_replypacket.releaseLock(); 
}


//----------------------------------------------------------------------
IFR_FetchChunk::~IFR_FetchChunk()
{
}

IFR_Retcode IFR_FetchChunk::init ()
{
    DBUG_METHOD_ENTER(IFR_FetchChunk, init);
    IFR_Retcode rc = IFR_OK;
    
    this->m_ChunkSize = m_copieddata.getPartArguments();
    // IFR_Byte *replydata = m_ReplySegment.getPartDataPos ();
    m_CurrentOffset = 0;
    m_CurrentRecord = (IFR_Byte*)(m_copieddata.GetRawPart()) + (m_CurrentOffset + this->m_RecordSize);
    if (m_AbsoluteStartRow > 0) {
        m_StartIndex = m_AbsoluteStartRow;
        m_EndIndex = m_AbsoluteStartRow + m_ChunkSize - 1;
    } else {
        if (m_RowsInResultSet != -1) {
            m_StartIndex = m_RowsInResultSet - m_AbsoluteStartRow + m_ChunkSize;
            m_EndIndex = m_StartIndex + m_ChunkSize - 1;
        } else {
            m_StartIndex = m_AbsoluteStartRow;
            m_EndIndex = m_AbsoluteStartRow + m_ChunkSize - 1;
        }
    }
    //>>> SQL TRACE
    IFR_SQL_TRACE << "FETCH BUFFER START: " << m_StartIndex << endl
                  << "FETCH BUFFER END  : " << m_EndIndex << endl;
    //<<< SQL TRACE
    determineFlags (m_MaxRows);
    DBUG_RETURN(rc);
}

void IFR_FetchChunk::determineFlags (int maxRows)
{
    bool wasLastPart = m_copieddata.isLastPacket();
    
    if (wasLastPart) {
    if ((this->m_Type == IFR_TYPE_FIRST) ||
        (this->m_Type == IFR_TYPE_LAST) ||
        (this->m_Type == IFR_TYPE_RELATIVE_DOWN)) {
        this->m_First = true;
        this->m_Last = true;
    } else if ((this->m_Type == IFR_TYPE_ABSOLUTE_UP) ||
               (this->m_Type == IFR_TYPE_ABSOLUTE_DOWN) ||
               (this->m_Type == IFR_TYPE_RELATIVE_UP)) {
        this->m_Last = true;
    }
    }
    
    if (m_StartIndex == 1) {
        this->m_First = true;
    }
    if (m_EndIndex == -1) {
        this->m_Last = true;
    }
    if (maxRows > 0 && isForward () && (m_EndIndex >= maxRows)) {
        this->m_EndIndex = maxRows;
        this->m_ChunkSize = maxRows + 1 - m_StartIndex;
        this->m_Last = true;
    }
}

bool IFR_FetchChunk::setRow (int row)
{
    if ((m_StartIndex <= row) && (m_EndIndex >= row)) {
        unsafeMove (row - m_StartIndex - m_CurrentOffset);
        return true;
    }
    
    // some tricks depending on whether we are on last/first chunk
    if (isForward () && m_Last && (row < 0) && (row >= m_StartIndex - m_EndIndex - 1)) {
        // move backward to the row from the end index, but 
        // honor the row number start at 1, make this
        // relative to chunk by subtracting start index
        // and relative for the move by subtracting the 
        // current offset
        unsafeMove (m_EndIndex + row + 1 - m_StartIndex - m_CurrentOffset);
        return true;
    }
    if (!isForward() && m_First && (row > 0) && (row <= m_EndIndex - m_StartIndex + 1)) {
        // simple. row is > 0. m_StartIndex if positive were 1 ...
        unsafeMove (row - 1 - m_CurrentOffset); 
    }
    // if we know the number of rows, we can compute this anyway
    // by inverting the row
    if (m_RowsInResultSet != -1 &&
        ((m_StartIndex < 0 && row > 0) || (m_StartIndex > 0 && row < 0))) {
    int inverted_row = (row > 0) 
        ? (row - m_RowsInResultSet - 1)
        : (row + m_RowsInResultSet + 1);
    return setRow (inverted_row);
    }
    
    return false;
}

void IFR_FetchChunk::unsafeMove (int relativepos)
{
    this->m_CurrentOffset += relativepos;
    this->m_CurrentRecord += relativepos * this->m_RecordSize;
    m_copieddata.setCurrentRowStart(this->m_CurrentOffset * this->m_RecordSize);
}





bool IFR_FetchChunk::containsRow (int row)
{
  if ((m_StartIndex <= row) && (m_EndIndex >= row)) {
    return true;
  }
  // some tricks depending on whether we are on last/first chunk
  if (isForward () && m_Last && (row < 0)) {
      return row >= m_StartIndex - m_EndIndex - 1; 
  }
  if (!isForward() && m_First && (row > 0)) {
    return row <= m_EndIndex - m_StartIndex + 1;
  }
  // if we know the number of rows, we can compute this anyway
  // by inverting the row
  if (m_RowsInResultSet != -1 &&
      (((m_StartIndex < 0) && (row > 0)) || ((m_StartIndex > 0) && (row < 0)))) {
    int inverted_row = (row > 0) 
      ? (row - m_RowsInResultSet - 1)
      : (row + m_RowsInResultSet + 1);
    return m_StartIndex <= inverted_row && m_EndIndex >= inverted_row;
  }
  
  return false;
}

bool IFR_FetchChunk::move (int relativepos)
{
  if ((m_CurrentOffset + relativepos < 0) || 
      (m_CurrentOffset + relativepos >= m_ChunkSize)) {
    return false;
  } else {
    unsafeMove (relativepos);
    return true;
  }
}

IFR_Retcode IFR_FetchChunk::getCurrentData (IFRPacket_DataPart& part)
{
    DBUG_FETCHCHUNK_METHOD_ENTER(IFR_FetchChunk, getCurrentData);
    part = m_copieddata;
    if(part.isValid()) {
        DBUG_RETURN(IFR_OK);
    } else {
        DBUG_RETURN(IFR_NOT_OK);
    }
}

void IFR_FetchChunk::moveToUpperBound ()
{
  int relativepos = m_ChunkSize - m_CurrentOffset - 1;
  this->m_CurrentRecord += relativepos * this->m_RecordSize;
  this->m_CurrentOffset = m_ChunkSize - 1;
}




