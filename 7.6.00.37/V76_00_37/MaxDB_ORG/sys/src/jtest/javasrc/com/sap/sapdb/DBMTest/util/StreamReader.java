/*!
	\file    StreamReader.java
	\author  TiloH
	\ingroup DBM Server tests
	\brief   

    ========== licence begin  GPL
    Copyright (c) 2005-2006 SAP AG

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
package com.sap.sapdb.DBMTest.util;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.testframe.driver.TestProtocol;

/**
 * @author d025561
 *
 * MarcW's StreamReader class for everybody 
 */
public class StreamReader extends Thread
{
	private InputStream m_Stream = null;
	private ByteArrayOutputStream m_BOS = new ByteArrayOutputStream();
	private byte[] m_Data = new byte[1024];
	
	private boolean m_IsStopped = false;
	
	private DBMTestCase m_testCase=null;

	public void stopReading() {
		m_IsStopped = true;
	}
	
	public StreamReader( InputStream stream, DBMTestCase testCase ) {
		m_Stream = stream;
		m_testCase=testCase;
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
            m_testCase.addDBMMessage(
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
