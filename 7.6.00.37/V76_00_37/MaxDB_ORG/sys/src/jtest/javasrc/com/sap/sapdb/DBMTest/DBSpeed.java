package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DBSpeed.java
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

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBSpeed
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer db_speed command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DBSpeed extends DBMTestCase
{
  public DBSpeed() throws SQLException, TestDatabaseException
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
      //db_speed on existing database - speed 'FAST'
      m_DBM.execCmd("db_speed",DBMUtility.OK_OK);
      String param = m_DBM.getAnswer();

      if (param.indexOf("FAST") < 0) {
        addMessage("db_speed - expected 'FAST' mode", TestProtocol.ERROR, param);
      } // end if

      //Restarting database with slow kernel
      m_DBM.execCmd("db_offline",DBMUtility.OK_OK);
      m_DBM.execCmd("db_online -s",DBMUtility.OK_OK);

      //db_speed on existing database - speed 'SLOW'
      m_DBM.execCmd("db_speed",DBMUtility.OK_OK);
      param = m_DBM.getAnswer();

      if (param.indexOf("SLOW") < 0) {
        addMessage("db_speed - expected 'SLOW' mode", TestProtocol.ERROR, param);
      } // end if

      //Restarting database with test kernel
      m_DBM.execCmd("db_offline",DBMUtility.OK_OK);
      m_DBM.execCmd("db_online -t",DBMUtility.OK_OK);

      //db_speed on existing database - speed 'TEST'
      m_DBM.execCmd("db_speed",DBMUtility.OK_OK);
      param = m_DBM.getAnswer();

      if (param.indexOf("TEST") < 0) {
        addMessage("db_speed - expected 'TEST' mode", TestProtocol.ERROR, param);
      } // end if
      
      //Restarting database with fast kernel
      m_DBM.execCmd("db_offline",DBMUtility.OK_OK);
      m_DBM.execCmd("db_online -f",DBMUtility.OK_OK);
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
    return "DBSpeed";
  } // end getTestClassId
} //end of class MediumGet