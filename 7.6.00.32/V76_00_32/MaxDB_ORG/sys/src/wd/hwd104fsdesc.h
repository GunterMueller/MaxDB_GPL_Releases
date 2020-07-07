/*!**********************************************************************

  module: hwd104fsdesc.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem descriptor

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

#ifndef HWD104FSDESC_H
#define HWD104FSDESC_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd26err.h"
#include "hwd101dbfs.h"
#include "hwd103inode.h"
#include "hwd105dynbuf.h"
#include "hwd106pool.h"

/*!**********************************************************************

  Chapter: twd104FSDescP

  description:
  
************************************************************************/

#define DBFS_TABLE_OWNER_WD104 "SYSDBA."

typedef struct st_fsdesc *twd104FSDescP;

/* Create and Destroy
 */
twd104FSDescP wd104CreateFSDesc( SQLHENV		 henv,
                                 SQLHDBC		 hdbc,
                                 const char		*viewOwner,
                                 sapdbwa_UInt4	 partitionId,
                                 sapdbwa_Bool    isOldSchema,
                                 const char     *tableContainer,
                                 const char     *tableInode,
                                 twd26ErrP		 err		);


void wd104SetConn( twd104FSDescP   fsdesc,
                   twd106ConnP     conn );


sapdbwa_Bool wd104DestroyFSDesc( twd104FSDescP fsdesc );

const char* wd104GetTableContainer( twd104FSDescP fsDesc );
const char* wd104GetTableInode( twd104FSDescP fsDesc );
sapdbwa_Bool wd104IsOldSchema( twd104FSDescP fsDesc );

SQLHDBC wd104GetHDBC(twd104FSDescP fsDesc);
sapdbwa_UInt4 wd104GetPartitionId(twd104FSDescP fsDesc);

/* Inode handling
 */
sapdbwa_Bool wd104DbDeleteInode( twd104FSDescP   fsdesc,
                                 twd103InodeP    Inode,
                                 twd26ErrP       err );


sapdbwa_Bool wd104DbDeleteInodeRecursive( twd104FSDescP   fsdesc,
                                        twd103InodeP    Inode,
                                        twd26ErrP       err );


sapdbwa_Bool wd104DbGetInodeByName( twd104FSDescP   fsdesc,
                                  twd103InodeId  *Directory,
                                  char           *Name,
                                  twd103InodeP    Inode,
                                  twd26ErrP       err );


sapdbwa_Bool wd104GetInodeById( twd104FSDescP        fsdesc,
                              twd103InodeId       *inodeId,
                              twd103InodeP         inode,
                              twd26ErrP            err );

/* newInode is only set if inodeType is inode_file */
sapdbwa_Bool wd104DbInsertInode( twd104FSDescP        fsdesc,
                               twd103InodeId       *directory,
                               twd103InodeId       *linkInode,
                               char                *name,
                               enum en_inode_wd103  inodeType,
                               twd103InodeP         newInode,
                               twd26ErrP            err );


sapdbwa_Bool wd104MoveInode( twd104FSDescP        fsdesc,
                           twd103InodeId       *inodeId,
                           twd103InodeId       *newParentInodeId,
                           const char          *newName,
                           twd26ErrP            err );


sapdbwa_Bool wd104Copy( twd104FSDescP           desc,
                      twd103InodeP            sourceInode,
                      twd103InodeP            destDir,
                      const char             *newName,
                      sapdbwa_Bool              withLinks,
                      twd26ErrP               err );

/* Container functions
 */
sapdbwa_Bool wd104InsertContainer( twd104FSDescP   fsdesc,
                                 twd103InodeP    inode,
                                 sapdbwa_Bool      compress,
                                 sapdbwa_Bool      compressedData,
                                 twd26ErrP       err );


/* open container for Inode. Inode->compressed is set true or false.
 * With uncompress you can choose whether the data should be uncompressed
 * when reading with wd104DbReadContainer. */
sapdbwa_Bool wd104DbOpenContainer( twd104FSDescP        fsdesc,
                                 twd103InodeP         Inode,
                                 sapdbwa_Bool           uncompress,
                                 twd26ErrP            err );

/* Open container for overwriting */
sapdbwa_Bool wd104DbOpenContForUpdate( twd104FSDescP        fsdesc,
                                     twd103InodeP         Inode,
                                     sapdbwa_Bool           compress,
                                     sapdbwa_Bool           compressedData,
                                     twd26ErrP            err );


/* Read dat from a container, that was opened with wd104DbOpenContainer
 * compressed data is only uncompressed if the uncompress flag of
 * wd104DbOpenContainer was true. In Rtc the number of bytes written to
 * Buf is returned. */
sapdbwa_Bool wd104DbReadContainer( twd104FSDescP        fsdesc,
                                 twd103InodeP         Inode,
                                 char                *Buf,
                                 sapdbwa_UInt4          Length,
                                 sapdbwa_UInt4         *Rtc,
                                 twd26ErrP            err );


sapdbwa_Bool wd104DbWriteCompressed( twd104FSDescP        fsdesc,
                                   twd103InodeP         Inode,
                                   twd105DynBufP        dynBuf,
                                   sapdbwa_UInt4         *Rtc,
                                   twd26ErrP            err );

/* note: when writing compressed data Length must be at
 * least 8 for the first call of wd104DbWriteContainer, because
 * the total length it taken from the compress header, which is 8 bytes
 * long. */
sapdbwa_Bool wd104DbWriteContainer( twd104FSDescP        fsdesc,
                                  twd103InodeP         Inode,
                                  char                *Buf,
                                  sapdbwa_UInt4          Length,
                                  sapdbwa_UInt4         *Rtc,
                                  twd26ErrP            err );


sapdbwa_Bool wd104DbCloseContainer( twd104FSDescP        fsdesc,
                                  twd103InodeP         Inode,
                                  twd26ErrP            err );


/* Directory Listing
 */
sapdbwa_Bool wd104DbSelectInodeByDirectory( twd104FSDescP    fsDesc,
                                          twd103InodeId   *inodeId,
                                          twd101OrderFlag  orderFlag,
                                          twd26ErrP        err );


sapdbwa_Bool wd104DbFetchInodeByDirectory( twd104FSDescP    fsDesc,
                                         twd103InodeP     inode,
                                         twd26ErrP        err );

sapdbwa_Bool wd104DbDeselectInodeByDirectory( twd104FSDescP   fsDesc,
                                            twd26ErrP       err );

/* misc
 */
sapdbwa_Bool wd104FormatDBFS( twd104FSDescP fsdesc, twd26ErrP err );


sapdbwa_Bool wd104DbTransaction( twd104FSDescP        fsdesc,
                               sapdbwa_Bool           commit,
                               twd26ErrP            err );


sapdbwa_Bool wd104Rename( twd104FSDescP  fsdesc,
                        twd103InodeP   inode,
                        const char    *newName,
                        twd26ErrP      err );

/*!**********************************************************************

  EndChapter: twd104FSDescP

************************************************************************/
#endif
