/*!===========================================================================

  module:       StudioTD_Util.h

  ------------------------------------------------------------------------------

  responsible:  MarkusS

  special area: SQL Studio

  description:  Definition of utility functions for the table defintion class.

  version:      7.4

  created:      2000-05-18

  updated:      2000-05-18

  ------------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG
  



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




============================================================================*/

#ifndef STUDIOTD_UTIL_H
#define StUDIOTD_UTIL_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SQLStudio/TableDefinition/StudioTD_Global.h"
#include "SAPDBCommon/SAPDB_Types.hpp"


/*===========================================================================*
 *  PROTOTYPES                                                               *
 *===========================================================================*/
/*!---------------------------------------------------------------------------
  function:     UpperCase
  description:  Convert the given string to uppercase.
  arguments:    str - String to convert
  returns:      StudioTD_String
----------------------------------------------------------------------------*/
StudioTD_String UpperCase (StudioTD_String str);
/*!---------------------------------------------------------------------------
  function:     LowerCase
  description:  Convert the given string to lowercase.
  arguments:    str - String to convert
  returns:      StudioTD_String
----------------------------------------------------------------------------*/
StudioTD_String LowerCase (StudioTD_String str);
/*!---------------------------------------------------------------------------
  function:     ProperCase
  description:  Convert the given string to proper case.
  arguments:    str - String to convert
  returns:      StudioTD_String
----------------------------------------------------------------------------*/
StudioTD_String ProperCase (StudioTD_String str);

/*!---------------------------------------------------------------------------
  function:     AtoI
  description:  Convert the given string to an integer.
  arguments:    str - String to convert
  returns:      SAPDB_Int
----------------------------------------------------------------------------*/
SAPDB_Int AtoI (StudioTD_String str);

/*!---------------------------------------------------------------------------
  function:     NextToken
  description:  Find the next token in the given string which is separated by
                the given separator.
  arguments:    str - String to search
                sep - Separator string
                pos - Start position for the search
  returns:      StudioTD_String
----------------------------------------------------------------------------*/
StudioTD_String NextToken (StudioTD_String str, StudioTD_String sep, SAPDB_UInt *pos);

/*!-------------------------------------------------------------------------
  function:     string2XML
  description:  converts xml meta characters like '"<>& to &apos; ...
                see : http://www.w3.org/TR/2000/REC-xml-20001006 
	                  2.4 Character Data and Markup
				  
  returns:      StudioTD_String
  --------------------------------------------------------------------------*/
StudioTD_String string2XML ( StudioTD_String  str  );

/*!-------------------------------------------------------------------------
function:     escapeQuoteChar
description:  replaces every occurrence of 'quotechar' in 'sToModify'
	          with two 'quotechar's .
returns:      SAPDB_ULong (number of replaced occurrences)
--------------------------------------------------------------------------*/
SAPDB_ULong escapeQuoteChar(StudioTD_String& sToModify, 
						    const StudioTD_String& quotechar);

#endif   /* STUDIOTD_UTIL_H */
