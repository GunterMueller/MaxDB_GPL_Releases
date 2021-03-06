/*
        ========== licence begin  GPL
        Copyright (C) 2006 SAP AG

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
#ifndef LOG_MESSAGES_HPP
#define LOG_MESSAGES_HPP

// Generated Header file Log_Messages.hpp.
// Generated by genmsg V2.2 DO NOT EDIT

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

#define LOG_NO_INFOPAGE_UPD                          SAPDBErr_MessageList::Info,20000,"no update of LogInfoPage: DeviceState = %s, LogIsEmpty = %s",2
#define LOG_NO_INFOPAGE_UPD_ID 20000
#define LOG_IS_FULL                                  SAPDBErr_MessageList::Info,20001,"Log is full.",0
#define LOG_IS_FULL_ID 20001
#define LOG_SEQUENCE_NOT_FOUND                       SAPDBErr_MessageList::Info,20002,"Expected ioseq %s, found %s-%s on off %s",4
#define LOG_SEQUENCE_NOT_FOUND_ID 20002
#define LOG_SEQUENCE_NOT_IN_RANGE                    SAPDBErr_MessageList::Info,20003,"Ioseq %s not on device. Greatest IOSeq is %s",2
#define LOG_SEQUENCE_NOT_IN_RANGE_ID 20003
#define LOG_FORMATTING                               SAPDBErr_MessageList::Info,20004,"Formatting from offset %s to %s.",2
#define LOG_FORMATTING_ID 20004
#define LOG_RESTART_INFO                             SAPDBErr_MessageList::Info,20005,"Restart from ioseq %s @ off %s to ioseq %s @ off %s",4
#define LOG_RESTART_INFO_ID 20005
#define LOG_DEVICE_INFO_BASIC                        SAPDBErr_MessageList::Info,20006,"%s queues, flushmode is '%s', devstate is '%s'",3
#define LOG_DEVICE_INFO_BASIC_ID 20006
#define LOG_DEVICE_INFO_OLDEST_NOT_SAVED             SAPDBErr_MessageList::Info,20007,"Oldest not saved is ioseq %s @ off %s",2
#define LOG_DEVICE_INFO_OLDEST_NOT_SAVED_ID 20007
#define LOG_DEVICE_INFO_FIRST_KNOWN                  SAPDBErr_MessageList::Info,20008,"First known on LogVolume is ioseq %s @ off %s",2
#define LOG_DEVICE_INFO_FIRST_KNOWN_ID 20008
#define LOG_DEVICE_INFO_UNCOMPLETE_CYCLE             SAPDBErr_MessageList::Info,20009,"LogVolume not completely written until now",0
#define LOG_DEVICE_INFO_UNCOMPLETE_CYCLE_ID 20009
#define LOG_CHECK_RESULT                             SAPDBErr_MessageList::Info,20010,"Result after checking the log device: '%s'",1
#define LOG_CHECK_RESULT_ID 20010
#define LOG_ADD_DEVSPACE_PENDING                     SAPDBErr_MessageList::Info,20011,"LogVolume was added: adjust size from %s to %s",2
#define LOG_ADD_DEVSPACE_PENDING_ID 20011
#define LOG_ENTRY_INFO                               SAPDBErr_MessageList::Info,20012,"%s#%s:TR%s(%s)[%s]@%s.%s'%s':%s:%s",10
#define LOG_ENTRY_INFO_ID 20012
#define LOG_PAGE_REPAIRED                            SAPDBErr_MessageList::Info,20013,"partly written LogPage repaired off=%s, ioseq=%s",2
#define LOG_PAGE_REPAIRED_ID 20013
#define LOG_SAVEPOINT_FOUND                          SAPDBErr_MessageList::Info,20014,"savepoint-iosequence %s searched and found on log at offset %s",2
#define LOG_SAVEPOINT_FOUND_ID 20014
#define LOG_ABORTED_CLEAR_LOG                        SAPDBErr_MessageList::Warning,20015,"Log is cleared again (was aborted).",0
#define LOG_ABORTED_CLEAR_LOG_ID 20015
#define LOG_SAVE_FOR_ABORTED_CLEAR_LOG               SAPDBErr_MessageList::Warning,20016,"Log is tried to be saved although clear has been aborted.",0
#define LOG_SAVE_FOR_ABORTED_CLEAR_LOG_ID 20016
#define LOG_AUTOSAVE_ABORTED_HIST_LOST               SAPDBErr_MessageList::Warning,20017,"(AUTO)SAVE LOG has been aborted due to missing backup history",0
#define LOG_AUTOSAVE_ABORTED_HIST_LOST_ID 20017
#define LOG_SAVE_DENIED_AUTOOVERWRITE                SAPDBErr_MessageList::Warning,20018,"(AUTO)SAVE LOG has been aborted due to log-auto-overwrite",0
#define LOG_SAVE_DENIED_AUTOOVERWRITE_ID 20018
#define LOG_SAVE_DENIED_WRITING_OFF                  SAPDBErr_MessageList::Warning,20019,"(AUTO)SAVE LOG has been aborted due to switched off logwriting",0
#define LOG_SAVE_DENIED_WRITING_OFF_ID 20019
#define LOG_BACKUP_DISABLED                          SAPDBErr_MessageList::Warning,20020,"Log backups are disabled",0
#define LOG_BACKUP_DISABLED_ID 20020
#define LOG_PAGECHECK_FAILED                         SAPDBErr_MessageList::Warning,20021,"LogPage at offset %s ioseq %s-%s should be repaired later on",3
#define LOG_PAGECHECK_FAILED_ID 20021
#define LOG_LASTKNOWN_NOT_FOUND                      SAPDBErr_MessageList::Error,20022,"LastKnown not found on log",0
#define LOG_LASTKNOWN_NOT_FOUND_ID 20022
#define LOG_OLDEST_NOT_SAVED_NOT_FOUND               SAPDBErr_MessageList::Error,20023,"OldestNotSaved not found on log",0
#define LOG_OLDEST_NOT_SAVED_NOT_FOUND_ID 20023
#define LOG_SAVEPOINT_NOT_FOUND                      SAPDBErr_MessageList::Error,20024,"Savepoint not found on log",0
#define LOG_SAVEPOINT_NOT_FOUND_ID 20024
#define LOG_SAVEPOINT_IOSEQ_NOT_FOUND                SAPDBErr_MessageList::Error,20025,"Savepoint-iosequence %s searched but not found on log",1
#define LOG_SAVEPOINT_IOSEQ_NOT_FOUND_ID 20025
#define LOG_INCONSISTENT_SEQUENCE                    SAPDBErr_MessageList::Error,20026,"iosequences on log volume are inconsistent.",0
#define LOG_INCONSISTENT_SEQUENCE_ID 20026
#define LOG_CYCLE_NOT_FOUND                          SAPDBErr_MessageList::Error,20027,"no cycle found on log",0
#define LOG_CYCLE_NOT_FOUND_ID 20027
#define LOG_SAVEPOINT_SEQ_NOT_FOUND                  SAPDBErr_MessageList::Error,20028,"Savepoint iosequence not found on log",0
#define LOG_SAVEPOINT_SEQ_NOT_FOUND_ID 20028
#define LOG_DBIDENT_MISMATCH                         SAPDBErr_MessageList::Error,20029,"DBIdentifier of DataVolume (%s) and LogVolume (%s) does not match",2
#define LOG_DBIDENT_MISMATCH_ID 20029
#define LOG_HISTORY_LOST_NO_LOGSAVE_ALLOWED          SAPDBErr_MessageList::Error,20030,"Backup of Log not allowed. Devicestate is %s.",1
#define LOG_HISTORY_LOST_NO_LOGSAVE_ALLOWED_ID 20030
#define LOG_FREE_FAILED_PROT                         SAPDBErr_MessageList::Error,20031,"Free LogPages failed: No save to pipe happened before",0
#define LOG_FREE_FAILED_PROT_ID 20031
#define LOG_FREE_FAILED_IOSEQ                        SAPDBErr_MessageList::Error,20032,"Free LogPages failed: Free IOSeqRange %s...%s but OldstNotSaved=%s",3
#define LOG_FREE_FAILED_IOSEQ_ID 20032
#define LOG_FREEPIPE_FAILED_IOSEQ                    SAPDBErr_MessageList::Error,20033,"Free LogPages after save to pipe failed: Wrong IOSeq (%s!=%s or %s!=%s)",4
#define LOG_FREEPIPE_FAILED_IOSEQ_ID 20033
#define LOG_INVALID_LASTWRITABLE                     SAPDBErr_MessageList::Error,20034,"HotStandby: SetLastWritable: Offset not on LogVolume %s",1
#define LOG_INVALID_LASTWRITABLE_ID 20034
#define LOG_NOT_ENOUGH_PAGEFRAMES                    SAPDBErr_MessageList::Error,20035,"Allocation of pageframe failed",0
#define LOG_NOT_ENOUGH_PAGEFRAMES_ID 20035
#define LOG_REPAIR_PAGE_FAILED                       SAPDBErr_MessageList::Error,20036,"repair logpage failed at %s (prevIOSeq %s, nextIOSeq %s)",3
#define LOG_REPAIR_PAGE_FAILED_ID 20036
#define LOG_END_OF_LOG_EXPECTED                      SAPDBErr_MessageList::Error,20037,"repair logpage failed: END_OF_LOG expected from pageCheck",0
#define LOG_END_OF_LOG_EXPECTED_ID 20037
#define LOG_CONSISTENT_EXPECTED                      SAPDBErr_MessageList::Error,20038,"repair logpage failed: CONSISTENT expected from pageCheck",0
#define LOG_CONSISTENT_EXPECTED_ID 20038
#define LOG_ACTIONOBJECT_INFO                        SAPDBErr_MessageList::Error,20039,"%s: oid=%s.%s:%s bodysize=%s keysize=%s",6
#define LOG_ACTIONOBJECT_INFO_ID 20039
#define LOG_RESTORE_NEEDS_CLEARED_LOG                SAPDBErr_MessageList::Error,20040,"Logrecovery is not allowed, because state of log volume is 'HistoryLost' (log must be cleared)",0
#define LOG_RESTORE_NEEDS_CLEARED_LOG_ID 20040
#define LOG_SAVE_DISTANCE_TOO_SMALL                  SAPDBErr_MessageList::Error,20041,"Parameter LOG_SEGMENT_SIZE is greater then size of log area. %s < %s + %s(seg size)",3
#define LOG_SAVE_DISTANCE_TOO_SMALL_ID 20041
#define LOG_VOLUME_TOO_SMALL                         SAPDBErr_MessageList::Error,20042,"Parameter LOG_IO_QUEUE may be too large or size of log area too small. %s < %s + %s(reserved), reserved = #queues=%s * qsize=%s",5
#define LOG_VOLUME_TOO_SMALL_ID 20042
#define LOG_ACTION_FILE_INFO                         SAPDBErr_MessageList::Error,20043,"%s: action=%s filename='%s'",3
#define LOG_ACTION_FILE_INFO_ID 20043
#define LOG_NORMAL_END_FOUND                         SAPDBErr_MessageList::Info,20044,"normal end of log found at off %s lastseq %s.",2
#define LOG_NORMAL_END_FOUND_ID 20044
#define LOG_UNTIL_DATE_REACHED                       SAPDBErr_MessageList::Info,20045,"until date reached at %s,%s (off = %s, lastseq = %s)",4
#define LOG_UNTIL_DATE_REACHED_ID 20045
#define LOG_UNTIL_IOSEQ_REACHED                      SAPDBErr_MessageList::Info,20046,"until IOSeq %s reached (Offset %s, date %s/%s)",4
#define LOG_UNTIL_IOSEQ_REACHED_ID 20046
#define LOG_READ_QSEQ_INCONSISTENT                   SAPDBErr_MessageList::Error,20047,"read: offset %s expected queue sequence %s found %s",3
#define LOG_READ_QSEQ_INCONSISTENT_ID 20047
#define LOG_READ_IOSEQ_INCONSISTENT                  SAPDBErr_MessageList::Error,20048,"read: offset %s expected iosequence %s found %s",3
#define LOG_READ_IOSEQ_INCONSISTENT_ID 20048
#define LOG_READ_LAST_KNOWN_NOT_FOUND                SAPDBErr_MessageList::Error,20049,"read: offset %s expected last known iosequence %s found %s",3
#define LOG_READ_LAST_KNOWN_NOT_FOUND_ID 20049
#define LOG_READ_UNEXPECTED_ERROR                    SAPDBErr_MessageList::Error,20050,"LogIterator ISeqReader: expected last known iosequence %s found %s",2
#define LOG_READ_UNEXPECTED_ERROR_ID 20050
#define LOG_STANDBY_TOO_SLOW                         SAPDBErr_MessageList::Error,20051,"Standby reads log slower than master instance writes log or log is corrupted",0
#define LOG_STANDBY_TOO_SLOW_ID 20051
#define LOG_SAVEITER_INV_SEG_SIZE                    SAPDBErr_MessageList::Error,20052,"Log Segment Size is invalid (0)",0
#define LOG_SAVEITER_INV_SEG_SIZE_ID 20052
#define LOG_WRITER_USER_SUSPENDED                    SAPDBErr_MessageList::Info,20053,"LogWriter should be supended by user",0
#define LOG_WRITER_USER_SUSPENDED_ID 20053
#define LOG_WRITER_USER_RESUMED                      SAPDBErr_MessageList::Info,20054,"LogWriter resumed by user",0
#define LOG_WRITER_USER_RESUMED_ID 20054
#define LOG_WRITER_SUSPENDED_BY_USER                 SAPDBErr_MessageList::Info,20055,"LogWriter suspended by user with ioseq: %s",1
#define LOG_WRITER_SUSPENDED_BY_USER_ID 20055
#define LOG_DEVICE_FULL                              SAPDBErr_MessageList::Warning,20056,"LOG FULL: task %s suspended",1
#define LOG_DEVICE_FULL_ID 20056
#define LOG_QUEUE_SPACE_AVAILABLE                    SAPDBErr_MessageList::Error,20057,"Can not reserve %s (%s) bytes. Maximum is %s bytes.",3
#define LOG_QUEUE_SPACE_AVAILABLE_ID 20057
#define LOG_OPENTRANS_FILE_INFO                      SAPDBErr_MessageList::Info,20058,"%s svpid=%s",2
#define LOG_OPENTRANS_FILE_INFO_ID 20058
#define LOG_WAIT_FOR_REMOVE_HISTORY_CANCELLED        SAPDBErr_MessageList::Warning,20059,"History: removal of complete history is cancelled",0
#define LOG_WAIT_FOR_REMOVE_HISTORY_CANCELLED_ID 20059
#define LOG_DEL_UNDO_FILE                            SAPDBErr_MessageList::Info,20060,"%s: histfileroot=%s, trans=%s root=%s, last=%s",5
#define LOG_DEL_UNDO_FILE_ID 20060
#define LOG_EXISTING_HISTORY_FILES                   SAPDBErr_MessageList::Info,20061,"History: %s (%s) files existing",2
#define LOG_EXISTING_HISTORY_FILES_ID 20061
#define LOG_BEGIN_REMOVE_COMPLETE_HISTORY            SAPDBErr_MessageList::Info,20062,"History: begin removing complete history",0
#define LOG_BEGIN_REMOVE_COMPLETE_HISTORY_ID 20062
#define LOG_END_REMOVE_COMPLETE_HISTORY              SAPDBErr_MessageList::Info,20063,"History: end removing complete history",0
#define LOG_END_REMOVE_COMPLETE_HISTORY_ID 20063
#define LOG_HISTORY_FILE_REGISTERED                  SAPDBErr_MessageList::Info,20064,"History: all history files registered, GC is ready",0
#define LOG_HISTORY_FILE_REGISTERED_ID 20064
#define LOG_WAIT_FOR_REMOVE_HISTORY                  SAPDBErr_MessageList::Info,20065,"History: waiting for removal of complete history",0
#define LOG_WAIT_FOR_REMOVE_HISTORY_ID 20065
#define LOG_HIST_DIR_INFO                            SAPDBErr_MessageList::Info,20066,"%s: registered files %s, max used are %s",3
#define LOG_HIST_DIR_INFO_ID 20066
#define LOG_HISTDIR_RESIZE_FAILED                    SAPDBErr_MessageList::Info,20067,"Resize of HistoryDirectory failed fileno=%s, cap=%s",2
#define LOG_HISTDIR_RESIZE_FAILED_ID 20067
#define LOG_HISTORY_INVALID_FILENO                   SAPDBErr_MessageList::Info,20068,"invalid fileno %s (%s) was given",2
#define LOG_HISTORY_INVALID_FILENO_ID 20068
#define LOG_CONSISTENT_VIEW_INFO                     SAPDBErr_MessageList::Info,20069,"consistent view=%s",1
#define LOG_CONSISTENT_VIEW_INFO_ID 20069
#define LOG_PAGEREF_INFO                             SAPDBErr_MessageList::Info,20070,"%s %s.%s",3
#define LOG_PAGEREF_INFO_ID 20070
#define LOG_ACTION_RENAME_FILE_INFO                  SAPDBErr_MessageList::Info,20071,"%s: rename file old.'%s' -> new.'%s'(%s)",4
#define LOG_ACTION_RENAME_FILE_INFO_ID 20071
#define LOG_ILLEGAL_PAGEREF                          SAPDBErr_MessageList::Error,20072,"The given pageref is illegal. Either nil or unaligned.",0
#define LOG_ILLEGAL_PAGEREF_ID 20072
#define LOG_UNKNOWN_RESULT                           SAPDBErr_MessageList::Error,20073,"%s returned unknown result code %s",2
#define LOG_UNKNOWN_RESULT_ID 20073
#define LOG_WRONG_HISTORY                            SAPDBErr_MessageList::Error,20074,"The history entry found does not belong to given oid.",0
#define LOG_WRONG_HISTORY_ID 20074
#define LOG_REMOVE_FILE_FAILED                       SAPDBErr_MessageList::Error,20075,"Parts of a table could not be removed.",0
#define LOG_REMOVE_FILE_FAILED_ID 20075
#define LOG_INFO_VERIFY                              SAPDBErr_MessageList::Info,20076,"Verify of %s with root %s was successfull",2
#define LOG_INFO_VERIFY_ID 20076
#define LOG_INFO_VERIFY_OPENTRANS                    SAPDBErr_MessageList::Info,20077,"Verify: transaction %s found as open",1
#define LOG_INFO_VERIFY_OPENTRANS_ID 20077
#define LOG_TRANS_CONTEXT_HEAD                       SAPDBErr_MessageList::Info,20078,"Trans: #%s %s %s [%s] %s",5
#define LOG_TRANS_CONTEXT_HEAD_ID 20078
#define LOG_TRANS_CONTEXT_FILES                      SAPDBErr_MessageList::Info,20079,"Trans: %s, root:%s last:%s lastseq:%s (%s)",5
#define LOG_TRANS_CONTEXT_FILES_ID 20079
#define LOG_TRANS_CONTEXT_NO_REDOLOG                 SAPDBErr_MessageList::Info,20080,"Trans: Redo log writing is DISABLED.",0
#define LOG_TRANS_CONTEXT_NO_REDOLOG_ID 20080
#define LOG_TRANS_CONTEXT_SESSION                    SAPDBErr_MessageList::Info,20081,"Trans: Session: %s.%s.%s",3
#define LOG_TRANS_CONTEXT_SESSION_ID 20081
#define LOG_TRANS_CONTEXT_TASK_ENTRY                 SAPDBErr_MessageList::Info,20082,"Trans: T%s TRIDX:%s",2
#define LOG_TRANS_CONTEXT_TASK_ENTRY_ID 20082
#define LOG_LOCAL_REDO_BUFFER_SIZE_SET               SAPDBErr_MessageList::Info,20083,"Size of LocalRedoEntryBuffer/transactions: %s",1
#define LOG_LOCAL_REDO_BUFFER_SIZE_SET_ID 20083
#define LOG_ERROR_VERIFY                             SAPDBErr_MessageList::Error,20084,"Verify of %s with root %s gots error %s",3
#define LOG_ERROR_VERIFY_ID 20084
#define LOG_TRANS_ERROR_OCCURRED                     SAPDBErr_MessageList::Error,20085,"Trans %s: TrError = %s",2
#define LOG_TRANS_ERROR_OCCURRED_ID 20085
#define LOG_ACTION_NO_MIN_COPY_LENGTH                SAPDBErr_MessageList::Error,20086,"Action '%s' has no MinCopyLength",1
#define LOG_ACTION_NO_MIN_COPY_LENGTH_ID 20086
#define LOG_ALLOCATION_FAILED                        SAPDBErr_MessageList::Error,20087,"Allocation of '%s' failed.",1
#define LOG_ALLOCATION_FAILED_ID 20087
#define LOG_REDO_PARTIAL_ROLLBACK_FAILED             SAPDBErr_MessageList::Error,20088,"Partial rollback during redo failed with '%s'.",1
#define LOG_REDO_PARTIAL_ROLLBACK_FAILED_ID 20088
#define LOG_TRANS_INIT_NOT_CORRECT                   SAPDBErr_MessageList::Error,20089,"The Log_Trans context is not correctly initialzed for %s mode.",1
#define LOG_TRANS_INIT_NOT_CORRECT_ID 20089
#define LOG_INVALID_LOG_UNDO_SEQUENCE                SAPDBErr_MessageList::Error,20090,"PrevUndoSequence - 1 (%s) should be equal to CurrentUndoSeqence (%s)",2
#define LOG_INVALID_LOG_UNDO_SEQUENCE_ID 20090
#define LOG_REDO_WRITE_INVALID_ACTION                SAPDBErr_MessageList::Error,20091,"writing redo entry, types invalid: AfterImageType='%s', ActionType='%s'",2
#define LOG_REDO_WRITE_INVALID_ACTION_ID 20091
#define LOG_REDO_WRONG_TRANSNO                       SAPDBErr_MessageList::Error,20092,"T%s must not find entries to redo for T%s",2
#define LOG_REDO_WRONG_TRANSNO_ID 20092
#define LOG_UNDOPAGE_VERIFY                          SAPDBErr_MessageList::Error,20093,"PageNo: %s bottom: %s.",2
#define LOG_UNDOPAGE_VERIFY_ID 20093
#define LOG_GET_UNDO_ENTRY_MAXSIZE_MISMATCH          SAPDBErr_MessageList::Error,20094,"GetUndoEntry: maxsize %s is smaller then entrysize %s",2
#define LOG_GET_UNDO_ENTRY_MAXSIZE_MISMATCH_ID 20094
#define LOG_NEW_DBIDENT                              SAPDBErr_MessageList::Info,20095,"New DBIdentifier set (%s)",1
#define LOG_NEW_DBIDENT_ID 20095
#define LOG_FREE                                     SAPDBErr_MessageList::Error,20096,"FreeLog after backup of log failed: ioseq: %s->%s, off: %s->%s",4
#define LOG_FREE_ID 20096
#define LOG_NON_CONTINUOUS_IOSEQ                     SAPDBErr_MessageList::Error,20097,"IOSequences are not continuous: %s is not predecessor of %s",2
#define LOG_NON_CONTINUOUS_IOSEQ_ID 20097
#define LOG_READ_REDO_ENTRY_FAILED                   SAPDBErr_MessageList::Error,20098,"GetRedoEntry from Redo file failed.",0
#define LOG_READ_REDO_ENTRY_FAILED_ID 20098
#define LOG_UNEXPECTED_REDO_ENTRY_SEQUENCE           SAPDBErr_MessageList::Error,20099,"After Continue() found redo entry sequence %s expected %s",2
#define LOG_UNEXPECTED_REDO_ENTRY_SEQUENCE_ID 20099
#define LOG_READ_IMAGE_AFTER_CONTINUE_FAILED         SAPDBErr_MessageList::Error,20100,"After Continue() the image could not be read",0
#define LOG_READ_IMAGE_AFTER_CONTINUE_FAILED_ID 20100
#define LOG_ACTION_COULD_NOT_BE_READ                 SAPDBErr_MessageList::Error,20101,"Log action could not be read",0
#define LOG_ACTION_COULD_NOT_BE_READ_ID 20101
#define LOG_ACTION_FROM_WRONG_TRANSACTION            SAPDBErr_MessageList::Error,20102,"Action to process is from T%s but should from T%s",2
#define LOG_ACTION_FROM_WRONG_TRANSACTION_ID 20102
#define LOG_ACTION_IN_REDO_NOT_ALLOWED               SAPDBErr_MessageList::Error,20103,"Action '%s' for redo not allowed.",1
#define LOG_ACTION_IN_REDO_NOT_ALLOWED_ID 20103
#define LOG_ACTION_IN_UNDO_NOT_ALLOWED               SAPDBErr_MessageList::Error,20104,"Action '%s' for undo not allowed.",1
#define LOG_ACTION_IN_UNDO_NOT_ALLOWED_ID 20104
#define LOG_ACTION_UNKNOWN_FOR_REDO                  SAPDBErr_MessageList::Error,20105,"Action '%s' not known for redo.",1
#define LOG_ACTION_UNKNOWN_FOR_REDO_ID 20105
#define LOG_NOT_ALL_REDO_ENTRIES_REDONE              SAPDBErr_MessageList::Error,20106,"Trans::Redo Only %s entries of %s entries re-done.",2
#define LOG_NOT_ALL_REDO_ENTRIES_REDONE_ID 20106
#define LOG_ACTION_GET_FILE_FAILED                   SAPDBErr_MessageList::Error,20107,"FileDirectory GetFile results in error %s.",1
#define LOG_ACTION_GET_FILE_FAILED_ID 20107
#define LOG_ACTION_FILE_NOT_MARKED_DELETED           SAPDBErr_MessageList::Error,20108,"File '%s' must be marked as deleted, state is %s.",2
#define LOG_ACTION_FILE_NOT_MARKED_DELETED_ID 20108
#define LOG_ACTION_DESTROY_FILE_FAILED               SAPDBErr_MessageList::Error,20109,"Destroy file failed.",0
#define LOG_ACTION_DESTROY_FILE_FAILED_ID 20109
#define LOG_ACTION_UNQUALIFIED_DELETE_WRONG_FILETYPE SAPDBErr_MessageList::Error,20110,"File must be of type Table, is: %s.",1
#define LOG_ACTION_UNQUALIFIED_DELETE_WRONG_FILETYPE_ID 20110
#define LOG_ACTION_SETFILEDIRCOUNTER                 SAPDBErr_MessageList::Error,20111,"FileNo: '%s'.",1
#define LOG_ACTION_SETFILEDIRCOUNTER_ID 20111

#endif /* LOG_MESSAGES_HPP */
