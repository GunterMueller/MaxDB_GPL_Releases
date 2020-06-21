package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: RecoverStateCheck.java
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

import com.sap.dbtech.powertoys.DBM;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        RecoverStateCheck
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer recover_state_check command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class RecoverStateCheck extends BackupState
{
  private ThreadGroup myThreads;

  public RecoverStateCheck() throws SQLException, TestDatabaseException
  {
    super();

    try
    {
      jbInit();
    }
    catch(Exception e)
    {
      e.printStackTrace();
    }
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);
    //create message table for checking DBM replies
    DBM dbm = null;
    String instroot = null;
    String outPath = getTestClassId();
    String mHost = getDatabase().getHost() + ":" + getDatabase().getPort();
    String mName = getDatabase().getName();
    try
    {
      dbm = DBM.dbDBM(mHost, mName);
      instroot = dbm.cmd("dbm_version INSTROOT").trim();
    }
    catch (com.sap.dbtech.rte.comm.RTEException e)
    {
      // standard error handler
      addMessage(this.toString() + ".run",
                 TestProtocol.ERROR,
                 e.toString());
    }
    catch (com.sap.dbtech.powertoys.DBMException e)
    {
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
      //util connect
      oDBM.execCmd("util_connect",DBMUtility.OK_OK);
      //create 1 backup media
      oDBM.execCmd("medium_put datamedium "+outPath+"1 file data 0 8 yes",DBMUtility.OK_OK);
      oDBM.execCmd("backup_start datamedium recovery data autoignore",DBMUtility.OK_OK);

      //Starting proper thread for recover_check
      RecoverCheckThread RCT = new RecoverCheckThread(getDatabase(), this, myThreads, mHost, mName, instroot);
      RCT.start();
      //waits for notify
      synchronized(RCT) {
        RCT.wait();
      }
      //Creating vector for label and two flags
      Vector LabelArray = new Vector();
      boolean m_LabelRead = false;
      boolean m_ThreadStillRunning = true;

      oDBM.execCmd("recover_state_check",DBMUtility.OK_OK);
      String Label = oDBM.getAnswer();

      //String will be splitted by Return
      while (m_LabelRead == false && m_ThreadStillRunning == true) {

        StringTokenizer st_label = new StringTokenizer(Label,"\n");
        //String will be splitted by Tabulator
        while (st_label.hasMoreTokens()) {

          StringTokenizer st_st = new StringTokenizer(st_label.nextToken(),"\t");
          while (st_st.hasMoreTokens()) {
            LabelArray.add(st_st.nextToken());
          }
        }
        //as long as there are threads in working
        if (myThreads.activeCount() == 0) {
          m_ThreadStillRunning = false;
        }

        if (m_ThreadStillRunning == true) {

          // Checking whether recover_state_check was successful

          System.out.println(LabelArray.get(6).toString());
          System.out.println(LabelArray.get(7).toString());
          System.out.println(LabelArray.get(8).toString());
          System.out.println(LabelArray.get(9).toString());
          System.out.println(LabelArray.get(10).toString());
          System.out.println(LabelArray.get(11).toString());
          System.out.println(LabelArray.get(12).toString());



          if (!LabelArray.get(9).toString().equalsIgnoreCase(mName))
          {
            oDBM.execCmd("recover_state_check",DBMUtility.OK_OK,false);
            Label = oDBM.getAnswer();

            LabelArray.removeAllElements();
          } else {

            long a = Long.parseLong((LabelArray.get(13)).toString());   // Pages Transferred
            long b = Long.parseLong((LabelArray.get(15)).toString());   // Pages Left
            long c = Long.parseLong((LabelArray.get(29)).toString());   // Page Count
            //checking whether transferred pages plus left pages equals page count
            if (a + b == c) {
              m_LabelRead = true;
            }
          }
        }
        Thread.sleep(10);
      }

      //Service release
      oDBM.execCmd("service_release",DBMUtility.OK_OK);
      //util connect
      oDBM.execCmd("util_release",DBMUtility.OK_OK);

      //checking whether result is correct
      if (m_LabelRead == true) {
          addMessage(this.toString() + ".run",
                 TestProtocol.INFO,
                 "Strings in label found \n"+Label);
      } else {
          addMessage(this.toString() + ".run",
                 TestProtocol.ERROR,
                 "Strings in label not found \n"+Label);
      }
    }
    // catch any kind of exception and write it to the protocol
    catch (Exception e) {
      // standard error handler

      this.handleExceptions(e);
    } // end try catch

    // clean up, i.e. close statements and connections
    finally {
      try {
        // print all parameters from the parameter object to the protocol.
        printParametersToProtocol();
      }
      catch (Exception e) {
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
    return "RecoverStateCheck";
  } // end getTestClassId

  private void jbInit() {
    myThreads = new ThreadGroup("DBM_Test_Threads");
  }
} //end of class MediumGet