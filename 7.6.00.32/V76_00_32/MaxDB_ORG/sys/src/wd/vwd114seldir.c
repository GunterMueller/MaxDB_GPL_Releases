/*!**********************************************************************

  module: vwd114seldir.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem directory listings

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

#include <stdio.h>

#include "hwd29date.h"
#include "hwd104fsdesc.h"
#include "hwd114seldir.h"

/*!**********************************************************************

  Chapter: prototypes for internal functions

************************************************************************/

#define SELECT_STMT_1   "SELECT InodeId, Parentdirectory, Name, Type, Size, ModificationDate, ModificationTime FROM"
#define SELECT_STMT_2   "WHERE PartitionId = ? And Parentdirectory"

sapdbwa_Bool wd114_DbSelectInodeByDirectoryOrder
( twd114SelectDirDescP  desc,
  twd103InodeId        *inodeId,
  twd101OrderFlag       OrderFlag,
  twd26ErrP             err );

/*!**********************************************************************

  EndChapter: prototypes for internal functions

************************************************************************/

/*!**********************************************************************

  Chapter: twd114SelectDirDescP

************************************************************************/

#define DB_MAX_NAME_LEN 245

#define BEGEXCL_ODBC
#define ENDEXCL_ODBC

struct st_select_dir_desc
{
    twd104FSDescP    fsDesc;
    SQLHDBC          hdbc;
    SQLHSTMT         selectHstmt;
    sapdbwa_UInt4	 partitionId;	/* User's partition */
}; /* struct st_select_dir_desc */


twd114SelectDirDescP wd114CreateSelectDirDesc( twd104FSDescP fsDesc )
{
    sapdbwa_Bool           allocatOK;
    twd114SelectDirDescP newDesc;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_select_dir_desc),
                (sapdbwa_UInt1**) &newDesc,
                &allocatOK );
    if (!allocatOK) {
        newDesc = NULL;
    } else {
        newDesc->fsDesc      = fsDesc;
        newDesc->hdbc        = wd104GetHDBC(fsDesc);
        newDesc->selectHstmt = SQL_NULL_HSTMT;
        newDesc->partitionId = wd104GetPartitionId(fsDesc);        
    }; /* else */

    return newDesc;
} /* wd114CreateSelectDirDesc */


void wd114DestroySelectDirDesc(  twd114SelectDirDescP desc )
{
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) desc );
} /* wd114DestroySelectDirDesc */


sapdbwa_Bool wd114DbDeselectInodeByDirectory( twd114SelectDirDescP desc,
                                            twd26ErrP            err )
{
  sapdbwa_Bool   closeSuccess = sapdbwa_True;
  SQLRETURN    sqlRetcode = SQL_SUCCESS;

  if (desc->selectHstmt != SQL_NULL_HSTMT) {
      sqlRetcode = SQLFreeStmt( desc->selectHstmt, SQL_DROP );
      if (!wd00Success( sqlRetcode )) {
          closeSuccess = sapdbwa_False;
          wd26SetOdbcError( err,
                            sqlRetcode,
                            SQL_NULL_HENV,
                            SQL_NULL_HDBC,
                            desc->selectHstmt );
      }; /* if */
  }; /* if */
  
  return closeSuccess;
} /* wd114DbDeselectInodeByDirectory */


/*
 * Retrieve the inode records from the 'Result' which was created by
 * 'DbSelectInodeByDirectory' (make sure).
 * Return the inode through '*Inode' which is allocated by the caller.
 * If no more data was found, return False.
 */
sapdbwa_Bool wd114DbFetchInodeByDirectory( twd114SelectDirDescP desc,
                                           twd103InodeP         inode,
                                           twd26ErrP            err )
{
    RETCODE   sqlRetcode;
    char      TypeBuffer[MAX_TYPENAME_LEN_WD103 + 1];
    SQLLEN    cbDummy1;
    SQLLEN    cbDummy2;
    SQLLEN    cbDummy3;
    SQLLEN    cbDummy4;
    SQLLEN    cbDummy5;
    SQLLEN    dateLen;
    SQLLEN    timeLen;
    SQLLEN    localInodeSize = 0;
    tsp00_Byte const        *srcAt = NULL;
    tsp81_UCS2Char          *destAt = NULL;
	tsp81_UCS2Char const    *srcAtUCS2 = NULL;
    tsp00_Byte		        *destAtUCS2 = NULL;
    
    sqlRetcode = SQLBindCol( desc->selectHstmt,
                             1,
                             SQL_C_BINARY,
                             &inode->InodeId,
                             MAX_SYSKEY_LEN_WD103,
                             &cbDummy1);
    if (wd00Success( sqlRetcode )) {
        sqlRetcode = SQLBindCol( desc->selectHstmt,
                                 2,
                                 SQL_C_BINARY,
                                 &inode->ParentDirectoryId,
                                 MAX_SYSKEY_LEN_WD103,
                                 &cbDummy2 );
        if (wd00Success( sqlRetcode )) {
            sqlRetcode = SQLBindCol( desc->selectHstmt,
                                     3,
                                     SQL_C_WCHAR,
                                     inode->NameUCS2,
                                     2*(DB_MAX_NAME_LEN_WD103+1),
                                     &cbDummy3);
            if (wd00Success( sqlRetcode )) {
                sqlRetcode = SQLBindCol( desc->selectHstmt,
                                         4,
                                         SQL_C_CHAR,
                                         TypeBuffer,
                                         MAX_TYPENAME_LEN_WD103,
                                         &cbDummy4);
                if (wd00Success( sqlRetcode )) {
                    sqlRetcode = SQLBindCol( desc->selectHstmt,
                                             5,
                                             SQL_C_SLONG,
                                             &localInodeSize,
                                             0,
                                             &cbDummy5);
                    if (wd00Success( sqlRetcode )) {
                        sqlRetcode = SQLBindCol( desc->selectHstmt,
                                                 6,
                                                 SQL_C_CHAR,
                                                 inode->modificationDate,
                                                 ODBC_DATE_LEN_WD29,
                                                 &dateLen);
                        if (wd00Success( sqlRetcode )) {
                            sqlRetcode = SQLBindCol( desc->selectHstmt,
                                                     7,
                                                     SQL_C_CHAR,
                                                     inode->modificationTime,
                                                     ODBC_TIME_LEN_WD29,
                                                     &timeLen);
                        }; /* if */
                    }; /* if */
                }; /* if */
            }; /* if */
        }; /* if */
    }; /* if */
    
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err,
                          sqlRetcode,
                          SQL_NULL_HENV,
                          SQL_NULL_HDBC,
                          desc->selectHstmt);
        return sapdbwa_False;
    }; /* if */

    sp83UTF8ConvertToUCS2((const tsp00_Byte*) inode->Name, 
			              (const tsp00_Byte*)(inode->Name+strlen(inode->Name)+1), 
		                 &srcAt, 
			              (tsp81_UCS2Char*) inode->NameUCS2, 
			              (tsp81_UCS2Char*)(inode->NameUCS2+2*(DB_MAX_NAME_LEN_WD103+1)), 
			             &destAt, WD_IS_UCS2_SWAPED);
    
    sqlRetcode = SQLFetch( desc->selectHstmt );
    if (sqlRetcode == SQL_NO_DATA_FOUND) {
        /* no more inodes */
        return sapdbwa_False;
    }; /* if */
    if (!wd00Success(sqlRetcode)) {
        wd26SetOdbcError( err,
                          sqlRetcode,
                          SQL_NULL_HENV,
                          SQL_NULL_HDBC,
                          desc->selectHstmt );
        return sapdbwa_False;
    }; /* if */

    /* Convert UCS2 strings to UTF8 strings */
    sp83UTF8ConvertFromUCS2((const tsp81_UCS2Char*) inode->NameUCS2,  
				            (const tsp81_UCS2Char*)(inode->NameUCS2+sp81UCS2strlen(inode->NameUCS2)*2),
				            &srcAtUCS2, WD_IS_UCS2_SWAPED,
                            (tsp00_Byte*)inode->Name, (tsp00_Byte*)(inode->Name+DB_MAX_NAME_LEN_WD103),
                            &destAtUCS2);

    /* localInodeSize is 8 byte integer on 64 bit platforms */
    inode->Size = (sapdbwa_Int4) localInodeSize;
    if (cbDummy1 == 0 || cbDummy1 == SQL_NULL_DATA)
        wd103inodeidInit( inode->InodeId );
    if (cbDummy2 == 0 || cbDummy2 == SQL_NULL_DATA)
        wd103inodeidInit( inode->ParentDirectoryId );
    if (cbDummy5 == 0 || cbDummy5 == SQL_NULL_DATA)
        inode->Size = 0;

    inode->Type = wd103Text2InodeType( TypeBuffer );
    inode->modificationTime[5] = '\0'; /* cut off the seconds */
    
    return sapdbwa_True;
} /* wd114DbFetchInodeByDirectory */


sapdbwa_Bool wd114DbSelectInodeByDirectory( twd114SelectDirDescP  desc,
                                          twd103InodeId        *inodeId,
                                          twd101OrderFlag       orderFlag,
                                          twd26ErrP             err )
{
    return wd114_DbSelectInodeByDirectoryOrder( desc,
                                                inodeId,
                                                orderFlag,
                                                err );
} /* wd114DbSelectInodeByDirectory */

/*!**********************************************************************

  EndChapter: twd114SelectDirDescP

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

/*
 * Select all inodes which have parent directory 'DirectoryId'.
 * "No data found" still yields True, detect this later when fetching.
 * Setup 'Result' for subsequent fetches.
 * output is ordered by the given OrderFlag; This has no effect on 
 * the actual output format but defines only the output sequence 
 */
sapdbwa_Bool wd114_DbSelectInodeByDirectoryOrder ( twd114SelectDirDescP  desc,
												   twd103InodeId        *inodeId,
												   twd101OrderFlag       OrderFlag,
												   twd26ErrP             err		)
{
    RETCODE	sqlRetcode;
    SQLLEN  cbDummy = MAX_SYSKEY_LEN_WD103;
    char	RootOrLevel[10];
    DBFS_STMT;

    sqlRetcode = SQLAllocStmt( desc->hdbc, &desc->selectHstmt );
    if (!wd00Success(sqlRetcode)) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, desc->hdbc, SQL_NULL_HSTMT );
        return sapdbwa_False;
    }; /* if */

    if (wd103inodeIsNull( inodeId ))
        strcpy( RootOrLevel, "IS NULL " );
    else
        strcpy( RootOrLevel, "= ? " );

    SET_DBFS_STMT_EX2(SELECT_STMT_1, wd104GetTableInode(desc->fsDesc), SELECT_STMT_2, RootOrLevel, "");
    switch (OrderFlag)
        {
        case name_asc:
            strcat( DBFS_STMT_VAR, "ORDER BY Name ASC");
            break;
        case name_desc:
            strcat( DBFS_STMT_VAR, "ORDER BY Name DESC");
            break;
        case size_asc:
            strcat( DBFS_STMT_VAR, "ORDER BY Size ASC");
            break;
        case size_desc:
            strcat( DBFS_STMT_VAR, "ORDER BY Size DESC");
            break;
        case type_asc:
        case type_asc_name_asc:
            strcat( DBFS_STMT_VAR, "ORDER BY Type ASC, Name ASC");
            break;
        case type_desc:
        case type_desc_name_desc:
            strcat( DBFS_STMT_VAR, "ORDER BY Type DESC, Name DESC");
            break;
        case type_asc_name_desc:
            strcat( DBFS_STMT_VAR, "ORDER BY Type ASC, Name DESC");
            break;
        case type_desc_name_asc:
            strcat( DBFS_STMT_VAR, "ORDER BY Type DESC, Name ASC");
            break;
        case not_def:
        default:
            break;
        }

    sqlRetcode = SQLPrepare( desc->selectHstmt,
                             (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
    if (!wd00Success(sqlRetcode))
        {
            wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, desc->hdbc,
                              desc->selectHstmt );
            BEGEXCL_ODBC;
            SQLFreeStmt( desc->selectHstmt, SQL_DROP);
            ENDEXCL_ODBC;
            return sapdbwa_False;
        }

	/* Bind input parameter for partition id */
    sqlRetcode = SQLBindParameter( desc->selectHstmt,
                                   1,
                                   SQL_PARAM_INPUT,
                                   SQL_C_ULONG,
                                   SQL_INTEGER,
                                   0,
                                   0,
                                   &(desc->partitionId),
                                   0,
                                   0 );
    if (!wd00Success( sqlRetcode ))
        {
            wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, desc->hdbc,
                              desc->selectHstmt );
            BEGEXCL_ODBC;
            SQLFreeStmt( desc->selectHstmt, SQL_DROP);
            ENDEXCL_ODBC;
            return sapdbwa_False;
        }

    if (!wd103inodeIsNull(inodeId)) {
        sqlRetcode = SQLBindParameter( desc->selectHstmt,
                                       2,
                                       SQL_PARAM_INPUT,
                                       SQL_C_BINARY,
                                       SQL_BINARY,
                                       0,
                                       0,
                                       inodeId,
                                       MAX_SYSKEY_LEN_WD103,
                                       &cbDummy );
        if (!wd00Success( sqlRetcode ))
            {
                wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, desc->hdbc,
                                  desc->selectHstmt );
                BEGEXCL_ODBC;
                SQLFreeStmt( desc->selectHstmt, SQL_DROP);
                ENDEXCL_ODBC;
                return sapdbwa_False;
            }
    }; /* if */
    
    sqlRetcode = SQLExecute( desc->selectHstmt );
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, desc->hdbc,
                          desc->selectHstmt );
        BEGEXCL_ODBC;
        SQLFreeStmt( desc->selectHstmt, SQL_DROP);
        ENDEXCL_ODBC;
        return sapdbwa_False;
    }; /* if */
    
    return sapdbwa_True;
} /* wd114DbSelectInodeByDirectory */

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/
