/*!
    @file           DBMSrvShM_LineAccessorEventing.hpp
    @author         MarcW
    @brief          line accessor class for lines of type eventing

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

#if !defined(DBMSrvShM_LineAccessorEventing_HPP)
#define DBMSrvShM_LineAccessorEventing_HPP

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorBase.hpp"

/*!
    @brief line accessor class for lines of type eventing

    It is possible to create instances that give access to line layouts that live in
    local memory. These instances of course don't lock/unlock the shm in their accessor
    methods.

    Methods that access information that is stored in members that are declared in DBMSrvShM_LineLayoutBase
    cannot be implemented in DBMSrvShM_LineAccessorBase, because it is necessary to know
    on which line types it is operated on and virtual methods are no possible.
*/
class DBMSrvShM_LineAccessorEventing : public DBMSrvShM_LineAccessorBase {

public:
    enum InternalCommStatus {
        LOCAL,        // must be the first in this enumeration
        LOCALREMOTE
    };

    enum VisibilityStatus {
        INTERNAL,        // must be the first in this enumeration
        OPEN
    };


    /*!
        @brief create an accessor for an exisiting line

        The line to which this accessor gives access must already exist in shm. If not,
        subsequent calls to accessor methods with not work.
        @param index [IN] inde xof line in shm
    */
    DBMSrvShM_LineAccessorEventing(const int instanceID, DBMSrvMsg_Error& errOut);

    /*!
        @brief create an accessor for an not exisiting line

        The line to which this accessor gives access is created in shm.
        @param filePath [IN] path to configuration file for event dispatcher
    */
    DBMSrvShM_LineAccessorEventing(
            const char* dispatcherPID,
            const char* visibilityStatus,
            const char* commStatus,
            const char* dbmUser,
            const char* nodeName,
            const char* configFilePath,
            const char* logFilepath,
            DBMSrvMsg_Error& errOut);

    /*! @brief cleanup */
    ~DBMSrvShM_LineAccessorEventing() {};

    /*! @brief set pid of background process */
    bool setBGSRVPid(DBMSrvMsg_Error& errOut);

    /*!
        get background process' id from layout
        @return background process' id from layout
    */
    bool getBGSRVPid(RTE_OSPid& pid, DBMSrvMsg_Error& errOut);

    /*! @brief remove any process id from layout */
    bool removeBGSRVPid(DBMSrvMsg_Error& errOut);

    int getInstanceID() const {return m_InstanceID;}
    bool getCommunicationStatus(InternalCommStatus& commStatus, DBMSrvMsg_Error& errOut);
    bool setCommunicationStatus(const InternalCommStatus newcommStatus, DBMSrvMsg_Error& errOut);
    bool getVisibilityStatus(VisibilityStatus& visibilityStatus, DBMSrvMsg_Error& errOut);
    bool setVisibilityStatus(const VisibilityStatus newVisibilityStatus, DBMSrvMsg_Error& errOut);
    bool getDBMUser(char*& dbmUser, DBMSrvMsg_Error& errOut);
    bool setDBMUser(const char* newDbmUser, DBMSrvMsg_Error& errOut);
    bool getNodeName(char*& nodeName, DBMSrvMsg_Error& errOut);
    bool setNodeName(const char* newNodeName, DBMSrvMsg_Error& errOut);
    bool getConfigFilePath(char*& configFilePath, DBMSrvMsg_Error& errOut);
    bool setConfigFilePath(const char* newConfigFilePath, DBMSrvMsg_Error& errOut);
    bool getLogFilePath(char*& logFilePath, DBMSrvMsg_Error& errOut);
    bool setLogFilePath(const char* newLogFilePath, DBMSrvMsg_Error& errOut);

    bool surplusHidden() {return m_VisibiltyCheck;}

    // errOut must be empty, otherwise return value makes no sense
    bool existsEventing(DBMSrvMsg_Error& errOut);

    /*! @brief set cancel flag */
    bool doCancel(DBMSrvMsg_Error& errOut);

    /*! @brief unset cancel flag */
    bool unCancel(DBMSrvMsg_Error& errOut);

    /*!
        @brief check cancel flag

        @return true is cancel flag is set
    */
    bool isCancelled(bool& iscanc, DBMSrvMsg_Error& errOut);

    /*! @brief remove the eventing line in shm that this accessor represents */
    bool removeLineEventing(DBMSrvMsg_Error& errOut);

private:
    int m_InstanceID;
    bool m_VisibiltyCheck;
};

#endif //DBMSrvShM_LineAccessorEventing_HPP
