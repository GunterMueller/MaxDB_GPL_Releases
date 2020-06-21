package com.sap.sapdb.DBMTest;

/*!
  -----------------------------------------------------------------------------
  module: DBMTrace.java
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

import com.sap.sapdb.DBMTest.util.*;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCase;
import com.sap.sapdb.testframe.testcase.TestCaseException;

/**
 * DBMServer trace test
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBMTrace
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  checks the trace commands of the DBMServer
 * version:      7.2.*
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class DBMTrace extends TestCase {

  /**
   * A minimal constructor.Constructor.
   */
  public DBMTrace() throws SQLException, TestDatabaseException {
          super();
  } // end DBMTrace

  /**
   * Clean up test, i.e. delete data created in prepare, ...
   * @exception TestCaseException Should be thrown, when cleanUp fails
   */
  public static void cleanUp() throws TestCaseException {
    // do nothing
  } // end cleanUp

  /**
   * Returns (defines) the name of the test class. If this method doesn't exist, the test driver
   * will set the name to the name of the java class. To make the test name independend
   * from the java implementation, this method should return a unique string.
   * Attention: The uniqueness is not automatically checked by anybody.
   * @return The UNIQUE name of the test class
   */
  public static String getTestClassId() {
    return "DBMTrace";
  } // end getTestClassId

  /**
   * Sets the timeout value in milliseconds. After the timeout is reached, the test will be aborted
   * by the test driver.
   * @return The default return value is 0, which means there is no timeout value set. Test will never
   * be aborted by the test driver.
   */
  public static long getTimeout() {
    return 0;
  } // end getTimeout

  /**
   * Sets weather test can create and work with verification data or not.
   * @return The default return value is false, which means that no verification data
   * is used to verify the test results. In this case the test has to implement the verification
   * itself.
   */
  public static boolean isReferenceEnabled() {
    return false;
  } // end isReferenceEnabled

  /**
   * Sets weather test can be executed only on SAPDB or not.
   * @return The default return value is true. This means, the test can run on SAPDB only.
   */
  public static boolean isSAPOnly() {
          return true;
  } // end isSAPOnly

  /**
   * Prepare test, i.e. create users, tables and data
   * @exception TestCaseException Should be thrown, when preparation fails
   */
  public static void prepare() throws TestCaseException {
    // do nothing
  } // end prepare

  /**
   * run() contains the test implementation
   */
  public void run() {

    DBMUtility     oDBM    = new  DBMUtility(getDatabase(), this);
    DBMAnswerTable oAnswer = new DBMAnswerTable();
    String         sTemp;

    //  create one global try-catch block to handle exceptions
    try {

      // *** 1. Create a defined trace statev ***

      // switch db_state to warm
      oDBM.execCmd("db_online", DBMUtility.OK_OK);

      // switch all trace off
      oDBM.execCmd("trace_off all", DBMUtility.OK_OK);

      // check trace state (all OFF?)
      oDBM.execCmd("trace_show", DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(oDBM.getAnswer());
      // find a line with value ON
      if (oAnswer.getField("ON", "state", "state").length() > 0) {
        // error there are rows with "ON" in state
        addMessage("trace_off all", TestProtocol.ERROR , "Could not switch off all traces!");
        return;
      } // end if

      // *** 2. Check all trace options ***

      // on
      oDBM.execCmd("trace_on TRACE_AK"                        , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_DEFAULT"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_DELETE"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_INDEX"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_INSERT"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_LOCK"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_LONG"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OBJECT"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OBJECT_ADD"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OBJECT_ALTER"              , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OBJECT_FREE"               , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OBJECT_GET"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OPTIMIZE"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_ORDER"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_ORDER_STANDARD"            , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_PAGES"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_PRIMARY_TREE"              , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_SELECT"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_TIME"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_UPDATE"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_STOP_ERRORCODE"            , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_ALLOCATOR"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_CATALOG"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_CLIENTKERNELCOM"           , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_COMMON"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_COMMUNICATION"             , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_CONVERTER"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_DATACHAIN"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_DATAPAM"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_DATATREE"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_DBPROC"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_FBM"                       , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_FILEDIR"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_FRAMECTRL"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_IOMAN"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_IPC"                       , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_JOIN"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_KSQL"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_LOGACTION"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_LOGHISTORY"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_LOGPAGE"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_LOGTRANS"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_LOGVOLUME"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_MEMORY"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OBJECTCONTAINER"           , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OMS_CONTAINERDIR"          , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OMS_CONTEXT"               , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OMS_ERROR"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OMS_FLUSHCACHE"            , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OMS_INTERFACE"             , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OMS_KEY"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OMS_KEYRANGE"              , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OMS_LOCK"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OMS_MEMORY"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OMS_NEWOBJ"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OMS_SESSION"               , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OMS_STREAM"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_OMS_VAROBJECT"             , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_PAGER"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_RUNTIME"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_SHAREDSQL"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_SQLMANAGER"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_SRVTASKS"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_SYNCHRONISATION"           , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_SYSVIEW"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_TABLE"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_VOLUME"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_BACKUP"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_DATACACHE"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_KB_REGIONS"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_LOCK"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_LOCK_SUPPLY"               , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_REGIONS"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_TASK_SPECIFIC_CATALOGCACHE", DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_TRANSLIST"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_TREE"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_TREE_LOCKS"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_COMMON"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_CONVERTER"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_DATAPAGELOG"               , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_FBM"                       , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_IOMAN"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_LOGHISTORY"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_LOGPAGE"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_LOGTRANS"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_LOGVOLUME"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_SRVTASKS"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_QUERYREWRITE"              , DBMUtility.OK_OK);

      // off
      oDBM.execCmd("trace_off TRACE_AK"                        , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_DEFAULT"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_DELETE"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_INDEX"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_INSERT"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_LOCK"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_LONG"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OBJECT"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OBJECT_ADD"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OBJECT_ALTER"              , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OBJECT_FREE"               , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OBJECT_GET"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OPTIMIZE"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_ORDER"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_ORDER_STANDARD"            , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_PAGES"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_PRIMARY_TREE"              , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_SELECT"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_TIME"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_UPDATE"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_STOP_ERRORCODE"            , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_ALLOCATOR"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_CATALOG"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_CLIENTKERNELCOM"           , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_COMMON"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_COMMUNICATION"             , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_CONVERTER"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_DATACHAIN"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_DATAPAM"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_DATATREE"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_DBPROC"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_FBM"                       , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_FILEDIR"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_FRAMECTRL"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_IOMAN"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_IPC"                       , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_JOIN"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_KSQL"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_LOGACTION"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_LOGHISTORY"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_LOGPAGE"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_LOGTRANS"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_LOGVOLUME"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_MEMORY"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OBJECTCONTAINER"           , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OMS_CONTAINERDIR"          , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OMS_CONTEXT"               , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OMS_ERROR"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OMS_FLUSHCACHE"            , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OMS_INTERFACE"             , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OMS_KEY"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OMS_KEYRANGE"              , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OMS_LOCK"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OMS_MEMORY"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OMS_NEWOBJ"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OMS_SESSION"               , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OMS_STREAM"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_OMS_VAROBJECT"             , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_PAGER"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_RUNTIME"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_SHAREDSQL"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_SQLMANAGER"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_SRVTASKS"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_SYNCHRONISATION"           , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_SYSVIEW"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_TABLE"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_VOLUME"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_BACKUP"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_DATACACHE"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_KB_REGIONS"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_LOCK"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_LOCK_SUPPLY"               , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_REGIONS"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_TASK_SPECIFIC_CATALOGCACHE", DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_TRANSLIST"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_TREE"                      , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_TREE_LOCKS"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_COMMON"                    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_CONVERTER"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_DATAPAGELOG"               , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_FBM"                       , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_IOMAN"                     , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_LOGHISTORY"                , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_LOGPAGE"                   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_LOGTRANS"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_LOGVOLUME"                 , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off CHECK_SRVTASKS"                  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off TRACE_QUERYREWRITE"              , DBMUtility.OK_OK);

      // all option
      oDBM.execCmd("trace_on  all"    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off all"    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on  all 7"  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off all"    , DBMUtility.OK_OK);

      // check trace state (all OFF?)
      oDBM.execCmd("trace_show", DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(oDBM.getAnswer());
      // find a line with value ON
      if (oAnswer.getField("ON", "state", "state").length() > 0) {
        // error there are rows with "ON" in state
        addMessage("trace_off all", TestProtocol.ERROR , "There are values on: " + oAnswer.getField("ON", "state", "Name"));
        return;
      } // end if

      // *** 3. Check the restart shutdown behaviour ***

      // switch some on
      oDBM.execCmd("trace_on TRACE_INDEX"        , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_DELETE"       , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_ALLOCATOR 3"  , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on CHECK_CONVERTER 4"  , DBMUtility.OK_OK);
 
      // check trace state
      oDBM.execCmd("trace_show", DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(oDBM.getAnswer());
      if (oAnswer.getField("TRACE_INDEX", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_INDEX", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_LOCK", "name", "state").compareTo("OFF") != 0) {
        addMessage("TRACE_LOCK", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_DELETE", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_DELETE", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_ALLOCATOR", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_ALLOCATOR", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_ALLOCATOR", "name", "level").compareTo("3") != 0) {
        addMessage("TRACE_ALLOCATOR", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("CHECK_CONVERTER", "name", "state").compareTo("ON") != 0) {
        addMessage("CHECK_CONVERTER", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("CHECK_CONVERTER", "name", "level").compareTo("4") != 0) {
        addMessage("CHECK_CONVERTER", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if

      // state to offline
      oDBM.execCmd("db_offline" , DBMUtility.OK_OK);

      // check trace state
      oDBM.execCmd("trace_show", DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(oDBM.getAnswer());
      // "TRACE_INDEX"
      if (oAnswer.getField("TRACE_INDEX", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_INDEX", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_LOCK", "name", "state").compareTo("OFF") != 0) {
        addMessage("TRACE_LOCK", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_DELETE", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_DELETE", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_ALLOCATOR", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_ALLOCATOR", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_ALLOCATOR", "name", "level").compareTo("3") != 0) {
        addMessage("TRACE_ALLOCATOR", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("CHECK_CONVERTER", "name", "state").compareTo("ON") != 0) {
        addMessage("CHECK_CONVERTER", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("CHECK_CONVERTER", "name", "level").compareTo("4") != 0) {
        addMessage("CHECK_CONVERTER", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if

      // set in offline
      oDBM.execCmd("trace_on TRACE_LOCK" , DBMUtility.OK_OK);

      // check trace state
      oDBM.execCmd("trace_show", DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(oDBM.getAnswer());
      // "TRACE_INDEX"
      if (oAnswer.getField("TRACE_INDEX", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_INDEX", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_LOCK", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_LOCK", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_DELETE", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_DELETE", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_ALLOCATOR", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_ALLOCATOR", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_ALLOCATOR", "name", "level").compareTo("3") != 0) {
        addMessage("TRACE_ALLOCATOR", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("CHECK_CONVERTER", "name", "state").compareTo("ON") != 0) {
        addMessage("CHECK_CONVERTER", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("CHECK_CONVERTER", "name", "level").compareTo("4") != 0) {
        addMessage("CHECK_CONVERTER", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      oDBM.execCmd("trace_off TRACE_LOCK" , DBMUtility.OK_OK);

      // state to cold
      oDBM.execCmd("db_cold" , DBMUtility.OK_OK);

      // check trace state
      oDBM.execCmd("trace_show", DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(oDBM.getAnswer());
      // "TRACE_INDEX"
      if (oAnswer.getField("TRACE_INDEX", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_INDEX", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_LOCK", "name", "state").compareTo("OFF") != 0) {
        addMessage("TRACE_LOCK", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_DELETE", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_DELETE", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_ALLOCATOR", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_ALLOCATOR", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_ALLOCATOR", "name", "level").compareTo("3") != 0) {
        addMessage("TRACE_ALLOCATOR", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("CHECK_CONVERTER", "name", "state").compareTo("ON") != 0) {
        addMessage("CHECK_CONVERTER", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("CHECK_CONVERTER", "name", "level").compareTo("4") != 0) {
        addMessage("CHECK_CONVERTER", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if

      // change in cold mode
      oDBM.execCmd("trace_on LOCK"   , DBMUtility.OK_OK);
      oDBM.execCmd("trace_off INDEX" , DBMUtility.OK_OK);

      // check trace state
      oDBM.execCmd("trace_show", DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(oDBM.getAnswer());
      // "TRACE_INDEX"
      if (oAnswer.getField("TRACE_INDEX", "name", "state").compareTo("OFF") != 0) {
        addMessage("TRACE_INDEX", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_LOCK", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_LOCK", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_DELETE", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_DELETE", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_ALLOCATOR", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_ALLOCATOR", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_ALLOCATOR", "name", "level").compareTo("3") != 0) {
        addMessage("TRACE_ALLOCATOR", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("CHECK_CONVERTER", "name", "state").compareTo("ON") != 0) {
        addMessage("CHECK_CONVERTER", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("CHECK_CONVERTER", "name", "level").compareTo("4") != 0) {
        addMessage("CHECK_CONVERTER", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_LOCK", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_LOCK", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if

      // state to warm
      oDBM.execCmd("db_online" , DBMUtility.OK_OK);

      // check trace state
      oDBM.execCmd("trace_show", DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(oDBM.getAnswer());
      // "TRACE_INDEX"
      if (oAnswer.getField("TRACE_INDEX", "name", "state").compareTo("OFF") != 0) {
        addMessage("TRACE_INDEX", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_LOCK", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_LOCK", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_DELETE", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_DELETE", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_ALLOCATOR", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_ALLOCATOR", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_ALLOCATOR", "name", "level").compareTo("3") != 0) {
        addMessage("TRACE_ALLOCATOR", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("CHECK_CONVERTER", "name", "state").compareTo("ON") != 0) {
        addMessage("CHECK_CONVERTER", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("CHECK_CONVERTER", "name", "level").compareTo("4") != 0) {
        addMessage("CHECK_CONVERTER", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if
      if (oAnswer.getField("TRACE_LOCK", "name", "state").compareTo("ON") != 0) {
        addMessage("TRACE_LOCK", TestProtocol.ERROR , "Wrong trace state");
        return;
      } // end if

      // *** 4. switch all to off again ***

      oDBM.execCmd("trace_off all"    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_show", DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(oDBM.getAnswer());
      // find a line with value ON
      if (oAnswer.getField("ON", "state", "state").length() > 0) {
        // error there are rows with "ON" in state
        addMessage("trace_off all", TestProtocol.ERROR , "There are values on: " + oAnswer.getField("ON", "state", "Name"));
        return;
      } // end if


      // *** 5. create a trace file

      // empty trace in warm mode
      oDBM.execCmd("trace_clear"             , DBMUtility.OK_OK);
      oDBM.execCmd("trace_flush"             , DBMUtility.OK_OK);
      oDBM.execCmd("trace_prot a"            , DBMUtility.OK_OK);
      oDBM.execCmd("file_getfirst KNLTRCPRT" , DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(oDBM.getAnswer());
      if (oAnswer.getRowCount() > 20) {
        // error there are rows with "ON" in state
        addMessage("trace_prot", TestProtocol.ERROR , "Trace not empty");
        return;
      } // end if

      // filled trace in warm mode
      oDBM.execCmd("trace_clear"             , DBMUtility.OK_OK);
      oDBM.execCmd("trace_on TRACE_DEFAULT"  , DBMUtility.OK_OK);
      oDBM.execCmd("sql_execute select * from users", DBMUtility.OK_OK);
      oDBM.execCmd("trace_flush"             , DBMUtility.OK_OK);
      oDBM.execCmd("trace_prot a"            , DBMUtility.OK_OK);
      oDBM.execCmd("file_getfirst KNLTRCPRT" , DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(oDBM.getAnswer());
      if (oAnswer.getRowCount() < 20) {
        // error there are rows with "ON" in state
        addMessage("trace_prot", TestProtocol.ERROR , "Trace is empty");
        return;
      } // end if

      // *** 6. switch all to off again ***

      oDBM.execCmd("trace_off all"    , DBMUtility.OK_OK);
      oDBM.execCmd("trace_show", DBMUtility.OK_OK);
      oAnswer.analyzeFixColsByHeader(oDBM.getAnswer());
      // find a line with value ON
      if (oAnswer.getField("ON", "state", "state").length() > 0) {
        // error there are rows with "ON" in state
        addMessage("trace_off all", TestProtocol.ERROR , "There are values on: " + oAnswer.getField("ON", "state", "Name"));
        return;
      } // end if
      oDBM.execCmd("trace_clear"             , DBMUtility.OK_OK);
      oDBM.execCmd("trace_flush"             , DBMUtility.OK_OK);


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
      } // end try catch
    } // end finally
  } // end run

} // end class DBMTrace
