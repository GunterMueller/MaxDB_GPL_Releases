/*!**********************************************************************

  module: hwd113delinode.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem DeleteInode function

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

#ifndef HWD113DELINODE_H
#define HWD113DELINODE_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd26err.h"
#include "hwd103inode.h"
#include "hwd104fsdesc.h"

/*!**********************************************************************

  Chapter: twd113DeleteInodeDescP

  description:
  
************************************************************************/

typedef struct st_delete_inode_desc *twd113DeleteInodeDescP;

twd113DeleteInodeDescP wd113CreateDeleteInodeDesc( twd104FSDescP fsDesc,
                                                   SQLHDBC       hdbc,
                                                   sapdbwa_UInt4 partitionId );

void wd113DestroyDeleteInodeDesc(  twd113DeleteInodeDescP desc );

sapdbwa_Bool wd113DeleteInode( twd104FSDescP          fsdesc,
							   twd113DeleteInodeDescP desc,
							   twd103InodeP           Inode,
                               twd26ErrP              err     );

sapdbwa_Bool wd113DeleteInodeRecursive( twd104FSDescP          fsdesc,
							            twd113DeleteInodeDescP desc,
							            twd103InodeP           Inode,
                                        twd26ErrP              err     );

/*!**********************************************************************

  EndChapter: twd113DeleteInodeDescP

************************************************************************/
#endif
