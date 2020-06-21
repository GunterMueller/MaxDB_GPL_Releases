/*!
  -----------------------------------------------------------------------------
  module: SecondDB.java
  -----------------------------------------------------------------------------

  ========== licence begin  GPL
  Copyright (c) 2002-2006 SAP AG

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
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import com.sap.sapdb.testframe.driver.TestProtocol;
import com.sap.sapdb.testframe.testcase.TestCase;


public class Zipper extends com.sap.sapdb.testframe.utilities.Zipper{

	String          m_Name=null;
	ZipOutputStream m_ZipOutputStream=null;
	TestCase		m_TestCase=null;
	
	public Zipper(String nameOfZipFile, TestCase testcase) {
		m_Name=nameOfZipFile;

		try {
			m_ZipOutputStream = new ZipOutputStream(new FileOutputStream(m_Name));
			m_ZipOutputStream.setLevel(9);
		}
		catch(FileNotFoundException e) {
			if(testcase!=null)
				testcase.addMessage("    ziping into file "+m_Name+"    ", TestProtocol.ERROR,
                    "FileNotFoundException caught while creating zip file "+m_Name+":\n"+ e.getMessage());
		}
		
		m_TestCase=testcase;
	}
	
	public boolean addDirectory(String directoryName)
	{
		boolean rc=true;
	    File outputFolder = new File(directoryName);
	    
	    if( outputFolder.exists() ) {
	    	File[] files = outputFolder.listFiles();
	    	
	    	if( files.length > 0 ) {
    			for( int fileCount=0; fileCount<files.length; ++fileCount) {
    				if(!addFile(files[fileCount].getName()))
    					rc=false;
    			}
	    	}
	    }
	    return rc;
	}
	
	public boolean addFile(String fileName) {
		boolean rc=true;
		try {
			m_ZipOutputStream.putNextEntry(new ZipEntry(fileName));
			
			byte[] data = new byte[1024];
			int byteCount;
			BufferedInputStream bis =new BufferedInputStream(new FileInputStream(fileName));
			
			while ((byteCount = bis.read(data, 0, data.length)) > -1)
				m_ZipOutputStream.write(data, 0, byteCount);
	 
			bis.close();
		}
		catch( IOException ioe ) {
			rc=false;
			if(m_TestCase!=null)
				m_TestCase.addMessage("    ziping into file "+m_Name+"    ", TestProtocol.ERROR,
						"IOException caught while adding "+fileName+" to zip file "+m_Name+":\n" + ioe.getMessage());
        }
		return rc;
	}
	
	public boolean addFileAs(String fileName, String fileNameInArchive)
	{
		boolean rc=true;
		File original=new File(fileName);

		if(original.exists())
		{
			try {
				
				BufferedInputStream bis=new BufferedInputStream(new FileInputStream(fileName));
				BufferedOutputStream bos=new BufferedOutputStream(new FileOutputStream(fileNameInArchive));
				
				byte[] data = new byte[1024];
				int byteCount;
				
				while ((byteCount = bis.read(data, 0, data.length)) > -1)
					bos.write(data, 0, byteCount);

				bis.close();
				bos.close();
			}
			catch( IOException ioe ) {
				rc=false;
				if(m_TestCase!=null)
					m_TestCase.addMessage("    ziping into file "+m_Name+"    ", TestProtocol.ERROR,
							"IOException caught while adding "+fileName+" to zip file "+m_Name+":\n" + ioe.getMessage());
	        }
			
			if(rc)
				rc=addFile(fileNameInArchive);
			
			File copy=new File(fileNameInArchive);
			
			copy.delete();
		}
		
		return rc;
	}
	
	public boolean close() {
		boolean rc=true;
		try {
			m_ZipOutputStream.close();
		}
		catch( IOException ioe ) {
			rc=false;
			if(m_TestCase!=null)
				m_TestCase.addMessage("    ziping into file "+m_Name+"    ", TestProtocol.ERROR,
						"IOException caught while closing zip file "+m_Name+":\n" + ioe.getMessage());
        }
		
		return rc;
	}
}
