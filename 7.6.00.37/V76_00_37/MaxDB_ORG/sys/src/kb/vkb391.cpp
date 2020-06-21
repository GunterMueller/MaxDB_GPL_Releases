/*!
  @file           vkb391.cpp
  @author         MartinB
  @author         UweH
  @ingroup        LogArea
  @brief          Interface to logging component

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


#include "Logging/Log_Page.hpp"
#include "Logging/Log_SaveIterator.hpp"
#include "Logging/Log_Volume.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "KernelCommon/Kernel_Task.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_TaskWaitList.hpp"
#include "KernelCommon/Kernel_Version.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Backup.h"
#include "SAPDBCommon/Messages/SDBMsg_Log.h"

#include "hkb03_1.h" // kb03Check
#include "hkb57.h"   // k57save_restartrecord
#include "hkb57_1.h" // k57restartrec
#include "heo47.h"   // sql47_ptoc

#include "hkb391.h"

/*---------------------------------------------------------------------------*/
externPascal void kb391InitLogSaveIterator (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_Int2                           maxToBeRead,
    pasbool                              dbIsCold,
    pasbool                              forAutosave,
    pasbool                              completeLog,
    pasbool                              repeatable)

{
    SAPDBTRACE_ROUTINE_DEBUG ("kb391InitLogSaveIterator", LogVolume_Trace, 5);

    trans.trError_gg00 = e_ok;

    if (maxToBeRead < 0)
    {
        RTE_Crash( SAPDBErr_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                      "kb391InitLogSaveIterator:max_to_be_read < 0"));
    }

    SAPDBMem_IRawAllocator& allocator =
              *(reinterpret_cast <SAPDBMem_IRawAllocator*>(trans.trAllocator_gg00));

    Log_Volume           &log = Log_Volume::Instance();
    Log_Volume::SaveRange  saveRange;

    if (forAutosave)                                  //PTS 1128703 mb 2004-04-23
    {
        saveRange = Log_Volume::OnlyCompleteLogSegment;
    }
    else if (completeLog)
    {
        saveRange = Log_Volume::AllAtOnce;
    }
    else
    {
        saveRange = Log_Volume::AllIncompleteSegmentsAllowed;
    }

    Log_Volume::InitLogSaveResult  result = log.InitLogSaveIterator( trans.trTaskId_gg00,
                                                              allocator,
                                                              saveRange,
                                                              repeatable,
                                                              maxToBeRead);
    switch (result)
    {
        case Log_Volume::Okay:
                trans.trError_gg00 = e_ok;
                break;
        case Log_Volume::NoLogToSave:
                if ( forAutosave )
                    trans.trError_gg00 = e_incomplete_logsegm;
                else
                    trans.trError_gg00 = e_no_log_to_save;
                break;
        case Log_Volume::HistoryIsLost:
                trans.trError_gg00 = e_backup_history_lost; 
                break;
        case Log_Volume::IncompleteSegment:
                trans.trError_gg00 = e_incomplete_logsegm;
                break;
        case Log_Volume::InitializeFailed:
        default:
                trans.trError_gg00 = e_redo_log_read_error;
                break;
    }
}
/*---------------------------------------------------------------------------*/
externPascal void kb391InvalidateSaveIterator ( )
{
    Log_Volume::Instance().GetLogSaveIter().Invalidate();
}
/*-----------------------------------------------------------------------------------*/
tgg00_BasisError MapFreeLogResult(const Log_Volume::FreeLogResult result)
{
    
    switch (result)
    {
        case Log_Volume::FreeLogOk:
        case Log_Volume::RepeatedLogSaveNeeded:
            return e_ok;
            break;
        case Log_Volume::IOSequenceDoesNotMatch:
            return e_invalid;
            break;
        case Log_Volume::LogVolumeNotAccessible:
        case Log_Volume::NoSaveLogExecutedBefore:
        default:
            return e_log_error;
    }
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb391FreeLog ( tsp00_TaskId                         taskId,
                                 tgg00_BasisError      VAR_VALUE_REF  trError)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb391FreeLog", LogVolume_Trace, 5);
    Log_Volume& log = Log_Volume::Instance();

    trError = MapFreeLogResult(log.FreeLog (taskId));    
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb391FreeLogForPipe (
    tsp00_TaskId                         taskId,
    const Log_IOSequenceNo&              firstSavedIOsequence,
    const Log_IOSequenceNo&              lastSavedIOsequence,
    tgg00_BasisError      VAR_VALUE_REF  trError)
{
    // PTS 1114791 mb 2002-08-22 new
    Log_Volume &log = Log_Volume::Instance();

    trError = MapFreeLogResult(log.FreeLogForPipe(taskId, firstSavedIOsequence, lastSavedIOsequence));
}


/*---------------------------------------------------------------------------*/
externPascal void kb391ReadNextLogpages (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tkb3_block_ptr                       queue_buffer,
    tsp00_Int2            VAR_VALUE_REF  numRead)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb391ReadNextLogpages", LogVolume_Trace, 5);

    Log_SaveIterator&  iterator=Log_Volume::Instance().GetLogSaveIter();

    if ( iterator.IsValid() )
    {
        Trans_Context        &context = Trans_Context::GetContext(trans);
        Log_DeviceBlockCount  unsignedNumPagesRead;
    
        if ( ! iterator.GetNextLogPages ( trans.trTaskId_gg00, 
                                          queue_buffer, 
                                          unsignedNumPagesRead,
                                          context.ErrorList() ) )
        {
            trans.trError_gg00 = e_log_error;
        }
        numRead = unsignedNumPagesRead;
    }
    else
    {
        numRead = 0;
    }
}
/*---------------------------------------------------------------------------*/
externPascal void kb391GetSaveIterInfo (
    Kernel_Time&                         startTime,
    Kernel_Date&                         startDate,
    Kernel_Time&                         endTime,
    Kernel_Date&                         endDate,
    Log_IOSequenceNo&                    startIOSeq,
    Log_IOSequenceNo&                    endIOSeq,
    Log_RawDeviceOffset&                 pageCount,
    tsp00_Int4           VAR_VALUE_REF  backupCount)
{
    Log_SaveIterator & saveIter = Log_Volume::Instance().GetLogSaveIter();
    
    saveIter.GetStartTime(startDate, startTime);
    saveIter.GetEndTime(endDate, endTime);
    startIOSeq =  saveIter.GetStartIOSequence();
    endIOSeq =    saveIter.GetEndIOSequence();
    backupCount = saveIter.GetLogBackupCount();
    pageCount =   saveIter.GetSizeOfCurrentSegment(); // PTS 1138588 mb 2005-11-01
}

/*---------------------------------------------------------------------------*/
externPascal void kb391GetNumOfPagesLeftForSegment (
    tsp00_Uint4           VAR_VALUE_REF  numOfUnsavedPages)
{
    if ( Log_Volume::Instance().GetLogSaveIter().IsValid() )
        numOfUnsavedPages = 0;
	else
        numOfUnsavedPages = Log_Volume::Instance().GetLogSaveIter().GetNumPagesLeft();
}
/*---------------------------------------------------------------------------*/
externPascal void kb391GetNumOfPagesLeftForLogDev (
    tsp00_Uint4           VAR_VALUE_REF  numOfUnsavedPages)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb391GetNumOfPagesLeftForLogDev", LogVolume_Trace, 5);

    Log_Volume &log = Log_Volume::Instance();
    Log_SaveIterator&  iterator=Log_Volume::Instance().GetLogSaveIter();

    if ( iterator.IsInvalid() )
    {
        numOfUnsavedPages = log.GetNumberOfUnsavedPages();
        return;
    }

    Log_DeviceBlockCount autosaveDistance = log.GetAutosaveDistance();

    if ( log.GetNumberOfUnsavedPages() >= autosaveDistance )
        numOfUnsavedPages = log.GetNumberOfUnsavedPages()
                            - autosaveDistance + iterator.GetNumPagesLeft();
    else
        // the last segment to be saved
        numOfUnsavedPages = iterator.GetNumPagesLeft();
}
//----------------------------------------------------------------------------
externPascal pasbool kb391IOSeqIsInPage (
    const Log_IOSequenceNo               &   ioSequence,
          tkb00_Page         VAR_VALUE_REF   page)
{
    Data_PageFrame frame(&page, FrameCtrl_ICommon::GetInstance().GetPageSize());
    Log_Page logPage(frame);
    return ioSequence.IsInRange(logPage.ReadFirstWriterIOSequenceNo(),
                                logPage.ReadLastWriterIOSequenceNo());
}
//----------------------------------------------------------------------------
externPascal pasbool kb391IOSeqIsYoungerThanPage (
    const Log_IOSequenceNo&                 ioSequence,
    const Log_IOSequenceNo&                 oldestKnownIOSeq,
    tkb00_Page            VAR_VALUE_REF  page)
{
    Data_PageFrame frame(&page, FrameCtrl_ICommon::GetInstance().GetPageSize());
    Log_Page logPage(frame);

    return Log_IOSequenceNo::LeftIsYounger( ioSequence,  
                                            logPage.ReadLastWriterIOSequenceNo(),
                                            oldestKnownIOSeq);
}
//----------------------------------------------------------------------------
Kernel_TaskWaitList kb391WaitListAutosaveRead;
Kernel_TaskWaitList kb391WaitListAutosaveWrite;
Kernel_TaskWaitList kb391WaitListBackupRead;
Kernel_TaskWaitList kb391WaitListBackupWrite;
//----------------------------------------------------------------------------
externPascal void kb391InitializeWaitLists ()
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb391InitializeWaitLists", LogVolume_Trace, 5);
	// necessary as long as we have a C++ Main so the compiler generates
    // code to call the constructors of global variables.
    kb391WaitListAutosaveRead  = Kernel_TaskWaitList();
    kb391WaitListAutosaveWrite = Kernel_TaskWaitList();
    kb391WaitListBackupRead    = Kernel_TaskWaitList();
    kb391WaitListBackupWrite   = Kernel_TaskWaitList();
}
//----------------------------------------------------------------------------
static inline Kernel_TaskWaitList& GetWaitList ( bool forRead,
                                                 bool forAutosave )
{
    if ( forRead )
        return forAutosave ? kb391WaitListAutosaveRead : kb391WaitListBackupRead;
	else
        return forAutosave ? kb391WaitListAutosaveWrite : kb391WaitListBackupWrite;
}
/*---------------------------------------------------------------------------*/
externPascal void kb391ResumeAllWaitingTask (
    pasbool                              forAutosave)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb391ResumeAllWaitingTask", LogVolume_Trace, 5);
    if ( forAutosave )
    {
        kb391WaitListAutosaveRead.ResumeAll();
        kb391WaitListAutosaveWrite.ResumeAll();
    }
    else
    {
        kb391WaitListBackupRead.ResumeAll();
        kb391WaitListBackupWrite.ResumeAll();
    }
}
/*---------------------------------------------------------------------------*/
externPascal void kb391ResumeFirstWaitingTask (
    pasbool                              forRead,
    pasbool                              forAutosave)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb391ResumeFirstWaitingTask", LogVolume_Trace, 5);
    GetWaitList(forRead,forAutosave).ResumeFirst();
}
/*---------------------------------------------------------------------------*/
externPascal void kb391InsertWaitingTaskAtLast (
    tsp00_TaskId                         taskid,
    tgg00_WaitContext     VAR_VALUE_REF  waitcontext,
    pasbool                              forRead,
    pasbool                              forAutosave)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb391InsertWaitingTaskAtLast", LogVolume_Trace, 5);
    GetWaitList(forRead,forAutosave).InsertAsLast (taskid, waitcontext);
}
/*---------------------------------------------------------------------------*/
externPascal void kb391Create_RTEConf_ParameterAccess (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_Addr            VAR_VALUE_REF  params)
{
    SAPDBMem_IRawAllocator& allocator    = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(trans.trAllocator_gg00));
    const SAPDB_Char *      dummyDbname  = "";
    const SAPDB_Int4        dummyVersion = 0;

    if ( params != 0 )
        RTE_Crash(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_ARGUMENT_FAILED, "params == 0"));

    RTEConf_Parameter* auxParams = new (allocator)
        RTEConf_Parameter(dummyDbname, dummyVersion, RTECONF_MAXNAMELENGTH, RTECONF_MAXSTRINGLENGTH);

    if ( auxParams == 0 )
        trans.trError_gg00 = e_no_more_memory;

    params = reinterpret_cast<tsp00_Addr>(auxParams);
}
/*---------------------------------------------------------------------------*/
externPascal void kb391AppendTo_RTEConf_ParameterAccess (
    RTEConf_Parameter&                   params,
    tkb3_xparam_page      VAR_VALUE_REF  xpPage)
{
    SAPDB_Byte * data = reinterpret_cast<SAPDB_Byte*>(&(xpPage.xp_data()));
    SAPDBErr_MessageList  error;
    if ( ! params.AddBinaryData(data, xpPage.xp_length(), error) )
    {
        RTE_Message(error);
        return;
    }
}
/*---------------------------------------------------------------------------*/
externPascal tgg00_BasisError kb391CheckParameterForRestore (
    RTEConf_Parameter&                   params)
{
    SAPDBErr_MessageList error;
    SAPDB_Bool           dataRead;

    if ( ! params.InterpretBinaryData(dataRead, error) )
    {
        RTE_Message(error);
        return e_conf_parameter_error;
    }

    // Add the checks here
    
    RTEConf_Parameter::String valueFromBackup;
    RTEConf_Parameter::String valueFromInstance;
    const SAPDB_UTF8 * paramName = UTF8("_UNICODE");

    if ( ! RTEConf_ParameterAccess::Instance()->GetString(paramName, valueFromInstance, error) ) 
    {
        RTE_Message(error);
        return e_conf_parameter_error;
    }

    if ( ! params.GetReference(paramName, valueFromBackup, error) ) 
    {
        RTE_Message(error);
        return e_conf_parameter_error;
    }
    
    if ( valueFromInstance[0] != valueFromBackup[0] )
    {
        RTE_Message(SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"_UNICODE must be equal"));
        return e_wrong_configuration;
    }

    return e_ok;
}
/*---------------------------------------------------------------------------*/
externPascal void kb391Destroy_RTEConf_ParameterAccess (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_Addr            VAR_VALUE_REF  params)
{
    SAPDBMem_IRawAllocator& allocator = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(trans.trAllocator_gg00));
    destroy(params, allocator);
    params = 0;
}
/*-----------------------------------------------------------------------------------*/

externPascal pasbool kb391SupressDBIdentCheckForMigration (    // PTS 1116030 mb 2002-06-03 new
    tsp00_Version         VAR_ARRAY_REF  backupVersion)
{
    // Migration: DBIdentifier may not be correct initialized in 7.4.1 or 7.4.2 with build > 7
    Kernel_Version auxBackupVersion (backupVersion);

    if ( auxBackupVersion == Kernel_Version(7,4,1)
         ||
         (auxBackupVersion == Kernel_Version(7,4,2) && auxBackupVersion.BuildNo() > 7) )
    {
        return true;
    }

    return false;
}

/*-----------------------------------------------------------------------------------*/

externPascal void kb391CheckLogTapeAndDataCompatibility (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tkb00_Page&                    restartRecord,
    tsp00_Version         VAR_ARRAY_REF  backupVersion,
    tsp00_Line            VAR_ARRAY_REF  dbident,
    const Log_IOSequenceNo&                 firstIOSeq,
    const Log_IOSequenceNo&                 lastIOSeq)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb391CheckLogTapeAndDataCompatibility", Common_Trace, 5);
    
    Msg_List &errlist = Trans_Context::GetContext(trans).ErrorList();
    trans.trError_gg00 = e_ok;

    const Log_IOSequenceNo lastSavepointIOSequence = restartRecord.rstLastSavept_kb00().svpIOsequence_kb00;
    
    // PTS 1113550 mb 2002-05-06
    if ( lastSavepointIOSequence.IsInvalid() )
    {
        if ( firstIOSeq == MIN_IOSEQUENCE )
        {
            // for a restart from the Log_IOSequence 0 into an empty DB: Copy DB-Identifier
            k57restartrec->rstDbIdent_kb00() = dbident;
            k57save_restartrecord (trans.trTaskId_gg00);
    
            Log_Volume &log = Log_Volume::Instance();
            log.SetLogDBIdent (trans.trTaskId_gg00,dbident);
        }
        else
        {
            // RESTORE LOG directly after INIT CONFIG without RESTORE DATA
            trans.trError_gg00 = e_incompatible_log;
            errlist.AppendNewMessage(
                Msg_List(
                    Msg_List::Error,
                    SDBMSG_LOG_SAVEPOINT_IOSEQ_TAPE_MISMATCH,
                    Msg_Arg(SDBMSGTAG_LOG_SAVEPOINT_IOSEQ_TAPE_MISMATCH__LOG_IO_SEQUENCE,SAPDB_ToString(lastSavepointIOSequence)),
                    Msg_Arg(SDBMSGTAG_LOG_SAVEPOINT_IOSEQ_TAPE_MISMATCH__FIRST_IOSEQUENCE,SAPDB_ToString(firstIOSeq)),
                    Msg_Arg(SDBMSGTAG_LOG_SAVEPOINT_IOSEQ_TAPE_MISMATCH__LAST_IOSEQUENCE,SAPDB_ToString(lastIOSeq))));
        }
        return;
    }

    /* PTS 1113550 mb 2002-22-03 && PTS 1133214 mb 2005-01-11 */
    if (restartRecord.rstDbIdent_kb00()[0] != '\0')
    {
        if (dbident[0] != '\0')
        {
            if (memcmp(dbident, &(restartRecord.rstDbIdent_kb00()), sizeof(tsp00_Line)))
            {
                if (!kb391SupressDBIdentCheckForMigration(backupVersion))
                {
                    SAPDB_Char dataDBIdent[sizeof(tsp00_Line) + 1];
                    SAPDB_Char logDBIdent[sizeof(tsp00_Line) + 1];
            
                    sql47_ptoc(logDBIdent, (SAPDB_Char*)dbident, sizeof(tsp00_Line));
                    sql47_ptoc(dataDBIdent, (SAPDB_Char*) &(restartRecord.rstDbIdent_kb00()), sizeof(tsp00_Line));
                    errlist.AppendNewMessage(Log_Exception(__CONTEXT__,LOG_DBIDENT_MISMATCH, dataDBIdent, logDBIdent));
                    errlist.AddDetail       (SDBMSG_LOG_LOG_AND_DATA_INCOMPATIBLE,
                                             Msg_Arg(SDBMSGTAG_LOG_LOG_AND_DATA_INCOMPATIBLE__REASON,"The DB Identifiers of data and logbackup are not equal"));

                    trans.trError_gg00 = e_invalid_label;
                    return;
                }
            }
        }

        // check logvolume and logbackup
        Log_Volume & log = Log_Volume::Instance();
        tsp00_Line   logVolumeDBIdent;
        log.GetLogDBIdent ( logVolumeDBIdent );
        if ((logVolumeDBIdent[0] != '\0') && !log.LogIsEmpty())
        {
            if (memcmp(logVolumeDBIdent, &(restartRecord.rstDbIdent_kb00()), sizeof(tsp00_Line)))
            {
                if (!kb391SupressDBIdentCheckForMigration(backupVersion))
                {
                    SAPDB_Char dataDBIdent[sizeof(tsp00_Line) + 1];
                    SAPDB_Char logDBIdent[sizeof(tsp00_Line) + 1];
            
                    sql47_ptoc(logDBIdent, (SAPDB_Char*)logVolumeDBIdent, sizeof(tsp00_Line));
                    sql47_ptoc(dataDBIdent, (SAPDB_Char*) &(restartRecord.rstDbIdent_kb00()), sizeof(tsp00_Line));
                    errlist.AppendNewMessage(Log_Exception(__CONTEXT__,LOG_DBIDENT_MISMATCH, dataDBIdent, logDBIdent));
                    errlist.AddDetail       (SDBMSG_LOG_LOG_AND_DATA_INCOMPATIBLE,
                                             Msg_Arg(SDBMSGTAG_LOG_LOG_AND_DATA_INCOMPATIBLE__REASON,"The DB Identifiers of data and log are not equal"));
                    
                    trans.trError_gg00 = e_invalid_label;
                    return;
                }
            }
        }
    }

    if ( lastSavepointIOSequence.IsValid())
    {
        if (!lastSavepointIOSequence.IsInRange(firstIOSeq, lastIOSeq))
        {
            errlist.AppendNewMessage(
                Msg_List(
                    Msg_List::Error,
                    SDBMSG_LOG_SAVEPOINT_IOSEQ_TAPE_MISMATCH,
                    Msg_Arg(SDBMSGTAG_LOG_SAVEPOINT_IOSEQ_TAPE_MISMATCH__LOG_IO_SEQUENCE,SAPDB_ToString(lastSavepointIOSequence)),
                    Msg_Arg(SDBMSGTAG_LOG_SAVEPOINT_IOSEQ_TAPE_MISMATCH__FIRST_IOSEQUENCE,SAPDB_ToString(firstIOSeq)),
                    Msg_Arg(SDBMSGTAG_LOG_SAVEPOINT_IOSEQ_TAPE_MISMATCH__LAST_IOSEQUENCE,SAPDB_ToString(lastIOSeq))));
            trans.trError_gg00 = e_incompatible_log;
            return;
        }
    }
}

/*-----------------------------------------------------------------------------------*/

externPascal pasbool kb391ValidLogSaveIterator ()
{
    return Log_Volume::Instance().GetLogSaveIter().IsValid();    
}

/*-----------------------------------------------------------------------------------*/

externPascal void kb391CheckLogBackup (
    tsp00_TaskId                         taskId,
    pasbool               VAR_VALUE_REF  rteCancelFlag,
    const Log_IOSequenceNo&                 firstIOSequence,
    const Log_IOSequenceNo&                 lastIOSequence,
    tgg00_BasisError      VAR_VALUE_REF  trError)
{
    if ( ! Log_Volume::Instance().CheckLogBackup(taskId,
                                                 rteCancelFlag,
                                                 firstIOSequence,
                                                 lastIOSequence))
    {
        trError = e_log_error;
    }
    else
    {
        trError = e_ok;
    }
}

/*-----------------------------------------------------------------------------------*/

externPascal void kb391AppendTapeErrorToMessageList (
    tsp00_TaskId                         taskid,
    pasbool                              isInfo,
    pasbool                              writeImmediately,
    tsp00_Int4                           mediano,
    tsp00_VFilename       VAR_ARRAY_REF  medianame,
    tsp00_ErrText         VAR_ARRAY_REF  errortext)
{
    // PTS 1134891 UH 2005-09-20 new
    Trans_Context *pTrans      = Kernel_Task::Instance().GetTransContext(*RTETask_ITask::ByTaskID(taskid));

    pTrans->ErrorList().AppendNewMessage(
        Msg_List( isInfo ? Msg_List::Info : Msg_List::Error,
                  SDBMSG_BACKUP_MEDIUM_ERROR,
                  Msg_Arg(SDBMSGTAG_BACKUP_MEDIUM_ERROR__BACKUP_MEDIA_NO,    SAPDB_ToString(mediano) ),
                  Msg_Arg(SDBMSGTAG_BACKUP_MEDIUM_ERROR__BACKUP_MEDIUM_NAME, SAPDB_ToString(medianame.asCharp(),medianame.length(),medianame.length())),
                  Msg_Arg(SDBMSGTAG_BACKUP_MEDIUM_ERROR__ERRORTEXT,          SAPDB_ToString(errortext.asCharp(),errortext.length(),errortext.length())) ) );
    
    if ( pTrans->OldTrans().trError_gg00 != e_ok ) 
        Kernel_AppendBasisError(pTrans->OldTrans().trError_gg00, pTrans->ErrorList(), "Backup error occured");
    
    if ( writeImmediately != 0 )
    {
        RTE_Message(pTrans->ErrorList());
        pTrans->ErrorList().ClearMessageList();
    }
}

/*-----------------------------------------------------------------------------------*/

externPascal void kb391InputParameters (
    tgg00_MessBlock       VAR_VALUE_REF  mblock,
    tkb3_save_static_ptr                 glob)
{
    tgg00_SaveRestoreInputParam &input = mblock.mb_qual()->msave_restore();
    Trans_Context               &trans = Trans_Context::GetContext(*mblock.mb_trns());
    tgg00_BasisError            &error = mblock.mb_trns()->trError_gg00;

    if ( kb03Check.chkRegion_kb00 )
        g08excl_check (trans.Task().ID(), g08save0 + glob->sta_region);

    if ( g01no_of_backup_devs() <= 0
         ||
         g01no_of_backup_devs() > MAX_TAPES_GG00 )
    {
        error = e_invalid_configuration;
        trans.ErrorList().AppendNewMessage(
            Msg_List( Msg_List::Error, SDBMSG_BACKUP_CONFIG_MISMATCH,
                      Msg_Arg(SDBMSGTAG_BACKUP_CONFIG_MISMATCH__TITLE,       "Configured maximum backup media"),
                      Msg_Arg(SDBMSGTAG_BACKUP_CONFIG_MISMATCH__VALUE,       SAPDB_ToString(g01no_of_backup_devs())),
                      Msg_Arg(SDBMSGTAG_BACKUP_CONFIG_MISMATCH__LIMIT_LOWER, SAPDB_ToString(1)),
                      Msg_Arg(SDBMSGTAG_BACKUP_CONFIG_MISMATCH__LIMIT_UPPER, SAPDB_ToString(MAX_TAPES_GG00)) ) );
    }

    if ( input.sripHostTapeNum_gg00 < 1
         ||
         input.sripHostTapeNum_gg00 > g01no_of_backup_devs() )
    {
        error = e_invalid_configuration;
        trans.ErrorList().AppendNewMessage(
            Msg_List( Msg_List::Error, SDBMSG_BACKUP_CONFIG_MISMATCH,
                      Msg_Arg(SDBMSGTAG_BACKUP_CONFIG_MISMATCH__TITLE,       "Backup media count"),
                      Msg_Arg(SDBMSGTAG_BACKUP_CONFIG_MISMATCH__VALUE,       SAPDB_ToString(input.sripHostTapeNum_gg00)),
                      Msg_Arg(SDBMSGTAG_BACKUP_CONFIG_MISMATCH__LIMIT_LOWER, SAPDB_ToString(1)),
                      Msg_Arg(SDBMSGTAG_BACKUP_CONFIG_MISMATCH__LIMIT_UPPER, SAPDB_ToString(g01no_of_backup_devs())) ) );
    }
    
    if ( input.sripBlocksize_gg00 == 0 )
        input.sripBlocksize_gg00 = ckb3_default_block_count;
        
    if ( input.sripBlocksize_gg00 < ckb3_min_pages_per_block
         ||
         input.sripBlocksize_gg00 > g01backup_block_count() )
    {
        trans.ErrorList().AppendNewMessage(
            Msg_List( Msg_List::Warning, SDBMSG_BACKUP_CONFIG_MISMATCH,
                      Msg_Arg(SDBMSGTAG_BACKUP_CONFIG_MISMATCH__TITLE,       "BLOCKSIZE"),
                      Msg_Arg(SDBMSGTAG_BACKUP_CONFIG_MISMATCH__VALUE,       SAPDB_ToString(input.sripBlocksize_gg00)),
                      Msg_Arg(SDBMSGTAG_BACKUP_CONFIG_MISMATCH__LIMIT_LOWER, SAPDB_ToString(ckb3_min_pages_per_block)),
                      Msg_Arg(SDBMSGTAG_BACKUP_CONFIG_MISMATCH__LIMIT_UPPER, SAPDB_ToString(g01backup_block_count())) ) );
        input.sripBlocksize_gg00 = g01backup_block_count();
        trans.ErrorList().AppendNewMessage(Msg_List(Msg_List::Info,SDBMSG_BACKUP_BLOCK_COUNT_CHANGED));
    }
}
