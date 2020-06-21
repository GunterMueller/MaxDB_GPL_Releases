/*!**************************************************************************

    module      : Log_ReadWriteActionImage.cpp

    -------------------------------------------------------------------------

    responsible : UweH

    special area: Logging
    description : defines a class to read/write before and after images of any action

    last changed: 2001-03-28

    -------------------------------------------------------------------------

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




*****************************************************************************/
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_BeforeImage.hpp"
#include "Logging/Log_Transaction.hpp"
#include "SQLManager/SQLMan_Context.hpp"

#include "hkb560.h"
#include "ggg00.h"

//---------------------------------------------------------------------------

bool Log_ActionImageReader::PersistentRead (SAPDB_Byte             *&pImage,
                                            SAPDB_UInt4              ImageLen,
                                            SAPDBMem_IRawAllocator *&pUsedAllocator,
                                            SAPDBErr_MessageList    &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionImageReader::PersistentRead", LogAction_Trace, 5);
    
    bool bAllocatorUsed = false;
    
    const Data_SplitSpaceReader::Result ReadResult =
        m_SplitSpaceReader.Read ( Log_AlignedImageLen (ImageLen),
                                  pImage,
                                  m_IRawAllocator,
                                  bAllocatorUsed );
    
    pUsedAllocator = bAllocatorUsed ? &m_IRawAllocator : 0;
    
    if ( ReadResult != Data_SplitSpaceReader::ok
         &&
         ReadResult != Data_SplitSpaceReader::moreSpaceAvailable )
    {
        m_SplitSpaceReader.AppendContextInfo(__CONTEXT__,errlist, "Log_ActionImageReader::m_SplitSpaceReader");
        return false;
    }
    SAPDBTRACE_IF (LogAction_Trace, 6, Kernel_TraceBuffer (pImage, ImageLen); )
    return true;    
}

//---------------------------------------------------------------------------

bool Log_ActionImageReader::CopyImage (SAPDB_Byte           *pImage,
                                       SAPDB_UInt4           ImageLen,
                                       SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionImageReader::CopyImage", LogAction_Trace, 5);

    SAPDB_Byte             *pImageForRead;
    SAPDBMem_IRawAllocator *pImageAllocator = NULL;
    
    if(!PersistentRead (pImageForRead, ImageLen, pImageAllocator, errlist))
        return false;

    SAPDB_MemCopyNoCheck (pImage, pImageForRead, ImageLen);
    
    if ( pImageAllocator != 0 )
        pImageAllocator->Deallocate (pImageForRead);
	return true;
}

//---------------------------------------------------------------------------

bool Log_ActionImageWriter::AlignImage (SAPDB_UInt4           WrittenLength,
                                        SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionImageWriter::AlignImage", LogAction_Trace, 5);
    
    SAPDB_UInt4 AlignedLen = Log_AlignedImageLen (WrittenLength);
    
    if ( AlignedLen <= WrittenLength )
        return true;

    SAPDB_Byte PaddingBytes[7];
    
    SAPDB_MemFillNoCheck (PaddingBytes, 0, 7);
    
    m_WriteResult = m_SplitSpaceWriter.Write (PaddingBytes, AlignedLen - WrittenLength);
    
    if ( IsResultOk() )
        return true;

    m_SplitSpaceWriter.AppendContextInfo(__CONTEXT__,errlist,"Log_ActionImageWriter::m_SplitSpaceWriter");
	return false;
}

//----------------------------------------------------------------------------

bool Log_ActionImageWriter::UnalignedPersistentWrite (const void           *pSource,
                                                      SAPDB_UInt4           SourceLen,
                                                      SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionImageWriter::UnalignedPersistentWrite", LogAction_Trace, 5);
    
    SAPDBTRACE_IF   (LogAction_Trace, 6, Kernel_TraceBuffer (pSource, SourceLen));
    
    m_WriteResult = m_SplitSpaceWriter.Write (REINTERPRET_CAST (const SAPDB_Byte*, pSource), SourceLen);
    
    if ( IsResultOk() )
        return true;

    m_SplitSpaceWriter.AppendContextInfo(__CONTEXT__,errlist,"Log_ActionImageWriter::m_SplitSpaceWriter");
	return false;
}


//----------------------------------------------------------------------------

bool Log_ReadWriteActionImage::ReadPersistentFormat (Data_SplitSpaceReader  &Reader,
                                                     SAPDBMem_IRawAllocator &Allocator,
                                                     SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ReadWriteActionImage::ReadPersistentFormat", LogAction_Trace, 5);
    Log_ActionImageReader ActionImageWriter (Reader, Allocator);
    return ReadImagePersistent (ActionImageWriter, errlist);
}

//----------------------------------------------------------------------------

void Log_ReadWriteActionImage::WriteAfterImage (tgg00_TransContext &trans) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ReadWriteActionImage::WriteAfterImage", LogAction_Trace, 5);

    SAPDBErr_MessageList errlist;

    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError(trans.trError_gg00, errlist, "Log_ReadWriteActionImage::WriteAfterImage entered with error");
    else
    {
        Log_Transaction &logtrans = *(reinterpret_cast<Log_Transaction*>(trans.trLogContext_gg00));
        trans.trError_gg00 = kb560ConvertWriteRedoEntryResultToBasisError
        (
            logtrans.WriteRedoEntry (*this, errlist)
        );
    }

    if( trans.trError_gg00 != e_ok )
    {
        if ( ! SQLMan_Context::GetContext() )
            RTE_Message(errlist);
        else
        {
            if ( ! SQLMan_Context::GetContext()->GetErrorList().IsEmpty() )
                RTE_Message(SQLMan_Context::GetContext()->GetErrorList());
            SQLMan_Context::GetContext()->GetErrorList() = errlist;
        }
    }
}

//----------------------------------------------------------------------------

void Log_ReadWriteActionImage::WriteBeforeImage (tgg00_TransContext &trans) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ReadWriteActionImage::WriteBeforeImage", LogAction_Trace, 5);

    SAPDBErr_MessageList errlist;

    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError(trans.trError_gg00, errlist, "Log_ReadWriteActionImage::WriteAfterImage entered with error");
    else
    {
        Data_PageNo      unusedPageNo;
        Data_PageOffset  unusedOffset;
        Log_BeforeImage  beforeImage;
        Log_Transaction &logtrans = *(reinterpret_cast<Log_Transaction*>(trans.trLogContext_gg00));
        switch ( logtrans.WriteUndoEntry (beforeImage, *this, unusedPageNo, unusedOffset, errlist) )
        {
            case Log_UndoFile::writeBeforeImageOk:
                break;
            case Log_UndoFile::writeBeforeImageError:
                trans.trError_gg00 = e_undo_log_write_error;
                break;
            case Log_UndoFile::writeBeforeImageNoSpaceAvailable:
                trans.trError_gg00 = e_work_rolled_back;
                break;
            default:
                trans.trError_gg00 = e_undo_log_write_error;
                break;
        }
    }

    if( trans.trError_gg00 != e_ok )
    {
        if ( ! SQLMan_Context::GetContext() )
            RTE_Message(errlist);
        else
        {
            if ( ! SQLMan_Context::GetContext()->GetErrorList().IsEmpty() )
                RTE_Message(SQLMan_Context::GetContext()->GetErrorList());
            SQLMan_Context::GetContext()->GetErrorList() = errlist;
        }
    }
}

//----------------------------------------------------------------------------

void Log_ReadWriteActionImage::WriteBeforeImage (tgg00_TransContext &trans,
                                                 tgg00_ObjTransInfo &TransInfo) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ReadWriteActionImage::WriteBeforeImage", LogAction_Trace, 5);

    SAPDBErr_MessageList errlist;

    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError(trans.trError_gg00, errlist, "Log_ReadWriteActionImage::WriteAfterImage entered with error");
    else
    {
        Data_PageNo      pageNo;
        Data_PageOffset  offset;
        Log_BeforeImage  beforeImage;
        beforeImage.SetTransInfoAndObjState ( TransInfo );
        Log_Transaction &logtrans = *(reinterpret_cast<Log_Transaction*>(trans.trLogContext_gg00));
        switch ( logtrans.WriteUndoEntry (beforeImage, *this, pageNo, offset, errlist) )
        {
            case Log_UndoFile::writeBeforeImageOk:
                break;
            case Log_UndoFile::writeBeforeImageError:
                trans.trError_gg00 = e_undo_log_write_error;
                break;
            case Log_UndoFile::writeBeforeImageNoSpaceAvailable:
                trans.trError_gg00 = e_work_rolled_back;
                break;
            default:
                trans.trError_gg00 = e_undo_log_write_error;
                break;
        }
        TransInfo.otrBeforeRef_gg00.gg91BuildRef (pageNo, offset);
    }
    if( trans.trError_gg00 != e_ok )
    {
        if ( ! SQLMan_Context::GetContext() )
            RTE_Message(errlist);
        else
        {
            if ( ! SQLMan_Context::GetContext()->GetErrorList().IsEmpty() )
                RTE_Message(SQLMan_Context::GetContext()->GetErrorList());
            SQLMan_Context::GetContext()->GetErrorList() = errlist;
        }
    }
}

//----------------------------------------------------------------------------

bool Log_ReadWriteActionImage::WritePersistentFormat (Data_SplitSpaceWriter &Writer,
                                                      SAPDBErr_MessageList  &errlist) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ReadWriteActionImage::WritePersistentFormat", LogAction_Trace, 5);

    Log_ActionImageWriter ActionImageWriter (Writer);

    return WriteImagePersistent (ActionImageWriter, errlist)
           &&
           ActionImageWriter.IsWrittenCompletely();
}
