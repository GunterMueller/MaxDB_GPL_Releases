package com.sap.sapdb.DBMTest;

/*!
  -----------------------------------------------------------------------------
  module: ParamRestart.java
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

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCase;
import com.sap.sapdb.testframe.testcase.TestCaseException;

/**
 * DBMServer trace test
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        ParamRestart
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  checks the restart behavior and the parameters
 *               especially __PARAM_CHANGED___, __PARAM_VERIFIED__, _DIAG_SEM
 * version:      7.2.*
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class ParamRestart extends TestCase {

  /**
   * A minimal constructor.Constructor.
   */
  public ParamRestart() throws SQLException, TestDatabaseException {
          super();
  } // end ParamRestart

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
    return "ParamRestart";
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

    //  create one global try-catch block to handle exceptions
    try {
      // *** 1. check the current state ***
      oDBM.execCmd("db_restart", DBMUtility.OK_OK);

      oDBM.execCmd("param_getvalue __PARAM_CHANGED___", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("1") == 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_CHANGED___", TestProtocol.ERROR , "Configuration is unexpectly dirty!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue __PARAM_VERIFIED__", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0000-00-00 00:00:00") == 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_VERIFIED__", TestProtocol.ERROR , "Timestamp is unexpectly 0!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue _DIAG_SEM", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0") == 0) {
        // error there are rows with "ON" in state
        addMessage("_DIAG_SEM", TestProtocol.ERROR , "Crashflag is unexpectly 0!");
        return;
      } // end if

      // *** 2. check after offline ***
      oDBM.execCmd("db_offline", DBMUtility.OK_OK);

      oDBM.execCmd("param_getvalue __PARAM_CHANGED___", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("1") == 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_CHANGED___", TestProtocol.ERROR , "Configuration is unexpectly dirty!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue __PARAM_VERIFIED__", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0000-00-00 00:00:00") == 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_VERIFIED__", TestProtocol.ERROR , "Timestamp is unexpectly 0!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue _DIAG_SEM", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0") != 0) {
        // error there are rows with "ON" in state
        addMessage("_DIAG_SEM", TestProtocol.ERROR , "Crashflag is unexpectly 1!");
        return;
      } // end if

      // *** 3. check after admin ***
      oDBM.execCmd("db_admin", DBMUtility.OK_OK);

      oDBM.execCmd("param_getvalue __PARAM_CHANGED___", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("1") == 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_CHANGED___", TestProtocol.ERROR , "Configuration is unexpectly dirty!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue __PARAM_VERIFIED__", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0000-00-00 00:00:00") == 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_VERIFIED__", TestProtocol.ERROR , "Timestamp is unexpectly 0!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue _DIAG_SEM", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0") == 0) {
        // error there are rows with "ON" in state
        addMessage("_DIAG_SEM", TestProtocol.ERROR , "Crashflag is unexpectly 0!");
        return;
      } // end if

      // *** 4. check after warm ***
      oDBM.execCmd("db_online", DBMUtility.OK_OK);

      oDBM.execCmd("param_getvalue __PARAM_CHANGED___", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("1") == 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_CHANGED___", TestProtocol.ERROR , "Configuration is unexpectly dirty!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue __PARAM_VERIFIED__", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0000-00-00 00:00:00") == 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_VERIFIED__", TestProtocol.ERROR , "Timestamp is unexpectly 0!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue _DIAG_SEM", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0") == 0) {
        // error there are rows with "ON" in state
        addMessage("_DIAG_SEM", TestProtocol.ERROR , "Crashflag is unexpectly 0!");
        return;
      } // end if

      // *** 5. make it dirty ***
      oDBM.execCmd("param_getvalue MAXUSERTASKS", DBMUtility.OK_OK);
      int nTasks = Integer.decode(oDBM.getAnswer().trim()).intValue();

      nTasks = ++nTasks;
      oDBM.execCmd("param_directput MAXUSERTASKS " + nTasks, DBMUtility.OK_OK);
      oDBM.execCmd("param_checkall", DBMUtility.OK_OK);


      // *** 6. dirty check in warm mode ***
      oDBM.execCmd("param_getvalue __PARAM_CHANGED___", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("1") != 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_CHANGED___", TestProtocol.ERROR , "Configuration is unexpectly clean!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue __PARAM_VERIFIED__", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0000-00-00 00:00:00") != 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_VERIFIED__", TestProtocol.ERROR , "Timestamp is unexpectly not 0!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue _DIAG_SEM", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0") == 0) {
        // error there are rows with "ON" in state
        addMessage("_DIAG_SEM", TestProtocol.ERROR , "Crashflag is unexpectly 0!");
        return;
      } // end if

      // *** 7. dirty check after offline ***
      oDBM.execCmd("db_offline", DBMUtility.OK_OK);

      oDBM.execCmd("param_getvalue __PARAM_CHANGED___", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("1") != 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_CHANGED___", TestProtocol.ERROR , "Configuration is unexpectly clean!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue __PARAM_VERIFIED__", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0000-00-00 00:00:00") != 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_VERIFIED__", TestProtocol.ERROR , "Timestamp is unexpectly not 0!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue _DIAG_SEM", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0") != 0) {
        // error there are rows with "ON" in state
        addMessage("_DIAG_SEM", TestProtocol.ERROR , "Crashflag is unexpectly 1!");
        return;
      } // end if

      // *** 9. dirty check after admin ***
      oDBM.execCmd("db_admin", DBMUtility.OK_OK);

      oDBM.execCmd("param_getvalue __PARAM_CHANGED___", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("1") != 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_CHANGED___", TestProtocol.ERROR , "Configuration is unexpectly clean!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue __PARAM_VERIFIED__", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0000-00-00 00:00:00") != 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_VERIFIED__", TestProtocol.ERROR , "Timestamp is unexpectly not 0!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue _DIAG_SEM", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0") == 0) {
        // error there are rows with "ON" in state
        addMessage("_DIAG_SEM", TestProtocol.ERROR , "Crashflag is unexpectly 0!");
        return;
      } // end if

      // *** 10. dirty check after warm (restart) ***
      oDBM.execCmd("db_online", DBMUtility.OK_OK);

      oDBM.execCmd("param_getvalue __PARAM_CHANGED___", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("1") == 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_CHANGED___", TestProtocol.ERROR , "Configuration is unexpectly dirty!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue __PARAM_VERIFIED__", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0000-00-00 00:00:00") == 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_VERIFIED__", TestProtocol.ERROR , "Timestamp is unexpectly 0!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue _DIAG_SEM", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0") == 0) {
        // error there are rows with "ON" in state
        addMessage("_DIAG_SEM", TestProtocol.ERROR , "Crashflag is unexpectly 0!");
        return;
      } // end if

      // *** 11. cleanup ***
      nTasks = --nTasks;
      oDBM.execCmd("param_directput MAXUSERTASKS " + nTasks, DBMUtility.OK_OK);
      oDBM.execCmd("param_checkall", DBMUtility.OK_OK);
      oDBM.execCmd("db_restart", DBMUtility.OK_OK);

      oDBM.execCmd("param_getvalue __PARAM_CHANGED___", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("1") == 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_CHANGED___", TestProtocol.ERROR , "Configuration is unexpectly dirty!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue __PARAM_VERIFIED__", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0000-00-00 00:00:00") == 0) {
        // error there are rows with "ON" in state
        addMessage("__PARAM_VERIFIED__", TestProtocol.ERROR , "Timestamp is unexpectly 0!");
        return;
      } // end if
      oDBM.execCmd("param_getvalue _DIAG_SEM", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("0") == 0) {
        // error there are rows with "ON" in state
        addMessage("_DIAG_SEM", TestProtocol.ERROR , "Crashflag is unexpectly 0!");
        return;
      } // end if

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

} // end class ParamRestart
