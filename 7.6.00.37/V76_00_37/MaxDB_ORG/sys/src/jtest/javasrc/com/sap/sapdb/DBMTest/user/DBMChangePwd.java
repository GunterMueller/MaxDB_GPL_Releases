package com.sap.sapdb.DBMTest.user;

/*!
  -----------------------------------------------------------------------------
  module: DBMChangePwd.java
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
 * Class:        DBMChangePwd
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  changes passwords of the user dbm-user, sysdba and domain
 * version:      7.2.*
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class DBMChangePwd extends DBMTestCase
{
  /**
   * A minimal constructor.Constructor.
   */
  public DBMChangePwd() throws SQLException, TestDatabaseException
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
    return "DBMChangePwd";
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
   * runDBMTest() contains the test implementation
   */
  public void runDBMTest()
  {
    String     sUserPut;
    String     sPassword;
    String     sConnect;
    String     sCommand;

    //  create one global try-catch block to handle exceptions
    try
    {
      // 1. Database Manager Operator (DBM)

      // 1.1 WARM mode
      m_DBM.execCmd("db_online", DBMUtility.OK_OK);

      //set password
      sPassword = "elch1";
      m_DBM.execCmd("user_put " + getDatabase().getDBMUser() + " PASSWORD=" + sPassword, DBMUtility.OK_OK);
      
      // check password
      m_DBM.execCmd("user_logon " + getDatabase().getDBMUser() + "," + sPassword, DBMUtility.OK_OK);
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);

      m_DBM.execCmd("db_cold", DBMUtility.OK_OK);
      m_DBM.execCmd("util_connect", DBMUtility.OK_OK);
      m_DBM.execCmd("util_connect " + getDatabase().getDBMUser() + "," + sPassword, DBMUtility.OK_OK);

      m_DBM.execCmd("db_online", DBMUtility.OK_OK);
      m_DBM.execCmd("util_connect", DBMUtility.OK_OK);
      m_DBM.execCmd("util_connect " + getDatabase().getDBMUser() + "," + sPassword, DBMUtility.OK_OK);
      m_DBM.execCmd("sql_connect", DBMUtility.OK_OK);
      m_DBM.execCmd("sql_connect " + getDatabase().getDBMUser() + "," + sPassword, DBMUtility.OK_OK);

      // 1.2 COLD mode
      m_DBM.execCmd("db_cold", DBMUtility.OK_OK);
      
      //set password
      sPassword = "elch2";
      m_DBM.execCmd("user_put " + getDatabase().getDBMUser() + " PASSWORD=" + sPassword, DBMUtility.OK_OK);
      
      // check password
      m_DBM.execCmd("user_logon " + getDatabase().getDBMUser() + "," + sPassword, DBMUtility.OK_OK);
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);

      m_DBM.execCmd("db_cold", DBMUtility.OK_OK);
      m_DBM.execCmd("util_connect", DBMUtility.OK_OK);
      m_DBM.execCmd("util_connect " + getDatabase().getDBMUser() + "," + sPassword, DBMUtility.OK_OK);

      m_DBM.execCmd("db_online", DBMUtility.OK_OK);
      m_DBM.execCmd("util_connect", DBMUtility.OK_OK);
      m_DBM.execCmd("util_connect " + getDatabase().getDBMUser() + "," + sPassword, DBMUtility.OK_OK);
      m_DBM.execCmd("sql_connect", DBMUtility.OK_OK);
      m_DBM.execCmd("sql_connect " + getDatabase().getDBMUser() + "," + sPassword, DBMUtility.OK_OK);

      // 1.3 OFFLINE mode
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      
      //set password
      sPassword = "elch3";
      m_DBM.execCmd("user_put " + getDatabase().getDBMUser() + " PASSWORD=" + sPassword, DBMUtility.OK_OK);
      
      // check password
      m_DBM.execCmd("user_logon " + getDatabase().getDBMUser() + "," + sPassword, DBMUtility.OK_OK);
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);

      m_DBM.execCmd("db_cold", DBMUtility.OK_OK);
      m_DBM.execCmd("util_connect", DBMUtility.OK_OK);
      m_DBM.execCmd("util_connect " + getDatabase().getDBMUser() + "," + sPassword, DBMUtility.OK_OK);

      m_DBM.execCmd("db_online", DBMUtility.OK_OK);
      m_DBM.execCmd("util_connect", DBMUtility.OK_OK);
      m_DBM.execCmd("util_connect " + getDatabase().getDBMUser() + "," + sPassword, DBMUtility.OK_OK);
      m_DBM.execCmd("sql_connect", DBMUtility.OK_OK);
      m_DBM.execCmd("sql_connect " + getDatabase().getDBMUser() + "," + sPassword, DBMUtility.OK_OK);

      // 1.5 restore old password
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      sUserPut = "user_put " + getDatabase().getDBMUser() + " PASSWORD=" + getDatabase().getDBMPassword();
      m_DBM.execCmd(sUserPut, DBMUtility.OK_OK);
      
      // 1.6 set and delete second password
      // check non existing password
      sConnect = "user_logon " + getDatabase().getDBMUser() + ",schneewittchen";
      m_DBM.execCmd(sConnect, DBMUtility.ERR_USRFAIL);
      sConnect = "user_logon " + getDatabase().getDBMUser() + "," + getDatabase().getDBMPassword(); 
      m_DBM.execCmd(sConnect, DBMUtility.OK_OK);
      // set password and check
      sUserPut = "user_put " + getDatabase().getDBMUser() + " SECONDPASSWORD=schneewittchen";
      m_DBM.execCmd(sUserPut, DBMUtility.OK_OK);
      sConnect = "user_logon " + getDatabase().getDBMUser() + ",schneewittchen";
      m_DBM.execCmd(sConnect, DBMUtility.OK_OK);
      // delete password and check
      sUserPut = "user_put " + getDatabase().getDBMUser() + " SECONDPASSWORD=";
      m_DBM.execCmd(sUserPut, DBMUtility.OK_OK);
      sConnect = "user_logon " + getDatabase().getDBMUser() + ",schneewittchen";
      m_DBM.execCmd(sConnect, DBMUtility.ERR_USRFAIL);
      // connect with first password
      sConnect = "user_logon " + getDatabase().getDBMUser() + "," + getDatabase().getDBMPassword(); 
      m_DBM.execCmd(sConnect, DBMUtility.OK_OK);
		
      // 1.7 check exakt the DBMGui command 
      sUserPut = "user_put " + getDatabase().getDBMUser() + " SERVERRIGHTS=+DBInfoRead,+SystemCmd,+ExecLoad,+UserMgm,+DBFileRead,+Backup,+InstallMgm,+LoadSysTab,+ParamCheckWrite,+ParamFull,+ParamRead,+DBStart,+DBStop,+Recovery,+AccessSQL,+AccessUtility,+SharedMemoryMgm,+SchedulerMgm,+Scheduling,+EvtDispMgm,+EvtDisp DISABLED=NO COMMENT=\"\" SECONDPASSWORD=";
      m_DBM.execCmd(sUserPut, DBMUtility.OK_OK);

      // 2. SYSDBA (DBA)
      sUserPut = "user_put " + getDatabase().getDBAUser() + " PASSWORD=elch";

      // 2.1 OFFLINE mode
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      m_DBM.execCmd(sUserPut, DBMUtility.ERR_DBNORUN);

      // 2.2 COLD mode
      m_DBM.execCmd("db_cold", DBMUtility.OK_OK);
      m_DBM.execCmd(sUserPut, DBMUtility.ERR_DBNORUN);

      // 2.3 WARM mode
      m_DBM.execCmd("db_online", DBMUtility.OK_OK);
      m_DBM.execCmd(sUserPut, DBMUtility.OK_OK);

      // 2.4 Check new password
      sConnect = "util_connect " + getDatabase().getDBAUser() + ",elch";
      m_DBM.execCmd(sConnect, DBMUtility.OK_OK);

      sConnect = "sql_connect " + getDatabase().getDBAUser() + ",elch";
      m_DBM.execCmd(sConnect, DBMUtility.OK_OK);

      sConnect = "user_logon " + getDatabase().getDBAUser() + ",elch";
      m_DBM.execCmd(sConnect, DBMUtility.OK_OK);

      // 2.5 restore old password
      sConnect = "user_logon " + getDatabase().getDBMUser() + ","  + getDatabase().getDBMPassword();
      m_DBM.execCmd(sConnect, DBMUtility.OK_OK);

      sUserPut = "user_put " + getDatabase().getDBAUser() + " PASSWORD=" + getDatabase().getDBAPassword();
      m_DBM.execCmd(sUserPut, DBMUtility.OK_OK);
      
      // 3. Tracking of SYSDBA password changes
      
      // 3.1. ALTER PASSWORD <old> to <new> by SYSDBA
      // set
      sCommand = "db_execute user-type=DBA alter password " + getDatabase().getDBAPassword() + " to willy";
      m_DBM.execCmd(sCommand, DBMUtility.OK_OK);

      // check
      m_DBM.execCmd("db_execute user-type=DBA select user from dual", DBMUtility.OK_OK);
      
      // reset
      sCommand = "db_execute alter password "  + getDatabase().getDBAUser() + " " + getDatabase().getDBAPassword();
      m_DBM.execCmd(sCommand, DBMUtility.OK_OK);
      
      // 3.2. ALTER PASSWORD <user> <new> by SYSDBA
      // set
      sCommand = "db_execute user-type=DBA alter password " + getDatabase().getDBAUser() + " willy";
      m_DBM.execCmd(sCommand, DBMUtility.OK_OK);

      // check
      m_DBM.execCmd("db_execute user-type=DBA select user from dual", DBMUtility.OK_OK);
      
      // reset
      sCommand = "db_execute alter password "  + getDatabase().getDBAUser() + " " + getDatabase().getDBAPassword();
      m_DBM.execCmd(sCommand, DBMUtility.OK_OK);
      
      // 3.2. ALTER PASSWORD <user> <new> by DBM
      // set
      sCommand = "db_execute alter password " + getDatabase().getDBAUser() + " willy";
      m_DBM.execCmd(sCommand, DBMUtility.OK_OK);

      // check
      m_DBM.execCmd("db_execute user-type=DBA select user from dual", DBMUtility.OK_OK);
      
      // reset
      sCommand = "db_execute alter password "  + getDatabase().getDBAUser() + " " + getDatabase().getDBAPassword();
      m_DBM.execCmd(sCommand, DBMUtility.OK_OK);

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

} // end class DBMChangePwd
