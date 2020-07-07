/*!
  @file           geo00_2.h
  @author         JoergM
  @special area   Kernel Request types, Task types and Task states
  @brief          Kernel internal used definitions
  @see            example.html ...

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



#ifndef GEO00_2_H
#define GEO00_2_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/* --- Values for database state */
#define SERVER_UNKOWN_EO00                   0
#define SERVER_OFFLINE_EO00                  20
#define SERVER_STARTING_EO00                 21
#define SERVER_ADMIN_EO00                    22
#define SERVER_STANDBY_EO00                  23
#define SERVER_ONLINE_EO00                   24
/* --- only the termination states should be greater or equal 'SERVER_SHUTDOWN'!!!!! */
#define SERVER_SHUTDOWN_EO00                 25 /* -- 'vshutdown' normal shutdown */
#define SERVER_SHUTDOWNREINIT_EO00           26 /* -- 'vshutdown' */
#define SERVER_SHUTDOWNKILL_EO00             27 /* -- 'vshutdown' */
#define SERVER_STOP_EO00                     28 /* -- 'XSTOP' or external kill if database was in cold state */
#define SERVER_KILL_EO00                     29 /* -- 'XSTOP' or external kill if database was in warm state */
#define SERVER_ABORT_EO00                    30 /* -- 'vabort' or any other critcal exception */
#define SERVER_STOPPED_EO00                  31
#define SERVER_UNDEFINED_EO00                0xFFFFFFFF

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   task types 
     PLEASE ALSO ALTER THE EQUIVALENT IN RTETASK_COMMONQUEUEELEMENT.HPP!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* --- Request types */
#define REQ_VATTACH_EO00	                      1
#define REQ_VBLOCKIO_EO00                         2
#define REQ_VDETACH_EO00                          3
#define REQ_INITOK_EO00                           4
#define REQ_VRESUME_EO00                          5
#define REQ_VSIGNAL_EO00                          6
#define REQ_VSLEEP_EO00                           7
#define REQ_CONNECT_EO00                          8
#define REQ_VRECEIVE_EO00                        10
#define REQ_SHUTDOK_EO00                         11
#define REQ_SENDER_JOB_FREE_EO00                 12
#if defined(_WIN32)
#define REQ_VENDEXCL_EO00                        13
#define REQ_RESCHEDULE_EO00                      14
#define REQ_SHUTDOWN_RESUME_TW_EO00              15
#else
#define REQ_BIG_CONNECT_EO00                     13
#define REQ_ADDRESS_CONNECT_EO00                 14
#define REQ_BIG_ADDR_CONNECT_EO00                15
#endif /* _WIN32 */
#define REQ_ASYNOPEN_EO00                        16
#define REQ_ASYNCLOSE_EO00                       17
#define REQ_ASYNIO_EO00                          18
#define REQ_ASYNWAIT_EO00                        19
#if defined(_WIN32)
#define REQ_VNSHUTDOWN_REPLY_EO00                20
#define REQ_OUTGOING_CONNECT_EO00                21
#define REQ_OUTGOING_CONNECT_REPLY_EO00          22
#define REQ_OPEN_WAKEUP_SD_EO00                  23
#define REQ_CONNECT_RC_EO00                      24
#define REQ_RELEASE_RC_EO00                      25
#define REQ_CLOSE_OUTGOING_EO00                  26
#define REQ_CLOSE_OUTGOING_REPLY_EO00            27
#define REQ_FINISH_SV_EO00                       28
#define REQ_ASYNCNTL_EO00                        29
#define REQ_OVERLAPPED_IO_FINISHED_EO00          30
#else
#define REQ_EXCLUSIVE_EO00                       20
#define REQ_VNSHUTDOWN_EO00                      21
#define REQ_ASYNCNTL_EO00                        22
#define REQ_VMARK_BAD_DEV_EO00                   23
#define REQ_TEST_AND_SET_EO00                    24
#define REQ_RESCHEDULE_EO00                      25
#define REQ_IO_FINISHED_EO00                     26
#endif /* _WIN32 */
#define REQ_MARK_BAD_DEVSPACE_EO00               31
#define REQ_DCOM_CREATE_DCOMI_EO00               32
#define REQ_DCOM_REMOVE_DCOMI_EO00               33
#define REQ_DCOM_PARAMINFO_EO00                  34
#define REQ_DCOM_CO_CREATE_INSTANCE_EO00         35
#define REQ_DCOM_RELEASE_INSTANCE_EO00           36
#define REQ_DCOM_CO_GET_CLASSOBJECT_EO00         37
#define REQ_DCOM_TRANSFER_EO00                   38
#define REQ_DCOM_RELASE_ALL_EO00                 39
#define REQ_VSTOP_EO00                           40
#define REQ_VVECTORIO_EO00                       41
#define REQ_DEBUG_TASK_EO00                      42
#define REQ_INSERT_MOVING_TASK_E000              43
#define REQ_NEW_MOVE_TASK_MEAS_INTERVAL_EO00     44
#define REQ_MOVE_TASK_EO00                       45
#define REQ_NEW_COMMAND_EO00                     46
#define REQ_REMOVE_TASK_FROM_UKT_EO00            47
#define REQ_RESUME_TW_EO00                       48
#define REQ_COMMUNICATION_RECEIVE_WAIT           49
#define REQ_COMMUNICATION_CONNECT_WAIT           50
#define REQ_CONNECT_REQUEST_EO00                 51
#define REQ_TASK_SLEEP_EO00                      52
#define REQ_LEAVE_RWREGION_EO00                  53
#define REQ_CANCEL_TASK_EO00                     54
#define REQ_REQUEST_TIMED_OUT_EO00               55
#define REQ_YIELDING_EO00                        56
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   task types 
     PLEASE ALSO ALTER THE EQUIVALENT IN RTETASK_COMMONQUEUEELEMENT.HPP!
   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   task types 
     PLEASE ALSO ALTER THE EQUIVALENT IN RTETASK_ITASK.HPP !
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
#define TT_TI_EO00                   11
#define TT_TL_EO00                   12
#define TT_AL_EO00                   13
#define TT_TW_EO00                   14
#define TT_DW_EO00                   15
#define TT_SN_EO00                   16
#define TT_RC_EO00                   17
#define TT_UT_EO00                   18

#define TT_EV_EO00                   20
#define TT_GC_EO00                   21
#define TT_BUP_EO00                  22

#define TT_SV_EO00                   30
#define TT_US_EO00                   40
#define TT_LAST_TYPE                 40
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   task types 
     PLEASE ALSO ALTER THE EQUIVALENT IN RTETASK_ITASK.HPP !
   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/


/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   task state 
     PLEASE ALSO ALTER THE EQUIVALENT IN RTETASK_TASK.HPP !
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/* --- Values for kernel task states */
#define TSK_INITIALIZED_EO00                 10
#define TSK_INACTIVE_EO00                    11
#define TSK_MARKED_FOR_STARTUP_EO00          12
#define TSK_RUNNING_EO00                     13
#define TSK_CONNECTWAIT_EO00                 14
#define TSK_VDEVSIZE_EO00                    15
#define TSK_VATTACH_EO00                     16

#define TSK_VDETACH_EO00                     20
#define TSK_VFOPEN_EO00                      21
#define TSK_VFWRITE_EO00                     23
#define TSK_VFCLOSE_EO00                     24
#define TSK_VRELEASE_EO00                    25
#define TSK_VSHUTDOWN_EO00                   26
#define TSK_VRECEIVE_EO00                    27
#define TSK_VREPLY_EO00                      28

#define TSK_VBEGEXCL_EO00                    35
#define TSK_VENDEXCL_EO00                    36
#define TSK_VSUSPEND_EO00                    37
#define TSK_VWAIT_EO00                       38
#define TSK_SLEEP_EO00                       39
#define TSK_VDIAGINIT_EO00                   40
#define TSK_VOPMSG_EO00                      41
#define TSK_LOCKED_EO00                      42
#define TSK_TERMINATED_EO00                  43
#define TSK_RUNNABLE_EO00                    44
#define TSK_ASYNOPEN_EO00                    45
#define TSK_ASYNCLOSE_EO00                   46
#define TSK_ASYNIO_EO00                      47
#define TSK_ASYNWAIT_READ_EO00               48
#define TSK_ASYNWAIT_WRITE_EO00              49

#define TSK_IO_READ_EO00                     52
#define TSK_IO_WRITE_EO00                    53

#define TSK_VNCLEAR_EO00                     57
#define TSK_ASYNCNTL_EO00                    58
#define TSK_DCOM_OBJ_CALLED_EO00             59     
#define TSK_RESCHEDULE_MSEC_EO00             60     

#define TSK_INSERT_EVENT_EO00                65     
#define TSK_WAIT_FOR_EVENT_EO00              66     
#define TSK_STOPPED_EO00                     67
#define TSK_VVECTORIO_EO00                   68
#define TSK_VDUALVECTORIO_EO00               69

#define TSK_ENTERRWREGION_EO00               71
#define TSK_LEAVERWREGION_EO00               72

#define TSK_YIELDING_ON_SPINLOCK_EO00        73

#define TSK_LAST_EO00                        74
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   task state 
     PLEASE ALSO ALTER THE EQUIVALENT IN RTETASK_TASK.HPP !
   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/



#define MX_SUSPEND_REASONS_EO00            256
#define MX_DEVQUEUELEN_STAT_EO00             3

#define TASK_IS_IN_NON_QUEUE_EO00            0
#define TASK_IS_IN_UKT_QUEUE_EO00            3
#define TASK_IS_IN_COM_QUEUE_EO00            4
#define TASK_IS_IN_RUN_QUEUE_EO00            6

/* define for the extension of the old knldiag file that is saved at database */
/* starting */
#define KNLDIAG_EXT_OLD                      ".old" 

/***************************************************************/
#if defined(_WIN32)
/***************************************************************/

#define TASK_IS_IN_UToU_QUEUE_EO00           1
#define TASK_IS_IN_RAV_QUEUE_EO00            2
#define TASK_IS_IN_REX_QUEUE_EO00            5

#endif /* WIN32 */


#if defined(_WIN32)
/*>>>>>>>>>>>  N O   L O N G E R  U S E D .  >>>><<<<<<<<>>>>>>>>>>*/
// --- Request types
#define REQ_VATTACH                     REQ_VATTACH_EO00
#define REQ_VBLOCKIO                    REQ_VBLOCKIO_EO00
#define REQ_VDETACH                     REQ_VDETACH_EO00
#define REQ_INITOK                      REQ_INITOK_EO00
#define REQ_VRESUME                     REQ_VRESUME_EO00
#define REQ_VSIGNAL                     REQ_VSIGNAL_EO00
#define REQ_VSLEEP                      REQ_VSLEEP_EO00
#define REQ_CONNECT                     REQ_CONNECT_EO00
#define REQ_VRECEIVE                    REQ_VRECEIVE_EO00
#define REQ_SHUTDOK                     REQ_SHUTDOK_EO00
#define REQ_SENDER_JOB_FREE             REQ_SENDER_JOB_FREE_EO00
#define REQ_VENDEXCL                    REQ_VENDEXCL_EO00
#define REQ_RESCHEDULE                  REQ_RESCHEDULE_EO00
#define REQ_SHUTDOWN_RESUME_TW          REQ_SHUTDOWN_RESUME_TW_EO00
#define REQ_ASYNOPEN                    REQ_ASYNOPEN_EO00
#define REQ_ASYNCLOSE                   REQ_ASYNCLOSE_EO00
#define REQ_ASYNIO                      REQ_ASYNIO_EO00
#define REQ_ASYNWAIT                    REQ_ASYNWAIT_EO00
#define REQ_VNSHUTDOWN_REPLY            REQ_VNSHUTDOWN_REPLY_EO00
#define REQ_OUTGOING_CONNECT            REQ_OUTGOING_CONNECT_EO00
#define REQ_OUTGOING_CONNECT_REPLY      REQ_OUTGOING_CONNECT_REPLY_EO00
#define REQ_OPEN_WAKEUP_SD              REQ_OPEN_WAKEUP_SD_EO00
#define REQ_CONNECT_RC                  REQ_CONNECT_RC_EO00
#define REQ_RELEASE_RC                  REQ_RELEASE_RC_EO00
#define REQ_CLOSE_OUTGOING              REQ_CLOSE_OUTGOING_EO00
#define REQ_CLOSE_OUTGOING_REPLY        REQ_CLOSE_OUTGOING_REPLY_EO00
#define REQ_FINISH_SV                   REQ_FINISH_SV_EO00
#define REQ_ASYNCNTL                    REQ_ASYNCNTL_EO00
#define REQ_OVERLAPPED_IO_FINISHED      REQ_OVERLAPPED_IO_FINISHED_EO00
#define REQ_MARK_BAD_DEVSPACE           REQ_MARK_BAD_DEVSPACE_EO00
#define REQ_NEW_MOVE_TASK_MEAS_INTERVAL REQ_NEW_MOVE_TASK_MEAS_INTERVAL_EO00
#define REQ_NEW_COMMAND                 REQ_NEW_COMMAND_EO00
#define REQ_REMOVE_TASK_FROM_UKT        REQ_REMOVE_TASK_FROM_UKT_EO00
#else
/*
 *  Request types
 */
#define         REQ_VATTACH             1
#define         REQ_VBLOCKIO            2
#define         REQ_VDETACH             3
#define         REQ_INITOK              4
#define         REQ_VRESUME             5
#define         REQ_VSIGNAL             6
#define         REQ_VSLEEP              7
#define         REQ_CONNECT             8
#define         REQ_VRECEIVE            10
#define         REQ_SHUTDOK             11
#define         REQ_BIG_CONNECT         13
#define         REQ_ADDRESS_CONNECT     14
#define         REQ_BIG_ADDR_CONNECT    15
#define         REQ_ASYNOPEN            16
#define         REQ_ASYNCLOSE           17
#define         REQ_ASYNIO              18
#define         REQ_ASYNWAIT            19
#define         REQ_EXCLUSIVE           20
#define         REQ_VNSHUTDOWN          21
#define         REQ_ASYNCNTL            22
#define         REQ_VMARK_BAD_DEV       23
#define         REQ_TEST_AND_SET        24
#define         REQ_RESCHEDULE          25
#define         REQ_IO_FINISHED         26
#endif /* WIN32 */
/*<<<<<<<<<<<<<<<<<<  N O   L O N G E R  U S E D . <<<<<<<<<<<<<<<<<<<*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO00_2_H */
