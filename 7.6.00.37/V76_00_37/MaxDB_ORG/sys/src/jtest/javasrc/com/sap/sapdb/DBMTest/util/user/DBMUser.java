package com.sap.sapdb.DBMTest.util.user;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Properties;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.Vector;

import com.sap.dbtech.powertoys.DBM;
import com.sap.dbtech.powertoys.DBMException;
import com.sap.dbtech.rte.comm.RTEException;
import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabase;

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

public class DBMUser {

    static private DBM m_RootDBMSession = null;
    static private TestDatabase m_DataBase = null;
    static private DBMTestCase m_DBMTestCase = null;
    static private Vector m_CreatedUsersOnDisk = null;
    static private Vector m_ExistingUsersOnDisk = null;

    static private Set m_OpenSessions = null;
    
    static {
        m_CreatedUsersOnDisk = new Vector();
        m_ExistingUsersOnDisk = new Vector();
        m_OpenSessions = new HashSet();
    }

    private DBMUtility m_MyDBMUtility = null;    
    private DBM m_MyDBMSession = null;
    private boolean m_IsLoggedOn;
    private HashMap m_MyRights = new HashMap(); 
    private final String m_Name;
    private final String m_Password;
    
    /**
     * Sets the static information.
     * 
     * Called by the DBMTestCase class only.
     * 
     * @param aDBMUtility the root DBMUtility object initially constructed.
     * @param aTestDataBase the database on which we test
     * @param aDBMTestCase the current test case
     * @throws DBMUserManagementException
     */
    static public void setCommmonInformation(
            DBM aDBMSession,
            TestDatabase aTestDataBase,
            DBMTestCase aDBMTestCase) throws DBMUserManagementException {
        m_RootDBMSession = aDBMSession;
        m_DataBase = aTestDataBase;
        m_DBMTestCase = aDBMTestCase;
        //m_RootDBMUtility = new DBMUtility(m_DataBase, m_DBMTestCase, m_RootDBMSession);

        try {
            String users = m_RootDBMSession.cmd("user_getall");
            // no blank, no tab as delimiters
            StringTokenizer usersTok = new StringTokenizer(users, "\n\r\f");
            while( usersTok.hasMoreTokens() )
                m_ExistingUsersOnDisk.add(usersTok.nextToken());
        }
        catch( Exception e) {
            throw new DBMUserManagementException("getAllUsers", e.getMessage());
        }
    }

    /**
     * Removes created users.
     * Called by the DBMTestCase class only.
     * 
     * @throws DBMUserManagementException 
     */
    static public void removeCreatedUsers() throws DBMUserManagementException {
        
        // close any open session (what if they are busy?)
        class SessionReleaser extends Thread {
	    	private Exception m_ExOut = null;
	    	public Exception getException() {
	    		return m_ExOut;
	    	}
        	public void run() {
	        	Iterator sessionIterator = m_OpenSessions.iterator(); 
		        while (sessionIterator.hasNext()) {
		            DBM session = (DBM) sessionIterator.next();
		            try {
		                session.release();
		                sessionIterator.remove();
		            }
		            catch( RTEException rtee) {
		            	m_ExOut = rtee;
		            	return;
		            }
		        }
	    	}
        };
        
        SessionReleaser sessionReleaser = new SessionReleaser();
        sessionReleaser.start();
        
        try {
        	sessionReleaser.join(120 * 1000); // allow 2 minutes for closing all sessions
        	Exception exceptionInReleaser = sessionReleaser.getException();
        	if( exceptionInReleaser != null )
        		throw new DBMUserManagementException("release sessions, exception caught", exceptionInReleaser.getMessage());
        }
        catch( InterruptedException ire ) {
        	throw new DBMUserManagementException("release sessions, exception caught", ire.getMessage());
        }
        
        // delete the users from user store
        Iterator accountIterator = m_CreatedUsersOnDisk.iterator();
        while(accountIterator.hasNext() ) {
            removeUser((String)accountIterator.next());
        }
        m_CreatedUsersOnDisk.clear();
    }

    /**
     * Remove specified user from DBM user store
     * 
     * @param userID
     * @throws DBMUserManagementException 
     */
    static private void removeUser( String userID ) throws DBMUserManagementException {
        try {
            // we don't care about the reply...
            m_RootDBMSession.cmd("user_delete " + userID);
        }
        catch (Exception e) {
            if( e instanceof DBMException && 
                ((DBMException) e).getErrorCode() == DBMUtility.ERR_USRREAD ) {
                // ignore error: trying to delete a user that does not exist
            }
            else
                throw new DBMUserManagementException("removeUser", userID, e.getMessage());
        }
    }

    /**
     * Check whether the user with the specified userID exisits in user
     * store (user profile container)
     *      
     * @param userID
     * @return true if it exists, false otherwise
     * @throws DBMUserManagementException
     */
    static public boolean existsUserOnDisk( String userID ) throws DBMUserManagementException {
        try {
            m_RootDBMSession.cmd("user_get " + userID);
            return true;
        }
        catch (Exception e) {
            if( e instanceof DBMException &&
                ((DBMException) e).getErrorCode() == DBMUtility.ERR_USRREAD )
                return false;
            else
                throw new DBMUserManagementException("existsUserOnDisk", userID, e.getMessage());
        }
    }
    
    /**
     * Creates a new user. User is created in DBM user store (user profile
     * container). If it already exists, it is overwritten there. This new
     * user has all avaliable rights. 
     * 
     * Users that existed before the test, cannot be accessed through this class.
     * Hence there is not danger that "important" users like e.g. dbm are touched.
     *  
     * @param aName the new user's userid
     * @param aPassword the new user's password
     * @throws DBMUserManagementException
     */
    public DBMUser(String aName, String aPassword) throws DBMUserManagementException {

        // if it's one that existed initially, we quit
        if( m_ExistingUsersOnDisk.contains(aName) )
            throw new DBMUserManagementException("createUser", aName, "already exists");

        // remove old user account        
        m_CreatedUsersOnDisk.remove(aName);
        removeUser(aName);

        m_Name = aName;
        m_Password = aPassword;
        m_IsLoggedOn = false;

        try {
            m_RootDBMSession.cmd("user_create " + m_Name + ',' + m_Password);
            m_CreatedUsersOnDisk.add(m_Name);
            setRights(ServerRight.getAllRights());
        }
        catch( Exception e ) { 
            throw new DBMUserManagementException("createUser", aName, e.getMessage());
        }
    }


    /**
     * logs on this user (using DBMServer command user_logon). The user waits for ever for a
     * response from the DBM Server.
     *
     * @throws DBMUserManagementException
     */
    public void logon() throws DBMUserManagementException {
    	logon(0L);
    }
        
    /**
     * logs on this user (using DBMServer command user_logon) with a communication timeout.
     * The user waits for at most communicationTimeout seconds for a response from the DBM Server
     * util he gives up waiting. Subsequent command executions (including logout) fail with aRTEException.
     * This user is useless then.
     *
     * @throws DBMUserManagementException
     */
    public void logon(long communicationTimeout) throws DBMUserManagementException {
        if( isLoggendOn() )
            return;
            
        try {
            Properties dbmProps = new Properties();
            dbmProps.put("host", m_DataBase.getHost() + ':' + m_DataBase.getPort());
            dbmProps.put("dbname", m_DataBase.getName());
            dbmProps.put("communicationtimeout", Long.toString(communicationTimeout<0?0:communicationTimeout));
            
        	m_MyDBMSession = new DBM(dbmProps);
            m_MyDBMSession.cmd("user_logon " + m_Name + ',' + m_Password);
            m_OpenSessions.add(m_MyDBMSession);
            m_MyDBMUtility = new DBMUtility(m_DataBase, m_DBMTestCase , m_MyDBMSession);
            m_IsLoggedOn = true;
        }
        catch( Exception e ) { 
            throw new DBMUserManagementException("logon", m_Name, e.getMessage());            
        }
    }

    /**
     * logs off this user
     *  
     * @throws DBMUserManagementException 
     */
    public void logoff() throws DBMUserManagementException {
        try {
            m_MyDBMSession.cmd("exit");
            m_IsLoggedOn = false;
            m_OpenSessions.remove(m_MyDBMSession);
        }
        catch( Exception e ) { 
            throw new DBMUserManagementException( "logoff " + e.getClass().getName(), m_Name, e.getMessage());
        }
    }

    /**
     * Check if this user is logged on
     * 
     * @return true if this user is logged on, otherwise false
     */
    public boolean isLoggendOn() {
        return m_IsLoggedOn;
    }

    /**
     * Check if this user exists in DBM user store
     * 
     * @return true if this user exists in DBM user store, otherwise false
     */
    public boolean existsOnDisk() throws DBMUserManagementException {
        return DBMUser.existsUserOnDisk(m_Name);
    }

    /**
     * Executes a DBM server command, if the user is logged on.
     * Get the answer with getAnswer() method.
     * 
     * @param aCommand command to be executed
     * @param nExpected the expected reply code
     * @return true if expected reply came, otherwise false
     */
    public boolean execCmd(String aCommand, int aExpectedReturnCode) {
        return
            isLoggendOn() &&
            m_MyDBMUtility.execCmd(aCommand, aExpectedReturnCode); 
    }

    /**
     * Same as execCmd(String aCommand, int aExpectedReturnCode), plus
     * write an info message to the protocol in case of success.
     * 
     * @param aCommand
     * @param aExpectedReturnCode
     * @param writeInfo
     * @return true if expexted reply came, otherwise false
     */
    public boolean execCmd(
                    String aCommand,
                    int aExpectedReturnCode,
                    boolean writeInfo) {
        return
            isLoggendOn() &&
            m_MyDBMUtility.execCmd(aCommand, aExpectedReturnCode, writeInfo); 
    }

    /**
     * Executes a DBM server command and a "check" DBM server command, if the
     * user is logged on.
     * 
     * @param aCommand command to execute
     * @param aCheckCommand check command to execute after aCommand.
     * @param aExpectedReply substring of the reply of check command, that is
     *        expected to be contained there. 
     * @return true if aExpectedReply is substring of the reply of the check
     *         command, false otherwise. 
     */
    public boolean execCmd(String aCommand, String aCheckCommand, String aExpectedReply) {
        return
            isLoggendOn() && 
            m_MyDBMUtility.execCmd(aCommand, aCheckCommand, aExpectedReply); 
    }

    /**
     * Get the reply of the last DBMServer command
     * 
     * @return reply of the last DBMServer command
     */
    public String getLastCommandReply() {
        if( isLoggendOn() )
            return m_MyDBMUtility.getAnswer();
        else
            return "";
    }

    /**
     * Get this user's name
     * 
     * @return this user's name
     */
    public String getName() {
        return m_Name;
    }

    /**
     * Get this user's password
     * 
     * @return this user's password
     */
    public String getPassword() {
        return m_Password;
    }

    /**
     * Grant a right to this users
     * 
     * @param newRightIndex index of right to grant
     * @return true if right could be granted, false otherwise
     * @throws DBMUserManagementException
     */
    public void grantRight(int newRightIndex) throws DBMUserManagementException {
        ServerRight newRight = ServerRight.get(newRightIndex);
        if( newRight == null)
            throw new DBMUserManagementException("grantRight", m_Name,
                "right with index " + newRightIndex + " does not exist");
           
        try {
            m_RootDBMSession.cmd(
                    "user_put " +
                    m_Name +
                    " SERVERRIGHTS=+" +
                    newRight.m_Text);
            m_MyRights.put(new Integer(newRightIndex),newRight);
        }
        catch (Exception e) {
            throw new DBMUserManagementException("grantRight", m_Name, e.getMessage());
        }
    }

    /**
     * Take away a right from this user.
     * 
     * @see ServerRight
     * @param oldRight index of the right to be revoked
     * @throws DBMUserManagementException 
     */
    public void revokeRight(int oldRightIndex) throws DBMUserManagementException  {
        ServerRight oldRight = ServerRight.get(oldRightIndex);
        if( oldRight == null)
        throw new DBMUserManagementException("revokeRight", m_Name,
            "right with index " + oldRightIndex + " does not exist");

        try {
            m_RootDBMSession.cmd(
                    "user_put " +
                    m_Name +
                    " SERVERRIGHTS=-" +
                    oldRight.m_Text);
            m_MyRights.remove(new Integer(oldRightIndex));
        }
        catch (Exception e) {
            throw new DBMUserManagementException("revokeRight", m_Name, e.getMessage());
        }
    }

    /**
     * Replaces the current rights of the user with the rights
     * in the new set. The map must contain only ServerRight objects.  
     *  
     * @param newRightsSet the new set of rights
     * @throws DBMUserManagementException
     *     
     */
    public void setRights(HashMap newRightsSet) throws DBMUserManagementException {
        // remove the old rights
        while( !m_MyRights.isEmpty() )
            revokeRight(((ServerRight)(m_MyRights.values().iterator().next())).m_ID);

        // grant the new rights
        Iterator it = newRightsSet.values().iterator();
        while( it.hasNext() )
            grantRight(((ServerRight)(it.next())).m_ID);
    }

    /**
     * Get current rights of this user.
     * 
     * @return a HashMap containing the rights of this user.
     */
    public HashMap getRights() {
        HashMap outRights = new HashMap(m_MyRights);
        return outRights;    
    }

    /**
     * Check whether this user has right with index rightIndex.
     * @see ServerRight
     * 
     * @param rightIndex
     * @return true if this user has this right, false otherwise
     */
    public boolean hasRight(int rightIndex) {
        return m_MyRights.containsKey(new Integer(rightIndex));
    }
}
