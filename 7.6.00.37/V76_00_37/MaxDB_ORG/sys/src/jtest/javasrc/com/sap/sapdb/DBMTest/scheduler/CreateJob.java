package com.sap.sapdb.DBMTest.scheduler;

/*
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

import java.sql.SQLException;

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

public class CreateJob extends SchedulerTestCase {

    static private final int TESTJOB_TIME_DELTA = 5*60; // job should be executed in 5 minutes
    static private final boolean EXPECTING_NO_EXECUTION = false;
    static private final boolean EXPECTING_EXECUTION = true;

    /**
     * @throws SQLException
     * @throws TestDatabaseException
     */
    public CreateJob() throws SQLException, TestDatabaseException {
        super();
    }

    /* (non-Javadoc)
     * @see java.lang.Runnable#run()
     */
    public void runSchedulerTest() {
        try {
            String scheduledTimeString = scheduleJob(TESTJOB_TIME_DELTA);
            //saveSchedulerPid(); is done inside scheduleJob

            // job must not be executed prematurely
            int waitSeconds = TESTJOB_TIME_DELTA - 10;
            while( waitSeconds > 0 ) {
                Thread.sleep(1000);
                if( !jobExecutionStatusOkay(EXPECTING_NO_EXECUTION) ) {
                    String nowTimeString = getTimeStringInFuture(0);
                    if( isBefore(nowTimeString, scheduledTimeString) ) {
                        throw new SchedulerException(
                            "Expected job execution at " +
                            scheduledTimeString +
                            ", but execution was before "+
                            nowTimeString);
                    }
                    else
                    	break;
                }
                --waitSeconds;
            }

            // give scheduler 5 minutes to execute the job
            waitSeconds = 300;
            while( waitSeconds > 0 ) {
                Thread.sleep(1000);
                if( jobExecutionStatusOkay(EXPECTING_EXECUTION) )
                    break;
                --waitSeconds;
            }
            
            if( waitSeconds == 0 ) {
                // job was not executed
                throw new SchedulerException(
                    "Expected job execution at " +
                    scheduledTimeString +
                    ", but execution never occurred");
            }
        }
        catch( SchedulerException scdEx ) {
            addSchedulerTestMessage(
                getTestId(),
                TestProtocol.ERROR,
                "Scheduling error occurred...",
                scdEx);
        }
        catch ( InterruptedException irEx ) {
            addSchedulerTestMessage(
                getTestId(),
                TestProtocol.ERROR,
                "Scheduling error occurred, got interrupted...",
                irEx);
        
        }
        finally {
            postTestCleanup();
        }
    }

    boolean jobExecutionStatusOkay( boolean expectedResult )
            throws SchedulerException {

        boolean isExpectedReply = m_DBM.execCmd(SCD_CHECK_CMD, DBMUtility.OK_OK);
        String cmdAnswer = m_DBM.getAnswer();            
        if( !isExpectedReply )
            throw new SchedulerException(SCD_CHECK_CMD, cmdAnswer);
        
        if( expectedResult == EXPECTING_EXECUTION) {        
            // the answer must contain the value we set earlier
            if( cmdAnswer.indexOf(SCD_CMD_PARAM_VAL) < 0 )
                return false;
        }
        else {
            // the answer must not contain the value we set earlier
            if( cmdAnswer.indexOf(SCD_CMD_PARAM_VAL) >= 0 )
                return false;
        }
        return true;
    }
}

