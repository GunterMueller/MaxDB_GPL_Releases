package com.sap.sapdb.DBMTest.programs;

/*!
  -----------------------------------------------------------------------------
  module: DBMCli.java
  -----------------------------------------------------------------------------


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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
import java.sql.SQLException;
import java.util.StringTokenizer;

import com.sap.dbtech.powertoys.DBM;
import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCaseException;
import com.sap.sapdb.testframe.utilities.Execute;

/**
 * DBMServer test example
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBMCli
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer test example
 * version:      7.2.*
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class DBMCli extends DBMTestCase
{
  /**
   * A minimal constructor.Constructor.
   */
  public DBMCli() throws SQLException, TestDatabaseException
  {
          super();
  } // end DBMCli

  /**
   * Clean up test, i.e. delete data created in prepare, ...
   * @exception TestCaseException Should be thrown, when cleanUp fails
   */
  public static void cleanUp() throws TestCaseException
  {
    // do nothing
  } // end cleanUp

  /**
   * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
   * will set the name to the name of the java class. To make the test name independend
   * from the java implementation, this method should return a unique string.
   * Attention: The uniqueness is not automatically checked by anybody.
   * @return The UNIQUE name of the test class
   */
  public static String getTestClassId()
  {
    return "DBMCli";
  } // end getTestClassId

  /**
   * Sets weather test can create and work with verification data or not.
   * @return The default return value is false, which means that no verification data
   * is used to verify the test results. In this case the test has to implement the verification
   * itself.
   */
  public static boolean isReferenceEnabled()
  {
    return false;
  } // end isReferenceEnabled

  /**
   * Sets weather test can be executed only on SAPDB or not.
   * @return The default return value is true. This means, the test can run on SAPDB only.
   */
  public static boolean isSAPOnly()
  {
          return true;
  } // end isSAPOnly

  /**
   * Prepare test, i.e. create users, tables and data
   * @exception TestCaseException Should be thrown, when preparation fails
   */
  public static void prepare() throws TestCaseException
  {
    // do nothing
  } // end prepare

  /**
   * run() contains the test implementation
   */
  public void runDBMTest()
  {
    //  create one global try-catch block to handle exceptions
    try
    {
    	StringTokenizer Tokens;
    	String          strValue;
    	
    	StringTokenizer TokensTest;
    	String          strValueTest;
    	
    	StringBuffer ProcessOutput;
    	Execute      oDBMCli;
    	
    	DBM oDBMSession = DBM.dbrootDBM(getDatabase().getHost() 
                + ':' 
                + getDatabase().getPort(), 
                getDBRoot());
        DBMUtility  oDBM = new DBMUtility(getDatabase(), this, oDBMSession);
   	
        // 1. Check of "dbmcli -s dbm_getpath indepprogpath"
    	
    	// get value via Testframe
        oDBM.execCmd("dbm_getpath IndepProgPath", DBMUtility.OK_OK);
        Tokens = new StringTokenizer(oDBM.getAnswer().toString().trim(),"\n");
        strValue = oDBM.getAnswer().toString().trim();
        if (Tokens.hasMoreTokens()) {
        	strValue = Tokens.nextToken().trim();
        } // end if
        
        // get value via dbmcli
    	ProcessOutput = new StringBuffer();
    	oDBMCli = new Execute("dbmcli -s -R "+this.getDBRoot()+" dbm_getpath IndepProgPath", null , ProcessOutput);
        oDBMCli.exec();
    	TokensTest = new StringTokenizer(ProcessOutput.toString().trim(),"\n");
    	strValueTest = ProcessOutput.toString().trim();
        if (TokensTest.hasMoreTokens() && (TokensTest.nextToken().trim().compareTo("OK") == 0)) {
        	strValueTest = TokensTest.nextToken().trim();
        } // end if
      
        // check result
        if (oDBMCli.getExitValue() == 0) {
            if (strValueTest.compareTo(strValue) != 0) {
                addMessage("dbmcli does not work!",
                           TestProtocol.ERROR,
                           "Expected: "+strValue+"\n"
                           +"Found: "+strValueTest+"\n"
                           +"("+__FILE__()+","+ __LINE__()+")");
            } // end if
        } else {
            addMessage("dbmcli does not work!",
                       TestProtocol.ERROR,
                       "Command: dbmcli -s dbm_getpath IndepProgPath\n"
                       +"Output: "+ProcessOutput.toString().trim()
                       +"\n("+__FILE__()+","+ __LINE__()+")");
        } // end if
        
        // 2. Check of "dbmcli -s dbm_getpath indepdatapath"
    	
    	// get value via Testframe
        oDBM.execCmd("dbm_getpath IndepDataPath", DBMUtility.OK_OK);
        Tokens = new StringTokenizer(oDBM.getAnswer().toString().trim(),"\n");
        strValue = oDBM.getAnswer().toString().trim();
        if (Tokens.hasMoreTokens()) {
        	strValue = Tokens.nextToken().trim();
        } // end if
        
        // get value via dbmcli
    	ProcessOutput = new StringBuffer();
    	oDBMCli = new Execute("dbmcli -s -R "+this.getDBRoot()+" dbm_getpath IndepDataPath", null , ProcessOutput);
        oDBMCli.exec();
    	TokensTest = new StringTokenizer(ProcessOutput.toString().trim(),"\n");
    	strValueTest = ProcessOutput.toString().trim();
        if (TokensTest.hasMoreTokens() && (TokensTest.nextToken().trim().compareTo("OK") == 0)) {
        	strValueTest = TokensTest.nextToken().trim();
        } // end if
      
        // check result
        if (oDBMCli.getExitValue() == 0) {
            if (strValueTest.compareTo(strValue) != 0) {
                addMessage("dbmcli does not work!",
                           TestProtocol.ERROR,
                           "Expected: "+strValue+"\n"
                           +"Found: "+strValueTest+"\n"
                           +"("+__FILE__()+","+ __LINE__()+")");
            } // end if
        } else {
            addMessage("dbmcli does not work!",
                       TestProtocol.ERROR,
                       "Command: dbmcli -s dbm_getpath IndepDataPath\n"
                       +"Output: "+ProcessOutput.toString().trim()
                       +"\n("+__FILE__()+","+ __LINE__()+")");
        } // end if
        
        // 3. Check of "dbmcli -s dbm_version BUILD"
    	
       
        // get value via dbmcli
    	ProcessOutput = new StringBuffer();
    	oDBMCli = new Execute("dbmcli -s -R "+this.getDBRoot()+" dbm_version BUILD", null , ProcessOutput);
        oDBMCli.exec();
    	TokensTest = new StringTokenizer(ProcessOutput.toString().trim(),"\n");
    	strValueTest = ProcessOutput.toString().trim();
        if (TokensTest.hasMoreTokens() && (TokensTest.nextToken().trim().compareTo("OK") == 0)) {
        	strValueTest = TokensTest.nextToken().trim();
        } // end if
      
        // check result
        if (oDBMCli.getExitValue() == 0) {
        	if (!strValueTest.startsWith("DBMServer ")) {
				addMessage("dbmcli does not work!",
						TestProtocol.ERROR,
						"Expected: DBMServer ...\n"
						+"Found: "+strValueTest+"\n"
						+"("+__FILE__()+","+ __LINE__()+")");
			} // end if
        } else {
            addMessage("dbmcli does not work!",
                       TestProtocol.ERROR,
                       "Command: dbmcli -s dbm_version BUILD\n"
                       +"Output: "+ProcessOutput.toString().trim()
                       +"\n("+__FILE__()+","+ __LINE__()+")");
        } // end if
        
        oDBMSession.release();

    }
    // catch any kind of exception and write it to the protocol
    catch (Exception e)
    {
      // standard error handler
      this.handleExceptions(e);
    } // end try catch

    // clean up, i.e. close statements and connections
    finally
    {
      try
      {
        // print all parameters from the parameter object to the protocol.
        printParametersToProtocol();
      }
      catch (Exception e)
      {
        // do nothing
      } // end try catch
    } // end finally
  } // end run

} // end class DBMCli
