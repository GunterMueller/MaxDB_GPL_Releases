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
public class ListCommand extends EventingClientTestCase {

	private static final long WAITINGMILLIS   = 30000L; // 30 seconds
	private static final String ACTION        = "never_executed_in_this_test";
	private static final String NAMECONDITION = "DATABASEFULL";
	private static final String CFGFILE       = "list.cfg";

	
	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public ListCommand() throws SQLException, TestDatabaseException {
		super();
	}

	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.eventing.client.EventingClientTestCase#runEventingClientTestCase()
	 */
	protected void runEventingClientTestCase() {

		// first make one entry in the configuration file
		m_CommandLine.setCommand("add");
		m_CommandLine.setConfigFile(CFGFILE);
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
			return;
		}
		
		if( 0 != ed.getExitValue() ) {
			// event dispatcher returned unsuccessfully
			addMessage(
					"event dispatcher returned unsuccessfully", 
					TestProtocol.ERROR,
					"return code: " + ed.getExitValue() + "\n" + sb.toString());
			m_UploadFiles = true;
			return;
		}

		// event dispatcher returned successfully, now get the list
		m_CommandLine.setCommand("list");
		m_CommandLine.setConfigFile(CFGFILE);
		// call the event dispatcher
		tokens = m_CommandLine.toTokenArray();
		sb = new StringBuffer();
		EventDispatcher ed1 = new EventDispatcher(tokens, sb);
		try {
			Thread execThread = ed1.execAsync();
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
			return;
		}
		
		if( 0 != ed1.getExitValue() ) {
			// event dispatcher returned unsuccessfully
			addMessage(
					"event dispatcher returned unsuccessfully", 
					TestProtocol.ERROR,
					"return code: " + ed1.getExitValue() + "\n" + sb.toString());
			m_UploadFiles = true;
			return;
		}

		// examine the answer
		String[] answerLines = sb.toString().split("\\n");
		do {
			if( 4 != answerLines.length )
				break;
			if( !answerLines[0].trim().equals("OK") )
				break;
			if( !answerLines[1].trim().equals("ID 0") )
				break;
			if( !answerLines[2].trim().equals("condition: ((Name == \"DATABASEFULL\"))") )
				break;
			if( !answerLines[3].trim().equals("Command: never_executed_in_this_test") )
				break;

			return;
		} while(true);

		addMessage(
				"event dispatcher unexpected answer", 
				TestProtocol.ERROR,
				"answer:\n" + sb.toString());
		m_UploadFiles = true;
	}
}
