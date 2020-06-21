package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: BackupReplace.java
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
 * Class:        BackupReplace
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer medium_get command tests
 * version:      7.2.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class BackupReplace extends BackupState
{
  public BackupReplace()  throws SQLException, TestDatabaseException
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
           //create two mediums
           oDBM.execCmd("medium_put datamedium "+outPath+"1 file data 32 8 yes",DBMUtility.OK_OK);
           oDBM.execCmd("medium_put datamediumnew "+outPath+"1 file data 32 8 yes",DBMUtility.OK_OK);

           //util connect
           oDBM.execCmd("util_connect",DBMUtility.OK_OK);
           //start backup
           oDBM.execCmd("backup_start datamedium",DBMUtility.OK_OK);

           //Without medium name
           oDBM.execCmd("backup_replace",DBMUtility.ERR_TOFEWPARAM);

           //With wrong medium name
           oDBM.execCmd("backup_replace wrongname",DBMUtility.ERR_MENOTFOUND);

           //With right name and small medium
           oDBM.execCmd("backup_replace datamedium",DBMUtility.OK_OK);
           msgTab.setMessage(oDBM.getAnswer());
           checkValue(msgTab,REP_RETURNCODE,"-8020");

           //With medium with wrong location
           oDBM.execCmd("medium_put datamediumnew /wrongpath/1"+outPath+"1 file data 0 8 yes",DBMUtility.OK_OK);
           oDBM.execCmd("backup_replace datamediumnew",DBMUtility.ERR_SQL);

           //Medium cannot be overwritten
           oDBM.execCmd("medium_put datamediumnew "+outPath+"1 file data 0 8 no",DBMUtility.OK_OK);
           oDBM.execCmd("backup_replace datamediumnew",DBMUtility.ERR_BAOVERWRITE);

           //Positive test
           oDBM.execCmd("medium_put datamediumnew "+outPath+"1 file data 0 8 yes",DBMUtility.OK_OK);
           oDBM.execCmd("backup_replace datamediumnew",DBMUtility.OK_OK);
           //check reply
           msgTab.setMessage(oDBM.getAnswer());
           checkValue(msgTab,REP_RETURNCODE,"0");
           checkValue(msgTab,REP_DATABASE,getDatabase().getName());
           checkValue(msgTab,REP_DEVICESUSED,"1");
           checkValue(msgTab,REP_LOCATION,outPath+"1");
           checkValue(msgTab,REP_MEDIANAME,"datamedium");


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
    return "BackupReplace";
  } // end getTestClassId

} //end of class BackupReplace