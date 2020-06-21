package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DBMSetpath.java
  -----------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
import java.util.StringTokenizer;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBMSetpath
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer dbm_setpath command tests
 * version:      7.4.*
 * Copyright (c) 2002-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DBMSetpath extends DBMTestCase
{
  public DBMSetpath() throws SQLException, TestDatabaseException
  {
    super();
  }
  /**
   * run() contains the test implementation
   */
  public void runDBMTest()
  {
    TestDatabaseSap sapdb   = (TestDatabaseSap) getDatabase();
    String myIndepDataPath  = "";

    //  create one global try-catch block to handle exceptions
    try
    {
      if (m_DBM.getSrvProperty("dbmversion.os").compareTo("UNIX") == 0) {
        addMessage("Class member function run()",
                   TestProtocol.WARNING,
                   getTestClassId() + " does not work on UNIX systems");
      } else {

        // Getting original value
      	m_DBM.execCmd("dbm_getpath IndepDataPath", DBMUtility.OK_OK);
        String DBMAnwser = m_DBM.getAnswer();
        StringTokenizer st_param = new StringTokenizer(DBMAnwser,"\n");
        while (st_param.hasMoreTokens()) {
          myIndepDataPath = st_param.nextToken().trim();
          break;
        } // end while

        if (DBMAnwser.indexOf("Path extracted from $DBROOT") >= 0) {
          addMessage("'dbm_setpath' is not working --> ", TestProtocol.WARNING, m_DBM.getAnswer());
        } else {
        	// setting path
        	m_DBM.execCmd("dbm_setpath",                                           DBMUtility.ERR_TOFEWPARAM);
        	m_DBM.execCmd("dbm_setpath IndepDataPath",                             DBMUtility.ERR_TOFEWPARAM);
        	m_DBM.execCmd("dbm_setpath IndepDataPath \"" + myIndepDataPath + "\"", DBMUtility.OK_OK);

        	// checking path
        	m_DBM.execCmd("dbm_getpath IndepDataPath", DBMUtility.OK_OK);
        	if (m_DBM.getAnswer().compareTo(myIndepDataPath) >= 0) {
        		addMessage("'dbm_setpath'", TestProtocol.INFO, m_DBM.getAnswer());
        	} else {
        		addMessage("'dbm_setpath'", TestProtocol.ERROR, m_DBM.getAnswer());
        	} // end if
        } // end if
    
      } // end if
    } catch (Exception e) {
      // standard error handler
      this.handleExceptions(e);
    } // end try catch

    // clean up, i.e. close statements and connections
    finally {
      try {
        // print all parameters from the parameter object to the protocol.
        printParametersToProtocol();
      } catch (Exception e) {
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
    return "DBMSetpath";
  } // end getTestClassId
} //end of class MediumGet