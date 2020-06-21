/*!
    @file           DBMSrvShM_LineAccessorEventing.cpp
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

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorEventing.hpp"

DBMSrvShM_LineAccessorEventing::DBMSrvShM_LineAccessorEventing(
        const int instanceID, DBMSrvMsg_Error& errOut)
        : DBMSrvShM_LineAccessorBase(),
          m_InstanceID(instanceID) {

    if( DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) ) {
        DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(instanceID, errOut);
        
        if( !errOut.IsEmpty() ) {
            DBMSrvMsg_Error unlockError;
            DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
            errOut.Overrule(unlockError);
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_EVT_NO_CREATE,
	            Msg_Arg(SDBMSGTAG_DBMSRV_EVT_NO_CREATE__DISPID, m_InstanceID)));
            return;
        }

        m_Exists = (le != NULL);

        if( m_Exists ) {
            setIndex(le->getIndex());
            m_Validity = true;
        }
        
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
        if( !errOut.IsEmpty() ) {
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_EVT_NO_CREATE,
	            Msg_Arg(SDBMSGTAG_DBMSRV_EVT_NO_CREATE__DISPID, m_InstanceID)));
            m_Validity = false;
        }
    }
}

bool DBMSrvShM_LineAccessorEventing::existsEventing(DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    if( DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) ) {
        DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);
        if( !errOut.IsEmpty() ) {
            DBMSrvMsg_Error unlockError;
            DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
            errOut.Overrule(unlockError);
            m_Validity = false;
            return false;
        }

        m_Exists = (le != NULL);

        DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
        if( !errOut.IsEmpty() ) {
            m_Exists = false;
            m_Validity = false;
        }
    }
    else {
        m_Exists = false;
        m_Validity = false;
    }

    return DBMSrvShM_LineAccessorBase::exists();
}

DBMSrvShM_LineAccessorEventing::DBMSrvShM_LineAccessorEventing(
        const char* dispatcherPID,
        const char* visibilityStatusString,    
        const char* commStatusString,
        const char* dbmUser,
        const char* nodeName,
        const char* configFilePath,
        const char* logFilePath,
        DBMSrvMsg_Error& errOut)
        : DBMSrvShM_LineAccessorBase(),
          m_VisibiltyCheck(false) {

    errOut.ClearMessageList();
    SAPDB_Byte visibilityStatus(atoi(visibilityStatusString));
    SAPDB_Byte commStatus(atoi(commStatusString));

    m_VisibiltyCheck = (visibilityStatus == INTERNAL);

    if( DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) ) {
        DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(
                m_VisibiltyCheck,
                atoi(dispatcherPID),
                commStatus,
                dbmUser,
                nodeName,
                configFilePath,
                logFilePath,
                errOut);

        if( !errOut.IsEmpty() ) {
            DBMSrvMsg_Error unlockError;
            DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
            errOut.Overrule(unlockError);
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_EVT_NO_CREATE_PID,
	            Msg_Arg(SDBMSGTAG_DBMSRV_EVT_NO_CREATE_PID__PID, dispatcherPID)));
            return;
        }


        m_Exists = (le != NULL);
        
        if( m_Exists ) {
            setIndex(le->getIndex());
            m_InstanceID = le->getInstanceID();
            m_Validity = true;
        }
        
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);    
    }

    if( !m_Exists || !errOut.IsEmpty() )
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_EVT_NO_CREATE_PID,
	        Msg_Arg(SDBMSGTAG_DBMSRV_EVT_NO_CREATE_PID__PID, dispatcherPID)));
}

bool DBMSrvShM_LineAccessorEventing::getCommunicationStatus(
        InternalCommStatus& commStatus, DBMSrvMsg_Error& errOut) {

    errOut.ClearMessageList();
    DBMSrvShM_LineLayoutEventing* le(NULL);

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( m_Exists )
        commStatus = (InternalCommStatus)le->getInternalCommStatus();

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::setCommunicationStatus(
        const InternalCommStatus newCommStatus, DBMSrvMsg_Error& errOut) {

    errOut.ClearMessageList();

    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( m_Exists )
        le->setInternalCommStatus((SAPDB_Byte)newCommStatus);

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::getVisibilityStatus(
        VisibilityStatus& visibilityStatus, DBMSrvMsg_Error& errOut) {

    errOut.ClearMessageList();    
    DBMSrvShM_LineLayoutEventing* le(NULL);
    
    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( m_Exists )
        visibilityStatus = (VisibilityStatus)(le->isInternal()?0:1);

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::setVisibilityStatus(
        const VisibilityStatus newVisibilityStatus, DBMSrvMsg_Error &errOut) {

    errOut.ClearMessageList();

    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);
    
    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }
    
    m_Exists = (le != NULL);

    if( m_Exists )
        le->setAccessibleFlag((SAPDB_Byte)newVisibilityStatus);

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::getDBMUser(char*& dbmUser, DBMSrvMsg_Error &errOut) {
    
    errOut.ClearMessageList();
    if( !isValid() )
        return false;

    DBMSrvShM_LineLayoutEventing* le(NULL);

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( m_Exists ) {
        size_t len = le->getDBMUserLen();
        dbmUser = new char[len+1];
        if( dbmUser != NULL ) {
            SAPDB_memcpy(dbmUser, le->getDBMUser(), len);
            dbmUser[len] = '\0';
        }
    }

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::setDBMUser(const char* newDbmUser, DBMSrvMsg_Error &errOut) {

    errOut.ClearMessageList();
    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);
    
    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }
    
    m_Exists = (le != NULL);


    if( m_Exists ) {
        SAPDB_Byte* start = le->getDBMUser();
        size_t oldLen = le->getDBMUserLen();
        size_t newLen = strlen(newDbmUser);
        if( DBMSrvShM_ShmAdmin::TheAdmin().shift(
                DBMSrvShM_ShmAdmin::TheAdmin().LineTypeEventing,
                start,
                (int)newLen - (int)oldLen, errOut) ) {
            le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);
            m_Exists = (le != NULL);
            if( m_Exists )
                le->setDBMUser(newDbmUser);
        }
    }

    DBMSrvMsg_Error unlockError;
    DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
    errOut.Overrule(unlockError);

    return m_Exists && errOut.IsEmpty();
}


bool DBMSrvShM_LineAccessorEventing::getNodeName(char*& outNodeName, DBMSrvMsg_Error &errOut) {
    
    errOut.ClearMessageList();
    DBMSrvShM_LineLayoutEventing* le(NULL);

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);
    
    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( m_Exists ) {
        size_t len = le->getNodeNameLen();
        outNodeName = new char[len+1];
        if( outNodeName != NULL ) {
            SAPDB_memcpy(outNodeName, le->getNodeName(), len);
            outNodeName[len] = '\0';
        }
    }

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::setNodeName(const char* newNodeName, DBMSrvMsg_Error &errOut) {
    
    errOut.ClearMessageList();
    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( m_Exists ) {
        SAPDB_Byte* start = le->getNodeName();
        size_t oldLen = le->getNodeNameLen();
        size_t newLen = strlen(newNodeName);
        if( DBMSrvShM_ShmAdmin::TheAdmin().shift(
                DBMSrvShM_ShmAdmin::TheAdmin().LineTypeEventing,
                start,
                (int)newLen - (int)oldLen, errOut) ) {
            le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);
            m_Exists = (le != NULL);
            if( m_Exists )
                le->setNodeName(newNodeName);
        }
    }

    DBMSrvMsg_Error unlockError;
    DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
    errOut.Overrule(unlockError);

    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::getConfigFilePath(char*& configFilePath, DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    DBMSrvShM_LineLayoutEventing* le(NULL);

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( m_Exists ) {
        size_t len = le->getConfigFilePathLen();
        configFilePath = new char[len+1];
        if( configFilePath != NULL ) {
            SAPDB_memcpy(configFilePath, le->getConfigFilePath(), len);
            configFilePath[len] = '\0';
        }
    }

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::setConfigFilePath(const char* newConfigFilePath, DBMSrvMsg_Error& errOut) {

    errOut.ClearMessageList();
    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( m_Exists ) {
        SAPDB_Byte* start = le->getConfigFilePath();
        size_t oldLen = le->getConfigFilePathLen();
        size_t newLen = strlen(newConfigFilePath);
        if( DBMSrvShM_ShmAdmin::TheAdmin().shift(
                DBMSrvShM_ShmAdmin::TheAdmin().LineTypeEventing,
                start,
                (int)newLen - (int)oldLen, errOut) ) {
            le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);
            m_Exists = (le != NULL);
            if( m_Exists )
                le->setConfigFilePath(newConfigFilePath);
        }
    }

    DBMSrvMsg_Error unlockError;
    DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
    errOut.Overrule(unlockError);

    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::getLogFilePath(char*& logFilePath, DBMSrvMsg_Error& errOut) {

    errOut.ClearMessageList();
    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( m_Exists ) {
        size_t len = le->getLogFilePathLen();
        logFilePath = new char[len+1];
        if( logFilePath != NULL ) {
            SAPDB_memcpy(logFilePath, le->getLogFilePath(), len);
            logFilePath[len] = '\0';
        }
    }

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::setLogFilePath(const char* newLogFilePath, DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);
    m_Exists = (le != NULL);
    if( m_Exists ) {
        SAPDB_Byte* start = le->getLogFilePath();
        size_t oldLen = le->getLogFilePathLen();
        size_t newLen = strlen(newLogFilePath);
        if( DBMSrvShM_ShmAdmin::TheAdmin().shift(
                DBMSrvShM_ShmAdmin::TheAdmin().LineTypeEventing,
                start,
                (int)newLen - (int)oldLen, errOut) ) {
            le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);
            m_Exists = (le != NULL);
            if( m_Exists )
                le->setLogFilePath(newLogFilePath);
        }
    }

    DBMSrvMsg_Error unlockError;
    DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
    errOut.Overrule(unlockError);

    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::removeBGSRVPid(DBMSrvMsg_Error& errOut) {

    errOut.ClearMessageList();
    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);
    if( m_Exists )
        le->removeBGSRVPid();

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::setBGSRVPid(DBMSrvMsg_Error& errOut) {
   
    errOut.ClearMessageList();
    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);
    if( m_Exists )
        le->setBGSRVPid(RTESys_ProcessID());
    
    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::getBGSRVPid( RTE_OSPid& pid, DBMSrvMsg_Error& errOut) {

    DBMSrvShM_LineLayoutEventing* le(NULL);
    errOut.ClearMessageList();

    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);

    if( m_Exists )
        pid = le->getBGSRVPid();

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::doCancel(DBMSrvMsg_Error& errOut) {

    errOut.ClearMessageList();

    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);    
    if( m_Exists )
        le->doCancel();
    
    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::unCancel(DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    DBMSrvShM_LineLayoutEventing* le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);

    if( !errOut.IsEmpty() ) {
        DBMSrvMsg_Error unlockError;
        DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
        errOut.Overrule(unlockError);
        return false;
    }

    m_Exists = (le != NULL);    
    if( m_Exists )
        le->unCancel();

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::isCancelled(bool& iscanc, DBMSrvMsg_Error& errOut) {

    DBMSrvShM_LineLayoutEventing* le(NULL);
    errOut.ClearMessageList();

    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    le = DBMSrvShM_ShmAdmin::TheAdmin().getLineLayoutEventing(m_InstanceID, errOut);
    m_Exists = (le != NULL);

    iscanc = !m_Exists || le->isCancelled();

    DBMSrvShM_ShmAdmin::TheAdmin().unlock(errOut);
    return m_Exists && errOut.IsEmpty();
}

bool DBMSrvShM_LineAccessorEventing::removeLineEventing(DBMSrvMsg_Error& errOut) {
 
    errOut.ClearMessageList();

    if( !isValid() )
        return false;

    if( !DBMSrvShM_ShmAdmin::TheAdmin().lock(errOut) )
        return false;

    if( DBMSrvShM_ShmAdmin::TheAdmin().removeLineLayoutEventing(m_InstanceID, errOut) )
        m_Exists = false;

    DBMSrvMsg_Error unlockError;
    DBMSrvShM_ShmAdmin::TheAdmin().unlock(unlockError);
    errOut.Overrule(unlockError);
    return errOut.IsEmpty();
}