/*!
  @file           Data_PageSplitSpace.cpp
  @author         UweH
  @author         MartinB
  @brief          Implements methods of Data_PageSplitSpace and Data_PageSplitSpaceBackwardRead

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_PageSplitSpace.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "RunTime/RTE_Message.hpp"

/*===========================================================================*
 *  IMPLEMENTATION OF Data_PageSplitSpace                                    *
 *===========================================================================*/

//------------------------------------------------------------------------------

Data_PageSplitSpace::~Data_PageSplitSpace()
{
    if ( ! IsAssigned() )
        return;
    
    if ( DataPageLog_Check.ChecksLevel(5) )
    {
        SAPDBErr_MessageList errlist;
        if ( ! Verify(false,errlist) )
        {
            WriteToTrace("~Data_PageSplitSpace");
            errlist = errlist + Data_Exception(__CONTEXT__,DATA_PAGE_SPLIT_SPACE_VERIFY,
                                         SAPDB_ToString(PageNo()));
            if ( DataPageLog_Check.ChecksLevel(6) )
                RTE_Crash(errlist);
            else
                RTE_Message(errlist);
        }
    }
}
    
//------------------------------------------------------------------------------

void Data_PageSplitSpace::SpaceInfo
               (Data_SplitRecordLength  WantedSize,
                Data_RecordLength       MinimumSize,
                bool                   &FirstPartFits,
                bool                   &AllFits)
{
    Data_SplitRecordLength AvailableSpace = Data_Page::FreeSpace();

    AllFits = AlignRecordSize(WantedSize)+sizeof(Data_SplitRecordLength) <= AvailableSpace;
    if ( AllFits )
    {
        FirstPartFits = true;
        return;
    }
    FirstPartFits = MinimumSize+sizeof(Data_SplitRecordLength) <= AvailableSpace;
}

/*===========================================================================*
 *  IMPLEMENTATION OF Data_PageSplitSpaceBackwardRead                        *
 *===========================================================================*/

//------------------------------------------------------------------------------

Data_PageOffset Data_PageSplitSpaceBackwardRead::GetOffset (SAPDB_Byte* partptr,
                                                            SAPDB_UInt  partlength,
                                                            bool        bCheck) const
{
    #ifdef SAPDB_SLOW
    if ( bCheck )
    {
        if ( ! ReferenceIsValid (partptr) )
        {
            Kernel_VTrace() << "partlength=" << partlength;
            RTE_Crash(Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"ReferenceIsValid (partptr)"));
        }
        if ( partlength > FrameSize() )
        {
            Kernel_VTrace() << "partlength=" << partlength;
            RTE_Crash(Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"partlength <= FrameSize()"));
        }
    }
    #endif
    
    const Data_PageOffset offset =
        Data_PageOffset(partptr - FramePtr()) + partlength + sizeof(Data_SplitRecordLength);

    #ifdef SAPDB_SLOW
    if ( bCheck )
    {
        if ( offset >= FrameSize() )
        {
            Kernel_VTrace() << "partlength="                << partlength
                            << ", offset="                  << offset
                            << ", AlignRecordSize(offset)=" << AlignRecordSize(offset);
            RTE_Crash(Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"offset < FrameSize()"));
        }
        if ( offset != AlignRecordSize(offset) )
        {
            Kernel_VTrace() << "partlength="                << partlength
                            << ", offset="                  << offset
                            << ", AlignRecordSize(offset)=" << AlignRecordSize(offset);
            RTE_Crash(Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"offset == AlignRecordSize(offset)"));
        }
    }
    #endif
    
    return offset;
}

//------------------------------------------------------------------------------

SAPDB_Bool Data_PageSplitSpaceBackwardRead::Verify(bool                  isAdmin,
                                                   SAPDBErr_MessageList &errlist) const
{
    if ( ! Data_BasePage::Verify(isAdmin,errlist) )
        return false;

    Data_PageOffset currentOffset = FirstFreeOffset (FramePtr()) - 1;

    while ( currentOffset > MinSpaceOffset() )
    {
        if ( currentOffset != AlignRecordSize(currentOffset) )
        {
            WriteToTrace("Verify failed");
            return false;
        }
        currentOffset -=
            *(reinterpret_cast<Data_SplitRecordLength*>
                     (currentOffset - sizeof(Data_SplitRecordLength)));
    }
    return true;
}

//------------------------------------------------------------------------------

void Data_PageSplitSpaceBackwardRead::WriteToTrace (const SAPDB_Char *title) const
{
    Data_Page::WriteToTrace(title!=NULL?title:"Data_PageSplitSpaceBackwardRead");

    SAPDB_UInt             spaceCount    = 0;
    Data_PageOffset        currentOffset = FirstFreeOffset (FramePtr()) - 1;
    Data_SplitRecordLength currentLength;

    if ( currentOffset == MinSpaceOffset() )
        Kernel_VTrace() << "page is empty";
    else if ( currentOffset < MinSpaceOffset() )
        Kernel_VTrace() << "FirstFreeOffset too small: " << currentOffset;

    while ( currentOffset > MinSpaceOffset() )
    {
        if ( currentOffset != AlignRecordSize(currentOffset) )
        {
            Kernel_VTrace() << "currentOffset not properly aligned: " << currentOffset;
            break;
        }
        currentLength =
            *(reinterpret_cast<Data_SplitRecordLength*>
                     (currentOffset - sizeof(Data_SplitRecordLength)));
        Kernel_VTrace() << "space #" << spaceCount << " @" << currentOffset << " len: " << currentLength;
        currentOffset -= currentLength;
        ++spaceCount;
    }
}

//------------------------------------------------------------------------------
/*
|-----------WantedSize=15----------------| ReservedSize=0
|-----------(WantedSize+aligned+Length)=18-----------|

|-----ReservedSize=10---|12| real size is 12 and WantedSize=8
|-----ReservedSize=8--|22| real size is 10 and WantedSize=0
*/
void Data_PageSplitSpaceBackwardRead::ReserveSpace
                  (Data_SplitRecordLength &ReservedSize,
                   Data_SplitRecordLength &WantedSize,
                   Data_RecordLength       MinimumSize,
                   Data_RecordLength      &CurrentReservedSize,
                   SAPDB_Byte*            &CurrentSpace)
{
    SAPDBERR_ASSERT_STATE( IsAssigned() );
    
    // How much space is really required
    
    Data_SplitRecordLength AddedSize;
    
    if ( 0 == ReservedSize )
        AddedSize = (AlignRecordSize(WantedSize) - WantedSize) + 
                    sizeof(Data_SplitRecordLength);
    else
        AddedSize = sizeof(Data_SplitRecordLength);
    
    CurrentReservedSize = WantedSize + AddedSize;

    // How much space can really be reserved
    
    Data_SplitRecordLength AvailableSpace = Data_Page::FreeSpace();
    
    if ( CurrentReservedSize > AvailableSpace )
        CurrentReservedSize = AvailableSpace;

    // Check the minimum size at the first time
    if ( 0 == ReservedSize
         &&
         AvailableSpace < MinimumSize + sizeof(Data_SplitRecordLength) )
    {
        CurrentReservedSize = 0;
        return;
    }

    // No space available in page
    
    if ( 0 == CurrentReservedSize ) return;
    
    // space is reserved now
            
    CurrentSpace = Data_Page::ReserveSpace (CurrentReservedSize);

    // ReservedSize is assigned.
    
    ReservedSize += CurrentReservedSize;
    
    // write the length at the end of the new record space
    *(reinterpret_cast<Data_SplitRecordLength*>(CurrentSpace + CurrentReservedSize - sizeof(Data_SplitRecordLength)))
        = ReservedSize;

    // hide internal record length bytes for external usage
    
    CurrentReservedSize -= AddedSize;

    // WantedSize becomes RestSize of space which must be written
    
    WantedSize -= CurrentReservedSize;
}

/*===========================================================================*
 *  IMPLEMENTATION OF Data_PageSplitSpaceForwardRead                         *
 *===========================================================================*/

//------------------------------------------------------------------------------

Data_PageOffset Data_PageSplitSpaceForwardRead::GetOffset (SAPDB_Byte* partptr,
                                                           SAPDB_UInt  partlength,
                                                           bool        bCheck) const
{
    #ifdef SAPDB_SLOW
    if ( bCheck )
    {
        if ( ! ReferenceIsValid (partptr) )
        {
            Kernel_VTrace() << "partlength=" << partlength;
            RTE_Crash(Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"ReferenceIsValid (partptr)"));
        }
        if ( partlength > FrameSize() )
        {
            Kernel_VTrace() << "partlength=" << partlength;
            RTE_Crash(Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"partlength <= FrameSize()"));
        }
    }
    #endif
    
    const Data_PageOffset offset =
        Data_PageOffset(partptr - FramePtr()) - sizeof(Data_SplitRecordLength);

    #ifdef SAPDB_SLOW
    if ( bCheck )
    {
        if ( offset >= FrameSize() )
        {
            Kernel_VTrace() << "partlength="                << partlength
                            << ", offset="                  << offset
                            << ", AlignRecordSize(offset)=" << AlignRecordSize(offset);
            RTE_Crash(Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"offset < FrameSize()"));
        }
        if ( offset != AlignRecordSize(offset) )
        {
            Kernel_VTrace() << "partlength="                << partlength
                            << ", offset="                  << offset
                            << ", AlignRecordSize(offset)=" << AlignRecordSize(offset);
            RTE_Crash(Data_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"offset == AlignRecordSize(offset)"));
        }
    }
    #endif

    return offset;
}

//------------------------------------------------------------------------------

SAPDB_Bool Data_PageSplitSpaceForwardRead::Verify(bool                  isAdmin,
                                                  SAPDBErr_MessageList &errlist) const
{
    if ( ! Data_BasePage::Verify(isAdmin,errlist) )
        return false;

    Data_PageOffset currentOffset = MinSpaceOffset();

    while ( currentOffset < FirstFreeOffset (FramePtr()) )
    {
        if ( currentOffset != AlignRecordSize(currentOffset) )
        {
            WriteToTrace("Verify failed");
            return false;
        }
        currentOffset += *(reinterpret_cast<Data_SplitRecordLength*>(currentOffset));
    }
    return true;
}

//------------------------------------------------------------------------------

void Data_PageSplitSpaceForwardRead::WriteToTrace (const SAPDB_Char *title) const
{
    Data_Page::WriteToTrace(title!=NULL?title:"Data_PageSplitSpaceForwardRead");

    SAPDB_UInt             spaceCount    = 0;
    Data_PageOffset        currentOffset = MinSpaceOffset();
    Data_SplitRecordLength currentLength;

    if ( currentOffset == FirstFreeOffset (FramePtr()) )
        Kernel_VTrace() << "page is empty";
    else if ( currentOffset >= FirstFreeOffset (FramePtr()) )
        Kernel_VTrace() << "FirstFreeOffset too small: " << currentOffset;

    while ( currentOffset < FirstFreeOffset (FramePtr()) )
    {
        if ( currentOffset != AlignRecordSize(currentOffset) )
        {
            Kernel_VTrace() << "currentOffset not properly aligned: " << currentOffset;
            break;
        }
        currentLength = *(reinterpret_cast<Data_SplitRecordLength*>(currentOffset));
        Kernel_VTrace() << "space #" << spaceCount << " @" << currentOffset << " len: " << currentLength;
        currentOffset += currentLength;
        ++spaceCount;
    }
}

//------------------------------------------------------------------------------
/*
|-----------WantedSize=15----------------| CurrentReservedSize=0
|-----------(WantedSize+aligned+Length)=18-----------| CurrentReservedSize=18

|18|----ReservedSize=8---| CurrentReservedSize=8 and WantedSize=10
|12-----ReservedSize=10--| CurrentReservedSize=10 and WantedSize=0
*/
void Data_PageSplitSpaceForwardRead::ReserveSpace
                  (Data_SplitRecordLength &WantedSize,
                   Data_RecordLength       FirstPartMiniumSize,
                   Data_RecordLength      &CurrentReservedSize,
                   SAPDB_Byte*            &CurrentSpace)
{
    SAPDBERR_ASSERT_STATE( IsAssigned() );
    
    // How much space is really required
    
    Data_SplitRecordLength AddedSize;
    
    if ( 0 == CurrentReservedSize )
        AddedSize = (AlignRecordSize(WantedSize) - WantedSize) + 
                    sizeof(Data_SplitRecordLength);
    else
        AddedSize = sizeof(Data_SplitRecordLength);
    
    WantedSize += AddedSize;

    // How much space can really be reserved
    
    Data_SplitRecordLength AvailableSpace = Data_Page::FreeSpace();
    
    // Check the minimum size at the first time

    if ( 0 == CurrentReservedSize
         &&
         AvailableSpace < FirstPartMiniumSize + sizeof(Data_SplitRecordLength) )
    {
        AvailableSpace  = 0;
        WantedSize     -= AddedSize;
        return;
    }

    if ( WantedSize > AvailableSpace )
        CurrentReservedSize = AvailableSpace;
    else
        CurrentReservedSize = WantedSize;

    // space is reserved now
            
    CurrentSpace = Data_Page::ReserveSpace (CurrentReservedSize);

    // write the length at the beginning of the new record space

    *(REINTERPRET_CAST(Data_SplitRecordLength*,CurrentSpace)) = WantedSize;

    CurrentSpace += sizeof(Data_SplitRecordLength);

    // WantedSize becomes RestSize of space which must be written
    
    WantedSize -= CurrentReservedSize;

    // hide internal record length bytes for external usage
    
    CurrentReservedSize -= AddedSize;
}

