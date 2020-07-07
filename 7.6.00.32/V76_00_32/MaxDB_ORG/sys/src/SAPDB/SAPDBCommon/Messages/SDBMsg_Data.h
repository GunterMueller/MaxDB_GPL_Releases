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

#ifndef SDBMSG_DATA_H
#define SDBMSG_DATA_H
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

#ifndef SDB_RELEASE_070600  /* don't merge this define - only this date: 2006-02-22 17:21:17 */
#error SDBMsg_Data.h was probably integrated between releases, do not integrate this header file, \
use buildmsg
#endif


#define SDBMSG_DATA_COMPONENT "Data"
/* Begin of all message defines for component Data -> */
/* Begin of defines for message UPDSTAT_GETTABLESIZE_FAILED: ID = 1, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_DATA_UPDSTAT_GETTABLESIZE_FAILED 1, "Data", __FILE__, __LINE__, "Could not determine size of table $FILENO$", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_GETTABLESIZE_FAILED__FILENO "FILENO"
/* <- End of defines for message UPDSTAT_GETTABLESIZE_FAILED */
/* Begin of defines for message UPDSTAT_POSITION_ITERATOR_FAILED: ID = 2, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_DATA_UPDSTAT_POSITION_ITERATOR_FAILED 2, "Data", __FILE__, __LINE__, "Error while positioning iterator in request file $FILENO$; error $RETURNCODE$", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_POSITION_ITERATOR_FAILED__FILENO "FILENO"
#define SDBMSGTAG_DATA_UPDSTAT_POSITION_ITERATOR_FAILED__RETURNCODE "RETURNCODE"
/* <- End of defines for message UPDSTAT_POSITION_ITERATOR_FAILED */
/* Begin of defines for message UPDSTAT_UPDATERECORD_FAILED: ID = 3, VERSION = 1, TAGS = 4 -> */
#define SDBMSG_DATA_UPDSTAT_UPDATERECORD_FAILED 3, "Data", __FILE__, __LINE__, "Error while updating record in request file $FILENO$; key $KEYASHEX$; error $RETURNCODE$", (Msg_Has4Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_UPDATERECORD_FAILED__FILENO "FILENO"
#define SDBMSGTAG_DATA_UPDSTAT_UPDATERECORD_FAILED__KEYASHEX "KEYASHEX"
#define SDBMSGTAG_DATA_UPDSTAT_UPDATERECORD_FAILED__RETURNCODE "RETURNCODE"
/* <- End of defines for message UPDSTAT_UPDATERECORD_FAILED */
/* Begin of defines for message UPDSTAT_DELETE_ENTRY_FAILED: ID = 4, VERSION = 1, TAGS = 4 -> */
#define SDBMSG_DATA_UPDSTAT_DELETE_ENTRY_FAILED 4, "Data", __FILE__, __LINE__, "Error while deleting current entry in request file $FILENO$; key $KEYASHEX$; error $RETURNCODE$", (Msg_Has4Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_DELETE_ENTRY_FAILED__FILENO "FILENO"
#define SDBMSGTAG_DATA_UPDSTAT_DELETE_ENTRY_FAILED__KEYASHEX "KEYASHEX"
#define SDBMSGTAG_DATA_UPDSTAT_DELETE_ENTRY_FAILED__RETURNCODE "RETURNCODE"
/* <- End of defines for message UPDSTAT_DELETE_ENTRY_FAILED */
/* Begin of defines for message OUT_OF_MEMORY: ID = 5, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_DATA_OUT_OF_MEMORY 5, "Data", __FILE__, __LINE__, "Out of memory; requested memory in bytes $ALLOC_SIZE$", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_OUT_OF_MEMORY__ALLOC_SIZE "ALLOC_SIZE"
/* <- End of defines for message OUT_OF_MEMORY */
/* Begin of defines for message UPDSTAT_JOBLIST_ERROR: ID = 6, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_DATA_UPDSTAT_JOBLIST_ERROR 6, "Data", __FILE__, __LINE__, "Server tasklist returned unexpected status on wait; list size $SIZE$", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_JOBLIST_ERROR__SIZE "SIZE"
/* <- End of defines for message UPDSTAT_JOBLIST_ERROR */
/* Begin of defines for message UPDSTAT_UNEXPECTED_JOB_RESULT: ID = 7, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_DATA_UPDSTAT_UNEXPECTED_JOB_RESULT 7, "Data", __FILE__, __LINE__, "Server job returned an unexpected result code $RETURNCODE$", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_UNEXPECTED_JOB_RESULT__RETURNCODE "RETURNCODE"
/* <- End of defines for message UPDSTAT_UNEXPECTED_JOB_RESULT */
/* Begin of defines for message NOT_INITIALIZED: ID = 8, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_DATA_NOT_INITIALIZED 8, "Data", __FILE__, __LINE__, "Object not initialized", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message NOT_INITIALIZED */
/* Begin of defines for message UPDSTAT_ABORTING: ID = 9, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_DATA_UPDSTAT_ABORTING 9, "Data", __FILE__, __LINE__, "Aborting statistics computation due to system error", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message UPDSTAT_ABORTING */
/* Begin of defines for message UPDSTAT_OLD_FILEVERSION: ID = 10, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_DATA_UPDSTAT_OLD_FILEVERSION 10, "Data", __FILE__, __LINE__, "Update Stat for table $FILENO$ aborted due to metadata change", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_OLD_FILEVERSION__FILENO "FILENO"
/* <- End of defines for message UPDSTAT_OLD_FILEVERSION */
/* Begin of defines for message UPDSTAT_CREATE_DISTINCT_FILE_FAILED: ID = 11, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_DATA_UPDSTAT_CREATE_DISTINCT_FILE_FAILED 11, "Data", __FILE__, __LINE__, "Error during distinct file creation for table $FILENO$, error = $RETURNCODE$", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_CREATE_DISTINCT_FILE_FAILED__FILENO "FILENO"
#define SDBMSGTAG_DATA_UPDSTAT_CREATE_DISTINCT_FILE_FAILED__RETURNCODE "RETURNCODE"
/* <- End of defines for message UPDSTAT_CREATE_DISTINCT_FILE_FAILED */
/* Begin of defines for message UPDSTAT_GET_LOCK_TABLE_FAILED: ID = 12, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_DATA_UPDSTAT_GET_LOCK_TABLE_FAILED 12, "Data", __FILE__, __LINE__, "Error during locking of table $FILENO$", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_GET_LOCK_TABLE_FAILED__FILENO "FILENO"
/* <- End of defines for message UPDSTAT_GET_LOCK_TABLE_FAILED */
/* Begin of defines for message UPDSTAT_SELF_PROCESS_JOB: ID = 13, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_DATA_UPDSTAT_SELF_PROCESS_JOB 13, "Data", __FILE__, __LINE__, "Parallel 'Update Statistics' coordinator is processing table $FILENO$ itself", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_SELF_PROCESS_JOB__FILENO "FILENO"
/* <- End of defines for message UPDSTAT_SELF_PROCESS_JOB */
/* Begin of defines for message UPDSTAT_CREATE_TRANS_FAILED: ID = 14, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_DATA_UPDSTAT_CREATE_TRANS_FAILED 14, "Data", __FILE__, __LINE__, "Could not create internal transaction for table $FILENO$", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_CREATE_TRANS_FAILED__FILENO "FILENO"
/* <- End of defines for message UPDSTAT_CREATE_TRANS_FAILED */
/* Begin of defines for message UPDSTAT_INVALID_JOBID: ID = 15, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_DATA_UPDSTAT_INVALID_JOBID 15, "Data", __FILE__, __LINE__, "Job id $JOB_ID$ not in run queue; queue size =  $SIZE$", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_INVALID_JOBID__JOB_ID "JOB_ID"
#define SDBMSGTAG_DATA_UPDSTAT_INVALID_JOBID__SIZE "SIZE"
/* <- End of defines for message UPDSTAT_INVALID_JOBID */
/* Begin of defines for message UPDSTAT_TABLEINFO_NOT_IN_LIST: ID = 16, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_DATA_UPDSTAT_TABLEINFO_NOT_IN_LIST 16, "Data", __FILE__, __LINE__, "TableInfo not in job queue for table with fileNo $FILENO$", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_TABLEINFO_NOT_IN_LIST__FILENO "FILENO"
/* <- End of defines for message UPDSTAT_TABLEINFO_NOT_IN_LIST */
/* Begin of defines for message UPDSTAT_RESOURCE_SHORTAGE: ID = 17, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_DATA_UPDSTAT_RESOURCE_SHORTAGE 17, "Data", __FILE__, __LINE__, "Serializing  statistics computation due to resource shortage", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message UPDSTAT_RESOURCE_SHORTAGE */
/* Begin of defines for message UPDSTAT_OUT_OF_RESOURCES: ID = 18, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_DATA_UPDSTAT_OUT_OF_RESOURCES 18, "Data", __FILE__, __LINE__, "Aborting statistics computation due to resource shortage", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message UPDSTAT_OUT_OF_RESOURCES */
/* Begin of defines for message UPDSTAT_OUT_OF_MEMORY: ID = 19, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_DATA_UPDSTAT_OUT_OF_MEMORY 19, "Data", __FILE__, __LINE__, "Could not allocate $ALLOC_SIZE$ bytes, trying to go on", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDSTAT_OUT_OF_MEMORY__ALLOC_SIZE "ALLOC_SIZE"
/* <- End of defines for message UPDSTAT_OUT_OF_MEMORY */
/* Begin of defines for message FDIR_COUNTER_MISMATCH: ID = 20, VERSION = 2, TAGS = 5 -> */
#define SDBMSG_DATA_FDIR_COUNTER_MISMATCH 20, "Data", __FILE__, __LINE__, "Updated '$TITLE$' from $FDIR_PAGE_COUNT$ to $COUNTED_PAGES$ for root $PAGENO$", (Msg_Has5Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("2"))
#define SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__PAGENO "PAGENO"
#define SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__TITLE "TITLE"
#define SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__FDIR_PAGE_COUNT "FDIR_PAGE_COUNT"
#define SDBMSGTAG_DATA_FDIR_COUNTER_MISMATCH__COUNTED_PAGES "COUNTED_PAGES"
/* <- End of defines for message FDIR_COUNTER_MISMATCH */
/* Begin of defines for message UPDCNT_COLLECTION_SYSTEM_ERROR: ID = 21, VERSION = 1, TAGS = 3 -> */
#define SDBMSG_DATA_UPDCNT_COLLECTION_SYSTEM_ERROR 21, "Data", __FILE__, __LINE__, "Unexpected error during file directory counter collection: $BASIS_ERROR$", (Msg_Has3Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_UPDCNT_COLLECTION_SYSTEM_ERROR__BASIS_ERROR "BASIS_ERROR"
#define SDBMSGTAG_DATA_UPDCNT_COLLECTION_SYSTEM_ERROR__FILENO "FILENO"
/* <- End of defines for message UPDCNT_COLLECTION_SYSTEM_ERROR */
/* Begin of defines for message DAMAGED_DATA_RECORD_FOUND_1: ID = 22, VERSION = 1, TAGS = 6 -> */
#define SDBMSG_DATA_DAMAGED_DATA_RECORD_FOUND_1 22, "Data", __FILE__, __LINE__, "Damaged data record found", (Msg_Has6Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__FILENO "FILENO"
#define SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__FILETYPE "FILETYPE"
#define SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__PAGENO "PAGENO"
#define SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__ROOT "ROOT"
#define SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__PERSISTENT_TYPE "PERSISTENT_TYPE"
#define SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__RECORD_POSITION "RECORD_POSITION" /* optional */
#define SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__RECORD_INDEX "RECORD_INDEX" /* optional */
#define SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__INTERNAL_FILENAME "INTERNAL_FILENAME" /* optional */
#define SDBMSGTAG_DATA_DAMAGED_DATA_RECORD_FOUND_1__KNL_BASE_ERROR "KNL_BASE_ERROR" /* optional */
/* <- End of defines for message DAMAGED_DATA_RECORD_FOUND_1 */
/* Begin of defines for message UDF_NOT_PACKED_TABLES: ID = 23, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_DATA_UDF_NOT_PACKED_TABLES 23, "Data", __FILE__, __LINE__, "User defined functions are not supported for packed tables", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message UDF_NOT_PACKED_TABLES */
/* Begin of defines for message TABLE_SCAN_CANCELED: ID = 24, VERSION = 1, TAGS = 2 -> */
#define SDBMSG_DATA_TABLE_SCAN_CANCELED 24, "Data", __FILE__, __LINE__, "Range scan on table with FileID '$FILENO$ ' canceled", (Msg_Has2Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
#define SDBMSGTAG_DATA_TABLE_SCAN_CANCELED__FILENO "FILENO"
/* <- End of defines for message TABLE_SCAN_CANCELED */
/* Begin of defines for message CHECK_LOB_CANCELED: ID = 25, VERSION = 1, TAGS = 1 -> */
#define SDBMSG_DATA_CHECK_LOB_CANCELED 25, "Data", __FILE__, __LINE__, "Check LOB canceled", (Msg_Has1Args*)0, Msg_SimpleArg("_MESSAGEVERSION", reinterpret_cast<const SAPDB_UTF8*>("1"))
/* <- End of defines for message CHECK_LOB_CANCELED */
/* <- End of message defines for component Data */

/* some helpful enumerations (created by buildmsg.pl) -> */

#define SDBMSG_DATA_ENUMERATION \
                    Data_UPDSTAT_GETTABLESIZE_FAILED=1, \
                    Data_UPDSTAT_POSITION_ITERATOR_FAILED=2, \
                    Data_UPDSTAT_UPDATERECORD_FAILED=3, \
                    Data_UPDSTAT_DELETE_ENTRY_FAILED=4, \
                    Data_OUT_OF_MEMORY=5, \
                    Data_UPDSTAT_JOBLIST_ERROR=6, \
                    Data_UPDSTAT_UNEXPECTED_JOB_RESULT=7, \
                    Data_NOT_INITIALIZED=8, \
                    Data_UPDSTAT_ABORTING=9, \
                    Data_UPDSTAT_OLD_FILEVERSION=10, \
                    Data_UPDSTAT_CREATE_DISTINCT_FILE_FAILED=11, \
                    Data_UPDSTAT_GET_LOCK_TABLE_FAILED=12, \
                    Data_UPDSTAT_SELF_PROCESS_JOB=13, \
                    Data_UPDSTAT_CREATE_TRANS_FAILED=14, \
                    Data_UPDSTAT_INVALID_JOBID=15, \
                    Data_UPDSTAT_TABLEINFO_NOT_IN_LIST=16, \
                    Data_UPDSTAT_RESOURCE_SHORTAGE=17, \
                    Data_UPDSTAT_OUT_OF_RESOURCES=18, \
                    Data_UPDSTAT_OUT_OF_MEMORY=19, \
                    Data_FDIR_COUNTER_MISMATCH=20, \
                    Data_UPDCNT_COLLECTION_SYSTEM_ERROR=21, \
                    Data_DAMAGED_DATA_RECORD_FOUND_1=22, \
                    Data_UDF_NOT_PACKED_TABLES=23, \
                    Data_TABLE_SCAN_CANCELED=24, \
                    Data_CHECK_LOB_CANCELED=25
#define SDBMSG_DATA_INITIALIZATIONLIST(x) \
                    x(SDBMSG_DATA_UPDSTAT_GETTABLESIZE_FAILED, "UPDSTAT_GETTABLESIZE_FAILED"), \
                    x(SDBMSG_DATA_UPDSTAT_POSITION_ITERATOR_FAILED, "UPDSTAT_POSITION_ITERATOR_FAILED"), \
                    x(SDBMSG_DATA_UPDSTAT_UPDATERECORD_FAILED, "UPDSTAT_UPDATERECORD_FAILED"), \
                    x(SDBMSG_DATA_UPDSTAT_DELETE_ENTRY_FAILED, "UPDSTAT_DELETE_ENTRY_FAILED"), \
                    x(SDBMSG_DATA_OUT_OF_MEMORY, "OUT_OF_MEMORY"), \
                    x(SDBMSG_DATA_UPDSTAT_JOBLIST_ERROR, "UPDSTAT_JOBLIST_ERROR"), \
                    x(SDBMSG_DATA_UPDSTAT_UNEXPECTED_JOB_RESULT, "UPDSTAT_UNEXPECTED_JOB_RESULT"), \
                    x(SDBMSG_DATA_NOT_INITIALIZED, "NOT_INITIALIZED"), \
                    x(SDBMSG_DATA_UPDSTAT_ABORTING, "UPDSTAT_ABORTING"), \
                    x(SDBMSG_DATA_UPDSTAT_OLD_FILEVERSION, "UPDSTAT_OLD_FILEVERSION"), \
                    x(SDBMSG_DATA_UPDSTAT_CREATE_DISTINCT_FILE_FAILED, "UPDSTAT_CREATE_DISTINCT_FILE_FAILED"), \
                    x(SDBMSG_DATA_UPDSTAT_GET_LOCK_TABLE_FAILED, "UPDSTAT_GET_LOCK_TABLE_FAILED"), \
                    x(SDBMSG_DATA_UPDSTAT_SELF_PROCESS_JOB, "UPDSTAT_SELF_PROCESS_JOB"), \
                    x(SDBMSG_DATA_UPDSTAT_CREATE_TRANS_FAILED, "UPDSTAT_CREATE_TRANS_FAILED"), \
                    x(SDBMSG_DATA_UPDSTAT_INVALID_JOBID, "UPDSTAT_INVALID_JOBID"), \
                    x(SDBMSG_DATA_UPDSTAT_TABLEINFO_NOT_IN_LIST, "UPDSTAT_TABLEINFO_NOT_IN_LIST"), \
                    x(SDBMSG_DATA_UPDSTAT_RESOURCE_SHORTAGE, "UPDSTAT_RESOURCE_SHORTAGE"), \
                    x(SDBMSG_DATA_UPDSTAT_OUT_OF_RESOURCES, "UPDSTAT_OUT_OF_RESOURCES"), \
                    x(SDBMSG_DATA_UPDSTAT_OUT_OF_MEMORY, "UPDSTAT_OUT_OF_MEMORY"), \
                    x(SDBMSG_DATA_FDIR_COUNTER_MISMATCH, "FDIR_COUNTER_MISMATCH"), \
                    x(SDBMSG_DATA_UPDCNT_COLLECTION_SYSTEM_ERROR, "UPDCNT_COLLECTION_SYSTEM_ERROR"), \
                    x(SDBMSG_DATA_DAMAGED_DATA_RECORD_FOUND_1, "DAMAGED_DATA_RECORD_FOUND_1"), \
                    x(SDBMSG_DATA_UDF_NOT_PACKED_TABLES, "UDF_NOT_PACKED_TABLES"), \
                    x(SDBMSG_DATA_TABLE_SCAN_CANCELED, "TABLE_SCAN_CANCELED"), \
                    x(SDBMSG_DATA_CHECK_LOB_CANCELED, "CHECK_LOB_CANCELED")
/* <- some helpful enumerations */

#endif /* SDBMSG_DATA_H */
