package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: Info.java
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
 * Class:        Info
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer info command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class Info extends BackupState
{
  public Vector sOption;

  public Info() throws SQLException, TestDatabaseException
  {
    super();
    sOption = new Vector();
    //Options for dbmcli command info
    sOption.add("BADDEVSPACES");
    sOption.add("BADIDX");
    sOption.add("CACHES");
    //sOption.add("CLSCON");      For LiveCaches only
    sOption.add("COLUMNS");
    sOption.add("DATA");
    sOption.add("DEVSPACES");
    sOption.add("DISABLEDIDX");
    sOption.add("INDEXES");
    sOption.add("INFOS");
    sOption.add("IO");
    sOption.add("LOCKS");
    sOption.add("LOG");
    //sOption.add("OMSCACHES");   For LiveCaches only
    //sOption.add("OMSMON");      For LiveCaches only
    sOption.add("PARAMS");
    sOption.add("STATE");
    sOption.add("SYSDDCONFIG");
    sOption.add("SYSDDMONITOR");
    sOption.add("SYSDDSRVDB");
    sOption.add("TABLES");
    sOption.add("UNUSEDIDX");
    sOption.add("UPDSTAT");
    sOption.add("UPDSTATW");
    sOption.add("USERS");
    sOption.add("VERSIONS");
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
      oDBM.execCmd("info",DBMUtility.ERR_TOFEWPARAM);
      //show, option
      for (int i = 0;i < sOption.size();i++) {

        //System.out.println("info "+sOption.get(i));
        oDBM.execCmd("info "+sOption.get(i), DBMUtility.OK_OK, false);

        if (oDBM.getAnswer().length() >= 0) {

          addMessage("info - \"" + sOption.get(i) + "\"", TestProtocol.INFO, oDBM.getAnswer());
        } else {
          addMessage("info - \"" + sOption.get(i) + "\"", TestProtocol.ERROR, oDBM.getAnswer());
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
    return "Info";
  } // end getTestClassId
} //end of class MediumGet