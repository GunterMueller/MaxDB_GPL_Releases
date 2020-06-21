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
package com.sap.sapdb.DBMTest.memory;

import java.sql.SQLException;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.DBMTest.util.user.DBMUser;
import com.sap.sapdb.DBMTest.util.user.DBMUserManagementException;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * @author d026948
 *
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
public class LeakDiscoverer extends DBMTestCase {

	protected DBMUser m_StressingUser = null;
	protected double m_BytesBefore = 0.0;
	protected double m_BytesIntermediate = 0.0;
	protected double m_BytesAfter = 0.0;
	protected int m_IntermediateIteration = -1;
	protected int m_Iterations = 1000; // number of iterations per check
	
	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public LeakDiscoverer() throws SQLException, TestDatabaseException {
		super();
		// TODO Auto-generated constructor stub
	}

	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.base.DBMTestCase#runDBMTest()
	 */
	protected void runDBMTest() {
		
		// check, whether the test can run on the current platform
		m_DBM.execCmd("dbm_getmemoryusage", DBMUtility.OK_ERR_IGNORE);
		if( !m_DBM.getAnswer().trim().matches("(\\d)+")) { // at least one digit, but nothing else
	    	addMessage(
	    			"this test cannot run on this platform",
	                TestProtocol.WARNING,
					"sorry...");
	    	return;
		}
		
		try {
			m_StressingUser = new DBMUser("stressingUser", "stressingUser");
		}
		catch( DBMUserManagementException due ) {
	    	addMessage(
	    			"could not create stressing user",
	                TestProtocol.ERROR,
					"something went wrong...",
					due);
		}

		boolean loopOkay = false;
		
		/**
		 * test dbm_version
		 */
		prepareStep("dbm_version");
		loopOkay = true;
		m_Iterations = 1000;
		for(int i=0; loopOkay && i<m_Iterations; ++i) {
			loopOkay = m_StressingUser.execCmd("dbm_version", DBMUtility.OK_OK);
			if( 1 == i )
				checkIntermediate(i);
			
		}
		endStep();

		/**
		 * test param_getfull
		 */
		prepareStep("param_getfull");
		loopOkay = true;
		m_Iterations = 100;
		for(int i=0; loopOkay && i<m_Iterations; ++i) {
			loopOkay = m_StressingUser.execCmd("param_getfull _DIAG_SEM", DBMUtility.OK_OK);
			if( 1 == i )
				checkIntermediate(i);
		}
		endStep();
		
		/**
		 * test user_logon
		 */
		prepareStep("user_logon");
		loopOkay = true;
		m_Iterations = 1000;
		for(int i=0; loopOkay && i<m_Iterations; ++i) {
			loopOkay = m_StressingUser.execCmd(
							"user_logon stressingUser,stressingUser", DBMUtility.OK_OK);
			if( 1 == i )
				checkIntermediate(i);
		}
		endStep();
		
		/**
		 * test sql session
		 */
		prepareStep("sql session");
		loopOkay = true;
		m_Iterations = 1000;
		String user = getDatabase().getDBAUser();
		String password = getDatabase().getDBAPassword();
		for(int i=0; loopOkay && i<m_Iterations; ++i) {
			loopOkay = m_StressingUser.execCmd(
					"sql_connect " + user +	"," + password,
					DBMUtility.OK_OK);
			loopOkay = loopOkay && m_StressingUser.execCmd(
					"sql_execute select * from dual where user = '" + user + "'",
					DBMUtility.OK_OK);
			loopOkay = loopOkay && m_StressingUser.execCmd("sql_release", DBMUtility.OK_OK);
			if( 1 == i )
				checkIntermediate(i);			
		}
		endStep();

		/**
		 * test backup history
		 */
		prepareStep("backup history");
		loopOkay = true;
		m_Iterations = 100;
		for(int i=0; loopOkay && i<m_Iterations; ++i) {
			loopOkay = m_StressingUser.execCmd("backup_history_open", DBMUtility.OK_OK);
			loopOkay = loopOkay && m_StressingUser.execCmd("backup_history_close", DBMUtility.OK_OK);
			if( 1 == i )
				checkIntermediate(i);
		}
		endStep();
		
		/**
		 * test event session
		 */
		prepareStep("event session");
		loopOkay = true;
		m_Iterations = 1000;
		for(int i=0; loopOkay && i<m_Iterations; ++i) {
			loopOkay = m_StressingUser.execCmd("event_available", DBMUtility.OK_ERR_IGNORE);
			loopOkay = loopOkay && m_StressingUser.execCmd("event_release", DBMUtility.OK_ERR_IGNORE);
			if( 1 == i )
				checkIntermediate(i);
		}
		endStep();
	}

	protected double getCurrentlyUsedMemory() {
		double usedMemory = 0.0;

		// call it 3 times
		for( int i=0; i<3; ++i) {
			if( m_StressingUser.execCmd("dbm_getmemoryusage", DBMUtility.OK_OK) )
				usedMemory = Double.parseDouble(m_StressingUser.getLastCommandReply().trim());
		}
		return usedMemory;
	}
	
	protected void prepareStep(String stepName) {
		this.nextStep(stepName);
		addMessage(
    			"step " + getStep(),
                TestProtocol.TRACE,
				"step start");
		try {
			m_StressingUser.logon(300000L); // 5 minutes
		}
		catch( DBMUserManagementException due ) {
	    	addMessage(
	    			"could not logon stressing user",
	                TestProtocol.ERROR,
					getStep(),
					due);
		}
		m_BytesBefore = getCurrentlyUsedMemory();
	}
	
	protected void checkIntermediate(int iteration) {
		m_IntermediateIteration = iteration;
		m_BytesIntermediate = getCurrentlyUsedMemory();
	}
	
	protected void endStep() {
		m_BytesAfter = getCurrentlyUsedMemory();
				
		try {
			m_StressingUser.logoff();
		}
		catch( DBMUserManagementException due ) {
	    	addMessage(
	    			"could not logoff stressing user",
	                TestProtocol.ERROR,
					getStep(),
					due);
		}
		
		if( m_IntermediateIteration == -1 ) {
			addMessage(
	    			"memory after " + m_Iterations + " iterations vs. memory before",
	    			m_BytesBefore==m_BytesAfter?TestProtocol.TRACE:TestProtocol.WARNING,
					Double.toString(m_BytesAfter / m_BytesBefore * 100.0)+"%\n" +
					(m_BytesBefore/1024.0) + " kb before\n" + 
					(m_BytesAfter/1024.0) + " kb after " + m_Iterations +"\n");
		}
		else {
			addMessage(
	    			"memory after " + m_Iterations + " iterations vs. memory before",
	    			TestProtocol.TRACE,
					Double.toString(m_BytesAfter / m_BytesBefore * 100.0)+"%\n" +
					(m_BytesBefore/1024.0) + " kb before\n" + 
					(m_BytesAfter/1024.0) + " kb after " + m_Iterations +"\n");
			addMessage(
	    			"memory after " + m_Iterations + " iterations vs. memory after " + m_IntermediateIteration + " iterations",
	    			m_BytesIntermediate==m_BytesAfter?TestProtocol.TRACE:TestProtocol.WARNING,
					Double.toString(m_BytesAfter / m_BytesIntermediate * 100.0)+"%\n" +
					(m_BytesIntermediate/1024.0) + " kb after " + m_IntermediateIteration + " iterations " + "\n" + 
					(m_BytesAfter/1024.0) + " kb after " + m_Iterations +"\n");
		
		}
		addMessage(
    			"step " + getStep(),
                TestProtocol.TRACE,
				"step end");
		m_IntermediateIteration = -1;
	}
}
