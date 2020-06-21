package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DBAdmin.java
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
 * Class:        DBAdmin
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer db_admin command tests
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DBAdmin extends DBMTestCase
{
  public DBAdmin() throws SQLException, TestDatabaseException
  {
    super();
  }
  /**
   * run() contains the test implementation
   */
  public void runDBMTest()
  {
  	try {
  	
    //  db_admin without option
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_admin", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);
      if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
        addMessage("'db_admin' - in default mode", TestProtocol.INFO, m_DBM.getAnswer());
      } else {
        addMessage("'db_admin' - in default mode", TestProtocol.ERROR, m_DBM.getAnswer());
      }
      
      // db_admin, option -f
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_admin -f", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);
      if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
        addMessage("'db_admin -f' - in fast mode", TestProtocol.INFO, m_DBM.getAnswer());
      } else {
        addMessage("'db_admin -f' - in fast mode", TestProtocol.ERROR, m_DBM.getAnswer());
      }
    
      // db_admin, option -fast
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_admin -fast", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);
      if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
        addMessage("'db_admin -fast' - in fast mode", TestProtocol.INFO, m_DBM.getAnswer());
      } else {
        addMessage("'db_admin -fast' - in fast mode", TestProtocol.ERROR, m_DBM.getAnswer());
      }

      // db_admin, option -s 
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_admin -s", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);
      if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
        addMessage("'db_admin -s' - in slow mode", TestProtocol.INFO, m_DBM.getAnswer());
      } else {
        addMessage("'db_admin -s' - in slow mode", TestProtocol.ERROR, m_DBM.getAnswer());
      }
    
      // db_admin, option -slow
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_admin -slow", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);
      if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
        addMessage("'db_admin -slow' - in slow mode", TestProtocol.INFO, m_DBM.getAnswer());
      } else {
        addMessage("'db_admin -slow' - in slow mode", TestProtocol.ERROR, m_DBM.getAnswer());
      }
        
      // db_admin, option -t
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_admin -t", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);

      if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
        addMessage("'db_admin -t' - in test mode", TestProtocol.INFO, m_DBM.getAnswer());
      } else {
        addMessage("'db_admin -t' - in test mode", TestProtocol.ERROR, m_DBM.getAnswer());
      }
    
      // db_admin, option -test
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_admin -test", DBMUtility.OK_OK);
      Thread.sleep(1000);
      m_DBM.execCmd("db_state", DBMUtility.OK_OK, false);
      if (m_DBM.getAnswer().indexOf("ADMIN") >= 0) {
        addMessage("'db_admin -test' - in test mode", TestProtocol.INFO, m_DBM.getAnswer());
      } else {
        addMessage("'db_admin -test' - in test mode", TestProtocol.ERROR, m_DBM.getAnswer());
      }

      // db_restart, normal
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
    return "DBAdmin";
  } // end getTestClassId
  
} //end of class DBAdmin