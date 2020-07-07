/*!
    @file           DBMSrvShM_Base.hpp
    @author         MarcW
    @brief          Base class for shared memory - declaration

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

#if !defined(DBMSrvShM_Base_HPP)
#define DBMSrvShM_Base_HPP

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_Types.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_HalfSwitcher.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_ParticipantSet.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_Persistence.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "RunTime/System/RTESys_ProcessID.hpp"
#include "ToolsCommon/Tools_Vector.hpp"


// number of chars in file version string (including terminator)
#define DBMSrvShM_Base_SHM_VERSION_LENGTH 32
#define DBMSrvShM_Base_SHM_PLATFORMID_LENGTH 128
#define DBMSrvShM_Base_SHM_VERSIONPREFIX "VERSION: "
#define DBMSrvShM_Base_SHM_PLATFORMIDPREFIX "DBMServer Shared Memory: "

/*!
    @brief DBMServer shared memory communication base class

    This shared memory implementation uses the memory mapped file. The memory
    mapped file is wrapped by this class. Access methods to shm are provided.
    @see heo41.h
*/
class DBMSrvShM_Base {

public:
    /*!
        @brief constructor

        @param shmDir [IN] folder in which shared memory file is to be created
        @param limitedAccess [IN] if true, only the info part of the shared
            memory is accessed, otherwise full access is requested
    */
    DBMSrvShM_Base (const char* infoFile, const char* dataFile, DBMSrvMsg_Error &errOut );

    /*! @brief destructor */
    virtual ~DBMSrvShM_Base();

    bool isValid() { return m_IsValid; }
    
    /*!
        @brief return const pointers to first and last byte of datapart of shm

        Note: If no data part is allocated at the time of the call, both will be zero.
        The values can change. Call this method directly before using them!
    */
    void getMemoryBorders( const SAPDB_Byte*& firstByte, const SAPDB_Byte*& lastByte ) {
        firstByte = m_pSharedMemory;
        lastByte = m_pLastValidAddress;
    }

    /*!
        @brief version check 
        
        @return true, if the current program can work with the shm that's used
    */
    bool versionOkay();

    /*!
        @brief get the version of the shared memory file on disk

        @return string containing the version (caller is responsible for memory)
    */
    int getActualVersion();

    /*!
        @brief get the version of the code (also expected shared memory version)

        @return string containing the version (caller is responsible for memory)
    */
    int getRequiredVersion();

    /*! @brief locks shm 
    
        @return true, if shm could be locked
    */
    bool lockAndAttachMemory(DBMSrvMsg_Error& errOut);

    /*! @brief unlocks shm */
    bool unlockAndDetachMemory(DBMSrvMsg_Error& errOut);

    bool removeMeFromPlayerList(DBMSrvMsg_Error& errOut);
    
    void getPlayerPIDsInto(Tools_Vector<RTE_OSPid>& resultVector);

    /*!
        @brief change shm size

        The size of the shm can be changed dynamically. All memory from firstbyte
        up to the current end of the shm is shifted by newBytes bytes. After calling
        this method, there is newBytes new room in the shared memory (firstByte now points
        to the first now unused byte).

        @param firstByte [IN] pointer to first byte to shift
        @param newBytes [IN] offset (positive: shm is extended, negative: shm shrinks)
        @return true if operation completed successfully, false otherwise
    */
    bool extendMemory(SAPDB_Byte* firstByte, int newBytes, DBMSrvMsg_Error& errOut);

    /*! @brief number of types of lines that can be administered */
    static const int NUMBER_OF_LINETYPES;

    /*!
        @brief return start of a specific line type

        @param lineType [IN] the requested line type
        @return pointer to first line
        @see DBMSrvShM_Admin::LineType
    */
    SAPDB_Byte* getStartOf(int lineType, DBMSrvMsg_Error& errOut);

    /*!
        @brief shift start of a specific line type

        In shm, all lines of a specific type are stored together. The order in which line
        types occur in shm is hardcoded. If a line of a type, which occurs before the lines
        of another type, is inserted/removed/changed in length, the place in shm, where
        the first line the other type starts, changes. Since the start of each line type is stored
        in shm itself, it must be changed there.

        @param lineType [IN] the requested line type
        @param offset [IN] number of bytes that eventing start changes (can be negative)
        @see DBMSrvShM_Admin::LineType
    */
    bool shiftStartOf(int lineType, int offset, DBMSrvMsg_Error& errOut);

    /*!
        @brief get next index of a new line a speicific line type

        For each line type, the index to give to a new line is stored in shm. When creating a
        new line, it is incremented. It is an integer, so overflow should be no
        problem.
        @param lineType [IN] the requested line type
        @return next index to give
        @see DBMSrvShM_Admin::LineType
    */
    int getNextIndexOf(int lineType, DBMSrvMsg_Error& errOut);

    /*!
        @brief set next index of a new line of a specific line type
        
        @param lineType [IN] the requested line type
        @param nextIndex [IN] next index to give
        @see DBMSrvShM_Admin::LineType
    */
    bool setNextIndexOf(int lineType, int nextIndex, DBMSrvMsg_Error& errOut);

    /*!
        @brief get the number of existing lines a specific line type

        For each line type, the number of the existing lines is stored in shm. It changes when
        a new line of this specific type is created/deleted.

        @param lineType [IN] the requested line type
        @return number of existing lines
        @see DBMSrvShM_Admin::LineType
    */
    int getExistingLinesOf(int lineType, DBMSrvMsg_Error& errOut);

    /*!
        @brief set number of existing lines of a specific type
        
        @param lineType [IN] the requested line type
        @param existingLines [IN] new number of existing lines of type eventing
        @see DBMSrvShM_Admin::LineType
    */
    bool setExistingLinesOf(int lineType, int existingLines, DBMSrvMsg_Error& errOut);

private:
    bool lockMemory(bool attach, DBMSrvMsg_Error& errOut);
    bool unlockMemory(bool detach, DBMSrvMsg_Error& errOut);
    void calculateDefaultShMSize();

protected:
    void calculateInfoSize();
    void setPointersIntoInfo();
    bool minimalInit(const char* datafile, DBMSrvMsg_Error& errOut);

    bool cleanupDeadElements( DBMSrvMsg_Error& errOut );
    bool removeZombieLines( int lineType, DBMSrvMsg_Error& errOut );

    DBMSrvShM_Base();

    static const int VERSION;

    RTE_OSPid m_MyPID;
    char* m_DataFile;
    virtual SAPDB_Byte* openSharedMemory(const char* const aShmId, const unsigned int aSize, DBMSrvMsg_Error& errOut);

    /*! @brief set to false if memory could not be allocated */
    bool m_IsValid;


    /*! @brief points to last byte in data part of shared memory, if allocated, NULL otherwise */
    SAPDB_Byte* m_pLastValidAddress;

    /*!
        @brief return a version

        @param current [IN] if true, version of current shared memory in use is
            returned, otherwise the version required by the code is returned
        @return the version
    */
    int getVersion(bool current);

    /*!
        @brief get a pointer to shm

        Necessary for adjusting shm size.
        @return true if shared memory could be allocated, false otherwise
        @see detachMemory()
    */
    bool attachMemory(DBMSrvMsg_Error& errOut);

    /*!
        @brief detach from shm

        Necessary for adjusting shm size.
        @see detachMemory()
    */
    void detachMemory();

    bool isInScope(SAPDB_Byte* dest, SAPDB_Byte* source, SAPDB_Long bytesToCopy, DBMSrvMsg_Error& errOut);

    SAPDB_Byte* m_pSharedMemory;
    unsigned int m_SizeInfoShm;
    SAPDB_Byte* m_pInfoShm;

    RTESync_Spinlock* m_pSpinlock;
    RTESync_Spinlock* m_pBackinglock;

    RTE_OSPid* m_pLockingPid;
    int* m_pLockCounter;
    DBMSrvShM_ParticipantSet* m_pPlayers;

    // our own halfswitcher
    DBMSrvShM_HalfSwitcher m_HalfSwitcher;
    // pointer to the halfswitcher into its extra shm
    DBMSrvShM_SaveType<DBMSrvShM_HalfSwitcher>* m_pSaveHalfSwitcher;
    
    bool m_LengthOperationOpen;

    // pointer to the data part (the stuff directly befor the lines themeselves),
    // to be copied to otehr half...
    SAPDB_Byte* m_pData;

    // shm version administration
    char m_CodeVersion[DBMSrvShM_Base_SHM_VERSION_LENGTH];
    DBMSrvShM_SaveType<char[DBMSrvShM_Base_SHM_VERSION_LENGTH]>* m_pShmVersion;
    char* m_PlatformIDFile;

    /*! @brief maximum size of shm */
    static const size_t SHM_SIZE_MAX;
    static const SAPDB_UInt4 SLEEP_TIME;
    static const char m_PlatformIDCode[DBMSrvShM_Base_SHM_PLATFORMID_LENGTH];

    static bool m_FirstShMAttach;
};

#endif //DBMSrvShM_Base_HPP
