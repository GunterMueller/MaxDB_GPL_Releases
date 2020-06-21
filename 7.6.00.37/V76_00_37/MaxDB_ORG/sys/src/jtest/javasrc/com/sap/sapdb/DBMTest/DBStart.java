package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DBStart.java
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
 * Class:        DBStart
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer db_start command tests
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DBStart extends DBMTestCase
{
  public DBStart() throws SQLException, TestDatabaseException
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

      // db_start without option
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_start", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

      if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
        addMessage("'db_start' - in default mode", TestProtocol.INFO, m_DBM.getAnswer());
      } else {
        addMessage("'db_start' - in default mode", TestProtocol.ERROR, m_DBM.getAnswer());
      }
      
        // db_start, option -f
        m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_start -f", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

        if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
          addMessage("'db_start -f' - in fast mode", TestProtocol.INFO, m_DBM.getAnswer());
        } else {
          addMessage("'db_start -f' - in fast mode", TestProtocol.ERROR, m_DBM.getAnswer());
        }

        // db_start, option -fast
        m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_start -fast", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

        if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
          addMessage("'db_start -fast' - in fast mode", TestProtocol.INFO, m_DBM.getAnswer());
        } else {
          addMessage("'db_start -fast' - in fast mode", TestProtocol.ERROR, m_DBM.getAnswer());
        }

        // db_start, option -s
        m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_start -s", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

        if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
          addMessage("'db_start -s' - in slow mode", TestProtocol.INFO, m_DBM.getAnswer());
        } else {
          addMessage("'db_start -s' - in slow mode", TestProtocol.ERROR, m_DBM.getAnswer());
        }

        // db_start, option -slow
        m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_start -slow", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

        if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
          addMessage("'db_start -slow' - in slow mode", TestProtocol.INFO, m_DBM.getAnswer());
        } else {
          addMessage("'db_start -slow' - in slow mode", TestProtocol.ERROR, m_DBM.getAnswer());
        }

        // db_start, option -t
        m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_start -t", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

        if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
          addMessage("'db_start -t' - in test mode", TestProtocol.INFO, m_DBM.getAnswer());
        } else {
          addMessage("'db_start -t' - in test mode", TestProtocol.ERROR, m_DBM.getAnswer());
        }

        // db_start, option -test
        m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_start -test", DBMUtility.OK_OK);
        Thread.sleep(1000);
        m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

        if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
          addMessage("'db_start -test' - in test mode", TestProtocol.INFO, m_DBM.getAnswer());
        } else {
          addMessage("'db_start -test' - in test mode", TestProtocol.ERROR, m_DBM.getAnswer());
        }

      // db_restart
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      m_DBM.execCmd("db_online", DBMUtility.OK_OK);
      m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

      if (m_DBM.getAnswer().indexOf("ONLINE") >= 0) {
        addMessage("'db_online' - in default mode", TestProtocol.INFO, m_DBM.getAnswer());
      } else {
        addMessage("'db_online' - in default mode", TestProtocol.ERROR, m_DBM.getAnswer());
      }

    } catch (Exception e) {
      // standard error handler
      this.handleExceptions(e);
    } // end try catch

  } // end runDBMTest
  
  /**
   * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
   * will set the name to the name of the java class. To make the test name independend
   * from the java implementation, this method should return a unique string.
   * Attention: The uniqueness is not automatically checked by anybody.
   * @return The UNIQUE name of the test class
   */
  public static String getTestClassId() {
    return "DBStart";
  } // end getTestClassId

} //end of class DBStart