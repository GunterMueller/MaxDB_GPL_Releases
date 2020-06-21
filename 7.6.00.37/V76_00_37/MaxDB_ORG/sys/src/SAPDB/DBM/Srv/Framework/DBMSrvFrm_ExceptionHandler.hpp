/*!
  \file    DBMSrvFrm_ExceptionHandler.hpp
  \author  MarcW
  \ingroup DBMServer framework
  \brief   handle exceptions

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

#if !defined(DBMSrvFrm_ExceptionHandler_hpp)
#define DBMSrvFrm_ExceptionHandler_hpp

#include "gsp00.h"

#include "DBM/Srv/IO/DBMSrvIO_SharedTextFile.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

/*!
    @brief global function for writing one line of a stacktrace
*/
void DBMSrvFrm_ExceptionHandler_StrackWritingCallback( const char* const backtraceString, void* const contextInformation );

/*!
    @brief global function for writing a message how the dumping went
*/
void DBMSrvFrm_ExceptionHandler_DumpWritingCallback( const char* const messageString, void* const contextInformation );

/*!
    @brief class for handling exceptions
*/
class DBMSrvFrm_ExceptionHandler {

public:
    DBMSrvFrm_ExceptionHandler();
    ~DBMSrvFrm_ExceptionHandler();

    bool initialize( DBMSrvMsg_Error& errOut );

#if defined( _WIN32 )
    int dumpInformation( unsigned int exCode, struct _EXCEPTION_POINTERS *ep );
#else
    void dumpInformation( int signalCode );
#endif

private:
#if defined( _WIN32 )
    void writeExceptionHeader(unsigned int exCode, void* exAddress);
#else
    void writeExceptionHeader(int exCode);
#endif
    void writeExceptionLine( const char* const backtraceString );
    void writeDumpStatusMessage( const char* const messageString );

    bool m_IsInitialized;
    DBMSrvIO_SharedTextFile* m_StackTraceFile;
    tsp00_Pathc m_DumpFileName;

#if defined( _WIN32 )
    HANDLE m_DumperThread;
#else
    SAPDB_Int4 m_StackTraceSemaphore;
#endif

    char m_WriteBuffer[1024];

    friend void DBMSrvFrm_ExceptionHandler_StrackWritingCallback( const char* const backtraceString, void* const contextInformation );
    friend void DBMSrvFrm_ExceptionHandler_DumpWritingCallback( const char* const outputString, void* const contextInformation );
};

#endif //DBMSrvFrm_ExceptionHandler_hpp
