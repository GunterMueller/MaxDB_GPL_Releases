/*!
    @file           DBMSrvShM_LineIteratorEventing.cpp
    @author         MarcW
    @brief

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

#include "DBM/Srv/SharedMemory/DBMSrvShM_LineIteratorEventing.hpp"

DBMSrvShM_LineIteratorEventing::DBMSrvShM_LineIteratorEventing(
        DBMSrvMsg_Error& errOut, bool iterateInternals)
      : m_IterateInternals(iterateInternals),
        m_Valid(true) {

    errOut.ClearMessageList();
    if( DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) ) {
        const DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getFirstLineLayoutEventing(errOut);

        while( le != NULL && errOut.IsEmpty() ) {

            // step over the "other"
            if( m_IterateInternals != le->isInternal()) {
                le = DBMSrvShM_ShmAdmin::TheAdmin().getNextLineLayoutEventing(le, errOut);
                continue;
            }

            size_t dataLen(0);
            char* sharedData(NULL);

            // instance ID
            int instanceID(le->getInstanceID());

            int dispatcherProcessID((int)le->getBGSRVPid());

            // communication status
            SAPDB_Byte communicationStatus(le->getInternalCommStatus());

            // user name
            dataLen = le->getDBMUserLen();
            sharedData = (char*)le->getDBMUser();
            char* localDBMUser = new char[dataLen+1];
            if( localDBMUser == NULL ) {
                m_Valid = false;
                break;
            }
            localDBMUser[0] = '\0';
            SAPDB_strncat(localDBMUser, sharedData, dataLen);

            // node name
            dataLen = le->getNodeNameLen();
            sharedData = (char*)le->getNodeName();
            char* localNodeName = new char[dataLen+1];
            if( localNodeName == NULL ) {
                m_Valid = false;
                break;
            }
            localNodeName[0] = '\0';
            SAPDB_strncat(localNodeName, sharedData, dataLen);

            // log file
            dataLen = le->getLogFilePathLen();
            sharedData = (char*)le->getLogFilePath();
            char* localLogFile = new char[dataLen+1];
            if( localLogFile == NULL ) {
                m_Valid = false;
                break;
            }
            localLogFile[0] = '\0';
            SAPDB_strncat(localLogFile, sharedData, dataLen);

            // config file
            dataLen = le->getConfigFilePathLen();
            sharedData = (char*)le->getConfigFilePath();
            char* localConfigFile = new char[dataLen+1];
            if( localConfigFile == NULL ) {
                m_Valid = false;
                break;
            }
            localConfigFile[0] = '\0';
            SAPDB_strncat(localConfigFile, sharedData, dataLen);

            LineData* newLine = new LineData(
                                        instanceID,
                                        dispatcherProcessID,
                                        (DBMSrvShM_LineAccessorEventing::InternalCommStatus)communicationStatus,
                                        localDBMUser,
                                        localNodeName,
                                        localConfigFile,
                                        localLogFile);
            if( newLine == NULL ) {
                m_Valid = false;
                break;
            }

            m_EventingLines.push_back(newLine);
            le = DBMSrvShM_ShmAdmin::TheAdmin().getNextLineLayoutEventing(le, errOut);
        }

        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        if( !errOut.IsEmpty() )
            m_Valid = false;

        if( m_Valid && m_EventingLines.size() > 0 )
            m_LineIterator = m_EventingLines.begin();
    }
    else
        m_Valid = false;

    if( !m_Valid )
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_EVT_NO_ITERATOR_CREATE));

}

DBMSrvShM_LineIteratorEventing::LineData::LineData(
        int instanceID,
        int dispatcherProcessID,
        DBMSrvShM_LineAccessorEventing::InternalCommStatus communicationStatus,
        char* dbmUser,
        char* nodeName,
        char* configFilePath,
        char* logFilePath)
        : m_InstanceID(instanceID),
          m_DispatcherProcessID(dispatcherProcessID),
          m_CommunicationStatus(communicationStatus),
          m_DBMUser(dbmUser),
          m_NodeName(nodeName),
          m_ConfigFilePath(configFilePath),
          m_LogFilePath(logFilePath) {
}


DBMSrvShM_LineIteratorEventing::LineData::~LineData() {
    delete[] m_DBMUser;
    delete[] m_NodeName;
    delete[] m_ConfigFilePath;
    delete[] m_LogFilePath;
}

int DBMSrvShM_LineIteratorEventing::getInstanceID() {
    if( m_EventingLines.size() > 0 )
        return (*m_LineIterator)->m_InstanceID;
    else
        return -1;
}

int DBMSrvShM_LineIteratorEventing::getDispatcherProcessID() {
    if( m_EventingLines.size() > 0 )
        return (*m_LineIterator)->m_DispatcherProcessID;
    else
        return -1;
}

DBMSrvShM_LineAccessorEventing::InternalCommStatus DBMSrvShM_LineIteratorEventing::getCommunicationStatus() {
    if( m_EventingLines.size() > 0 )
        return (*m_LineIterator)->m_CommunicationStatus;
    else
        return DBMSrvShM_LineAccessorEventing::LOCAL; // the save alternative
}

const char* DBMSrvShM_LineIteratorEventing::getDBMUser() {
    if( m_EventingLines.size() > 0 )
        return (*m_LineIterator)->m_DBMUser;
    else
        return NULL;
}

const char* DBMSrvShM_LineIteratorEventing::getNodeName() {
    if( m_EventingLines.size() > 0 )
        return (*m_LineIterator)->m_NodeName;
    else
        return NULL;
}

const char* DBMSrvShM_LineIteratorEventing::getConfigFilePath() {
    if( m_EventingLines.size() > 0 )
        return (*m_LineIterator)->m_ConfigFilePath;
    else
        return NULL;
}

const char* DBMSrvShM_LineIteratorEventing::getLogFilePath() {
    if( m_EventingLines.size() > 0 )
        return (*m_LineIterator)->m_LogFilePath;
    else
        return NULL;
}
