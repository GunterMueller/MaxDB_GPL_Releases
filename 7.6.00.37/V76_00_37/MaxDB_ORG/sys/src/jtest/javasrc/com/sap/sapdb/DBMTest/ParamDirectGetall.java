package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: ParamDirectGetall.java
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
 * Class:        ParamDirectGetall
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer param_directgetall command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class ParamDirectGetall extends BackupState
{
  private Vector ParamLabel;

  public ParamDirectGetall() throws SQLException, TestDatabaseException
  {
    super();

    ParamLabel = new Vector();
    //Expected label (some keywords only) of param_directgetall
    ParamLabel.add("KERNELVERSION");
    ParamLabel.add("INSTANCE_TYPE");
    ParamLabel.add("DEFAULT_CODE");
    ParamLabel.add("DATE_TIME_FORMAT");
    ParamLabel.add("CONTROLUSERID");
    ParamLabel.add("CONTROLPASSWORD");
    ParamLabel.add("MAXLOGVOLUMES");
    ParamLabel.add("MAXDATAVOLUMES");
    ParamLabel.add("LOG_MIRRORED");
    ParamLabel.add("LOG_SEGMENT_SIZE");
    ParamLabel.add("LOG_IO_QUEUE");
    ParamLabel.add("MAXCPU");
    ParamLabel.add("MAXUSERTASKS");
    ParamLabel.add("MAXSERVERTASKS");
    ParamLabel.add("MAXLOCKS");
    ParamLabel.add("SESSION_TIMEOUT");
    ParamLabel.add("REQUEST_TIMEOUT");
    ParamLabel.add("RUNDIRECTORY");
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
      //Getting info 
      oDBM.execCmd("param_directgetall",DBMUtility.OK_OK);
      String param = oDBM.getAnswer();

      //Creating vector for label
      Vector ParamArray = new Vector();

      StringTokenizer st_param = new StringTokenizer(param,"\n");
      while (st_param.hasMoreTokens())
      {
        StringTokenizer st_st = new StringTokenizer(st_param.nextToken(),"\t");
        while (st_st.hasMoreTokens())
        {
          ParamArray.add(st_st.nextToken());
        }
      }

      int m_paramok_count = 0;
      int k = 0;
      //Checking the gotten label string
      while (k < ParamLabel.size()) {
        for (int i = 0;i < ParamArray.size();i++) {

          //System.out.println(ParamArray.get(i).toString().trim());
          //System.out.println(ParamLabel.get(k).toString().trim());

          if (ParamArray.get(i).toString().trim().equals(ParamLabel.get(k).toString().trim())) {
            m_paramok_count++;
            break;
          }
        }
        k++;
      }

      //System.out.println(m_paramok_count + "  :  " + ParamLabel.size());

      if (m_paramok_count == ParamLabel.size()) {
        addMessage(this.toString() + ".run",
                   TestProtocol.INFO,
                   "Both labels are equal."+param);
      } else {
        addMessage(this.toString() + ".run",
                   TestProtocol.ERROR,
                   "Both labels are not equal."+param);
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
    return "ParamDirectGetall";
  } // end getTestClassId
} //end of class MediumGet