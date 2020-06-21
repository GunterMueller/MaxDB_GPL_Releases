/*!**********************************************************************

  module: vwd119copy.c

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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "hwd119copy.h"

#include "SAPDBCommon/SAPDB_string.h"

#define RECURSIVE_SELECT_STMT_1 "SELECT InodeId,Name,Type,Container,Size,LinkInode FROM"
#define RECURSIVE_SELECT_STMT_2 "WHERE ParentDirectory=?"

sapdbwa_Bool wd119_Prepare( twd119CopyDescP desc,
                          twd26ErrP       err );

sapdbwa_Bool wd119_InsertInode( twd104FSDescP      fsdesc,
                              twd103InodeId       *directory,
                              twd103InodeId       *linkInode,
                              char                *name,
                              enum en_inode_wd103  inodeType,
                              twd103InodeP         newInode,
                              twd26ErrP            err );

/* copy the container from sourceInode to destInode */
sapdbwa_Bool wd119_CopyContainer( twd119CopyDescP desc,
                                twd104FSDescP     fsDesc,
                                twd103InodeP      sourceInode,
                                twd103InodeP      destInode,
                                twd26ErrP         err );

/* copy recursively all subdirectories and files under sourceInode to
 * destParentInode */
sapdbwa_Bool wd119_CopyInode( twd119CopyDescP desc,
                            twd104FSDescP     fsDesc,
                            twd103InodeP      sourceInode,
                            twd103InodeP      destParentInode,
                            sapdbwa_Bool      withLinks,
                            twd26ErrP         err );

/*!**********************************************************************

  Chapter: twd119CopyDescP

************************************************************************/

struct st_copy_desc
{
    twd104FSDescP	 fsDesc;
    SQLHDBC			 hdbc;
    const char		*viewOwner;
    sapdbwa_UInt4	 partitionId;	/* User's partition */

    SQLHSTMT         selectHstmt;
    sapdbwa_Bool     prepared;

    /* Binding for inode typename */
    char             typeBuffer[MAX_TYPENAME_LEN_WD103+1];
    SQLLEN           typeIndi;

    twd103Inode      inode;       /* Bindings for the output parameters */
    SQLLEN           inodeIdIndi;
    SQLLEN           inodeNameIndi;
    SQLLEN           containerIndi;
    SQLLEN           inodeSizeIndi;
    SQLLEN           linkInodeIndi;
}; /* struct st_copy_desc */


twd119CopyDescP wd119CreateCopyDesc( twd104FSDescP	 fsDesc,
                                     SQLHDBC		 hdbc,
                                     const char		*viewOwner,
                                     sapdbwa_UInt4	 partitionId	)
{
    sapdbwa_Bool       allocatOK;
    twd119CopyDescP  newDesc;

    sapdbwa_SQLALLOCAT( sizeof( struct st_copy_desc),
                   (sapdbwa_UInt1**) &newDesc,
                   &allocatOK );
    if (!allocatOK) {
        newDesc = NULL;
    } else {
        /* init newDesc */
        newDesc->fsDesc      = fsDesc;
        newDesc->hdbc        = hdbc;
        newDesc->viewOwner   = viewOwner;
        newDesc->partitionId = partitionId;
        newDesc->prepared    = sapdbwa_False;
    }; /* else */

    return newDesc;
} /* wd119CreateCopyDesc */


void wd119DestroyCopyDesc(  twd119CopyDescP desc )
{
    if (desc->prepared) {
        SQLFreeStmt( desc->selectHstmt, SQL_DROP );
    }; /* if */

    sapdbwa_SQLFREE( (sapdbwa_UInt1*) desc );
} /* wd119DestroyCopyDesc */


sapdbwa_Bool wd119Copy( twd119CopyDescP  desc,
                        twd103InodeP     sourceInode,
                        twd103InodeP     destDir,
                        const char      *newName,
                        sapdbwa_Bool     withLinks,
                        twd26ErrP        err )
{
    sapdbwa_Bool   copyOk   = sapdbwa_True;
    twd103Inode  newInode;
    SQLRETURN    sqlRetcode = SQL_SUCCESS;
    twd104FSDescP localFSDesc = NULL;

    if (!desc->prepared) {
        if (wd119_Prepare( desc, err )) {
            desc->prepared = sapdbwa_True;
        } else {
            copyOk = sapdbwa_False;
        }; /* else */
    }; /* if */

    if (copyOk) {
        twd103InodeType  newInodeType = no_inode;
        twd103InodeId    linkInodeId;
        
        if (withLinks && newInodeType == inode_file) {
            /* generate link. Dont copy file. */
            newInodeType = inode_link;
            SAPDB_memcpy( &linkInodeId, &sourceInode->InodeId, sizeof( linkInodeId ));
        } else {
            /* make copy copy of file dir or link */
            newInodeType = sourceInode->Type;
            SAPDB_memcpy( &linkInodeId, &sourceInode->LinkInodeId, sizeof( linkInodeId ));
        };
        desc->inodeIdIndi = MAX_SYSKEY_LEN_WD103;
        copyOk = wd119_InsertInode( desc->fsDesc,
                                    &destDir->InodeId,
                                    &linkInodeId,
                                    (char*) newName,
                                    newInodeType, &newInode, err );
        if (copyOk) {
            localFSDesc = wd104CreateFSDesc( SQL_NULL_HENV, desc->hdbc,
                                             desc->viewOwner, desc->partitionId,
                                             wd104IsOldSchema(desc->fsDesc),
                                             wd104GetTableContainer(desc->fsDesc),
                                             wd104GetTableInode(desc->fsDesc),
                                             err );
            if (localFSDesc != NULL) {
                copyOk = wd119_CopyInode( desc,
                                          localFSDesc,
                                          sourceInode,
                                          &newInode,
                                          withLinks,
                                          err );
                wd104DestroyFSDesc( localFSDesc );
            }; /* if */
        } else {
            /* when we get an integrity constraint violation then
             * a file or dir with newName already exists. */
            if (wd26GetId( err ) == ERR_ODBC_WD26
                && wd26GetNativeOdbcErr( err ) == 250) {
                wd26SetErr( err,
                            ERR_FILE_OR_DIR_EXISTS_WD26,
                            newName,
                            NULL );                
            }; /* if */
        }; /* else */
    }; /* if */

    return copyOk;
} /* wd119Copy */

/*!**********************************************************************

  EndChapter: twd119CopyDescP

************************************************************************/
/*!**********************************************************************

  Chapter: twd119InodeStack

************************************************************************/

typedef struct
{
    sapdbwa_Int4      stackSize;
    sapdbwa_Int4      stackPos;
    twd103Inode    *stackArray;
} twd119InodeStack;


sapdbwa_Bool wd119_InitStack( twd119InodeStack *stack,
                            sapdbwa_Int4        initialStackSize )
{
    sapdbwa_Bool allocatOK = sapdbwa_True;
    
    sapdbwa_SQLALLOCAT( initialStackSize*sizeof(twd103Inode),
                   (sapdbwa_UInt1**) &stack->stackArray,
                   &allocatOK );
    if (allocatOK) {
        stack->stackSize = initialStackSize;
        stack->stackPos = 0;
    }; /* if */

    return allocatOK;
} /* wd119_InitStack */


void wd119_ExitStack( twd119InodeStack *stack )
{
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) stack->stackArray );
    stack->stackArray = NULL;
} /* wd119_ExitStack */


sapdbwa_Bool wd119_Push( twd119InodeStack *stack, twd103Inode *inode )
{
    sapdbwa_Bool allocatOK = sapdbwa_True;

    if (stack->stackPos >= stack->stackSize) {
        stack->stackSize *= 2;
        sapdbwa_SQLREALLOCAT( stack->stackSize*sizeof(twd103Inode),
                         (sapdbwa_UInt1**) &stack->stackArray,
                         &allocatOK );
    }; /* if */

    if (allocatOK) {
        SAPDB_memcpy( &stack->stackArray[stack->stackPos++],
                inode, sizeof(twd103Inode));
    }; /* if */

    return allocatOK;
} /* wd119_Push */


sapdbwa_Bool wd119_Pop( twd119InodeStack *stack, twd103Inode *inode )
{
    if (stack->stackPos > 0) {
        stack->stackPos--;
        SAPDB_memcpy( inode,
                &stack->stackArray[stack->stackPos], sizeof(twd103Inode));
        return sapdbwa_True;
    } else {
        return sapdbwa_False;
    }; /* else */
} /* wd119_Pop */


/*!**********************************************************************

  EndChapter: twd119InodeStack

************************************************************************/
/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

sapdbwa_Bool wd119_Prepare( twd119CopyDescP desc,
                            twd26ErrP       err )
{
    sapdbwa_Bool prepareOK = sapdbwa_True;
    SQLRETURN  sqlRetcode = SQL_SUCCESS;
    DBFS_STMT;

    /* prepare recursive select */
    sqlRetcode = SQLAllocStmt( desc->hdbc, &desc->selectHstmt );
    if (wd00Success( sqlRetcode )) {
        if (wd00Success(sqlRetcode)) {
            SET_DBFS_STMT(RECURSIVE_SELECT_STMT_1, wd104GetTableInode(desc->fsDesc), RECURSIVE_SELECT_STMT_2);
            sqlRetcode = SQLPrepare( desc->selectHstmt, (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
            if (( sqlRetcode =
                  SQLBindCol( desc->selectHstmt,
                              1,
                              SQL_C_BINARY,
                              &desc->inode.InodeId,
                              MAX_SYSKEY_LEN_WD103,
                              &desc->inodeIdIndi ) == SQL_SUCCESS)
                && (sqlRetcode =
                    SQLBindCol( desc->selectHstmt,
                                2,
                                SQL_C_WCHAR,
                                desc->inode.NameUCS2,
                                2*(DB_MAX_NAME_LEN_WD103+1),
                                &desc->inodeNameIndi ) == SQL_SUCCESS)
                && (sqlRetcode =
                    SQLBindCol( desc->selectHstmt,
                                3,
                                SQL_C_CHAR,
                                desc->typeBuffer,
                                MAX_TYPENAME_LEN_WD103,
                                &desc->typeIndi ) == SQL_SUCCESS)
                && (sqlRetcode =
                    SQLBindCol( desc->selectHstmt,
                                4,
                                SQL_C_BINARY,
                                &desc->inode.ContainerId,
                                MAX_SYSKEY_LEN_WD103,
                                &desc->containerIndi ) == SQL_SUCCESS)
                && (sqlRetcode =
                    SQLBindCol( desc->selectHstmt,
                                5,
                                SQL_C_ULONG,
                                &(desc->inode.Size),
                                0,
                                &desc->inodeSizeIndi ) == SQL_SUCCESS)
                && (sqlRetcode =
                    SQLBindCol( desc->selectHstmt,
                                6,
                                SQL_C_BINARY,
                                &desc->inode.LinkInodeId,
                                MAX_SYSKEY_LEN_WD103,
                                &desc->linkInodeIndi ) == SQL_SUCCESS)) {
            }; /* if */
        }; /* if */
        if (!wd00Success(sqlRetcode)) {
            wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, SQL_NULL_HDBC,
                              desc->selectHstmt );
            prepareOK = sapdbwa_False;
        }; /* if */
    } else {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->selectHstmt );
        prepareOK = sapdbwa_False;
    }; /* else */

    return prepareOK;
} /* wd119_Prepare */


sapdbwa_Bool wd119_CopyContainer( twd119CopyDescP  desc,
                                twd104FSDescP    fsDesc,
                                twd103InodeP     sourceInode,
                                twd103InodeP     destInode,
                                twd26ErrP        err )
{
    sapdbwa_Bool copyOk = sapdbwa_True;
    
    /* create container */
    if ((copyOk = wd104DbOpenContainer( desc->fsDesc,
                                        sourceInode, sapdbwa_False, err ))
        && (copyOk=wd104InsertContainer( fsDesc,
                                         destInode, sapdbwa_False,
                                         sourceInode->compressed, err ))) {
            char         copyBuffer[4096];
            sapdbwa_UInt4  bytesRead    = 0;
            sapdbwa_UInt4  bytesWritten = 0;
            
            do {
                copyOk = wd104DbReadContainer( desc->fsDesc,
                                               sourceInode,
                                               copyBuffer,
                                               sizeof( copyBuffer ),
                                               &bytesRead, err );
                if (copyOk && bytesRead > 0) {
                    copyOk = wd104DbWriteContainer( fsDesc,
                                                    destInode,
                                                    copyBuffer,
                                                    bytesRead,
                                                    &bytesWritten,
                                                    err );
                }; /* if */
            } while (copyOk && bytesRead > 0);
    }; /* if */
    /* because we read without uncompressing, we have have to set the size
     * manually PTS 1105554 */
    destInode->Size = sourceInode->Size;
    copyOk = (wd104DbCloseContainer( fsDesc, destInode, err )
              && wd104DbCloseContainer( desc->fsDesc, sourceInode, err ));

    return copyOk;
} /* wd119_CopyContainer */


sapdbwa_Bool wd119_CopyInode( twd119CopyDescP  desc,
							  twd104FSDescP    fsDesc,
							  twd103InodeP     sourceInode,
							  twd103InodeP     destParentInode,
							  sapdbwa_Bool     withLinks,
							  twd26ErrP        err				)
{
    sapdbwa_Bool      copyOk = sapdbwa_True;
    SQLRETURN         sqlRetcode = SQL_SUCCESS;
    twd119InodeStack  inodeStack;
    twd119InodeStack  newInodeStack;
    twd103Inode       actInode;
    twd103Inode       newInode;
    SQLLEN            inodeIndi = MAX_SYSKEY_LEN_WD103;
	tsp81_UCS2Char  *srcAtUCS2 = NULL;
    tsp00_Byte		*destAtUCS2 = NULL;
    
    copyOk = wd119_InitStack( &inodeStack, 100 );
    if (copyOk) {
        copyOk = wd119_InitStack( &newInodeStack, 100 );
        if (!copyOk) {
            wd119_ExitStack( &inodeStack );
        };
    };

	/* Bind input parameter for parent directory id */
	sqlRetcode = SQLBindParameter( desc->selectHstmt,
								   1,
								   SQL_PARAM_INPUT,
								   SQL_C_BINARY,
								   SQL_BINARY,
								   0,
								   0,
								   &actInode,
								   MAX_SYSKEY_LEN_WD103,
								   &inodeIndi );

    if (copyOk && wd00Success(sqlRetcode)) {
        copyOk = wd119_Push( &inodeStack, sourceInode );
        copyOk = wd119_Push( &newInodeStack, destParentInode );
        while (copyOk && wd00Success(sqlRetcode)
               && wd119_Pop( &inodeStack, &actInode )
               && wd119_Pop( &newInodeStack, &newInode )) {
            if (!withLinks && actInode.Type == inode_file) {
                /* copy container */
                copyOk = wd119_CopyContainer( desc, fsDesc,
                                              &actInode, &newInode,
                                              err );
            } else { /* directory */
                /* get childs */
                sqlRetcode = SQLExecute( desc->selectHstmt );
                while (wd00Success(sqlRetcode) && copyOk) {
                    sqlRetcode = SQLFetch( desc->selectHstmt );
                    if (wd00Success(sqlRetcode)) {
                        twd103Inode    insertedInode;
                        twd103InodeId  linkInodeId;
                        
                        /* Convert UCS2 strings to UTF8 strings */
                        sp83UTF8ConvertFromUCS2((const tsp81_UCS2Char*)desc->inode.NameUCS2,  
							        (const tsp81_UCS2Char*)desc->inode.NameUCS2+sp81UCS2strlen(desc->inode.NameUCS2)*2,
							        (tsp81_UCS2Char const **)&srcAtUCS2, WD_IS_UCS2_SWAPED,
                                    (tsp00_Byte*)desc->inode.Name, (tsp00_Byte*)desc->inode.Name+DB_MAX_NAME_LEN_WD103,
                                    &destAtUCS2);

                        desc->inode.Type =
                            wd103Text2InodeType( desc->typeBuffer );
                        /* create new inode */
                        if (withLinks && (desc->inode.Type == inode_file)) {
                            /* create link to original file */
                            SAPDB_memcpy( &linkInodeId,
                                    &desc->inode.InodeId,
                                    sizeof( linkInodeId ));
                            desc->inode.Type = inode_link;
                        } else {
                            /* copy file */
                            SAPDB_memcpy( &linkInodeId,
                                    &desc->inode.LinkInodeId,
                                    sizeof( linkInodeId ));
                        };
                        copyOk = wd119_InsertInode( desc->fsDesc,
                                                    &newInode.InodeId,
                                                    &linkInodeId,
                                                    desc->inode.Name,
                                                    desc->inode.Type,
                                                    &insertedInode, err );
                        if (copyOk) {
                            if (desc->inode.Type == inode_file) {
                                /* copy files immetiately */
                                copyOk = wd119_CopyContainer( desc,
                                                              fsDesc,
                                                              &desc->inode,
                                                              &insertedInode,
                                                              err );
                            } else if (desc->inode.Type == inode_directory) {
                                /* put dir inodes on stack */
                                copyOk = wd119_Push( &inodeStack,
                                                     &desc->inode )
                                    && wd119_Push( &newInodeStack,
                                                   &insertedInode );
                            }; /* else */
                        }; /* if */
                    }; /* if */
                }; /* while */
                sqlRetcode = SQLFreeStmt( desc->selectHstmt, SQL_CLOSE ); 
            }; /* else */
        }; /* while */
        wd119_ExitStack( &inodeStack );
        wd119_ExitStack( &newInodeStack );        
    }; /* if */

    if (!wd00Success( sqlRetcode ) && sqlRetcode != SQL_NO_DATA_FOUND) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, desc->hdbc,
                          desc->selectHstmt );
        copyOk = sapdbwa_False;
    }; /* if */

    return copyOk;
} /* wd119_CopyInode */


sapdbwa_Bool wd119_InsertInode( twd104FSDescP        fsdesc,
                                twd103InodeId       *directory,
                                twd103InodeId       *linkInode,
                                char                *name,
                                enum en_inode_wd103  inodeType,
                                twd103InodeP         newInode,
                                twd26ErrP            err )
{
    sapdbwa_Bool  insertOk = sapdbwa_True;
    
    insertOk = wd104DbInsertInode( fsdesc,
                                   directory,
                                   linkInode,
                                   name,
                                   inodeType,
                                   newInode,
                                   err );
    if (insertOk) {
        insertOk = wd104DbGetInodeByName( fsdesc,
                                          directory,
                                          name,
                                          newInode,
                                          err );
    };


    return insertOk;
} /* wd119_InsertInode */

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/
