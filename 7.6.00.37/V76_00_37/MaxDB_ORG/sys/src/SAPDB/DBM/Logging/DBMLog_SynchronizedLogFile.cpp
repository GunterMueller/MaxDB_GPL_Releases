/*!
  @file    DBMLog_SynchronizedLogFile.cpp
  @author  MarcW
  @brief   clog file base class

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


*/

#include "DBM/Logging/DBMLog_SynchronizedLogFile.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp"

#include "gcn002.h"

DBMLog_SynchronizedLogFile::DBMLog_SynchronizedLogFile(const char* aFileName)
    : DBMLog_LogFile( aFileName, false ) {
}

DBMLog_SynchronizedLogFile::~DBMLog_SynchronizedLogFile() {
}

bool DBMLog_SynchronizedLogFile::writeOneEventList( const Msg_List& aEventList) {

    bool listWritten(false);
    DBMSrvMsg_Error neverSeen;

    DBMSrvShM_LineAccessorResourceLock logFileLock(m_FileName, neverSeen);
    if( neverSeen.IsEmpty() ) {
        if( DBMSrvShM_LineAccessorResourceLock::LockStateLocked == logFileLock.lock(neverSeen) ) {
            listWritten = tin105_SequentialLogFile::writeOneEventList( aEventList );
            if( DBMSrvShM_LineAccessorResourceLock::LockStateNotLocked != logFileLock.unlock(neverSeen) )
                listWritten = false;
        }
    }
    return listWritten;
}
