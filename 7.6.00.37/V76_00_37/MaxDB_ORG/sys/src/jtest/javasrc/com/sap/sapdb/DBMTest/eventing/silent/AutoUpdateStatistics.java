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
import java.util.Iterator;
import java.util.Vector;

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestStatement;

public class AutoUpdateStatistics extends EventingSilentTestCase {

	private static final int WAIT_TIMEOUT = 30*60; // 30 minutes
	private static final String COULD_NOT_READ_CONTENT = "could not read content from DBA.SYSUPDSTATWANTED";
	private static final String GET_CONTENT = "select * from DBA.SYSUPDSTATWANTED";
	private static final String NO_DISPATCHER_STOP = "could not stop event dispatcher";
	private static final String TABLE_NOT_CLEARED_AGAIN = "table was not cleared again after another " + WAIT_TIMEOUT + " minutes, content:\n";
	private static final String TABLE_NOT_CLEARED = "auto_update_statistics ON did not clear table after " + WAIT_TIMEOUT + " minutes, content:\n";
	private static final String NO_DISPATCHER_START = "could not start event dispatcher";
	private static final String KERNEL_INSERTED_NOTHING = "no initial content in SYSUPDSTATWANTED -> kernel inserted nothing!";
	private static final String DISPATCHER_RUNNING = "Dispatcher running, but it should not";
	private static final String DISPATCHER_NOT_RUNNING = "Dispatcher not running, but it should be";
	private static final String DISPATCHER_CONFIGURED_TO_RUN = "Dispatcher configured to run, but it should not";
	private static final String DISPATCHER_NOT_CONFIGURED_TO_RUN = "Dispatcher not configured to run, but it should be";
	private static final String ERROR_WAITING = "error while waiting for update statistics";
	private static final String ERROR_PROVOKING_UPDATE = "error making kernel want statistics updated";
	private static final int TABLE_NOT_EXISTANT = -4004;
	private static final String DROP_TABLE_2 = "drop table TESTUPDSTAT2";
	private static final String DROP_TABLE_1 = "drop table TESTUPDSTAT1";
	private static final String COUNT_RECORDS = "select count(*) from DBA.SYSUPDSTATWANTED";
	private static final String JOIN_SELECT = "select * from TESTUPDSTAT1,TESTUPDSTAT2 where TESTUPDSTAT1.INTCOL1 = TESTUPDSTAT2.INTCOL2";
	private static final String CREATE_TABLE_2 = "create table TESTUPDSTAT2 (INTCOL2 int)";
	private static final String CREATE_TABLE_1 = "create table TESTUPDSTAT1 (INTCOL1 int)";
	private static final String INSERT_INTO_TABLE2 = "insert into TESTUPDSTAT2 values(2)";
	private static final String INSERT_INTO_TABLE1 = "insert into TESTUPDSTAT1 values(1)";
	private static final String ERROR_TABLE_2 = "error dropping table TESTUPDSTAT2";
	private static final String ERROR_TABLE_1 = "error dropping table TESTUPDSTAT1";
	private static final String ERROR_COUNTING = "error counting records";

	private class AutoUpdateStatisticsException extends Exception {
		public AutoUpdateStatisticsException(String text) {
			super(text);
		}
		public AutoUpdateStatisticsException(String text, Throwable cause) {
			super(text, cause);
		}
		public static final long serialVersionUID = 96L; // :-)
	}
	
	private Connection m_Connection = null;
	
	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public AutoUpdateStatistics() throws SQLException, TestDatabaseException {
		super();
	}

	private int getLinesInUpdateStatisticsTable() throws AutoUpdateStatisticsException {
        
		int rowsInResultSet = -1;
		try {
			TestStatement stmt = new TestStatement(this, m_Connection);;
			stmt.execute(COUNT_RECORDS);
			ResultSet res = stmt.getResultSet();
	    	if( res.next() )
	    		rowsInResultSet = res.getInt(1);
	    	else {
	        	super.addMessage(super.getTestId(), TestProtocol.ERROR, ERROR_COUNTING);
	        	throw new AutoUpdateStatisticsException(ERROR_COUNTING);
	    	}
	    	stmt.close();
		}
		catch( SQLException sqle ) {
        	super.addMessage(super.getTestId(), TestProtocol.ERROR, ERROR_COUNTING, sqle);
        	throw new AutoUpdateStatisticsException(ERROR_COUNTING, sqle);
		}
		return rowsInResultSet;
	}

	private void dropTables() throws AutoUpdateStatisticsException {
		AutoUpdateStatisticsException ause = null;
		try {
			TestStatement stmt = new TestStatement(this, m_Connection);
            stmt.executeUpdate(DROP_TABLE_1);
            stmt.close();
        }
        catch( SQLException sqle ) {
        	if( sqle.getErrorCode() != TABLE_NOT_EXISTANT ) {
	        	super.addMessage(super.getTestId(), TestProtocol.ERROR, ERROR_TABLE_1, sqle);
	        	ause = new AutoUpdateStatisticsException(ERROR_TABLE_1, sqle);
        	}
        }
		try {
			TestStatement stmt = new TestStatement(this, m_Connection);
            stmt.executeUpdate(DROP_TABLE_2);
            stmt.close();
        }
        catch( SQLException sqle ) {
        	if( sqle.getErrorCode() != TABLE_NOT_EXISTANT ) {
	        	super.addMessage(super.getTestId(), TestProtocol.ERROR, ERROR_TABLE_2, sqle);
	        	throw new AutoUpdateStatisticsException(ERROR_TABLE_2, sqle);
        	}
        }
        if( ause != null )
        	throw ause;
	}
	
	private void makeKernelWantStatisticsUpdate() throws AutoUpdateStatisticsException {
		try {
			TestStatement stmt = new TestStatement(this, m_Connection);
	        stmt.executeUpdate(CREATE_TABLE_1);
	        stmt.executeUpdate(CREATE_TABLE_2);
	        stmt.executeUpdate(INSERT_INTO_TABLE1);
	        stmt.executeUpdate(INSERT_INTO_TABLE2);
	        stmt.executeQuery(JOIN_SELECT);
            stmt.close();
        }
        catch( SQLException sqle ) {
        	super.addMessage(super.getTestId(), TestProtocol.ERROR, ERROR_PROVOKING_UPDATE, sqle);
        	throw new AutoUpdateStatisticsException(ERROR_PROVOKING_UPDATE, sqle);
        }
	}
	
	private boolean checkSystemTableIsCleared() throws AutoUpdateStatisticsException {
		int count = WAIT_TIMEOUT;
		try {
			int lines = getLinesInUpdateStatisticsTable();
			while( count>0 && lines != 0 ) {
				Thread.sleep(1000L);
				lines = getLinesInUpdateStatisticsTable();
			}
			return lines == 0;
		}
		catch( Exception e ) {
			super.addMessage(super.getTestId(), TestProtocol.ERROR, ERROR_WAITING, e);
			throw new AutoUpdateStatisticsException(ERROR_WAITING, e);
		}
	}
	
	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.base.DBMTestCase#runDBMTest()
	 */
	protected void runDBMTest() {

        try {
      	
    		// initial status, dispatcher must be OFF and configured to be OFF
    		if( isDispatcherConfiguredToRun() ) {
                super.addMessage(super.getTestId(), TestProtocol.ERROR, DISPATCHER_CONFIGURED_TO_RUN);
                throw new AutoUpdateStatisticsException(DISPATCHER_CONFIGURED_TO_RUN);
    		}
    		if( isDispatcherRunning() ) {
                super.addMessage(super.getTestId(), TestProtocol.ERROR, DISPATCHER_RUNNING);
                throw new AutoUpdateStatisticsException(DISPATCHER_RUNNING);
    		}

    		// create the sql objects
        	m_Connection = m_DB.connect(m_DB.getDBAUser(),m_DB.getDBAPassword());
			m_Connection.setAutoCommit(true);

			// create entry in system tables
			dropTables();
			makeKernelWantStatisticsUpdate();
			
			// check system table, we expect it to contain at least one record
            Vector content = getContent();
            if( content.isEmpty() ) {
            	// we failed creating the entries
            	super.addMessage(super.getTestId(),
            			TestProtocol.ERROR, KERNEL_INSERTED_NOTHING);
            	throw new AutoUpdateStatisticsException(KERNEL_INSERTED_NOTHING);
            }
            
    		// activate autoextend feature
    		if( !m_DBM.execCmd("auto_update_statistics ON", DBMUtility.OK_OK) ) {
    			super.addMessage(super.getTestId(), TestProtocol.ERROR, NO_DISPATCHER_START);
    			throw new AutoUpdateStatisticsException(NO_DISPATCHER_START);
    		}

    		// dispatcher must now be ON and configured to be ON
    		if( !isDispatcherConfiguredToRun() ) {
                super.addMessage(super.getTestId(), TestProtocol.ERROR, DISPATCHER_NOT_CONFIGURED_TO_RUN);
                throw new AutoUpdateStatisticsException(DISPATCHER_NOT_CONFIGURED_TO_RUN);
    		}
    		if( !isDispatcherRunning() ) {
                super.addMessage(super.getTestId(), TestProtocol.ERROR, DISPATCHER_NOT_RUNNING);
                throw new AutoUpdateStatisticsException(DISPATCHER_NOT_RUNNING);
    		}
    		
    		if( !checkSystemTableIsCleared() ) {
                content = getContent();
                StringBuffer text = writeContent(content);
            	super.addMessage(super.getTestId(),
            			TestProtocol.ERROR,
            			TABLE_NOT_CLEARED + text);
            	throw new AutoUpdateStatisticsException(TABLE_NOT_CLEARED + text);
    		}
    		
			// create entry in system tables again
			dropTables();
			makeKernelWantStatisticsUpdate();
    		
    		if( !checkSystemTableIsCleared() ) {
                content = getContent();
                StringBuffer text = writeContent(content);
            	super.addMessage(super.getTestId(),
            			TestProtocol.ERROR,
            			TABLE_NOT_CLEARED_AGAIN + text);
            	throw new AutoUpdateStatisticsException(TABLE_NOT_CLEARED_AGAIN + text);
    		}
    		
        }
        catch( Exception e ) {
            super.addMessage(super.getTestId(), TestProtocol.ERROR,
                e.getClass().getName() + " caught: " + e.getMessage() + ". abort.", e );
        }
        finally {
            try {
            	dropTables();
                m_Connection.close();
            } catch (Exception e) {
                super.addMessage(super.getTestId(), TestProtocol.ERROR,
                    e.getClass().getName() + " caught while cleaning up: " + e.getMessage() + ". abort." );

            }
			if( !m_DBM.execCmd("auto_update_statistics OFF", DBMUtility.OK_OK) )
				super.addMessage(super.getTestId(), TestProtocol.ERROR, NO_DISPATCHER_STOP);

			// final status
			if( isDispatcherConfiguredToRun()) {
	            super.addMessage(super.getTestId(), TestProtocol.ERROR, DISPATCHER_CONFIGURED_TO_RUN);
			}
			
			if( isDispatcherRunning() ) {
				super.addMessage(super.getTestId(), TestProtocol.ERROR, DISPATCHER_RUNNING);
			}
        }
	}

	private Vector getContent() throws AutoUpdateStatisticsException {
		Vector content = new Vector();
		try {
			TestStatement stmt = new TestStatement(this, m_Connection);
			stmt.execute(GET_CONTENT);
			ResultSet res = stmt.getResultSet();
			int columns = res.getMetaData().getColumnCount();
			
			while( res.next() ) {
				StringBuffer line = new StringBuffer();
				for( int currentColumn=1; currentColumn <= columns; ++currentColumn ) {
					String value = res.getString(currentColumn);
					if( value == null ) {
						value = "null";
					}
					else {
						value = value.trim();
						if( value.length() == 0)
							value = "empty";
					}
					line.append(value);
					if( currentColumn != columns )
						line.append('|');
				}
				content.add(line);
			}
		}
		catch( SQLException sqle ) {
			super.addMessage(super.getTestId(), TestProtocol.ERROR, COULD_NOT_READ_CONTENT, sqle);
			throw new AutoUpdateStatisticsException(COULD_NOT_READ_CONTENT, sqle);
		}
		return content;
	}
	
	private StringBuffer writeContent( Vector tableContent) {
		StringBuffer content = new StringBuffer();
		Iterator linesIterator = tableContent.iterator();
		while( linesIterator.hasNext() ) {
			StringBuffer line = (StringBuffer)linesIterator.next();
			line.append('\n');
			content.append(line);
		}
		return content;
	}
}