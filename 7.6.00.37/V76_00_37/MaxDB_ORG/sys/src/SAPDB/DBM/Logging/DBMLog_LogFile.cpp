/*!
  @file    DBMLog_LogFile.cpp
  @author  MarcW
  @brief   clog file base class

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

#include "SAPDBCommon/SAPDB_string.h"
#include "DBM/Logging/DBMLog_LogFile.hpp"

DBMLog_LogFile::DBMLog_LogFile(const char* aFileName, bool isForPanic)
        : tin105_SequentialLogFile( aFileName, LOG_ALL_MIN105 ),
          m_FileName(NULL),
          m_IsForPanic(isForPanic) {
    m_FileName = new char[strlen(aFileName)+1];
    if( m_FileName == NULL ) {
        disable();
        return;
    }
    SAPDB_strcpy(m_FileName, aFileName);
}

DBMLog_LogFile::~DBMLog_LogFile() {
    if( m_FileName != NULL )
        delete[] m_FileName;
}

bool DBMLog_LogFile::isForPanic() {
    return m_IsForPanic;
}

bool DBMLog_LogFile::writeOneEventList( const Msg_List& aEventList) {
    return tin105_SequentialLogFile::writeOneEventList( aEventList );
}