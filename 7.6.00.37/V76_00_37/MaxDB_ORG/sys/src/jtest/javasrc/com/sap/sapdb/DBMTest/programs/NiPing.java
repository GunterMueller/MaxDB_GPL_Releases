package com.sap.sapdb.DBMTest.programs;

/*!
  -----------------------------------------------------------------------------
  module: NiPing.java
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
import java.util.StringTokenizer;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCaseException;
import com.sap.sapdb.testframe.utilities.Execute;

/**
 * DBMServer test example
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        NiPing
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer test example
 * version:      7.2.*
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class NiPing extends DBMTestCase
{
  /**
   * A minimal constructor.Constructor.
   */
  public NiPing() throws SQLException, TestDatabaseException
  {
          super();
  } // end NiPing

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
    return "NiPing";
  } // end getTestClassId

  /**
   * Sets the timeout value in milliseconds. After the timeout is reached, the test will be aborted
   * by the test driver.
   * @return The default return value is 0, which means there is no timeout value set. Test will never
   * be aborted by the test driver.
   */
  public static long getTimeout()
  {
    return 120000;
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
        // get IndepProgPath (to extend libpath with shared objects for niping)
    	m_DBM.execCmd("dbm_getpath IndepProgPath", DBMUtility.OK_OK);
        StringTokenizer Tokens = new StringTokenizer(m_DBM.getAnswer().toString().trim(),"\n");
        String strProgPath = m_DBM.getAnswer().toString().trim();
        if (Tokens.hasMoreTokens())
        	strProgPath = Tokens.nextToken();
    	
    	// start niping at the server
    	m_DBM.execCmd("exec_nipingsrv", DBMUtility.OK_OK);

    	// ping server
    	StringBuffer ProcessOutput = new StringBuffer();
        int          nRc           = 0;

        // do something here
        if (m_DBM.getSrvProperty("dbmversion.os").compareTo("UNIX") == 0) {
            // execute UNIX process
        	Execute oNiPing = new Execute(this.getDBRoot()+ "/sap/niping -c -H " + getDatabase().getHost(), 
            		                      new String[] {"LD_LIBRARY_PATH="+strProgPath+"/lib",
        			                                    "SHLIB_PATH="+strProgPath+"/lib",
					                                    "LIBPATH="+strProgPath+"/lib"},
										  ProcessOutput);
        	oNiPing.exec();
        	nRc = oNiPing.getExitValue();
        } else {
        	// determine SystemRoot environment variable needed for niping
	    	StringBuffer oSystemRootBuffer = new StringBuffer();
	    	Execute oSystemRoot = new Execute("cmd /c set SystemRoot", null ,  oSystemRootBuffer);
	        oSystemRoot.exec();
	        
            // execute windows process
        	Execute oNiPing = new Execute(this.getDBRoot()+ "\\sap\\niping -c -H " + getDatabase().getHost(), 
                                          new String[] {"NO_SAP_ICU_PATH=1",
            		                                    "PATH="+strProgPath+"\\lib;"+strProgPath+"\\bin;"+strProgPath+"\\pgm",
            		                                    oSystemRootBuffer.toString().trim()}, 
										  ProcessOutput);
        	
        	oNiPing.exec();
        	nRc = oNiPing.getExitValue();
        } // end if
        
        if (nRc != 0)
            addMessage("NiPing - niping does not reach the server", TestProtocol.ERROR, ProcessOutput.toString());
       
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

} // end class NiPing
