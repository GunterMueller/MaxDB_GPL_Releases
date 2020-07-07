/*!
    @file           DBMSrvShM_Persistence.hpp
    @author         MarcW
    @brief          description of the shared memory, of maintenance "from outside"

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

#if !defined(DBMSrvShM_Persistence_HPP)
#define DBMSrvShM_Persistence_HPP

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

/*! @brief description of the shared memory, of maintenance "from outside" */
class DBMSrvShM_Persistence {
public:
    DBMSrvShM_Persistence(
            const char* rootDir,
            const char* dbname,
            DBMSrvMsg_Error& errOut);

    ~DBMSrvShM_Persistence();

    bool isValid() {return m_IsValid;}

    const char* getRootDir() const {
        return m_RootDir;
    }
    const char* getInfoFile() const {
        return m_InfoFileAbs;
    }
    const char* getDataFile() const {
        return m_DataFileAbs;
    }

    bool exists( bool& doesExist, DBMSrvMsg_Error& errOut) const;
    bool remove( DBMSrvMsg_Error &errList);

    
    static const char* const m_FileNameBase;
    static const char* const m_FileExtensionData;
    static const char* const m_FileExtensionAdmin;

private:
    char* m_RootDir;
    char* m_InfoFileAbs;
    char* m_DataFileAbs;

    bool m_IsValid;
};

#endif //DBMSrvShM_Persistence_HPP
