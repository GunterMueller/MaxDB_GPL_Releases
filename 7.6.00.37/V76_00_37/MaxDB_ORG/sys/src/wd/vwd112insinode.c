/*!**********************************************************************

  module: hwd112insinode.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem InsertInode function

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

#include "hwd29date.h"
#include "hwd104fsdesc.h"
#include "hwd112insinode.h"

#include "SAPDBCommon/SAPDB_string.h"

/*!**********************************************************************

  Chapter: Prototypes and constants for internal functions

************************************************************************/

#define INSERT_INODE_STMT_PRE   "INSERT INTO"
#define INSERT_INODE_STMT_POST  "( ViewOwner, PartitionId, ParentDirectory, Name, Type, LinkInode, CreationDate, CreationTime, \
                                 ModificationDate, ModificationTime ) VALUES ( USERGROUP, ?, ?, ?, ?, ?, ?, ?, ?, ?)"

sapdbwa_Bool wd112_Execute( twd112InsertInodeDescP desc, twd26ErrP err );

sapdbwa_Bool wd112_PrepareStmt( twd112InsertInodeDescP desc, twd26ErrP err );

sapdbwa_Bool wd112_PrepareInsertStmt( twd112InsertInodeDescP desc,
                                    twd26ErrP              err );

char *wd112_InodeType2Text( enum en_inode_wd103 InodeType );

sapdbwa_Bool wd112_ConvertToUCS2( twd112InsertInodeDescP desc );

/*!**********************************************************************

  Chapter: twd112InsertInodeDescP

************************************************************************/

struct st_insert_inode_desc
{
    twd104FSDescP   fsDesc;
    SQLHDBC         hdbc;
    
    SQLHSTMT        insertHstmt; /* used to insert inode */

    /* Binding for input parameter ViewOwner */
    const char     *viewOwner;
    tsp81_UCS2Char  viewOwnerUCS2[2*(64+1)];
    SQLLEN          ownerIndi;
    
    /* Binding for input parameter name */
    char            nameParam[DB_MAX_NAME_LEN_WD103+1];
    tsp81_UCS2Char  nameParamUCS2[2*(DB_MAX_NAME_LEN_WD103+1)];
    SQLLEN          nameIndi;

    /* Binding for input parameter directory */
    char            dirParam[MAX_SYSKEY_LEN_WD103+1];
    SQLLEN          dirIndi;

    /* Binding for input parameter linkInode */
    char            linkParam[MAX_SYSKEY_LEN_WD103+1];
    SQLLEN          linkIndi;
    
    /* Binding for input parameter inode typename */
    char            typeParam[MAX_TYPENAME_LEN_WD103+1];
    SQLLEN          typeIndi;

    /* Binding for creation/modification date/time */
    char            dateParam[ODBC_DATE_LEN_WD29];
    SQLLEN          dateIndi;
    char            timeParam[ODBC_TIME_LEN_WD29];
    SQLLEN          timeIndi;
    
    sapdbwa_Bool    prepared;
    sapdbwa_UInt4	partitionId;	/* User's partition */
}; /* struct st_insert_inode_desc */


twd112InsertInodeDescP wd112CreateInsertInodeDesc( twd104FSDescP   fsdesc,
                                                   SQLHDBC         hdbc,
                                                   const char     *viewOwner,
                                                   sapdbwa_UInt4   partitionId )
{
    sapdbwa_Bool              allocatOK;
    twd112InsertInodeDescP  newDesc;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_insert_inode_desc),
                   (sapdbwa_UInt1**) &newDesc,
                   &allocatOK );
    if (!allocatOK) {
        newDesc = NULL;
    } else {
        newDesc->insertHstmt = SQL_NULL_HSTMT;
        newDesc->fsDesc      = fsdesc;
        newDesc->hdbc        = hdbc;
        newDesc->prepared    = sapdbwa_False;
        newDesc->viewOwner   = viewOwner;
        newDesc->partitionId = partitionId;
    }; /* else */

    return newDesc;
} /* wd112CreateInsertInodeDesc */


void wd112DestroyInsertInodeDesc(  twd112InsertInodeDescP desc )
{
    RETCODE retcode;

    if (desc->prepared) {
        retcode = SQLFreeStmt( desc->insertHstmt, SQL_DROP );
    }; /* if */
    
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) desc );
} /* wd112DestroyInsertInodeDesc */


/* Insert a record in the Inode table and set the appropriate fields.
 * The following restrictions apply:
 *      - Just assume the name does not contain slashes.
 *      - Name != "." && Name != ".."
 *      - Directory == NULL || Directory == a valid inode.
 *
 * Return the newly created inode through the structure *NewInode which
 * is allocated by the caller. Only if insert was successful.
 */
sapdbwa_Bool wd112DbInsertInode( twd112InsertInodeDescP  desc,
                               twd103InodeId          *directory,
                               twd103InodeId          *linkInode,
                               char                   *name,
                               enum en_inode_wd103     inodeType,
                               twd103InodeP            newInode,
                               twd26ErrP               err )
{
    sapdbwa_Bool retcode = sapdbwa_True;
    
    if (!strcmp(name, ".") || !strcmp(name, ".."))
        return sapdbwa_False;

    strcpy( desc->typeParam, wd112_InodeType2Text( inodeType ));
    desc->ownerIndi = SQL_NTS;
    desc->typeIndi  = SQL_NTS;
    desc->dateIndi  = SQL_NTS;
    desc->timeIndi  = SQL_NTS;
    desc->ownerIndi = SQL_NTS;
    
    /* get actual date/time */
    wd29GetActualDate( desc->dateParam );
    wd29GetActualTime( desc->timeParam );
    
    /* set input parameters */
    if (wd103inodeIsNull(directory)) {
        desc->dirIndi = SQL_NULL_DATA;
    } else {
        SAPDB_memcpy( desc->dirParam, directory, MAX_SYSKEY_LEN_WD103 );
        desc->dirIndi = MAX_SYSKEY_LEN_WD103;
    }; /* else */
    strcpy( desc->nameParam, name );
    desc->nameIndi = SQL_NTS;
    if (inodeType == inode_link) {
        SAPDB_memcpy( desc->linkParam, linkInode, MAX_SYSKEY_LEN_WD103 );
        desc->linkIndi = MAX_SYSKEY_LEN_WD103;
    } else {
        desc->linkIndi = SQL_NULL_DATA;
    };
    /* prepare stmts if necessary */
    if (!desc->prepared) {
        retcode = wd112_PrepareInsertStmt( desc, err );
    }; /* if */

    /* execute insert statement */
    if (retcode) {
        if (!wd112_ConvertToUCS2(desc)) {
            retcode = wd112_Execute( desc, err );
        } else {
            retcode = wd112_Execute( desc, err );
        }
    }; /* if */

    if (retcode && inodeType == inode_file) {
        retcode = wd104DbGetInodeByName( desc->fsDesc,
                                         directory,
                                         name,
                                         newInode,
                                         err );
    }; /* if */

    return retcode;
} /* wd112DbInsertInode */

/*!**********************************************************************

  EndChapter: twd112InsertInodeDescP

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

sapdbwa_Bool wd112_ConvertToUCS2( twd112InsertInodeDescP desc )
{

    tsp00_Byte const    *srcAt = NULL;
    tsp81_UCS2Char      *destAt = NULL;

    if (!desc) {
        return sapdbwa_False;
    }

    sp83UTF8ConvertToUCS2((const tsp00_Byte*) desc->viewOwner, 
					      (const tsp00_Byte*)(desc->viewOwner+strlen(desc->viewOwner)+1), 
					      &srcAt, 
					      (tsp81_UCS2Char*) desc->viewOwnerUCS2, 
					      (tsp81_UCS2Char*)(desc->viewOwnerUCS2+2*(64+1)), 
					      &destAt, WD_IS_UCS2_SWAPED);

    sp83UTF8ConvertToUCS2((const tsp00_Byte*) desc->nameParam, 
					      (const tsp00_Byte*)(desc->nameParam+strlen(desc->nameParam)+1), 
					      &srcAt, 
					      (tsp81_UCS2Char*) desc->nameParamUCS2, 
					      (tsp81_UCS2Char*)(desc->nameParamUCS2+2*(DB_MAX_NAME_LEN_WD103+1)), 
					      &destAt, WD_IS_UCS2_SWAPED);

    return sapdbwa_True;

}

/*===========================================================================*/

sapdbwa_Bool wd112_Execute( twd112InsertInodeDescP desc, twd26ErrP err )
{
    RETCODE     sqlRetcode = SQL_SUCCESS;
    sapdbwa_Bool  retcode    = sapdbwa_True;
    
    sqlRetcode = SQLExecute( desc->insertHstmt );
    if (!wd00Success(sqlRetcode)) {
        retcode = sapdbwa_False;
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->insertHstmt );
        /* when we get an integrity constraint violation then
         * a file or dir with newName already exists. */
        if (wd26GetNativeOdbcErr( err ) == 250) {
            wd26SetErr( err,
                        ERR_FILE_OR_DIR_EXISTS_WD26,
                        desc->nameParam,
                        NULL );                
        }; /* if */
    }; /* if */
    SQLFreeStmt( desc->insertHstmt, SQL_CLOSE );
    
    return retcode;
} /* wd112_Execute */


sapdbwa_Bool wd112_PrepareInsertStmt( twd112InsertInodeDescP desc,
                                      twd26ErrP              err )
{
    sapdbwa_Bool	retcode    = sapdbwa_True;
    RETCODE			sqlRetcode = SQL_SUCCESS;
    DBFS_STMT;
    
    /* Allocate and prepare stmt */
    sqlRetcode = SQLAllocStmt( desc->hdbc, &desc->insertHstmt );
    if ( sqlRetcode != SQL_SUCCESS) {
        retcode = sapdbwa_False;
    } else {
        SET_DBFS_STMT(INSERT_INODE_STMT_PRE, wd104GetTableInode(desc->fsDesc), INSERT_INODE_STMT_POST);
        sqlRetcode = SQLPrepare( desc->insertHstmt, (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
        if ( !wd00Success(sqlRetcode)) {
            retcode = sapdbwa_False;
        }; /* if */
    }; /* else */

    /* Bind input parameter */
    if (retcode) {
        /*
        if ( (sqlRetcode =
	      SQLBindParameter( desc->insertHstmt,
                            1,
                            SQL_PARAM_INPUT,
                            SQL_C_WCHAR,
                            SQL_WCHAR,
                            0,
                            0,
                            (char*) desc->viewOwnerUCS2,
                            2*(64+1),
                            &desc->ownerIndi )) != SQL_SUCCESS) {
            retcode = sapdbwa_False;
        } else*/ if ( (sqlRetcode =
                     SQLBindParameter( desc->insertHstmt,
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
        } else if ( (sqlRetcode =
                     SQLBindParameter( desc->insertHstmt,
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
        } else if ((sqlRetcode =
		    SQLBindParameter( desc->insertHstmt,
				      3,
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
		    SQLBindParameter( desc->insertHstmt,
				      4,
				      SQL_PARAM_INPUT,
				      SQL_C_CHAR,
				      SQL_CHAR,
				      0,
				      0,
				      desc->typeParam,
				      DB_MAX_NAME_LEN_WD103,
				      &desc->typeIndi )) != SQL_SUCCESS) {
            retcode = sapdbwa_False;
        } else if ( (sqlRetcode =
                     SQLBindParameter( desc->insertHstmt,
                                       5,
                                       SQL_PARAM_INPUT,
                                       SQL_C_BINARY,
                                       SQL_BINARY,
                                       0,
                                       0,
                                       desc->linkParam,
                                       MAX_SYSKEY_LEN_WD103,
                                       &desc->linkIndi )) != SQL_SUCCESS) {
            retcode = sapdbwa_False;
        } else if ((sqlRetcode =                    
		    SQLBindParameter( desc->insertHstmt,
				      6,
				      SQL_PARAM_INPUT,
				      SQL_C_CHAR,
				      SQL_DATE,
				      0,
				      0,
				      desc->dateParam,
				      ODBC_DATE_LEN_WD29,
				      &desc->dateIndi )) != SQL_SUCCESS) {
            retcode = sapdbwa_False;
        } else if ((sqlRetcode =
		    SQLBindParameter( desc->insertHstmt,
				      7,
				      SQL_PARAM_INPUT,
				      SQL_C_CHAR,
				      SQL_TIME,
				      0,
				      0,
				      desc->timeParam,
				      ODBC_TIME_LEN_WD29,
				      &desc->timeIndi )) != SQL_SUCCESS) {
            retcode = sapdbwa_False;
        } else if ((sqlRetcode =
		    SQLBindParameter( desc->insertHstmt,
				      8,
				      SQL_PARAM_INPUT,
				      SQL_C_CHAR,
				      SQL_DATE,
				      0,
				      0,
				      desc->dateParam,
				      ODBC_DATE_LEN_WD29,
				      &desc->dateIndi )) != SQL_SUCCESS) {
            retcode = sapdbwa_False;
        } else if ((sqlRetcode =
		    SQLBindParameter( desc->insertHstmt,
				      9,
				      SQL_PARAM_INPUT,
				      SQL_C_CHAR,
				      SQL_TIME,
				      0,
				      0,
				      desc->timeParam,
				      ODBC_TIME_LEN_WD29,
				      &desc->timeIndi )) != SQL_SUCCESS) {
            retcode = sapdbwa_False;            
        }; /* else */            
    }; /* if */
    
    if (!retcode) {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, desc->hdbc, desc->insertHstmt );
        SQLFreeStmt( desc->insertHstmt, SQL_DROP );
    } else {
        desc->prepared = sapdbwa_True;
    };        
    
    return retcode;
} /* wd112_PrepareInsertStmt */


char *wd112_InodeType2Text( enum en_inode_wd103 InodeType )
{
    switch (InodeType) {
    case inode_directory:
        return ("dir");
    case inode_file:
        return ("file");
    case inode_link:
        return ("link");
    default:
        return ("");
    }; /* switch */
} /* InodeType2Text */

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/
