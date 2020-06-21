package com.sap.sapdb.DBMTest;

/*!
  -----------------------------------------------------------------------------
  module: DBMExample.java
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
import com.sap.sapdb.testframe.testcase.TestCaseException;

/**
 * DBMServer test example
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBMExample
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer test example
 * version:      7.2.*
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class DBMExample extends DBMTestCase
{
  /**
   * A minimal constructor.Constructor.
   */
  public DBMExample() throws SQLException, TestDatabaseException
  {
          super();
  } // end DBMExample

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
    return "DBMExample";
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

  /**
   * run() contains the test implementation
   */
  public void runDBMTest()
  {
    //  create one global try-catch block to handle exceptions
    try
    {
      // successfull command
      m_DBM.execCmd("dbm_version", DBMUtility.OK_OK);
      // unsucessfull command without extended error description
      m_DBM.execCmd("dbm_unsinn", DBMUtility.ERR_COMMAND);
      // successfull command
      m_DBM.execCmd("param_startsession", DBMUtility.OK_OK);
      // unsucessfull command with extended error description
      m_DBM.execCmd("param_commitsession", DBMUtility.ERR_XPCHECK);
      // successfull command
      m_DBM.execCmd("param_abortsession", DBMUtility.OK_OK);
      // unsuccessfull command with second error code
      m_DBM.execCmd("sql_connect nicht,vorhanden", DBMUtility.ERR_SQL);
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

} // end class DBMExample
