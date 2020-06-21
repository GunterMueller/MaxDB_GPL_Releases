package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: Show.java
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
import java.util.Vector;

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        Show
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer show command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class Show extends BackupState
{
  public Vector OptionArray;

  public Show() throws SQLException, TestDatabaseException
  {
    super();
    OptionArray = new Vector();
    //Options for dbmcli command show
    OptionArray.add("IO");
    OptionArray.add("AIO");
    OptionArray.add("STORAGE");
    OptionArray.add("TASKS");
    OptionArray.add("ACTIVE");
    OptionArray.add("RUNNABLE");
    OptionArray.add("T_C");
    OptionArray.add("VERSIONS");
    OptionArray.add("REGIONS");
    OptionArray.add("STATE");
    OptionArray.add("RTE");
    OptionArray.add("QUEUES");
    OptionArray.add("SUSPENDS");
    OptionArray.add("SLEEP");
    OptionArray.add("THRD_TIMES");
    OptionArray.add("ALL");
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);
    //  create one global try-catch block to handle exceptions
    try
    {
      //Too few parameters - show
      oDBM.execCmd("show",DBMUtility.ERR_TOFEWPARAM);
      //show, option
      for (int i = 0;i < OptionArray.size();i++) {

        oDBM.execCmd("show "+OptionArray.get(i),DBMUtility.OK_OK,false);

        if (oDBM.getAnswer().length() >= 0) {

          addMessage("show - \"" + OptionArray.get(i) + "\"", TestProtocol.INFO, oDBM.getAnswer());
        } else {
          addMessage("show - \"" + OptionArray.get(i) + "\"", TestProtocol.ERROR, oDBM.getAnswer());
        }
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
    return "Show";
  } // end getTestClassId
} //end of class MediumGet