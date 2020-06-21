/*!
    @file           DBMSrvShM_BaseEnumerator.cpp
    @author         MarcW
    @brief          Base class for shared memory - for unlocking

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

#include "heo41.h"

#include "DBM/Srv/SharedMemory/DBMSrvShM_AlignmentHelper.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_BaseEnumerator.hpp"
#include "RunTime/System/RTESys_LockArea.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"

DBMSrvShM_BaseEnumerator* DBMSrvShM_BaseEnumerator::getInstance(const char* infoFile, const char* dataFile, DBMSrvMsg_Error& errOut) {

    DBMSrvShM_BaseEnumerator* newInstance(NULL);
    newInstance = new DBMSrvShM_BaseEnumerator();
    if( newInstance == NULL )
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
    else
        newInstance->deferredConstructor(infoFile, dataFile, errOut);
    return newInstance;
}

DBMSrvShM_BaseEnumerator::DBMSrvShM_BaseEnumerator()
        : DBMSrvShM_Base() {
}

bool DBMSrvShM_BaseEnumerator::deferredConstructor( const char* infoFile, const char* dataFile, DBMSrvMsg_Error &errOut ) {

    unsigned int memSize(0);
    int tryCount(100);
    errOut.ClearMessageList();

    if( !minimalInit(dataFile, errOut) )
        return false;

    calculateInfoSize();

    /*
        We need to use a read memory barrier here to force all processors to dump their respective caches to memory.
        In "normal" usage of the shared memory, this is implicitly done by the spinlocks. But here we try to access
        the shared memory in an unlocked fashion, so we need to force the dumping manually to be sure to read
        the must current data from shared memory.
        
        We do the following:
        1. create a local copy of the info
        2. create a local copy of the data
        3. perform a memory read barrier
        4. create another local copy of the info
        
        Now we compare the two copies of the info file. If they are identical, we have a consistent snapshot of the shared memory.
        This works because changes in data imply preceding changes in info.    
    */

    SAPDB_Byte* infoDouble(NULL);

    while( tryCount > 0 ) {
        
        // create local copies of memory
        m_pInfoShm = openSharedMemory(infoFile, m_SizeInfoShm, errOut);
        if( !errOut.IsEmpty() )
            break;

        setPointersIntoInfo();
        DBMSrvShM_HalfSwitcher halfSwitcher;
        m_pSaveHalfSwitcher->getValue(halfSwitcher);
        memSize = (unsigned int)halfSwitcher.getMemSize();
        m_pSharedMemory = openSharedMemory(dataFile, memSize, errOut);
        if( !errOut.IsEmpty() )
            break;

        RTESys_ReadMemoryBarrier();
        infoDouble = openSharedMemory(infoFile, m_SizeInfoShm, errOut);
        if( !errOut.IsEmpty() )
            break;
        if( 0 != memcmp(m_pInfoShm, infoDouble, m_SizeInfoShm) ) // something changed (either in info or data file)
            --tryCount;
        else
            tryCount = -1;

        if( infoDouble != NULL ) {
            delete infoDouble;
            infoDouble = NULL;
        }
    }

    if( !errOut.IsEmpty() )
        return false;


    while( tryCount == 0 ) { // not a real loop, just to be able to use break statement

        // we never got equal memory parts, there is traffic, shm seems to work, so take part in the game!
        m_pInfoShm = DBMSrvShM_Base::openSharedMemory(infoFile, m_SizeInfoShm, errOut);
        if( !errOut.IsEmpty() )
            break;
        
        setPointersIntoInfo();
        if( lockAndAttachMemory(errOut) ) {
            // lockMemory sets m_pLastValidAddress and m_pSharedMemory, because it calls attach
            // copy the info shared memory
            SAPDB_Byte* localMemoryInfo = new SAPDB_Byte[m_SizeInfoShm];
            memcpy(localMemoryInfo, m_pInfoShm, (size_t)m_SizeInfoShm);

            // copy the data shared memory
            memSize = (unsigned int)((m_pLastValidAddress  - m_pSharedMemory) + 1);
            SAPDB_Byte* localMemoryData = new SAPDB_Byte[memSize];
            memcpy(localMemoryData, m_pSharedMemory, (size_t)memSize);
            unlockAndDetachMemory(errOut); // also frees shared memory
            if( !errOut.IsEmpty() )
                break;
            
            // set info to local copy
            sqlFreeSharedMem(m_pInfoShm, m_SizeInfoShm);
            m_pInfoShm = localMemoryInfo;
            setPointersIntoInfo();
            
            // set data to local copy
            m_pSharedMemory = localMemoryData;
        }
        break;
    }

    if( errOut.IsEmpty() ) {
        m_pData = m_pSharedMemory;
        m_pLastValidAddress = (SAPDB_Byte*)(m_pSharedMemory + (memSize-1));

        m_pSaveHalfSwitcher->getValue(m_HalfSwitcher);

        m_IsValid = true;

        // now we have our own copy of the shared memory and all methods (also from base class) operate on it
    }
    return m_IsValid;
}

DBMSrvShM_BaseEnumerator::~DBMSrvShM_BaseEnumerator() {
    m_IsValid = false;
    delete m_pSharedMemory;
    delete m_pInfoShm;
    m_pSharedMemory = NULL;
    m_pInfoShm = NULL;
}

SAPDB_Byte* DBMSrvShM_BaseEnumerator::openSharedMemory(const char* const aShmId, const unsigned int aSize, DBMSrvMsg_Error& errOut) {
    SAPDB_Byte* localMemory(NULL);
    SAPDB_Byte* sharedMemory = DBMSrvShM_Base::openSharedMemory(aShmId, aSize, errOut);
    if( sharedMemory != NULL ) {
        localMemory = new SAPDB_Byte[aSize];
        if( localMemory == NULL )
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
        else
            memcpy(localMemory, sharedMemory, (size_t)aSize);
        sqlFreeSharedMem(sharedMemory, aSize);
    }
    return localMemory;
}

bool DBMSrvShM_BaseEnumerator::getPlatformIdCode(const char*& platformIDOut) {
    platformIDOut = m_PlatformIDCode;
    return true;
}

bool DBMSrvShM_BaseEnumerator::getPlatformIdFile(char*& platformIDOut, DBMSrvMsg_Error& errOut) {
    
    platformIDOut = new char[DBMSrvShM_Base_SHM_PLATFORMID_LENGTH+1];
    if( platformIDOut == NULL ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
        return false;
    }
    
    char* currentChar(m_PlatformIDFile);
    int currentPos(0);
    
    // copy the chars
    while( pointsIntoInfo(currentChar) && currentPos<DBMSrvShM_Base_SHM_PLATFORMID_LENGTH ) {
        platformIDOut[currentPos] = *currentChar;
        ++currentPos;
        ++currentChar;
    }
    
    // fill rest with 0, we have at least one 0
    while( currentPos<=DBMSrvShM_Base_SHM_PLATFORMID_LENGTH ) {
        platformIDOut[currentPos] = '\0';
        ++currentPos;
    }
    return true;
}

bool DBMSrvShM_BaseEnumerator::getVersionFile(int& versionOut, DBMSrvMsg_Error& errOut) {
    
    SAPDB_Byte* lastByte = (SAPDB_Byte*)m_pShmVersion;
    lastByte = lastByte + sizeof(*m_pShmVersion) - 1;
    if( pointsIntoInfo(m_pShmVersion) && pointsIntoInfo(lastByte) ) {
        versionOut = getRequiredVersion();
        return true;
    }
    else
        return false;
}

bool DBMSrvShM_BaseEnumerator::getVersionCode(int& versionOut) {
    
    versionOut = getActualVersion();
    return true;
}

bool DBMSrvShM_BaseEnumerator::getLockState( bool normalLock, bool& locked, DBMSrvMsg_Error& errOut) {

    SAPDB_Byte* start(NULL);
    SAPDB_Byte* end(NULL);
    RTESync_Spinlock* lockTest(NULL);
    RTESys_LockArea* lockArea(NULL);
    DBMSrvShM_AlignmentHelper alignmentHelper;

    start = (SAPDB_Byte*)alignmentHelper.nextAlignedAddress(m_pShmVersion, sizeof(*m_pShmVersion));
    if( !normalLock )
        start = (SAPDB_Byte*)alignmentHelper.nextAlignedAddress(start, sizeof(RTESys_LockArea));
    
    end = start + sizeof(RTESys_LockArea);
    if( pointsIntoInfo(start) && pointsIntoInfo(end) ) {
        lockArea = (RTESys_LockArea*) start;
        lockTest = new RTESync_Spinlock(lockArea->GetLockValue(), false, false);
        // this is actually the check
        locked = lockTest->TryLock();
        delete lockTest;
        return true;
    }
    else
        return false;
}

bool DBMSrvShM_BaseEnumerator::getLockingPid( RTE_OSPid& lockingPid, DBMSrvMsg_Error& errOut) {

    SAPDB_Byte* lastByte = (SAPDB_Byte*)m_pLockingPid;
    lastByte = lastByte + sizeof(RTE_OSPid) - 1;
    if( pointsIntoInfo(m_pLockingPid) && pointsIntoInfo(lastByte) ) {
        lockingPid = *m_pLockingPid;
        return true;
    }
    else 
        return false;
}

bool DBMSrvShM_BaseEnumerator::getLockCounter( int& lockCounter, DBMSrvMsg_Error& errOut) {

    SAPDB_Byte* lastByte = (SAPDB_Byte*)m_pLockCounter;
    lastByte = lastByte + sizeof(int) - 1;
    if( pointsIntoInfo(m_pLockCounter) && pointsIntoInfo(lastByte) ) {
        lockCounter = *m_pLockCounter;
        return true;
    }
    else 
        return false;
}

bool DBMSrvShM_BaseEnumerator::getHalfSwitcher( DBMSrvShM_HalfSwitcher& switcherOut, DBMSrvMsg_Error& errOut) {

    SAPDB_Byte* lastByte = (SAPDB_Byte*)m_pSaveHalfSwitcher;
    lastByte = lastByte + sizeof(*m_pSaveHalfSwitcher) - 1;
    if( pointsIntoInfo(m_pSaveHalfSwitcher) && pointsIntoInfo(lastByte) ) {
        m_pSaveHalfSwitcher->getValue(switcherOut);
        return true;
    }
    else 
        return false;
}

bool DBMSrvShM_BaseEnumerator::getParticipantSet(DBMSrvShM_ParticipantSet& setOut, DBMSrvMsg_Error& errOut) {

    SAPDB_Byte* lastByte = (SAPDB_Byte*)m_pPlayers;
    lastByte = lastByte + sizeof(*m_pPlayers) - 1;
    if( pointsIntoInfo(m_pPlayers) && pointsIntoInfo(lastByte) ) {
        setOut = *m_pPlayers;
        return true;
    }
    else 
        return false;
}

bool DBMSrvShM_BaseEnumerator::pointsIntoInfo( void* infoPointer ) {
    SAPDB_Byte* pointer = (SAPDB_Byte*)infoPointer;
    return (m_pInfoShm <= pointer) && (pointer < m_pInfoShm + m_SizeInfoShm);
}


//errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOENUMERATOR));
