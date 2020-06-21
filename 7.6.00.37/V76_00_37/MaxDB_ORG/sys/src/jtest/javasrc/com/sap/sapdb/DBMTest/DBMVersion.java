package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DBMVersion.java
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
import java.util.Vector;

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBMVersion
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer dbm_version command tests
 * version:      7.4.*
 * Copyright (c) 2002-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DBMVersion extends BackupState
{
  public Vector vLabel;

  public DBMVersion() throws SQLException, TestDatabaseException
  {
    super();

    vLabel = new Vector();

    vLabel.add("VERSION");
    vLabel.add("BUILD");
    vLabel.add("OS");
    vLabel.add("INSTROOT");
    vLabel.add("LOGON");
    vLabel.add("CODE");
    vLabel.add("SWAP");
    vLabel.add("UNICODE");
    vLabel.add("INSTANCE");
    vLabel.add("SYSNAME");
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM         = new  DBMUtility(getDatabase(), this);
    TestDatabaseSap sapdb   = (TestDatabaseSap) getDatabase();
    boolean m_bLabel        = false;

    //  create one global try-catch block to handle exceptions
    try
    {
      oDBM.execCmd("dbm_version", DBMUtility.OK_OK);
      String sLabel = oDBM.getAnswer();

      StringTokenizer stParam = new StringTokenizer(sLabel,"\n");

      while (stParam.hasMoreTokens()) {

        for (int i = 0;i < vLabel.size(); i++) {

          if (stParam.nextToken().toString().indexOf(vLabel.get(i).toString()) >= 0) {

            m_bLabel = true;
          }
          if (i == vLabel.size() - 1 && m_bLabel == false) {

            break;
          }
        }
      }

      if (m_bLabel == true) {

        addMessage("'dbm_version'", TestProtocol.INFO, sLabel);
      } else {
        addMessage("'dbm_version'", TestProtocol.ERROR, sLabel);
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
    return "DBMSetpath";
  } // end getTestClassId
} //end of class MediumGet