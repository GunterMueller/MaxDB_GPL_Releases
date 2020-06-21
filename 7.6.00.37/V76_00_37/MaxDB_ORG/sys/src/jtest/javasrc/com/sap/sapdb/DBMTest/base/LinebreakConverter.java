/*
  ========== licence begin  GPL
  Copyright (c) 1998-2006 SAP AG

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

public class LinebreakConverter {
	
	public static String convertToOSSpecificLinebreaks(String string) {
		String linebreak=System.getProperty("line.separator");
      	String convertedString=new String();

      	while(string.length()>0) {
      		int lbpart=string.indexOf('\n');
      		int lb=string.indexOf(linebreak);
      		
      		while(0<=lbpart &&
      			  0<=lb &&
      			  (lbpart>=lb || lbpart<lb+linebreak.length())) {
      			
      			int nextLBPart=(string.substring(lb+linebreak.length())).indexOf('\n');
      			int nextLB=(string.substring(lb+linebreak.length())).indexOf(linebreak);
      			
      			if(0<=nextLBPart)
      			    lbpart+=linebreak.length()+nextLBPart;
      			else
      				lbpart=-1;
      			
      			if(0<=nextLB)
      			    lb+=linebreak.length()+nextLB;
      			else
      				lb=-1;
      		}
      			
      		if(lbpart>=0) {
      			convertedString+=(string.substring(0, lbpart)+linebreak);
      			string=string.substring(lbpart+1);
      		}
      		else
      		{
      			convertedString+=string;
      			string="";
      		}
      	}

      	return convertedString;
	}
	
	public static String convertTo(String string, String newLineBreak) {
		String linebreak=System.getProperty("line.separator");
      	String convertedString=new String();

      	while(string.length()>0) {
      		int min=string.indexOf('\n');
      		int length=1;
      		int lb=string.indexOf(linebreak);

      		if(0<=lb && lb<=min) {
      			min=lb;
      			length=linebreak.length();
      		}

      		if(0<=min) {
      			convertedString+=(string.substring(0, min)+newLineBreak);
      			string=string.substring(min+length);
      		}
      		else
      		{
      			convertedString+=string;
      			string="";
      		}
      	}

      	return convertedString;
	}
}
