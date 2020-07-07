/*!

  @file        vbd610.cpp
  @author      UweH
  @ingroup     Index
  @brief       This is the implementation of cbd610_RecordList
*/
/*!
\if EMIT_LICENCE
  copyright:    Copyright (c) 2001-2005 SAP AG


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

#include "gbd610.h"
#include "gsp00_2.h"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Index.h"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "DataAccess/Data_Types.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"

bool
cbd610_RecordList::Merge (const cbd610_RecordList &Source,
						        SAPDB_Int4         Offset,
                                Msg_List          &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("cbd610_RecordList::Merge", DataIndex_Trace, 5);

          SAPDB_Int4 &TargetRecCount = m_pNode->nd_record_cnt();
    const SAPDB_Int4  SourceRecCount = Source.RecordCount();
	const SAPDB_Int4  ShiftLen       = SourceRecCount << DIVISOR_POINTERSIZE_BD00;
	const SAPDB_Int4  OldMaxIndex    = MaxRecordIndex();
	
	/* move all PointerListItems from the given node to the current node */
    if ( 0 == SAPDB_MemCopy( GetRecordInfo(TargetRecCount-1+SourceRecCount),
                             Source.GetConstRecordInfo(SourceRecCount-1),
                             ShiftLen,
                             "VBD610", __CONTEXT__, errlist) )
    {
        return false;
    }
	
    TargetRecCount += SourceRecCount;
	
	UpdatePositionsRange (OldMaxIndex + 1, TargetRecCount - 1,
		                  DUMMY_REC_OFFSET_GBD610, DUMMY_REC_OFFSET_GBD610, Offset);
    
    return true;
}

/*---------------------------------------------------------------------------*/

bool 
cbd610_RecordList::AddPosition (SAPDB_Int4  RecIndex,
                                SAPDB_Int4  RecPos, 
                                SAPDB_Int4  RecLen,
                                bool        withConsistentReadInfo,
                                Msg_List   &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("cbd610_RecordList::AddPosition", DataIndex_Trace, 5);

    if (0 != m_pNode->nd_record_cnt()) // something to move = list is not empty
    {
		if (RecIndex <= MaxRecordIndex())
		{
            if ( 0 == SAPDB_MemMove( GetRecordInfo(MaxRecordIndex()+1),
                                     GetRecordInfo(MaxRecordIndex()),
                                    (MaxRecordIndex() - RecIndex + 1) << DIVISOR_POINTERSIZE_BD00,
                                    "VBD610", __CONTEXT__, errlist) )
            {
                errlist.AddDetail( SDBMSG_INDEX_RECINDEX_OUT_OF_RANGE,
                                   Msg_Arg(SDBMSGTAG_INDEX_RECINDEX_OUT_OF_RANGE__RECINDEX,   SAPDB_ToString(RecIndex)),
                                   Msg_Arg(SDBMSGTAG_INDEX_RECINDEX_OUT_OF_RANGE__MAXRECINDEX,SAPDB_ToString(MaxRecordIndex())));
                return false;
            }
		}
		else
            if (RecIndex > MaxRecordIndex() + 1)
            {
                errlist.Assign( Msg_List(Msg_List::ErrorMsg, SDBMSG_INDEX_RECINDEX_OUT_OF_RANGE,
                                Msg_Arg(SDBMSGTAG_INDEX_RECINDEX_OUT_OF_RANGE__RECINDEX,   SAPDB_ToString(RecIndex)),
                                Msg_Arg(SDBMSGTAG_INDEX_RECINDEX_OUT_OF_RANGE__MAXRECINDEX,SAPDB_ToString(MaxRecordIndex()))));
                return false;
            }
    }

    SetPosition(RecIndex, RecPos, RecLen, withConsistentReadInfo);

    m_pNode->nd_record_cnt()++;

    #ifdef SAPDB_SLOW
    if ( DataIndex_Trace.TracesLevel(6) ) 
    {
        Kernel_VTrace trace;
        trace << "Added: RecIndex: " << RecIndex
              << ", RecPos: " << RecPos
              << ", RecLen: " << RecLen
              << ", withConsistentReadInfo: " <<  withConsistentReadInfo << FlushLine;
    }
    #endif

    return true;
}

/*---------------------------------------------------------------------------*/

bool
cbd610_RecordList::DelPosition (SAPDB_Int4  RecIndex,
                                Msg_List   &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("cbd610_RecordList::DelPosition", DataIndex_Trace, 5);

   /* This function deletes the pointerlist entry */
   /* with index RecIndex from the pointerlist    */

    if ( 0 == m_pNode->nd_record_cnt() )
	{
		// No Record in page to delete
        errlist.Assign( Msg_List(Msg_List::ErrorMsg, SDBMSG_INDEX_RECINDEX_OUT_OF_RANGE,
                        Msg_Arg(SDBMSGTAG_INDEX_RECINDEX_OUT_OF_RANGE__RECINDEX,   SAPDB_ToString(RecIndex)),
                        Msg_Arg(SDBMSGTAG_INDEX_RECINDEX_OUT_OF_RANGE__MAXRECINDEX,SAPDB_ToString(MaxRecordIndex()))));
        return false;
	}
    
    if ( 0 == SAPDB_MemMove( GetRecordInfo(MaxRecordIndex()-1),
                             GetRecordInfo(MaxRecordIndex()),
                            (MaxRecordIndex() - RecIndex) << DIVISOR_POINTERSIZE_BD00,
                            "VBD610", __CONTEXT__, errlist) )
    {
        errlist.AddDetail( SDBMSG_INDEX_RECINDEX_OUT_OF_RANGE,
                           Msg_Arg(SDBMSGTAG_INDEX_RECINDEX_OUT_OF_RANGE__RECINDEX,   SAPDB_ToString(RecIndex)),
                           Msg_Arg(SDBMSGTAG_INDEX_RECINDEX_OUT_OF_RANGE__MAXRECINDEX,SAPDB_ToString(MaxRecordIndex())));
        #ifdef SAPDB_SLOW            
        if ( DataIndex_Trace.TracesLevel(6) )
            Print();
        #endif
        return false;
    }

    m_pNode->nd_record_cnt() --;

    #ifdef SAPDB_SLOW
    if ( DataIndex_Trace.TracesLevel(6) ) 
        Kernel_VTrace() << "Deleted RecIndex: " << RecIndex;
    #endif

    return true;
}
    
/*---------------------------------------------------------------------------*/

bool 
cbd610_RecordList::Copy (const cbd610_RecordList &source,
                               Msg_List          &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("cbd610_RecordList::Copy", DataIndex_Trace, 5);
    
    m_pNode->nd_pt2()             = source.m_pNode->nd_pt2();
    m_pNode->nd_record_cnt()      = source.m_pNode->nd_record_cnt();
    m_pNode->nd_bottom  ()        = source.m_pNode->nd_bottom  ();  // only for check
    m_pNode->ndPageVersion_bd00() = source.m_pNode->ndPageVersion_bd00();
    m_pNode->ndLeafCount_bd00()   = source.m_pNode->ndLeafCount_bd00();


    if ( 0 == SAPDB_MemCopy( GetRecordInfo(MaxRecordIndex()),
                             source.GetConstRecordInfo(MaxRecordIndex()),
                             RecordCount() << DIVISOR_POINTERSIZE_BD00,
                             "cbd610_RecordList::Copy", __CONTEXT__, errlist) )
    {
        return false;
    }
    return true;
}

/*---------------------------------------------------------------------------*/

void 
cbd610_RecordList::Print() const
{
    Kernel_VTrace trace;
    
    trace << "Version: " << Version() << FlushLine;
    
    for (SAPDB_Int4 CurrRecIndex = FIRST_REC_INDEX_BD00;
         CurrRecIndex < RecordCount();
         CurrRecIndex++)
    {
        trace << "RecIndex["     << CurrRecIndex
              << "]: Position: " << GetPosition(CurrRecIndex)
              << ", Length: "    << GetLength(CurrRecIndex) << FlushLine;
        Kernel_TraceBuffer ( (SAPDB_Byte*)m_pNode + GetPosition(CurrRecIndex) - 1,
                             GetLength(CurrRecIndex), "", sizeof(*m_pNode));
    }
}

/*---------------------------------------------------------------------------*/

bool  
cbd610_RecordList::DelHighIndexPositions (SAPDB_Int4 NumDeletedRecords,
                                          Msg_List   &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("cbd610_RecordList::DelHighIndexPositions", DataIndex_Trace, 5);

    if ( RecordCount() == 0 )
        return false;
        
    if ( 0 == SAPDB_MemMove( GetRecordInfo(MaxRecordIndex() - NumDeletedRecords),
                             GetConstRecordInfo(MaxRecordIndex()),
                             (RecordCount() - NumDeletedRecords) << DIVISOR_POINTERSIZE_BD00,
                             "VBD610", __CONTEXT__, errlist) )
    {
        return false;
    }

    m_pNode->nd_record_cnt() -= NumDeletedRecords;

    #ifdef SAPDB_SLOW
    if ( DataIndex_Trace.TracesLevel(6) ) 
        Kernel_VTrace() << "NumDeletedRecords: " << NumDeletedRecords;
    #endif
    return true;
}

/*---------------------------------------------------------------------------*/

struct QuickSortElem
{
    SAPDB_Int4  qseLeft;
    SAPDB_Int4  qseRight;
};

/*---------------------------------------------------------------------------*/
void
cbd610_RecordList::SortVersion0 (SAPDB_Int4   NumRecPosElem,
                                 RecordInfo  *pMinRecPosElem)
{
    
    SAPDBTRACE_METHOD_DEBUG ("cbd610_RecordList::SortVersion0", DataIndex_Trace, 5);

    const tsp00_Int4 MaxQuickSortStack = 16;
    tsp00_Int4       i;
    tsp00_Int4       j;
    tsp00_Int4       Left;
    tsp00_Int4       Right;
    tsp00_Int4       PivotIndex;
    tsp00_Int4       StackIndex = 0;
    RecordInfo       AuxElem;
    RecordInfo       PivotElem;
    ARRAY_VAR_MSP00 (Stack, QuickSortElem, MaxQuickSortStack);
    
    
    Stack [StackIndex].qseLeft  = 0;
    Stack [StackIndex].qseRight = NumRecPosElem - 1;
    do
    {
        Left  = Stack [StackIndex].qseLeft;
        Right = Stack [StackIndex].qseRight;
        StackIndex --;
        do
        {
            i          = Left;
            j          = Right;
            PivotIndex = (Left + Right) >> 1;
            PivotElem.position = (pMinRecPosElem + PivotIndex)->position;
            do
            {
                while ( (pMinRecPosElem + i)->position  < PivotElem.position ) i++;
                
                while (PivotElem.position < (pMinRecPosElem + j)->position) j--;
                
                if (i <= j)
                {
                    AuxElem.position = (pMinRecPosElem + i)->position; 
                    (pMinRecPosElem + i)->position = (pMinRecPosElem + j)->position; 
                    (pMinRecPosElem + j)->position = AuxElem.position;
                    i++;
                    j--;
                }
            }
            while (i <= j);
            
            if ((j - Left) < (Right - i))
            {
                if (i < Right)
                {
                    StackIndex++;
                    Stack [StackIndex].qseLeft  = i;
                    Stack [StackIndex].qseRight = Right;
                }
                Right = j;
            }
            else
            {
                if (Left < j)
                {
                    StackIndex++;
                    Stack [StackIndex].qseLeft  = Left;
                    Stack [StackIndex].qseRight = j;
                }
                Left = i;
            }
        }
        while (Left < Right);
    }
    while (StackIndex >= 0);
}

/*---------------------------------------------------------------------------*/
void 
cbd610_RecordList::SortVersion1 (SAPDB_Int4   NumRecPosElem,
                                 RecordInfo  *pMinRecPosElem)
{
    
    SAPDBTRACE_METHOD_DEBUG ("cbd610_RecordList::SortVersion1", DataIndex_Trace, 5);

    const tsp00_Int4 MaxQuickSortStack = 16;
    tsp00_Int4       i;
    tsp00_Int4       j;
    tsp00_Int4       Left;
    tsp00_Int4       Right;
    tsp00_Int4       PivotIndex;
    tsp00_Int4       StackIndex = 0;
    RecordInfo       AuxElem;
    RecordInfo       PivotElem;
    ARRAY_VAR_MSP00 (Stack, QuickSortElem, MaxQuickSortStack);
    
    
    Stack [StackIndex].qseLeft  = 0;
    Stack [StackIndex].qseRight = NumRecPosElem - 1;
    do
    {
        Left  = Stack [StackIndex].qseLeft;
        Right = Stack [StackIndex].qseRight;
        StackIndex --;
        do
        {
            i          = Left;
            j          = Right;
            PivotIndex = (Left + Right) >> 1;
            PivotElem.posLength.position = (pMinRecPosElem + PivotIndex)->posLength.position;
            PivotElem.posLength.length   = (pMinRecPosElem + PivotIndex)->posLength.length;
            do
            {
                while ( (pMinRecPosElem + i)->posLength.position  < PivotElem.posLength.position ) i++;
                
                while (PivotElem.posLength.position < (pMinRecPosElem + j)->posLength.position) j--;
                
                if (i <= j)
                {
                    AuxElem.posLength.position = (pMinRecPosElem + i)->posLength.position; 
                    AuxElem.posLength.length   = (pMinRecPosElem + i)->posLength.length; 

                    (pMinRecPosElem + i)->posLength.position = (pMinRecPosElem + j)->posLength.position; 
                    (pMinRecPosElem + i)->posLength.length   = (pMinRecPosElem + j)->posLength.length; 

                    (pMinRecPosElem + j)->posLength.position = AuxElem.posLength.position;
                    (pMinRecPosElem + j)->posLength.length   = AuxElem.posLength.length;
                    
                    i++;
                    j--;
                }
            }
            while (i <= j);
            
            if ((j - Left) < (Right - i))
            {
                if (i < Right)
                {
                    StackIndex++;
                    Stack [StackIndex].qseLeft  = i;
                    Stack [StackIndex].qseRight = Right;
                }
                Right = j;
            }
            else
            {
                if (Left < j)
                {
                    StackIndex++;
                    Stack [StackIndex].qseLeft  = Left;
                    Stack [StackIndex].qseRight = j;
                }
                Left = i;
            }
        }
        while (Left < Right);
    }
    while (StackIndex >= 0);
}

/*---------------------------------------------------------------------------*/
void 
cbd610_RecordList::Sort (SAPDB_Int4   Version,
                         SAPDB_Int4   NumRecPosElem,
                         RecordInfo  *pMinRecPosElem)
{
    
    SAPDBTRACE_METHOD_DEBUG ("cbd610_RecordList::Sort", DataIndex_Trace, 5);
    if ( Version > 0 )
        SortVersion1(NumRecPosElem, pMinRecPosElem);
    else
        SortVersion0(NumRecPosElem, pMinRecPosElem);
}
