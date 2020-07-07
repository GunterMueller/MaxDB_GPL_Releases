/*!***************************************************************************

  module      : Log_Test
  responsible : UweH
  special area: DataAccess
  created     : 2000-11-22
  last changed: 2000-11-22 12:00
  Copyright (c) 2000-2005 SAP AG
  description : entrypoint for component tests



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


*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h" // tgg00_TransContext
#include "Logging/Log_Test.hpp"
#include "Logging/Log_ActionUndoRedo.hpp"
#include "Logging/Log_Statistics.hpp"
#include "Logging/Log_QueueStatistics.hpp"

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
 *  LOCAL FUNCTIONS                                                          *
 *===========================================================================*/
void Log_Test::PerformTest (       tgg00_TransContext& Trans,
                             const Log_TestCase        testCase)
{
    switch ( testCase )
    {
        case Log_TestIOSequences:
            TestIOsequences();
            break;
        case Log_TestLogPage:
        case Log_TestLogInfoPage:
            // not implemented yet
            break;
        case Log_TestActions:
            Log_ActionTestAllActions();
            break;
        case Log_ResetStatistics:
            Log_StatisticManager.ResetStatistics(Trans.trTaskId_gg00);
            Log_QueueStatisticManager.ResetStatistics(Trans.trTaskId_gg00);
            break;
        default:
            break;
    }
}

/* --------------------------------------------------------------------------- */

void Log_Test::TestIOsequences()
{
    Kernel_VTrace trace;
    
    Log_IOSequenceNo startSeq(10);
    
    int i,j;
    for (i=0; i < 20; i++)
    {
        for (j=0; j < 20; j++)
        {
            Log_IOSequenceNo addResult = (startSeq + j);
            Log_IOSequenceNo subResult = (startSeq - j);
                            
            trace << " IOSEQ ADD " << startSeq << " + " << j << " = " << addResult << NewLine;
            trace << " IOSEQ SUB " << startSeq << " - " << j << " = " << subResult << NewLine;
            if ((addResult - j) != startSeq)
            {
                trace << " IOSEQ COMP1 " << addResult - j << NewLine;
                RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"(addResult - j) != startSeq") );
            }
            if ((subResult + j) != startSeq)
            {
                trace << " IOSEQ COMP2 " << subResult + j << NewLine;
                RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"(subResult + j) != startSeq") );
            }
        }
        --startSeq;
        if (startSeq.IsInvalid())
        {
            RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"startSeq.IsInvalid()") );
        }
    }
    for (i=0; i < 20; i++)
    {
        ++startSeq;
    }
    if (startSeq != Log_IOSequenceNo(10))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"startSeq != Log_IOSequenceNo(10)") );
    }
    
    Log_IOSequenceNo leftBound  = Log_IOSequenceNo(10);
    Log_IOSequenceNo rightBound = Log_IOSequenceNo(20);
    Log_IOSequenceNo checkedIOSeq;

    checkedIOSeq = Log_IOSequenceNo(10); 
    if (!checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "10 is in range 10..20") );
    }

    checkedIOSeq = Log_IOSequenceNo(11); 
    if (!checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "11 is in range 10..20") );
    }

    checkedIOSeq = Log_IOSequenceNo(19); 
    if (!checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "19 is in range 10..20") );
    }
    
    checkedIOSeq = Log_IOSequenceNo(20); 
    if (!checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "20 is in range 10..20") );
    }

    checkedIOSeq = Log_IOSequenceNo(9); 
    if (checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "9 is not in range 10..20") );
    }

    checkedIOSeq = Log_IOSequenceNo(21); 
    if (checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "21 is not in range 10..20") );
    }

    leftBound =    Log_IOSequenceNo(19); 
    rightBound =   Log_IOSequenceNo(20);

    checkedIOSeq = Log_IOSequenceNo(19); 
    if (! checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "19 is not in range 19..20") );
    }
    
    checkedIOSeq = Log_IOSequenceNo(20); 
    if (! checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "20 is not in range 19..20") );
    }

    leftBound =    Log_IOSequenceNo(20); 
    if (! checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "20 is not in range 20..20") );
    }
    
    leftBound =    Log_IOSequenceNo(4000000000); 
    rightBound =   Log_IOSequenceNo(20);

    checkedIOSeq = Log_IOSequenceNo(4000000000); 
    if (!checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "4000000000 is in range 4000000000..4294967294,0..20") );
    }

    checkedIOSeq = Log_IOSequenceNo(4000000001); 
    if (!checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "4000000001 is in range 4000000000..4294967294,0..20") );
    }

    checkedIOSeq = Log_IOSequenceNo(4294967294); 
    if (!checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "4294967294 is in range 4000000000..4294967294,0..20") );
    }

    checkedIOSeq = Log_IOSequenceNo(0); 
    if (!checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "0 is in range 4000000000..4294967294,0..20") );
    }

    checkedIOSeq = Log_IOSequenceNo(1); 
    if (!checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "1 is in range 4000000000..4294967294,0..20") );
    }

    checkedIOSeq = Log_IOSequenceNo(19); 
    if (!checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "19 is in range 4000000000..4294967294,0..20") );
    }

    checkedIOSeq = Log_IOSequenceNo(20); 
    if (!checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "20 is in range 4000000000..4294967294,0..20") );
    }

    checkedIOSeq = Log_IOSequenceNo(21); 
    if (checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "21 is not in range 4000000000..4294967294,0..20") );
    }

    checkedIOSeq = Log_IOSequenceNo(3999999999); 
    if (checkedIOSeq.IsInRange(leftBound,rightBound))
    {
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                  "3999999999 is not in range 4000000000..4294967294,0..20") );
    }
}
                
void Log_Test::CheckIsOlder(   Log_IOSequenceNo left,
                            Log_IOSequenceNo right,
                            Log_IOSequenceNo min)
{
    if (!Log_IOSequenceNo::LeftIsOlder(left,right,min))
    {
        Kernel_VTrace trace;
        trace << " LeftIsOlder ( " << left << " , " << right << " , " << min << " ) is FALSE";
        RTE_Crash(Log_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,"LeftIsOlder(...)") );
    }
}

