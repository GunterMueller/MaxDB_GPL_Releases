/*!
  \file    DBMSrvScd_Persistor.cpp
  \author  MarcW
  \brief   persistor implementation

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#include "DBM/Srv/Scheduler/DBMSrvScd_Persistor.hpp"

DBMSrvScd_Persistor::DBMSrvScd_Persistor( const char* folder )
        : m_Persistence(NULL) {
    
    Tools_DynamicUTF8String strFile(folder);
#ifdef _WIN32
    strFile += "\\";
#else
    strFile += "/";
#endif
    strFile += m_FileName;
    m_Persistence = new Tools_Properties(strFile);
}

bool DBMSrvScd_Persistor::persistJob( DBMSrvScd_Job* aJob) {
    if( aJob->getID() == -1 ) {
        //we must give it a ID
        int newID(-1);
        int currID(-1);
        int oldID(-1);
        Tools_List<Tools_Property*>* vProps = m_Persistence->getAllProperties();
        Tools_List<Tools_Property*>::iterator prop = NULL;

        for( prop = vProps->begin() ; prop != vProps->end(); prop++ ) {
            DBMSrvScd_Job* storedJob = new DBMSrvScd_Job((*prop)->getValue());
            delete *prop;
            currID = storedJob->getID();
            delete storedJob;
            if( currID > oldID+1 ) {
                newID = oldID+1;
                break;
            }
            else {
                oldID = currID;
            }
        }

        if( newID == -1 )
            newID = currID + 1;
        
        aJob->setID(newID);
    }

    char idbuf[16];
    sprintf(idbuf, m_KeyFormat, aJob->getID());
    Tools_DynamicUTF8String key(idbuf);
    char* serJob(aJob->serialize());
    Tools_DynamicUTF8String value(serJob);
    delete[] serJob;

    m_Persistence->setProperty(key,value);
    Msg_List neverSeen;
    return m_Persistence->store(neverSeen);
}

bool DBMSrvScd_Persistor::removeJob( int aJobID ) {
    char idbuf[16];
    sprintf(idbuf, m_KeyFormat, aJobID);
    Tools_DynamicUTF8String key(idbuf);

    m_Persistence->removeProperty(key);
    Msg_List neverSeen;
    if( m_Persistence->isEmpty() )
        return m_Persistence->deleteStorage(neverSeen);
    else
        return m_Persistence->store(neverSeen);
}

DBMSrvScd_JobList* DBMSrvScd_Persistor::loadJobs() {
    DBMSrvScd_JobList* vJobs = new DBMSrvScd_JobList();

    Msg_List neverSeen;
    m_Persistence->load(neverSeen);
    Tools_List<Tools_Property*>* vProps = m_Persistence->getAllProperties();
    Tools_List<Tools_Property*>::iterator prop = NULL;
    for( prop = vProps->begin(); prop != vProps->end(); prop++ ) {
        DBMSrvScd_Job* newJob = new DBMSrvScd_Job((*prop)->getValue());
        delete *prop;
        vJobs->push_back(newJob);
    }
    delete vProps;
    return vJobs;
}

const char* const DBMSrvScd_Persistor::m_FileName = "dbmsrvscd.cfg";
// for cn42 file administration
const char* const DBMSrvScd_Persistor::m_FileNameBase = "dbmsrvscd";
const char* const DBMSrvScd_Persistor::m_FileNameExtension = ".cfg";

const char* const DBMSrvScd_Persistor::m_KeyFormat = "%015d";