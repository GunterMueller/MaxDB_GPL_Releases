package com.sap.sapdb.DBMTest.registration;
/*!
  -----------------------------------------------------------------------------
  module: Register.java
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

import java.util.StringTokenizer;
import java.util.Vector;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBReg
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer db_reg command tests
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class Register extends DBMTestCase
{
  public Register() throws SQLException, TestDatabaseException
  {
    super();
  }
  /**
   * run() contains the test implementation
   */
  public void runDBMTest()
  {
    boolean bFast  = false;
    boolean bSlow  = false;
    boolean bTest  = false;
    
    //  create one global try-catch block to handle exceptions
    try
    {
      m_DBM.execCmd("db_enum", DBMUtility.OK_OK);
      String param = m_DBM.getAnswer();

      //Creating vector for param
      Vector ParamArray = new Vector();

      //Checking what kernel are registerable
      StringTokenizer st_param = new StringTokenizer(param,"\n");
      while (st_param.hasMoreTokens()) {

        String ParamLine = st_param.nextToken();

        if (ParamLine.trim().indexOf(getDatabase().getName()) >= 0) {

          StringTokenizer st_st = new StringTokenizer(ParamLine,"\t");
          String DBSpeed = st_st.nextToken();
          DBSpeed = st_st.nextToken();
          DBSpeed = st_st.nextToken();
          DBSpeed = st_st.nextToken();

          if (DBSpeed.trim().indexOf("fast") >= 0) {
            bFast = true;
          }
          if (DBSpeed.trim().indexOf("slow") >= 0) {
            bSlow = true;
          }
          if (DBSpeed.trim().indexOf("test") >= 0) {
            bTest = true;
          }
        }
      }
      
      if (!bFast) {
        addMessage("db_reg", TestProtocol.ERROR , "Fast kernel not found");
      } // end if
      if (!bSlow) {
        addMessage("db_reg", TestProtocol.ERROR , "Slow kernel not found");
      } // end if
      if (!bTest) {
        addMessage("db_reg", TestProtocol.ERROR , "Test kernel not found");
      } // end if
      
      m_DBM.execCmd("db_reg", DBMUtility.OK_OK);
      m_DBM.execCmd("db_offline", DBMUtility.OK_OK);
      
      // try the ROOT option
      // It is not possible to make a real test with changing the root, because of
      // we have no other installation in a "change root" environment. But if we
      // had another installation 
      m_DBM.execCmd("db_reg ROOT " + m_DBM.getSrvProperty("dbmversion.instroot"), DBMUtility.OK_OK);
      m_DBM.execCmd("db_reg ROOT " + "unsinn", DBMUtility.ERR_FILECHK);

      
      if (m_DBM.getSrvProperty("dbmversion.os").compareTo("UNIX") != 0) {
	      // try the AUTO option
	      // It is not possible to check the success.
	      m_DBM.execCmd("db_reg AUTO", DBMUtility.OK_OK);
	      m_DBM.execCmd("db_reg AUTO OFF", DBMUtility.OK_OK);
	      
	      // try the USER option
	      // It is not possible to check the success.
	      m_DBM.execCmd("db_reg USER tilli,willy", DBMUtility.ERR_RTE);
	      m_DBM.execCmd("db_reg USER OFF", DBMUtility.OK_OK);
      } // end if
                 
      // try the GROUP option
      // It is not possible to check the GROUP option because I do not
      // know anything about the operating system user groups at the
      // test system.
      
      // try the DB option
      // The DB option will be checked in the com.sap.sapdb.DBMTest.register.Unregister
      // test.
      
      m_DBM.execCmd("db_online", DBMUtility.OK_OK);
      
    } catch (Exception e) {
      // standard error handler
      this.handleExceptions(e);
    } // end try catch

  } // end runDBMTest
  
  /**
   * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
   * will set the name to the name of the java class. To make the test name independend
   * from the java implementation, this method should return a unique string.
   * Attention: The uniqueness is not automatically checked by anybody.
   * @return The UNIQUE name of the test class
   */
  public static String getTestClassId() {
    return "Register";
  } // end getTestClassId

} //end of class DBReg