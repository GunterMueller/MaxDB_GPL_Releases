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

public class State extends SchedulerTestCase {

    static private final int TESTJOB_TIME_DELTA = 30*60; //30 minutes, we're not interested in execution
    static private final boolean EXPECTING_ON = true;
    static private final boolean EXPECTING_OFF = false;


    /**
     * @throws SQLException
     * @throws TestDatabaseException
     */
    public State() throws SQLException, TestDatabaseException {
        super();
    }

    /* (non-Javadoc)
     * @see java.lang.Runnable#run()
     */
    public void runSchedulerTest() {

    	try {
    		checkState(EXPECTING_OFF);
            String timeString = scheduleJob(TESTJOB_TIME_DELTA);
            checkState(EXPECTING_ON);
            //saveSchedulerPid(); is done inside scheduleJob
        }
        catch (SchedulerException scdEx ) {
            addSchedulerTestMessage(
                getTestId(),
                TestProtocol.ERROR,
                "Scheduling error occurred...",
                scdEx);

        }
        finally {
            postTestCleanup();
        }
    }

    private void checkState(boolean aExpectedState ) throws SchedulerException {
        String cmdAnswer = "";
        boolean isExpectedReply = false;
        
        int waitHalfSeconds = 60;
        while( waitHalfSeconds > 0 ) {
            isExpectedReply = m_DBM.execCmd(
                    CMD_get_state,
                    DBMUtility.OK_OK);
            cmdAnswer = m_DBM.getAnswer();

            if( !isExpectedReply ) {
                throw new SchedulerException(
                    CMD_get_state,
                    cmdAnswer);
            }            
            if( cmdAnswer.indexOf(aExpectedState==EXPECTING_ON?"ON":"OFF") >= 0 )
                break;
            
            --waitHalfSeconds;
            
            try {
                Thread.sleep(500);
            }
            catch( InterruptedException ire ) {
                throw new SchedulerException(ire.getMessage());
            }
        }

        if(aExpectedState == EXPECTING_ON) {
            if( cmdAnswer.indexOf("ON") < 0)
                throw new SchedulerException (
                    "Expected scheduler to be ON, but received " +
                    cmdAnswer);
        }
        else /*if(aExpectedState == EXPECTING_OFF)*/ {
            if( cmdAnswer.indexOf("OFF") < 0)
                throw new SchedulerException (
                    "Expected scheduler to be OFF, but received " +
                    cmdAnswer);
        }
    }
}
