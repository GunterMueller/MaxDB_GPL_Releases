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
package com.sap.sapdb.DBMTest.eventing.client;

import java.sql.SQLException;

import com.sap.sapdb.DBMTest.eventing.client.util.EventDispatcher;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * @author d026948
 *
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
public class StartStateStop extends EventingClientTestCase {

	//private EventDispatcher m_ListeningDispatcher = null;
	
	private static final long WAITINGMILLIS   = 300000L; // 5 minutes
	private static final String ACTION        = "never_executed_in_this_test";
	private static final String NAMECONDITION = "DATABASEFULL";
	private static final String CFGFILE       = "startstatestop.cfg";
	private static final String LOGFILE       = "startstatestop.log";
	
	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public StartStateStop() throws SQLException, TestDatabaseException {
		super();
	}

	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.eventing.client.EventingClientTestCase#runEventingClientTestCase()
	 */
	protected void runEventingClientTestCase() {

		// first prepare the configuration file
		m_CommandLine.setConfigFile(CFGFILE);
		if( !prepareConfiguration() )
			return;

		// check the event dispatchers that the DB knows of (should be none here)
		if( !checkState(0) )
			return;
		
		// now start the listening dispatcher
		m_CommandLine.setCommand("start");
		m_CommandLine.setLogFile(LOGFILE);
		String[] listenerCommand = m_CommandLine.toTokenArray();
		StringBuffer listenerReply = new StringBuffer();
		EventDispatcher listeningDispatcher = new EventDispatcher(listenerCommand,listenerReply);
		Thread execThread = listeningDispatcher.execAsync();

		
		if( !sleep(5000) )
			return;

		// expecting one event dispatcher now
		if( !checkState(1) )
			return;

		// now stop the listening dispatcher
		if( !sendStopCommand() )
			return;

		if( !sleep(5000) )
			return;

		// expecting no dispatcher now
		if( !checkState(0) )
			return;

		if( listeningDispatcher.isAlive()) {
			addMessage(
					"listening event dispatcher still running",
					TestProtocol.ERROR,
					listenerReply.toString());
			m_UploadFiles = true;
		}
		else if( 0 != listeningDispatcher.getExitValue() ) {
			addMessage(
					"listening event dispatcher returned unsuccessfully",
					TestProtocol.ERROR,
					"return code: " + listeningDispatcher.getExitValue() + "\n" + listenerReply.toString());
			m_UploadFiles = true;
		}
	}


	boolean sendStopCommand() {
		m_CommandLine.setCommand("stop");
		String[] tokens = m_CommandLine.toTokenArray();
		StringBuffer sb = new StringBuffer();
		EventDispatcher ed = new EventDispatcher(tokens, sb);
		try {
			Thread execThread = ed.execAsync();
			execThread.join(WAITINGMILLIS);
		}
		catch( Exception e) {
			// something very bad happened
			addMessage(
					"error stopping listening dispatcher",
					TestProtocol.ERROR,
					sb.toString(),
					e);
			m_UploadFiles = true;
			return false;
		}
		
		if( 0 != ed.getExitValue() ) {
			// event dispatcher returned unsuccessfully
			addMessage(
					"event dispatcher returned unsuccessfully from stopping listening dispatcher", 
					TestProtocol.ERROR,
					"return code: " + ed.getExitValue() + "\n" + sb.toString());
			m_UploadFiles = true;
			return false;
		}
		return true;
	}
	
	boolean checkState(int expectedDispatchers) {
		// call the event dispatcher

		m_CommandLine.setCommand("state");
		String[] tokens = m_CommandLine.toTokenArray();
		StringBuffer sb = new StringBuffer();
		EventDispatcher ed = new EventDispatcher(tokens, sb);
		try {
			Thread execThread = ed.execAsync();
			execThread.join(WAITINGMILLIS);
		}
		catch( Exception e) {
			// something very bad happened
			addMessage(
					"error checking state",
					TestProtocol.ERROR,
					sb.toString(),
					e);
			m_UploadFiles = true;
			return false;
		}
		
		if( 0 != ed.getExitValue() ) {
			// event dispatcher returned unsuccessfully
			addMessage(
					"event dispatcher returned unsuccessfully from checking state", 
					TestProtocol.ERROR,
					"return code: " + ed.getExitValue() + "\n" + sb.toString());
			m_UploadFiles = true;
			return false;
		}
		
		// event dispatcher returned successfully
		// check the answer
		String[] answerLines = sb.toString().split("\\n");
		do {
			if( 1 + 5*expectedDispatchers != answerLines.length )
				break;
			if( !answerLines[0].trim().equals("OK") )
				break;
			
			if( expectedDispatchers != 0 ) {
				boolean noError = true;
				for( int i=0; noError && i < expectedDispatchers; ++i) {
					if( !answerLines[1+i*5+0].trim().startsWith("Instance ID") )
						noError = false;
					if( noError && !answerLines[1+i*5+1].trim().startsWith("user:") )
						noError = false;
					if( noError && !answerLines[1+i*5+2].trim().startsWith("host:") )
						noError = false;
					if( noError && !answerLines[1+i*5+3].trim().startsWith("configuration file:") )
						noError = false;
					if( noError && !answerLines[1+i*5+4].trim().startsWith("log file:") )
						noError = false;
				}
				if( !noError )
					break;
			}
			return true;
		} while(true);

		// not the expected answer
		addMessage(
				"event dispatcher unexpected answer", 
				TestProtocol.ERROR,
				"answer:\n" + sb.toString());
		m_UploadFiles = true;
		return false;
	}
	
	boolean sleep(long millis) {
		try {
			Thread.sleep(millis);
			return true;
		}
		catch( InterruptedException ie) {
			addMessage(
					"someone woke me up",
					TestProtocol.ERROR,
					"this should not happen here (planned sleepign time: " + millis + "ms)",
					ie);
			m_UploadFiles = true;
		}
		return false;
	}
	
	boolean prepareConfiguration() {
		// build the command line
		m_CommandLine.setCommand("add");
		m_CommandLine.setEventAction(ACTION);
		m_CommandLine.setEventNameCondition(NAMECONDITION);

		// call the event dispatcher
		String[] tokens = m_CommandLine.toTokenArray();
		StringBuffer sb = new StringBuffer();
		EventDispatcher ed = new EventDispatcher(tokens, sb);
		try {
			Thread execThread = ed.execAsync();
			execThread.join(WAITINGMILLIS);
		}
		catch( Exception e) {
			// something very bad happened
			addMessage(
					"error executing event dispatcher",
					TestProtocol.ERROR,
					sb.toString(),
					e);
			m_UploadFiles = true;
			return false;
		}
		
		if( 0 != ed.getExitValue() ) {
			// event dispatcher returned unsuccessfully
			addMessage(
					"event dispatcher returned unsuccessfully", 
					TestProtocol.ERROR,
					"return code: " + ed.getExitValue() + "\n" + sb.toString());
			m_UploadFiles = true;
			return false;
		}
		return true;
	}
}
