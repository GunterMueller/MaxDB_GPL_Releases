/*!
  @file           IFR_FetchChunk.h
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
#ifndef IFR_FETCHCHUNK_H
#define IFR_FETCHCHUNK_H

#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/Packet/IFRPacket_ReplySegment.h"

//======================================================================
// IFR_FetchChunkDataPart
//======================================================================
// Mimics all behaviour of the data part, except that the destructor 
// will clear the memory occupied, as it was allocated and copied 
// before.
class IFR_FetchChunkDataPart 
    : public IFRPacket_DataPart
{
public:
    inline IFR_FetchChunkDataPart()
    :allocator(0)
    {}

    inline IFR_FetchChunkDataPart(IFR_Byte *copiedbuffer, 
                           IFR_Int2 currentrecord, 
                           IFR_Int2 recordsize, 
                           IFR_StringEncoding encoding,
                           SAPDBMem_IRawAllocator& _allocator)
    :IFRPacket_DataPart((tsp1_part*)copiedbuffer, encoding, currentrecord, recordsize),
     allocator(&_allocator)
    {
    }

    inline ~IFR_FetchChunkDataPart()
    {
        IFR_Byte *to_del=(IFR_Byte*)GetRawPart();
        if(to_del) { 
            IFR_ALLOCATOR(*allocator).Deallocate(to_del);
        }
    }

private:
    SAPDBMem_IRawAllocator* allocator;
};

/**
 * The outcome of a particular fetch operation.  A fetch operation
 * results in one (when the fetch size is 1) or more (when the fetch
 * size is >1) data rows returned from the database server. Depending on
 * the kind of the fetch, the positioning in the result at the database 
 * server and the start and end index computation does differ.
 */
class IFR_FetchChunk 
    : public IFRUtil_RuntimeItem
{
public:
    enum FetchChunkFlags {
        IFR_TYPE_FIRST          = 1,
        IFR_TYPE_LAST           = 2,
        IFR_TYPE_ABSOLUTE_UP    = 3,
        IFR_TYPE_ABSOLUTE_DOWN  = 4,
        IFR_TYPE_RELATIVE_UP    = 5,
        IFR_TYPE_RELATIVE_DOWN  = 6
    };


  /**
   * Creates a new fetch chunk.
   * @param type the type of the fetch operation.
   * @param absoluteStartRow the start row of this chunk. If negative, this is calculated from 
   *   the end of the result set.
   * @param replyPacket the database server reply of the fetch
   * @param recordSize the size of one row.
   * @param maxRows the <code>m_MaxRows</code> property of the statement that created this result.
   * @param rowsInResultSet the number of rows in this result set, or -1 if not known.
   */
  IFR_FetchChunk (int type, 
                  int absoluteStartRow,
                  IFRPacket_ReplyPacket& replyPacket,
                  int recordSize,
                  int maxRows,
                  int rowsInResultSet,
                  IFR_FetchChunk *previousChunk,
                  IFR_Bool& memory_ok,
                  IFR_UInt4 *fetchRowsCounter,
                  IFR_UInt8 *dataReadSizeCounter);

    IFR_FetchChunk(PIn_ReplySegment& replysegment,
                   IFRPacket_ReplyPacket& replypacket,
                   IFR_Int4 maxRows,
                   IFR_Bool& memory_ok,
                   IFR_UInt4 *fetchRowsCounter,
                   IFR_UInt8 *dataReadSizeCounter);

  
    virtual ~IFR_FetchChunk(); 

  /**
   * Initialization of fetch chunk. Separate function, because constructor should not throw exceptions.
   *
   * @return <code>IFR_OK</code> if initialization ok; <code>IFR_NOT_OK</code> otherwise.
   */
  IFR_Retcode init ();

    /**
     * Gets the start index of the fetch chunk.
     * @return The start index (smallest valid index).
     */
    inline IFR_Int4 getStart()
    {
        return m_StartIndex;
    }

    /**
     * Gets the end index of the fetch chunk.
     * @return The end index (largest valid index).
     */
    inline IFR_Int4 getEnd()
    {
        return m_EndIndex;
    }

  /**
   * Sets the current record to the supplied absolute position.
   * @param row the absolute row. 
   * @return <code>true</code> if the row was set, <code>false</code> otherwise.
   */
  bool setRow (int row);

    /**
     * Updates the number of rows in the result set. 
     * @param rows the number of rows in the result set.
     */
    inline void setRowsInResultSet (IFR_Int4 rows)
    {
        m_RowsInResultSet = rows;
    }

    /**
     * Get the current position within the result set.
     * @return the current position in the result set.
     */
    inline IFR_Int4 getLogicalPos ()
    {
        return m_StartIndex + m_CurrentOffset;
    }

    /**
     * Get the current offset within the result set.
     * @return the current offset in the result set.
     */
    inline IFR_Int4 getCurrentOffset ()
    {
        return m_CurrentOffset;
    }

    /**
     * Called because there is a result set where the last element
     * is now interesting. This is the fact in a <code>FETCH LAST</code>
     * operation.
     */
    void moveToUpperBound ();

    inline IFR_Bool isForward()
    {
        return (m_Type == IFR_TYPE_FIRST ||
                m_Type == IFR_TYPE_ABSOLUTE_UP ||
                m_Type == IFR_TYPE_RELATIVE_UP);
    }

    /**
     * Returns whether this chunk is the first one.
     * <b>Take care, that this information may not be reliable.</b>
     * @return <code>true</code> if this is the first, and <code>false</code> if this
     *   is not first or the information is not known.
     */
    IFR_Bool isFirst ()
    {
        return m_First;
    }

    /**
     * Returns whether this chunk is the last one.
     * <b>Take care, that this information may not be reliable.</b>
     * @return <code>true</code> if this is the last, and <code>false</code> if this
     *   is not first or the information is not known.
     */
    IFR_Bool isLast()
    {
        return m_Last;
    }
    
    /**
     * Sets the <code>last</code> flag.
     * @param last the new value.
     */
    inline void setLast (IFR_Bool last) 
    {
        m_Last = last;
    }

    /**
     * Gets the size of this chunk.
     * @return the number of rows in this chunk.
     */
    inline IFR_Int4 size()
    {
        return m_ChunkSize;
    }

  /**
   * Returns whether the given row is truly inside the chunk.
   * @param row the row to check. Rows <0 count from the end of the result.
   * @return <code>true</code> if the row is inside, <code>false</code> if it's not
   * or the condition could not be determined due to an unknown end of result set.
   */
  bool containsRow (int row);

  /**
   * Moves the position inside the chunk by a relative offset.
   * @param relativepos the relative moving offset.
   * @return <code>true</code> if it was moved, <code>false</code> otherwise.
   */
  bool move (int relativepos);

    /**
     * Returns the current data part positioned at the current position, 
     * hereby acquiring the data lock.
     * @param part Data part istance to which the data part is assigned.
     * @return <code>IFR_OK</code> if all went ok, <code>
     */
    IFR_Retcode getCurrentData (IFRPacket_DataPart& part);

    inline IFR_FetchChunkDataPart *getCopiedData() 
    {
        return &m_copieddata;
    }
    
    inline void setRecordSize(IFR_Int4 recordSize)
    {
        m_RecordSize = recordSize;
        m_copieddata.setRecordSize(recordSize);
    }

    inline IFR_Bool isResultSetClosed() const 
    { 
        return m_CursorClosed; 
    }
private:
    
    /**
     * Moves the position inside the chunk by a relative offset, but unchecked.
     * @param relativepos the relative moving offset.
     */
    void unsafeMove (int relativepos);

    /**
     * Determines whether this chunk is the first and/or last of
     * a result set. This is done by checking the index boundaries,
     * and also the LAST PART information of the reply packet.
     * A forward chunk is also the last if it contains the record at
     * the <code>maxRows</code> row, as the user decided to make
     * the limit here.
     * @param maxRows the <code>maxRows</code> limit of the statement
     */
    void determineFlags (int maxRows);

    IFR_Int4 m_Type;             //!< Type of fetch chunk (IFR_TYPE_XXX constants).
    IFR_Int4 m_RecordSize;       //!< Number of bytes in a row.
    IFR_Int4 m_RowsInResultSet;  //!< Number of rows in result set.
    IFR_Int4 m_ChunkSize;        //!< Number of elements in the chunk.
    IFR_Int4 m_StartIndex;       //!< Index of 1st row in this chunk.
    IFR_Int4 m_EndIndex;         //!< Index of last row in this chunk.

    IFR_Int4 m_CurrentOffset;     //!< Current offset in this chunk, starting with 0.
    IFR_Int4 m_AbsoluteStartRow;  //!< Start row of this chunk.
    IFR_Int4 m_MaxRows;           //!< Max Rows setting-

    IFR_Bool m_Last;              //!< Flag set if the chunk is the last chunk.
    IFR_Bool m_First;             //!< Flag set if this chunk is the first chunk.
    IFR_Bool m_CursorClosed;      //!< Set when a cursor close was signalled.
    
    IFR_Byte*              m_CurrentRecord; //!< Pointer to current record in data part.
    IFRPacket_ReplySegment m_ReplySegment;  //!< Reply segment of the chunk.
    IFRPacket_ReplyPacket  m_replypacket;   //!< Reply packet of the chunk.
    IFR_FetchChunkDataPart m_copieddata;    //!< The copied data part from the original packet.
    
    friend class IFR_ResultSet;
};

#endif // IFR_FETCHCHUNK_H
