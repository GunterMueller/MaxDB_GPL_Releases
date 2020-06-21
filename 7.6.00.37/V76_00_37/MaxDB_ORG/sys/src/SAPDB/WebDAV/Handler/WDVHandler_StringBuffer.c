/****************************************************************************

  module      : WDVHandler_StringBuffer.c

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


	#include "SAPDB/WebDAV/Handler/WDVHandler_StringBuffer.h"

	/*------------------------------------------------------------------------
		Function:	createStrBuffer
	--------------------------------------------------------------------------*/
	WDVH_HttpReplyBody createStrBuffer(sapdbwa_UInt4 size) {

		sapdbwa_Bool		allocatOK;
		WDVH_HttpReplyBody	buf;

		WDVHandler_Allocat(sizeof(struct strBuffer),(sapdbwa_UInt1**) &buf,&allocatOK);
		if (allocatOK) {
			if (size<=1024)
				buf->blocksize = 1024;
			else 
				buf->blocksize = size;
			
			WDVHandler_Allocat(sizeof(SAPDB_Char)*buf->blocksize,(sapdbwa_UInt1**) &buf->buf,&allocatOK);
			if (!allocatOK) {
				buf->blockcount = 0;
				buf->contentlength = 0;
				return NULL;
			} else {
				buf->blockcount = 1;
				buf->contentlength = 1;
				strcpy(buf->buf,"");
				return buf;
			}
		} else {
			buf->blockcount = 0;
			buf->contentlength = 0;
			return NULL;
		}

	}

	/*------------------------------------------------------------------------
		Function:	destroyStrBuffer
	--------------------------------------------------------------------------*/
	sapdbwa_Bool destroyStrBuffer(WDVH_HttpReplyBody buf) {

		WDVHandler_Free((sapdbwa_UInt1*)buf->buf);
		WDVHandler_Free((sapdbwa_UInt1*)buf);
		return true;

	}

	/*------------------------------------------------------------------------
		Function:	resetStrBuffer
	--------------------------------------------------------------------------*/
	sapdbwa_Bool resetStrBuffer(WDVH_HttpReplyBody buf) {

		buf->contentlength = 1;
		strcpy(buf->buf,"");
		return true;

	}

	/*------------------------------------------------------------------------
		Function:	appendStrBuffer
	--------------------------------------------------------------------------*/
	sapdbwa_Bool appendStrBuffer(WDVH_HttpReplyBody buf, char *string) {

		sapdbwa_Bool		allocatOK;
		sapdbwa_UInt4		blocks;
		sapdbwa_UInt4		i;
		char				*temp;

		if (strlen(string) <= (buf->blocksize * buf->blockcount) - buf->contentlength) {
			/* string fits into remaining space of buf */
			for(i=0;i<=strlen(string);i++) {
				buf->buf[(buf->contentlength-1)+i] = string[i];
			}
			buf->buf[(buf->contentlength-1)+strlen(string)] = '\0';

			buf->contentlength += strlen(string);
			return true;
		} else {
			/* string is to long for remaining space of buf */
			blocks = (strlen(string) / buf->blocksize) + 1;
			WDVHandler_Allocat(sizeof(SAPDB_Char)*buf->blocksize,
							   (sapdbwa_UInt1**) &temp,
							   &allocatOK);
			if (allocatOK) {
				strcpy(temp,buf->buf);
				WDVHandler_Free((sapdbwa_UInt1*)buf->buf);
				buf->buf = temp;
			}
/*			WDVHandler_ReAllocat(sizeof(SAPDB_Char)*(blocks*buf->blocksize),
								 (sapdbwa_UInt1**) &buf->buf,
								 &allocatOK);
*/			if (allocatOK) {
				for(i=0;i<=strlen(string);i++) {
					buf->buf[(buf->contentlength-1)+i] = string[i];
				}
				buf->buf[(buf->contentlength-1)+strlen(string)+1] = '\0';

				buf->contentlength += strlen(string);
				buf->blockcount += blocks;
				return true;
			} else {
				return false;
			}
		}
	}

	/*------------------------------------------------------------------------
		Function:	getStrBufferContent
	--------------------------------------------------------------------------*/
	WDVH_Char* getStrBufferContent(WDVH_HttpReplyBody buf) {

		return buf->buf;

	}

	/*------------------------------------------------------------------------
		Function:	getStrBufferLength
	--------------------------------------------------------------------------*/
	WDVH_UInt4 getStrBufferLength(WDVH_HttpReplyBody buf) {

		return buf->contentlength-1;

	}
