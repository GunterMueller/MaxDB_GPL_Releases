package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DBDrop.java
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
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.utilities.Execute;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBDrop
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer db_crop command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DBDrop extends BackupState
{
  private DBMAnswerTable m_oVersion;

  public DBDrop() throws SQLException, TestDatabaseException
  {
    super();
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM         = new  DBMUtility(getDatabase(), this);
    TestDatabaseSap sapdb   = (TestDatabaseSap) getDatabase();
    m_oVersion              = new DBMAnswerTable();
    String currentCmd       = "";

    //  create one global try-catch block to handle exceptions
    try
    {
      //dropping database - without creating one
      oDBM.execCmd("dbm_version", DBMUtility.OK_OK);
      m_oVersion.analyzeColsByFieldSeparator(oDBM.getAnswer(), '=');

      if (m_oVersion.getField("OS", 0, 1).compareTo("UNIX") == 0) {

        currentCmd = sapdb.getInstRoot() + oDBM.getSrvProperty("file.separator") + "bin" +
                     oDBM.getSrvProperty("file.separator") + "dbmcli -d DBDrop -u dbm,dbm db_drop";
      } else {

        currentCmd = sapdb.getInstRoot() + oDBM.getSrvProperty("file.separator") + "pgm" +
                     oDBM.getSrvProperty("file.separator") + "dbmcli -d DBDrop -u dbm,dbm db_drop";
      } // end if

      boolean L_RC = exeCommand(currentCmd);

      if (L_RC == false) {
        addMessage("DBDrop - dropping database.", TestProtocol.INFO, "db_drop did not work.");
      } else {
        addMessage("DBDrop - dropping database.", TestProtocol.ERROR, "db_drop worked.");
      }

      currentCmd = "";

      //Creating new database 'DBDrop'
      if (m_oVersion.getField("OS", 0, 1).compareTo("UNIX") == 0) {

        currentCmd = sapdb.getInstRoot() + oDBM.getSrvProperty("file.separator") + "bin" +
                     oDBM.getSrvProperty("file.separator") + "dbmcli db_create DBDrop dbm,dbm";
      } else {

        currentCmd = sapdb.getInstRoot() + oDBM.getSrvProperty("file.separator") + "pgm" +
                     oDBM.getSrvProperty("file.separator") + "dbmcli.exe -R "+
                     sapdb.getInstRoot()+" db_create DBDrop dbm,dbm";
      } // end if

      L_RC = exeCommand(currentCmd);

      if (L_RC == true) {
        addMessage("DBDrop - creating database.", TestProtocol.INFO, "db_create worked.");
      } else {
        addMessage("DBDrop - creating database.", TestProtocol.ERROR, "db_create did not work.");
      }

      currentCmd = "";

      //Dropping existing database 'DBDrop'
      if (m_oVersion.getField("OS", 0, 1).compareTo("UNIX") == 0) {

        currentCmd = sapdb.getInstRoot() + oDBM.getSrvProperty("file.separator") + "bin" +
                     oDBM.getSrvProperty("file.separator") + "dbmcli -d DBDrop -u dbm,dbm db_drop";
      } else {

        currentCmd = sapdb.getInstRoot() + oDBM.getSrvProperty("file.separator") + "pgm" +
                     oDBM.getSrvProperty("file.separator") + "dbmcli -d DBDrop -u dbm,dbm db_drop";
      } // end if

      L_RC = exeCommand(currentCmd);

      if (L_RC == true) {
        addMessage("DBDrop - dropping database.", TestProtocol.INFO, "db_drop worked fine.");
      } else {
        addMessage("DBDrop - dropping database.", TestProtocol.ERROR, "db_drop did not work.");
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
    return "DBDrop";
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

      return false;
    }
  }
} //end of class MediumGet