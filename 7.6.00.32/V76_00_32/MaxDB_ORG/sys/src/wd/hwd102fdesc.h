/*!**********************************************************************

  module: hwd102fdesc.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  File descriptors for DB-Filesystem

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

#ifndef HWD102FDESC_H
#define HWD102FDESC_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"

#include <sys/types.h>

#if !defined(WIN32)
#include <dirent.h>
#else
/* struct dirent - same as Unix */
struct dirent {
    long d_ino;                   /* inode */
    off_t d_off;                  /* offset to this dirent */
    unsigned short d_reclen;      /* length of d_name */
    char d_name[MAX_FILENAME_LEN_WD00+1];    /* filename (null terminated) */
};
#endif

#include "hwd101dbfs.h"
#include "hwd103inode.h"
#include "hwd105dynbuf.h"
#include "hwd106pool.h"

/*!**********************************************************************

  Chapter: twd102FileDescP

  description:
  
************************************************************************/

typedef struct st_file_desc *twd102FileDescP;

twd102FileDescP wd102CreateFileDesc( sapdbwa_UInt4 index );

void wd102DestroyFileDesc( twd102FileDescP fdesc );

struct st_db_direct {
 twd103InodeId ino;     /* inode number of entry */
 sapdbwa_UInt2   reclen;  /* length of this record */
 sapdbwa_UInt2   namelen; /* length of string in d_name */
 char          name[MAX_FILENAME_LEN_WD00 + 1]; /* name must be no longer
                                                 * than this */
};

typedef struct st_file_desc
{
    sapdbwa_UInt2      index;      /* index in fieleDescArray      */
    twd106ConnP      connection; /* connection used to access db */
    enum en_access   access;     /* open flag */
    twd103Inode      inode;      /* inode description of opened file */
    sapdbwa_UInt4      filePos;    /* read position in file */
    twd105DynBufP    writeComprBuf; /* used to collect data for compressed
                                     * writing */
    struct dirent    actDirent;  /* actual dirent as returned by wd101ReadDir */
    
    struct st_fd_info {
        sapdbwa_UInt2          PosInDE;
        struct st_db_direct  DirectoryEntry; /* Buffer for inodeio */
    } extraInfo;
} twd102FileDesc;

/*!**********************************************************************

  EndChapter: twd102FileDescP

************************************************************************/
#endif

