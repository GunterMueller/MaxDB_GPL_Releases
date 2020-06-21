/*!
  @ingroup        XML_XPathIndex
  @file           XMLXPath_ParseDocument.h
  @author         FerdiF
  @brief          SAX parser interface
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

#ifndef XMLXPATH_PARSEDOCUMENT_H
#define XMLXPATH_PARSEDOCUMENT_H

#include "idl/expat/expat.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Interface.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Types.h"

/*!
   @brief       parse complete document using read handler to read the 
                document
   @param       pCtx [in]       context handle
   @param       pUserData [in]  user data to be used within read handler
   @param       readHandler [in] read handler
   @return      XMLXPath_Idx_RetCode
 */
XMLXPath_Idx_RetCode
XMLXPath_PD_ParseDocument(void *pCtx,
                          void *pUserData,
                          XMLXPath_Idx_ReadXmlBuff readHandler);

/*!
   @brief          parse next part of a  document
   @param          pCtx [in]          context handle
   @param          buf [in]           buffer containing part of document
   @param          bufLen [in]        length of document part within buffer
   @param          parseUserData [in] user data pointer, this pointer isn't
                                      used internally.
                                      It's only purpose is to allow the
                                      caller to pass XML document specific
                                      information to the specified
                                      read and index handler
   @param          endOfDoc [in]      end of document flag
   @return         XMLXPath_Idx_RetCode
 */
XMLXPath_Idx_RetCode
XMLXPath_PD_ParseDocumentBuf(void          *pCtx,
                             unsigned char *buf,
                             SAPDB_Int      bufLen,
                             void          *parseUserData,
                             SAPDB_Bool     endOfDoc);

#endif /* XMLXPATH_PARSEDOCUMENT_H */
