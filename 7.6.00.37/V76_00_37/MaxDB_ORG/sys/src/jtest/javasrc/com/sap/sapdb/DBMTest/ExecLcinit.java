package com.sap.sapdb.DBMTest;

/*!
  -----------------------------------------------------------------------------
  module: ExecLcinit.java
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
import com.sap.sapdb.testframe.testcase.TestCase;
import com.sap.sapdb.testframe.testcase.TestCaseException;

/**
 * DBMServer ExecLcinit
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        ExecLcinit
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer ExecLcinit
 * version:      7.2.*
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class ExecLcinit extends TestCase
{
  /**
   * A minimal constructor.Constructor.
   */
  public ExecLcinit() throws SQLException, TestDatabaseException
  {
          super();
  } // end ExecLcinit

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
    return "ExecLcinit";
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
  public void run()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);

    //  create one global try-catch block to handle exceptions
    try
    {
      // Check for non MCOD 
      oDBM.execCmd("param_directput INSTANCE_TYPE LVC", DBMUtility.OK_OK);
      oDBM.execCmd("param_directput MCOD NO", DBMUtility.OK_OK);
      oDBM.execCmd("param_checkall", DBMUtility.OK_OK);

          // INIT
              // OFFLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit init", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","No MCOD/Init/OFFLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit init debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'OmsTstKnl","No MCOD/Init/OFFLINE/Debug");

              // ADMIN
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit init", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","No MCOD/Init/ADMIN/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit init debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'OmsTstKnl","No MCOD/Init/ADMIN/Debug");
              // ONLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit init", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","No MCOD/Init/ONLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit init debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'OmsTstKnl","No MCOD/Init/ONLINE/Debug");

          // RESTART
              // OFFLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit restart", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","No MCOD/Restart/OFFLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit restart debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'OmsTstKnl","No MCOD/Restart/OFFLINE/Debug");
              // ADMIN
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit restart", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","No MCOD/Restart/ADMIN/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit restart debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'OmsTstKnl","No MCOD/Restart/ADMIN/Debug");
              // ONLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit restart", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","No MCOD/Restart/ONLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit restart debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","No MCOD/Restart/ONLINE/Debug");

          // SLOW
              // OFFLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit slow", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'SlowKnl","No MCOD/Slow/OFFLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit slow debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'SlowKnl","No MCOD/Slow/OFFLINE/Debug");
              // ADMIN
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit slow", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'SlowKnl","No MCOD/Slow/ADMIN/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit slow debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'SlowKnl","No MCOD/Slow/ADMIN/Debug");
              // ONLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit slow", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","No MCOD/Slow/ONLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit slow debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","No MCOD/Slow/ONLINE/Debug");

          // REGISTER
              // OFFLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit register", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","No MCOD/Register/OFFLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit register debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'OmsTstKnl","No MCOD/Register/OFFLINE/Normal");
              // ADMIN
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit register", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","No MCOD/Register/ADMIN/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit register debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'OmsTstKnl","No MCOD/Register/ADMIN/Debug");
              // ONLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit register", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","No MCOD/Register/ONLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit register debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","No MCOD/Register/ONLINE/Debug");

          // SHUTDOWN
              // OFFLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit shutdown", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("OFFLINE","","No MCOD/Shutdown/OFFLINE");
              // ADMIN
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit shutdown", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("OFFLINE","","No MCOD/Shutdown/ADMIN");
              // ONLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit shutdown", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("OFFLINE","","No MCOD/Shutdown/ONLINE");

          // STOP
              // OFFLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit stop", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("OFFLINE","","No MCOD/Stop/OFFLINE");
              // ADMIN
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit stop", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("OFFLINE","","No MCOD/Stop/ADMIN");
              // ONLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit stop", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("OFFLINE","","No MCOD/Stop/ONLINE");

      // Check for MCOD 
      oDBM.execCmd("param_directput MCOD YES", DBMUtility.OK_OK);
      oDBM.execCmd("param_checkall", DBMUtility.OK_OK);

          // INIT
              // OFFLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit init", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Init/OFFLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit init debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'OmsTstKnl","MCOD/Init/OFFLINE/Debug");
              // ADMIN
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit init", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Init/ADMIN/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit init debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'OmsTstKnl","MCOD/Init/ADMIN/Debug");
              // ONLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit init", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Init/ONLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit init debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Init/ONLINE/Debug");

          // RESTART
              // OFFLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit restart", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Restart/OFFLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit restart debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'OmsTstKnl","MCOD/Restart/OFFLINE/Debug");
              // ADMIN
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit restart", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Restart/ADMIN/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit restart debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'OmsTstKnl","MCOD/Restart/ADMIN/Debug");
              // ONLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit restart", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Restart/ONLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit restart debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Restart/ONLINE/Debug");

          // SLOW
              // OFFLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit slow", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'SlowKnl","MCOD/Slow/OFFLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit slow debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'SlowKnl","MCOD/Slow/OFFLINE/Debug");
              // ADMIN
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit slow", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'SlowKnl","MCOD/Slow/ADMIN/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit slow debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'SlowKnl","MCOD/Slow/ADMIN/Debug");
              // ONLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit slow", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Slow/ONLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit slow debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Slow/ONLINE/Debug");

          // REGISTER
              // OFFLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit register", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Register/OFFLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit register debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'OmsTstKnl","MCOD/Register/OFFLINE/Normal");
              // ADMIN
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit register", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Register/ADMIN/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit register debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'OmsTstKnl","MCOD/Register/ADMIN/Debug");
              // ONLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit register", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Register/ONLINE/Normal");
                  // Debug
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit register debug", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("ONLINE","'Kernel","MCOD/Register/ONLINE/Debug");

          // SHUTDOWN
              // OFFLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit shutdown", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("OFFLINE","","MCOD/Shutdown/OFFLINE");
              // ADMIN
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit shutdown", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("OFFLINE","","MCOD/Shutdown/ADMIN");
              // ONLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit shutdown", DBMUtility.ERR_EXECUTE);
                  oDBM.checkStateAndKernel("ONLINE","","MCOD/Shutdown/ONLINE");

          // STOP
              // OFFLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit stop", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("OFFLINE","","MCOD/Stop/OFFLINE");
              // ADMIN
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_admin", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit stop", DBMUtility.OK_OK);
                  oDBM.checkStateAndKernel("OFFLINE","","MCOD/Stop/ADMIN");
              // ONLINE
                  // Normal
                  oDBM.execCmd("db_offline", DBMUtility.OK_OK);
                  oDBM.execCmd("db_online", DBMUtility.OK_OK);
                  oDBM.execCmd("exec_lcinit stop", DBMUtility.ERR_EXECUTE);
                  oDBM.checkStateAndKernel("ONLINE","","MCOD/Stop/ONLINE");

      oDBM.execCmd("param_directput INSTANCE_TYPE OLTP", DBMUtility.OK_OK);
      oDBM.execCmd("param_directput MCOD NO", DBMUtility.OK_OK);
      oDBM.execCmd("param_checkall", DBMUtility.OK_OK);

      oDBM.execCmd("db_offline", DBMUtility.OK_OK);
      oDBM.execCmd("db_reinstall", DBMUtility.OK_OK);
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

} // end class ExecLcinit
