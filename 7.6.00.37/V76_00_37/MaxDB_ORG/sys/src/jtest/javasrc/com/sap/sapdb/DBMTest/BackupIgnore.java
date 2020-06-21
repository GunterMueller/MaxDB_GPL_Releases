package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: BackupIgnore.java
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

import com.sap.sapdb.DBMTest.util.*;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        BackupIgnore
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer medium_get command tests
 * version:      7.2.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class BackupIgnore extends BackupState
{
  public BackupIgnore() throws SQLException, TestDatabaseException
  {
          super();
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);
    //create message table for checking DBM replies
    MessageTable msgTab = new MessageTable();
//    String outPath = getDataOutputPath();
    String outPath = getTestClassId();

    //  create one global try-catch block to handle exceptions
    try
    {
           //fill the db with values
           fillDatabase(oDBM);

           //Without starting backup

           oDBM.execCmd("util_connect",DBMUtility.OK_OK);
           oDBM.execCmd("backup_ignore",DBMUtility.ERR_NOBACKUPSESSION);

           //With starting backup to a small parallel medium

           //Start backup
           oDBM.execCmd("medium_put para1//datamedium1 "+outPath+"1 file data 32 8 yes",DBMUtility.OK_OK);
           oDBM.execCmd("medium_put para1//datamedium2 "+outPath+"2 file data 32 8 yes",DBMUtility.OK_OK);
           //backup start

           oDBM.execCmd("backup_start para1",DBMUtility.OK_OK);

           //backup_cancel
           oDBM.execCmd("backup_ignore",DBMUtility.OK_OK);
           //check reply
           msgTab.setMessage(oDBM.getAnswer());
           checkValue(msgTab,REP_RETURNCODE,"-8020");

           //With starting backup to a small medium

           //Start backup
           oDBM.execCmd("medium_put datamedium "+outPath+"1 file data 32 8 yes",DBMUtility.OK_OK);
           //backup start
           oDBM.execCmd("backup_start datamedium",DBMUtility.OK_OK);
           //backup_cancel
           oDBM.execCmd("backup_ignore",DBMUtility.ERR_SQL);

           oDBM.execCmd("util_release",DBMUtility.OK_OK);

    }
    // catch any kind of exception and write it to the protocol
    catch (Exception e)
    {
      // standard error handler
      this.handleExceptions(e);
    } // end try catch

    // clean up, i.e. close statements and connections
    finally
    {
      try
      {
        // print all parameters from the parameter object to the protocol.
        printParametersToProtocol();
      }
      catch (Exception e)
      {
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
  public static String getTestClassId()
  {
    return "BackupIgnore";
  } // end getTestClassId

} //end of class BackupIgnore