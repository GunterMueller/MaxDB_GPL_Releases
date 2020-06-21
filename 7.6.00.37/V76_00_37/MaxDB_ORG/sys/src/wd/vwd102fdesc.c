/*!**********************************************************************

  module: vwd102fdesc.c

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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "hwd102fdesc.h"
#include "hwd103inode.h"

/*!**********************************************************************

  Chapter: twd102FileDescP

************************************************************************/


twd102FileDescP wd102CreateFileDesc( sapdbwa_UInt4 index )
{
    sapdbwa_Bool         allocatOK;
    twd102FileDescP    newDesc;
    
    sapdbwa_SQLALLOCAT( sizeof(struct st_file_desc),
                (sapdbwa_UInt1**) &newDesc,
                &allocatOK );
    if (allocatOK) {
        newDesc->writeComprBuf = wd105CreateDynBuf( 512*1024 );
        if (newDesc->writeComprBuf != NULL) {
            newDesc->index = index;
            newDesc->connection = NULL;
            newDesc->access = no_access;
            newDesc->filePos = 0;
            newDesc->extraInfo.PosInDE = 0;
            newDesc->actDirent.d_name[0] = '\0';
            /* set other fields in dirent ... */
            
            wd103inodeidInit( newDesc->extraInfo.DirectoryEntry.ino );
            newDesc->extraInfo.DirectoryEntry.reclen = 0;
            newDesc->extraInfo.DirectoryEntry.namelen = 0;
        } else {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newDesc );            
            newDesc = NULL;
        }; /* else */
    } else {
        newDesc = NULL;
    }; /* else */

    return newDesc;
} /* wd102CreateFileDesc */


void wd102DestroyFileDesc( twd102FileDescP fdesc )
{
    wd105DestroyDynBuf( fdesc->writeComprBuf );
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) fdesc );
} /* wd102DestroyFileDesc */

/*!**********************************************************************

  EndChapter: twd102FileDescP

************************************************************************/
