/*!
  @ingroup        XML_XPathIndex
  @file           XMLXPath_ParseSaxHandling.h
  @author         FerdiF
  @brief          code called by SAX handlers to extract index values
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

#ifndef XMLXPATH_PARSESAXHANDLING_H
#define XMLXPATH_PARSESAXHANDLING_H

#include "SAPDB/XML/XPathEngine/XMLXPath_Interface.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Types.h"

/*!
   forward declaration of XMLXPath_PSH_ItemPtr
*/
typedef struct XMLXPath_PSH_Item XMLXPath_PSH_Item, *XMLXPath_PSH_ItemPtr;
/*!
   declaration of XMLXPath_PSH_Item
*/
struct XMLXPath_PSH_Item
{
    XMLXPath_PSH_ItemPtr   next;                /*!< next item ptr */
    XMLXPath_PSH_ItemPtr   prev;                /*!< prev item ptr */
    SAPDB_UTF8            *indexValueStr;       /*!< ??? */
    XMLXPath_IndexListPtr  idxList;             /*!< ??? */
    SAPDB_Int2             actualStepPos;       /*!< ??? */
    SAPDB_Int2             actualLevel;         /*!< ??? */
    SAPDB_Int2             removeLevel;         /*!< ??? */
    SAPDB_Bool             isValueStep;         /*!< ??? */
    SAPDB_Bool             foundXPath;          /*!< ??? */
    SAPDB_Bool             foundFirstValue;     /*!< ??? */
};

/*!
   @brief       create internal index execute information
   @param       pCtx        [in]  context handle
   @return      XMLXPath_Idx_RetCode
 */
XMLXPath_Idx_RetCode
XMLPath_PSH_CreateIdxWorkItemList(XMLXPath_ContextPtr pCtx);

/*!
  @brief        SAX character handler
  @param        pCtx        [in]  context handle
  @param        s           [in]  value string ( not 0 terminated )
  @param        len         [in]  length of value string
  @return       XMLXPath_Idx_RetCode
 */
void
XMLXPath_PSH_CharacterHandler(XMLXPath_ContextPtr pCtx,
                              const SAPDB_UTF8*   s,
                              SAPDB_Int           len);

/*!
  @brief        SAX end element handler
  @param        pCtx        [in]  context handle
  @param        elementName [in]  element name
 */
void
XMLXPath_PSH_EndElementHandler(XMLXPath_ContextPtr pCtx,
                               const SAPDB_UTF8*   elementName);

/*!
  @brief        release resources
  @param        pCtx        [in]  context handle
 */
void
XMLXPath_PSH_ReleaseResources(XMLXPath_ContextPtr pCtx);

/*!
  @brief        SAX start element handler
  @param        pCtx        [in]  context handle
  @param        elementName [in]  element name
  @param        atts        [in]  array of all attributes
*/
void
XMLXPath_PSH_StartElementHandler(XMLXPath_ContextPtr pCtx,
                                 const SAPDB_UTF8*   elementName,
                                 const SAPDB_UTF8**  atts);

#endif /* XMLXPATH_PARSESAXHANDLING_H */
