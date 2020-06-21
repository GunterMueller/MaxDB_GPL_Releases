package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DBCons.java
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
 * Class:        DBCons
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer db_cons command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DBCons extends DBMTestCase
{
  public DBCons() throws SQLException, TestDatabaseException
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
      //Too few parameters
      m_DBM.execCmd("db_cons",DBMUtility.ERR_TOFEWPARAM);
      //db_cons - option
      m_DBM.execCmd("db_cons TIME ENABLE",DBMUtility.OK_OK);
      m_DBM.execCmd("db_cons TIME DISABLE",DBMUtility.OK_OK);
      //db_cons - option
      m_DBM.execCmd("db_cons SHOW",DBMUtility.OK_OK);
      //db_cons - option
      m_DBM.execCmd("db_cons IO",new int [] {DBMUtility.OK_OK,DBMUtility.ERR_EXECUTE}, false);
      // check answer
      if ((m_DBM.getAnswer().indexOf("usage: x_cons") == -1) && (m_DBM.getAnswer().indexOf("nknown command:") == -1)) {
		  addMessage("Unexpected answer of db_cons!", TestProtocol.ERROR,
				  "Output:\n"+m_DBM.getAnswer()+"\n("+__FILE__()+","+ __LINE__()+")");
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
    return "DBCons";
  } // end getTestClassId
} //end of class MediumGet
