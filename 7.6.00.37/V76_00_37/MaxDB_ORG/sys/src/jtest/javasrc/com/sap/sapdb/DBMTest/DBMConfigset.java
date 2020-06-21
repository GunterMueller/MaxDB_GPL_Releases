package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DBMConfigset.java
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
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBMConfigset
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer dbm_configset command tests
 * version:      7.4.*
 * Copyright (c) 2002-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DBMConfigset extends DBMTestCase
{
  public DBMConfigset() throws SQLException, TestDatabaseException
  {
    super();
  }

/**
   * run() contains the test implementation
   */
  public void runDBMTest()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);

    //  create one global try-catch block to handle exceptions
    try
    {
      // 1. negativ testing for error message
      oDBM.execCmd("dbm_configset", DBMUtility.ERR_TOFEWPARAM);
      
      // 2. value should not exist
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 2)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if
      
      // 3. set value
      oDBM.execCmd("dbm_configset DBMCONFIGSETTEST 2", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("2") != 0) {
        addMessage(getTestClassId()+" (Phase 3.1)", TestProtocol.ERROR, "'2' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configget INSTANCE DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("2") != 0) {
        addMessage(getTestClassId()+" (Phase 3.2)", TestProtocol.ERROR, "'2' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configget INSTALLATION DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 3.3)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configget GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 3.4)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if
      
      // 4. delete value
      oDBM.execCmd("dbm_configset DBMCONFIGSETTEST", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 4)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if

      // 5. set value explicit for INSTANCE
      oDBM.execCmd("dbm_configset INSTANCE DBMCONFIGSETTEST 2", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("2") != 0) {
        addMessage(getTestClassId()+" (Phase 5.1)", TestProtocol.ERROR, "'2' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configget INSTANCE DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("2") != 0) {
        addMessage(getTestClassId()+" (Phase 5.2)", TestProtocol.ERROR, "'2' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configget INSTALLATION DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 5.3)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configget GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 5.4)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if

      // 6. delete value
      oDBM.execCmd("dbm_configset INSTANCE DBMCONFIGSETTEST", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 6)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if
      
      // 7. set value explicit for INSTALLATION
      oDBM.execCmd("dbm_configset INSTALLATION DBMCONFIGSETTEST 2", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("2") != 0) {
        addMessage(getTestClassId()+" (Phase 7.1)", TestProtocol.ERROR, "'2' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configget INSTANCE DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 7.2)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configget INSTALLATION DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("2") != 0) {
        addMessage(getTestClassId()+" (Phase 7.3)", TestProtocol.ERROR, "'2' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configget GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 7.4)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if
      
      // 8. delete value
      oDBM.execCmd("dbm_configset INSTALLATION DBMCONFIGSETTEST", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 8)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if
      
      // 9. set value explicit for GLOBAL
      oDBM.execCmd("dbm_configset GLOBAL DBMCONFIGSETTEST 2", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("2") != 0) {
        addMessage(getTestClassId()+" (Phase 9.1)", TestProtocol.ERROR, "'2' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configget INSTANCE DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 9.2)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configget INSTALLATION DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 9.3)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configget GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("2") != 0) {
        addMessage(getTestClassId()+" (Phase 9.4)", TestProtocol.ERROR, "'2' expected, got: " + oDBM.getAnswer());
      } // end if
      
      // 10. delete parameter
      oDBM.execCmd("dbm_configset GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 10)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if
      
      // 11. testing overrule value
      oDBM.execCmd("dbm_configset INSTALLATION DBMCONFIGSETTEST 2", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("2") != 0) {
        addMessage(getTestClassId()+" (Phase 11.1)", TestProtocol.ERROR, "'2' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configset DBMCONFIGSETTEST 7", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("7") != 0) {
        addMessage(getTestClassId()+" (Phase 11.2)", TestProtocol.ERROR, "'7' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configset DBMCONFIGSETTEST", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("2") != 0) {
        addMessage(getTestClassId()+" (Phase 11.3)", TestProtocol.ERROR, "'2' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configset INSTANCE DBMCONFIGSETTEST 7", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("7") != 0) {
        addMessage(getTestClassId()+" (Phase 11.4)", TestProtocol.ERROR, "'7' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configset INSTANCE DBMCONFIGSETTEST", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("2") != 0) {
        addMessage(getTestClassId()+" (Phase 11.5)", TestProtocol.ERROR, "'2' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configset GLOBAL DBMCONFIGSETTEST 9", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("2") != 0) {
        addMessage(getTestClassId()+" (Phase 11.6)", TestProtocol.ERROR, "'2' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configset INSTALLATION DBMCONFIGSETTEST", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().compareTo("9") != 0) {
        addMessage(getTestClassId()+" (Phase 11.7)", TestProtocol.ERROR, "'9' expected, got: " + oDBM.getAnswer());
      } // end if
      oDBM.execCmd("dbm_configset GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 11.8)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if
      
      // 12. delete parameter
      oDBM.execCmd("dbm_configset GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
      oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
      if (oDBM.getAnswer().trim().length() > 0) {
        addMessage(getTestClassId()+" (Phase 12)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
      } // end if
      
    }  catch (Exception e) {
      // standard error handler
      this.handleExceptions(e);
    } // end try catch

    // clean up, i.e. close statements and connections
    finally {
      try {
        // print all parameters from the parameter object to the protocol.
        printParametersToProtocol();
      }  catch (Exception e) {
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
    return "DBMConfigset";
  } // end getTestClassId
} //end of class MediumGet