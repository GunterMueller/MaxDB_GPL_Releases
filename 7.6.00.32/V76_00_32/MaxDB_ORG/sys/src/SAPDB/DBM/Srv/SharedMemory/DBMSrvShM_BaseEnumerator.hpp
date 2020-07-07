/*!
    @file           DBMSrvShM_BaseEnumerator.hpp
    @author         MarcW
    @brief          Base class for shared memory - for enumeration of content

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

#if !defined(DBMSrvShM_BaseEnumerator_HPP)
#define DBMSrvShM_BaseEnumerator_HPP

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Base.hpp"

class DBMSrvShM_BaseEnumerator: public DBMSrvShM_Base {

public:
    static DBMSrvShM_BaseEnumerator* getInstance(const char* infoFile, const char* dataFile, DBMSrvMsg_Error& errOut);

    /*! @brief destructor */
    virtual ~DBMSrvShM_BaseEnumerator();

    // platformID
    bool getPlatformIdFile(char*& platformIDOut, DBMSrvMsg_Error& errOut);
    bool getPlatformIdCode(const char*& platformIDOut);

    // version
    bool getVersionFile(int& versionOut, DBMSrvMsg_Error& errOut);
    bool getVersionCode(int& versionOut);

    // locks
    bool getLockState( bool normalLock, bool& locked, DBMSrvMsg_Error& errOut);

    // locking pid
    bool getLockingPid( RTE_OSPid& lockingPid, DBMSrvMsg_Error& errOut);

    // lock counter
    bool getLockCounter( int& lockCounter, DBMSrvMsg_Error& errOut);

    // halfswitcher
    bool getHalfSwitcher( DBMSrvShM_HalfSwitcher& switcherOut, DBMSrvMsg_Error& errOut);

    // set of players
    bool getParticipantSet(DBMSrvShM_ParticipantSet& setOut, DBMSrvMsg_Error& errOut);

protected:
    virtual SAPDB_Byte* openSharedMemory(const char* const aShmId, const unsigned int aSize, DBMSrvMsg_Error& errOut);

private:
    DBMSrvShM_BaseEnumerator();
    bool deferredConstructor( const char* infoFile, const char* dataFile, DBMSrvMsg_Error &errOut);

    bool pointsIntoInfo( void* infoPointer );
};

#endif //DBMSrvShM_BaseEnumerator_HPP
