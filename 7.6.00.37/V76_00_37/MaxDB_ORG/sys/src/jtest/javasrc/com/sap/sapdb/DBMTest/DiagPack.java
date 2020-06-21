package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DiagPack.java
  -----------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;


/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DiagPack
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer diag_pack command tests
 * version:      7.4.*
 * Copyright (c) 2002-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DiagPack extends DBMTestCase
{

  public DiagPack() throws SQLException, TestDatabaseException
  {
    super();
  }
  /**
   * run() contains the test implementation
   */
  public void runDBMTest()
  {
    TestDatabaseSap sapdb     = (TestDatabaseSap) getDatabase();
    boolean m_bHistEntryFound = false;
    String m_sDiagFile        = "";

    //  create one global try-catch block to handle exceptions
    try
    {
      m_sDiagFile = "\"" +
      	            sapdb.getRunDir() + 
                    m_DBM.getSrvProperty("file.separator") +
                    getTestClassId() +
                    ".tgz" +
                    "\"";

      m_DBM.execCmd("sql_connect " + getDatabase().getDBAUser() + "," + getDatabase().getDBAPassword(), DBMUtility.OK_OK);
      // Provoking kernel crash
      m_DBM.execCmd("sql_execute cccrashhh", DBMUtility.ERR_SQL);

      Thread.sleep(20000);

      m_DBM.execCmd("db_online -f", DBMUtility.OK_OK);

      // Getting diag history list
      m_DBM.execCmd("diag_pack", DBMUtility.OK_OK);
      m_DBM.execCmd("diag_pack file", DBMUtility.ERR_PARAM);
      m_DBM.execCmd("diag_pack file=" + m_sDiagFile, DBMUtility.OK_OK);
      m_DBM.execCmd("diag_pack file=" + m_sDiagFile + " class",                        DBMUtility.ERR_PARAM);
      m_DBM.execCmd("diag_pack file=" + m_sDiagFile + " class=protocol",               DBMUtility.OK_OK);
      m_DBM.execCmd("diag_pack file=" + m_sDiagFile + " class=backup",                 DBMUtility.OK_OK);
      m_DBM.execCmd("diag_pack file=" + m_sDiagFile + " class=config",                 DBMUtility.OK_OK);
      m_DBM.execCmd("diag_pack file=" + m_sDiagFile + " class=protocol,backup,config", DBMUtility.OK_OK);
      m_DBM.execCmd("diag_pack file=" + m_sDiagFile + " class=protocol date",          DBMUtility.ERR_PARAM);

      m_DBM.execCmd("diag_pack class=protocol", DBMUtility.OK_OK);
      m_DBM.execCmd("diag_pack class=backup", DBMUtility.OK_OK);
      m_DBM.execCmd("diag_pack class=config", DBMUtility.OK_OK);

      m_DBM.execCmd("diag_pack class=protocol,backup,config", DBMUtility.OK_OK);
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
    return "DiagPack";
  } // end getTestClassId
} //end of class DiagPack