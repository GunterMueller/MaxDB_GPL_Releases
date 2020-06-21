package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: SQLRecreateIndexSingle.java
  -----------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.testcase.TestCase;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        SQLRecreateIndexSingle
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer sql_recreateindex command tests
 * version:      7.4.*
 * Copyright (c) 2000-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class SQLRecreateIndexSingle extends TestCase
{
  public SQLRecreateIndexSingle()  throws SQLException, TestDatabaseException
  {
    super();
  }
  /**
   * run() contains the test implementation
   */
  public void run()
  {
    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);

    //  create one global try-catch block to handle exceptions
    try
    {
      // 1. Creating the bad indexes for sysdba
      //SQL Connect
      oDBM.execCmd("db_connect " + getUser() + "," + getPassword(),DBMUtility.OK_OK);
      //Creating new table
      oDBM.execCmd("db_execute create table BADIDXTEST (A fixed(10), B char(10), C char(10))",DBMUtility.OK_OK);
      //Insert values
      oDBM.execCmd("db_execute insert BADIDXTEST values( 1, 'a', 'a' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert BADIDXTEST values( 2, 'b', 'aa' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert BADIDXTEST values( 3, 'c', 'aaa' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert BADIDXTEST values( 4, 'd', 'aaaa' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert BADIDXTEST values( 5, 'e', 'aaaaa' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert BADIDXTEST values( 6, 'f', 'aaaaaa' )",DBMUtility.OK_OK);
      //Creating the indexes
      oDBM.execCmd("db_execute create index BADIDXTEST1 on BADIDXTEST ( A desc )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute create index BADIDXTEST2 on BADIDXTEST ( A asc, C desc )",DBMUtility.OK_OK);

      // make index bad
      oDBM.execCmd("db_execute diagnose alter index BADIDXTEST1  on BADIDXTEST bad",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute diagnose alter index BADIDXTEST2  on BADIDXTEST bad",DBMUtility.OK_OK);

      // create test user
      oDBM.execCmd("db_execute create user sapr3 password sap dba",DBMUtility.OK_OK);
      // create test schema 
      oDBM.execCmd("db_execute create schema DBMTEST",DBMUtility.OK_OK);

      //SQL Release
      oDBM.execCmd("db_release",DBMUtility.OK_OK);

      // 2. Creating the bad indexes for sapr3
      //SQL Connect
      oDBM.execCmd("db_connect sapr3,sap",DBMUtility.OK_OK);
      //Creating new table
      oDBM.execCmd("db_execute create table BADIDXTEST (A fixed(10), B char(10), C char(10))",DBMUtility.OK_OK);
      //Insert values
      oDBM.execCmd("db_execute insert BADIDXTEST values( 1, 'a', 'a' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert BADIDXTEST values( 2, 'b', 'aa' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert BADIDXTEST values( 3, 'c', 'aaa' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert BADIDXTEST values( 4, 'd', 'aaaa' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert BADIDXTEST values( 5, 'e', 'aaaaa' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert BADIDXTEST values( 6, 'f', 'aaaaaa' )",DBMUtility.OK_OK);
      //Creating the indexes
      oDBM.execCmd("db_execute create index BADIDXTEST1 on BADIDXTEST ( A desc )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute create index BADIDXTEST2 on BADIDXTEST ( A asc, C desc )",DBMUtility.OK_OK);

      // make index bad
      oDBM.execCmd("db_execute diagnose alter index BADIDXTEST1  on BADIDXTEST bad",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute diagnose alter index BADIDXTEST2  on BADIDXTEST bad",DBMUtility.OK_OK);

      //SQL Release
      oDBM.execCmd("db_release",DBMUtility.OK_OK);

      // 3. Creating the bad indexes for dbmtest
      //SQL Connect
      oDBM.execCmd("db_connect " + getUser() + "," + getPassword(),DBMUtility.OK_OK);
      //Creating new table
      oDBM.execCmd("db_execute create table DBMTEST.BADIDXTEST (A fixed(10), B char(10), C char(10))",DBMUtility.OK_OK);
      //Insert values
      oDBM.execCmd("db_execute insert DBMTEST.BADIDXTEST values( 1, 'a', 'a' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert DBMTEST.BADIDXTEST values( 2, 'b', 'aa' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert DBMTEST.BADIDXTEST values( 3, 'c', 'aaa' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert DBMTEST.BADIDXTEST values( 4, 'd', 'aaaa' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert DBMTEST.BADIDXTEST values( 5, 'e', 'aaaaa' )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute insert DBMTEST.BADIDXTEST values( 6, 'f', 'aaaaaa' )",DBMUtility.OK_OK);
      //Creating the indexes
      oDBM.execCmd("db_execute create index BADIDXTEST1 on DBMTEST.BADIDXTEST ( A desc )",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute create index BADIDXTEST2 on DBMTEST.BADIDXTEST ( A asc, C desc )",DBMUtility.OK_OK);

      // make index bad
      oDBM.execCmd("db_execute diagnose alter index BADIDXTEST1  on DBMTEST.BADIDXTEST bad",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute diagnose alter index BADIDXTEST2  on DBMTEST.BADIDXTEST bad",DBMUtility.OK_OK);

      //SQL Release
      oDBM.execCmd("db_release",DBMUtility.OK_OK);

      // 1. recreate index for sysdba
      // check for bad indexes
      oDBM.execCmd("db_execute select * from SYSDBM.INFO_BAD_INDEXES",DBMUtility.OK_OK);

      // Recreate bad indexes - explicit
      oDBM.execCmd("sql_recreateindex "+getUser()+".BADIDXTEST.BADIDXTEST1",DBMUtility.OK_OK);
		 
      // check for bad indexes
      oDBM.execCmd("db_execute SELECT * FROM SYSDBM.INFO_BAD_INDEXES",DBMUtility.OK_OK);

      // Recreate bad indexes - explicit
      oDBM.execCmd("sql_recreateindex "+getUser()+".BADIDXTEST.BADIDXTEST2",DBMUtility.OK_OK);

      // check for bad indexes
      oDBM.execCmd("db_execute select * from SYSDBM.INFO_BAD_INDEXES",DBMUtility.OK_OK);

      // check for existence of index
      oDBM.execCmd("db_execute select * from DOMAIN.INDEXES where SCHEMANAME='"+getUser()+"' and OWNER='"+getUser()+"' and TABLENAME='BADIDXTEST' and INDEXNAME like 'BADIDXTEST%'",DBMUtility.OK_OK);

      // 2. recreate index for sapr3
      // check for bad indexes
      oDBM.execCmd("db_execute select * from SYSDBM.INFO_BAD_INDEXES",DBMUtility.OK_OK);

      //Recreate bad indexes - explicit
      oDBM.execCmd("sql_recreateindex SAPR3.BADIDXTEST.BADIDXTEST1",DBMUtility.OK_OK);

      // check for bad indexes
      oDBM.execCmd("db_execute SELECT * FROM SYSDBM.INFO_BAD_INDEXES",DBMUtility.OK_OK);

      //Recreate bad indexes - explicit
      oDBM.execCmd("sql_recreateindex SAPR3.BADIDXTEST.BADIDXTEST2",DBMUtility.OK_OK);

      // check for bad indexes
      oDBM.execCmd("db_execute select * from SYSDBM.INFO_BAD_INDEXES",DBMUtility.OK_OK);

      // check for existence of index
      oDBM.execCmd("db_execute select * from DOMAIN.INDEXES where SCHEMANAME='SAPR3' and OWNER='SAPR3' and TABLENAME='BADIDXTEST' and INDEXNAME like 'BADIDXTEST%'",DBMUtility.OK_OK);

      // 3. recreate index for DBMTEST
      // check for bad indexes
      oDBM.execCmd("db_execute select * from SYSDBM.INFO_BAD_INDEXES",DBMUtility.OK_OK);

      //Recreate bad indexes - explicit
      oDBM.execCmd("sql_recreateindex DBMTEST.BADIDXTEST.BADIDXTEST1",DBMUtility.OK_OK);

      // check for bad indexes
      oDBM.execCmd("db_execute SELECT * FROM SYSDBM.INFO_BAD_INDEXES",DBMUtility.OK_OK);

      //Recreate bad indexes - explicit
      oDBM.execCmd("sql_recreateindex DBMTEST.BADIDXTEST.BADIDXTEST2",DBMUtility.OK_OK);

      // check for bad indexes
      oDBM.execCmd("db_execute select * from SYSDBM.INFO_BAD_INDEXES",DBMUtility.ERR_SQL);

      // check for existence of index
      oDBM.execCmd("db_execute select * from DOMAIN.INDEXES where SCHEMANAME='DBMTEST' and OWNER='"+getUser()+"' and TABLENAME='BADIDXTEST' and INDEXNAME like 'BADIDXTEST%'",DBMUtility.OK_OK);
      
      
      //Droping existing table
      oDBM.execCmd("db_connect " + getUser() + "," + getPassword(),DBMUtility.OK_OK);
      oDBM.execCmd("db_execute drop schema DBMTEST",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute drop user sapr3",DBMUtility.OK_OK);
      oDBM.execCmd("db_execute drop table BADIDXTEST",DBMUtility.OK_OK);
      oDBM.execCmd("db_release",DBMUtility.OK_OK);
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

  /**
   * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
   * will set the name to the name of the java class. To make the test name independend
   * from the java implementation, this method should return a unique string.
   * Attention: The uniqueness is not automatically checked by anybody.
   * @return The UNIQUE name of the test class
   */
  public static String getTestClassId() {
    return "SQLRecreateIndexSingle";
  } // end getTestClassId

} //end of class SQLRecreateIndexSingle
