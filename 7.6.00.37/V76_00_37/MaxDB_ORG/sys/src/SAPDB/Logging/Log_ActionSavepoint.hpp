/*!
  @file           Log_ActionSavepoint.hpp
  @ingroup        ActionHandling
  @author         UweH
  @brief          defines class Log_ActionSavepoint

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
#ifndef Log_ActionSavepoint_H
#define Log_ActionSavepoint_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "DataAccess/Data_ISplitSpace.hpp"
#include "Logging/Log_IAction.hpp"
#include "hgg11.h"   // g11kernel_version

/*!
   @brief          Handle for manipulating OMS objects.
   @copydoc        Log_IAction
 */
class Log_ActionSavepoint : public Log_IAction
{
    friend struct Head;

public:

    /// constructor for the redo-reader. The values are set be a call to
    /// ReadPersistenFormat()
    Log_ActionSavepoint ()
    : Log_IAction(Log_SavepointEntry)
    {
        m_Head.Reason = Log_SVPReasonUnknown;
        m_Head.additionalInfo = noAdditionalInfo;
    }
    
    /// constructor for well known-data (the reason)
    Log_ActionSavepoint (Log_SavepointReason Reason)
    : Log_IAction(Log_SVPReasonRestart == Reason ?Log_FinishOpenTrans:Log_SavepointEntry)
    {
        m_Head.Reason = Reason;
        if (m_Head.Reason == Log_SVPReasonRestart)
        {
            m_Head.additionalInfo = dbKernelVersion;
            g11kernel_version(m_Body.kernelVersion);
        }
    }
    
    /// The log entry must be written even if logqueue full
    virtual bool ForceLogWriting () const
    {
        // PTS 1117320 UH 2002-10-28 added
        return true;
    }

    /// The writer of this entry will wait for writing and receive the writing position.
    virtual bool WaitForLogWriting () const
    {
        // PTS 1117320 UH 2002-10-28 added
        return true;
    }

    /// if this returns false a local redo entry cash must be flushed
    virtual bool LocalRedoEntryCachingPossible() const
    {
        return false;
    }

    
    /// minimal length usabale by CopyEntry
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return sizeof (Head);
    }

    /// only persistent header size
    virtual SAPDB_UInt GetMinimalPersistentLength() const
    {
        return GetMinimalLengthToCopyEntry();
    }
    
    /// complete persistent size
    virtual SAPDB_UInt GetPersistentLength() const
    {
        if (HasKernelVersionString())
            return GetMinimalPersistentLength() + sizeof(Body);
        return GetMinimalPersistentLength();
    }
    
    /// writes the entry in a persistent format
    virtual bool WritePersistentFormat (Data_SplitSpaceWriter &Writer,
                                        SAPDBErr_MessageList  &errlist) const
    {
        Head                          *pHead;
        Data_SplitSpaceWriter::Result  result =
            Writer.Reserve (sizeof(Head), reinterpret_cast<SAPDB_Byte*&>(pHead));
            
        if ( (HasKernelVersionString()   && Data_SplitSpaceWriter::moreSpaceAvailable != result)
             ||
             (! HasKernelVersionString() && Data_SplitSpaceWriter::ok != result) )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                    "Writer.reserve error");
            Writer.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        *pHead = m_Head;
        
        if ( HasKernelVersionString() )
        {
            result = Writer.Write (reinterpret_cast<const SAPDB_Byte*>(&m_Body), sizeof(m_Body));
            if ( Data_SplitSpaceWriter::ok != result )
            {
                errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                        "Writer.Write error");
                Writer.AppendContextInfo(__CONTEXT__,errlist);
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }
        return true;
    }
    
    /// reads from persistent image
    virtual bool ReadPersistentFormat (Data_SplitSpaceReader  &Reader,
                                       SAPDBMem_IRawAllocator &Allocator,
                                       SAPDBErr_MessageList   &errlist)
    {
        Head                          *pHead;
        Data_SplitSpaceReader::Result  result =
            Reader.Reserve (sizeof(Head), reinterpret_cast<SAPDB_Byte*&>(pHead));
        if ( Data_SplitSpaceReader::ok != result
             &&
             Data_SplitSpaceWriter::moreSpaceAvailable != result )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                    "Reader.Reserve error");
            Reader.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        m_Head = *pHead;
        
        if ( HasKernelVersionString() )
        {
            SAPDBERR_ASSERT_STATE( Data_SplitSpaceWriter::moreSpaceAvailable == result );

            result = Reader.Read (reinterpret_cast<SAPDB_Byte*>(&m_Body),sizeof(m_Body));
            if ( Data_SplitSpaceReader::ok != result )
            {
                errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                        "Reader.Read error");
                Reader.AppendContextInfo(__CONTEXT__,errlist);
                AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }
        }
        return true;
    }
    
    /// writes entry information to the knltrace
    virtual void WriteToTrace (const char * Title) const
    {}

    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const
	{}

    /// returns whether the savepoint-entry has an DB-Version-String or not
    bool HasKernelVersionString () const
    {
        return (m_Head.additionalInfo == dbKernelVersion);
    }

    /// returns the kernel-version of the savepoint. If nothing had been
    /// read, it returns a string filled with spaces
    tsp00_Version GetKernelVersionString () const
    {
        SAPDBERR_ASSERT_STATE( HasKernelVersionString() );

        return  m_Body.kernelVersion;
    }

    /// defines the kind of additional information, which follows the header
    enum SavepointAdditionalInfo 
    {
        noAdditionalInfo = 0,
        dbKernelVersion = 1
    };

    /// the description of the Header of the persistent information
    struct Head
    {
        SAPDB_UInt1 Reason;          ///< the reason for the savepoint (1 byte)
        SAPDB_UInt1 additionalInfo;  ///< what kind of data follows the header (1 byte) 
        SAPDB_UInt2 Filler2;         ///< filler (2 byte)

        /// constructor
        Head()
        {
            Reason         = Log_SVPReasonUnknown;
            additionalInfo = noAdditionalInfo;
            Filler2        = 0;
        }
    };

    /// Body definition
    struct Body
    {
        /// some entries contain the version of the running kernel software
        tsp00_Version kernelVersion;
        /// defeault constructor
        Body()
        {
            kernelVersion.SetBlank();
        }
    };

    /// the in memory-representation of the header information
    Head m_Head;
    
    /// the in memory-representation of the additional information. Valid only
    /// if HasKernelVersionString() == true.
    Body m_Body;
};


#endif  /* Log_ActionSavepoint_H */
