/*!**********************************************************************

  module: vwd111getinode.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem GetInodeByName function

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
#include <string.h>
#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>

#include "hwd104fsdesc.h"
#include "hwd111getinode.h"

#include "SAPDBCommon/SAPDB_string.h"

/*!**********************************************************************

  Chapter: Prototypes and constants for internal functions

************************************************************************/

#define SELECT_INODE_STMT_PRE   "SELECT InodeId,Parentdirectory,Name,Container,LinkInode,Size,Type, ModificationDate, ModificationTime FROM"
#define SELECT_INODE_STMT_POST  "WHERE Name = ?  AND Parentdirectory = ?"

#define SELECT_INODE_STMT1_PRE  "SELECT InodeId,Parentdirectory,Name,Container,LinkInode,Size,Type, ModificationDate, ModificationTime FROM"
#define SELECT_INODE_STMT1_POST "WHERE PartitionId = ? And Name = ?  AND Parentdirectory IS NULL"

#define SELECT_INODE_BY_ID_STMT_PRE     "SELECT InodeId,Parentdirectory,Name,Container,LinkInode,Size,Type, ModificationDate, ModificationTime FROM"
#define SELECT_INODE_BY_ID_STMT_POST    "WHERE InodeId = ?"

sapdbwa_Bool wd111_PrepareStmts( twd111GetInodeDescP desc, twd26ErrP err );

sapdbwa_Bool wd111_PrepareStmt1( twd111GetInodeDescP desc, twd26ErrP err );

sapdbwa_Bool wd111_PrepareStmtById( twd111GetInodeDescP desc, twd26ErrP err );

sapdbwa_Bool wd111_BindColumns( twd111GetInodeDescP  desc,
                                SQLHSTMT             hstmt,
                                twd26ErrP            err );

/*!**********************************************************************

  Chapter: twd111GetInodeDescP

************************************************************************/


struct st_get_inode_desc
{
    twd104FSDescP fsDesc;
    SQLHDBC       hdbc;
    SQLHSTMT      selectHstmt;  /* used to retrieve inode by name */
    SQLHSTMT      selectHstmt1; /* parentDir IS NULL */
    SQLHSTMT      selectByIdHstmt;
    
    /* Binding for input parameter name */
    char            nameParam[DB_MAX_NAME_LEN_WD103+1];
    tsp81_UCS2Char  nameParamUCS2[2*(DB_MAX_NAME_LEN_WD103+1)];
    SQLLEN          nameIndi;

    /* Binding for input parameter directory */
    char          dirParam[MAX_SYSKEY_LEN_WD103+1];
    SQLLEN        dirIndi;

    /* Binding for input parameter inode typename */
    char          typeBuffer[MAX_TYPENAME_LEN_WD103+1];
    SQLLEN        typeIndi;

    twd103InodeP  inode;       /* Bindings for the output parameters */
    SQLLEN        inodeIdIndi;
    SQLLEN        parentDirIndi;
    SQLLEN        inodeNameIndi;
    SQLLEN        containerIndi;
    SQLLEN        linkInodeIndi;
    SQLLEN        inodeSizeIndi;
    SQLLEN        dateIndi;
    SQLLEN        timeIndi;
    
    sapdbwa_Bool    prepared;	    /* sapdbwa_True: selectStmt is prepared and vars are	*/
	                                /* bound; sapdbwa_False: selectStmt is not prepared		*/
		                            /* and no vars are bound								*/

    sapdbwa_UInt4	partitionId;	/* User's partition										*/
}; /* struct st_get_inode_desc */


twd111GetInodeDescP wd111CreateGetInodeDesc( twd104FSDescP    fsDesc )
{
    sapdbwa_Bool           allocatOK;
    twd111GetInodeDescP  newDesc;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_get_inode_desc),
                   (sapdbwa_UInt1**) &newDesc,
                   &allocatOK );
    if (!allocatOK) {
        newDesc = NULL;
    } else {
        newDesc->fsDesc = fsDesc;
        newDesc->selectHstmt = SQL_NULL_HSTMT;
        newDesc->selectHstmt1 = SQL_NULL_HSTMT;
        newDesc->selectByIdHstmt = SQL_NULL_HSTMT;
        newDesc->inode = wd103CreateInode();
        if (newDesc->inode == NULL) {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newDesc );
            newDesc = NULL;
        } else {
            newDesc->hdbc        = wd104GetHDBC(fsDesc);
            newDesc->prepared    = sapdbwa_False;
            newDesc->partitionId = wd104GetPartitionId(fsDesc);
        }; /* else */
    }; /* else */

    return newDesc;
} /* wd111CreateGetInodeDesc */


void wd111DestroyGetInodeDesc(  twd111GetInodeDescP desc )
{
    RETCODE retcode;

    if (desc->prepared) {
        retcode = SQLFreeStmt( desc->selectHstmt, SQL_DROP );
        retcode = SQLFreeStmt( desc->selectHstmt1, SQL_DROP );
        desc->prepared = sapdbwa_False;
    }; /* if */
    if (desc->selectByIdHstmt != SQL_NULL_HSTMT) {
        retcode = SQLFreeStmt( desc->selectByIdHstmt, SQL_DROP );
    };        
    wd103DestroyInode( desc->inode );
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) desc );
} /* wd111DestroyGetInodeDesc */


/*
 * Find a record in table Inode and return 'st_inode' which is allocated
 * by the caller. Search with:
 *      Name = 'Name' and
 *       (ParentDirectory = 'Directory' or ParentDirectory is NULL)
 *
 * Distinguish:
 *      With Directory (Directory != NULL && *Directory != NULL)
 *
 *      Without Directory (Directory == NULL || *Directory == NULL)
 * 
 * (in case "Without Directory" 'Name' is meant to refer to an inode
 * in the root directory.)
 *
 * Special treatment when looking for the following names:
 *
 *      "."     Return the inode of the 'Directory'.
 *      ".."    Return the inode of the parent directory of 'Directory'.
 *
 * A record for the root directory inode does not exist in the inode table.
 * It corresponds to the inode with syskey NULL (which doesn't exist).
 * When returning the root inode just return an 'st_inode' with nulls and
 * 'Type = inode_directory'.
 */
sapdbwa_Bool wd111DbGetInodeByName( twd111GetInodeDescP  desc,
                                  twd103InodeId       *Directory,
                                  char                *Name,
                                  twd103InodeP         Inode,
                                  twd26ErrP            err )
{
    RETCODE          sqlRetcode;
    sapdbwa_Bool     retcode = sapdbwa_True;
    sapdbwa_Bool     error   = sapdbwa_False;
    twd103Inode      tmpInode;
    SQLHSTMT         selectHstmt;
    tsp00_Byte const        *srcAt = NULL;
    tsp81_UCS2Char          *destAt = NULL;
	tsp81_UCS2Char const    *srcAtUCS2 = NULL;
    tsp00_Byte		        *destAtUCS2 = NULL;
    
    /* prepare stmt if necessary */
    if (!desc->prepared) {
        retcode = wd111_PrepareStmts( desc, err );
        if (!retcode) {
            return sapdbwa_False;
        }; /* if */
    }; /* if */
    
    if (!error) {
        /* determine input parameters for select stmt */
        if (wd103inodeIsNull(Directory)) {
            if (!strcmp(Name, ".") || !strcmp(Name, "..")) {
                memset( Inode, 0, sizeof(*Inode) );
                Inode->Type = inode_directory;
                return sapdbwa_True;
            } else {
                desc->dirIndi = SQL_NULL_DATA; /* root dir */

                strcpy( desc->nameParam, Name );
                desc->nameIndi = SQL_NTS;

                sp83UTF8ConvertToUCS2((const tsp00_Byte*) desc->nameParam, 
					                  (const tsp00_Byte*)(desc->nameParam+strlen(desc->nameParam)+1),
					                  &srcAt, 
					                  (tsp81_UCS2Char*) desc->nameParamUCS2, 
					                  (tsp81_UCS2Char*)(desc->nameParamUCS2+2*(DB_MAX_NAME_LEN_WD103+1)),
					                  &destAt, WD_IS_UCS2_SWAPED);

                selectHstmt = desc->selectHstmt1; /* parentDir IS NULL */
            };
        } else {
            if (!strcmp(Name, ".")) {
                SAPDB_memcpy( desc->nameParam, Directory, MAX_SYSKEY_LEN_WD103 );
                desc->nameIndi = MAX_SYSKEY_LEN_WD103;

                desc->dirIndi = SQL_NULL_DATA;

                selectHstmt = desc->selectHstmt1; /* parentDir IS NULL */
            } else if (!strcmp( Name, "..")) {
                if (!wd111DbGetInodeByName( desc, Directory, ".",
                                            &tmpInode, err )) {
                    error = sapdbwa_True;
                    
                    return sapdbwa_False;
                };
                if (!wd111DbGetInodeByName( desc,
                                            &tmpInode.ParentDirectoryId,
                                            ".", Inode, err )) {
                    return sapdbwa_False;
                };
                return sapdbwa_True;
            } else {
                strcpy( desc->nameParam, Name );
                desc->nameIndi = SQL_NTS;

                sp83UTF8ConvertToUCS2((const tsp00_Byte*) desc->nameParam, 
					                  (const tsp00_Byte*)(desc->nameParam+strlen(desc->nameParam)+1), 
					                  &srcAt, 
					                  (tsp81_UCS2Char*) desc->nameParamUCS2, 
					                  (tsp81_UCS2Char*)(desc->nameParamUCS2+2*(DB_MAX_NAME_LEN_WD103+1)), 
					                  &destAt, WD_IS_UCS2_SWAPED);

                SAPDB_memcpy( desc->dirParam, Directory, MAX_SYSKEY_LEN_WD103 );
                desc->dirIndi = MAX_SYSKEY_LEN_WD103;

                selectHstmt = desc->selectHstmt;
            }; /* else */
        }; /* else */
    }; /* if (!error) */
    
    /* execute stmt and fetch result */
    if (retcode && !error) {
        sqlRetcode = SQLExecute( selectHstmt );
        if (!wd00Success(sqlRetcode)) {
            retcode = sapdbwa_False;
            error = sapdbwa_True;
        } else {
            sqlRetcode = SQLFetch( selectHstmt );
            if (!wd00Success(sqlRetcode)) {
                retcode = sapdbwa_False;
                if (sqlRetcode != SQL_NO_DATA_FOUND) {
                    error = sapdbwa_True;
                }; /* if */
            } else {
                /* Convert UCS2 strings to UTF8 strings */
                sp83UTF8ConvertFromUCS2((const tsp81_UCS2Char*)desc->inode->NameUCS2,  
							(const tsp81_UCS2Char*)(desc->inode->NameUCS2+sp81UCS2strlen(desc->inode->NameUCS2)*2),
							&srcAtUCS2, WD_IS_UCS2_SWAPED,
                            (tsp00_Byte*)desc->inode->Name, (tsp00_Byte*)(desc->inode->Name+DB_MAX_NAME_LEN_WD103),
                            &destAtUCS2);

                /* proccess result */
                if ( desc->inodeIdIndi == 0
                     || desc->inodeIdIndi  == SQL_NULL_DATA)
                    wd103inodeidInit( desc->inode->InodeId );
                if ( desc->parentDirIndi == 0
                     || desc->parentDirIndi == SQL_NULL_DATA)
                    wd103inodeidInit( desc->inode->ParentDirectoryId );
                if ( desc->containerIndi == 0
                     || desc->containerIndi == SQL_NULL_DATA)
                    wd103inodeidInit( desc->inode->ContainerId );
                if ( desc->inodeSizeIndi == 0
                     || desc->inodeSizeIndi == SQL_NULL_DATA)
                    desc->inode->Size = 0;
                desc->inode->Type = wd103Text2InodeType( desc->typeBuffer );

                wd103CopyInode( desc->inode, Inode );
            }; /* else */
        }; /* else */
    }; /* if */
    if (error) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, SQL_NULL_HDBC,
                          selectHstmt );
    }; /* if */
    SQLFreeStmt( selectHstmt, SQL_CLOSE );
    
    return retcode;
} /* wd111DbGetInodeByName */


sapdbwa_Bool wd111GetInodeById( twd111GetInodeDescP  desc,
                              twd103InodeId       *inodeId,
                              twd103InodeP         inode,
                              twd26ErrP            err )
{
    RETCODE      sqlRetcode = SQL_SUCCESS;
    sapdbwa_Bool   retcode = sapdbwa_True;
    sapdbwa_Bool   error = sapdbwa_False;
	tsp81_UCS2Char  *srcAtUCS2 = NULL;
    tsp00_Byte		*destAtUCS2 = NULL;

    if (desc->selectByIdHstmt == SQL_NULL_HSTMT) {
        retcode = wd111_PrepareStmtById( desc, err );
        if (!retcode) {
            return sapdbwa_False;
        }; /* if */
    };
    SAPDB_memcpy( &desc->dirParam, inodeId, MAX_SYSKEY_LEN_WD103 );
    desc->dirIndi = MAX_SYSKEY_LEN_WD103;

    /* execute stmt and fetch result */
    sqlRetcode = SQLExecute( desc->selectByIdHstmt );
    if (!wd00Success(sqlRetcode)) {
        retcode = sapdbwa_False;
        error = sapdbwa_True;
    } else {
        sqlRetcode = SQLFetch( desc->selectByIdHstmt );
        if (!wd00Success(sqlRetcode)) {
            retcode = sapdbwa_False;
            if (sqlRetcode != SQL_NO_DATA_FOUND) {
                error = sapdbwa_True;
            }; /* if */
        } else {
            /* proccess result */
            /* Convert UCS2 strings to UTF8 strings */
            sp83UTF8ConvertFromUCS2((const tsp81_UCS2Char*)desc->inode->NameUCS2,  
						(const tsp81_UCS2Char*)desc->inode->NameUCS2+sp81UCS2strlen(desc->inode->NameUCS2)*2,
						(tsp81_UCS2Char const **)&srcAtUCS2, WD_IS_UCS2_SWAPED,
                        (tsp00_Byte*)desc->inode->Name, (tsp00_Byte*)desc->inode->Name+DB_MAX_NAME_LEN_WD103,
                        &destAtUCS2);

            if ( desc->inodeIdIndi == 0
                 || desc->inodeIdIndi  == SQL_NULL_DATA)
                wd103inodeidInit( desc->inode->InodeId );
            if ( desc->parentDirIndi == 0
                 || desc->parentDirIndi == SQL_NULL_DATA)
                wd103inodeidInit( desc->inode->ParentDirectoryId );
            if ( desc->containerIndi == 0
                 || desc->containerIndi == SQL_NULL_DATA)
                wd103inodeidInit( desc->inode->ContainerId );
            if ( desc->inodeSizeIndi == 0
                 || desc->inodeSizeIndi == SQL_NULL_DATA)
                desc->inode->Size = 0;
            desc->inode->Type = wd103Text2InodeType( desc->typeBuffer );

            wd103CopyInode( desc->inode, inode );
        }; /* else */
    }; /* else */
    if (error) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->selectByIdHstmt );
    }; /* if */
    SQLFreeStmt( desc->selectByIdHstmt, SQL_CLOSE );

    return retcode;
} /* wd111GetInodeById */


/*!**********************************************************************

  EndChapter: twd111GetInodeDescP

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

sapdbwa_Bool wd111_PrepareStmts( twd111GetInodeDescP desc, twd26ErrP err )
{
    sapdbwa_Bool retcode = sapdbwa_True;
    RETCODE    sqlRetcode;
    DBFS_STMT;
    
    /* Allocate and prepare stmt */
    if (( sqlRetcode =
	      SQLAllocStmt( desc->hdbc, &desc->selectHstmt )) != SQL_SUCCESS) {
        retcode = sapdbwa_False;
    } else {
        SET_DBFS_STMT(SELECT_INODE_STMT_PRE, wd104GetTableInode(desc->fsDesc), SELECT_INODE_STMT_POST);
        if ( (sqlRetcode =
              SQLPrepare( desc->selectHstmt, (UCHAR*) DBFS_STMT_VAR, SQL_NTS ))
             != SQL_SUCCESS) {
            retcode = sapdbwa_False;
        }; /* if */
    }; /* else */

    /* Bind input parameter */
    if (retcode) {
        if ((sqlRetcode =
	     SQLBindParameter( desc->selectHstmt,
			       1,
			       SQL_PARAM_INPUT,
			       SQL_C_WCHAR,
			       SQL_WCHAR,
			       0,
			       0,
			       desc->nameParamUCS2,
 				   2*(DB_MAX_NAME_LEN_WD103+1),
			       &desc->nameIndi )) != SQL_SUCCESS) {
            retcode = sapdbwa_False;
        } else if ((sqlRetcode =
		    SQLBindParameter( desc->selectHstmt,
				      2,
				      SQL_PARAM_INPUT,
				      SQL_C_BINARY,
				      SQL_BINARY,
				      0,
				      0,
				      desc->dirParam,
				      MAX_SYSKEY_LEN_WD103,
				      &desc->dirIndi )) != SQL_SUCCESS) {
            retcode = sapdbwa_False;
        }; /* if */
    }; /* if */
    
    /* Bind output columns */
    if (retcode) {
        retcode = wd111_BindColumns( desc, desc->selectHstmt, err );
        if (retcode) {
            retcode = wd111_PrepareStmt1( desc, err );
        }; /* if */    
    }; /* if */
    
    if (!retcode) {
        desc->prepared = sapdbwa_False;
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, desc->hdbc, desc->selectHstmt );
        SQLFreeStmt( desc->selectHstmt, SQL_DROP );
        desc->selectHstmt = SQL_NULL_HSTMT;
    } else {
        desc->prepared = sapdbwa_True;
    }; /* else */
    
    return retcode;
} /* wd111_PrepareStmts */


sapdbwa_Bool wd111_PrepareStmt1( twd111GetInodeDescP desc, twd26ErrP err )
{
    sapdbwa_Bool	retcode = sapdbwa_True;
    RETCODE			sqlRetcode;
    DBFS_STMT;
    
    /* Allocate and prepare stmt */
    if ((sqlRetcode =
	     SQLAllocStmt( desc->hdbc, &desc->selectHstmt1 )) != SQL_SUCCESS) {
        retcode = sapdbwa_False;
    } else {
        SET_DBFS_STMT(SELECT_INODE_STMT1_PRE, wd104GetTableInode(desc->fsDesc), SELECT_INODE_STMT1_POST);
        if ( SQLPrepare( desc->selectHstmt1, (UCHAR*) DBFS_STMT_VAR, SQL_NTS )
             != SQL_SUCCESS) {
            retcode = sapdbwa_False;
        }; /* if */
    }; /* else */

    /* Bind input parameter */
    if (retcode) {
        if ((sqlRetcode =
             SQLBindParameter( desc->selectHstmt1,
                               1,
                               SQL_PARAM_INPUT,
                               SQL_C_ULONG,
                               SQL_INTEGER,
                               0,
                               0,
                               &(desc->partitionId),
                               0,
                               0 )) != SQL_SUCCESS) {
	    retcode = sapdbwa_False;
        }; /* if */

        if ( retcode == sapdbwa_True &&
			(sqlRetcode =
             SQLBindParameter( desc->selectHstmt1,
                               2,
                               SQL_PARAM_INPUT,
                               SQL_C_WCHAR,
                               SQL_WCHAR,
                               0,
                               0,
                               desc->nameParamUCS2,
        				       2*(DB_MAX_NAME_LEN_WD103+1),
                               &desc->nameIndi )) != SQL_SUCCESS) {
	    retcode = sapdbwa_False;
        }; /* if */
    }; /* if */
    
    /* Bind output columns */
    if (retcode) {
        retcode = wd111_BindColumns( desc, desc->selectHstmt1, err );
    }; /* if */

    if (!retcode) {
        desc->prepared = sapdbwa_False;
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, desc->hdbc, desc->selectHstmt1 );
        SQLFreeStmt( desc->selectHstmt1, SQL_DROP);
        desc->selectHstmt1 = SQL_NULL_HSTMT;
    }; /* else */
    
    return retcode;
} /* wd111_PrepareStmt1 */


sapdbwa_Bool wd111_PrepareStmtById( twd111GetInodeDescP desc, twd26ErrP err )
{
    sapdbwa_Bool retcode = sapdbwa_True;
    RETCODE    sqlRetcode;
    DBFS_STMT;
    
    /* Allocate and prepare stmt */
    if ((sqlRetcode =
	     SQLAllocStmt( desc->hdbc, &desc->selectByIdHstmt )) != SQL_SUCCESS) {
        retcode = sapdbwa_False;
    } else {
        SET_DBFS_STMT(SELECT_INODE_BY_ID_STMT_PRE, wd104GetTableInode(desc->fsDesc), SELECT_INODE_BY_ID_STMT_POST);
        if ( SQLPrepare( desc->selectByIdHstmt, (UCHAR*) DBFS_STMT_VAR, SQL_NTS )
             != SQL_SUCCESS) {
            retcode = sapdbwa_False;
        }; /* if */
    }; /* else */

    /* Bind input parameter */
    if (retcode) {
        if ((sqlRetcode =
             SQLBindParameter( desc->selectByIdHstmt,
                               1,
                               SQL_PARAM_INPUT,
                               SQL_C_BINARY,
                               SQL_BINARY,
                               0,
                               0,
                               desc->dirParam,
                               MAX_SYSKEY_LEN_WD103,
                               &desc->dirIndi )) != SQL_SUCCESS) {
	    retcode = sapdbwa_False;
        }; /* if */
    }; /* if */
    
    /* Bind output columns */
    if (retcode) {
        retcode = wd111_BindColumns( desc, desc->selectByIdHstmt, err );
    }; /* if */

    if (!retcode) {
        desc->prepared = sapdbwa_False;
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, desc->hdbc, desc->selectHstmt1 );
        SQLFreeStmt( desc->selectByIdHstmt, SQL_DROP);
        desc->selectByIdHstmt = SQL_NULL_HSTMT;
    }; /* else */
    
    return retcode;
} /* wd111_PrepareStmtById */


sapdbwa_Bool wd111_BindColumns( twd111GetInodeDescP  desc,
                                SQLHSTMT             hstmt,
                                twd26ErrP            err )
{

    RETCODE sqlRetcode = SQL_SUCCESS;

    /* Bind output columns */
    sqlRetcode = SQLBindCol(hstmt, 1, SQL_C_BINARY, &desc->inode->InodeId,
                            MAX_SYSKEY_LEN_WD103, &desc->inodeIdIndi);
    if (sqlRetcode!= SQL_SUCCESS) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC, hstmt );

        SQLFreeStmt( hstmt, SQL_DROP);

        return sapdbwa_False;
    }

    sqlRetcode = SQLBindCol(hstmt, 2, SQL_C_BINARY, &desc->inode->ParentDirectoryId,
                            MAX_SYSKEY_LEN_WD103, &desc->parentDirIndi);
    if (sqlRetcode!= SQL_SUCCESS) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC, hstmt );

        SQLFreeStmt( hstmt, SQL_DROP);

        return sapdbwa_False;
    }

    sqlRetcode = SQLBindCol(hstmt, 3, SQL_C_WCHAR, desc->inode->NameUCS2,
				            2*(DB_MAX_NAME_LEN_WD103+1), &desc->inodeNameIndi );
    if (sqlRetcode!= SQL_SUCCESS) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC, hstmt );

        SQLFreeStmt( hstmt, SQL_DROP);

        return sapdbwa_False;
    }

    sqlRetcode = SQLBindCol(hstmt, 4, SQL_C_BINARY, &desc->inode->ContainerId,
                            MAX_SYSKEY_LEN_WD103, &desc->containerIndi );
    if (sqlRetcode!= SQL_SUCCESS) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC, hstmt );

        SQLFreeStmt( hstmt, SQL_DROP);

        return sapdbwa_False;
    }

    sqlRetcode = SQLBindCol(hstmt, 5, SQL_C_BINARY, &desc->inode->LinkInodeId,
                            MAX_SYSKEY_LEN_WD103, &desc->linkInodeIndi );
    if (sqlRetcode!= SQL_SUCCESS) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC, hstmt );

        SQLFreeStmt( hstmt, SQL_DROP);

        return sapdbwa_False;
    }

    sqlRetcode = SQLBindCol(hstmt, 6, SQL_C_ULONG, &(desc->inode->Size), 0, &desc->inodeSizeIndi );
    if (sqlRetcode!= SQL_SUCCESS) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC, hstmt );

        SQLFreeStmt( hstmt, SQL_DROP);

        return sapdbwa_False;
    }

    sqlRetcode = SQLBindCol(hstmt, 7, SQL_C_CHAR, desc->typeBuffer,
                            MAX_TYPENAME_LEN_WD103, &desc->typeIndi );
    if (sqlRetcode!= SQL_SUCCESS) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC, hstmt );

        SQLFreeStmt( hstmt, SQL_DROP);

        return sapdbwa_False;
    }

    sqlRetcode = SQLBindCol(hstmt, 8, SQL_C_CHAR, desc->inode->modificationDate,
                            ODBC_DATE_LEN_WD29, &desc->dateIndi );
    if (sqlRetcode!= SQL_SUCCESS) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC, hstmt );

        SQLFreeStmt( hstmt, SQL_DROP);

        return sapdbwa_False;
    }

    sqlRetcode = SQLBindCol(hstmt, 9, SQL_C_CHAR, desc->inode->modificationTime,
                            ODBC_TIME_LEN_WD29, &desc->timeIndi );
    if (sqlRetcode!= SQL_SUCCESS) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC, hstmt );

        SQLFreeStmt( hstmt, SQL_DROP);

        return sapdbwa_False;
    }

    return sapdbwa_True;

} /* wd111_BindColumns */

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/

