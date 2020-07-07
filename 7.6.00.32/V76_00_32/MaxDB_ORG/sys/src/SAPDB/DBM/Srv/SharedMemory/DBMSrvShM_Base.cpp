/*!
    @file           DBMSrvShM_Base.cpp
    @author         MarcW
    @brief          Base class for shared memory - implementation

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

#include <stdio.h>
#include <time.h>

#include "heo06.h"
#include "heo41.h"

#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_AlignmentHelper.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_Base.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_InitialisationSynchroniser.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutBase.hpp"
#include "RunTime/RTE_ISystem.hpp"
#include "RunTime/System/RTESys_LockArea.hpp"
#include "RunTime/System/RTESys_ProcessState.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "ToolsCommon/CString/ToolsCStr_Str.hpp"
#include "ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp"

const int DBMSrvShM_Base::VERSION(2);

const size_t DBMSrvShM_Base::SHM_SIZE_MAX(1024*1024);
const int DBMSrvShM_Base::NUMBER_OF_LINETYPES(3);
const SAPDB_UInt4 DBMSrvShM_Base::SLEEP_TIME(30000);
bool DBMSrvShM_Base::m_FirstShMAttach(true);

const char DBMSrvShM_Base::m_PlatformIDCode[DBMSrvShM_Base_SHM_PLATFORMID_LENGTH] =
    DBMSrvShM_Base_SHM_PLATFORMIDPREFIX"<SDB_SUBST_ENV:OSSPEC>, <SDB_SUBST_ENV:MACH>"; // 0-bytes are in the remaining elements


bool DBMSrvShM_Base::minimalInit(const char* datafile, DBMSrvMsg_Error& errOut) {
    // code version
    for(int i=0; i<DBMSrvShM_Base_SHM_VERSION_LENGTH; m_CodeVersion[i++]=' ');
    sprintf(m_CodeVersion, DBMSrvShM_Base_SHM_VERSIONPREFIX"%20d", VERSION);

    if( !ToolsCStr_Str::AllocCpy(m_DataFile, datafile, errOut) ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
        return false;
    }
    else
        return true;

}

void DBMSrvShM_Base::calculateInfoSize() {
    DBMSrvShM_AlignmentHelper alignmentHelper;
    m_SizeInfoShm = (unsigned int)(
                    alignmentHelper.alignedSizeOf(sizeof(m_PlatformIDCode)) + 
                    alignmentHelper.alignedSizeOf(sizeof(*m_pShmVersion)) +
                    alignmentHelper.alignedSizeOf(sizeof(RTESys_LockArea)) +
                    alignmentHelper.alignedSizeOf(sizeof(RTESys_LockArea)) +
                    alignmentHelper.alignedSizeOf(sizeof(*m_pLockingPid)) +
                    alignmentHelper.alignedSizeOf(sizeof(*m_pLockCounter)) +
                    alignmentHelper.alignedSizeOf(sizeof(*m_pSaveHalfSwitcher)) +
                    alignmentHelper.alignedSizeOf(sizeof(*m_pPlayers))
                    );
}

void DBMSrvShM_Base::setPointersIntoInfo() {
        
    DBMSrvShM_AlignmentHelper alignmentHelper;
    
    m_PlatformIDFile = (char*) m_pInfoShm;

    m_pShmVersion = (DBMSrvShM_SaveType<char [DBMSrvShM_Base_SHM_VERSION_LENGTH]>*)
                alignmentHelper.nextAlignedAddress(m_PlatformIDFile, sizeof(m_PlatformIDCode));

    RTESys_LockArea* lockArea = (RTESys_LockArea*)alignmentHelper.nextAlignedAddress(m_pShmVersion, sizeof(*m_pShmVersion));
    m_pSpinlock = new RTESync_Spinlock(
                    lockArea->GetLockValue(),
                    false,
                    false);
    
    lockArea = (RTESys_LockArea*)alignmentHelper.nextAlignedAddress(lockArea, sizeof(RTESys_LockArea));
    m_pBackinglock = new RTESync_Spinlock(
                    lockArea->GetLockValue(),
                    false,
                    false);
    
    m_pLockingPid = (RTE_OSPid*)alignmentHelper.nextAlignedAddress(lockArea, sizeof(RTESys_LockArea));

    m_pLockCounter = (int*)alignmentHelper.nextAlignedAddress(m_pLockingPid, sizeof(RTE_OSPid));

    m_pSaveHalfSwitcher = (DBMSrvShM_SaveType<DBMSrvShM_HalfSwitcher>*)
                alignmentHelper.nextAlignedAddress(m_pLockCounter, sizeof(*m_pLockCounter));

    m_pPlayers = (DBMSrvShM_ParticipantSet*)
                alignmentHelper.nextAlignedAddress(m_pSaveHalfSwitcher, sizeof(*m_pSaveHalfSwitcher));
}

DBMSrvShM_Base::DBMSrvShM_Base()
        : m_IsValid(false),
          m_MyPID(RTESys_ProcessID()),
          m_DataFile(NULL),
          m_pShmVersion(NULL),
          m_PlatformIDFile(NULL),
          m_LengthOperationOpen(false),
          m_pSharedMemory(NULL),
          m_pInfoShm(NULL),
          m_SizeInfoShm(0),
          m_pSpinlock(NULL),
          m_pBackinglock(NULL),
          m_pLockingPid(NULL),
          m_pLockCounter(NULL),
          m_pSaveHalfSwitcher(NULL),
          m_pPlayers(NULL),
          m_pData(NULL) {
}        

DBMSrvShM_Base::DBMSrvShM_Base(const char* infoFile, const char* dataFile, DBMSrvMsg_Error &errOut )
        : m_IsValid(false),
          m_MyPID(RTESys_ProcessID()),
          m_DataFile(NULL),
          m_pShmVersion(NULL),
          m_PlatformIDFile(NULL),
          m_LengthOperationOpen(false),
          m_pSharedMemory(NULL),
          m_pInfoShm(NULL),
          m_SizeInfoShm(0),
          m_pSpinlock(NULL),
          m_pBackinglock(NULL),
          m_pLockingPid(NULL),
          m_pLockCounter(NULL),
          m_pSaveHalfSwitcher(NULL),
          m_pPlayers(NULL),
          m_pData(NULL) {

    errOut.ClearMessageList();

    if( !minimalInit(dataFile, errOut) )
        return;

    bool initializingProcess(
        DBMSrvShM_InitialisationSynchroniser::permissionToInitialize(infoFile));

    calculateInfoSize();
    m_pInfoShm = DBMSrvShM_Base::openSharedMemory(infoFile, m_SizeInfoShm, errOut);
    
    if( m_pInfoShm != NULL ) {
        setPointersIntoInfo();

        if( initializingProcess ) {
            // we're creating the shared memory
            strncpy(m_PlatformIDFile, m_PlatformIDCode, DBMSrvShM_Base_SHM_PLATFORMID_LENGTH);
            m_pSpinlock->Unlock();
            m_pBackinglock->Unlock();
            *m_pLockingPid=0;
            m_pShmVersion->setValue(m_CodeVersion);
            m_pPlayers->clear();
            m_pPlayers->insert(m_MyPID);

            // default shm size
            calculateDefaultShMSize(); 

            // now the losers can continue...
            ++(*m_pLockCounter);
        }
        else {
            // shared memory existed earlier (we're reusing it)
            
            // check, if platform fits
            if( 0 != strncmp(m_PlatformIDFile, m_PlatformIDCode, DBMSrvShM_Base_SHM_PLATFORMID_LENGTH) ) {
                // platform does not fit
                char filePlatformID[DBMSrvShM_Base_SHM_PLATFORMID_LENGTH];
                strncpy(filePlatformID, m_PlatformIDFile+strlen(DBMSrvShM_Base_SHM_PLATFORMIDPREFIX), DBMSrvShM_Base_SHM_PLATFORMID_LENGTH);
                filePlatformID[DBMSrvShM_Base_SHM_PLATFORMID_LENGTH-1] = '\0';
                if( 0 == strlen(filePlatformID) ) 
                    strcpy(filePlatformID, "Unknown (0x00)");

                errOut.Overrule(DBMSrvMsg_Error(
                    SDBMSG_DBMSRV_SHMPLATFORMMISMATCH,
                    Msg_Arg(SDBMSGTAG_DBMSRV_SHMPLATFORMMISMATCH__SHMFILEPLATFORM, filePlatformID),
                    Msg_Arg(SDBMSGTAG_DBMSRV_SHMPLATFORMMISMATCH__SHMCODEPLATFORM, (char*)(&m_PlatformIDCode[strlen(DBMSrvShM_Base_SHM_PLATFORMIDPREFIX)]))));
                return;
            }

            // wait until lockcounter is != 0 (otherwise someone else is still
            // initializing shared memory)
            while( (*m_pLockCounter) == 0 )
                RTE_ISystem::DoSleep(100);

            // here we have a fully initialized shared memory
            DBMSrvMsg_Error lockError;
            while( !lockMemory(false, lockError) )
                RTE_ISystem::DoSleep(100);

            int fileVersion(getVersion(true));
            int codeVersion(getVersion(false));

            if( fileVersion < codeVersion ) {
                // code version is newer than the file version
                // migration!

                // update the file version in the shi
                m_pShmVersion->setValue(m_CodeVersion);
                fileVersion = codeVersion;

                // remove the shm
                tsp05_RteFileError rteFileError;
                sqlferasec( m_DataFile, &rteFileError );
                if( rteFileError.sp5fe_result != vf_ok )
                    errOut.Overrule(DBMSrvMsg_RTEError(rteFileError));

                // calculate default size of shm (that might be a new one!)
                calculateDefaultShMSize();

                // existing shared memory is migrated
                // now we can work with the existing shared memory
            }
            else if( fileVersion == codeVersion ) {
                // the versions are identical
                // do nothing
            }
            else {
                // code version is older than the file version
                // we cannot work with the existing shared memory
                errOut.Overrule(DBMSrvMsg_Error(
                        SDBMSG_DBMSRV_SHMVERSIONMISMATCH,
                        Msg_Arg(SDBMSGTAG_DBMSRV_SHMVERSIONMISMATCH__DBMSRV_SHMVERSCURR, fileVersion),
                        Msg_Arg(SDBMSGTAG_DBMSRV_SHMVERSIONMISMATCH__DBMSRV_SHMVERSREQ, codeVersion)));
            }
            if( fileVersion == codeVersion ) {
                if( cleanupDeadElements(errOut) )
                    m_pPlayers->insert(m_MyPID);
            }
            unlockMemory(false, lockError);
            errOut.Overrule(lockError);
        }
        // successful end of constructor...
        m_IsValid = errOut.IsEmpty() && m_pInfoShm!=NULL && m_pSpinlock!=NULL && m_pBackinglock!=NULL;
    }
}

void DBMSrvShM_Base::calculateDefaultShMSize() {
    // for each line type, there are 2 save ints and one save size_t
    DBMSrvShM_HalfSwitcher tmpHS;
    size_t initialSize(0);

    initialSize =  NUMBER_OF_LINETYPES * (sizeof(DBMSrvShM_SaveType<size_t>) +
                                        2 * sizeof(DBMSrvShM_SaveType<int>));

    tmpHS.setMemSize(initialSize);
    tmpHS.setDataSize(initialSize);
    tmpHS.setOffset(0);
    if( m_pSaveHalfSwitcher != NULL )
        m_pSaveHalfSwitcher->setValue(tmpHS);
    m_LengthOperationOpen = false;
    m_HalfSwitcher = tmpHS;
}

bool DBMSrvShM_Base::cleanupDeadElements( DBMSrvMsg_Error& errOut ) {
    // the dead pids and all lines that belong to dead pids are removed here
    
    if( m_FirstShMAttach ) {
        m_FirstShMAttach = false;
        for( int i=0; i<NUMBER_OF_LINETYPES; ++i ) {
            DBMSrvMsg_Error removeError;
            if( !removeZombieLines(i, removeError) ) {
                errOut.Overrule(removeError);
                return false;
            }
        }
        return m_pPlayers->removeZombies(errOut);
    }
    else
        return true;
}

bool DBMSrvShM_Base::removeZombieLines( int lineType, DBMSrvMsg_Error& errOut ) {

    DBMSrvMsg_Error zombieError;
    bool okay(false);

    while( true ) { // no loop, just to be able to write break...

        // attach to data file
        if( !attachMemory(zombieError) )
            break;

        // how many lines of this type do we have?
        int numOfLines(getExistingLinesOf(lineType, zombieError));
        if( !zombieError.IsEmpty() )
            break;

        // 
        DBMSrvShM_LineLayoutBase* lyt(NULL);
        if( numOfLines > 0 ) {
            int linesToCheck(numOfLines);
            // make lyt point to first line of this type
            DBMSrvShM_LineLayoutBase* lyt((DBMSrvShM_LineLayoutBase*)getStartOf(lineType, zombieError));
            // loop on all lines of this type
            while( linesToCheck > 0 ) {

                if( RTESys_ProcessState(lyt->getBGSRVPid()).State() != RTESys_ProcessState::Alive ) {
                    // this line belongs to a dead process: remove it
                    int offset( (int)(-1 * ((SAPDB_Byte*)lyt->getNext()-(SAPDB_Byte*)lyt)) );
                    if( !extendMemory((SAPDB_Byte*)lyt, offset, zombieError) )
                        break;
                    if( !shiftStartOf(lineType, offset, zombieError) )
                        break;
                    --numOfLines;
                    if( !setExistingLinesOf(lineType, numOfLines, zombieError) )
                        break;
                    // we removed the line, make lyt point to first line of this type again
                    lyt = (DBMSrvShM_LineLayoutBase*)getStartOf(lineType, zombieError);
                    linesToCheck = numOfLines;
                }
                else {
                    // line belongs to a living process, check the next one
                    --linesToCheck;
                    lyt = lyt->getNext();
                }
            }
            if( linesToCheck > 0 ) // error
                break;
        }

        // successful cleanup of this lineType
        okay = true;
        break;
    }

    detachMemory();

    if( !okay ) {
        errOut.Overrule(zombieError);
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOCLEANUP));
    }
    return okay;
}

SAPDB_Byte* DBMSrvShM_Base::openSharedMemory(const char* const aShmId, const unsigned int aSize, DBMSrvMsg_Error& errOut) {
    int i(10);

    SAPDB_Byte* theShm = (SAPDB_Byte*) sqlAllocSharedMem(aShmId, aSize );
    // we do this, because  on some OS we must try it several times...
    while( i>0 && theShm==NULL ) {
        // sleep for 50ms maximum
        RTE_ISystem::DoSleep((unsigned int)((double)((50*rand()))/(double)(1.0+RAND_MAX)));
        theShm = (SAPDB_Byte*) sqlAllocSharedMem(aShmId, aSize);
        --i;
    }

    if( theShm == NULL ) {
        char size[32];
        sprintf(size, "%u", aSize);
        errOut.Overrule(
            DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMALLOCFAILED, Msg_Arg("DBMSRV_COMP1", aShmId), Msg_Arg("DBMSRV_COMP2", size)));
    }

    return theShm;
}

bool DBMSrvShM_Base::attachMemory(DBMSrvMsg_Error& errOut) {
    DBMSrvShM_HalfSwitcher tmpHS;
    if( m_LengthOperationOpen ) {
        tmpHS = m_HalfSwitcher;
    }
    else {
        m_pSaveHalfSwitcher->getValue(tmpHS);
    }

    unsigned int memSize = (unsigned int)tmpHS.getMemSize();

    m_pSharedMemory = DBMSrvShM_Base::openSharedMemory(m_DataFile, memSize, errOut);
    m_pData = m_pSharedMemory;
    
    if( m_pSharedMemory == NULL ) {
        m_pLastValidAddress = NULL;
        return false;
    }
    else {
        m_pLastValidAddress = (SAPDB_Byte*)(m_pSharedMemory + (memSize-1));
        return true;
    }
}

void DBMSrvShM_Base::detachMemory() {
    if( m_pSharedMemory != NULL ) {
        unsigned int memSize = (unsigned int)m_HalfSwitcher.getMemSize();

        sqlFreeSharedMem(m_pSharedMemory, memSize);
        m_pSharedMemory = NULL;
        m_pLastValidAddress = NULL;
    }
}

bool DBMSrvShM_Base::removeMeFromPlayerList(DBMSrvMsg_Error& errOut) {
    m_pPlayers->remove(m_MyPID);
    return true;
}

DBMSrvShM_Base::~DBMSrvShM_Base() {
    
    ToolsCStr_Str::Dealloc(m_DataFile);

    if( m_pSpinlock != NULL )
        delete m_pSpinlock;
    if( m_pBackinglock != NULL )
        delete m_pBackinglock;
    
    if( m_pSharedMemory != NULL )
        detachMemory(); // detach from data shm

    if( m_pInfoShm != NULL )
        sqlFreeSharedMem(m_pInfoShm, m_SizeInfoShm); // detach from ino shm
}

void DBMSrvShM_Base::getPlayerPIDsInto(Tools_Vector<RTE_OSPid>& resultVector) {
    m_pPlayers->getPIDsInto(resultVector);
}

// precondition: the process that runs me has locked it

bool DBMSrvShM_Base::extendMemory(SAPDB_Byte* firstByte, int newBytes, DBMSrvMsg_Error& errOut) {

    if( newBytes == 0 ) {
        return true;
    }
    else {
        if( m_LengthOperationOpen ) {
            m_pSaveHalfSwitcher->setValue(m_HalfSwitcher);
            m_LengthOperationOpen = false;
        }

        // hier haben wir also keine length operation offen,
        // d.h. der switcher im shm und der eigene zeigen dasselbe wahre
        size_t bytes((size_t)abs(newBytes));
        size_t copyOffset(firstByte - (m_pData + m_HalfSwitcher.getOffset()));
        size_t oldDataSize(m_HalfSwitcher.getDataSize());
        DBMSrvShM_HalfSwitcher tmpHS;
        tmpHS = m_HalfSwitcher;

        if( !tmpHS.isUpperHalf() ) {
            // copy data to upper half
            if( isInScope(m_pData, m_pData + tmpHS.getOffset(), tmpHS.getDataSize(), errOut) )
                ToolsMeMa_GuardedOperation::MemMove(
                        m_pData,
                        m_pData + tmpHS.getOffset(),
                        tmpHS.getDataSize(),
                        __FILE__,
                        __LINE__);
            else {
                errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMSHIFTERROR,
	                Msg_Arg(SDBMSGTAG_DBMSRV_SHMSHIFTERROR__ADDRESS, (char*)SAPDB_ToString(firstByte, SAPDB_ToStringClass::hex)),
	                Msg_Arg(SDBMSGTAG_DBMSRV_SHMSHIFTERROR__BYTES_NEEDED, newBytes)));
                return false;
            }
            tmpHS.setMemSize(tmpHS.getDataSize());
            tmpHS.setOffset(0);
            m_pSaveHalfSwitcher->setValue(tmpHS);
        }

        // everything is in upper half of shared memory, no length operation is open
        size_t tmpMemSize;
        size_t tmpDataSize;
        size_t tmpOffset;
        if( newBytes > 0 ) {
            tmpMemSize = 2 * tmpHS.getMemSize() + 2 * bytes; // make sure that upper half is never smaller than lower half
            tmpDataSize = tmpHS.getDataSize() + bytes;
            tmpOffset = tmpHS.getDataSize() + bytes;
        }
        else {
            tmpMemSize = 2 * tmpHS.getMemSize() - bytes;
            tmpDataSize = tmpHS.getDataSize() - bytes;
            tmpOffset = tmpHS.getDataSize();
        }

        tmpHS.setMemSize(tmpMemSize);
        tmpHS.setDataSize(tmpDataSize);
        tmpHS.setOffset(tmpOffset);

        detachMemory(); __shm_crashtest_divideByZero(0, __FILE__);
        m_HalfSwitcher = tmpHS;

        m_LengthOperationOpen = true;
        if( !attachMemory(errOut) ) {
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMSHIFTERROR,
	            Msg_Arg(SDBMSGTAG_DBMSRV_SHMSHIFTERROR__ADDRESS, (char*)SAPDB_ToString(firstByte, SAPDB_ToStringClass::hex)),
	            Msg_Arg(SDBMSGTAG_DBMSRV_SHMSHIFTERROR__BYTES_NEEDED, newBytes)));
            return false;
        }
        // now shm is open with new size, copy data to lower half
        SAPDB_Byte* source(NULL);
        SAPDB_Byte* dest(NULL);
        SAPDB_Long bytesToCopy(0);

        // copy the part from start to copyOffset down
        dest = m_pData + tmpOffset;
        source = m_pData;
        bytesToCopy = copyOffset;
        if( bytesToCopy > 0 ) {
            if( isInScope(dest, source, bytesToCopy, errOut) )
                ToolsMeMa_GuardedOperation::MemMove(
                    dest,
                    source,
                    bytesToCopy,
                    __FILE__,
                    __LINE__);
            else {
                errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMSHIFTERROR,
	                Msg_Arg(SDBMSGTAG_DBMSRV_SHMSHIFTERROR__ADDRESS, (char*)SAPDB_ToString(firstByte, SAPDB_ToStringClass::hex)),
	                Msg_Arg(SDBMSGTAG_DBMSRV_SHMSHIFTERROR__BYTES_NEEDED, newBytes)));
                return false;
            }
        }
        if( newBytes > 0 ) {
            // fill in newBytes zero bytes
            dest = m_pData + tmpOffset + copyOffset;
            source = dest; // so we can call the same scope check method
            bytesToCopy = (SAPDB_Long)newBytes;
            if( bytesToCopy > 0 ) {
                if( isInScope(dest, source, bytesToCopy, errOut) )
                    ToolsMeMa_GuardedOperation::MemFill(
                        dest, (SAPDB_Byte)0, bytesToCopy, __FILE__, __LINE__);
                else {
                    errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMSHIFTERROR,
	                    Msg_Arg(SDBMSGTAG_DBMSRV_SHMSHIFTERROR__ADDRESS, (char*)SAPDB_ToString(firstByte, SAPDB_ToStringClass::hex)),
	                    Msg_Arg(SDBMSGTAG_DBMSRV_SHMSHIFTERROR__BYTES_NEEDED, newBytes)));
                    return false;
                }
            }
            // copy the rest behind the zeroes
            dest = m_pData + tmpOffset + copyOffset + bytes;
            source = m_pData + copyOffset;
            bytesToCopy = oldDataSize - copyOffset;
        }
        else {
            // we're shrinking it
            dest = m_pData + tmpOffset + copyOffset;
            source = m_pData + copyOffset + bytes;
            bytesToCopy = oldDataSize - (copyOffset + bytes);
        }
        // copy the tail
        if( bytesToCopy > 0 ) {
            if( isInScope(dest, source, bytesToCopy, errOut) )
                ToolsMeMa_GuardedOperation::MemMove(
                    dest,
                    source,
                    bytesToCopy,
                    __FILE__,
                    __LINE__);
            else {
                errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMSHIFTERROR,
	                Msg_Arg(SDBMSGTAG_DBMSRV_SHMSHIFTERROR__ADDRESS, (char*)SAPDB_ToString(firstByte, _T_p)),
	                Msg_Arg(SDBMSGTAG_DBMSRV_SHMSHIFTERROR__BYTES_NEEDED, newBytes)));
                return false;
            }
        }
        return true;
    }
}

bool DBMSrvShM_Base::isInScope(SAPDB_Byte* dest, SAPDB_Byte* source, SAPDB_Long bytesToCopy, DBMSrvMsg_Error& errOut) {
    if( dest < m_pData ||
        dest > m_pLastValidAddress ||
        dest + (size_t)bytesToCopy-1 > m_pLastValidAddress ||
        source < m_pData ||
        source > m_pLastValidAddress ||
        source + (size_t)bytesToCopy-1 > m_pLastValidAddress ) {
        
        SAPDB_ToStringClass destTag((void*)dest, _T_p);
        SAPDB_ToStringClass sourceTag((void*)source, _T_p);

        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMMEMORYACCESS,
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMMEMORYACCESS__ADDRESS, (const char*)destTag),
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMMEMORYACCESS__ADDRESS1, (const char*)sourceTag),
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMMEMORYACCESS__BYTE_SIZE, bytesToCopy)));

        return false;
    }
    else
        return true;
}

bool DBMSrvShM_Base::lockAndAttachMemory(DBMSrvMsg_Error& errOut) {
    return lockMemory( true, errOut);    
}

bool DBMSrvShM_Base::lockMemory(bool attach, DBMSrvMsg_Error& errOut) {

    bool viaBackingLock(false);
    bool takeOver(false);
    int currentCount(0);
    RTE_OSPid localLockerPIDCopy(RTE_UNDEF_OSPID);

    SAPDB_Bool  bHaveNoLock(m_pSpinlock->TryLock());

    if( bHaveNoLock ) {
        localLockerPIDCopy = *m_pLockingPid;
        // we could not get lock
        if( localLockerPIDCopy == 0 ) {

            // somebody has the lock but there is no pid of him
            // maybe he is trying to write the pid, give him a chance

            // save the counter
            currentCount = *m_pLockCounter;

            // try to get the lock until SLEEP_TIME will be reached
            SAPDB_UInt4 nSleepTime       = 0;
            SAPDB_UInt4 nSleepIntervall  = 128;
            SAPDB_Bool  bHaveNoLock      = true;
            while ((nSleepTime < SLEEP_TIME) && bHaveNoLock) {
               RTE_ISystem::DoSleep(nSleepIntervall);
               nSleepTime += nSleepIntervall;
               nSleepIntervall *= 2;
               bHaveNoLock = m_pSpinlock->TryLock();
            } // end while

            localLockerPIDCopy = *m_pLockingPid;
            if( bHaveNoLock ) {
                // even after waiting, we could not get the lock
                takeOver = (localLockerPIDCopy == 0 && currentCount == *m_pLockCounter);
                // takeOver true means the locking process still could not
                // write its pid to shm
            }
            else {
                // now we could get the lock, okay
            }
        }
        else {
            // there is a pid, if process is dead, take over
            RTESys_ProcessState procState(localLockerPIDCopy);
            takeOver = procState.State() != RTESys_ProcessState::Alive;
        }

        // check if locker is still alive
        if( bHaveNoLock && !takeOver ) {
            // there is still action on the shared memory (either lockcounter changed or pid is not 0)
            m_pSpinlock->Lock();
            __shm_crashtest_divideByZero(2, __FILE__);
        }
        else if( bHaveNoLock && takeOver ) {
            // locker is not alive or stuck while entering its pid
            // so we try to take over the session
            currentCount = *m_pLockCounter;
            m_pBackinglock->Lock();
///////////// crashing between here...
            __shm_crashtest_divideByZero(10, __FILE__);
            if( currentCount != *m_pLockCounter ) {
                // someone else got the lock before me, I give up
                m_pBackinglock->Unlock();
                errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOLOCK));
                return false;
            }
            else {
                // okay, nobody else managed to get the lock, we take the shm
                viaBackingLock = true;
            }
        }
        else {
            // we have the lock, do nothing
        }
    }

    // here I am the only one

    // increment lock counter, never let it be zero (initialization sync would fail)
    (*m_pLockCounter) += (((*m_pLockCounter)+1) == 0)?2:1;

    // copy our pid to shm
    __shm_crashtest_divideByZero(9, __FILE__);
    *m_pLockingPid = RTESys_ProcessID();
    if( viaBackingLock ) {
        m_pBackinglock->Unlock();
///////////////// ... and here can cause the shm to remain locked!!
    }
    __shm_crashtest_divideByZero(1, __FILE__);
    m_pSaveHalfSwitcher->getValue(m_HalfSwitcher);
    
    // attach memory if requested
    if( attach ) {
        bool out(attachMemory(errOut));
        if( !out )
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOLOCK));
        return out;
    }
    else
        return true;
}

bool DBMSrvShM_Base::unlockAndDetachMemory(DBMSrvMsg_Error& errOut) {
    return unlockMemory( true, errOut);
}

bool DBMSrvShM_Base::unlockMemory(bool detach, DBMSrvMsg_Error& errOut) {
    
    if( *m_pLockingPid != m_MyPID ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMWRONGUNLOCKPID,
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMWRONGUNLOCKPID__PID, *m_pLockingPid)));
        return false;
    }

    // if size changed, persist switcher
    if( m_LengthOperationOpen ) {
        m_pSaveHalfSwitcher->setValue(m_HalfSwitcher);
        m_LengthOperationOpen = false;
    }

    if( detach )
        // detach from memory
        detachMemory();

    // remove my pid
    *m_pLockingPid = 0;
    __shm_crashtest_divideByZero(4, __FILE__);
    m_pSpinlock->Unlock();
    return true;
}

bool DBMSrvShM_Base::versionOkay() {
    return getActualVersion() == getRequiredVersion();
}

int DBMSrvShM_Base::getActualVersion() {
    return getVersion(true);
}

int DBMSrvShM_Base::getRequiredVersion() {
    return getVersion(false);
}

int DBMSrvShM_Base::getVersion(bool current) {
    if( current ) {
        char buf[DBMSrvShM_Base_SHM_VERSION_LENGTH];
        m_pShmVersion->getValue(buf);
        size_t index(strlen(DBMSrvShM_Base_SHM_VERSIONPREFIX));
        return atoi((char*)&buf[index]);
    }
    else
        return VERSION;
}

SAPDB_Byte* DBMSrvShM_Base::getStartOf(int lineType, DBMSrvMsg_Error& errOut) {

    // pointer to place where start is stored (as offset)
    DBMSrvShM_SaveType<size_t>* tmpSaveSO((DBMSrvShM_SaveType<size_t>*)
       (m_pData +
        m_HalfSwitcher.getOffset() + 
        lineType * sizeof(DBMSrvShM_SaveType<size_t>) +
        lineType * 2 * sizeof(DBMSrvShM_SaveType<int>)
        ));

    // check whether we may access this pointer
    SAPDB_Byte* firstByte = (SAPDB_Byte*)tmpSaveSO;
    SAPDB_Byte* lastByte = ((SAPDB_Byte*)tmpSaveSO) + sizeof(*tmpSaveSO) - 1;
    if( firstByte < m_pData || lastByte >= m_pData + m_HalfSwitcher.getMemSize() ) {
        SAPDB_ToStringClass address((void*)firstByte, _T_p);
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMSOADDRESS,
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMSOADDRESS__ADDRESS, (const char*)address),
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMSOADDRESS__SHMLINETYPE, lineType)));
        return NULL;
    }
    

    size_t startOf(0);
    tmpSaveSO->getValue(startOf);
    // return the absolute position in shm plus stored offset
    return m_pData +
           m_HalfSwitcher.getOffset() + 
           NUMBER_OF_LINETYPES * sizeof(DBMSrvShM_SaveType<size_t>) +
           NUMBER_OF_LINETYPES * 2 * sizeof(DBMSrvShM_SaveType<int>) +
           startOf;
}

bool DBMSrvShM_Base::shiftStartOf(int lineType, int offset, DBMSrvMsg_Error& errOut) {

    // shift all line types with higher!! index
    // (stored after!! the lineType)
    SAPDB_Byte* shmEnd(m_pData + m_HalfSwitcher.getMemSize());

    for( int i = lineType + 1; i < NUMBER_OF_LINETYPES; i++ ) {
        DBMSrvShM_SaveType<size_t>* tmpSaveSO((DBMSrvShM_SaveType<size_t>*)
        (m_pData +
            m_HalfSwitcher.getOffset() + 
            i * sizeof(DBMSrvShM_SaveType<size_t>) +
            i * 2 * sizeof(DBMSrvShM_SaveType<int>)
        ));
        
        // check whether we may access this pointer
        SAPDB_Byte* firstByte = (SAPDB_Byte*)tmpSaveSO;
        SAPDB_Byte* lastByte = ((SAPDB_Byte*)tmpSaveSO) + sizeof(*tmpSaveSO) - 1;
        if( firstByte < m_pData || lastByte >= shmEnd ) {
            SAPDB_ToStringClass address((void*)firstByte, _T_p);
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMSOADDRESS,
	            Msg_Arg(SDBMSGTAG_DBMSRV_SHMSOADDRESS__ADDRESS, (const char*)address),
	            Msg_Arg(SDBMSGTAG_DBMSRV_SHMSOADDRESS__SHMLINETYPE, lineType)));
            return false;
        }

        size_t tmpSO;
        tmpSaveSO->getValue(tmpSO);
        tmpSO = (size_t)(((int)tmpSO) + offset);
        tmpSaveSO->setValue(tmpSO);
    }
    return true;
}

int DBMSrvShM_Base::getNextIndexOf(int lineType, DBMSrvMsg_Error& errOut) {

    int nextIndexOf(-1);
    // pointer to place where index is stored
    DBMSrvShM_SaveType<int>* saveNI((DBMSrvShM_SaveType<int>*)
       (m_pData +
        m_HalfSwitcher.getOffset() + 
        lineType * sizeof(DBMSrvShM_SaveType<size_t>) +
        lineType * 2 * sizeof(DBMSrvShM_SaveType<int>) + 
        sizeof(DBMSrvShM_SaveType<size_t>) + 
        sizeof(DBMSrvShM_SaveType<int>)
        ));

    // check whether we may access this pointer
    SAPDB_Byte* firstByte = (SAPDB_Byte*)saveNI;
    SAPDB_Byte* lastByte = ((SAPDB_Byte*)saveNI) + sizeof(*saveNI) - 1;
    if( firstByte < m_pData || lastByte >= m_pData + m_HalfSwitcher.getMemSize() ) {
        SAPDB_ToStringClass address((void*)firstByte, _T_p);
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNIADDRESS,
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMNIADDRESS__ADDRESS, (const char*)address),
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMNIADDRESS__SHMLINETYPE, lineType)));
        return -1;
    }

    saveNI->getValue(nextIndexOf);
    return nextIndexOf;
}

bool DBMSrvShM_Base::setNextIndexOf(int lineType, int nextIndex, DBMSrvMsg_Error& errOut) {

    if( nextIndex<0 )
        nextIndex = 0;

    // pointer to place where index is stored
    DBMSrvShM_SaveType<int>* saveNI((DBMSrvShM_SaveType<int>*)
       (m_pData +
        m_HalfSwitcher.getOffset() + 
        lineType * sizeof(DBMSrvShM_SaveType<size_t>) +
        lineType * 2 * sizeof(DBMSrvShM_SaveType<int>) + 
        sizeof(DBMSrvShM_SaveType<size_t>) + 
        sizeof(DBMSrvShM_SaveType<int>)
        ));

    // check whether we may access this pointer
    SAPDB_Byte* firstByte = (SAPDB_Byte*)saveNI;
    SAPDB_Byte* lastByte = ((SAPDB_Byte*)saveNI) + sizeof(*saveNI) - 1;
    if( firstByte < m_pData || lastByte >= m_pData + m_HalfSwitcher.getMemSize() ) {
        SAPDB_ToStringClass address((void*)firstByte, _T_p);
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNIADDRESS,
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMNIADDRESS__ADDRESS, (const char*)address),
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMNIADDRESS__SHMLINETYPE, lineType)));
        return false;
    }

    saveNI->setValue(nextIndex);
    return true;
}

int DBMSrvShM_Base::getExistingLinesOf(int lineType, DBMSrvMsg_Error& errOut) {
    int exLines(-1);
    // pointer to place where number is stored
    DBMSrvShM_SaveType<int>* saveEL((DBMSrvShM_SaveType<int>*)
       (m_pData +
        m_HalfSwitcher.getOffset() + 
        lineType * sizeof(DBMSrvShM_SaveType<size_t>) +
        lineType * 2 * sizeof(DBMSrvShM_SaveType<int>) + 
        sizeof(DBMSrvShM_SaveType<size_t>)
        ));

    // check whether we may access this pointer
    SAPDB_Byte* firstByte = (SAPDB_Byte*)saveEL;
    SAPDB_Byte* lastByte = ((SAPDB_Byte*)saveEL) + sizeof(*saveEL) - 1;
    if( firstByte < m_pData || lastByte >= m_pData + m_HalfSwitcher.getMemSize() ) {
        SAPDB_ToStringClass address((void*)firstByte, _T_p);
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMEXADDRESS,
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMEXADDRESS__ADDRESS, (const char*)address),
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMEXADDRESS__SHMLINETYPE, lineType)));
        return -1;
    }

    saveEL->getValue(exLines);
    return exLines;
}

bool DBMSrvShM_Base::setExistingLinesOf(int lineType, int existingLines, DBMSrvMsg_Error& errOut) {

    // pointer to place where number is stored
    DBMSrvShM_SaveType<int>* saveEL((DBMSrvShM_SaveType<int>*)
       (m_pData +
        m_HalfSwitcher.getOffset() + 
        lineType * sizeof(DBMSrvShM_SaveType<size_t>) +
        lineType * 2 * sizeof(DBMSrvShM_SaveType<int>) + 
        sizeof(DBMSrvShM_SaveType<size_t>)
        ));
    
    // check whether we may access this pointer
    SAPDB_Byte* firstByte = (SAPDB_Byte*)saveEL;
    SAPDB_Byte* lastByte = ((SAPDB_Byte*)saveEL) + sizeof(*saveEL) - 1;
    if( firstByte < m_pData || lastByte >= m_pData + m_HalfSwitcher.getMemSize() ) {
        SAPDB_ToStringClass address((void*)firstByte, _T_p);
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMEXADDRESS,
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMEXADDRESS__ADDRESS, (const char*)address),
	        Msg_Arg(SDBMSGTAG_DBMSRV_SHMEXADDRESS__SHMLINETYPE, lineType)));
        return false;
    }

    saveEL->setValue(existingLines);
    return true;
}
