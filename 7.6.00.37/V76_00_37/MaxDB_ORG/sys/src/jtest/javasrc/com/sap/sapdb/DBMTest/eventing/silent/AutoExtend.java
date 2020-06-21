/*
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
package com.sap.sapdb.DBMTest.eventing.silent;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Date;

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCaseException;
import com.sap.sapdb.testframe.testcase.TestPreparedStatement;
import com.sap.sapdb.testframe.testcase.TestStatement;

public class AutoExtend extends EventingSilentTestCase {

	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public AutoExtend() throws SQLException, TestDatabaseException {
		super();
	}

	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.base.DBMTestCase#runDBMTest()
	 */
	protected void runDBMTest() {	
		// initial status
		if( isDispatcherConfiguredToRun() ) {
            super.addMessage(super.getTestId(), TestProtocol.ERROR, "Dispatcher configured to run, but it should not");
		}
		if( isDispatcherRunning() ) {
            super.addMessage(super.getTestId(), TestProtocol.ERROR, "Dispatcher running, but it should not");
		}
	
		// activate autoextend feature
		m_DBM.execCmd("auto_extend ON", DBMUtility.OK_OK);

		
		// check if dispatcher is running and cnfiguration was changed
		if( !isDispatcherConfiguredToRun() ) {
            super.addMessage(super.getTestId(), TestProtocol.ERROR, "Dispatcher not configured to run, but it should be");
		}
		if( !isDispatcherRunning() ) {
            super.addMessage(super.getTestId(), TestProtocol.ERROR, "Dispatcher not running, but it should");
		}

		super.addMessage(super.getTestId(), TestProtocol.INFO, "Start filling database");
		
        Connection con                     = null;
        int count                          = 0;
        TestStatement stmt                 = null;
        TestPreparedStatement prstmtCheck  = null;
        TestPreparedStatement prstmtInsert = null;
		
        // fill data buffer
        StringBuffer testDataBuffer = new StringBuffer();
        for(int i=0; i<120; i++)
            testDataBuffer.append("0123456789abcdefghijklmnopqrstuvwxyz");
        String testData = new String(testDataBuffer);
        
        try {
			con = m_DB.connect(m_DB.getDBAUser(),m_DB.getDBAPassword());
            con.setAutoCommit(false);
			// first, create the table
            stmt = new TestStatement(this, con);
            try {
                stmt.executeUpdate("drop table TESTFILLER");
            }
            catch( SQLException sqle ) {
                // ignore this
            }
			stmt.executeUpdate("create table TESTFILLER (cnt int, text char(128), myblob long byte, primary key(cnt))");
			// for checking
			prstmtCheck = new TestPreparedStatement(this, con, "select \"Data            (%)\" from SYSDBM.info_state"); 

			// fill the table
			prstmtInsert = new TestPreparedStatement(this, con, "insert TESTFILLER values (?,?,?)");
			boolean goon = true;
			Date myd = new Date();

            int initialVols = getDataVolumes();
			while( goon ) {
				prstmtInsert.setInt(1, count++);
				myd.setTime(System.currentTimeMillis());
				prstmtInsert.setString(2,super.getTestId() + " " + myd.toString());
                prstmtInsert.setBytes(3,testData.getBytes());
				prstmtInsert.executeUpdate();
				
				if( (count % 1000) == 0 ) {
					// get current filling degree
					if( getDataVolumes() > initialVols ) {
						// obvisouly, a new volume was added
						goon = false;
						super.addMessage(super.getTestId(), TestProtocol.INFO,
							"at least one data volume was added. good." );
					}
                    else {
                        ResultSet res = prstmtCheck.executeQuery();
                        res.next();
                        if( res.getInt(1) > 95 ) {
                        	int check = 0;
                            // recheck in loop, handler is asynchronous
                            while(check < 300 && !(getDataVolumes() > initialVols)) {
                            	++check;
                                Thread.sleep(1000L);
                                super.addMessage(super.getTestId(), TestProtocol.INFO,
                                		"waiting for " + check  + "seconds altogether now");
                            }
                            if( !(getDataVolumes() > initialVols) )
                                throw new TestCaseException( "a new volume should have been added, but obvisouly wasn't. bad." );
                        }
                    }
				}				
			}
            super.addMessage(super.getTestId(), TestProtocol.INFO, "End filling database" );
		}
        catch (TestDatabaseException e) {
            String message = null;
            String className = null;
            if( e.getOriginalException() != null ) {
                message = e.getOriginalException().getMessage();
                className = e.getOriginalException().getClass().getName();
            }
            else {
                message = e.getMessage();
                className = e.getClass().getName();
            }
            super.addMessage(super.getTestId(), TestProtocol.ERROR,
                className + " caught: " + message + ". abort." );
        }
        catch( Exception e ) {
            super.addMessage(super.getTestId(), TestProtocol.ERROR,
                e.getClass().getName() + " caught: " + e.getMessage() + ". abort." );
        }
        finally {
            super.addMessage(super.getTestId(), TestProtocol.INFO,
                "number of records inserted: " + count); 
            try {
                if( stmt != null )
                    stmt.close();
                if( prstmtCheck != null )
                    prstmtCheck.close();
                if( prstmtInsert != null )
                    prstmtInsert.close();

                stmt = new TestStatement(this, con);
                stmt.executeUpdate("drop table TESTFILLER");
                con.commit();
                stmt.close();
                con.close();
            } catch (SQLException e) {
                super.addMessage(super.getTestId(), TestProtocol.ERROR,
                    e.getClass().getName() + " caught while cleaning up: " + e.getMessage() + ". abort." );

            }
			if( !m_DBM.execCmd("auto_extend OFF", DBMUtility.OK_OK) )
				super.addMessage(super.getTestId(), TestProtocol.ERROR, "could not stop event dispatcher");

			// final status
			if( isDispatcherConfiguredToRun()) {
	            super.addMessage(super.getTestId(), TestProtocol.ERROR, "Dispatcher configured to run, but it should not");
			}
			
			if( isDispatcherRunning() ) {
				super.addMessage(super.getTestId(), TestProtocol.ERROR, "Dispatcher running, but it should not");
			}
        }
	}

	/*
     * not counting the number of volumes, just the number of lines in the
     * answer... good enough.
     */
    private int getDataVolumes() {
        int vols = 0;
        m_DBM.execCmd("param_getvolsall DATA", DBMUtility.OK_OK);
        String res = m_DBM.getAnswer(); 
        int length = res.length();
        for(int count = 0; count < length; count++) {
            if( res.charAt(count) == '\n' )
                vols++;            
        }                
        return vols;
    }
	
}
