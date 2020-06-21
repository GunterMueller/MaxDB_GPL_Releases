package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: MediumGetAll.java
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
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        MediumGetAll
 * -----------------------------------------------------------------------------
 * responsible:  Fedorov Alexander
 * special area: DBMServer Java Test
 * description:  DBMServer medium_getall command tests
 * version:      7.2.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class MediumGetAll extends MediumPut
{
  public MediumGetAll()  throws SQLException, TestDatabaseException
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

    //  create one global try-catch block to handle exceptions
    try
    {
      //create media and check if created
      execCmd(oDBM,msgTab,NAME,LOCATION,FILE,DATA,"4","4",YES,YES,OS_CMD);

      //Too few parameters
      if (oDBM.execCmd("medium_getall",DBMUtility.OK_OK))
      {
          //parse reply
          msgTab.setMessage(oDBM.getAnswer());

         //find new medium
         //create reply string
         String rep = NAME + "\t" + LOCATION + "\t" + FILE + "\t" + DATA
                           + "\t" + "4" + "\t" + "4" + "\t" + YES + "\t" + YES;

         boolean res = false;
         //compare medium_getall command with compare string
         for (int r = 0 ; r < msgTab.getRowCount(); r++)
         {
             if (msgTab.getRow(r).startsWith(rep))
             {
                 res = true;
             }
         }
         if (!res)
         {
            addMessage("command medium_getall return wrong result.", TestProtocol.ERROR , msgTab.getMessage());
         }
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
    return "MediumGetAll";
  } // end getTestClassId

} //end of class MediumGetAll