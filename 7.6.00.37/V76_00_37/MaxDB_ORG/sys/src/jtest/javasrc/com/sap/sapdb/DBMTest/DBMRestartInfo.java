package com.sap.sapdb.DBMTest;

/*!
  -----------------------------------------------------------------------------
  module: DBMRestartInfo.java
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

import com.sap.sapdb.DBMTest.util.*;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCase;
import com.sap.sapdb.testframe.testcase.TestCaseException;

/**
 * DBMServer trace test
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBMRestartInfo
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  checks the db_restartinfo command of the DBMServer
 * version:      7.2.*
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class DBMRestartInfo extends TestCase {

  /**
   * A minimal constructor.Constructor.
   */
  public DBMRestartInfo() throws SQLException, TestDatabaseException {
          super();
  } // end DBMRestartInfo

  /**
   * Clean up test, i.e. delete data created in prepare, ...
   * @exception TestCaseException Should be thrown, when cleanUp fails
   */
  public static void cleanUp() throws TestCaseException {
    // do nothing
  } // end cleanUp

  /**
   * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
   * will set the name to the name of the java class. To make the test name independend
   * from the java implementation, this method should return a unique string.
   * Attention: The uniqueness is not automatically checked by anybody.
   * @return The UNIQUE name of the test class
   */
  public static String getTestClassId() {
    return "DBMRestartInfo";
  } // end getTestClassId

  /**
   * Sets the timeout value in milliseconds. After the timeout is reached, the test will be aborted
   * by the test driver.
   * @return The default return value is 0, which means there is no timeout value set. Test will never
   * be aborted by the test driver.
   */
  public static long getTimeout() {
    return 0;
  } // end getTimeout

  /**
   * Sets weather test can create and work with verification data or not.
   * @return The default return value is false, which means that no verification data
   * is used to verify the test results. In this case the test has to implement the verification
   * itself.
   */
  public static boolean isReferenceEnabled() {
    return false;
  } // end isReferenceEnabled

  /**
   * Sets weather test can be executed only on SAPDB or not.
   * @return The default return value is true. This means, the test can run on SAPDB only.
   */
  public static boolean isSAPOnly() {
          return true;
  } // end isSAPOnly

  /**
   * Prepare test, i.e. create users, tables and data
   * @exception TestCaseException Should be thrown, when preparation fails
   */
  public static void prepare() throws TestCaseException {
    // do nothing
  } // end prepare

  /**
   * run() contains the test implementation
   */
  public void run() {

    DBMUtility     oDBM    = new  DBMUtility(getDatabase(), this);
    DBMAnswerTable oAnswer = new DBMAnswerTable();

    int nUsedPage;
    int nFirstPage;

    
    //  create one global try-catch block to handle exceptions
    try {

      // *** 1. Check with normal filled log ***
      
      // switch db_state to admin
      oDBM.execCmd("db_offline", DBMUtility.OK_OK);
      oDBM.execCmd("db_admin", DBMUtility.OK_OK);

      // read the restart info
      oDBM.execCmd("db_restartinfo", DBMUtility.OK_OK);

      // check the restartinfo
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');

      // check Used LOG Page
      nUsedPage = Integer.parseInt(oAnswer.getField("Used LOG Page", 0, 1));
      if ( nUsedPage < 0 || nUsedPage == Integer.MAX_VALUE) {
        // error 
        addMessage("db_restartinfo", TestProtocol.ERROR , "Unexpected value of Used Log Page! Found: " + nUsedPage + " Expected: >= 0 and < " + Integer.MAX_VALUE);
      } // end if

      // check First LOG Page
      nFirstPage = Integer.parseInt(oAnswer.getField("First LOG Page", 0, 1));
      if ( nFirstPage < 0 || nFirstPage == Integer.MAX_VALUE) {
        // error 
        addMessage("db_restartinfo", TestProtocol.ERROR , "Unexpected value of First Log Page! Found: " + nFirstPage + " Expected: >= 0 and < " + Integer.MAX_VALUE);
      } // end if

      // *** 2. Check after clear log ***
      
      // switch db_state to admin
      oDBM.execCmd("db_offline", DBMUtility.OK_OK);
      oDBM.execCmd("db_admin", DBMUtility.OK_OK);

      // switch clear the log
      oDBM.execCmd("util_execute clear log", DBMUtility.OK_OK);

      // read the restart info
      oDBM.execCmd("db_restartinfo", DBMUtility.OK_OK);

      // check the restartinfo
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');

      // check Used LOG Page
      nUsedPage = Integer.parseInt(oAnswer.getField("Used LOG Page", 0, 1));
      if ( nUsedPage < 0 || nUsedPage == Integer.MAX_VALUE) {
        // error 
        addMessage("db_restartinfo", TestProtocol.ERROR , "Unexpected value of Used Log Page! Found: " + nUsedPage  + " Expected: >= 0 and < " + Integer.MAX_VALUE);
      } // end if

      // check First LOG Page
      nFirstPage = Integer.parseInt(oAnswer.getField("First LOG Page", 0, 1));
      if ( nFirstPage != Integer.MAX_VALUE) {
        // error 
        addMessage("db_restartinfo", TestProtocol.ERROR , "Unexpected value of First Log Page! Found:  " + nFirstPage  + " Expected: " + Integer.MAX_VALUE);
      } // end if

      // *** 3. clean up ***
      
      oDBM.execCmd("db_offline", DBMUtility.OK_OK);
      oDBM.execCmd("db_admin", DBMUtility.OK_OK);
      oDBM.execCmd("db_activate dba,dba", DBMUtility.OK_OK);
      oDBM.execCmd("load_systab -ud domain", DBMUtility.OK_OK);

    }
    // catch any kind of exception and write it to the protocol
    catch (Exception e) {
      // standard error handler
      this.handleExceptions(e);
    } // end try catch

    // clean up, i.e. close statements and connections
    finally {
      try {
        // print all parameters from the parameter object to the protocol.
        printParametersToProtocol();
      }
      catch (Exception e) {
      } // end try catch
    } // end finally
  } // end run

} // end class DBMRestartInfo
