/*!
    @file           DBMSrvShM_LineAccessorResourceLock.hpp
    @author         MarcW
    @brief          line accessor class for lines of type resource lock

\if EMIT_LICENCE

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


\endif
*/

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorBase.hpp"

#if !defined(DBMSrvShM_LineAccessorResourceLock_HPP)
#define DBMSrvShM_LineAccessorResourceLock_HPP

/*! @brief line accessor class for lines of type resource lock */
class DBMSrvShM_LineAccessorResourceLock : public DBMSrvShM_LineAccessorBase {

public:
    enum LockState {
        LockStateNotLocked,
        LockStateLocked,
        LockStateError
    };


    /*!
        @brief create an accessor for a resource lock line

        @param [IN] resource name
    */
    DBMSrvShM_LineAccessorResourceLock(const char* resourceName, DBMSrvMsg_Error& errOut);

    const char* getResourceName() const {return m_ResourceName;}

    /*! @brief release the lock */
    ~DBMSrvShM_LineAccessorResourceLock();

    /*!
        @brief obtain resource lock
        @return LockStateLocked if successful, errOut is untouched
                LockStateNotLocked if lock could not be obtained, an error is reported in errOut (this is not an error here though!!)
                LockStateError if lock could not be obtained due to an error, this error is reported in errOut
    */
    LockState lock(DBMSrvMsg_Error& errOut);
    LockState unlock(DBMSrvMsg_Error& errOut);

private:
    char* m_ResourceName;
    RTE_OSPid m_MyPid;

    static const int m_TryCount;
    static const int m_SleepMillis;
};

#endif //DBMSrvShM_LineAccessorResourceLock_HPP
