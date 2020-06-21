/*!
    @file           DBMSrvShM_BaseUnlocker.hpp
    @author         MarcW
    @brief          Base class for shared memory - for unlocking

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

#if !defined(DBMSrvShM_BaseUnlocker_HPP)
#define DBMSrvShM_BaseUnlocker_HPP

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Base.hpp"

/*!
    @brief DBMServer shared memory communication base class

    This shared memory implementation uses the memory mapped file. The memory
    mapped file is wrapped by this class. Access methods to shm are provided.
    @see heo41.h
*/
class DBMSrvShM_BaseUnlocker: public DBMSrvShM_Base {

public:
    static DBMSrvShM_BaseUnlocker* getInstance(const char* infoFile, const char* dataFile, DBMSrvMsg_Error& errOut);

    /*! @brief destructor */
    virtual ~DBMSrvShM_BaseUnlocker();

    bool forceUnlock(DBMSrvMsg_Error& errOut);

private:
    DBMSrvShM_BaseUnlocker();
    bool deferredConstructor( const char* infoFile, const char* dataFile, DBMSrvMsg_Error &errOut);
};

#endif //DBMSrvShM_BaseUnlocker_HPP
