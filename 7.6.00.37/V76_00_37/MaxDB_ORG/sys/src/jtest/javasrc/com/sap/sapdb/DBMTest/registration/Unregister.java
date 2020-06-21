package com.sap.sapdb.DBMTest.registration;
/*!
  -----------------------------------------------------------------------------
  module: Unregister.java
  -----------------------------------------------------------------------------


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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
import java.sql.SQLException;

import com.sap.dbtech.powertoys.DBM;
import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.utilities.Execute;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBReg
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer db_reg command tests
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class Unregister extends DBMTestCase
{
  public Unregister() throws SQLException, TestDatabaseException
  {
    super();
  }
  /**
   * run() contains the test implementation
   */
  public void runDBMTest()
  {
    //  create one global try-catch block to handle exceptions
    try
    {
        TestDatabaseSap oDB = (TestDatabaseSap) getDatabase();
    	String sDB = "UR" + oDB.getKernelBuild().substring(8,11) + oDB.getKernelBuild().substring(12);
   	  	System.out.println(sDB);

    	// create funny test database
        StringBuffer ProcessOutput = new StringBuffer();
        Execute oProcess = new Execute("dbmcli -R "+ this.getDBRoot() + " db_create "+sDB+" xxx,yyy", null, ProcessOutput);
        oProcess.exec();
        System.out.println(ProcessOutput.toString());
        if (!ProcessOutput.toString().trim().equals("OK")) {
            addMessage("DBCreate - creating database.", TestProtocol.ERROR, "db_create did not work.");
        } // end if
    	DBM oDBMCreateSession = DBM.dbDBM(getDatabase().getHost() 
						    	              + ':' 
							                  + getDatabase().getPort(), 
							                  sDB);
        DBMUtility  oDBMCreate = new DBMUtility(getDatabase(), this, oDBMCreateSession);
        oDBMCreate.execCmd("user_logon xxx,yyy", DBMUtility.OK_OK);
        oDBMCreate.execCmd("param_rmfile", DBMUtility.OK_OK);
        oDBMCreate.execCmd("param_startsession", DBMUtility.OK_OK);
        oDBMCreate.execCmd("param_init OLTP", DBMUtility.OK_OK);
        oDBMCreate.execCmd("param_checkall", DBMUtility.OK_OK);
        oDBMCreate.execCmd("param_commitsession", DBMUtility.OK_OK);
        oDBMCreate.execCmd("param_addvolume 1 DATA dat001 F 5000", DBMUtility.OK_OK);
        oDBMCreate.execCmd("param_addvolume 1 LOG log001 F 2000", DBMUtility.OK_OK);
        oDBMCreate.execCmd("db_admin", DBMUtility.OK_OK);
        oDBMCreate.execCmd("db_activate aaa,bbb", DBMUtility.OK_OK);
        oDBMCreate.execCmd("load_systab", DBMUtility.OK_OK);
                               
        // second DBM object for test
    	DBM oDBMSession = DBM.dbrootDBM(getDatabase().getHost() 
	                                    + ':' 
                                        + getDatabase().getPort(), 
                                        getDBRoot());
        DBMUtility  oDBM = new DBMUtility(getDatabase(), this, oDBMSession);

    	// register with active database
        oDBMCreate.execCmd("db_reg DB "+sDB+" xxx,yyy", DBMUtility.ERR_CMDOPTION);
        
        // register existing datbase
        oDBM.execCmd("db_reg DB "+sDB+" xxx,yyy", DBMUtility.ERR_DBEXIST);
                
    	
    	// object for unregister database
    	DBM oDBMUnregSession = DBM.dbDBM(getDatabase().getHost() 
                                             + ':' 
                                             + getDatabase().getPort(), 
                                             sDB);
        DBMUtility  oDBMUnreg = new DBMUtility(getDatabase(), this, oDBMUnregSession);
        
    	// unregister running database
        oDBMUnreg.execCmd("user_logon xxx,yyy", DBMUtility.OK_OK);
        oDBMUnreg.execCmd("db_unreg", DBMUtility.ERR_DBRUN);
        
//    	// unregister with other DBMServer
//        oDBMUnreg.execCmd("db_offline", DBMUtility.OK_OK);
//        oDBMUnreg.execCmd("db_unreg", DBMUtility.ERR_NOT_EXCLUSIVE);

    	// unregister
        oDBMUnreg.execCmd("db_offline", DBMUtility.OK_OK);
        oDBMCreateSession.release();
        oDBMUnreg.execCmd("db_unreg", DBMUtility.OK_OK);
        oDBMUnregSession.release();
        
        // register never existing database database
        oDBM.execCmd("db_reg DB " 
      		         + "unsinn" 
   		             + " " 
   		             + getDatabase().getDBMUser() 
   		             + "," 
   		             + getDatabase().getDBMPassword(), 
   		             DBMUtility.ERR_XPNOTFOUND);
        
        // register existing database
        oDBM.execCmd("db_reg DB " 
      		         + getDatabase().getName() 
    		         + " " 
         		     + getDatabase().getDBMUser() 
         		     + "," 
    	    	     + getDatabase().getDBMPassword(), 
   	                 DBMUtility.ERR_DBEXIST);
        
        // register with wrong user pwd
        oDBM.execCmd("db_reg DB "+sDB+" xxx,yy", DBMUtility.ERR_USRFAIL);
       
        // register with
        oDBM.execCmd("db_reg DB "+sDB+" xxx,yyy", DBMUtility.OK_OK);
        
        oDBMSession.release();
        
		// unregister with dbmunreg
		String Sep = m_DBM.getSrvProperty("file.separator");
		ProcessOutput = new StringBuffer();
		oProcess = new Execute(this.getDBRoot()+Sep+"pgm"+Sep+"dbmunreg "+sDB, null , ProcessOutput);
		oProcess.exec();
		if (oProcess.getExitValue() != 0) {
		  addMessage("dbmunreg does not work!", TestProtocol.ERROR,
			  "Output: "+ProcessOutput.toString().trim()+"\n("+__FILE__()+","+ __LINE__()+")");
		} // end if
		
        // register with dbmreg
		ProcessOutput = new StringBuffer();
		oProcess = new Execute(this.getDBRoot()+Sep+"pgm"+Sep+"dbmreg "+sDB, null , ProcessOutput);
		oProcess.exec();
		if (oProcess.getExitValue() != 0) {
		  addMessage("dbmreg does not work!", TestProtocol.ERROR,
			  "Output: "+ProcessOutput.toString().trim()+"\n("+__FILE__()+","+ __LINE__()+")");
		} // end if
        
        // clean up
	    DBM oDBMCleanupSession = DBM.dbDBM(getDatabase().getHost()+':'+getDatabase().getPort(), sDB);
        DBMUtility  oDBMCleanup = new DBMUtility(getDatabase(), this, oDBMCleanupSession);
        oDBMCleanup.execCmd("user_logon xxx,yyy", DBMUtility.OK_OK);
        oDBMCleanup.execCmd("db_online", DBMUtility.OK_OK);
        oDBMCleanup.execCmd("db_offline", DBMUtility.OK_OK);
        oDBMCleanup.execCmd("db_drop", DBMUtility.OK_OK);
        oDBMCleanupSession.release();
           	
    } catch (Exception e) {
      // standard error handler
      this.handleExceptions(e);
    } // end try catch

  } // end runDBMTest
  
  /**
   * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
   * will set the name to the name of the java class. To make the test name independend
   * from the java implementation, this method should return a unique string.
   * Attention: The uniqueness is not automatically checked by anybody.
   * @return The UNIQUE name of the test class
   */
  public static String getTestClassId() {
    return "Unregister";
  } // end getTestClassId

} //end of class DBReg