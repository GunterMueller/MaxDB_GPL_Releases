package com.sap.sapdb.DBMTest.scheduler;

import java.sql.SQLException;

import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.DBMTest.util.user.DBMUser;
import com.sap.sapdb.DBMTest.util.user.DBMUserManagementException;
import com.sap.sapdb.DBMTest.util.user.ServerRight;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

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

public class SchedulingAuthorizations extends SchedulerTestCase {

    /**
     * @throws SQLException
     * @throws TestDatabaseException
     */
    public SchedulingAuthorizations() throws SQLException, TestDatabaseException {
        super();
    }

    /* (non-Javadoc)
     * @see com.sap.sapdb.DBMTest.base.DBMTestCase#runDBMTest()
     */
    public void runSchedulerTest() {

        // create two users
        DBMUser scdadmin = null;
        DBMUser scduser = null;
        try {
            scdadmin = new DBMUser("scdadmin", "scdadmin");
            scduser = new DBMUser("scduser", "scduser");

            if( !scdadmin.existsOnDisk() || !scduser.existsOnDisk() ) {
                addMessage("prepare users", 
                    TestProtocol.ERROR, 
                    "users " + scdadmin.getName() + " and " + scduser.getName() + " not on disk");
                return;
            }

            // adjust the rights
            // no user right for scdadmin, but should still be able to
            // use it since he is admin
            // scduser should not be able to start/stop the scheduler
            
            scdadmin.revokeRight(ServerRight.SchedulerUse);
            scduser.revokeRight(ServerRight.SchedulerAdmin);
            
            scdadmin.logon();
            scduser.logon();
        }
        catch( DBMUserManagementException dbmue ) {        
            addMessage("prepare users", 
                TestProtocol.ERROR, 
                "something went wrong...",
                dbmue);
            return;
        }

        // users exist, now start testing
        scduser.execCmd("scheduler_start", DBMUtility.ERR_MISSRIGHT);
        scduser.execCmd("scheduler_stop", DBMUtility.ERR_MISSRIGHT);
        scdadmin.execCmd("scheduler_stop", DBMUtility.OK_OK);
    }
}
