/*!
    @file           DBMSrvShM_LineIteratorEventing.hpp
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

#if !defined(DBMSrvShM_LineIteratorEventing_HPP)
#define DBMSrvShM_LineIteratorEventing_HPP

#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorEventing.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutEventing.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_ShmAdmin.hpp"
#include "ToolsCommon/Tools_Vector.hpp"

class DBMSrvShM_LineIteratorEventing {
public:
    DBMSrvShM_LineIteratorEventing(DBMSrvMsg_Error& errOut, bool iterateInternals=false);
    ~DBMSrvShM_LineIteratorEventing();

    bool isValid() const {return m_Valid;}
    bool hasNext() const;
    void next() {++m_LineIterator;}

    int getInstanceID();
    int getDispatcherProcessID();
    DBMSrvShM_LineAccessorEventing::InternalCommStatus getCommunicationStatus();
    const char* getDBMUser();
    const char* getNodeName();
    const char* getConfigFilePath();
    const char* getLogFilePath();

private:
    class LineData {
    public:
        LineData(
                int instanceID,
                int dispatcherProcessID,
                DBMSrvShM_LineAccessorEventing::InternalCommStatus communicationStatus,
                char* dbmUser,
                char* nodeName,
                char* configFilePath,
                char* logFilePath);
        ~LineData();

        int m_InstanceID;
        int m_DispatcherProcessID;
        DBMSrvShM_LineAccessorEventing::InternalCommStatus m_CommunicationStatus;
        char* m_DBMUser;
        char* m_NodeName;
        char* m_ConfigFilePath;
        char* m_LogFilePath;
    };

    bool m_IterateInternals;
    bool m_Valid;
    Tools_Vector<LineData*> m_EventingLines;
    Tools_Vector<LineData*>::iterator m_LineIterator;
};

inline bool DBMSrvShM_LineIteratorEventing::hasNext() const {
    return m_EventingLines.size() > 0 && m_LineIterator != m_EventingLines.end();
}

inline DBMSrvShM_LineIteratorEventing::~DBMSrvShM_LineIteratorEventing() {
    // clear the vector
    for( m_LineIterator = m_EventingLines.begin(); m_LineIterator != m_EventingLines.end(); ++m_LineIterator )
        delete *m_LineIterator;
}

#endif //DBMSrvShM_LineIteratorEventing_HPP
