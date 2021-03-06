/*
        ========== licence begin  GPL
        Copyright (c) 2006 SAP AG

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

#ifndef SDBMSG_LOG_H
#define SDBMSG_LOG_H
/*
This header file was generated by buildmsg.pl
--- Do not edit....
*/
#ifdef __cplusplus
#ifndef MSG_LIST_HPP
#include "SAPDB/Messages/Msg_List.hpp"
#endif
#ifndef MSG_ARG_HPP
#include "SAPDB/Messages/Msg_Arg.hpp"
#endif
#ifndef MSG_SIMPLEARG_HPP
#include "SAPDB/Messages/Msg_SimpleArg.hpp"
#endif
#endif

#ifndef SDB_RELEASE_070600  /* don't merge this define - only this date: 2006-02-21 12:51:18 */
#error SDBMsg_Log.h was probably integrated between releases, do not integrate this header file, \
use buildmsg
#endif


#define SDBMSG_LOG_COMPONENT "Log"
/* Begin of all message defines for component Log -> */
/* Begin of defines for message ALLOCATION_OF_SAVEPOINT_COORDINATOR_FAILED: ID = 1, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_LOG_ALLOCATION_OF_SAVEPOINT_COORDINATOR_FAILED 1, "Log", __FILE__, __LINE__, "Out of heap memory detected, while allocating savepoint coordinator.", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message ALLOCATION_OF_SAVEPOINT_COORDINATOR_FAILED */
/* Begin of defines for message TEMP_ROLLBACK_FAILED: ID = 2, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_LOG_TEMP_ROLLBACK_FAILED 2, "Log", __FILE__, __LINE__, "Error '$ERRORTEXT$' during rollback of temporary tables occured.", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_TEMP_ROLLBACK_FAILED__ERRORTEXT "ERRORTEXT"
/* <- End of defines for message TEMP_ROLLBACK_FAILED */
/* Begin of defines for message AFTERIMAGE_INFO: ID = 3, VERSION = 1, TAGS = 7 -> */
#define SDBMSG_LOG_AFTERIMAGE_INFO 3, "Log", __FILE__, __LINE__, "$TITLE$: type=$TYPE$/$VERSION$ isdone=$BOOL$ seq=$LOG_IO_SEQUENCE$ TR$TRANS_ID$", (Msg_Has7Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_AFTERIMAGE_INFO__BOOL "BOOL"
#define SDBMSGTAG_LOG_AFTERIMAGE_INFO__LOG_IO_SEQUENCE "LOG_IO_SEQUENCE"
#define SDBMSGTAG_LOG_AFTERIMAGE_INFO__TITLE "TITLE"
#define SDBMSGTAG_LOG_AFTERIMAGE_INFO__TRANS_ID "TRANS_ID"
#define SDBMSGTAG_LOG_AFTERIMAGE_INFO__TYPE "TYPE"
#define SDBMSGTAG_LOG_AFTERIMAGE_INFO__VERSION "VERSION"
/* <- End of defines for message AFTERIMAGE_INFO */
/* Begin of defines for message BEFORE_IMAGE_INFO: ID = 4, VERSION = 1, TAGS = 10 -> */
#define SDBMSG_LOG_BEFORE_IMAGE_INFO 4, "Log", __FILE__, __LINE__, "$TITLE$: type=$TYPE$/$VERSION$ state=$STATE$ CV$CONSISTENT_VIEW$ prevUpdT$PREV_UPD_TRANS_ID$ prevstate=$PREV_STATE$ histref=$PAGENO$.$PAGE_OFFSET$", (Msg_Has10Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_BEFORE_IMAGE_INFO__CONSISTENT_VIEW "CONSISTENT_VIEW"
#define SDBMSGTAG_LOG_BEFORE_IMAGE_INFO__PAGENO "PAGENO"
#define SDBMSGTAG_LOG_BEFORE_IMAGE_INFO__PAGE_OFFSET "PAGE_OFFSET"
#define SDBMSGTAG_LOG_BEFORE_IMAGE_INFO__PREV_STATE "PREV_STATE"
#define SDBMSGTAG_LOG_BEFORE_IMAGE_INFO__PREV_UPD_TRANS_ID "PREV_UPD_TRANS_ID"
#define SDBMSGTAG_LOG_BEFORE_IMAGE_INFO__STATE "STATE"
#define SDBMSGTAG_LOG_BEFORE_IMAGE_INFO__TITLE "TITLE"
#define SDBMSGTAG_LOG_BEFORE_IMAGE_INFO__TYPE "TYPE"
#define SDBMSGTAG_LOG_BEFORE_IMAGE_INFO__VERSION "VERSION"
/* <- End of defines for message BEFORE_IMAGE_INFO */
/* Begin of defines for message IACTION_INFO: ID = 6, VERSION = 1, TAGS = 8 -> */
#define SDBMSG_LOG_IACTION_INFO 6, "Log", __FILE__, __LINE__, "Action $ACTION_TYPE$.$ACTION_VERSION$ Length=$ACTION_LENGTH$($ACTION_MIN_LENGTH$) ForceWriting=$ACTION_FORCE_WRITE$ WaitForWrite=$ACTION_WAIT_FOR_WR$ LocalCacheAllowed=$ACTION_LOCAL_CACHE$", (Msg_Has8Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_IACTION_INFO__ACTION_TYPE "ACTION_TYPE"
#define SDBMSGTAG_LOG_IACTION_INFO__ACTION_VERSION "ACTION_VERSION"
#define SDBMSGTAG_LOG_IACTION_INFO__ACTION_FORCE_WRITE "ACTION_FORCE_WRITE"
#define SDBMSGTAG_LOG_IACTION_INFO__ACTION_WAIT_FOR_WR "ACTION_WAIT_FOR_WR"
#define SDBMSGTAG_LOG_IACTION_INFO__ACTION_LOCAL_CACHE "ACTION_LOCAL_CACHE"
#define SDBMSGTAG_LOG_IACTION_INFO__ACTION_LENGTH "ACTION_LENGTH"
#define SDBMSGTAG_LOG_IACTION_INFO__ACTION_MIN_LENGTH "ACTION_MIN_LENGTH"
/* <- End of defines for message IACTION_INFO */
/* Begin of defines for message IOSEQUENCE_OUT_OF_RANGE: ID = 7, VERSION = 1, TAGS = 4 -> */
#define SDBMSG_LOG_IOSEQUENCE_OUT_OF_RANGE 7, "Log", __FILE__, __LINE__, "IOSequence $CURRENT_IOSEQUENCE$ not in intervall of $FIRST_IOSEQUENCE$ - $LAST_IOSEQUENCE$.", (Msg_Has4Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_IOSEQUENCE_OUT_OF_RANGE__FIRST_IOSEQUENCE "FIRST_IOSEQUENCE"
#define SDBMSGTAG_LOG_IOSEQUENCE_OUT_OF_RANGE__CURRENT_IOSEQUENCE "CURRENT_IOSEQUENCE"
#define SDBMSGTAG_LOG_IOSEQUENCE_OUT_OF_RANGE__LAST_IOSEQUENCE "LAST_IOSEQUENCE"
/* <- End of defines for message IOSEQUENCE_OUT_OF_RANGE */
/* Begin of defines for message LOG_AND_DATA_INCOMPATIBLE: ID = 8, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_LOG_LOG_AND_DATA_INCOMPATIBLE 8, "Log", __FILE__, __LINE__, "Log- and Data are not compatible because '$REASON$'.", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_LOG_AND_DATA_INCOMPATIBLE__REASON "REASON"
/* <- End of defines for message LOG_AND_DATA_INCOMPATIBLE */
/* Begin of defines for message NUM_ACTIVE_QUEUES_INCREASED: ID = 9, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_LOG_NUM_ACTIVE_QUEUES_INCREASED 9, "Log", __FILE__, __LINE__, "The number of active logging-queues has been increased to $LOG_QUEUE_COUNT$", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_NUM_ACTIVE_QUEUES_INCREASED__LOG_QUEUE_COUNT "LOG_QUEUE_COUNT"
/* <- End of defines for message NUM_ACTIVE_QUEUES_INCREASED */
/* Begin of defines for message NUM_ACTIVE_QUEUES_DECREASED: ID = 10, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_LOG_NUM_ACTIVE_QUEUES_DECREASED 10, "Log", __FILE__, __LINE__, "The number of active logging-queues has been decreased to $LOG_QUEUE_COUNT$", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_NUM_ACTIVE_QUEUES_DECREASED__LOG_QUEUE_COUNT "LOG_QUEUE_COUNT"
/* <- End of defines for message NUM_ACTIVE_QUEUES_DECREASED */
/* Begin of defines for message OPEN_TRANS_IS_ROLLBACKED: ID = 11, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_LOG_OPEN_TRANS_IS_ROLLBACKED 11, "Log", __FILE__, __LINE__, "Transaction $TRANS_ID$ is open after log reading and forced to rollback $NUMBER$ entries.", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_OPEN_TRANS_IS_ROLLBACKED__TRANS_ID "TRANS_ID"
#define SDBMSGTAG_LOG_OPEN_TRANS_IS_ROLLBACKED__NUMBER "NUMBER"
/* <- End of defines for message OPEN_TRANS_IS_ROLLBACKED */
/* Begin of defines for message NO_SERVERTASK_FOR_SAVEPOINT: ID = 12, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_LOG_NO_SERVERTASK_FOR_SAVEPOINT 12, "Log", __FILE__, __LINE__, "No servertask for savepoint available.", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message NO_SERVERTASK_FOR_SAVEPOINT */
/* Begin of defines for message PAGE_INFO1: ID = 14, VERSION = 1, TAGS = 6 -> */
#define SDBMSG_LOG_PAGE_INFO1 14, "Log", __FILE__, __LINE__, "$TITLE$: LogPage  $FIRST_IOSEQUENCE$ -  $LAST_IOSEQUENCE$, Queue:$LOG_QUEUE_COUNT$. $LOG_IO_SEQUENCE$", (Msg_Has6Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_PAGE_INFO1__LOG_IO_SEQUENCE "LOG_IO_SEQUENCE"
#define SDBMSGTAG_LOG_PAGE_INFO1__TITLE "TITLE"
#define SDBMSGTAG_LOG_PAGE_INFO1__FIRST_IOSEQUENCE "FIRST_IOSEQUENCE"
#define SDBMSGTAG_LOG_PAGE_INFO1__LAST_IOSEQUENCE "LAST_IOSEQUENCE"
#define SDBMSGTAG_LOG_PAGE_INFO1__LOG_QUEUE_COUNT "LOG_QUEUE_COUNT"
/* <- End of defines for message PAGE_INFO1 */
/* Begin of defines for message PAGE_TYPE_INCORRECT: ID = 15, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_LOG_PAGE_TYPE_INCORRECT 15, "Log", __FILE__, __LINE__, "The pagetype is not correct. expected=$TYPE$, found= $UINT4$", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_PAGE_TYPE_INCORRECT__TYPE "TYPE"
#define SDBMSGTAG_LOG_PAGE_TYPE_INCORRECT__UINT4 "UINT4"
/* <- End of defines for message PAGE_TYPE_INCORRECT */
/* Begin of defines for message OFFSET_NO_ALIGNED: ID = 16, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_LOG_OFFSET_NO_ALIGNED 16, "Log", __FILE__, __LINE__, "Offset $OFFSET_FOUND$ of logentry found but is not properly aligned ($OFFSET_EXPECTED$).", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_OFFSET_NO_ALIGNED__OFFSET_FOUND "OFFSET_FOUND"
#define SDBMSGTAG_LOG_OFFSET_NO_ALIGNED__OFFSET_EXPECTED "OFFSET_EXPECTED"
/* <- End of defines for message OFFSET_NO_ALIGNED */
/* Begin of defines for message LOG_ENTRY_LENGTH_NOT_ALIGNED: ID = 17, VERSION = 1, TAGS = 4 -> */
#define SDBMSG_LOG_LOG_ENTRY_LENGTH_NOT_ALIGNED 17, "Log", __FILE__, __LINE__, "Entry #$ENTRY_COUNT$ has an illegal entry length  $NUMBER1$; expected:  $NUMBER2$", (Msg_Has4Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_LOG_ENTRY_LENGTH_NOT_ALIGNED__NUMBER1 "NUMBER1"
#define SDBMSGTAG_LOG_LOG_ENTRY_LENGTH_NOT_ALIGNED__NUMBER2 "NUMBER2"
#define SDBMSGTAG_LOG_LOG_ENTRY_LENGTH_NOT_ALIGNED__ENTRY_COUNT "ENTRY_COUNT"
/* <- End of defines for message LOG_ENTRY_LENGTH_NOT_ALIGNED */
/* Begin of defines for message SAVEPOINT_IOSEQ_TAPE_MISMATCH: ID = 18, VERSION = 1, TAGS = 4 -> */
#define SDBMSG_LOG_SAVEPOINT_IOSEQ_TAPE_MISMATCH 18, "Log", __FILE__, __LINE__, "The LogIOSequencenumber of the savepoint ($LOG_IO_SEQUENCE$) is not in the range from the backup [$FIRST_IOSEQUENCE$,$LAST_IOSEQUENCE$]", (Msg_Has4Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_SAVEPOINT_IOSEQ_TAPE_MISMATCH__LOG_IO_SEQUENCE "LOG_IO_SEQUENCE"
#define SDBMSGTAG_LOG_SAVEPOINT_IOSEQ_TAPE_MISMATCH__FIRST_IOSEQUENCE "FIRST_IOSEQUENCE"
#define SDBMSGTAG_LOG_SAVEPOINT_IOSEQ_TAPE_MISMATCH__LAST_IOSEQUENCE "LAST_IOSEQUENCE"
/* <- End of defines for message SAVEPOINT_IOSEQ_TAPE_MISMATCH */
/* Begin of defines for message MULTILOGQUEUE_NOT_ALLOWED: ID = 19, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_LOG_MULTILOGQUEUE_NOT_ALLOWED 19, "Log", __FILE__, __LINE__, "The parameter LOG_QUEUE_COUNT must not be greater than 1 if LOCAL_REDO_BUFFER_SIZE > 0", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_LOG_MULTILOGQUEUE_NOT_ALLOWED__LOG_QUEUE_COUNT "LOG_QUEUE_COUNT" /* optional */
#define SDBMSGTAG_LOG_MULTILOGQUEUE_NOT_ALLOWED__LOG_REDOBUFFERSIZE "LOG_REDOBUFFERSIZE" /* optional */
/* <- End of defines for message MULTILOGQUEUE_NOT_ALLOWED */
/* <- End of message defines for component Log */

/* some helpful enumerations (created by buildmsg.pl) -> */

#define SDBMSG_LOG_ENUMERATION \
                    Log_ALLOCATION_OF_SAVEPOINT_COORDINATOR_FAILED=1, \
                    Log_TEMP_ROLLBACK_FAILED=2, \
                    Log_AFTERIMAGE_INFO=3, \
                    Log_BEFORE_IMAGE_INFO=4, \
                    Log_IACTION_INFO=6, \
                    Log_IOSEQUENCE_OUT_OF_RANGE=7, \
                    Log_LOG_AND_DATA_INCOMPATIBLE=8, \
                    Log_NUM_ACTIVE_QUEUES_INCREASED=9, \
                    Log_NUM_ACTIVE_QUEUES_DECREASED=10, \
                    Log_OPEN_TRANS_IS_ROLLBACKED=11, \
                    Log_NO_SERVERTASK_FOR_SAVEPOINT=12, \
                    Log_PAGE_INFO1=14, \
                    Log_PAGE_TYPE_INCORRECT=15, \
                    Log_OFFSET_NO_ALIGNED=16, \
                    Log_LOG_ENTRY_LENGTH_NOT_ALIGNED=17, \
                    Log_SAVEPOINT_IOSEQ_TAPE_MISMATCH=18, \
                    Log_MULTILOGQUEUE_NOT_ALLOWED=19
#define SDBMSG_LOG_INITIALIZATIONLIST(x) \
                    x(SDBMSG_LOG_ALLOCATION_OF_SAVEPOINT_COORDINATOR_FAILED, "ALLOCATION_OF_SAVEPOINT_COORDINATOR_FAILED"), \
                    x(SDBMSG_LOG_TEMP_ROLLBACK_FAILED, "TEMP_ROLLBACK_FAILED"), \
                    x(SDBMSG_LOG_AFTERIMAGE_INFO, "AFTERIMAGE_INFO"), \
                    x(SDBMSG_LOG_BEFORE_IMAGE_INFO, "BEFORE_IMAGE_INFO"), \
                    x(SDBMSG_LOG_IACTION_INFO, "IACTION_INFO"), \
                    x(SDBMSG_LOG_IOSEQUENCE_OUT_OF_RANGE, "IOSEQUENCE_OUT_OF_RANGE"), \
                    x(SDBMSG_LOG_LOG_AND_DATA_INCOMPATIBLE, "LOG_AND_DATA_INCOMPATIBLE"), \
                    x(SDBMSG_LOG_NUM_ACTIVE_QUEUES_INCREASED, "NUM_ACTIVE_QUEUES_INCREASED"), \
                    x(SDBMSG_LOG_NUM_ACTIVE_QUEUES_DECREASED, "NUM_ACTIVE_QUEUES_DECREASED"), \
                    x(SDBMSG_LOG_OPEN_TRANS_IS_ROLLBACKED, "OPEN_TRANS_IS_ROLLBACKED"), \
                    x(SDBMSG_LOG_NO_SERVERTASK_FOR_SAVEPOINT, "NO_SERVERTASK_FOR_SAVEPOINT"), \
                    x(SDBMSG_LOG_PAGE_INFO1, "PAGE_INFO1"), \
                    x(SDBMSG_LOG_PAGE_TYPE_INCORRECT, "PAGE_TYPE_INCORRECT"), \
                    x(SDBMSG_LOG_OFFSET_NO_ALIGNED, "OFFSET_NO_ALIGNED"), \
                    x(SDBMSG_LOG_LOG_ENTRY_LENGTH_NOT_ALIGNED, "LOG_ENTRY_LENGTH_NOT_ALIGNED"), \
                    x(SDBMSG_LOG_SAVEPOINT_IOSEQ_TAPE_MISMATCH, "SAVEPOINT_IOSEQ_TAPE_MISMATCH"), \
                    x(SDBMSG_LOG_MULTILOGQUEUE_NOT_ALLOWED, "MULTILOGQUEUE_NOT_ALLOWED")
/* <- some helpful enumerations */

#endif /* SDBMSG_LOG_H */
