/*!
  @ingroup        XML_XPathIndex
  @file           XMLXPath_Interface.h
  @author         FerdiF
  @brief          Functions to extract index values out of XML documents
                  The values to be extracted are defined via XPATH expressions
                  Parsing is done using a SAX Parser
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

#ifndef XMLXPATH_INTERFACE
#define XMLXPATH_INTERFACE

#define  XMLXPath_RC_OK         0   /*!< ok                  */
#define  XMLXPath_RC_FAILED     1   /*!< failed              */  
#define  XMLXPath_RC_NOMEMORY   2   /*!< no memory available */
#define  XMLXPath_RC_SYNTAX     3   /*!< syntax error        */

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/*! returncode type for XML_XPathIndex modules */
typedef SAPDB_Int XMLXPath_Idx_RetCode; 

 /*#include "SAPDB/XML/XPathEngine/XMLXPath_Indexing.h"*/
 /*#include "SAPDB/XML/XPathEngine/XMLXPath_Error.h"*/

#endif /* XMLXPATH_INTERFACE */
