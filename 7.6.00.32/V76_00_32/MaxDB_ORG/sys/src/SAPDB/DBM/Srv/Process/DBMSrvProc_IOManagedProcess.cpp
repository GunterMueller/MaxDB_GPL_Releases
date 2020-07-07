/*!
  \file    DBMSrvProc_IOManagedProcess.cpp
  \author  MarcW
  \brief   managed process implementation

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

#include <stdio.h>
#include <time.h>

#include "gcn00.h"

#include "DBM/Logging/DBMLog_Logger.hpp"
#include "DBM/Srv/Process/DBMSrvProc_IOManagedProcess.hpp"
#include "RunTime/RTE_ISystem.hpp"
#include "ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp"

int DBMSrvProc_IOManagedProcess::m_InstanceCount(0);
DBMSrvMsg_ErrorHandler DBMSrvProc_IOManagedProcess::m_ErrorHandler;

DBMSrvProc_IOManagedProcess::DBMSrvProc_IOManagedProcess(
    const char* aDBName,
    const char* aUserID,
    const char* aPassword,
    const char* aCommandLine)
        : m_Process(NULL),
          m_DBName(new char[strlen(aDBName)+1]),
          m_ServerPid(0),
          m_Me2Process(NULL),
          m_Process2Me(NULL),
          m_AnswerBuffer(new char[m_AnswerSize]),
          m_CurrentWriteOffset(0),
          m_CommandAnswerComplete(false) {

    if( m_InstanceCount == 0 ) {
        // I am the first one
        m_ErrorHandler.activate();
    }
    ++m_InstanceCount;

    int wrc(0);
    long bytesWritten;

    m_Me2Process = new tni34_APipe(tni34_APipe::Out,1,0);
    if( m_Me2Process == NULL )
        logError(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANNOWP, Msg_Arg(SDBMSGTAG_DBMSRV_IOMANNOWP__DBMSRV_COMP, wrc)));
    else if( !m_Me2Process->IsGood() ) {
        logError(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANBADWP, Msg_Arg(SDBMSGTAG_DBMSRV_IOMANBADWP__DBMSRV_COMP, wrc)));
    }

    m_Process2Me = new tni34_APipe(tni34_APipe::In,1,0);
    if( m_Process2Me == NULL )
        logError(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANNORP, Msg_Arg(SDBMSGTAG_DBMSRV_IOMANNORP__DBMSRV_COMP, wrc)));
    else if( !m_Process2Me->IsGood() )
        logError(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANBADRP, Msg_Arg(SDBMSGTAG_DBMSRV_IOMANBADRP__DBMSRV_COMP, wrc)));

    strcpy(m_DBName, aDBName);

    // start dbmcli
    char startBuf[256];
    time_t now;
    time(&now);
    struct tm* strNow = localtime(&now);
    sprintf(startBuf, m_ProtocolSeparatorStart,
                strNow->tm_mday,
                strNow->tm_mon+1,
                strNow->tm_year+1900,
                strNow->tm_hour,
                strNow->tm_min,
                strNow->tm_sec);
    digestString(startBuf);

    sprintf(m_CmdBuffer, m_StartTemplate, m_DBName);
    // order of these 3 statements matters!
    m_Process = new tni33_ABackgroundProcess(m_CmdBuffer, m_Me2Process, m_Process2Me);
    wrc = m_Me2Process->Open();
    if( !wrc )
        logError(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANPROCWP, Msg_Arg(SDBMSGTAG_DBMSRV_IOMANPROCWP__DBMSRV_COMP, wrc)));

    wrc = m_Process2Me->Open();
    if( !wrc )
        logError(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANPROCRP, Msg_Arg(SDBMSGTAG_DBMSRV_IOMANPROCRP__DBMSRV_COMP, wrc)));

    digestPrompt();

    // logon
    digestString(m_ProtocolSeparatorLogon);
    sprintf(m_CmdBuffer, m_LogonTemplate, aUserID, aPassword);
    wrc=m_Me2Process->Write(m_CmdBuffer,
                                long(strlen(m_CmdBuffer)),
                                bytesWritten);
    if( !wrc )
        logError(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANPROCLOGON, Msg_Arg(SDBMSGTAG_DBMSRV_IOMANPROCLOGON__DBMSRV_COMP, wrc)));

    digestPrompt();

    // get pid of DBMServer that will serve this job
    size_t oldWriteOffset(m_CurrentWriteOffset);
    wrc=m_Me2Process->Write(m_CommandDbmGetpid,
                                long(strlen(m_CommandDbmGetpid)),
                                bytesWritten);
    if( !wrc )
        logError(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANPROCPID, Msg_Arg(SDBMSGTAG_DBMSRV_IOMANPROCPID__DBMSRV_COMP, wrc)));

    digestPrompt();

    *(m_AnswerBuffer + m_CurrentWriteOffset) = '\0';
    const char* pidStr =  strstr(m_AnswerBuffer + oldWriteOffset, ANSWER_OK_CN00);
    if( pidStr != NULL ) {
        pidStr = strstr(pidStr, LINE_SEPSTRING_CN00) + strlen(LINE_SEPSTRING_CN00);
        if( NULL != pidStr && pidStr < m_AnswerBuffer + m_CurrentWriteOffset )
            m_ServerPid = (RTE_OSPid) atoi(pidStr);
    }

    // execute command
    digestString(m_ProtocolSeparatorExecute);
    sprintf(m_CmdBuffer, m_CommandTemplate, aCommandLine);
    wrc=m_Me2Process->Write(m_CmdBuffer,
                                long(strlen(m_CmdBuffer)),
                                bytesWritten);
    if( !wrc )
        logError(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANPROCCMD,
                                 Msg_Arg(SDBMSGTAG_DBMSRV_IOMANPROCCMD__DBMSRV_COMP1, wrc),
                                 Msg_Arg(SDBMSGTAG_DBMSRV_IOMANPROCCMD__DBMSRV_COMP2, aCommandLine)));

    if( !m_Process->IsRunning() ) {
        char buf[32];
        sprintf(buf, "%d", m_ServerPid);
        DBMLog_Logger::getInstance().addInfoString("%s server dead after successful writing command", buf);
    }
}

char* DBMSrvProc_IOManagedProcess::getLog(size_t& aLength) const {
    char* retBuf = new char[m_CurrentWriteOffset+1];
    if( retBuf == NULL )
        aLength = 0;
    else {
        retBuf[m_CurrentWriteOffset]='\0';
        ToolsMeMa_GuardedOperation::MemCopy(
            retBuf, m_AnswerBuffer, m_CurrentWriteOffset, __FILE__, __LINE__);
        aLength = m_CurrentWriteOffset+1;
    }
    return retBuf;
}

char* DBMSrvProc_IOManagedProcess::getReply(size_t& aLength) const {
    size_t logSize(0);
    aLength =0;

    // the log is 0-terminated
    char* log = getLog(logSize);
    if( log == NULL )
        return NULL;

    char* reply = strstr(log, m_ProtocolSeparatorExecute);
    if( reply == NULL ) {
        delete log;
        return NULL;
    }

    // make reply point to the DBMServer's reply
    reply += strlen(m_ProtocolSeparatorExecute);

    // find the end
    char* end(strstr(reply, m_StandardAnswerEndMarker));
    if( end != NULL ) {
        end += strlen(m_StandardAnswerEndMarker);
        *end = '\0';
        aLength = 1 + end - reply;
    }
    else
        aLength = logSize - (reply-log);

    char* newReply = new char[aLength];
    if( newReply != NULL ) 
        ToolsMeMa_GuardedOperation::MemCopy(
            newReply, reply, aLength, __FILE__, __LINE__);
    else
        aLength = 0;

    delete log;
    return newReply;
}

bool DBMSrvProc_IOManagedProcess::getReturnCode( int& returnCode ) const {
    if( !m_CommandAnswerComplete )
        return false;
    else {
        returnCode = 1;
        size_t replyLen(0);
        char* reply = getReply(replyLen);
        int dbmsrvCode(1);
        if( reply != NULL ) {
            char* current(reply);
            // the answer start with "ERR", go to next line
            if (replyLen >= strlen("ERR") && strncmp(current, "ERR", strlen("ERR")) == 0) {
                while( *current!='\n' && *current!='\0' )
                    ++current;
                if( *current == '\n' )
                    ++current;
                dbmsrvCode = atoi(current);
                if( dbmsrvCode != 0 )
                    // we found a number
                    returnCode = dbmsrvCode;
            }
            else if( replyLen >= strlen("OK") && strncmp(current, "OK", strlen("OK")) == 0 ) {
                // the answer starts with "OK"
                returnCode = 0;
            }
            else {
                // no "ERR" and no "OK" found, we consider this as error
                // returnCode will be 1
            }
            delete[] reply;
        }
        return true;
    }
}   

bool DBMSrvProc_IOManagedProcess::isRunning() const {
    return !m_CommandAnswerComplete && m_Process->IsRunning()>0;
}

DBMSrvProc_IOManagedProcess::~DBMSrvProc_IOManagedProcess() {
    int wrc;
    long bytesWritten;

    // exit
    digestString(m_ProtocolSeparatorExit);
    sprintf(m_CmdBuffer, m_ExitCommand);
    wrc=m_Me2Process->Write(m_CmdBuffer,
                                long(strlen(m_CmdBuffer)),
                                bytesWritten);

    if( !wrc ) {
        logError(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANPROCEXIT, Msg_Arg(SDBMSGTAG_DBMSRV_IOMANPROCEXIT__DBMSRV_COMP, wrc)));
    }
    
    digestAnswer(m_DigestingTimeComplete, m_StandardAnswerEndMarker);
/*
    FILE* f = fopen( "c:\\mp_destr.txt","a+");
    //FILE* f = fopen( "/sapdb/data/wrk/MP76_A/zz_text","a+");
    fprintf(f, "\"%s\" is being destroyed\n", m_AnswerBuffer);
    fclose(f);
*/
    if( m_AnswerBuffer != NULL )
        delete m_AnswerBuffer;
    
    if( m_Process != NULL ) {
        delete m_Process;
        m_Process = NULL;
    }

    if( m_DBName != NULL )
        delete m_DBName;

    wrc = m_Me2Process->Remove();
    if( !wrc )
        logError(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANPROCDELWP, Msg_Arg(SDBMSGTAG_DBMSRV_IOMANPROCDELWP__DBMSRV_COMP, wrc)));

    wrc = m_Process2Me->Remove();
    if( !wrc )
        logError(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANPROCDELRP, Msg_Arg(SDBMSGTAG_DBMSRV_IOMANPROCDELRP__DBMSRV_COMP, wrc)));

    delete m_Me2Process;
    delete m_Process2Me;

    --m_InstanceCount;
    if( m_InstanceCount == 0 ) {
        // I was the last one...
        m_ErrorHandler.deactivate();
    }
}


void DBMSrvProc_IOManagedProcess::digestString(const char* aString) {
    SAPDB_strcpy( m_AnswerBuffer+m_CurrentWriteOffset, aString );
    m_CurrentWriteOffset += strlen(aString);
}

bool DBMSrvProc_IOManagedProcess::digestAnswer(
        const time_t allowedTime, 
        const char* endMarker) {
    int rrc;
    long bytesRead(0);
    time_t startTime(time(0));
    bool localEndReached(false);
    size_t lenEndMarker(strlen(endMarker));

    while(  !localEndReached &&
            (time(0)-startTime < allowedTime) &&
            (m_Process2Me->IsDataAvailable() || m_Process->IsRunning()) ) {
        // end not reached yet and we still have timeand there is data or the chance that there
        // will be data (since the process is still running)
        while(  !localEndReached &&
                m_Process2Me->IsDataAvailable() ) {
            // there is data, so we read until there is no more data or the expected end is reached
            rrc = m_Process2Me->Read(m_AnswerBuffer+m_CurrentWriteOffset, 1, bytesRead);
            if( !rrc ) {
                char returnCode[32];
                sprintf(returnCode, "%d", rrc);
                DBMLog_Logger::getInstance().addInfoString("pipe problem (%s) at reading data", returnCode);
            }

            if( m_CurrentWriteOffset < m_AnswerSize-1 )
                ++m_CurrentWriteOffset;

            if( m_CurrentWriteOffset >= lenEndMarker )
                localEndReached = (0 == strncmp(
                    m_AnswerBuffer+m_CurrentWriteOffset-lenEndMarker,
                    endMarker,
                    lenEndMarker));
        }
        if( !localEndReached )
            RTE_ISystem::DoSleep(m_SleepTimeWhileDigesting);
    }
    return localEndReached;
}

bool DBMSrvProc_IOManagedProcess::digestPrompt() {
    char promptMarkerBuffer[256];
    sprintf(promptMarkerBuffer, m_StartAnswerEndMarkerTemplate, m_DBName);

    bool answerComplete = digestAnswer( m_DigestingTimeComplete, promptMarkerBuffer);
    if( !answerComplete && m_Process2Me->IsDataAvailable() ) {
        // if we get here the time is up and we still don't have read away all of the prompt...
        DBMLog_Logger::getInstance().addMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANNOEND));
    }
    return answerComplete;
}

void DBMSrvProc_IOManagedProcess::digestNextCommandAnswerPart() {
    if( !m_CommandAnswerComplete )
        m_CommandAnswerComplete = digestAnswer(m_DigestingTimePart, m_StandardAnswerEndMarker);
}

void DBMSrvProc_IOManagedProcess::logError(
    const DBMSrvMsg_Error & error)
{
    Msg_List errList;
    m_ErrorHandler.getLastError(errList);
    errList.Overrule(error);

    if( m_Process != NULL && !m_Process->IsRunning() ) {
        char retcodeBuf[128];
        char pidBuf[32];
        char serverPidBuf[32];
        sprintf(retcodeBuf, "%lu", m_Process->GiveReturnCode());
        sprintf(pidBuf, "%d", (int)m_Process->getPid());
        sprintf(serverPidBuf, "%d", (int)m_ServerPid);
        errList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_IOMANPROCPROCDIED,  Msg_Arg("DBMSRV_COMP1", pidBuf),  Msg_Arg("DBMSRV_COMP2", serverPidBuf),  Msg_Arg("DBMSRV_COMP3", retcodeBuf)));
    }

    DBMLog_Logger::getInstance().addMessageList(errList);
}

const char* const DBMSrvProc_IOManagedProcess::m_StartTemplate = "dbmcli -d %s";
const char* const DBMSrvProc_IOManagedProcess::m_LogonTemplate = "user_logon %s,%s\n";
const char* const DBMSrvProc_IOManagedProcess::m_CommandDbmGetpid = "dbm_getpid\n";
const char* const DBMSrvProc_IOManagedProcess::m_CommandTemplate = "%s\n";
const char* const DBMSrvProc_IOManagedProcess::m_StartAnswerEndMarkerTemplate = "%s>";
const char* const DBMSrvProc_IOManagedProcess::m_ExitCommand = "exit\n";
const char* const DBMSrvProc_IOManagedProcess::m_StandardAnswerEndMarker = "---";
const char* const DBMSrvProc_IOManagedProcess::m_ProtocolSeparatorStart = "\n### start ### (%02d/%02d/%d, %02d:%02d:%02d)\n";
const char* const DBMSrvProc_IOManagedProcess::m_ProtocolSeparatorLogon = "\n### logon ###\n";
const char* const DBMSrvProc_IOManagedProcess::m_ProtocolSeparatorExecute = "\n### execute ###\n";
const char* const DBMSrvProc_IOManagedProcess::m_ProtocolSeparatorExit = "\n### exit ###\n";
const int DBMSrvProc_IOManagedProcess::m_SleepTimeWhileDigesting = 10;
const int DBMSrvProc_IOManagedProcess::m_SleepTimeWhileLogon = 100;
const size_t DBMSrvProc_IOManagedProcess::m_AnswerSize = 20*1024;
const time_t DBMSrvProc_IOManagedProcess::m_DigestingTimePart = 3;
const time_t DBMSrvProc_IOManagedProcess::m_DigestingTimeComplete = 10;
