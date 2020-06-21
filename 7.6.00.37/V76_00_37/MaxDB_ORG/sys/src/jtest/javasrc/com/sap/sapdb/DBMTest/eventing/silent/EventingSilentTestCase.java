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
package com.sap.sapdb.DBMTest.eventing.silent;

import java.sql.SQLException;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;

public abstract class EventingSilentTestCase extends DBMTestCase {

	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public EventingSilentTestCase() throws SQLException, TestDatabaseException {
		super();
		// TODO Auto-generated constructor stub
	}

	protected boolean isDispatcherConfiguredToRun() {
		m_DBM.execCmd("dbm_configget RunEventDispatcher", DBMUtility.OK_OK);
		return m_DBM.getAnswer().toLowerCase().indexOf("yes") >= 0;
	}

	protected boolean isDispatcherRunning() {
		boolean b = false;
		m_DBM.execCmd("auto_extend SHOW", DBMUtility.OK_OK);
	    if( m_DBM.getAnswer().indexOf(("ON")) >= 0 )
	    	b = true;
	    if( !b ) {
		    m_DBM.execCmd("auto_update_statistics SHOW", DBMUtility.OK_OK);
		    if( m_DBM.getAnswer().indexOf(("ON")) >= 0 )
		    	b = true;
	    }
		return b;
	}
}
