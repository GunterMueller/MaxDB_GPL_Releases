/*!
	\file    NotConfiguredBackupTools.java
	\author  TiloH
	\ingroup DBM Server test
	\brief   

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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

package com.sap.sapdb.DBMTest.backup.external;

import java.sql.SQLException;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * @author d025561
 * 
 * The following test case creates a backup medium for all external backup tools known by the
 * DBM Server and tries to use all three backup media/backup tools. These tries must fail, because
 * the backup tools are not comfigured before. Furthermore the DBA History is switched on to check,
 * if the DBA History writing during usage of external backup tools is not chrashing the DBM Server
 * 
 */
public class NotConfiguredBackupTools extends DBMTestCase {

	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public NotConfiguredBackupTools() throws SQLException, TestDatabaseException {
		super();
	}

	/**
	 * @see com.sap.sapdb.testframe.testcase.TestCase#run()
	 */
	public void runDBMTest()
	{
		String os = System.getProperty("os.name");
		super.addMessage(super.getTestId(), TestProtocol.INFO, "Operating System name for Java is '" + os+"'.");

		//prepare the actual test
		try
		{
			String pipeDir = null;
			
			if(os.toUpperCase().startsWith("WINDOWS"))
				pipeDir="\\\\.\\pipe\\";
			else
				pipeDir=m_DB.getRunDir()+"/";
				
			//switch on the DBA History (TODO can be removed, when test frame is doing this for all DBM tests) 
		    m_DBM.execCmd("dbm_configset -raw DBADTL 1", DBMUtility.OK_OK);

			//create a Backint for MaxDB medium
			m_DBM.execCmd("medium_put BackData \""+pipeDir+"backintpipe\" pipe data 0 8 yes NO \"\" BACK", DBMUtility.OK_OK);

			//create a NetWorker medium
			m_DBM.execCmd("medium_put NSRData \""+pipeDir+"nsrpipe\" pipe data 0 8 yes NO \"\" NSR", DBMUtility.OK_OK);
			
			//create a TSM medium
			m_DBM.execCmd("medium_put TSMData \""+pipeDir+"tsmpipe\" pipe data 0 8 yes NO \"\" TSM", DBMUtility.OK_OK);
			
			m_DBM.execCmd("util_connect", DBMUtility.OK_OK);
		}
		catch (Exception e)
		{
			super.addMessage(
				super.getTestId(),
				TestProtocol.ERROR,
				e.getClass().getName()
					+ " caught while preparing test ( "
					+ e.getMessage()
					+ "). Aborting test.");
		}
		
		//test if a not configured backup tools are reported as such
		try
		{	
			m_DBM.execCmd("backup_start BackData", DBMUtility.ERR_TOOLCHK);
			m_DBM.execCmd("backup_start NSRData", DBMUtility.ERR_TOOLCHK);
			m_DBM.execCmd("backup_start TSMData", DBMUtility.ERR_TOOLCHK);
		}
		catch (Exception e)
		{
			super.addMessage(
				super.getTestId(),
				TestProtocol.ERROR,
				e.getClass().getName()
					+ " caught during test ( "
					+ e.getMessage()
					+ "). Aborting failed test.");
		}
		
		//clean up as much as possible
		finally
		{
			try
			{
				m_DBM.execCmd("dbm_configset -raw DBADTL", DBMUtility.OK_OK);
				m_DBM.execCmd("medium_delete BackData", DBMUtility.OK_OK);
	 		    m_DBM.execCmd("medium_delete NSRData", DBMUtility.OK_OK);
				m_DBM.execCmd("medium_delete TSMData", DBMUtility.OK_OK);
			    m_DBM.execCmd("util_release", DBMUtility.OK_OK);
			}
			catch(Exception e)
			{
				super.addMessage(
					super.getTestId(),
					TestProtocol.ERROR,
					e.getClass().getName()
						+ " caught while cleaning up test resources ( "
						+ e.getMessage()
						+ "). Aborting test.");
			}
		}
	}

	/**
	   * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
	   * will set the name to the name of the java class. To make the test name independend
	   * from the java implementation, this method should return a unique string.
	   * Attention: The uniqueness is not automatically checked by anybody.
	   * @return The UNIQUE name of the test class
	   */
	public static String getTestClassId() {
		return "NotConfiguredBackupTools";
	} // end getTestClassId
}
