/*!**************************************************************************

  module      : Log_ActionMultiple.cpp
  special area: Logging
  responsible : UweH
  last changed: 2004-07-29  12:00
  copyright:    Copyright by SAP AG, 2000
  description : implements class Log_ActionMultiple



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

#include "Logging/Log_ActionMultiple.hpp"
#include "Logging/Log_AfterImage.hpp"
#include "Logging/Log_Transaction.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

#define ALIGNMENT 4
/* --------------------------------------------------------------------------*/
inline SAPDB_UInt Align(SAPDB_UInt value)
{
    return ((value-1) & ~(ALIGNMENT-1)) + ALIGNMENT;
}

/* --------------------------------------------------------------------------*/
bool Log_ActionMultiple::ReadPersistentFormat (Data_SplitSpaceReader  &Reader,
                                               SAPDBMem_IRawAllocator &Allocator,
                                               SAPDBErr_MessageList   &errlist)
{
    errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"DO NOT USE THIS FUNCTION");
    return false;
}

/* --------------------------------------------------------------------------*/
bool Log_ActionMultiple::WritePersistentFormat (Data_SplitSpaceWriter &Writer,
                                                SAPDBErr_MessageList  &errlist) const
{
    if ( m_Head.totalLengthOfActions == 0 )
    {
        WriteToTrace ("m_Head.totalLengthOfActions != 0");
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_Head.totalLengthOfActions != 0");
        return false;
    }
    if ( m_Head.actionCount == 0 )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"m_Head.actionCount != 0");
        return false;
    }

    Head *pHead;

    if ( Writer.Reserve (sizeof(Head), reinterpret_cast<SAPDB_Byte*&>(pHead))
         !=
         Data_SplitSpaceWriter::moreSpaceAvailable )
    {
        WriteToTrace ("Writer.Reserve()");
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Writer.Reserve()");
        return false;
    }

    *pHead = m_Head;

    if ( Writer.Write (m_Buffer.GetBufferPointer(), m_Head.totalLengthOfActions)
         !=
         Data_SplitSpaceWriter::ok )
    {
        WriteToTrace ("Writer.Write()");
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Writer.Write()");
        return false;
    }
    return true;
}

/* --------------------------------------------------------------------------*/
void Log_ActionMultiple::WriteToTrace (const char * Title) const
{
    Kernel_VTrace trace;
    
    if ( Title != NULL )
        trace << Title << " ";
	trace << "# Actions: " << m_Head.actionCount
          << ", totalLength: "  << m_Head.totalLengthOfActions
          << FlushLine;
    
    if ( IsEmpty() )
        return;
        
    tgg91_TransNo     dummyTransno;
    SAPDB_UInt        actionno = 1;
    Log_AfterImage    image;
    SAPDB_UInt        actionLength;
    const SAPDB_Byte *pActionSpace;
    Iterator          actioniter  = GetFirstAction();

    while ( actioniter.GetAction(image, dummyTransno, actionLength, pActionSpace) )
    {
        trace << "#" << actionno
              << " length: " << actionLength << FlushLine;
        image.WriteToTrace();
        actioniter.Next();
        actionno++;
    }
}

/* -------------------------------------------------------------------------------- */
Log_ActionMultiple::AppendResult
Log_ActionMultiple::Append (Log_EntrySequence     redoSequence,
                            Log_EntrySequence     undoSequence,
                            const Log_IAction    &action,
                            SAPDBErr_MessageList &errlist)
{
    if ( ! m_Buffer.IsAssigned() )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Local Redo Log Buffer is, so the buffer must be allocated");
        return appendError;
    }
	
    const SAPDB_UInt reservedSize = sizeof(ActionHeader) + action.GetPersistentLength();

    if ( reservedSize != Align(reservedSize) )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"The logentry which should be appended is aligned.");
        return appendError;
    }
    
    if ( action.GetType() == Log_MultipleActions )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"A multiple action must not be stored in the local redo log buffer.");
        return appendError;
    }
    
    if ( ! m_Buffer.Reserve (reservedSize) )
        return appendBufferIsFull; // buffer overflow. the buffer has to be flushed first before appending new entries

    Data_SplitSpaceWriter  writer (m_Buffer);
    SAPDB_Byte            *pAuxHeader;

    if ( writer.Reserve (sizeof(ActionHeader), pAuxHeader)
         !=
         Data_SplitSpaceWriter::moreSpaceAvailable )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Writing the header of a logentry to the local redo log buffer.");
        return appendError;
    }

    ActionHeader &header = *(reinterpret_cast<ActionHeader*>(pAuxHeader));

    header = ActionHeader(sizeof(ActionHeader) + action.GetPersistentLength(),
                          action.GetType(),
                          action.GetPersistentVersion(),
                          redoSequence,
                          undoSequence);

    m_Head.totalLengthOfActions += reservedSize;
    m_Head.actionCount++;
    
    if ( ! action.WritePersistentFormat (writer, errlist) )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Writing the action of a logentry to the local redo log buffer.");
        return appendError;
    }
	#ifdef SAPDB_SLOW
    WriteToTrace ("Log_ActionMultiple::Append() end");
    #endif
    return appendOk;
}

/* -------------------------------------------------------------------------------- */
void Log_ActionMultiple::ActionHeader::Crash()
{
    RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"log action type must be valid"));
}

/* -------------------------------------------------------------------------------- */
void Data_SplitSpaceBuffer::Delete()
{
    if ( 0 == m_Buffer )
        return;
    m_pAllocator->Deallocate(m_Buffer);
    m_pAllocator      = 0;
    m_Buffer          = 0;
    m_BufferSize      = 0;
    m_FirstFreeOffset = 0;
    Release();
}
/* -------------------------------------------------------------------------------- */
bool Log_ActionMultiple::Iterator::GetAction( Log_AfterImage      &image,
                                              const tgg91_TransNo &currentTransNo,
                                              SAPDB_UInt          &actionLength,
                                              const SAPDB_Byte*   &pActionSpace) const
{
    if ( ! IsValid() )
        return false;

    const SAPDB_Byte *pActionHeader = m_Buffer.GetBufferPointer()
                                      +
                                      m_CurrentOffset;
    const ActionHeader   &actionhead = *(reinterpret_cast<const ActionHeader *>(pActionHeader));
    const Log_ActionType  actionType = actionhead.actionType < Log_ActionType_DO_NOT_USE
                                       ? Log_ActionType(actionhead.actionType)
                                       : Log_IllegalType;
    
    actionLength = actionhead.length - sizeof(ActionHeader);
    pActionSpace = pActionHeader + sizeof(ActionHeader);
    
    
    image.PrepareForWrite( actionType,
                           actionhead.actionVersion,
                           currentTransNo,
                           actionhead.redosequence,
                           actionhead.undosequence );

    if ( image.GetActionType() == Log_MultipleActions )
        RTE_Crash( Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"RECURSIVE CALL") );

    return true;
}

/* -------------------------------------------------------------------------------- */
bool Log_ActionMultiple::Iterator::Next()
{
    if ( ! IsValid() )
        return false;

    const SAPDB_Byte *pActionHeader = m_Buffer.GetBufferPointer()
                                      +
                                      m_CurrentOffset;
    const ActionHeader &actionhead = *(reinterpret_cast<const ActionHeader *>(pActionHeader));
    
    m_CurrentOffset += actionhead.length;
    
    return IsValid();
}
