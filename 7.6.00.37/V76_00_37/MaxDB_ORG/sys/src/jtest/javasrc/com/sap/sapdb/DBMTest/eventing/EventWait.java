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

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.DBMTest.util.user.DBMUser;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * @author d026948
 *
 */
public class EventWait extends DBMTestCase {

	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public EventWait() throws SQLException, TestDatabaseException {
		super();
	}

	/**
	 * 
	 * @author d026948
	 *
	 * async execution of event_wait command with the passed user, this inner class could
	 * access event private members of the outer class. Not needed here
	 */
	class WaitingThread extends Thread {
		private DBMUser m_WaitingUser = null;
		
		public WaitingThread( DBMUser waitingUser) {
			m_WaitingUser = waitingUser;
		}
		
		public void run() {
			m_WaitingUser.execCmd("event_wait", DBMUtility.OK_OK);
		}
	}

	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.base.DBMTestCase#runDBMTest()
	 */
	protected void runDBMTest() {
		boolean eventFound = false;

		//  create one global try-catch block to handle exceptions
		try {
			// let a DBM user wait...
			DBMUser waitingUser = new DBMUser("waitUser", "waitUser");
			waitingUser.logon(300000L); // 5 minutes
			WaitingThread waiter = new WaitingThread(waitingUser);
			waiter.start();

			// execute a event_set command to provoke event creation of type "EVENT"
			m_DBM.execCmd("event_set DBFILLINGABOVELIMIT HIGH 18", DBMUtility.OK_OK);
			
			// wait for the waiter to return
			waiter.join();
			
			// check if there is the correct answer
			String waitResult = waitingUser.getLastCommandReply();
			
			// release the event task expicitly and terminate the DBMServer
			waitingUser.execCmd("event_release", DBMUtility.OK_OK);
			waitingUser.logoff();

			eventFound = waitResult.indexOf("NAME=EVENT") >= 0;
			if (eventFound) {
				super.addMessage(super.getTestId(), TestProtocol.INFO,
						"waiting succeeded\n" + waitResult);
			} else {
				super.addMessage(super.getTestId(), TestProtocol.ERROR,
						"waiting failed\n" + waitResult);
			}
	
		}
		// catch any kind of exception and write it to the protocol
		catch (Exception e) {
			// standard error handler
			super.handleExceptions(e);
		}

		// clean up, i.e. close statements and connections
		finally {
			m_DBM.execCmd("event_delete DBFILLINGABOVELIMIT 18", DBMUtility.OK_OK);
			try {
				// print all parameters from the parameter object to the protocol.
				super.printParametersToProtocol();
			} catch (Exception e) {
				// do nothing
			}
		}
	}
}
