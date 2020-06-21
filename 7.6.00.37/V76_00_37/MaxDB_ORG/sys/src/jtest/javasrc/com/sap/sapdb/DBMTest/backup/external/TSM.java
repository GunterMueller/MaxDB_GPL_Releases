/*
 ========== licence begin  GPL
 Copyright (c) 1998-2005 SAP AG

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

import java.io.File;
import java.net.InetAddress;
import java.sql.SQLException;
import java.util.StringTokenizer;

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestDriver;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCase;
import com.sap.sapdb.testframe.utilities.Execute;

/**
 * @author d025561
 */
public class TSM extends TestCase {

	/**
	 * @throws java.sql.SQLException
	 * @throws com.sap.sapdb.testframe.driver.TestDatabaseException
	 */
	public TSM() throws SQLException, TestDatabaseException {
		super();
	}

	/* (non-Javadoc)
	 * @see java.lang.Runnable#run()
	 */
	public void run() {
		TestDatabaseSap db= (TestDatabaseSap) super.getDatabase();
		DBMUtility      dbm= new DBMUtility(getDatabase(), this);
		
		String os = System.getProperty("os.name");
		String pathSep=System.getProperty("file.separator");
		String nameADSMOptFile=null;
		String hostName=null;
	    String adintCleanupCall=null;
	    String [] adintEnvironment={null};
		
		super.addMessage(super.getTestId(), TestProtocol.INFO, "Operating System name for Java is '" + os+"'.");

		//prepare the actual test
		try
		{
			nameADSMOptFile=db.getRunDir()+pathSep+"adsm.opt";
			hostName=InetAddress.getLocalHost().getHostName();
			
			if(0<hostName.indexOf('.'))
		    	hostName=hostName.substring(0, hostName.indexOf('.'));
			
			adintCleanupCall=TestDriver.getJTestRoot()
	        					+pathSep+"bin"
								+pathSep+"adint2 "+
								"-d -n "+hostName+"_"+db.getName();

			adintEnvironment[0]="ADA_OPT="+nameADSMOptFile;
			
			String pipeDir = null;
			
			if(os.toUpperCase().startsWith("WINDOWS"))
				pipeDir="\\\\.\\pipe\\";
			else
				pipeDir=db.getRunDir()+pathSep;
			
			dbm.execCmd("medium_put DataTSM \""+pipeDir+"tsmpipe\" pipe data 0 0 NO NO \"\" TSM", DBMUtility.OK_OK);
			dbm.execCmd("dbm_configset -raw ADINT "+TestDriver.getJTestRoot()+pathSep+"bin", DBMUtility.OK_OK);
			dbm.execCmd("dbm_configset -raw ADA_OPT "+nameADSMOptFile, DBMUtility.OK_OK);
			new File(nameADSMOptFile).createNewFile();
		    dbm.execCmd("util_connect", DBMUtility.OK_OK);

			//cleanup old backups, as rundirectories 
    		super.addMessage(super.getTestId(), TestProtocol.TRACE, "calling: '"+adintCleanupCall+"' with environment set to "+adintEnvironment[0]+".");

    		Execute adint2=new Execute(adintCleanupCall, adintEnvironment);
    		adint2.exec(new File(db.getInstRoot()));

    		super.addMessage(super.getTestId(), TestProtocol.TRACE, "adint2 ended with exit code "+adint2.getExitValue()+".");
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
		
		//do the testing
		try
		{	
			dbm.execCmd("backup_start DataTSM", DBMUtility.OK_OK);
			dbm.execCmd("backup_start DataTSM", DBMUtility.OK_OK);
			dbm.execCmd("backup_ext_ids_get DataTSM", DBMUtility.OK_OK);
			dbm.execCmd("backup_ext_ids_list", DBMUtility.OK_OK);

			String list=dbm.getAnswer();
			String ebid=null;
			
			StringTokenizer lines = new StringTokenizer(list,"\n");
			
			lines.nextToken();
		    
			while(null==ebid && lines.hasMoreTokens())
			{
		        StringTokenizer columns = new StringTokenizer(lines.nextToken(), "|");

		        columns.nextToken();
		        ebid=columns.nextToken();
		    }
			
			dbm.execCmd("service_connect", DBMUtility.OK_OK);
			dbm.execCmd("recover_check DataTSM DATA EBID "+ebid, DBMUtility.OK_OK);
			dbm.execCmd("db_admin", DBMUtility.OK_OK);
			dbm.execCmd("util_connect", DBMUtility.OK_OK);
			dbm.execCmd("recover_start DataTSM DATA EBID "+ebid, DBMUtility.OK_OK);
			dbm.execCmd("db_online", DBMUtility.OK_OK);
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
				dbm.execCmd("medium_delete DataTSM", DBMUtility.OK_OK);
				dbm.execCmd("dbm_configset -raw ADINT", DBMUtility.OK_OK);
				dbm.execCmd("dbm_configset -raw ADA_OPT", DBMUtility.OK_OK);
				new File(nameADSMOptFile).delete();
			    dbm.execCmd("util_release", DBMUtility.OK_OK);

        		super.addMessage(super.getTestId(), TestProtocol.TRACE, "calling: '"+adintCleanupCall+"' with environment set to "+adintEnvironment[0]+".");

        		Execute adint2=new Execute(adintCleanupCall, adintEnvironment);
        		adint2.exec(new File(db.getInstRoot()));

        		super.addMessage(super.getTestId(), TestProtocol.TRACE, "adint2 ended with exit code "+adint2.getExitValue()+".");
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
	
	public static String getTestClassId() {
		return "TSM";
	} // end getTestClassId
}
