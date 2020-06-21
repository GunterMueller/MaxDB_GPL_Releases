/*!
  @ingroup        XML_XPathIndex
  @file           XMLXPath_Error.h
  @author         FerdiF
  @brief          get/set last error description
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

#ifndef XMLXPATH_ERROR_H
#define XMLXPATH_ERROR_H

#include "SAPDB/XML/XPathEngine/XMLXPath_Indexing.h"

/*!
   @brief       get last error description
                ( each call of the following function resets the error:
                XMLXPath_Idx_DefineIndex, XMLXPath_Idx_Parse )
   @param       pCtx [in]       context handle
   @param       errNo [in,out]  variable to receive error number
   @param       errMsg [in,out] buffer to receive error message
   @param       maxMsgLen [in]  maximal buffer size
   @return      XMLXPath_Idx_RetCode
 */
XMLXPath_Idx_RetCode
XMLXPath_Err_GetError(void       *pCtx,
                      SAPDB_Int  *errNo,
                      SAPDB_UTF8 *errMsg,
                      SAPDB_Int   maxMsgLen );

/*!
  @brief        set error using only an error number
  @param        pCtx        [in]  context handle
  @param        errNo       [in]  error number
  @return       XMLXPath_Idx_RetCode 
*/
XMLXPath_Idx_RetCode
XMLXPath_Err_SetError(void      *pCtx,
                      SAPDB_Int  errNo );

/*!
  @brief        set error using an error number and an address pointing to 
                the actual position within an XPath expression where the
                parsing error occurred

  @param        pCtx        [in]  context handle
  @param        errNo       [in]  error number
  @param        errPos      [in]  XPath parse position pointer 
  @return       XMLXPath_Idx_RetCode 
*/
XMLXPath_Idx_RetCode
XMLXPath_Err_SetErrorParse(void             *pCtx,
                           SAPDB_Int         errNo,
                           const SAPDB_UTF8 *errPos );

/*!
  @brief        set error SAX parser error
  @param        pCtx        [in]  context handle
  @param        errCode     [in]  parser error code
  @param        errMsg      [in]  parser error message
  @param        lineno      [in]  line number within XML document
  @param        columnno    [in]  column number within XML document
  @param        byteIdx     [in]  byte index from beginning of XML document
  @return       XMLXPath_Idx_RetCode 
*/
XMLXPath_Idx_RetCode
XMLXPath_Err_SetErrorSaxParser(void             *pCtx,
                               SAPDB_Int         errCode,
                               const SAPDB_UTF8 *errMsg,
                               SAPDB_Int         lineno,
                               SAPDB_Int         columnno,
                               SAPDB_Int         byteIdx );


/*!
  @brief        set error using an error number and an parameter string
  @param        pCtx        [in]  context handle
  @param        errNo       [in]  error number
  @param        str         [in]  error parameter string
  @return       XMLXPath_Idx_RetCode 
*/
XMLXPath_Idx_RetCode
XMLXPath_Err_SetErrorString(void             *pCtx,
                            SAPDB_Int         errNo,
                            const SAPDB_UTF8 *str);

#endif /* XMLXPATH_ERROR_H */
