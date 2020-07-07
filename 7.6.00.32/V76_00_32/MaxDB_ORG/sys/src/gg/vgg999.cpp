/*!**************************************************************************
    module      : vgg999.cpp
    author      : UweH
    special area: KernelCommon
    description : KernelAdministration_Interface
    last changed: 2002-12-11
    copyright:    Copyright (c) 2001-2005 SAP AG


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




*****************************************************************************/
#include "Logging/Log_Types.hpp"
#include "hgg999.h"
#include "hgg02.h" // g02date_time_to_int
#include "hsp41.h" // s41p4int
#include "hbd91.h" // migration variables
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "KernelCommon/Kernel_Administration.hpp"
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "KernelCommon/Kernel_IAdminHotStandby.hpp"
#include "KernelCommon/Kernel_IAdminBackupRecovery.hpp"
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "KernelCommon/Kernel_IAdminConfig.hpp"
#include "KernelCommon/Kernel_Version.hpp"
#include "KernelCommon/Kernel_Exception.hpp"
#include "KernelCommon/Kernel_Messages.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_Task.hpp"
#include "Transaction/Trans_Context.hpp"
#include "Trace/Trace_Writer.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Admin.h"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "SAPDBCommon/SAPDB_string.h"

/* --------------------------------------------------------------------------- */
static inline void CopyNodeIdToNodename(tsp00_NodeId & nodeId, tsp00_NodeIdc & nodeName)
{
    SAPDB_MemFillNoCheck(nodeName, 0, sizeof(nodeName));
    SAPDB_MemMoveNoCheck(nodeName, &nodeId, nodeId.length() );
}
/* --------------------------------------------------------------------------- */
externPascal void gg999Activate (
    tgg00_TransContext    VAR_VALUE_REF  trans)
{
    SAPDBErr_MessageList &errlist = SQLMan_Context::GetContext()->GetErrorList();
    
    if ( ! errlist.IsEmpty() )
    {
        RTE_Message(errlist+Kernel_Exception(__CONTEXT__,KERNEL_UNHANDLED_MESSAGES)); // called until handled by sql manager
        errlist.ClearMessageList();
    }
    
    const Kernel_IAdminConfig::ActivateResult result =
        Kernel_IAdminConfig::Instance().Activate(trans.trTaskId_gg00,trans,errlist);
        
    switch ( result )
    {
        case Kernel_IAdminConfig::ActivateOk:
            trans.trError_gg00 = e_ok;
            break;
        case Kernel_IAdminConfig::ActivateNotAllowed:
            trans.trError_gg00 = e_dbm_command_not_possible;
            break;
        case Kernel_IAdminConfig::ActivateRestartInternFailed:
            Kernel_IAdminRestartShutdown::Instance().Offline(trans.trError_gg00);
        default:
            trans.trError_gg00 = e_not_implemented;
            break;
    }
    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError (trans.trError_gg00,errlist,"Activate");
}
/* --------------------------------------------------------------------------- */
externPascal void gg999Restart (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_Date            VAR_ARRAY_REF  untildate,
    tsp00_Time            VAR_ARRAY_REF  untiltime,
    tsp00_Uint4                          untilIOSequence)
{
    SAPDBErr_MessageList &errlist = SQLMan_Context::GetContext()->GetErrorList();
    
    if ( ! errlist.IsEmpty() )
    {
        RTE_Message(errlist+Kernel_Exception(__CONTEXT__,KERNEL_UNHANDLED_MESSAGES)); // called until handled by sql manager
        errlist.ClearMessageList();
    }
    
    tsp00_Int4 intUntildate;
    tsp00_Int4 intUntilTime;
    g02date_time_to_int (untildate, untiltime, intUntildate, intUntilTime);

    const Kernel_IAdminRestartShutdown::RestartResult result =
        Kernel_IAdminRestartShutdown::Instance().Restart (trans.trTaskId_gg00,
                                                          trans,
                                                          Kernel_Date(intUntildate),
                                                          Kernel_Time(intUntilTime),
                                                          Log_IOSequenceNo(untilIOSequence),
                                                          errlist);
    switch ( result )
    {
    case Kernel_IAdminRestartShutdown::RestartOk:
        trans.trError_gg00 = e_ok;
        break;
    case Kernel_IAdminRestartShutdown::RestartNotAllowed:
        trans.trError_gg00 = e_dbm_command_not_possible;
        break;
    case Kernel_IAdminRestartShutdown::RestartRestartInternFailed:
        Kernel_IAdminRestartShutdown::Instance().Offline(trans.trError_gg00);
    default:
        trans.trError_gg00 = e_not_implemented;
    }
    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError (trans.trError_gg00,errlist,"Restart");
}
/* --------------------------------------------------------------------------- */
externPascal void gg999Shutdown (
    tgg00_TransContext    VAR_VALUE_REF  trans)
{
    Kernel_IAdminRestartShutdown::Instance().Shutdown (trans);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999Offline (
    tgg00_BasisError                     error)
{
    Kernel_IAdminRestartShutdown::Instance().Offline(error);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999Abort (
    pasbool                              writeCore)
{
    Msg_List msg ( Msg_List::Error, SDBMSG_ADMIN_ABORT,
                   Msg_Arg("ERRORTEXT",(writeCore!=0?"core is written":"without core")));
    Kernel_IAdminRestartShutdown::Instance().Offline( msg, true, writeCore!=0 );
}
/* --------------------------------------------------------------------------- */
externPascal void gg999ClearLog (
    tsp00_TaskId                         taskid)
{
    Kernel_IAdminConfig::Instance().ClearLog (taskid);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999RestoreLog (
    tgg00_MessBlock       VAR_VALUE_REF  mblock)
{
    tgg00_TransContext   &oldcontext = *(mblock.mb_trns());
    tsp00_TaskId         &taskid     = oldcontext.trTaskId_gg00;
    SAPDBErr_MessageList &errlist    = SQLMan_Context::GetContext()->GetErrorList();
    
    if ( ! errlist.IsEmpty() )
    {
        RTE_Message(errlist+Kernel_Exception(__CONTEXT__,KERNEL_UNHANDLED_MESSAGES)); // called until handled by sql manager
        errlist.ClearMessageList();
    }

    const Kernel_IAdminBackupRecovery::RestoreLogResult
            result = Kernel_IAdminBackupRecovery::Instance().RestoreLog ( taskid,
                                                                          oldcontext,
                                                                          mblock,
                                                                          errlist );
    switch ( result )
    {
        case Kernel_IAdminBackupRecovery::RestoreLogOk:
            oldcontext.trError_gg00 = e_ok;
            break;
        case Kernel_IAdminBackupRecovery::RestoreLogWrongRole:
            oldcontext.trError_gg00 = e_hotstandby_wrong_role;
            break;
        case Kernel_IAdminBackupRecovery::RestoreLogNotAllowed:
            oldcontext.trError_gg00 = e_dbm_command_not_possible;
            break;
        case Kernel_IAdminBackupRecovery::RestoreLogRestartInternFailed:
        case Kernel_IAdminBackupRecovery::RestoreLogInitError:
        case Kernel_IAdminBackupRecovery::RestoreLogFatalError:
            Kernel_IAdminRestartShutdown::Instance().Offline(oldcontext.trError_gg00);
        case Kernel_IAdminBackupRecovery::RestoreLogError:
            // trError_gg00 is set
            break;
        case Kernel_IAdminBackupRecovery::RestoreLogNewMediumRequired:
            oldcontext.trError_gg00 = e_new_hostfile_required;
            break;
        default:
            oldcontext.trError_gg00 = e_not_implemented;
    }
    if (    (oldcontext.trError_gg00 != e_ok) 
         && (oldcontext.trError_gg00 != e_new_hostfile_required)) // PTS 1135184 mb 2005-04-25 
	{
        Kernel_AppendBasisError (oldcontext.trError_gg00,errlist,"RestoreLog");
	}
}
/* --------------------------------------------------------------------------- */
externPascal void gg999RedoLogForRestore (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_Date            VAR_ARRAY_REF  untildate,
    tsp00_Time            VAR_ARRAY_REF  untiltime,
    tsp00_Uint4                          untilIOSequence)
{
    if ( SQLMan_Context::GetContext() != 0 )
    {
        SAPDBErr_MessageList &globalErrorList = SQLMan_Context::GetContext()->GetErrorList();
        if ( ! globalErrorList.IsEmpty() )
        {
            RTE_Message(globalErrorList+Kernel_Exception(__CONTEXT__,KERNEL_UNHANDLED_MESSAGES)); // called until handled by sql manager
            globalErrorList.ClearMessageList();
        }
    }

    tsp00_Int4 intUntildate;
    tsp00_Int4 intUntilTime;
    g02date_time_to_int (untildate, untiltime, intUntildate, intUntilTime);

    SAPDBErr_MessageList errlist;
    
    const Kernel_IAdminBackupRecovery::RedoLogResult result =
        Kernel_IAdminBackupRecovery::Instance().RedoLog ( trans.trTaskId_gg00, trans,
                                                          Rst_RedoKindRestoreLog,
                                                          Log_RawDeviceOffset(),
                                                          Kernel_Date(intUntildate),
                                                          Kernel_Time(intUntilTime),
                                                          Log_IOSequenceNo(untilIOSequence),
                                                          errlist);
    switch ( result )
    {
        case Kernel_IAdminBackupRecovery::RedoLogOk:
            trans.trError_gg00 = e_ok;
            break;
        case Kernel_IAdminBackupRecovery::RedoLogNotAllowed:
            trans.trError_gg00 = e_dbm_command_not_possible;
            break;
        case Kernel_IAdminBackupRecovery::RedoLogUntilTimeInvalid:
        case Kernel_IAdminBackupRecovery::RedoLogUntilIOSequenceInvalid:
            trans.trError_gg00 = e_invalid;
            break;
        case Kernel_IAdminBackupRecovery::RedoLogReOpenLogForWritingFailed:
        case Kernel_IAdminBackupRecovery::RedoLogRedoLogFailed:
            if ( trans.trError_gg00 == e_ok )
                trans.trError_gg00 = e_redo_log_execute_error;
            if ( errlist.IsEmpty() )
                Kernel_IAdminRestartShutdown::Instance().Offline(trans.trError_gg00);
            else
                Kernel_IAdminRestartShutdown::Instance().Offline(errlist);
        case Kernel_IAdminBackupRecovery::RedoLogRedoLogCancelled:
            trans.trError_gg00 = e_cancelled;
            break;
        default:
            trans.trError_gg00 = e_not_implemented;
            break;
    }
    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError (trans.trError_gg00,errlist,"RedoLog");

    if ( SQLMan_Context::GetContext() != 0 )
    {
        SAPDBErr_MessageList &globalErrorList = SQLMan_Context::GetContext()->GetErrorList();
        globalErrorList = globalErrorList + errlist;
    }
}
/* --------------------------------------------------------------------------- */
externPascal void gg999AbortRedoLogByRestoreComponent (
    tsp00_TaskId                         taskid,
    tgg00_BasisError                     abortReason)
{
    Kernel_IAdminBackupRecovery::Instance().AbortRedoLogByRestoreComponent(taskid, abortReason);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999GetRedoLogProgressInfo (
    tsp00_Int4            VAR_VALUE_REF  transread,
    tsp00_Int4            VAR_VALUE_REF  transredone)
{
    SAPDB_UInt auxtransread   = 0;
    SAPDB_UInt auxtransredone = 0;
    Kernel_IAdminRestartShutdown::Instance().GetRedoManager().GetProgressInfo(auxtransread,auxtransredone);
    transread   = auxtransread;
    transredone = auxtransredone;
}
/* --------------------------------------------------------------------------- */
externPascal tgg00_BasisError gg999DenyBackupAndRestore ()
{
    if ( Kernel_IAdminBackupRecovery::Instance().DenyBackupAndRestore() )
        return e_ok;
    else
        return e_dbm_command_not_possible;
}
/* --------------------------------------------------------------------------- */
externPascal void gg999PermitBackupAndRestore ()
{
    Kernel_IAdminBackupRecovery::Instance().PermitBackupAndRestore();
}
/* --------------------------------------------------------------------------- */
externPascal void gg999BackupData (
    tgg00_MessBlock       VAR_VALUE_REF  mblock)
{
    Kernel_IAdminBackupRecovery::Instance().BackupData
        ( *(mblock.mb_trns()),
          (mm_database == mblock.mb_type2() ? Kernel_IAdminBackupRecovery::backupComplete
                                            : Kernel_IAdminBackupRecovery::backupIncrementell),
          mblock );
}
/* --------------------------------------------------------------------------- */
externPascal void gg999RestoreData (
    tgg00_MessBlock       VAR_VALUE_REF  mblock)
{
    Kernel_IAdminBackupRecovery::Instance().RestoreData( *(mblock.mb_trns()), mblock );
}
/* --------------------------------------------------------------------------- */
externPascal void gg999BackupLog (
    tgg00_MessBlock       VAR_VALUE_REF  mblock)
{
    Kernel_IAdminBackupRecovery::Instance().BackupLog (*(mblock.mb_trns()), mblock);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999BackupRestoreHandling (
    tgg00_MessBlock       VAR_VALUE_REF  mblock)
{
    Kernel_IAdminBackupRecovery::Instance().BackupRestoreHandling (*(mblock.mb_trns()), mblock);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999InitConfig (
    tgg00_TransContext    VAR_VALUE_REF  trans)
{
    Kernel_IAdminConfig::Instance().InitConfig(trans);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999AddDataVolume (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_VFilename       VAR_ARRAY_REF  volumeName,
    SAPDB_Int4                           newVolumeSize,
    SAPDB_Int2                           volumeId)
{
    Kernel_IAdminConfig::Instance().AddDataVolume (trans,
                                                   volumeName,
                                                   newVolumeSize,
                                                   volumeId);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999AddLogVolume (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_VFilename       VAR_ARRAY_REF  volumeName,
    tsp00_VFilename       VAR_ARRAY_REF  mirrorVolumeName,
    SAPDB_Int4                           newVolumeSize,
    SAPDB_Int2                           volumeId)
{
    Kernel_IAdminConfig::Instance().AddLogVolume (trans,
                                                  volumeName,
                                                  mirrorVolumeName,
                                                  newVolumeSize,
                                                  volumeId);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999DropDataVolume (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    SAPDB_Int2                           volumeId)
{
    Kernel_IAdminConfig::Instance().DropDataVolume (trans, volumeId);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999GetRestartVersion (
    tgg00_MessBlock       VAR_VALUE_REF  mblock)
{
    tgg00_TransContext &trans   = *(mblock.mb_trns());
    tgg00_BasisError   &trError = trans.trError_gg00;
    Log_IOSequenceNo    firstUsedIOsequence;
    Log_IOSequenceNo    firstLogIOsequence;
    tsp00_Line          dbidentRestartrecord;
    tsp00_Line          dbidentLogInfo;
    RTE_Nodename        masterNodeName;
    bool                logWritingEnabled;
    bool                logAutoOverwrite;
    bool                restartable;
    SAPDB_Int4          converterVersion;
    SAPDB_Int4          logQueueCount;
    
    Kernel_IAdminInfo::Instance().
        GetRestartVersion (trans,
                           firstUsedIOsequence,  firstLogIOsequence,
                           dbidentRestartrecord, sizeof(dbidentRestartrecord),
                           dbidentLogInfo,       sizeof(dbidentLogInfo),
                           masterNodeName,
                           logWritingEnabled,    logAutoOverwrite,         logQueueCount,
                           restartable,          converterVersion);

    if ( trError != e_ok )
    {
        mblock.mb_type().becomes(m_return_error);
        return;
    }

    tsp00_NodeId auxMasterNodeName;
    auxMasterNodeName.SetBlank();
    SAPDB_RangeMove (__FILE__, 1, strlen(masterNodeName), sizeof(auxMasterNodeName),
                     masterNodeName,  1, auxMasterNodeName, 1, strlen(masterNodeName), trError);
    if ( trError != e_ok )
    {
        mblock.mb_data_len() = 0;
        mblock.mb_type().becomes(m_return_error);
        return;
    }
    // Prepare MessBuffer for result packet
        
    mblock.mb_data_len() = 3 * RESNUM_MXSP00             +
                           2 * (sizeof (tsp00_Line)+1)   +
                           3 * RESNUM_MXSP00             +
                           1 * sizeof (tsp00_NodeId)+1   +   // PTS 1111229 E.Z.
                           1 * RESNUM_MXSP00             +   // PTS 1114658 UH 2002-03-06
                           11+1                          +   // compatible version
                           1 * RESNUM_MXSP00;                // TS 1130336 mb 2004-08-12 queue count

    if ( mblock.mb_data_len() > mblock.mb_data_size() )
    {
        trError              = e_too_small_mb_qual_part;
        mblock.mb_data_len() = 0;
        mblock.mb_type().becomes(m_return_error);
        return;
    }

    tgg00_BasisError dummyTrError = e_ok;
    tsp00_NumError   dummyNumError;
    tsp00_Int4       currentBufferPos = 1;
    tsp00_MoveObj   &buffer           = mblock.mb_data()->mbp_buf();
 
    // ***** 1 **** FIRST IOSequence NEEDED *****
    buffer[currentBufferPos-1] = csp_defined_byte;
    s41pluns (buffer, currentBufferPos + 1, 10, 0,
              firstUsedIOsequence.IsValid() ? (SAPDB_UInt)firstUsedIOsequence.RawValue() : 0, dummyNumError);
    currentBufferPos += RESNUM_MXSP00;
    // ***** 2 **** FIRST IOSequence which had been found on the LogVolume *****
    buffer[currentBufferPos-1] = csp_defined_byte;
    s41pluns (buffer, currentBufferPos + 1, 10, 0,
              firstLogIOsequence.IsValid() ? (SAPDB_UInt)firstLogIOsequence.RawValue() : MAX_INT4_SP00, dummyNumError);
    currentBufferPos += RESNUM_MXSP00;
    // ***** 2 **** RESTARTABLE *****
    buffer[currentBufferPos-1] = csp_defined_byte;
    s41p4int (buffer, currentBufferPos + 1, restartable, dummyNumError);
    currentBufferPos += RESNUM_MXSP00;
    // ***** 4 **** DB IDENT FROM RESTARTRECORD *****
    buffer[currentBufferPos-1] = csp_defined_byte;
    SAPDB_RangeMove  (__FILE__,  1,
            sizeof (dbidentRestartrecord), sizeof (tsp00_Buf),
            dbidentRestartrecord, 1, buffer, currentBufferPos + 1,
            sizeof (dbidentRestartrecord), dummyTrError);
    currentBufferPos += (1 + sizeof (dbidentRestartrecord));
    // ***** 5 **** DB IDENT FROM LOGINFO *****
    buffer[currentBufferPos-1] = csp_defined_byte;
    SAPDB_RangeMove  (__FILE__,  2,
            sizeof (dbidentLogInfo), sizeof (tsp00_Buf),
            dbidentLogInfo, 1, buffer, currentBufferPos + 1,
            sizeof (dbidentLogInfo), dummyTrError);
    currentBufferPos += (1 + sizeof (dbidentLogInfo));
    // ***** 6 **** RESTARTABLE WITOUT LOG *****
    buffer[currentBufferPos-1] = csp_defined_byte;
    s41p4int (buffer, currentBufferPos + 1, true/*compatibility: restartableWithoutLog*/, dummyNumError);
    currentBufferPos += RESNUM_MXSP00;
    // *********************************************************************************+
    // !!!! DO NOT CHANGE ANYTHING ABOVE !!!! (The order is needed by dbmserver.)
    // *********************************************************************************+
    // ***** 7 **** LOG DEVSPACE ENABLED *****
    buffer[currentBufferPos-1] = csp_defined_byte;
    s41p4int (buffer, currentBufferPos + 1, logWritingEnabled, dummyNumError);
    currentBufferPos += RESNUM_MXSP00;
    // ***** 8 **** LOG AUTO OVERWRITE *****
    buffer[currentBufferPos-1] = csp_defined_byte;
    s41p4int (buffer, currentBufferPos + 1, logAutoOverwrite, dummyNumError);
    currentBufferPos += RESNUM_MXSP00;
    // ***** 9 **** MASTER SERVERNODE *****
    // PTS 1111229 E.Z.
    buffer[currentBufferPos-1] = csp_ascii_blank;
    SAPDB_RangeMove  (__FILE__,  3,
            sizeof (tsp00_NodeId), sizeof (tsp00_Buf),
            auxMasterNodeName, 1, buffer, currentBufferPos + 1,
            sizeof (tsp00_NodeId), dummyTrError);
    currentBufferPos += (1 + sizeof (tsp00_NodeId));
    // ***** 10 **** CONVERTER VERSION *****
    // PTS 1115847 E.Z.
    buffer[currentBufferPos-1] = csp_defined_byte;
    s41pluns (buffer, currentBufferPos + 1, 10, 0, converterVersion, dummyNumError);
    currentBufferPos += RESNUM_MXSP00;
    // ***** 11 **** OLDEST COMPATIBLE VERSION *****
    const SAPDB_UInt2 maxStringLength = 11;
    Kernel_Version    oldestCompatibleVersion = Kernel_Migration::GetOldestCompatibleVersion();
    SAPDB_UInt2       maj,min,corr,build,addBlanks=0;
    SAPDB_Char        versionString [maxStringLength+1];
    maj    = oldestCompatibleVersion.MajorVersion();
    min    = oldestCompatibleVersion.MinorVersion();
    corr   = oldestCompatibleVersion.CorrectionLevel();
    build  = oldestCompatibleVersion.BuildNo();
    SAPDB_sprintf( versionString, sizeof(versionString), "%d.%d.%02d.%02d", maj, min, corr, build); 
    SAPDB_memcpy ( &(buffer[currentBufferPos]), versionString, maxStringLength);
    if ( maj < 10 ) addBlanks++;
    if ( min < 10 ) addBlanks++;
    SAPDB_memset ( &(buffer[currentBufferPos+maxStringLength-addBlanks]), ' ', maxStringLength-(maxStringLength-addBlanks));
    buffer[currentBufferPos-1] = csp_ascii_blank;
    currentBufferPos += (maxStringLength+1);
    // ***** 12 **** QUEUE COUNT ***** (PTS 1130336 mb 2004-08-13)
    buffer[currentBufferPos-1] = csp_defined_byte;
    s41pluns (buffer, currentBufferPos + 1, 10, 0, logQueueCount, dummyNumError);
    currentBufferPos += RESNUM_MXSP00;
    // ***** **** *****
    mblock.mb_data_len() = currentBufferPos;
    mblock.mb_type().becomes(m_return_result);
}
/* --------------------------------------------------------------------------- */
externPascal pasbool gg999IsCheckBackupConfigured ()
{
    return Kernel_IAdminInfo::Instance().IsCheckBackupConfigured();
}
/* --------------------------------------------------------------------------- */
externPascal void gg999GetNewCommandId (
    tsp00_TaskId                         taskid,
    tgg00_UtilCmdId       VAR_VALUE_REF  id)
{
    Kernel_IAdminInfo::Instance().GetNewCommandId(taskid, id);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999ReIntegrateBadLogVolume (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_VFilename       VAR_ARRAY_REF  devicename)
{
    Kernel_IAdminConfig::Instance().ReIntegrateBadLogVolume(trans, devicename);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999GetBadLogVolume (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_VFilename       VAR_ARRAY_REF  devicename)
{
    Kernel_IAdminConfig::Instance().GetBadLogVolume(trans, devicename);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999SetLogAutoOverwrite (
    tsp00_TaskId                         taskid,
    pasbool                              on)
{
    Kernel_IAdminConfig::Instance().SetLogAutoOverwrite (taskid, 1==on);
}
/* --------------------------------------------------------------------------- */
externPascal pasbool gg999IsLogAutoOverwriteEnabled (
    tsp00_TaskId                         taskid)
{
    return Kernel_IAdminInfo::Instance().IsLogAutoOverwriteEnabled(taskid);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999SetLogWriting (
    tsp00_TaskId                         taskid,
    pasbool                              on)
{
    Kernel_IAdminConfig::Instance().SetLogWriting (taskid, 1==on);
}
/* --------------------------------------------------------------------------- */
externPascal pasbool gg999IsLogWritingEnabled (
    tsp00_TaskId                         taskid)
{
    return Kernel_IAdminInfo::Instance().IsLogWritingEnabled(taskid);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999SetKernelStateOnline ()
{
    Kernel_IAdminRestartShutdown::Instance().SetKernelStateOnline();
}
/* --------------------------------------------------------------------------- */
externPascal pasbool gg999KernelStateIsOnline ()
{
    return Kernel_IAdminInfo::Instance().KernelStateIsOnline();
}
/* --------------------------------------------------------------------------- */
externPascal pasbool gg999KernelStateIsAdmin ()
{
    return Kernel_IAdminInfo::Instance().KernelStateIsAdmin();
}
/* --------------------------------------------------------------------------- */
externPascal pasbool gg999KernelStateIsRedoLogProcessing ()
{
    return Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing();
}
/* --------------------------------------------------------------------------- */
externPascal void gg999GetBlockAddress (
    tsp00_TaskId                         taskId,
    tsp00_PageNo                         pageNo,
    tgg00_RecoveryMode_Param             recoveryMode,
    tsp00_Int4            VAR_VALUE_REF  deviceNo,
    tsp00_Int4            VAR_VALUE_REF  blockNo)
{
     
    deviceNo = 0;
    blockNo  = 0;

    Data_PageRecoveryMode   recMode;

    switch( recoveryMode )
    {
        case rmPerm_egg00:
                recMode.SetMode( Data_Dynamic, Data_Recoverable );
            break;
        case rmStatic_egg00:
                recMode.SetMode( Data_Static, Data_Recoverable );
            break;
        case rmTemp_egg00:
                recMode.SetMode( Data_Dynamic, Data_NotRecoverable );
        default:
            return;
    }

    const RTETask_ITask &task = *RTETask_ITask::ByTaskID( taskId ); 

    Data_PageNo         dataPageNo = pageNo;
    IOMan_BlockAddress  block;

    Kernel_IAdminDiagnose::Instance().GetBlockAddress( task, dataPageNo, recMode, block );
    
    if( block.IsValid())
    {
        deviceNo = block.GetDeviceNo();
        blockNo  = block.GetBlockNo();
    }
}
/* --------------------------------------------------------------------------- */
externPascal void gg999SetNewBlockAddress (
    tsp00_TaskId                         taskId,
    tgg00_BasisError      VAR_VALUE_REF  error,
    tsp00_PageNo                         pageNo,
    tgg00_RecoveryMode_Param             recoveryMode,
    tsp00_Int4                           deviceNo,
    tsp00_Int4                           blockNo)
{

    Data_PageRecoveryMode   recMode;
    
    switch( recoveryMode )
    {
        case rmPerm_egg00:
            recMode.SetMode( Data_Dynamic, Data_Recoverable );
            break;
        case rmStatic_egg00:
            recMode.SetMode( Data_Static, Data_Recoverable );
            break;
        default: // rmTemp_egg00 and rmNone_egg00
            return;
    }
    const RTETask_ITask &task      = *RTETask_ITask::ByTaskID( taskId ); 
    Data_PageNo         dataPageNo = pageNo;
    IOMan_BlockAddress  block( deviceNo, blockNo );
    
    if ( ! Kernel_IAdminDiagnose::Instance().SetNewBlockAddress( 
        task, dataPageNo, recMode, block) )
    {
        error = e_disk_not_accessible;
    }
}
/* --------------------------------------------------------------------------- */
externPascal void gg999DeleteBlockAddress (
    tsp00_TaskId                         taskId,
    tgg00_BasisError      VAR_VALUE_REF  error,
    tsp00_PageNo                         pageNo,
    tgg00_RecoveryMode_Param             recoveryMode)
{
    Data_PageRecoveryMode   recMode;

    switch( recoveryMode )
    {
        case rmPerm_egg00:
            recMode.SetMode( Data_Dynamic, Data_Recoverable );
            break;
        case rmStatic_egg00:
            recMode.SetMode( Data_Static, Data_Recoverable );
            break;
        default: // rmTemp_egg00 and rmNone_egg00
            return;
    }
    const RTETask_ITask &task      = *RTETask_ITask::ByTaskID( taskId ); 
    Data_PageNo         dataPageNo = pageNo;
    
    if ( ! Kernel_IAdminDiagnose::Instance().DeleteBlockAddress( 
        task, dataPageNo, recMode ))
    {
        error = e_disk_not_accessible;
    }
}
/* --------------------------------------------------------------------------- */
externPascal tgg00_BasisError gg999SuspendLogWriter (
    tsp00_TaskId                         taskid,
    Log_IOSequenceNo&                    lastWrittenIOsequence)
{
    Log_IOSequenceNo aux;
    if ( ! Kernel_IAdminConfig::Instance().SuspendLogWriter (taskid, aux) )
    {
        lastWrittenIOsequence = aux;
        return e_dbm_command_not_possible;
    }
    lastWrittenIOsequence = aux;
    return e_ok;
}
/* --------------------------------------------------------------------------- */
externPascal void gg999ResumeLogWriter (
    tsp00_TaskId                         taskid)
{
    Kernel_IAdminConfig::Instance().ResumeLogWriter(taskid);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999NewDBIdentIfHistoryLost (
    tsp00_TaskId                         taskid)
{
    Kernel_IAdminBackupRecovery::Instance().SetNewDBIdentifierIfHistoryLost(taskid);
}
/* --------------------------------------------------------------------------- */
externPascal pasbool gg999RoleIsStandby ()
{
    return Kernel_IAdminHotStandby::Instance().RoleIsStandby();
}
/* --------------------------------------------------------------------------- */
externPascal void gg999InitStandby (
    tgg00_TransContext    VAR_VALUE_REF  trans)
{

    SAPDBErr_MessageList &errlist = SQLMan_Context::GetContext()->GetErrorList();
    Kernel_IAdminHotStandby::InitStandbyResult result;
    
    if ( ! errlist.IsEmpty() )
    {
        RTE_Message(errlist+Kernel_Exception(__CONTEXT__,KERNEL_UNHANDLED_MESSAGES)); // called until handled by sql manager
        errlist.ClearMessageList();
    }

    result = Kernel_IAdminHotStandby::Instance().InitStandby(trans.trTaskId_gg00, errlist);

    switch ( result )
    {
        case Kernel_IAdminHotStandby::InitStandbyOk:
            trans.trError_gg00 = e_ok;
            break;
        case Kernel_IAdminHotStandby::InitStandbyWrongRole:
            trans.trError_gg00 = e_hotstandby_wrong_role;
            break;
        case Kernel_IAdminHotStandby::InitStandbyNotAllowed:
            trans.trError_gg00 = e_dbm_command_not_possible;
            break;
        case Kernel_IAdminHotStandby::InitStandbyCannotReadMaster:
            Kernel_IAdminRestartShutdown::Instance().Offline(e_disk_not_accessible);
            break;
        case Kernel_IAdminHotStandby::InitStandbyCannotReadOwnName:
            Kernel_IAdminRestartShutdown::Instance().Offline(e_disk_not_accessible);
            break;
        case Kernel_IAdminHotStandby::InitStandbyCreateDataVolumesFailed:
            Kernel_IAdminRestartShutdown::Instance().Offline(e_disk_not_accessible);
            break;
        case Kernel_IAdminHotStandby::InitStandbyCommunicationError:
            Kernel_IAdminRestartShutdown::Instance().Offline(e_hotstandby_cmd_send_failed);
            break;
        case Kernel_IAdminHotStandby::InitStandbyEstablishMirrorFailed:
            Kernel_IAdminRestartShutdown::Instance().Offline(e_hotstandby_establish_mirror_failed);
            break;
        case Kernel_IAdminHotStandby::InitStandbyFreeMemoryFailed:
            Kernel_IAdminRestartShutdown::Instance().Offline();
            break;
        default:
            trans.trError_gg00 = e_not_implemented;
            break;
    }
    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError (trans.trError_gg00,errlist,"InitStandby");
}
/* --------------------------------------------------------------------------- */
externPascal void gg999InsertStandby (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_NodeId          VAR_ARRAY_REF  newNode)
{
    SAPDBErr_MessageList                         &errlist = SQLMan_Context::GetContext()->GetErrorList();
    Kernel_IAdminHotStandby::InsertStandbyResult  result;
    tsp00_NodeIdc                                 nodename;
    
    if ( ! errlist.IsEmpty() )
    {
        RTE_Message(errlist+Kernel_Exception(__CONTEXT__,KERNEL_UNHANDLED_MESSAGES)); // called until handled by sql manager
        errlist.ClearMessageList();
    }

    CopyNodeIdToNodename(newNode, nodename);
    
    result = Kernel_IAdminHotStandby::Instance().InsertStandby(trans.trTaskId_gg00, nodename, errlist);

    switch ( result )
    {
        case Kernel_IAdminHotStandby::InsertStandbyOk:
            trans.trError_gg00 = e_ok;
            break;
        case Kernel_IAdminHotStandby::InsertStandbyWrongRole:
            trans.trError_gg00 = e_hotstandby_wrong_role;
            break;
        case Kernel_IAdminHotStandby::InsertStandbyNotAllowed:
            trans.trError_gg00 = e_dbm_command_not_possible;
            break;
        case Kernel_IAdminHotStandby::InsertStandbyAddNodeFailed:
            trans.trError_gg00 = e_hotstandby_add_node_failed;
            break;
        default:
            trans.trError_gg00 = e_not_implemented;
            break;
    }
    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError (trans.trError_gg00,errlist,"InsertStandby");
}
/* --------------------------------------------------------------------------- */
externPascal void gg999RemoveStandby (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_NodeId          VAR_ARRAY_REF  newNode)
{
    SAPDBErr_MessageList                         &errlist = SQLMan_Context::GetContext()->GetErrorList();
    Kernel_IAdminHotStandby::RemoveStandbyResult  result;
    tsp00_NodeIdc                                 nodename;

    if ( ! errlist.IsEmpty() )
    {
        RTE_Message(errlist+Kernel_Exception(__CONTEXT__,KERNEL_UNHANDLED_MESSAGES)); // called until handled by sql manager
        errlist.ClearMessageList();
    }

    CopyNodeIdToNodename(newNode, nodename);

    result = Kernel_IAdminHotStandby::Instance().RemoveStandby(trans.trTaskId_gg00, nodename, errlist);

    switch ( result )
    {
        case Kernel_IAdminHotStandby::RemoveStandbyOk:
            trans.trError_gg00 = e_ok;
            break;
        case Kernel_IAdminHotStandby::RemoveStandbyWrongRole:
            trans.trError_gg00 = e_hotstandby_wrong_role;
            break;
        case Kernel_IAdminHotStandby::RemoveStandbyNodeNotFound:
            trans.trError_gg00 = e_hotstandby_del_node_failed;
            break;
        case Kernel_IAdminHotStandby::RemoveStandbyNotAllowed:
            trans.trError_gg00 = e_dbm_command_not_possible;
            break;
        default:
            trans.trError_gg00 = e_not_implemented;
            break;
    }
    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError (trans.trError_gg00,errlist,"RemoveStandby");
}
/* --------------------------------------------------------------------------- */
externPascal void gg999RestartStandby (
    tgg00_TransContext    VAR_VALUE_REF  trans)
{
    SAPDBErr_MessageList                          &errlist = SQLMan_Context::GetContext()->GetErrorList();
    Kernel_IAdminHotStandby::RestartStandbyResult  result;

    if ( ! errlist.IsEmpty() )
    {
        RTE_Message(errlist+Kernel_Exception(__CONTEXT__,KERNEL_UNHANDLED_MESSAGES)); // called until handled by sql manager
        errlist.ClearMessageList();
    }

    result = Kernel_IAdminHotStandby::Instance().RestartStandby(trans.trTaskId_gg00, trans, errlist);

    switch ( result )
    {
        case Kernel_IAdminHotStandby::RestartStandbyOk:
            trans.trError_gg00 = e_ok;
            break;
        case Kernel_IAdminHotStandby::RestartStandbyNotAllowed:
            trans.trError_gg00 = e_dbm_command_not_possible;
            break;
        case Kernel_IAdminHotStandby::RestartStandbyWrongRole:
            trans.trError_gg00 = e_hotstandby_wrong_role;
            break;
        case Kernel_IAdminHotStandby::RestartStandbyRestartPrepareFailed:
            Kernel_IAdminRestartShutdown::Instance().Offline(trans.trError_gg00);
            break;
        case Kernel_IAdminHotStandby::RestartStandbyTriggerServerFailed:
            Kernel_IAdminRestartShutdown::Instance().Offline(trans.trError_gg00);
            break;
        default:
            trans.trError_gg00 = e_not_implemented;
            break;
    }
    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError (trans.trError_gg00,errlist,"RestartStandby");
}
/* --------------------------------------------------------------------------- */
externPascal void gg999PrepareStandby (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_NodeId          VAR_ARRAY_REF  newStandbyNode)
{
    SAPDBErr_MessageList                          &errlist = SQLMan_Context::GetContext()->GetErrorList();
    Kernel_IAdminHotStandby::PrepareStandbyResult  result;
    tsp00_NodeIdc                                  nodename;

    if ( ! errlist.IsEmpty() )
    {
        RTE_Message(errlist+Kernel_Exception(__CONTEXT__,KERNEL_UNHANDLED_MESSAGES)); // called until handled by sql manager
        errlist.ClearMessageList();
    }

    CopyNodeIdToNodename(newStandbyNode, nodename);

    result = Kernel_IAdminHotStandby::Instance().PrepareStandby( trans.trTaskId_gg00,
                                                                 trans,
                                                                 errlist,
                                                                 nodename );

    switch ( result )
    {
        case Kernel_IAdminHotStandby::PrepareStandbyOk:
            trans.trError_gg00 = e_ok;
            break;
        case Kernel_IAdminHotStandby::PrepareStandbyAddNodeFailed:
            trans.trError_gg00 = e_hotstandby_add_node_failed;
            break;
        case Kernel_IAdminHotStandby::PrepareStandbySendInfoToStandbyFailed:
            trans.trError_gg00 = e_hotstandby_wrong_role;
            break;
        case Kernel_IAdminHotStandby::PrepareStandbyStartSavepointFailed:
            break;
        case Kernel_IAdminHotStandby::PrepareStandbySplitMirrorFailed:
            trans.trError_gg00 = e_hotstandby_establish_mirror_failed;
            break;
        case Kernel_IAdminHotStandby::PrepareStandbyFreeMemoryFailed:
            Kernel_IAdminRestartShutdown::Instance().Offline(trans.trError_gg00);
            break;
        default:
            trans.trError_gg00 = e_not_implemented;
            break;
    }
    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError (trans.trError_gg00,errlist,"PrepareStandby");
}
/* --------------------------------------------------------------------------- */
externPascal void gg999TakeOver (
    tgg00_TransContext    VAR_VALUE_REF  trans)
{
    SAPDBErr_MessageList                    &errlist = SQLMan_Context::GetContext()->GetErrorList();
    Kernel_IAdminHotStandby::TakeOverResult  result;

    if ( ! errlist.IsEmpty() )
    {
        RTE_Message(errlist+Kernel_Exception(__CONTEXT__,KERNEL_UNHANDLED_MESSAGES)); // called until handled by sql manager
        errlist.ClearMessageList();
    }

    result = Kernel_IAdminHotStandby::Instance().TakeOver( trans.trTaskId_gg00,
                                                           Kernel_Date(),
                                                           Kernel_Time(),
                                                           errlist );
    switch ( result )
    {
        case Kernel_IAdminHotStandby::TakeOverOk:
            trans.trError_gg00 = e_ok;
            break;
        case Kernel_IAdminHotStandby::TakeOverWrongRole:
            trans.trError_gg00 = e_hotstandby_wrong_role;
            break;
        case Kernel_IAdminHotStandby::TakeOverNotAllowed:
            trans.trError_gg00 = e_dbm_command_not_possible;
            break;
        case Kernel_IAdminHotStandby::TakeOverSetRoleToMasterFailed:
            Kernel_IAdminRestartShutdown::Instance().Offline(e_disk_not_accessible);
            break;
        case Kernel_IAdminHotStandby::TakeOverOpenLogAreaFailed:
            Kernel_IAdminRestartShutdown::Instance().Offline(e_disk_not_accessible);
            break;
        default:
            trans.trError_gg00 = e_not_implemented;
            break;
    }
    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError (trans.trError_gg00,errlist,"TakeOver");
}
/* --------------------------------------------------------------------------- */
externPascal void gg999RegisterStandby (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_NodeId          VAR_ARRAY_REF  newStandbyNode,
    tsp00_Int4                           firstNeededOffset,
    tsp00_Int4            VAR_VALUE_REF  maxValidOffset)
{
    SAPDBErr_MessageList                           &errlist = SQLMan_Context::GetContext()->GetErrorList();
    SAPDBMem_IRawAllocator                         &allocator = *(reinterpret_cast <SAPDBMem_IRawAllocator*>(trans.trAllocator_gg00));
    Kernel_IAdminHotStandby::RegisterStandbyResult  result;

    if ( ! errlist.IsEmpty() )
    {
        RTE_Message(errlist+Kernel_Exception(__CONTEXT__,KERNEL_UNHANDLED_MESSAGES)); // called until handled by sql manager
        errlist.ClearMessageList();
    }

    Log_RawDeviceOffset firstNeeded = firstNeededOffset;
    Log_RawDeviceOffset maxValid    = maxValidOffset;
    tsp00_NodeIdc       nodename;
    CopyNodeIdToNodename(newStandbyNode, nodename);

    result = Kernel_IAdminHotStandby::Instance().RegisterStandby ( trans.trTaskId_gg00,
                                                                   allocator,
                                                                   nodename,
                                                                   firstNeeded,
                                                                   maxValid,
                                                                   errlist );

    maxValidOffset = maxValid.RawValue();

    switch ( result )
    {
        case Kernel_IAdminHotStandby::RegisterStandbyOk:
            trans.trError_gg00 = e_ok;
            break;
        case Kernel_IAdminHotStandby::RegisterStandbyWrongRole:
            trans.trError_gg00 = e_hotstandby_wrong_role;
            break;
        case Kernel_IAdminHotStandby::RegisterStandbyNotAllowed:
            trans.trError_gg00 = e_dbm_command_not_possible;
            break;
        case Kernel_IAdminHotStandby::RegisterStandbyAddNodeFailed:
            trans.trError_gg00 = e_hotstandby_add_node_failed;
            break;
        case Kernel_IAdminHotStandby::RegisterStandbyPropagationFailed:
            trans.trError_gg00 = e_hotstandby_cmd_send_failed;
            break;
        default:
            trans.trError_gg00 = e_not_implemented;
            break;
    }
    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError (trans.trError_gg00,errlist,"RegisterStandby");
}
/* --------------------------------------------------------------------------- */
externPascal tsp00_Int4 gg999Synchronize (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_Int4                           newMaxOffset)
{
    SAPDBErr_MessageList                       &errlist = SQLMan_Context::GetContext()->GetErrorList();
    Log_RawDeviceOffset                         lastRedoOffset;
    Kernel_IAdminHotStandby::SynchronizeResult  result;

    if ( ! errlist.IsEmpty() )
    {
        RTE_Message(errlist+Kernel_Exception(__CONTEXT__,KERNEL_UNHANDLED_MESSAGES)); // called until handled by sql manager
        errlist.ClearMessageList();
    }

    result = Kernel_IAdminHotStandby::Instance().Synchronize ( trans.trTaskId_gg00,
                                                               newMaxOffset,
                                                               lastRedoOffset,
                                                               errlist );
    switch ( result )
    {
        case Kernel_IAdminHotStandby::SynchronizeOk:
            trans.trError_gg00 = e_ok;
            break;
        case Kernel_IAdminHotStandby::SynchronizeWrongRole:
            trans.trError_gg00 = e_hotstandby_wrong_role;
            break;
        case Kernel_IAdminHotStandby::SynchronizeNotAllowed:
            trans.trError_gg00 = e_dbm_command_not_possible;
            break;
        default:
            trans.trError_gg00 = e_not_implemented;
            break;
    }
    if ( trans.trError_gg00 != e_ok )
        Kernel_AppendBasisError (trans.trError_gg00,errlist,"Synchronize");
    return lastRedoOffset.RawValue();
}
/* --------------------------------------------------------------------------- */

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void gg999BroadcastBackupHistoryEntry (
    tsp00_TaskId                         taskid,
    char *                               backup_info,
    tsp00_Int4                           info_len)
{
    Kernel_IAdminHotStandby::Instance().BroadcastBackupHistoryEntry(
                    taskid,
                    backup_info,
                    info_len);
}

/* --------------------------------------------------------------------------- */
externPascal void gg999BroadcastBackupMediumDefEntry (
    tsp00_TaskId                         taskid,
    char *                               backup_info,
    tsp00_Int4                           info_len)
{
    Kernel_IAdminHotStandby::Instance().BroadcastBackupMediumDefEntry(
                    taskid, 
                    backup_info,
                    info_len);
}


/* --------------------------------------------------------------------------- */
externPascal void gg999CreateSnapShot (
    tgg00_TransContext    VAR_VALUE_REF  trans)
{
    Kernel_IAdminConfig::Instance().CreateSnapShot (trans);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999ActivateSnapShot (
    tgg00_TransContext    VAR_VALUE_REF  trans)
{
    Kernel_IAdminConfig::Instance().ActivateSnapShot (trans);
}
/* --------------------------------------------------------------------------- */
externPascal void gg999DropSnapShot (
    tgg00_TransContext    VAR_VALUE_REF  trans)
{
    Kernel_IAdminConfig::Instance().DropSnapShot (trans);
}
/* --------------------------------------------------------------------------- */
externPascal pasbool gg999CheckDataCompatibility (
    tsp00_Version         VAR_ARRAY_REF  checkVersion)
{
    Msg_List          localErrorList;
    Kernel_Migration &migration = Kernel_IAdminConfig::Instance().MigrationHandler();
    if ( ! migration.CheckCompatibility( Kernel_Migration::data,
                                         Kernel_Version(checkVersion),
                                         localErrorList ) )
	{
        Trans_Context *pTrans = Kernel_Task::Instance().GetTransContext (*RTETask_ITask::Self());
        if ( pTrans == 0 )
            RTE_Message(localErrorList);
        else
            pTrans->ErrorList().AppendNewMessage(localErrorList);
        return false;
	}
    return true;
}
/* --------------------------------------------------------------------------- */
externPascal pasbool gg999OmsContainerNeedMigration ()
{
    /* still no migration needed for releases > 7.6.00
    Kernel_Migration &migration = Kernel_IAdminConfig::Instance().MigrationHandler();

    bd91MigrateToNewLockprotocol      = migration.SearchForChangeInfo ("omsContainerNewLockProtocol").IsEnabled();
    bd91MigrateToDoubleLinkedFreelist = migration.SearchForChangeInfo ("omsContainerDoubleLinkedFreeList").IsEnabled();
    return bd91MigrateToNewLockprotocol || bd91MigrateToDoubleLinkedFreelist;
    */
    return false;
}
/* --------------------------------------------------------------------------- */
externPascal void gg999WriteUserForcedDump( 
                                            tsp00_TaskId                         TaskId,
                                            tsp00_VFilename       VAR_ARRAY_REF  FileName,
                                            tgg00_DumpStateSet    VAR_ARRAY_REF  DumpStateSet)
{
    const RTETask_ITask &task = *RTETask_ITask::ByTaskID( TaskId ); 
    Trace_Writer::GetInstance().UserForcedDump( task, FileName, DumpStateSet );
}
/* --------------------------------------------------------------------------- */
externPascal void gg999CheckData( 
    tgg00_TransContext    VAR_VALUE_REF  trans,
    pasbool                              bWithExtendedCheck,
    pasbool                              bWithIndexes,
    pasbool                              bWithDeleteUnreferencedBlobs, // PTS 1133300 UH 2005-01-11
    tsp00_Int4            VAR_VALUE_REF  badIndexCount,
    tsp00_Int4            VAR_VALUE_REF  totalBadCount)
{
    Kernel_IAdminDiagnose::Instance().CheckData( trans, bWithExtendedCheck, bWithIndexes, bWithDeleteUnreferencedBlobs, // PTS 1133300 UH 2005-01-11
        badIndexCount,totalBadCount );
}
/* --------------------------------------------------------------------------- */
externPascal void gg999CheckDataWithUpdate( 
    tgg00_TransContext    VAR_VALUE_REF  trans,
    pasbool                              bWithExtendedCheck,
    tsp00_Int4            VAR_VALUE_REF  pagesUsed,
    tsp00_Int4            VAR_VALUE_REF  blocksReleased,
    tsp00_Int4            VAR_VALUE_REF  badIndexCount,
    tsp00_Int4            VAR_VALUE_REF  totalBadCount)
{
    Kernel_IAdminDiagnose::Instance().CheckDataWithUpdate( trans, bWithExtendedCheck,
        pagesUsed, blocksReleased, badIndexCount,totalBadCount );
}
/* --------------------------------------------------------------------------- */
externPascal void ggg999Diagnose( tgg00_MessBlock VAR_VALUE_REF  messBlock )
{
    Kernel_IAdminDiagnose::Instance().Diagnose( messBlock );
}
/* --------------------------------------------------------------------------- */
externPascal void gg999MigrateVolumes( tgg00_TransContext    VAR_VALUE_REF  trans )
{
    SAPDBErr_MessageList msgList;
    
    Kernel_IAdminConfig::Instance().MigrateVolumes( trans, msgList );
    
    if(( e_ok != trans.trError_gg00 ) && ( ! msgList.IsEmpty())){
        SQLMan_Context::GetContext()->GetErrorList() = msgList;
    }
}
/* --------------------------------------------------------------------------- */

externPascal void gg999TryToResumeRedoReader (
    tsp00_TaskId                         taskid)
{
    Kernel_IAdminRestartShutdown::Instance().GetRedoManager().TryToResumeRedoReader(taskid);
}
