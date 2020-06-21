/*!
    @file           DBMSrvShM_LineLayoutBase.cpp
    @author         MarcW
    @brief          base class for line layout - implementation

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

#include <memory.h>

#include "DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutBase.hpp"
#include "RunTime/System/RTESys_ProcessState.hpp"

void DBMSrvShM_LineLayoutBase::addSizeDelta(size_t oldPartLen, size_t newPartLen) {
    
    if( oldPartLen == newPartLen )
        return;
    
    size_t currentSize;
    m_LineSize.getValue(currentSize);
    
    if( oldPartLen < newPartLen ) // delta is positive
        currentSize = currentSize + (newPartLen-oldPartLen);
    else                          // delta is negative
        currentSize = currentSize - (oldPartLen-newPartLen);
    
    m_LineSize.setValue(currentSize);
}


DBMSrvShM_LineLayoutBase* DBMSrvShM_LineLayoutBase::getNext() const {
    size_t overallSize;
    m_LineSize.getValue(overallSize);
    return (DBMSrvShM_LineLayoutBase*)((SAPDB_Byte*)this + overallSize); 
}

int DBMSrvShM_LineLayoutBase::getIndex() const {
    int retIndex(0);
    m_Index.getValue(retIndex);
    return retIndex;
}

void DBMSrvShM_LineLayoutBase::setIndex( int index ) {
    m_Index.setValue(index);
}

void DBMSrvShM_LineLayoutBase::removeBGSRVPid() {
    m_BGSRVPid.setValue(0);
}

RTE_OSPid DBMSrvShM_LineLayoutBase::getBGSRVPid() const {
    RTE_OSPid retPid(0);
    m_BGSRVPid.getValue(retPid);
    return retPid;
}

void DBMSrvShM_LineLayoutBase::setBGSRVPid(const RTE_OSPid& pid) {
    m_BGSRVPid.setValue(pid);
}

void DBMSrvShM_LineLayoutBase::doCancel() {
    m_CancelFlag.setValue(1);
}

void DBMSrvShM_LineLayoutBase::unCancel() {
    m_CancelFlag.setValue(0);
}

bool DBMSrvShM_LineLayoutBase::isCancelled() {
    SAPDB_Byte retFlag;
    m_CancelFlag.getValue(retFlag);
    return retFlag == 1;
}
