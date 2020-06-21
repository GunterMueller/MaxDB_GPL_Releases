/*!
	\file    DBMTestCase.java
	\author  TiloH
	\ingroup DBM Server tests
	\brief   

    ========== licence begin  GPL
    Copyright (c) 2004-2006 SAP AG

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

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.StringWriter;
import java.io.PrintWriter;
import java.sql.SQLException;
import java.text.SimpleDateFormat;
import java.util.Enumeration;
import java.util.GregorianCalendar;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.zip.ZipOutputStream;

import com.sap.dbtech.powertoys.DBM;
import com.sap.dbtech.rte.comm.RTEException;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.DBMTest.util.user.DBMUser;
import com.sap.sapdb.DBMTest.util.user.DBMUserManagementException;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCase;

/**
 * @author d025561
 *
 * A test case providing a protected database object and a protected
 * DBMUtility object for derived test cases.
 * 
 * Subclasses must implement the runDBMTest() method instead of the run method.
 * 
 */
public abstract class DBMTestCase extends TestCase {
	
	protected TestDatabaseSap m_DB = null;
	protected DBMUtility m_DBM = null;
	private long m_InitialErrFileSize = 0L;
	private String m_DBHostName = null;
	
	private StringBuffer m_Messages = new StringBuffer();
	private static final String m_OverviewPrefix="    DBM messages: ";

	/**
	 * @throws java.sql.SQLException
	 * @throws com.sap.sapdb.testframe.driver.TestDatabaseException
	 */
	public DBMTestCase() throws SQLException, TestDatabaseException {
		super();
	}
    
    public final void run() {
        m_DB  = (TestDatabaseSap) super.getDatabase();
        m_DBHostName = getDBHostName();
        
        // get initial size of err file (grows when DBMServers crash), remove old dmp file
        try {
        	File indepDataWrk = new File( m_DB.getIndepDataPath() + File.separator + "wrk" );
        	File errFile = new File( indepDataWrk, "dbmsrv_" + m_DBHostName + ".err" );
        	m_InitialErrFileSize = errFile.length();
        	File dumpFile = new File( indepDataWrk, "dbmsrv_" + m_DBHostName + ".dmp" );
        	if( dumpFile.exists() )
        		dumpFile.delete();
        }
        catch( TestDatabaseException tde ) {
            addDBMMessage(
                    "could not calculate initial size of dbmsrv_<hostname>.err  or remove old dbmsrv_<hostname>.dmp file",
                    TestProtocol.ERROR,
                    tde.getMessage());
        }

        // remove old zipfile if it exists
        File oldZipFile = null;
        try {
            oldZipFile = getZipFile(); 
            if( oldZipFile.exists() )
                oldZipFile.delete();
        }
        catch( TestDatabaseException tde ) {
            String canonPath = null;
            try {
                canonPath = oldZipFile.getCanonicalPath();
            } catch (IOException ioe) {
                canonPath = oldZipFile.getAbsolutePath();
            }
            addDBMMessage(
                    "could not delete old zip " + canonPath,
                    TestProtocol.ERROR,
                    tde.getMessage());
            return;
        }

        // create utility objects
        DBM theDBMSession = null;
        try {
            theDBMSession = 
                DBM.dbDBM(m_DB.getHost() + ':' + m_DB.getPort(), m_DB.getName());
            m_DBM = new DBMUtility(m_DB, this, theDBMSession);

            // try to logon with the DBM user from testframe
            String logonCmd =
                    "user_logon " +
                    m_DB.getDBMUser()+
                    ',' +
                    m_DB.getDBMPassword();
           
            theDBMSession.cmd(logonCmd);
        }
        catch (Exception e) {
            addDBMMessage(
                "preparation",
                TestProtocol.ERROR,
                "could not create DBM session, abort test.",
                e);
            return;
        }

        // initialize user management 
        try {
            DBMUser.setCommmonInformation(theDBMSession, m_DB, this);
        }
        catch( DBMUserManagementException dbmue ) {        
            addDBMMessage(
                "preparation",
                TestProtocol.ERROR,
                "could not create DBM session, abort test.",
                dbmue);
        }

        
        // call test implementation in subclass
        try {
        	runDBMTest();
        }
        catch(Exception e) {
        	addDBMMessage(
        	    this.getClass().getName(),
				TestProtocol.ERROR,
                "uncaught exception in "+this.getClass().getName()+".runDBMTest()",
                e);
        }
        
        try {
            // remove any created users from the test database
            DBMUser.removeCreatedUsers();
            theDBMSession.release();
        }
        catch(RTEException rtee) {
            addDBMMessage(
                    "post-test cleanup",
                    TestProtocol.ERROR,
                    "release session",
                    rtee);
        }
        catch( DBMUserManagementException dbmue ) {
            addDBMMessage(
                "post-test user cleanup",
                TestProtocol.ERROR,
                "something went wrong...",
                dbmue);
        }
        
        // if there is anything new in the err file or if there is a memory dump, we have this uploaded
        try {
        	final File indepDataWrk = new File( m_DB.getIndepDataPath() + File.separator + "wrk" );
        	class ErrorFileNameFilter implements FilenameFilter {
        		public ErrorFileNameFilter() {
        			m_AcceptedFileName = new String("dbmsrv_") + m_DBHostName + "."; 
        		}
        		public boolean accept( File folder, String fileName ) {
        			// a file is accepted, if its either a DBMServer dump or a err file
        			// that has grown
        			boolean accepted = false; 
        			if( fileName.equals(m_AcceptedFileName + "err") ) {
        				File file = new File(folder, fileName);
        				accepted = file.length() > DBMTestCase.this.m_InitialErrFileSize;
        			}
        			else if( fileName.equals(m_AcceptedFileName + "dmp") )
        				accepted = true;
        			return accepted;
        		}
        		private String m_AcceptedFileName;
        	}

        	File[] errorFiles = indepDataWrk.listFiles(new ErrorFileNameFilter());
        	if( errorFiles.length > 0 ) {
        		// there are files to zip
            	File backupFile = backupZipFile();
        		ZipOutputStream zos = new ZipOutputStream(new FileOutputStream(getZipFile(), true));
            	if( backupFile != null ) {
            		// if there is zip file already, copy its content
            		ZipFile oldZip = new ZipFile(backupFile);
            		Enumeration entries = oldZip.entries();
            		while (entries.hasMoreElements()) {
						ZipEntry entry = (ZipEntry) entries.nextElement();
						zos.putNextEntry(entry);
						BufferedInputStream bis = new BufferedInputStream(oldZip.getInputStream(entry));          			
	                    int byteCount;
	                    byte[] data = new byte[1024];                        
	                    while ((byteCount = bis.read(data, 0, data.length)) > -1) {
	                        zos.write(data, 0, byteCount);
	                    }
					}
            		oldZip.close();
            		backupFile.delete();
            	}

        		// now add the new files
        		int errorFilesLength = errorFiles.length;
            	while( errorFilesLength > 0 ) {
            		File errorFile = errorFiles[errorFilesLength-1]; 
        			ZipEntry entry = new ZipEntry(errorFile.getName());
                    zos.putNextEntry(entry);
                    BufferedInputStream bis = new BufferedInputStream(new FileInputStream(errorFile));
                    int byteCount;
                    byte[] data = new byte[1024];                        
                    while ((byteCount = bis.read(data, 0, data.length)) > -1) {
                        zos.write(data, 0, byteCount);
                    }
	        		errorFile.delete(); 
					--errorFilesLength;
	        	}
	        	zos.close();
        	}
        }
    	catch( IOException ioe ) {
            addDBMMessage(
                    "error zipping stacktrace and/or memory dump",
                    TestProtocol.ERROR,
                    "something went wrong...",
					ioe);
    	}
        catch (TestDatabaseException tde ) {
            addDBMMessage(
                    "error zipping stacktrace and/or memory dump",
                    TestProtocol.ERROR,
                    "something went wrong...",
					          tde);
        }
        
        //if test failed, report all errors/infos/traces reported in the test (do not use addDBMMesage here!)
        if(0!=getErrorCounter())
        {
        	addMessage("    test overview    ", TestProtocol.TRACE, m_Messages.toString());
        }
    }

	private File backupZipFile() throws TestDatabaseException {
    	File original = getZipFile();
    	if( original.exists() ) {
    		File backup = new File(original.getParentFile(), "tmp_" + original.getName());
			backup.delete();
			if( !original.renameTo(backup) ) {
	            addMessage(
	                    "error zipping stacktrace and/or memory dump",
	                    TestProtocol.TRACE,
	                    "could not rename " + original.getName() + " to " + backup.getName());
	            return null;
			}
			else
				return backup;
    	}
    	else
    		return null;
    }
    
    private String getDBHostName() {
   	
    	StringBuffer hostNameBuffer = new StringBuffer(m_DB.getHostName().toUpperCase());
    	int firstdot = hostNameBuffer.indexOf(".");
    	if( firstdot == -1 )
    		return hostNameBuffer.toString();
    	else
    		return hostNameBuffer.substring(0, firstdot);
    }
    
    /**
     * DBM tests must implement this method instead of the run() method
     */
    protected abstract void runDBMTest() throws Exception;

    /**
     * Subclasses can create whatever file-type object from the returned file
     * object to store information in. This file will be uploaded by the test
     * framework, if the test fails.
     * 
     * The file name is computed automatically.
     * 
     * Creating a ZipOutputStream for storing multiple files in this single
     * file makes good sense, if a FileOutputStream is not sufficient. 
     * 
     * @return File object to store test information
     * @throws TestDatabaseException
     */
    protected File getZipFile() throws TestDatabaseException {
        return new File(m_DB.getRunDir() + 
                            File.separator +
                            this.getClass().getName() +
                            ZIP_FILE_POSTFIX);
    }

    private static final String ZIP_FILE_POSTFIX = "_files.zip";

    /**
     * called by the test framework. If a file was created during the test, this
     * is the place where the test framework takes it.
     */
    public final String getLogFile() {
    	
    	if(0==getErrorCounter()) //store additional log files within WebDAV only, if any errors occurred during the test 
			return null;
		else
	        try {
	            File upLoadee = getZipFile();
	            if( upLoadee.exists() ) {
	                addMessage(getTestId(), TestProtocol.INFO, upLoadee.getCanonicalPath());
	                return upLoadee.getCanonicalPath();
	            }
	            else
	                return null;
	        }
	        catch( Exception e ) {
	            // no so good...
	            return null;
	        }
    }
    
    /*
     * Returns the source code line where a caller has been placed this function call 
     */
    public final int  __LINE__()
    {
    	String szPosition = getPosition();

    	// look for colon
    	szPosition = szPosition.substring(szPosition.indexOf(":") + 1);
    	// look for closing bracket
        szPosition = szPosition.substring(0, szPosition.indexOf(")"));
    	
    	return Integer.parseInt(szPosition);
    } // end __LINE__
    
    /*
     * Returns the source code file where a caller has been placed this function call
     */
    public final String  __FILE__()
    {
    	String szPosition = getPosition();

    	// look for open bracket
    	szPosition = szPosition.substring(szPosition.indexOf("(") + 1);
    	// look for colon
        szPosition = szPosition.substring(0, szPosition.indexOf(":"));
    	
    	return szPosition;
    } // end __LINE__
    
    /*
     * Gets the third line from the current stackbacktrace
     * 
     * The number of skipped lines is very important because of this private function
     * will be used by the functions __LINE__ and __FILE__ of this class and these
     * functions must be skipped too to from the stackbacktrace because of the caller
     * of __LINE__ or __FILE__ wants his own position and not a position inside this 
     * module.
     *  
     * So we have to skip this function getPosition() and the function __LINE__() or __FILE__()
     * to reach the function of the caller.
     * 
     */
    private final String getPosition()
    {
        // local write objects
        StringWriter oStringW = new StringWriter();
        PrintWriter  oPrintW  = new PrintWriter(oStringW);
      
        // generate a exception
        new Exception("").printStackTrace(oPrintW);
        oPrintW.close();

        // extract the string
        String szPosition = oStringW.toString();
   	
        // locate the line
        for(int i = 0; i < 3; ++i) {
        	szPosition = szPosition.substring(szPosition.indexOf("at ") + 3);
        } // end or

        // cut the end 
        szPosition = szPosition.substring(0, szPosition.indexOf("at "));
        
        return szPosition;
    } // end getPosition
	
    private void addMessageHeader(StringBuffer message, String messageID, char type, Exception e)
    {
    	message.append(m_OverviewPrefix);
    	
		switch(type)
		{
			case TestProtocol.INFO:  message.append("INFO     ");break;
			case TestProtocol.TRACE: message.append("TRACE    ");break;
			case TestProtocol.ERROR: message.append("ERROR    ");break;
			default:                 message.append("UNKNOWN  ");
		}

		message.append(this.getTestId());
		message.append(" ");
		message.append(messageID);
		message.append(" ");
		GregorianCalendar now=new GregorianCalendar();
		
		SimpleDateFormat format=new SimpleDateFormat("dd.MM.yyyy HH:mm:ss");        
		
		message.append(format.format(now.getTime()));
		
		if(null!=e)
			message.append(" "+e.getClass());

		message.append("\n");
    }
    
    private void addMessageInformation(StringBuffer message, String messageID, char type, String information)
    {
    	message.append(m_OverviewPrefix);
		message.append(LinebreakConverter.convertTo(information, "\n"+m_OverviewPrefix));
		message.append("\n");
    }
    
    private void addMessageTrailer(StringBuffer message)
    {
    	message.append(m_OverviewPrefix);
		message.append("\n");
		
		message.append(m_OverviewPrefix);
		message.append("------------------------------------------------------------------------------\n");
    }
    
	public void addDBMMessage(String messageID, char type, String message)
	{
		StringBuffer newMessage = new StringBuffer();
		
		addMessageHeader(newMessage,  messageID, type, null);
		addMessageInformation(newMessage, messageID, type, message);
		addMessageTrailer(newMessage);
		
		m_Messages.append(LinebreakConverter.convertToOSSpecificLinebreaks(newMessage.toString()));

		super.addMessage(messageID, type, message);
	}

	public void addDBMMessage(String messageID, char type, String message, Exception e)
	{
		StringBuffer newMessage = new StringBuffer();

		addMessageHeader(newMessage, messageID, type, e);
		
		String completeMessage=null;
		
		if(e!=null)
		{
			ByteArrayOutputStream os=new ByteArrayOutputStream();
			PrintWriter pw=new PrintWriter(os, true);

			e.printStackTrace(pw);
			completeMessage= " - "+message+"\n !!! "+e.getMessage()+"\nTraceback (innermost first) {"+os.toString()+" } End of traceback";
	    }
		else
			completeMessage=message;
		
		addMessageInformation(newMessage, messageID, type, completeMessage);
		addMessageTrailer(newMessage);
		
		m_Messages.append(LinebreakConverter.convertToOSSpecificLinebreaks(newMessage.toString()));

		super.addMessage(messageID, type, message, e);
	}
}
