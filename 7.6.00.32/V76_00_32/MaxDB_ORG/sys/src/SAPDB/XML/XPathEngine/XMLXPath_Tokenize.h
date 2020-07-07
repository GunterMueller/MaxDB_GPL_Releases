/*!
  @ingroup        XML_XPathIndex
  @file           XMLXPath_Tokenize.h
  @author         FerdiF
  @brief          tokenize XPath expression
*/

/*

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

#ifndef XMLXPATH_TOKENIZE_H
#define XMLXPATH_TOKENIZE_H

#include "SAPDB/XML/XPathEngine/XMLXPath_Types.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Indexing.h"

/*!
   @brief          Function for tokenizing an XPath expression
   @param          pCtx [in] context handle
   @param          xpathStrBase [in] 0 basePath
   @param          xpathStrValue [in] 0 valuePath
   @return         XMLXPath_Idx_RetCode

 */
XMLXPath_Idx_RetCode
XMLXPath_Tok_Tokenize(XMLXPath_ContextPtr  pCtx,
                      const SAPDB_UTF8    *xpathStrBase,
                      const SAPDB_UTF8    *xpathStrValue);

#endif /* XMLXPATH_TOKENIZE_H */
