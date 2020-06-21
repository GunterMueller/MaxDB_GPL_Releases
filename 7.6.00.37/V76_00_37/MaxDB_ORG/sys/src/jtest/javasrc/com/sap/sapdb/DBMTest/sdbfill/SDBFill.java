/*!
	\file    SDBFill.java
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

package com.sap.sapdb.DBMTest.sdbfill;

import java.io.*;
import java.sql.SQLException;

import com.sap.sapdb.testframe.driver.TestDatabaseException;
import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.driver.TestDatabaseSap;

import com.sap.sapdb.DBMTest.base.DBMTestCase;
import com.sap.sapdb.DBMTest.util.DBMUtility;
import com.sap.sapdb.DBMTest.util.StreamReader;

/**
 * @author d025561
 *
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
public class SDBFill extends DBMTestCase {

	/**
	 * @throws java.sql.SQLException
	 * @throws com.sap.sapdb.testframe.driver.TestDatabaseException
	 */
	public SDBFill() throws SQLException, TestDatabaseException {
		super();
		// TODO Auto-generated constructor stub
	}

	private void callSDBFill(String call) throws IOException, InterruptedException
	{
		callSDBFill(call, 0);
	}
	
	private void callSDBFill(String call, int expectedRC) throws IOException, InterruptedException
	{
		Process p = Runtime.getRuntime().exec(call);
		
		StreamReader errorReader = new StreamReader(p.getErrorStream(), this);
		StreamReader outputReader = new StreamReader(p.getInputStream(), this);
		
		errorReader.start();
		outputReader.start();
		
		p.waitFor();

		if( errorReader.isAlive() )
			errorReader.stopReading();
		if( outputReader.isAlive() )
			outputReader.stopReading();
		
		errorReader.join();
		outputReader.join();
		
		if(expectedRC==p.exitValue())
		{
			this.addDBMMessage(
	    			"call of '"+call+"'.",
	                TestProtocol.INFO,
					"'"+call+"' did work. Exit code was "+p.exitValue()+".");
		}
		else
	    {
	    	this.addDBMMessage(
	    			"call of '"+call+"'.",
	                TestProtocol.ERROR,
	                "'"+call+"' did not work. Exit code was "+p.exitValue()+".\n"+
					"sdbfill output:\n"+
					outputReader.getData()+
					"\nsdbfill error output:\n"+
					errorReader.getData());
	    }
	}
	
	public void runDBMTest()
	{
		//create one global try-catch block to handle exceptions
        try
		{		
        	if(getDatabase().isSap())
        	{
        		DBMUtility dbm=new DBMUtility(getDatabase(), this);
        		String fileSep=dbm.getSrvProperty("file.separator");

        		String sdbFill=((TestDatabaseSap)getDatabase()).getInstRoot()
				                 +fileSep+"bin"
								 +fileSep+"sdbfill ";

	        	String userAndPassword=getDatabase().getDBAUser()+" "+getDatabase().getDBAPassword();
				
	        	callSDBFill(sdbFill+getDatabase().getName()+" init 0 0 0 0 a "+userAndPassword);
	        	callSDBFill(sdbFill+getDatabase().getName()+" create 0 1000 500 1 a "+userAndPassword);
	        	callSDBFill(sdbFill+getDatabase().getName()+" append 1000 2000 10 0 a "+userAndPassword);
	        	callSDBFill(sdbFill+getDatabase().getName()+" check 0 2000 100 0 a "+userAndPassword);
	        	callSDBFill(sdbFill+getDatabase().getName()+" delete 0 2000 33 0 a "+userAndPassword);
	        	callSDBFill(sdbFill+getDatabase().getName()+" check 0 1 100 0 a "+userAndPassword, 1);
        	}
        	else
        		this.addDBMMessage(
    	    			"get Inst-Root",
    	                TestProtocol.ERROR,
    	                "database "+ getDatabase().getName()+" is no SAP database - can not determine Inst-Root");
        }
        // catch any kind of exception and write it to the log file
        catch (Exception e) {
            // standard error handler

            this.handleExceptions(e);
        } // end try catch

        // clean up, i.e. close statements and connections
        finally {
            try {
                // print all parameters from the parameter object to the protocol.
                printParametersToProtocol();
            } catch (Exception e) {
                // do nothing
            } // end try catch
        } // end finally
	}
}
