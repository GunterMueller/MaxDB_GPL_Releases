/*!**************************************************************************

  module      : Log_ActionObject.cpp
  special area: Logging
  responsible : UweH
  last changed: 2000-09-25  12:00
  copyright:    Copyright (c) 2000-2005 SAP AG
  description : implements class Log_ActionObject



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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_ActionObject.hpp"
#include "Logging/Log_AfterImage.hpp"
#include "Logging/Log_BeforeImage.hpp"
#include "hbd04.h"
#include "hbd95.h"

/* --------------------------------------------------------------------------*/
bool Log_ActionObject::InitNew (const tgg00_FileId    &fileid,
              const tgg92_KernelOid &oid,
              const tgg91_PageRef   &version,
              SAPDB_Int4             keySize,
              SAPDB_Byte            *pKey,
              SAPDB_Int1             contVarObject, /* PTS 1127661 FF */
              SAPDBErr_MessageList  &errlist)
{
    m_Type           = Log_NewObject;
    m_Head.fileno    = fileid.fileDirFileId_gg00();
    m_Head.oid       = oid;
    m_Head.keySize   = keySize;
    m_Head.bodySize  = 0;
    m_Head.version   = version;
    m_Head.flags     = 0;
    m_Head.filler    = 0;
    m_Head.filler2   = 0;

    m_pKey           = pKey;
    if ( contVarObject > 0 )  /* PTS 1127661 FF */
    {
        m_Head.flags |= Head::MarkedVarObject;   /* PTS 1127661 FF */
        if ( contVarObject == 2 )
            m_Head.flags |= Head::ContVarObject; /* PTS 1127661 FF */
    }
    if ( keySize < 0 )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"keySize <= 0");
        AppendContextInfo(__CONTEXT__,errlist,"Log_ActionObject::InitNew");
        return false;
    }
    return true;
}

/* --------------------------------------------------------------------------*/
void Log_ActionObject::InitDelete (
                 FileDir_FileNo        &fileno,
                 const tgg92_KernelOid &oid,
                 const tgg91_PageRef   &version,
                 bool                   reused,          /* PTS 1127661 FF */
                 SAPDB_Int1             contVarObject)   /* PTS 1127661 FF */
{
    m_Type           = Log_DeleteObject;
    m_Head.fileno    = fileno;
    m_Head.oid       = oid;
    m_Head.version   = version;
    m_Head.bodySize  = 0;
    m_Head.keySize   = 0;
    m_Head.flags     = 0;
    m_Head.filler    = 0;
    m_Head.filler2   = 0;
    if ( reused ) /* PTS 1127661 FF */
        m_Head.flags |= Head::Reused;
    if ( contVarObject > 0 )                                  /* PTS 1127661 FF */
    {
        m_Head.flags |= Head::MarkedVarObject;   /* PTS 1127661 FF */
        if ( contVarObject == 2 )
            m_Head.flags |= Head::ContVarObject; /* PTS 1127661 FF */
    }
}

/* --------------------------------------------------------------------------*/
bool Log_ActionObject::InitInsUpd (Log_ActionType         actiontype,
                 const tgg00_FileId    &fileid,
                 const tgg92_KernelOid &oid,
                 const tgg91_PageRef   &version,
                 SAPDB_Int4             keySize,
                 SAPDB_Int4             bodySize,
                 SAPDB_Byte            *pBody,
                 SAPDB_Int1             contVarObject,   /* PTS 1127661 FF */
                 bool                   reused,          /* PTS 1127661 FF */
                 SAPDBErr_MessageList  &errlist)
{
    m_Type           = actiontype;
    m_Head.fileno    = fileid.fileDirFileId_gg00();
    m_Head.oid     = oid;
    m_Head.version = version;
    m_Head.flags     = 0;
    if ( reused )                                             /* PTS 1127661 FF */
        m_Head.flags |= Head::Reused;                         /* PTS 1127661 FF */
    if ( contVarObject > 0 )                                  /* PTS 1127661 FF */
    {
        m_Head.flags |= Head::MarkedVarObject;                /* PTS 1127661 FF */
        if ( contVarObject == 2 )
            m_Head.flags |= Head::ContVarObject;              /* PTS 1127661 FF */
    }
    m_Head.bodySize  = bodySize - keySize;
    m_pBody          = pBody;
    m_Head.keySize   = 0;
    m_Head.filler    = 0;
    m_Head.filler2   = 0;
    if ( bodySize < 0 )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"bodySize >= 0");
        AppendContextInfo(__CONTEXT__,errlist,"Log_ActionObject::InitInsUpd");
        return false;
    }
    if ( keySize < 0 )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"keySize >= 0");
        AppendContextInfo(__CONTEXT__,errlist,"Log_ActionObject::InitInsUpd");
        return false;
    }
    if ( bodySize < keySize )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"bodySize >= keySize");
        AppendContextInfo(__CONTEXT__,errlist,"Log_ActionObject::InitInsUpd");
        return false;
    }
    return true;
}
    
/* --------------------------------------------------------------------------*/
bool Log_ActionObject::WritePersistentFormat
        (Data_SplitSpaceWriter &Writer,
         SAPDBErr_MessageList  &errlist) const
{
    Head                          *pHead;
    Data_SplitSpaceWriter::Result  result;
    // set the structured data

    result = Writer.Reserve (sizeof(Head), REINTERPRET_CAST(SAPDB_Byte*&, pHead));
    if ( Data_SplitSpaceWriter::moreSpaceAvailable != result
         &&
         Data_SplitSpaceWriter::ok                 != result )
	{
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                "WriteResult is not ok and not moreAvailable");
        Writer.AppendContextInfo(__CONTEXT__,errlist);
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
	}
    
    *pHead = m_Head;

    SAPDBERR_ASSERT_STATE( m_Head.keySize == 0 || m_Head.bodySize == 0 );
    
    if ( m_Head.bodySize > 0 )
    {
        if ( m_Head.bodySize > sizeof(tbd_node) )
        {
            // PTS 1117126 UH 2002-08-07 check added
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                    "m_Head.bodySize > sizeof(tbd_node)");
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        result = Writer.Write (m_pBody, m_Head.bodySize);
    }
    else
        if ( m_Head.keySize > 0 )
        {
            if ( m_Head.keySize > sizeof(tbd_node) )
            {
                errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                        "m_Head.keySize > sizeof(tbd_node)");
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
            result = Writer.Write (m_pKey, m_Head.keySize);
        }
    if ( Data_SplitSpaceWriter::moreSpaceAvailable != result
         &&
         Data_SplitSpaceWriter::ok                 != result )
	{
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                "WriteResult is not ok and not moreAvailable");
        Writer.AppendContextInfo(__CONTEXT__,errlist);
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
	}

    SAPDB_UInt alignmentLength = GetAlignmentLength();
    
    if ( alignmentLength > 0 )
    {
        SAPDB_Byte filler [5] = "\0\0\0\0";
        result = Writer.Write (filler, alignmentLength);
        if ( Data_SplitSpaceWriter::ok != result )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                    "WriteResult is not ok");
            Writer.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }
    return true;
}

/* --------------------------------------------------------------------------*/
bool Log_ActionObject::ReadPersistentFormat
        (Data_SplitSpaceReader  &Reader,
         SAPDBMem_IRawAllocator &Allocator,
         SAPDBErr_MessageList   &errlist)
{
    Head                          *pHead;
    Data_SplitSpaceReader::Result  result;

    // set the structured data

    result = Reader.Reserve (sizeof(Head), REINTERPRET_CAST(SAPDB_Byte*&, pHead));
    if ( Data_SplitSpaceReader::moreSpaceAvailable != result
         &&
         Data_SplitSpaceReader::ok                 != result )
	{
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Can not reserve header");
        Reader.AppendContextInfo(__CONTEXT__,errlist);
        return false;
	}

    m_Head = *pHead;

    if ( m_Head.bodySize > 0 )
    {
        bool bAllocatorWasUsed = false;
        result = Reader.Read (m_Head.bodySize, m_pBody,
                              Allocator, bAllocatorWasUsed);
        if ( bAllocatorWasUsed )
            m_pAllocator = &Allocator;
    }
    else if ( m_Head.keySize > 0 )
    {
        bool bAllocatorWasUsed = false;
        result = Reader.Read (m_Head.keySize, m_pKey,
                              Allocator, bAllocatorWasUsed);
        if ( bAllocatorWasUsed )
            m_pAllocator = &Allocator;
    }

    if ( Data_SplitSpaceReader::moreSpaceAvailable != result
         &&
         Data_SplitSpaceReader::ok                 != result )
    {
        errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                "ReadResult is not ok and not moreAvailable");
        Reader.AppendContextInfo(__CONTEXT__,errlist);
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    SAPDB_UInt alignmentLength = GetAlignmentLength();
    
    if ( alignmentLength > 0 )
    {
        SAPDB_Byte filler[5];
        result = Reader.Read (filler, alignmentLength);
        if ( Data_SplitSpaceReader::ok != result )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                    "ReadResult is not ok");
            Reader.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }
    return true;
}

/* --------------------------------------------------------------------------*/
void Log_ActionObject::Undo (tgg00_TransContext &TransContext, Log_IImage &Image) const
{
    if (e_ok != TransContext.trError_gg00) return;
    
    /* PTS 1127661, 1124278, 1132014 FF */
    SAPDB_Int1 contVarObj = ((Head::MarkedVarObject & m_Head.flags) ? 1 : 0);
    if (Head::ContVarObject & m_Head.flags)   
        contVarObj = 2;
    bool reused = ((Head::Reused & m_Head.flags) ? true : false );

    if ( Image.IsAfterImage() )
    {
        TransContext.trError_gg00 = e_not_implemented;
        return;
    }

    Msg_List        &errlist      = Trans_Context::GetContext(TransContext).ErrorList();
    Log_BeforeImage &BeforeImage = *(static_cast<Log_BeforeImage*> (&Image));

    switch ( BeforeImage.GetActionType() )
    {
    case Log_NewObject:
        {

            bd04RollbackObj (TransContext, m_new,
                             m_Head.oid, m_Head.fileno,
                             BeforeImage.GetPreviousState(),
                             BeforeImage.GetPreviousImage(),
                             BeforeImage.GetPreviousUpdateTransNo(),
                             0, NULL,
                             contVarObj, reused); /* PTS 1127661 FF */

            if ( e_ok != TransContext.trError_gg00 )
                WriteToTrace ( "bd04RollbackObj: ERROR" );
            break;
        }
    case Log_LockObject: // PTS 1119250 UH 2002-12-03 should do nothing
        break;
    case Log_DeleteObject:
    case Log_InsertObject:
    case Log_UpdateObject: 
         {
            tgg00_MessType LogType;
            switch (BeforeImage.GetActionType())
            {
            case Log_DeleteObject: LogType.becomes(m_delete); break;
            case Log_InsertObject: LogType.becomes(m_insert); break;
            case Log_UpdateObject: LogType.becomes(m_update); break;
            case Log_LockObject:   LogType.becomes(m_lock  ); break;
            default:
                LogType.becomes(m_nil); // error
            }

            bd04RollbackObj (TransContext, LogType,
                             m_Head.oid, m_Head.fileno,
                             BeforeImage.GetPreviousState(),
                             BeforeImage.GetPreviousImage(),
                             BeforeImage.GetPreviousUpdateTransNo(),
                             m_Head.bodySize, m_pBody,
                             contVarObj, reused); /* PTS 1127661 FF */

            if ( e_ok != TransContext.trError_gg00 )
                WriteToTrace ( "bd04RollbackObj: ERROR" );
            else
                if ( LogAction_Trace.TracesLevel(6) )
                {
                    BeforeImage.WriteToTrace ( "bd04RollbackObj: OK" );
                    WriteToTrace (NULL);
                }
            break;
        }
    default:
        TransContext.trError_gg00 = e_not_implemented;
    }
}
    
/* --------------------------------------------------------------------------*/
void Log_ActionObject::Redo (tgg00_TransContext &TransContext, Log_IImage &Image, SAPDBErr_MessageList &Errlist) const
{
    bool bReused     = ((  m_Head.flags & Head::Reused ) > 0 ); /* PTS 1127661 FF */
    SAPDB_Int1 ContVarObj = ((( m_Head.flags & Head::ContVarObject ) > 0 ) ? 2 : 1);

    if ( ! Image.IsAfterImage() )
    {
        TransContext.trError_gg00 = e_not_implemented;
        return;
    }

    Log_AfterImage &AfterImage = *(static_cast<Log_AfterImage*> (&Image));

    switch ( Image.GetActionType() )
    {
    case Log_NewObject:    
		bd95RollforwardObj_NewObj (TransContext,
			m_Head.oid,
			m_Head.version,
			m_Head.fileno,
			m_Head.keySize,
			m_pKey,
			AfterImage.GetSequence(),
			AfterImage.GetTransNo(),
            ContVarObj,               /* PTS 1127661 FF */
            Errlist);
		break;
    case Log_InsertObject: 
    case Log_UpdateObject: 
        {
		    bd95Rollforward_UpdObj (TransContext,
			    m_Head.oid,
			    m_Head.version,
			    m_Head.fileno,
			    m_Head.bodySize,
			    m_pBody,
			    AfterImage.GetSequence(),
			    AfterImage.GetTransNo(),
                ContVarObj, bReused );     /* PTS 1127661 FF */             
		    break;
        }
    case Log_DeleteObject: 
		bd95RollforwardObj_DelObj (TransContext,
			m_Head.oid,
			m_Head.version,
			m_Head.fileno,
			AfterImage.GetSequence(),
			AfterImage.GetTransNo(),
            ContVarObj, bReused,     /* PTS 1127661 FF */             
            Errlist);
		break;
    default:
        TransContext.trError_gg00 = e_not_implemented;
        break;
    }
}

/* --------------------------------------------------------------------------*/
void Log_ActionObject::WriteToTrace (const char * Title) const
{
    Kernel_VTrace trace;
    
    if ( Title != NULL )
        trace << Title << " ";

    //trace << "ObjFileNo: " << m_Head.fileno;

    tgg92_KernelOid oid = m_Head.oid;

    if ( oid.gg92IsNilPno() )
        trace << ", Oid: nil";
    else
        trace << ", Oid: " << oid.gg92GetPno()
                           << "." << oid.gg92GetPos()
                           << ":" << oid.gg92GetFrameVers();
    trace << ", BodySize: "  << m_Head.bodySize << ", KeySize: " << m_Head.keySize << NewLine;
    #ifdef SAPDB_SLOW
    trace << "PersistentLength: "
          << GetPersistentLength() << " ("
          << GetMinimalPersistentLength() << ")\n";
	#endif
}

/* --------------------------------------------------------------------------*/
void Log_ActionObject::AppendContextInfo (SAPDB_Char  const * const  filename,
                                          SAPDB_UInt4 const          linenumber,
                                          SAPDBErr_MessageList      &errlist,
                                          SAPDB_Char  const * const  title) const
{
    tgg92_KernelOid oid = m_Head.oid;

    errlist = errlist +
              Log_Exception(__CONTEXT__,LOG_ACTIONOBJECT_INFO,
                            (title!=0?title:"Log_ActionObject"),
                            SAPDB_ToString(oid.gg92GetPno()),
                            SAPDB_ToString(oid.gg92GetPos()),
                            SAPDB_ToString(oid.gg92GetFrameVers()),
                            SAPDB_ToString(m_Head.bodySize),
                            SAPDB_ToString(m_Head.keySize));
}
