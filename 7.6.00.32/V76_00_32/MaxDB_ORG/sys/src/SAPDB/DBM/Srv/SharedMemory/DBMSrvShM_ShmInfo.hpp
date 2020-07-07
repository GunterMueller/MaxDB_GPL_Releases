/*!
    @file           DBMSrvShM_ShmInfo.hpp
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

#if !defined(DBMSrvShM_ShmInfo_HPP)
#define DBMSrvShM_ShmInfo_HPP

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmAdmin.hpp"
#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_BaseEnumerator.hpp"

/*!
    @brief DBMServer shm communication administration class
*/
class DBMSrvShM_ShmInfo : public DBMSrvShM_ShmAdmin {

public:
    DBMSrvShM_ShmInfo();
    virtual ~DBMSrvShM_ShmInfo();
    
    virtual DBMSrvShM_Base* getBase( const char* infoFile, const char* dataFile, DBMSrvMsg_Error& errOut);

    bool initialize(const char* dbname, DBMSrvMsg_Error& errOut);
    bool close(DBMSrvMsg_Error& errOut);

    bool getPlatformIdFile( char*& platformID, DBMSrvMsg_Error& errOut );
    bool getParticipantSet( DBMSrvShM_ParticipantSet& participants, DBMSrvMsg_Error& errOut );

    bool getVersionFile( int& versionFile, DBMSrvMsg_Error& errOut );
    bool getVersionCode( int& VersionCode, DBMSrvMsg_Error& errOut );

    bool getLockStateNormal( bool& locked, DBMSrvMsg_Error& errOut);
    bool getLockStateReserve( bool& locked, DBMSrvMsg_Error& errOut);
    bool getLockingPid( RTE_OSPid& lockingPid, DBMSrvMsg_Error& errOut);
    bool getLockCounter( int& currentCount, DBMSrvMsg_Error& errOut );

    bool getHalfSwitcher( DBMSrvShM_HalfSwitcher& halfSwitcher, DBMSrvMsg_Error& errOut);

protected:
    virtual void closeSharedMemoryInfoFile();

private:
    DBMSrvShM_BaseEnumerator* m_BaseEnumerator;

    DBMSrvShM_BaseEnumerator* getEnumerator() {
        return m_BaseEnumerator;
    }

    friend class DBMSrvShM_ShmInfoReplier; // allow direct access
};

#endif //DBMSrvShM_ShmInfo_HPP
