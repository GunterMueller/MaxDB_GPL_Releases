/*!
  -----------------------------------------------------------------------------
  module: SecondDB.java
  -----------------------------------------------------------------------------

  ========== licence begin  GPL
  Copyright (c) 2002-2006 SAP AG

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

package com.sap.sapdb.DBMTest.base;

import com.sap.dbtech.powertoys.DBM;
import com.sap.dbtech.rte.comm.RTEException;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.utilities.Execute;


public class SecondDB { //TODO: use class AdditionalDatabase of test framework for implementation (see .../oltptest/simple/AdditionalDatabase.java)

	private DBM        m_DBM=null;
	private DBMUtility m_DBMSession=null;
	private TestDatabaseSap m_OriginalDatabase=null;
	private DBMTestCase     m_TestCase=null;
	private String          m_NameSecondDB=null;
	
	protected void setParameters(DBMUtility oDBMCreate)
	{
	};
	
	protected void setVolumes(DBMUtility oDBMCreate)
	{
		oDBMCreate.execCmd("param_addvolume 1 DATA dat001 F 5000", DBMUtility.OK_OK);
        oDBMCreate.execCmd("param_addvolume 1 LOG log001 F 2000", DBMUtility.OK_OK);
	}
	
	public SecondDB(TestDatabaseSap originalDB, DBMTestCase testCase, String dbNamePrefix) throws TestDatabaseException {
		
		m_OriginalDatabase=originalDB;
		m_TestCase=testCase;
		m_NameSecondDB=dbNamePrefix.substring(0,2)
					   		+ originalDB.getKernelBuild().substring(8,11)
							+ originalDB.getKernelBuild().substring(12);
	}
	
	public void create() throws RTEException {

		m_TestCase.addMessage(
    			"creating secondary database for test.",
				TestProtocol.TRACE,
				"creating secondary database with name "+m_NameSecondDB);

    	// create test database with "funny" name
        StringBuffer ProcessOutput = new StringBuffer();
        Execute oProcess = new Execute("dbmcli -R "+ m_TestCase.getDBRoot() + " db_create "+m_NameSecondDB+" dbmx,dbmy", null, ProcessOutput);
        oProcess.exec();
        m_TestCase.addMessage(
    			"creating secondary database for test.",
				TestProtocol.TRACE,
				"output of dbmcli ... db_create ...: "+ProcessOutput.toString());

        if (!ProcessOutput.toString().trim().equals("OK")) {
        	m_TestCase.addMessage("DBCreate - creating database.", TestProtocol.ERROR, "db_create did not work.");
        }
        else
        {
	    	DBM m_DBM = DBM.dbDBM(m_OriginalDatabase.getHost() 
				    	              + ':' 
					                  + m_OriginalDatabase.getPort(), 
									  m_NameSecondDB);
	        DBMUtility  oDBMCreate = new DBMUtility(m_OriginalDatabase, m_TestCase, m_DBM);

	        oDBMCreate.execCmd("user_logon dbmx,dbmy", DBMUtility.OK_OK);
	        
	        oDBMCreate.execCmd("param_rmfile", DBMUtility.OK_OK);
	        oDBMCreate.execCmd("param_startsession", DBMUtility.OK_OK);
	        oDBMCreate.execCmd("param_init OLTP", DBMUtility.OK_OK);
	        setParameters(oDBMCreate);	        
	        oDBMCreate.execCmd("param_checkall", DBMUtility.OK_OK);
	        oDBMCreate.execCmd("param_commitsession", DBMUtility.OK_OK);
	        
	        setVolumes(oDBMCreate);
	        
	        oDBMCreate.execCmd("db_admin", DBMUtility.OK_OK);
	        oDBMCreate.execCmd("exit", DBMUtility.OK_OK);
	        m_DBM.release();
	        
	        //use new connection to database (switch to dbm.prt within rundirectory)
	        m_DBM = DBM.dbDBM(m_OriginalDatabase.getHost() 
  	              				+ ':' 
								+ m_OriginalDatabase.getPort(), 
								m_NameSecondDB);
	        m_DBMSession=new DBMUtility(m_OriginalDatabase, m_TestCase, m_DBM);
	        m_DBMSession.execCmd("user_logon dbmx,dbmy", DBMUtility.OK_OK);
	        m_DBMSession.execCmd("db_activate dbaa,dbab", DBMUtility.OK_OK);
	        m_DBMSession.execCmd("load_systab", DBMUtility.OK_OK);
        }
	}

	public void drop() throws RTEException {
		if(null!=m_DBMSession)
	        m_DBMSession.execCmd("exit",  DBMUtility.OK_OK);

		if(null!=m_DBM)
        	m_DBM.release();
		
		DBM oDBMCleanupSession=null;
		
		try {
			oDBMCleanupSession = DBM.dbDBM(m_OriginalDatabase.getHost()+':'+m_OriginalDatabase.getPort(), m_NameSecondDB);
		}
		catch(RTEException e)
		{
			return;
		}
		
        DBMUtility  oDBMCleanup = new DBMUtility(m_OriginalDatabase, m_TestCase, oDBMCleanupSession);
        
        oDBMCleanup.execCmd("user_logon dbmx,dbmy", DBMUtility.OK_OK);
        oDBMCleanup.execCmd("db_offline", DBMUtility.OK_OK);
        oDBMCleanup.execCmd("db_drop", DBMUtility.OK_OK);

        oDBMCleanupSession.release();
	}

	public DBMUtility getDBSession(){
		return m_DBMSession;
	}
	
	public String getName() {
		return m_NameSecondDB;
	}
	
}
