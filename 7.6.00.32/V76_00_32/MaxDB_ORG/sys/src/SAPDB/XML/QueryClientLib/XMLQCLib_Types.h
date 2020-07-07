#ifndef XMLQCLIB_TYPES_H
#define XMLQCLIB_TYPES_H

/****************************************************************************

  module      : XMLQCLib_Types.h

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
	#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"
	#include "sapdbwa.hpp" //nocheck
	extern "C" {
		#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Definition.h"
		#include "SAPDB/XML/QueryServerLib/XMLQSLib_Definition.h"
	}

	/*===================================================================
	// Defines
	//===================================================================*/


	/*===================================================================
	// Types
	//===================================================================*/
	/* Basic types */
	typedef unsigned int		XMLQCLIB_UInt4;
	typedef unsigned short		XMLQCLIB_UInt2;
	typedef unsigned char		XMLQCLIB_UInt1;
	typedef short				XMLQCLIB_Int2;
	typedef int			        XMLQCLIB_Int4;
	typedef unsigned char		XMLQCLIB_Bool;
	typedef size_t				XMLQCLIB_Length;
	typedef char				XMLQCLIB_Char;
	typedef unsigned char		XMLQCLIB_UChar;
	typedef unsigned char		*XMLQCLIB_PChar;
	typedef double				XMLQCLIB_Double;

	/*--------------------------------------------------------------------------
	declaration: sapdbwa_{ULong | Long}
	description: basic long data types for fastest execution
				 (machine-dependent size)
	--------------------------------------------------------------------------*/
	#if defined(WIN32) && defined(_M_IA64)
	typedef unsigned __int64    XMLQCLIB_ULong;      /* long (fast, unsigned) */
	typedef signed __int64      XMLQCLIB_Long;       /* long (fast, signed)   */
	#else
	typedef unsigned long       XMLQCLIB_ULong;      /* long (fast, unsigned) */
	typedef signed long         XMLQCLIB_Long;       /* long (fast, signed)   */
	#endif

	#define XMLQCLIB_True			((XMLQCLIB_Bool)1)
	#define XMLQCLIB_False			((XMLQCLIB_Bool)0)

	typedef void                *XMLQCLIB_Buffer;
	typedef XMLQCLIB_ULong        XMLQCLIB_BufferLength;

	/*-------------------------------------------------------------------
	// Message Types
	//-------------------------------------------------------------------*/
	#define XMLQCLIB_MSGTY_ERROR			0
	#define XMLQCLIB_MSGTY_WARNING			1
	#define XMLQCLIB_MSGTY_INFO			2
	#define XMLQCLIB_MSGTY_REQUEST			3
	#define XMLQCLIB_MSGTY_OK				4
	#define XMLQCLIB_MSGTY_CLEAR			5


	/*-------------------------------------------------------------------
	// Connection User Data
	//-------------------------------------------------------------------*/
	struct st_xmlqclib_conn_user_data {
		XMLIMAPI_Handle		xmlHandle;
	};
	typedef struct st_xmlqclib_conn_user_data	*XMLQCLIB_ConnUserData;
	
#endif