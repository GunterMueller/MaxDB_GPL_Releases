/*!
  @file           RTE_ConsoleStandardOutput.h
  @author         StefanP
  @brief          Console Output
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



#ifndef RTE_CONSOLESTANDARDOUTPUT_H
#define RTE_CONSOLESTANDARDOUTPUT_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/



/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(_WIN32)
#define SAPDB_UINT8_FORMAT "I64u"
#define SAPDB_INT8_FORMAT "I64d"
#else
#if defined(BIT64)
#define SAPDB_UINT8_FORMAT "lu"
#define SAPDB_INT8_FORMAT "ld"
#else
#define SAPDB_UINT8_FORMAT "llu"
#define SAPDB_INT8_FORMAT "lld"
#endif
#endif

/*
 --- XCONS standard output ---
*/


#define XCONS_SERVERDB                     "\nSERVERDB: %s\n"
#define XCONS_GATEWAY                      "\nGateway: %s\n"

/* CR 1103476 */
/* CR 1115172 */
#define XCONS_SHOW_TASK_MOVE_HEADER        "\n"                                                            \
                                           "%-3.3s  %3.3s %-7.7s %-14.14s %11.11s %11.11s %-10.10s \n"   \
                                           "%-3.3s  %3.3s %-7.7s %-14.14s %11.11s %11.11s %-10.10s \n",  \
                                           "ID", "UKT", "TASK", "Current", "Max.",     "Sum",      "Move", \
                                	         "",   "",    "type", "state",   "Runnable", "Runnable", "lock"

# define XCONS_SHOW_TASK_MOVE_INFO         "T%-3d %3d %-7.7s %-14.14s %4" SAPDB_INT8_FORMAT ".%06" SAPDB_INT8_FORMAT " %4" SAPDB_INT8_FORMAT ".%06" SAPDB_INT8_FORMAT " %-10.10s\n"

#define XCONS_SHOW_TASK_HEADER             "\n"                                                                   \
                                           "%-3.3s  %3.3s %4.4s   %-7.7s %7.7s %-14.14s %8.8s %-8.8s   %-4.4s \n" \
                                           "%-3.3s  %3.3s %4.4s   %-7.7s %7.7s %-14.14s %8.8s %-8.8s   %-4.4s \n", \
                                           "ID", "UKT", XCONS_SYS, "TASK", "APPL", "Current", "Timeout",  "Region",   "Wait", \
                                	         "",   "",    "tid",     "type", "pid",  "state",   "priority", "cnt try", "item"

#define XCONS_SHOW_TASK_INFO               "T%-3d %3d %6s %-7.7s %7s%c%-14.14s%c%c%c  %4.4s %-4" SAPDB_UINT8_FORMAT " %3s %6s %-" SAPDB_UINT8_FORMAT "(%c)\n"
#define XCONS_SHOW_SERVER_UNUSED           "\n%5d Server Task(s) is/are unused"
#define XCONS_SHOW_TASK_CONN_INFO          "\n%5d Task(s) is/are in State %s\n"
#define XCONS_SHOW_TASK_NO_TIMEOUT         "no"

#define XCONS_SHOW_IO_HEADER               "\n"                                         \
                                           "%-30.30s %5.5s %10.10s %10.10s %10.10s %10.10s\n"               \
                                           "%-30.30s %5.5s %10.10s %10.10s %10.10s %10.10s\n",              \
                                           "Volume", "Devs.", "Read(s)",  "Read", "Write(s)", "Write",\
                                           "Name",     "No.",   "Count", "Pages",    "Count", "Pages"

#define XCONS_SHOW_IO_INFO                 "%-30.30s %5d %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT "\n"
#define XCONS_SHOW_IO_INFO_TOTAL_STR       "total I/O:"
#define XCONS_SHOW_IO_INFO_TOTAL           "%30.30s       %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT "\n"

#define XCONS_SHOW_REGION_HEADER           "\n"                                                                           \
                                           "%5s  %-8.8s  %-5.5s %10.10s %10.10s %10.10s %8.8s %4.4s   %s\n",                       \
                                           "Index", "Region", "Owner", "Get-Cnt", "Tas-Cnt","Coll.", "Waits", "Excl","Coll % "
#define XCONS_SHOW_REGION_INFO             "%5d  %-8.8s  %-5.5s %10u %10u %10u %8u %4d %6.2f %%\n"
#define XCONS_SHOW_REGION_WAIT_HD          "       waiting:"
#define XCONS_SHOW_REGION_WAIT_I           "  T%-4d"
#define XCONS_SHOW_REGION_WAIT_ILF         "\n                 T%-4d"

#define XCONS_SHOW_STOR_DYN_POOL           "\n" \
                                           "Type                      allocated         used       unused\n"\
                                           "Shared dyn pool         %8d KB  %8d KB  %8d KB\n"
#define XCONS_SHOW_STOR_DYN_DATA           "Shared dyn data         %8d KB  %8d KB  %8d KB\n"
#define XCONS_SHOW_STOR_RTE_ALLOC_POOL_MX  "\n" \
                                           "Type                      allocated          max       cnt\n"\
                                           "RTE allocation pool     %8d KB  %8d MB  %8d\n"
#define XCONS_SHOW_STOR_RTE_ALLOC_POOL     "\n" \
                                           "Type                      allocated       cnt\n"\
                                           "RTE allocation pool     %8d KB  %8d\n"

#define XCONS_SHOW_STOR_ALLOC_STCK         "\n"    \
                                           "Type                      allocated\n"\
                                           "Task stacks             %8d KB\n"
#define XCONS_SHOW_STOR_KGS                "Kernel global section   %8d KB\n"
#define XCONS_SHOW_STOR_KSS                "Kernel shared section   %8d KB\n"

#define XCONS_SHOW_STOR_TSK_HEADER         "\n"                                       \
                                           "%-5.5s %-7.7s  %10.10s  %10.10s\n"        \
                                           "%-5.5s %-7.7s  %10.10s  %10.10s \n",      \
                                           "Task", "Type", "Stack used", "Stack free",\
                                           "ID",   "",     "KB",          "KB"
#define XCONS_SHOW_STOR_TSK_INFO           "T%-4d %-7.7s  %10.1f  %10.1f\n"

#define XCONS_SHOW_RTE_KT_HEADER           "\nKernel Threads:\n"            \
                                           " %-11.11s %10.10s  %-12.12s\n"  \
                                           " %-11.11s %10.10s  %-12.12s\n", \
                                           "Thread", XCONS_SYS,"State",     \
                                           "Name",  "Tid",  ""
#define XCONS_SHOW_RTE_KT_INFO             " %-11.11s %10.10s  %-12.12s\n"
#define XCONS_SHOW_RTE_KT_COORD            "COORDINATOR"
#define XCONS_SHOW_RTE_KT_TIMER            "TIMER"
#define XCONS_SHOW_RTE_KT_CLOCK            "CLOCK"
#define XCONS_SHOW_RTE_KT_DEV0             "DEV0"
#define XCONS_SHOW_RTE_KT_DCOM0            "DCOM0"
#define XCONS_SHOW_RTE_KT_ASYNC0           "ASYNC0"
#define XCONS_SHOW_RTE_KT_CONSOLE          "CONSOLE"
#define XCONS_SHOW_RTE_KT_REQUESTOR        "REQUESTOR"
#define XCONS_SHOW_RTE_DEV_WORKER_HEADER   "\nIO Worker Threads:\n"                                             \
                                           " %-12.12s  %10.10s  %-11.11s  %-13.13s\n"             \
                                           " %-12.12s  %10.10s  %-11.11s  %-13.13s\n",            \
                                           "Thread", XCONS_SYS, "State", "IO", \
                                           "Name",   "Tid",     "",      "Counter"
#define XCONS_SHOW_RTE_DEV_WORKER_INFO     " IO-WORKER%-3d  %10.10s  %-11.11s  %-10d\n"
#define XCONS_SHOW_RTE_CONCURR_DEV_WORKER  "\n %d IO Worker threads executing concurrently\n"

#define XCONS_SHOW_RTE_UKT_HEADER          "\nUser Kernel Threads:\n"                                             \
                                           " %-6.6s  %10.10s  %-8.8s %11.11s %11.11s %6.6s %5.5s %s\n"             \
                                           " %-6.6s  %10.10s  %-8.8s %11.11s %11.11s %6.6s %5.5s %s\n",            \
                                           "Thread", XCONS_SYS, "State", "Dispatch", "TaskSwitch", "Active" , "Total" , "Task", \
                                           "Name",   "Tid",     "",      "Counter",  "Counter",    "Tasks"  , "Tasks" , "Cluster"
#define XCONS_SHOW_RTE_UKT_INFO            " UKT%-3d  %10.10s  %-8.8s %11.0f %11.0f %6d %5d %s\n"
#define XCONS_SHOW_RTE_UKT_CPU_HEADER      "\nProcessor information:\n"
#define XCONS_SHOW_RTE_UKT_CPU_INFO        " Processors     : %d\n Processor cores: %d\n"
#define XCONS_SHOW_RTE_UKT_REL_HEADER      "\nUKT processor relationship:\n"
#define XCONS_SHOW_RTE_UKT_REL_MASK_HD1    "         CPU: %s\n"
#define XCONS_SHOW_RTE_UKT_REL_MASK_HD2    "              %s\n"
#define XCONS_SHOW_RTE_UKT_REL_MASK_INFO   " UKT%-3d Mask: %s %c\n"

#define XCONS_SHOW_RTE_IO_HEADER           "\nI/O via UKTs and I/O Threads:\n"                                    \
                                           " %-6.6s  %10.10s  %-20.20s %5.5s  %8.8s  %8.8s    %5.5s\n"            \
                                           " %-6.6s  %10.10s  %-20.20s %5.5s  %8.8s  %8.8s  %4.4s %4.4s\n",       \
                                           "Thread",XCONS_SYS,"Volume", "Devs.", "Read",  "Write" , "Queue",    \
                                           "Name",  "Tid",    "Name",     "No.",   "Count", "Count" , "Len.", "Max."
#define XCONS_SHOW_RTE_IO_UKT_INFO         " UKT%-3d  %10.10s  %-20.20s %5d  %8d  %8d   --  (--)\n"
#define XCONS_SHOW_RTE_IO_DEV_INFO         " I/O%-3d  %10.10s  %-20.20s %5d  %8d  %8d   %2d  (%2d)\n"

#define XCONS_SHOW_ASYNCIO_HEADER          "\nI/O via asynchronous I/O Threads:\n"                                 \
                                           " %-20.20s %10.10s  %3.3s %6.6s %6.6s %8.8s %8.8s   %5.5s\n"            \
                                           " %-20.20s %10.10s  %3.3s %6.6s %6.6s %8.8s %8.8s %4.4s %4.4s\n",       \
                                           "Device", XCONS_SYS, "R/W", "Max   ",  "Block","Read",  "Write" , "Queue",    \
                                           "Name",   "Tid",     "",    "Blocks",  "Size", "Count", "Count" , "Len.", "Max."
#define XCONS_SHOW_ASYNCIO_INFO            " %-20.20s %10.10s  %3s %6d %6d %8d %8d  %2d  (%2d)\n"

#define XCONS_SHOW_AWE_INFO                "\nAWE infos:\n"                                   \
                                           " Number of physical pages                %8d\n" \
                                           " Number of free physical pages           %8d\n" \
                                           " Number of map area pages (cache)        %8d\n" \
                                           " Number of free map area pages (cache)   %8d\n" \
                                           " Number of map calls                     %8d\n" \
                                           " Number of unmap calls                   %8d\n"


#if defined (_WIN64)
# define XCONS_SHOW_AWE_MAP_PAGE_HEADER    "\nAWE map page infos:\n\n"               \
                                           " %-18.18s   %-10.10s %-4.4s %-20.20s\n",\
                                           "MapAddress", "PhysPageNo", "Res.",   "Map Count"
# define XCONS_SHOW_AWE_MAP_PAGE_INFO      " 0x%016I64X - %-10d YES  %-d\n"
# define XCONS_SHOW_AWE_UNMAP_PAGE_INFO    " 0x%016I64X - NO         %-4.4s %-d\n"
#else
# define XCONS_SHOW_AWE_MAP_PAGE_HEADER    "\nAWE map page infos:\n\n"               \
                                           " %-10.10s   %-10.10s %-4.4s %-20.20s\n",\
                                           "MapAddress", "PhysPageNo", "Res.",   "Map Count"
# define XCONS_SHOW_AWE_MAP_PAGE_INFO      " 0x%08X - %-10d YES  %-d\n"
# define XCONS_SHOW_AWE_UNMAP_PAGE_INFO    " 0x%08X - NO         %-4.4s %-d\n"
#endif
#define XCONS_SHOW_AWE_PHYS_PAGE_HEADER    "\nAWE physical page infos:\n\n"            \
                                           " %-10.10s   %-4.4s %-20.20s\n",            \
                                           "PageNo", "Res.",   "Map Count"

#define XCONS_SHOW_AWE_PHYS_PAGE_INFO      " %-10d - %-4.4s %-d\n"

#define XCONS_FORMAT_SHOW_CONNECTION_TASK                     " Task T%d UKT%d  Task type: %s\n"
#define XCONS_FORMAT_SHOW_CONNECTION_STATE_NOT_CONNECTED      "    state '%s' connect time not set\n"
#define XCONS_FORMAT_SHOW_CONNECTION_STATE_TIME               "    state '%s' connect time %s"
#define XCONS_FORMAT_SHOW_CONNECTION_STATE_DUMPTIME           "    state '%s' connect time not convertable[%ld]\n"
#define XCONS_FORMAT_SHOW_CONNECTION_LOCAL                    "    LOCAL CONNECTION  peer pid %d\n"
#define XCONS_FORMAT_SHOW_CONNECTION_REMOTE	                  "    REMOTE CONNECTION peer pid %d\n" \
		                                                      "                      remote pid %s remote node %s\n"
#define XCONS_FORMAT_SHOW_CONNECTION_NO_TASK_CONNECTED		  "\n no Task connected\n"

#define XCONS_SHOW_MOVE_INFO               "\nBalancing Info:\n"          \
                                           " Load balancing      -  %s\n" \
                                           " Interval time       -  %d (s)\n" \
                                           " Last interval start -  %s (ddd:hh:mm:ss)\n"  \
                                           " Move intervals      -  %d\n"  \
                                           " Minimum diff.       -  %d (%%)\n"  \
                                           " Value equality      -  %d (%%)\n"

#define XCONS_SHOW_LAST_TASK_MOVES_HEADER  "\n\nLast task moves:\n"                                     \
                                           " %-14.14s  %-5.5s %11.11s   %-3.3s %11.11s   %-3.3s %11.11s\n"  \
                                           " %14.14s  %-5.5s %11.11s   %-3.3s %11.11s   %-3.3s %11.11s\n", \
                                           "Task moved",      "Task",    "Max.",     "Src", "Max.",     "Dst",  "Max.", \
                                           "(ddd:hh:mm:ss)",   "",        "Runnable", "UKT",    "Runnable", "UKT",    "Runnable"
# define XCONS_SHOW_LAST_TASK_MOVES_INFO   " %13.13s   T%-4d %4" SAPDB_INT8_FORMAT ".%06" SAPDB_INT8_FORMAT "   %-3d %4" SAPDB_INT8_FORMAT ".%06" SAPDB_INT8_FORMAT "   %-3d %4" SAPDB_INT8_FORMAT ".%06" SAPDB_INT8_FORMAT "\n"

#define XCONS_SHOW_MOVE_UKT_HEADER         "\n\nUser Kernel Threads:\n"                                            \
                                           " %-3.3s  %-7.7s  %-14.14s  %11.11s  %11.11s  %11.11s\n"             \
                                           " %-3.3s  %-7.7s  %14.14s  %11.11s  %11.11s  %11.11s\n",            \
                                           "UKT",    "Movable",   "Interval start",  "Max.",      "Sum" ,       "UKT", \
                                           "",       "Tasks",     "(ddd:hh:mm:ss)",   "Runnable",  "Runnable"  , "Idle Time"
# define XCONS_SHOW_MOVE_UKT_INFO          " %3d  %-7d  %13.13s   %4" SAPDB_INT8_FORMAT ".%06" SAPDB_INT8_FORMAT "  %4" SAPDB_INT8_FORMAT ".%06" SAPDB_INT8_FORMAT "  %4" SAPDB_INT8_FORMAT ".%06" SAPDB_INT8_FORMAT " \n"

#define XCONS_INFO_LINE                    "-------------------------------------------------------------------------------"

#define XCONS_SHOW_QUEUE_STAT_HEADER       "\nTask Queue Statistics:\n"                           \
                                           "\n%s\n%5s      %9s     %9s    %9s\n%s\n",             \
                                           XCONS_INFO_LINE, "Task ", "UToU", "UKT", "COM", XCONS_INFO_LINE

#define XCONS_SHOW_QUEUE_STAT_INFO         "T%-4d     %10u    %10u   %10u  \n"

#define TASK_COUNT_FORMAT_TWO_COUNTS       "%16s %-10lu   %40s %-lu\n"
#define TASK_COUNT_FORMAT_THREE_COUNTS     "%16s %-8" SAPDB_UINT8_FORMAT " %17s %-8" SAPDB_UINT8_FORMAT " %17s %-" SAPDB_UINT8_FORMAT "\n"
#define FORMAT_CNT_AND_TIME                "%16s %-10" SAPDB_UINT8_FORMAT "   %40s %-3.4f\n"
#define FORMAT_TWO_COUNTS_AND_TIME         "%16s %-8" SAPDB_UINT8_FORMAT " %17s %-8" SAPDB_UINT8_FORMAT " %17s %-3.4f\n"
#define FORMAT_COUNT_AND_TWO_TIMES         "%16s %-8" SAPDB_UINT8_FORMAT " %17s %-8.4f %17s %-8.4f\n"

#define XCONS_SHOW_TASK_COUNT_HEADER \
                                                   "\n%9.9s T%-4d %32.32s ( pid = %-10.10s ) %9.9s\n"
#define XCONS_SHOW_TASK_COUNT_TRAILER              "%s\n",XCONS_INFO_LINE

#define XCONS_SHOW_TASK_COUNT_REM_NODE_AND_PID     "%16s %-30.30s   %20s %s\n"
#define XCONS_SHOW_TASK_COUNT_REM_NODE             "remote_node   :"
#define XCONS_SHOW_TASK_COUNT_REM_PID              "remote_pid    :"

/* PTS 1106969 */
#define XCONS_SHOW_TASK_COUNT_OMS_PROCEDURE        "%16s %-62.62s\n"
#define XCONS_SHOW_TASK_COUNT_OMS_PROCEDURE_NAME   "COM procedure :"

#define XCONS_SHOW_TASK_COUNT_OMS_CALLBACK_STAT    "%16s %-30.30s   %20s %d\n"
#define XCONS_SHOW_TASK_COUNT_OMS_CALLBACK_METHOD  "OMS callback  :"
#define XCONS_SHOW_TASK_COUNT_OMS_CALLBACK_CNT     "callback_cnt  :"

#define XCONS_SHOW_TASK_COUNT_DISP_COMMAND_CNT     "%16s %-10" SAPDB_UINT8_FORMAT "   %40s %-lu\n"
#define XCONS_SHOW_TASK_COUNT_KT_DISP_CALL_CNT     "dispatcher_cnt:"
#define XCONS_SHOW_TASK_COUNT_KT_COMMAND_CNT       "command_cnt   :"

#define XCONS_SHOW_TASK_COUNT_EXCL_SELF_SUSP_CNT   "%16s %-10" SAPDB_UINT8_FORMAT "   %40s %-" SAPDB_UINT8_FORMAT "\n"

#define XCONS_SHOW_TASK_COUNT_KT_EXCLUSIVE_CNT     "exclusive_cnt :"
#define XCONS_SHOW_TASK_COUNT_KT_SELF_SUSP_CNT     "self_susp_cnt :"

#define XCONS_SHOW_TASK_COUNT_WAIT_FOR_DEV         " I/O on volume: %s\n"

#define XCONS_SHOW_TASK_COUNT_PG_PER_IO            "%16s %-10" SAPDB_UINT8_FORMAT "   %40s %-4.1f\n"
#define XCONS_SHOW_TASK_COUNT_IO_CNT               "%16s %-10" SAPDB_UINT8_FORMAT "   %40s %-" SAPDB_UINT8_FORMAT "\n"
#define XCONS_SHOW_TASK_COUNT_ASYN_CNT_PG          "%16s %-10" SAPDB_UINT8_FORMAT "   %40s %-" SAPDB_UINT8_FORMAT "\n"
#define XCONS_SHOW_TASK_COUNT_STATE_CNT_TIME       FORMAT_COUNT_AND_TWO_TIMES
#define XCONS_SHOW_TASK_COUNT_IO_CNT_PG_TIME       FORMAT_TWO_COUNTS_AND_TIME
#define XCONS_SHOW_TASK_COUNT_SELF_R_CNT_AVG_TIME  FORMAT_CNT_AND_TIME
#define XCONS_SHOW_TASK_COUNT_SELF_W_CNT_AVG_TIME  FORMAT_CNT_AND_TIME
#define XCONS_SHOW_TASK_COUNT_DEV_R_CNT_AVG_TIME   FORMAT_CNT_AND_TIME
#define XCONS_SHOW_TASK_COUNT_DEV_W_CNT_AVG_TIME   FORMAT_CNT_AND_TIME
#define XCONS_SHOW_TASK_COUNT_ASYN_R_CNT_AVG_TIME  FORMAT_CNT_AND_TIME
#define XCONS_SHOW_TASK_COUNT_ASYN_W_CNT_AVG_TIME  FORMAT_CNT_AND_TIME

#define XCONS_SHOW_TASK_COUNT_KT_PG_PER_IO         "pages_per_io  :"
#define XCONS_SHOW_TASK_COUNT_KT_SELF_R_CNT        "self_read_io  :"
#define XCONS_SHOW_TASK_COUNT_KT_SELF_R_PG         "self_read_pg  :"
#define XCONS_SHOW_TASK_COUNT_KT_SELF_W_CNT        "self_write_io :"
#define XCONS_SHOW_TASK_COUNT_KT_SELF_W_PG         "self_write_pg :"
#define XCONS_SHOW_TASK_COUNT_KT_SELF_R_AVG_TIME   "avg_self_rd_tm:"
#define XCONS_SHOW_TASK_COUNT_KT_SELF_W_AVG_TIME   "avg_self_wr_tm:"

#define XCONS_SHOW_TASK_COUNT_KT_DEV_R_CNT         "dev_read_io   :"
#define XCONS_SHOW_TASK_COUNT_KT_DEV_R_PG          "dev_read_pg   :"
#define XCONS_SHOW_TASK_COUNT_KT_DEV_W_CNT         "dev_write_io  :"
#define XCONS_SHOW_TASK_COUNT_KT_DEV_W_PG          "dev_write_pg  :"
#define XCONS_SHOW_TASK_COUNT_KT_DEV_R_REL_TIME    "rel_dev_rd_tm :"
#define XCONS_SHOW_TASK_COUNT_KT_DEV_R_ABS_TIME    "abs_dev_rd_tm :"
#define XCONS_SHOW_TASK_COUNT_KT_DEV_W_REL_TIME    "rel_dev_wr_tm :"
#define XCONS_SHOW_TASK_COUNT_KT_DEV_W_ABS_TIME    "abs_dev_wr_tm :"

#define XCONS_SHOW_TASK_COUNT_KT_ASYN_R_CNT        "asyn_read_io  :"
#define XCONS_SHOW_TASK_COUNT_KT_ASYN_R_PG         "asyn_read_pg  :"
#define XCONS_SHOW_TASK_COUNT_KT_ASYN_W_CNT        "asyn_write_io :"
#define XCONS_SHOW_TASK_COUNT_KT_ASYN_W_PG         "asyn_write_pg :"
#define XCONS_SHOW_TASK_COUNT_KT_ASYN_R_REL_TIME   "rel_asyn_rd_tm:"
#define XCONS_SHOW_TASK_COUNT_KT_ASYN_R_ABS_TIME   "abs_asyn_rd_tm:"
#define XCONS_SHOW_TASK_COUNT_KT_ASYN_W_REL_TIME   "rel_asyn_wr_tm:"
#define XCONS_SHOW_TASK_COUNT_KT_ASYN_W_ABS_TIME   "abs_asyn_wr_tm:"

#define XCONS_SHOW_TASK_COUNT_RESUME_CNT " Resume count %-2d total %-10d History ["
#define XCONS_SHOW_TASK_COUNT_RESUME_HISTORY " T%d"
#define XCONS_SHOW_TASK_COUNT_RESUME_END_HISTORY " ]\n"

#define XCONS_SHOW_TASK_COUNT_WAIT_SLEEP_SUSP_CNT  \
                                                   TASK_COUNT_FORMAT_THREE_COUNTS
#define XCONS_SHOW_TASK_COUNT_VWAIT_CNT_AVG_TIME   FORMAT_CNT_AND_TIME
#define XCONS_SHOW_TASK_COUNT_KT_STATE_VWAIT_CNT   "state_vwait   :"
#define XCONS_SHOW_TASK_COUNT_KT_VWAIT_REL_TIME    "rel_wait_time :"
#define XCONS_SHOW_TASK_COUNT_KT_VWAIT_ABS_TIME    "abs_wait_time :"
#define XCONS_SHOW_TASK_COUNT_VSLEEP_CNT_AVG_TIME  FORMAT_CNT_AND_TIME
#define XCONS_SHOW_TASK_COUNT_KT_STATE_VSLEEP_CNT  "state_vsleep  :"
#define XCONS_SHOW_TASK_COUNT_KT_VSLEEP_REL_TIME   "rel_sleep_time:"
#define XCONS_SHOW_TASK_COUNT_KT_VSLEEP_ABS_TIME   "abs_sleep_time:"
#define XCONS_SHOW_TASK_COUNT_VSUSP_CNT_AVG_TIME   FORMAT_CNT_AND_TIME
#define XCONS_SHOW_TASK_COUNT_KT_STATE_VSUSP_CNT   "state_vsusp   :"
#define XCONS_SHOW_TASK_COUNT_KT_VSUSP_REL_TIME    "rel_susp_time :"
#define XCONS_SHOW_TASK_COUNT_KT_VSUSP_ABS_TIME    "abs_susp_time :"
#define XCONS_SHOW_TASK_COUNT_RCV_RPL_INFO         "%16s %-8" SAPDB_UINT8_FORMAT " %17s %-8u %17s %-3.4f\n"
#define XCONS_SHOW_TASK_COUNT_KT_RCV_RPL_CNT       "rcv_rpl_count :"
#define XCONS_SHOW_TASK_COUNT_KT_RCV_RPL_TO_LONG   "rcv_rpl_long  :"
#define XCONS_SHOW_TASK_COUNT_KT_RCV_RPL_AVG_TIME  "avg_rcv_rpl_t :"
#define XCONS_SHOW_TASK_COUNT_RPL_RCV_INFO         FORMAT_CNT_AND_TIME
#define XCONS_SHOW_TASK_COUNT_KT_RPL_RCV_CNT       "rpl_rcv_count :"
#define XCONS_SHOW_TASK_COUNT_KT_RPL_RCV_REL_TIME  "rel_rpl_rcv_t :"
#define XCONS_SHOW_TASK_COUNT_KT_RPL_RCV_ABS_TIME  "abs_rpl_rcv_t :"

#define XCONS_SHOW_TASK_COUNT_PRIO_TOT_OTH         TASK_COUNT_FORMAT_TWO_COUNTS
#define XCONS_SHOW_TASK_COUNT_KT_PRIO_CNT          "prio_total_cnt:"
#define XCONS_SHOW_TASK_COUNT_KT_PRIO_FROM_OTHER   "from_oth_ukt:  "

#define XCONS_SHOW_TOTAL_STAT_HEADER                \
                                                   "\nTotal counter statistics:\n=========================\n\n"

#define TOTAL_COUNT_FORMAT_ONE_COUNT               "%16s %-10u\n"
#define TOTAL_COUNT_FORMAT_TWO_COUNTS              "%16s %-10u   %30s %-u\n"
#define TOTAL_COUNT_FORMAT_TWO_DBL_COUNTS          "%16s %-10.0f   %30s %-.0f\n"

#define XCONS_SHOW_TOTAL_DISP_COMMAND_CNT          "%16s %-10" SAPDB_UINT8_FORMAT "   %40s %-" SAPDB_UINT8_FORMAT "\n"
#define XCONS_SHOW_TOTAL_KT_DISP_CALL_CNT          "total_disp_calls    :"
#define XCONS_SHOW_TOTAL_KT_COMMAND_CNT            "total_commands      :"

#define XCONS_SHOW_TOTAL_KT_STATE_VWAIT_VSUSP_CNT  "%16s %-10" SAPDB_UINT8_FORMAT "   %40s %-" SAPDB_UINT8_FORMAT "\n"
#define XCONS_SHOW_TOTAL_KT_STATE_VWAIT_CNT        "total_state_vwait   :"
#define XCONS_SHOW_TOTAL_KT_STATE_VSUSP_CNT        "total_state_vsuspend:"

#define XCONS_SHOW_TOTAL_KT_STATE_VSLEEP           "%16s %-10" SAPDB_UINT8_FORMAT "\n"
#define XCONS_SHOW_TOTAL_KT_STATE_VSLEEP_CNT       "total_state_vsleep  :"

#define XCONS_SHOW_TOTAL_SELFIO_DEVIO_CNT          "%16s %-10" SAPDB_UINT8_FORMAT "   %40s %-" SAPDB_UINT8_FORMAT "\n"
#define XCONS_SHOW_TOTAL_KT_SELFIO_CNT             "total_self_io       :"
#define XCONS_SHOW_TOTAL_KT_DEVIO_CNT              "total_dev_io        :"

#define XCONS_SHOW_TOTAL_SELFIO_DEVIO_CNT          "%16s %-10" SAPDB_UINT8_FORMAT "   %40s %-" SAPDB_UINT8_FORMAT "\n"
#define XCONS_SHOW_TOTAL_KT_SELFIO_CNT             "total_self_io       :"
#define XCONS_SHOW_TOTAL_KT_DEVIO_CNT              "total_dev_io        :"

#define XCONS_SHOW_TOTAL_REG_ACC_BEGEXCL_COLL      "%16s %-10" SAPDB_UINT8_FORMAT "   %40s %-" SAPDB_UINT8_FORMAT "\n"
#define XCONS_SHOW_TOTAL_KT_REG_ACCESS             "total_region_access :"
#define XCONS_SHOW_TOTAL_KT_BEGEXCL_COLL           "begexcl_tas_coll    :"

#define XCONS_SHOW_TOTAL_REG_COLL_ENDEXCL_COLL     "%16s %-10" SAPDB_UINT8_FORMAT "   %40s %-" SAPDB_UINT8_FORMAT "\n"
#define XCONS_SHOW_TOTAL_KT_REG_COLL               "total_region_coll   :"
#define XCONS_SHOW_TOTAL_KT_ENDEXCL_COLL           "endexcl_tas_coll    :"

#define XCONS_SHOW_TOTAL_KT_REG_WAIT               "%16s %-10" SAPDB_UINT8_FORMAT "\n"
#define XCONS_SHOW_TOTAL_KT_REG_WAIT_CNT           "total_region_wait   :"

#define XCONS_SHOW_TOTAL_PRIO_CNT_AND_OTHER        "%16s %-10u   %40s %-u\n"
#define XCONS_SHOW_TOTAL_KT_PRIO_CNT               "total_prio_calls    :"
#define XCONS_SHOW_TOTAL_KT_PRIO_OTHER             "total_prio_other    :"

#define XCONS_SHOW_TOTAL_SYMBOL_RESOLUTION_LOCKS   "%30s %-10" SAPDB_UINT8_FORMAT "\n"
#define XCONS_SHOW_TOTAL_SYMBOL_RESOLUTION_LOCK_CNT "total_symbol_resolution_locks:"

#define XCONS_SHOW_UKT_SLEEP_STAT_HEADER \
                                                   "\n%s\n            Idle   dd:hh:mm:ss             I/O Wait   dd:hh:mm:ss         RQlen\n%s\n", \
                                                   XCONS_INFO_LINE , \
                                                   XCONS_INFO_LINE

#define XCONS_SHOW_UKT_SLEEP_STAT_INFO             "UKT%-2d %10" SAPDB_UINT8_FORMAT "  %s  %6.2f%%  %10" SAPDB_UINT8_FORMAT "  %s  %6.2f%% %4lu\n"

#define XCONS_SHOW_SUSPEND_HEADER \
                                                   "\nList of suspend reasons:\n%s\n","========================"
#define XCONS_SHOW_SUSPEND_TOTAL                   "\ntotal suspends: %7lu\n\n"
#define XCONS_SHOW_SUSPEND_CNT_PERCENT             "%s : %7lu ( %6.2f%% ) %s\n"
 
#define XCONS_SHOW_QUEUES_HEAD                     "UKT%d tid 0x%x \n"
#define XCONS_SHOW_UKT_QUEUE_HEAD                  "    UKTQueue: \n"
#define XCONS_SHOW_UKT_QUEUE_ELEMENT               "          task T%d \n"
#define XCONS_SHOW_UKT_QUEUE_PRIO_ELEMENT          "          task T%d, prio %d\n"
#define XCONS_SHOW_COM_QUEUE_HEAD                  "    COMQueue: \n"
#define XCONS_SHOW_COM_QUEUE_ELEMENT               "          task T%d \n"
#define XCONS_SHOW_COM_QUEUE_PRIO_ELEMENT          "          task T%d, prio %d\n"
#define XCONS_SHOW_RUN_QUEUE_HEAD                  "    RUNQueue: \n"
#define XCONS_SHOW_RUN_QUEUE_ELEMENT               "          task T%-3d (%s), dyn_prio %d, %s\n"
#define XCONS_SHOW_RUN_QUEUE_ELEMENT_REQ           "          task T%-3d (%s), dyn_prio %d, %s %s\n"

#define XCONS_SHOW_THREAD_TIMES_HEADER     "\n%s\n %-6.6s%8.8s  %-28.28s %-15.15s   %-15.15s\n"        \
                                               " %-6.6s%8.8s  %-28.28s %-15.15s   %-15.15s\n%s\n",     \
                                           XCONS_INFO_LINE, "Thread", "Win", "Task",   "   System", "   User" , \
                                           "Name",   "Tid", "Cluster"," d hh:mm:ss:ms"," d hh:mm:ss:ms", XCONS_INFO_LINE

#define XCONS_FORMAT_SHOW_THREAD_TIMES     " %-6.6s%8d  %-28.28s %2.2s %2d:%02d:%02d:%03d   %2.2s %2d:%02d:%02d:%03d\n"
#define XCONS_FORMAT_SHOW_THREAD_TOT_TIMES "%s\n %-44.44s %2.2s %2d:%02d:%02d:%03d   %2.2s %2d:%02d:%02d:%03d\n", \
                                           XCONS_INFO_LINE,"total time used by shown threads: " 

/* --- SERVERDB states */

#define XCONS_SHOW_STATE_INFO              "\nThe SERVERDB state is %s\n"
#define XCONS_SHOW_STATE_INFO_TERM_TIMEOUT "\nThe SERVERDB state is %s  - Termination timeout: %d\n"
#define XCONS_SHOW_STATE_STARTING          "STARTING      "
#define XCONS_SHOW_STATE_STANDBY           "STANDBY       "
#define XCONS_SHOW_STATE_COLD              "ADMIN         "
#define XCONS_SHOW_STATE_WARM              "ONLINE        "
#define XCONS_SHOW_STATE_SHTDWN            "SHUTDOWN      "
#define XCONS_SHOW_STATE_SHTDWNREI         "SHUTDOWNREINIT"
#define XCONS_SHOW_STATE_SHTDWNKIL         "SHUTDOWNKILL  "
#define XCONS_SHOW_STATE_STOP              "STOP          "
#define XCONS_SHOW_STATE_KILL              "KILL          "
#define XCONS_SHOW_STATE_ABORT             "ABORT         "
#define XCONS_SHOW_STATE_UNKNOWN           "UNKNOWN       "
#define XCONS_SHOW_STATE_OFFLINE           "OFFLINE       "


/* --- task states */

#define XCONS_TSK_STATE_INITIALIZE         "Not Connected "
#define XCONS_TSK_STATE_INACTIVE           "Inactive      "
#define XCONS_TSK_STATE_MARKED_FOR_STRT    "Mark for Start"
#define XCONS_TSK_STATE_RUNNING            "Running       "
#define XCONS_TSK_STATE_CONNECTWAIT        "Connect wait  "
#define XCONS_TSK_STATE_VDEVSIZE           "Vdevsize      "
#define XCONS_TSK_STATE_VATTACH            "Vattach       "
#define XCONS_TSK_STATE_VDETACH            "Vdetach       "
#define XCONS_TSK_STATE_VFOPEN             "Vfopen        "
#define XCONS_TSK_STATE_VFWRITE            "Vfwrite       "
#define XCONS_TSK_STATE_VFCLOSE            "Vfclose       "
#define XCONS_TSK_STATE_VRELEASE           "Vrelease      "
#define XCONS_TSK_STATE_VSHUTDOWN          "Vshutdown     "
#define XCONS_TSK_STATE_VRECEIVE           "Command wait  "
#define XCONS_TSK_STATE_VREPLY             "Command reply "
#define XCONS_TSK_STATE_VBEGEXCL           "Vbegexcl      "
#define XCONS_TSK_STATE_VENDEXCL           "Vendexcl      "
#define XCONS_TSK_STATE_VSUSPEND           "Vsuspend      "
#define XCONS_TSK_STATE_VWAIT              "Vwait         "
#define XCONS_TSK_STATE_VSLEEP             "Vsleep        "
#define XCONS_TSK_STATE_VDIAGINIT          "Diaginit      "
#define XCONS_TSK_STATE_VOPMSG             "Vopmsg        "
#define XCONS_TSK_STATE_LOCKED             "Locked        "
#define XCONS_TSK_STATE_TERMINATED         "Terminated    "
#define XCONS_TSK_STATE_RUNNABLE           "Runnable      "
#define XCONS_TSK_STATE_ASYNOPEN           "AsynOpen      "
#define XCONS_TSK_STATE_ASYNCLOSE          "AsynClose     "
#define XCONS_TSK_STATE_ASYNIO             "AsynIO        "
#define XCONS_TSK_STATE_ASYNWAIT_READ      "AsynWaitRead  "
#define XCONS_TSK_STATE_ASYNWAIT_WRITE     "AsynWaitWrite "
#define XCONS_TSK_STATE_IO_READ            "IO Wait (R)   "
#define XCONS_TSK_STATE_IO_WRITE           "IO Wait (W)   "
#define XCONS_TSK_STATE_DCOM_OBJ_CALLED    "DcomObjCalled "
#define XCONS_TSK_STATE_RESCHEDULE_MSEC    "RescheduleMsec"
#define XCONS_TSK_STATE_ASYNCNTL           "Asyncntl      "
#define XCONS_TSK_STATE_INSERT_EVENT       "InsertEvent   "
#define XCONS_TSK_STATE_WAIT_FOR_EVENT     "WaitForEvent  "
#define XCONS_TSK_STATE_STOPPED            "Stopped       "
#define XCONS_TSK_STATE_VVECTORIO          "Vvectorio     "
#define XCONS_TSK_STATE_VDUALVECTORIO      "Vdualvectorio "
#define XCONS_TSK_STATE_VBEGEXCL           "Vbegexcl      "
#define XCONS_TSK_STATE_VENTERRWREGION     "VenterRWRegion"
#define XCONS_TSK_STATE_VLEAVERWREGION     "VleaveRWRegion"
#define XCONS_TSK_STATE_YIELDING_ON_SPINLOCK "SpinlockYield "
#define XCONS_TSK_STATE_UNKNOWN            "UNKNOWN       "


/* --- KT/UKT states */

#define XCONS_KT_STATE_INITIALIZED         "Initialized"
#define XCONS_KT_STATE_RUNNING             "Running    "
#define XCONS_KT_STATE_TERMINATING         "Terminating"
#define XCONS_KT_STATE_TERMINATED          "Terminated "
#define XCONS_KT_STATE_SLEEPING            "Sleeping   "
#define XCONS_KT_STATE_DETACHED            "Detached   "
#define XCONS_KT_STATE_ATTACHED            "Attached   "
#define XCONS_KT_STATE_UNKNOWN             "UNKNOWN"

/* --- task request types */

#define XCONS_REQ_VATTACH                     "REQ_VATTACH"
#define XCONS_REQ_VBLOCKIO                    "REQ_VBLOCKIO"
#define XCONS_REQ_VDETACH                     "REQ_VDETACH"
#define XCONS_REQ_INITOK                      "REQ_INITOK"
#define XCONS_REQ_VRESUME                     "REQ_VRESUME"
#define XCONS_REQ_VSIGNAL                     "REQ_VSIGNAL"
#define XCONS_REQ_VSLEEP                      "REQ_VSLEEP"
#define XCONS_REQ_CONNECT                     "REQ_CONNECT"
#define XCONS_REQ_VRECEIVE                    "REQ_VRECEIVE"
#define XCONS_REQ_SHUTDOK                     "REQ_SHUTDOK"
#define XCONS_REQ_SENDER_JOB_FREE             "REQ_SENDER_JOB_FREE"
#define XCONS_REQ_VENDEXCL                    "REQ_VENDEXCL"
#define XCONS_REQ_RESCHEDULE                  "REQ_RESCHEDULE"
#define XCONS_REQ_SHUTDOWN_RESUME_TW          "REQ_SHUTDOWN_RESUME_TW"
#define XCONS_REQ_ASYNOPEN                    "REQ_ASYNOPEN"
#define XCONS_REQ_ASYNCLOSE                   "REQ_ASYNCLOSE"
#define XCONS_REQ_ASYNIO                      "REQ_ASYNIO"
#define XCONS_REQ_ASYNWAIT                    "REQ_ASYNWAIT"
#define XCONS_REQ_VNSHUTDOWN_REPLY            "REQ_VNSHUTDOWN_REPLY"
#define XCONS_REQ_OUTGOING_CONNECT            "REQ_OUTGOING_CONNECT"
#define XCONS_REQ_OUTGOING_CONNECT_REPLY      "REQ_OUTGOING_CONNECT_REPLY"
#define XCONS_REQ_OPEN_WAKEUP_SD              "REQ_OPEN_WAKEUP_SD"
#define XCONS_REQ_CONNECT_RC                  "REQ_CONNECT_RC"
#define XCONS_REQ_RELEASE_RC                  "REQ_RELEASE_RC"
#define XCONS_REQ_CLOSE_OUTGOING              "REQ_CLOSE_OUTGOING"
#define XCONS_REQ_CLOSE_OUTGOING_REPLY        "REQ_CLOSE_OUTGOING_REPLY"
#define XCONS_REQ_FINISH_SV                   "REQ_FINISH_SV"
#define XCONS_REQ_ASYNCNTL                    "REQ_ASYNCNTL"
#define XCONS_REQ_VMARK_BAD_DEV               "REQ_VMARK_BAD_DEV"
#define XCONS_REQ_TEST_AND_SET                "REQ_TEST_AND_SET"
#define XCONS_REQ_BIG_CONNECT                 "REQ_BIG_CONNECT"
#define XCONS_REQ_ADDRESS_CONNECT             "REQ_ADDRESS_CONNECT"
#define XCONS_REQ_BIG_ADDR_CONNECT            "REQ_BIG_ADDR_CONNECT"
#define XCONS_REQ_EXCLUSIVE                   "REQ_EXCLUSIVE"
#define XCONS_REQ_VNSHUTDOWN                  "REQ_VNSHUTDOWN"
#define XCONS_REQ_IO_FINISHED                 "REQ_IO_FINISHED"
#define XCONS_REQ_DCOM_CREATE_DCOMI           "REQ_DCOM_CREATE_DCOMI"
#define XCONS_REQ_DCOM_REMOVE_DCOMI           "REQ_DCOM_REMOVE_DCOMI"
#define XCONS_REQ_DCOM_PARAMINFO			  "REQ_DCOM_PARAMINFO"
#define XCONS_REQ_DCOM_CO_CREATE_INSTANCE     "REQ_DCOM_CO_CREATE_INSTANCE"
#define XCONS_REQ_DCOM_RELEASE_INSTANCE		  "REQ_DCOM_RELEASE_INSTANCE"
#define XCONS_REQ_DCOM_CO_GET_CLASSOBJECT     "REQ_DCOM_CO_GET_CLASSOBJECT"
#define XCONS_REQ_DCOM_TRANSFER			      "REQ_DCOM_TRANSFER"
#define XCONS_REQ_DCOM_RELASE_ALL			  "REQ_DCOM_RELASE_ALL"
#define XCONS_REQ_VSTOP                       "REQ_VSTOP"
#define XCONS_REQ_VVECTORIO                   "REQ_VVECTORIO"
#define XCONS_REQ_TYPE_UNKNOWN                "REQ_UNKNOWN"
#define XCONS_REQ_YIELDING                    "REQ_YIELDING"

/* --- task storage descriptions */

#define XCONS_STOR_DES_CODESIZE            "CODESIZE"
#define XCONS_STOR_VDES_CODESIZE           "KB"
#define XCONS_STOR_DES_TSK_STACK_SIZE      "TASK_STACKSIZE"
#define XCONS_STOR_VDES_TSK_STACK_SIZE     "KB"
#define XCONS_STOR_DES_KGS                 "KERNEL_GLOB_SECTION"
#define XCONS_STOR_VDES_KGS                "KB"
#define XCONS_STOR_DES_KSS                 "KERNEL_SHRD_SECTION"
#define XCONS_STOR_VDES_KSS                "KB"
#define XCONS_STOR_DES_SHRD_DYN_POOLFREE   "UNUSED_SHARED_DYN_POOL"
#define XCONS_STOR_VDES_SHRD_DYN_POOLFREE  "KB"
#define XCONS_STOR_DES_SHRD_DYN_POOL_USED  "USED_SHARED_DYN_POOL"
#define XCONS_STOR_VDES_SHRD_DYN_POOL_USED "KB"
#define XCONS_STOR_DES_SHRD_DYN_DATFREE    "UNUSED_SHARED_DYN_DATA"
#define XCONS_STOR_VDES_SHRD_DYN_DATFREE   "PAGES"
#define XCONS_STOR_DES_SHRD_DYN_DAT_USED   "USED_SHARED_DYN_DATA"
#define XCONS_STOR_VDES_SHRD_DYN_DAT_USED  "PAGES"
#define XCONS_STOR_DES_RTE_ALL_POOL_USED   "USED_RTE_ALLOC_POOL"
#define XCONS_STOR_VDES_RTE_ALL_POOL_USED  "KB"
#define XCONS_STOR_DES_RTE_ALL_POOL_MAX    "MAX_RTE_ALLOC_POOL"
#define XCONS_STOR_VDES_RTE_ALL_POOL_MAX   "MB"
#define XCONS_STOR_DES_KRN_ALL_POOL_USED   "USED_KERNEL_ALLOC_POOL"
#define XCONS_STOR_VDES_KRN_ALL_POOL_USED  "KB"
#define XCONS_STOR_DES_KRN_ALL_POOL_MAX    "MAX_KERNEL_ALLOC_POOL"
#define XCONS_STOR_VDES_KRN_ALL_POOL_MAX   "KB"
#define XCONS_STOR_DES_EXISTING_TASKS      "EXISTING_TASKS"
#define XCONS_STOR_VDES_EXISTING_TASKS     ""

/* --- task rte descriptions */

#define XCONS_RTE_DESC_DEV_VTRACE          "DEV-VTRACE"
#define XCONS_RTE_DESC_DEV                 "DEV"
#define XCONS_RTE_DESC_UKT                 "UKT%-7d"

/* --- task version descriptions */
#define XCONS_VERS_DESC_KERNEL             "KERNEL"
#define XCONS_VERS_DESC_RTE                "RTE"


#endif  /* RTE_CONSOLESTANDARDOUTPUT_H */
