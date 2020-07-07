/*!
    @file           DBMSrvShM_LineLayoutEventing.hpp
    @author         MarcW
    @brief          class for line layout for lines of type eventing

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

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutBase.hpp"

#if !defined(DBMSrvShM_LineLayoutEventing_HPP)
#define DBMSrvShM_LineLayoutEventing_HPP

/*!
    @brief Layout description for lines of type eventing

    The only extra attribute in a line of type eventing is the path to
    the configuration file for the event dispatcher. This path can have variable
    length. Thus the only extra member is a variable of type size_t, that contains
    the length of the path without the trailing zero. The path itself is stored in
    shm directly behind the bytes for the last member of the class.
*/
class DBMSrvShM_LineLayoutEventing: public DBMSrvShM_LineLayoutBase {
private:
    DBMSrvShM_SaveType<int> m_InstanceID;
    DBMSrvShM_SaveType<SAPDB_Byte> m_IsInternal;
    DBMSrvShM_SaveType<SAPDB_Byte> m_InternalCommStatus;

    DBMSrvShM_SaveType<size_t> m_LenDBMUser;
    DBMSrvShM_SaveType<size_t> m_LenNodeName;
    DBMSrvShM_SaveType<size_t> m_LenLogFilePath;
    DBMSrvShM_SaveType<size_t> m_LenConfigFilePath;

public:

    void setInitialSize() {
        m_LineSize.setValue(sizeof(*this));
    }

    void setInstanceID( const int instanceID ) {m_InstanceID.setValue(instanceID);}
    int getInstanceID() const;

    void setInternalCommStatus( const SAPDB_Byte newStatus ) {
        m_InternalCommStatus.setValue(newStatus);
    }
    SAPDB_Byte getInternalCommStatus() const;

    bool isInternal() const ;
    void setAccessibleFlag(SAPDB_Byte accessFlag) {m_IsInternal.setValue(accessFlag);}

    void setDBMUser( const char* dbmUser );
    size_t getDBMUserLen() const;
    SAPDB_Byte* getDBMUser() const;

    void setNodeName( const char* nodeName );
    size_t getNodeNameLen() const;
    SAPDB_Byte* getNodeName() const;

    void setLogFilePath( const char* logFilePath );
    size_t getLogFilePathLen() const;
    SAPDB_Byte* getLogFilePath() const;

    void setConfigFilePath( const char* cfgFilePath );
    size_t getConfigFilePathLen() const ;
    SAPDB_Byte* getConfigFilePath() const;

    DBMSrvShM_LineLayoutEventing* getNext() const;
};
#endif //DBMSrvShM_LineLayoutEventing_HPP
