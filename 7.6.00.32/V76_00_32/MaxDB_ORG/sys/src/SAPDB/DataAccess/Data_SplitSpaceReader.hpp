/*!
  @file           Data_SplitSpaceReader.hpp
  @author         UweH
  @brief          defines class Data_SplitSpaceReader

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
#ifndef Data_SplitSpaceReader_HPP
#define Data_SplitSpaceReader_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "DataAccess/Data_ISplitSpace.hpp"

/// allocator
class SAPDBMem_IRawAllocator;
/// for context info
class SAPDBErr_MessageList;

/*!
   @class Data_SplitSpaceReader
   @brief This handle is used to read from a Data_ISplitSpace
 */
class Data_SplitSpaceReader
{
public:
    /// redefine PartNo
    typedef Data_ISplitSpace::PartNo PartNo;
    /// definition of the result
    enum Result
    {
        ok, moreSpaceAvailable, noMoreToRead, spaceToSmall
    };
    /*!
       @brief This is used to read from a split space. Space must be assigned.
       @param Space [in/out] 
       @param SpaceCanBeReleased [in] default is false
     */

    Data_SplitSpaceReader (Data_ISplitSpace &Space,
                           bool              SpaceCanBeReleased = false) // PTS 1114994 UH 2002-04-17
        : m_Space (Space),
          m_SpaceCanBeReleased(SpaceCanBeReleased) // PTS 1114994 UH 2002-04-17
    {
        Reset();
    }
    /// This is used to reset the internal values if the split space was changed outside. Space must be assigned.
    void Reset ()
    {
        m_CurrentPart = 0;
        (void)m_Space.CheckAlignment();
        m_Space.GetPart (m_CurrentPart, m_CurrentLength, m_CurrentValue);
        m_CurrentPartRestLength = m_CurrentLength;
    }
    /*!
        @param Length [in] - length of space to reserve
        @param Space [out] - pointer to continous space.
        @return NewResult - ok, moreSpaceAvailable or spaceToSmall
        @brief The specified space is reserved, if length can be allocated in one peace.
     */
    Result Reserve (SAPDB_UInt   Length,
                    SAPDB_Byte* &Space);
    /*!
        @brief This is used, if target space is not available.
        @param TargetPartLength [in] - length of value
        @param TargetPart [out] - value to copy in
        @param Allocator [in/out] - used, space must be allocated
        @param bAllocatorWasUsed [out] - true, if allocator was used, needed for destroy
        @return Result may be ok - Everything, which should be copied was copied.
                         moreSpaceAvailable - More can be moved to current space.
                         noMoreToRead - no more to read
                         spaceToSmall - if allocator fails
     */
    Result Read (SAPDB_UInt              TargetPartLength,
                 SAPDB_Byte*            &TargetPart,
                 SAPDBMem_IRawAllocator &Allocator,
                 bool                   &bAllocatorWasUsed);
    /*!
        @brief this is used, if TargetPart is completely available.
        @param TargetPart [in] - value to copy in
        @param TargetPartLength [in] - length of value
        @return Result may be: ok - Everything, which should be copied was copied.
                                           moreSpaceAvailable - More can be moved to current space.
                                           noMoreToRead - no more to read
     */
    Result Read (SAPDB_Byte* TargetPart,
                 SAPDB_UInt  TargetPartLength);
    /// Writes the members to the trace file.
    void WriteToTrace (const char* title = 0) const;

    /// Append context info as messagelist
    void AppendContextInfo (SAPDB_Char  const * const  filename,
                            SAPDB_UInt4 const          linenumber,
                            SAPDBErr_MessageList      &errlist,
                            SAPDB_Char  const * const  title = 0) const;

    /// returns true if internal used split space is assigned
    bool IsAssigned ()
    {
        return m_Space.IsAssigned();
    }

private:
    /// this is used, to take the next part of the space.
    void TakeNextPart();

private:

    /// The space, which should be accessed.
    Data_ISplitSpace &m_Space;
    /// Current part no of the space.
    PartNo m_CurrentPart;
    /// Current value of current part
    SAPDB_Byte* m_CurrentValue;
    /// Length of m_CurrentValue.
    SAPDB_UInt m_CurrentLength;
    /// current part rest length 
    SAPDB_UInt m_CurrentPartRestLength;
    /// space can be released
    bool m_SpaceCanBeReleased; // PTS 1114994 UH 2002-04-17
};
#endif // Data_SplitSpaceReader_HPP