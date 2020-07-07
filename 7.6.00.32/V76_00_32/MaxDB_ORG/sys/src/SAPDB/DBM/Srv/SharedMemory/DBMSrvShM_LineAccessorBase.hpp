/*!
    @file           DBMSrvShM_LineAccessorBase.hpp
    @author         MarcW
    @brief          base class for line accessors

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

#include "SAPDB/DBM/Srv/SharedMemory/DBMSrvShM_ShmAdmin.hpp"

#if !defined(DBMSrvShM_LineAccessorBase_HPP)
#define DBMSrvShM_LineAccessorBase_HPP

/*!
    @brief base class for line accessors

    A shm application accesses information from the shm only through accessor classes. One
    instance of such a class represents one line in the shm. The shm itself is accessed from
    the accessor though the corresponding layout class.

    The accessor is responsible for locking and unlocking the shm through the shm admin class.

    Besides telling the shmadmin class where in the file system to put the shm file, a shm
    application just needs accessor classes.
*/
class DBMSrvShM_LineAccessorBase {
private:
    int m_Index;

protected:
    /*!
        @brief set the index to the passed value

        @param index [IN] the new index value
    */
    void setIndex( const int index ) {
        m_Index = index;
    }

    /*! @brief does this thing really repersent something from shm? */
    bool m_Validity;

    /*! @brief indicator if line exists in shm */
    bool m_Exists;

public:
    /*!
        @brief constructor

        An index is stored in the accessor.
        @param index [IN] index of the line that this accessor is responsible for
    */
    DBMSrvShM_LineAccessorBase(const int index=0)
        : m_Index(index),
          m_Exists(false),
          m_Validity(false) {
    }

    /*! @brief empty desctructor */
    virtual ~DBMSrvShM_LineAccessorBase() {}

    /*!
        @brief get the index of this accessor

        @return index of this accessor
    */
    int getIndex() const {
        return m_Index;
    }

    /*!
        @brief check after construction

        After call to the constructor of accessor classes,
        the application should check if the object it has
        is something real... Only if true is returned, use
        ful work with this object is possible.
        @return true if object is valid, false otherwise
    */
    bool isValid() const {
        return m_Validity;
    }

    /*!
        @brief does the line exist in shm?
        
        Get status if the line that is represented by this accessor does really exist in shm
        @return true if it does
    */
    virtual bool exists() {
        return m_Exists;
    }
};

#endif //DBMSrvShM_LineAccessorBase_HPP
