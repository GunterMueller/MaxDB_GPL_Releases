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

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.StringReader;
import java.sql.SQLException;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.HashSet;
import java.util.Iterator;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

public abstract class SchedulerTestCase extends DBMTestCase {

    private static final String ERRTXT_CLEANUP = "Error occurred in cleanup...";
    private static final int PID_UNDEFINED = -1;

    private int m_SchedulerPid;

    // scheduler commands 
    protected static final String CMD_list = "scheduler_list_jobs";
    protected static final String CMD_schedule = "scheduler_create_job";
    protected static final String CMD_start = "scheduler_start";
    protected static final String CMD_stop = "scheduler_stop";
    protected static final String CMD_delete_job = "scheduler_delete_job";
    protected static final String CMD_deactivate_job = "scheduler_deactivate_job";
    protected static final String CMD_get_state = "scheduler_state";

    // shared memory command
    protected static final String CMD_shmInfo = "dbm_shm_info";    

    //private static final String ANSWERLINE_OWNPID = "own pid ";

    // file names for uploading
    private static final String FILE_DBMPRT = "dbm.prt"; 
    private static final String EXTENSION_DBMSHI = ".dbm.shi";
    private static final String EXTENSION_DBMSHM = ".dbm.shm";
    private static final String FILE_SCDPRT = "dbmsrvscd.cfg";
    
    // tokens in answer from shared memory info command (CMD_shmInfo)
    private static final String TOKEN_PROCESSES = "PROCESSES";
    private static final String TOKEN_PERSISTENCE = "PERSISTENCE";
    private static final String TOKEN_LOCKCOUNTER = "LOCKCOUNTER";
    private static final String TOKEN_ONWPID = "(*)";

    
    // scheduled dbmcli command
    protected static final String SCD_CMD_PARAM_NAME =
            "DummyTestKey";
    protected static final String SCD_CMD_PARAM_VAL =
            "DummyTestValue";
    protected static final String SCD_CMD =
            "dbm_configset " +
            SCD_CMD_PARAM_NAME +
            " " +
            SCD_CMD_PARAM_VAL;

    protected static final String SCD_CHECK_CMD = 
            "dbm_configget " +
            SCD_CMD_PARAM_NAME;

    protected static final String SCD_CMD_UNDO =
            "dbm_configset -raw " +
            SCD_CMD_PARAM_NAME;
            
    private DateFormat m_DateFormatter;
    /**
     * @throws java.sql.SQLException
     * @throws com.sap.sapdb.testframe.driver.TestDatabaseException
     */
    public SchedulerTestCase() throws SQLException, TestDatabaseException {
        super();
        m_SchedulerPid = PID_UNDEFINED;
        m_DateFormatter = new SimpleDateFormat("HH:mm:ss");
    }
    
    public abstract void runSchedulerTest();
    
    private HashSet m_DBMServerPIDSet;
    
    public void runDBMTest() {
    	m_DBMServerPIDSet = getConnectedPIDs();
    	
    	runSchedulerTest();
    }
    
    private HashSet getConnectedPIDs() {
    	
    	boolean isExpectedReply = m_DBM.execCmd(
                CMD_shmInfo,
                DBMUtility.OK_OK);
        String cmdAnswer = m_DBM.getAnswer();
        if( !isExpectedReply ) {
            addMessage(
                getTestId(),
                TestProtocol.ERROR,
                "Could not get DBMServer PIDs...",
                new SchedulerException(CMD_shmInfo, cmdAnswer));
        }
        BufferedReader answerReader = new BufferedReader(new StringReader(cmdAnswer));
        String line = new String("");
        HashSet pidSet = new HashSet();

        try {
	        while( line != null && !line.equals(TOKEN_PROCESSES) )
	        	line = answerReader.readLine();
	        
	    	if( line != null )
	    		line = answerReader.readLine();
	    	if( line != null )
	    		line = answerReader.readLine();
	        // cmdAnswer now points to the first pid
	        while( line != null && line.trim().length()>0 && !line.equals(TOKEN_LOCKCOUNTER) ) {
	        	Integer pid = new Integer(line.substring(0, line.indexOf(' ')));
	        	pidSet.add(pid);
	        	line = answerReader.readLine();
	        }
	        answerReader.close();
        }
        catch( IOException ioe ) {
            addMessage(
                    getTestId(),
                    TestProtocol.ERROR,
                    "Could not analyze DBMServer PIDs...",
                    new SchedulerException(CMD_shmInfo, cmdAnswer));
        }
        return pidSet;
    }
    
    private void saveSchedulerPid() throws SchedulerException {

        int waitHalfSeconds = 600; // 5 minutes
        while( waitHalfSeconds > 0 ) {
        	HashSet currentPIDs = getConnectedPIDs();
        	currentPIDs.removeAll(m_DBMServerPIDSet);
        	if( waitHalfSeconds == 1 && currentPIDs.size() == 0 ) {
        		// nothing to do, wait a little, if it's not the last loop
        	}
        	else if( waitHalfSeconds == 1 && currentPIDs.size()>1 ) {
        		// more than one scheduler pid was added, it might be the DBM server that was used for starting scheduler
        		// wait a little, if it's not the last loop
        		StringBuffer errMsg = new StringBuffer("more than one new PID...: ");
        		for( Iterator iter = currentPIDs.iterator(); iter.hasNext(); ) {
        			// this loops only once since there is only one element in the set
					Integer element = (Integer) iter.next();
					errMsg.append(element.toString() +  ", ");
				}
        		
        		StringBuffer pidsBefore = new StringBuffer("PIDs before: ");
        		for( Iterator iter = m_DBMServerPIDSet.iterator(); iter.hasNext(); ) {
        			// this loops only once since there is only one element in the set
					Integer element = (Integer) iter.next();
					pidsBefore.append(element.toString() +  ", ");
				}
        		
        		errMsg.append(pidsBefore);
                addMessage(
                        getTestId(),
                        TestProtocol.ERROR,
						errMsg.toString());
        	}
        	else if( currentPIDs.size() == 1 ) {
        		// size is 1
        		for( Iterator iter = currentPIDs.iterator(); iter.hasNext(); ) {
        			// this loops only once since there is only one element in the set
					Integer element = (Integer) iter.next();
					m_SchedulerPid = element.intValue();
				}
        		return;
        	}

        	// wait
            --waitHalfSeconds;
            try {
                Thread.sleep(500);
            }
            catch( InterruptedException ire ) {
            	addMessage(
                        getTestId(),
                        TestProtocol.INFO,
                        "interrupted while waiting for scheduler PID");                	
            }        		
        }

        if( m_SchedulerPid == PID_UNDEFINED) {
            // error
            throw new SchedulerException("Error saving scheduler pid", "No scheduler pid found");
        }
    }
    
    protected String getTimeStringInFuture(int secondsFromNow ) {
        Calendar c = Calendar.getInstance();
        c.add(Calendar.SECOND, secondsFromNow);
        return m_DateFormatter.format(c.getTime());
    }

    /**
     * The two time strings must be correct (format "HH:mm:ss")!
     * 
     * @param aTimeString
     * @param otherTimeString
     * @return true if aTimeString represents a time that is earlier than otherTimeString
     */
    boolean isBefore( String aTimeString, String otherTimeString )
                                            throws SchedulerException {
        try {
            m_DateFormatter.parse(aTimeString);
            GregorianCalendar aCalender = new GregorianCalendar();
            GregorianCalendar otherCalender = new GregorianCalendar();
            aCalender.setTime(m_DateFormatter.parse(aTimeString));
            otherCalender.setTime(m_DateFormatter.parse(otherTimeString));

            // if we're on a new day...
            if( aCalender.get(Calendar.HOUR_OF_DAY) == 23 && otherCalender.get(Calendar.HOUR_OF_DAY) == 0)
                aCalender.roll(Calendar.HOUR_OF_DAY, 1);

            return otherCalender.after(aCalender);
        }
        catch( ParseException pe ) {
            throw new SchedulerException("time parsing error: " + pe.getMessage());
        }
    }


    protected void postTestCleanup() {

        // first delete job, there is only the one and only job with ID 0!!
        String cmdDeleteJob = CMD_delete_job + " 0";
        boolean isExpectedReply = m_DBM.execCmd(cmdDeleteJob, DBMUtility.OK_OK);
        String cmdAnswer = m_DBM.getAnswer();

        if( !isExpectedReply ) {
            addMessage(
                getTestId(),
                TestProtocol.ERROR,
                ERRTXT_CLEANUP,
                new SchedulerException(cmdDeleteJob, cmdAnswer));
        }

        isExpectedReply = m_DBM.execCmd(CMD_stop, DBMUtility.OK_OK);
        cmdAnswer = m_DBM.getAnswer();
        
        if( !isExpectedReply ) {
            addMessage(
                getTestId(),
                TestProtocol.ERROR,
                ERRTXT_CLEANUP,
                new SchedulerException(CMD_stop, cmdAnswer));
        }
        else {
            isExpectedReply = m_DBM.execCmd(CMD_shmInfo, DBMUtility.OK_OK);
            cmdAnswer = m_DBM.getAnswer();
        
            if( !isExpectedReply ) {
                addMessage(
                    getTestId(),
                    TestProtocol.ERROR,
                    ERRTXT_CLEANUP,
                    new SchedulerException(CMD_shmInfo, cmdAnswer));
            }
            else {
            	boolean pidIsRemoved = false;
            	int waitingHalfSeconds = 600; // allow scheduler minutes to remove its pid from playerlist
            	while( !pidIsRemoved && waitingHalfSeconds > 0 ) {
                    HashSet currentPIDs = getConnectedPIDs();
                    currentPIDs.removeAll(m_DBMServerPIDSet);
                    if( currentPIDs.size() > 0 ) {
                        // scheduler's pid is still there, go on waiting
                    	--waitingHalfSeconds;
                        try {
                            Thread.sleep(500);
                        }
                        catch( InterruptedException ire ) {
                        	addMessage(
                                    getTestId(),
                                    TestProtocol.INFO,
                                    "interrupted while waiting for scheduler to remove its PID");                	
                        }        		
                    	
                    }
                    else
                    	pidIsRemoved = true;
            	}
                if( !pidIsRemoved ) {
                    // scheduler's pid is still there, that is an error
                    addMessage(
                        getTestId(),
                        TestProtocol.ERROR,
                        ERRTXT_CLEANUP + " scheduler pid " +
                            m_SchedulerPid + " still exists.",
                        new SchedulerException(CMD_shmInfo, cmdAnswer));
                }
                else
                	m_SchedulerPid = PID_UNDEFINED;
            }
        }
        
        // undo result of scheduled command
        isExpectedReply = m_DBM.execCmd(SCD_CMD_UNDO, DBMUtility.OK_OK);
        cmdAnswer = m_DBM.getAnswer();
        if( !isExpectedReply ) {
            addMessage(
                getTestId(),
                TestProtocol.ERROR,
                ERRTXT_CLEANUP,
                new SchedulerException(CMD_shmInfo, cmdAnswer));
        }
    }

    protected String scheduleJob(int timeDeltaSeconds) throws SchedulerException {

        // start the scheduler
        if( !m_DBM.execCmd(CMD_start, DBMUtility.OK_OK) ) {
            throw new SchedulerException(CMD_start, m_DBM.getAnswer());
        }

        saveSchedulerPid();
        
        String cmdAnswer;
        boolean isExpectedReply;

        String timeString = getTimeStringInFuture(timeDeltaSeconds);
        isExpectedReply = m_DBM.execCmd(
                CMD_schedule +
                " " +
                timeString +
                " " +
                "\"" + SCD_CMD + "\"",
                DBMUtility.OK_OK);
        cmdAnswer = m_DBM.getAnswer();
        if( isExpectedReply ) {
            if( !cmdAnswer.trim().startsWith("0") ) {
                // first scheduled job should receive 0 (zero) as job ID
                throw new SchedulerException(
                    "Scheduling job. Expected job ID 0, but received: " +
                    "\"" + 
                    cmdAnswer +
                    "\"");
            }
        }       
        else
            throw new SchedulerException(SCD_CMD, cmdAnswer);    
        
        return timeString;        
    }
    
    /**
     * if there's something severe, we zip shared memory files,
     * dbm protocol, and scheduler
     */  
    protected void addSchedulerTestMessage(String pMessageId, char pMessageType, String pMessageText, Exception pException) {
        addMessage(pMessageId, pMessageType, pMessageText, pException);
        if( pMessageType == TestProtocol.ERROR || pMessageType == TestProtocol.ABORT ) {
            ZipOutputStream zos = null;
            try {
                ZipEntry[] schedulingEntry = new ZipEntry[4];
                schedulingEntry[0] = new ZipEntry( m_DB.getRunDir() + File.separator + FILE_DBMPRT );
                schedulingEntry[1] = new ZipEntry( m_DB.getIndepDataPath() + File.separator + "wrk" + File.separator + m_DB.getName() + EXTENSION_DBMSHI );
                schedulingEntry[2] = new ZipEntry( m_DB.getIndepDataPath() + File.separator + "wrk" + File.separator + m_DB.getName() + EXTENSION_DBMSHM );
                schedulingEntry[3] = new ZipEntry( m_DB.getRunDir() + File.separator + FILE_SCDPRT );
    
                zos = new ZipOutputStream(new FileOutputStream(getZipFile()));

                // now put data into the zip
                byte[] data = new byte[1024];
                int byteCount;
                BufferedInputStream bis = null;
            
                for( int count=0; count<schedulingEntry.length; ++count ) {
                    File f = new File(schedulingEntry[count].getName());
                    if( f.exists() ) {
                        zos.putNextEntry(schedulingEntry[count]);
                        bis = new BufferedInputStream(new FileInputStream(f));
                        while ((byteCount = bis.read(data, 0, data.length)) > -1) {
                            zos.write(data, 0, byteCount);
                        }
                    }                    
                }
                zos.close();        
            }
            catch( IOException ioe ) {
                addMessage(getTestId(), TestProtocol.ERROR,
                    "IOException caught while creating zip for uploading logs: " + ioe.getMessage());
            }
            catch( TestDatabaseException tde ) {
                addMessage(getTestId(), TestProtocol.ERROR,
                    "TestDatabaseException caught while creating zip for uploading logs: " + tde.getMessage());
            }
        }
    }
}
