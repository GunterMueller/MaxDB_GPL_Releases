/*!
  @defgroup       XML_XPathIndex Subset of XPath expression used to build
                                 document indices   
*/
/*!
  @ingroup        XML_XPathIndex
  @file           XMLXPath_Indexing.h
  @author         FerdiF
  @brief          Select values within XML documents which can be used to
                  build indexes on XML documents:
                  The values to be selected are specified via ( a subset of )
                  XPATH expressions.
                  The evaluation is done using one SAX parse thru the document.
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


#ifndef XMLXPATH_INDEXING_H
#define XMLXPATH_INDEXING_H

#include "SAPDB/XML/XPathEngine/XMLXPath_Interface.h"

/*!
    @brief  defining if only the first matching value (XMLXPath_Idx_value) 
            or
            all matching values within the XML document should be deliverd.
            (used as parameter of function 'XMLXPath_Idx_DefineIndex')
*/
typedef enum
{
    XMLXPath_Idx_value  = 1,
    XMLXPath_Idx_set    = 2
} XMLXPath_Idx_Category;

/*!
   @brief     Handler function to be called when an index value is found.
              This Handler function has to be provided by the user and the
              function pointer is passed as parameter when defining an index
              using function 'XMLXPath_Idx_DefineIndex'
   @param     parserUserData [in] user data ptr, which has been passed to
                                  function 'XMLXPath_Idx_Parse' or                                  
                                  XMLXPath_Idx_ParseBuf'
   @param     idxUserData [in]    user data ptr, which has been passed to
                                  function 'XMLXPath_Idx_DefineIndex'
   @param     idxValue [in]       0 terminated UTF-8 value string found
   @return    XMLXPath_Idx_RetCode
 */
typedef XMLXPath_Idx_RetCode 
(*XMLXPath_Idx_IndexHandler) (void             *parserUserData,
                              void             *idxUserData,
                              const SAPDB_UTF8 *idxValue );

/*!
   @brief     Handler function to be called by the SAX parser to read the
              XML document.
              This Handler function has to be provided by the user and the
              function pointer is passed as parameter when calling
              function 'XMLXPath_Idx_Parse'
   @param     parserUserData [in] user data ptr, which has been passed to 
                                  function 'XMLXPath_Idx_Parse'
   @param     buffer [out]        communication buffer for the XML Document
   @param     datata_len [out]    length of the filled buffer part
   @param     data_end [out]      end of document flag 
                                  ( 0 = no end, 1 = end of data )
   @return    XMLXPath_Idx_RetCode
 */
typedef XMLXPath_Idx_RetCode 
(*XMLXPath_Idx_ReadXmlBuff) (void           *parserUserData,
                             unsigned char **buffer,
                             SAPDB_Int      *data_len,
                             SAPDB_Int      *data_end );

/*!
   @brief     Initialize a context handle.
              This function has to be called first. All other functions 
              depend on the context handle to exist.
   @param     pCtx [out] context handle ( 0 = no end, 1 = end of data )
   @return    XMLXPath_Idx_RetCode 
              ( only possible error return code: XMLXPath_RC_NOMEMORY )
 */
XMLXPath_Idx_RetCode
XMLXPath_Idx_Initialize( void **pCtx );

/*!
   @brief     Define the values to be extracted out of XML documents
              using XPath 
              This function has to be called before calling function
              XMLXPath_Idx_Parse
   @param     pCtx [in]          context handle
   @param     idxUserData [in]   user data pointer, this pointer isn't 
                                 used internally.                                  
                                 It's only purpose is to allow the                                   
                                 caller to pass index specific                                   
                                 information to the specified                                   
                                 index handler
   @param     idxHandler [in]    index handler
   @param     basePath [in]      XPath string defining the basepath 
                                 (category 'XMLXPath_Idx_set' is always
                                 assumed for basePath)
   @param     valuePath [in]     XPath string defining the valuepath
   @param     valueCategory [in] category used for valuepath
   @return    XMLXPath_Idx_RetCode
 */
XMLXPath_Idx_RetCode
XMLXPath_Idx_DefineIndex(void                      *pCtx,
                         void                      *idxUserData,
                         XMLXPath_Idx_IndexHandler  idxHandler,
                         SAPDB_UTF8                *basePath,
                         SAPDB_UTF8                *valuePath,
                         XMLXPath_Idx_Category      valueCategory );

/*!
   @brief     scan a complete XML document and extract values for all 
              defined 'indexes'
              The extracted values are passed to the caller, via 
              index handler functions.
              The XML document is read using the read handler function
              If more than one document is to be parsed using the same
              set of index definitions the XMLXPath_Idx_Parse function 
              may be called for each document without the need to redefine
              the indexes.
   @param     pCtx [in]          context handle
   @param     parseUserData [in] user data pointer, this pointer isn't
                                 used internally.
                                 It's only purpose is to allow the
                                 caller to pass XML document specific
                                 information to the specified
                                 read and index handler
   @param     readHandler [in]   read handler
   @return    XMLXPath_Idx_RetCode
 */
XMLXPath_Idx_RetCode
XMLXPath_Idx_Parse(void                     *pCtx, 
                   void                     *parseUserData, 
                   XMLXPath_Idx_ReadXmlBuff  readHandler );

/*!
   @brief     scan one buffer of a XML document and extract values for all 
              defined 'indexes'
              The extracted values are passed to the caller, via 
              index handler functions.
   @param     pCtx [in]          context handle
   @param     buf [in]           buffer ( UTF8 or UCS2 )
   @param     bufLen [in]        length of filled buffer part
   @param     parseUserData [in] user data pointer, this pointer isn't 
                                 used internally.
                                 It's only purpose is to allow the
                                 caller to pass XML document specific
                                 information to the specified
                                 read and index handler
   @param     endOfDoc [in]      end of document ( last buffer )
   @return    XMLXPath_Idx_RetCode
 */
XMLXPath_Idx_RetCode
XMLXPath_Idx_ParseBuf(void          *pCtx, 
                      unsigned char *buf,
                      SAPDB_Int      bufLen,
                      void          *parseUserData, 
                      SAPDB_Bool     endOfDoc );

/*!
   @brief     clean up context handle.
   @param     pCtx [in] context handle
   @return    XMLXPath_Idx_RetCode
 */
XMLXPath_Idx_RetCode
XMLXPath_Idx_Terminate( void *pCtx );

#endif  /* XMLXPATH_INDEX_H */
