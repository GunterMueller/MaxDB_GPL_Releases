/*
    Copyright (c) 2000-2005 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

/*!**********************************************************************

  module: Studio_Functions.cpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: SQLStudio

  description:  different helper functions
				

  see also:		/


************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "SAPDB/SQLStudio/Studio_Functions.hpp"

void SimplifyWS (const Tools_DynamicUTF8String &sInString,Tools_DynamicUTF8String &sOutString)

{

	//init
	sOutString = sInString;
	
	if (sInString.Size() < 1) return;
    
	
	Tools_DynamicUTF8String::BasisElementIndex Pos;
	Tools_DynamicUTF8String strCr = "\r";
	Tools_DynamicUTF8String strLf = "\n";
	Tools_DynamicUTF8String strTab = "\t";
	Tools_DynamicUTF8String strSpace = " ";
	
	Pos = sOutString.Find(strCr);
	if(Pos != Tools_DynamicUTF8String::NPos){

		while (Pos != Tools_DynamicUTF8String::NPos) {
			
			sOutString.Erase(Pos,1);
			Pos = sOutString.Find(strCr);
		}
	}

	Pos = sOutString.Find(strLf);
	if(Pos != Tools_DynamicUTF8String::NPos){		
		
		while (Pos != Tools_DynamicUTF8String::NPos) {
			
			sOutString.Erase(Pos,1);
			Pos = sOutString.Find(strLf);
		}
	}
	Pos = sOutString.Find(strTab);
	if(Pos != Tools_DynamicUTF8String::NPos){
	
		while (Pos != Tools_DynamicUTF8String::NPos) {
			
			sOutString.Erase(Pos,1);
			Pos = sOutString.Find(strTab);
		}
	}

	Pos = sOutString.Find(strSpace);
	if(Pos != Tools_DynamicUTF8String::NPos && Pos == 0){
	
		while (Pos != Tools_DynamicUTF8String::NPos && Pos == 0) {
			
			sOutString.Erase(Pos,1);
			Pos = sOutString.Find(strSpace);
		}
	}

	Pos = sOutString.RFind(strSpace);
	if(Pos != Tools_DynamicUTF8String::NPos && Pos == sOutString.BasisSize()-1) {
	
		while (Pos != Tools_DynamicUTF8String::NPos && Pos == sOutString.BasisSize()-1) {
			
			sOutString.Erase(Pos,1);
			Pos = sOutString.RFind(strSpace);
		}
	}
}





