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
package com.sap.sapdb.DBMTest.eventing;

import java.sql.SQLException;
import java.util.StringTokenizer;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * @author d026948
 *
 */
public class EventRelease extends DBMTestCase {

	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public EventRelease() throws SQLException, TestDatabaseException {
		super();
	}

	private String eventTaskExists() {
	    String eventDBMSrvPid = null;
		String line = null;
	    m_DBM.execCmd("show tasks", DBMUtility.OK_OK, false);
		StringTokenizer stParam = new StringTokenizer(m_DBM.getAnswer(), "\n");

		boolean pidFound = false;
		while (stParam.hasMoreTokens()) {
			line = stParam.nextToken();
			int eventIndex = line.indexOf("Event");
			if (eventIndex >= 0) {

				// the the PID of the dbmserver that is connected
				int firstPidDigit = eventIndex + line.substring(eventIndex).indexOf(' ');
				while( line.charAt(firstPidDigit) == ' ' )
				    ++firstPidDigit;
				int behindLastPidDigit = firstPidDigit + line.substring(firstPidDigit).indexOf(' ');
				int pid = -1;
				while( !pidFound ) {
					eventDBMSrvPid = line.substring(firstPidDigit, behindLastPidDigit).trim();
					if( eventDBMSrvPid.length() == 0 )
						break;
					try {
						pid = Integer.parseInt(eventDBMSrvPid);
						pidFound = true;
					}
					catch( NumberFormatException nfe ) {
						--behindLastPidDigit;
					}
				}
				if( pidFound && (pid==0) )
					pidFound = false;
				
				break;
			}
		}
		
		if( pidFound )
		    return Integer.toHexString(Integer.parseInt(eventDBMSrvPid));
		else
		    return null;
	}
	
	
	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.base.DBMTestCase#runDBMTest()
	 */
	protected void runDBMTest() {
	    
		//  create one global try-catch block to handle exceptions
		try {
		    String eventDBMSrvPid = eventTaskExists();

			if (eventDBMSrvPid != null) {
				super.addMessage("initial 'show tasks'", TestProtocol.ERROR,
						"Event task found, DBMServer PID: " + eventDBMSrvPid + '.');
			} else {
				super.addMessage("initial 'show tasks'", TestProtocol.INFO,
						"Event task not found");
			}

			// create an event task in kernel
			m_DBM.execCmd("event_available", DBMUtility.ERR_EVENTNONEAVAIL,	false);
			
			eventDBMSrvPid = eventTaskExists();
			
			if (eventDBMSrvPid != null) {
				super.addMessage("second 'show tasks'", TestProtocol.INFO,
						"Event task found, DBMServer PID: " + eventDBMSrvPid + '.');
			} else {
				super.addMessage("second 'show tasks'", TestProtocol.ERROR,
						"Event task not found");
			}

			// remove the event task in kernel
			m_DBM.execCmd("event_release", DBMUtility.OK_OK, false);

			eventDBMSrvPid = eventTaskExists();

			if (eventDBMSrvPid != null) {
				super.addMessage("final 'show tasks'", TestProtocol.ERROR,
						"Event task found, DBMServer PID: " + eventDBMSrvPid + '.');
			} else {
				super.addMessage("final 'show tasks'", TestProtocol.INFO,
						"Event task not found");
			}
		}
		// catch any kind of exception and write it to the protocol
		catch (Exception e) {
			// standard error handler

			super.handleExceptions(e);
		}

		// clean up, i.e. close statements and connections
		finally {
			try {
				// print all parameters from the parameter object to the protocol.
				super.printParametersToProtocol();
			} catch (Exception e) {
				// do nothing
			}
		}
	}
}