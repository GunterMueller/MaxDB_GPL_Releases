/*!**********************************************************************

  module: vwd115container.c

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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <limits.h> /* LONG_MAX */
#include <string.h> /* memset */

#include "hpa102odcompr.h"
#include "hwd104fsdesc.h"
#include "hwd115container.h"

#include "SAPDBCommon/SAPDB_string.h"

#define COMPRESSED   '1'
#define UNCOMPRESSED '0'
#define MAX_COMPR_BLOCK_SIZE (1024*64)

#define INSERT_CONTAINER_STMT_PRE   "INSERT INTO"
#define INSERT_CONTAINER_STMT_POST  "(ViewOwner, Inode, Size, Compressed) VALUES (USERGROUP, ?, 0, ?)"

#define SELECT_SYSKEY_STMT_PRE      "SELECT ContainerId FROM"
#define SELECT_SYSKEY_STMT_POST     "WHERE Inode = ?"

#define UPDATE_INODE_STMT_PRE       "UPDATE"
#define UPDATE_INODE_STMT_POST      "(Container) VALUES (?) WHERE InodeId = ?"

#define UPDATE_CONT_STMT_PRE        "UPDATE"
#define UPDATE_CONT_STMT_POST       "(Data) VALUES (?) WHERE ContainerId = ?"

#define SELECT_DATA_STMT_PRE        "SELECT Data, Compressed FROM"
#define SELECT_DATA_STMT_POST       "WHERE ContainerId = ?"

#define UPDATE_INODESIZE_STMT_PRE   "UPDATE"
#define UPDATE_INODESIZE_STMT_POST  "(Size) VALUES (?) WHERE InodeId = ?"

#define UPDATE_CONTSIZE_STMT_PRE    "UPDATE"
#define UPDATE_CONTSIZE_STMT_POST   "(Size) VALUES (?) WHERE ContainerId = ?"

#define DELETE_INODE_STMT_PRE       "DELETE"
#define DELETE_INODE_STMT_POST      "WHERE Inode = ?"

sapdbwa_Bool wd115_AllocStmts( twd115ContainerDescP desc,
                             SQLHDBC              hdbc,
                             twd26ErrP            err );
    
sapdbwa_Bool wd115_DropStmts( twd115ContainerDescP desc );

sapdbwa_Bool wd115_PrepareInsertStmt( twd115ContainerDescP desc,
                                      twd26ErrP            err );

sapdbwa_Bool wd115_PrepareDeleteInodeStmt( twd115ContainerDescP desc,
                                           twd26ErrP            err );

sapdbwa_Bool wd115_PrepareSelectSyskeyStmt( twd115ContainerDescP desc,
                                          twd26ErrP            err );

sapdbwa_Bool wd115_PrepareStmts( twd115ContainerDescP desc,
                               twd26ErrP            err );

sapdbwa_Bool wd115_PrepareUpdateInodeStmt( twd115ContainerDescP desc,
                                         twd26ErrP            err );

sapdbwa_Bool wd115_PrepareUpdateContStmt( twd115ContainerDescP desc,
                                        twd26ErrP            err );

sapdbwa_Bool wd115_PrepareSelectDataStmt( twd115ContainerDescP desc,
                                        twd26ErrP            err );

sapdbwa_Bool wd115_PrepareUpdateInodeSizeStmt( twd115ContainerDescP desc,
                                             twd26ErrP            err );

sapdbwa_Bool wd115_PrepareUpdateContSizeStmt( twd115ContainerDescP desc,
                                            twd26ErrP            err );

/*!**********************************************************************

  Chapter: twd115ContainerDescP

************************************************************************/

struct st_container_desc
{
    twd104FSDescP       fsDesc;
    SQLHDBC             hdbc;
    const char         *viewOwner;
    tsp81_UCS2Char      viewOwnerUCS2[2*(DB_MAX_USERNAME_LEN_WD00+1)];
    sapdbwa_UInt4		partitionId;	/* User's partition */
    
    char                compressFlag;
    twd103InodeId       inodeId;
    twd103InodeId       containerId;
    SQLLEN              sizeVal;
    SQLLEN              cbNode;
    SQLLEN              cbCompr;
    SQLLEN              cbContainer;
    SQLLEN              cbBinBuf;
    SQLLEN              cbSize;
    SQLLEN              cbViewOwner;
    
    SQLHSTMT            insertHstmt;
    SQLHSTMT            selectSyskeyHstmt;
    SQLHSTMT            updateInodeHstmt;
    SQLHSTMT            updateContHstmt;
    SQLHSTMT            selectDataHstmt;
    SQLHSTMT            updateInodeSizeHstmt;
    SQLHSTMT            updateContSizeHstmt;
	SQLHSTMT			deleteHstmt;			/* Prepared delete statement			*/
    
    sapdbwa_Bool          prepared;
    sapdbwa_Bool          compress;				/* compress data when writing			*/
    sapdbwa_Bool          compressed;			/* is data in container compressed		*/
    sapdbwa_Bool          uncompress;			/* uncompress when reading				*/
    sapdbwa_Bool          comprInitialized;
    sapdbwa_Bool          decomprInitialized;
    sapdbwa_Bool          writeContainer;		/* is the container opened for writing?	*/
    
    tpa102_ODCompr      odcompr;
    tpa102_ODDecompr    oddecompr;
}; /* struct st_container_desc */


twd115ContainerDescP wd115CreateContainerDesc( twd104FSDescP   fsdesc,
                                               SQLHDBC         hdbc,
                                               const char     *viewOwner,
                                               sapdbwa_UInt4   partitionId,
                                               twd26ErrP       err )
{
    sapdbwa_Bool             allocatOK;
    twd115ContainerDescP     newDesc;
    tsp00_Byte const        *srcAt = NULL;
    tsp81_UCS2Char          *destAt = NULL;

    sapdbwa_SQLALLOCAT( sizeof( struct st_container_desc),
                   (sapdbwa_UInt1**) &newDesc,
                   &allocatOK );
    if (!allocatOK) {
        newDesc = NULL;
    } else {
        newDesc->insertHstmt           = SQL_NULL_HSTMT;
        newDesc->deleteHstmt		   = SQL_NULL_HSTMT;
        newDesc->selectSyskeyHstmt     = SQL_NULL_HSTMT;
        newDesc->updateInodeHstmt      = SQL_NULL_HSTMT;
        newDesc->updateContHstmt       = SQL_NULL_HSTMT;
        newDesc->selectDataHstmt       = SQL_NULL_HSTMT;
        newDesc->updateInodeSizeHstmt  = SQL_NULL_HSTMT;
        newDesc->updateContSizeHstmt   = SQL_NULL_HSTMT;
        newDesc->fsDesc                = fsdesc;
        newDesc->hdbc                  = hdbc;
        newDesc->viewOwner             = viewOwner;
        newDesc->partitionId           = partitionId;
        newDesc->writeContainer        = sapdbwa_False;
        newDesc->odcompr               = NULL;
        newDesc->oddecompr             = NULL;
        newDesc->compress              = sapdbwa_False;
        newDesc->compressed            = sapdbwa_False;
        newDesc->uncompress            = sapdbwa_True;
        newDesc->comprInitialized      = sapdbwa_False;
        newDesc->decomprInitialized    = sapdbwa_False;
        newDesc->sizeVal               = 0;

        sp83UTF8ConvertToUCS2((const tsp00_Byte*) newDesc->viewOwner, 
					          (const tsp00_Byte*)(newDesc->viewOwner+strlen(newDesc->viewOwner)+1), 
					          &srcAt, 
					          (tsp81_UCS2Char*) newDesc->viewOwnerUCS2,
					          (tsp81_UCS2Char*)(newDesc->viewOwnerUCS2+2*(64+1)),
					          &destAt, WD_IS_UCS2_SWAPED);

        if (pa102AllocCompr( &newDesc->odcompr, MAX_COMPR_BLOCK_SIZE )
            && pa102AllocDecompr( &newDesc->oddecompr )
            && wd115_AllocStmts( newDesc, hdbc, err )
            && wd115_PrepareStmts( newDesc, err )) {
            newDesc->prepared   = sapdbwa_True;
        } else {
            wd115DestroyContainerDesc( newDesc );
            newDesc = NULL;
        }; /* else */
    }; /* else */

    return newDesc;
} /* wd115CreateContainerInodeDesc */


void wd115DestroyContainerDesc( twd115ContainerDescP desc )
{
    if (desc->odcompr != NULL) {
        pa102FreeCompr( desc->odcompr );
        desc->odcompr = NULL;
    }; /* if */
    if (desc->oddecompr != NULL) {
        pa102FreeDecompr( desc->oddecompr );
        desc->oddecompr = NULL;
    }; /* if */
    wd115_DropStmts( desc );
    
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) desc );
} /* wd115DestroyContainerDesc */

#define UNEXP_RET_INTL_ERR		"Unexpected ODBC Return Code"

/*
 * Enter a new entry into the container table. Enter the id of the container
 * entry into the inode and vice-versa.
 */
sapdbwa_Bool wd115InsertContainer( twd115ContainerDescP desc,
                                 twd103InodeP         inode,
                                 sapdbwa_Bool           compress,
                                 sapdbwa_Bool           compressedData,
                                 twd26ErrP            err )
{
    RETCODE                  sqlRetcode;
    PTR                      pToken;
    
    /* set values for input parameters */
    desc->compress = compress && !compressedData; /* dont compress twice */
    desc->compressed = compress || compressedData;
    desc->compressFlag = (desc->compressed) ? COMPRESSED : UNCOMPRESSED;
    desc->cbCompr = 1;
    desc->inodeId = inode->InodeId;
    desc->cbNode = MAX_SYSKEY_LEN_WD103;
    desc->cbViewOwner = SQL_NTS;
    
    /* Insert into container */
    sqlRetcode = SQLExecute( desc->insertHstmt );
    
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->insertHstmt );
        return sapdbwa_False;
    }; /* if */

    desc->cbNode = MAX_SYSKEY_LEN_WD103;    
    sqlRetcode = SQLExecute( desc->selectSyskeyHstmt );

    if (wd00Success(sqlRetcode)) {
        sqlRetcode = SQLFetch( desc->selectSyskeyHstmt );
    } else {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->selectSyskeyHstmt );
        SQLFreeStmt( desc->selectSyskeyHstmt, SQL_CLOSE);
        return sapdbwa_False;
    }; /* if */
    
    if (desc->cbContainer == 0 || desc->cbContainer == SQL_NULL_DATA)
        wd103inodeidInit(desc->containerId);
    SQLFreeStmt( desc->selectSyskeyHstmt, SQL_CLOSE );

    /* update inode table */
    desc->cbNode = MAX_SYSKEY_LEN_WD103;

    sqlRetcode = SQLExecute( desc->updateInodeHstmt );
    
    if (!wd00Success( sqlRetcode )) {        
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->updateInodeHstmt );
        SQLFreeStmt( desc->updateInodeHstmt, SQL_DROP);
        return sapdbwa_False;
    }; /* if */

    /* update container table */
    desc->cbNode = MAX_SYSKEY_LEN_WD103;
    desc->cbBinBuf = SQL_LEN_DATA_AT_EXEC(0);
    
    sqlRetcode = SQLExecute( desc->updateContHstmt );

    if (sqlRetcode != SQL_NEED_DATA) {
        wd26SetErr( err, ERR_INTERNAL_WD26, UNEXP_RET_INTL_ERR, NULL );
        SQLFreeStmt( desc->updateContHstmt, SQL_DROP);
        return sapdbwa_False;
    }; /* if */

    sqlRetcode = SQLParamData( desc->updateContHstmt, &pToken);

    if (sqlRetcode != SQL_NEED_DATA) {
        wd26SetErr( err, ERR_INTERNAL_WD26, UNEXP_RET_INTL_ERR, NULL );
        SQLFreeStmt( desc->updateContHstmt, SQL_DROP);
        return sapdbwa_False;
    }; /* if */
    inode->ContainerId     = desc->containerId;
    desc->writeContainer   = sapdbwa_True;
    desc->comprInitialized = sapdbwa_False;
    desc->decomprInitialized = sapdbwa_False;
    
    return sapdbwa_True;
} /* wd115InsertContainer */

/************************************************************/
/*															*/
/* Delete an entry in the container table by an inode id	*/
/*															*/
/************************************************************/
sapdbwa_Bool wd115DeleteContainer( twd115ContainerDescP  desc,
                                   twd103InodeId        *inodeId,
                                   twd26ErrP             err      )
{
    RETCODE	sqlRetcode;
    
    /* set values for input parameters */
    SAPDB_memcpy( &desc->inodeId, inodeId, sizeof(twd103InodeId) );
    desc->cbNode = MAX_SYSKEY_LEN_WD103;
    desc->cbViewOwner = SQL_NTS;
    
    /* Delete container record */
    sqlRetcode = SQLExecute( desc->deleteHstmt );
    
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->deleteHstmt );
        return sapdbwa_False;
    }; /* if */
    
    return sapdbwa_True;
}


/*
 * Find the entry in the container table.
 */
sapdbwa_Bool wd115DbOpenContainer( twd115ContainerDescP desc,
                                 twd103InodeP         Inode,
                                 sapdbwa_Bool           uncompress,
                                 twd26ErrP            err )
{
    SQLLEN   cbNode = MAX_SYSKEY_LEN_WD103;
    RETCODE  sqlRetcode;
    
    desc->cbContainer = MAX_SYSKEY_LEN_WD103;
    desc->cbViewOwner = SQL_NTS;
    desc->containerId = Inode->ContainerId;
    sqlRetcode = SQLExecute( desc->selectDataHstmt );
    if (wd00Success( sqlRetcode) ) {
        sqlRetcode = SQLFetch(desc->selectDataHstmt );
        desc->compressed = (desc->compressFlag == COMPRESSED);
        Inode->compressed = desc->compressed;
    } else {
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->selectDataHstmt );
        SQLFreeStmt( desc->selectDataHstmt, SQL_CLOSE);
        return sapdbwa_False;
    }; /* if */
    desc->writeContainer     = sapdbwa_False;
    desc->decomprInitialized = sapdbwa_False;
    desc->uncompress         = uncompress;
    
    return sapdbwa_True;
} /* wd115DbOpenContainer */


sapdbwa_Bool wd115DbOpenContForUpdate( twd115ContainerDescP desc,
                                     twd103InodeP         inode,
                                     sapdbwa_Bool           compress,
                                     sapdbwa_Bool           compressedData,
                                     twd26ErrP            err )
{
    RETCODE                  sqlRetcode;
    PTR                      pToken;
    
    /* set values for input parameters */
    desc->compress = compress && !compressedData; /* dont compress twice */
    desc->compressed = compress || compressedData;
    desc->compressFlag = (desc->compressed) ? COMPRESSED : UNCOMPRESSED;
    desc->cbCompr = 1;
    desc->inodeId = inode->InodeId;
    desc->cbNode = MAX_SYSKEY_LEN_WD103;
    desc->cbViewOwner = SQL_NTS;
    desc->containerId = inode->ContainerId;
    
    /* update container table */
    desc->cbNode = MAX_SYSKEY_LEN_WD103;
    desc->cbBinBuf = SQL_LEN_DATA_AT_EXEC(0);
    
    sqlRetcode = SQLExecute( desc->updateContHstmt );

    if (sqlRetcode != SQL_NEED_DATA) {
        wd26SetErr( err, ERR_INTERNAL_WD26, UNEXP_RET_INTL_ERR, NULL );
        SQLFreeStmt( desc->updateContHstmt, SQL_DROP);
        return sapdbwa_False;
    }; /* if */

    sqlRetcode = SQLParamData( desc->updateContHstmt, &pToken);

    if (sqlRetcode != SQL_NEED_DATA) {
        wd26SetErr( err, ERR_INTERNAL_WD26, UNEXP_RET_INTL_ERR, NULL );
        SQLFreeStmt( desc->updateContHstmt, SQL_DROP);
        return sapdbwa_False;
    }; /* if */
    inode->ContainerId       = desc->containerId;
    desc->writeContainer     = sapdbwa_True;
    desc->comprInitialized   = sapdbwa_False;
    desc->decomprInitialized = sapdbwa_False;

    return sapdbwa_True;
} /* wd115DbOpenContForUpdate */


/*
 * Read the data of the blob in the container entry opened by
 * 'DbOpenContainer'.
 * This read starts where the last read ended (etc...).
 * Return the data read through '*Buf' which maximum length is 'Length'.
 * The number of bytes actually read is returned through '*Rtc'.
 * As long as data was read return True (even when truncated).
 */
sapdbwa_Bool wd115DbReadContainer( twd115ContainerDescP desc,
                                 twd103InodeP         Inode,
                                 char                *Buf,
                                 sapdbwa_UInt4          Length,
                                 sapdbwa_UInt4         *Rtc,
                                 twd26ErrP            err )
{
    SQLLEN                   cbValue;
    RETCODE                  retcode;
    sapdbwa_Int4             comprErr = 0;
    
    *Rtc = 0;

    if (Length > (sapdbwa_UInt4) LONG_MAX)
        Length = (sapdbwa_UInt4) LONG_MAX;

    if (desc->compressed && desc->uncompress) {
        if (!desc->decomprInitialized) {
            pa102InitDecompr( desc->oddecompr, desc->selectDataHstmt, 1 );
            desc->decomprInitialized = sapdbwa_True;
        }; /* if */
        retcode = pa102GetData( desc->oddecompr,
                                Buf, (SDWORD) Length, (SQLINTEGER*) &cbValue, &comprErr );
    } else {
        retcode = SQLGetData( desc->selectDataHstmt,
                              1,
                              SQL_C_BINARY,
                              Buf,
                              (SDWORD) Length,
                              &cbValue );
    }; /* else */
    
    if (!wd00Success(retcode) && (retcode != SQL_NO_DATA_FOUND)) {
        if (comprErr >= 0) {
            wd26SetOdbcError( err,
                              retcode,
                              SQL_NULL_HENV,
                              SQL_NULL_HDBC,
                              desc->selectDataHstmt );
        } else {
            wd26SetErr( err, ERR_UNCOMPRESS_WD26, (char*) &comprErr, NULL );
        }; /* else */
        return sapdbwa_False;
    }; /* if */

    if (cbValue == SQL_NULL_DATA) {
        *Rtc = 0;
        return sapdbwa_True;
    }; /* if */
    
    if (retcode == SQL_NO_DATA_FOUND) {
        *Rtc = 0;
    } else if (cbValue == SQL_NO_TOTAL) {
        *Rtc = (SDWORD) Length;  
    } else {
        *Rtc = min((SDWORD) Length, cbValue);
    }; /* else */
    
    return sapdbwa_True;
} /* wd115DbReadContainer */


/*
 * Write the data of the blob in the container entry created by
 * 'DbCreateContainer()'.
 * This write starts where the last write ended (etc...).
 * The data to be written is in 'Buf' with length 'Length'. '*Rtc' is
 * set to the number of bytes actually written .
 */
sapdbwa_Bool wd115DbWriteContainer( twd115ContainerDescP desc,
                                  twd103InodeP         Inode,
                                  char                *Buf,
                                  sapdbwa_UInt4          Length,
                                  sapdbwa_UInt4         *Rtc,
                                  twd26ErrP            err )
{
    RETCODE  sqlRetcode = SQL_SUCCESS;

    if (desc->compress) {
        /* wd115WriteCompressed must be used instead */
        wd26SetErr( err, ERR_INTERNAL_WD26,
                    "wd115WriteContainer called for compressed data", NULL );
        return sapdbwa_False;
    }; /* if */
    
    sqlRetcode = SQLPutData( desc->updateContHstmt,
                             Buf,
                             Length );
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err,
                          sqlRetcode,
                          SQL_NULL_HENV,
                          SQL_NULL_HDBC,
                          desc->updateContHstmt );
        *Rtc = 0;
        return sapdbwa_False;
    }; /* if */

    *Rtc = Length;
    if (!desc->compressed) {
        /* *Rtc is the umcompressed length */
        Inode->Size += *Rtc;
    } else if (Inode->Size == 0) { /* first call of Write */
        /* get the total uncompressed length from header of compressed data */
        Inode->Size = (sapdbwa_UInt4) pa102GetLen( desc->odcompr,
                                                 (UCHAR*) Buf );
    }; /* else */
    return sapdbwa_True;
} /* wd115WriteContainer */


sapdbwa_Bool wd115DbWriteCompressed( twd115ContainerDescP desc,
                                   twd103InodeP         Inode,
                                   twd105DynBufP        dynBuf,
                                   sapdbwa_UInt4         *Rtc,
                                   twd26ErrP            err )
{
    RETCODE    sqlRetcode = SQL_SUCCESS;
    sapdbwa_Int4 comprErr   = 0;
    sapdbwa_Int4 totalSize  = wd105GetTotalSize( dynBuf );
    
    if (!desc->compress) {
        /* wd115WriteContainer must be used instead */
        wd26SetErr( err, ERR_INTERNAL_WD26,
                    "wd115WriteCompressed called for uncompressed data",
                    NULL );
        return sapdbwa_False;
    }; /* if */

    if (!desc->comprInitialized) {
        sqlRetcode = pa102InitCompr( desc->odcompr,
                                     desc->updateContHstmt,
                                     totalSize,
                                     &comprErr );
        if (!wd00Success( sqlRetcode )) {
            if (comprErr >= 0) {
                wd26SetOdbcError( err,
                                  sqlRetcode,
                                  SQL_NULL_HENV,
                                  SQL_NULL_HDBC,
                                  desc->updateContHstmt );
            } else {
                wd26SetErr( err, ERR_COMPRESS_WD26, (char*) &comprErr, NULL );
            }; /* else */
            *Rtc = 0;
            return sapdbwa_False;
        }; /* if */        
        desc->comprInitialized = sapdbwa_True;
    }; /* if */
    
    sqlRetcode = wd105PutDataCompressed( dynBuf,
                                         desc->odcompr,
                                         &comprErr );
    if (!wd00Success( sqlRetcode )) {
        if (comprErr >= 0) {
            wd26SetOdbcError( err,
                              sqlRetcode,
                              SQL_NULL_HENV,
                              SQL_NULL_HDBC,
                              desc->updateContHstmt );
        } else {
            wd26SetErr( err, ERR_COMPRESS_WD26, (char*) &comprErr, NULL );
        }; /* else */
        *Rtc = 0;
        return sapdbwa_False;
    }; /* if */

    *Rtc = totalSize;
    Inode->Size += *Rtc;
    return sapdbwa_True;
} /* wd115WriteCompressed */

/*
 * Close the container which was opened or created by DbCreateContainer resp.
 * DbOpenContainer. Make sure the inode entry in the inode table and the
 * container entry in the container table are updated with the size of
 * the container blob.
 */
sapdbwa_Bool wd115DbCloseContainer( twd115ContainerDescP desc,
                                  twd103InodeP         Inode,
                                  twd26ErrP            err )
{
    sapdbwa_Bool               CloseSuccess = sapdbwa_True;
    SQLLEN                   cb;
    SQLLEN                   cbNode;
    RETCODE                  retcode = SQL_SUCCESS;
    PTR                      pToken;
    
    if ( desc->writeContainer ) {
        if (Inode->Size == 0) {
            /* call SQLPutData to prevent function sequence error */
            retcode = SQLPutData( desc->updateContHstmt,
                                  NULL, 0 );
        }; /* if */
        if (wd00Success( retcode )) {
            retcode = SQLParamData( desc->updateContHstmt, &pToken );
        }; /* if */
        if (!wd00Success(retcode)) {
            wd26SetOdbcError( err, retcode,
                              SQL_NULL_HENV,
                              SQL_NULL_HDBC,
                              desc->updateContHstmt );
            SQLFreeStmt( desc->updateContHstmt, SQL_CLOSE );
            return sapdbwa_False;
        }; /* if */

        SQLFreeStmt( desc->updateContHstmt, SQL_CLOSE );

        desc->cbSize  = 0;
        desc->sizeVal = (SDWORD) Inode->Size;
        desc->cbContainer = MAX_SYSKEY_LEN_WD103;
        desc->containerId = Inode->ContainerId;
        
        retcode = SQLExecute( desc->updateContSizeHstmt );
        if (!wd00Success( retcode )) {
            wd26SetOdbcError( err, retcode,
                              SQL_NULL_HENV,
                              SQL_NULL_HDBC,
                              desc->updateContSizeHstmt );
            SQLFreeStmt( desc->updateContSizeHstmt, SQL_CLOSE );
            return sapdbwa_False;
        }; /* if */

        desc->cbSize  = 0;
        desc->inodeId = Inode->InodeId;
        desc->cbNode  = MAX_SYSKEY_LEN_WD103;
        retcode = SQLExecute( desc->updateInodeSizeHstmt );
        if (!wd00Success(retcode)) {
            wd26SetOdbcError( err, retcode,
                              SQL_NULL_HENV,
                              SQL_NULL_HDBC,
                              desc->updateInodeSizeHstmt );
            SQLFreeStmt( desc->updateInodeSizeHstmt, SQL_CLOSE );
            return sapdbwa_False;
        }; /* if */
        desc->writeContainer     = sapdbwa_False;
        desc->comprInitialized   = sapdbwa_False;
        desc->decomprInitialized = sapdbwa_False;
        retcode = SQLFreeStmt( desc->updateInodeSizeHstmt, SQL_CLOSE );
        if (!wd00Success(retcode)) {
                wd26SetOdbcError( err,
                                  retcode,
                                  SQL_NULL_HENV,
                                  desc->hdbc,
                                  desc->updateInodeSizeHstmt );
                return sapdbwa_False;
        }; /* if */ 
    } else {
        retcode = SQLFreeStmt( desc->selectDataHstmt, SQL_CLOSE );
        if (!wd00Success(retcode)) {
                wd26SetOdbcError( err,
                                  retcode,
                                  SQL_NULL_HENV,
                                  desc->hdbc,
                                  desc->updateInodeSizeHstmt );
                CloseSuccess = sapdbwa_False;
        }; /* if */
    }; /* else */
    
    return CloseSuccess;
} /* wd115DbCloseContainer */

/*!**********************************************************************

  EndChapter: twd115ContainerDescP

************************************************************************/

sapdbwa_Bool wd115_AllocStmts( twd115ContainerDescP desc,
                             SQLHDBC              hdbc,
                             twd26ErrP            err )
{
    SQLRETURN  sqlRetcode;
    
    sqlRetcode = SQLAllocStmt( hdbc,
                               &desc->insertHstmt );
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT );
        return sapdbwa_False;
    }; /* if */
    
    sqlRetcode = SQLAllocStmt( hdbc,
                               &desc->deleteHstmt );
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT );
        return sapdbwa_False;
    }; /* if */
    
    sqlRetcode = SQLAllocStmt( hdbc,
                               &desc->selectSyskeyHstmt );
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT );
        return sapdbwa_False;
    }; /* if */
    
    sqlRetcode = SQLAllocStmt( hdbc,
                               &desc->updateInodeHstmt );
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT );
        return sapdbwa_False;
    }; /* if */

    sqlRetcode = SQLAllocStmt( hdbc,
                               &desc->updateContHstmt );
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT );
        return sapdbwa_False;
    }; /* if */

    sqlRetcode = SQLAllocStmt( hdbc,
                               &desc->selectDataHstmt );
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT );
        return sapdbwa_False;
    }; /* if */

    sqlRetcode = SQLAllocStmt( hdbc,
                               &desc->updateContSizeHstmt );
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT );
        return sapdbwa_False;
    }; /* if */

    sqlRetcode = SQLAllocStmt( hdbc,
                               &desc->updateInodeSizeHstmt );
    if (!wd00Success( sqlRetcode )) {
        wd26SetOdbcError( err, sqlRetcode, SQL_NULL_HENV, hdbc, SQL_NULL_HSTMT );
        return sapdbwa_False;
    }; /* if */

    return sapdbwa_True;
} /* wd115_AllocStmts */


sapdbwa_Bool wd115_DropStmts( twd115ContainerDescP desc )
{
    if (desc->insertHstmt != SQL_NULL_HSTMT) {
        SQLFreeStmt( desc->insertHstmt, SQL_DROP );
        desc->insertHstmt = SQL_NULL_HSTMT;
    }; /* if */

    if (desc->deleteHstmt != SQL_NULL_HSTMT) {
        SQLFreeStmt( desc->deleteHstmt, SQL_DROP );
        desc->deleteHstmt = SQL_NULL_HSTMT;
    }; /* if */

    if (desc->selectSyskeyHstmt != SQL_NULL_HSTMT) {
        SQLFreeStmt( desc->selectSyskeyHstmt, SQL_DROP );
        desc->selectSyskeyHstmt = SQL_NULL_HSTMT;
    }; /* if */

    if (desc->updateInodeHstmt != SQL_NULL_HSTMT) {
        SQLFreeStmt( desc->updateInodeHstmt, SQL_DROP );
        desc->updateInodeHstmt = SQL_NULL_HSTMT;
    }; /* if */

    if (desc->updateContHstmt != SQL_NULL_HSTMT) {
        SQLFreeStmt( desc->updateContHstmt, SQL_DROP );
        desc->updateContHstmt = SQL_NULL_HSTMT;
    }; /* if */

    if (desc->selectDataHstmt != SQL_NULL_HSTMT) {
        SQLFreeStmt( desc->selectDataHstmt, SQL_DROP );
        desc->selectDataHstmt = SQL_NULL_HSTMT;
    }; /* if */

    return sapdbwa_True;
} /* wd115_DropStmts */


sapdbwa_Bool wd115_PrepareStmts( twd115ContainerDescP desc,
                               twd26ErrP            err )
{
    sapdbwa_Bool prepareOK = sapdbwa_True;
    
    prepareOK = wd115_PrepareInsertStmt( desc, err )
        && wd115_PrepareDeleteInodeStmt( desc, err )
        && wd115_PrepareSelectSyskeyStmt( desc, err )
        && wd115_PrepareUpdateInodeStmt( desc, err )
        && wd115_PrepareUpdateContStmt( desc, err )
        && wd115_PrepareSelectDataStmt( desc, err )
        && wd115_PrepareUpdateInodeSizeStmt( desc, err )
        && wd115_PrepareUpdateContSizeStmt( desc, err );

    return prepareOK;
} /* wd115_PrepareStmts */


sapdbwa_Bool wd115_PrepareInsertStmt( twd115ContainerDescP desc,
                                      twd26ErrP            err )
{
    SQLRETURN sqlRetcode = SQL_SUCCESS;
    DBFS_STMT;

    SET_DBFS_STMT(INSERT_CONTAINER_STMT_PRE, wd104GetTableContainer(desc->fsDesc), INSERT_CONTAINER_STMT_POST);
    sqlRetcode = SQLPrepare( desc->insertHstmt,
                             (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
    if ( wd00Success(sqlRetcode) ) {
        /*
        sqlRetcode = SQLBindParameter( desc->insertHstmt,
                                       1,
                                       SQL_PARAM_INPUT,
                                       SQL_C_WCHAR,
                                       SQL_WCHAR,
                                       0,
                                       0,
                                       desc->viewOwnerUCS2,
                                       2*(DB_MAX_USERNAME_LEN_WD00+1),
                                       &desc->cbViewOwner );
        if ( wd00Success(sqlRetcode) ) {
        */
            sqlRetcode = SQLBindParameter( desc->insertHstmt,
                                           1,
                                           SQL_PARAM_INPUT,
                                           SQL_C_BINARY,
                                           SQL_BINARY,
                                           0,
                                           0,
                                           &desc->inodeId,
                                           MAX_SYSKEY_LEN_WD103,
                                           &desc->cbNode );
            if ( wd00Success(sqlRetcode) ) {         
                sqlRetcode = SQLBindParameter( desc->insertHstmt,
                                               2,
                                               SQL_PARAM_INPUT,
                                               SQL_C_BINARY,
                                               SQL_CHAR,
                                               0,
                                               0,
                                               &desc->compressFlag,
                                               1,
                                               &desc->cbCompr );
            }; /* if */
        /*};  */
    }; /* if */
    if (!wd00Success( sqlRetcode )) {        
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->insertHstmt );
    }; /* if */
    
    return wd00Success(sqlRetcode);
} /* wd115_PrepareInsertStmt */


sapdbwa_Bool wd115_PrepareDeleteInodeStmt( twd115ContainerDescP desc,
                                           twd26ErrP            err )
{
    SQLRETURN sqlRetcode = SQL_SUCCESS;
    DBFS_STMT;

    SET_DBFS_STMT(DELETE_INODE_STMT_PRE, wd104GetTableContainer(desc->fsDesc), DELETE_INODE_STMT_POST);
    sqlRetcode = SQLPrepare( desc->deleteHstmt, (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
    if ( wd00Success(sqlRetcode) ) {
        sqlRetcode = SQLBindParameter( desc->deleteHstmt,
                                       1,
                                       SQL_PARAM_INPUT,
                                       SQL_C_BINARY,
                                       SQL_BINARY,
                                       0,
                                       0,
                                       &desc->inodeId,
                                       MAX_SYSKEY_LEN_WD103,
                                       &desc->cbNode );
    }; /* if */
    if (!wd00Success( sqlRetcode )) {        
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->deleteHstmt );
    }; /* if */
    
    return wd00Success(sqlRetcode);

}


sapdbwa_Bool wd115_PrepareSelectSyskeyStmt( twd115ContainerDescP desc,
                                          twd26ErrP            err )
{
    SQLRETURN sqlRetcode = SQL_SUCCESS;
    DBFS_STMT;

    SET_DBFS_STMT(SELECT_SYSKEY_STMT_PRE, wd104GetTableContainer(desc->fsDesc),SELECT_SYSKEY_STMT_POST);
    sqlRetcode = SQLPrepare( desc->selectSyskeyHstmt,
                             (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
    if (wd00Success(sqlRetcode)) {
        sqlRetcode = SQLBindParameter( desc->selectSyskeyHstmt,
                                       1,
                                       SQL_PARAM_INPUT,
                                       SQL_C_BINARY,
                                       SQL_BINARY,
                                       0,
                                       0,
                                       &desc->inodeId,
                                       MAX_SYSKEY_LEN_WD103,
                                       &desc->cbNode );
        if (wd00Success(sqlRetcode)) {
            sqlRetcode = SQLBindCol( desc->selectSyskeyHstmt,
                                     1,
                                     SQL_C_BINARY,
                                     &desc->containerId,
                                     MAX_SYSKEY_LEN_WD103,
                                     &desc->cbContainer );
        }; /* if */
    }; /* if */
    if (!wd00Success( sqlRetcode )) {        
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->selectSyskeyHstmt );
    }; /* if */
    
    return wd00Success(sqlRetcode);
} /* wd115_PrepareSelectSyskeyStmt */


sapdbwa_Bool wd115_PrepareUpdateInodeStmt( twd115ContainerDescP desc,
                                         twd26ErrP            err )
{
    SQLRETURN sqlRetcode = SQL_SUCCESS;
    DBFS_STMT;

    SET_DBFS_STMT(UPDATE_INODE_STMT_PRE, wd104GetTableInode(desc->fsDesc), UPDATE_INODE_STMT_POST);
    sqlRetcode = SQLPrepare( desc->updateInodeHstmt,
                             (UCHAR*) DBFS_STMT_VAR, SQL_NTS);
    if (wd00Success( sqlRetcode )) {
        sqlRetcode = SQLBindParameter( desc->updateInodeHstmt,
                                       1,
                                       SQL_PARAM_INPUT,
                                       SQL_C_BINARY,
                                       SQL_BINARY,
                                       0,
                                       0,
                                       &desc->containerId,
                                       MAX_SYSKEY_LEN_WD103,
                                       &desc->cbContainer );
        if (wd00Success( sqlRetcode )) {        
            sqlRetcode = SQLBindParameter( desc->updateInodeHstmt,
                                           2,
                                           SQL_PARAM_INPUT,
                                           SQL_C_BINARY,
                                           SQL_BINARY,
                                           0,
                                           0,
                                           &desc->inodeId,
                                           MAX_SYSKEY_LEN_WD103,
                                           &desc->cbNode );
        }; /* if */
    }; /* if */
    if (!wd00Success( sqlRetcode )) {        
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->updateInodeHstmt );
    }; /* if */

    return wd00Success(sqlRetcode);
} /* wd115_PrepareUpdateInodeStmt */


sapdbwa_Bool wd115_PrepareUpdateContStmt( twd115ContainerDescP desc,
                                        twd26ErrP            err )
{
    SQLRETURN sqlRetcode = SQL_SUCCESS;
    DBFS_STMT;

    SET_DBFS_STMT(UPDATE_CONT_STMT_PRE, wd104GetTableContainer(desc->fsDesc), UPDATE_CONT_STMT_POST);
    sqlRetcode = SQLPrepare( desc->updateContHstmt,
                             (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
    if (wd00Success( sqlRetcode )) {
        sqlRetcode = SQLBindParameter( desc->updateContHstmt,
                                       1,
                                       SQL_PARAM_INPUT,
                                       SQL_C_BINARY,
                                       SQL_BINARY,
                                       0,
                                       0,
                                       (PTR) 1,
                                       0,
                                       &desc->cbBinBuf );
        if (wd00Success( sqlRetcode )) {
            sqlRetcode = SQLBindParameter( desc->updateContHstmt,
                                           2,
                                           SQL_PARAM_INPUT,
                                           SQL_C_BINARY,
                                           SQL_BINARY,
                                           0,
                                           0,
                                           &desc->containerId,
                                           MAX_SYSKEY_LEN_WD103,
                                           &desc->cbNode );
        }; /* if */
    }; /* if */
    if (!wd00Success( sqlRetcode )) {        
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->updateContHstmt );
    }; /* if */

    return wd00Success(sqlRetcode);
} /* wd115_PrepareUpdateContStmt */


sapdbwa_Bool wd115_PrepareSelectDataStmt( twd115ContainerDescP desc,
                                        twd26ErrP            err )
{
    SQLRETURN sqlRetcode = SQL_SUCCESS;
    DBFS_STMT;

    SET_DBFS_STMT(SELECT_DATA_STMT_PRE, wd104GetTableContainer(desc->fsDesc), SELECT_DATA_STMT_POST);
    sqlRetcode = SQLPrepare( desc->selectDataHstmt,
                             (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
    if (wd00Success( sqlRetcode) ) {
        sqlRetcode = SQLBindParameter( desc->selectDataHstmt,
                                       1,
                                       SQL_PARAM_INPUT,
                                       SQL_C_BINARY,
                                       SQL_BINARY,
                                       0,
                                       0,
                                       &desc->containerId,
                                       MAX_SYSKEY_LEN_WD103,
                                       &desc->cbContainer );
        if (wd00Success(sqlRetcode)) {
            sqlRetcode = SQLBindCol( desc->selectDataHstmt,
                                     2,
                                     SQL_C_BINARY,
                                     &desc->compressFlag,
                                     1,
                                     &desc->cbCompr );
        }; /* if */
    }; /* if */
    if (!wd00Success( sqlRetcode )) {        
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->selectDataHstmt );
    }; /* if */

    return wd00Success(sqlRetcode);    
} /* wd115_PrepareSelectDataStmt */


sapdbwa_Bool wd115_PrepareUpdateInodeSizeStmt( twd115ContainerDescP desc,
                                             twd26ErrP            err )
{
    SQLRETURN sqlRetcode = SQL_SUCCESS;
    DBFS_STMT;
    
    SET_DBFS_STMT(UPDATE_INODESIZE_STMT_PRE, wd104GetTableInode(desc->fsDesc), UPDATE_INODESIZE_STMT_POST);
    sqlRetcode = SQLPrepare( desc->updateInodeSizeHstmt,
                             (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
    if (wd00Success( sqlRetcode )) {
        sqlRetcode = SQLBindParameter( desc->updateInodeSizeHstmt,
                                       1,
                                       SQL_PARAM_INPUT,
                                       SQL_C_SLONG,
                                       SQL_INTEGER,
                                       0,
                                       0,
                                       &desc->sizeVal,
                                       0,
                                       &desc->cbSize );
        if (wd00Success( sqlRetcode )) {
            sqlRetcode = SQLBindParameter( desc->updateInodeSizeHstmt,
                                           2,
                                           SQL_PARAM_INPUT,
                                           SQL_C_BINARY,
                                           SQL_BINARY,
                                           0,
                                           0,
                                           &desc->inodeId,
                                           MAX_SYSKEY_LEN_WD103,
                                           &desc->cbNode );
        }; /* if */
    }; /* if */
    if (!wd00Success( sqlRetcode )) {        
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->updateInodeSizeHstmt );
    }; /* if */

    return wd00Success(sqlRetcode);
} /* wd115_PrepareUpdateInodeSizeStmt */


sapdbwa_Bool wd115_PrepareUpdateContSizeStmt( twd115ContainerDescP desc,
                                            twd26ErrP            err )
{
    SQLRETURN sqlRetcode = SQL_SUCCESS;
    DBFS_STMT;
    
    SET_DBFS_STMT(UPDATE_CONTSIZE_STMT_PRE, wd104GetTableContainer(desc->fsDesc), UPDATE_CONTSIZE_STMT_POST);
    sqlRetcode = SQLPrepare( desc->updateContSizeHstmt,
                             (UCHAR*) DBFS_STMT_VAR, SQL_NTS );
    if (wd00Success(sqlRetcode)) {
        sqlRetcode = SQLBindParameter( desc->updateContSizeHstmt,
                                       1,
                                       SQL_PARAM_INPUT,
                                       SQL_C_SLONG,
                                       SQL_INTEGER,
                                       0,
                                       0,
                                       &desc->sizeVal,
                                       0,
                                       &desc->cbSize );
        if (wd00Success(sqlRetcode)) {
            sqlRetcode = SQLBindParameter( desc->updateContSizeHstmt,
                                           2,
                                           SQL_PARAM_INPUT,
                                           SQL_C_BINARY,
                                           SQL_BINARY,
                                           0,
                                           0,
                                           &desc->containerId,
                                           MAX_SYSKEY_LEN_WD103,
                                           &desc->cbContainer );
        }; /* if */
    }; /* if */    
    if (!wd00Success( sqlRetcode )) {        
        wd26SetOdbcError( err, sqlRetcode,
                          SQL_NULL_HENV, SQL_NULL_HDBC,
                          desc->updateContSizeHstmt );
    }; /* if */

    return wd00Success(sqlRetcode);    
} /* wd115_PrepareUpdateContSizeStmt */
