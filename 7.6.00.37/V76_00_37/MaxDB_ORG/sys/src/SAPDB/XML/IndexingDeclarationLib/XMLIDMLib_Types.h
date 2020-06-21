#ifndef XMLIDMLIB_TYPES_H
#define XMLIDMLIB_TYPES_H

/****************************************************************************

  module      : XMLMdlLib_Types.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-06-20  15:52
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/ToolsCommon/Tools_Vector.hpp"
	#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"
	#include "sapdbwa.hpp" //nocheck
	extern "C" {
		#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Definition.h"
/*		#include "SAPDB/XML/QueryServerLib/XMLQSLib_Definition.h"*/
	}

	/*===================================================================
	// Defines
	//===================================================================*/


	/*===================================================================
	// Types
	//===================================================================*/
	/* Basic types */
	typedef unsigned int		XMLIDMLIB_UInt4;
	typedef unsigned short		XMLIDMLIB_UInt2;
	typedef unsigned char		XMLIDMLIB_UInt1;
	typedef short				XMLIDMLIB_Int2;
	typedef int			        XMLIDMLIB_Int4;
	typedef unsigned char		XMLIDMLIB_Bool;
	typedef size_t				XMLIDMLIB_Length;
	typedef char				XMLIDMLIB_Char;
	typedef unsigned char		XMLIDMLIB_UChar;
	typedef unsigned char		*XMLIDMLIB_PChar;
	typedef double				XMLIDMLIB_Double;

	/*--------------------------------------------------------------------------
	declaration: sapdbwa_{ULong | Long}
	description: basic long data types for fastest execution
				 (machine-dependent size)
	--------------------------------------------------------------------------*/
	#if defined(WIN32) && defined(_M_IA64)
	typedef unsigned __int64    XMLIDMLIB_ULong;      /* long (fast, unsigned) */
	typedef signed __int64      XMLIDMLIB_Long;       /* long (fast, signed)   */
	#else
	typedef unsigned long       XMLIDMLIB_ULong;      /* long (fast, unsigned) */
	typedef signed long         XMLIDMLIB_Long;       /* long (fast, signed)   */
	#endif

	#define XMLIDMLIB_True			((XMLIDMLIB_Bool)1)
	#define XMLIDMLIB_False			((XMLIDMLIB_Bool)0)

	typedef void                *XMLIDMLIB_Buffer;
	typedef XMLIDMLIB_ULong        XMLIDMLIB_BufferLength;

	/*-------------------------------------------------------------------
	// Message Types
	//-------------------------------------------------------------------*/
	#define XMLIDMLIB_MSGTY_ERROR			0
	#define XMLIDMLIB_MSGTY_WARNING			1
	#define XMLIDMLIB_MSGTY_INFO			2
	#define XMLIDMLIB_MSGTY_REQUEST			3
	#define XMLIDMLIB_MSGTY_OK				4
	#define XMLIDMLIB_MSGTY_CLEAR			5

	/*-------------------------------------------------------------------
	// Listen
	//-------------------------------------------------------------------*/
 	struct st_xml_index_gui {
		XMLIDMLIB_Char				IndexID[55];
		XMLIDMLIB_Char				PropertyID[55];
		XMLIDMLIB_Char				indexName[800];
		XMLIDMLIB_Char				description[512];
		XMLIDMLIB_Char				xpathBasePath[512];
		XMLIDMLIB_Char				xpathValuePath[512];
	};
	typedef st_xml_index_gui				XMLIDMLib_XmlIndex;
	typedef st_xml_index_gui				*XMLIDMLib_XmlIndexP;

	typedef Tools_Vector<st_api_xml_index*>		XMLIDMLib_XmlIndexList;

	struct st_doc_class_gui {
		XMLIDMLIB_Char				docClassID[55];
		XMLIDMLIB_Char				docClassName[512];
		XMLIDMLIB_Char				description[512];
		XMLIDMLib_XmlIndexList		*assignedIndices;
	};
	typedef st_doc_class_gui				XMLIDMLib_DocClass;
	typedef st_doc_class_gui				*XMLIDMLib_DocClassP;

	typedef Tools_Vector<st_doc_class_gui*>		XMLIDMLib_DocClassList;



	/*-------------------------------------------------------------------
	// Connection User Data
	//-------------------------------------------------------------------*/
	struct st_conn_user_data {
		XMLIMAPI_Handle		xmlHandle;
	};
	typedef struct st_conn_user_data	*XMLIDMLIB_ConnUserData;
	
#endif