#ifndef WDVHANDLER_STRINGBUFFER_H
#define WDVHANDLER_STRINGBUFFER_H
/****************************************************************************

  module      : WDVHandler_StringBuffer.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-03-13  11:45
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

	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonInclude.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_ErrorInclude.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_DAVMethods.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlFragments.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_XmlParser.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_Connection.h"

	struct strBuffer {
		sapdbwa_UInt4	blocksize;
		sapdbwa_UInt4	blockcount;
		sapdbwa_UInt4	contentlength;
		char			*buf;
	};

	typedef struct strBuffer	*WDVH_HttpReplyBody;
	typedef struct strBuffer	*WDVH_StringBuffer;

	/*------------------------------------------------------------------------
	Function	:	createStrBuffer
	Description	:	creation of a string buffer with initial size blocksize
	Parameters	:	[in] blocksize (initial size 1024, iff not given otherwise)
	Return		:	pointer to buffer (NULL if something's gone wrong)
	--------------------------------------------------------------------------*/
	WDVH_HttpReplyBody createStrBuffer(sapdbwa_UInt4 size);

	/*------------------------------------------------------------------------
	Function	:	destroyStrBuffer
	Description	:	destructor for string buffer
	Parameters	:	[in] pointer to buffer
	Return		:	true/false for action worked out well/gone wrong
	--------------------------------------------------------------------------*/
	sapdbwa_Bool destroyStrBuffer(WDVH_HttpReplyBody buf);

	/*------------------------------------------------------------------------
	Function	:	resetStrBuffer
	Description	:	reset Buffer to length blocksize and clear the stored string
					to ""
	Parameters	:	[in] pointer to buffer
	Return		:	true/false for action worked out well/gone wrong
	--------------------------------------------------------------------------*/
	sapdbwa_Bool resetStrBuffer(WDVH_HttpReplyBody buf);

	/*------------------------------------------------------------------------
	Function	:	appendStrBuffer
	Description	:	append a given string to the buffer
	Parameters	:	[in] pointer to buffer
					[in] string
	Return		:	true/false for action worked out well/gone wrong
	--------------------------------------------------------------------------*/
	sapdbwa_Bool appendStrBuffer(WDVH_HttpReplyBody buf, char *string);

	/*------------------------------------------------------------------------
	Function	:	getStrBufferContent
	Description	:	returns the content of the buffer as string
	Parameters	:	[in] pointer to buffer
	Return		:	pointer to content string
	--------------------------------------------------------------------------*/
	WDVH_Char* getStrBufferContent(WDVH_HttpReplyBody buf);

	/*------------------------------------------------------------------------
	Function	:	getStrBufferLength
	Description	:	returns the length of the content of the buffer
	Parameters	:	[in] pointer to buffer
	Return		:	length
	--------------------------------------------------------------------------*/
	WDVH_UInt4 getStrBufferLength(WDVH_HttpReplyBody buf);

#endif