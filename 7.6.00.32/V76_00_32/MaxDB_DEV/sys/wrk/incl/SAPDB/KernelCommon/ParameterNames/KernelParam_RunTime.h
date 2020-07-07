/*!
  @file     KernelParam_RunTime.h
  @author   JoergM
  @ingroup  RunTime
  @brief    Mapping for configuration parameter names used by the RunTime
  @see            

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

#ifndef KERNELPARAM_RUNTIME_HPP
#define KERNELPARAM_RUNTIME_HPP

/*! cserv name of the configuration parameter _AK_DUMP_ALLOWED */
#define KERNELPARAM__AK_DUMP_ALLOWED (SAPDB_UTF8 *)"_AK_DUMP_ALLOWED"
/*! cserv name of the configuration parameter SYMBOL_DEMANGLING */
#define KERNELPARAM_SYMBOL_DEMANGLING (SAPDB_UTF8 *)"SYMBOL_DEMANGLING"
/*! cserv name of the configuration parameter LOG_VOLUME_NAME_ */
#define KERNELPARAM_LOG_VOLUME_NAME_ (SAPDB_UTF8 *)"LOG_VOLUME_NAME_"
/*! cserv name of the configuration parameter ARCHIVE_LOG_ */
#define KERNELPARAM_ARCHIVE_LOG_ (SAPDB_UTF8 *)"ARCHIVE_LOG_"
/*! cserv name of the configuration parameter LOG_VOLUME_TYPE_ */
#define KERNELPARAM_LOG_VOLUME_TYPE_ (SAPDB_UTF8 *)"LOG_VOLUME_TYPE_"
/*! cserv name of the configuration parameter ARCHIVE_TYPE_ */
#define KERNELPARAM_ARCHIVE_TYPE_ (SAPDB_UTF8 *)"ARCHIVE_TYPE_"
/*! cserv name of the configuration parameter LOG_VOLUME_SIZE_ */
#define KERNELPARAM_LOG_VOLUME_SIZE_ (SAPDB_UTF8 *)"LOG_VOLUME_SIZE_"
/*! cserv name of the configuration parameter ARCHIVE_SIZE_ */
#define KERNELPARAM_ARCHIVE_SIZE_ (SAPDB_UTF8 *)"ARCHIVE_SIZE_"
/*! cserv name of the configuration parameter BACKUP_BLOCK_CNT */
#define KERNELPARAM_BACKUP_BLOCK_CNT (SAPDB_UTF8 *)"BACKUP_BLOCK_CNT"
/*! cserv name of the configuration parameter _BACKUP_HISTFILE */
#define KERNELPARAM__BACKUP_HISTFILE (SAPDB_UTF8 *)"_BACKUP_HISTFILE"
/*! cserv name of the configuration parameter _BACKUP_MED_DEF */
#define KERNELPARAM__BACKUP_MED_DEF (SAPDB_UTF8 *)"_BACKUP_MED_DEF"
/*! cserv name of the configuration parameter CACHE_SIZE */
#define KERNELPARAM_CACHE_SIZE (SAPDB_UTF8 *)"CACHE_SIZE"
/*! cserv name of the configuration parameter CAT_CACHE_SUPPLY */
#define KERNELPARAM_CAT_CACHE_SUPPLY (SAPDB_UTF8 *)"CAT_CACHE_SUPPLY"
/*! cserv name of the configuration parameter _COMMENT */
#define KERNELPARAM__COMMENT (SAPDB_UTF8 *)"_COMMENT"
/*! cserv name of the configuration parameter CONTROLUSERID */
#define KERNELPARAM_CONTROLUSERID (SAPDB_UTF8 *)"CONTROLUSERID"
/*! cserv name of the configuration parameter CONTROLPASSWORD */
#define KERNELPARAM_CONTROLPASSWORD (SAPDB_UTF8 *)"CONTROLPASSWORD"
/*! cserv name of the configuration parameter CONVERTER_CACHE */
#define KERNELPARAM_CONVERTER_CACHE (SAPDB_UTF8 *)"CONVERTER_CACHE"
/*! cserv name of the configuration parameter DIAG_HISTORY_PATH */
#define KERNELPARAM_DIAG_HISTORY_PATH (SAPDB_UTF8 *)"DIAG_HISTORY_PATH"
/*! cserv name of the configuration parameter DIAG_HISTORY_NUM */
#define KERNELPARAM_DIAG_HISTORY_NUM (SAPDB_UTF8 *)"DIAG_HISTORY_NUM"
/*! cserv name of the configuration parameter _DIAG_SEM */
#define KERNELPARAM__DIAG_SEM (SAPDB_UTF8 *)"_DIAG_SEM"
/*! cserv name of the configuration parameter DATA_CACHE */
#define KERNELPARAM_DATA_CACHE (SAPDB_UTF8 *)"DATA_CACHE"
/*! cserv name of the configuration parameter DATA_VOLUME_MODE_ */
#define KERNELPARAM_DATA_VOLUME_MODE_ (SAPDB_UTF8 *)"DATA_VOLUME_MODE_"
/*! cserv name of the configuration parameter _DATA_CACHE_RGNS */
#define KERNELPARAM__DATA_CACHE_RGNS (SAPDB_UTF8 *)"_DATA_CACHE_RGNS"
/*! cserv name of the configuration parameter DATA_VOLUME_NAME_ */
#define KERNELPARAM_DATA_VOLUME_NAME_ (SAPDB_UTF8 *)"DATA_VOLUME_NAME_"
/*! cserv name of the configuration parameter DATADEV_ */
#define KERNELPARAM_DATADEV_ (SAPDB_UTF8 *)"DATADEV_"
/*! cserv name of the configuration parameter DATA_VOLUME_TYPE_ */
#define KERNELPARAM_DATA_VOLUME_TYPE_ (SAPDB_UTF8 *)"DATA_VOLUME_TYPE_"
/*! cserv name of the configuration parameter DATA_TYPE_ */
#define KERNELPARAM_DATA_TYPE_ (SAPDB_UTF8 *)"DATA_TYPE_"
/*! cserv name of the configuration parameter DATA_VOLUME_SIZE_ */
#define KERNELPARAM_DATA_VOLUME_SIZE_ (SAPDB_UTF8 *)"DATA_VOLUME_SIZE_"
/*! cserv name of the configuration parameter DATA_SIZE_ */
#define KERNELPARAM_DATA_SIZE_ (SAPDB_UTF8 *)"DATA_SIZE_"
/*! cserv name of the configuration parameter _DELAY_COMMIT */
#define KERNELPARAM__DELAY_COMMIT (SAPDB_UTF8 *)"_DELAY_COMMIT"
/*! cserv name of the configuration parameter _DELAY_LOGWRITER */
#define KERNELPARAM__DELAY_LOGWRITER (SAPDB_UTF8 *)"_DELAY_LOGWRITER"
/*! cserv name of the configuration parameter DEVNO_BIT_COUNT */
#define KERNELPARAM_DEVNO_BIT_COUNT (SAPDB_UTF8 *)"DEVNO_BIT_COUNT"
/*! cserv name of the configuration parameter _IOPROCS_SWITCH */
#define KERNELPARAM__IOPROCS_SWITCH (SAPDB_UTF8 *)"_IOPROCS_SWITCH"
/*! cserv name of the configuration parameter KERNELDIAGSIZE */
#define KERNELPARAM_KERNELDIAGSIZE (SAPDB_UTF8 *)"KERNELDIAGSIZE"
/*! cserv name of the configuration parameter _EVENTFILE */
#define KERNELPARAM__EVENTFILE (SAPDB_UTF8 *)"_EVENTFILE"
/*! cserv name of the configuration parameter _EVENTSIZE */
#define KERNELPARAM__EVENTSIZE (SAPDB_UTF8 *)"_EVENTSIZE"
/*! cserv name of the configuration parameter EXPAND_COM_TRACE */
#define KERNELPARAM_EXPAND_COM_TRACE (SAPDB_UTF8 *)"EXPAND_COM_TRACE"
/*! cserv name of the configuration parameter FBM_VOLUME_BALANCE */
#define KERNELPARAM_FBM_VOLUME_BALANCE (SAPDB_UTF8 *)"FBM_VOLUME_BALANCE"
/*! cserv name of the configuration parameter _FBM_DEVSP_BALANCE */
#define KERNELPARAM__FBM_DEVSP_BALANCE (SAPDB_UTF8 *)"_FBM_DEVSP_BALANCE"
/*! cserv name of the configuration parameter FBM_VOLUME_COMPRESSION */
#define KERNELPARAM_FBM_VOLUME_COMPRESSION (SAPDB_UTF8 *)"FBM_VOLUME_COMPRESSION"
/*! cserv name of the configuration parameter _FBM_DEVSP_COMPR */
#define KERNELPARAM__FBM_DEVSP_COMPR (SAPDB_UTF8 *)"_FBM_DEVSP_COMPR"
/*! cserv name of the configuration parameter FORMAT_DATAVOLUME */
#define KERNELPARAM_FORMAT_DATAVOLUME (SAPDB_UTF8 *)"FORMAT_DATAVOLUME"
/*! cserv name of the configuration parameter FORMATTING_MODE */
#define KERNELPARAM_FORMATTING_MODE (SAPDB_UTF8 *)"FORMATTING_MODE"
/*! cserv name of the configuration parameter GATEWAYTYPE */
#define KERNELPARAM_GATEWAYTYPE (SAPDB_UTF8 *)"GATEWAYTYPE"
/*! cserv name of the configuration parameter HEAP_MEM_POOL */
#define KERNELPARAM_HEAP_MEM_POOL (SAPDB_UTF8 *)"HEAP_MEM_POOL"
/*! cserv name of the configuration parameter HS_DELAY_TIME_ */
#define KERNELPARAM_HS_DELAY_TIME_ (SAPDB_UTF8 *)"HS_DELAY_TIME_"
/*! cserv name of the configuration parameter HS_NODE_ */
#define KERNELPARAM_HS_NODE_ (SAPDB_UTF8 *)"HS_NODE_"
/*! cserv name of the configuration parameter HS_STORAGE_DLL */
#define KERNELPARAM_HS_STORAGE_DLL (SAPDB_UTF8 *)"HS_STORAGE_DLL"
/*! cserv name of the configuration parameter INSTANCE_TYPE */
#define KERNELPARAM_INSTANCE_TYPE (SAPDB_UTF8 *)"INSTANCE_TYPE"
/*! cserv name of the configuration parameter _IOPROCS_FOR_PRIO */
#define KERNELPARAM__IOPROCS_FOR_PRIO (SAPDB_UTF8 *)"_IOPROCS_FOR_PRIO"
/*! cserv name of the configuration parameter _IOPROCS_FOR_READER */
#define KERNELPARAM__IOPROCS_FOR_READER (SAPDB_UTF8 *)"_IOPROCS_FOR_READER"
/*! cserv name of the configuration parameter _KERNELDUMPFILE */
#define KERNELPARAM__KERNELDUMPFILE (SAPDB_UTF8 *)"_KERNELDUMPFILE"
/*! cserv name of the configuration parameter _KERNELTRACEFILE */
#define KERNELPARAM__KERNELTRACEFILE (SAPDB_UTF8 *)"_KERNELTRACEFILE"
/*! cserv name of the configuration parameter KERNELTRACESIZE */
#define KERNELPARAM_KERNELTRACESIZE (SAPDB_UTF8 *)"KERNELTRACESIZE"
/*! cserv name of the configuration parameter LOAD_BALANCING_CHK */
#define KERNELPARAM_LOAD_BALANCING_CHK (SAPDB_UTF8 *)"LOAD_BALANCING_CHK"
/*! cserv name of the configuration parameter LOAD_BALANCING_DIF */
#define KERNELPARAM_LOAD_BALANCING_DIF (SAPDB_UTF8 *)"LOAD_BALANCING_DIF"
/*! cserv name of the configuration parameter LOAD_BALANCING_EQ */
#define KERNELPARAM_LOAD_BALANCING_EQ (SAPDB_UTF8 *)"LOAD_BALANCING_EQ"
/*! cserv name of the configuration parameter LOG_MODE */
#define KERNELPARAM_LOG_MODE (SAPDB_UTF8 *)"LOG_MODE"
/*! cserv name of the configuration parameter M_ARCHIVE_LOG_ (obsolete needed for migration) */
#define KERNELPARAM_M_ARCHIVE_LOG_ (SAPDB_UTF8 *)"M_ARCHIVE_LOG_"
/*! cserv name of the configuration parameter M_ARCHIVE_TYPE_  (obsolete needed for migration) */
#define KERNELPARAM_M_ARCHIVE_TYPE_ (SAPDB_UTF8 *)"M_ARCHIVE_TYPE_"
/*! cserv name of the configuration parameter M_LOG_VOLUME_NAME_ */
#define KERNELPARAM_M_LOG_VOLUME_NAME_ (SAPDB_UTF8 *)"M_LOG_VOLUME_NAME_"
/*! cserv name of the configuration parameter M_LOG_VOLUME_TYPE_ */
#define KERNELPARAM_M_LOG_VOLUME_TYPE_ (SAPDB_UTF8 *)"M_LOG_VOLUME_TYPE_"
/*! cserv name of the configuration parameter M_DATA_VOLUME_NAME_ */
#define KERNELPARAM_M_DATA_VOLUME_NAME_ (SAPDB_UTF8 *)"M_DATA_VOLUME_NAME_"
/*! cserv name of the configuration parameter M_SYSDEV_ */
#define KERNELPARAM_M_SYSDEV_ (SAPDB_UTF8 *)"M_SYSDEV_"
/*! cserv name of the configuration parameter _MAX_BACKUP_TASKS */
#define KERNELPARAM__MAX_BACKUP_TASKS (SAPDB_UTF8 *)"_MAX_BACKUP_TASKS"
/*! cserv name of the configuration parameter _MAX_MESSAGE_FILES */
#define KERNELPARAM__MAX_MESSAGE_FILES (SAPDB_UTF8 *)"_MAX_MESSAGE_FILES"
/*! cserv name of the configuration parameter MAXUSERTASKS */
#define KERNELPARAM_MAXUSERTASKS (SAPDB_UTF8 *)"MAXUSERTASKS"
/*! cserv name of the configuration parameter MAXARCHIVELOGS */
#define KERNELPARAM_MAXARCHIVELOGS (SAPDB_UTF8 *)"MAXARCHIVELOGS"
/*! cserv name of the configuration parameter MAXBACKUPDEVS */
#define KERNELPARAM_MAXBACKUPDEVS (SAPDB_UTF8 *)"MAXBACKUPDEVS"
/*! cserv name of the configuration parameter MAXCPU */
#define KERNELPARAM_MAXCPU (SAPDB_UTF8 *)"MAXCPU"
/*! cserv name of the configuration parameter MAXDATADEVSPACES */
#define KERNELPARAM_MAXDATADEVSPACES (SAPDB_UTF8 *)"MAXDATADEVSPACES"
/*! cserv name of the configuration parameter MAXPAGER */
#define KERNELPARAM_MAXPAGER (SAPDB_UTF8 *)"MAXPAGER"
/*! cserv name of the configuration parameter _MAXDATAWRITER */
#define KERNELPARAM__MAXDATAWRITER (SAPDB_UTF8 *)"_MAXDATAWRITER"
/*! cserv name of the configuration parameter MAXVOLUMES */
#define KERNELPARAM_MAXVOLUMES (SAPDB_UTF8 *)"MAXVOLUMES"
/*! cserv name of the configuration parameter _MAXDEVSPACES */
#define KERNELPARAM__MAXDEVSPACES (SAPDB_UTF8 *)"_MAXDEVSPACES"
/*! cserv name of the configuration parameter _MAXEVENTS */
#define KERNELPARAM__MAXEVENTS (SAPDB_UTF8 *)"_MAXEVENTS"
/*! cserv name of the configuration parameter _MAXEVENTTASKS */
#define KERNELPARAM__MAXEVENTTASKS (SAPDB_UTF8 *)"_MAXEVENTTASKS"
/*! cserv name of the configuration parameter _MAXGARBAGE_COLL */
#define KERNELPARAM__MAXGARBAGE_COLL (SAPDB_UTF8 *)"_MAXGARBAGE_COLL"
/*! cserv name of the configuration parameter _IOPROCS_PER_DEV */
#define KERNELPARAM__IOPROCS_PER_DEV (SAPDB_UTF8 *)"_IOPROCS_PER_DEV"
/*! cserv name of the configuration parameter MAXRGN_REQUEST */
#define KERNELPARAM_MAXRGN_REQUEST (SAPDB_UTF8 *)"MAXRGN_REQUEST"
/*! cserv name of the configuration parameter MAXSERVERTASKS */
#define KERNELPARAM_MAXSERVERTASKS (SAPDB_UTF8 *)"MAXSERVERTASKS"
/*! cserv name of the configuration parameter _MAXSYSDEVSPACES */
#define KERNELPARAM__MAXSYSDEVSPACES (SAPDB_UTF8 *)"_MAXSYSDEVSPACES"
/*! cserv name of the configuration parameter _MAXTASK_STACK */
#define KERNELPARAM__MAXTASK_STACK (SAPDB_UTF8 *)"_MAXTASK_STACK"
/*! cserv name of the configuration parameter MEMORY_SLOT_SIZE */
#define KERNELPARAM_MEMORY_SLOT_SIZE (SAPDB_UTF8 *)"MEMORY_SLOT_SIZE"
/*! cserv name of the configuration parameter MEMORY_ALLOCATION_LIMIT */
#define KERNELPARAM_MEMORY_ALLOCATION_LIMIT (SAPDB_UTF8 *)"MEMORY_ALLOCATION_LIMIT"
/*! cserv name of the configuration parameter _MINREPLY_SIZE */
#define KERNELPARAM__MINREPLY_SIZE (SAPDB_UTF8 *)"_MINREPLY_SIZE"
/*! cserv name of the configuration parameter _MIRRORED_DATA */
#define KERNELPARAM__MIRRORED_DATA (SAPDB_UTF8 *)"_MIRRORED_DATA"
/*! cserv name of the configuration parameter MMAP_MEM_POOL */
#define KERNELPARAM_MMAP_MEM_POOL (SAPDB_UTF8 *)"MMAP_MEM_POOL"
/*! cserv name of the configuration parameter _MP_DISP_LOOPS */
#define KERNELPARAM__MP_DISP_LOOPS (SAPDB_UTF8 *)"_MP_DISP_LOOPS"
/*! cserv name of the configuration parameter _MP_DISP_PRIO */
#define KERNELPARAM__MP_DISP_PRIO (SAPDB_UTF8 *)"_MP_DISP_PRIO"
/*! cserv name of the configuration parameter _MP_RGN_BUSY_WAIT */
#define KERNELPARAM__MP_RGN_BUSY_WAIT (SAPDB_UTF8 *)"_MP_RGN_BUSY_WAIT"
/*! cserv name of the configuration parameter _MP_RGN_DIRTY_READ */
#define KERNELPARAM__MP_RGN_DIRTY_READ (SAPDB_UTF8 *)"_MP_RGN_DIRTY_READ"
/*! cserv name of the configuration parameter MP_RGN_LOOP */
#define KERNELPARAM_MP_RGN_LOOP (SAPDB_UTF8 *)"MP_RGN_LOOP"
/*! cserv name of the configuration parameter _MP_RGN_PRIO */
#define KERNELPARAM__MP_RGN_PRIO (SAPDB_UTF8 *)"_MP_RGN_PRIO"
/*! cserv name of the configuration parameter _MP_RGN_QUEUE */
#define KERNELPARAM__MP_RGN_QUEUE (SAPDB_UTF8 *)"_MP_RGN_QUEUE"
/*! cserv name of the configuration parameter OFFICIAL_NODE */
#define KERNELPARAM_OFFICIAL_NODE (SAPDB_UTF8 *)"OFFICIAL_NODE"
/*! cserv name of the configuration parameter OMS_HEAP_LIMIT */
#define KERNELPARAM_OMS_HEAP_LIMIT (SAPDB_UTF8 *)"OMS_HEAP_LIMIT"
/*! cserv name of the configuration parameter _OMS_RGNS */
#define KERNELPARAM__OMS_RGNS (SAPDB_UTF8 *)"_OMS_RGNS"
/*! cserv name of the configuration parameter _PACKET_SIZE */
#define KERNELPARAM__PACKET_SIZE (SAPDB_UTF8 *)"_PACKET_SIZE"
/*! cserv name of the configuration parameter _PAGE_SIZE */
#define KERNELPARAM__PAGE_SIZE (SAPDB_UTF8 *)"_PAGE_SIZE"
/*! cserv name of the configuration parameter KERNELVERSION */
#define KERNELPARAM_KERNELVERSION (SAPDB_UTF8 *)"KERNELVERSION"
/*! cserv name of the configuration parameter __PARAM_VERIFIED__ */
#define KERNELPARAM___PARAM_VERIFIED__ (SAPDB_UTF8 *)"__PARAM_VERIFIED__"
/*! cserv name of the configuration parameter _PRIO_BASE_COM */
#define KERNELPARAM__PRIO_BASE_COM (SAPDB_UTF8 *)"_PRIO_BASE_COM"
/*! cserv name of the configuration parameter _PRIO_BASE_IOC */
#define KERNELPARAM__PRIO_BASE_IOC (SAPDB_UTF8 *)"_PRIO_BASE_IOC"
/*! cserv name of the configuration parameter _PRIO_BASE_RAV */
#define KERNELPARAM__PRIO_BASE_RAV (SAPDB_UTF8 *)"_PRIO_BASE_RAV"
/*! cserv name of the configuration parameter _PRIO_BASE_REX */
#define KERNELPARAM__PRIO_BASE_REX (SAPDB_UTF8 *)"_PRIO_BASE_REX"
/*! cserv name of the configuration parameter _PRIO_BASE_U2U */
#define KERNELPARAM__PRIO_BASE_U2U (SAPDB_UTF8 *)"_PRIO_BASE_U2U"
/*! cserv name of the configuration parameter _PRIO_FACTOR */
#define KERNELPARAM__PRIO_FACTOR (SAPDB_UTF8 *)"_PRIO_FACTOR"
/*! cserv name of the configuration parameter _ROW_RGNS */
#define KERNELPARAM__ROW_RGNS (SAPDB_UTF8 *)"_ROW_RGNS"
/*! cserv name of the configuration parameter _RTEDUMPFILE */
#define KERNELPARAM__RTEDUMPFILE (SAPDB_UTF8 *)"_RTEDUMPFILE"
/*! cserv name of the configuration parameter RUNDIRECTORY */
#define KERNELPARAM_RUNDIRECTORY (SAPDB_UTF8 *)"RUNDIRECTORY"
/*! cserv name of the configuration parameter SERVERDB */
#define KERNELPARAM_SERVERDB (SAPDB_UTF8 *)"SERVERDB"
/*! cserv name of the configuration parameter SET_VOLUME_LOCK */
#define KERNELPARAM_SET_VOLUME_LOCK (SAPDB_UTF8 *)"SET_VOLUME_LOCK"
/*! cserv name of the configuration parameter SHM_MEM_POOL */
#define KERNELPARAM_SHM_MEM_POOL (SAPDB_UTF8 *)"SHM_MEM_POOL"
/*! cserv name of the configuration parameter SHOW_MAX_STACK_USE */
#define KERNELPARAM_SHOW_MAX_STACK_USE (SAPDB_UTF8 *)"SHOW_MAX_STACK_USE"
/*! cserv name of the configuration parameter SUPPRESS_CORE */
#define KERNELPARAM_SUPPRESS_CORE (SAPDB_UTF8 *)"SUPPRESS_CORE"
/*! cserv name of the configuration parameter SYSDEV_ */
#define KERNELPARAM_SYSDEV_ (SAPDB_UTF8 *)"SYSDEV_"
/*! cserv name of the configuration parameter SYS_SIZE_ */
#define KERNELPARAM_SYS_SIZE_ (SAPDB_UTF8 *)"SYS_SIZE_"
/*! cserv name of the configuration parameter _TAB_RGNS */
#define KERNELPARAM__TAB_RGNS (SAPDB_UTF8 *)"_TAB_RGNS"
/*! cserv name of the configuration parameter _TASK_STACK_AREA */
#define KERNELPARAM__TASK_STACK_AREA (SAPDB_UTF8 *)"_TASK_STACK_AREA"
/*! cserv name of the configuration parameter _TASKCLUSTER_ */
#define KERNELPARAM__TASKCLUSTER_ (SAPDB_UTF8 *)"_TASKCLUSTER_"
/*! cserv name of the configuration parameter TRACE_PAGES_LW */
#define KERNELPARAM_TRACE_PAGES_LW (SAPDB_UTF8 *)"TRACE_PAGES_LW"
/*! cserv name of the configuration parameter TRACE_PAGES_AL */
#define KERNELPARAM_TRACE_PAGES_AL (SAPDB_UTF8 *)"TRACE_PAGES_AL"
/*! cserv name of the configuration parameter TRACE_PAGES_BUP */
#define KERNELPARAM_TRACE_PAGES_BUP (SAPDB_UTF8 *)"TRACE_PAGES_BUP"
/*! cserv name of the configuration parameter TRACE_PAGES_PG */
#define KERNELPARAM_TRACE_PAGES_PG (SAPDB_UTF8 *)"TRACE_PAGES_PG"
/*! cserv name of the configuration parameter TRACE_PAGES_DW */
#define KERNELPARAM_TRACE_PAGES_DW (SAPDB_UTF8 *)"TRACE_PAGES_DW"
/*! cserv name of the configuration parameter TRACE_PAGES_EV */
#define KERNELPARAM_TRACE_PAGES_EV (SAPDB_UTF8 *)"TRACE_PAGES_EV"
/*! cserv name of the configuration parameter TRACE_PAGES_GC */
#define KERNELPARAM_TRACE_PAGES_GC (SAPDB_UTF8 *)"TRACE_PAGES_GC"
/*! cserv name of the configuration parameter TRACE_PAGES_SV */
#define KERNELPARAM_TRACE_PAGES_SV (SAPDB_UTF8 *)"TRACE_PAGES_SV"
/*! cserv name of the configuration parameter TRACE_PAGES_TI */
#define KERNELPARAM_TRACE_PAGES_TI (SAPDB_UTF8 *)"TRACE_PAGES_TI"
/*! cserv name of the configuration parameter TRACE_PAGES_TW */
#define KERNELPARAM_TRACE_PAGES_TW (SAPDB_UTF8 *)"TRACE_PAGES_TW"
/*! cserv name of the configuration parameter TRACE_PAGES_US */
#define KERNELPARAM_TRACE_PAGES_US (SAPDB_UTF8 *)"TRACE_PAGES_US"
/*! cserv name of the configuration parameter TRACE_PAGES_UT */
#define KERNELPARAM_TRACE_PAGES_UT (SAPDB_UTF8 *)"TRACE_PAGES_UT"
/*! cserv name of the configuration parameter _TRANS_RGNS */
#define KERNELPARAM__TRANS_RGNS (SAPDB_UTF8 *)"_TRANS_RGNS"
/*! cserv name of the configuration parameter _TREE_RGNS */
#define KERNELPARAM__TREE_RGNS (SAPDB_UTF8 *)"_TREE_RGNS"
/*! cserv name of the configuration parameter _UNICODE */
#define KERNELPARAM__UNICODE (SAPDB_UTF8 *)"_UNICODE"
/*! cserv name of the configuration parameter _USE_ASYNC_IO */
#define KERNELPARAM__USE_ASYNC_IO (SAPDB_UTF8 *)"_USE_ASYNC_IO"
/*! cserv name of the configuration parameter _USE_IOPROCS_ONLY */
#define KERNELPARAM__USE_IOPROCS_ONLY (SAPDB_UTF8 *)"_USE_IOPROCS_ONLY"
/*! cserv name of the configuration parameter USE_OPEN_DIRECT */
#define KERNELPARAM_USE_OPEN_DIRECT (SAPDB_UTF8 *)"USE_OPEN_DIRECT"
/*! cserv name of the configuration parameter _USE_PROC_SCOPE */
#define KERNELPARAM__USE_PROC_SCOPE (SAPDB_UTF8 *)"_USE_PROC_SCOPE"
/*! cserv name of the configuration parameter USE_SYSTEM_PAGE_CACHE */
#define KERNELPARAM_USE_SYSTEM_PAGE_CACHE (SAPDB_UTF8 *)"USE_SYSTEM_PAGE_CACHE"
/*! cserv name of the configuration parameter _UTILITY_PROTFILE */
#define KERNELPARAM__UTILITY_PROTFILE (SAPDB_UTF8 *)"_UTILITY_PROTFILE"
/*! cserv name of the configuration parameter UTILITY_PROTSIZE */
#define KERNELPARAM_UTILITY_PROTSIZE (SAPDB_UTF8 *)"UTILITY_PROTSIZE"
/*! cserv name of the configuration parameter VALLOC_MEM_POOL */
#define KERNELPARAM_VALLOC_MEM_POOL (SAPDB_UTF8 *)"VALLOC_MEM_POOL"
/*! cserv name of the configuration parameter __PARAM_CHANGED___ */
#define KERNELPARAM___PARAM_CHANGED___ (SAPDB_UTF8 *)"__PARAM_CHANGED___"
/*! cserv name of the configuration parameter FORBID_LOAD_BALANCING */
#define KERNELPARAM_FORBID_LOAD_BALANCING   (SAPDB_UTF8 *)"FORBID_LOAD_BALANCING"
/*! cserv name of the configuration parameter EXTERNAL_DUMP_REQUEST */
#define KERNELPARAM_EXTERNAL_DUMP_REQUEST (SAPDB_UTF8 *)"EXTERNAL_DUMP_REQUEST"
/*! cserv name of the configuration parameter AUTHENTICATION_ALLOW */
#define KERNELPARAM_AUTHENTICATION_ALLOW   (SAPDB_UTF8 *)"AUTHENTICATION_ALLOW"
/*! cserv name of the configuration parameter AUTHENTICATION_DENY */
#define KERNELPARAM_AUTHENTICATION_DENY   (SAPDB_UTF8 *)"AUTHENTICATION_DENY"
/*! cserv name of the configuration parameter TIME_MEASUREMENT */
#define KERNELPARAM_TIME_MEASUREMENT (SAPDB_UTF8 *)"TIME_MEASUREMENT"
/*! cserv name of the configuration parameter MAX_MESSAGE_LIST_LENGTH */
#define KERNELPARAM_MAX_MESSAGE_LIST_LENGTH  (SAPDB_UTF8 *)"MAX_MESSAGE_LIST_LENGTH"
/*! cserv name of the configuration parameter SYMBOL_RESOLUTION */
#define KERNELPARAM_SYMBOL_RESOLUTION (SAPDB_UTF8 *)"SYMBOL_RESOLUTION"
/*! cserv name of the configuration parameter PREALLOCATE_IOWORKER */
#define KERNELPARAM_PREALLOCATE_IOWORKER   (SAPDB_UTF8 *)"PREALLOCATE_IOWORKER"
/*++++++++++++++++*OS DEPENDEND PARAMETERS *+++++++++++++++++++*/
/***************************************************************/
#if defined(_WIN32)
/***************************************************************/

/*! cserv name of the configuration parameter ALLOW_COMPR_DEVSP */
#define KERNELPARAM_ALLOW_COMPR_DEVSP (SAPDB_UTF8 *)"ALLOW_COMPR_DEVSP"
/*! cserv name of the configuration parameter DYNAMIC_SERVER */
#define KERNELPARAM_DYNAMIC_SERVER (SAPDB_UTF8 *)"DYNAMIC_SERVER"
/*! cserv name of the configuration parameter MEM_ENHANCE_LIMIT */
#define KERNELPARAM_MEM_ENHANCE_LIMIT (SAPDB_UTF8 *)"MEM_ENHANCE_LIMIT"
/*! cserv name of the configuration parameter RESTART_SHUTDOWN */
#define KERNELPARAM_RESTART_SHUTDOWN (SAPDB_UTF8 *)"RESTART_SHUTDOWN"
/*! cserv name of the configuration parameter USE_MEM_ENHANCE */
#define KERNELPARAM_USE_MEM_ENHANCE (SAPDB_UTF8 *)"USE_MEM_ENHANCE"
/*! cserv name of the configuration parameter USE_FIBERS */
#define KERNELPARAM_USE_FIBERS (SAPDB_UTF8 *)"USE_FIBERS"
/*! cserv name of the configuration parameter UKT_CPU_RELATIONSHIP */
#define KERNELPARAM_UKT_CPU_REL (SAPDB_UTF8 *)"UKT_CPU_RELATIONSHIP"
/*! cserv name of the configuration parameter MINI_DUMP */
#define KERNELPARAM_MINI_DUMP (SAPDB_UTF8 *)"MINI_DUMP"


/***************************************************************/
#else
/***************************************************************/

/*! cserv name of the configuration parameter OPMSG1 */
#define KERNELPARAM_OPMSG1 (SAPDB_UTF8 *)"OPMSG1"
/*! cserv name of the configuration parameter OPMSG2 */
#define KERNELPARAM_OPMSG2 (SAPDB_UTF8 *)"OPMSG2"
/*! cserv name of the configuration parameter _SHMCHUNK */
#define KERNELPARAM__SHMCHUNK (SAPDB_UTF8 *)"_SHMCHUNK"
/*! cserv name of the configuration parameter _SHMKERNEL */
#define KERNELPARAM__SHMKERNEL (SAPDB_UTF8 *)"_SHMKERNEL"
/*! cserv name of the configuration parameter USE_COROUTINES */
#define KERNELPARAM_USE_COROUTINES (SAPDB_UTF8 *)"USE_COROUTINES"
/*! cserv name of the configuration parameter USE_STACK_ON_STACK */
#define KERNELPARAM_USE_STACK_ON_STACK (SAPDB_UTF8 *)"USE_STACK_ON_STACK"
/*! cserv name of the configuration parameter USE_UCONTEXT */
#define KERNELPARAM_USE_UCONTEXT   (SAPDB_UTF8 *)"USE_UCONTEXT"
/*! cserv name of the configuration parameter SIMULATE_VECTORIO */
#define KERNELPARAM_SIMULATE_VECTORIO (SAPDB_UTF8 *)"SIMULATE_VECTORIO"
/*! cserv name of the configuration parameter UKT_CPU_RELATIONSHIP */
#define KERNELPARAM_UKT_CPU_RELATIONSHIP (SAPDB_UTF8 *)"UKT_CPU_RELATIONSHIP"

/***************************************************************/
#endif
/***************************************************************/
/***************************************************************/
#if defined(_WIN32) || defined(LINUX)
/***************************************************************/
/*! cserv name of the configuration parameter HIRES_TIMER_TYPE */
#define KERNELPARAM_HIRES_TIMER_TYPE (SAPDB_UTF8 *)"HIRES_TIMER_TYPE"
/***************************************************************/
#endif
/***************************************************************/

#endif /* KERNELPARAM_RUNTIME_HPP */


