/*!
  -----------------------------------------------------------------------------
  module: RecoverVolume.java
  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 2000-2006 SAP AG

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

package com.sap.sapdb.DBMTest.recover;

import java.io.File;
import java.sql.SQLException;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.base.LinebreakConverter;
import com.sap.sapdb.DBMTest.base.SecondDB;
import com.sap.sapdb.DBMTest.base.Zipper;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;

public class RecoverVolume extends DBMTestCase{

	String m_ZipfileName=null;
	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public RecoverVolume() throws SQLException, TestDatabaseException {
		super();
	}

	class SecondDBForRecoverVolume extends SecondDB{
		String m_RunDir=null;
		
		SecondDBForRecoverVolume(TestDatabaseSap originalDB, DBMTestCase testCase, String dbNamePrefix, String runDir) throws TestDatabaseException {
			
			super(originalDB, testCase, dbNamePrefix);
			m_RunDir=runDir;
		}
		
		protected void setParameters(DBMUtility oDBMCreate)
		{
			oDBMCreate.execCmd("param_put LOG_MIRRORED YES", DBMUtility.OK_OK);
			
			if(null!=m_RunDir)
				oDBMCreate.execCmd("param_put RUNDIRECTORY \""+m_RunDir+"\"", DBMUtility.OK_OK);
		};
		
		protected void setVolumes(DBMUtility oDBMCreate)
		{
			oDBMCreate.execCmd("param_addvolume 1 DATA dat001 F 5000", DBMUtility.OK_OK);
	        oDBMCreate.execCmd("param_addvolume 1 LOG  log001 F 2000", DBMUtility.OK_OK);
	        oDBMCreate.execCmd("param_addvolume 1 MLOG logM001 F 2000", DBMUtility.OK_OK);
		}
	};
	
	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.base.DBMTestCase#runDBMTest()
	 */
	protected void runDBMTest() throws Exception {
		//	  create one global try-catch block to handle exceptions
	    try
		{		
	    	String runDirSecondDB=m_DB.getRunDir()+System.getProperty("file.separator")+"RecoverVolume";
			Zipper zip=new Zipper(getZipFile().getAbsolutePath(), this);
	    	
	    	SecondDBForRecoverVolume secondDB=new SecondDBForRecoverVolume((TestDatabaseSap) getDatabase(), this, "RV", runDirSecondDB);

	    	secondDB.drop(); //drop any remains of previous test
	    	secondDB.create();
			
			DBMUtility dbmSecondDB=secondDB.getDBSession();
			
	    	if(null==dbmSecondDB)
	    		addMessage(
	    				"    Getting DB session    ",
						TestProtocol.ERROR,
						"got no session to second test db "+secondDB.getName()+".");
	    	else
	    	{
	 			dbmSecondDB.execCmd("db_admin", DBMUtility.OK_OK);

	 			File LogVol =
	                new File(
	                		runDirSecondDB
	                        + System.getProperty("file.separator")
	                        + "log001");
	 			
	 			if(!LogVol.delete())
	 				addMessage("    Deleting log volume    ", TestProtocol.ERROR, "Could not delete log volume "+LogVol.getAbsoluteFile());
	 			else
	 			{
	 				int [] allowedRCs={DBMUtility.ERR_SQL};
	 				dbmSecondDB.execCmd("db_online", allowedRCs);

	 				zip.addFileAs(runDirSecondDB+File.separator+"knldiag", runDirSecondDB+File.separator+"knldiag.db_online 1st try");
	 				
	 				do
	 				{
	 				    dbmSecondDB.execCmd("db_state", DBMUtility.OK_OK);
	 					addMessage(
	 							"    Polling for db state OFFLINE    ",
	 							TestProtocol.TRACE,
								"database state: \n"
								+LinebreakConverter.convertToOSSpecificLinebreaks(dbmSecondDB.getAnswer()));
 						Thread.sleep(1000);
	 				}
	 				while(-1==dbmSecondDB.getAnswer().indexOf("OFFLINE"));
	 				
	 				dbmSecondDB.execCmd("db_admin", DBMUtility.OK_OK);
	 				dbmSecondDB.execCmd("db_connect", DBMUtility.OK_OK);
	 				dbmSecondDB.execCmd("recover_volume log001", DBMUtility.OK_OK);
	 				dbmSecondDB.execCmd("db_release", DBMUtility.OK_OK);
	 				dbmSecondDB.execCmd("db_online", DBMUtility.OK_OK);
	 				
	 				if(!LogVol.exists())
	 					addMessage(
	 		    				"    Checking existence of recreated log volume    ",
	 							TestProtocol.ERROR,
	 							"recover volume did not recreate the log volume "+LogVol.getAbsoluteFile()+".");
	 				
	 				dbmSecondDB.execCmd("db_offline", DBMUtility.OK_OK);
	 			}
	    	}

	    	zip.addFile(runDirSecondDB+File.separator+"knldiag");
	    	zip.addFile(runDirSecondDB+File.separator+"knldiag.err");
	    	zip.addFile(runDirSecondDB+File.separator+"dbm.utl");
			zip.addFile(runDirSecondDB+File.separator+"dbm.prt");
			
			zip.close();
			
	    	secondDB.drop();
	    	
	    } catch (Exception e) {
	      // standard error handler
	      this.handleExceptions(e);
	    } // end try catch
	}
}
