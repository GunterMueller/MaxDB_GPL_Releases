/*!**********************************************************************

  module: hwd115container.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem container functions

  see also:

  -----------------------------------------------------------------------

  copyright:             Copyright (c) 1998-2005 SAP AG



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

#ifndef HWD115CONTAINER_H
#define HWD115CONTAINER_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd26err.h"
#include "hwd103inode.h"
#include "hwd104fsdesc.h"

/*!**********************************************************************

  Chapter: twd115ContainerDescP

  description:
  
************************************************************************/

typedef struct st_container_desc *twd115ContainerDescP;

twd115ContainerDescP wd115CreateContainerDesc( twd104FSDescP   fsdesc,
                                               SQLHDBC         hdbc,
                                               const char     *viewOwner,
                                               sapdbwa_UInt4   partitionId,
                                               twd26ErrP       err );


void wd115DestroyContainerDesc(  twd115ContainerDescP desc );


sapdbwa_Bool wd115InsertContainer( twd115ContainerDescP desc,
                                 twd103InodeP         inode,
                                 sapdbwa_Bool           compress,
                                 sapdbwa_Bool           compressedData,
                                 twd26ErrP            err );


sapdbwa_Bool wd115DeleteContainer( twd115ContainerDescP  desc,
                                   twd103InodeId        *inodeId,
                                   twd26ErrP             err      );


sapdbwa_Bool wd115DbOpenContainer( twd115ContainerDescP desc,
                                 twd103InodeP         Inode,
                                 sapdbwa_Bool           uncompress,
                                 twd26ErrP            err );


sapdbwa_Bool wd115DbOpenContForUpdate( twd115ContainerDescP desc,
                                     twd103InodeP         Inode,
                                     sapdbwa_Bool           compress,
                                     sapdbwa_Bool           compressedData,
                                     twd26ErrP            err );


sapdbwa_Bool wd115DbReadContainer( twd115ContainerDescP desc,
                                 twd103InodeP         Inode,
                                 char                *Buf,
                                 sapdbwa_UInt4          Length,
                                 sapdbwa_UInt4         *Rtc,
                                 twd26ErrP            err );


sapdbwa_Bool wd115DbWriteContainer( twd115ContainerDescP desc,
                                  twd103InodeP         Inode,
                                  char                *Buf,
                                  sapdbwa_UInt4          Length,
                                  sapdbwa_UInt4         *Rtc,
                                  twd26ErrP            err );


sapdbwa_Bool wd115DbWriteCompressed( twd115ContainerDescP desc,
                                   twd103InodeP         Inode,
                                   twd105DynBufP        dynBuf,
                                   sapdbwa_UInt4         *Rtc,
                                   twd26ErrP            err );


sapdbwa_Bool wd115DbCloseContainer( twd115ContainerDescP desc,
                                  twd103InodeP         Inode,
                                  twd26ErrP            err );


/*!**********************************************************************

  EndChapter: twd115ContainerDescP

************************************************************************/
#endif

