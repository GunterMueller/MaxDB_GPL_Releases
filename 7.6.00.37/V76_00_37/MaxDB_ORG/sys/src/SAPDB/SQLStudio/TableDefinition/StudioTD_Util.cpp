/*!===========================================================================

  module:       StudioTD_Util.cpp

  ----------------------------------------------------------------------------

  responsible:  MarkusS, Hans-GeorgB

  special area: SQL Studio

  description:  Implementation of utility functions used by the table definition class StudioTD_TableDef.

  see:          www.sapdb.org

  version:      7.4

  created:      2000-05-18

  updated:      2000-05-18

  ----------------------------------------------------------------------------

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include <stdlib.h>
#include "SQLStudio/TableDefinition/StudioTD_Util.h"


/*===========================================================================*
 *     UpperCase                                                             *
 *===========================================================================*/
StudioTD_String UpperCase (StudioTD_String str)
{
  StudioTD_String  s = str;
  return s.ToUpper();
}

/*===========================================================================*
 *     LowerCase                                                             *
 *===========================================================================*/
StudioTD_String LowerCase (StudioTD_String str)
{
  StudioTD_String  s = str;
  return s.ToLower();
}

/*===========================================================================*
 *     ProperCase                                                            *
 *===========================================================================*/
StudioTD_String ProperCase (StudioTD_String str)
{
  StudioTD_String  s1 = str.SubStr (0, 1);
  StudioTD_String  s2 = str.SubStr (1);
  
  s1.ToUpper();
  s1 += s2.ToLower();
  
  return s1;
}

/*===========================================================================*
 *     AtoI                                                                  *
 *===========================================================================*/
SAPDB_Int AtoI (StudioTD_String str)
{
  return atoi ((SAPDB_Char *)str.StrPtr());
}


/*===========================================================================*
 *     NextToken                                                             *
 *===========================================================================*/
StudioTD_String NextToken (StudioTD_String str, StudioTD_String sep, SAPDB_UInt *pos)
{
  StudioTD_String   token;
  SAPDB_UInt        end;
  
  end   = str.Find (sep, *pos);

  if(end == StudioTD_String::NPos)
  {
    *pos = 0;
    return StudioTD_String();
  }
  
  token = str.SubStrBasis (*pos, end);
  *pos = end + 1;

  return token;
}

/*===========================================================================*
 *     string2XML                                                            *
 *===========================================================================*/
StudioTD_String string2XML ( StudioTD_String  str  )
{

   StudioTD_String  patterns((SAPDB_UTF8*)"&\"'<>");
   StudioTD_String  convertedstr = str;

   SAPDB_UInt  start = 0, end ;

   while ( (end = convertedstr.FindFirstOf (patterns, start) ) != StudioTD_String::NPos ) 
   {
	   switch ( convertedstr[end] ) 
	   {
		   case ( '&' ):
			   {
			   convertedstr.Replace(end,1,"&amp;");
			   end += 5;
			   break;
			   }
		   case ( '\"' ):
			   {
			   convertedstr.Replace(end,1,"&quot;");
			   end += 6;
			   break;
			   }
		   case ( '\'' ):
			   {
			   convertedstr.Replace(end,1,"&apos;");
			   end += 6;
			   break;
			   }
		   case ( '<' ):
			   {
			   convertedstr.Replace(end,1,"&lt;");
			   end += 4;
			   break;
			   }
		   case ( '>' ):
			   {
			   convertedstr.Replace(end,1,"&gt;");
			   end += 4;
			   break;
			   }

		   default:
			   break;
	   }

	   start = end + 1;

   }

   return convertedstr;

}


/*===========================================================================*
 *     StudioTD_TableDef::escapeQuoteChar                                  *
 *===========================================================================*/
SAPDB_ULong escapeQuoteChar(StudioTD_String& sToModify, 
							const StudioTD_String& quotechar)
{

	SAPDB_ULong retval = 0;
	
	if (sToModify.Size() >= 1) 
	{
		StudioTD_String::BasisElementIndex Pos;
		StudioTD_String sReplace = quotechar;

		sReplace += quotechar;
	
		for(Pos = sToModify.Find(quotechar), retval = 0;
		    Pos != StudioTD_String::NPos;
			Pos = sToModify.Find(quotechar, Pos+2), retval++ )
		{
			sToModify.Replace(Pos,1,sReplace);
		}

	}

	return retval;
}



 
