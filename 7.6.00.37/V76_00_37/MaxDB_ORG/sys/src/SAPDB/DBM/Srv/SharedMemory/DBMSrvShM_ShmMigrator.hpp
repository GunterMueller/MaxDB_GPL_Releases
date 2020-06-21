/*!
    @file           DBMSrvShM_ShmMigrator.hpp
    @author         MarcW
    @brief          shared memory administration: migration

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

#if !defined(DBMSrvShM_ShmMigrator_HPP)
#define DBMSrvShM_ShmMigrator_HPP

class DBMSrvShM_ShmMigrator {

public:
    ~DBMSrvShM_ShmMigrator() {}
    
    static void deleteShmFromRundirectory(const char* runDirectory);


private:
    DBMSrvShM_ShmMigrator() {}

    static const char* const infoFile;
    static const char* const dataFile;
};

#endif //DBMSrvShM_ShmInfo_HPP
