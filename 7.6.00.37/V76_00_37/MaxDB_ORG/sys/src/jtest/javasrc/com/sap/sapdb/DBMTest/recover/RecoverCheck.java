package com.sap.sapdb.DBMTest.recover;
/*!
  -----------------------------------------------------------------------------
  module: RecoverCheck.java
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

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.*;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        RecoverCheck
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer recover_check command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class RecoverCheck extends DBMTestCase
{
  public RecoverCheck() throws SQLException, TestDatabaseException
  {
          super();
  }
  /**
   * run() contains the test implementation
   */
  public void runDBMTest()
  {
    //create message table for checking DBM replies
    MessageTable msgTab = new MessageTable();
    String rep = null;
    String outPath = getTestClassId();

    //  create one global try-catch block to handle exceptions
    try
    {
      //util connect
	  m_DBM.execCmd("util_connect", DBMUtility.OK_OK);

      //create 1 backup media
	  m_DBM.execCmd("medium_put datamedium "+outPath+"1 file data 0 8 yes", DBMUtility.OK_OK);
	  m_DBM.execCmd("backup_start datamedium recovery data autoignore", DBMUtility.OK_OK);

      //getting label of backup medium
      m_DBM.execCmd("service_connect", DBMUtility.OK_OK);
      m_DBM.execCmd("recover_check datamedium data", DBMUtility.OK_OK);
      String Label = m_DBM.getAnswer();

      //Creating vector for label
      Vector LabelArray = new Vector();

      StringTokenizer st_Label = new StringTokenizer(Label,"\n");
      while (st_Label.hasMoreTokens())
      {
        StringTokenizer st_st = new StringTokenizer(st_Label.nextToken(),"\t");
        while (st_st.hasMoreTokens())
        {
          LabelArray.add(st_st.nextToken());
        }
      }

      boolean labelCheckOK=true;
      
      //checking whether result is correct
      if(!LabelArray.get(1).toString().equals("0"))                                                                   //Returncode
      {
		labelCheckOK=false;
      	
		super.addMessage(
		             	super.getTestId(),
						TestProtocol.ERROR,
		                LabelArray.get(1).toString()+" does not match expected return code 0. Test failed.");
      }
	  	
      if(LabelArray.get(11).toString().length() != 40)                                                               //Length of Kernel Version
      {
		labelCheckOK=false;
      	
		super.addMessage(
						super.getTestId(),
						TestProtocol.ERROR,
						Integer.toString(LabelArray.get(11).toString().length())+" does not match expected kernel version length of 40. Test failed.");
      }
      
      if(!LabelArray.get(19).toString().equalsIgnoreCase("datamedium"))                                               //Medianame
      {
		labelCheckOK=false;
      	
		super.addMessage(
						super.getTestId(),
						TestProtocol.ERROR,
						LabelArray.get(19).toString()+" does not match the expected medium name datamedium. Test failed.");

      }
      
      if(LabelArray.get(24).toString().indexOf("DAT_") != 0)                                                         //Label
      {
		labelCheckOK=false;
      	
		super.addMessage(
						super.getTestId(),
						TestProtocol.ERROR,
		                "Label "+LabelArray.get(24).toString()+" does not start with the expected DAT_. Test failed.");
      	
      }
      
      if(!LabelArray.get(26).toString().equalsIgnoreCase("true"))                                                     //Is Consistent
      {
		labelCheckOK=false;
  	
		super.addMessage(
						super.getTestId(),
						TestProtocol.ERROR,
						LabelArray.get(26).toString()+" does not match the expected consistency value true. Test failed.");

      }
          
      if(LabelArray.get(41).toString().indexOf(LabelArray.get(7).toString()+":"+LabelArray.get(9).toString()) != 0)    //Database ID
      {
		labelCheckOK=false;
  	
		super.addMessage(
						super.getTestId(),
						TestProtocol.ERROR,
						LabelArray.get(41).toString()+" does not match the expected db identifier "+LabelArray.get(7).toString()+":"+LabelArray.get(9).toString()+". Test failed.");
      }
          
      if(labelCheckOK)
      {
          addMessage(this.toString() + ".run",
                 TestProtocol.INFO,
                 "Recover check is ok!");
      }
      else
      {
          addMessage(this.toString() + ".run",
                 TestProtocol.ERROR,
                 "Recover check is not ok (see above)!");
                 
		  super.addMessage(
						 super.getTestId(),
						 TestProtocol.ERROR,
          				 "Got the following reply for recover_check:\n"+Label);
      }
      //service release
      m_DBM.execCmd("service_release", DBMUtility.OK_OK);
      //util release
      m_DBM.execCmd("util_release", DBMUtility.OK_OK);
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
    return "RecoverCheck";
  } // end getTestClassId

} //end of class MediumGet