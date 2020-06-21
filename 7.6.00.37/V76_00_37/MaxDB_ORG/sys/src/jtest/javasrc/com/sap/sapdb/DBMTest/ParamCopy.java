package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: ParamCopy.java
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
import java.util.Properties;

import com.sap.dbtech.powertoys.DBM;
import com.sap.sapdb.DBMTest.util.*;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.utilities.Execute;


/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        ParamCopy
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer param_copy command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class ParamCopy extends BackupState
{
  private DBM m_DBM;
  private DBMUtility m_uDBM;
  private Properties properties;

  private TestDatabaseSap sapdb;

  private String dbrootKeyC = "dbroot";
  private String dbnameKeyC = "dbname";
  private String hostKeyC = "host";

  private String mHost;
  private String mName;
  private String mRoot;

  private DBMAnswerTable m_oVersion;

  public ParamCopy() throws SQLException, TestDatabaseException
  {
    super();

    sapdb = (TestDatabaseSap) getDatabase();

    mHost = getDatabase().getHost() + ":" + getDatabase().getPort();
    mName = "ParamC";

    try {
      mRoot = sapdb.getInstRoot();
    }
    catch (Exception e) {
      // standard error handler

      this.handleExceptions(e);
    } // end try catch
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM   = new  DBMUtility(getDatabase(), this);
    m_oVersion        = new DBMAnswerTable();
    String currentCmd = "";
    //  create one global try-catch block to handle exceptions
    try
    {
      currentCmd = "dbmcli -R "+ sapdb.getInstRoot()+" db_create " + mName + " dbm,dbm";

      boolean L_RC = exeCommand(currentCmd);

      if (L_RC == true) {
        addMessage(mName + " - creating database.", TestProtocol.INFO, "db_create worked.");
      } else {
        addMessage(mName + " - creating database.", TestProtocol.ERROR, "db_create did not work.");
      }

      properties = new Properties();
      properties.setProperty (hostKeyC, mHost);
      properties.setProperty (dbnameKeyC, mName);
      properties.setProperty (dbrootKeyC, mRoot);

      try
      {
        m_DBM = new DBM(properties);
      }
      catch (com.sap.dbtech.rte.comm.RTEException e)
      {
        addMessage(this.toString() + ".run",
                    TestProtocol.ERROR,
                    e.toString());
      }
      //Inital new dbm session
      m_uDBM = new DBMUtility(getDatabase(), this, m_DBM);

      //connecting to database Copy
      m_uDBM.execCmd("user_logon dbm,dbm" ,DBMUtility.OK_OK);
      //Param start session
      m_uDBM.execCmd("param_startsession" ,DBMUtility.OK_OK);
      //Too few parameters
      m_uDBM.execCmd("param_copy",DBMUtility.ERR_TOFEWPARAM);
      //Parameter copy of current database
      m_uDBM.execCmd("param_copy "+getDatabase().getName() ,DBMUtility.OK_OK);
      //droping database instance
      m_uDBM.execCmd("db_drop" ,DBMUtility.OK_OK);

    } catch (Exception e) {
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
    return "ParamCopy";
  } // end getTestClassId

  //Executes a statement without dbm session
  private boolean exeCommand(String command) {

    String mTestName = getTestClassId();

    StringBuffer ProcessOutput = new StringBuffer();
    Execute DbmcliProcess = new Execute(command, null, ProcessOutput);
    DbmcliProcess.exec();
    if (ProcessOutput.toString().trim().equals("OK")) {
      return true;
    } else {
      addMessage(command,
                 TestProtocol.ERROR,
                 ProcessOutput.toString());
      return false;
    }
  }
} //end of class MediumGet