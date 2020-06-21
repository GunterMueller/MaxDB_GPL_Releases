/*!
  @file           heo00x.h
  @author         JoergM
  @special area   
  @brief          RTE - XCONS  include module
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


#ifndef HEO00X_H
#define HEO00X_H


#include "heo00.h"
#include "geo00_2.h"

/*
// +---------------------------------------------------------------+
// |  TASK STATE REASON                                            |
// +---------------------------------------------------------------+
*/
typedef char SHORT_REASON_STR[14+1] ;
typedef char LONG_REASON_STR [40+1] ;
 
typedef struct suspend_state_reason
  {
  SHORT_REASON_STR                      szShortReason ;
  LONG_REASON_STR                       szLongReason ;
  } SUSPEND_STATE_REASON;


/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    If you extend the following suspend reason table then do not forget 
    to add a further define for that reason in RTETask_SuspendReason.h.
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

static SUSPEND_STATE_REASON SuspendReasonText[MX_SUSPEND_REASONS_EO00] =
  {
    {  "Vsuspend (000)" , " NICHT BENUTZEN                         " } ,
    {  "Eventing (001)" , "vwait_for_event: waiting for event      " } ,
    {  "DelFile  (002)" , "bd95_RemoveObjFile                      " } ,
    {  "Vsuspend (003)" , "                                        " } ,
    {  "Vsuspend (004)" , "                                        " } ,
    {  "Vsuspend (005)" , "                                        " } ,
    {  "Vsuspend (006)" , "                                        " } ,
    {  "Vsuspend (007)" , "                                        " } ,
    {  "Vsuspend (008)" , "                                        " } ,
    {  "Vsuspend (009)" , "                                        " } ,
    {  "TableRootExcl " , "bd13GetNode                             " } ,
    {  "TableRootShare" , "bd13GetNode                             " } ,
    {  "TableNodeExcl " , "bd13GetNode                             " } ,
    {  "TableNodeShare" , "bd13GetNode                             " } ,
    {  "TableLvl1Excl " , "bd13GetNode                             " } ,
    {  "TableLvl1Share" , "bd13GetNode                             " } ,
    {  "InvRootExcl   " , "bd13GetNode                             " } ,
    {  "InvRootShare  " , "bd13GetNode                             " } ,
    {  "InvNodeExcl   " , "bd13GetNode                             " } ,
    {  "InvNodeShare  " , "bd13GetNode                             " } ,
    {  "TempRootExcl  " , "bd13GetNode                             " } ,
    {  "TempRootShare " , "bd13GetNode                             " } ,
    {  "TempNodeExcl  " , "bd13GetNode                             " } ,
    {  "TempNodeShare " , "bd13GetNode                             " } ,
    {  "ObjRootExcl   " , "bd13GetNode                             " } ,
    {  "ObjRootShare  " , "bd13GetNode                             " } ,
    {  "ObjNodeExcl   " , "bd13GetNode                             " } ,
    {  "ObjNodeShare  " , "bd13GetNode                             " } ,
    {  "SVP-End  (028)" , "bd13WriteExclusiveLockedRoot            " } ,
    {  "UndoRootExcl  " , "bd13GetNode                             " } ,
    {  "UndoRootShare " , "bd13GetNode                             " } ,
    {  "UndoNodeExcl  " , "bd13GetNode                             " } ,
    {  "UndoNodeShare " , "bd13GetNode                             " } ,
    {  "HistRootExcl  " , "bd13GetNode                             " } ,
    {  "HistRootShare " , "bd13GetNode                             " } ,
    {  "HistNodeExcl  " , "bd13GetNode                             " } ,
    {  "HistNodeShare " , "bd13GetNode                             " } ,
    {  "Vsuspend (037)" , "                                        " } ,
    {  "Vsuspend (038)" , "                                        " } ,
    {  "Vsuspend (039)" , "                                        " } ,
    {  "IOWait(W)(040)" , "bd13WriteExclusiveLockedRoot            " } ,
    {  "IOWait(R)(041)" , "b13get_node: await read              :-(" } ,
    {  "IOWait(W)(042)" , "b13get_node                          :-(" } ,
    {  "DC OvFlow(043)" , "b13get_node: sysbuf_overflow (SVP)   :-(" } ,
    {  "IOWait(W)(044)" , "b13pfree_pno                         :-(" } ,
    {  "SVP-End  (045)" , "b13pfree_pno                         :-(" } ,
    {  "DC OvFlow(046)" , "bd13frame_for: sysbuf_overflow       :-(" } ,
    {  "Vsuspend (047)" , "                                        " } ,
    {  "Vsuspend (048)" , "                                        " } ,
    {  "Vsuspend (049)" , "                                        " } ,
    {  "Vsuspend (050)" , "                                        " } ,
    {  "VTRACE IO(051)" , "Trace_Manager::PrepareAndWaitForFlush   " } ,
    {  "InitState(052)" , "                                        " } ,
    {  "Vsuspend (053)" , "                                        " } ,
    {  "PagerWaitWritr" , "Pager_Controller::WaitForPagerWritReply " } ,
    {  "DropWaitBackup" , "DropVolume::Admin WaitForBackupData     " } ,
    {  "FreeWait (056)" , "bd13frame_for_new_node                  " } ,
    {  "IOWait(R (057)" , "b13b_get_node_by_blockaddr           :-(" } ,
    {  "IOWait(W)(058)" , "b13b_get_node_by_blockaddr           :-(" } ,
    {  "DC OvFlow(059)" , "b13b_get_node: sysbuf_overflow (SVP) :-(" } ,
    {  "Vsuspend (060)" , "                                        " } ,
    {  "Vsuspend (061)" , "                                        " } ,
    {  "Vsuspend (062)" , "                                        " } ,
    {  "Vsuspend (063)" , "                                        " } ,
    {  "Vsuspend (064)" , "                                        " } ,
    {  "Vsuspend (065)" , "                                        " } ,
    {  "Vsuspend (066)" , "                                        " } ,
    {  "Vsuspend (067)" , "                                        " } ,
    {  "Vsuspend (068)" , "                                        " } ,
    {  "Vsuspend (069)" , "                                        " } ,
    {  "InvSelSrv(070)" , "Join_InvSelectServer::GetRequest        " } ,
    {  "InvSelSrv(071)" , "Join_InvSelectServer::ReserveRowBuffer  " } ,
    {  "InvSel   (072)" , "InvSelectIterator::Suspend              " } , 
    {  "Vsuspend (073)" , "                                        " } ,
    {  "Vsuspend (074)" , "                                        " } ,
    {  "Vsuspend (075)" , "                                        " } ,
    {  "Vsuspend (076)" , "                                        " } ,
    {  "Vsuspend (077)" , "                                        " } ,
    {  "Vsuspend (078)" , "                                        " } ,
    {  "Vsuspend (079)" , "                                        " } ,
    {  "Vsuspend (080)" , "                                        " } ,
    {  "Vsuspend (081)" , "                                        " } ,
    {  "Vsuspend (082)" , "                                        " } ,
    {  "Vsuspend (083)" , "                                        " } ,
    {  "Vsuspend (084)" , "                                        " } ,
    {  "Vsuspend (085)" , "                                        " } ,
    {  "Vsuspend (086)" , "                                        " } ,
    {  "Vsuspend (087)" , "                                        " } ,
    {  "Vsuspend (088)" , "                                        " } ,
    {  "Vsuspend (089)" , "                                        " } ,
    {  "Vsuspend (090)" , "                                        " } ,
    {  "Vsuspend (091)" , "                                        " } ,
    {  "Vsuspend (092)" , "                                        " } ,
    {  "Vsuspend (093)" , "                                        " } ,
    {  "Vsuspend (094)" , "                                        " } ,
    {  "Vsuspend (095)" , "                                        " } ,
    {  "Vsuspend (096)" , "                                        " } ,
    {  "Vsuspend (097)" , "                                        " } ,
    {  "Vsuspend (098)" , "                                        " } ,
    {  "Vsuspend (099)" , "                                        " } ,
    {  "Vsuspend (100)" , "                                        " } ,
    {  "Vsuspend (101)" , "                                        " } ,
    {  "Vsuspend (102)" , "                                        " } ,
    {  "Vsuspend (103)" , "                                        " } ,
    {  "Vsuspend (104)" , "                                        " } ,
    {  "Vsuspend (105)" , "                                        " } ,
    {  "Vsuspend (106)" , "                                        " } ,
    {  "Vsuspend (107)" , "                                        " } ,
    {  "Vsuspend (108)" , "                                        " } ,
    {  "Vsuspend (109)" , "                                        " } ,
    {  "Vsuspend (110)" , "                                        " } ,
    {  "Vsuspend (111)" , "                                        " } ,
    {  "Vsuspend (112)" , "                                        " } ,
    {  "Vsuspend (113)" , "                                        " } ,
    {  "Vsuspend (114)" , "                                        " } ,
    {  "Vsuspend (115)" , "                                        " } ,
    {  "Vsuspend (116)" , "                                        " } ,
    {  "Vsuspend (117)" , "                                        " } ,
    {  "Vsuspend (118)" , "                                        " } ,
    {  "Vsuspend (119)" , "                                        " } ,
    {  "Vsuspend (120)" , "                                        " } ,
    {  "Vsuspend (121)" , "                                        " } ,
    {  "Vsuspend (122)" , "                                        " } ,
    {  "Vsuspend (123)" , "                                        " } ,
    {  "Vsuspend (124)" , "                                        " } ,
    {  "Vsuspend (125)" , "                                        " } ,
    {  "Vsuspend (126)" , "                                        " } ,
    {  "Vsuspend (127)" , "                                        " } ,
    {  "Vsuspend (128)" , "                                        " } ,
    {  "Vsuspend (129)" , "                                        " } ,
    {  "Vsuspend (130)" , "                                        " } ,
    {  "Vsuspend (131)" , "                                        " } ,
    {  "Vsuspend (132)" , "                                        " } ,
    {  "Vsuspend (133)" , "                                        " } ,
    {  "Vsuspend (134)" , "                                        " } ,
    {  "Vsuspend (135)" , "                                        " } ,
    {  "Vsuspend (136)" , "                                        " } ,
    {  "Vsuspend (137)" , "                                        " } ,
    {  "Vsuspend (138)" , "                                        " } ,
    {  "Vsuspend (139)" , "                                        " } ,
    {  "Vsuspend (140)" , "                                        " } ,
    {  "Vsuspend (141)" , "                                        " } ,
    {  "Vsuspend (142)" , "                                        " } ,
    {  "Vsuspend (143)" , "                                        " } ,
    {  "Vsuspend (144)" , "                                        " } ,
    {  "Vsuspend (145)" , "                                        " } ,
    {  "Vsuspend (146)" , "                                        " } ,
    {  "Vsuspend (147)" , "                                        " } ,
    {  "Vsuspend (148)" , "                                        " } ,
    {  "Vsuspend (149)" , "                                        " } ,
    {  "Vsuspend (150)" , "                                        " } ,
    {  "Vsuspend (151)" , "                                        " } ,
    {  "Vsuspend (152)" , "                                        " } ,
    {  "Vsuspend (153)" , "                                        " } ,
    {  "Vsuspend (154)" , "                                        " } ,
    {  "Vsuspend (155)" , "                                        " } ,
    {  "Vsuspend (156)" , "                                        " } ,
    {  "Vsuspend (157)" , "                                        " } ,
    {  "Vsuspend (158)" , "                                        " } ,
    {  "Vsuspend (159)" , "                                        " } ,
    {  "Vsuspend (160)" , "                                        " } ,
    {  "Vsuspend (161)" , "                                        " } ,
    {  "Vsuspend (162)" , "                                        " } ,
    {  "Vsuspend (163)" , "                                        " } ,
    {  "Vsuspend (164)" , "                                        " } ,
    {  "Vsuspend (165)" , "                                        " } ,
    {  "Vsuspend (166)" , "                                        " } ,
    {  "Vsuspend (167)" , "                                        " } ,
    {  "Vsuspend (168)" , "                                        " } ,
    {  "Vsuspend (169)" , "                                        " } ,
    {  "Vsuspend (170)" , "                                        " } ,
    {  "Vsuspend (171)" , "                                        " } ,
    {  "Vsuspend (172)" , "                                        " } ,
    {  "Vsuspend (173)" , "                                        " } ,
    {  "Vsuspend (174)" , "                                        " } ,
    {  "Vsuspend (175)" , "                                        " } ,
    {  "Vsuspend (176)" , "                                        " } ,
    {  "Vsuspend (177)" , "                                        " } ,
    {  "Vsuspend (178)" , "                                        " } ,
    {  "Vsuspend (179)" , "                                        " } ,
    {  "Vsuspend (180)" , "                                        " } ,
    {  "Vsuspend (181)" , "                                        " } ,
    {  "Vsuspend (182)" , "                                        " } ,
    {  "Vsuspend (183)" , "                                        " } ,
    {  "Vsuspend (184)" , "                                        " } ,
    {  "Vsuspend (185)" , "                                        " } ,
    {  "Vsuspend (186)" , "                                        " } ,
    {  "Vsuspend (187)" , "                                        " } ,
    {  "Vsuspend (188)" , "                                        " } ,
    {  "Vsuspend (189)" , "                                        " } ,
    {  "Vsuspend (190)" , "                                        " } ,
    {  "Vsuspend (191)" , "                                        " } ,
    {  "Vsuspend (192)" , "                                        " } ,
    {  "Vsuspend (193)" , "                                        " } ,
    {  "Vsuspend (194)" , "                                        " } ,
    {  "Vsuspend (195)" , "                                        " } ,
    {  "Vsuspend (196)" , "                                        " } ,
    {  "DB FULL  (197)" , "Conv_HandleDBFull                       " } ,
    {  "DB FULL  (198)" , "FBM_HandleDBFull                        " } ,
    {  "Vsuspend (199)" , "                                        " } ,
    {  "Vsuspend (200)" , "                                        " } ,
    {  "Vsuspend (201)" , "                                        " } ,
    {  "Vsuspend (202)" , "k38e_autosave_end                       " } ,
    {  "Vsuspend (203)" , "kb39read_wait                           " } ,
    {  "Vsuspend (204)" , "kb39write_wait                          " } ,
    {  "Vsuspend (205)" , "                                        " } ,
    {  "Vsuspend (206)" , "                                        " } ,
    {  "Vsuspend (207)" , "                                        " } ,
    {  "Vsuspend (208)" , "                                        " } ,
    {  "Vsuspend (209)" , "                                        " } ,
    {  "Vsuspend (210)" , "                                        " } ,
    {  "Vsuspend (211)" , "                                        " } ,
    {  "Vsuspend (212)" , "                                        " } ,
    {  "WaitForJob(s) " , "SrvTasks_Task::Execute() WaitForJobEnd  " } ,
    {  "SrvTaskNotFree" , "SrvTasks_Task::RegisterJob() TaskNotFree" } ,
    {  "Vsuspend (215)" , "                                        " } ,
    {  "Vsuspend (216)" , "k38st_autosave_start (new tape required)" } ,
    {  "WaitForFreeTra" , "all trans entries used                  " } ,
    {  "Vsuspend (218)" , "                                        " } ,
    {  "Vsuspend (219)" , "                                        " } ,
    {  "Vsuspend (220)" , "                                        " } ,
    {  "Vsuspend (221)" , "                                        " } ,
    {  "Vsuspend (222)" , "                                        " } ,
    {  "Vsuspend (223)" , "                                        " } ,
    {  "Vsuspend (224)" , "kb39wait_for_redo                       " } ,
    {  "Vsuspend (225)" , "kb57wait_for_stamp_flush                " } ,
    {  "Vsuspend (226)" , "                                        " } ,
    {  "Vsuspend (227)" , "RedoReader waits for finished jobs      " } ,
    {  "Vsuspend (228)" , "RedoReader waits for end of all RedoTask" } ,
    {  "Vsuspend (229)" , "bd91StopAllGarbageCollectors            " } ,
    {  "Prep-End (230)" , "bd13GetNode                             " } ,
    {  "NoRedoJob(231)" , "Rst_RedoTrafficControl::ExecuteJobs()   " } ,
    {  "SyncRedo (232)" , "Rst_RedoTrafficControl::WaitForIoSeq()  " } ,
    {  "LogQ FULL(233)" , "Log_Queue::ReserveSpace              :-(" } ,
    {  "LogIOwait(234)" , "Log_Queue::UserTaskEOTReady             " } ,
    {  "Vsuspend (235)" , "                                        " } ,
    {  "Vsuspend (236)" , "Conv_FreePageNoWaitDuringSavepoint      " } ,
    {  "Vsuspend (237)" , "Conv_ExpandWaitDuringSavepoint          " } ,
    {  "Vsuspend (238)" , "Conv_WaitForConvPageIOToGetSpecPageNo   " } ,
    {  "Vsuspend (239)" , "Conv_DropWaitDuringSavepoint            " } ,
    {  "DC OvFlow(240)" , "b13pfree: sysbuf_overflow (SVP)      :-(" } ,
    {  "LOG FULL (241)" , "Log_Writer                              " } ,
    {  "InitState(242)" , "Log_Writer                              " } ,
    {  "SVP-wait (243)" , "Log_Savepoint::StartSavepointAndWait    " } ,
    {  "SVP-wait (244)" , "Log_Savepoint::TriggerAndWait (DBM-OP)  " } ,
    {  "SVP-init (245)" , "Log_Savepoint::StartCoordinator         " } ,
    {  "LOG FULL (246)" , "Log_Queue::ReserveSpace              :-(" } ,
    {  "Vsuspend (247)" , "Log_Writer::Suspend (wait for suspend)  " } ,
    {  "USR HOLD (248)" , "Log_Writer suspended by user            " } ,
    {  "Synchron (249)" , "HS::RedoReader:last valid offset reached" } ,
    {  "TakeOver (250)" , "HS::Admin: wait for end of redo         " } ,
    {  "Vsuspend (251)" , "HS::Admin: wait for suspend redo-reader " } ,
    {  "Vsuspend (252)" , "HS::Admin: sync susp. until reader init " } ,
    {  "Vsuspend (253)" , "Pager::BeginSVP: wait for dataCacheFlush" } ,
    {  "Vsuspend (254)" , "                                        " } ,
    {  "No-Work  (255)" , "Task is waiting for work                " }
  } ;


#endif  /* HEO00X_H */
