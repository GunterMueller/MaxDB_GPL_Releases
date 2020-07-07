/*!**********************************************************************

  module: WDVCAPI_XMLIndexing.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  WebDAV XML Indexing

  see also:

  -----------------------------------------------------------------------

						Copyright (c) 2000-2005 SAP AG



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




************************************************************************/

#ifndef WDVCAPI_XML_INDEXING_H
#define WDVCAPI_XML_INDEXING_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef void                             *WDVCAPI_XmlXPathHandle;
typedef struct st_wdvcapi_xml_doc_class  *WDVCAPI_XmlDocClass;
typedef struct st_wdvcapi_xml_index      *WDVCAPI_XmlIndex;

struct st_wdvcapi_xml_doc_class {
    WDVCAPI_XmlDocClassId   docClassId;
    WDVCAPI_XmlXPathHandle  hXPath;
    WDVCAPI_XmlIndex        hIndexList;
    WDVCAPI_XmlDocClass     hNextDocClass;
};

struct st_wdvcapi_xml_index {
    WDVCAPI_WDV             hWDV;
    XMLIMAPI_XmlIndex		index;
    WDVCAPI_XmlIndex        hNextIndex;
};

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool XMLIndexing_CreateXmlDocClass( WDVCAPI_WDV          wdv,
                                        WDVCAPI_XmlDocClass *hDocClass );

WDV_Bool XMLIndexing_DestroyXmlDocClass( WDVCAPI_XmlDocClass hDocClass );

WDV_Bool XMLIndexing_GetXmlDocClass( WDVCAPI_WDV            wdv,
                                     WDVCAPI_XmlDocClassId  xmlDocClassId,
                                     WDVCAPI_XmlDocClass   *xmlDocClass );

/***********************************************************************

	End

 ***********************************************************************/

#endif
