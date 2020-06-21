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

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.sql.SQLException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.eventing.client.util.CommandLine;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * @author d026948
 *
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
public abstract class EventingClientTestCase extends DBMTestCase {

	private String m_IndepProgPathBin = null;
	protected boolean m_UploadFiles = false;
	protected CommandLine m_CommandLine = null;
	
	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public EventingClientTestCase() throws SQLException, TestDatabaseException {
		super();
	}

	public void runDBMTest() {

		// preparations
		// set folder for files used in this test
		CommandLine.initialize(super.getDataOutputPath());

		m_CommandLine =	new CommandLine(m_DB.getDBMUser()+","+m_DB.getDBMPassword(), m_DB.getName());
		
		// run the testcase
		m_UploadFiles = false;
		runEventingClientTestCase();
		if( m_UploadFiles ) {
			File outputFolder = new File(super.getDataOutputPath());
			if( outputFolder.exists() ) {
				File[] outputFiles = outputFolder.listFiles();
				if( outputFiles.length > 0 ) {
					ZipOutputStream zos = null;
					try {
						zos = new ZipOutputStream(new FileOutputStream(getZipFile()));
						for( int fileCount=0; fileCount<outputFiles.length; ++fileCount) {
							zos.putNextEntry(new ZipEntry(outputFiles[fileCount].getName()));
							byte[] data = new byte[1024];
			                int byteCount;
			                BufferedInputStream bis = null;
	                        bis = new BufferedInputStream(new FileInputStream(outputFiles[fileCount]));
	                        while ((byteCount = bis.read(data, 0, data.length)) > -1) {
	                            zos.write(data, 0, byteCount);
	                        }
	                        bis.close();
						}
		                zos.close();
					}
		            catch( IOException ioe ) {
		                addMessage(getTestId(), TestProtocol.ERROR,
		                    "IOException caught while creating zip for uploading files: " + ioe.getMessage());
		            }
		            catch( TestDatabaseException tde ) {
		                addMessage(getTestId(), TestProtocol.ERROR,
		                    "TestDatabaseException caught while creating zip for uploading files: " + tde.getMessage());
		            }
				}
            }
		}
		
		// cleanup
		CommandLine.cleanup();
	}
	
	protected abstract void runEventingClientTestCase();
	
	protected String getIndeProgPathBin() {
		if( null != m_IndepProgPathBin)
			return m_IndepProgPathBin;
		else {
			boolean pathOkay = m_DBM.execCmd("dbm_getpath IndepProgPath", DBMUtility.OK_OK);
			m_IndepProgPathBin = m_DBM.getAnswer().trim(); 
		    if( !pathOkay ) {
		    	addMessage(
		                "error getting IndepProgPath from DBMServer",
		                TestProtocol.ERROR,
						m_IndepProgPathBin);
		    }
		    else {
		    	m_IndepProgPathBin = m_IndepProgPathBin + File.separator + "bin";
		    }
			
		}
		return m_IndepProgPathBin;
	}

	protected String getFile(String fileName) {
		return super.getDataOutputPath() + File.separator + fileName;
	}
}