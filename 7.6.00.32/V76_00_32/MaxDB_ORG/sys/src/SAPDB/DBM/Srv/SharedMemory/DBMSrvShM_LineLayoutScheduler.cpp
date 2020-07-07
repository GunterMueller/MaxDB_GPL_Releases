/*!
    @file           DBMSrvShM_LineLayoutScheduler.cpp
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
#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutScheduler.hpp"
#include "ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp"

void DBMSrvShM_LineLayoutScheduler::setUserName(const char* aUserName) {
    
    size_t lenUserName(strlen(aUserName));
    addSizeDelta(getUserNameLen(), lenUserName);

    m_LenUserName.setValue(lenUserName);
    
    SAPDB_Byte* start = (SAPDB_Byte*)this + sizeof(*this);
    ToolsMeMa_GuardedOperation::MemCopy(start, aUserName, lenUserName, __FILE__, __LINE__);
}

void DBMSrvShM_LineLayoutScheduler::setVariablePart( const char* newContent ) {
    
    size_t lenVarPart(strlen(newContent));
    addSizeDelta(getVariablePartLen(), lenVarPart);

    m_LenVariablePart.setValue(lenVarPart);
    SAPDB_Byte* start =
        (SAPDB_Byte*)this +
        sizeof(*this) + 
        getUserNameLen();
    ToolsMeMa_GuardedOperation::MemCopy(start, newContent, lenVarPart, __FILE__, __LINE__);
}

size_t DBMSrvShM_LineLayoutScheduler::getUserNameLen() const {
    size_t retLen;
    m_LenUserName.getValue(retLen);
    return retLen;
}

size_t DBMSrvShM_LineLayoutScheduler::getVariablePartLen() const {
    size_t retLen;
    m_LenVariablePart.getValue(retLen);
    return retLen;
}

SAPDB_Byte* DBMSrvShM_LineLayoutScheduler::getUserName() const {
    return ((SAPDB_Byte*)this) + sizeof(*this);
}

SAPDB_Byte* DBMSrvShM_LineLayoutScheduler::getVariablePart() const {
    return ((SAPDB_Byte*)this) + sizeof(*this) + getUserNameLen();
}

DBMSrvShM_LineLayoutScheduler* DBMSrvShM_LineLayoutScheduler::getNext() const {
    return (DBMSrvShM_LineLayoutScheduler*)(DBMSrvShM_LineLayoutBase::getNext());
}
