/*!**********************************************************************

  module: hwd119copy.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem copy function

  see also:

  -----------------------------------------------------------------------

  copyright:               Copyright (c) 1999-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#ifndef HWD119COPY_H
#define HWD119COPY_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd26err.h"
#include "hwd103inode.h"
#include "hwd104fsdesc.h"

/*!**********************************************************************

  Chapter: twd119CopyDescP

  description:
  
************************************************************************/

typedef struct st_copy_desc *twd119CopyDescP;

twd119CopyDescP wd119CreateCopyDesc( twd104FSDescP	 fsDesc,
                                     SQLHDBC		 hdbc,
                                     const char		*viewOwner,
                                     sapdbwa_UInt4	 partitionId	);

void wd119DestroyCopyDesc(  twd119CopyDescP desc );

sapdbwa_Bool wd119Copy( twd119CopyDescP	 desc,
                      twd103InodeP		 sourceInode,
                      twd103InodeP		 destDir,
                      const char		*newName,
                      sapdbwa_Bool		 withLinks,
                      twd26ErrP			 err	);

/*!**********************************************************************

  EndChapter: twd119CopyDescP

************************************************************************/
#endif
