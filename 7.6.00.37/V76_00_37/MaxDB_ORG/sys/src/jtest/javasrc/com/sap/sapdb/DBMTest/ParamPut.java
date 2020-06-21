package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: ParamPut.java
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
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        ParamPut
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer param_put command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class ParamPut extends BackupState
{
  private Vector ParamLabel_before;
  private Vector ParamLabel_after;

  public ParamPut() throws SQLException, TestDatabaseException
  {
    super();

    ParamLabel_before = new Vector();
    ParamLabel_after = new Vector();
    //Expected label (some keywords only) of param_directget
    ParamLabel_before.add("MAXDATAVOLUMES");
    ParamLabel_before.add("2");
    ParamLabel_after.add("MAXDATAVOLUMES");
    ParamLabel_after.add("3");
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);
    TestDatabaseSap sapdb = (TestDatabaseSap) getDatabase();

    //  create one global try-catch block to handle exceptions
    try
    {
      //opening param session
      oDBM.execCmd("param_startsession",DBMUtility.OK_OK);
      //param_put without param_commitsession
      oDBM.execCmd("param_put MAXDATAVOLUMES 3",DBMUtility.OK_OK);
      //closing param session
      oDBM.execCmd("param_abortsession",DBMUtility.OK_OK);
      //Getting info of MAXDATAVOLUMES
      oDBM.execCmd("param_directget MAXDATAVOLUMES",DBMUtility.OK_OK);
      String param = oDBM.getAnswer();

      //System.out.println(param);

      //Creating vector for label
      Vector ParamArray = new Vector();

      StringTokenizer st_param = new StringTokenizer(param,"\t");
      while (st_param.hasMoreTokens()) {
        ParamArray.add(st_param.nextToken());
      }

      boolean m_Label_OK = false;
      //Compare of both label arrays
      for (int i = 0;i < ParamArray.size();i++) {
        if (ParamArray.get(i).toString().trim().equals(ParamLabel_before.get(i).toString().trim())) {
          m_Label_OK = true;

          //System.out.println(ParamArray.get(i).toString().trim());
          //System.out.println(ParamLabel_before.get(i).toString().trim());

        } else {
          m_Label_OK = false;

          //System.out.println(ParamArray.get(i).toString().trim());
          //System.out.println(ParamLabel_before.get(i).toString().trim());
        }
      }

      if (m_Label_OK == true) {

        addMessage(this.toString() + ".run",
                   TestProtocol.INFO,
                   "Both labels are equal - param_put without session commit.\n"+param);
      } else {
        addMessage(this.toString() + ".run",
                   TestProtocol.ERROR,
                   "Both labels are not equal - param_put without session commit.\n"+param);
      }

      //opening param session
      oDBM.execCmd("param_startsession",DBMUtility.OK_OK);
      //param_put with param_commitsession
      oDBM.execCmd("param_put MAXDATAVOLUMES 3",DBMUtility.OK_OK);
      oDBM.execCmd("param_checkall",DBMUtility.OK_OK);
      oDBM.execCmd("param_commitsession",DBMUtility.OK_OK);

      //Restarting Database
      oDBM.execCmd("db_offline -" + sapdb.getKernelMode(),DBMUtility.OK_OK);
      oDBM.execCmd("db_online -" + sapdb.getKernelMode(),DBMUtility.OK_OK);

      //Getting info of MAXDATAVOLUMES
      oDBM.execCmd("param_directget MAXDATAVOLUMES",DBMUtility.OK_OK);
      param = oDBM.getAnswer();

      //System.out.println(param);

      //Cleaning vector for label
      ParamArray.removeAllElements();

      st_param = new StringTokenizer(param,"\t");
      while (st_param.hasMoreTokens()) {
        ParamArray.add(st_param.nextToken());
      }

      m_Label_OK = false;
      //Compare of both label arrays
      for (int i = 0;i < ParamArray.size();i++) {
        if (ParamArray.get(i).toString().trim().equals(ParamLabel_after.get(i).toString().trim())) {
          m_Label_OK = true;

          //System.out.println(ParamArray.get(i).toString().trim());
          //System.out.println(ParamLabel_after.get(i).toString().trim());

        } else {
          m_Label_OK = false;

          //System.out.println(ParamArray.get(i).toString().trim());
          //System.out.println(ParamLabel_after.get(i).toString().trim());
        }
      }

      if (m_Label_OK == true) {

        addMessage(this.toString() + ".run",
                   TestProtocol.INFO,
                   "Both labels are equal - param_put with session commit.\n"+param);
      } else {
        addMessage(this.toString() + ".run",
                   TestProtocol.ERROR,
                   "Both labels are not equal - param_put with session commit.\n"+param);
      }

      //Setting back to default value
      //opening param session
      oDBM.execCmd("param_startsession",DBMUtility.OK_OK);
      //param_put with param_commitsession
      oDBM.execCmd("param_put MAXDATAVOLUMES 2",DBMUtility.OK_OK);
      oDBM.execCmd("param_checkall",DBMUtility.OK_OK);
      oDBM.execCmd("param_commitsession",DBMUtility.OK_OK);

      //Restarting Database
      oDBM.execCmd("db_offline -" + sapdb.getKernelMode(),DBMUtility.OK_OK);
      oDBM.execCmd("db_online -" + sapdb.getKernelMode(),DBMUtility.OK_OK);
      /**/
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
    return "ParamPut";
  } // end getTestClassId
} //end of class MediumGet