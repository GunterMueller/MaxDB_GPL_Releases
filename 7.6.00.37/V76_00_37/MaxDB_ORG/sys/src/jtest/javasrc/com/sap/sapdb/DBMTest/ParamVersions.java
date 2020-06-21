package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: ParamVersions.java
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
import com.sap.sapdb.DBMTest.util.DBMAnswerTable;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;

import java.util.StringTokenizer;
import java.util.Vector;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        ParamVersions
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer param_versions command tests
 * version:      7.4.*
 * Copyright (c) 2002-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class ParamVersions extends DBMTestCase
{
  private Vector ParamLabel;

  public ParamVersions() throws SQLException, TestDatabaseException
  {
    super();

    ParamLabel = new Vector();
  }
  /**
   * run() contains the test implementation
   */
  public void runDBMTest()
  {
    DBMAnswerTable oAnswer  = new DBMAnswerTable();
    TestDatabaseSap sapdb   = (TestDatabaseSap) getDatabase();
    //  create one global try-catch block to handle exceptions
    try
    {
      //Param versions
      m_DBM.execCmd("param_versions",DBMUtility.OK_OK);
      String param = m_DBM.getAnswer();

      //Creating vector for label
      Vector ParamArray = new Vector();

      StringTokenizer st_param = new StringTokenizer(param,"\n");
      while (st_param.hasMoreTokens()) {
        ParamArray.add(st_param.nextToken());
      }
      //Flag for search
      if (ParamArray.size() > 0) {
        //Comparing the shown parameter with the expected
        for (int i = 0;i<ParamArray.size();i++) {
          if ((ParamArray.get(i).toString().indexOf( sapdb.getInstRoot()
                                                    +m_DBM.getSrvProperty("file.separator")
                                                    +"config"
                                                    +m_DBM.getSrvProperty("file.separator")
                                                    +getDatabase().getName().toUpperCase()) < 0) &&
              (ParamArray.get(i).toString().indexOf( sapdb.getIndepDataPath()
                                                    +m_DBM.getSrvProperty("file.separator")
                                                    +"config"
                                                    +m_DBM.getSrvProperty("file.separator")
                                                    +getDatabase().getName().toUpperCase()) < 0) &&
              (ParamArray.get(i).toString().indexOf( "/usr/spool/sql"
                                                    +m_DBM.getSrvProperty("file.separator")
                                                    +"config"
                                                    +m_DBM.getSrvProperty("file.separator")
                                                    +getDatabase().getName().toUpperCase()) < 0)                                                    
                                                    ) {
            addMessage("param_versions", TestProtocol.ERROR ,
            		   ParamArray.get(i).toString() 
					   +"is not in \n\""
					   +sapdb.getInstRoot()
					   +"\" \nor \n\""
					   +sapdb.getIndepDataPath()
					   +"\"");
            break;
          } // end if
        }  // end for
      } // end if
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
    return "ParamVersions";
  } // end getTestClassId
} //end of class MediumGet