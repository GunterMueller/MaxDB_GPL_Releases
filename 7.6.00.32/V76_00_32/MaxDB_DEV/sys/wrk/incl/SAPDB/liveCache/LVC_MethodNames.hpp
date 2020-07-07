/* @lastChanged: "1998-10-22  8:37"
* @filename:    LVC_MethodNames.hpp
* @purpose:     "LiveCache Interface used by DB-Procedures"
* @release:     7.4.0.0
* @see:         ""
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

#ifndef LVC_METHODNAMES_HPP
#define LVC_METHODNAMES_HPP

/* +++ PTS 1106969   x_cons extensions for external db-procedures */
/* these entries are used within veo38.cpp to identify the        */
/* called IliveCacheSink Method.                                  */
/* MethodIdx-Member within teo52_TASKOMSStatisticRec              */
const int ABAPRead_method_idx             = 3;
const int ABAPWrite_method_idx            = 4;
const int AllocPacket_method_idx          = 5;
const int BasisError_method_idx           = 6;
const int CloseVersion_method_idx         = 7;
const int ConsoleMsg_method_idx           = 8;
const int CreateOpenVersion_method_idx    = 9;
const int DeleteAllObj_method_idx         = 10;
const int DeleteObj_method_idx            = 11;
const int EndConsistentView_method_idx    = 12;
const int EnterCriticalSection_method_idx = 13;
const int GetErrorMsg_method_idx          = 14;
const int GetObj_method_idx               = 15;
const int GetObjWithKey_method_idx        = 16;
const int HResultError_method_idx         = 17;
const int IsDebugEnvironment_method_idx   = 18;
const int IsLocked_method_idx             = 19;
const int LeaveCriticalSection_method_idx = 20;
const int LockObj_method_idx              = 21;
const int MultiPurpose_method_idx         = 22;
const int NewConsistentView_method_idx    = 23;
const int NewObj_method_idx               = 24;
const int NextObj_method_idx              = 25;
const int CreateContainer_method_idx      = 26;
const int Rollback_method_idx             = 27;
const int SetError_method_idx             = 28;
const int SQLReceive_method_idx           = 29;
const int SQLRequest_method_idx           = 30;
const int SubtransCommit_method_idx       = 31;
const int SubtransRollback_method_idx     = 32;
const int SubtransStart_method_idx        = 33;
const int UnlockObj_method_idx            = 34;
const int UpdateObj_method_idx            = 35;
const int Vtrace_method_idx               = 36;
const int GetDefaultContext_method_idx    = 37;
const int SetDefaultContext_method_idx    = 38;
const int OpenFile_method_idx             = 39;
const int WriteFile_method_idx            = 40;
const int CloseFile_method_idx            = 41;
const int SyncFile_method_idx             = 42;
const int GetDateTime_method_idx          = 43;
const int GetClock_method_idx             = 44;
const int DropVersion_method_idx          = 45;
const int CreateSchema_method_idx         = 46;
const int ExistsSchema_method_idx         = 47;
const int DropSchema_method_idx           = 48;
const int GetSchemaName_method_idx        = 49;
const int ExistsFile_method_idx           = 50;
const int DropFile_method_idx             = 51;
const int GetWorkingDirectory_method_idx  = 52;
const int GetDbrootSapPath_method_idx     = 53;
const int LoadLibraryEx_method_idx        = 54;
const int FreeLibrary_method_idx          = 55;
const int DropContainer_method_idx        = 56;
const int GetContainerInfo_method_idx     = 57;
const int IsObjHistoryUsed_method_idx     = 58;
const int Reschedule_method_idx           = 59;
const int LockUpdObj_method_idx           = 60;
const int Signal_method_idx               = 61; /* PTS 1110287 */
const int Wait_method_idx                 = 62; /* PTS 1110287 */
const int ExistsContainer_method_idx      = 63; /* PTS 1110334 */
const int CreateKeyIterator_method_idx    = 64;
const int DestroyKeyIterator_method_idx   = 65;
const int NextObjFromKeyIter_method_idx   = 66;
const int ResetVersion_method_idx         = 67; /* PTS 1115134 */
const int PrevObjFromKeyIter_method_idx   = 68;     /* PTS 1119480 */
const int PrevObj_method_idx              = 69;     /* PTS 1119480 */
const int StartUnloadedVersionIter_method_idx = 70; /* PTS 1119480 */
const int NextVal_idx                     = 71;
const int IsUnicodeInstance_idx           = 72;
const int NextOids_method_idx             = 73;     /* PTS 1120478 */
const int GetObjects_method_idx           = 74;     /* PTS 1120478 */
const int GetContainerId_idx              = 75;     /* PTS 1120478 */
const int DeleteObjMass_method_idx        = 76;     /* PTS 1120478 */
const int LockObjMass_method_idx          = 77;     /* PTS 1120478 */
const int LockUpdObjMass_method_idx       = 78;     /* PTS 1120478 */
const int UpdateObjMass_method_idx        = 79;     /* PTS 1120478 */
const int UpdateVarObjMass_method_idx     = 80;     /* PTS 1121449 */
const int NewObjMass_method_idx           = 81;     /* PTS 1121449 */
const int GetObjectsWithKey_method_idx    = 82;     /* PTS 1122194 */
const int LockSharedObj_method_idx        = 83;     /* PTS 1130435 */
const int LockSharedObjMass_method_idx    = 84;     /* PTS 1130435 */
const int SetTransactionCommentMethod_idx = 85;     /* PTS 1130956 */
const int Sleep_method_idx                = 86;     /* PTS 1132163 */
const int IsTestKnlBadAllocActiveMethod_idx = 87;
const int ResetMonitorMethod_idx          = 88;
const int UnlockSharedObj_method_idx      = 89;
const int LockContainer_method_idx        = 90;

#define LVC_MAX_LIVECACHE_SINK_METHODS 91

#ifdef DEFINE_LVC_METHOD_TXT

static char *LiveCacheMethodText[LVC_MAX_LIVECACHE_SINK_METHODS] =
{
	"Method_idx_0",
	"Method_idx_1",
	"Method_idx_2",
	"ABAPRead_method",
	"ABAPWrite_method",
	"AllocPacket_method",
	"BasisError_method",
	"CloseVersion_method",
	"ConsoleMsg_method",
	"CreateOpenVersion",
	"DeleteAllObj_method",
	"DeleteObj_method",
	"EndConsistentView",
	"EnterCriticalSection",
	"GetErrorMsg_method",
	"GetObj_method",
	"GetObjWithKey",
	"HResultError_method",
	"IsDebugEnvironment",
	"IsLocked_method",
	"LeaveCriticalSection_method",
	"LockObj_method",
	"MultiPurpose_method",
	"NewConsistentView_method",
	"NewObj_method",
	"NextObj_method",
	"RegisterClass_method",
	"Rollback_method",
	"SetError_method",
	"SQLReceive_method",
	"SQLRequest_method",
	"SubtransCommit_method",
	"SubtransRollback_method",
	"SubtransStart_method",
	"UnlockObj_method",
	"UpdateObj_method",
	"Vtrace_method",
	"GetDefaultContext_method",
	"SetDefaultContext_method",
	"OpenFile_method",
	"WriteFile_method",
	"CloseFile_method",
	"SyncFile_method",
	"GetDateTime_method",
	"GetClock_method",
	"DropVersion_method",
	"CreateSchema_method",
	"ExistsSchema_method",
	"DropSchema_method",
	"GetSchemaName_method",
	"ExistsFile_method",
	"DropFile_method",
	"GetWorkingDirectory_method",
	"GetDbrootSapPath_method",
	"LoadLibraryEx_method",
	"FreeLibrary_method",
	"DropContainer_method",
	"GetContainerInfo_method",
	"IsObjHistoryUsed_method",
	"Reschedule_method",
	"LockUpdObj_method",
	"Signal_method", /* PTS 1110287 */
	"Wait_method", /* PTS 1110287 */
	"ExistsContainer_method", /* PTS 1110334 */
	"CreateKeyIterator_method",
	"DestroyKeyIterator_method",
	"NextObjFromKeyIter_method",
	"ResetVersion_method" /* PTS 1115134 */
	"PrevObjFromKeyIter_method",      /* PTS 1119480*/
	"PrevObj_method",                 /* PTS 1119480*/ 
	"StartUnloadedVersionIter_method",/* PTS 1119480*/
  "NextVal_method",
  "IsUnicodeInstance_method",
  "NextOids_method",                /* PTS 1120478 */
  "GetObjects_method",              /* PTS 1120478 */
  "GetContainerId_method",          /* PTS 1120478 */
  "DeleteObjMass_method",           /* PTS 1120478 */
  "LockObjMass_method",             /* PTS 1120478 */
  "LockUpdObjMass_method",          /* PTS 1120478 */
  "UpdateObjMass_method",           /* PTS 1120478 */ 
  "UpdateVarObjMass_method",        /* PTS 1121449 */
  "NewObjMass_method",              /* PTS 1121449 */
  "GetObjectsWithKey_method",       /* PTS 1122194 */
  "LockSharedObj_method",           /* PTS 1130435 */
  "LockSharedObjMass_method",       /* PTS 1130435 */
  "SetTransactionComment_method",   /* PTS 1130956 */
  "Sleep_method",                   /* PTS 1132163 */
  "IsTestKnlBadAllocActiveMethod",
  "ResetMonitorMethod",
  "UnlockSharedObj_method",  
  "LockContainer_method"
};
/* +++ PTS 1106969   x_cons extensions for external db-procedures */
#endif  /* DEFINE_LVC_METHOD_TXT */

#endif  /* LVC_METHODNAMES_HPP */
