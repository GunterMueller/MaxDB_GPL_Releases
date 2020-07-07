/*!**********************************************************************

  module: vwd118moveinode.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem MoveInode function

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

#include "hwd104fsdesc.h"
#include "hwd118moveinode.h"

sapdbwa_Bool wd118_Prepare( twd118MoveInodeDescP desc, twd26ErrP err );

/*!**********************************************************************

  Chapter: twd118MoveInodeDescP

************************************************************************/

#define UPDATE_INODE_STMT_1 "UPDATE"
#define UPDATE_INODE_STMT_2 "(ParentDirectory,Name) VALUES (?,?) WHERE InodeId=?"

struct st_move_inode_desc
{
    twd104FSDescP   fsDesc;
    SQLHDBC			hdbc;
    sapdbwa_UInt4	partitionId;	/* User's partition */
    SQLHSTMT		updateHstmt;
    sapdbwa_Bool	prepared;
}; /* struct st_move_inode_desc */


twd118MoveInodeDescP wd118CreateMoveInodeDesc( twd104FSDescP    fsDesc )
{
    sapdbwa_Bool            allocatOK;
    twd118MoveInodeDescP  newDesc;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_move_inode_desc),
                   (sapdbwa_UInt1**) &newDesc,
                   &allocatOK );
    if (!allocatOK) {
        newDesc = NULL;
    } else {
        /* init newDesc */
        newDesc->fsDesc      = fsDesc;
        newDesc->hdbc        = wd104GetHDBC(fsDesc);
        newDesc->partitionId = wd104GetPartitionId(fsDesc);
        newDesc->prepared    = sapdbwa_False;
    }; /* else */

    return newDesc;
} /* wd118CreateMoveInodeDesc */


void wd118DestroyMoveInodeDesc(  twd118MoveInodeDescP desc )
{
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) desc );
} /* wd118DestroyMoveInodeDesc */


sapdbwa_Bool wd118MoveInode( twd118MoveInodeDescP  desc,
                             twd103InodeId        *inodeId,
                             twd103InodeId        *newParentInodeId,
                             const char           *newName,
                             twd26ErrP             err )
{
    SQLRETURN           sqlRetcode = SQL_SUCCESS;
    SQLLEN              nameLen = SQL_NTS;
    SQLLEN              inodeLen = MAX_SYSKEY_LEN_WD103;
    SQLLEN              parentInodeLen = MAX_SYSKEY_LEN_WD103;
    sapdbwa_Bool        moveOk = sapdbwa_True;
    tsp81_UCS2Char      newNameUCS2[2*(DB_MAX_NAME_LEN_WD103+1)];
    tsp00_Byte const    *srcAt = NULL;
    tsp81_UCS2Char      *destAt = NULL;
    
    if (newParentInodeId == NULL || wd103inodeIsNull(newParentInodeId)) {
        parentInodeLen = SQL_NULL_DATA;
    }; /* if */
    
    if (!desc->prepared) {
        if (wd118_Prepare( desc, err )) {
            desc->prepared = sapdbwa_True;
        } else {
            moveOk = sapdbwa_False;
        }; /* else */
    }; /* if */

    if (moveOk) {
        if (wd00Success(sqlRetcode = SQLBindParameter( desc->updateHstmt,
                                                       1,
                                                       SQL_PARAM_INPUT,
                                                       SQL_C_BINARY,
                                                       SQL_BINARY,
                                                       0,
                                                       0,
                                                       newParentInodeId,
                                                       0,
                                                       &parentInodeLen ))
            && wd00Success(sqlRetcode = SQLBindParameter( desc->updateHstmt,
                                                          2,
                                                          SQL_PARAM_INPUT,
                                                          SQL_C_WCHAR,
                                                          SQL_WCHAR,
                                                          0,
                                                          0,
                                                          newNameUCS2,
                                                          0,
                                                          &nameLen ))
            && wd00Success(sqlRetcode = SQLBindParameter( desc->updateHstmt,
                                                          3,
                                                          SQL_PARAM_INPUT,
                                                          SQL_C_BINARY,
                                                          SQL_BINARY,
                                                          0,
                                                          0,
                                                          (char*) inodeId,
                                                          0,
                                                          &inodeLen ))) {

            sp83UTF8ConvertToUCS2((const tsp00_Byte*) newName, 
			                      (const tsp00_Byte*)(newName+strlen(newName)+1),
		                         &srcAt, 
			                      (tsp81_UCS2Char*) newNameUCS2,
			                      (tsp81_UCS2Char*)(newNameUCS2+2*(DB_MAX_NAME_LEN_WD103+1)),
			                     &destAt, WD_IS_UCS2_SWAPED);

            sqlRetcode = SQLExecute( desc->updateHstmt );
        }; /* if */

        if (!wd00Success( sqlRetcode)) {
            wd26SetOdbcError( err, sqlRetcode,
                              SQL_NULL_HENV, desc->hdbc, desc->updateHstmt );
            /* when we get an integrity constraint violation then
             * a file or dir with newName already exists. */
            if (wd26GetNativeOdbcErr( err ) == 250) {
                wd26SetErr( err,
                            ERR_FILE_OR_DIR_EXISTS_WD26,
                            newName,
                            NULL );                
            }; /* if */
            moveOk = sapdbwa_False;
        }; /* if */
    }; /* if */            
        
    return moveOk;
} /* wd118MoveInode */
 
/*!**********************************************************************

  EndChapter: twd117RenameInodeDescP

************************************************************************/

sapdbwa_Bool wd118_Prepare( twd118MoveInodeDescP desc, twd26ErrP err )
{
    SQLRETURN   sqlRetcode = SQL_SUCCESS;
    sapdbwa_Bool  prepareOk = sapdbwa_True;
    DBFS_STMT;

    sqlRetcode = SQLAllocStmt( desc->hdbc, &desc->updateHstmt );
    if (wd00Success(sqlRetcode)) {
        SET_DBFS_STMT(UPDATE_INODE_STMT_1, wd104GetTableInode(desc->fsDesc), UPDATE_INODE_STMT_2);
        sqlRetcode = SQLPrepare( desc->updateHstmt,
                                 (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
        if (!wd00Success( sqlRetcode )) {
            wd26SetOdbcError( err, sqlRetcode,
                              SQL_NULL_HENV, desc->hdbc, desc->updateHstmt );
            prepareOk = sapdbwa_False;
            SQLFreeStmt( desc->updateHstmt, SQL_DROP );
        }; /* if */
    } else {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, desc->hdbc, SQL_NULL_HSTMT );
        prepareOk = sapdbwa_False;
    }; /* else */

    return prepareOk;
} /* wd118_Prepare */
