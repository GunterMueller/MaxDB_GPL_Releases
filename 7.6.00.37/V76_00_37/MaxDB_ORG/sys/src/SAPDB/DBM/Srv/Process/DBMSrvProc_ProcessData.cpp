/*!
  \file    DBMSrvProc_ProcessData.cpp
  \author  MarcW
  \brief   server process implementation

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


*/

#include "DBM/Srv/Process/DBMSrvProc_ProcessData.hpp"
#include "RunTime/System/RTESys_ProcessState.hpp"

#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#elif defined(LINUX)
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#endif


#if defined(LINUX)
const char* const DBMSrvProc_ProcessData::m_StatusFileName("/proc/%d/status");
const char* const DBMSrvProc_ProcessData::m_VMSizeKey("VmSize");
#endif

DBMSrvProc_ProcessData::DBMSrvProc_ProcessData( RTE_OSPid pid, DBMSrvMsg_Error& errOut )
        : m_Valid(false),
          m_Pid(pid) {
    updateValidFlag();
}

void DBMSrvProc_ProcessData::updateValidFlag() {
    RTESys_ProcessState processState(m_Pid);
    m_Valid = processState.State() == RTESys_ProcessState::Alive;
}

size_t DBMSrvProc_ProcessData::getMemorySize( DBMSrvMsg_Error& errOut ) {
    errOut.ClearMessageList();


#if defined(_WIN32) || defined( LINUX )
    if( !isValid() )
        return false;
    updateValidFlag();
    if( !isValid() ) {
        // create an error message
        errOut.Overrule(DBMSrvMsg_Error(
                SDBMSG_DBMSRV_PROCESSDATA_INVALID,
                Msg_Arg("PID", ToStr(m_Pid, _T_d ))));
        return 0;
    }
#endif

#if defined(_WIN32)

    HANDLE processHandle;
    PROCESS_MEMORY_COUNTERS procMemCounters;
    processHandle = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, m_Pid);

    if( NULL == processHandle ) {
        errOut.Overrule(DBMSrvMsg_Error(
                SDBMSG_DBMSRV_PROCESSDATA_ERR_SIZE,
                Msg_Arg("PID", ToStr(m_Pid, _T_d ))));
        return 0;
    }
    
    size_t memorySize(0);
    if ( GetProcessMemoryInfo( processHandle, &procMemCounters, sizeof(procMemCounters)) )
        memorySize = procMemCounters.PagefileUsage; // this is indeed the VM size
    else {
        errOut.Overrule(DBMSrvMsg_Error(
                SDBMSG_DBMSRV_PROCESSDATA_ERR_SIZE,
                Msg_Arg("PID", ToStr(m_Pid, _T_d ))));
    }
    CloseHandle( processHandle );
    return memorySize;

#elif defined(LINUX)
    char buffer[4096];

    // use buffer for filename
    sprintf(buffer, m_StatusFileName, m_Pid);
	int fd = open(buffer, O_RDONLY);
    if( fd >= 0 ) {
        // we could open the stat file, read it into buffer
        int len = read(fd, buffer, sizeof(buffer)-1);
	    close(fd);
        buffer[len] = '\0';
    }
    else {
        // create an error message
        errOut.Overrule(DBMSrvMsg_Error(
                SDBMSG_DBMSRV_PROCESSDATA_ERR_SIZE,
                Msg_Arg("PID", ToStr(m_Pid, _T_d ))));
        return 0;
    }

    // now find key in status file
    Tools_DynamicUTF8String procStatus(buffer);
    Tools_DynamicUTF8String::BasisElementIndex position(procStatus.Find(m_VMSizeKey));
    position = procStatus.FindFirstOf("0123456789", position);
    return atol(procStatus.SubStr(position).CharPtr()) * 1024;
#else
    errOut.Overrule(DBMSrvMsg_Error(
            SDBMSG_DBMSRV_PROCESSDATA_SIZE_NOTSUPPORTED));
    return 0;
#endif
}

size_t DBMSrvProc_ProcessData::getMemorySizeByAllocation( DBMSrvMsg_Error& errOut ) {

    size_t currentChunkSize(DBMSrvProc_ProcessData_INITIAL_CHUNKSIZE);
    size_t sumChunkSize(0);

    for( int i=0; i < DBMSrvProc_ProcessData_CHUNKS; ++i )
        m_MemoryChunks[i] = NULL;

    int j=0;
    do {
        m_MemoryChunks[j] = DBMSrvProc_ProcessData_NEW char[currentChunkSize];
        if( m_MemoryChunks[j] != NULL ) {
            sumChunkSize += currentChunkSize;
            ++j;
        }
        else {
            currentChunkSize = currentChunkSize / 2;
        }
      
    } while(currentChunkSize != 0 && j < DBMSrvProc_ProcessData_CHUNKS);

    //RTE_ISystem::DoSleep(1000*10); // 10 seconds
    
    // delete all allocated memory
    for( int idel=0; idel < j; ++idel )
        delete[] m_MemoryChunks[idel];

    return sumChunkSize;
}

