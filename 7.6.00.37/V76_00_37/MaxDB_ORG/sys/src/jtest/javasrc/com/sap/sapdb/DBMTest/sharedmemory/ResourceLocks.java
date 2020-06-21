package com.sap.sapdb.DBMTest.sharedmemory;

import java.sql.SQLException;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * 
 * @author d026948
 *
 * This test checks whether the DBMServer shared memory is still okay at the end of the test sequence.
 * Inplemented tests:
 * * number of resource lock entries in shared memory should be 0
 *  
 */
public class ResourceLocks extends DBMTestCase {

	private static final String MARKER = "*** Resource Locks";
	private static final String EXLINES = "Existing lines ";
	private static final int EXLINESLENGTH = ResourceLocks.EXLINES.length();
	
	public ResourceLocks() throws SQLException, TestDatabaseException {
		super();
	}

	protected void runDBMTest() throws Exception {

		int resourceLinesRemaining = -1;
		String shmStatusInfo = null;

		if( m_DBM.execCmd("dbm_shm_info", DBMUtility.OK_OK) ) {
			shmStatusInfo = m_DBM.getAnswer();
			
			int index = shmStatusInfo.indexOf(ResourceLocks.MARKER);
			if( index >= 0 ) {
				index = shmStatusInfo.indexOf(ResourceLocks.EXLINES, index);
			}
			
			if( index == -1 ) {
				addMessage("parse error",TestProtocol.ERROR,
						"token \"" + ResourceLocks.MARKER + "\" not found in\n" + shmStatusInfo);
				return;
			}
			else {
				int startIndex = index + ResourceLocks.EXLINESLENGTH;
				int endIndex = shmStatusInfo.indexOf('\n', startIndex);
				if( endIndex > 0 ) {
					resourceLinesRemaining = Integer.parseInt(shmStatusInfo.substring(startIndex, endIndex));
				}
				else {
					addMessage("parse error",TestProtocol.ERROR,
							"no newline found after token \"" + ResourceLocks.MARKER + "\" and number in\n" + shmStatusInfo);
					return;
				}
			}
		}
		if( resourceLinesRemaining != 0 ) {
			addMessage("not zero locks", TestProtocol.ERROR,
					"found \"" + resourceLinesRemaining + "\" in\n" + shmStatusInfo);
		}
	}
}
