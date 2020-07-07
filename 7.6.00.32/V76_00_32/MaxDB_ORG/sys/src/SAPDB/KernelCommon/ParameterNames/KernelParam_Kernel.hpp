/* @lastChanged: "2004-03-30"

  * @filename:   vgg011.cpp
  * @purpose:    "GG_param_functions"
  * @release:    7.6.0.0
  * @see:        ""
  *
  * @Copyright (c) 1999-2005 SAP AG"


    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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


#define KERNELPARAM_CALLSTACKLEVEL                   (RTEConf_Parameter::Name)"CALLSTACKLEVEL"
#define KERNELPARAM_CHECK_BACKUP                     (RTEConf_Parameter::Name)"CHECK_BACKUP"
#define KERNELPARAM_CHECK_DATACACHE                  (RTEConf_Parameter::Name)"CHECK_DATACACHE"
#define KERNELPARAM_CHECK_KB_REGIONS                 (RTEConf_Parameter::Name)"CHECK_KB_REGIONS"
#define KERNELPARAM_CHECK_LOCK                       (RTEConf_Parameter::Name)"CHECK_LOCK"
#define KERNELPARAM_CHECK_LOCK_SUPPLY                (RTEConf_Parameter::Name)"CHECK_LOCK_SUPPLY"
#define KERNELPARAM_CHECK_REGIONS                    (RTEConf_Parameter::Name)"CHECK_REGIONS"
#define KERNELPARAM_CHECK_TABLE_WIDTH                (RTEConf_Parameter::Name)"CHECK_TABLE_WIDTH"
#define KERNELPARAM_CHECK_TASK_SPECIFIC_CATALOGCACHE (RTEConf_Parameter::Name)"CHECK_TASK_SPECIFIC_CATALOGCACHE"
#define KERNELPARAM_CHECK_TRANSLIST                  (RTEConf_Parameter::Name)"CHECK_TRANSLIST"
#define KERNELPARAM_CHECK_TREE                       (RTEConf_Parameter::Name)"CHECK_TREE"
#define KERNELPARAM_CHECK_TREE_LOCKS                 (RTEConf_Parameter::Name)"CHECK_TREE_LOCKS"
#define KERNELPARAM_CLUSTERED_LOBS                   (RTEConf_Parameter::Name)"CLUSTERED_LOBS"
#define KERNELPARAM_COLUMNCOMPRESSION                (RTEConf_Parameter::Name)"COLUMNCOMPRESSION"
#define KERNELPARAM_CONVERTER_REGIONS                (RTEConf_Parameter::Name)"CONVERTER_REGIONS"
#define KERNELPARAM_DATE_TIME_FORMAT                 (RTEConf_Parameter::Name)"DATE_TIME_FORMAT"
#define KERNELPARAM_DDLTRIGGER                       (RTEConf_Parameter::Name)"DDLTRIGGER"
#define KERNELPARAM_DEADLOCK_DETECTION               (RTEConf_Parameter::Name)"DEADLOCK_DETECTION"
#define KERNELPARAM_DEFAULT_CODE                     (RTEConf_Parameter::Name)"DEFAULT_CODE"
#define KERNELPARAM_ENABLE_SYSTEM_TRIGGERS           (RTEConf_Parameter::Name)"ENABLE_SYSTEM_TRIGGERS"
#define KERNELPARAM_HASHED_RESULTSET                 (RTEConf_Parameter::Name)"HASHED_RESULTSET"
#define KERNELPARAM_HASHED_RESULTSET_CACHESIZE       (RTEConf_Parameter::Name)"HASHED_RESULTSET_CACHESIZE"
#define KERNELPARAM_LOG_BACKUP_TO_PIPE               (RTEConf_Parameter::Name)"LOG_BACKUP_TO_PIPE"
#define KERNELPARAM_LOG_SEGMENT_SIZE                 (RTEConf_Parameter::Name)"LOG_SEGMENT_SIZE"
#define KERNELPARAM_LRU_FOR_SCAN                     (RTEConf_Parameter::Name)"LRU_FOR_SCAN"
#define KERNELPARAM_MAX_HASHTABLE_MEMORY             (RTEConf_Parameter::Name)"MAX_HASHTABLE_MEMORY"
#define KERNELPARAM_MAX_SINGLE_HASHTABLE_SIZE        (RTEConf_Parameter::Name)"MAX_SINGLE_HASHTABLE_SIZE"
#define KERNELPARAM_MAXLOCKS                         (RTEConf_Parameter::Name)"MAXLOCKS"
#define KERNELPARAM_MAX_RETENTION_TIME               (RTEConf_Parameter::Name)"MAX_RETENTION_TIME"
#define KERNELPARAM_MIN_RETENTION_TIME               (RTEConf_Parameter::Name)"MIN_RETENTION_TIME"
#define KERNELPARAM_MONITOR_READ                     (RTEConf_Parameter::Name)"MONITOR_READ"
#define KERNELPARAM_MONITOR_ROWNO                    (RTEConf_Parameter::Name)"MONITOR_ROWNO"
#define KERNELPARAM_MONITOR_SELECTIVITY              (RTEConf_Parameter::Name)"MONITOR_SELECTIVITY"
#define KERNELPARAM_MONITOR_TIME                     (RTEConf_Parameter::Name)"MONITOR_TIME"
#define KERNELPARAM_OMS_HEAP_BLOCKSIZE               (RTEConf_Parameter::Name)"OMS_HEAP_BLOCKSIZE"
#define KERNELPARAM_OMS_HEAP_COUNT                   (RTEConf_Parameter::Name)"OMS_HEAP_COUNT"
#define KERNELPARAM_OMS_HEAP_THRESHOLD               (RTEConf_Parameter::Name)"OMS_HEAP_THRESHOLD"
#define KERNELPARAM_OMS_STREAM_TIMEOUT               (RTEConf_Parameter::Name)"OMS_STREAM_TIMEOUT"
#define KERNELPARAM_OMS_VERS_THRESHOLD               (RTEConf_Parameter::Name)"OMS_VERS_THRESHOLD"
#define KERNELPARAM_OPTIMIZE_AGGREGATION             (RTEConf_Parameter::Name)"OPTIMIZE_AGGREGATION"
#define KERNELPARAM_OPTIMIZE_FETCH_REVERSE           (RTEConf_Parameter::Name)"OPTIMIZE_FETCH_REVERSE"
#define KERNELPARAM_OPTIMIZE_MIN_MAX                 (RTEConf_Parameter::Name)"OPTIMIZE_MIN_MAX"
#define KERNELPARAM_OPTIMIZE_FIRST_ROWS              (RTEConf_Parameter::Name)"OPTIMIZE_FIRST_ROWS"
#define KERNELPARAM_OPTIMIZE_QUAL_ON_INDEX           (RTEConf_Parameter::Name)"OPTIMIZE_QUAL_ON_INDEX"
#define KERNELPARAM_OPTIM_CACHE                      (RTEConf_Parameter::Name)"OPTIM_CACHE"
#define KERNELPARAM_OPTIM_INV_ONLY                   (RTEConf_Parameter::Name)"OPTIM_INV_ONLY"
#define KERNELPARAM_OPTIM_MAX_MERGE                  (RTEConf_Parameter::Name)"OPTIM_MAX_MERGE"
#define KERNELPARAM_REQUEST_TIMEOUT                  (RTEConf_Parameter::Name)"REQUEST_TIMEOUT"
#define KERNELPARAM_SEQUENCE_CACHE                   (RTEConf_Parameter::Name)"SEQUENCE_CACHE"
#define KERNELPARAM_SESSION_TIMEOUT                  (RTEConf_Parameter::Name)"SESSION_TIMEOUT"
#define KERNELPARAM_SHOW_MAX_KB_STACK_USE            (RTEConf_Parameter::Name)"SHOW_MAX_KB_STACK_USE"
#define KERNELPARAM_SUBTREE_LOCKS                    (RTEConf_Parameter::Name)"SUBTREE_LOCKS"
#define KERNELPARAM_TRACE_AK                         (RTEConf_Parameter::Name)"TRACE_AK"
#define KERNELPARAM_TRACE_CONSOLE                    (RTEConf_Parameter::Name)"TRACE_CONSOLE"
#define KERNELPARAM_TRACE_DEFAULT                    (RTEConf_Parameter::Name)"TRACE_DEFAULT"
#define KERNELPARAM_TRACE_DELETE                     (RTEConf_Parameter::Name)"TRACE_DELETE"
#define KERNELPARAM_TRACE_INDEX                      (RTEConf_Parameter::Name)"TRACE_INDEX"
#define KERNELPARAM_TRACE_INSERT                     (RTEConf_Parameter::Name)"TRACE_INSERT"
#define KERNELPARAM_TRACE_LOCK                       (RTEConf_Parameter::Name)"TRACE_LOCK"
#define KERNELPARAM_TRACE_LONG                       (RTEConf_Parameter::Name)"TRACE_LONG"
#define KERNELPARAM_TRACE_OBJECT                     (RTEConf_Parameter::Name)"TRACE_OBJECT"
#define KERNELPARAM_TRACE_OBJECT_ADD                 (RTEConf_Parameter::Name)"TRACE_OBJECT_ADD"
#define KERNELPARAM_TRACE_OBJECT_ALTER               (RTEConf_Parameter::Name)"TRACE_OBJECT_ALTER"
#define KERNELPARAM_TRACE_OBJECT_FREE                (RTEConf_Parameter::Name)"TRACE_OBJECT_FREE"
#define KERNELPARAM_TRACE_OBJECT_GET                 (RTEConf_Parameter::Name)"TRACE_OBJECT_GET"
#define KERNELPARAM_TRACE_OMS_CONTAINERDIR           (RTEConf_Parameter::Name)"TRACE_OMS_CONTAINERDIR"
#define KERNELPARAM_TRACE_OMS_CONTEXT                (RTEConf_Parameter::Name)"TRACE_OMS_CONTEXT"
#define KERNELPARAM_TRACE_OMS_ERROR                  (RTEConf_Parameter::Name)"TRACE_OMS_ERROR"
#define KERNELPARAM_TRACE_OMS_FLUSHCACHE             (RTEConf_Parameter::Name)"TRACE_OMS_FLUSHCACHE"
#define KERNELPARAM_TRACE_OMS_INTERFACE              (RTEConf_Parameter::Name)"TRACE_OMS_INTERFACE"
#define KERNELPARAM_TRACE_OMS_KEY                    (RTEConf_Parameter::Name)"TRACE_OMS_KEY"
#define KERNELPARAM_TRACE_OMS_KEYRANGE               (RTEConf_Parameter::Name)"TRACE_OMS_KEYRANGE"
#define KERNELPARAM_TRACE_OMS_LOCK                   (RTEConf_Parameter::Name)"TRACE_OMS_LOCK"
#define KERNELPARAM_TRACE_OMS_MEMORY                 (RTEConf_Parameter::Name)"TRACE_OMS_MEMORY"
#define KERNELPARAM_TRACE_OMS_NEWOBJ                 (RTEConf_Parameter::Name)"TRACE_OMS_NEWOBJ"
#define KERNELPARAM_TRACE_OMS_SESSION                (RTEConf_Parameter::Name)"TRACE_OMS_SESSION"
#define KERNELPARAM_TRACE_OMS_STREAM                 (RTEConf_Parameter::Name)"TRACE_OMS_STREAM"
#define KERNELPARAM_TRACE_OMS_VAROBJECT              (RTEConf_Parameter::Name)"TRACE_OMS_VAROBJECT"
#define KERNELPARAM_TRACE_OMS_VERSION                (RTEConf_Parameter::Name)"TRACE_OMS_VERSION"
#define KERNELPARAM_TRACE_OPTIMIZE                   (RTEConf_Parameter::Name)"TRACE_OPTIMIZE"
#define KERNELPARAM_TRACE_ORDER                      (RTEConf_Parameter::Name)"TRACE_ORDER"
#define KERNELPARAM_TRACE_ORDER_STANDARD             (RTEConf_Parameter::Name)"TRACE_ORDER_STANDARD"
#define KERNELPARAM_TRACE_PAGES                      (RTEConf_Parameter::Name)"TRACE_PAGES"
#define KERNELPARAM_TRACE_PRIMARY_TREE               (RTEConf_Parameter::Name)"TRACE_PRIMARY_TREE"
#define KERNELPARAM_TRACE_SELECT                     (RTEConf_Parameter::Name)"TRACE_SELECT"
#define KERNELPARAM_TRACE_STOP_ERRORCODE             (RTEConf_Parameter::Name)"TRACE_STOP_ERRORCODE"
#define KERNELPARAM_TRACE_TIME                       (RTEConf_Parameter::Name)"TRACE_TIME"
#define KERNELPARAM_TRACE_UPDATE                     (RTEConf_Parameter::Name)"TRACE_UPDATE"
#define KERNELPARAM__CAT_CACHE_MINSIZE               (RTEConf_Parameter::Name)"_CAT_CACHE_MINSIZE"
#define KERNELPARAM__DW_IO_AREA_FLUSH                (RTEConf_Parameter::Name)"_DW_IO_AREA_FLUSH"
#define KERNELPARAM__DW_IO_AREA_SIZE                 (RTEConf_Parameter::Name)"_DW_IO_AREA_SIZE"
#define KERNELPARAM__DW_LRU_TAIL_FLUSH               (RTEConf_Parameter::Name)"_DW_LRU_TAIL_FLUSH"
#define KERNELPARAM__IDXFILE_LIST_SIZE               (RTEConf_Parameter::Name)"_IDXFILE_LIST_SIZE"
#define KERNELPARAM__LOCK_SUPPLY_BLOCK               (RTEConf_Parameter::Name)"_LOCK_SUPPLY_BLOCK"
#define KERNELPARAM__MAXTRANS                        (RTEConf_Parameter::Name)"_MAXTRANS"
#define KERNELPARAM__MBLOCK_DATA_SIZE                (RTEConf_Parameter::Name)"_MBLOCK_DATA_SIZE"
#define KERNELPARAM__MBLOCK_QUAL_SIZE                (RTEConf_Parameter::Name)"_MBLOCK_QUAL_SIZE"
#define KERNELPARAM__MBLOCK_STACK_SIZE               (RTEConf_Parameter::Name)"_MBLOCK_STACK_SIZE"
#define KERNELPARAM__MBLOCK_STRAT_SIZE               (RTEConf_Parameter::Name)"_MBLOCK_STRAT_SIZE"
#define KERNELPARAM__READAHEAD_BLOBS                 (RTEConf_Parameter::Name)"_READAHEAD_BLOBS"
#define KERNELPARAM__RESTART_TIME                    (RTEConf_Parameter::Name)"_RESTART_TIME"
#define KERNELPARAM__SERVERDB_FOR_SAP                (RTEConf_Parameter::Name)"_SERVERDB_FOR_SAP"
#define KERNELPARAM__WORKDATA_SIZE                   (RTEConf_Parameter::Name)"_WORKDATA_SIZE"
#define KERNELPARAM__WORKSTACK_SIZE                  (RTEConf_Parameter::Name)"_WORKSTACK_SIZE"
#define KERNELPARAM__TRANS_THRESHOLD_VALUE           (RTEConf_Parameter::Name)"TRANS_THRESHOLD_VALUE"
#define KERNELPARAM__TRANS_HISTORY_SIZE              (RTEConf_Parameter::Name)"TRANS_HISTORY_SIZE"

#define KERNELPARAM_USESELECTFETCH                   (RTEConf_Parameter::Name)"USESELECTFETCH"
#define KERNELPARAM_USEVARIABLEINPUT                 (RTEConf_Parameter::Name)"USEVARIABLEINPUT"
