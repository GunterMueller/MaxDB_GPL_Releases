/*! @defgroup ActionHandling Action handling */
/*!
    @ingroup ActionHandling
    @file    Log_IAction.hpp
	@author  UweH
	@brief   Defines the interface Log_IAction for all actions.

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
#ifndef Log_IAction_H
#define Log_IAction_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Transaction/Trans_Context.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Log.h"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_Types.hpp"
#include "ggg00.h"

class   SAPDBMem_IRawAllocator;
class   Data_SplitSpaceWriter;
class   Data_SplitSpaceReader;
class   Log_IImage;
class   SAPDBErr_MessageList;

/*!
   @brief Baseclass for all actions.

   - Actions define an operation, something, which ist executed.
   - The persistent format describes what is to be done.
   - To undo an action, the inverse action must be created and written to an undo file.
 */
class Log_IAction
{
public:
    /// constructor
    Log_IAction(Log_ActionType type)
    : m_Type (type)
    {}
    /// This returns the actions type.
    Log_ActionType GetType () const
    {
        return m_Type;
    }
    /// returns true if this action handler is assigned a valid action type
    bool IsValid() const
    {
        return m_Type != Log_NoOp;
    }
    /// Returns the version of the log action entry. Default is 0.
    virtual Log_ActionVersion GetPersistentVersion() const
    {
        return 0;
    }
    /// Some special actions need to be written with no restrictions like LOGFULL. These entries may return true.
	// PTS 1117320 UH 2002-10-28 added
    virtual bool ForceLogWriting () const
    {
        return false;
    }
    /// Some special actions need a log position. These entries may return true. e.g. commit, rollback, savepoint
	// PTS 1117320 UH 2002-10-28 added
    virtual bool WaitForLogWriting () const
    {
        return false;
    }
    /// if this returns false a local redo entry cash must be flushed
    virtual bool LocalRedoEntryCachingPossible() const
    {
        return true;
    }
    /// if this returns true the current action used in an undo context must be processed by the GarbageCollector
    virtual bool GarbageCollectionNeeded() const
    {
        return false;
    }
    /// This returns the persistent length of the actions image, which must not be split.
    virtual SAPDB_UInt GetMinimalPersistentLength () const = 0;
    /// This returns the total persistent length of the actions image.
    virtual SAPDB_UInt GetPersistentLength () const = 0;
    /// Everything is written which is needed to re-execute this action.
    virtual bool WritePersistentFormat (Data_SplitSpaceWriter &Writer,
                                        SAPDBErr_MessageList  &errlist) const = 0;
    /// Everything is read which is needed to re-execute this action.
    virtual bool ReadPersistentFormat (Data_SplitSpaceReader  &Reader,
                                       SAPDBMem_IRawAllocator &Allocator,
                                       SAPDBErr_MessageList   &errlist) = 0;
    /// Executes the redo action
    virtual void Redo (tgg00_TransContext &TransContext, Log_IImage &Image, SAPDBErr_MessageList &Errlist) const
    {
        TransContext.trError_gg00 = e_not_implemented;
        AppendContextInfo (__CONTEXT__, Errlist, "Redo() not defined by action");
    }
    /// Executes the action.
    virtual void Execute (tgg00_TransContext &TransContext) const
    {
        TransContext.trError_gg00 = e_not_implemented;
        AppendContextInfo (__CONTEXT__, Trans_Context::GetContext(TransContext).ErrorList(), "Execute() not defined by action");
    }
    /// Executes the undo action
    virtual void Undo (tgg00_TransContext &TransContext, Log_IImage &Image) const
    {
        TransContext.trError_gg00 = e_not_implemented;
        AppendContextInfo (__CONTEXT__, Trans_Context::GetContext(TransContext).ErrorList(), "Undo() not defined by action");
	}
    /// The action is written to the trace file.
    virtual void WriteToTrace (const char * Title) const
    {
        Kernel_VTrace trace;
        if ( Title != 0 )
            trace << Title << FlushLine;
        trace << "Type: " << Log_GetActionTypeString(m_Type) << "." << GetPersistentVersion()
              << ", Len=" << GetPersistentLength() << " (" << GetMinimalPersistentLength() << ")"
              << FlushLine;
        trace << "- ForceLogWriting: "   << (ForceLogWriting()  ? "true":"false")
              << ", WaitForLogWriting: " << (WaitForLogWriting()? "true":"false")
              << ", LocalRedoEntryCachingPossible: " << (LocalRedoEntryCachingPossible()? "true":"false")
              << FlushLine;
    }
    /// append some info to the errlist
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const
	{
        errlist.AddDetail( SDBMSG_LOG_IACTION_INFO,
                           Msg_Arg(SDBMSGTAG_LOG_IACTION_INFO__ACTION_TYPE,        Log_GetActionTypeString(GetType()) ),
                           Msg_Arg(SDBMSGTAG_LOG_IACTION_INFO__ACTION_VERSION,     SAPDB_ToString(GetPersistentVersion())),
	                       Msg_Arg(SDBMSGTAG_LOG_IACTION_INFO__ACTION_LENGTH,      SAPDB_ToString(GetPersistentLength())),
	                       Msg_Arg(SDBMSGTAG_LOG_IACTION_INFO__ACTION_MIN_LENGTH,  SAPDB_ToString(GetMinimalPersistentLength())),
	                       Msg_Arg(SDBMSGTAG_LOG_IACTION_INFO__ACTION_FORCE_WRITE, (ForceLogWriting()  ? "true":"false") ),
	                       Msg_Arg(SDBMSGTAG_LOG_IACTION_INFO__ACTION_WAIT_FOR_WR, (WaitForLogWriting()? "true":"false") ),
	                       Msg_Arg(SDBMSGTAG_LOG_IACTION_INFO__ACTION_LOCAL_CACHE, (LocalRedoEntryCachingPossible()?"true":"false") ));
    }
protected:
    /// The actions type.
    Log_ActionType m_Type;
};
#endif  /* Log_IAction_H */
