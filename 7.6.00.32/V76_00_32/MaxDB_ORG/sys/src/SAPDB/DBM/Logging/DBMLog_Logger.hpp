/*!
  @file    DBMLog_Logger.hpp
  @author  MarcW
  @brief   logger

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

#if !defined(DBMLog_Logger_hpp)
#define DBMLog_Logger_hpp

#include "hin100.h"

#include "DBM/Srv/Message/DBMSrvMsg_Info.hpp"
#include "DBM/Logging/DBMLog_LogFile.hpp"
#include "Messages/Msg_List.hpp"
#include "ToolsCommon/Tools_Vector.hpp"

/*!
    @brief logs dbm server commands

    @todo To guarantee exclusive access to the log file, a lock is set during writing.
*/
class DBMLog_Logger {
public:
    /*!
        @brief who are we?

        The first 8 characters of the passed string are printed into the
        column "Label" in the log file.
        @param aDBMComponent [IN] content that is added to passed event and message lists
    */
    void setDBMComponent(const char* aDBMComponent);

    /*! @brief files are destroyed when logger is destroyed */
    void addFile( DBMLog_LogFile* aLogFile );

    /*!
        @brief log a Msg_List object

        @param aMessageList [IN] the message list to be logged
    */
    void addMessageList(const Msg_List& aMessageList);

    /*!
        @brief log a text with arguments

        Convinience method. Is equivalend to logging a DBMSrvMsg_Info object that
        contains one entry.
        @param aText [IN] format string
        @param argNum [IN] number of arguments following
        @param arg0 [IN] string argument 0
        @param arg1 [IN] string argument 1
        @param arg2 [IN] string argument 2
        @param arg3 [IN] string argument 3
        @param arg4 [IN] string argument 4
        @param arg5 [IN] string argument 5
        @param arg6 [IN] string argument 6
        @param arg7 [IN] string argument 7
        @param arg8 [IN] string argument 8
        @param arg9 [IN] string argument 9
    */
    void addInfoString(
        const char* aText,
        const char* const arg0 = NULL,
        const char* const arg1 = NULL,
        const char* const arg2 = NULL,
        const char* const arg3 = NULL,
        const char* const arg4 = NULL,
        const char* const arg5 = NULL,
        const char* const arg6 = NULL,
        const char* const arg7 = NULL,
        const char* const arg8 = NULL,
        const char* const arg9 = NULL);

    /*! @brief must be called by using process before termination */
    void freeResources();

    /*!
        @brief returns an instance of the logger
    */
    static DBMLog_Logger& getInstance();

private:
    DBMLog_Logger();
    ~DBMLog_Logger();

    tin100_Log* m_NormalLogger;
    tin100_Log* m_PanicLogger;
    Tools_Vector<DBMLog_LogFile*> m_RegisteredFiles;

    int m_PanicFileCount;
    int m_NormalFileCount;
    bool m_InDestructor;

    char* m_DBMComponent;
    
    static DBMLog_Logger m_Instance;
    static const char* const m_DefaultDBMComponent;
};

inline void DBMLog_Logger::addInfoString(
        const char* aText,
        const char* const arg0,
        const char* const arg1,
        const char* const arg2,
        const char* const arg3,
        const char* const arg4,
        const char* const arg5,
        const char* const arg6,
        const char* const arg7,
        const char* const arg8,
        const char* const arg9) {
    addMessageList(
        DBMSrvMsg_Info(
            aText,
            arg0,
            arg1,
            arg2,
            arg3,
            arg4,
            arg5,
            arg6,
            arg7,
            arg8,
            arg9));
 }
#endif //DBMLog_Logger_hpp
