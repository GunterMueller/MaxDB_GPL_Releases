package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: BackupHistoryListnext.java
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
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        BackupHistoryListnext
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer backup_history_listnext command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class BackupHistoryListnext extends MediumPut
{
  public BackupHistoryListnext()  throws SQLException, TestDatabaseException
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
      //create 59 backups
	  oDBM.execCmd("medium_put datamedium "+outPath+" file data 0 8 yes",DBMUtility.OK_OK,false);
      for(int i=1;i<60;i++)
      {  
        oDBM.execCmd("backup_start datamedium recovery data autoignore",DBMUtility.OK_OK,false);
      }
      //get backup history
      oDBM.execCmd("backup_history_open",DBMUtility.OK_OK);
      oDBM.execCmd("backup_history_listnext",DBMUtility.OK_OK);
      String list_1 = oDBM.getAnswer();
      
      int numberOfHistoryListNext=1;
      
      String testResult="";
      boolean foundError=false;
      boolean foundEnd=false;
      //checking whether result is correct
      if (list_1.indexOf("CONTINUE") == 0 )
      {
          testResult+="First backup_history_listnext produced the expected \"CONTINUE\".";
      }
      else
      {
          testResult+="First backup_history_listnext did not produce the expected \"CONTINUE\".";
          foundError=true; 
      }
      
      testResult+="\nAnswer was:\n--->\n"+list_1+"<---\n";
      
      final int maxLoopCount=100; //to avoid endless loop tests

      while( !foundError && !foundEnd && numberOfHistoryListNext<maxLoopCount) // list_1.indexOf("CONTINUE") == 0 /*&& list_1.indexOf("END") != 0*/ )
      {
		  Integer i= new Integer(++numberOfHistoryListNext);
		  
	      oDBM.execCmd("backup_history_listnext",DBMUtility.OK_OK);
		  list_1 = oDBM.getAnswer();
		  
		  testResult+="\n"+i.toString()+". backup_history_listnext produced ";
		  if (list_1.indexOf("CONTINUE") == 0)
	      {
              testResult+="a \"CONTINUE\".";
		  }
		  else
		      if(list_1.indexOf("END") == 0)
		      {
		          testResult+="a \"END\".";
		          foundEnd=true;
			  }
			  else
			  {
		      	  testResult+="neither a \"CONTINUE\" nor an \"END\" - that is bad :(";
		          foundError=true;
			  }
          testResult+="\nAnswer was:\n--->\n"+list_1+"<---\n";
      }
      
      if(!foundError && !foundEnd)
      {
      	  Integer i= new Integer(maxLoopCount);
          testResult+="Did not found \"END\" within maximum of "+i.toString()+" loops - Could be a problem of the test or a real error.";
		  foundError=true;
      }
      
      addMessage("within "+getTestClassId()+".run()",
	             foundError?TestProtocol.ERROR:TestProtocol.INFO,
		         testResult);

      //closing history backup list
      oDBM.execCmd("backup_history_open",DBMUtility.OK_OK);
      //util release
      oDBM.execCmd("util_release",DBMUtility.OK_OK);

      //Run Directory clean up
      //System.out.println(sapdb.getRunDir()+"//"+outPath+i);
      File f = new File(sapdb.getRunDir()+oDBM.getSrvProperty("file.separator")+outPath);
      f.delete();
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
    return "BackupHistoryListnext";
  } // end getTestClassId

} //end of class MediumGet