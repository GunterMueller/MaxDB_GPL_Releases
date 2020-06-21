package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: BackupHistoryClose.java
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

import com.sap.sapdb.DBMTest.util.*;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        BackupHistoryClose
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer backup_history_close command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class BackupHistoryClose extends MediumPut
{
  public BackupHistoryClose()  throws SQLException, TestDatabaseException
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
    String rep = null;
    String outPath = getTestClassId();
    TestDatabaseSap sapdb = (TestDatabaseSap) getDatabase();

    //  create one global try-catch block to handle exceptions
    try
    {
      //util connect
      oDBM.execCmd("util_connect",DBMUtility.OK_OK);
      //create 5 backup media
      for(int i=1;i<5;i++)
      {
        oDBM.execCmd("medium_put datamedium "+outPath+i+" file data 0 8 yes",DBMUtility.OK_OK,false);
        oDBM.execCmd("backup_start datamedium recovery data autoignore",DBMUtility.OK_OK,false);
      }
      //util release
      oDBM.execCmd("util_release",DBMUtility.OK_OK);
      //backup_history_listnext - with opening history session
      //opening history backup list
      oDBM.execCmd("backup_history_open",DBMUtility.OK_OK);
      oDBM.execCmd("backup_history_listnext",DBMUtility.OK_OK);
      //opening history backup list
      oDBM.execCmd("backup_history_close",DBMUtility.OK_OK);
      //backup_history_listnext - without opening history session
      oDBM.execCmd("backup_history_listnext",DBMUtility.ERR_NOHISTORYSESSION);

      //Run Directory clean up
      for(int i=1;i<5;i++)
      {
        //System.out.println(sapdb.getRunDir()+"//"+outPath+i);
        File f = new File(sapdb.getRunDir()+"//"+outPath+i);
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
  public static String getTestClassId()
  {
    return "BackupHistoryClose";
  } // end getTestClassId

} //end of class MediumGet