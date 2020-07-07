/*!**************************************************************************

  module      : Log_Types.cpp
  special area: Logging
  responsible : UweH
  created     : 2000-09-25  12:00
  last changed: 2000-10-27  12:00
  copyright:    Copyright (c) 2000-2005 SAP AG
  description : output functions for types defined here.



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


*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Logging/Log_Types.hpp"
#ifndef INDIAGNOSE
#include "KernelCommon/Kernel_VTrace.hpp"
#endif
#include "RunTime/RTE_Message.hpp"
#include "Logging/Log_Exceptions.hpp"

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

static const char* ActionTypeStrings [] = // PTS 1134693 UH 2005-04-06 changed to static(local)
{
    "NoOp",
    "Commit",
    "Rollback",
    "PartialRollback",
    "NewObject",
    "InsertObject",
    "UpdateObject",
    "DeleteObject",
    "LockObject",
    "CreateObjectFile",
    "DropObjectFile",
    "InsertRecord",
    "DeleteRecord",
    "UpdateRecord",
    "ModifyTable",
    "ChangeFileType",
    "CreateFile",
    "CreateIndex",
    "CreateTable",
    "RenameFile",
    "SuccFileVersion",
    "UpdFileStatistic",
    "CopyBlob",
    "CopySameBlob",
    "ExpandBlob",
    "TruncateBlob",
    "UpdateBlob",
    "RefreshSequence",
    "RefreshSurrogate",
    "FinishOpenTrans",
    "SavepointEntry",
    "MarkedDeleted",
    "ReorganizeFile",
    "SetFileAttributes",
    "NOT_USED",
    "NOT USED",
    "CreateIndexFile",
    "ReassignIndexFile",
    "MarkFileAsDeleted",
    "CreateShortColumnFile",
    "UnqualifiedDelete",
    "MultipleActions",
    "SetFileDirCounter",
    "+++IllegalType+++" // PTS 1134693 UH 2005-04-06
};

/*---------------------------------------------------------------------------*/

const char* Log_GetActionTypeString(const Log_ActionType type)
{
    // PTS 1134693 UH 2005-04-06 new
    if ( type > Log_IllegalType )
        return ActionTypeStrings[Log_IllegalType];
	else
        return ActionTypeStrings[type];
}

/*---------------------------------------------------------------------------*/
const char * Log_ErrorStateStrings [] =
{
    "Ok",                                   //  0
    "LogAndDataIncompatible",               //  1
    "RawAllocationFailed",                  //  2
    "PageAllocationFailed",                 //  3
    "LogDeviceTooSmall",                    //  4
    "InconsistentLogDeviceIOSequence",      //  5
    "LogSaveDistanceTooLarge",              //  6
    "LogSizeHasDecreased",                  //  7
    "InconsistentLogInfoPage",              //  8
    "SavepointPositionNotFound",            //  9 
    "SaveForAbortedClearLog",               // 10
    "LogAreaOpenError",                     // 11
    "LogAndSoftwareIncompatible",           // 12
    "LogQueueCreationFailed"                // 13
};

/*---------------------------------------------------------------------------*/
const char * Log_DeviceStateStrings [] =
{
    "Okay",
    "HistoryLost",
    "Cleared",
    "Clearing",
    "PartiallyCleared",
    "PartiallyClearing"
};

/*---------------------------------------------------------------------------*/
const char * Log_SysviewDeviceStateStrings [] =
{
    "OK",
    "HISTORY LOST",
    "CLEARED",
    "CLEARING",
    "PARTIALLY CLEARED",
    "PARTIALLY CLEARING"
};

/*---------------------------------------------------------------------------*/
const char * Log_FlushModeStrings [] =
{
    "MinimizeSpace",
    "MaximizeSafety"
};

/*---------------------------------------------------------------------------*/
const char * Log_SysviewFlushModeStrings [] =
{
    "MINIMIZE SPACE",
    "MAXIMIZE SAFETY"
};

/*---------------------------------------------------------------------------*/
const char * Log_SavepointReasonStrings [] =
{
    "Unknown",
    "SaveData",
    "SavePages",
    "Restart",
    "Shutdown",
    "Distance",
    "Time",
    "UserRequested",
    "TransRequested", // not used
    "FreeBlockManagement",
    "Pager",
    "BDLockList", // not used
    "EndReadOnly",
    "DataCache", // not used
    "Log_Writer",
    "Converter",
    "EndLoad",
    "CreateIndex",
    "CheckTable",
    "CatalogMigration",
    "SplitMirror",
    "DropVolume",
    "StartDropVolume"
};

/*---------------------------------------------------------------------------*/
SAPDB_OStream & operator << (SAPDB_OStream &ostream, Log_RawDeviceOffset offset)
{
    if (offset.IsInvalid())
    {
        return ostream << "Nil";
    }
    else
    {
        return ostream << offset();
    }
}

/*---------------------------------------------------------------------------*/
SAPDB_OStream & operator << (SAPDB_OStream &ostream, Log_IOSequenceNo ioSeq)
{
    if (ioSeq.IsInvalid())
    {
        return ostream << "Nil";
    }
    else
    {
        return ostream << ioSeq();
    }
}

#ifdef INDIAGNOSE
/*---------------------------------------------------------------------------*/
void Log_EntryInfo::WriteToTrace(const char *title)
{}
#else
/*---------------------------------------------------------------------------*/
void Log_EntryInfo::WriteToTrace(const char *title)
{
    Kernel_VTrace trace;
    if ( title != NULL )
        trace << title << ": ";
    trace << pageoffset << "." << entryoffset
          << " [" << iosequence << "] '"
          << Log_GetActionTypeString(entrytype)
          << "' " << entrydate << "-" << entrytime;
}
#endif

/*---------------------------------------------------------------------------*/
void Log_EntryInfo::Message (const char *title,
              SAPDB_UInt  msgcount,
              SAPDB_UInt  transno,
              SAPDB_UInt  entrysequence) const
{
    #ifndef INDIAGNOSE
    RTE_Message ( Log_Exception ( __CONTEXT__, LOG_ENTRY_INFO,
                  title,
                  SAPDB_ToString(msgcount,_T_d),
                  SAPDB_ToString(transno,_T_d),
                  SAPDB_ToString(entrysequence,_T_d),
                  SAPDB_ToString(iosequence,_T_d),
                  SAPDB_ToString(pageoffset,_T_d),
                  SAPDB_ToString(entryoffset,_T_d),
                  Log_GetActionTypeString(entrytype),
                  SAPDB_ToString(entrydate,_T_d),
                  SAPDB_ToString(entrytime,_T_d) ) );
    #endif
}

/* --------------------------------------------------------------------------- */
void Log_EntryInfo::AppendContextInfo (SAPDB_Char  const * const  filename,
                                       SAPDB_UInt4 const          linenumber,
                                       SAPDB_UInt                 transno,
                                       SAPDB_UInt                 entrysequence,
                                       SAPDBErr_MessageList      &errlist) const
{
    #ifndef INDIAGNOSE
    errlist = errlist +
              Log_Exception ( filename,linenumber, LOG_ENTRY_INFO,
                  "#", "#",
                  SAPDB_ToString(transno,_T_d),
                  SAPDB_ToString(entrysequence,_T_d),
                  SAPDB_ToString(iosequence,_T_d),
                  SAPDB_ToString(pageoffset,_T_d),
                  SAPDB_ToString(entryoffset,_T_d),
                  Log_GetActionTypeString(entrytype),
                  SAPDB_ToString(entrydate,_T_d),
                  SAPDB_ToString(entrytime,_T_d) );
    #endif
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
