/*!
    @file           DBMSrvShM_LineAccessorResourceLock.cpp
    @author         MarcW
    @brief          definition of class for accessing resource locks

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

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp"
#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutResourceLock.hpp"
#include "SAPDBCommon/SAPDB_string.h"

const int DBMSrvShM_LineAccessorResourceLock::m_TryCount(3000);
const int DBMSrvShM_LineAccessorResourceLock::m_SleepMillis(200);


DBMSrvShM_LineAccessorResourceLock::DBMSrvShM_LineAccessorResourceLock(const char* resourceName, DBMSrvMsg_Error& errOut)
        : DBMSrvShM_LineAccessorBase(),
          m_MyPid(RTESys_ProcessID()),
          m_ResourceName(new char[strlen(resourceName)+1]) {

    SAPDB_strcpy(m_ResourceName, resourceName);

    if( !DBMSrvShM_ShmAdmin::TheAdmin().isInitialized() ) {
        // lock not created, cannot work with this object
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_RESLOCK_NO_CREATE,
	        Msg_Arg(SDBMSGTAG_DBMSRV_RESLOCK_NO_CREATE__RESOURCE, m_ResourceName)));
    }
}

DBMSrvShM_LineAccessorResourceLock::~DBMSrvShM_LineAccessorResourceLock() {

    DBMSrvMsg_Error neverSeen;

    if( DBMSrvShM_ShmAdmin::TheAdmin().lock(neverSeen) ) {
        DBMSrvShM_LineLayoutResourceLock* lrl = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutResourceLock(m_ResourceName, neverSeen);
        if( lrl != NULL && lrl->lockResource(m_MyPid) )
            DBMSrvShM_ShmAdmin::TheAdmin().removeLineLayoutResourceLock(m_ResourceName, neverSeen);

        DBMSrvShM_ShmAdmin::TheAdmin().unlock(neverSeen);
    }
    delete[] m_ResourceName;
}

DBMSrvShM_LineAccessorResourceLock::LockState DBMSrvShM_LineAccessorResourceLock::lock(DBMSrvMsg_Error& errOut) {

    bool gotLock(false);
    errOut.ClearMessageList();

    RTE_OSPid lockerPid(RTE_UNDEF_OSPID);

    int count(m_TryCount); // 3000 * 200ms = 10 minutes until we give up
    while( !gotLock && errOut.IsEmpty() && count>0 ) {
        if( DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) ) {
            DBMSrvShM_LineLayoutResourceLock* lrl =
                        DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutResourceLock(m_ResourceName, errOut);
            if( lrl != NULL ) 
                lockerPid = lrl->getBGSRVPid();
            gotLock = (lrl != NULL) && lrl->lockResource(m_MyPid);
            DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
        }
        if( gotLock )
            break;
        else {
            RTE_ISystem::DoSleep(m_SleepMillis);
            --count;
        }
    }

    if( errOut.IsEmpty() ) {
        if( !gotLock )
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMRESLOCK, 
                Msg_Arg(SDBMSGTAG_DBMSRV_SHMRESLOCK__DBMSRV_COMP, m_ResourceName),
                Msg_Arg(SDBMSGTAG_DBMSRV_SHMRESLOCK__TIME_VALUE, (m_SleepMillis*m_TryCount)/1000),
                Msg_Arg(SDBMSGTAG_DBMSRV_SHMRESLOCK__PID,(int)lockerPid)));
        return gotLock?LockStateLocked:LockStateNotLocked;
    }
    else {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_RESLOCK_NO_LOCK,
	        Msg_Arg(SDBMSGTAG_DBMSRV_RESLOCK_NO_LOCK__RESOURCE, m_ResourceName)));

        return LockStateError;
    }
}

DBMSrvShM_LineAccessorResourceLock::LockState DBMSrvShM_LineAccessorResourceLock::unlock(DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    
    bool shmLocked = DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut);
    if( !shmLocked ) {
        // could not lock the shared memory, that is an error
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_RESLOCK_NO_UNLOCK,
	        Msg_Arg(SDBMSGTAG_DBMSRV_RESLOCK_NO_UNLOCK__RESOURCE, m_ResourceName)));
        return LockStateError;
    }

    DBMSrvShM_LineLayoutResourceLock* lrl = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutResourceLock(m_ResourceName, errOut);
    
    if( lrl == NULL || !errOut.IsEmpty() ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_RESLOCK_NO_UNLOCK,
	        Msg_Arg(SDBMSGTAG_DBMSRV_RESLOCK_NO_UNLOCK__RESOURCE, m_ResourceName)));
        return LockStateError;
    }
    bool releasedLock( lrl->unlockResource(m_MyPid) );

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);

    if( errOut.IsEmpty() )
        return releasedLock?LockStateNotLocked:LockStateLocked;
    else {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_RESLOCK_NO_LOCK,
	        Msg_Arg(SDBMSGTAG_DBMSRV_RESLOCK_NO_LOCK__RESOURCE, m_ResourceName)));

        return LockStateError;
    }
}
