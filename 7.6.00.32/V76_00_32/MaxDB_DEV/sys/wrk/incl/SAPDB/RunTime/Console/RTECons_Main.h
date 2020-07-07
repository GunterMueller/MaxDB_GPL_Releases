/*!
  @file           RTECons_Main.h
  @author         StefanP
  @brief          Console (XCons) - Main Module
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



#ifndef RTECONS_MAIN_H
#define RTECONS_MAIN_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#if defined(_WIN32)
  #define XCONS_SYS                        "Win"
#else
  #define XCONS_SYS                        "UNIX"
#endif

#if defined(_WIN32)

#define XCONS_USAGE                        "usage: x_cons [<serverdb>] <Command> [<Interval> [<Repeat>]]\r\n" \
                                           "\r\n"                                                            \
                                           "    SERVERDB                         ::   <serverdb>\n"             \
                                           "\r\n"                                                            \
                                           "    Commands: \r\n"                                           \
                                           "      time measurement               ::  TIME   <ENABLE | DISABLE> \r\n"   \
                                           "      cancel the command of task     ::  CANCEL <taskindex>\r\n"   \
                                           "      release cached system pages    ::  PAGECACHE_RELEASE\r\n"   \
                                           "      diagnose system page cache     ::  DIAGNOSE_PAGECACHE\r\n"   \
                                           "      kill the session of task       ::  KILL <taskindex>  \r\n"   \
                                           "      set UKT affinity               ::  AFFINITY <uktindex>\r\n"  \
                                           "      set processor affinity mask    ::  PROCMASK <procmask> [ DW | SV | US | GC]\r\n"  \
                                           "      flush trace file               ::  TRACE_FLUSH       \r\n"   \
                                           "      show statistics/states         ::  SHOW IO           \r\n"   \
                                           "                                     ::  SHOW AIO       (backup only) \r\n"   \
                                           "                                     ::  SHOW STORAGE      \r\n"   \
                                           "                                     ::  SHOW TASKS        \r\n"   \
                                           "                                     ::  SHOW ACTIVE    [ DW | SV | US | GC]\r\n"   \
                                           "                                     ::  SHOW RUNNABLE  [ DW | SV | US | GC]\r\n"   \
                                           "      show move info (load balancing)::  SHOW MOVEINFO     \r\n"   \
                                           "      show task counts               ::  SHOW T_CNT     [ DW | SV | US | GC]\r\n"   \
                                           "      show task queues               ::  SHOW T_QUEUE      \n"   \
                                           "      show tasks move info           ::  SHOW T_MOVE       \r\n"   \
                                           "                                         SHOW VERSIONS     \r\n"   \
                                           "                                         SHOW REGIONS      \r\n"   \
                                           "                                         SHOW STATE        \r\n"   \
                                           "                                         SHOW RTE          \r\n"   \
                                           "                                         SHOW QUEUES       \r\n"   \
                                           "      suspend reasons                ::  SHOW SUSPENDS     \r\n"   \
                                           "      UKT sleep statistic            ::  SHOW SLEEP        \r\n"   \
                                           "      Thread time usage              ::  SHOW THRD_TIMES   \r\n"   \
                                           "                                         SHOW ALL          \r\n"

#else

#define XCONS_USAGE                        "usage: x_cons [<serverdb>] <Command> [<Interval> [<Repeat>]]\n" \
                                           "\n"                                                            \
                                           " SERVERDB                    ::  <serverdb>\n"             \
                                           "\n"                                                            \
                                           " Commands: \n"                                           \
                                           "  time measurement           ::  TIME   <ENABLE | DISABLE> \n"   \
                                           "  cancel the command of task ::  CANCEL <taskindex>\n"   \
                                           "  release cached system pages::  PAGECACHE_RELEASE\n"   \
                                           "  diagnose system page cache ::  DIAGNOSE_PAGECACHE\n"   \
                                           "  kill the session of task   ::  KILL <taskindex>  \n"   \
                                           "  flush trace file           ::  TRACE_FLUSH       \n" \
                                           "  show statistics/states     ::  SHOW IO           \n"   \
                                           "                                 SHOW AIO       (backup only) \n"   \
                                           "                                 SHOW DEV_IO    (volume only)\n"   \
                                           "                                 SHOW STORAGE      \n"   \
                                           "                                 SHOW TASKS     [ DW | SV | US ]\n"   \
                                           "     alternative for TASKS   ::  SHOW PROCESSES [ DW | SV | US ]\n"   \
                                           "                                 SHOW ACTIVE    [ DW | SV | US ]\n"   \
                                           "                                 SHOW RUNNABLE  [ DW | SV | US ]\n"   \
                                           "  show move info (load bal.) ::  SHOW MOVEINFO     \n"   \
                                           "  show task counts           ::  SHOW T_CNT     [ DW | SV | US | T<taskindex>]\n"   \
                                           "  show task queues           ::  SHOW T_QUEUE      \n"   \
                                           "  show task regions          ::  SHOW T_REG        \n"   \
                                           "  show task statistics       ::  SHOW T_STAT       \n"   \
                                           "  show tasks move info       ::  SHOW T_MOVE       \n"   \
                                           "                                 SHOW VERSIONS     \n"   \
                                           "                                 SHOW REGIONS      \n"   \
                                           "                                 SHOW STATE        \n"   \
                                           "                                 SHOW RTE          \n"   \
                                           "                                 SHOW QUEUES       \n"   \
                                           "  suspend reasons            ::  SHOW SUSPENDS     \n"   \
                                           "  UKT sleep statistic        ::  SHOW SLEEP        \n"   \
                                           "  Thread stacks              ::  SHOW TSTACK       \n"   \
                                           "                                 SHOW ALL          \n"

#endif


#define XCONS_SHOW_VERS_KERNEL             "\nKernel Version  '%s'\n"
#define XCONS_SHOW_VERS_RTE                "RTE    Version  '%s'\n"

#define XCONS_CMD_SHOW                     "SHOW      "
#define XCONS_CMD_UPDATE                   "UPDATE    "
#define XCONS_CMD_CANCEL                   "CANCEL    "
#define XCONS_CMD_KILL                     "KILL      "
#define XCONS_CMD_DROP                     "DROP      "
#define XCONS_CMD_HELP1                    "HELP      "
#define XCONS_CMD_HELP2                    "?         "
#define XCONS_CMD_TOP                      "TOP       "
#define XCONS_CMD_TIME_MEASURE             "TIME      "
#define XCONS_CMD_RESET_CNT                "RESET     "
#define XCONS_CMD_GETPARAM                 "GETPARAM  "
/*#define XCONS_CMD_PUTPARAM                 "PUTPARAM  "*/
#define XCONS_CMD_MINI                     "MINI      "
#define XCONS_CMD_CRASH                    "CRASH     "
#define XCONS_CMD_DUMP                     "DUMP      "
#define XCONS_CMD_ENABLE                   "ENABLE    "
#define XCONS_CMD_DISABLE                  "DISABLE   "
#define XCONS_CMD_DEBUGLEVEL               "DEBUGLEVEL"
#define XCONS_CMD_PROCESSOR_MASK           "PROCMASK  "
#define XCONS_CMD_UKT_AFFINITY             "AFFINITY  "
#define XCONS_CMD_ERRIOR                   "ERRIOR    "
#define XCONS_CMD_ERRIOW                   "ERRIOW    "
#define XCONS_CMD_DEBUG_BREAK              "DEBUGBREAK"
#define XCONS_CMD_DEBUG_TASK               "DEBUGTASK "
#define XCONS_CMD_SET_LB_CHK               "LD_BAL_CHK"
#define XCONS_CMD_TRACE_FLUSH              "TRACE_FLUSH"
#define XCONS_CMD_TRACELEVEL               "TRACELEVEL"
#define XCONS_CMD_PAGECACHE_RELEASE        "PAGECACHE_RELEASE"
#define XCONS_CMD_DIAGNOSE_PAGECACHE       "DIAGNOSE_PAGECACHE"


#define XCONS_MIN_CMD_LEN_SHOW             1
#define XCONS_MIN_CMD_LEN_UPDATE           1
#define XCONS_MIN_CMD_LEN_CANCEL           2
#define XCONS_MIN_CMD_LEN_KILL             1
#define XCONS_MIN_CMD_LEN_DROP             1
#define XCONS_MIN_CMD_LEN_HELP1            1
#define XCONS_MIN_CMD_LEN_HELP2            1
#define XCONS_MIN_CMD_LEN_TIME_MEASURE     2
#define XCONS_MIN_CMD_LEN_RESET_CNT        2
#define XCONS_MIN_CMD_LEN_GETPARAM         8
/*#define XCONS_MIN_CMD_LEN_PUTPARAM         8*/
#define XCONS_MIN_CMD_LEN_MINI             4
#define XCONS_MIN_CMD_LEN_CRASH            5
#define XCONS_MIN_CMD_LEN_DUMP             4
#define XCONS_MIN_CMD_LEN_ENABLE           1
#define XCONS_MIN_CMD_LEN_DISABLE          2
#define XCONS_MIN_CMD_LEN_DEBUGLEVEL       2
#define XCONS_MIN_CMD_LEN_UKT_AFFINITY     2
#define XCONS_MIN_CMD_LEN_PROCESSOR_MASK   3
#define XCONS_MIN_CMD_LEN_ERRIO            6
#define XCONS_MIN_CMD_LEN_DEBUG_BREAK      10
#define XCONS_MIN_CMD_LEN_DEBUG_TASK       9 
#define XCONS_MIN_CMD_SET_LB_CHK_TASK      2
#define XCONS_MIN_CMD_LEN_TRACE_FLUSH      5
#define XCONS_MIN_CMD_LEN_TRACELEVEL       6
#define XCONS_MIN_CMD_LEN_PAGECACHE_RELEASE  2
#define XCONS_MIN_CMD_LEN_DIAGNOSE_PAGECACHE 2

#define XCONS_CMD_IO                       "IO        "
#define XCONS_CMD_AIO                      "AIO       "
#define XCONS_CMD_STORAGE                  "STORAGE   "
#define XCONS_CMD_TASKS                    "TASKS     "
#define XCONS_CMD_ACTIVE_TASKS             "ACTIVE    "
#define XCONS_CMD_RUNNABLE_TASKS           "RUNNABLE  "
#define XCONS_CMD_VERSIONS                 "VERSIONS  "
#define XCONS_CMD_REGIONS                  "REGIONS   "
#define XCONS_CMD_STATE                    "STATE     "
#define XCONS_CMD_RTE                      "RTE       "
#define XCONS_CMD_ALL                      "ALL       "
#define XCONS_CMD_TABLES                   "TABLES    "
#define XCONS_CMD_TASK_STAT                "T_STAT    "
#define XCONS_CMD_TASK_COUNTS              "T_CNT     "
#define XCONS_CMD_QUEUE_STAT               "T_QUEUE   "
#define XCONS_CMD_TOTAL_COUNTS             "TOTAL     "
#define XCONS_CMD_UKT_SLEEP_STAT           "SLEEP     "
#define XCONS_CMD_SUSPENDS                 "SUSPENDS  "
#define XCONS_CMD_UKT_QUEUES               "QUEUES    "
#define XCONS_CMD_SET                      "SET       "
#define XCONS_CMD_CLEAR                    "CLEAR     "
#define XCONS_CMD_THREAD_TIMES             "THRD_TIMES"
#define XCONS_CMD_AWE                      "AWE       "
#define XCONS_CMD_CONNECTIONS              "CONNECTIONS"
#define XCONS_CMD_MOVE_INFO                "MOVEINFO  "
#define XCONS_CMD_TASK_MOVE                "T_MOVE    "
#define XCONS_CMD_RWREGIONS                "RWREGIONS "

#define XCONS_CMD_TRACE_XCONS              "XCONS     "
#define XCONS_CMD_TRACE_CONSOLE            "CONSOLE   "

#define XCONS_MIN_CMD_LEN_IO               2
#define XCONS_MIN_CMD_LEN_AIO              3
#define XCONS_MIN_CMD_LEN_STORAGE          3
#define XCONS_MIN_CMD_LEN_TASKS            1
#define XCONS_MIN_CMD_LEN_ACTIVE_TASKS     2
#define XCONS_MIN_CMD_LEN_RUNNABLE_TASKS   2
#define XCONS_MIN_CMD_LEN_VERSIONS         1
#define XCONS_MIN_CMD_LEN_REGIONS          1
#define XCONS_MIN_CMD_LEN_STATE            3
#define XCONS_MIN_CMD_LEN_RTE              1
#define XCONS_MIN_CMD_LEN_ALL              2
#define XCONS_MIN_CMD_LEN_TABLES           6
#define XCONS_MIN_CMD_LEN_TASK_STAT        3
#define XCONS_MIN_CMD_LEN_TASK_COUNTS      3
#define XCONS_MIN_CMD_LEN_QUEUE_STAT       3
#define XCONS_MIN_CMD_LEN_TOTAL_COUNTS     2
#define XCONS_MIN_CMD_LEN_UKT_SLEEP_STAT   2
#define XCONS_MIN_CMD_LEN_SUSPENDS         2
#define XCONS_MIN_CMD_LEN_UKT_QUEUES       1
#define XCONS_MIN_CMD_LEN_SET              1
#define XCONS_MIN_CMD_LEN_CLEAR            1
#define XCONS_MIN_CMD_LEN_THREAD_TIMES     2
#define XCONS_MIN_CMD_LEN_AWE              3
#define XCONS_MIN_CMD_LEN_CONNECTIONS      2
#define XCONS_MIN_CMD_LEN_TASK_MOVE        3
#define XCONS_MIN_CMD_LEN_MOVE_INFO        4
#define XCONS_MIN_CMD_LEN_TRACE_XCONS      2
#define XCONS_MIN_CMD_LEN_TRACE_CONSOLE    4
#define XCONS_MIN_CMD_LEN_RWREGIONS        2


#define COM_UNDEF           0
#define COM_CANCEL          1
#define COM_KILL            2
#define COM_SHOW_UPDATE     3
#define COM_HELP            4
#define COM_DROP            5
#define COM_TIME_MEASURE    6
#define COM_RESET           7
#define COM_GETPARAM        8
/*#define COM_PUTPARAM        9*/
#define COM_CRASH_DUMP     10
#define COM_SET_DEBUGLEVEL 11
#define COM_UKT_AFFINITY   12
#define COM_ERRIOR         13
#define COM_ERRIOW         14
#define COM_DEBUG_BREAK    15
#define COM_PROCESSOR_MASK 16
#define COM_DEBUG_TASK     17
#define COM_SET_LB_CHK     18
#define COM_TRACE_FLUSH    19
#define COM_TRACELEVEL     20
#define COM_PAGECACHE_RELEASE  21
#define COM_DIAGNOSE_PAGECACHE 22
#define COM_MINI_DUMP          27


#define PARA_NONE         0x00
#define PARA_ENABLE       0x01
#define PARA_DISABLE      0x02

#define PARA_IO             0x01
#define PARA_STORAGE        0x02
#define PARA_TASKS          0x04
#define PARA_VERSIONS       0x08
#define PARA_REGIONS        0x10
#define PARA_STATE          0x20
#define PARA_RTE            0x40
#define PARA_TASK_STAT      0x100
#define PARA_TASK_COUNTS    0x200
#define PARA_QUEUE_STAT     0x400
#define PARA_TOTAL_COUNTS   0x800
#define PARA_ACTIVE         0x1000
#define PARA_UKT_SL_STAT    0x2000
#define PARA_SUSPENDS       0x4000
#define PARA_UKT_QUEUES     0x8000
#define PARA_RUNNABLE       0x10000
#define PARA_AIO            0x20000
#define PARA_THREAD_TIMES   0x40000
#define PARA_AWE            0x80000
#define PARA_CONNECTIONS    0x100000
#define PARA_TASK_MOVE      0x200000
#define PARA_MOVE_INFO      0x400000
#define PARA_TRACE_XCONS    0x1000000
#define PARA_TRACE_CONSOLE  0x2000000
#define PARA_RWREGIONS      0x4000000

#define PARA_SET          0x01
#define PARA_CLEAR        0x02

#define PARA_DEFAULT     PARA_IO | PARA_TASKS | PARA_VERSIONS

#if defined ( ALL_WITH_AWE )
# define PARA_ALL          ( 0xFFFFFFFF - PARA_ACTIVE - PARA_RUNNABLE )
#else
# define PARA_ALL          ( 0xFFFFFFFF - PARA_ACTIVE - PARA_RUNNABLE - PARA_AWE )
#endif


/* Test: */
#define XCONS_CMD_TEST01            "TEST01    "
#define XCONS_CMD_TEST02            "TEST02    "
#define XCONS_CMD_TEST03            "TEST03    "
#define COM_TEST01                  30
#define COM_TEST02                  31
#define COM_TEST03                  32

#define XCONS_MIN_CMD_LEN_TEST01    6
#define XCONS_MIN_CMD_LEN_TEST02    6
#define XCONS_MIN_CMD_LEN_TEST03    6

#define ARGS(max, cnt)              (((max)-(cnt)) >= 1)

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
typedef struct 
{
    SAPDB_UInt4                     lCommand;
    SAPDB_Long                      lParam;
    SAPDB_Long                      lValue;
    SAPDB_Char *                    pszParam;
    SAPDB_Long                      lExtParam;
    SAPDB_Long                      lInterval;
    SAPDB_Long                      lRepeatCount;
    RTE_DBName                      serverDB;
} RTE_ConsoleRequestRecord;


/*===========================================================================*
 *  EXTERN "C" PROTOTYPES                                                    *
 *===========================================================================*/


#if !defined(externC)
# define externC extern "C"
#endif  


/*!
  @brief Console Session C-function

  C function used to gain a standard session to the console thread 

  For function description refer to class RTECons_StandardSession.
*/

externC SAPDB_Bool  RTECons_ProcessStandardRequest  (int    argc, char  *argv[]);

/*!
  @brief retrive x_cons build number as C-string
 */
externC  char*      XConsBuildNumber                (void);

#endif  /* RTECONS_MAIN_H */
