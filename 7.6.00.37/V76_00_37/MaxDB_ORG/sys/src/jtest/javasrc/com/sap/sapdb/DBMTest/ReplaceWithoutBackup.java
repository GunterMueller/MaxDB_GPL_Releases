package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: ReplaceWithoutBackup.java
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

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        ReplaceWithoutBackup
 * -----------------------------------------------------------------------------
 * responsible:  Fedorov Alexander
 * special area: DBMServer Java Test
 * description:  DBMServer medium_get command tests
 * version:      7.2.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class ReplaceWithoutBackup extends BackupState {

  public ReplaceWithoutBackup()  throws SQLException, TestDatabaseException {
          super();
  }
  /**
   * run() contains the test implementation
   */
  public void run() {

    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);
//    String outPath = getDataOutputPath();
    String outPath = getTestClassId();

    //  create one global try-catch block to handle exceptions
    try {
           //create mediums
           oDBM.execCmd("medium_put datamedium "+outPath+"1 file data 0 8 yes",DBMUtility.OK_OK);
           oDBM.execCmd("backup_replace datamedium",DBMUtility.ERR_NOBACKUPSESSION);

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
    return "ReplaceWithoutBackup";
  } // end getTestClassId

} //end of class ReplaceWithoutBackup