package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DiagHistlist.java
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

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DiagHistlist
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer diag_histlist command tests
 * version:      7.4.*
 * Copyright (c) 2002-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DiagHistlist extends BackupState
{

  public DiagHistlist() throws SQLException, TestDatabaseException
  {
    super();
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM           = new  DBMUtility(getDatabase(), this);
    boolean m_bHistEntryFound = false;
    int     nCounter          = 0;

    //  create one global try-catch block to handle exceptions
    try
    {
      if (oDBM.getSrvProperty("dbmversion.os").compareTo("UNIX") == 0) {

        addMessage(this.toString() + ".run",
                   TestProtocol.WARNING,
                   "DiagHistlist does not work on UNIX systems");
      } else {
        // No kernel crash
//        oDBM.execCmd("diag_histlist", DBMUtility.ERR_FILE);

        // Provoking kernel crash
        oDBM.execCmd("db_cons crash dump", DBMUtility.OK_OK);
        
        oDBM.execCmd("db_state", DBMUtility.OK_OK, false);
        while (((oDBM.getAnswer().toUpperCase().indexOf("STOPPED INCORRECTLY") < 0) ||
                (oDBM.getAnswer().toUpperCase().indexOf("OFFLINE")             < 0)    ) &&
                (nCounter < 5)) {
          Thread.sleep(20000);
          oDBM.execCmd("db_state", DBMUtility.OK_OK, false);
          ++nCounter;
        }

        oDBM.execCmd("db_online -f", DBMUtility.OK_OK);

        Thread.sleep(10000);

        // Getting diag history list
        oDBM.execCmd("diag_histlist", DBMUtility.OK_OK);

        StringTokenizer stParam = new StringTokenizer(oDBM.getAnswer(),"\n");

        while (stParam.hasMoreTokens()) {

          if (stParam.nextToken().toString().toUpperCase().indexOf("DIAGHISTORY" +
              oDBM.getSrvProperty("file.separator") + getDatabase().getName().toUpperCase()) >= 0) {

            m_bHistEntryFound = true;
            break;
          }
        }

        if (m_bHistEntryFound == true) {

          addMessage("'diag_histlist'", TestProtocol.INFO, oDBM.getAnswer());
        } else {
          addMessage("'diag_histlist'", TestProtocol.ERROR, oDBM.getAnswer());
        }

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
    return "DiagHistlist";
  } // end getTestClassId
} //end of class MediumGet