/*!
    @file           DBMSrvShM_LineLayoutResourceLock.cpp
    @author         MarcW
    @brief          

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
#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutResourceLock.hpp"
#include "SAPDB/RunTime/System/RTESys_ProcessState.hpp"
#include "ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp"

void DBMSrvShM_LineLayoutResourceLock::setResourceName( const char* resourceName ) {

    size_t lenName(strlen(resourceName));
    addSizeDelta(getResourceNameLen(), lenName);

    m_LenResourceName.setValue(lenName);

    SAPDB_Byte* start = (SAPDB_Byte*)this + sizeof(*this);
    ToolsMeMa_GuardedOperation::MemCopy(start, resourceName, lenName, __FILE__, __LINE__);
}

SAPDB_Byte* DBMSrvShM_LineLayoutResourceLock::getResourceName() const {
    return ((SAPDB_Byte*)this) + sizeof(*this);
}

size_t DBMSrvShM_LineLayoutResourceLock::getResourceNameLen() const {
    size_t retLen;
    m_LenResourceName.getValue(retLen);
    return retLen;
}

bool DBMSrvShM_LineLayoutResourceLock::lockResource( RTE_OSPid& requestingPid ) {
    RTE_OSPid lockingPid(getBGSRVPid());
    if( lockingPid == 0 ) {
        setBGSRVPid(requestingPid);
        return true;
    }
    else if( lockingPid != requestingPid ) {
        RTESys_ProcessState procState(lockingPid);
        if( procState.State() == RTESys_ProcessState::Alive )
            return false;
        else {
            setBGSRVPid(requestingPid);
            return true;
        }
    }
    else {
        return true;
    }
}

bool DBMSrvShM_LineLayoutResourceLock::unlockResource( RTE_OSPid& requestingPid ) {
    RTE_OSPid lockingPid(getBGSRVPid());
    if( lockingPid == requestingPid ) {
        setBGSRVPid(0);
        return true;
    }
    else {
        RTESys_ProcessState procState(lockingPid);
        if( procState.State() == RTESys_ProcessState::Alive )
            return false;
        else {
            setBGSRVPid(0);
            return true;
        }
    }
}

DBMSrvShM_LineLayoutResourceLock* DBMSrvShM_LineLayoutResourceLock::getNext() const {
    return (DBMSrvShM_LineLayoutResourceLock*)(DBMSrvShM_LineLayoutBase::getNext());
}
