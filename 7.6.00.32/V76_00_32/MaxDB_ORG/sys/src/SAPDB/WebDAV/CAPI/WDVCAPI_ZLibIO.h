/*!**********************************************************************

  module: WDVCAPI_ZLibIO.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  ZLib Compression / Decompression

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

#ifndef WDVCAPI_ZLIB_IO_H
#define WDVCAPI_ZLIB_IO_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <stdio.h>

#include <WDVCAPI_Definition.h>

#include "extern/zlib/zutil.h"

/***********************************************************************

	Types & Structs

 ***********************************************************************/

typedef struct gz_stream    *WDVCAPI_ZLib;

typedef struct gz_stream {
    z_stream stream;
    int      z_err;   /* error code for last stream operation */
    int      z_eof;   /* set if end of input file */
    SQLHSTMT file;   /* .gz file */
    Byte     *inbuf;  /* input buffer */
    Byte     *outbuf; /* output buffer */
    uLong    crc;     /* crc32 of uncompressed data */
    char     *msg;    /* error message */
    SQLHSTMT path;   /* path name for debugging only */
    int      transparent; /* 1 if input file is not a .gz file */
    char     mode;    /* 'w' or 'r' */
    long     startpos; /* start of compressed data in file (header skipped) */
} gz_stream;

/***********************************************************************

	Prototypes

 ***********************************************************************/

gzFile ZLib_gzopen( SQLHSTMT	 hStmt,
				    char		*mode );

int ZLib_gzwrite ( gzFile	    file,
				   const voidp buf,
				   unsigned    len );

int ZEXPORT ZLib_gzclose (gzFile          file,
                          unsigned long  *compressedLength );

/***********************************************************************

	End

 ***********************************************************************/

#endif
