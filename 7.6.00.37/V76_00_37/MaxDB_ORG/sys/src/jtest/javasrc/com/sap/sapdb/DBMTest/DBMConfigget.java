package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: DBMConfigget.java
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
 * Class:        DBMConfigget
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer dbm_configget command tests
 * version:      7.4.*
 * Copyright (c) 2002-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class DBMConfigget extends DBMTestCase
{
  // constructor
  public DBMConfigget() throws SQLException, TestDatabaseException
  {
    super();
  } // end method DBMConfigget
  
  // class name
  public static String getTestClassId() 
  {
    return "DBMConfigget";
  } // end method  getTestClassId

  // run the test  
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
          addMessage(getTestClassId()+" (Phase 2.1)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 2.2)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 2.3)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 2.4)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 2.5)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 2.6)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 2.7)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget GLOBAL", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 2.8)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        
        // 3. set value (implicit to instance)
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
        oDBM.execCmd("dbm_configget", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") < 0) {
          addMessage(getTestClassId()+" (Phase 3.5)", TestProtocol.ERROR, "DBMCONFIGSETTEST expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") < 0) {
          addMessage(getTestClassId()+" (Phase 3.6)", TestProtocol.ERROR, "DBMCONFIGSETTEST expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 3.7)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget GLOBAL", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 3.8)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        
        // 4. delete value
        oDBM.execCmd("dbm_configset DBMCONFIGSETTEST", DBMUtility.OK_OK);
        oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 4.1)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 4.2)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 4.3)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 4.4)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 4.5)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 4.6)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 4.7)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget GLOBAL", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 4.8)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if

        // 5. set value (explicit to INSTANCE)
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
        oDBM.execCmd("dbm_configget", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") < 0) {
          addMessage(getTestClassId()+" (Phase 5.5)", TestProtocol.ERROR, "DBMCONFIGSETTEST expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") < 0) {
          addMessage(getTestClassId()+" (Phase 5.6)", TestProtocol.ERROR, "DBMCONFIGSETTEST expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 5.7)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget GLOBAL", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 5.8)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        
        // 6. delete value (explicit from INSTANCE)
        oDBM.execCmd("dbm_configset INSTANCE DBMCONFIGSETTEST", DBMUtility.OK_OK);
        oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 6.1)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 6.2)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 6.3)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 6.4)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 6.5)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 6.6)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 6.7)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget GLOBAL", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 6.8)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        
        // 7. set value (explicit to INSTALLATION)
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
        oDBM.execCmd("dbm_configget", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") < 0) {
            addMessage(getTestClassId()+" (Phase 7.5)", TestProtocol.ERROR, "DBMCONFIGSETTEST expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
            addMessage(getTestClassId()+" (Phase 7.6)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") < 0) {
            addMessage(getTestClassId()+" (Phase 7.7)", TestProtocol.ERROR, "DBMCONFIGSETTEST expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget GLOBAL", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 7.8)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        
        // 8. delete value (explicit from INSTALLATION)
        oDBM.execCmd("dbm_configset INSTALLATION DBMCONFIGSETTEST", DBMUtility.OK_OK);
        oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 8.1)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 8.2)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 8.3)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 8.4)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 8.5)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 8.6)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 8.7)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget GLOBAL", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 8.8)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        
        // 9. set value (explicit to GLOBAL)
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
        oDBM.execCmd("dbm_configget", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") < 0) {
            addMessage(getTestClassId()+" (Phase 9.5)", TestProtocol.ERROR, "DBMCONFIGSETTEST expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
            addMessage(getTestClassId()+" (Phase 9.6)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
            addMessage(getTestClassId()+" (Phase 9.7)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
          } // end if
        oDBM.execCmd("dbm_configget GLOBAL", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") < 0) {
            addMessage(getTestClassId()+" (Phase 9.8)", TestProtocol.ERROR, "DBMCONFIGSETTEST expected, got: " + oDBM.getAnswer());
        } // end if
        
        // 10. delete value (explicit from GLOBAL)
        oDBM.execCmd("dbm_configset GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
        oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 10.1)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 10.2)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 10.3)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
          addMessage(getTestClassId()+" (Phase 10.4)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 10.5)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTANCE", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 10.6)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget INSTALLATION", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 10.7)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configget GLOBAL", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().indexOf("DBMCONFIGSETTEST") >= 0) {
          addMessage(getTestClassId()+" (Phase 10.8)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
        } // end if
        
        // 11. overrule
        oDBM.execCmd("dbm_configset GLOBAL DBMCONFIGSETTEST 2", DBMUtility.OK_OK);
        oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().compareTo("2") != 0) {
            addMessage(getTestClassId()+" (Phase 11.1)", TestProtocol.ERROR, "'2' expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configset INSTALLATION DBMCONFIGSETTEST 3", DBMUtility.OK_OK);
        oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().compareTo("3") != 0) {
            addMessage(getTestClassId()+" (Phase 11.2)", TestProtocol.ERROR, "'3' expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configset INSTANCE DBMCONFIGSETTEST 4", DBMUtility.OK_OK);
        oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().compareTo("4") != 0) {
            addMessage(getTestClassId()+" (Phase 11.3)", TestProtocol.ERROR, "'4' expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configset DBMCONFIGSETTEST 5", DBMUtility.OK_OK);
        oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().compareTo("5") != 0) {
            addMessage(getTestClassId()+" (Phase 11.4)", TestProtocol.ERROR, "'5' expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configset DBMCONFIGSETTEST", DBMUtility.OK_OK);
        oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().compareTo("3") != 0) {
            addMessage(getTestClassId()+" (Phase 11.5)", TestProtocol.ERROR, "'3' expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configset GLOBAL DBMCONFIGSETTEST", DBMUtility.OK_OK);
        oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().compareTo("3") != 0) {
            addMessage(getTestClassId()+" (Phase 11.6)", TestProtocol.ERROR, "'3' expected, got: " + oDBM.getAnswer());
        } // end if
        oDBM.execCmd("dbm_configset INSTALLATION DBMCONFIGSETTEST", DBMUtility.OK_OK);
        oDBM.execCmd("dbm_configget DBMCONFIGSETTEST", DBMUtility.OK_OK);
        if (oDBM.getAnswer().trim().length() > 0) {
            addMessage(getTestClassId()+" (Phase 11.7)", TestProtocol.ERROR, "Nothing expected, got: " + oDBM.getAnswer());
          } // end if
                
    } catch (Exception e) {
      // standard error handler
      this.handleExceptions(e);
    } // end try catch

    // clean up, i.e. close statements and connections
    finally {
      try {
        // print all parameters from the parameter object to the protocol.
        printParametersToProtocol();
      } catch (Exception e) {
        // do nothing
      } // end try catch
    } // end finally
  
  } // end method runDBMTest

} //end class
