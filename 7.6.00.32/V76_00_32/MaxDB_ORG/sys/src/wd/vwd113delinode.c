/*!**********************************************************************

  module: vwd113delinode.c

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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "hwd113delinode.h"

#include "SAPDBCommon/SAPDB_string.h"

/*!**********************************************************************

  Chapter: twd113DeleteInodeDescP

************************************************************************/

#define RECURSIVE_SELECT_STMT_1 "DECLARE C CURSOR FOR WITH RECURSIVE keys(InodeKey) AS ( SELECT InodeId FROM"
#define RECURSIVE_SELECT_STMT_2 "WHERE InodeId=? UNION ALL SELECT InodeId FROM"
#define RECURSIVE_SELECT_STMT_3 ", keys WHERE ParentDirectory=InodeKey ) SELECT InodeKey FROM keys"

#define SELECT_COUNT_STMT_1     "SELECT COUNT(*) FROM"
#define SELECT_COUNT_STMT_2     "WHERE ParentDirectory=?"

#define DELETE_STMT_1           "DELETE FROM"
#define DELETE_STMT_2           "WHERE InodeId = ? Or LinkInode = ?"

sapdbwa_Bool wd113_Prepare( twd113DeleteInodeDescP desc,
                          twd26ErrP              err );

struct st_delete_inode_desc
{
    twd104FSDescP    fsDesc;
    SQLHDBC          hdbc;
    sapdbwa_UInt4	 partitionId;	/* User's partition */

    SQLHSTMT         recursiveHstmt;
    SQLHSTMT         selectCountHstmt;
    SQLHSTMT         deleteHstmt;
    sapdbwa_Bool     prepared;

    twd103InodeId    inodeId;
    SQLLEN           cbInodeId;
    sapdbwa_Int4     count;
    
    SQLLEN           cbCount;
    SQLLEN           cbNode;
}; /* struct st_delete_inode_desc */

#define BEGEXCL_ODBC /* do we need this ? */
#define ENDEXCL_ODBC

twd113DeleteInodeDescP wd113CreateDeleteInodeDesc( twd104FSDescP fsDesc,
                                                   SQLHDBC       hdbc,
                                                   sapdbwa_UInt4 partitionId )
{
    sapdbwa_Bool              allocatOK;
    twd113DeleteInodeDescP  newDesc;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_delete_inode_desc),
                   (sapdbwa_UInt1**) &newDesc,
                   &allocatOK );
    if (!allocatOK) {
        newDesc = NULL;
    } else {
        /* init newDesc */
        newDesc->fsDesc      = fsDesc;
        newDesc->hdbc        = hdbc;
        newDesc->partitionId = partitionId;
        newDesc->prepared    = sapdbwa_False;
    }; /* else */

    return newDesc;
} /* wd113CreateDeleteInodeDesc */


void wd113DestroyDeleteInodeDesc(  twd113DeleteInodeDescP desc )
{
    if (desc->prepared) {
        SQLFreeStmt( desc->recursiveHstmt, SQL_DROP );
        SQLFreeStmt( desc->selectCountHstmt, SQL_DROP );
        SQLFreeStmt( desc->deleteHstmt, SQL_DROP );
    }; /* if */
    
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) desc );
} /* wd113DestroyDeleteInodeDesc */


/*
 * Delete the record from the inode table which corresponds to 'Inode'.
 *
 * We (!) can't (!) assume foreign key with delete cascade so that all chained information
 * (in the Container table) is deleted (see also WA.ins).
 */
sapdbwa_Bool wd113DeleteInode( twd104FSDescP          fsdesc,
							   twd113DeleteInodeDescP desc,
							   twd103InodeP           Inode,
                               twd26ErrP              err )
{
    RETCODE					sqlRetcode;
    SDWORD					cbNodeDummy = MAX_SYSKEY_LEN_WD103;
    char					deleteStmtStr[MAX_SQLSTR_LEN_WD00];
    
    if (!desc->prepared) {
        if (wd113_Prepare( desc, err )) {
            desc->prepared = sapdbwa_True;
        } else {
            return sapdbwa_False;
        }; /* else */
    }; /* if */

    /* check if directory is empty */
    desc->cbInodeId = MAX_SYSKEY_LEN_WD103;
    SAPDB_memcpy( &desc->inodeId, &Inode->InodeId, MAX_SYSKEY_LEN_WD103 );
    sqlRetcode = SQLExecute( desc->selectCountHstmt );
    if (wd00Success( sqlRetcode )) {
        sqlRetcode = SQLFetch( desc->selectCountHstmt );
        if (!wd00Success( sqlRetcode )) {
            /* fetch failed */
            wd26SetOdbcError( err, sqlRetcode,
                              SQL_NULL_HENV,
                              desc->hdbc, desc->selectCountHstmt );
            SQLFreeStmt( desc->selectCountHstmt, SQL_CLOSE );
            return sapdbwa_False;
        }; /* if */
        sqlRetcode = SQLFreeStmt( desc->selectCountHstmt, SQL_CLOSE );
    }; /* if */
    if (!wd00Success( sqlRetcode )) {
        /* execute or SQLFreeStmt failed */
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, desc->hdbc, desc->selectCountHstmt );
        return sapdbwa_False;
    }; /* if */
    if (desc->count > 0) {
        /* dir not empty */
        /* we dont have the path here so set err in vwd101dbfs */
        /* wd26SetErr( err, ERR_DIR_NOT_EMPTY_WD26, Path, NULL ); */
        return(sapdbwa_False);
    }; /* if */

    SAPDB_memcpy( &desc->inodeId, &Inode->InodeId, sizeof(twd103InodeId) );
    desc->cbInodeId = MAX_SYSKEY_LEN_WD103;
    
    sqlRetcode = SQLExecute( desc->deleteHstmt );
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, desc->hdbc, desc->deleteHstmt );
        return sapdbwa_False;
    }; /* if */

	/* Delete according container entries */
	return wd104DeleteContainer( fsdesc, &desc->inodeId, err );

}


sapdbwa_Bool wd113DeleteInodeRecursive( twd104FSDescP          fsdesc,
							            twd113DeleteInodeDescP desc,
							            twd103InodeP           Inode,
                                        twd26ErrP              err )
{
    sapdbwa_Bool			deleteOK = sapdbwa_True;
    SQLRETURN				sqlRetcode = SQL_SUCCESS;
    SQLLEN					cbNode;
    
    if (!desc->prepared) {
        if (wd113_Prepare( desc, err )) {
            desc->prepared = sapdbwa_True;
        } else {
            deleteOK = sapdbwa_False;
        }; /* else */
    }; /* if */

    if (deleteOK) {
        cbNode = MAX_SYSKEY_LEN_WD103;
        sqlRetcode = SQLBindParameter( desc->recursiveHstmt,
                                       1,
                                       SQL_PARAM_INPUT,
                                       SQL_C_BINARY,
                                       SQL_BINARY,
                                       0,
                                       0,
                                       &Inode->InodeId,
                                       MAX_SYSKEY_LEN_WD103,
                                       &cbNode );
        if (wd00Success( sqlRetcode)) {
            sqlRetcode = SQLExecute( desc->recursiveHstmt );
            if (wd00Success( sqlRetcode )) {
                do {
					/* Get inode to delete	*/
                    sqlRetcode = SQLFetch( desc->recursiveHstmt );

					/* Delete inode entry in inode table	*/
                    if (wd00Success(sqlRetcode)) {
                        /* output column of recursiveStmt and input parameter	*/
                        /* of deleteStmt are bound to desc->inodeId				*/
                        sqlRetcode = SQLExecute( desc->deleteHstmt );

						/* Delete inode entry in container table	*/
						deleteOK = wd104DeleteContainer( fsdesc, &desc->inodeId, err );
                    }; /* if */
                } while (wd00Success(sqlRetcode) && deleteOK == sapdbwa_True );
                sqlRetcode = SQLFreeStmt( desc->recursiveHstmt, SQL_CLOSE );
            }; /* if */
        }; /* if */
        if (!wd00Success(sqlRetcode) && sqlRetcode != SQL_NO_DATA_FOUND) {
            wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, SQL_NULL_HDBC,
                              desc->recursiveHstmt );
            deleteOK = sapdbwa_False;
        }; /* if */
    }; /* if */

    return deleteOK;
} /* wd113DeleteInodeRecursive */


/*!**********************************************************************

  EndChapter: twd111GetInodeDescP

************************************************************************/

sapdbwa_Bool wd113_Prepare( twd113DeleteInodeDescP desc,
                          twd26ErrP              err )
{
    sapdbwa_Bool prepareOK = sapdbwa_True;
    SQLRETURN  sqlRetcode = SQL_SUCCESS;
    DBFS_STMT;

    desc->cbInodeId = MAX_SYSKEY_LEN_WD103;
    /* prepare recursive select */
    sqlRetcode = SQLAllocStmt( desc->hdbc, &desc->recursiveHstmt );
    if (wd00Success( sqlRetcode )) {
        if (wd00Success(sqlRetcode)) {
            SET_DBFS_STMT_EX2(RECURSIVE_SELECT_STMT_1, wd104GetTableInode(desc->fsDesc),
                              RECURSIVE_SELECT_STMT_2, wd104GetTableInode(desc->fsDesc),
                              RECURSIVE_SELECT_STMT_3);
            sqlRetcode = SQLPrepare( desc->recursiveHstmt,
                                     (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
            if (wd00Success(sqlRetcode)) {
                sqlRetcode = SQLBindCol( desc->recursiveHstmt,
                                         1, SQL_C_BINARY,
                                         &desc->inodeId,
                                         MAX_SYSKEY_LEN_WD103,
                                         &desc->cbInodeId );
            }; /* if */
        }; /* if */
    }; /* if */

    if (!wd00Success(sqlRetcode)) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->recursiveHstmt );
        prepareOK = sapdbwa_False;
    }; /* if */
    
    /* prepare select count */
    if (wd00Success( sqlRetcode )) {
        sqlRetcode = SQLAllocStmt( desc->hdbc, &desc->selectCountHstmt );
        if (wd00Success(sqlRetcode)) {
            SET_DBFS_STMT(SELECT_COUNT_STMT_1, wd104GetTableInode(desc->fsDesc), SELECT_COUNT_STMT_2);
            sqlRetcode = SQLPrepare( desc->selectCountHstmt,
                                     (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
            if (wd00Success(sqlRetcode)) {
                sqlRetcode = SQLBindParameter( desc->selectCountHstmt,
                                               1,
                                               SQL_PARAM_INPUT,
                                               SQL_C_BINARY,
                                               SQL_BINARY,
                                               0,
                                               0,
                                               &desc->inodeId,
                                               MAX_SYSKEY_LEN_WD103,
                                               &desc->cbInodeId );
                if (wd00Success(sqlRetcode)) {
                    sqlRetcode = SQLBindCol( desc->selectCountHstmt,
                                             1, SQL_C_LONG,
                                             &desc->count,
                                             sizeof(desc->count),
                                             &desc->cbCount );
                }; /* if */
            }; /* if */
        }; /* if */
        if (!wd00Success(sqlRetcode)) {
            wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, SQL_NULL_HDBC,
                              desc->selectCountHstmt );
            prepareOK = sapdbwa_False;
        }; /* if */
    }; /* if */

    /* prepare delete */
    if (wd00Success( sqlRetcode )) {
        sqlRetcode = SQLAllocStmt( desc->hdbc, &desc->deleteHstmt );
        if (wd00Success(sqlRetcode)) {
            SET_DBFS_STMT(DELETE_STMT_1, wd104GetTableInode(desc->fsDesc), DELETE_STMT_2);
            sqlRetcode = SQLPrepare( desc->deleteHstmt,
                                     (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
			/* Binding for column InodeId	*/
            if (wd00Success(sqlRetcode)) {
                sqlRetcode = SQLBindParameter( desc->deleteHstmt,
                                               1,
                                               SQL_PARAM_INPUT,
                                               SQL_C_BINARY,
                                               SQL_BINARY,
                                               0,
                                               0,
                                               &desc->inodeId,
                                               MAX_SYSKEY_LEN_WD103,
                                               &desc->cbInodeId );
            }; /* if */
			/* Binding for column LinkInode	*/
			if (wd00Success(sqlRetcode)) {
				sqlRetcode = SQLBindParameter( desc->deleteHstmt,
											   2,
											   SQL_PARAM_INPUT,
											   SQL_C_BINARY,
											   SQL_BINARY,
											   0,
											   0,
											   &desc->inodeId,
											   MAX_SYSKEY_LEN_WD103,
											   &desc->cbInodeId );
			}; /* if */
        }; /* if */    
        if (!wd00Success(sqlRetcode)) {
            wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, SQL_NULL_HDBC,
                              desc->deleteHstmt );
            prepareOK = sapdbwa_False;
        }; /* if */
    }; /* if */

    return prepareOK;
} /* wd113_Prepare */
