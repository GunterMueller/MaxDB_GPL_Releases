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

  module: Studio_Functions.hpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: SQLStudio

  description:  different helper functions
				

  see also:		/


************************************************************************/

#ifndef STUDIO_FUNCTIONS_HPP
#define STUDIO_FUNCTIONS_HPP

#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"


/*!-------------------------------------------------------------------------
	function:     SimplifyWS
	description:  Simplifies all multiple occurences of white spaces   
                  (blanks, tabs and CR/LFs) in the given input string. 
                  to one single blank.
	arguments:	  [in] sInString, input string
				  [out] sOutString, output string
				  
				  
	return value: /
--------------------------------------------------------------------------*/
void SimplifyWS (const Tools_DynamicUTF8String &sInString,Tools_DynamicUTF8String &sOutString);


#endif
