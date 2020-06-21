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
import java.util.StringTokenizer;

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

public class ListJobs extends SchedulerTestCase {

    private static final String ANSWERLINE_TIME = "runs at:";
    private static final String ANSWERLINE_COMMAND = "command:";
    static private final int TESTJOB_TIME_DELTA = 30*60; //30 minutes, we're not interested in execution

    /**
     * @throws SQLException
     * @throws TestDatabaseException
     */
    public ListJobs() throws SQLException, TestDatabaseException {
        super();
    }

    /* (non-Javadoc)
     * @see java.lang.Runnable#run()
     */
    public void runSchedulerTest() {
        try {
            // initial list, should be empty
            boolean isExpectedReply = m_DBM.execCmd(CMD_list, DBMUtility.OK_OK);
            String cmdAnswer = m_DBM.getAnswer();            

            if( isExpectedReply ) {
                if( cmdAnswer.trim().length() > 0 ) {
                    // the list should be empty
                    throw new SchedulerException(
                        "Initial list. Expected empty job list, but received: " +
                        cmdAnswer);
                }
            }
            else
                throw new SchedulerException(CMD_list, cmdAnswer);
                
            // schedule a job
            String timeString = scheduleJob(TESTJOB_TIME_DELTA);

            //saveSchedulerPid(); is done inside scheduleJob

            // now list again
            isExpectedReply = m_DBM.execCmd(CMD_list, DBMUtility.OK_OK);
            cmdAnswer = m_DBM.getAnswer();            

            if( isExpectedReply ) {
                // analyze the answer
                StringTokenizer strTok = new StringTokenizer(cmdAnswer, "\n\r");
                if( strTok.countTokens() != 6 ) {
                    throw new SchedulerException(
                        "Final list, expected 6 lines, but received " +
                        strTok.countTokens() +
                        " lines: " +
                        "\"" + 
                        cmdAnswer +
                        "\"");
                }
                while( strTok.hasMoreTokens() ) {
                    String answerLine = strTok.nextToken();
                    if( answerLine.indexOf(ANSWERLINE_COMMAND) >= 0 ) {
                        // check if the command is correct
                        if( answerLine.indexOf(SCD_CMD) < 0 ) {
                            // it's not our command
                            throw new SchedulerException(
                                "Final list, wrong command, expected " +
                                SCD_CMD +
                                ", but received " +
                                "\"" +
                                answerLine +
                                "\"");
                        }
                    }
                    else if( answerLine.indexOf(ANSWERLINE_TIME) >= 0 ) {
                        // check if the scheduled time is correct
                        if( answerLine.indexOf(timeString) < 0 ) {
                            // time is wrong
                            throw new SchedulerException(
                                "Final list, wrong time, expected " +
                                timeString +
                                ", but received " +
                                "\"" +
                                answerLine +
                                "\"");
                        }
                    }
                }
            }
            else
                throw new SchedulerException(CMD_list, cmdAnswer);

        }
        catch( SchedulerException scdEx ) { 
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
}
