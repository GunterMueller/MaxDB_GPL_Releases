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

#if !defined(DBMSrvShM_ShmInfoReplier_HPP)
#define DBMSrvShM_ShmInfoReplier_HPP

#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBuffer.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_ShmInfo.hpp"

class DBMSrvShM_ShmInfoReplier {
    
public:
    DBMSrvShM_ShmInfoReplier(const char* dbName, DBMSrv_Reply& theReply);
    ~DBMSrvShM_ShmInfoReplier();

    tcn00_Error replyInfo();

private:

    bool replyVersionInformation();
    bool replyLockStatus();
    bool replyPlayers();
    bool replyLockCounter();
    bool replyHalfSwitcher();

    bool replyLines();
    
    bool replyEventingLines();
    bool replyResourceLockLines();
    bool replySchedulerLines();


    DBMSrvShM_ShmInfo m_ShmInfo;
    DBMSrvMsg_Error m_InitialisationError;
    DBMSrvInfrReply_ReplyBuffer m_ReplyBuffer;
    DBMSrv_Reply& m_BufferedReply;
    DBMSrv_Reply& m_RealReply;
    char m_ReplyLine[1024];

    static const char* const m_AliveText;
    static const char* const m_DeadText;
    static const char* const m_UnknownText;
    static const char* const m_ProcHeadFormat;
    static const char* const m_ProcLineFormat;
    static const char* const m_PidText;
    static const char* const m_OwnPidMarker;
    static const char* const m_ChapterProc;
    static const char* const m_ChapterLock;
    static const char* const m_ChapterVers;
    static const char* const m_ChapterPers;
    static const char* const m_ShmLockedText;
    static const char* const m_ShmUnlockedText;
    static const char* const m_ShmLockNormal;
    static const char* const m_ShmLockBacking;
    static const char* const m_ShmLockingPid;
    static const char* const m_ShmVersionRequired;
    static const char* const m_ShmVersionFound;
};

#endif //DBMSrvShM_ShmInfoReplier_HPP
