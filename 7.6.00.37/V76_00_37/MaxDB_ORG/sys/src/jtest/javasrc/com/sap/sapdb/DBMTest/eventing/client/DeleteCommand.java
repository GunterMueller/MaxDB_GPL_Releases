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

import java.io.FileInputStream;
import java.sql.SQLException;

import com.sap.sapdb.DBMTest.eventing.client.util.DispatcherConfiguration;
import com.sap.sapdb.DBMTest.eventing.client.util.EventDispatcher;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * @author d026948
 *
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
public class DeleteCommand extends EventingClientTestCase {

	private static final long WAITINGMILLIS   = 30000L; // 30 seconds
	private static final String ACTION        = "never_executed_in_this_test";
	private static final String NAMECONDITION = "DATABASEFULL";
	private static final String CFGFILE       = "delete.cfg";
	
	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public DeleteCommand() throws SQLException, TestDatabaseException {
		super();
	}

	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.eventing.client.EventingClientTestcase#runEventingClientTestCase()
	 */
	protected void runEventingClientTestCase() {

		// first, add two entries
		m_CommandLine.setCommand("add");
		m_CommandLine.setConfigFile(CFGFILE);
		m_CommandLine.setEventAction(ACTION);
		m_CommandLine.setEventNameCondition(NAMECONDITION);
		
		// call the event dispatcher
		String[] tokens = m_CommandLine.toTokenArray();
		StringBuffer sb = new StringBuffer();
		EventDispatcher ed1 = new EventDispatcher(tokens, sb);
		try {
			Thread execThread = ed1.execAsync();
			execThread.join(WAITINGMILLIS);
		}
		catch( Exception e) {
			// something very bad happened
			addMessage(
					"error adding first entry to configuration",
					TestProtocol.ERROR,
					sb.toString(),
					e);
			m_UploadFiles = true;
			return;
		}

		if( 0 != ed1.getExitValue() ) {
			// event dispatcher returned unsuccessfully
			addMessage(
					"error adding first entry to configuration", 
					TestProtocol.ERROR,
					"return code: " + ed1.getExitValue() + "\n" + sb.toString());
			m_UploadFiles = true;
			return;
		}

		
		sb = new StringBuffer();
		EventDispatcher ed2 = new EventDispatcher(tokens, sb);
		try {
			Thread execThread = ed2.execAsync();
			execThread.join(WAITINGMILLIS);
		}
		catch( Exception e) {
			// something very bad happened
			addMessage(
					"error adding second entry to configuration",
					TestProtocol.ERROR,
					sb.toString(),
					e);
			m_UploadFiles = true;
			return;
		}

		if( 0 != ed2.getExitValue() ) {
			// event dispatcher returned unsuccessfully
			addMessage(
					"error adding second entry to configuration", 
					TestProtocol.ERROR,
					"return code: " + ed2.getExitValue() + "\n" + sb.toString());
			m_UploadFiles = true;
			return;
		}

		// now we should have one version line and 2 configuration lines
		DispatcherConfiguration cfg = new DispatcherConfiguration();
		try {
			FileInputStream fis = new FileInputStream(getFile(CFGFILE));
			cfg.load(fis);
			fis.close();
		} catch (Exception e) {
			addMessage(
					"could not load configuration file", 
					TestProtocol.ERROR,
					getFile(CFGFILE),
					e);
			m_UploadFiles = true;
			return;
		}
		
		if( 3 != cfg.size()) {
			addMessage(
					"unexpected contents of configuration file, should have 3 lines, but has " + cfg.size(), 
					TestProtocol.ERROR,
					getFile(CFGFILE));
			m_UploadFiles = true;
			return;
		}

		// now delete the line that we added first
		m_CommandLine.setCommand("delete");
		m_CommandLine.setEntryIndex(0); // first entry was assigned the index 0
		tokens = m_CommandLine.toTokenArray();
				
		sb = new StringBuffer();
		EventDispatcher ed3 = new EventDispatcher(tokens, sb);
		try {
			Thread execThread = ed3.execAsync();
			execThread.join(WAITINGMILLIS);
		}
		catch( Exception e) {
			// something very bad happened
			addMessage(
					"error adding second entry to configuration",
					TestProtocol.ERROR,
					sb.toString(),
					e);
			m_UploadFiles = true;
			return;
		}

		if( 0 != ed3.getExitValue() ) {
			// event dispatcher returned unsuccessfully
			addMessage(
					"error removing first entry from configuration", 
					TestProtocol.ERROR,
					"return code: " + ed3.getExitValue() + "\n" + sb.toString());
			m_UploadFiles = true;
			return;
		}

		// now recheck the confgiuration file
		cfg.clear();
		try {
			FileInputStream fis = new FileInputStream(getFile(CFGFILE));
			cfg.load(fis);
			fis.close();
		}
		catch (Exception e) {
			addMessage(
					"could not load configuration file", 
					TestProtocol.ERROR,
					getFile(CFGFILE),
					e);
			m_UploadFiles = true;
			return;
		}
		
		if( 2 != cfg.size() ) {
			addMessage(
					"unexpected contents of configuration file, should have 2 lines, but has " + cfg.size(), 
					TestProtocol.ERROR,
					getFile(CFGFILE));
			m_UploadFiles = true;
			return;
		}
		
		String eventCondition = "0000000001.((Name == \"" + NAMECONDITION +"\"))";
		String action = cfg.getProperty(eventCondition);
		if( null == action || !action.equals(ACTION)) {
			addMessage(
					"wrong dispatching line", 
					TestProtocol.ERROR,
					"expected key not found: " + eventCondition);
			m_UploadFiles = true;			
		}
	}
}
