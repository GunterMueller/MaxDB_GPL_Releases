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

import com.sap.sapdb.testframe.utilities.Execute;

/**
 * @author d026948
 *
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
public class EventDispatcher extends Execute {

	private boolean m_StopSignaled = false;
	
	/**
	 * @param commandLineTokens
	 * @param outputBuffer
	 */
	public EventDispatcher(String[] commandLineTokens, StringBuffer outputBuffer) {
		super(commandLineTokens, null, outputBuffer);
		// TODO Auto-generated constructor stub
	}

	public Thread execAsync() {
		final EventDispatcher me = this;
		Thread execThread = new Thread() {
			public void run() {
				me.exec();
				while( !m_StopSignaled && me.isAlive() ) {
					try {
						Thread.sleep(100);
					}
					catch( InterruptedException ie ) {
						m_StopSignaled = true;
					}
				}
			}
		};
		execThread.start();
		return execThread;
	}

	public void stopExec() {
		m_StopSignaled = true;
	}
	
}
