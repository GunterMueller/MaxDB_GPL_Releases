package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: BackupStart.java
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
 * Class:        BackupStart
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer medium_get command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class BackupStart extends BackupState
{
  public BackupStart()  throws SQLException, TestDatabaseException
  {
          super();
  }
  /**
   * Checks reply with return code
   */
   private void checkRightReply(String msg, String retCode)
   {
           //check reply
       MessageTable msgTab = new MessageTable();
       msgTab.setMessage(msg);
       checkValue(msgTab,REP_RETURNCODE,retCode);
       checkValue(msgTab,REP_DATABASE,getDatabase().getName());
       checkValue(msgTab,REP_DEVICESUSED,"1");
//       checkValue(msgTab,REP_LOCATION,getDataOutputPath()+"1");
       checkValue(msgTab,REP_LOCATION,getTestClassId()+"1");
       checkValue(msgTab,REP_MEDIANAME,"datamedium");

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
           oDBM.execCmd("medium_put datamedium "+outPath+"1 file data 0 8 yes",DBMUtility.OK_OK);

           //util connect
           oDBM.execCmd("util_connect",DBMUtility.OK_OK);

           //Without medium name
           oDBM.execCmd("backup_start",DBMUtility.ERR_TOFEWPARAM);

           //With wrong medium name
           oDBM.execCmd("backup_start wrongname",DBMUtility.ERR_MENOTFOUND);

           //With wrong medium "backup_for"
           oDBM.execCmd("backup_start datamedium REC",DBMUtility.ERR_PARAM);

           //With "backup_for" parameter omitted
           oDBM.execCmd("backup_start datamedium data",DBMUtility.OK_OK);
           checkRightReply(oDBM.getAnswer(),"0");

           //With wrong type of backup
           oDBM.execCmd("backup_start datamedium dat",DBMUtility.ERR_PARAM);

           //With type of backup omitted
           oDBM.execCmd("medium_put datamedium /wrongpath file",DBMUtility.OK_OK);
           oDBM.execCmd("backup_start datamedium recovery autoignore",DBMUtility.ERR_BANOSAVETYPE);

           //With wrong “autoignore” parameter of backup
           oDBM.execCmd("backup_start datamedium recovery data autoignor",DBMUtility.ERR_PARAM);

           //With wrong medium location
           oDBM.execCmd("medium_put datamedium /wrongpath/1 file data 0 8 yes",DBMUtility.OK_OK);
           oDBM.execCmd("backup_start datamedium",DBMUtility.ERR_SQL);

           //Backup for recovery
           oDBM.execCmd("medium_put datamedium "+outPath+"1 file data 0 8 yes",DBMUtility.OK_OK);
           oDBM.execCmd("backup_start datamedium recovery data autoignore",DBMUtility.OK_OK);
           checkRightReply(oDBM.getAnswer(),"0");


           //Backup for migration
           oDBM.execCmd("medium_put datamedium "+outPath+"1 file data 0 8 yes",DBMUtility.OK_OK);
           oDBM.execCmd("backup_start datamedium migration data autoignore",DBMUtility.OK_OK);
           checkRightReply(oDBM.getAnswer(),"0");

           //Incremental backup
           oDBM.execCmd("medium_put datamedium "+outPath+"1 file pages 0 8 yes",DBMUtility.OK_OK);
           oDBM.execCmd("backup_start datamedium pages",DBMUtility.OK_OK);
           checkRightReply(oDBM.getAnswer(),"0");

           //Log backup
           oDBM.execCmd("medium_put datamedium "+outPath+"1 file log 0 8 yes",DBMUtility.OK_OK);
           oDBM.execCmd("autolog_off",DBMUtility.OK_OK);
           oDBM.execCmd("backup_start datamedium log",DBMUtility.OK_OK);
           checkRightReply(oDBM.getAnswer(),"0");

           //Medium can not be overwritten
           oDBM.execCmd("medium_put datamedium "+outPath+"1 file data 0 8 no",DBMUtility.OK_OK);
           oDBM.execCmd("backup_start datamedium data",DBMUtility.ERR_BAOVERWRITE);

           //Small size medium
           oDBM.execCmd("medium_put datamedium "+outPath+"1 file data 32 8 yes",DBMUtility.OK_OK);
           oDBM.execCmd("backup_start datamedium data",DBMUtility.OK_OK);
           checkRightReply(oDBM.getAnswer(),"-8020");

           oDBM.execCmd("backup_cancel",DBMUtility.OK_OK);

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
    return "BackupStart";
  } // end getTestClassId

} //end of class BackupStart