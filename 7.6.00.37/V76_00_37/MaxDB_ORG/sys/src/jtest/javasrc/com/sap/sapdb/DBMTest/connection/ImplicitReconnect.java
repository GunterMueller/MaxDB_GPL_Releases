/*!
    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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

package com.sap.sapdb.DBMTest.connection;

import java.sql.SQLException;
import com.sap.dbtech.powertoys.DBM;
import com.sap.dbtech.rte.comm.RTEException;
import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;

public class ImplicitReconnect extends DBMTestCase {

	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public ImplicitReconnect() throws SQLException, TestDatabaseException {
		super();
	}

	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.base.DBMTestCase#runDBMTest()
	 */
	protected void runDBMTest() throws RTEException {
		
		//prepare
		DBM        secondDBMSession= DBM.dbDBM(m_DB.getHost() + ':' + m_DB.getPort(), m_DB.getName());
		DBMUtility dbm_2=new DBMUtility(m_DB, this, secondDBMSession);
		
		m_DBM.execCmd("medium_put DATA_connection.ImplicitReconnect DATA_connection.ImplicitReconnect FILE DATA 0 0 YES", DBMUtility.OK_OK);
		m_DBM.execCmd("medium_put DATALimit_connection.ImplicitReconnect DATALimit_connection.ImplicitReconnect FILE DATA 100 0 YES", DBMUtility.OK_OK);
		
        dbm_2.execCmd("user_logon "+m_DB.getDBMUser()+','+m_DB.getDBMPassword(), DBMUtility.OK_OK);

        //reconnect db_execute after db_admin by 2nd DBM Server
		m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_execute select * from tables", DBMUtility.OK_OK);
		dbm_2.execCmd("db_admin", DBMUtility.OK_OK);
		m_DBM.execCmd("db_execute select * from tables", DBMUtility.ERR_SQL, -101);
		
		// reconnect db_execute after db_restart by 2nd DBM Server
		m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_execute select * from tables", DBMUtility.OK_OK);
		dbm_2.execCmd("db_restart", DBMUtility.OK_OK);
		m_DBM.execCmd("db_execute select * from tables", DBMUtility.OK_OK);
		
		//reconnect db_executenice after db_admim by 2nd DBM Server
		m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_executenice select * from tables", DBMUtility.OK_OK);
		dbm_2.execCmd("db_admin", DBMUtility.OK_OK);
		m_DBM.execCmd("db_executenice select * from tables", DBMUtility.ERR_SQL, -101);
		
		//reconnect db_executenice after db_restart by 2nd DBM Server
		m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_executenice select * from tables", DBMUtility.OK_OK);
		dbm_2.execCmd("db_restart", DBMUtility.OK_OK);
		m_DBM.execCmd("db_executenice select * from tables", DBMUtility.OK_OK);
		
		
		
		//reconnect db_execute after db_admin by own DBM-Server
		m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_execute select * from tables", DBMUtility.OK_OK);
		m_DBM.execCmd("db_admin", DBMUtility.OK_OK);
		m_DBM.execCmd("db_execute select * from tables", DBMUtility.ERR_SQL, -101);
		
		//reconnect db_execute after db_restart by own DBM-Server
		m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_execute select * from tables", DBMUtility.OK_OK);
		m_DBM.execCmd("db_restart", DBMUtility.OK_OK);
		m_DBM.execCmd("db_execute select * from tables", DBMUtility.OK_OK);
		
		//reconnect db_executenice after db_admim by own DBM-Server
		m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_executenice select * from tables", DBMUtility.OK_OK);
		m_DBM.execCmd("db_admin", DBMUtility.OK_OK);
		m_DBM.execCmd("db_executenice select * from tables", DBMUtility.ERR_SQL, -101);
		
		//reconnect db_executenice after db_restart by own DBM-Server
		m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_executenice select * from tables", DBMUtility.OK_OK);
		m_DBM.execCmd("db_restart", DBMUtility.OK_OK);
		m_DBM.execCmd("db_executenice select * from tables", DBMUtility.OK_OK);
		
		
		//db_fetch must not reconnect
		int [] allowedDBMCodes=new int [] {DBMUtility.OK_OK, DBMUtility.ERR_RTE};
		
		m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_execute select * from tables", DBMUtility.OK_OK);
		dbm_2.execCmd("db_restart", DBMUtility.OK_OK);
		
		do {
			m_DBM.execCmd("db_fetch", allowedDBMCodes);
		}
		while(DBMUtility.OK_OK==m_DBM.getLastError());
		
		//db_release must release the session
		m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_release", DBMUtility.OK_OK);
		m_DBM.execCmd("backup_start DATA_connection.ImplicitReconnect", DBMUtility.ERR_NODBSESSION);
		
		//no implicit reconnect after a failed db_connect
		m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect xyz,xyz", DBMUtility.ERR_SQL);
		m_DBM.execCmd("backup_start DATA_connection.ImplicitReconnect", DBMUtility.ERR_NODBSESSION);
        
        //reconnect by db_execute* breaks backup session
        m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect", DBMUtility.OK_OK);
		m_DBM.execCmd("backup_start DataLimit_connection.ImplicitReconnect", DBMUtility.OK_OK);
		dbm_2.execCmd("db_restart", DBMUtility.OK_OK);
		m_DBM.execCmd("db_execute select * from tables", DBMUtility.OK_OK);
		m_DBM.execCmd("backup_replace Data_connection.ImplicitReconnect", DBMUtility.ERR_NOBACKUPSESSION);
        
        //reconnect by db_execute* breaks recover session
        m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect", DBMUtility.OK_OK);
		m_DBM.execCmd("backup_start DataLimit_connection.ImplicitReconnect", DBMUtility.OK_OK);
		m_DBM.execCmd("backup_replace Data_connection.ImplicitReconnect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_admin", DBMUtility.OK_OK);
		m_DBM.execCmd("recover_start DataLimit_connection.ImplicitReconnect", DBMUtility.OK_OK);
		dbm_2.execCmd("db_offline", DBMUtility.OK_OK);
		dbm_2.execCmd("db_admin", DBMUtility.OK_OK);
		m_DBM.execCmd("db_execute select * from tables", DBMUtility.ERR_SQL, -101);
		m_DBM.execCmd("recover_replace Data_connection.ImplicitReconnect", DBMUtility.ERR_NOBACKUPSESSION);
		m_DBM.execCmd("recover_start DataLimit_connection.ImplicitReconnect", DBMUtility.OK_OK);
		m_DBM.execCmd("recover_replace Data_connection.ImplicitReconnect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		
		//reconnect by backup_start breaks db_execute* session
		m_DBM.execCmd("db_online", DBMUtility.OK_OK);
		m_DBM.execCmd("db_connect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_execute select * from tables", DBMUtility.OK_OK);
		dbm_2.execCmd("db_restart", DBMUtility.OK_OK);
		m_DBM.execCmd("backup_start Data_connection.ImplicitReconnect", DBMUtility.OK_OK);
		m_DBM.execCmd("db_fetch", DBMUtility.ERR_RESULTLOST);


		//clean up
        m_DBM.execCmd("medium_delete Data_connection.ImplicitReconnect", DBMUtility.OK_OK);
        m_DBM.execCmd("medium_delete DataLimit_connection.ImplicitReconnect", DBMUtility.OK_OK);

		secondDBMSession.release();
	}
}
