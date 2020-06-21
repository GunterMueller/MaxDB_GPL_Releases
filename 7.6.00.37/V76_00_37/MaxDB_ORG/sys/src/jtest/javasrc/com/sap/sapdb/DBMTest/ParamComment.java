package com.sap.sapdb.DBMTest;

/*!
  -----------------------------------------------------------------------------
  module: ParamComment.java
  -----------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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
import com.sap.sapdb.DBMTest.util.DBMAnswerTable;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCaseException;

/* <xmp>
 * -----------------------------------------------------------------------------
 * Class:        ParamComment
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer comment sfor parameters test
 * version:      7.6. and higher
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class ParamComment extends DBMTestCase
{
  /**
   * A minimal constructor.Constructor.
   */
  public ParamComment() throws SQLException, TestDatabaseException
  {
          super();
  } // end DBMExample

  /**
   * Clean up test, i.e. delete data created in prepare, ...
   * @exception TestCaseException Should be thrown, when cleanUp fails
   */
  public static void cleanUp() throws TestCaseException
  {
    // do nothing
  } // end cleanUp

  /**
   * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
   * will set the name to the name of the java class. To make the test name independend
   * from the java implementation, this method should return a unique string.
   * Attention: The uniqueness is not automatically checked by anybody.
   * @return The UNIQUE name of the test class
   */
  public static String getTestClassId()
  {
    return "ParamComment";
  } // end getTestClassId

  /**
   * Sets the timeout value in milliseconds. After the timeout is reached, the test will be aborted
   * by the test driver.
   * @return The default return value is 0, which means there is no timeout value set. Test will never
   * be aborted by the test driver.
   */
  public static long getTimeout()
  {
    return 0;
  } // end getTimeout

  /**
   * Sets weather test can create and work with verification data or not.
   * @return The default return value is false, which means that no verification data
   * is used to verify the test results. In this case the test has to implement the verification
   * itself.
   */
  public static boolean isReferenceEnabled()
  {
    return false;
  } // end isReferenceEnabled

  /**
   * Sets weather test can be executed only on SAPDB or not.
   * @return The default return value is true. This means, the test can run on SAPDB only.
   */
  public static boolean isSAPOnly()
  {
          return true;
  } // end isSAPOnly

  /**
   * Prepare test, i.e. create users, tables and data
   * @exception TestCaseException Should be thrown, when preparation fails
   */
  public static void prepare() throws TestCaseException
  {
    // do nothing
  } // end prepare

  /**
   * run() contains the test implementation
   */
  public void runDBMTest()
  {
    //  create one global try-catch block to handle exceptions
    try
    {
      DBMAnswerTable oAnswer = new DBMAnswerTable();
      
      // get original value for MAXUSERTASKS
      m_DBM.execCmd("param_getvalue MAXUSERTASKS", DBMUtility.OK_OK);
      String szMaxUserTasks = m_DBM.getAnswer();
      szMaxUserTasks = szMaxUserTasks.substring(0, szMaxUserTasks.length()-1);
      int    nMaxUserTask   = Integer.parseInt(szMaxUserTasks);

      // 1. param_put MAXUSERTASKS without comment
      m_DBM.execCmd("param_put " +TestParameter+ " " + (nMaxUserTask+1), DBMUtility.OK_OK);
      
      // check history
      m_DBM.execCmd("param_gethistory name=" +TestParameter, DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(m_DBM.getAnswer());
      if (oAnswer.getField(0, "NAME").compareTo(TestParameter) != 0 || 
          oAnswer.getField(0, "COMMENT").length() != 0) {
        addMessage("param_gethistory", TestProtocol.ERROR ,
                   "Expected " + TestParameter + " without comment and found\n" +
	               oAnswer.getRow(0));
      } // end if
      
      // 2. param_put MAXUSERTASKS with comment
      m_DBM.execCmd("param_put " +TestParameter+ " " + (nMaxUserTask+2) + " \"" + Comment1 + "\"", DBMUtility.OK_OK);
      
      // check history
      m_DBM.execCmd("param_gethistory name=" +TestParameter, DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(m_DBM.getAnswer());
      if (oAnswer.getField(0, "NAME").compareTo(TestParameter) != 0 || 
          oAnswer.getField(0, "COMMENT").compareTo(Comment1) != 0) {
        addMessage("param_gethistory", TestProtocol.ERROR ,
                   "Expected " + TestParameter + " with comment \""+Comment1+"\" and found\n" +
                   oAnswer.getField(0, "COMMENT"));
      } // end if
      
      // 3. param_directputput MAXUSERTASKS without comment
      m_DBM.execCmd("param_directput " +TestParameter+ " " + (nMaxUserTask+3), DBMUtility.OK_OK);
      
      // check history
      m_DBM.execCmd("param_gethistory name=" +TestParameter, DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(m_DBM.getAnswer());
      if (oAnswer.getField(0, "NAME").compareTo(TestParameter) != 0 || 
          oAnswer.getField(0, "COMMENT").length() != 0) {
        addMessage("param_gethistory", TestProtocol.ERROR ,
                   "Expected " + TestParameter + " without comment and found\n" +
	               oAnswer.getRow(0));
      } // end if
      
      // 4. param_directput MAXUSERTASKS with comment
      m_DBM.execCmd("param_directput " +TestParameter+ " " + (nMaxUserTask+4) + " \"" + Comment2 + " \"", DBMUtility.OK_OK);
      
      // check history
      m_DBM.execCmd("param_gethistory name=" +TestParameter, DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(m_DBM.getAnswer());
      if (oAnswer.getField(0, "NAME").compareTo(TestParameter) != 0 || 
          oAnswer.getField(0, "COMMENT").compareTo(Comment2) != 0) {
        addMessage("param_gethistory", TestProtocol.ERROR ,
                   "Expected " + TestParameter + " with comment \""+Comment2+"\" and found\n" +
                   oAnswer.getField(0, "COMMENT"));
      } // end if

      // 5. param_directdel MAXUSERTASKS with comment
      m_DBM.execCmd("param_directdel " +TestParameter + " \"" + Comment3 + " \"", DBMUtility.OK_OK);
      
      // check history
      m_DBM.execCmd("param_gethistory name=" +TestParameter, DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(m_DBM.getAnswer());
      if (oAnswer.getField(0, "NAME").compareTo(TestParameter) != 0 || 
          oAnswer.getField(0, "COMMENT").compareTo(Comment3) != 0) {
        addMessage("param_gethistory", TestProtocol.ERROR ,
                   "Expected " + TestParameter + " with comment \""+Comment3+"\" and found\n" +
                   oAnswer.getField(0, "COMMENT"));
      } // end if
      
      // restore parameter
      m_DBM.execCmd("param_put " +TestParameter+ " " + nMaxUserTask, DBMUtility.OK_OK);
      
    }
    // catch any kind of exception and write it to the protocol
    catch (Exception e)
    {
      // standard error handler
      this.handleExceptions(e);
    } // end try catch

    // clean up, i.e. close statements and connections
    finally
    {
      try
      {
        // print all parameters from the parameter object to the protocol.
        printParametersToProtocol();
      }
      catch (Exception e)
      {
        // do nothing
      } // end try catch
    } // end finally
  } // end run

  public static final String TestParameter = "MAXUSERTASKS";
  public static final String Comment1 = "Das ist der Kommentar 1";
  public static final String Comment2 = "Das ist der Kommentar 2";
  public static final String Comment3 = "Das ist der Kommentar 3";
} // end class ParamComment
