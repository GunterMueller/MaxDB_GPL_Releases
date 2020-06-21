/*!**********************************************************************

  module: vwd104fsdesc.h

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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/
#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>

#include "hwd104fsdesc.h"
#include "hwd111getinode.h"
#include "hwd112insinode.h"
#include "hwd113delinode.h"
#include "hwd114seldir.h"
#include "hwd115container.h"
#include "hwd117reninode.h"
#include "hwd118moveinode.h"
#include "hwd119copy.h"

/*!**********************************************************************

  prototypes and constants for internal functions

************************************************************************/

#define DELETE_INODE_STMT_PRE   "DELETE FROM"
#define DELETE_INODE_STMT_POST  "Where PartitionId = %d"

sapdbwa_Bool wd104_ExecSql( SQLHDBC         hdbc,
                            char           *stmtStr,
                            sapdbwa_UInt4	partitionId,
                            twd26ErrP       err );

sapdbwa_Bool wd104_MakeContainerDesc( twd104FSDescP fsdesc, twd26ErrP err );

sapdbwa_Bool wd104_MakeSelectDirDesc( twd104FSDescP fsdesc, twd26ErrP err );

/*!**********************************************************************

  Chapter: twd104FSDescP

************************************************************************/


struct st_fsdesc
{
    SQLHENV                 henv;
    SQLHDBC                 hdbc;
    sapdbwa_Bool            isOldSchema;
    const char             *tableContainer;
    const char             *tableInode;
    sapdbwa_UInt4			partitionId;	/* User's partition	*/
    char                    viewOwner[DB_MAX_NAME_LEN_WD103+1];
    
    twd111GetInodeDescP     getInodeDesc;
    twd112InsertInodeDescP  insertInodeDesc;
    twd113DeleteInodeDescP  deleteInodeDesc;
    twd114SelectDirDescP    selectDirDesc;
    twd115ContainerDescP    containerDesc;
    twd117RenameInodeDescP  renameInodeDesc;
    twd118MoveInodeDescP    moveInodeDesc;
    twd119CopyDescP         copyDesc;
}; /* struct st_fsdesc */

void wd104_toupper( char *str )
{
    char *pos = str;
    
    while ( *pos != '\0' ) {
        *pos = toupper( *pos );
        pos++;
    } ; /* while */
} /* wd104_toupper */

/*===========================================================================*/

const char* wd104GetTableContainer( twd104FSDescP fsDesc )
{

    if (fsDesc) {
        return fsDesc->tableContainer;
    } else {
        return NULL;
    };

}

/*===========================================================================*/

const char* wd104GetTableInode( twd104FSDescP fsDesc )
{

    if (fsDesc) {
        return fsDesc->tableInode;
    } else {
        return NULL;
    };

}

/*===========================================================================*/

sapdbwa_Bool wd104IsOldSchema( twd104FSDescP fsDesc )
{

    if (fsDesc) {
        return fsDesc->isOldSchema;
    } else {
        return sapdbwa_False;
    };

}

/*===========================================================================*/

SQLHDBC wd104GetHDBC(twd104FSDescP fsDesc)
{

    return fsDesc->hdbc;

}

/*===========================================================================*/

sapdbwa_UInt4 wd104GetPartitionId(twd104FSDescP fsDesc)
{

    return fsDesc->partitionId;

}

/*===========================================================================*/

twd104FSDescP wd104CreateFSDesc( SQLHENV		 henv,
                                 SQLHDBC		 hdbc,
                                 const char		*viewOwner,
                                 sapdbwa_UInt4	 partitionId,
                                 sapdbwa_Bool    isOldSchema,
                                 const char     *tableContainer,
                                 const char     *tableInode,
                                 twd26ErrP		 err		)
{
    sapdbwa_Bool     allocatOK;
    twd104FSDescP  newFSDesc;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_fsdesc),
						(sapdbwa_UInt1**) &newFSDesc,
						&allocatOK );

    if (!allocatOK) {
        newFSDesc = NULL;
        wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
    } else {
        newFSDesc->henv            = henv;
        newFSDesc->hdbc            = hdbc;
        newFSDesc->isOldSchema     = isOldSchema;
        newFSDesc->tableContainer  = tableContainer;
        newFSDesc->tableInode      = tableInode;
        newFSDesc->getInodeDesc    = NULL;
        newFSDesc->insertInodeDesc = NULL;
        newFSDesc->deleteInodeDesc = NULL;
        newFSDesc->selectDirDesc   = NULL;
        newFSDesc->containerDesc   = NULL;
        newFSDesc->renameInodeDesc = NULL;
        newFSDesc->moveInodeDesc   = NULL;
        newFSDesc->copyDesc        = NULL;
        newFSDesc->partitionId     = partitionId;
        strncpy( newFSDesc->viewOwner, viewOwner,
                 sizeof(newFSDesc->viewOwner)-1);
        newFSDesc->viewOwner[sizeof(newFSDesc->viewOwner)-1] = '\0';
        if (newFSDesc->viewOwner[0] != '"') {
            wd104_toupper( newFSDesc->viewOwner );
        };
    }; /* else */

    return newFSDesc;
} /* wd104CreateFSDesc */


sapdbwa_Bool wd104DestroyFSDesc( twd104FSDescP fsdesc )
{
    /* free descs */
    if ( fsdesc->getInodeDesc != NULL ) {
        wd111DestroyGetInodeDesc( fsdesc->getInodeDesc );
    }; /* if */
    if ( fsdesc->insertInodeDesc != NULL ) {
        wd112DestroyInsertInodeDesc( fsdesc->insertInodeDesc );
    }; /* if */
    if ( fsdesc->deleteInodeDesc != NULL ) {
        wd113DestroyDeleteInodeDesc( fsdesc->deleteInodeDesc );
    }; /* if */
    if ( fsdesc->selectDirDesc != NULL ) {
        wd114DestroySelectDirDesc( fsdesc->selectDirDesc );
    }; /* if */
    if ( fsdesc->containerDesc != NULL ) {
        wd115DestroyContainerDesc( fsdesc->containerDesc );
    }; /* if */
    if ( fsdesc->renameInodeDesc != NULL ) {
        wd117DestroyRenameInodeDesc( fsdesc->renameInodeDesc );
    }; /* if */
    if ( fsdesc->moveInodeDesc != NULL ) {
        wd118DestroyMoveInodeDesc( fsdesc->moveInodeDesc );
    }; /* if */
    if ( fsdesc->copyDesc != NULL ) {
        wd119DestroyCopyDesc( fsdesc->copyDesc );
    }; /* if */

    sapdbwa_SQLFREE( (sapdbwa_UInt1*) fsdesc );

    return sapdbwa_True;
} /* wd104DestroyFSDesc */


sapdbwa_Bool wd104DbDeleteInode( twd104FSDescP   fsdesc,
                                 twd103InodeP    Inode,
                                 twd26ErrP       err )
{

    if (fsdesc->deleteInodeDesc == NULL) {
        fsdesc->deleteInodeDesc = wd113CreateDeleteInodeDesc( fsdesc,
                                                              fsdesc->hdbc,
                                                              fsdesc->partitionId );
        if (fsdesc->deleteInodeDesc == NULL) {
            wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
            return sapdbwa_False;
        }; /* if */
    }; /* if */
    
	/* Delete inode entry	*/
    return wd113DeleteInode( fsdesc, fsdesc->deleteInodeDesc, Inode, err );

}


sapdbwa_Bool wd104DbDeleteInodeRecursive( twd104FSDescP   fsdesc,
                                          twd103InodeP    Inode,
                                          twd26ErrP       err )
{
    if (fsdesc->deleteInodeDesc == NULL) {
        fsdesc->deleteInodeDesc =
            wd113CreateDeleteInodeDesc( fsdesc,
                                        fsdesc->hdbc,
                                        fsdesc->partitionId );
        if (fsdesc->deleteInodeDesc == NULL) {
            wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
            return sapdbwa_False;
        }; /* if */
    }; /* if */
          
    return wd113DeleteInodeRecursive( fsdesc, fsdesc->deleteInodeDesc, Inode, err );
} /* wd104DbDeleteInodeRecursive */


sapdbwa_Bool wd104FormatDBFS( twd104FSDescP fsdesc, twd26ErrP err )
{
    sapdbwa_Bool retcode = sapdbwa_True;
    DBFS_STMT;

    SET_DBFS_STMT(DELETE_INODE_STMT_PRE, wd104GetTableInode(fsdesc), DELETE_INODE_STMT_POST);

    if (wd104_ExecSql( fsdesc->hdbc, DBFS_STMT_VAR,
                           fsdesc->partitionId, err )) {
        retcode = sapdbwa_True;
    } else {
        retcode = sapdbwa_False;
    }; /* else */

    return retcode;
} /* wd104FormatDBFS */


sapdbwa_Bool wd104DbGetInodeByName( twd104FSDescP   fsdesc,
                                    twd103InodeId  *Directory,
                                    char           *Name,
                                    twd103InodeP    Inode,
                                    twd26ErrP       err )
{
    if (fsdesc->getInodeDesc == NULL) {
        fsdesc->getInodeDesc = wd111CreateGetInodeDesc(fsdesc);
        if (fsdesc->getInodeDesc == NULL) {
            wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
            return sapdbwa_False;
        }; /* if */
    }; /* if */
          
    return wd111DbGetInodeByName( fsdesc->getInodeDesc,
                                  Directory,
                                  Name,
                                  Inode,
                                  err );
} /* wd104DbGetInodeByName */


sapdbwa_Bool wd104GetInodeById( twd104FSDescP        fsdesc,
                                twd103InodeId       *inodeId,
                                twd103InodeP         inode,
                                twd26ErrP            err )
{
    if (fsdesc->getInodeDesc == NULL) {
        fsdesc->getInodeDesc = wd111CreateGetInodeDesc(fsdesc);
        if (fsdesc->getInodeDesc == NULL) {
            wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
            return sapdbwa_False;
        }; /* if */
    }; /* if */
          
    return wd111GetInodeById( fsdesc->getInodeDesc,
                              inodeId,
                              inode,
                              err );
} /* wd104GetInodeById */


sapdbwa_Bool wd104DbInsertInode( twd104FSDescP        fsdesc,
                                 twd103InodeId       *directory,
                                 twd103InodeId       *linkInode,
                                 char                *name,
                                 enum en_inode_wd103  inodeType,
                                 twd103InodeP         newInode,
                                 twd26ErrP            err )
{
    if (fsdesc->insertInodeDesc == NULL) {
        fsdesc->insertInodeDesc =
            wd112CreateInsertInodeDesc( fsdesc,
                                        fsdesc->hdbc,
                                        fsdesc->viewOwner,
                                        fsdesc->partitionId );
        if (fsdesc->insertInodeDesc == NULL) {
            wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
            return sapdbwa_False;
        }; /* if */
    }; /* if */
          
    return wd112DbInsertInode( fsdesc->insertInodeDesc,
                               directory,
                               linkInode,
                               name,
                               inodeType,
                               newInode,
                               err );
} /* wd104DbInsertInode */


sapdbwa_Bool wd104MoveInode( twd104FSDescP        fsdesc,
                             twd103InodeId       *inodeId,
                             twd103InodeId       *newParentInodeId,
                             const char          *newName,
                             twd26ErrP            err )
{
    if (fsdesc->moveInodeDesc == NULL) {
        fsdesc->moveInodeDesc = wd118CreateMoveInodeDesc(fsdesc);
        if (fsdesc->moveInodeDesc == NULL) {
            wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
            return sapdbwa_False;
        }; /* if */
    }; /* if */
          
    return wd118MoveInode( fsdesc->moveInodeDesc,
                           inodeId,
                           newParentInodeId,
                           newName,
                           err );
} /* wd104MoveInode */
                          

sapdbwa_Bool wd104Copy( twd104FSDescP       fsdesc,
                        twd103InodeP        sourceInode,
                        twd103InodeP        destDir,
                        const char         *newName,
                        sapdbwa_Bool        withLinks,
                        twd26ErrP           err )
{
    if (fsdesc->copyDesc == NULL) {
        fsdesc->copyDesc =
            wd119CreateCopyDesc( fsdesc,
                                 fsdesc->hdbc,
                                 fsdesc->viewOwner,
                                 fsdesc->partitionId );
        if (fsdesc->copyDesc == NULL) {
            wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
            return sapdbwa_False;
        }; /* if */
    }; /* if */
          
    return wd119Copy( fsdesc->copyDesc,
                      sourceInode, destDir, newName,
                      withLinks, err );
} /* wd104MoveInode */


sapdbwa_Bool wd104InsertContainer( twd104FSDescP   fsdesc,
                                 twd103InodeP    inode,
                                 sapdbwa_Bool      compress,
                                 sapdbwa_Bool      compressedData,
                                 twd26ErrP       err )
{
    if (!wd104_MakeContainerDesc( fsdesc, err )) {
        return sapdbwa_False;
    }; /* if */
    
    return wd115InsertContainer( fsdesc->containerDesc,
                                 inode, compress, compressedData, err );
} /* wd104InsertContainer */

/*****************************************************************************
 ***
 ***	wd104DeleteContainer
 ***
 *****************************************************************************/

sapdbwa_Bool wd104DeleteContainer( twd104FSDescP  fsdesc,
                                   twd103InodeId *inodeId,
                                   twd26ErrP      err    )
{

	/* Container management initialized?	*/
    if (!wd104_MakeContainerDesc( fsdesc, err )) {
        return sapdbwa_False;
    };
    
	/* Delete container record	*/
    return wd115DeleteContainer( fsdesc->containerDesc, inodeId, err );

}


sapdbwa_Bool wd104DbOpenContainer( twd104FSDescP        fsdesc,
                                 twd103InodeP         Inode,
                                 sapdbwa_Bool           uncompress,
                                 twd26ErrP            err )
{
    if (!wd104_MakeContainerDesc( fsdesc, err )) {
        return sapdbwa_False;
    }; /* if */

    return wd115DbOpenContainer( fsdesc->containerDesc, Inode,
                                 uncompress, err );
} /* wd104DbOpenContainer */


sapdbwa_Bool wd104DbOpenContForUpdate( twd104FSDescP        fsdesc,
                                     twd103InodeP         Inode,
                                     sapdbwa_Bool           compress,
                                     sapdbwa_Bool           compressedData,
                                     twd26ErrP            err )
{
    if (!wd104_MakeContainerDesc( fsdesc, err )) {
        return sapdbwa_False;
    }; /* if */

    return wd115DbOpenContForUpdate( fsdesc->containerDesc, Inode,
                                     compress, compressedData,
                                     err );    
} /* wd104DbOpenContForUpdate */


sapdbwa_Bool wd104DbReadContainer( twd104FSDescP        fsdesc,
                                 twd103InodeP         Inode,
                                 char                *Buf,
                                 sapdbwa_UInt4          Length,
                                 sapdbwa_UInt4         *Rtc,
                                 twd26ErrP            err )
{
    if (!wd104_MakeContainerDesc( fsdesc, err )) {
        return sapdbwa_False;
    }; /* if */

    return wd115DbReadContainer( fsdesc->containerDesc,
                                 Inode,
                                 Buf,
                                 Length,
                                 Rtc,
                                 err );
} /* wd104DbReadContainer */


sapdbwa_Bool wd104DbWriteCompressed( twd104FSDescP        fsdesc,
                                   twd103InodeP         Inode,
                                   twd105DynBufP        dynBuf,
                                   sapdbwa_UInt4         *Rtc,
                                   twd26ErrP            err )
{
    if (!wd104_MakeContainerDesc( fsdesc, err )) {
        return sapdbwa_False;
    }; /* if */
    return wd115DbWriteCompressed( fsdesc->containerDesc,
                                   Inode,
                                   dynBuf,
                                   Rtc,
                                   err );
} /* wd104DbWriteCompressed */


sapdbwa_Bool wd104DbWriteContainer( twd104FSDescP        fsdesc,
                                  twd103InodeP         Inode,
                                  char                *Buf,
                                  sapdbwa_UInt4          Length,
                                  sapdbwa_UInt4         *Rtc,
                                  twd26ErrP            err )
{
    if (!wd104_MakeContainerDesc( fsdesc, err )) {
        return sapdbwa_False;
    }; /* if */
    return wd115DbWriteContainer( fsdesc->containerDesc,
                                  Inode,
                                  Buf,
                                  Length,
                                  Rtc,
                                  err );
} /* wd104DbWriteContainer */


sapdbwa_Bool wd104DbCloseContainer( twd104FSDescP        fsdesc,
                                  twd103InodeP         Inode,
                                  twd26ErrP            err )
{
    if (!wd104_MakeContainerDesc( fsdesc, err )) {
        return sapdbwa_False;
    }; /* if */

    return wd115DbCloseContainer( fsdesc->containerDesc,
                                  Inode,
                                  err );
} /* wd104DbCloseContainer */



/* Directory Listing
 */
sapdbwa_Bool wd104DbSelectInodeByDirectory( twd104FSDescP    fsDesc,
                                          twd103InodeId   *inodeId,
                                          twd101OrderFlag  orderFlag,
                                          twd26ErrP        err )
{
    if (!wd104_MakeSelectDirDesc( fsDesc, err )) {
        return sapdbwa_False;
    }; /* if */

    return wd114DbSelectInodeByDirectory( fsDesc->selectDirDesc,
                                          inodeId,
                                          orderFlag,
                                          err );
} /* wd104DbSelectInodeByDirectory */


sapdbwa_Bool wd104DbFetchInodeByDirectory( twd104FSDescP    fsDesc,
                                         twd103InodeP     inode,
                                         twd26ErrP        err )
{
    if (!wd104_MakeSelectDirDesc( fsDesc, err )) {
        return sapdbwa_False;
    }; /* if */
    
    return wd114DbFetchInodeByDirectory( fsDesc->selectDirDesc,
                                         inode,
                                         err );
} /* wd104DbFetchInodeByDirectory */


sapdbwa_Bool wd104DbDeselectInodeByDirectory( twd104FSDescP   fsDesc,
                                            twd26ErrP       err )
{
    if (!wd104_MakeSelectDirDesc( fsDesc, err )) {
        return sapdbwa_False;
    }; /* if */

    return wd114DbDeselectInodeByDirectory( fsDesc->selectDirDesc, err );
} /* wd104DbDeselectInodeDirectory */


/*
 * `DbTransaction' terminates the current transaction for connection
 * `Connection' and starts a new transaction.
 * If `Commit' is True, the current transaction is committed, otherwise it is
 * rolled back.
 * A transaction is started implicitly when a database connection is
 * established or after a transaction is ended. Thus a call to `DbTransaction'
 * ends the current transaction and starts the next transaction.
 * If the transaction is succesfully ended, True is returned. 
 * Otherwise False is returned and an errortext is passed back 
 * through `ErrorText'.
 */
sapdbwa_Bool wd104DbTransaction( twd104FSDescP        fsdesc,
                               sapdbwa_Bool           commit,
                               twd26ErrP            err )
{
    sapdbwa_Bool retcode = sapdbwa_True;
    UWORD      SqlCommitRollback;
    RETCODE    sqlRetcode = SQL_SUCCESS;
    
    switch (commit) {
    case sapdbwa_True:
        SqlCommitRollback = SQL_COMMIT;
        break;
    default:
        SqlCommitRollback = SQL_ROLLBACK;
    }; /* switch */
    
    if (( sqlRetcode= SQLTransact( fsdesc->henv,
                                   fsdesc->hdbc,
                                   SqlCommitRollback)) != SQL_SUCCESS) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, fsdesc->hdbc, SQL_NULL_HSTMT );
        retcode = sapdbwa_False;
    }; /* if */
    
    return retcode;
} /* wd104DbTransaction */


sapdbwa_Bool wd104Rename( twd104FSDescP  fsdesc,
                          twd103InodeP   inode,
                          const char    *newName,
                          twd26ErrP      err )
{
    if (fsdesc->renameInodeDesc == NULL) {
        fsdesc->renameInodeDesc = wd117CreateRenameInodeDesc(fsdesc);
        if (fsdesc->renameInodeDesc == NULL) {
            return sapdbwa_False;
        }; /* if */
    }; /* if */
          
    return wd117RenameInode( fsdesc->renameInodeDesc, inode, newName, err );
} /* wd104Rename */

/*!**********************************************************************

  EndChapter: twd104FSDesc

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

sapdbwa_Bool wd104_ExecSql( SQLHDBC         hdbc,
                            char           *stmtStr,
                            sapdbwa_UInt4	partitionId,
                            twd26ErrP       err )
{
    sapdbwa_Bool  retcode = sapdbwa_True;
    RETCODE     sqlRetcode;
    SQLHSTMT    hstmt = SQL_NULL_HSTMT;
    char        stmt[MAX_SQLSTR_LEN_WD00];
    
    sqlRetcode = SQLAllocStmt( hdbc, &hstmt );
    if (sqlRetcode != SQL_SUCCESS) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT );
        retcode = sapdbwa_False;
    } else {
        sprintf( stmt, stmtStr, partitionId );
        sqlRetcode = SQLExecDirect( hstmt, (UCHAR*) stmt, SQL_NTS );
        if (!wd00Success(sqlRetcode)) {
            wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, hdbc, hstmt );
            retcode = sapdbwa_False;
        }; /* if */
        sqlRetcode = SQLFreeStmt( hstmt, SQL_DROP );
    }; /* else */

    return retcode;
} /* wd104_ExecSql */

sapdbwa_Bool wd104_MakeContainerDesc( twd104FSDescP fsdesc,
                                      twd26ErrP     err )
{
    if (fsdesc->containerDesc == NULL) {
        fsdesc->containerDesc = wd115CreateContainerDesc( fsdesc,
                                                          fsdesc->hdbc,
                                                          fsdesc->viewOwner,
                                                          fsdesc->partitionId,
                                                          err );
    }; /* if */

    return (fsdesc->containerDesc != NULL);
} /* wd104_MakeContainerDesc */


sapdbwa_Bool wd104_MakeSelectDirDesc( twd104FSDescP fsdesc, twd26ErrP err )
{
    if (fsdesc->selectDirDesc == NULL) {
        fsdesc->selectDirDesc = wd114CreateSelectDirDesc(fsdesc);
        if (fsdesc->selectDirDesc == NULL) {
            wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
        }; /* if */
    }; /* if */

    return (fsdesc->selectDirDesc != NULL);
} /* wd104_MakeSelectDirDesc */

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/
