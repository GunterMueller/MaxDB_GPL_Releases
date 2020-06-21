package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: ParamGetHistory.java
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
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        ParamGetHistory
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer param_gethistory command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class ParamGetHistory extends BackupState
{
  public ParamGetHistory() throws SQLException, TestDatabaseException
  {
    super();
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);
    TestDatabaseSap sapdb = (TestDatabaseSap) getDatabase();

    //  create one global try-catch block to handle exceptions
    try
    {
      //Changing some Parameters for history
      for (int i=3;i<6;i++) {
        //opening param session
        oDBM.execCmd("param_startsession",DBMUtility.OK_OK);
        //param_put with param_commitsession
        oDBM.execCmd("param_put MAXDATAVOLUMES "+i,DBMUtility.OK_OK);
        oDBM.execCmd("param_checkall",DBMUtility.OK_OK);
        oDBM.execCmd("param_commitsession",DBMUtility.OK_OK);
        //Restarting Database
        oDBM.execCmd("db_offline -" + sapdb.getKernelMode(),DBMUtility.OK_OK);
        oDBM.execCmd("db_online -" + sapdb.getKernelMode(),DBMUtility.OK_OK);
      }

      //Open parameter history with - param_gethistory
      oDBM.execCmd("param_gethistory",DBMUtility.OK_OK);
      String param = oDBM.getAnswer();

      //System.out.println(param);

      if (param.length() > 100) {
        addMessage(this.toString() + ".run",
                   TestProtocol.INFO,
                   "param_gethistorynext worked fine");
      } else {
        addMessage(this.toString() + ".run",
                   TestProtocol.ERROR,
                   "param_gethistorynext did not work fine");
      }
      //Setting back to default value
      //opening param session
      oDBM.execCmd("param_startsession",DBMUtility.OK_OK);
      //param_put with param_commitsession
      oDBM.execCmd("param_put MAXDATAVOLUMES 2",DBMUtility.OK_OK);
      oDBM.execCmd("param_checkall",DBMUtility.OK_OK);
      oDBM.execCmd("param_commitsession",DBMUtility.OK_OK);

      //Restarting Database
      oDBM.execCmd("db_offline -" + sapdb.getKernelMode(),DBMUtility.OK_OK);
      oDBM.execCmd("db_online -" + sapdb.getKernelMode(),DBMUtility.OK_OK);
      /**/
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
    return "ParamGetHistory";
  } // end getTestClassId
} //end of class MediumGet