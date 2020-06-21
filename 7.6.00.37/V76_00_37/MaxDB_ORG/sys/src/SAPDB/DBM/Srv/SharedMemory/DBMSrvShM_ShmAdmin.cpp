/*!
    @file           DBMSrvShM_Shmadmin.cpp
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

#include "gsp01.h"
#include "heo01.h"

#include "DBM/Srv/Message/DBMSrvMsg_NewRTEError.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_ShmAdmin.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp"
#include "ToolsCommon/Tools_HexBuffer.hpp"

const int DBMSrvShM_ShmAdmin::maxAllowedLinesEventing(7);
const int DBMSrvShM_ShmAdmin::maxAllowedLinesResourceLock(100);
const int DBMSrvShM_ShmAdmin::maxAllowedLinesScheduler(1);

DBMSrvShM_ShmAdmin::DBMSrvShM_ShmAdmin()
        : m_ScopeMemberCheckOnly(LineTypeUninitialized), 
          m_ShM(NULL),
          m_Initialized(false),
          m_Persistence(NULL) {
}

DBMSrvShM_ShmAdmin::~DBMSrvShM_ShmAdmin() {
    DBMSrvMsg_Error neverSeen;
    close(neverSeen);
}

DBMSrvShM_ShmAdmin DBMSrvShM_ShmAdmin::TheInstance;

DBMSrvShM_Base* DBMSrvShM_ShmAdmin::getBase( const char* infoFile, const char* dataFile, DBMSrvMsg_Error& errOut) {

    DBMSrvShM_Base * rc=new DBMSrvShM_Base(m_Persistence->getInfoFile(), m_Persistence->getDataFile(), errOut);

    if(0==rc)
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM)); // memory error

    return rc;
}

bool DBMSrvShM_ShmAdmin::initialize(const char* dbname, DBMSrvMsg_Error& errOut) {
    
    if( m_Initialized )
        return true;
    bool errorFound(false);

    if( dbname == NULL || strlen(dbname) <= 0 ) {
        // invalid DB name (we should never get here...)
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_INVDBNAME,
            Msg_Arg(SDBMSGTAG_DBMSRV_INVDBNAME__DBMSRV_COMP, "\"\""))); // display empty string
        errorFound = true;
    }

    // here is the place where we get the independent wrk path (with trailing path delimiter)
    tsp00_Pathc indepWrkPath;
    tsp01_RteError rteError;
    if( !errorFound && !sqlGetIndependentWrkPath(indepWrkPath, TERM_WITH_DELIMITER_EO01, &rteError) ) {
        errOut.Overrule(DBMSrvMsg_NewRTEError(rteError));
        errorFound = true;
    }

    // we know where to put it and what the name must be
    // create persistence object
    if( !errorFound ) {
        m_Persistence = new DBMSrvShM_Persistence(indepWrkPath.asCharp(), dbname, errOut);
        if( m_Persistence == NULL || !m_Persistence->isValid() )
            errorFound = true;
    }

    if( !errorFound ) {
        m_Initialized = true;

        // create shm base object
        if(!openSharedMemoryInfoFile(errOut))
            errorFound=true;

        // detach from shared memory (we just tested availability)
        closeSharedMemoryInfoFile();

        if(errorFound)
            m_Initialized=false;
    }
    else
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOTAVAILABLE));

    return m_Initialized;
}

bool DBMSrvShM_ShmAdmin::close( DBMSrvMsg_Error& errOut ) {

    if( !m_Initialized )
        return true;

    if(!openSharedMemoryInfoFile(errOut))
        return false;

    // remove me from player list, if that was not possible, stay connected and keep the base object
    if( lock(errOut) && m_ShM->removeMeFromPlayerList(errOut) ) {
        unlock(errOut);
        closeSharedMemoryInfoFile();
        //m_Initialized = false;
        return true;
    }
    else {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOCLOSE));
        return false;
    }
}

bool DBMSrvShM_ShmAdmin::removePersistence( DBMSrvMsg_Error& errOut ) {

    errOut.ClearMessageList();
    bool removeResult(true);

    if( m_Persistence != NULL ) {
        removeResult = m_Persistence->isValid() && m_Persistence->remove(errOut);
        if( removeResult ) {
            delete m_Persistence;
            m_Persistence = NULL;
        }

        m_Initialized=false; //this shared memory can not longer be used
    }

    if( !errOut.IsEmpty() )
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNODELETE));

    return removeResult;
}

bool DBMSrvShM_ShmAdmin::lock(DBMSrvMsg_Error& errOut) {

    if( !openSharedMemoryInfoFile(errOut) ) //open the shared memory as somebody wants to work with it
        return false;

    int attempts(10);
    while ( attempts > 0) {
        errOut.ClearMessageList();
        if( m_ShM->lockAndAttachMemory(errOut) )
            return true;
        else
            --attempts;
    }

    closeSharedMemoryInfoFile(); //if we can not lock it, close it
    return false;
}

void DBMSrvShM_ShmAdmin::unlock(DBMSrvMsg_Error& errOut) {

    if(isInitializedAndOpen(errOut))
    {
        m_ShM->unlockAndDetachMemory(errOut);
        closeSharedMemoryInfoFile(); //detach from shared memory, as it can not be used after unlock by this process anyway 
    }
}

bool DBMSrvShM_ShmAdmin::shift(DBMSrvShM_ShmAdmin::LineType lineType, SAPDB_Byte* firstByte, int offset, DBMSrvMsg_Error& errOut){
    if( isInitializedAndOpen(errOut) ) {
        // extend memory (or shrink)
        if( !m_ShM->extendMemory(firstByte, offset, errOut) || !m_ShM->shiftStartOf(lineType, offset, errOut)) {
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMERREXTEND,
	            Msg_Arg(SDBMSGTAG_DBMSRV_SHMERREXTEND__SHMLINETYPE, (int)lineType)));
            return false;
        }
        m_ScopeMemberCheckOnly = lineType;
        return true;
    }
    else
        return false;
}

DBMSrvShM_LineLayoutResourceLock* DBMSrvShM_ShmAdmin::getLineLayoutResourceLock
                (const char* resourceName, DBMSrvMsg_Error& errOut) {
    if( isInitializedAndOpen(errOut) ) {
        int existingLines(m_ShM->getExistingLinesOf(LineTypeResourceLock, errOut));
        if( existingLines > maxAllowedLinesResourceLock || existingLines < 0 ) {
            // should never happen
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMLINESEXCEEDED,
	            Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINESEXCEEDED__SHMLINETYPE,(int)LineTypeResourceLock)));
            return NULL;
        }
        DBMSrvShM_LineLayoutResourceLock* lyt(NULL);
        int nextIndex(m_ShM->getNextIndexOf(LineTypeResourceLock, errOut));
        if( nextIndex < 0 ) {
            // should never happen
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMLINESEXCEEDED,
	            Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINESEXCEEDED__SHMLINETYPE,(int)LineTypeResourceLock)));
            return NULL;
        }
        
        int count = existingLines;
        lyt = (DBMSrvShM_LineLayoutResourceLock*)m_ShM->getStartOf(LineTypeResourceLock, errOut);

        bool inScope(true);

        while(
            count>0 &&
            (inScope = isInScope(lyt, errOut)) &&
            !(strlen(resourceName) == lyt->getResourceNameLen() &&
            0 == strncmp(resourceName, (const char*)lyt->getResourceName(), lyt->getResourceNameLen())) ) {

            lyt = lyt->getNext();
            --count;
        }
        
        if( !inScope ) {
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNORESOURCELYT,
                Msg_Arg(SDBMSGTAG_DBMSRV_SHMNORESOURCELYT__RESOURCE, resourceName)));
            return NULL;
        }

        if( count > 0 ) {
            // a line for this resource exists already
            if( isInScope(lyt, errOut) )
                return lyt;
            else {
                errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNORESOURCELYT,
                    Msg_Arg(SDBMSGTAG_DBMSRV_SHMNORESOURCELYT__RESOURCE, resourceName)));
                return NULL;
            }
        }
        else {
            // this line does not exist, so create a new one
            // make room for new line
            if( !shift(  LineTypeResourceLock,
                    (SAPDB_Byte*)lyt,
                    (int)(sizeof(DBMSrvShM_LineLayoutResourceLock) + strlen(resourceName)), errOut) ) {
                        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNORESOURCELYT,
                            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNORESOURCELYT__RESOURCE, resourceName)));
                        return NULL;
            }
            // update admin information
            if( !m_ShM->setNextIndexOf(LineTypeResourceLock, nextIndex+1, errOut) ||
                !m_ShM->setExistingLinesOf(LineTypeResourceLock, existingLines+1, errOut) ) {
                errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNORESOURCELYT,
                    Msg_Arg(SDBMSGTAG_DBMSRV_SHMNORESOURCELYT__RESOURCE, resourceName)));
                return NULL;
            }

            count = existingLines;
            lyt = (DBMSrvShM_LineLayoutResourceLock*)m_ShM->getStartOf(LineTypeResourceLock, errOut);
            
            for( ; count>0 && (inScope=isInScope(lyt, errOut)); --count)
                lyt = lyt->getNext();

            if( count > 0 || !inScope ) {
                // some existing layout on the way is outof scope
                errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNORESOURCELYT,
                    Msg_Arg(SDBMSGTAG_DBMSRV_SHMNORESOURCELYT__RESOURCE, resourceName)));
                return NULL;
            }
            else {
                lyt->setInitialSize();
                lyt->setIndex(nextIndex);
                lyt->setResourceName(resourceName);
                lyt->setBGSRVPid(0);
                return lyt;
            }
        }
    }
    else
        return NULL;
}

bool DBMSrvShM_ShmAdmin::removeLineLayoutResourceLock
                (const char* resourceName, DBMSrvMsg_Error& errOut) {

    if( !isInitializedAndOpen(errOut) )
        return false;
                    
    int existingLines(m_ShM->getExistingLinesOf(LineTypeResourceLock, errOut));
    if( existingLines<0 ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNORESOURCELYTDEL,
            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNORESOURCELYTDEL__RESOURCE, resourceName)));
        return false;    
    }
    DBMSrvShM_LineLayoutResourceLock* lyt(NULL);

    int count = existingLines;
    lyt = (DBMSrvShM_LineLayoutResourceLock*)m_ShM->getStartOf(LineTypeResourceLock, errOut);

    bool inScope(true);

    while(
        count>0 &&
        (inScope=isInScope(lyt, errOut)) &&
        !(strlen(resourceName) == lyt->getResourceNameLen() &&
          0 == strncmp( resourceName, (const char*)lyt->getResourceName(), lyt->getResourceNameLen())) ) {
        lyt = lyt->getNext();
        --count;
    }
    
    if( !inScope ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNORESOURCELYTDEL,
            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNORESOURCELYTDEL__RESOURCE, resourceName)));
        return false;
    }


    if( count > 0 ) {
        // a line for this resource exists, we must remove it
        if( shift(LineTypeResourceLock, (SAPDB_Byte*)lyt, -1 * (int)((SAPDB_Byte*)lyt->getNext()-(SAPDB_Byte*)lyt), errOut) ) {
            --existingLines;
            if( m_ShM->setExistingLinesOf(LineTypeResourceLock, existingLines, errOut) ) 
                return true;
            else {
                errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNORESOURCELYTDEL,
                    Msg_Arg(SDBMSGTAG_DBMSRV_SHMNORESOURCELYTDEL__RESOURCE, resourceName)));
                return false;
            }
        }
        else {
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNORESOURCELYTDEL,
                Msg_Arg(SDBMSGTAG_DBMSRV_SHMNORESOURCELYTDEL__RESOURCE, resourceName)));
            return false;
        }
    }
    else {
        // no layout found, cannot remove it
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNORESOURCELYTDEL,
            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNORESOURCELYTDEL__RESOURCE, resourceName)));
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNORESOURCELYTFOUND,
            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNORESOURCELYTFOUND__RESOURCE, resourceName)));
        return false;
    }
}


DBMSrvShM_LineLayoutEventing* DBMSrvShM_ShmAdmin::getLineLayoutEventing(int instanceID, DBMSrvMsg_Error& errOut ) {
    
    errOut.ClearMessageList();

    if( !isInitializedAndOpen(errOut) )
        return NULL;
    
    int existingLines(m_ShM->getExistingLinesOf(LineTypeEventing, errOut));
    if( existingLines<0 ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOEVTLYT,
            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNOEVTLYT__DISPID, instanceID)));
        return NULL;
    }
    else if( existingLines == 0 )
        return NULL;
    
    DBMSrvShM_LineLayoutEventing* lyt(NULL);
    bool inScope(true);
    
    lyt = (DBMSrvShM_LineLayoutEventing*)m_ShM->getStartOf(LineTypeEventing, errOut);
    for( ; existingLines>0 && (inScope=isInScope(lyt, LineTypeEventing, errOut)); --existingLines) {
        if( lyt->getInstanceID() == instanceID )
            return lyt;
        lyt = lyt->getNext();
    }

    if( !inScope ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOEVTLYT,
            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNOEVTLYT__DISPID, instanceID)));
    }
    else {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOEVTLYTFOUND,
            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNOEVTLYTFOUND__DISPID, instanceID)));
    }

    return NULL;
}

bool DBMSrvShM_ShmAdmin::removeLineLayoutEventing(int instanceID, DBMSrvMsg_Error& errOut) {

    if( !isInitializedAndOpen(errOut) )
        return false;

    errOut.ClearMessageList();
    DBMSrvShM_LineLayoutEventing* lyt(getLineLayoutEventing(instanceID, errOut));
    
    if( lyt == NULL && errOut.IsEmpty() )
        return true;

    if( !isInScope(lyt, LineTypeEventing, errOut) ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOEVTLYTDEL,
            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNOEVTLYTDEL__DISPID, instanceID)));
        return false;
    }

    // line exists, we must remove it
    if( shift(LineTypeEventing, (SAPDB_Byte*)lyt, -1 * (int)((SAPDB_Byte*)lyt->getNext()-(SAPDB_Byte*)lyt), errOut) ) {
        int existingLines(m_ShM->getExistingLinesOf(LineTypeEventing, errOut));
        if( existingLines<0 ) {
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOEVTLYTDEL,
                Msg_Arg(SDBMSGTAG_DBMSRV_SHMNOEVTLYTDEL__DISPID, instanceID)));
            return false;
        }
        --existingLines;
        if( !m_ShM->setExistingLinesOf(LineTypeEventing, existingLines, errOut) ) {
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOEVTLYTDEL,
                Msg_Arg(SDBMSGTAG_DBMSRV_SHMNOEVTLYTDEL__DISPID, instanceID)));
            return false;
        }
        return true;
    }
    else {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOEVTLYTDEL,
            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNOEVTLYTDEL__DISPID, instanceID)));
        return false;
    }
}

DBMSrvShM_LineLayoutEventing* DBMSrvShM_ShmAdmin::getLineLayoutEventing(
        bool& visibilityCheck,
        int dispatcherPID,
        SAPDB_Byte commStatus,
        const char* dbmUser,
        const char* nodeName,
        const char* configFilePath,
        const char* logFilePath,
        DBMSrvMsg_Error& errOut) {

    if( !isInitializedAndOpen(errOut) )
        return false;

    int existingLines(m_ShM->getExistingLinesOf(LineTypeEventing, errOut));

    if( existingLines > maxAllowedLinesEventing || existingLines<0 ) {
        // should never happen
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMLINESEXCEEDED,
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINESEXCEEDED__SHMLINETYPE,(int)LineTypeEventing)));
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOEVTLYTNEW,
            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNOEVTLYTNEW__PID, dispatcherPID)));
        return NULL;
    }

    bool makeInternal(visibilityCheck);

    DBMSrvShM_LineLayoutEventing* lyt(NULL);
    int nextIndex(m_ShM->getNextIndexOf(LineTypeEventing, errOut));
    if( nextIndex<0 ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOEVTLYTNEW,
            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNOEVTLYTNEW__PID, dispatcherPID)));
        return NULL;
    }

    int lastInstanceID(-1); // this will be the ID of the new instance
    int currInstanceID(-1);
    bool slotFound(false);
    bool inScope(true);

    int count = existingLines;
    lyt = (DBMSrvShM_LineLayoutEventing*)m_ShM->getStartOf(LineTypeEventing, errOut);
    for( ; count>0 && (inScope=isInScope(lyt, LineTypeEventing, errOut)); --count) {
        if( visibilityCheck && lyt->isInternal() )
            return NULL;

        currInstanceID = lyt->getInstanceID();
        if( !slotFound ) {
            if( currInstanceID > lastInstanceID+1 ) {
                // found a slot
                slotFound = true;
                ++lastInstanceID;
            }
            else
                lastInstanceID = currInstanceID;
        }
        lyt = lyt->getNext();
    }
    
    if( !inScope ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOEVTLYTNEW,
            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNOEVTLYTNEW__PID, dispatcherPID)));
        return NULL;
    }

    visibilityCheck = false;

    if( !slotFound )
        ++lastInstanceID;

    if( count>0 )
        return NULL;

    // now lyt points behind last eventing eventing line layout

    // make room for new line
    if( shift(
            LineTypeEventing,
            (SAPDB_Byte*)lyt,
            (int)(sizeof(DBMSrvShM_LineLayoutEventing) +
                strlen(dbmUser) + strlen(nodeName) + strlen(configFilePath) + strlen(logFilePath)), errOut) ) {
    
        // update admin information
        if( !m_ShM->setNextIndexOf(LineTypeEventing, nextIndex+1, errOut) ||
            !m_ShM->setExistingLinesOf(LineTypeEventing, existingLines+1, errOut) ) {
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOEVTLYTNEW,
                Msg_Arg(SDBMSGTAG_DBMSRV_SHMNOEVTLYTNEW__PID, dispatcherPID)));
            return NULL;
        }

        count = existingLines;
        lyt = (DBMSrvShM_LineLayoutEventing*)m_ShM->getStartOf(LineTypeEventing, errOut);
        for( ; count>0 && (inScope=isInScope(lyt, LineTypeEventing, errOut)); count--)
            lyt = lyt->getNext();

        if( count>0 )
            return NULL;

        lyt->setInitialSize();
        lyt->setIndex(nextIndex);
        lyt->setAccessibleFlag(makeInternal?0:1);
        lyt->setInstanceID(lastInstanceID);
        lyt->setBGSRVPid((RTE_OSPid)dispatcherPID);
        lyt->setInternalCommStatus(commStatus);
        lyt->unCancel();
        lyt->setDBMUser(dbmUser);
        lyt->setNodeName( nodeName );
        lyt->setLogFilePath( logFilePath );
        lyt->setConfigFilePath( configFilePath );

        return lyt;
    }
    else {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOEVTLYTNEW,
            Msg_Arg(SDBMSGTAG_DBMSRV_SHMNOEVTLYTNEW__PID, dispatcherPID)));
        return NULL;
    }
}

const DBMSrvShM_LineLayoutEventing* DBMSrvShM_ShmAdmin::getFirstLineLayoutEventing(DBMSrvMsg_Error& errOut) {
    
    errOut.ClearMessageList();
    int existingLines(m_ShM->getExistingLinesOf(LineTypeEventing, errOut));
    if( existingLines < 0 )
        return NULL;
    else if( existingLines == 0 )
        return NULL;
    else {
        const DBMSrvShM_LineLayoutEventing* lyt = (DBMSrvShM_LineLayoutEventing*)m_ShM->getStartOf(LineTypeEventing, errOut);
        if( !isInScope(lyt, LineTypeEventing, errOut )) {
            return NULL;
        }
        else
            return lyt;
    }
}

const DBMSrvShM_LineLayoutEventing* DBMSrvShM_ShmAdmin::getNextLineLayoutEventing(
        const DBMSrvShM_LineLayoutEventing* aLayout, DBMSrvMsg_Error& errOut) {

    errOut.ClearMessageList();
    if( aLayout == NULL )
        return NULL;

    if( !isInScope(aLayout, LineTypeEventing, errOut) )
        return NULL;
    
    DBMSrvShM_LineLayoutEventing* next = aLayout->getNext();
    
    if( (SAPDB_Byte*)next >= m_ShM->getStartOf(LineTypeResourceLock, errOut) ) // linetype behind...
        return NULL;
    else
        return isInScope(next, LineTypeEventing, errOut)?next:NULL;
}

DBMSrvShM_LineLayoutScheduler* DBMSrvShM_ShmAdmin::getLineLayoutScheduler( bool createIfNecessary, DBMSrvMsg_Error& errOut ) {
    if( !isInitializedAndOpen(errOut) )
        return NULL;

    
    int existingLines(m_ShM->getExistingLinesOf(LineTypeScheduler, errOut));
    if( existingLines<0 ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOSCHEDLYT));
        return NULL;
    }

    if( !createIfNecessary && (existingLines == 0) )
        return NULL;

    if( existingLines > maxAllowedLinesScheduler ) {
        // should never happen
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMLINESEXCEEDED,
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINESEXCEEDED__SHMLINETYPE,(int)LineTypeScheduler)));
        return NULL;
    }

    DBMSrvShM_LineLayoutScheduler* lyt(NULL);
    if( existingLines == maxAllowedLinesScheduler ) {
        // it exists, so return it
        lyt = (DBMSrvShM_LineLayoutScheduler*) m_ShM->getStartOf(LineTypeScheduler, errOut);
        if( isInScope(lyt, errOut) )
            return lyt;
        else {
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOSCHEDLYT));
            return NULL;
        }
    }

    int nextIndex(m_ShM->getNextIndexOf(LineTypeScheduler, errOut));
    if( nextIndex<0 ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOSCHEDLYT));
        return NULL;
    }

    int count = existingLines;
    lyt = (DBMSrvShM_LineLayoutScheduler*)m_ShM->getStartOf(LineTypeScheduler, errOut);

    // now lyt points behind last line layout of preceding type

    bool inScope(true);
    for( ; count>0 && (inScope=isInScope(lyt,errOut)); --count)
        lyt = lyt->getNext();

    if( !inScope ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOSCHEDLYT));
        return NULL;
    }


    // make room for new line
    if( shift(LineTypeScheduler, (SAPDB_Byte*)lyt, sizeof(DBMSrvShM_LineLayoutScheduler), errOut) ) {
        // update admin information
        if( !m_ShM->setNextIndexOf(LineTypeScheduler, nextIndex+1, errOut) ||
            !m_ShM->setExistingLinesOf(LineTypeScheduler, existingLines+1, errOut) ) {
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOSCHEDLYT));
            return NULL;
        }
    }
    else {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOSCHEDLYT));
        return NULL;
    }

    count = existingLines;
    lyt = (DBMSrvShM_LineLayoutScheduler*)m_ShM->getStartOf(LineTypeScheduler, errOut);
    for( ; count>0 && (inScope=isInScope(lyt,errOut)); count--)
        lyt = lyt->getNext();

    if( !inScope ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOSCHEDLYT));
        return NULL;
    }

    lyt->setInitialSize();
    lyt->setIndex(nextIndex);
    lyt->setBGSRVPid(0);
    lyt->setInfoType(0);
    lyt->setRecipient(0);
    lyt->setVariablePart("");
    lyt->unCancel();
    return lyt;
}

bool DBMSrvShM_ShmAdmin::removeLineLayoutScheduler(DBMSrvMsg_Error& errOut) {
    
    if( !isInitializedAndOpen(errOut) )
        return false;

    errOut.ClearMessageList();
    DBMSrvShM_LineLayoutScheduler* lyt(getLineLayoutScheduler(false, errOut));
    
    if( lyt == NULL && errOut.IsEmpty() )
        return true;
    
    if( !isInScope(lyt, errOut) ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOSCHEDLYTDEL));
        return false;
    }

    // line exists, we must remove it
    if( shift(LineTypeScheduler, (SAPDB_Byte*)lyt, -1 * (int)((SAPDB_Byte*)lyt->getNext()-(SAPDB_Byte*)lyt), errOut) ) {
        int existingLines(m_ShM->getExistingLinesOf(LineTypeScheduler, errOut));
        if( existingLines<1 ) {
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOSCHEDLYTDEL));
            return false;
        }
        --existingLines;
        if( !m_ShM->setExistingLinesOf(LineTypeScheduler, existingLines, errOut) ) {
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOSCHEDLYTDEL));
            return false;
        }
        return true;
    }
    else {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOSCHEDLYTDEL));
        return false;
    }
}

bool DBMSrvShM_ShmAdmin::isInScope( const DBMSrvShM_LineLayoutResourceLock* aLayout, DBMSrvMsg_Error& errOut) {

    int hangOver(0);
    bool baseCheckOkay(false);

    do {
        const SAPDB_Byte* lastByte(NULL);

        // common base check
        baseCheckOkay = scopeBaseCheck(aLayout, lastByte);
        if( !baseCheckOkay )
            break;

        // check if the member variables are in scope
        hangOver = (int)(((SAPDB_Byte*)((SAPDB_Byte*)aLayout + (sizeof(*aLayout)-1)) - lastByte));
        if( hangOver > 0 )
            break;

        if( m_ScopeMemberCheckOnly == DBMSrvShM_ShmAdmin::LineTypeResourceLock ) {
            m_ScopeMemberCheckOnly = DBMSrvShM_ShmAdmin::LineTypeUninitialized;
            return true;
        }

        // now we know we can access the members
        if( (SAPDB_Byte*)((SAPDB_Byte*)aLayout + sizeof(*aLayout)-1 + aLayout->getResourceNameLen()) > lastByte )
            break;

        return true;
    } while(true);

    const SAPDB_Byte* firstByte(NULL);
    const SAPDB_Byte* lastByte(NULL);
    m_ShM->getMemoryBorders( firstByte, lastByte );
    errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMLINEOUTOFSCOPE,
        Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINEOUTOFSCOPE__SIZE,(char*)SAPDB_ToString((size_t)(lastByte-firstByte), SAPDB_ToStringClass::hex)),
	    Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINEOUTOFSCOPE__SHMLINETYPE, (int)LineTypeResourceLock),
        Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINEOUTOFSCOPE__BYTE_OFFSET, aLayout==NULL?"(not available)":(char*)SAPDB_ToString((size_t)(((SAPDB_Byte*)(aLayout))-firstByte), SAPDB_ToStringClass::hex)),
	    Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINEOUTOFSCOPE__SHMLINEDATA,
            baseCheckOkay?Tools_HexBuffer((void*)aLayout, sizeof(*aLayout)-(size_t)hangOver).GiveHexBuffer():"(null)")));
    return false;
}

bool DBMSrvShM_ShmAdmin::isInScope( const DBMSrvShM_LineLayoutEventing* aLayout, LineType lineType, DBMSrvMsg_Error& errOut) {

    int hangOver(0);
    bool baseCheckOkay(false);

    do {
        const SAPDB_Byte* lastByte(NULL);

        // common base check
        baseCheckOkay = scopeBaseCheck(aLayout, lastByte);
        if( !baseCheckOkay )
            break;

        // check if the member variables are in scope
        hangOver = (int)(((SAPDB_Byte*)((SAPDB_Byte*)aLayout + (sizeof(*aLayout)-1)) - lastByte));
        if( hangOver > 0 )
            break;

        if( m_ScopeMemberCheckOnly == DBMSrvShM_ShmAdmin::LineTypeEventing) {
            m_ScopeMemberCheckOnly = DBMSrvShM_ShmAdmin::LineTypeUninitialized;
            return true;
        }

        // now we know we can access the members
            if( (SAPDB_Byte*)(
                    (SAPDB_Byte*)aLayout +
                    sizeof(*aLayout)-1 +
                    aLayout->getDBMUserLen() +
                    aLayout->getNodeNameLen() +
                    aLayout->getConfigFilePathLen() +
                    aLayout->getLogFilePathLen()) > lastByte )
            break;
        
        // all checks passed
        return true;
    } while(true);

    const SAPDB_Byte* firstByte(NULL);
    const SAPDB_Byte* lastByte(NULL);
    m_ShM->getMemoryBorders( firstByte, lastByte );
    errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMLINEOUTOFSCOPE,
        Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINEOUTOFSCOPE__SIZE,(char*)SAPDB_ToString((size_t)(lastByte-firstByte), SAPDB_ToStringClass::hex)),
	    Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINEOUTOFSCOPE__SHMLINETYPE, (int)lineType),
        Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINEOUTOFSCOPE__BYTE_OFFSET, aLayout==NULL?"(not available)":(char*)SAPDB_ToString((size_t)(((SAPDB_Byte*)(aLayout))-firstByte), SAPDB_ToStringClass::hex)),
	    Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINEOUTOFSCOPE__SHMLINEDATA,
            baseCheckOkay?Tools_HexBuffer((void*)aLayout, sizeof(*aLayout)-(size_t)hangOver).GiveHexBuffer():"(null)")));
    return false;
}

bool DBMSrvShM_ShmAdmin::isInScope( const DBMSrvShM_LineLayoutScheduler* aLayout, DBMSrvMsg_Error& errOut) {
    
    int hangOver(0);
    bool baseCheckOkay(false);

    do {
        const SAPDB_Byte* lastByte(NULL);

        // common base check
        baseCheckOkay = scopeBaseCheck(aLayout, lastByte);
        if( !baseCheckOkay )
            break;

        // check if the member variables are in scope
        hangOver = (int)(((SAPDB_Byte*)((SAPDB_Byte*)aLayout + (sizeof(*aLayout)-1)) - lastByte));
        if( hangOver > 0 )
            break;

        if( m_ScopeMemberCheckOnly == DBMSrvShM_ShmAdmin::LineTypeScheduler ) {
            m_ScopeMemberCheckOnly = DBMSrvShM_ShmAdmin::LineTypeUninitialized;
            return true;
        }

        // now we know we can access the members
        if( (SAPDB_Byte*)((SAPDB_Byte*)aLayout + sizeof(*aLayout)-1 + aLayout->getVariablePartLen() + aLayout->getUserNameLen()) > lastByte )
            break;
        
        // all checks passed
        return true;
    } while(true);

    const SAPDB_Byte* firstByte(NULL);
    const SAPDB_Byte* lastByte(NULL);
    m_ShM->getMemoryBorders( firstByte, lastByte );
    errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMLINEOUTOFSCOPE,
        Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINEOUTOFSCOPE__SIZE,(char*)SAPDB_ToString((size_t)(lastByte-firstByte), SAPDB_ToStringClass::hex)),
	    Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINEOUTOFSCOPE__SHMLINETYPE, (int)LineTypeScheduler),
        Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINEOUTOFSCOPE__BYTE_OFFSET, aLayout==NULL?"(not available)":(char*)SAPDB_ToString((size_t)(((SAPDB_Byte*)(aLayout))-firstByte), SAPDB_ToStringClass::hex)),
	    Msg_Arg(SDBMSGTAG_DBMSRV_SHMLINEOUTOFSCOPE__SHMLINEDATA,
            baseCheckOkay?Tools_HexBuffer((void*)aLayout, sizeof(*aLayout)-(size_t)hangOver).GiveHexBuffer():"(null)")));
    return false;
}

bool DBMSrvShM_ShmAdmin::scopeBaseCheck(
            const DBMSrvShM_LineLayoutBase * aLayout,
            const SAPDB_Byte*& shmLastByte) {
    // check layout and shm pointers themselves
    if( aLayout == NULL || m_ShM == NULL )
        return false;

    const SAPDB_Byte* shmFirstByte(NULL);
    // get the current memory borders
    m_ShM->getMemoryBorders(shmFirstByte, shmLastByte);

    // check if there is shm allocated
    if( shmFirstByte == NULL || shmLastByte == NULL )
        return false;

    // check if the start of the layout is in scope   
    return ((SAPDB_Byte*)aLayout >= shmFirstByte) && ((SAPDB_Byte*)aLayout <= shmLastByte);
}

bool DBMSrvShM_ShmAdmin::isInitializedAndOpen(DBMSrvMsg_Error & error)
{
    if( m_Initialized && 0!=m_ShM )
        return true;
    else
    {
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOTAVAILABLE));
        return false;
    }
}

bool DBMSrvShM_ShmAdmin::openSharedMemoryInfoFile(
        DBMSrvMsg_Error & error)
{
    if( m_Initialized && 0==m_ShM ) //everything is ready to open the ....shi file but it is not mapped into memory yet
    {
        m_ShM=getBase(m_Persistence->getInfoFile(), m_Persistence->getDataFile(), error);

        if( 0!=m_ShM && !error.IsEmpty() )
            closeSharedMemoryInfoFile();
    }

    return isInitializedAndOpen(error);
}

 void DBMSrvShM_ShmAdmin::closeSharedMemoryInfoFile()
 {
     if(m_ShM)
     {
         delete m_ShM;
         m_ShM=0;
     }
 }
