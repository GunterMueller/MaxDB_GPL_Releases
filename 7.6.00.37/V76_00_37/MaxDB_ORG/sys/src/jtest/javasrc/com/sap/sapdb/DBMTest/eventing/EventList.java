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
public class EventList extends DBMTestCase {

	private static final String m_EventName = "DBFILLINGABOVELIMIT";
	private static final String m_EventPriority = "MEDIUM";
	private static final String m_EventParameter = "23";
	
	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public EventList() throws SQLException, TestDatabaseException {
		super();
	}

	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.base.DBMTestCase#runDBMTest()
	 */
	protected void runDBMTest() {
		boolean eventFound = false;

		// create one global try-catch block to handle exceptions
		try {
			m_DBM.execCmd(
					"event_set " +
					m_EventName +
					" " +
					m_EventPriority + 
					" " +
					m_EventParameter,
					DBMUtility.OK_OK);

			// checking whether event is shown in event list
			m_DBM.execCmd("event_list", DBMUtility.OK_OK, false);
			String eventList = m_DBM.getAnswer();
			StringTokenizer stParam = new StringTokenizer(eventList, "\n");
			while (stParam.hasMoreTokens()) {
				String line = stParam.nextToken();
				if (line.indexOf(m_EventName) >= 0) {
					if( line.indexOf(m_EventPriority) >= 0 && line.indexOf(m_EventParameter) >= 0) {
						eventFound = true;
						break;
					}
				}
			}

			if( eventFound ) {
				super.addMessage(super.getTestId(), TestProtocol.INFO,
						"listing event (before) succeeded\n" + eventList);
			} else {
				super.addMessage(super.getTestId(), TestProtocol.ERROR,
						"listing event (before) failed\n" + eventList);
			}

			//Deleting event
			m_DBM.execCmd(
					"event_delete " +
					m_EventName +
					" " +
					m_EventParameter,
					DBMUtility.OK_OK);

			// checking whether event is shown in event list
			m_DBM.execCmd("event_list", DBMUtility.OK_OK, false);
			eventList = m_DBM.getAnswer();
			eventFound = false;
			stParam = new StringTokenizer(eventList, "\n");
			while (stParam.hasMoreTokens()) {
				String line = stParam.nextToken().toString().toUpperCase();
				if (line.indexOf(m_EventName) >= 0) {
					if( line.indexOf(m_EventPriority) >= 0 && line.indexOf(m_EventParameter) >= 0) {
						eventFound = true;
						break;
					}
				}
			}

			if( !eventFound ) {
				super.addMessage(super.getTestId(), TestProtocol.INFO,
						"listing event (after) succeeded\n" + eventList);
			} else {
				super.addMessage(super.getTestId(), TestProtocol.ERROR,
						"listing event (after) failed\n" + eventList);
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
