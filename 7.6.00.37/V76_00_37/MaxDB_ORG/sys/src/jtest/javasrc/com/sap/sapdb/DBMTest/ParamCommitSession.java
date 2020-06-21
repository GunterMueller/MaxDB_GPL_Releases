package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: ParamCommitSession.java
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

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        ParamCommitSession
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer param_commitsession command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class ParamCommitSession extends BackupState
{
  public ParamCommitSession() throws SQLException, TestDatabaseException
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
      //Commit session without param_startsession or param_checkall
      oDBM.execCmd("param_commitsession",DBMUtility.ERR_NOXPSESSION);
      oDBM.execCmd("param_commitsession NOCLOSE",DBMUtility.ERR_NOXPSESSION);

      //Commit session with param_startsession but without param_checkall
      oDBM.execCmd("param_startsession",DBMUtility.OK_OK);
      oDBM.execCmd("param_commitsession NOCLOSE",DBMUtility.ERR_XPCHECK);
      oDBM.execCmd("param_commitsession",DBMUtility.ERR_XPCHECK);

      //Commit session with param_startsession and param_checkall
      oDBM.execCmd("param_startsession",DBMUtility.OK_OK);
      oDBM.execCmd("param_checkall",DBMUtility.OK_OK);
      oDBM.execCmd("param_commitsession NOCLOSE",DBMUtility.OK_OK);
      oDBM.execCmd("param_commitsession",DBMUtility.OK_OK);
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
    return "ParamCommitSession";
  } // end getTestClassId
} //end of class MediumGet