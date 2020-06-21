package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: SQLExecute.java
  -----------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        SQLExecute
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer sql_execute command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

//public class SQLExecute extends BackupState
public class SQLExecute extends TestCase
{
  public SQLExecute() throws SQLException, TestDatabaseException
  {
    super();
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM       = new  DBMUtility(getDatabase(), this);
    //  create one global try-catch block to handle exceptions
    try
    {
      // without statement
      oDBM.execCmd("sql_execute", DBMUtility.ERR_TOFEWPARAM);
      // with statement
      oDBM.execCmd("sql_execute select * from tables", DBMUtility.OK_OK);

      // with sql_connect
      oDBM.execCmd("sql_connect", DBMUtility.OK_OK);
      // without statement
      oDBM.execCmd("sql_execute", DBMUtility.ERR_TOFEWPARAM);
      // with statement
      oDBM.execCmd("sql_execute select * from tables", DBMUtility.OK_OK);
      // release
      oDBM.execCmd("sql_release", DBMUtility.OK_OK);

      // database offline
      oDBM.execCmd("db_offline", DBMUtility.OK_OK);
      oDBM.execCmd("sql_execute", DBMUtility.ERR_TOFEWPARAM);
      oDBM.execCmd("sql_execute select * from tables", DBMUtility.ERR_DBNORUN);

      // database online
      oDBM.execCmd("db_online", DBMUtility.OK_OK);
      
      // check usertypes
      oDBM.execCmd("sql_execute select * from dual where user = '" + getDatabase().getDBMUser() + "'", DBMUtility.OK_OK);
      oDBM.execCmd("sql_execute user-type=DBM select * from dual where user = '" + getDatabase().getDBMUser() + "'", DBMUtility.OK_OK);
      oDBM.execCmd("sql_execute user-type=DBA select * from dual where user = '" + getDatabase().getDBAUser() + "'", DBMUtility.OK_OK);
      oDBM.execCmd("sql_execute user-type=DBA create user schneewittchen password zwerg", DBMUtility.OK_OK);
      oDBM.execCmd("user_sap schneewittchen,zwerg", DBMUtility.OK_OK);
      oDBM.execCmd("sql_execute user-type=SAP select * from dual where user = 'SCHNEEWITTCHEN'", DBMUtility.OK_OK);
      oDBM.execCmd("sql_execute user-type=DBA drop user schneewittchen", DBMUtility.OK_OK);
      
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
        // do nothing
      } // end try catch
    } // end finally
  } // end run
  /**
   * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
   * will set the name to the name of the java class. To make the test name independend
   * from the java implementation, this method should return a unique string.
   * Attention: The uniqueness is not automatically checked by anybody.
   * @return The UNIQUE name of the test class
   */
  public static String getTestClassId() {
    return "SQLExecute";
  } // end getTestClassId
} //end of class MediumGet