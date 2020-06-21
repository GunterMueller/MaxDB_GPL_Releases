package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: RecoverReplace.java
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
import java.io.File;
import java.sql.SQLException;

import com.sap.dbtech.powertoys.DBM;
import com.sap.sapdb.DBMTest.util.*;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        RecoverReplace
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer recover_replace command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class RecoverReplace extends BackupState
{
  private ThreadGroup myThreads;

  public RecoverReplace()  throws SQLException, TestDatabaseException
  {
    super();
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);
    DBM dbm = null;
    TestDatabaseSap sapdb = (TestDatabaseSap) getDatabase();
    //create message table for checking DBM replies
    MessageTable msgTab = new MessageTable();
    String instroot = null;
    String outPath = getTestClassId();

    //  create one global try-catch block to handle exceptions
    try
    {
      //fill the db with values
      fillDatabase(oDBM);
      //With starting backup
      oDBM.execCmd("util_connect",DBMUtility.OK_OK);
      //Start backup
      oDBM.execCmd("medium_put datamedium "+outPath+"0 file data 32 8 yes",DBMUtility.OK_OK);
      //backup start
      oDBM.execCmd("backup_start datamedium recovery data",DBMUtility.OK_OK);

      msgTab.setMessage(oDBM.getAnswer());
      String RC_NewMedium = msgTab.getCell(0,1);

      //System.out.println(RC_NewMedium);

      int i = 1;  //Count for Backup-Media

      while(RC_NewMedium.equals("-8020")) {

        oDBM.execCmd("backup_replace datamedium "+outPath+i,DBMUtility.OK_OK,false);
        msgTab.setMessage(oDBM.getAnswer());

        RC_NewMedium = msgTab.getCell(0,1);
        i++;
      }
      //util release
      oDBM.execCmd("util_release",DBMUtility.OK_OK);

      //With started recovery
      //Setting Database to Admin-Mode
      oDBM.execCmd("db_cold -" + sapdb.getKernelMode(),DBMUtility.OK_OK);
      //util connect
      oDBM.execCmd("util_connect",DBMUtility.OK_OK);
      //start a recovery
      oDBM.execCmd("recover_start datamedium data",DBMUtility.OK_OK);

      msgTab.setMessage(oDBM.getAnswer());
      RC_NewMedium = msgTab.getCell(0,1);

      //System.out.println(oDBM.getAnswer().toString());

      i = 1;  //Count for Backup-Media

      while(RC_NewMedium.equals("-8020")) {

        oDBM.execCmd("recover_replace datamedium "+outPath+i,DBMUtility.OK_OK,false);
        msgTab.setMessage(oDBM.getAnswer());

        RC_NewMedium = msgTab.getCell(0,1);

        //System.out.println(RC_NewMedium);
        i++;
      }
      //util release
      oDBM.execCmd("util_release",DBMUtility.OK_OK);
      //Setting Database to Online-Mode
      oDBM.execCmd("db_online -" + sapdb.getKernelMode(),DBMUtility.OK_OK);

      //Run Directory clean up
      for(int k=0;k<i;k++)
      {
        //System.out.println(sapdb.getRunDir()+"//"+outPath+k);
        File f = new File(sapdb.getRunDir()+"//"+outPath+k);
        f.delete();
      }
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
  public static String getTestClassId() {
    return "RecoverReplace";
  } // end getTestClassId
} //end of class BackupCancel