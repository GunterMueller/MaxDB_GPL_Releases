/*!
  @file           Log_BeforeImage.hpp
  @ingroup        Logging
  @author         UweH
  @brief          defines class Log_BeforeImage

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
*/
#ifndef Log_BeforeImage_H
#define Log_BeforeImage_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "DataAccess/Data_SplitSpace.hpp"
#include "DataAccess/Data_SplitSpaceWriter.hpp"
#include "DataAccess/Data_SplitSpaceReader.hpp"
#include "DataAccess/Data_PageSplitSpace.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_IImage.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Log.h"
#include "ggg91.h"

class Log_UndoPage;
/// a special split space to read before images
typedef Data_SplitSpace<Log_UndoPage> Log_BeforeImageSpace;


/// This defines the common part of all before images.
class Log_BeforeImage : public Log_IImage
{
public:

    /// default constructor which invalidates this image
    Log_BeforeImage ()
    : m_pState(0)
    , m_Head()
    {}
    
    /// used by Log_Transaction::WriteUndoEntry()
    void PrepareForWrite (      Log_ActionType     type,
                                Log_ActionVersion  actionVersion,
                          const tgg91_TransNo     &consistentViewNo,
                          const Log_EntrySequence &sequence,
                          const Log_EntrySequence &lastRedoSequence )
    {
        m_Head.ActionType       = type;
        m_Head.ActionVersion    = actionVersion;
        m_Head.ConsistentView   = consistentViewNo;
        m_Head.Sequence         = sequence;
        m_Head.LastRedoSequence = lastRedoSequence;

        m_Head.Filler1          = 0;
    }
    /// initializes the transinfo data for consistent read
    void SetTransInfoAndObjState ( const tgg00_ObjTransInfo  &transInfo,
                                         tgg00_ObjState_Enum  previousState = obsFree_egg00 )
    {
        m_Head.PreviousUpdateTransNo = transInfo.otrUpdTransId_gg00;
        m_Head.PreviousImage         = transInfo.otrBeforeRef_gg00;
        m_Head.PreviousState.becomes(previousState);
    }
    /// The persistent length is calculated and returned.
    static SAPDB_UInt GetPersistentLength ()
    {
        return sizeof (Head);
    }
    /// returns true, if image is an after image.
    virtual bool IsAfterImage () const
    {
        return false;
    }
    /*!
    @brief  The Space must be initialized with the value given by GetPersistentLength().
    @param  Writer [in] 
    @param  errlist [in]
    @return true if successfull
    */
    virtual bool WritePersistentFormat (Data_SplitSpaceWriter &Writer,
                                        SAPDBErr_MessageList  &errlist) const
    {
        Data_SplitSpaceWriter::Result  result;
        Head                          *pHead;
        
        result = Writer.Reserve (sizeof(Head), reinterpret_cast<SAPDB_Byte*&>(pHead));
                        
        if ( Data_SplitSpaceWriter::ok                 != result
             &&
             Data_SplitSpaceWriter::moreSpaceAvailable != result )
        {
            Writer.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(errlist);
            return false;
        }

        if ( Log_NoOp == m_Head.ActionType )
        {
            errlist = Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"Log_BeforeImage::WritePersistentFormat() ActionType must be valid.");
            return false;
        }

        *pHead = m_Head;
        return true;
    }
    /*!
    @brief          The Space must be initialized with the value given by GetPersistentLength().
    @param          Reader [in/out] 
    @param          errlist [in] 
    @return         true if successfull
    */
    virtual bool ReadPersistentFormat (Data_SplitSpaceReader &Reader,
                                       SAPDBErr_MessageList  &errlist)
    {
        Data_SplitSpaceReader::Result  result;
        Head                          *pHead = 0;
        
        result = Reader.Reserve (sizeof(Head), reinterpret_cast<SAPDB_Byte*&>(pHead));
                        
        if ( (Data_SplitSpaceReader::ok                 != result
              &&
              Data_SplitSpaceReader::moreSpaceAvailable != result)
              ||
              (0 == pHead) ) // PTS 1118382 UH 2002-10-17
        {
            Reader.AppendContextInfo(__CONTEXT__,errlist);
            AppendContextInfo(errlist);
            return false;
        }

        m_Head   = *pHead;
        m_pState = &(pHead->StatePersistent);
        return true;
    }
    /// writes the before image to the trace.
    virtual void WriteToTrace (const char * Title = NULL) const
    {
        Kernel_VTrace trace;
        
        if ( Title != NULL )
            trace << Title << "\n";
        trace << "BeforeImage: (" << GetSequence() << ") "
              << Log_GetActionTypeString(GetActionType())
              << "/"            << GetPersistentVersion()
              << " state= "     << m_Head.StatePersistent
              << " ConsistView: " << m_Head.ConsistentView.gg90GetInt4() << NewLine;
        trace << "PrevUpdTrNo: " << m_Head.PreviousUpdateTransNo.gg90GetInt4()
              << ", PrevState: " << (int)GetPreviousState()
              << ", PrevImage: " << m_Head.PreviousImage.gg91RefPno()
              << "." << m_Head.PreviousImage.gg91RefPos() << NewLine;
    }
    /// append some information to an errorlist
    virtual void AppendContextInfo (Msg_List                  &errlist,
                                    SAPDB_Char  const * const  title = 0) const
    {
        errlist.AddDetail( SDBMSG_LOG_BEFORE_IMAGE_INFO,
                           Msg_Arg("TITLE",             (title != 0 ? title : "BeforeImage")),
                           Msg_Arg("TYPE",              Log_GetActionTypeString(GetActionType())),
                           Msg_Arg("VERSION",           SAPDB_ToString(GetPersistentVersion())),
                           Msg_Arg("STATE",             SAPDB_ToString(GetState(false))),
                           Msg_Arg("CONSISTENT_VIEW",   SAPDB_ToString(m_Head.ConsistentView.gg90GetInt4())),
                           Msg_Arg("PREV_UPD_TRANS_ID", SAPDB_ToString(m_Head.PreviousUpdateTransNo.gg90GetInt4())),
                           Msg_Arg("PREV_STATE",        SAPDB_ToString(GetPreviousState())),
                           Msg_Arg("PAGENO",            SAPDB_ToString(m_Head.PreviousImage.gg91RefPno())),
                           Msg_Arg("PAGE_OFFSET",       SAPDB_ToString(m_Head.PreviousImage.gg91RefPos())) );
    }
    /// returns the transaction local sequence
    virtual Log_EntrySequence GetSequence() const
    {
        return m_Head.Sequence;
    }
    /// returns the current action type
    virtual Log_ActionType GetActionType() const
    {
        if ( m_Head.ActionType >= Log_ActionType_DO_NOT_USE )
            return Log_NoOp;
        else
            return static_cast<Log_ActionType>(m_Head.ActionType);
    }
    /// Returns the action version.
    virtual Log_ActionVersion GetPersistentVersion() const
    {
        return m_Head.ActionVersion;
    }
    /// used by Log_UndoFile::WriteUndoEntry(nil-pos)
    void InvalidateReferenceToPreviousBeforeImage()
    {
        m_Head.PreviousImage.gg91SetNilRef();
    }
    /// Within the entry it is stored, that it is rollbacked by transaction.
    void SetRollbacked (bool inPersistentFormat)
    {
        SetState (inPersistentFormat, rollbacked);
    }
    /// Within the entry it is stored, that it is deleted by garbage collector.
    void SetDeleted (bool inPersistentFormat)
    {
        SetState (inPersistentFormat, deleted);
    }
    /// returns true, if entry is already rollbacked.
    bool IsRollbacked (bool inPersistentFormat) const
    {
        return GetState (inPersistentFormat) == rollbacked;
    }
    /// returns true, if entry is already deleted.
    bool IsDeleted (bool inPersistentFormat) const
    {
        return GetState (inPersistentFormat) == deleted;
    }
    /// return consistent view no
    const tgg91_TransNo& GetConsistentView() const 
    {
        return m_Head.ConsistentView;
    }
    /// returns previos updater id
    const tgg91_TransNo& GetPreviousUpdateTransNo() const
    {
        return m_Head.PreviousUpdateTransNo;
    }
    /// returns reference to previous before image
    const tgg91_PageRef& GetPreviousImage () const
    {
        return m_Head.PreviousImage;
    }
    /// set previous object / record state
    const tgg00_ObjState& GetPreviousState() const
    {
        return m_Head.PreviousState;
    }

public:

	/// the internal working state of this before image
    enum State
    {
        created    = 0,  ///< the undo entry for this action was just created
        rollbacked = 1,  ///< the action was already rollbacked
        deleted    = 2   ///< the action  was processed by garbage collector
    };

private:

    void SetState (bool  inPersistentFormat,
                   State state)
    {
        SAPDB_Byte * pDestination = inPersistentFormat ? m_pState : &(m_Head.StatePersistent);
            
        switch (state)
        {
            case created:
                *pDestination = 0;
                break;
            case rollbacked:
                *pDestination = 1;
                break;
            case deleted:
                *pDestination = 2;
                break;
            default:
                RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"unknown before image state"));
        }
    }

    State GetState (bool inPersistentFormat) const
    {
        if (inPersistentFormat)
        {
            SAPDBERR_ASSERT_STATE(m_pState != NULL);
            return State(*m_pState);
        }
        else
            return State(m_Head.StatePersistent);
    }

    /// This is the structured information of the before image
    struct Head
    {
        SAPDB_Byte        ActionType;             ///< (Log_ActionType) type of action                                    1 byte
        SAPDB_Byte        StatePersistent;        ///< (State) current working state of this image                        1 byte
        tgg00_ObjState    PreviousState;          ///< previous state of the object/record processed                      1 byte
        SAPDB_Byte        ActionVersion;          ///< the version of the persistent layout of the following action entry 1 byte
        Log_EntrySequence Sequence;               ///< the transaction local sequence number                              4 bytes
        Log_EntrySequence LastRedoSequence;       ///< the corresponding trans local redo sequence                        4 bytes
        tgg91_TransNo     ConsistentView;         ///< the id of the current consistent view                              6 bytes
        tgg91_TransNo     PreviousUpdateTransNo;  ///< update trans no of revious change                                  6 bytes
        tgg91_PageRef     PreviousImage;          ///< page reference to previous before image                            6 bytes
        SAPDB_UInt2       Filler1;                ///< not yet used                                                       2 byte
                                                  //                                                                     32 bytes        
        bool IsValid() const
        {
            return Log_NoOp != ActionType;
        }

        Head()
        {
            ActionType      = Log_NoOp;
            StatePersistent = created; // this cannot be set in PrepareForWrite because kb67DelUpdObj sets this to rollbacked
        }
    };

    /// the structured header itself
    Head m_Head;

    /// a reference to the memory where the before image was read into. this is used to update the state on the undo page
    SAPDB_Byte *m_pState;
};
#endif  /* Log_BeforeImage_H */
