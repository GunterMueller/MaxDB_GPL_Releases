/*!
    @file           DBMSrvShM_LineLayoutRsourceLock.hpp
    @author         MarcW
    @brief          class for line layout for lines of type resource lock

\if EMIT_LICENCE

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


\endif
*/

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_LineLayoutBase.hpp"

#if !defined(DBMSrvShM_LineLayoutResourceLock_HPP)
#define DBMSrvShM_LineLayoutResourceLock_HPP

/*!
    @brief Layout description for lines of type resource lock

    The only extra attribute in a line of type eventing is the name of the resource that
    is to be locked. The resource can be anything, e.g. a file or something abstract like
    assuring exclusive execution of a kernel command.
        
    The name can have variable length. Thus the only extra member is a variable of type
    size_t, that contains the length of the path without the trailing zero. The name itself
    is stored in shm directly behind the bytes for the last member of the class.
*/
class DBMSrvShM_LineLayoutResourceLock: public DBMSrvShM_LineLayoutBase {
private:
    DBMSrvShM_SaveType<size_t> m_LenResourceName;

public:
    void setInitialSize() {
        m_LineSize.setValue(sizeof(*this));
    }

    bool lockResource(RTE_OSPid& requestingPid);
    bool unlockResource(RTE_OSPid& requestingPid);

    /*!
        @brief set the resource name

        @param resourceName [IN] name of resource to be locked
    */
    void setResourceName( const char* resourceName );

    /*!
        @brief get the resource name

        @return a pointer into shm 
    */
    SAPDB_Byte* getResourceName() const;

    /*!
        @brief get length of resource name

        @return length of resource name
    */
    size_t getResourceNameLen() const;

    /*!
        @brief get next line of type resource lock

        Returns pointer into shm behind current line of type resource lock. This pointer
        is valid only if there is a next line in shm of this type!
        This is not checked here.
        @return pointer to next line of type resource lock
    */
    DBMSrvShM_LineLayoutResourceLock* getNext() const;
};

#endif //DBMSrvShM_LineLayoutResourceLock_HPP
