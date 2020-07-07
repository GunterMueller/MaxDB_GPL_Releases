/*!
  @file           Log_AfterImage.hpp
  @ingroup        Logging
  @author         UweH
  @brief          defines class Log_AfterImage, Log_AfterImageSpace

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



#ifndef Log_AfterImage_H
#define Log_AfterImage_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_Types.hpp"
#include "DataAccess/Data_SplitSpaceWriter.hpp"
#include "DataAccess/Data_SplitSpaceReader.hpp"
#include "Logging/Log_AfterImageSpace.hpp"
#include "Logging/Log_IImage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Log.h"

#include "ggg91.h"

/// this entry had been already redone 
#define LOG_AFTER_IMAGE_IS_NOT_DONE SAPDB_FALSE

/// this entry had not been redone 
#define LOG_AFTER_IMAGE_IS_DONE     SAPDB_TRUE

/*!
    @class          Log_AfterImage
    @brief          This defines the common part of all after images.
                    It is in a persistent format.
 */
class Log_AfterImage : public Log_IImage
{
public:

    /// default constructor
    Log_AfterImage ()
    : m_pIsDone (0) // PTS 1115068 UH 2002-03-27
    {
		// PTS 1115068 UH 2002-03-27 initialization is done in ctor of m_Head
    }

    /// The persistent length is calculated and returned.
    static SAPDB_UInt GetPersistentLength ()
    {
        return sizeof(Head);
    }

    virtual bool IsAfterImage () const
    {
        return true;
    }

    virtual bool WritePersistentFormat (Data_SplitSpaceWriter &Writer,
                                        SAPDBErr_MessageList  &errlist) const
    {
        if ( Log_NoOp == m_Head.ActionType )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Log_AfterImage::WritePersistentFormat() ActionType must be valid.");
            return false;
        }

        Data_SplitSpaceWriter::Result  result;
        Head                          *pHead;
        
        result = Writer.Reserve (sizeof(Head), REINTERPRET_CAST(SAPDB_Byte*&,pHead));
                        
        if ( Data_SplitSpaceWriter::ok                 != result
             &&
             Data_SplitSpaceWriter::moreSpaceAvailable != result )
        {
            Writer.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(errlist);
            return false;
        }

        *pHead = m_Head;
        return true;
    }

    virtual bool ReadPersistentFormat (Data_SplitSpaceReader &Reader,
                                       SAPDBErr_MessageList  &errlist) // PTS 1115068 UH 2002-03-27 & was missing
    {
        Data_SplitSpaceReader::Result  result;
        Head                          *pHead;

        result = Reader.Reserve (sizeof(Head), REINTERPRET_CAST(SAPDB_Byte*&,pHead));
                        
        if ( Data_SplitSpaceReader::ok                 != result
             &&
             Data_SplitSpaceReader::moreSpaceAvailable != result )
        {
            Reader.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(errlist);
            return false;
        }

        m_Head    = *pHead;
        m_pIsDone = &(pHead->IsDone); // PTS 1115068 UH 2002-03-27

        return true;
    }

    virtual Log_ActionType GetActionType() const
    {
        if ( m_Head.ActionType >= Log_ActionType_DO_NOT_USE )
            return Log_NoOp;
        else
            return STATIC_CAST(Log_ActionType, m_Head.ActionType);
    }

    /// Returns the action version.
    virtual Log_ActionVersion GetPersistentVersion() const
    {
        return m_Head.ActionVersion;
    }

    /// returns the transaction-number
    const tgg91_TransNo& GetTransNo() const
    {
        return m_Head.TransNo;
    }

    /// returns the redo-sequence
    virtual Log_EntrySequence GetSequence() const
    {
        return m_Head.Sequence;
    }

    /// Within the entry it is stored, that it is re done.
    void SetIsDone ()
    {
		// PTS 1115068 UH 2002-03-27
        SAPDB_Byte *pIsDone = m_pIsDone == NULL ? &m_Head.IsDone : m_pIsDone;
        *pIsDone = LOG_AFTER_IMAGE_IS_DONE;
    }

    /// true, if entry is already re done.
    bool IsDone () const
    {
		// PTS 1115068 UH 2002-03-27
        const SAPDB_Byte *pIsDone = m_pIsDone == NULL ? &m_Head.IsDone : m_pIsDone;
        return LOG_AFTER_IMAGE_IS_DONE == *pIsDone;
    }
    /// all header fields which are not set before writing are set now
    void PrepareForWrite( Log_ActionType       type,
                          Log_ActionVersion    actionVersion,
                          const tgg91_TransNo &transno,
                          Log_EntrySequence    sequence,
                          Log_EntrySequence    lastUndoSequence )
    {
        m_Head.ActionType       = type;
        m_Head.ActionVersion    = actionVersion;
        m_Head.TransNo          = transno;
        m_Head.Sequence         = sequence;
        m_Head.LastUndoSequence = lastUndoSequence;
        m_Head.IsDone           = 0;
        m_Head.Filler1          = 0;
        m_Head.Filler2          = 0;
    }
    /// returns size of internal header
    static SAPDB_UInt GetHeaderSize()
    {
        return sizeof(Head);
    }
    
    virtual void WriteToTrace (const char *title = NULL) const
    {
        if ( title != NULL )
            Kernel_VTrace() << title;
        Kernel_VTrace() << "ActionType: " << Log_GetActionTypeString(GetActionType())
                        << "/"            << GetPersistentVersion()
                        << ", IsDone: "   << (IsDone()?"yes":"no")
                        << ", Sequence: " << GetSequence()
                        << ", Transno: "  << GetTransNo().gg90GetInt4();
    }

    /// append some info to the errlist
    virtual void AppendContextInfo (Msg_List                  &errlist,
                                    SAPDB_Char  const * const  title = 0) const
    {

        errlist.AddDetail( SDBMSG_LOG_AFTERIMAGE_INFO,
                           Msg_Arg("TITLE",           (title != 0 ? title : "Log_AfterImage")),
                           Msg_Arg("TYPE",            Log_GetActionTypeString(GetActionType())),
                           Msg_Arg("VERSION",         SAPDB_ToString(GetPersistentVersion())),
                           Msg_Arg("BOOL",            (IsDone()?"yes":"no")),
                           Msg_Arg("LOG_IO_SEQUENCE", SAPDB_ToString(GetSequence())),
                           Msg_Arg("TRANS_ID",        SAPDB_ToString(GetTransNo().gg90GetInt4())) );
    }

private:

    /// This is the structured header for the after-image
    struct Head
    {
        SAPDB_Byte        ActionType;       ///< The type of action, which is logged. (1 byte)
        SAPDB_Bool1       IsDone;           ///< if true: The action had been re-done (1 byte)
        SAPDB_Byte        ActionVersion;    ///< The version of the log action; changes if the action changes its parameters or semantics (1 byte)
        SAPDB_Byte        Filler1;          ///< filler (1 byte)
        Log_EntrySequence Sequence;         ///< This entry sequence no is local to the transaction. (4 bytes)
        Log_EntrySequence LastUndoSequence; ///< the sequencenumber of this entry for rollback (4 bytes)
        tgg91_TransNo     TransNo;          ///< The id of the transaction. (6 bytes)
        SAPDB_UInt2       Filler2;          ///< The subtransaction layer. (2 byte)
    };

    /// the in memory representation of the head
    Head m_Head;

    /*! @brief      flag, which denotes, if the AfterImage had been redone.
     *
     *  In the case of a rollback of a rollback, an entrie must not been
     *  rollbacked twice. In order to ensure this, the isDone-flag is checked
     *  before the entry is redone and set afterwards. Because the flag
     *  must be set not only in memory, but on the real page, this pointer
     *  may point to the isDone-Flag in the underlying page. */
    SAPDB_Byte *m_pIsDone;
};


/*===========================================================================*
*  DEFINITION OF INLINE METHODS                                              *
*============================================================================*/
                                       
#endif  /* Log_AfterImage_H */
