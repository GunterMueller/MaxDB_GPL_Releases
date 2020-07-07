/*!**********************************************************************

  module: hwd103inode.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem Inodes

  see also:

  -----------------------------------------------------------------------

  copyright:                  Copyright (c) 1998-2005 SAP AG



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

#ifndef HWD103INODE_H
#define HWD103INODE_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <string.h> /* memset */

#include "gwd00.h"
#include "hwd26err.h"
#include "hwd29date.h"

#include "hsp81.h"
#include "hsp83.h"

#include "SAPDBCommon/SAPDB_string.h"

/*!**********************************************************************

  Chapter: twd103InodeP

  description:
  
************************************************************************/

typedef void *DBFILE;

typedef struct st_inode     *twd103InodeP;

#define MAX_SYSKEY_LEN_WD103  8
#define DB_MAX_NAME_LEN_WD103 245

/* Maximum Length of an inode type name */
#define MAX_TYPENAME_LEN_WD103 10

typedef struct st_inodeid
{
    char  id[MAX_SYSKEY_LEN_WD103+1];
} twd103InodeId;

/*#define wd103inodeidcpy(a,b) SAPDB_memcpy(&a,&b, sizeof(twd103InodeId))*/
#define wd103inodeidcmp(a,b) memcmp(&a,&b, sizeof(twd103InodeId))
#define wd103inodeidInit(a)  memset(&a, 0, sizeof(twd103InodeId))
#define wd103inodeIsNull(ap)  ((ap)->id[7] == 0 && (ap)->id[6] == 0 \
                               && (ap)->id[5] == 0 && (ap)->id[4] == 0 \
                               && (ap)->id[3] == 0 && (ap)->id[2] == 0 \
                               && (ap)->id[1] == 0 && (ap)->id[0] == 0)

typedef enum en_inode_wd103 {
    no_inode = 0,
    inode_directory,
    inode_file,
    inode_link
} twd103InodeType;

typedef char    t_name[DB_MAX_NAME_LEN_WD103+1];

typedef struct st_inode {
    twd103InodeId        InodeId;
    twd103InodeId        ParentDirectoryId;
    twd103InodeType      Type;
    t_name               Name;
    tsp81_UCS2Char       NameUCS2[2*(DB_MAX_NAME_LEN_WD103+1)];
    sapdbwa_Bool         ContainerExists;
    twd103InodeId        ContainerId;
    twd103InodeId        LinkInodeId;
    twd103InodeId        ContainerLongHandle;
    sapdbwa_UInt4        FilePos;
    sapdbwa_UInt4        Size;
    char                 modificationDate[ODBC_DATE_LEN_WD29];
    char                 modificationTime[ODBC_TIME_LEN_WD29];
    sapdbwa_Bool         compressed;
} twd103Inode;

twd103InodeP wd103CreateInode();

void wd103DestroyInode(  twd103InodeP inode );

void wd103CopyInode( twd103InodeP sourceInode, twd103InodeP destInode );

void wd103InitInode( twd103InodeP inode );

enum en_inode_wd103 wd103Text2InodeType( char *text );
    
/*!**********************************************************************

  EndChapter: twd103InodeP

************************************************************************/
#endif
