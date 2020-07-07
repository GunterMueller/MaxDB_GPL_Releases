/*!
  @file           Data_SplitSpaceWriter.hpp
  @author         UweH
  @brief          defines class Data_SplitspaceWriter

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
#ifndef Data_SplitspaceWriter_HPP
#define Data_SplitspaceWriter_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "DataAccess/Data_ISplitSpace.hpp"

/// for context info
class SAPDBErr_MessageList;

/*!
   @class  Data_SplitSpaceWriter
   @brief  This handle is used to write to a Data_SplitspaceWriter
 */

class Data_SplitSpaceWriter
{
public:
    /// redefine PartNo
    typedef Data_ISplitSpace::PartNo PartNo;
    /// This is used to create an invalid writer
    Data_SplitSpaceWriter()
        : m_pSplitSpace (0)
    {
        Reset();
    }
    /// This is used to write to a split space. Space must be assigned.
    Data_SplitSpaceWriter (Data_ISplitSpace &splitSpace)
        : m_pSplitSpace (&splitSpace)
    {
        Reset();
    }
    /// This is used to reset the internal values if the split space was changed outside. Space must be assigned.
    void Reset ()
    {
        m_CurrentPart = 0;
        if ( m_pSplitSpace != 0 )
        {
            (void)m_pSplitSpace->CheckAlignment();
            m_pSplitSpace->GetPart (m_CurrentPart, m_CurrentLength, m_CurrentValue);
        }
        else
        {
            m_CurrentLength = 0;
            m_CurrentValue  = 0;
        }
        m_CurrentPartRestLength = m_CurrentLength;
    }
    /// This method updates internal values inthe case that the underlying
    /// splitspace has been changed between a break/continue (new for PTS 1135389)
    void UpdateAfterBreak ()
    {
        if ( m_pSplitSpace != 0 )
            m_pSplitSpace->GetPart (m_CurrentPart, m_CurrentLength, m_CurrentValue);
            // PTS 1135389 mb 2005-05-06 in case of an redoentry which had been "breaked"
            // do not change the currentRestLength, but keep them for continuing the
            // copying of the logentry
        else
        {
            m_CurrentPart = 0;
            m_CurrentLength = 0;
            m_CurrentValue  = 0;
            m_CurrentPartRestLength = 0;
        }
    }
    /// Definition of the result
    enum Result
    {
        ok, moreSpaceAvailable, noMoreToWrite, spaceToSmall
    };
    /*!
       @brief The specified space is reserved, if length can be allocated in one piece.
       @param Length [in] - length of space to reserve
       @param Space [out] - pointer to continous space.
       @return ok, moreSpaceAvailable or spaceToSmall
     */
    Result Reserve (SAPDB_UInt  Length,
                   SAPDB_Byte* &Space);
    /*!
        @brief The given part (pointer+length) is written to underlaying the split space.
        @param SourcePart [in] - source value to copy
        @param SourcePartLength [in] - length of source value
        @return ok means: everything, which should be copied was copied.
                moreSpaceAvailable means: more can be moved to current space.
                spaceToSmall means: The split space is not big enough.
     */
    Result Write (const SAPDB_Byte   *SourcePart,
                  const SAPDB_UInt    SourcePartLength);
                  
    /// skipes the given number of bytes in order to allow a resume of an interrupted writing
    /// (PTS 1137832 mb 2005-11-08)
    Result SkipBytes (const SAPDB_UInt bytesToBeSkipped)
    {
        return Write(0, bytesToBeSkipped);
    }
    
    /// Writes the members to the trace file.
    void WriteToTrace (const char* title = 0) const;

    /// Append context info as messagelist
    void AppendContextInfo (SAPDB_Char  const * const  filename,
                            SAPDB_UInt4 const          linenumber,
                            SAPDBErr_MessageList      &errlist,
                            SAPDB_Char  const * const  title = 0) const;
    /// returns true if internal used split space is assigned
    bool IsAssigned () const
    {
        return m_pSplitSpace != 0 && m_pSplitSpace->IsAssigned();
    }
    /// returns true, if the writer has a splitspace
    bool HasSplitSpace () const
    {
        return m_pSplitSpace != 0;
    }
private:

    /// this is used, to take the next part of the space.
    void TakeNextPart();

private:

    /// The space, which should be accessed.
    Data_ISplitSpace *m_pSplitSpace;
    /// Current part no of the space.
    PartNo m_CurrentPart;
    /// Current value of current part
    SAPDB_Byte* m_CurrentValue;
    /// Length of m_CurrentValue.
    SAPDB_UInt m_CurrentLength;
    /// current part rest length
    SAPDB_UInt m_CurrentPartRestLength;
};
#endif // Data_SplitspaceWriter_HPP
