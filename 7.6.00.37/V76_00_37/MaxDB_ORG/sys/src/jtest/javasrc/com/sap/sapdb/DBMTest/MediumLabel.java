package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: MediumLabel.java
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

import com.sap.sapdb.DBMTest.util.*;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        MediumLabel
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer medium_label command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class MediumLabel extends MediumPut
{
  public MediumLabel() throws SQLException, TestDatabaseException
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

    //  create one global try-catch block to handle exceptions
    try
    {
      //util connect
      oDBM.execCmd("util_connect",DBMUtility.OK_OK);

      //create 1 backup media
      oDBM.execCmd("medium_put datamedium "+outPath+"1 file data 0 8 yes",DBMUtility.OK_OK);
      oDBM.execCmd("backup_start datamedium recovery data autoignore",DBMUtility.OK_OK);

      //getting label of backup medium
      oDBM.execCmd("medium_label datamedium",DBMUtility.OK_OK);
      String label = oDBM.getAnswer();

      //Creating vector for label
      Vector LabelArray = new Vector();

      StringTokenizer st_label = new StringTokenizer(label,"\n");
      while (st_label.hasMoreTokens())
      {
        StringTokenizer st_st = new StringTokenizer(st_label.nextToken(),"\t");
        while (st_st.hasMoreTokens())
        {
          LabelArray.add(st_st.nextToken());
        }
      }

      //checking whether result is correct
      if (LabelArray.get(1).toString().equals("0") &&                                                                   //Returncode
          LabelArray.get(11).toString().length() == 40 &&                                                               //Length of Kernel Version
          LabelArray.get(22).toString().indexOf("DAT_") == 0 &&                                                         //Label
          LabelArray.get(24).toString().equalsIgnoreCase("true") &&                                                     //Is Consistent
          LabelArray.get(39).toString().indexOf(LabelArray.get(7).toString()+":"+LabelArray.get(9).toString()) == 0)    //Database ID
      {
          addMessage(this.toString() + ".run",
                 TestProtocol.INFO,
                 "Label string was checked and is ok!");
      }
      else
      {
          addMessage(this.toString() + ".run",
                 TestProtocol.ERROR,
                 "Label string was checked and is not ok!");
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
    return "MediumLabel";
  } // end getTestClassId

} //end of class MediumGet