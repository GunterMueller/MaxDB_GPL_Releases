/*!**********************************************************************

  module: hwd112insinode.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem InsertInode function

  see also:

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 1998-2005 SAP AG



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

#ifndef HWD112INSINODE_H
#define HWD112INSINODE_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd26err.h"
#include "hwd103inode.h"
#include "hwd104fsdesc.h"

/*!**********************************************************************

  Chapter: twd112InsertInodeDescP

  description:
  
************************************************************************/

typedef struct st_insert_inode_desc *twd112InsertInodeDescP;

twd112InsertInodeDescP wd112CreateInsertInodeDesc( twd104FSDescP   fsdesc,
                                                   SQLHDBC         hdbc,
                                                   const char     *viewOwner,
                                                   sapdbwa_UInt4   partitionId );


void wd112DestroyInsertInodeDesc(  twd112InsertInodeDescP desc );


/* newInode is only set if inodeType is inode_file */
sapdbwa_Bool wd112DbInsertInode( twd112InsertInodeDescP  desc,
                               twd103InodeId          *directory,
                               twd103InodeId          *linkInode,
                               char                   *name,
                               enum en_inode_wd103     inodeType,
                               twd103InodeP            newInode,
                               twd26ErrP               err );

/*!**********************************************************************

  EndChapter: twd112InsertInodeDescP

************************************************************************/
#endif

