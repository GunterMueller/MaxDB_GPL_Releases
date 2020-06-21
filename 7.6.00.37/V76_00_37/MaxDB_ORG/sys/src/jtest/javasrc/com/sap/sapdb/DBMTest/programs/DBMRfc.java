package com.sap.sapdb.DBMTest.programs;

/*!
  -----------------------------------------------------------------------------
  module: DBMRfc.java
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
 * Class:        DBMRfc
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer test example
 * version:      7.2.*
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class DBMRfc extends DBMTestCase
{
  /**
   * A minimal constructor.Constructor.
   */
  public DBMRfc() throws SQLException, TestDatabaseException
  {
          super();
  } // end DBMRfc

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
    return "DBMRfc";
  } // end getTestClassId

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
	    m_DBM.execCmd("dbm_getpath IndepProgPath", DBMUtility.OK_OK);
	    
        StringTokenizer Tokens = new StringTokenizer(m_DBM.getAnswer().toString().trim(),"\n");
        String strProgPath = m_DBM.getAnswer().toString().trim();
        if (Tokens.hasMoreTokens())
        {
        	strProgPath = Tokens.nextToken();
        }
        
    	StringBuffer ProcessOutput = new StringBuffer();
        
        // do something here
        if (m_DBM.getSrvProperty("dbmversion.os").compareTo("UNIX") == 0) {
            // execute UNIX process
        	Execute oDbmrfc = new Execute("dbmrfc", 
            		                      new String[] {"LD_LIBRARY_PATH="+strProgPath+"/lib",
        			                                    "SHLIB_PATH="+strProgPath+"/lib",
					                                    "LIBPATH="+strProgPath+"/lib"},
										  ProcessOutput);
            oDbmrfc.exec();
        } else {
            // execute windows process
        	Execute oDbmrfc = new Execute("dbmrfc", 
            		                      new String[] {"NO_SAP_ICU_PATH=1",
            		                                    "PATH="+strProgPath+"\\lib;"+strProgPath+"\\bin;"+strProgPath+"\\pgm"}, 
										  ProcessOutput);
            oDbmrfc.exec();
        } // end if

        // check result
        if (ProcessOutput.toString().trim().length() == 0) {
            addMessage("DBMRfc does work!",
                       TestProtocol.INFO,
                       ProcessOutput.toString().trim());
        } else {
            addMessage("DBMRfc does not work!",
                       TestProtocol.ERROR,
                       ProcessOutput.toString().trim());
        } // end if

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

} // end class DBMRfc
