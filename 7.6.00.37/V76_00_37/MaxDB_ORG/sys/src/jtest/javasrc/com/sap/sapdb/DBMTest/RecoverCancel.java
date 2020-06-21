package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: RecoverCancel.java
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
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        RecoverCancel
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer recover_cancel command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class RecoverCancel extends BackupState
{
  private ThreadGroup myThreads;

  public RecoverCancel()  throws SQLException, TestDatabaseException
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
    String mHost = getDatabase().getHost() + ":" + getDatabase().getPort();
    String mName = getDatabase().getName();
    try
    {
      dbm = DBM.dbDBM(mHost, mName);
      instroot = dbm.cmd("dbm_version INSTROOT").trim();
    } catch (com.sap.dbtech.rte.comm.RTEException e) {
      // standard error handler
      addMessage(this.toString() + ".run",
                 TestProtocol.ERROR,
                 e.toString());
    } catch (com.sap.dbtech.powertoys.DBMException e) {
      // standard error handler
      addMessage(this.toString() + ".run",
                 TestProtocol.ERROR,
                 e.toString());
    }


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

        oDBM.execCmd("backup_replace datamedium "+outPath+i,DBMUtility.OK_OK);
        msgTab.setMessage(oDBM.getAnswer());

        RC_NewMedium = msgTab.getCell(0,1);
        i++;
      }
      //Util release
      oDBM.execCmd("util_release",DBMUtility.OK_OK);
      //Util connect
      oDBM.execCmd("util_connect",DBMUtility.OK_OK);
      //Without started backup
      oDBM.execCmd("recover_cancel",DBMUtility.ERR_NOBACKUPSESSION);

      //With started backup
      //Setting Database to Admin-Mode
      oDBM.execCmd("db_cold" ,DBMUtility.OK_OK);
      //Util connect
      oDBM.execCmd("util_connect",DBMUtility.OK_OK);
      //start a recovery
      oDBM.execCmd("recover_start datamedium data",DBMUtility.OK_OK);
      //recover_cancel
      oDBM.execCmd("recover_cancel",DBMUtility.OK_OK);
      //check reply
      msgTab.setMessage(oDBM.getAnswer());
      checkValue(msgTab,REP_RETURNCODE,"0");

      //Util release
      oDBM.execCmd("util_release",DBMUtility.OK_OK);

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
    return "RecoverCancel";
  } // end getTestClassId
} //end of class BackupCancel