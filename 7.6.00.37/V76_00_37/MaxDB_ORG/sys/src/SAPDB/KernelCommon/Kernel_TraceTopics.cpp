/*!*********************************************************************

  module:       Kernel_TraceTopics.cpp

  ----------------------------------------------------------------------

  responsible:  DanielD

  special area: Kernel Tracing

  description:  contains instances of kernel trace topics

  ----------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG

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


  *********************************************************************/

#include "gsp00.h"
#include "ggg00.h"
#include "hgg941.h"
#include "heo51.h"

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"
#include "Trace/Trace_Entry.hpp"
#include "KernelCommon/Kernel_TraceTopics.hpp"

/* ------------------------------------------------------------------ */

// SAPDBDiag_CheckTopic Demo_Switch    ("DEMO");
//   don't forget to add new topics to cserv.pcf
//   please do not add check topics that are not used !
//   there is no need to have check topic AND corresponding trace topic

/* ------------------------------------------------------------------ */
//SAPDBDiag_CheckTopic AllocatorCheck      ("ALLOCATOR","Checks for allocators");
//SAPDBDiag_CheckTopic Catalog_Check       ("CATALOG","Checks for catalog management");
SAPDBDiag_CheckTopic Common_Check        ("COMMON","Checks for commonly used code and Database Manager Commands");
SAPDBDiag_CheckTopic Converter_Check     ("CONVERTER","Checks for converter");
//SAPDBDiag_CheckTopic ConverterPage_Check ("CONVERTERPAGE","Checks for all data converter pages");
//SAPDBDiag_CheckTopic DataChain_Check     ("DATACHAIN","Checks for operations on data page chains");
//SAPDBDiag_CheckTopic DataPageInv_Check   ("DATAPAGEINV","Checks for index data pages");
SAPDBDiag_CheckTopic DataPageLog_Check   ("DATAPAGELOG","Checks for undo, redo, history data pages");
//SAPDBDiag_CheckTopic DataPagePrim_Check  ("DATAPAGEPRIM","Checks for primary data pages");
//SAPDBDiag_CheckTopic DataPam_Check       ("DATAPAM","Checks for page access with the new interface");
//SAPDBDiag_CheckTopic DataTree_Check      ("DATATREE","Checks for update statistics on trees");
SAPDBDiag_CheckTopic DataIndex_Check     ("DATAINDEX","Checks for index trees");
//SAPDBDiag_CheckTopic DBProc_Check        ("DBPROC","Checks for db-procedures, functions and triggers");
SAPDBDiag_CheckTopic FBM_Check           ("FBM","Checks for Free Block Management");
//SAPDBDiag_CheckTopic FileDir_Check       ("FILEDIR","Checks for file directory operations");
//SAPDBDiag_CheckTopic FrameCtrl_Check     ("FRAMECTRL","Checks for framecontrol");
SAPDBDiag_CheckTopic IOMan_Check         ("IOMAN","Checks for I/O manager");
//SAPDBDiag_CheckTopic Join_Check          ("JOIN","Checks for operator-join implementation");
//SAPDBDiag_CheckTopic KernelSQL_Check     ("KSQL","Checks for internal SQL statements");
//SAPDBDiag_CheckTopic LogAction_Check     ("LOGACTION","Checks of actions in online, undo or redo case");
SAPDBDiag_CheckTopic LogHistory_Check    ("LOGHISTORY","Checks for consistent view and garbage collection");
SAPDBDiag_CheckTopic LogPage_Check       ("LOGPAGE","Checks for log pages on log volumes");
SAPDBDiag_CheckTopic LogTrans_Check      ("LOGTRANS","Checks for restart, savepoint and transactionhandling");
SAPDBDiag_CheckTopic LogVolume_Check     ("LOGVOLUME","Checks for read and write access of the log area");
//SAPDBDiag_CheckTopic Memory_Check        ("MEMORY","Checks for memory management");
//SAPDBDiag_CheckTopic Pager_Check         ("PAGER","Checks for pager tasks");
//SAPDBDiag_CheckTopic Runtime_Check       ("RUNTIME","Checks for runtime environment");
SAPDBDiag_CheckTopic SrvTasks_Check      ("SRVTASKS","Checks for the server task management");
//SAPDBDiag_CheckTopic Table_Check         ("TABLE","Checks for the new table handling");
SAPDBDiag_CheckTopic HashedResultSet_Check         ("HASHED_RESULTSET","Checks for the haseh resultset");

/* ------------------------------------------------------------------ */

SAPDBDiag_CheckTopic * const Kernel_AllCheckSwitches [] = {
//    &AllocatorCheck,
//    &Catalog_Check,
    &Common_Check,
    &Converter_Check,
//    &ConverterPage_Check,
//    &DataChain_Check,
//    &DataPageInv_Check,
    &DataPageLog_Check,
//    &DataPagePrim_Check,
//    &DataPam_Check,
//    &DataTree_Check,
    &DataIndex_Check,
//    &DBProc_Check,
    &FBM_Check,
//    &FileDir_Check,
//    &FrameCtrl_Check,
    &IOMan_Check,
//    &Join_Check,
//    &KernelSQL_Check,
//    &LogAction_Check,
    &LogHistory_Check,
    &LogPage_Check,
    &LogTrans_Check,
    &LogVolume_Check,
//    &Memory_Check,
//    &Pager_Check,
//    &Runtime_Check,
    &SrvTasks_Check,
//    &Table_Check,
    &HashedResultSet_Check,
    0
};

static inline int GetCheckSwitchCount ()
{
    static int switchCount = -1;

    if (switchCount == -1) {
        for (switchCount = 0; Kernel_AllCheckSwitches [switchCount] != 0; ++switchCount) {
            // counting is enough
        }
    }
    return switchCount;
}

/* ------------------------------------------------------------------ */
//   don't forget to add new topics to cserv.pcf
//   please do not add trace topics that are not used !
//   there is no need to have check topic AND corresponding trace topic

SAPDBTrace_Topic AllocatorTrace             ("ALLOCATOR","Trace for allocators");
SAPDBTrace_Topic Catalog_Trace              ("CATALOG","Trace for catalog management");
SAPDBTrace_Topic CKC_Trace                  ("CLIENTKERNELCOM","Trace for communication between client and server");
SAPDBTrace_Topic Common_Trace               ("COMMON","Trace for commonly used code and Database Manager Commands");
SAPDBTrace_Topic Communication_Trace        ("COMMUNICATION","Trace for communication");
SAPDBTrace_Topic Converter_Trace            ("CONVERTER","Trace for converter");
//SAPDBTrace_Topic ConverterPage_Trace        ("CONVERTERPAGE","Trace for all data converter pages");
SAPDBTrace_Topic DataChain_Trace            ("DATACHAIN","Trace for operations on data page chains");
//SAPDBTrace_Topic DataPageInv_Trace          ("DATAPAGEINV","Trace for index data pages");
//SAPDBTrace_Topic DataPageLog_Trace          ("DATAPAGELOG","Trace for undo, redo, history data pages");
//SAPDBTrace_Topic DataPagePrim_Trace         ("DATAPAGEPRIM","Trace for primary data pages");
SAPDBTrace_Topic DataPam_Trace              ("DATAPAM","Trace for page access with the new interface");
SAPDBTrace_Topic DataTree_Trace             ("DATATREE","Trace for update statistics on trees");
SAPDBTrace_Topic DataIndex_Trace            ("DATAINDEX","Trace for index trees");
SAPDBTrace_Topic DBProc_Trace               ("DBPROC","Trace for db-procedures, functions and triggers");
SAPDBTrace_Topic Devspace_Trace             ("VOLUME","Trace for volume management in RTE");
SAPDBTrace_Topic FBM_Trace          	    ("FBM","Trace for Free Block Management");
SAPDBTrace_Topic FileDir_Trace              ("FILEDIR", "Trace for file directory operations");
SAPDBTrace_Topic FrameCtrl_Trace            ("FRAMECTRL","Trace for framecontrol");
SAPDBTrace_Topic IOMan_Trace                ("IOMAN","Trace for I/O manager");
SAPDBTrace_Topic IPC_Trace                  ("IPC","Trace for interprocess communication");
SAPDBTrace_Topic Join_Trace                 ("JOIN","Trace for operator-join implementation");
SAPDBTrace_Topic KernelSQL_Trace            ("KSQL","Trace for internal SQL statements");
SAPDBTrace_Topic LogAction_Trace            ("LOGACTION","Trace of actions in online, undo or redo case");
SAPDBTrace_Topic LogHistory_Trace           ("LOGHISTORY","Trace for consistent view and garbage collection");
SAPDBTrace_Topic LogPage_Trace              ("LOGPAGE","Trace for pages on log volumes");
SAPDBTrace_Topic LogTrans_Trace             ("LOGTRANS","Trace for restart, savepoint and transactionhandling");
SAPDBTrace_Topic LogVolume_Trace            ("LOGVOLUME","Trace for read and write access of the log area");
SAPDBTrace_Topic Memory_Trace               ("MEMORY","Trace for memory management");
SAPDBTrace_Topic Messages_Trace             ("MESSAGES","Trace for generated messages");
SAPDBTrace_Topic ObjContainer_Trace         ("OBJECTCONTAINER","Trace for container for liveCache objects");
SAPDBTrace_Topic Pager_Trace        	    ("PAGER","Trace for pager tasks");
SAPDBTrace_Topic Runtime_Trace              ("RUNTIME","Trace for runtime environment");
SAPDBTrace_Topic SrvTasks_Trace             ("SRVTASKS","Trace for servertask management");
SAPDBTrace_Topic SQLMan_Trace               ("SQLMANAGER","Trace for the SQL manager component");
SAPDBTrace_Topic SharedSQL_Trace            ("SHAREDSQL","Trace for the SharedSQL component");
SAPDBTrace_Topic Sync_Trace                 ("SYNCHRONISATION","Trace for synchronization of shared resources");
SAPDBTrace_Topic SysView_Trace              ("SYSVIEW","Trace for management of systemviews");
SAPDBTrace_Topic Table_Trace                ("TABLE","Trace for the new table handling");
SAPDBTrace_Topic QueryRewrite_Trace         ("QUERYREWRITE","Checks for the new query rewriting");

/* ------------------------------------------------------------------ */

SAPDBTrace_Topic * const Kernel_AllTraceTopics [] = {
    &AllocatorTrace,
    &Catalog_Trace,
    &CKC_Trace,
    &Common_Trace,
    &Communication_Trace,
    &Converter_Trace,
    //&ConverterPage_Trace,
    &DataChain_Trace,
    //&DataPageInv_Trace,
    //&DataPageLog_Trace,
    //&DataPagePrim_Trace,
    &DataPam_Trace,
    &DataTree_Trace,
    &DataIndex_Trace,
    &DBProc_Trace,
    &Devspace_Trace,
    &FBM_Trace,
    &FileDir_Trace,
    &FrameCtrl_Trace,
    &IOMan_Trace,
    &IPC_Trace,
    &Join_Trace,
    &KernelSQL_Trace,
    &LogAction_Trace,
    &LogHistory_Trace,
    &LogPage_Trace,
    &LogTrans_Trace,
    &LogVolume_Trace,
    &Memory_Trace,
    &Messages_Trace,
    &ObjContainer_Trace,
    &Pager_Trace,
    &Runtime_Trace,
    &SrvTasks_Trace,
    &SQLMan_Trace,
    &SharedSQL_Trace,
    &Sync_Trace,
    &SysView_Trace,
    &Table_Trace,
    &QueryRewrite_Trace,
    0
};

static inline int GetTraceTopicCount ()
{
    static int switchCount = -1;

    if (switchCount == -1) {
        for (switchCount = 0; Kernel_AllTraceTopics [switchCount] != 0; ++switchCount) {
            // counting is enough
        }
    }
    return switchCount;
}

/* ------------------------------------------------------------------ */

/*!*********************************************************************

  Class: VtracePrinter

 **********************************************************************/
class VtracePrinter : public SAPDBTrace_LinePrinter
{
public:
    /* function: contructor
     */
    VtracePrinter ();

    /* function: PrintLn
       description: writes a line to the output medium
     */
    virtual void PrintLn(const SAPDB_Char * buf, int buflen);

};

/*! EndClass: VtracePrinter */

/* ------------------------------------------------------------------ */

/* function: contructor
 */
VtracePrinter::VtracePrinter ()
{
}

/* ------------------------------------------------------------------ */

/* function: PrintLn
   description: writes a line to the output medium
 */
void VtracePrinter::PrintLn(const SAPDB_Char * buf, int buflen)
{
    tsp00_TaskId  taskId;
    vgetpid (taskId);

    /* This prevents crash if trace called from outside UKT code */
    if ( 0 != taskId )
    {
        Trace_CommonEntry entry( tgg00_Debug::fromConst(ta),
                 tgg00_VtraceType::fromConst(td_none),
                 reinterpret_cast<const tsp00_Byte *> (buf), buflen);
    }
}

/* ------------------------------------------------------------------ */

static VtracePrinter mainVtracePrinter;

/*! --------------------------------------------------------------------
function:    Kernel_MultiSwitch
description: parse and exefcutes a line with switch instructions

    The line is of the form "<id> [<int>] [<id> [<int>] ] ...

    An id '*' serves as a wildcard for all topics.

    interface in vgg941.
*/
externPascal void Kernel_MultiSwitch (const char*  TopicStr, /* c20 */
                                      int          TopicStrLen)
{
    SAPDBDiag_CheckTopic::MultiSwitch (Kernel_AllCheckSwitches, TopicStr, TopicStrLen);
    SAPDBTrace_Topic::MultiSwitch (Kernel_AllTraceTopics, TopicStr, TopicStrLen);
}

/*! --------------------------------------------------------------------
function:    Kernel_CheckSwitch
description: parse and exefcutes a line with switch instructions

    The line is of the form "<id> [<int>] [<id> [<int>] ] ...

    An id '*' serves as a wildcard for all topics.

    interface in vgg941.
*/
externPascal void Kernel_CheckSwitch (const char*  TopicStr, /* c20 */
                                      int          TopicStrLen)
{
    SAPDBDiag_CheckTopic::MultiSwitch (Kernel_AllCheckSwitches, TopicStr, TopicStrLen);
}

/*! --------------------------------------------------------------------
function:    Kernel_TraceSwitch
description: parse and exefcutes a line with switch instructions

    The line is of the form "<id> [<int>] [<id> [<int>] ] ...

    An id '*' serves as a wildcard for all topics.

    interface in vgg941.
*/
externPascal void Kernel_TraceSwitch (const char*  TopicStr, /* c20 */
                                      int          TopicStrLen)
{
    SAPDBTrace_Topic::MultiSwitch (Kernel_AllTraceTopics, TopicStr, TopicStrLen);
}

/*! --------------------------------------------------------------------
function:    Kernel_InitTrace
description: sets the printer for all trace topics
*/

externPascal void Kernel_InitTrace ()
{
    SAPDBTrace_Topic::InitGroup (Kernel_AllTraceTopics, &mainVtracePrinter);
}

/*! --------------------------------------------------------------------
function:     Kernel_DiagInfo
description:
 */
bool Kernel_DiagInfo (
    int          requested,
    SAPDB_Char * kindOut,
    int          maxKindLen,
    SAPDB_Char * topicKeyOut,
    int          maxTopicKey,
    int        & levelOut,
    SAPDB_Char * topicDescOut,
    int          maxTopicDesc)
{
    int relativeRequested;

    int switchCount = GetCheckSwitchCount();
    relativeRequested = requested;
    if ((0 <= relativeRequested) && (relativeRequested < switchCount)) {
        Kernel_AllCheckSwitches [relativeRequested]->GetInfo (
            kindOut, maxKindLen, topicKeyOut, maxTopicKey, levelOut,
            topicDescOut, maxTopicDesc);
        return true;
    }
    int traceCount = GetTraceTopicCount();
    relativeRequested = requested - switchCount;
    if ((0 <= relativeRequested) && (relativeRequested < traceCount)) {
        Kernel_AllTraceTopics [relativeRequested]->GetInfo (
            kindOut, maxKindLen, topicKeyOut, maxTopicKey, levelOut,
            topicDescOut, maxTopicDesc);
        return true;
    }
    return false;
}

/* ------------------------------------------------------------------ */

static void fillIdentifier (
    char * identifier,
    int size)
{
    int len = static_cast<int> (strlen (identifier));
    if (len < size) {
        memset (identifier + len, ' ', size - len);
    }
}

/*! --------------------------------------------------------------------
function:     Kernel_DiagInfop
description:  gets information about a specific topic

    use as:

    int index = 0;
    while (Kernel_DiagInfop (index, kindOut, topicKeyOut, levelOut)) {
        ...
        ++index;
    }

    interface in vgg941.

arguments:
    requested [IN]    an index into the array of topics
    kindOut   [OUT]   "Vtrace" or "Check"
    topicKeyOut [OUT] the string used to switch the topic
    levelOut [OUT]    the current level
 */
externPascal pasbool Kernel_DiagInfop (
    int                requested,
    tsp00_Identifier * kindOut,
    tsp00_Sname      * topicKeyOut,
    int              & levelOut)
{
    bool         foundInfo;
    SAPDB_Char   topicDescOut[10];

    foundInfo = Kernel_DiagInfo (requested,
        kindOut->asCharp (), sizeof (tsp00_Identifier),
        topicKeyOut->asCharp (), sizeof (tsp00_Sname),
        levelOut, (SAPDB_Char *)&topicDescOut, sizeof (topicDescOut));
    if (foundInfo) {
        fillIdentifier (*kindOut, sizeof (tsp00_Identifier));
        fillIdentifier (*topicKeyOut, sizeof (tsp00_Sname));
		return true;
    }
    return false;
}

/*--------------------------------------------------------------------*/

bool Kernel_DiagTopicsIterator (
    int requested,
    SAPDBDiag_Topic ** topic)
{
    int switchCount = GetCheckSwitchCount();

    if ((0 <= requested) && (requested < switchCount)) {
        *topic = Kernel_AllCheckSwitches [requested];
        return true;
    }
    return false;
}

/*--------------------------------------------------------------------*/

void Kernel_SwitchDiagByName (
    const SAPDB_Char * topicName,
    int                newLevel)
{
    SAPDBDiag_Topic::SingleSwitch (
        reinterpret_cast<SAPDBDiag_Topic * const *> (Kernel_AllCheckSwitches),
        topicName, newLevel);
}

/*--------------------------------------------------------------------*/

bool Kernel_TraceTopicsIterator (
    int requested,
    SAPDBDiag_Topic ** topic)
{
    int switchCount = GetTraceTopicCount();

    if ((0 <= requested) && (requested < switchCount)) {
        *topic = Kernel_AllTraceTopics [requested];
        return true;
    }
    return false;
}

/*--------------------------------------------------------------------*/

void Kernel_SwitchTraceByName (
    const SAPDB_Char * topicName,
    int                newLevel)
{
    SAPDBDiag_Topic::SingleSwitch (
        reinterpret_cast<SAPDBDiag_Topic * const *> (Kernel_AllTraceTopics),
        topicName, newLevel);
}


