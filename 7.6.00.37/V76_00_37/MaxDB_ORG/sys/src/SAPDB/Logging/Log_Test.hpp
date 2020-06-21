/*!
  @file           Log_Test.hpp
  @author         UweH
  @ingroup        Logging
  @brief          defines the test call function

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


#ifndef Log_Test_HPP
#define Log_Test_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "Logging/Log_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
/// This testcases-class can be activated from vgg942.cpp
/// (PTS 1124724 mb 2003-11-3)
class Log_Test {
public:    
    /// defines the type of test
    typedef enum {
        Log_TestNone,           ///< no test at all
        Log_TestIOSequences,    ///< test Log_IOSequenceNo implementation
        Log_TestActions,        ///< test Log_Action implementation
        Log_TestLogPage,        ///< test Log_Page implementation
        Log_TestLogInfoPage,     ///< test Log_InfoPage implementation
        Log_ResetStatistics     ///< resets the values of the logging-statistics views
    } Log_TestCase;
    /// perform the specified test
    void PerformTest (       tgg00_TransContext& Trans,
                       const Log_TestCase        testCase);
private:
    /// test Log_IOSequenceNo implementation
    void TestIOsequences();

    /// tests if left is older than right. If not the kernel will crash
    void CheckIsOlder(      Log_IOSequenceNo left,
                            Log_IOSequenceNo right,
                            Log_IOSequenceNo min);
};
    
/*===========================================================================*
 *  EXTERNAL FUNCTIONS                                                       *
 *===========================================================================*/

#endif // Log_Test_HPP
