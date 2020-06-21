package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DBRestart.java
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
 * Class:        DBRestart
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer db_restart command tests
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DBRestart extends DBMTestCase
{
  public DBRestart() throws SQLException, TestDatabaseException
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

      //db_restart
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_restart", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

      if (m_DBM.getAnswer().indexOf("ONLINE") >= 0) {
        addMessage("'db_restart' - in default mode", TestProtocol.INFO, m_DBM.getAnswer());
      } else {
        addMessage("'db_restart' - in default mode", TestProtocol.ERROR, m_DBM.getAnswer());
      }
      
        //db_restart, option
        m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_restart -f", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

        if (m_DBM.getAnswer().indexOf("ONLINE") >= 0) {
          addMessage("'db_restart -f' - in fast mode", TestProtocol.INFO, m_DBM.getAnswer());
        } else {
          addMessage("'db_restart -f' - in fast mode", TestProtocol.ERROR, m_DBM.getAnswer());
        }
        //db_restart, option
        m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_restart -fast", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

        if (m_DBM.getAnswer().indexOf("ONLINE") >= 0) {
          addMessage("'db_restart -fast' - in fast mode", TestProtocol.INFO, m_DBM.getAnswer());
        } else {
          addMessage("'db_restart -fast' - in fast mode", TestProtocol.ERROR, m_DBM.getAnswer());
        }

        //db_restart, option
        m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_restart -s", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

        if (m_DBM.getAnswer().indexOf("ONLINE") >= 0) {
          addMessage("'db_restart -s' - in slow mode", TestProtocol.INFO, m_DBM.getAnswer());
        } else {
          addMessage("'db_restart -s' - in slow mode", TestProtocol.ERROR, m_DBM.getAnswer());
        }
        //db_restart, option
        m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_restart -slow", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

        if (m_DBM.getAnswer().indexOf("ONLINE") >= 0) {
          addMessage("'db_restart -slow' - in slow mode", TestProtocol.INFO, m_DBM.getAnswer());
        } else {
          addMessage("'db_restart -slow' - in slow mode", TestProtocol.ERROR, m_DBM.getAnswer());
        }
      
        //db_restart, option
        m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_restart -t", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

        if (m_DBM.getAnswer().indexOf("ONLINE") >= 0) {
          addMessage("'db_restart -t' - in test mode", TestProtocol.INFO, m_DBM.getAnswer());
        } else {
          addMessage("'db_restart -t' - in test mode", TestProtocol.ERROR, m_DBM.getAnswer());
        }
        //db_restart, option
        m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_restart -test", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

        if (m_DBM.getAnswer().indexOf("ONLINE") >= 0) {
          addMessage("'db_restart -test' - in test mode", TestProtocol.INFO, m_DBM.getAnswer());
        } else {
          addMessage("'db_restart -test' - in test mode", TestProtocol.ERROR, m_DBM.getAnswer());
        }

      //db_restart, option
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      m_DBM.execCmd("db_online", DBMUtility.OK_OK);
      m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

      if (m_DBM.getAnswer().indexOf("ONLINE") >= 0) {
        addMessage("'db_online' - in default mode", TestProtocol.INFO, m_DBM.getAnswer());
      } else {
        addMessage("'db_online' - in default mode", TestProtocol.ERROR, m_DBM.getAnswer());
      }
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
    return "DBRestart";
  } // end getTestClassId
  
} //end of class DBRestart