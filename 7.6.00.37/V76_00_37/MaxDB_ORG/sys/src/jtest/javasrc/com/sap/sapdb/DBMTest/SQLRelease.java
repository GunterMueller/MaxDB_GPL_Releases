package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: SQLRelease.java
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
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCase;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        SQLRelease
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer sql_release command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class SQLRelease extends TestCase
{
  public SQLRelease() throws SQLException, TestDatabaseException
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
    	int nTaskCountInit = 0;
    	int nTaskCount     = 0;
    	int nIndex         = 0;
    	
    	// check initial tasks
	    oDBM.execCmd("show tasks", DBMUtility.OK_OK);
	    nIndex = 0;
	    while ((nIndex = oDBM.getAnswer().toString().indexOf("Command wait", nIndex)) >= 0) {
	    	nIndex = nIndex + 12;
	    	++nTaskCountInit;
	    } // end while

	    //Without sql_connect
	    oDBM.execCmd("sql_release", DBMUtility.OK_OK);
	    oDBM.execCmd("show tasks", DBMUtility.OK_OK);
	    nIndex = 0;
	    nTaskCount = 0;
	    while ((nIndex = oDBM.getAnswer().toString().indexOf("Command wait", nIndex)) >= 0) {
	    	nIndex = nIndex + 12;
	    	++nTaskCount;
	    } // end while
        if (nTaskCount != nTaskCountInit) {
	    	addMessage("show tasks - unexpected user task found", TestProtocol.ERROR, oDBM.getAnswer());
        } // end if
	    
	    //With sql_connect
	    oDBM.execCmd("sql_connect", DBMUtility.OK_OK);
	    oDBM.execCmd("show tasks", DBMUtility.OK_OK);
	    nIndex = 0;
	    nTaskCount = 0;
	    while ((nIndex = oDBM.getAnswer().toString().indexOf("Command wait", nIndex)) >= 0) {
	    	nIndex = nIndex + 12;
	    	++nTaskCount;
	    } // end while
        if (nTaskCount != (nTaskCountInit + 1)) {
	    	addMessage("show tasks - no additional user task found", TestProtocol.ERROR, oDBM.getAnswer());
        } // end if
	    oDBM.execCmd("sql_release", DBMUtility.OK_OK);
	    oDBM.execCmd("show tasks", DBMUtility.OK_OK);
	    nIndex = 0;
	    nTaskCount = 0;
	    while ((nIndex = oDBM.getAnswer().toString().indexOf("Command wait", nIndex)) >= 0) {
	    	nIndex = nIndex + 12;
	    	++nTaskCount;
	    } // end while
        if (nTaskCount != nTaskCountInit) {
	    	addMessage("show tasks - unexpected user task found", TestProtocol.ERROR, oDBM.getAnswer());
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
    return "SQLRelease";
  } // end getTestClassId
} //end of class MediumGet