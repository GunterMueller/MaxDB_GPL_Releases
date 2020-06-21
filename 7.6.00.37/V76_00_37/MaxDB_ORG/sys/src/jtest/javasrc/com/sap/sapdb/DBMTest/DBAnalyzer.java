package com.sap.sapdb.DBMTest;

/*!
  -----------------------------------------------------------------------------
  module: DBAnalyzer.java
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

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCaseException;

/**
 * DBMServer test example
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBAnalyzer
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer test for DBAnalyzer commands
 *               This test tests the DBMServer commands dban_state, dban_start and
 *               dban_stop. 
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class DBAnalyzer extends DBMTestCase
{
  /**
   * A minimal constructor.Constructor.
   */
  public DBAnalyzer() throws SQLException, TestDatabaseException
  {
          super();
  } // end DBAnalyzer

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
    return "DBAnalyzer";
  } // end getTestClassId

  /**
   * Sets the timeout value in milliseconds. After the timeout is reached, the test will be aborted
   * by the test driver.
   * @return The default return value is 0, which means there is no timeout value set. Test will never
   * be aborted by the test driver.
   */
  public static long getTimeout()
  {
    return 0;
  } // end getTimeout

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

  public void RunAnalyzer (int nPhase, String sCommand, int nExpectedCode, String sExpectedAnswer)
  {
    m_DBM.execCmd(sCommand, nExpectedCode);
	if (m_DBM.getAnswer().indexOf(sExpectedAnswer) < 0) {
        addMessage(  "Error in Phase " 
        		   + nPhase 
				   + " with command " 
				   + sCommand 
				   + ". Expected answer \"" 
				   + sExpectedAnswer 
				   + "\" not found", 
				   TestProtocol.ERROR, m_DBM.getAnswer());
	} // end if
  }
  
  
  /**
   * run() contains the test implementation
   */
  public void runDBMTest()
  {
    //  create one global try-catch block to handle exceptions
    try
    {
    	// 1. dban_state
    	RunAnalyzer(1, "dban_state", DBMUtility.OK_OK, "INFO 20:");
    	
    	// 2. dban_stop
    	RunAnalyzer(2, "dban_stop", DBMUtility.ERR_EXECUTE, "ERROR 20:");
    	
        // 3. dban_start
    	RunAnalyzer(3, "dban_start", DBMUtility.OK_OK, "INFO 13:");
    	
        // 4. dban_state
    	RunAnalyzer(3, "dban_state", DBMUtility.OK_OK, "INFO 13:");
    	
    	// 5. dban_stop
    	RunAnalyzer(5, "dban_stop", DBMUtility.OK_OK, "Analyzer");
    	
    	// let the dbanalyzer terminate
    	Thread.sleep(20000);
    	    	
    	// 6. dban_state
    	RunAnalyzer(6, "dban_state", DBMUtility.OK_OK, "INFO 20:");
    	
    	// 7. dban_stop
    	RunAnalyzer(7, "dban_stop", DBMUtility.ERR_EXECUTE, "ERROR 20:");
    	
    	// provokate an error
        m_DBM.execCmd("db_execute user-type=DBA drop view DOMAIN.SYSMON_TASK_DETAIL", DBMUtility.OK_OK);

        // 8. dban_start
    	RunAnalyzer(8, "dban_start", DBMUtility.ERR_EXECUTE, "Unknown table name:SYSMON_TASK_DETAIL");
    	
    	// 9. dban_state
    	RunAnalyzer(9, "dban_state", DBMUtility.OK_OK, "INFO 20:");
    	
    	// 10. dban_stop
    	RunAnalyzer(10, "dban_stop", DBMUtility.ERR_EXECUTE, "ERROR 20:");
    	
    	// repair database
        m_DBM.execCmd("load_systab", DBMUtility.OK_OK);
        
        // 11. dban_start
    	RunAnalyzer(11, "dban_start", DBMUtility.OK_OK, "INFO 13:");
    	
        // 12. dban_state
    	RunAnalyzer(12, "dban_state", DBMUtility.OK_OK, "INFO 13:");
    	
    	// 13. dban_stop
    	RunAnalyzer(13, "dban_stop", DBMUtility.OK_OK, "Analyzer");
    	
    	// let the dbanalyzer terminate
    	Thread.sleep(20000);
    	
    	// 12. dban_state
    	RunAnalyzer(12, "dban_state", DBMUtility.OK_OK, "INFO 20:");
    	
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

} // end class DBAnalyzer
