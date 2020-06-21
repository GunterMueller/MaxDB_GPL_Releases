package com.sap.sapdb.DBMTest;

/*!
  -----------------------------------------------------------------------------
  module: DBMManyParams.java
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
 * DBMServer test example
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBMManyParams
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  Test with many many parameters
 * version:      7.2.*
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class DBMManyParams extends TestCase {

  /**
   * A minimal constructor.Constructor.
   */
  public DBMManyParams() throws SQLException, TestDatabaseException {
          super();
  } // end DBMManyParams

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
    return "DBMManyParams";
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

    DBMUtility oDBM        = new DBMUtility(getDatabase(), this);
    DBMAnswerTable oAnswer = new DBMAnswerTable();

    //  create one global try-catch block to handle exceptions
    try {

      // 1. set db to offline
      oDBM.execCmd("db_offline", DBMUtility.OK_OK);


      // 2. test with few parameters

      oDBM.execCmd("param_directgetall", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() == 0) {
        // error
        addMessage("param_directgetall", TestProtocol.ERROR , "No parameters found");
      } // end if

      oDBM.execCmd("param_directgetallnext", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() > 0) {
        // error
        addMessage("param_directgetallnext", TestProtocol.ERROR , "Parameters found");
      } // end if

      oDBM.execCmd("param_extgetall", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() == 0) {
        // error
        addMessage("param_extgetall", TestProtocol.ERROR , "No parameters found");
      } // end if

      // 3. add many devspaces

      int nIndex;
      for (nIndex = 1; nIndex < 75; ++nIndex) {
        oDBM.execCmd("param_adddevspace " + nIndex + " DATA DAT" + nIndex + " F 2000", DBMUtility.OK_OK);
        oDBM.execCmd("param_adddevspace " + nIndex + " LOG  LOG" + nIndex + " F 1000", DBMUtility.OK_OK);
      } // end if


      // 4. test with many parameters

      oDBM.execCmd("param_directgetall", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() == 0) {
        // error
        addMessage("param_directgetall", TestProtocol.ERROR , "No parameters found");
      } // end if

      oDBM.execCmd("dbm_next", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() != 2) {
        // error
        addMessage("dbm_next", TestProtocol.ERROR , "No next command found");
      } // end if

      oDBM.execCmd("param_directgetallnext", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() == 0) {
        // error
        addMessage("param_directgetallnext", TestProtocol.ERROR , "No next parameters found");
      } // end if

      oDBM.execCmd("param_directgetallnext", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() == 0) 
      {
        // error
        addMessage("param_directgetallnext", TestProtocol.ERROR , "No next parameters found");
      } // end if

      oDBM.execCmd("dbm_next", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() != 0) {
        // error
        addMessage("dbm_next", TestProtocol.ERROR , "Next command found");
      } // end if

      oDBM.execCmd("param_directgetallnext", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() > 0) {
        // error
        addMessage("param_directgetallnext", TestProtocol.ERROR , "More next parameters found");
      } // end if

      oDBM.execCmd("dbm_next", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() != 0) {
        // error
        addMessage("dbm_next", TestProtocol.ERROR , "Next command found");
      } // end if

      oDBM.execCmd("param_extgetall", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() == 0) {
        // error
        addMessage("param_extgetall", TestProtocol.ERROR , "No parameters found");
      } // end if

      oDBM.execCmd("dbm_next", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() != 2) {
        // error
        addMessage("dbm_next", TestProtocol.ERROR , "No next command found");
      } // end if

      oDBM.execCmd("param_extgetallnext", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() == 0) {
        // error
        addMessage("param_extgetallnext", TestProtocol.ERROR , "no next parameters found");
      } // end if

      oDBM.execCmd("dbm_next", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() != 2) {
        // error
        addMessage("dbm_next", TestProtocol.ERROR , "No next command found");
      } // end if

      oDBM.execCmd("param_extgetallnext", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() == 0) {
        // error
        addMessage("param_extgetallnext", TestProtocol.ERROR , "no next parameters found");
      } // end if

      oDBM.execCmd("dbm_next", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() != 0) {
        // error
        addMessage("dbm_next", TestProtocol.ERROR , "Next command found");
      } // end if

      oDBM.execCmd("param_extgetallnext", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() > 0) {
        // error
        addMessage("param_extgetallnext", TestProtocol.ERROR , "More next parameters found");
      } // end if

      oDBM.execCmd("dbm_next", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() != 0) {
        // error
        addMessage("dbm_next", TestProtocol.ERROR , "Next command found");
      } // end if

      // 5. restore "last known good" paramfile

      oDBM.execCmd("param_restore 1", DBMUtility.OK_OK);

      // 6. reset OK?

      oDBM.execCmd("param_directgetall", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() == 0) {
        // error
        addMessage("param_directgetall", TestProtocol.ERROR , "No parameters found");
      } // end if

      oDBM.execCmd("param_directgetallnext", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() > 0) {
        // error
        addMessage("param_directgetallnext", TestProtocol.ERROR , "Parameters found");
      } // end if

      oDBM.execCmd("param_extgetall", DBMUtility.OK_OK);
      oAnswer.analyzeColsByFieldSeparator(oDBM.getAnswer(), '\t');
      if ( oAnswer.getRowCount() == 0) {
        // error
        addMessage("param_extgetall", TestProtocol.ERROR , "No parameters found");
      } // end if

      // 7. set db to online
      oDBM.execCmd("db_online", DBMUtility.OK_OK);
      
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

} // end class DBMManyParams
