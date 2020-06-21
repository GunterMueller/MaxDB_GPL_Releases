/*!
	\file    DBFiller.java
	\author  TiloH
	\ingroup DBM Server tests
	\brief   class for inserting dummy data into a database
	
	========== licence begin  GPL
	Copyright (c) 2004-2005 SAP AG
	
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

package com.sap.sapdb.DBMTest.base;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import com.sap.sapdb.testframe.driver.TestDatabase;

public class DBFiller {

	public static final String tableName = "DBMTEMP";
	
	private static Connection getConnection(
							String dbName,
							String nodeName,
							int userPort,
							String userName,
							String userPass) throws Exception {
		
		String msg = "Driver com.sap.dbtech.jdbc.DriverSapDB not found.";
		Connection connection = null;
		
		if ((nodeName == null) || (nodeName.equals("")))
			nodeName="localhost";
		
		try {
			Class.forName ("com.sap.dbtech.jdbc.DriverSapDB");
			String service="jdbc:sapdb://"+ nodeName+ ":"+ userPort + "/"+ dbName;
			msg = "Unable to connect to "+service+" (user="+userName+" pass="+userPass+")";
			connection = DriverManager.getConnection(service,userName, userPass);
			return connection;
		}
		catch (Exception e)
		{
			throw new SQLException(msg);
		}
	}
	
	 /**
     * Closes PreparedStatement ps.
     */
    private static void closePrepStatement(PreparedStatement ps)
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
    private static void closeResultSet(ResultSet rs)
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
    
	private static String getOneStringValue(
					Connection connection,
					String sqlTxt) throws Exception {
		
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
	   * Executes query sqlTxt using connection. In case any exception throws DGException.
	   */
	private static void executeQuery(Connection connection, String sqlTxt) throws Exception
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
	
	public static void fill(
					TestDatabase db) throws Exception {
		
		//check if table exists
		Connection conn = null;
		
		try
		{
			conn=getConnection(
					db.getName(),
                    db.getHost(),
                    db.getPort(),
                    db.getDBAUser(),
                    db.getDBAPassword());
			
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
}
