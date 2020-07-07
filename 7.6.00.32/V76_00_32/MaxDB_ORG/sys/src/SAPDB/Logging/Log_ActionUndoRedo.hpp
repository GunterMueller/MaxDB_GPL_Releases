/*!
  @file    Log_ActionUndoRedo.hpp
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
#ifndef LOG_ACTION_UNDO_REDO_HPP
#define LOG_ACTION_UNDO_REDO_HPP

#include "Logging/Log_Types.hpp"
#include "Logging/Log_UndoFile.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Restart/Rst_RedoFile.hpp"

class Log_Transaction;
class Log_BeforeImage;
class Log_AfterImage;
class Data_SplitSpaceReader;
class SAPDBErr_MessageList;

/// this is needed for copying log entries during redo
SAPDB_UInt Log_ActionGetMinCopyLength (
    const Log_ActionType    type,
    const Log_ActionVersion version = 0);

/// this is needed for testing consistency
void Log_ActionTestAllActions();

/// performs an undo-action defined by an beforeImage
bool Log_ActionExecuteUndo (Log_Transaction        &trans,
                            Log_BeforeImage        &BeforeImage,
                            Data_SplitSpaceReader  &Reader,
                            Log_UndoFile::Iterator &iter,
                            SAPDBErr_MessageList   &errlist);

/// result for Log_ActionExecuteRedo
enum Log_ActionExecuteRedoResult // PTS ? 2005-01-03
{
    Log_ActionExecuteRedoOk,
    Log_ActionExecuteRedoIsAborted,
    Log_ActionExecuteRedoError
};

/// performs an redo-action defined by an afterImage
Log_ActionExecuteRedoResult Log_ActionExecuteRedo (Log_Transaction        &trans,
                                                   Log_AfterImage         &AfterImage,
                                                   Log_IOSequenceNo        IOSequence,
                                                   Data_SplitSpaceReader  &Reader,
                                                   Rst_RedoFile::Iterator &iter,
                                                   SAPDBErr_MessageList   &errlist);

/// This writes a partial rollback log entry to the redo log
Log_Transaction::LogState
Log_ActionWritePartialRollback (Log_Transaction      &trans,
                                Log_EntrySequence     UndoStopSequence,
                                Log_EntrySequence     RedoStopSequence,
                                SAPDBErr_MessageList &errlist);
#endif  /* LOG_ACTION_UNDO_REDO_HPP */
