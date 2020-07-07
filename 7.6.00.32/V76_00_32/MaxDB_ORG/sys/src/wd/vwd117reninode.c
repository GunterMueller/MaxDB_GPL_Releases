/*!**********************************************************************

  module: vwd117reninode.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem RenameInode function

  see also:

  -----------------------------------------------------------------------

  copyright:              Copyright (c) 1999-2005 SAP AG




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
#include "hwd117reninode.h"

#define UPDATE_CMD_1    "UPDATE"
#define UPDATE_CMD_2    "SET Name=? WHERE InodeId=?"

/*!**********************************************************************

  Chapter: twd117RenameInodeDescP

************************************************************************/

struct st_rename_inode_desc
{
    twd104FSDescP   fsDesc;
    SQLHDBC			hdbc;
    sapdbwa_UInt4	partitionId;	/* User's partition */
}; /* struct st_rename_inode_desc */


twd117RenameInodeDescP wd117CreateRenameInodeDesc( twd104FSDescP fsDesc )
{
    sapdbwa_Bool              allocatOK;
    twd117RenameInodeDescP  newDesc;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_rename_inode_desc),
                   (sapdbwa_UInt1**) &newDesc,
                   &allocatOK );
    if (!allocatOK) {
        newDesc = NULL;
    } else {
        /* init newDesc */
        newDesc->fsDesc      = fsDesc;
        newDesc->hdbc        = wd104GetHDBC(fsDesc);
        newDesc->partitionId = wd104GetPartitionId(fsDesc);
    }; /* else */

    return newDesc;
} /* wd117CreateRenameInodeDesc */


void wd117DestroyRenameInodeDesc(  twd117RenameInodeDescP desc )
{
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) desc );
} /* wd117DestroyRenameInodeDesc */


sapdbwa_Bool wd117RenameInode( twd117RenameInodeDescP  desc,
                               twd103InodeP            inode,
                               const char             *newName,
                               twd26ErrP               err )
{
    sapdbwa_Bool renameOk = sapdbwa_True;

    SQLHSTMT            hstmt;
    RETCODE             sqlRetcode;
    SQLLEN              nodeLen = MAX_SYSKEY_LEN_WD103;
    SQLLEN              nameLen = SQL_NTS;
    SQLLEN              rowCount = 0;
    tsp81_UCS2Char      newNameUCS2[2*(DB_MAX_NAME_LEN_WD103+1)];
    tsp00_Byte const    *srcAt = NULL;
    tsp81_UCS2Char      *destAt = NULL;
    DBFS_STMT;

    SET_DBFS_STMT(UPDATE_CMD_1, wd104GetTableInode(desc->fsDesc), UPDATE_CMD_2);
    if ((sqlRetcode = SQLAllocStmt( desc->hdbc, &hstmt )) != SQL_SUCCESS) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, desc->hdbc, SQL_NULL_HSTMT );
        renameOk = sapdbwa_False;
    } else {
        sp83UTF8ConvertToUCS2((const tsp00_Byte*) newName, 
				              (const tsp00_Byte*)(newName+strlen(newName)+1),
				             &srcAt, 
				              (tsp81_UCS2Char*) newNameUCS2,
				              (tsp81_UCS2Char*)(newNameUCS2+2*(DB_MAX_NAME_LEN_WD103+1)),
				             &destAt, WD_IS_UCS2_SWAPED);

        if (!wd00Success(SQLPrepare( hstmt,
                                        (UCHAR*) DBFS_STMT_VAR,
                                        SQL_NTS ))
               || !wd00Success(SQLBindParameter( hstmt,
                                                 1,
                                                 SQL_PARAM_INPUT,
                                                 SQL_C_WCHAR,
                                                 SQL_WCHAR,
                                                 0,
                                                 0,
                                                 newNameUCS2,
                                                 0,
                                                 &nameLen ))
               || !wd00Success(SQLBindParameter( hstmt,
                                                 2,
                                                 SQL_PARAM_INPUT,
                                                 SQL_C_BINARY,
                                                 SQL_BINARY,
                                                 0,
                                                 0,
                                                 &inode->InodeId,
                                                 MAX_SYSKEY_LEN_WD103,
                                                 &nodeLen ))) {
            wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, desc->hdbc, hstmt );
            renameOk = sapdbwa_False;
        }
    };

    if (renameOk) {
        sqlRetcode = SQLExecute( hstmt );
        if (!wd00Success( sqlRetcode )
            || !wd00Success(SQLRowCount( hstmt, &rowCount ))) {
            wd26SetOdbcError( err, sqlRetcode,
                              SQL_NULL_HENV, desc->hdbc, hstmt );
            renameOk = sapdbwa_False;
        } else if (rowCount == 0) {
            wd26SetErr( err, ERR_INTERNAL_WD26,
                        "wd117RenameInode: rowCount == 0", NULL );
        }; /* else */
    }; /* if */
    
    SQLFreeStmt( hstmt, SQL_DROP);
    
    return renameOk;
} /* wd117RenameInode */

/*!**********************************************************************

  EndChapter: twd111GetInodeDescP

************************************************************************/
