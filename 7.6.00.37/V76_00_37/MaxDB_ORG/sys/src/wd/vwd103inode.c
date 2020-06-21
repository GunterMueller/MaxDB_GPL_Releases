/*!**********************************************************************

  module: vwd103inode.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem Inodes

  see also:

  -----------------------------------------------------------------------

  copyright:               Copyright (c) 1998-2005 SAP AG




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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <string.h>

#include "hwd103inode.h"

#include "SAPDBCommon/SAPDB_string.h"

/*!**********************************************************************

  Chapter: twd103InodeP

************************************************************************/


twd103InodeP wd103CreateInode()
{
    sapdbwa_Bool allocatOK;
    twd103InodeP newInode;

    sapdbwa_SQLALLOCAT( sizeof( struct st_inode),
                   (sapdbwa_UInt1**) &newInode,
		&allocatOK );
    if (!allocatOK) {
        newInode = NULL;
    } else {
        wd103InitInode( newInode );
    }; /* else */

    return newInode;
} /* wd103CreateInode */


void wd103DestroyInode(  twd103InodeP inode )
{
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) inode );
} /* wd103DestroyInode */


void wd103CopyInode( twd103InodeP sourceInode, twd103InodeP destInode )
{
    SAPDB_memcpy( destInode, sourceInode, sizeof( struct st_inode ));
} /* wd103CopyInode */


void wd103InitInode( twd103InodeP inode )
{
    wd103inodeidInit( inode->InodeId );
    inode->FilePos = 0;
    wd103inodeidInit( inode->ParentDirectoryId );
    inode->Type = no_inode;
    inode->ContainerExists = false;
    wd103inodeidInit( inode->ContainerId );
    wd103inodeidInit( inode->LinkInodeId );
    wd103inodeidInit( inode->ContainerLongHandle );
    inode->FilePos = 0;
    inode->Size = 0;
    inode->modificationDate[0] = '\0';
    inode->modificationTime[0] = '\0';
    inode->compressed = false;
} /* wd103InitInode */


enum en_inode_wd103 wd103Text2InodeType( char *text )
{
    if (text[0] == 'f')
        return (inode_file);
    else if (text[0] == 'd')
        return (inode_directory);
    else if (text[0] == 'l')
        return (inode_link);
    
    return (no_inode);
} /* wd103Text2InodeType */

/*!**********************************************************************

  EndChapter: twd103InodeP

************************************************************************/
