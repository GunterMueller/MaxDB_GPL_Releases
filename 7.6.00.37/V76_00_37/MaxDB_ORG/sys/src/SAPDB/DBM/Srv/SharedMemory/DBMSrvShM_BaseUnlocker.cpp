/*!
    @file           DBMSrvShM_BaseUnlocker.cpp
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

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_BaseUnlocker.hpp"

DBMSrvShM_BaseUnlocker* DBMSrvShM_BaseUnlocker::getInstance(const char* infoFile, const char* dataFile, DBMSrvMsg_Error& errOut) {

    DBMSrvShM_BaseUnlocker* newInstance(NULL);
    newInstance = new DBMSrvShM_BaseUnlocker();
    if( newInstance == NULL )
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
    else
        newInstance->deferredConstructor(infoFile, dataFile, errOut);
    return newInstance;
}

DBMSrvShM_BaseUnlocker::DBMSrvShM_BaseUnlocker()
        : DBMSrvShM_Base() {
}

bool DBMSrvShM_BaseUnlocker::deferredConstructor( const char* infoFile, const char* dataFile, DBMSrvMsg_Error &errOut ) {

    if( !minimalInit(dataFile, errOut) )
        return false;

    calculateInfoSize();

    m_pInfoShm = openSharedMemory(infoFile, m_SizeInfoShm, errOut);
    
    if( m_pInfoShm == NULL ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOUNLOCKER));
    }
    else {
        setPointersIntoInfo();
        m_IsValid = true;
    }

    return m_IsValid;
}

DBMSrvShM_BaseUnlocker::~DBMSrvShM_BaseUnlocker() {
    m_IsValid = false;
}

bool DBMSrvShM_BaseUnlocker::forceUnlock(DBMSrvMsg_Error& errOut) {

    m_pSpinlock->Unlock();
    m_pBackinglock->Unlock();

    return true;
}
