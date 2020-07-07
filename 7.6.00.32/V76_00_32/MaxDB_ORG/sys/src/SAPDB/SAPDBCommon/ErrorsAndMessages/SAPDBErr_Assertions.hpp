/*!
  @file           SAPDBErr_Assertions.hpp
  @author         DanielD
  @brief          Assertions are used to check for programming errors.
  @verbatim           
   
   |Assert option              STATE       ARGUMENT    RANGE
   |-------------------------------------------------------------
   |Profile                 |
   |------------------------|
   |ASSERTION_FAST_CODE     |  OFF         OFF         OFF
   |ASSERTION_QUICK_CODE    |  OFF         OFF         EXCEPTION
   |ASSERTION_EXCEPTION_CODE|  EXCEPTION   EXCEPTION   EXCEPTION
   |ASSERTION_DEBUG_CODE    |  DEBUG       DEBUG       DEBUG
   |ASSERTION_CRASH_CODE    |  CRASH       CRASH       CRASH
   
  @endverbatim

  @brief          Other profile options can be inserted and have to be described above.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


/*------------------------------------------------------------------------------
                     !!! ATTENTION !!!
                     Function call RTE_StackBacktrace has to be inserted.
------------------------------------------------------------------------------*/

#ifndef SAPDBERR_ASSERTIONS_HPP
#define SAPDBERR_ASSERTIONS_HPP


#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "RunTime/RTE_CallDebugger.h"
#include "RunTime/RTE_Crash.hpp"

// Error messages generated from SAPDBErr_Messages.msg via genmsg.pl
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Messages.hpp"  /* nocheck */


// Definition of possible options for assertions
#define SAPDBERR_OFF         0
#define SAPDBERR_DEBUG       1
#define SAPDBERR_EXCEPTION   2
#define SAPDBERR_CRASH       3

// Setting the Assertion options according to the profile chosen
#if defined(ASSERTION_FAST_CODE)
    #define ASSERT_STATE_OPTION         SAPDBERR_OFF
    #define ASSERT_ARGUMENT_OPTION      SAPDBERR_OFF 
    #define ASSERT_RANGE_OPTION         SAPDBERR_OFF

#elif defined(ASSERTION_DEBUG_CODE)
    #define ASSERT_STATE_OPTION         SAPDBERR_DEBUG
    #define ASSERT_ARGUMENT_OPTION      SAPDBERR_DEBUG
    #define ASSERT_RANGE_OPTION         SAPDBERR_DEBUG

#elif defined(ASSERTION_QUICK_CODE)
    #define ASSERT_STATE_OPTION         SAPDBERR_OFF
    #define ASSERT_ARGUMENT_OPTION      SAPDBERR_OFF 
    #define ASSERT_RANGE_OPTION         SAPDBERR_EXCEPTION

#elif defined(ASSERTION_EXCEPTION_CODE)
    #define ASSERT_STATE_OPTION         SAPDBERR_EXCEPTION
    #define ASSERT_ARGUMENT_OPTION      SAPDBERR_EXCEPTION
    #define ASSERT_RANGE_OPTION         SAPDBERR_EXCEPTION

#elif defined(ASSERTION_CRASH_CODE)
    #define ASSERT_STATE_OPTION         SAPDBERR_CRASH
    #define ASSERT_ARGUMENT_OPTION      SAPDBERR_CRASH
    #define ASSERT_RANGE_OPTION         SAPDBERR_CRASH

#endif


/*!
   @brief          defines an exception class for the component SAPDBErr

 */

SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS( SAPDBErr );


/*!
   @brief          This assertion is used for checking the state of an object.


                    Example:
                    If a method of an object should only be executed if a pointer member m_Ptr 
                    is not null, the corresponding assertion would be:
                    SAPDBERR_ASSERT_STATE(m_Ptr != 0);

 */

#if defined(ASSERT_STATE_OPTION)

    #if ASSERT_STATE_OPTION == SAPDBERR_OFF

        #define SAPDBERR_ASSERT_STATE( _condition ) //nothing

    #elif ASSERT_STATE_OPTION == SAPDBERR_DEBUG

        #define SAPDBERR_ASSERT_STATE( _condition ) \
            if (!(_condition))                      \
                RTE_CallDebugger();

    #elif ASSERT_STATE_OPTION == SAPDBERR_EXCEPTION

        #define SAPDBERR_ASSERT_STATE( _condition ) \
            if (!(_condition))                      \
                throw SAPDBErr_Exception(__FILE__, __LINE__, SAPDBERR_ASSERT_STATE_FAILED, #_condition);

    #elif ASSERT_STATE_OPTION == SAPDBERR_CRASH

        #define SAPDBERR_ASSERT_STATE( _condition ) \
            if (!(_condition))                      \
                RTE_Crash(SAPDBErr_Exception(__FILE__, __LINE__, SAPDBERR_ASSERT_STATE_FAILED, #_condition));

    #else
        #error Unknown option for state assertions
    #endif

#else // Assertions switched off
    #error Option for state assertion not defined
#endif


/*!
   @brief          This assertion is used for checking the values of function arguments.


                    Example:
                    If a function f(int x) requires its argument x to be 
                    greater than zero, the corresponding assertion would be:
                    SAPDBERR_ASSERT_ARGUMENT(x > 0);

 */

#if defined(ASSERT_ARGUMENT_OPTION)

    #if ASSERT_ARGUMENT_OPTION == SAPDBERR_OFF

        #define SAPDBERR_ASSERT_ARGUMENT( _condition )  //nothing

    #elif ASSERT_ARGUMENT_OPTION == SAPDBERR_DEBUG

        #define SAPDBERR_ASSERT_ARGUMENT( _condition )  \
            if (!(_condition))                          \
                RTE_CallDebugger();

    #elif ASSERT_ARGUMENT_OPTION == SAPDBERR_EXCEPTION

        #define SAPDBERR_ASSERT_ARGUMENT( _condition )  \
            if (!(_condition))                          \
                throw SAPDBErr_Exception(__FILE__, __LINE__, SAPDBERR_ASSERT_ARGUMENT_FAILED, #_condition);

    #elif ASSERT_ARGUMENT_OPTION == SAPDBERR_CRASH

        #define SAPDBERR_ASSERT_ARGUMENT( _condition ) \
            if (!(_condition))                      \
                RTE_Crash(SAPDBErr_Exception(__FILE__, __LINE__, SAPDBERR_ASSERT_STATE_FAILED, #_condition));

    #else
        #error Unknown option for argument assertions
    #endif

#else // Assertions switched off
    #error Option for argument assertion not defined
#endif


/*!
   @brief          This assertion is used for checking the range of function arguments.


                    Example:
                    If a function f(int x) requires its argument x to be
                    between 10 and 20 (inclusive), the corresponding assertion would be:
                    SAPDBERR_ASSERT_RANGE(x, 10, 20);

 */

#if defined(ASSERT_RANGE_OPTION)

    #if ASSERT_RANGE_OPTION == SAPDBERR_OFF

        #define SAPDBERR_ASSERT_RANGE( _index, _min, _max )  //nothing

    #elif ASSERT_RANGE_OPTION == SAPDBERR_DEBUG

        #define SAPDBERR_ASSERT_RANGE( _index, _min, _max )     \
            if (((_index) < (_min)) || ((_index) > (_max)))     \
                RTE_CallDebugger();

    #elif ASSERT_RANGE_OPTION == SAPDBERR_EXCEPTION

        #define SAPDBERR_ASSERT_RANGE( _index, _min, _max )                     \
            if (((_index) < (_min)) || ((_index) > (_max)))                     \
                throw SAPDBErr_Exception(__FILE__, __LINE__,                    \
                                         SAPDBERR_ASSERT_RANGE_FAILED, #_index, \
                                         SAPDB_ToString((_index)),              \
                                         SAPDB_ToString((_min)),                \
                                         SAPDB_ToString((_max)));

    #elif ASSERT_RANGE_OPTION == SAPDBERR_CRASH

        #define SAPDBERR_ASSERT_RANGE( _index, _min, _max )                         \
            if (((_index) < (_min)) || ((_index) > (_max)))                         \
                RTE_Crash(SAPDBErr_Exception(__FILE__, __LINE__,                    \
                                             SAPDBERR_ASSERT_RANGE_FAILED, #_index, \
                                             SAPDB_ToString((_index)),              \
                                             SAPDB_ToString((_min)),                \
                                             SAPDB_ToString((_max))));

    #else
        #error Unknown option for range assertions
    #endif

#else // Assertions switched off
    #error Option for range assertion not defined
#endif



#endif //SAPDBERR_ASSERTIONS_HPP

