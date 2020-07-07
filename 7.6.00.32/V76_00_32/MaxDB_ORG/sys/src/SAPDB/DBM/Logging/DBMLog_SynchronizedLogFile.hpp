/*!
  @file    DBMLog_SynchronizedLogFile.hpp
  @author  MarcW
  @brief   synchronized logfile

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

#if !defined(DBMLog_SynchronizedLogFile_hpp)
#define DBMLog_SynchronizedLogFile_hpp

#include "DBM/Logging/DBMLog_LogFile.hpp"

/*!
    @brief log file
*/
class DBMLog_SynchronizedLogFile: public DBMLog_LogFile {
public:
    DBMLog_SynchronizedLogFile(const char* aFileName);
    virtual ~DBMLog_SynchronizedLogFile();

    virtual bool writeOneEventList( const Msg_List& aEventList);
};
#endif //DBMLog_SynchronizedLogFile_hpp