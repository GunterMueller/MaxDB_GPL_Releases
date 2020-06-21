package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: AutologCancel.java
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
 * Class:        AutologCancel
 * -----------------------------------------------------------------------------
 * responsible:  Fedorov Alexander
 * special area: DBMServer Java Test
 * description:  DBMServer medium_get command tests
 * version:      7.2.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class AutologCancel extends MediumPut
{
  public AutologCancel()  throws SQLException, TestDatabaseException
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

    //  create one global try-catch block to handle exceptions
    try
    {
           //set AUTOSAVE IS ON
           oDBM.execCmd("autolog_off",DBMUtility.OK_OK);
           oDBM.execCmd("medium_put logmedium tst1 file log",DBMUtility.OK_OK);
           oDBM.execCmd("autolog_on logmedium",DBMUtility.OK_OK);
           execCmd(oDBM,msgTab,"autolog_show","AUTOSAVE IS ON");

           //test autolog_cancel command
           oDBM.execCmd("autolog_cancel",DBMUtility.OK_OK);
           execCmd(oDBM,msgTab,"autolog_show","AUTOSAVE IS OFF");

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
    return "AutologCancel";
  } // end getTestClassId

} //end of class AutologCancel