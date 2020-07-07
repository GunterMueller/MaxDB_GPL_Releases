/*!
    @file           DBMSrvShM_ShmInfo.cpp
    @author         MarcW
    @brief          shared memory administration class for information

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

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmInfo.hpp"

DBMSrvShM_Base* DBMSrvShM_ShmInfo::getBase( const char* infoFile, const char* dataFile, DBMSrvMsg_Error& errOut)
{
    DBMSrvShM_Base * rc=DBMSrvShM_BaseEnumerator::getInstance( m_Persistence->getInfoFile(), m_Persistence->getDataFile(), errOut );

    if(0==rc)
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM)); // memory error

    return rc;
}

DBMSrvShM_ShmInfo::DBMSrvShM_ShmInfo()
        : DBMSrvShM_ShmAdmin(),
          m_BaseEnumerator(NULL) {
}

DBMSrvShM_ShmInfo::~DBMSrvShM_ShmInfo() {
    DBMSrvMsg_Error neverSeen;
    close(neverSeen);
}

bool DBMSrvShM_ShmInfo::initialize(const char* dbname, DBMSrvMsg_Error& errOut) {
    if( DBMSrvShM_ShmAdmin::initialize(dbname, errOut) ) {
        m_BaseEnumerator = (DBMSrvShM_BaseEnumerator*) m_ShM;
        return true;
    }
    else
        return false;
}

bool DBMSrvShM_ShmInfo::close(DBMSrvMsg_Error& errOut) {
    delete m_ShM;
    m_ShM = NULL;
    m_BaseEnumerator = NULL;
    m_Initialized = false;
    return true;
}

bool DBMSrvShM_ShmInfo::getPlatformIdFile( char*& platformID, DBMSrvMsg_Error& errOut ) {
    
    bool success(false);
    if( m_Initialized ) {
        success = m_BaseEnumerator->getPlatformIdFile(platformID, errOut);
    }

    return success;
}

bool DBMSrvShM_ShmInfo::getVersionFile( int& versionFile, DBMSrvMsg_Error& errOut ) {
    
    bool success(false);
    if( m_Initialized ) {
        success = m_BaseEnumerator->getVersionFile(versionFile, errOut);
    }

    return success;
}

bool DBMSrvShM_ShmInfo::getVersionCode( int& versionCode, DBMSrvMsg_Error& errOut ) {
    
    bool success(false);
    if( m_Initialized ) {
        // this call never fails
        success = m_BaseEnumerator->getVersionCode(versionCode);
    }

    return success;
}

bool DBMSrvShM_ShmInfo::getLockStateNormal( bool& locked, DBMSrvMsg_Error& errOut ) {

    bool success(false);
    if( m_Initialized ) {
        success = m_BaseEnumerator->getLockState(true, locked, errOut);
    }

    return success;
}

bool DBMSrvShM_ShmInfo::getLockStateReserve( bool& locked, DBMSrvMsg_Error& errOut ) {

    bool success(false);
    if( m_Initialized ) {
        success = m_BaseEnumerator->getLockState(false, locked, errOut);
    }

    return success;
}

bool DBMSrvShM_ShmInfo::getLockingPid( RTE_OSPid& lockingPid, DBMSrvMsg_Error& errOut) {
    bool success(false);
    if( m_Initialized ) {
        success = m_BaseEnumerator->getLockingPid(lockingPid, errOut);
    }

    return success;
}

bool DBMSrvShM_ShmInfo::getParticipantSet( DBMSrvShM_ParticipantSet& participants, DBMSrvMsg_Error& errOut ) {
    
    bool success(false);
    if( m_Initialized ) {
        success = m_BaseEnumerator->getParticipantSet(participants, errOut);
    }

    return success;
}

bool DBMSrvShM_ShmInfo::getLockCounter( int& currentCount, DBMSrvMsg_Error& errOut ) {
    
    bool success(false);
    if( m_Initialized ) {
        success = m_BaseEnumerator->getLockCounter(currentCount, errOut);
    }

    return success;
}

bool DBMSrvShM_ShmInfo::getHalfSwitcher( DBMSrvShM_HalfSwitcher& halfSwitcher, DBMSrvMsg_Error& errOut) {

    bool success(false);
    if( m_Initialized ) {
        success = m_BaseEnumerator->getHalfSwitcher(halfSwitcher, errOut);
    }

    return success;
}

void DBMSrvShM_ShmInfo::closeSharedMemoryInfoFile()
{
    // do nothing  DBMSrvShM_ShmInfo is working on ordinary normal memory, if we delete that here it is lost.
}
