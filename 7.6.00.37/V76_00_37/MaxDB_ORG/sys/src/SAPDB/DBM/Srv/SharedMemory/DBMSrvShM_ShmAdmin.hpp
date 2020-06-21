/*!
    @file           DBMSrvShM_ShmAdmin.hpp
    @author         MarcW
    @brief          shared memory administration class

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

#if !defined(DBMSrvShM_ShmAdmin_HPP)
#define DBMSrvShM_ShmAdmin_HPP

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Base.hpp"
#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutEventing.hpp"
#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutResourceLock.hpp"
#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutScheduler.hpp"
#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_Persistence.hpp"

/*!
    @brief DBMServer shm communication administration class

    Each process has a singleton of this class. Access to shm from higher level classes
    is only though this singleton. Creation, deletion and modifying of lines of each type
    is supported by according methods for each type of line.
    @see DBMSrvShM_Base
*/
class DBMSrvShM_ShmAdmin {
public:

    const DBMSrvShM_Persistence* getShmPersistence() {return m_Persistence;}

    /*!
        @brief status check
        @return true if the shm ca be used, false otherwise
    */
    bool isInitialized() {return m_Initialized;}

    /*! @brief initialize the shm administration
        
        This creates the shm files on disk in indepdata/wrk or attaches to them, if successful.
        Message list is not empty in case an error occurred.
        @return true if creation was successful and shm can be used, false otherwise
    */
    bool initialize(const char* dbname, DBMSrvMsg_Error& errOut);

    virtual DBMSrvShM_Base* getBase( const char* infoFile, const char* dataFile, DBMSrvMsg_Error& errOut);

    /*!
        @brief detach from the shm

        The base object is destroyed, if it exists, this DBMServer detaches from shared memory
    */
    bool close(DBMSrvMsg_Error& errOut);

    /*!
        @brief remove the shm persistence

        Removes the shared memory persistence. Should only be called after closing shared memory
    */
    bool removePersistence(DBMSrvMsg_Error& errOut);

    int getActualVersion() {
        return m_ShM->getActualVersion();
    }

    int getRequiredVersion() {
        return m_ShM->getRequiredVersion();
    }

    /*!
        @brief lock shm

        @return true, if exclusive access to the shm is guaranteed, else false
    */
    bool lock(DBMSrvMsg_Error& errOut);

    /*!
        @brief unlock shm

        Release exclusive access to the shm.
    */
    void unlock(DBMSrvMsg_Error& errOut);

    // all different line types, must be consistent with max number of lines types!!
    enum LineType {
        LineTypeEventing,
        LineTypeResourceLock,
        LineTypeScheduler,

        LineTypeUninitialized
    };

    /*!
        @brief change shm size

        @see DBMSrvShM_Base.extendMemory()
    */
    bool shift(DBMSrvShM_ShmAdmin::LineType lineType, SAPDB_Byte* firstByte, int offset, DBMSrvMsg_Error& errOut);

    /*! @brief destructor */
    virtual ~DBMSrvShM_ShmAdmin();

    /*
     *************** line type specifics
     */

    /*  schduler, one line at max */
    DBMSrvShM_LineLayoutScheduler* getLineLayoutScheduler(bool createIfNecessary, DBMSrvMsg_Error& errOut);
    bool removeLineLayoutScheduler(DBMSrvMsg_Error& errOut);    

    /* resource locks */
    DBMSrvShM_LineLayoutResourceLock* getLineLayoutResourceLock(const char* resourceName, DBMSrvMsg_Error& errOut);
    bool removeLineLayoutResourceLock(const char* resourceName, DBMSrvMsg_Error& errOut);

    /* eventing */

    /*!
        @brief return pointer to specific line in shm of type eventing

        A pointer to an existing line is returned.
        @param instanceID [IN] id of line to return
        @return pointer to line of type eventing in shm with id index
    */
    DBMSrvShM_LineLayoutEventing* getLineLayoutEventing(int instanceID, DBMSrvMsg_Error& errOut);

    /*!
        @brief get a new line in shm of type eventing

        A new line with the passed filepath is created in shm and a pointer to it is returned.
        @param filePath [IN] filepath to store in the new line
        @return pointer to the new line
    */
    DBMSrvShM_LineLayoutEventing* getLineLayoutEventing(
            bool& visibilityCheck,
            int dispatcherPID,
            SAPDB_Byte commStatus,
            const char* dbmUser,
            const char* nodeName,
            const char* configFilePath,
            const char* logFilePath,
            DBMSrvMsg_Error& errOut);
    /*!
        @brief remove specific line in shm of type eventing

        The line of type eventing is removed from shm if it exists.
        @param instanceID [IN] id of line to remove
    */
    bool removeLineLayoutEventing(int instanceID, DBMSrvMsg_Error& errOut);

    /*!
        @brief get pointer to first line of type eventing in shm

        If no line of type eventing exists, this pointer is not valid!
        @return pointer to first line of type eventing in shm
    */
    const DBMSrvShM_LineLayoutEventing* getFirstLineLayoutEventing(DBMSrvMsg_Error& errOut);
    const DBMSrvShM_LineLayoutEventing* getNextLineLayoutEventing( const DBMSrvShM_LineLayoutEventing* aLayout, DBMSrvMsg_Error& errOut);

    /*!
        @brief get number of existing of type eventing

        @return number of existing of type eventing
    */
    int getExistingLinesEventing(DBMSrvMsg_Error& errOut) {
        if( m_Initialized )
            return m_ShM->getExistingLinesOf(LineTypeEventing, errOut);
        else {
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOTAVAILABLE));
            return -1;
        }
    }

    static DBMSrvShM_ShmAdmin& TheAdmin() {
        return TheInstance;
    }

protected:
    DBMSrvShM_ShmAdmin();
    static DBMSrvShM_ShmAdmin TheInstance;

    DBMSrvShM_Base* m_ShM;
    bool m_Initialized;

    DBMSrvShM_Persistence* m_Persistence;

    /*
     *************** line type specifics
     */

    /* eventing */
    static const int maxAllowedLinesEventing;

    /* exclusive access to resources */
    static const int maxAllowedLinesResourceLock;

    /* communication with the scheduler */
    static const int maxAllowedLinesScheduler;

    LineType m_ScopeMemberCheckOnly;

    // check where a layout is completely in shm (including variable part!!)
    // for each line layout type we need an extra method
    bool isInScope( const DBMSrvShM_LineLayoutScheduler * aLayout, DBMSrvMsg_Error& errOut);
    bool isInScope( const DBMSrvShM_LineLayoutResourceLock * aLayout, DBMSrvMsg_Error& errOut);
    bool isInScope( const DBMSrvShM_LineLayoutEventing * aLayout, LineType lineType, DBMSrvMsg_Error& errOut);

    // called from the isInScope methods
    bool scopeBaseCheck(
        const DBMSrvShM_LineLayoutBase * aLayout,
        const SAPDB_Byte*& shmLastByte);

    bool isInitializedAndOpen(
        DBMSrvMsg_Error & error);
        
    bool openSharedMemoryInfoFile(
        DBMSrvMsg_Error & error);

    virtual void closeSharedMemoryInfoFile();
};

#endif //DBMSrvShM_ShmAdmin_HPP
