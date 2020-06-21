package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DBRestartInfo.java
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
import java.util.StringTokenizer;
import java.util.Vector;

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBRestartInfo
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer db_restartinfo command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DBRestartInfo extends MediumPut
{
  public DBRestartInfo()  throws SQLException, TestDatabaseException
  {
          super();
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);

    //  create one global try-catch block to handle exceptions
    try
    {
      //setting DB to modus admin
      oDBM.execCmd("db_admin",DBMUtility.OK_OK);

      //sending command db_restartinfo
      oDBM.execCmd("db_restartinfo",DBMUtility.OK_OK);
      String Info = oDBM.getAnswer();

      //Creating vector for label
      Vector InfoArray = new Vector();

      StringTokenizer st_label = new StringTokenizer(Info,"\n");
      while (st_label.hasMoreTokens())
      {
        StringTokenizer st_st = new StringTokenizer(st_label.nextToken(),"\t");
        while (st_st.hasMoreTokens())
        {
          InfoArray.add(st_st.nextToken());
        }
      }

      //checking whether result is correct
      if (InfoArray.get(5).toString().equals("1") &&                              //Restartable
          InfoArray.get(7).toString().indexOf(getDatabase().getName()) > 0 &&     //Id Restart Record
          InfoArray.get(9).toString().indexOf(getDatabase().getName()) > 0 &&     //Id LOG Info
          InfoArray.get(11).toString().equals("1"))                               //Consistent
      {
          addMessage(this.toString() + ".run",
                 TestProtocol.INFO,
                 "Info string was checked and is ok!");
      }
      else
      {
          addMessage(this.toString() + ".run",
                 TestProtocol.ERROR,
                 "Info string was checked and is not ok!"+"    - "+
                 InfoArray.get(7).toString().indexOf(getDatabase().getName())+" - "+
                 InfoArray.get(9).toString().indexOf(getDatabase().getName())+" - "+
                 InfoArray.get(7).toString()+" - "+
                 InfoArray.get(8).toString()+" - "+
                 InfoArray.get(9).toString()+" - "+
                 InfoArray.get(10).toString());
      }

      //setting DB to modus online
      oDBM.execCmd("db_online -f",DBMUtility.OK_OK);
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
    return "DBRestartInfo";
  } // end getTestClassId

} //end of class MediumGet