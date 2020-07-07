/*!
  @file    Log_ActionUndoRedo.cpp
  @ingroup Logging
  @author  UweH
  @brief   action handling for undo and redo

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
#include "Logging/Log_ActionUndoRedo.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "Logging/Log_AfterImage.hpp"
#include "Logging/Log_BeforeImage.hpp"

#include "Logging/Log_ActionCopyUpdBlob75.hpp"
#include "Logging/Log_ActionBlobCopy.hpp"
#include "Logging/Log_ActionBlobCopySame.hpp"
#include "Logging/Log_ActionBlobExpand.hpp"
#include "Logging/Log_ActionBlobTruncate.hpp"
#include "Logging/Log_ActionBlobUpdate.hpp"
#include "Logging/Log_ActionCreateTable.hpp"
#include "Logging/Log_ActionCreateDropFile.hpp"
#include "Logging/Log_ActionDropFile75.hpp"
#include "Logging/Log_ActionCreateTable75.hpp"
#include "Logging/Log_ActionFile.hpp"
#include "Logging/Log_ActionCreateIndexFile.hpp"
#include "Logging/Log_ActionCreateIndex75.hpp"
#include "Logging/Log_ActionCreateShortColumnFile.hpp"
#include "Logging/Log_ActionInsDelRec.hpp"
#include "Logging/Log_ActionInsDelRec75.hpp"
#include "Logging/Log_ActionModifyTable.hpp"
#include "Logging/Log_ActionObject.hpp"
#include "Logging/Log_ActionObjectFile.hpp"
#include "Logging/Log_ActionRefreshSurrogSeq.hpp"
#include "Logging/Log_ActionRenameFile75.hpp"
#include "Logging/Log_ActionReorganizeFile.hpp"
#include "Logging/Log_ActionReassignSecondaryFile.hpp"
#include "Logging/Log_ActionSetFileAttributes.hpp"
#include "Logging/Log_ActionSetFileDirCounter.hpp"
#include "Logging/Log_ActionUpdateRec.hpp"
#include "Logging/Log_ActionUpdateRec75.hpp"
#include "Logging/Log_ActionUnqualifiedDelete.hpp"
#include "Logging/Log_ActionPartialRollback.hpp"
#include "Logging/Log_ActionSavepoint.hpp"

#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_AutoPtr.hpp"
#include "Restart/Rst_RedoManager.hpp"
#include "Restart/Rst_IRedoTrafficControl.hpp"

/* -------------------------------------------------------------------------------- */
/// Writes information about the current entry into the kerneltrace 
static void TraceEntry ( const char*          title,
                         const tgg91_TransNo &TransNo,
                         Log_IOSequenceNo     SubTransNo,
                         Log_EntrySequence    sequence,
                         Log_ActionType       actionType)
{
    Kernel_VTrace() << title
                    << ": T" << TransNo.gg90GetInt4()
                    << "/"   << SubTransNo
                    << " ("  << sequence
                    << ") '" << Log_GetActionTypeString(actionType)
                    << "'";
}

/* -------------------------------------------------------------------------------- */
/// continues an undo/redo-operation, after the Undo/Redo had been temporarily released
/// its ressources (because it must not block the savepoint)
static bool UndoRedoContinue (const tgg91_TransNo    &transno,
                              Log_IImage             &image,
                              Data_IBreakable        &iter,
                              Data_SplitSpaceReader  &reader,
                              SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("UndoRedoContinue", LogTrans_Trace, 5);
        
    Log_IOSequenceNo iosequence;

    if ( ! iter.Continue(errlist) )
    {
        reader.AppendContextInfo(__CONTEXT__,errlist);
        image.AppendContextInfo(errlist); // TraceEntry ("entry",transno,iosequence,image.GetSequence(),image.GetActionType());
        iter.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    Log_EntrySequence checkSequence = image.GetSequence();
    
    reader.Reset();
    
    if ( image.IsAfterImage() )
    {
        Log_EntrySequence entrySequence;
        
        if ( ! Rst_RedoFile::GetEntryInfo(reader, iosequence, entrySequence, errlist) )
        {
            errlist = errlist + Log_Exception(__CONTEXT__,LOG_READ_REDO_ENTRY_FAILED);
            image.AppendContextInfo(errlist); // TraceEntry ("entry",transno,iosequence,image.GetSequence(),image.GetActionType());
            iter.AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
        
        if ( checkSequence != entrySequence )
        {
            errlist = Log_Exception(__CONTEXT__,LOG_UNEXPECTED_REDO_ENTRY_SEQUENCE,
                                                SAPDB_ToString(entrySequence),
                                                SAPDB_ToString(checkSequence));
            reader.AppendContextInfo(__CONTEXT__,errlist);
            image.AppendContextInfo(errlist); // TraceEntry ("entry",transno,iosequence,image.GetSequence(),image.GetActionType());
            iter.AppendContextInfo(__CONTEXT__,errlist);
            return false;
        }
    }

    if ( ! image.ReadPersistentFormat (reader, errlist) )
    {
        errlist = errlist + Log_Exception(__CONTEXT__,LOG_READ_IMAGE_AFTER_CONTINUE_FAILED);
        reader.AppendContextInfo(__CONTEXT__,errlist);
        image.AppendContextInfo(errlist); // TraceEntry ("entry",transno,iosequence,image.GetSequence(),image.GetActionType());
        iter.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    
    if ( image.GetSequence() != checkSequence )
    {
        errlist = errlist + Log_Exception(__CONTEXT__,
                                          LOG_UNEXPECTED_REDO_ENTRY_SEQUENCE,
                                          SAPDB_ToString(image.GetSequence()),
                                          SAPDB_ToString(checkSequence));
        reader.AppendContextInfo(__CONTEXT__,errlist);
        image.AppendContextInfo(errlist); // TraceEntry ("entry",transno,iosequence,image.GetSequence(),image.GetActionType());
        iter.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return true;
}

/* -------------------------------------------------------------------------------- */
/// reads the persistent  image of an action and executes it
static Log_ActionExecuteRedoResult RedoAction (
                        Log_Transaction        &trans,
                        Log_IImage             &image,
                        Log_IAction            &action,
                        Data_IBreakable        &iter,
                        Data_SplitSpaceReader  &reader,
                        SAPDBErr_MessageList   &errlist)
{
    if(!action.ReadPersistentFormat (reader, trans.Allocator(), errlist))
    {
        iter.AppendContextInfo   (__CONTEXT__,errlist);
        reader.AppendContextInfo (__CONTEXT__,errlist);
        image.AppendContextInfo  (errlist);
        trans.AppendContextInfo  (__CONTEXT__,errlist);
        return Log_ActionExecuteRedoError;
    }

    if(!iter.Break(errlist)) // PTS 1114994 2002-03-22
    {
        iter.AppendContextInfo   (__CONTEXT__,errlist);
        reader.AppendContextInfo (__CONTEXT__,errlist);
        image.AppendContextInfo  (errlist);
        trans.AppendContextInfo  (__CONTEXT__,errlist);
        return Log_ActionExecuteRedoError;
    }

    if ( LogTrans_Trace.TracesLevel(6) )
    {
        action.WriteToTrace("RedoAction: ");
    }

    action.Redo (trans.GetOldTransContext(), image, errlist);
    if (e_ok != trans.GetOldTransContext().trError_gg00)
    {
        iter.AppendContextInfo(__CONTEXT__,errlist);
        reader.AppendContextInfo(__CONTEXT__,errlist);
        trans.AppendContextInfo(__CONTEXT__,errlist);
        return Log_ActionExecuteRedoError;
    }

    if(!UndoRedoContinue(trans.GetRedoTransNo(), image, iter, reader, errlist)) // PTS 1114994 2002-03-22
    {
        action.AppendContextInfo(__CONTEXT__,errlist);
        iter.AppendContextInfo(__CONTEXT__,errlist);
        reader.AppendContextInfo(__CONTEXT__,errlist);
        image.AppendContextInfo(errlist);
        trans.AppendContextInfo(__CONTEXT__,errlist);
        return Log_ActionExecuteRedoError;
    }
    return Log_ActionExecuteRedoOk;
}

/* -------------------------------------------------------------------------------- */
/// reads the persistent image of an action and executes the undo action
static bool UndoAction (Log_Transaction        &trans,
                        Log_IImage             &beforeImage,
                        Log_IAction            &action,
                        Data_IBreakable        &iter,
                        Data_SplitSpaceReader  &reader,
                        SAPDBErr_MessageList   &errlist)
{
    if ( ! action.ReadPersistentFormat (reader, trans.Allocator(), errlist) )
    {
        trans.AppendContextInfo      (__CONTEXT__,errlist);
        beforeImage.AppendContextInfo(errlist);
        iter.AppendContextInfo       (__CONTEXT__,errlist);
        reader.AppendContextInfo     (__CONTEXT__,errlist);
        return false;
    }

    // the undo file resources must be released to avoid
    // a deadlock

    Log_EntrySequence checkSequence = beforeImage.GetSequence();

    if ( ! iter.Break(errlist) )
    {
        iter.AppendContextInfo       (__CONTEXT__,errlist);
        reader.AppendContextInfo     (__CONTEXT__,errlist);
        beforeImage.AppendContextInfo(errlist);
        trans.AppendContextInfo      (__CONTEXT__,errlist);
        return false;
    }

    if ( LogTrans_Trace.TracesLevel(6) )
        action.WriteToTrace("Undo: ");

    action.Undo (trans.GetOldTransContext(), beforeImage);
    if ( e_ok != trans.GetOldTransContext().trError_gg00 )
    {
        Kernel_AppendBasisError(trans.GetOldTransContext().trError_gg00, errlist, "action.Undo");
        action.AppendContextInfo     (__CONTEXT__,errlist);
        iter.AppendContextInfo       (__CONTEXT__,errlist);
        reader.AppendContextInfo     (__CONTEXT__,errlist);
        beforeImage.AppendContextInfo(errlist);
        trans.AppendContextInfo      (__CONTEXT__,errlist);
        return false;
    }
    if(!UndoRedoContinue(trans.GetRedoTransNo(), beforeImage, iter, reader, errlist))
    {
        action.AppendContextInfo      (__CONTEXT__,errlist);
        iter.AppendContextInfo        (__CONTEXT__,errlist);
        reader.AppendContextInfo      (__CONTEXT__,errlist);
        beforeImage.AppendContextInfo (errlist);
        trans.AppendContextInfo       (__CONTEXT__,errlist);
        return false;
    }
    return true;
}

/* -------------------------------------------------------------------------------- */
/// reads the persistent  image of an action and executes it
static bool UndoRedoAction (Log_Transaction        &trans,
                            Log_IImage             &image,
                            Log_IAction            &action,
                            Data_IBreakable        &iter,
                            Data_SplitSpaceReader  &reader,
                            SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("UndoRedoAction", LogTrans_Trace, 5);
    
    if (!action.ReadPersistentFormat (reader, trans.Allocator(), errlist))
    {
        trans.AppendContextInfo (__CONTEXT__,errlist);
        iter.AppendContextInfo  (__CONTEXT__,errlist);
        reader.AppendContextInfo(__CONTEXT__,errlist);
        image.AppendContextInfo (errlist);
        errlist = errlist + Log_Exception(__CONTEXT__,LOG_ACTION_COULD_NOT_BE_READ);
        return false;
    }

    if(!iter.Break(errlist))
    {
        trans.AppendContextInfo (__CONTEXT__,errlist);
        iter.AppendContextInfo  (__CONTEXT__,errlist);
        reader.AppendContextInfo(__CONTEXT__,errlist);
        image.AppendContextInfo (errlist);
        return false;
    }
   
    action.Execute (trans.GetOldTransContext());

    if(e_ok != trans.GetOldTransContext().trError_gg00)
    {
        trans.AppendContextInfo (__CONTEXT__,errlist);
        iter.AppendContextInfo  (__CONTEXT__,errlist);
        reader.AppendContextInfo(__CONTEXT__,errlist);
        image.AppendContextInfo (errlist);
        action.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }
    return UndoRedoContinue (trans.GetRedoTransNo(), image, iter, reader, errlist);
}

/*---------------------------------------------------------------------------*/
void Log_ActionTestAllActions()
{
	for ( Log_ActionType actionType = Log_NoOp;
	      actionType < Log_ActionType_DO_NOT_USE;
	      actionType = Log_ActionType(actionType + 1) )
	{
		const SAPDB_Char *typestr = Log_GetActionTypeString(actionType);
		Log_ActionGetMinCopyLength(actionType);
    }
}

/* --------------------------------------------------------------------------- */

SAPDB_UInt Log_ActionGetMinCopyLength (
    const Log_ActionType    type,
    const Log_ActionVersion version)
{
    switch (type)
    {
        case Log_NoOp:
        case Log_IllegalType:
            return 0;
        case Log_Commit:
        case Log_Rollback:
            return 0;
        case Log_PartialRollback:
            return Log_ActionPartialRollback::GetMinimalLengthToCopyEntry();
        case Log_NewObject:
        case Log_InsertObject:
        case Log_UpdateObject:
        case Log_DeleteObject:
        case Log_LockObject:
            return Log_ActionObject::GetMinimalLengthToCopyEntry();
        case Log_CreateObjectFile:
        case Log_DropObjectFile:
            return Log_ActionObjectFile::GetMinimalLengthToCopyEntry();
        case Log_InsertRecord:
        case Log_DeleteRecord:
        case Log_MarkedDeleted:
            if (version == Log_ActionInsDelRec::actionVersionStartingWith760002)
                return Log_ActionInsDelRec::GetMinimalLengthToCopyEntry();
            else
                return Log_ActionInsDelRec75::GetMinimalLengthToCopyEntry();
        case Log_UpdateRecord:
            if (version == Log_ActionUpdateRec::actionVersionStartingWith760002)
                return Log_ActionUpdateRec::GetMinimalLengthToCopyEntry();
            else
                return Log_ActionUpdateRec75::GetMinimalLengthToCopyEntry();
        case Log_ModifyTable:
            return Log_ActionModifyTable::GetMinimalLengthToCopyEntry();
        case Log_ChangeFileType:
            return Log_ActionChangeFileType::GetMinimalLengthToCopyEntry();
        case Log_CreateFile:
            return Log_ActionCreateFile::GetMinimalLengthToCopyEntry();
        case Log_CreateIndex:
            if (version == Log_ActionCreateIndex::actionVersionStartingWith760002)
                return Log_ActionCreateIndex::GetMinimalLengthToCopyEntry();
            else
                return Log_ActionCreateIndex75::GetMinimalLengthToCopyEntry();
        case Log_CreateTable:
            if (version == Log_ActionCreateTable::actionVersionStartingWith760018)
                return Log_ActionCreateTable::GetMinimalLengthToCopyEntry();
            else if (version == Log_ActionCreateTable::actionVersionStartingWith760002)
                return Log_ActionCreateTable01::GetMinimalLengthToCopyEntry();
            else
                return Log_ActionCreateTable75::GetMinimalLengthToCopyEntry();
        case Log_RenameFile:
            switch (version)
            {
            	case Log_ActionRenameFile::actionVersionStartingWith760011:
                    return Log_ActionRenameFile::GetMinimalLengthToCopyEntry();
                case Log_ActionRenameFile::actionVersionStartingWith760002:
                    return Log_ActionRenameFile760002::GetMinimalLengthToCopyEntry();
                case Log_ActionRenameFile::actionVersionStartingWith74:
                    return Log_ActionRenameFile75::GetMinimalLengthToCopyEntry();
				default:
                    RTE_Crash (Log_Exception(__CONTEXT__,LOG_ACTION_NO_MIN_COPY_LENGTH,
		                                                        Log_GetActionTypeString(type)));
			}
        case Log_SuccFileVersion:
            return Log_ActionSuccFileVersion::GetMinimalLengthToCopyEntry(); 
        case Log_UpdFileStatistic:
            return Log_ActionUpdFileStatistic::GetMinimalLengthToCopyEntry(); 
        case Log_CopyBlob:
            if (version == Log_ActionBlobCopy::actionVersionStartingWith760011)
                return Log_ActionBlobCopy::GetMinimalLengthToCopyEntry();
            else
                return Log_ActionCopyBlob75::GetMinimalLengthToCopyEntry(); 
        case Log_CopySameBlob:
            if (version == Log_ActionBlobCopySame::actionVersionStartingWith760011)
                return Log_ActionBlobCopySame::GetMinimalLengthToCopyEntry();
            else
                return Log_ActionCopySameBlob75::GetMinimalLengthToCopyEntry(); 
        case Log_ExpandBlob:
            if (version == Log_ActionBlobExpand::actionVersionStartingWith760011)
                return Log_ActionBlobExpand::GetMinimalLengthToCopyEntry();
            else
                return Log_ActionExpandBlob75::GetMinimalLengthToCopyEntry(); 
        case Log_TruncateBlob:
            if (version == Log_ActionBlobTruncate::actionVersionStartingWith760011)
                return Log_ActionBlobTruncate::GetMinimalLengthToCopyEntry();
            else
                return Log_ActionTruncateBlob75::GetMinimalLengthToCopyEntry(); 
        case Log_UpdateBlob:
            if (version == Log_ActionBlobUpdate::actionVersionStartingWith760011)
                return Log_ActionUpdateBlob75::GetMinimalLengthToCopyEntry();
            else
                return Log_ActionUpdateBlob75::GetMinimalLengthToCopyEntry();
        case Log_RefreshSequence:
            return Log_ActionRefreshSequence::GetMinimalLengthToCopyEntry(); 
        case Log_RefreshSurrogate:
            return Log_ActionRefreshSurrogate::GetMinimalLengthToCopyEntry(); 
        case Log_SavepointEntry:
        case Log_FinishOpenTrans:
            return Log_ActionSavepoint::GetMinimalLengthToCopyEntry();
        case Log_ReorganizeFile:
            return Log_ActionReorganizeFile::GetMinimalLengthToCopyEntry();
        case Log_Reusable_2:
        case Log_NotUsed_SetFileState:
            return 0;
        case Log_SetFileAttributes:
            return Log_ActionSetFileAttributes::GetMinimalLengthToCopyEntry();
        case Log_CreateIndexFile:
            return Log_ActionCreateIndexFile::GetMinimalLengthToCopyEntry();
        case Log_ReassignSecondaryFile:
            return Log_ActionReassignSecondaryFile::GetMinimalLengthToCopyEntry();
        case Log_MarkFileAsDeleted:
            return Log_ActionFile::GetMinimalLengthToCopyEntry();
        case Log_UnqualifiedDelete:
                return Log_ActionUnqualifiedDelete::GetMinimalLengthToCopyEntry(version);
        case Log_CreateShortColumnFile:
            return Log_ActionCreateShortColumnFile::GetMinimalLengthToCopyEntry();
        case Log_MultipleActions:
            return Log_ActionMultiple::GetMinimalLengthToCopyEntry();
        case Log_SetFileDirCounter:
            return Log_ActionSetFileDirCounter::GetMinimalLengthToCopyEntry();
        default:
		    RTE_Crash (Log_Exception(__CONTEXT__,LOG_ACTION_NO_MIN_COPY_LENGTH,
		                             Log_GetActionTypeString(type)));
    }
    return SAPDB_MAX_UINT4; // illegal length
}
/* --------------------------------------------------------------------------- */

Log_ActionExecuteRedoResult
     Log_ActionExecuteRedo (Log_Transaction        &trans,
                            Log_AfterImage         &AfterImage,
                            Log_IOSequenceNo        IOSequence,
                            Data_SplitSpaceReader  &Reader,
                            Rst_RedoFile::Iterator &iter,
                            SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Log_ActionExecuteRedo", LogTrans_Trace, 5);

    // PTS 1114994 2002-03-22 begin
    Rst_IRedoTrafficControl *redoControl = Kernel_IAdminRestartShutdown::Instance().GetRedoManager().GetRedoTrafficControl();
    tsp00_TaskId            &taskid       = trans.GetOldTransContext().trTaskId_gg00;
    bool                     waitInserted = false;
    tgg00_WaitContext        waitContext;

    if ( redoControl == 0 )
    {
        iter.AppendContextInfo      (__CONTEXT__,errlist);
        Reader.AppendContextInfo    (__CONTEXT__,errlist);
        AfterImage.AppendContextInfo(errlist);
        trans.AppendContextInfo     (__CONTEXT__,errlist);
        return Log_ActionExecuteRedoError;
    }

    switch ( redoControl->CheckIoSeq ( taskid, waitContext, IOSequence, waitInserted, errlist ) ) // PTS ? 2005-01-03
    {
        case Rst_IRedoTrafficControl::CheckIoSeqOk:
            break;
        case Rst_IRedoTrafficControl::CheckIoSeqRedoIsAborted: // PTS ? 2005-01-03
            return Log_ActionExecuteRedoIsAborted;
        case Rst_IRedoTrafficControl::CheckIoSeqError:
            ; // fall through default:
        default:
            iter.AppendContextInfo      (__CONTEXT__,errlist);
            Reader.AppendContextInfo    (__CONTEXT__,errlist);
            AfterImage.AppendContextInfo(errlist);
            trans.AppendContextInfo     (__CONTEXT__,errlist);
            return Log_ActionExecuteRedoError;
    }
        
    if ( waitInserted )
    {
        if(!iter.Break(errlist))
        {
            iter.AppendContextInfo      (__CONTEXT__,errlist);
            Reader.AppendContextInfo    (__CONTEXT__,errlist);
            AfterImage.AppendContextInfo(errlist);
            trans.AppendContextInfo     (__CONTEXT__,errlist);
        }

        if ( redoControl->WaitForIoSeq (taskid) )
            return Log_ActionExecuteRedoIsAborted;
            
        if(!UndoRedoContinue(trans.GetRedoTransNo(), AfterImage, iter, Reader, errlist))
            return Log_ActionExecuteRedoError;
    }
    
    if ( AfterImage.GetTransNo() != trans.GetRedoTransNo() )
    {
        errlist = Log_Exception(__CONTEXT__,LOG_ACTION_FROM_WRONG_TRANSACTION,
                                            SAPDB_ToString(AfterImage.GetTransNo().gg90GetInt4()),
                                            SAPDB_ToString(trans.GetRedoTransNo().gg90GetInt4()));
        iter.AppendContextInfo      (__CONTEXT__,errlist);
        Reader.AppendContextInfo    (__CONTEXT__,errlist);
        AfterImage.AppendContextInfo(errlist);
        trans.AppendContextInfo     (__CONTEXT__,errlist);
        return Log_ActionExecuteRedoError;
    }
    
    // PTS 1114994 2002-03-22 end

    if ( LogTrans_Trace.TracesLevel(6) )
		TraceEntry ( "Execute",
			         trans.GetRedoTransNo(),
				     IOSequence,
					 AfterImage.GetSequence(),
					 AfterImage.GetActionType() );

    switch ( AfterImage.GetActionType() )
    {
    case Log_NoOp:
        return Log_ActionExecuteRedoOk;
    case Log_NewObject:
    case Log_InsertObject:
    case Log_UpdateObject:
    case Log_DeleteObject:
        {
            Log_ActionObject action;
            return RedoAction (trans, AfterImage, action, iter, Reader, errlist);
        }
    case Log_LockObject:
    case Log_ReorganizeFile:
        errlist = Log_Exception(__CONTEXT__,
                                LOG_ACTION_IN_REDO_NOT_ALLOWED,
                                Log_GetActionTypeString(AfterImage.GetActionType()));
        iter.AppendContextInfo(__CONTEXT__,errlist);
        Reader.AppendContextInfo(__CONTEXT__,errlist);
        AfterImage.AppendContextInfo(errlist);
        trans.AppendContextInfo(__CONTEXT__,errlist);
        return Log_ActionExecuteRedoError;
    case Log_CreateObjectFile:
    case Log_DropObjectFile:
        {
            Log_ActionObjectFile action;
            return RedoAction (trans, AfterImage, action, iter, Reader, errlist);
        }
    case Log_InsertRecord:
    case Log_DeleteRecord:
    case Log_MarkedDeleted:
        {
            const bool NOT_IsBeforeImage = false;
            if (AfterImage.GetPersistentVersion() ==
                Log_ActionInsDelRec::actionVersionStartingWith760002)
            {
                Log_ActionInsDelRec ActionInsDelRec (AfterImage.GetActionType(), NOT_IsBeforeImage);
                return UndoRedoAction (trans, AfterImage, ActionInsDelRec, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
            }
            else
            {
                Log_ActionInsDelRec75 ActionInsDelRec (AfterImage.GetActionType(), NOT_IsBeforeImage);
                return UndoRedoAction (trans, AfterImage, ActionInsDelRec, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
            }
        }
    case Log_UpdateRecord:
        {
            if (AfterImage.GetPersistentVersion() ==
                Log_ActionInsDelRec::actionVersionStartingWith760002)
            {
                Log_ActionUpdateRec ActionUpdateRec;
                return UndoRedoAction (trans, AfterImage, ActionUpdateRec, iter, Reader, errlist)
                       ? Log_ActionExecuteRedoOk
                       : Log_ActionExecuteRedoError;
            }
            else
            {
                Log_ActionUpdateRec75 ActionUpdateRec;
                return UndoRedoAction (trans, AfterImage, ActionUpdateRec, iter, Reader, errlist)
                       ? Log_ActionExecuteRedoOk
                       : Log_ActionExecuteRedoError;
            }
        }
    case Log_ModifyTable:
        {
            Log_ActionModifyTable ActionModifyTable;
            return UndoRedoAction (trans, AfterImage, ActionModifyTable, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_ChangeFileType:
        {
            Log_ActionChangeFileType ActionChangeFileType;
            return UndoRedoAction (trans, AfterImage, ActionChangeFileType, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_CreateFile:
        {
            Log_ActionCreateFile ActionCreateFile;
            return UndoRedoAction (trans, AfterImage, ActionCreateFile, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_CreateIndexFile:
        {
            Log_ActionCreateIndexFile action;
            return UndoRedoAction (trans, AfterImage, action, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_ReassignSecondaryFile:
        {
            Log_ActionReassignSecondaryFile action;
            return UndoRedoAction (trans, AfterImage, action, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_CreateIndex:
        {
            if ( AfterImage.GetPersistentVersion() ==
                 Log_ActionCreateIndex::actionVersionStartingWith760002 )
            {
                Log_ActionCreateIndex ActionCreateIndex;
                return UndoRedoAction (trans, AfterImage, ActionCreateIndex, iter, Reader, errlist)
                       ? Log_ActionExecuteRedoOk
                       : Log_ActionExecuteRedoError;
            }
            else
            {
                Log_ActionCreateIndex75 ActionCreateIndex;
                return UndoRedoAction (trans, AfterImage, ActionCreateIndex, iter, Reader, errlist)
                       ? Log_ActionExecuteRedoOk
                       : Log_ActionExecuteRedoError;
            }
        }
    case Log_CreateTable:
        {
            if ( AfterImage.GetPersistentVersion() ==
                 Log_ActionCreateTable::actionVersionStartingWith760018 )
            {
                Log_ActionCreateTable ActionCreateTable;
                return UndoRedoAction (trans, AfterImage, ActionCreateTable, iter, Reader, errlist)
                       ? Log_ActionExecuteRedoOk
                       : Log_ActionExecuteRedoError;
            }
            if ( AfterImage.GetPersistentVersion() ==
                 Log_ActionCreateTable::actionVersionStartingWith760002 )
            {
                Log_ActionCreateTable01 ActionCreateTable;
                return UndoRedoAction (trans, AfterImage, ActionCreateTable, iter, Reader, errlist)
                       ? Log_ActionExecuteRedoOk
                       : Log_ActionExecuteRedoError;
            }
            else
            {
                Log_ActionCreateTable75 ActionCreateTable;
                return UndoRedoAction (trans, AfterImage, ActionCreateTable, iter, Reader, errlist)
                       ? Log_ActionExecuteRedoOk
                       : Log_ActionExecuteRedoError;
            }

        }
    case Log_RenameFile:
        switch ( AfterImage.GetPersistentVersion() )
        {
            case Log_ActionRenameFile::actionVersionStartingWith760011:
            {
                Log_ActionRenameFile action;
                return UndoRedoAction (trans, AfterImage, action, iter, Reader, errlist)
                       ? Log_ActionExecuteRedoOk
                       : Log_ActionExecuteRedoError;
            }
            case Log_ActionRenameFile::actionVersionStartingWith760002:
            {
                Log_ActionRenameFile760002 action;
                return UndoRedoAction (trans, AfterImage, action, iter, Reader, errlist)
                       ? Log_ActionExecuteRedoOk
                       : Log_ActionExecuteRedoError;
            }
            case Log_ActionRenameFile::actionVersionStartingWith74:
            {
                Log_ActionRenameFile75 ActionRenameFile;
                return UndoRedoAction (trans, AfterImage, ActionRenameFile, iter, Reader, errlist)
                       ? Log_ActionExecuteRedoOk
                       : Log_ActionExecuteRedoError;
            }
			default:
				return Log_ActionExecuteRedoError;
        }
    case Log_SuccFileVersion:
        {
            Log_ActionSuccFileVersion ActionSuccFileVersion;
            return UndoRedoAction (trans, AfterImage, ActionSuccFileVersion, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_UpdFileStatistic:
        {
            Log_ActionUpdFileStatistic ActionUpdFileStatistic;
            return UndoRedoAction (trans, AfterImage, ActionUpdFileStatistic, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_CopyBlob:
        if ( AfterImage.GetPersistentVersion()
             ==
             Log_ActionBlobCopy::actionVersionStartingWith760011 )
        {
            Log_ActionBlobCopy ActionCopyBlob;
            return UndoRedoAction (trans, AfterImage, ActionCopyBlob, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
        else
        {
            Log_ActionCopyBlob75 ActionCopyBlob;
            return UndoRedoAction (trans, AfterImage, ActionCopyBlob, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_CopySameBlob:
        if ( AfterImage.GetPersistentVersion()
             ==
             Log_ActionBlobCopySame::actionVersionStartingWith760011 )
        {
            Log_ActionBlobCopySame action;
            return UndoRedoAction (trans, AfterImage, action, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
        else
        {
            Log_ActionCopySameBlob75 action;
            return UndoRedoAction (trans, AfterImage, action, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_ExpandBlob:
        {
            Log_ActionExpandBlob75 ActionExpandBlob;
            return UndoRedoAction (trans, AfterImage, ActionExpandBlob, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_TruncateBlob:
        if ( AfterImage.GetPersistentVersion()
             ==
             Log_ActionBlobTruncate::actionVersionStartingWith760011 )
        {
            Log_ActionBlobTruncate action;
            return UndoRedoAction (trans, AfterImage, action, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
        else
        {
            Log_ActionTruncateBlob75 action;
            return UndoRedoAction (trans, AfterImage, action, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_UpdateBlob:
        if ( AfterImage.GetPersistentVersion()
             ==
             Log_ActionBlobUpdate::actionVersionStartingWith760011 )
        {
            Log_ActionBlobUpdate action;
            return UndoRedoAction (trans, AfterImage, action, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
        else
        {
            Log_ActionUpdateBlob75 action;
            return UndoRedoAction (trans, AfterImage, action, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_RefreshSequence:
        {
            Log_ActionRefreshSequence ActionRefreshSequence;
            return UndoRedoAction (trans, AfterImage, ActionRefreshSequence, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_RefreshSurrogate:
        {
            Log_ActionRefreshSurrogate ActionRefreshSurrogate;
            return UndoRedoAction (trans, AfterImage, ActionRefreshSurrogate, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_MarkFileAsDeleted:
        {
            Log_ActionFile action (AfterImage.GetActionType());
            return RedoAction (trans, AfterImage, action, iter, Reader, errlist);
        }
    case Log_CreateShortColumnFile:
        {
            Log_ActionCreateShortColumnFile action;
            return UndoRedoAction (trans, AfterImage, action, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_UnqualifiedDelete:
        {
            SAPDB_AutoPtr<Log_ActionUnqualifiedDelete> action( trans.Allocator(),
                Log_ActionUnqualifiedDelete::CreateObject(trans.Allocator(), AfterImage.GetPersistentVersion()));
            
            if ( action.GetPtr() == 0 )
            {
                trans.GetOldTransContext().trError_gg00 = e_no_more_memory;
                trans.AppendContextInfo(__CONTEXT__,errlist);
                AfterImage.AppendContextInfo(errlist);
                iter.AppendContextInfo(__CONTEXT__,errlist);
                Reader.AppendContextInfo(__CONTEXT__,errlist);
                return Log_ActionExecuteRedoError;
            }            
            
            return RedoAction (trans, AfterImage, *(action.GetPtr()), iter, Reader, errlist);
        }
    case Log_SetFileDirCounter:
        {
            Log_ActionSetFileDirCounter action;
            return UndoRedoAction (trans, AfterImage, action, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    case Log_SetFileAttributes:
        {
            Log_ActionSetFileAttributes action;
            return UndoRedoAction (trans, AfterImage, action, iter, Reader, errlist)
                   ? Log_ActionExecuteRedoOk
                   : Log_ActionExecuteRedoError;
        }
    default:
        #ifdef SAPDB_SLOW
        errlist = Log_Exception(__CONTEXT__, LOG_ACTION_UNKNOWN_FOR_REDO,
                                Log_GetActionTypeString(AfterImage.GetActionType()));
        iter.AppendContextInfo(__CONTEXT__,errlist);
        Reader.AppendContextInfo(__CONTEXT__,errlist);
        AfterImage.AppendContextInfo(errlist);
        trans.AppendContextInfo(__CONTEXT__,errlist);
        #endif
        return Log_ActionExecuteRedoOk;
    }
}

/* --------------------------------------------------------------------------- */
bool Log_ActionExecuteUndo (Log_Transaction        &trans,
                            Log_BeforeImage        &BeforeImage,
                            Data_SplitSpaceReader  &Reader,
                            Log_UndoFile::Iterator &iter,
                            SAPDBErr_MessageList   &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Log_Transaction::ExecuteUndoAction", LogTrans_Trace, 5);

    switch ( BeforeImage.GetActionType() )
    {
    case Log_NoOp:
        return true;
    case Log_NewObject:
    case Log_InsertObject:
    case Log_UpdateObject:
    case Log_DeleteObject:
    case Log_LockObject:
        {
            Log_ActionObject action;
            return UndoAction (trans, BeforeImage, action, iter, Reader, errlist);
        }
    case Log_CreateObjectFile:
    case Log_DropObjectFile:
        {
            Log_ActionObjectFile action;
            return UndoAction (trans, BeforeImage, action, iter, Reader, errlist);
        }
    case Log_ReorganizeFile:
        {
            Log_ActionReorganizeFile action;
            return UndoRedoAction (trans, BeforeImage, action, iter, Reader, errlist);
        }
    case Log_InsertRecord:
    case Log_DeleteRecord:
    case Log_MarkedDeleted:
        {
            const bool IsBeforeImage = true;
            const tgg91_TransNo updTrans = BeforeImage.GetPreviousUpdateTransNo();
            const tgg91_PageRef prevRef  = BeforeImage.GetPreviousImage();

            if (BeforeImage.GetPersistentVersion() ==
                Log_ActionInsDelRec::actionVersionStartingWith760002)
            {
                Log_ActionInsDelRec ActionInsDelRec (BeforeImage.GetActionType(), updTrans, prevRef);
                return UndoRedoAction (trans, BeforeImage, ActionInsDelRec, iter, Reader, errlist);
            }
            else
            {
                Log_ActionInsDelRec75 ActionInsDelRec (BeforeImage.GetActionType(), updTrans, prevRef);
                return UndoRedoAction (trans, BeforeImage, ActionInsDelRec, iter, Reader, errlist);
            }
        }
    case Log_UpdateRecord:
        {
            const tgg91_TransNo updTrans = BeforeImage.GetPreviousUpdateTransNo();
            const tgg91_PageRef prevRef  = BeforeImage.GetPreviousImage();

            if (BeforeImage.GetPersistentVersion() ==
                Log_ActionUpdateRec::actionVersionStartingWith760002)
            {
                Log_ActionUpdateRec ActionUpdateRec(updTrans, prevRef);
                return UndoRedoAction (trans, BeforeImage, ActionUpdateRec, iter, Reader, errlist);
            }
            else
            {
                Log_ActionUpdateRec75 ActionUpdateRec(updTrans, prevRef);
                return UndoRedoAction (trans, BeforeImage, ActionUpdateRec, iter, Reader, errlist);
            }
        }
    case Log_ModifyTable:
        errlist = Log_Exception(__CONTEXT__,
                                LOG_ACTION_IN_UNDO_NOT_ALLOWED,
                                Log_GetActionTypeString(BeforeImage.GetActionType()));
        BeforeImage.AppendContextInfo(errlist);
        iter.AppendContextInfo(__CONTEXT__,errlist);
        Reader.AppendContextInfo(__CONTEXT__,errlist);
        BeforeImage.AppendContextInfo(errlist);
        trans.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    case Log_ChangeFileType:
        {
            Log_ActionChangeFileType ActionChangeFileType;
            return UndoRedoAction (trans, BeforeImage, ActionChangeFileType, iter, Reader, errlist);
        }
    case Log_CreateFile:
        {
            if ( BeforeImage.GetPersistentVersion()
                 == Log_ActionDropFile::actionVersionStartingWith760002 )
            {
                Log_ActionDropFile ActionDropFile;
                return UndoRedoAction (trans, BeforeImage, ActionDropFile, iter, Reader, errlist);
            }
            else
            {
                Log_ActionDropFile75 ActionDropFile;
                return UndoRedoAction (trans, BeforeImage, ActionDropFile, iter, Reader, errlist);
            }
        }
    case Log_ReassignSecondaryFile:
        {
            Log_ActionReassignSecondaryFile action;
            return UndoRedoAction (trans, BeforeImage, action, iter, Reader, errlist);
        }
	case Log_CreateIndexFile:
	case Log_CreateShortColumnFile:
	case Log_CreateIndex:
    case Log_CreateTable:
        errlist = Log_Exception(__CONTEXT__,
                                LOG_ACTION_IN_UNDO_NOT_ALLOWED,
                                Log_GetActionTypeString(BeforeImage.GetActionType()));
        iter.AppendContextInfo(__CONTEXT__,errlist);
        Reader.AppendContextInfo(__CONTEXT__,errlist);
        BeforeImage.AppendContextInfo(errlist);
        trans.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    case Log_RenameFile:
        switch ( BeforeImage.GetPersistentVersion() )
        {
            case Log_ActionRenameFile::actionVersionStartingWith760011:
            {
                Log_ActionRenameFile action;
                return UndoRedoAction (trans, BeforeImage, action, iter, Reader, errlist);
            }
            case Log_ActionRenameFile::actionVersionStartingWith760002:
            {
                Log_ActionRenameFile760002 action;
                return UndoRedoAction (trans, BeforeImage, action, iter, Reader, errlist);
            }
            case Log_ActionRenameFile::actionVersionStartingWith74:
            {
                Log_ActionRenameFile75 action;
                return UndoRedoAction (trans, BeforeImage, action, iter, Reader, errlist);
            }
            default:
                return false;
        }
    case Log_SuccFileVersion:
        {
            Log_ActionSuccFileVersion ActionSuccFileVersion;
            return UndoRedoAction (trans, BeforeImage, ActionSuccFileVersion, iter, Reader, errlist);
        }
    case Log_UpdFileStatistic:
        {
            Log_ActionUpdFileStatistic ActionUpdFileStatistic;
            return UndoRedoAction (trans, BeforeImage, ActionUpdFileStatistic, iter, Reader, errlist);
        }
    case Log_CopyBlob:
        if ( BeforeImage.GetPersistentVersion()
             ==
             Log_ActionBlobCopy::actionVersionStartingWith760011 )
        {
            Log_ActionBlobCopy action;
            return UndoRedoAction (trans, BeforeImage, action, iter, Reader, errlist);
        }
        else
        {
            Log_ActionCopyBlob75 action;
            return UndoRedoAction (trans, BeforeImage, action, iter, Reader, errlist);
        }
    case Log_CopySameBlob:
	case Log_ExpandBlob:
        errlist = Log_Exception(__CONTEXT__,
                                LOG_ACTION_IN_UNDO_NOT_ALLOWED,
                                Log_GetActionTypeString(BeforeImage.GetActionType()));
        iter.AppendContextInfo(__CONTEXT__,errlist);
        Reader.AppendContextInfo(__CONTEXT__,errlist);
        BeforeImage.AppendContextInfo(errlist);
        trans.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    case Log_TruncateBlob:
        if ( BeforeImage.GetPersistentVersion()
             ==
             Log_ActionBlobTruncate::actionVersionStartingWith760011 )
        {
            Log_ActionBlobTruncate action;
            return UndoRedoAction (trans, BeforeImage, action, iter, Reader, errlist);
        }
        else
        {
            Log_ActionTruncateBlob75 action;
            return UndoRedoAction (trans, BeforeImage, action, iter, Reader, errlist);
        }
    case Log_UpdateBlob:
	case Log_RefreshSequence:
	case Log_RefreshSurrogate:
        errlist = Log_Exception(__CONTEXT__,
                                LOG_ACTION_IN_UNDO_NOT_ALLOWED,
                                Log_GetActionTypeString(BeforeImage.GetActionType()));
        iter.AppendContextInfo(__CONTEXT__,errlist);
        Reader.AppendContextInfo(__CONTEXT__,errlist);
        BeforeImage.AppendContextInfo(errlist);
        trans.AppendContextInfo(__CONTEXT__,errlist);
        return false;
    case Log_MarkFileAsDeleted:
        {
            Log_ActionFile action (BeforeImage.GetActionType());
            return UndoAction (trans,BeforeImage,action,iter,Reader,errlist);
        }
    case Log_UnqualifiedDelete:
        {
            SAPDB_AutoPtr<Log_ActionUnqualifiedDelete> action(
                trans.Allocator(),
                Log_ActionUnqualifiedDelete::CreateObject(trans.Allocator(), BeforeImage.GetPersistentVersion()));

            if ( action.GetPtr() == 0 )
            {
                trans.GetOldTransContext().trError_gg00 = e_no_more_memory;
                trans.AppendContextInfo(__CONTEXT__,errlist);
                BeforeImage.AppendContextInfo(errlist);
                iter.AppendContextInfo(__CONTEXT__,errlist);
                Reader.AppendContextInfo(__CONTEXT__,errlist);
                return false;
            }            
            
            return UndoAction (trans,BeforeImage,*(action.GetPtr()),iter,Reader,errlist);
        }
    case Log_SetFileAttributes:
        {
            Log_ActionSetFileAttributes action;
            return UndoAction (trans,BeforeImage,action,iter,Reader,errlist);
        }
    case Log_SetFileDirCounter:
        return true;
    default:
        #ifdef SAPDB_QUICK
        errlist = Log_Exception(__CONTEXT__, LOG_ACTION_UNKNOWN_FOR_REDO,
                                Log_GetActionTypeString(BeforeImage.GetActionType()));
        iter.AppendContextInfo(__CONTEXT__,errlist);
        Reader.AppendContextInfo(__CONTEXT__,errlist);
        BeforeImage.AppendContextInfo(errlist);
        trans.AppendContextInfo(__CONTEXT__,errlist);
        #endif
        return true;
    }
}

/* -------------------------------------------------------------------------------- */
Log_Transaction::LogState
Log_ActionWritePartialRollback (Log_Transaction      &trans,
                                Log_EntrySequence     UndoStopSequence,
                                Log_EntrySequence     RedoStopSequence,
                                SAPDBErr_MessageList &errlist)
{
    Log_ActionPartialRollback Action;
    
    Action.Initialize (UndoStopSequence, RedoStopSequence);
    return trans.WriteRedoEntry(Action, errlist);
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
