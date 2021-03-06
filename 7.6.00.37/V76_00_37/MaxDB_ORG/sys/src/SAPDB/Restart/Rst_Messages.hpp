/*
        ========== licence begin  GPL
        Copyright (C) 2005 SAP AG

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
#ifndef RST_MESSAGES_HPP
#define RST_MESSAGES_HPP

// Generated Header file Rst_Messages.hpp.
// Generated by genmsg V2.2 DO NOT EDIT

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

#define RST_HINT_PARAM_MAXTRANS            SAPDBErr_MessageList::Info,0,"Is parameter _MAXTRANS equal to the online setting ?",0
#define RST_HINT_PARAM_MAXTRANS_ID 0
#define RST_HINT_RESTORE_LOG               SAPDBErr_MessageList::Info,1,"It might be necessary to recover with a (different) log backup.",0
#define RST_HINT_RESTORE_LOG_ID 1
#define RST_HINT_CORRUPTED_LOG1            SAPDBErr_MessageList::Info,2,"The log volume contents read is invalid.",0
#define RST_HINT_CORRUPTED_LOG1_ID 2
#define RST_HINT_CORRUPTED_LOG2            SAPDBErr_MessageList::Info,3,"Try to backup the log in cold mode and begin reinstallation.",0
#define RST_HINT_CORRUPTED_LOG2_ID 3
#define RST_REDO_PROGRESS                  SAPDBErr_MessageList::Info,4,"Statistics: transactions read %s, redone %s",2
#define RST_REDO_PROGRESS_ID 4
#define RST_INFO_VERIFY                    SAPDBErr_MessageList::Info,5,"Verify of %s with root %s was successfull",2
#define RST_INFO_VERIFY_ID 5
#define RST_RECOV_TAPE                     SAPDBErr_MessageList::Info,6,"recovering log from tape from IOSeq '%s' until IOSeq: '%s'",2
#define RST_RECOV_TAPE_ID 6
#define RST_RECOV_LOG_VOL                  SAPDBErr_MessageList::Info,7,"recovering log from log_volume from IOSeq: '%s'",1
#define RST_RECOV_LOG_VOL_ID 7
#define RST_SAVEPOINT_VERSION              SAPDBErr_MessageList::Info,8,"The savepoint was written with version: '%s'.",1
#define RST_SAVEPOINT_VERSION_ID 8
#define RST_AFTERIMAGE_HEAD_INFO           SAPDBErr_MessageList::Info,9,"%s: val='%s' len=%s tot.len=%s",4
#define RST_AFTERIMAGE_HEAD_INFO_ID 9
#define RST_REDO_ABORTED                   SAPDBErr_MessageList::Warning,10,"ABORTED by external request",0
#define RST_REDO_ABORTED_ID 10
#define RST_ERROR_VERIFY                   SAPDBErr_MessageList::Error,11,"Verify of %s with root %s gots error %s",3
#define RST_ERROR_VERIFY_ID 11
#define RST_INVALID_REDO_OFFSET            SAPDBErr_MessageList::Error,12,"LastRedoOffset from Synchronize is invalid %s",1
#define RST_INVALID_REDO_OFFSET_ID 12
#define RST_START_TASK_ERROR               SAPDBErr_MessageList::Error,13,"Cannot start %s task %s from %s. Error: %s '%s'.",5
#define RST_START_TASK_ERROR_ID 13
#define RST_STOP_TASK_ERROR                SAPDBErr_MessageList::Error,14,"Cannot receive redo task. Error: %s '%s'.",2
#define RST_STOP_TASK_ERROR_ID 14
#define RST_REDO_TASK_RECEIVED_ERROR       SAPDBErr_MessageList::Error,15,"Recevied error %s '%s' from redo task.",2
#define RST_REDO_TASK_RECEIVED_ERROR_ID 15
#define RST_REDO_ERROR_FROM_RESTORE        SAPDBErr_MessageList::Error,16,"Recevied error %s '%s' from restore component.",2
#define RST_REDO_ERROR_FROM_RESTORE_ID 16
#define RST_DUPLICATE_REDO_JOB             SAPDBErr_MessageList::Error,17,"Duplicate redo job index:%s iosequence:%s trans:%s (redo list in knltrace).",3
#define RST_DUPLICATE_REDO_JOB_ID 17
#define RST_DUPLICATE_REDO_TRANSNO         SAPDBErr_MessageList::Error,18,"Duplicate redo transno index:%s iosequence:%s trans:%s (redo list in knltrace).",3
#define RST_DUPLICATE_REDO_TRANSNO_ID 18
#define RST_NO_FREE_REDO_JOB               SAPDBErr_MessageList::Error,19,"No free job entry for index:%s iosequence:%s trans:%s (redo list in knltrace).",3
#define RST_NO_FREE_REDO_JOB_ID 19
#define RST_INSERT_IOSEQUENCE_FAILED       SAPDBErr_MessageList::Error,20,"New redo job with eotseq:%s must be older then lastseq:%s (oldestseq:%s).",3
#define RST_INSERT_IOSEQUENCE_FAILED_ID 20
#define RST_NO_FREE_IOSEQ_LIST_ENTRY       SAPDBErr_MessageList::Error,21,"No free ioseqlist entry available for redo job with eotseq:%s",1
#define RST_NO_FREE_IOSEQ_LIST_ENTRY_ID 21
#define RST_NO_TRANS_ENTRY_AVAILABLE       SAPDBErr_MessageList::Error,22,"No more trans entries available for Tr%s.",1
#define RST_NO_TRANS_ENTRY_AVAILABLE_ID 22
#define RST_SAVEPOINT_NOT_FOUND            SAPDBErr_MessageList::Error,23,"Expected savepoint entry type is '%s'",1
#define RST_SAVEPOINT_NOT_FOUND_ID 23
#define RST_REDO_BEGIN_NOT_FOUND           SAPDBErr_MessageList::Error,24,"Begin of redo not found on the log.",0
#define RST_REDO_BEGIN_NOT_FOUND_ID 24
#define RST_FINISH_OPEN_TRANS_MISSING      SAPDBErr_MessageList::Error,25,"Logentry 'FinishOpenTrans' missing after truncated logentry.",0
#define RST_FINISH_OPEN_TRANS_MISSING_ID 25
#define RST_DIFFERENT_EOT_FOUND            SAPDBErr_MessageList::Error,26,"EndOfTrans %s found but state is %s.",2
#define RST_DIFFERENT_EOT_FOUND_ID 26
#define RST_ERROR_DURING_READ_ACTION       SAPDBErr_MessageList::Error,27,"The image of an action cannot be correctly read.",0
#define RST_ERROR_DURING_READ_ACTION_ID 27
#define RST_EOT_HANDLING_NO_TRANSENTRY     SAPDBErr_MessageList::Error,28,"Transentry for open trans not found.",0
#define RST_EOT_HANDLING_NO_TRANSENTRY_ID 28
#define RST_EOT_HANDLING_WRONG_CONTEXT     SAPDBErr_MessageList::Error,29,"Different Log_TransContext. It may be that transaction end was found twice.",0
#define RST_EOT_HANDLING_WRONG_CONTEXT_ID 29
#define RST_EOT_HANDLING_INSERT_JOB_FAILED SAPDBErr_MessageList::Error,30,"The redo transaction could not be inserted into list of redo jobs.",0
#define RST_EOT_HANDLING_INSERT_JOB_FAILED_ID 30
#define RST_SOFTWARE_VERSION_TOO_OLD       SAPDBErr_MessageList::Error,31,"A savepoint was found which was written by a newer software version.",0
#define RST_SOFTWARE_VERSION_TOO_OLD_ID 31
#define RST_CLOSE_OPEN_TRANS_FAILED        SAPDBErr_MessageList::Error,32,"The forced close of all open tranactions at the end of the log failed.",0
#define RST_CLOSE_OPEN_TRANS_FAILED_ID 32
#define RST_ERROR_DURING_PARTIAL_ROLLBACK  SAPDBErr_MessageList::Error,33,"Partial rollback during redo reading failed.",0
#define RST_ERROR_DURING_PARTIAL_ROLLBACK_ID 33
#define RST_ERROR_DURING_COPY_ENTRY        SAPDBErr_MessageList::Error,34,"The copy of the redo entry from log to redotransfile failed.",0
#define RST_ERROR_DURING_COPY_ENTRY_ID 34
#define RST_ERROR_DURING_COPYCONT_ENTRY    SAPDBErr_MessageList::Error,35,"The continuation-copy of the redo entry from log to redotransfile failed.",0
#define RST_ERROR_DURING_COPYCONT_ENTRY_ID 35
#define RST_ERROR_DURING_RELEASE_SPACE     SAPDBErr_MessageList::Error,36,"The release of an interrupted partially copied logentry failed",0
#define RST_ERROR_DURING_RELEASE_SPACE_ID 36
#define RST_UNEXPECTED_RESULT              SAPDBErr_MessageList::Error,37,"Function %s returned %s which is not expected.",2
#define RST_UNEXPECTED_RESULT_ID 37
#define RST_COPY_ENTRY_RESERVE_FAILED      SAPDBErr_MessageList::Error,38,"ReserveSpace in CopyEntry failed for length=%s (SplitSpace not assigned)",1
#define RST_COPY_ENTRY_RESERVE_FAILED_ID 38
#define RST_WRITE_FAILED                   SAPDBErr_MessageList::Error,39,"Error during writing the read logentry into redo file, result=%s",1
#define RST_WRITE_FAILED_ID 39
#define RST_NO_SPACE_HANDLER_FOR_REDO_COPY SAPDBErr_MessageList::Error,40,"RedoFile is missing or space could not be dereferenced.",0
#define RST_NO_SPACE_HANDLER_FOR_REDO_COPY_ID 40

#endif /* RST_MESSAGES_HPP */
