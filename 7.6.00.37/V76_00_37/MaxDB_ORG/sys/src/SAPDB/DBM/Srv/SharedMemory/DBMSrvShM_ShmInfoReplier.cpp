/*!
    @file           DBMSrvShM_ShmInfoReplier.hpp
    @author         MarcW
    @brief          shared memory reply information

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_ShmInfoReplier.hpp"
#include "RunTime/System/RTESys_ProcessState.hpp"

DBMSrvShM_ShmInfoReplier::DBMSrvShM_ShmInfoReplier(const char* dbName, DBMSrv_Reply& theReply)
        : m_ReplyBuffer(),
          m_BufferedReply(m_ReplyBuffer.getReply()),
          m_RealReply(theReply) {

    m_ShmInfo.initialize(dbName, m_InitialisationError);
}
 
DBMSrvShM_ShmInfoReplier::~DBMSrvShM_ShmInfoReplier() {
}

tcn00_Error DBMSrvShM_ShmInfoReplier::replyInfo() {

    if( !m_InitialisationError.IsEmpty() )
        return m_RealReply.startWithMessageList(m_InitialisationError);

    tcn00_Error returnValue(ERR_CN00);

    // start replying
    while(true) {

        if( !replyVersionInformation() )
            break;
        m_BufferedReply.appendLine();
        if( !replyLockStatus() )
            break;
        m_BufferedReply.appendLine();
        if( !replyPlayers() )
            break;
        m_BufferedReply.appendLine();
        if( !replyLockCounter() )
            break;
        m_BufferedReply.appendLine();
        if( !replyHalfSwitcher() )
            break;
        m_BufferedReply.appendLine();
        if( !replyLines() )
            break;
        returnValue = OK_CN00;
        break;
    }

    // m_InitialisationError is empty here
    if( !m_ShmInfo.close(m_InitialisationError) ) {
        m_BufferedReply.appendMessageList(m_InitialisationError);
        returnValue = ERR_CN00;
    }

    if( returnValue == OK_CN00 )
        m_RealReply.startWithOK();
    else
        m_RealReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_ERR));

    m_RealReply.appendString(m_BufferedReply.giveData());
    return returnValue;
}

bool DBMSrvShM_ShmInfoReplier::replyVersionInformation() {
    // get the version
    DBMSrvMsg_Error errOut;
    sprintf(m_ReplyLine, m_ChapterVers);
    m_BufferedReply.appendLine(m_ReplyLine);
    int versionCode(-1);
    int versionFile(-1);
    if( !m_ShmInfo.getVersionCode( versionCode, errOut ) ) {
        m_BufferedReply.appendMessageList(errOut);
        return false;
    }
    sprintf(m_ReplyLine, m_ShmVersionRequired, versionCode);
    m_BufferedReply.appendLine(m_ReplyLine);

    if( !m_ShmInfo.getVersionFile( versionFile, errOut ) ) {
        m_BufferedReply.appendMessageList(errOut);
        return false;
    }
    sprintf(m_ReplyLine, m_ShmVersionFound, versionFile);
    m_BufferedReply.appendLine(m_ReplyLine);
    return true;
}

bool DBMSrvShM_ShmInfoReplier::replyLockStatus() {

    sprintf(m_ReplyLine, m_ChapterLock);
    m_BufferedReply.appendLine(m_ReplyLine);

    bool normalLock(false);
    bool backingLock(false);
    RTE_OSPid lockerPid(0);
    DBMSrvMsg_Error errOut;

    if( !m_ShmInfo.getLockStateNormal(normalLock, errOut) ) {
        m_BufferedReply.appendMessageList(errOut);
        return false;
    }
    sprintf( m_ReplyLine, m_ShmLockNormal, normalLock?m_ShmLockedText:m_ShmUnlockedText);
    m_BufferedReply.appendLine(m_ReplyLine);

    if( !m_ShmInfo.getLockStateReserve(backingLock, errOut) ) {
        m_BufferedReply.appendMessageList(errOut);
        return false;
    }
    sprintf( m_ReplyLine, m_ShmLockBacking, backingLock?m_ShmLockedText:m_ShmUnlockedText);
    m_BufferedReply.appendLine(m_ReplyLine);

    if( !m_ShmInfo.getLockingPid(lockerPid, errOut) ) {
        m_BufferedReply.appendMessageList(errOut);
        return false;
    }
    sprintf( m_ReplyLine, m_ShmLockingPid, lockerPid);
    m_BufferedReply.appendLine(m_ReplyLine);
    return true;
}

bool DBMSrvShM_ShmInfoReplier::replyPlayers() {

    DBMSrvMsg_Error errOut;

    sprintf(m_ReplyLine,m_ChapterProc);
    m_BufferedReply.appendLine(m_ReplyLine);
    DBMSrvShM_ParticipantSet playerSet;
    if( !m_ShmInfo.getParticipantSet(playerSet, errOut) ) {
        m_BufferedReply.appendMessageList(errOut);
        return false;
    }

    Tools_Vector<RTE_OSPid> playerVector;
    Tools_Vector<RTE_OSPid>::iterator playerIterator = NULL;
    playerSet.getPIDsInto(playerVector);
    // for formatting the output (length of pid)
    int maxPidLen(0);
    for( playerIterator=playerVector.begin(); playerIterator!=playerVector.end(); ++playerIterator ) {
        RTE_OSPid pid(*playerIterator);
        int thisPidLen(0);
        while( pid > 0 ) {
            pid = pid / 10;
            ++thisPidLen;
        }
        if( thisPidLen > maxPidLen )
            maxPidLen = thisPidLen;
    }

    // create format string
    char format_line[32];
    char format_head[32];
    sprintf(format_head, m_ProcHeadFormat, maxPidLen);
    sprintf(format_line, m_ProcLineFormat, maxPidLen);

    sprintf( m_ReplyLine, format_head, m_PidText);
    m_BufferedReply.appendLine(m_ReplyLine);

    for( playerIterator=playerVector.begin(); playerIterator!=playerVector.end(); ++playerIterator ) {
        RTESys_ProcessState procState(*playerIterator);
        const char* pText;
        switch( procState.State() ) {
            case RTESys_ProcessState::Alive :
                pText = m_AliveText;
                break;
            case RTESys_ProcessState::Died :
                pText = m_DeadText;
                break;
            default:
                pText = m_UnknownText;
                break;
        }
        sprintf( m_ReplyLine, format_line, *playerIterator, pText,
            *playerIterator==RTESys_ProcessID()?m_OwnPidMarker:"");
        m_BufferedReply.appendLine(m_ReplyLine);
    }
    return true;
}

bool DBMSrvShM_ShmInfoReplier::replyLockCounter() {
    
    DBMSrvMsg_Error errOut;

    sprintf( m_ReplyLine, "%s", "LOCKCOUNTER");
    m_BufferedReply.appendLine(m_ReplyLine);
    int currentLockCount(0);
    if( !m_ShmInfo.getLockCounter(currentLockCount, errOut) ) {
        m_BufferedReply.appendMessageList(errOut);
        return false;
    }
    sprintf( m_ReplyLine, "%d", currentLockCount);
    m_BufferedReply.appendLine(m_ReplyLine);
    return true;
}

bool DBMSrvShM_ShmInfoReplier::replyHalfSwitcher() {

    DBMSrvMsg_Error errOut;

    sprintf( m_ReplyLine, "%s", "HALFSWITCHER");
    m_BufferedReply.appendLine(m_ReplyLine);
    DBMSrvShM_HalfSwitcher halfSwitcher;
    if( !m_ShmInfo.getHalfSwitcher(halfSwitcher, errOut) ) {
        m_BufferedReply.appendMessageList(errOut);
        return false;
    }
    SAPDB_ToStringClass memSize(halfSwitcher.getMemSize());
    SAPDB_ToStringClass dataSize(halfSwitcher.getDataSize());
    SAPDB_ToStringClass offset(halfSwitcher.getOffset());

    sprintf( m_ReplyLine, "overall size %s", (const char*)memSize);
    m_BufferedReply.appendLine(m_ReplyLine);
    sprintf( m_ReplyLine, "used size    %s", (const char*)dataSize);
    m_BufferedReply.appendLine(m_ReplyLine);
    sprintf( m_ReplyLine, "offset       %s", (const char*)offset);
    m_BufferedReply.appendLine(m_ReplyLine);
    return true;
}

bool DBMSrvShM_ShmInfoReplier::replyLines() {

    sprintf( m_ReplyLine, "%s", "=============");
    m_BufferedReply.appendLine(m_ReplyLine);
    sprintf( m_ReplyLine, "%s", "    LINES");
    m_BufferedReply.appendLine(m_ReplyLine);
    sprintf( m_ReplyLine, "%s", "=============");
    m_BufferedReply.appendLine(m_ReplyLine);
    while( true ) {
        if( !replyEventingLines() )
            return false;
        m_BufferedReply.appendLine();
        if( !replyResourceLockLines() )
            return false;
        m_BufferedReply.appendLine();
        if( !replySchedulerLines() )
            return false;

        return true;
    }
}

bool DBMSrvShM_ShmInfoReplier::replyEventingLines() {

    DBMSrvShM_BaseEnumerator* enumerator(m_ShmInfo.getEnumerator());
    SAPDB_Byte* pointer(NULL);

    sprintf( m_ReplyLine, "%s", "*** Eventing");
    m_BufferedReply.appendLine(m_ReplyLine);
    
    DBMSrvMsg_Error errOut;
    
    // get existing number of lines
    int lines(enumerator->getExistingLinesOf(DBMSrvShM_ShmAdmin::LineTypeEventing, errOut));
    if( !errOut.IsEmpty() ) {
        // report an error into the reply
        return false;
    }
    else {
        sprintf( m_ReplyLine, "Existing lines %d", lines);
        m_BufferedReply.appendLine(m_ReplyLine);
    }

    // display the lines
    pointer = enumerator->getStartOf(DBMSrvShM_ShmAdmin::LineTypeEventing, errOut);
    DBMSrvShM_LineLayoutEventing* eventing = (DBMSrvShM_LineLayoutEventing*)pointer;
    while( lines > 0 ) {
        // iterate all lines and display them
        if( m_ShmInfo.isInScope(eventing, DBMSrvShM_ShmAdmin::LineTypeEventing, errOut) ) {
            // index
            sprintf( m_ReplyLine, "index %d", eventing->getIndex());
            m_BufferedReply.appendLine(m_ReplyLine);
            // instance ID
            sprintf( m_ReplyLine, "\tinstance id           %d", eventing->getInstanceID());
            m_BufferedReply.appendLine(m_ReplyLine);
            // event dispatcher's DBM server
            sprintf( m_ReplyLine, "\tDispatcher pid        %d", (int)eventing->getBGSRVPid());
            m_BufferedReply.appendLine(m_ReplyLine);
            // visibility
            sprintf( m_ReplyLine, "\tvisibility            %s", (int)eventing->isInternal()?"internal":"open");
            m_BufferedReply.appendLine(m_ReplyLine);
            // communication status
            sprintf( m_ReplyLine, "\tcommunication status  %s", (int)eventing->getInternalCommStatus()?"local":"local/remote");
            m_BufferedReply.appendLine(m_ReplyLine);
            // user
            size_t userLen(eventing->getDBMUserLen());
            SAPDB_Byte* user(eventing->getDBMUser());
            sprintf( m_ReplyLine, "\tuser name             %.*s", (int)userLen, (const char*)user);
            m_BufferedReply.appendLine(m_ReplyLine);
            // node name
            size_t nodeLen(eventing->getNodeNameLen());
            SAPDB_Byte* node(eventing->getNodeName());
            sprintf( m_ReplyLine, "\tnode name             %.*s", (int)nodeLen, (const char*)node);
            m_BufferedReply.appendLine(m_ReplyLine);
            // config file
            size_t configLen(eventing->getConfigFilePathLen());
            SAPDB_Byte* config(eventing->getConfigFilePath());
            sprintf( m_ReplyLine, "\tconfig file           %.*s", (int)configLen, (const char*)config);
            m_BufferedReply.appendLine(m_ReplyLine);
            // log file
            size_t logLen(eventing->getLogFilePathLen());
            SAPDB_Byte* log(eventing->getLogFilePath());
            sprintf( m_ReplyLine, "\tlog file              %.*s", (int)logLen, (const char*)log);
            m_BufferedReply.appendLine(m_ReplyLine);
        }
        else {
            // report an error into the reply
            return false;
        }
        --lines;
        eventing = eventing->getNext();
    }
    return true;
}

bool DBMSrvShM_ShmInfoReplier::replyResourceLockLines() {

    DBMSrvShM_BaseEnumerator* enumerator(m_ShmInfo.getEnumerator());
    SAPDB_Byte* pointer(NULL);

    sprintf( m_ReplyLine, "%s", "*** Resource Locks");
    m_BufferedReply.appendLine(m_ReplyLine);
    
    DBMSrvMsg_Error errOut;
    
    // get existing number of lines
    int lines(enumerator->getExistingLinesOf(DBMSrvShM_ShmAdmin::LineTypeResourceLock, errOut));
    if( !errOut.IsEmpty() ) {
        // report an error into the reply
        return false;
    }
    else {
        sprintf( m_ReplyLine, "Existing lines %d", lines);
        m_BufferedReply.appendLine(m_ReplyLine);
    }

    // display the lines
    pointer = enumerator->getStartOf(DBMSrvShM_ShmAdmin::LineTypeResourceLock, errOut);
    DBMSrvShM_LineLayoutResourceLock* resourceLock = (DBMSrvShM_LineLayoutResourceLock*)pointer;
    while( lines > 0 ) {
        // iterate all lines and display them
        if( m_ShmInfo.isInScope(resourceLock, errOut) ) {
            sprintf( m_ReplyLine, "index %d", resourceLock->getIndex());
            m_BufferedReply.appendLine(m_ReplyLine);
            sprintf( m_ReplyLine, "\tlocking pid   %d", (int)resourceLock->getBGSRVPid());
            m_BufferedReply.appendLine(m_ReplyLine);
            size_t nameLen(resourceLock->getResourceNameLen());
            SAPDB_Byte* name(resourceLock->getResourceName());
            sprintf( m_ReplyLine, "\tresource name %.*s", (int)nameLen, (const char*)name);
            m_BufferedReply.appendLine(m_ReplyLine);
        }
        else {
            // report an error into the reply
            return false;
        }
        --lines;
        resourceLock = resourceLock->getNext();
    }
    return true;
}

bool DBMSrvShM_ShmInfoReplier::replySchedulerLines() {

    DBMSrvShM_BaseEnumerator* enumerator(m_ShmInfo.getEnumerator());
    SAPDB_Byte* pointer(NULL);

    sprintf( m_ReplyLine, "%s", "*** Scheduler communication");
    m_BufferedReply.appendLine(m_ReplyLine);
    
    DBMSrvMsg_Error errOut;
    
    // get existing number of lines
    int lines(enumerator->getExistingLinesOf(DBMSrvShM_ShmAdmin::LineTypeScheduler, errOut));
    if( !errOut.IsEmpty() ) {
        // report an error into the reply
        return false;
    }
    else {
        sprintf( m_ReplyLine, "Existing lines %d", lines);
        m_BufferedReply.appendLine(m_ReplyLine);
    }

    // display the lines
    pointer = enumerator->getStartOf(DBMSrvShM_ShmAdmin::LineTypeScheduler, errOut);
    DBMSrvShM_LineLayoutScheduler* scheduler = (DBMSrvShM_LineLayoutScheduler*)pointer;
    while( lines > 0 ) {
        // iterate all lines and display them
        if( m_ShmInfo.isInScope(scheduler, errOut) ) {
            // index
            sprintf( m_ReplyLine, "index %d", scheduler->getIndex());
            m_BufferedReply.appendLine(m_ReplyLine);
            // scheduler pid
            sprintf( m_ReplyLine, "\tscheduler pid    %d", (int)scheduler->getBGSRVPid());
            m_BufferedReply.appendLine(m_ReplyLine);
            // cancel flag
            sprintf( m_ReplyLine, "\tcancelled?       %s", scheduler->isCancelled()?"yes":"no");
            m_BufferedReply.appendLine(m_ReplyLine);
            // user name
            size_t nameLen(scheduler->getUserNameLen());
            SAPDB_Byte* name(scheduler->getUserName());
            sprintf( m_ReplyLine, "\tuser name        %.*s", (int)nameLen, (const char*)name);
            m_BufferedReply.appendLine(m_ReplyLine);
            // recipient
            sprintf( m_ReplyLine, "\trecipient        %s", DBMSrvShM_LineAccessorScheduler::getRecipientName((DBMSrvShM_LineAccessorScheduler::Recipient)scheduler->getRecipient()));
            m_BufferedReply.appendLine(m_ReplyLine);
            // info type
            sprintf( m_ReplyLine, "\tinformation type %d", (int)scheduler->getInfoType() );
            m_BufferedReply.appendLine(m_ReplyLine);
            // variable part
            size_t vpLen(scheduler->getVariablePartLen());
            SAPDB_Byte* vp(scheduler->getVariablePart());
            sprintf( m_ReplyLine, "\tvariable part    %.*s", (int)vpLen, (const char*)vp);
            m_BufferedReply.appendLine(m_ReplyLine);
        }
        else {
            // report an error into the reply
            return false;
        }
        --lines;
        scheduler = scheduler->getNext();
    }
    return true;
}

const char* const DBMSrvShM_ShmInfoReplier::m_AliveText = "ALIVE";
const char* const DBMSrvShM_ShmInfoReplier::m_DeadText = "DEAD";
const char* const DBMSrvShM_ShmInfoReplier::m_UnknownText = "UNKNOWN";

const char* const DBMSrvShM_ShmInfoReplier::m_ProcHeadFormat = "%%-%ds | status";
const char* const DBMSrvShM_ShmInfoReplier::m_ProcLineFormat = "%%-%dd | %%s%%s";
const char* const DBMSrvShM_ShmInfoReplier::m_PidText = "pid";
const char* const DBMSrvShM_ShmInfoReplier::m_OwnPidMarker = " (*)";
const char* const DBMSrvShM_ShmInfoReplier::m_ChapterProc = "PROCESSES";
const char* const DBMSrvShM_ShmInfoReplier::m_ChapterLock = "LOCKING";
const char* const DBMSrvShM_ShmInfoReplier::m_ChapterVers = "VERSION";
const char* const DBMSrvShM_ShmInfoReplier::m_ChapterPers = "PERSISTENCE";
const char* const DBMSrvShM_ShmInfoReplier::m_ShmLockedText = "LOCKED";
const char* const DBMSrvShM_ShmInfoReplier::m_ShmUnlockedText = "UNLOCKED";
const char* const DBMSrvShM_ShmInfoReplier::m_ShmLockNormal = "Normal lock  %s";
const char* const DBMSrvShM_ShmInfoReplier::m_ShmLockBacking = "Reserve lock %s";
const char* const DBMSrvShM_ShmInfoReplier::m_ShmLockingPid = "Locking pid  %d";
const char* const DBMSrvShM_ShmInfoReplier::m_ShmVersionRequired = "Required %d";
const char* const DBMSrvShM_ShmInfoReplier::m_ShmVersionFound = "Found    %d";
