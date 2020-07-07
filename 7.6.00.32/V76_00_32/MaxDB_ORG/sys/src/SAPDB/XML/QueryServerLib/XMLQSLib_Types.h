#ifndef XMLQSLIB_TYPES_H
#define XMLQSLIB_TYPES_H

/****************************************************************************

  module      : XMLQSLib_Types.h

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
	#include "hwd03waapi.h"
	#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Definition.h"
	#include "SAPDB/WebDAV/XIS/WDVXIS_ErrorCodes.h"

	/*===================================================================
	// Defines
	//===================================================================*/


	/*===================================================================
	// Types
	//===================================================================*/
	/* Basic types */
	typedef unsigned int		XMLQSLIB_UInt4;
	typedef unsigned short		XMLQSLIB_UInt2;
	typedef unsigned char		XMLQSLIB_UInt1;
	typedef short				XMLQSLIB_Int2;
	typedef int			        XMLQSLIB_Int4;
	typedef unsigned char		XMLQSLIB_Bool;
	typedef size_t				XMLQSLIB_Length;
	typedef char				XMLQSLIB_Char;
	typedef unsigned char		XMLQSLIB_UChar;
	typedef unsigned char		*XMLQSLIB_PChar;
	typedef double				XMLQSLIB_Double;

	/*--------------------------------------------------------------------------
	declaration: sapdbwa_{ULong | Long}
	description: basic long data types for fastest execution
				 (machine-dependent size)
	--------------------------------------------------------------------------*/
	#if defined(WIN32) && defined(_M_IA64)
	typedef unsigned __int64    XMLQSLIB_ULong;      /* long (fast, unsigned) */
	typedef signed __int64      XMLQSLIB_Long;       /* long (fast, signed)   */
	#else
	typedef unsigned long       XMLQSLIB_ULong;      /* long (fast, unsigned) */
	typedef signed long         XMLQSLIB_Long;       /* long (fast, signed)   */
	#endif

	#define XMLQSLIB_True			((XMLQSLIB_Bool)1)
	#define XMLQSLIB_False			((XMLQSLIB_Bool)0)

	typedef void                *XMLQSLIB_Buffer;
	typedef XMLQSLIB_ULong        XMLQSLIB_BufferLength;

#endif