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
package com.sap.sapdb.DBMTest.environment;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.sql.SQLException;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * @author d026948
 * 
 * This test reads parameters IndependentPrograms and DependentPrograms
 * from the test configuration.
 * 
 * This test checks, whether the executables listed in the two parameters are
 * loaded from the paths that they should be loaded from.
 * 
 */
public class PathChecker extends DBMTestCase {

	private static final String INDEPPROGS = "IndependentPrograms";
	private static final String DEPPROGS = "DependentPrograms";
	
	private static final String WHICH_WINDOWS = "where";
	private static final String WHICH_OTHER   = "which";
	private String m_Which = null;
	
	private boolean m_IsWindows = false;
	
	class StreamReader extends Thread {
		private InputStream m_Stream = null;
		private ByteArrayOutputStream m_BOS = new ByteArrayOutputStream();
		private byte[] m_Data = new byte[1024];
		
		private boolean m_IsStopped = false;
		
		public void stopReading() {
			m_IsStopped = true;
		}
		
		public StreamReader( InputStream stream) {
			m_Stream = stream;
		}
		
		public void run() {
			int byteCount;
			try {
				while ((byteCount = m_Stream.read(m_Data, 0, m_Data.length)) > -1) {
	                m_BOS.write(m_Data, 0, byteCount);
	                if( m_IsStopped )
	                	return;
	            }
			}
			catch( IOException ioe ) {
	            addMessage(
	                    "error reading stream " + m_Stream.toString(),
	                    TestProtocol.TRACE,
	                    "something went wrong...",
	                    ioe);
			}
		}

		public String getData() {
			if( m_BOS.size() == 0 )
				return "(no data)";
			else
				return m_BOS.toString();
		}
	}

	
	/**
	 * @throws SQLException
	 * @throws TestDatabaseException
	 */
	public PathChecker() throws SQLException, TestDatabaseException {
		super();
		m_IsWindows = System.getProperty("os.name").toLowerCase().startsWith("win");
		
		if( m_IsWindows )
			m_Which = WHICH_WINDOWS;
		else
			m_Which = WHICH_OTHER;
	}

	
	/**
	 * 
	 * @param imageName the executable's name without any path component
	 * @return path without filename, no trailing separator
	 */
	private File getExecutedImagePath(String imageName ) {
	
		StreamReader errorReader = null;
		StreamReader outputReader = null;
		Process process = null;
		int exitValue = -9999;
		
		if( m_IsWindows )
			imageName += ".exe";
		
		try {
			String[] processArgs = new String[2];
			processArgs[0] = m_Which;
			processArgs[1] = imageName;
			
			process = Runtime.getRuntime().exec(processArgs);
			errorReader = new StreamReader(process.getErrorStream());
			outputReader = new StreamReader(process.getInputStream());
			errorReader.start();
			outputReader.start();
			long startTime = System.currentTimeMillis();
			boolean notTerminated = true;
			while( notTerminated ) {
				try {
					exitValue = process.exitValue();
					notTerminated = false;
				}
				catch( IllegalThreadStateException itse ) {
					// process still running
				}
				if( System.currentTimeMillis() - startTime > 5*60*1000 ) // 5 minutes
					process.destroy();
				else
					Thread.sleep(100);
			}
	
			if( errorReader.isAlive() )
				errorReader.stopReading();
			if( outputReader.isAlive() )
				outputReader.stopReading();
			
			errorReader.join();
			outputReader.join();
		}
		catch (Exception e) {
            addMessage(
                    "error executing path checker",
                    TestProtocol.WARNING,
                    "something went wrong...",
					e,
					"return code: " + exitValue + "\n" +
					"errOut: " + (errorReader==null?"":errorReader.getData()) + "\n" +
					"stdOut: " + (outputReader==null?"":outputReader.getData()));
    		return null;
		}
		
		if( exitValue != 0 ) {
            addMessage(
                    "path checker returned with error",
                    TestProtocol.WARNING,
                    "something went wrong...",
					"return code: " + exitValue + "\n" +
					"errOut: " + errorReader.getData() + "\n" +
					"stdOut: " + outputReader.getData());
    		return null;
		}
		
		// execution was successful
		String path = null;
		if( m_IsWindows) {
			String occurrences[] = outputReader.getData().split("\n");
			//for( int i=0; i < occurrences.length; ++i )
			//	System.out.println(">> " + occurrences[i]);
			if( occurrences.length < 1 ) {
	            addMessage(
	                    "error extracting path",
	                    TestProtocol.WARNING,
	                    path);
			}
			path = occurrences[0];
		}
		else {
			path = outputReader.getData();
		}
		try {
			path = path.substring(0, path.lastIndexOf(File.separatorChar));
		}
		catch( IndexOutOfBoundsException ioobe ) {
            addMessage(
                    "error extracting path",
                    TestProtocol.WARNING,
                    path,
					ioobe);
            return null;
		}
		return new File(path);
	}
	
	
	/* (non-Javadoc)
	 * @see com.sap.sapdb.DBMTest.base.DBMTestCase#runDBMTest()
	 */
	protected void runDBMTest() {
		String indepProgPathViaDBM = "nothing";
		String depProgPathViaDBM = "nothing";
		
		// get the paths from DBM
		boolean pathOkay = m_DBM.execCmd("dbm_getpath IndepProgPath", DBMUtility.OK_OK);
		indepProgPathViaDBM = m_DBM.getAnswer().trim(); 
	    if( !pathOkay ) {
	    	addMessage(
                    "error getting IndepProgPath from DBMServer",
                    TestProtocol.WARNING,
					indepProgPathViaDBM);
	    }

		pathOkay = m_DBM.execCmd("dbm_version", DBMUtility.OK_OK);
		depProgPathViaDBM = m_DBM.getAnswer();
	    if( pathOkay ) {
	    	boolean pathFound = false;
	    	String lines[] = depProgPathViaDBM.split("\n");
	    	for( int i=0; !pathFound && i < lines.length; ++i ) {
	    		if( lines[i].startsWith("INSTROOT")) {
	    			try {
	    				depProgPathViaDBM = lines[i].substring(lines[i].indexOf('=')+1).trim();
	    				pathFound = true;
	    			}
	    			catch( IndexOutOfBoundsException iob ) {
	    		    	addMessage(
	    	                    "error getting depProgPath from DBMServer",
	    	                    TestProtocol.WARNING,
	    						depProgPathViaDBM,
								iob);
	    		    	pathOkay = false;
	    			}
	    		}
	    	}
	    }
	    
	    if( !pathOkay ) {
	    	addMessage(
                    "error getting depProgPath from DBMServer",
                    TestProtocol.WARNING,
					depProgPathViaDBM);
	    }

	    // check the independent programs
	    String independentPrograms[] = super.getParameterString(INDEPPROGS, "?").split(",");
	    if( !independentPrograms[0].equals("?") )
		    for( int i=0; i < independentPrograms.length; ++i) {
		    	addMessage("checking independent program", TestProtocol.TRACE, independentPrograms[i]);

		    	File imagePath = getExecutedImagePath(independentPrograms[i]);
				if( imagePath == null)
					continue;
			    
				if( (m_IsWindows && !imagePath.getPath().toLowerCase().startsWith(indepProgPathViaDBM.toLowerCase())) ||
				    (!m_IsWindows && !imagePath.getPath().startsWith(indepProgPathViaDBM)) ) {
			    	addMessage(
		                    "executable " + independentPrograms[i] + " would be executed from \"" + imagePath.getPath() + "\"",
		                    TestProtocol.WARNING,
							"correct path would be \"" + indepProgPathViaDBM + File.separator + "(bin|pgm)\"");
			    }
		    }
	    
	    // check the dependent programs
	    String dependentPrograms[] = super.getParameterString(DEPPROGS, "?").split(",");
	    if( !dependentPrograms[0].equals("?") )
		    for( int i=0; i < dependentPrograms.length; ++i) {
		    	addMessage("checking dependent program", TestProtocol.TRACE, dependentPrograms[i]);
		    	
		    	File imagePath = getExecutedImagePath(dependentPrograms[i]);
				if( imagePath == null)
					continue;
			    
				if( (m_IsWindows && !imagePath.getPath().toLowerCase().startsWith(depProgPathViaDBM.toLowerCase())) ||
					(!m_IsWindows && !imagePath.getPath().startsWith(depProgPathViaDBM)) ) {
			    	addMessage(
		                    "executable " + dependentPrograms[i] + " would be executed from \"" + imagePath.getPath() + "\"",
		                    TestProtocol.WARNING,
							"correct path would be \"" + depProgPathViaDBM + File.separator + "(bin|pgm)\"");
			    }
		    }
	}
}
