/*!
    @file           DBMSrvShM_LineLayoutEventing.cpp
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
#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutEventing.hpp"
#include "ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp"

int DBMSrvShM_LineLayoutEventing::getInstanceID() const {
    int outVal;
    m_InstanceID.getValue(outVal);
    return outVal;
}

bool DBMSrvShM_LineLayoutEventing::isInternal() const {
    SAPDB_Byte outVal;
    m_IsInternal.getValue(outVal);
    return outVal == 0;
};

SAPDB_Byte DBMSrvShM_LineLayoutEventing::getInternalCommStatus() const {
    SAPDB_Byte outVal;
    m_InternalCommStatus.getValue(outVal);
    return outVal;
}

void DBMSrvShM_LineLayoutEventing::setDBMUser( const char* dbmUser ) {
    // set the size
    size_t len(strlen(dbmUser));
    addSizeDelta(getDBMUserLen(), len);
    
    m_LenDBMUser.setValue(len);
    
    // set the data (behind the end of *this in memory)
    SAPDB_Byte* start = getDBMUser();
    ToolsMeMa_GuardedOperation::MemCopy(start, dbmUser, len, __FILE__, __LINE__);
}
 
SAPDB_Byte* DBMSrvShM_LineLayoutEventing::getDBMUser() const {
    return (SAPDB_Byte*)this + sizeof(*this);
}
    
size_t DBMSrvShM_LineLayoutEventing::getDBMUserLen() const {
    size_t retLen;
    m_LenDBMUser.getValue(retLen);
    return retLen;
}


void DBMSrvShM_LineLayoutEventing::setNodeName( const char* nodeName ) {
    // set the size
    size_t len(strlen(nodeName));
    addSizeDelta(getNodeNameLen(), len);

    m_LenNodeName.setValue(len);
    
    // set the data (behind the end of the DBM user in memory)
    SAPDB_Byte* start = getNodeName();
    ToolsMeMa_GuardedOperation::MemCopy(start, nodeName, len, __FILE__, __LINE__);
}

SAPDB_Byte* DBMSrvShM_LineLayoutEventing::getNodeName() const {
    return getDBMUser() + getDBMUserLen();
}

size_t DBMSrvShM_LineLayoutEventing::getNodeNameLen() const {
    size_t retLen;
    m_LenNodeName.getValue(retLen);
    return retLen;
}

void DBMSrvShM_LineLayoutEventing::setLogFilePath( const char* logFilePath ) {
    // set the size
    size_t len(strlen(logFilePath));
    addSizeDelta(getLogFilePathLen(), len);

    m_LenLogFilePath.setValue(len);
    
    // set the data (behind the end of the nodename in memory)
    SAPDB_Byte* start = getLogFilePath();
    ToolsMeMa_GuardedOperation::MemCopy(start, logFilePath, len, __FILE__, __LINE__);
}

SAPDB_Byte* DBMSrvShM_LineLayoutEventing::getLogFilePath() const {
    return getNodeName() + getNodeNameLen();
}

size_t DBMSrvShM_LineLayoutEventing::getLogFilePathLen() const {
    size_t retLen;
    m_LenLogFilePath.getValue(retLen);
    return retLen;
}

void DBMSrvShM_LineLayoutEventing::setConfigFilePath( const char* cfgFilePath ) {
    // set the size
    size_t len(strlen(cfgFilePath));
    addSizeDelta(getConfigFilePathLen(), len);

    m_LenConfigFilePath.setValue(len);
    
    // set the data (behind the end of the logfile in memory)
    SAPDB_Byte* start = getConfigFilePath();
    ToolsMeMa_GuardedOperation::MemCopy(start, cfgFilePath, len, __FILE__, __LINE__);
}

SAPDB_Byte* DBMSrvShM_LineLayoutEventing::getConfigFilePath() const {
    return getLogFilePath() + getLogFilePathLen();
}

size_t DBMSrvShM_LineLayoutEventing::getConfigFilePathLen() const {
    size_t retLen;
    m_LenConfigFilePath.getValue(retLen);
    return retLen;
}

DBMSrvShM_LineLayoutEventing* DBMSrvShM_LineLayoutEventing::getNext() const {
    return (DBMSrvShM_LineLayoutEventing*)(DBMSrvShM_LineLayoutBase::getNext());
}
