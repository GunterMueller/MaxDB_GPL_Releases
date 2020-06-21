/*!
	\file    BackgroundProcess.java
	\author  TiloH
	\ingroup DBM Server tests
	\brief   A class creating a thread, that starts a process in background
	
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

import java.io.File;

import com.sap.sapdb.testframe.utilities.Execute;

public class BackgroundProcess extends Thread
{
	Execute m_Process=null;
	String  m_RunDirectory=null;
	
	public BackgroundProcess(
			String commandLine,
			String[] environment,
			String runDirectory) {
		super();
		m_Process=new Execute(commandLine, environment);
		m_RunDirectory=runDirectory;
		start();
	}

	public void run()
	{
		m_Process.exec(new File(m_RunDirectory));
	}
}
