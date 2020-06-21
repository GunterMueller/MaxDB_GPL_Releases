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
package com.sap.sapdb.DBMTest.eventing.client.util;

import java.io.File;
import java.util.Vector;

/**
 * @author d026948
 *
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
public class CommandLine {

	public static final String COMMAND_ADD    = "add";
	public static final String COMMAND_DELETE = "delete";
	public static final String COMMAND_LIST   = "list";
	public static final String COMMAND_START  = "start";	
	public static final String COMMAND_STATE  = "state";
	public static final String COMMAND_STOP   = "stop";

	private static final String NO_HOST = "no host";
	
	private static File m_AbsoluteFolderPath = null;
	
	private String m_Command = null;
	private File m_ConfigFile = null;
	private File m_LogFile = null;
	public final String m_UserPwd;
	public final String m_Database;
	public final String m_Host;
	private String m_EventNameCondition = null;
	private String m_EventAction = null;
	private int m_EntryIndex = -1;
	private int m_DispatcherID = -1;

	private static final boolean m_IsWindows =
		System.getProperty("os.name").toLowerCase().startsWith("win");
	
	public static void initialize(String absoluteFolderPath) {
		m_AbsoluteFolderPath = new File(absoluteFolderPath);
	}
	
	public static void cleanup() {
		// delete the folder that we created
		if( null != m_AbsoluteFolderPath && m_AbsoluteFolderPath.exists() ) {
			File[] files = m_AbsoluteFolderPath.listFiles();
			for(int i=0; i<files.length; files[i++].delete()); 
			m_AbsoluteFolderPath.delete();
		}
	}
	
	public CommandLine(String userPwd, String database, String host) {
		m_UserPwd = userPwd;
		m_Database = database;
		m_Host = host;
	}

	public CommandLine(String userPwd, String database) {
		m_UserPwd = userPwd;
		m_Database = database;
		m_Host = NO_HOST;
	}

	public void setCommand(String command) {
		m_Command = command;
	}

	public String getCommand() {
		return m_Command;
	}

	public void setConfigFile(String configFileName) {
		m_ConfigFile = new File(m_AbsoluteFolderPath.getAbsolutePath() + File.separator + configFileName);
	}

	public void setLogFile(String logFileName) {
		m_LogFile = new File(m_AbsoluteFolderPath.getAbsolutePath() + File.separator + logFileName);
	}
	
	public void setEventNameCondition( String eventNameCondition ) {
		m_EventNameCondition = eventNameCondition;
	}
	
	public void setEventAction( String eventAction ) {
		m_EventAction = eventAction;
	}
	
	public void setEntryIndex(int index) {
		m_EntryIndex = index;
	}

	public void setDispatcherID(int dispatcherID) {
		m_DispatcherID = dispatcherID;
	}
	
	public String[] toTokenArray() {
		Vector commandLineVector = new Vector();
		commandLineVector.add(m_IsWindows?"dbmevtdisp.exe":"dbmevtdisp");
		
		if( m_Command.equals(COMMAND_ADD) ) {
			commandLineVector.add(COMMAND_ADD);
			
			commandLineVector.add(m_ConfigFile.getAbsolutePath());
			
			commandLineVector.add("Name");
			commandLineVector.add("==");
			commandLineVector.add(m_EventNameCondition);

			commandLineVector.add("Command");
			commandLineVector.add("==");
			commandLineVector.add(m_EventAction);
		}
		else if( m_Command.equals(COMMAND_DELETE) ) {
			commandLineVector.add(COMMAND_DELETE);
			
			commandLineVector.add(Integer.toString(m_EntryIndex));
			
			commandLineVector.add(m_ConfigFile.getAbsolutePath());
		}
		else if ( m_Command.equals(COMMAND_LIST) ) {
			commandLineVector.add(COMMAND_LIST);
			
			commandLineVector.add(m_ConfigFile.getAbsolutePath());
		}
		else if ( m_Command.equals(COMMAND_START) ) {
			commandLineVector.add(COMMAND_START);
			
			commandLineVector.add(m_ConfigFile.getAbsolutePath());
			
			commandLineVector.add("-l");
			commandLineVector.add(m_LogFile.getAbsolutePath());
			
			commandLineVector.add("-u");
			commandLineVector.add(m_UserPwd);
			
			commandLineVector.add("-d");
			commandLineVector.add(m_Database);
		}
		else if ( m_Command.equals(COMMAND_STATE) ) {
			commandLineVector.add(COMMAND_STATE);

			commandLineVector.add("-u");
			commandLineVector.add(m_UserPwd);
			
			commandLineVector.add("-d");
			commandLineVector.add(m_Database);
		}
		else if ( m_Command.equals(COMMAND_STOP) ) {
			commandLineVector.add(COMMAND_STOP);
			
			commandLineVector.add(Integer.toString(m_DispatcherID));
			
			commandLineVector.add("-u");
			commandLineVector.add(m_UserPwd);
			
			commandLineVector.add("-d");
			commandLineVector.add(m_Database);
		}
		
		String[] commandLineTokens = new String[commandLineVector.size()];
		commandLineVector.toArray(commandLineTokens);
		return commandLineTokens;
	}
}
