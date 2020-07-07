/*!
  @file           gbd610.h
  @author         UweH
  @ingroup        Index
  @brief          class cbd610_RecordList

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
#ifndef GBD610_H
#define GBD610_H

#include "ggg00.h"
#include "gbd00.h"

class Msg_List;

/// internally used
#define DUMMY_REC_OFFSET_GBD610 0

/*!
 * @class cbd610_RecordList
 * @brief Defines how records can be found in an index page
 * 
 * This class represents a list of int4 positions of records in a page.
 * Especially for Index SubTree Leaves the layout is different.
 * In this case it is a list of tuples: Position [uint2] and Length [10bit] and specialinfo [6bit].
 * 
 */ 
class cbd610_RecordList
{
public:
    /// default constructor sets all to 0
    cbd610_RecordList();
    /// constructor assigns given page
    cbd610_RecordList(tbd_nodeptr pNode);
    /// The list is initialized for the given node.
    /// If the node is a subtree leaf and it is updateable and it needs to be migrated it is done.
    void Assign (tbd_nodeptr pNode);
    /// this resets the list to be Deassigned()
    void Deassign ();
    /// returns true if the list is assigned
    bool IsAssigned() const;
    /// returns the position of a given record
    SAPDB_Int4 GetPosition (SAPDB_Int4 RecIndex) const;
    /// returns the offset of a given record
    SAPDB_Int4 GetOffset (SAPDB_Int4 RecIndex) const;
    /// returns the length of the specified record
    SAPDB_Int4 GetLength (SAPDB_Int4 RecIndex) const;
    /// returns the length of the specified record
    SAPDB_Int2 GetKeyLength (SAPDB_Int4 RecIndex) const;
    /// This retrieves a record and the length.
    /// False is returned if record with given index does not exist.
    bool GetRecord (SAPDB_Int4   RecIndex,
                    bool        &withConsistentReadInfo,
                    SAPDB_Int4  &Length,
                    SAPDB_Byte* &pRecord);
    /// False is returned if the key for the given index does not exist.
    bool GetKey (SAPDB_Int4   RecIndex,
                 SAPDB_Int2  &Length,
                 SAPDB_Byte* &pKey);
    /// False is returned if the key for the given index does not exist.
    bool GetConstKey (SAPDB_Int4          RecIndex,
                      SAPDB_Int2         &Length,
                      SAPDB_Byte * &pKey) const;
    /// sets the length for a given record
    void SetPosition (SAPDB_Int4 RecIndex,
                      SAPDB_Int4 Position,
                      SAPDB_Int4 Length,
                      bool       withConsistentReadInfo);
    /// sets the deleted flag
    void SetDeleted(SAPDB_Int4 RecIndex,
                    bool       isDeleted);
    /// sets the deleted flag
    bool IsDeleted(SAPDB_Int4 RecIndex) const;
    /// prints the list to the vtrace
    void Print() const;
    /// returns the numbers of records in the page and in the list
    SAPDB_Int4 RecordCount() const;
    /// append one list to another
    bool Merge (const cbd610_RecordList &Source,
                      SAPDB_Int4         Offset,
                      Msg_List          &errlist);
    /// add a new record position to the list
    bool AddPosition (SAPDB_Int4  RecIndex,
                      SAPDB_Int4  RecPos, 
                      Msg_List   &errlist);
    /// add a new record position to the list
    bool AddPosition (SAPDB_Int4  RecIndex,
                      SAPDB_Int4  RecPos, 
                      SAPDB_Int4  RecLen,
                      bool        withConsistentReadInfo,
                      Msg_List   &errlist);
    /// remove a record position from the list
    bool DelPosition (SAPDB_Int4  RecIndex,
                      Msg_List   &errlist);
    /// Copy the given list into the current list.
    bool Copy (const cbd610_RecordList &Source,
                     Msg_List          &errlist);
    /// sort the list, so that positions are ordered
    void SortForDistribution (SAPDB_Int4 LastDistribIndex);
    /// 
    void UpdatePositions (SAPDB_Int4 TargetPos,
                          SAPDB_Int4 ShiftLen);
    /// update positions for a range of records
    void UpdatePositionsRange (SAPDB_Int4 FirstIndex,
                               SAPDB_Int4 LastIndex,
                               SAPDB_Int4 MinRecOffset,
                               SAPDB_Int4 MaxRecOffset,
                               SAPDB_Int4 ShiftLen);
    /// This function deletes the highest NumOfDeletedRecords pointerlist entries from the pointerlist
    bool DelHighIndexPositions (SAPDB_Int4 NumDeletedRecords,
                                Msg_List   &errlist);
public:
// private: but on SUN this needs to be public
    struct RecordInfoPositionLength
    {
        SAPDB_UInt2 position;
        SAPDB_UInt2 length;
    };

private:

    union RecordInfo
    {
        SAPDB_Int4               position;
        RecordInfoPositionLength posLength;
        RecordInfo()
        : position(0)
        {}
        void SetValues( SAPDB_Int4 newPosition,
                        SAPDB_Int4 newLength,
                        bool       withConsistentReadInfo)
        {
            posLength.position = newPosition;
            posLength.length   = newLength;
            // SetHasConsistentReadInfo(withConsistentReadInfo);
        }
        // maximum key length is 2047
        // 11111111111 0x7FF
        // DC.. .NNN NNNN NNNN bits: D=deleted, C=ConsistentReadInfo, N=lengthValue
        void SetDeleted(bool isDeleted)
        {
            if ( isDeleted )
                posLength.length |= 0x8000;
            else
                posLength.length &= 0x7FFF;
        }
        bool IsDeleted() const
        {
            return (posLength.length & 0x8000) != 0;
        }
        SAPDB_UInt2 GetLength() const
        {
            return posLength.length & 0x7FFF;
        }
    };
    
    RecordInfo* GetRecordInfo(SAPDB_Int4 index)
    {
        return reinterpret_cast<RecordInfo*>(&m_pNode->nd_pointer_list()[MAX_POINTERINDEX_BD00 - POS_OFF_DIFF_BD00])
               -
               index;
    }

    RecordInfo const * GetConstRecordInfo(SAPDB_Int4 index) const
    {
        return reinterpret_cast<RecordInfo const *>(&m_pNode->nd_pointer_list()[MAX_POINTERINDEX_BD00 - POS_OFF_DIFF_BD00])
               -
               index;
    }

    SAPDB_UInt1 Version() const
    {
        return m_pNode->ndPageVersion_bd00();
    }
    
    SAPDB_Int4 MaxRecordIndex() const
    {
        return m_pNode->nd_record_cnt() - 1;
    }
    
    void Sort (SAPDB_Int4  Version,
               SAPDB_Int4  NumRecPosElem,
               RecordInfo *pMinRecPosElem);

    void SortVersion0 (SAPDB_Int4  NumRecPosElem,
                       RecordInfo *pMinRecPosElem);

    void SortVersion1 (SAPDB_Int4  NumRecPosElem,
                       RecordInfo *pMinRecPosElem);

    tbd_nodeptr m_pNode;
};
// -------------------------------------------------------------------------------------
inline
cbd610_RecordList::cbd610_RecordList()
{
    Deassign();
}
// -------------------------------------------------------------------------------------
inline
cbd610_RecordList::cbd610_RecordList(tbd_nodeptr pNode)
{
    Assign(pNode);
}
// -------------------------------------------------------------------------------------
inline void
cbd610_RecordList::Assign(tbd_nodeptr pNode)
{
    m_pNode = pNode;
}
//-------------------------------------------------------------------------------------
inline void
cbd610_RecordList::Deassign()
{
    m_pNode = 0;
}
// -------------------------------------------------------------------------------------
inline bool
cbd610_RecordList::IsAssigned() const
{
    return m_pNode != 0;
}
//-------------------------------------------------------------------------------------
inline bool cbd610_RecordList::AddPosition (SAPDB_Int4  RecIndex,
                                            SAPDB_Int4  RecPos, 
                                            Msg_List   &errlist)
{
    return AddPosition (RecIndex, RecPos, -1, false, errlist);
}
//-------------------------------------------------------------------------------------
inline SAPDB_Int4
cbd610_RecordList::GetPosition(SAPDB_Int4 RecIndex) const
{
    return Version() == 0
           ? GetConstRecordInfo(RecIndex)->position
           : GetConstRecordInfo(RecIndex)->posLength.position;
}
//-------------------------------------------------------------------------------------
inline SAPDB_Int4
cbd610_RecordList::GetOffset(SAPDB_Int4 RecIndex) const
{
    return GetPosition(RecIndex)-POS_OFF_DIFF_BD00;
}
//-------------------------------------------------------------------------------------
inline SAPDB_Int4
cbd610_RecordList::GetLength(SAPDB_Int4 RecIndex) const
{
    const RecordInfo *pElem = GetConstRecordInfo(RecIndex);

    if ( pElem == 0 )
        return -1;
    
    if ( Version() > 0 )
        return pElem->GetLength();

    const tsp00_BytePtr pSpace = reinterpret_cast<const tsp00_BytePtr>(m_pNode) + pElem->position - POS_OFF_DIFF_BD00;  
    return reinterpret_cast<const tgg00_RecPtr>(pSpace)->recLen_gg00();
}
//-------------------------------------------------------------------------------------
inline SAPDB_Int2
cbd610_RecordList::GetKeyLength(SAPDB_Int4 RecIndex) const
{
    const RecordInfo *pElem = GetConstRecordInfo(RecIndex);

    if ( pElem == 0 )
        return -1;
    
    if ( Version() > 0 )
        return pElem->GetLength(); // subtree leaves have only a key as a record

    const tsp00_BytePtr pSpace = reinterpret_cast<const tsp00_BytePtr>(m_pNode) + pElem->position - POS_OFF_DIFF_BD00;  
    return reinterpret_cast<const tgg00_RecPtr>(pSpace)->recKeyLen_gg00();
}
//-------------------------------------------------------------------------------------
inline bool
cbd610_RecordList::GetRecord (SAPDB_Int4   RecIndex,
                              bool        &withConsistentReadInfo,
                              SAPDB_Int4  &Length,
                              SAPDB_Byte* &pRecord)
{
    if ( RecIndex < 0 || RecIndex > MaxRecordIndex() )
        return false;
        
    RecordInfo* recInfo = GetRecordInfo(RecIndex); 

    if ( Version() > 0 )
    {
        pRecord = reinterpret_cast<SAPDB_Byte*>(m_pNode) + recInfo->posLength.position - POS_OFF_DIFF_BD00;
        Length  = recInfo->GetLength();
        withConsistentReadInfo = false; // +++++++++++++++
    }
    else
    {
        pRecord = reinterpret_cast<SAPDB_Byte*>(m_pNode) + recInfo->position - POS_OFF_DIFF_BD00;
        Length  = reinterpret_cast<tgg00_RecPtr>(pRecord)->recLen_gg00();
        withConsistentReadInfo = false;
    }

    return true;
}
//-------------------------------------------------------------------------------------
/// False is returned if the key for the given index does not exist.
inline bool
cbd610_RecordList::GetKey (SAPDB_Int4   RecIndex,
                           SAPDB_Int2  &Length,
                           SAPDB_Byte* &pKey)
{
    if ( RecIndex < 0 || RecIndex > MaxRecordIndex() )
        return false;

    RecordInfo *pInfo = GetRecordInfo(RecIndex);

    if ( Version() > 0 )
    {
        pKey    = reinterpret_cast<SAPDB_Byte*>(m_pNode) + pInfo->posLength.position - POS_OFF_DIFF_BD00;
        Length  = pInfo->GetLength();
    }
    else
    {
        tgg00_RecPtr pRecord;
    
        pRecord = reinterpret_cast<tgg00_RecPtr>(reinterpret_cast<SAPDB_Byte*>(m_pNode) + pInfo->position - POS_OFF_DIFF_BD00);
        Length  = reinterpret_cast<tgg00_RecPtr>(pRecord)->recKeyLen_gg00() & 0x7FFF;
        pKey    = reinterpret_cast<SAPDB_Byte*>(pRecord) + cgg_rec_key_offset;
    }

    return true;
}                        
//-------------------------------------------------------------------------------------
inline bool
cbd610_RecordList::GetConstKey (SAPDB_Int4         RecIndex,
                                SAPDB_Int2        &Length,
                                SAPDB_Byte* &pKey) const
{
    if ( RecIndex < 0 || RecIndex > MaxRecordIndex() )
        return false;

    const RecordInfo *pInfo = GetConstRecordInfo(RecIndex);

    if ( Version() > 0 )
    {
        pKey    = reinterpret_cast<SAPDB_Byte*>(m_pNode) + pInfo->posLength.position - POS_OFF_DIFF_BD00;
        Length  = pInfo->GetLength();
    }
    else
    {
        tgg00_RecPtr pRecord;
    
        pRecord = reinterpret_cast<tgg00_RecPtr>(reinterpret_cast<SAPDB_Byte*>(m_pNode) + pInfo->position - POS_OFF_DIFF_BD00);
        Length  = reinterpret_cast<tgg00_RecPtr>(pRecord)->recKeyLen_gg00() & 0x7FFF;
        pKey    = reinterpret_cast<SAPDB_Byte*>(pRecord) + cgg_rec_key_offset;
    }

    return true;
}                        
//-------------------------------------------------------------------------------------
inline void
cbd610_RecordList::SetPosition (SAPDB_Int4 RecIndex,
                                SAPDB_Int4 Position,
                                SAPDB_Int4 Length,
                                bool       withConsistentReadInfo)
{
    RecordInfo *pInfo = GetRecordInfo(RecIndex);
    if ( Version() > 0 && Length > 0)
        pInfo->SetValues(Position, Length, withConsistentReadInfo); 
    else
        pInfo->position = Position;
}
//-------------------------------------------------------------------------------------
inline void
cbd610_RecordList::SetDeleted(SAPDB_Int4 RecIndex,
                                   bool       isDeleted)
{
    if ( Version() > 0 )
        GetRecordInfo(RecIndex)->SetDeleted(isDeleted); 
}
//  -------------------------------------------------------------------------------------
inline bool
cbd610_RecordList::IsDeleted(SAPDB_Int4 RecIndex) const
{
    return Version() > 0
           ? GetConstRecordInfo(RecIndex)->IsDeleted()
           : false;
}
//-------------------------------------------------------------------------------------
inline SAPDB_Int4
cbd610_RecordList::RecordCount() const
{
    return m_pNode->nd_record_cnt();
}
//-------------------------------------------------------------------------------------
inline void
cbd610_RecordList::UpdatePositions (SAPDB_Int4 TargetPos,
                                    SAPDB_Int4 ShiftLen)
{
    for (SAPDB_Int4 RecIndex = FIRST_REC_INDEX_BD00;
                    RecIndex <= MaxRecordIndex();
                    ++ RecIndex )
    {
        if ( GetPosition(RecIndex) >= TargetPos)
        {
            if ( Version() > 0 )
                GetRecordInfo(RecIndex)->posLength.position += ShiftLen;
            else
                GetRecordInfo(RecIndex)->position += ShiftLen;
        }
    }
}
//-------------------------------------------------------------------------------------
inline void
cbd610_RecordList::UpdatePositionsRange (SAPDB_Int4 FirstIndex,
                                         SAPDB_Int4 LastIndex,
                                         SAPDB_Int4 MinRecOffset,
                                         SAPDB_Int4 MaxRecOffset,
                                         SAPDB_Int4 ShiftLen)
{
    const bool bIgnoreOffsets = DUMMY_REC_OFFSET_GBD610 == MinRecOffset
                                &&
                                DUMMY_REC_OFFSET_GBD610 == MaxRecOffset;
    SAPDB_Int4 CurrRecOffset;
	
    for ( SAPDB_Int4 CurrIndex = FirstIndex;
          CurrIndex <= LastIndex;
          ++CurrIndex )
    {
        CurrRecOffset = GetOffset(CurrIndex);
		
        if ( bIgnoreOffsets
             ||
             (CurrRecOffset >= MinRecOffset && CurrRecOffset < MaxRecOffset) ) 
        {
            if ( Version() > 0 )
                GetRecordInfo(CurrIndex)->posLength.position += ShiftLen;
            else
                GetRecordInfo(CurrIndex)->position += ShiftLen;
        }
    }
}
//-------------------------------------------------------------------------------------
inline void 
cbd610_RecordList::SortForDistribution (SAPDB_Int4 LastDistribIndex)
{
    const SAPDB_Int4 NumDistribIndexes = MaxRecordIndex() - LastDistribIndex + 1;

    if (NumDistribIndexes > 1)
        Sort (Version(), NumDistribIndexes, GetRecordInfo(MaxRecordIndex()));

    if (LastDistribIndex > 1)
        Sort (Version(), LastDistribIndex, GetRecordInfo(LastDistribIndex-1));
}
//-------------------------------------------------------------------------------------
#endif