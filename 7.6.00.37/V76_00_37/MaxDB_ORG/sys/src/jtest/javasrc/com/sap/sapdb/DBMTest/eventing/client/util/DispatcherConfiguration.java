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

import java.util.Properties;

/**
 * @author d026948
 *
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
public class DispatcherConfiguration extends Properties {

	 public String getProperty(String key) {
	 	String value = super.getProperty(key);
	 	if( null == value ) {
	 		String keyPrefix = key.substring(0, key.indexOf('='));
	 		value = super.getProperty(keyPrefix.trim());
	 		if( null != value ) {
	 			value = keyPrefix + "=" + value;
	 			int i=0;
	 			boolean notFound = true;
	 			int bracketCount = 0;
	 			//the separator between key and value must be ouside of any brackets
	 			while( i < value.length() && notFound ) {
	 				if( '=' == value.charAt(i) && 0 == bracketCount )
	 					return value.substring(i+1);
	 				else if('(' == value.charAt(i) ) 
	 					++bracketCount;
	 				else if( ')' == value.charAt(i) )
	 					--bracketCount;
	 				++i;
	 			}
	 		}
	 	}
		return value;
	 }
      
	 public String getProperty(String key, String defaultValue) {
	 	String value = getProperty(key);
	 	if( null == value )
	 		return defaultValue;
	 	else
	 		return value;
	 }
      
/*
	public static void main(String[] args) {
		DispatcherConfiguration test = new DispatcherConfiguration();
		
		try {
			test.load(new FileInputStream("c:\\add.cfg"));	
		} catch (Exception e) {
			e.printStackTrace();
		}
		String prop = test.getProperty("0000000000.((Name == \"DATABASEFULL\"))");
		System.out.println(prop);
	}
	*/
}
