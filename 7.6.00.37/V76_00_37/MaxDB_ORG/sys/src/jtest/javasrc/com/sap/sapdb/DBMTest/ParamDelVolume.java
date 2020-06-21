package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: ParamDelVolume.java
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
 * Class:        ParamDelVolume
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer param_delvolume command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class ParamDelVolume extends BackupState
{
  private Vector ParamLabelData;
  private Vector ParamLabelLog;

  public ParamDelVolume() throws SQLException, TestDatabaseException
  {
    super();

    ParamLabelLog  = new Vector();
    ParamLabelData = new Vector();
    //Expected label (some keywords only) of param_getvolsall
    ParamLabelLog.add("ARCHIVE_LOG_001");
    ParamLabelLog.add("LOG01.dat");
    ParamLabelData.add("DATADEV_0001");
    ParamLabelData.add("DAT01.dat");
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);
    TestDatabaseSap sapdb = (TestDatabaseSap) getDatabase();
    String name = getTestClassId();

    //  create one global try-catch block to handle exceptions
    try
    {

      //param_delvolume
      //Too few parameters
      oDBM.execCmd("param_delvolume",DBMUtility.ERR_TOFEWPARAM);
      //Too few parameters - Number of volume
      oDBM.execCmd("param_delvolume 1",DBMUtility.ERR_TOFEWPARAM);
      //Number of volume, Type of volume
      oDBM.execCmd("param_delvolume 1 DATA",DBMUtility.OK_OK);

      oDBM.execCmd("param_getvolsall",DBMUtility.OK_OK);
      String param = oDBM.getAnswer();

      //Creating vector for label
      Vector ParamArray = new Vector();

      StringTokenizer st_param = new StringTokenizer(param,"\n");
      while (st_param.hasMoreTokens()) {
        StringTokenizer st_st = new StringTokenizer(st_param.nextToken()," ");
          while (st_st.hasMoreTokens()) {

            ParamArray.add(st_st.nextToken());
        }
      }

      boolean m_Label_OK = false;
      boolean m_Label_Found = false;
      //Compare of both label arrays
      for (int i = 0;i < ParamLabelData.size();i++) {

        for(int k = 0;k < ParamArray.size();k++) {
          if (ParamLabelData.get(i).toString().trim().equals(ParamArray.get(k).toString().trim())) {
            m_Label_Found = true;
          }
        }
        if (m_Label_Found == true) {
          m_Label_OK = false;
          break;
        } else {
          m_Label_OK = true;
        }
        m_Label_Found = false;
      }

      if (m_Label_OK == true) {

        addMessage(this.toString() + ".run",
                   TestProtocol.INFO,
                   "The before created volumes could not be found.\n"+param);
      } else {
        addMessage(this.toString() + ".run",
                   TestProtocol.ERROR,
                   "The before created volumes are still in param file.\n"+param);
      }

      //Restoring old param file
      oDBM.execCmd("param_restore 1",DBMUtility.OK_OK);

      //param_delvolume
      //Too few parameters
      oDBM.execCmd("param_delvolume",DBMUtility.ERR_TOFEWPARAM);
      //Too few parameters - Number of volume
      oDBM.execCmd("param_delvolume 1",DBMUtility.ERR_TOFEWPARAM);
      //Number of volume, Type of volume
      oDBM.execCmd("param_delvolume 1 LOG",DBMUtility.OK_OK);

      oDBM.execCmd("param_getvolsall",DBMUtility.OK_OK);
      param = oDBM.getAnswer();

      //Creating vector for label
      ParamArray = new Vector();

      st_param = new StringTokenizer(param,"\n");
      while (st_param.hasMoreTokens()) {
        StringTokenizer st_st = new StringTokenizer(st_param.nextToken()," ");
          while (st_st.hasMoreTokens()) {

            ParamArray.add(st_st.nextToken());
        }
      }

      m_Label_OK = false;
      m_Label_Found = false;
      //Compare of both label arrays
      for (int i = 0;i < ParamLabelLog.size();i++) {

        for(int k = 0;k < ParamArray.size();k++) {
          if (ParamLabelLog.get(i).toString().trim().equals(ParamArray.get(k).toString().trim())) {
            m_Label_Found = true;
          }
        }
        if (m_Label_Found == true) {
          m_Label_OK = false;
          break;
        } else {
          m_Label_OK = true;
        }
        m_Label_Found = false;
      }

      if (m_Label_OK == true) {

        addMessage(this.toString() + ".run",
                   TestProtocol.INFO,
                   "The before created volumes could not be found.\n"+param);
      } else {
        addMessage(this.toString() + ".run",
                   TestProtocol.ERROR,
                   "The before created volumes are still in param file.\n"+param);
      }

      //Restoring old param file
      oDBM.execCmd("param_restore 1",DBMUtility.OK_OK);

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
    return "ParamDelVolume";
  } // end getTestClassId
} //end of class MediumGet