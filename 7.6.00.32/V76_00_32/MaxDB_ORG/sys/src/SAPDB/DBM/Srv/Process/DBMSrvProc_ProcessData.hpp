/*!
  \file    DBMSrvProc_ProcessData.hpp
  \author  MarcW
  \brief   server process declaration

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

#if !defined(DBMSrvProc_ProcessData_hpp)
#define DBMSrvProc_ProcessData_hpp

#include <new.h>
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

// max number of memory chunks that we can get, set it to 100000 for use
#define DBMSrvProc_ProcessData_CHUNKS 1

// initial chunk size depending on machine size
#if defined (BIT64)
#define DBMSrvProc_ProcessData_INITIAL_CHUNKSIZE 1024*1024*1024
#else
#define DBMSrvProc_ProcessData_INITIAL_CHUNKSIZE 1024*1024
#endif

// new operator that does not throw exceptions
#if defined(WIN32)
#define DBMSrvProc_ProcessData_NEW new
#elif defined(HPUX)
#define DBMSrvProc_ProcessData_NEW new(nothrow)
#else
#define DBMSrvProc_ProcessData_NEW new(std::nothrow)
#endif


/*!
    @brief a class providing information about the current process
*/
class DBMSrvProc_ProcessData {
public:
    /*!
        @brief constructor

        errOut is empty, if object is okay
    */
    DBMSrvProc_ProcessData( RTE_OSPid pid, DBMSrvMsg_Error& errOut );

    /*! @brief destructor */
    ~DBMSrvProc_ProcessData() {};

    /*!
        @brief returns current memory consumption of process in bytes
        @param errOut
        @return 0 if method failed
    */
    size_t getMemorySize( DBMSrvMsg_Error& errOut );

    bool isValid() const {return m_Valid;}

private:
    void updateValidFlag();

    RTE_OSPid m_Pid;
    bool m_Valid;

#if defined(LINUX)
    // filename and key in it that describes the virtual memory size
    static const char* const m_StatusFileName;
    static const char* const m_VMSizeKey;
#endif

    /*! @brief do not use this method */
    size_t getMemorySizeByAllocation( DBMSrvMsg_Error& errOut );
    char* m_MemoryChunks[DBMSrvProc_ProcessData_CHUNKS];
};

#endif //DBMSrvProc_ProcessData
