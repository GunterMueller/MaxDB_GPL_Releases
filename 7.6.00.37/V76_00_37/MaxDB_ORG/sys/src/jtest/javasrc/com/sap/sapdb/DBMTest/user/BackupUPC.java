package com.sap.sapdb.DBMTest.user;

/*!
  -----------------------------------------------------------------------------
  module: BackupUPC.java
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
import java.io.File;
import java.sql.SQLException;
import java.util.Calendar;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCaseException;


/**
 * DBMServer test example
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        BackupUPC
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  DBMServer test backup and restore of the UPC file
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class BackupUPC extends DBMTestCase
{
	
	long m_LastModificationTime; 

  /**
   * A minimal constructor.Constructor.
   */
  public BackupUPC() throws SQLException, TestDatabaseException
  {
    super();
      
    // calculate last modification time
    Calendar oldDate = Calendar.getInstance();
    oldDate.clear();
    oldDate.set(105+1900, 0, 1); // January 1st, 2005
    m_LastModificationTime = oldDate.getTime().getTime();
  } // end BackupUPC

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
    return "BackupUPC";
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
      TestDatabaseSap sapdb = (TestDatabaseSap) getDatabase();
    	
      File oUPC_original = new File(sapdb.getIndepDataPath()+"/config/" + sapdb.getName() + ".upc");
      File oUPC_renamed = new File(sapdb.getIndepDataPath()+"/config/" + sapdb.getName() + ".upc.org");
      File oUPC_copy = new File(sapdb.getRunDir() + "/dbm.upc");
      
      // create the datbase user
      m_DBM.execCmd("sql_execute user-type=DBA create user tilly password willy dba not exclusive", DBMUtility.OK_OK);
      
      // save as SAP user
      m_DBM.execCmd("user_sap tilly,willy", DBMUtility.OK_OK);

      // check SAP user
      m_DBM.execCmd("sql_execute user-type=SAP select user from dual", DBMUtility.OK_OK);
      
      // delete config/<dbname>.upc
      if (!oUPC_original.exists()) {
        addMessage(getTestClassId(), TestProtocol.ERROR, oUPC_original.getAbsolutePath() + " does not exists!"); 
      } else {
        oUPC_original.renameTo(oUPC_renamed);
      } // end if
      
      // check SAP user (should be successfull because of backup copy)
      m_DBM.execCmd("user_logon " + getDatabase().getDBMUser() + "," + getDatabase().getDBMPassword(), DBMUtility.OK_OK);
      m_DBM.execCmd("sql_execute user-type=SAP select user from dual", DBMUtility.OK_OK);
      
      // delete config/<dbname>.upc and <rundir>/dbm.upc
      if (!oUPC_original.exists()) {
        addMessage(getTestClassId(), TestProtocol.ERROR, oUPC_original.getAbsolutePath() + " does not exists!"); 
      } else {
      	oUPC_original.delete();
      } // end if
      if (!oUPC_copy.exists()) {
        addMessage(getTestClassId(), TestProtocol.ERROR, oUPC_copy.getAbsolutePath() + " does not exists!"); 
      } else {
      	oUPC_copy.delete();
      } // end if
    
      // rebuild UPC from fallback authorization
      m_DBM.execCmd("user_logon " + getDatabase().getDBMUser() + "," + getDatabase().getDBMPassword(), DBMUtility.OK_OK);
	  
	  // check load_systab
      m_DBM.execCmd("load_systab", DBMUtility.ERR_DBAUNKNOWN);
      m_DBM.execCmd("load_systab -u " +  getDatabase().getDBAUser() + "," + getDatabase().getDBAPassword(), DBMUtility.OK_OK);
      m_DBM.execCmd("load_systab", DBMUtility.OK_OK);

      // check SAP user
      m_DBM.execCmd("sql_execute user-type=SAP select user from dual", DBMUtility.ERR_USRREAD);
      m_DBM.execCmd("user_sap tilly,willy", DBMUtility.OK_OK);
      m_DBM.execCmd("sql_execute user-type=SAP select user from dual", DBMUtility.OK_OK);

      // touch  <rundir>/dbm.upc
      if (!oUPC_copy.exists()) {
        addMessage(getTestClassId(), TestProtocol.ERROR, oUPC_copy.getAbsolutePath() + " does not exists!"); 
      } else {
      	oUPC_copy.setLastModified(m_LastModificationTime);
      } // end if
      
      // authorization error
      m_DBM.execCmd("user_logon x,x", DBMUtility.ERR_USRFAIL);
      
      // check timestamp of config/<dbname>.upc
      if (!oUPC_original.exists()) {
        addMessage(getTestClassId(), TestProtocol.ERROR, oUPC_original.getAbsolutePath() + " does not exists!"); 
      } else {
        if (oUPC_original.lastModified() == m_LastModificationTime) {
            addMessage(getTestClassId(), TestProtocol.ERROR, oUPC_original.getAbsolutePath() + " has date " + oUPC_original.lastModified());
        } // end if
      } // end if
      
      // reset     
      m_DBM.execCmd("user_logon " + getDatabase().getDBMUser() + "," + getDatabase().getDBMPassword(), DBMUtility.OK_OK);
      m_DBM.execCmd("user_delete tilly", DBMUtility.OK_OK);
      m_DBM.execCmd("sql_execute user-type=DBA drop user tilly", DBMUtility.OK_OK);
      
      oUPC_renamed.renameTo(oUPC_original);
    
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

} // end class BackupUPC
