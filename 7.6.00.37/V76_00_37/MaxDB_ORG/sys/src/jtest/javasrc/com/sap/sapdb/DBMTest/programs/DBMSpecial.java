package com.sap.sapdb.DBMTest.programs;

/*!
  -----------------------------------------------------------------------------
  module: DBMSpecial.java
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
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCaseException;
import com.sap.sapdb.testframe.utilities.Execute;

/**
 * DBMServer test example
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBMSpecial
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer test example
 * version:      7.2.*
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class DBMSpecial extends DBMTestCase
{
  /**
   * A minimal constructor.Constructor.
   */
  public DBMSpecial() throws SQLException, TestDatabaseException
  {
          super();
  } // end DBMSpecial

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
    return "DBMSpecial";
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
		  String Sep = m_DBM.getSrvProperty("file.separator");
    	
		  StringBuffer ProcessOutput;
		  Execute      oProcess;
    	
		  // 1. Use dbmstop
		  ProcessOutput = new StringBuffer();
		  oProcess = new Execute(this.getDBRoot()+Sep+"pgm"+Sep+"dbmstop " + getDatabase().getName(), null , ProcessOutput);
		  oProcess.exec();
		  
		  if (oProcess.getExitValue() != 0) {
			  addMessage("dbmstop does not work!", TestProtocol.ERROR,
					  "Output: "+ProcessOutput.toString().trim()+"\n("+__FILE__()+","+ __LINE__()+")");
		  } // end if
		  
		  // 2. Use dbmstart
		  ProcessOutput = new StringBuffer();
		  oProcess = new Execute(this.getDBRoot()+Sep+"pgm"+Sep+"dbmstart " + getDatabase().getName(), null , ProcessOutput);
		  oProcess.exec();
		  
		  if (oProcess.getExitValue() != 0) {
			  addMessage("dbmstart does not work!", TestProtocol.ERROR,
					  "Output: "+ProcessOutput.toString().trim()+"\n("+__FILE__()+","+ __LINE__()+")");
		  } // end if
    	
		  // 3. Use dbmstop again
		  ProcessOutput = new StringBuffer();
		  oProcess = new Execute(this.getDBRoot()+Sep+"pgm"+Sep+"dbmstop " + getDatabase().getName(), null , ProcessOutput);
		  oProcess.exec();
		  
		  if (oProcess.getExitValue() != 0) {
			  addMessage("dbmstop does not work!", TestProtocol.ERROR,
					  "Output: "+ProcessOutput.toString().trim()+"\n("+__FILE__()+","+ __LINE__()+")");
		  } // end if
    	
		  // 4. Use dbmunreg (negative testing)
		  ProcessOutput = new StringBuffer();
		  oProcess = new Execute(this.getDBRoot()+Sep+"pgm"+Sep+"dbmunreg " + getDatabase().getName(), null , ProcessOutput);
		  oProcess.exec();
		  if (oProcess.getExitValue() != 0) {
			  addMessage("dbmunreg does not work!", TestProtocol.ERROR,
					  "Output: "+ProcessOutput.toString().trim()+"\n("+__FILE__()+","+ __LINE__()+")");
		  } // end if
    	
		  // 5. Use dbmreg  (negative testing)
		  ProcessOutput = new StringBuffer();
		  oProcess = new Execute(this.getDBRoot()+Sep+"pgm"+Sep+"dbmreg " + getDatabase().getName(), null , ProcessOutput);
		  oProcess.exec();
		  if (oProcess.getExitValue() != 0) {
			  addMessage("dbmreg does not work!", TestProtocol.ERROR,
					  "Output: "+ProcessOutput.toString().trim()+"\n("+__FILE__()+","+ __LINE__()+")");
		  } // end if
		  
		  // 6. Use dbmstart again
		  ProcessOutput = new StringBuffer();
		  oProcess = new Execute(this.getDBRoot()+Sep+"pgm"+Sep+"dbmstart " + getDatabase().getName(), null , ProcessOutput);
		  oProcess.exec();
		  
		  if (oProcess.getExitValue() != 0) {
			  addMessage("dbmstart does not work!", TestProtocol.ERROR,
					  "Output: "+ProcessOutput.toString().trim()+"\n("+__FILE__()+","+ __LINE__()+")");
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

} // end class DBMSpecial
