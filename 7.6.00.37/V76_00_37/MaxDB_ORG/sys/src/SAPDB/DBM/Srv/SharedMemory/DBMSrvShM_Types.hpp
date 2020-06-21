/*!
    @file           DBMSrvShM_Types.hpp
    @author         MarcW
    @brief          various basic types needed for robust shm implementation

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

#if !defined(DBMSrvShM_Types_HPP)
#define DBMSrvShM_Types_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"

#ifdef _DBMSRVSHM_CRASHTEST_
#include <stdio.h>
#endif

/*!
    @brief makes a kill-save class of a class

    The shared memory must be robust against killing of processes. Generally, during assigning
    a new value to a variable, the process can be killed...
    
    Solution: have room for two values in a variable and an indicator.
    After assigning a new value, swith the indicator (which is an atomic operation).
    The read method checks the indicator. All this is internal to this template...
*/
template <class T>
class DBMSrvShM_SaveType {
public:
    /*!
        @brief setter method

        @param newVal [IN] reference to the new value
    */
    inline void setValue( const T& newVal );

    /*!
        @brief getter method

        @param val [OUT] reference to new value
    */
    inline void getValue( T& val ) const;

private:
    SAPDB_Byte m_Selector;
    SAPDB_Byte m_Val0[sizeof(T)];
    SAPDB_Byte m_Val1[sizeof(T)];
    void toggleSelector() {
        // make sure that the new value is in memory before
        // flag is toggeled.
        RTESys_WriteMemoryBarrier();
        m_Selector==0?m_Selector=1:m_Selector=0;
    }
    bool useVal0() const {return m_Selector==0;}
};

/*!
    @brief specialization for SAPDB_Bytes

    Since byte operation are atomic already, we don't need the double space here...
    @see template <class T> class DBMSrvShM_SaveType
*/
template<>
class DBMSrvShM_SaveType<SAPDB_Byte> {
public:
    /*!
        @brief setter method

        @param newVal [IN] reference to the new value
    */
    void setValue( const SAPDB_Byte& newVal ) {m_Val = newVal;}

    /*!
        @brief getter method

        @param val [OUT] reference to new value
    */
    void getValue( SAPDB_Byte& val ) const {val = m_Val;}

private:
    SAPDB_Byte m_Val;
};

#ifdef _DBMSRVSHM_CRASHTEST_

// should be defined in com description
#ifndef _DBMSRVSHM_CRASHLOCATION_
#define _DBMSRVSHM_CRASHLOCATION_ -1
#endif

/*!
    @brief test class for perfoming a crash

    This class is conditionally compiled. It exists only if the define _DBMSRVSHM_CRASHTEST_
    exists.

    At places in code, where a crash for testing is wanted, place makro. If the define _DBMSRVSHM_CRASHTEST_
    is not set, the makro will expand to nothing.
*/
class DBMSrvShM_CrashTest {
public:
    /*!
        @brief performs a division by zero

        The locationID must be unique over all crash methods.

        @param locationID [IN] identifer of location from where this was called
        @param file [IN] file name of file from where this was called
    */
    static void divideByZero(int locationID, const char* file ) {
        printf("CRASHTEST, location %d, define %d\n", locationID,_DBMSRVSHM_CRASHLOCATION_);
        if( locationID != _DBMSRVSHM_CRASHLOCATION_ )
            return;
        printf("CRASHTEST, divide by zero at location %d, file %s\n", locationID, file);
        fflush(stdout);
        // this is for avoiding compiler's semantic check...
        int j = 1;
        j = decrement(j);
        int i = 1 / j;
        // need to do something with i, otherwise some compilers optimize the code...
        printf("never executed %d", i);
    }

    static void setCrashLocationID(int locID) {
        sm_locID = locID;
    }

    static int decrement( int i ) {
        return --i;
    }

private:
    static int sm_locID;
};
#define __shm_crashtest_divideByZero(locID, file) DBMSrvShM_CrashTest::divideByZero(locID, file)
#else
#define __shm_crashtest_divideByZero(locID, file) 
#endif //_DBMSRVSHM_CRASHTEST_



template <class T>
inline void DBMSrvShM_SaveType<T>::setValue( const T& newVal ) {
    // this works also for arrays, because for an array arr arr==&arr is true (both
    // are pointers to arr's first element)
    if( useVal0() ) {
        ToolsMeMa_GuardedOperation::MemCopy(m_Val1, &newVal, sizeof(T), __FILE__, __LINE__);
    }
    else {
        ToolsMeMa_GuardedOperation::MemCopy(m_Val0, &newVal, sizeof(T), __FILE__, __LINE__);
    }
    toggleSelector();
}

template <class T>
inline void DBMSrvShM_SaveType<T>::getValue( T& val ) const {
    if( useVal0() ) {
        ToolsMeMa_GuardedOperation::MemCopy(&val, m_Val0, sizeof(T), __FILE__, __LINE__);
    }
    else {
        ToolsMeMa_GuardedOperation::MemCopy(&val, m_Val1, sizeof(T), __FILE__, __LINE__);
    }
}

#endif //DBMSrvShM_Types_HPP
