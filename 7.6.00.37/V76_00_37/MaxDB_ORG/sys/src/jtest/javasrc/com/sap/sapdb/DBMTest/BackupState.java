package com.sap.sapdb.DBMTest;
/*!
  -----------------------------------------------------------------------------
  module: BackupState.java
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
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.TreeMap;

import com.sap.sapdb.DBMTest.util.*;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * DBMServer
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        BackupState
 * -----------------------------------------------------------------------------
 * responsible:  ChristianH
 * special area: DBMServer Java Test
 * description:  DBMServer medium_get command tests
 * version:      7.4.*
 * Copyright (c) 2002-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */

public class BackupState extends MediumPut
{
  public static final String tableName = "DBMTEMP";
  public static final String REP_RETURNCODE = "Returncode";
  public static final String REP_DATABASE = "Database";
  public static final String REP_MEDIANAME = "Medianame";
  public static final String REP_LOCATION = "Location";
  public static final String REP_ERRORTEXT = "Errortext";
  public static final String REP_SERVER = "Server";
  public static final String REP_DEVICESUSED = "Devices Used";

  private TreeMap tmReply = null;

  public BackupState()  throws SQLException, TestDatabaseException
  {
          super();
  }

  /**
   * return a connection
   */
  public Connection getConnection(String dbName,
                                  String nodeName,
								  int userPort,
                                  String userName,
                                  String userPass) throws Exception
  {
         String node = "localhost";
         String msg = "Driver com.sap.dbtech.jdbc.DriverSapDB not found.";
         Connection connection = null;
         if ((nodeName != null) && (!nodeName.equals("")))
         {
             node = nodeName;
         }
         try 
         {
             Class.forName ("com.sap.dbtech.jdbc.DriverSapDB");
             msg = "Unable to connect to jdbc:sapdb://"+ node+ ":"+ userPort + "/"+ dbName+"(user="+userName+" pass="+userPass+")";
             connection = DriverManager.getConnection("jdbc:sapdb://"+ node+ ":"+ userPort + "/"+ dbName,userName, userPass);
             return connection;
         }
         catch (Exception e)
         {
             throw new SQLException(msg);
         }
     }

  /**
   * Executes query sqlTxt using connection. In case any exception throws DGException.
   */
  public void executeQuery(Connection connection, String sqlTxt) throws Exception
  {
      PreparedStatement ps = null;
      int row = 0;
      try
      {
          ps = connection.prepareStatement(sqlTxt);
          //System.out.println(sqlTxt);
          row = ps.executeUpdate();
      }
      catch (Exception e)
      {
          closePrepStatement(ps);
          throw new Exception("executeQuery");
      }
  }

  /**
   * Executes query sqlTxt using connection. In case any exception throws DGException.
   */
  public String getOneStringValue(Connection connection,String sqlTxt) throws Exception
  {
      PreparedStatement ps = null;
      ResultSet rs = null;
      String ret = "0";
      try
      {
          ps = connection.prepareStatement(sqlTxt);
          rs = ps.executeQuery();
          if (rs.next())
          {
             if (rs.getObject(1)!=null)
             {
                ret = rs.getObject(1).toString();
             }
             //System.out.println(ret);
          }
          closeResultSet(rs);
          closePrepStatement(ps);
          return ret;
      }
      catch (Exception e)
      {
          closeResultSet(rs);
          closePrepStatement(ps);
          throw new Exception("getOneStringValue");
      }
  }
      /**
     * Closes PreparedStatement ps.
     */
    private void closePrepStatement(PreparedStatement ps)
    {
        try
        {
            ps.close();
        }
        catch (Exception e)
        {
          // do nothing
        }
    }

    /**
     * Closes ResultSet rs.
     */
    private void closeResultSet(ResultSet rs)
    {
        try
        {
            rs.close();
        }
        catch (Exception e)
        {
          // do nothing
        }
    }

  /**
   * Fill db with data
   */
   public void fillDatabase(DBMUtility dbm) throws Exception
   {
      //check if table exists
      Connection conn = null;
      try
      {
         conn = getConnection(getDatabase().getName(),
                              getDatabase().getHost(),
                              getDatabase().getPort(),
                              getDatabase().getDBAUser(),
                              getDatabase().getDBAPassword());
         //check if the table exists

         for(int n = 1;n < 3;n++) {

           String ret = getOneStringValue(conn, "select count(*) from tables where tablename='"+tableName+"_"+n+"'");

           if (!ret.equals("0"))
           {
              executeQuery(conn,"drop table "+tableName+"_"+n);
           }
           executeQuery(conn,"create table "+tableName+"_"+n+" as select * from MESSAGES");
           executeQuery(conn,"insert into "+tableName+"_"+n+" select * from MESSAGES");

           for (int i = 0; i < 6; i++) {
              executeQuery(conn,"insert into "+tableName+"_"+n+" select * from "+tableName+"_"+n);
           }
           //System.out.println("n = " + n);
         }

         conn.close();
      }
      catch (Exception e) {
          try {
             conn.close();
          }
          catch (Exception e1) {
            // do nothing
          }
          throw new Exception(e.getMessage());
      }
   }
  /**
   * run() contains the test implementation
   */
  public void run() {

    DBMUtility oDBM = new  DBMUtility(getDatabase(), this);
    //create message table for checking DBM replies
    MessageTable msgTab = new MessageTable();
    String rep = null;
//    String outPath = getDataOutputPath();
    String outPath = getTestClassId();

    //  create one global try-catch block to handle exceptions
    try {

           //Without starting backup
           oDBM.execCmd("backup_state",DBMUtility.OK_OK);
           msgTab.setMessage(oDBM.getAnswer());
           checkValue(msgTab,REP_RETURNCODE,"0");

           //With starting backup
           //Start backup
           oDBM.execCmd("medium_put datamedium "+outPath+"1 file data 32 8 yes",DBMUtility.OK_OK);
           //fill the db with values
           fillDatabase(oDBM);
           //backup start
           oDBM.execCmd("util_connect",DBMUtility.OK_OK);
           oDBM.execCmd("backup_start datamedium",DBMUtility.OK_OK);

           //backup_state
           oDBM.execCmd("backup_state",DBMUtility.OK_OK);
           //check reply
           msgTab.setMessage(oDBM.getAnswer());
           checkValue(msgTab,REP_RETURNCODE,"0");
           checkValue(msgTab,REP_DATABASE,getDatabase().getName());
           checkValue(msgTab,REP_DEVICESUSED,"1");

           oDBM.execCmd("util_release",DBMUtility.OK_OK);
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
    return "BackupState";
  } // end getTestClassId

   /**
    * checks reply value
    */
    public boolean checkValue(MessageTable msgTab,String name, String value) {
        for (int i=0; i<msgTab.getRowCount();i++) {

            if (msgTab.getCell(i,0).startsWith(name)&&msgTab.getCell(i,1).startsWith(value)) {
                return true;
            }
        }
        this.addMessage("string "+name+" "+value+" didn't found in reply.", TestProtocol.ERROR , msgTab.getMessage());
        return false;
    }


} //end of class BackupState