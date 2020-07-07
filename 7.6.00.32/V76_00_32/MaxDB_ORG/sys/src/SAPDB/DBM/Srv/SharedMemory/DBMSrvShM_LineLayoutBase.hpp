/*!
    @file           DBMSrvShM_LineLayoutBase.hpp
    @author         MarcW
    @brief          base class for line layout - definition

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

#if !defined(DBMSrvShM_LineLayoutBase_HPP)
#define DBMSrvShM_LineLayoutBase_HPP

#include "DBM/Srv/SharedMemory/DBMSrvShM_Types.hpp"
#include "RunTime/System/RTESys_ProcessID.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*!
    @brief base class for line layout descriptions

    The shm is used in a way, that pointers to layouts point into the shm and that
    the data is manipulated thourgh getter/setter methods for members of the layout classes.
    These members must be arrays of SAPDB_Bytes to avoid problems with memory alignment.

    If e.g. a line contains an integer, the member of the class describing this line is not an
    integer, but a SAPDB_Byte array with sizeof(int) cells. The setter/getter methods would take
    resp. return an int, but storing is performed by SAPDB_memcpy-ing the data to and from the byte
    array.

    New and delete opeations should not be performed with this class and subclasses. This class
    and subclasses must not contain virtual methods.

    This base class provides members and access methods to line data, that lines of all types
    possess: an index for identification, a pid for background process that owns the line, a
    flag for cancelling the backgrounop process.
*/
class DBMSrvShM_LineLayoutBase {

private:
    DBMSrvShM_SaveType<int> m_Index;
    DBMSrvShM_SaveType<RTE_OSPid> m_BGSRVPid;
    DBMSrvShM_SaveType<SAPDB_Byte> m_CancelFlag;

protected:
    DBMSrvShM_SaveType<size_t> m_LineSize;

public:
    void addSizeDelta(size_t oldPartLen, size_t newPartLen);

    DBMSrvShM_LineLayoutBase* getNext() const;


    /*!
        @brief get this line's index
        
        @return this line's index
    */
    int getIndex() const;

    /*!
        @brief set this line's index
        
        @param index [IN] this line's index
    */
    void setIndex( int index );


    /*!
        @brief get the background process' id from the line

        @return pid of background process
    */
    RTE_OSPid getBGSRVPid() const ;

    /*!
        @brief set the background process' id from the line

        @param pid [IN]pid of background process
    */
    void setBGSRVPid(const RTE_OSPid& pid);

    /*
        @brief remove background process' id from the line

        Set zero value instead.
    */
    void removeBGSRVPid();

    /* @brief set cancel flag */
    void doCancel();
    
    /* @brief reset cancel flag */
    void unCancel();
    
    /*!
        @brief check cancel flag

        @return true if cancel flag is set
    */
    bool isCancelled();
};

#endif //DBMSrvShM_LineLayoutBase_HPP
