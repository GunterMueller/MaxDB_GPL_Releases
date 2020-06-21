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
public class AddCommand extends EventingClientTestCase {

	private static final long WAITINGMILLIS   = 30000L; // 30 seconds
	private static final String ACTION        = "never_executed_in_this_test";
	private static final String NAMECONDITION = "DATABASEFULL";
	private static final String CFGFILE       = "add.cfg";
	private static final String FILEVERSION   = "FILEVERSION";
	
	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public AddCommand() throws SQLException, TestDatabaseException {
		super();
	}

	
	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.base.DBMTestCase#runDBMTest()
	 */
	public void runEventingClientTestCase() {
		
		// build the command line
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
		
		// event dispatcher returned successfully
		// check the answer
		String[] answerLines = sb.toString().split("\\n");
		if (2 != answerLines.length || !answerLines[0].trim().equals("OK") || !answerLines[1].trim().equals("0")) {
			// not the expected answer
			addMessage(
					"event dispatcher unexpected answer", 
					TestProtocol.ERROR,
					"answer:\n" + sb.toString());
			m_UploadFiles = true;
			return;
		}
		
		// check the contents of the configuration file
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
		
		if( 2 != cfg.size() ) {
			addMessage(
					"unexpected contents of configuration file, should have 2 lines, but has " + cfg.size(),
					TestProtocol.ERROR,
					getFile(CFGFILE));
			m_UploadFiles = true;
			return;
		}

		// check the file version
		String fileVersion = cfg.getProperty(FILEVERSION);
		String kernelVersion = null;
		try {
			kernelVersion = m_DB.getVersion();
			
		} catch (Exception e) {
			addMessage(
					"cannot check kernel version", 
					TestProtocol.ERROR,
					FILEVERSION + " is " + fileVersion,
					e);
			m_UploadFiles = true;
			return;
		}

		// eliminate first digit of build number
		kernelVersion = kernelVersion.substring(0, kernelVersion.lastIndexOf('.')+1) + kernelVersion.substring(kernelVersion.lastIndexOf('.')+2);
		if( !kernelVersion.equals(fileVersion) ) {
			addMessage(
					"wrong file version", 
					TestProtocol.ERROR,
					"expected " + kernelVersion + "; got " + fileVersion);
			m_UploadFiles = true;
			return;
		}
		
		// check the dispatcher line
		String action = cfg.getProperty("0000000000.((Name == \"" + NAMECONDITION +"\"))");
		if( null == action || !action.equals(ACTION)) {
			addMessage(
					"wrong dispatching line", 
					TestProtocol.ERROR,
					"expected " + ACTION + "; got " + action);
			m_UploadFiles = true;			
		}
	}
}

