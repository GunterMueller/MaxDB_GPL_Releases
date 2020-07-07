/*!**********************************************************************

  module: vwd101dbfs.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem API

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
#include <string.h>
#include <sys/stat.h>

#include "hwd20control.h"
#include "hwd26err.h"
#include "hwd35res.h"
#include "hwd101dbfs.h"
#include "hwd102fdesc.h"
#include "hwd103inode.h"
#include "hwd104fsdesc.h"
#include "hwd106pool.h"

#include "SAPDBCommon/SAPDB_string.h"

/*!**********************************************************************

  Chapter: prototypes for internal functions

************************************************************************/

struct st_dbfs {
    sapdbwa_Bool		exclusiveTransactions;	/* only SQLStudio gets non exclusive conn	*/
    twd106PoolP         pool;
    twd35ResourcePoolP  fileDescPool;			/* this manages all file-descriptors		*/
    sapdbwa_UInt4		partitionId;			/* Partition id for multiple partitions		*/
    twd26ErrP           err;					/* last error								*/    
}; /* struct st_dbfs */

typedef struct st_dbfs twd101DBFS;

/* max size of fileDescPool == max. number of open files */
#define MAX_DESC_ARRAY_SIZE 4096

#define DBMAXNAMELENGTH 245
#define DBVF_CPBLOCKSIZE	4096	/* too large .-> probleme with local
                                     * data on communications */
#define	DBMAXPATHNAMELENGTH (10*DBMAXNAMELENGTH)

enum en_io {
    no_io = 0,
    io_read,
    io_write
};

/* sapdbwa_False for files with extension .gif, .zip, ... */
sapdbwa_Bool wd101_AllowCompression( const char *path );

sapdbwa_Bool wd101_CloseInternal( twd101DBFSP dbfs, twd102FileDescP fd );

/* close without resetting error */
sapdbwa_Bool wd101_Close( twd101DBFSP dbfs, twd101DBFile fno );

sapdbwa_Bool wd101_CmdBasename( char         Basename[],
                                const char   Path[],
                                char         PathSeparator,
                                twd26ErrP    err );

sapdbwa_Bool wd101_Copy( twd101DBFSP     dbfs,
                         twd106ConnP     conn,
                         const char     *sourcePath,
                         const char     *destPath,
                         sapdbwa_Bool      withLinks );

/* delete inode if Path has given type (directory or file)
 * type 0 means any type. */
sapdbwa_Bool wd101_DeleteInode( twd101DBFSP      dbfs,
                                twd106ConnP      conn,
                                const char      *Path,
                                twd103InodeType  type,
                                sapdbwa_Bool       recursive );

void wd101_FreeDescArray( twd101DBFSP dbfs );

twd104FSDescP wd101_GetFSDescFromConnection( twd106ConnP	hconn,
											 sapdbwa_UInt4	partitionId,
                                             twd26ErrP		err			);

twd102FileDescP wd101_GetFreeDesc( twd101DBFSP  dbfs,
                                   twd106ConnP  conn );

void wd101_FreeDesc( twd101DBFSP  dbfs, twd102FileDescP fDesc );

sapdbwa_Bool wd101_FileIO( twd104FSDescP      fsDesc,
                           twd103InodeP       Inode,
                           void              *buf,
                           tsp00_Uint4        nbyte,
                           enum en_io         ioFlag,
                           tsp00_Uint4       *Rtc,
                           twd26ErrP          err );

sapdbwa_Bool wd101_FindInInode( twd104FSDescP  fsDesc,
                                twd103InodeId *Directory,
                                char          *Path,
                                twd103Inode   *Inode,
                                twd26ErrP      err );

sapdbwa_Bool wd101_Create( twd104FSDescP    fsDesc,
                           char            *Path,
                           twd103InodeId   *linkInode,
                           twd103InodeType  InodeType,
                           twd102FileDescP  fd,
                           twd101OpenFlag   oflag,
                           twd26ErrP        err );

sapdbwa_Bool wd101_GetFileDesc( twd101DBFSP      dbfs,
                                twd101DBFile     fno,
                                twd102FileDescP *fdesc );

/* used as destructor call-back-function */
sapdbwa_Bool wd101_DestroyFSDesc( void *fsdescHandle );

sapdbwa_Bool wd101_InitDBFS( twd101DBFSP	dbfs,
							 sapdbwa_UInt4	partitionId	);

sapdbwa_Bool wd101_InodeClose( twd104FSDescP    fsDesc,
                               twd102FileDescP  fd,
                               twd26ErrP        err );

sapdbwa_Bool wd101_InodeOpen( twd104FSDescP    fsDesc,
                              twd102FileDescP  fd,
                              twd101OrderFlag  orderFlag,
                              twd26ErrP        err );

sapdbwa_Bool wd101_InodeRead( twd104FSDescP    fsDesc,
                              twd102FileDescP  fd,
                              void            *buf,
                              tsp00_Uint4      nbyte,
                              twd103Inode     *inode,
                              tsp00_Uint4     *Rtc,
                              twd26ErrP        err );

void wd101_MakeDate( char *odbcDate, char fsDate[9] );

void wd101_MakeTimestamp( char *odbcDate,
                          char *odbcTime,
                          char  timeStamp[13] );

sapdbwa_Bool wd101_PathBasename( char		*Path,
								 tsp00_Int2	*PrefixLength,
								 char		*Basename,
								 twd26ErrP	 err		);

tsp00_Int4 wd101_DbReadEntry( twd101DBFSP          dbfs,
                              twd104FSDescP        fsDesc,
                              twd101DBFile         fno,
                              struct st_db_direct *DirectoryEntry,
                              twd103Inode         *inode );

sapdbwa_Bool wd101_Remove( twd104FSDescP    fsDesc,
                           char            *Path,
                           twd103InodeType  Type,
                           sapdbwa_Bool     recursive,
                           twd26ErrP        err );

sapdbwa_Bool wd101_CommonPathPrefix( const char	*path1,
									 const char	*path2,
									 twd26ErrP	 err	);

sapdbwa_Bool wd101_IsValidFilename( const char *name, twd26ErrP err );

sapdbwa_Bool wd101_ValidFilenameChar( char ch );

/* cut off path at the last '/' and store it in dirName */
void wd101_DirName( char *dirName, tsp00_Int4 len, const char *path );

/* sapdbwa_True if path is '\0' or '/' */
sapdbwa_Bool wd101_IsRoot( char *path );

/* sapdbwa_True if path2 is subdir of path1 */
sapdbwa_Bool wd101_IsSubDir( const char *path1, const char *path2 );

sapdbwa_Bool wd101_PathLengthOk( const char *path, twd26ErrP err );

sapdbwa_Bool wd101_SetMode( twd103InodeType  inodeType,
                            twd101Mode      *mode,
                            twd26ErrP        err );


sapdbwa_Bool wd101_PrependPath( char *destStr, char *newStr, tsp00_Int4 bufLen );

/*!**********************************************************************

  Chapter: twd101DBFS

************************************************************************/

static twd101DBFS wd101FS;

#define wd101_FileDesc2No(fd) (fd ? (fd->index) : -1)

/************************************************************************
 * command interface
 ************************************************************************/

sapdbwa_Bool WINAPI wd101InitDBFS()
{
    sapdbwa_Bool     retcode = sapdbwa_True;
    sapdbwa_SessionPoolP sessionPool = NULL;
    
    wd101FS.err = wd26CreateErr();

    if (wd101FS.err == NULL) {
        retcode = sapdbwa_False;
    } else {
        wd101FS.partitionId = DEFAULT_PARTITION_ID_WD101;
        sessionPool = wd20ApiDBFSInit( wd101FS.err );
        if (sessionPool == NULL) {
            retcode = sapdbwa_False;
            /* wd26DestroyErr( wd101FS.err ); */
        } else {
            wd101FS.pool = wd106CreatePoolFromSes( sessionPool );
            wd101FS.fileDescPool =
                wd35CreateResourcePool( MAX_DESC_ARRAY_SIZE );
            wd101FS.exclusiveTransactions = sapdbwa_True;
            if (wd101FS.fileDescPool == NULL) {
                retcode = sapdbwa_False;
                wd20ApiDBFSExit();
                wd26DestroyErr( wd101FS.err );
            }; /* if */
        }; /* else */
    }; /* else */

    return retcode;
} /* wd101InitDBFS */


sapdbwa_Bool WINAPI wd101ExitDBFS()
{
    wd101_FreeDescArray( &wd101FS );
    wd35DestroyResourcePool( wd101FS.fileDescPool );
    wd106DestroyPool( wd101FS.pool );
    wd20ApiDBFSExit();
    if (wd101FS.err != NULL) {
        wd26DestroyErr( wd101FS.err );
        wd101FS.err = NULL;
    };
    
    return sapdbwa_True;
} /* wd101ExitDBFS */


sapdbwa_Bool WINAPI wd101CmdCopy( const char     *sourcePath,
                                const char     *destPath )
{
    return wd101Copy( &wd101FS, sourcePath, destPath );
} /* wd101CmdCopy */


sapdbwa_Bool WINAPI wd101CmdFormatDBFS()
{
    return wd101FormatDBFS( &wd101FS );
} /* wd101CmdFormatDBFS */


tsp00_Uint4 WINAPI wd101CmdGetErrorId()
{
    return wd101GetErrorId( &wd101FS );
} /* wd101GetErrorId */


const char* WINAPI wd101CmdGetErrorMsg()
{
    return wd101GetErrorMsg( &wd101FS );
} /* wd101GetErrorMsg */


sapdbwa_Bool WINAPI wd101CmdMkDir( const char *path )
{
    return wd101MkDir( &wd101FS, path );
} /* wd101CmdMkDir */



/* Copy the file 'SrcPath' to 'DstPath' which is to be in the database.
 * If 'DstPath' is a database directory, then the basename of
 * 'SrcPath' is copied in that database directory.
 */
sapdbwa_Bool WINAPI wd101CmdCpIn( const char     SrcPath[],
                                const char     DstPath[] )
{
    char           cpBlock[DBVF_CPBLOCKSIZE];
    tsp00_Uint4    cpBlockLg = 0;
    FILE          *fin;
    tsp00_Uint4    wlg = 0;
    twd101DBFile   fno;
    struct st_db_stat  dbStat;
    char           tmpName[DBMAXPATHNAMELENGTH+1];
    char           SrcPathBasename[DBMAXNAMELENGTH+1];

    wd26ResetErr( wd101FS.err );
    if (!wd101_CmdBasename( SrcPathBasename,
                            (char*) SrcPath, OS_PATH_SEP_WD00,
                            wd101FS.err )) {
        return sapdbwa_False;
    }; /* if */
    if (strlen( (char*) SrcPathBasename) == 0) {
        wd26SetErr( wd101FS.err, ERR_FILE_NOT_FOUND_WD26, "", NULL );
        return(sapdbwa_False);
    }; /* if */

    /* Check if 'DstPath' is a directory */
    if (wd101Stat( &wd101FS, DstPath, &dbStat ) && dbStat.mode == DB_DIR) {
        strcpy( tmpName, (char*) DstPath);
        if (tmpName[strlen(tmpName)-1] != DBFS_PATH_SEP_WD101)
            strcat (tmpName, "/");
        strcat( tmpName, SrcPathBasename);
        DstPath = tmpName;      /* New DstPath */
    }; /* if */

    /* Open SrcPath, create DstPath, and copy, then close */
    if (NULL == (fin = fopen( (char*) SrcPath, "rb"))) {
        wd26SetErr( wd101FS.err, ERR_FILE_NOT_FOUND_WD26, SrcPath, NULL );
        return(sapdbwa_False);
    }; /* if */

    if (-1 == (fno = wd101Create( &wd101FS, DstPath,
                                  access_write_compressed ))) {
        fclose(fin);
        
        return(sapdbwa_False);
    }; /* if */

    while (0 < (cpBlockLg = fread(cpBlock, 1, sizeof(cpBlock), fin)))
        if (cpBlockLg != (wlg = wd101Write( &wd101FS,
                                            fno,
                                            cpBlock,
                                            cpBlockLg ))) {
            fclose(fin);
            wd101_Close( &wd101FS, fno );
            
            return(sapdbwa_False);
        }; /* if */
    
    fclose(fin);

    return wd101_Close( &wd101FS, fno );
} /* wd101CmdCpin */



sapdbwa_Bool WINAPI wd101CmdCpOut( const char   SrcPath[],
                                 const char   DstPath[] )
{
    char         cpBlock[DBVF_CPBLOCKSIZE];
    tsp00_Int4   cpBlockLg = 0;
    FILE        *fout;
    tsp00_Int4   rlg = 0;
    twd101DBFile fno;
    tsp00_Int4   TotRead = 0;
    char         SrcPathBasename[DBMAXNAMELENGTH+1];
    char         tmpName[DBMAXPATHNAMELENGTH+1];
    struct stat  fsStat;

    wd26ResetErr( wd101FS.err );
    if (!wd101_CmdBasename( SrcPathBasename,
                            (char*) SrcPath,
                            DBFS_PATH_SEP_WD101,
                            wd101FS.err )
        || !wd101_IsValidFilename( SrcPathBasename, wd101FS.err )) {
        return sapdbwa_False;
    }; /* if */
    if (strlen(SrcPathBasename) == 0) {
        wd26SetErr( wd101FS.err, ERR_FILE_NOT_FOUND_WD26, "", NULL );
        return(sapdbwa_False);
    }; /* if */

    if (-1 == (fno = wd101Open( &wd101FS, SrcPath, access_read ))) {
        return sapdbwa_False;
    }; /* if */

    /* Check if 'DstPath' is a directory */
    if (-1 != stat( DstPath, &fsStat) && (fsStat.st_mode & S_IFDIR)) {
        strcpy(tmpName, DstPath);
        if (tmpName[strlen(tmpName)-1] != OS_PATH_SEP_WD00) {
            tmpName[strlen(tmpName)+1] = '\0';
            tmpName[strlen(tmpName)] = OS_PATH_SEP_WD00;
        }; /* if */
        strcat(tmpName, SrcPathBasename);
        DstPath = tmpName;      /* New DstPath */
    }; /* if */

    if (NULL == (fout = fopen( (char*) DstPath, "wb"))) {
        wd101_Close( &wd101FS, fno );
        wd26SetErr( wd101FS.err, ERR_FILE_OPEN_WD26, DstPath, NULL );
        return (sapdbwa_False);
    }; /* if */
    while (0 < (cpBlockLg = wd101Read( &wd101FS,
                                       fno,
                                       cpBlock,
                                       sizeof(cpBlock) ))) {
        TotRead += cpBlockLg;
        rlg = fwrite( cpBlock, 1, cpBlockLg, fout);
        if (cpBlockLg != rlg) {
            fclose(fout);
            wd101_Close( &wd101FS, fno );
            wd26SetErr( wd101FS.err, ERR_WRITE_ERROR_WD26, DstPath, NULL );
            return(sapdbwa_False);
        }; /* if */
        if (cpBlockLg != sizeof(cpBlock))
            break;
    }; /* while */

    fclose( fout );

    if (!wd101_Close( &wd101FS, fno )) {
        return(sapdbwa_False);
    }; /* if */
    
    return (cpBlockLg >= 0);
} /* wd101CmdCpOut */


sapdbwa_Bool WINAPI wd101CmdLn( const char *targetPath, const char *newLink )
{
    return wd101CreateLink( &wd101FS, newLink, targetPath );
} /* wd101CmdLn */


sapdbwa_Bool WINAPI wd101CmdLs( const char  Path[] )
{
    twd104FSDescP        fsDesc;
    twd101DBFile         fno;
    struct st_db_stat    fsStat;
    struct st_db_direct  DirectoryEntry;
    char                 tmpPath[1024];
    char                *orgPath = (char*) Path;
    twd102FileDescP      fdesc;
    twd103Inode          inode;
    char                 modificationDate[9];
    char                 modificationTimestamp[9];

    wd103InitInode( &inode );
    wd26ResetErr( wd101FS.err );
    if (-1 == (fno = wd101OpenDir( &wd101FS, Path, name_asc ))) {
        return sapdbwa_False;
    }; /* if */

    if (!wd101_GetFileDesc( &wd101FS, fno, &fdesc )) {
        return sapdbwa_False;
    }; /* if */
    
    fsDesc = wd101_GetFSDescFromConnection( fdesc->connection,
                                            wd101FS.partitionId,
                                            wd101FS.err );
    if (fsDesc == NULL) {
        return sapdbwa_False;
    }; /* if */
        
    if (!wd101FStat( &wd101FS, fno, &fsStat )) {
        return sapdbwa_False;
    }; /* if */

    switch (fsStat.mode) {
    case DB_DIR:
        while ( wd101_DbReadEntry( &wd101FS, fsDesc, fno,
                                   &DirectoryEntry, &inode ) > 0) {
            wd101_MakeTimestamp( inode.modificationDate,
                                 inode.modificationTime,
                                 modificationTimestamp );
            /*wd101_MakeDate( inode.modificationDate, modificationDate );*/
                if (inode.Type == inode_directory) {
                    sprintf( tmpPath,
                             "drwxrwxrwx   1 wa       wa    %10d %s %s\n",
                             inode.Size, modificationTimestamp,
                             DirectoryEntry.name );
                    /*
                    sprintf( tmpPath, "%8s %6s         <DIR>          %s\n",
                             modificationDate,
                             inode.modificationTime,
                             DirectoryEntry.name );
                    */
                } else if (inode.Type == inode_link) {
                    sprintf( tmpPath,
                             "lrwxrwxrwx   1 wa       wa    %10d %s %s\n",
                             inode.Size, modificationTimestamp,
                             DirectoryEntry.name );
                } else {
                    sprintf( tmpPath,
                             "-rwxrwxrwx   1 wa       wa    %10d %s %s\n",
                             inode.Size, modificationTimestamp,
                             DirectoryEntry.name );
                    /*
                    sprintf( tmpPath, "%8s %6s            %11d %s\n",
                             modificationDate,
                             inode.modificationTime,
                             inode.Size,
                             DirectoryEntry.name );
                    */
                }; /* else */
                printf( tmpPath );
        }; /* while */
        break;
    case DB_FIL:
        /* format string ... */
        printf( Path );
        break;
    }; /* switch */

    return sapdbwa_True;
} /* CmdLs */


sapdbwa_Bool WINAPI wd101CmdRm( const char *Path )
{
    return wd101Rm( &wd101FS, Path );
} /* wd101CmdRm */


sapdbwa_Bool WINAPI wd101CmdRmDir( const char *Path, sapdbwa_Bool recursive )
{
    return wd101RmDir( &wd101FS, Path, recursive );
} /* wd101CmdRmDir */


sapdbwa_Bool WINAPI wd101CmdMv( const char *oldPath, const char *newPath )
{
    return wd101Mv( &wd101FS, oldPath, newPath );
} /* wd101CmdRm */


/************************************************************************
 * programming interface
 ************************************************************************/

twd101DBFSP WINAPI wd101CreateDBFS( sapdbwa_SessionPoolP	pool,
                                    sapdbwa_UInt4			partitionId )
{
    twd101DBFSP newDBFS;
    sapdbwa_Bool  allocatOK;

    sapdbwa_SQLALLOCAT( sizeof( struct st_dbfs ),
						(tsp00_Uint1**) &newDBFS,
						&allocatOK );
    if (!allocatOK) {
        newDBFS = NULL;
    } else {
        newDBFS->pool = wd106CreatePoolFromSes( pool );
        if (newDBFS->pool == NULL) {
            sapdbwa_SQLFREE( (tsp00_Uint1*) newDBFS );
            newDBFS = NULL;
        } else {
            if (!wd101_InitDBFS( newDBFS, partitionId )) {
                wd106DestroyPool( newDBFS->pool );
                sapdbwa_SQLFREE( (tsp00_Uint1*) newDBFS );
                newDBFS = NULL;
            }; /* if */
        }; /* else */
    }; /* else */

    return newDBFS;
} /* wd101CreateDBFS */


twd101DBFSP WINAPI wd101CreateDbcDBFS( sapdbwa_DBCPoolP	 dbcPool,
                                       sapdbwa_DBCP	     dbc,
                                       const char		*datasource,
                                       const char		*driver,
                                       const char		*serverNode,
                                       const char		*serverDb,
                                       const char		*user,
                                       const char		*password,
                                       const char		*sqltrace,
                                       sapdbwa_UInt4	 partitionId	)
{
    twd101DBFSP newDBFS;
    sapdbwa_Bool  allocatOK;

    sapdbwa_SQLALLOCAT( sizeof( struct st_dbfs ),
						(tsp00_Uint1**) &newDBFS,
						&allocatOK );

    if (!allocatOK) {
        newDBFS = NULL;
    } else {
        newDBFS->pool = wd106CreatePoolFromDbc( dbcPool, datasource, driver,
                                                serverNode, serverDb,
                                                user, password,
                                                sqltrace );
        if (newDBFS->pool == NULL) {
            sapdbwa_SQLFREE( (tsp00_Uint1*) newDBFS );
            newDBFS = NULL;
        } else {
            if (!wd101_InitDBFS( newDBFS, partitionId )) {
                wd106DestroyPool( newDBFS->pool );
                sapdbwa_SQLFREE( (tsp00_Uint1*) newDBFS );
                newDBFS = NULL;
            }; /* if */
            /* overwrite default (sapdbwa_True) for dbcDBFS which is used by SQLStudio */
            /* This is only for SQLStudio !!!*/
            newDBFS->exclusiveTransactions = sapdbwa_False;
            wd106SetDBCCon(newDBFS->pool, dbc);
        }; /* else */
    }; /* else */

    return newDBFS;
} /* wd101CreateDbcDBFS */


sapdbwa_Bool WINAPI wd101DestroyDBFS( twd101DBFSP dbfs )
{
    wd106DestroyPool( dbfs->pool );
    wd26DestroyErr( dbfs->err );
    wd101_FreeDescArray( dbfs );
    wd35DestroyResourcePool( dbfs->fileDescPool );
    sapdbwa_SQLFREE( (tsp00_Uint1*) dbfs );
    
    return sapdbwa_True;
} /* wd101DestroyDBFS */


sapdbwa_Bool wd101_Close( twd101DBFSP dbfs, twd101DBFile fno )
{
    sapdbwa_Bool       ok     = sapdbwa_True;
    twd102FileDescP  fd     = NULL;
    twd104FSDescP    fsDesc = NULL;
    
    if (wd101_GetFileDesc( dbfs, fno, &fd )) {
        ok = wd101_CloseInternal( dbfs, fd );
        fsDesc = wd101_GetFSDescFromConnection( fd->connection,
                                                dbfs->partitionId,
                                                dbfs->err );
        if (fsDesc != NULL) {
            wd104DbTransaction( fsDesc, ok, dbfs->err );
        };
        wd106Disconnect( fd->connection );
        wd106DestroyConn( fd->connection );
    } else {
        ok = sapdbwa_False;
    };

    return ok;
} /* wd101_Close */


sapdbwa_Bool WINAPI wd101Close( twd101DBFSP dbfs, twd101DBFile fno )
{
    sapdbwa_Bool ok = sapdbwa_True;
    
    wd26ResetErr( dbfs->err );
    ok = wd101_Close( dbfs, fno );                

    return ok;
} /* wd101Close */


sapdbwa_Bool WINAPI wd101_CloseDir( twd101DBFSP dbfs, twd102FileDescP fd )
{
    sapdbwa_Bool      ok = sapdbwa_True;
    twd104FSDescP   fsDesc = NULL;
    
    fsDesc = wd101_GetFSDescFromConnection( fd->connection,
                                            dbfs->partitionId,
                                            dbfs->err );
    if (fsDesc == NULL) {
        ok = sapdbwa_False;
    } else {
        if (fd->inode.Type == inode_directory)
            wd101_InodeClose( fsDesc, fd, dbfs->err );
        else
            ok = sapdbwa_False;
        
        if (!ok) {
            sapdbwa_Bool connDown = sapdbwa_False;
            /* check if connection is down */
            wd106CheckConn( fd->connection,
                            wd26GetNativeOdbcErr( dbfs->err ),
                            sapdbwa_GetSqlState( dbfs->err ),
                            &connDown );
        }; /* if */
        fd->access = no_access; /* file is not opened anymore */
        wd101_FreeDesc( dbfs, fd );
    }; /* else */
    
    return (ok);
} /* wd101_CloseDir */


sapdbwa_Bool WINAPI wd101CloseDir( twd101DBFSP dbfs, twd101DBFile fno )
{
    sapdbwa_Bool       ok = sapdbwa_True;
    twd102FileDescP  fd = NULL;
    
    wd26ResetErr( dbfs->err );
    if (wd101_GetFileDesc( dbfs, fno, &fd )) {
        twd104FSDescP  fsDesc = NULL;
        
        ok = wd101_CloseDir( dbfs, fd );
        fsDesc = wd101_GetFSDescFromConnection( fd->connection,
                                                dbfs->partitionId,
                                                dbfs->err );
        if (fsDesc == NULL) {
            ok = sapdbwa_False;
        } else {
            if (!wd104DbTransaction( fsDesc, ok, dbfs->err ))
                ok = sapdbwa_False;
        };
        
        wd106Disconnect( fd->connection );
        wd106DestroyConn( fd->connection );
    } else {
        ok = sapdbwa_False;
    };

    return ok;
} /* wd101CloseDir */


void WINAPI wd101CopyErrorMsg( twd101DBFSP     dbfs,
                               char           *buf,
                               tsp00_Uint4     bufLen )
{
    wd26CopyMsg( dbfs->err, buf, bufLen );
} /* wd101CopyErrorMsg */


sapdbwa_Bool WINAPI wd101Copy( twd101DBFSP     dbfs,
                             const char     *sourcePath,
                             const char     *destPath )
{
    sapdbwa_Bool   ok = sapdbwa_True;
    twd106ConnP  conn = wd106CreateConn( sapdbwa_False );

    wd26ResetErr( dbfs->err );
    if (conn != NULL) {
        if (!wd106Connect( dbfs->pool, conn, dbfs->err )) {
            ok = sapdbwa_False;
        } else {
            twd104FSDescP fsDesc = NULL;
                
            ok = wd101_Copy( dbfs, conn, sourcePath, destPath, sapdbwa_False );
            fsDesc = wd101_GetFSDescFromConnection( conn,
                                                    dbfs->partitionId,
                                                    dbfs->err );
            if (fsDesc != NULL) {
                if (!wd104DbTransaction( fsDesc, ok, dbfs->err )) {
                    ok = sapdbwa_False;
                };
            } else {
                ok = sapdbwa_False;
            };
            wd106Disconnect( conn );
        };
        wd106DestroyConn( conn );
    } else {
        wd26SetErr( dbfs->err, ERR_MEMORY_WD26, NULL, NULL );
        ok = sapdbwa_False;
    };

    return ok;
} /* wd101Copy */


sapdbwa_Bool WINAPI wd101CopyWithLinks( twd101DBFSP     dbfs,
                                      const char     *sourcePath,
                                      const char     *destPath )
{
    sapdbwa_Bool   ok = sapdbwa_True;
    twd106ConnP  conn = wd106CreateConn( sapdbwa_False );

    wd26ResetErr( dbfs->err );
    if (conn != NULL) {
        if (!wd106Connect( dbfs->pool, conn, dbfs->err )) {
            ok = sapdbwa_False;
        } else {
            twd104FSDescP fsDesc = NULL;
                
            ok = wd101_Copy( dbfs, conn, sourcePath, destPath, sapdbwa_True );
            fsDesc = wd101_GetFSDescFromConnection( conn,
                                                    dbfs->partitionId,
                                                    dbfs->err );
            if (fsDesc != NULL) {
                if (!wd104DbTransaction( fsDesc, ok, dbfs->err )) {
                    ok = sapdbwa_False;
                };
            } else {
                ok = sapdbwa_False;
            };
            wd106Disconnect( conn );
        };
        wd106DestroyConn( conn );
    } else {
        wd26SetErr( dbfs->err, ERR_MEMORY_WD26, NULL, NULL );
        ok = sapdbwa_False;
    };

    return ok;
} /* wd101CopyWithLinks */


sapdbwa_Bool wd101_Delete( twd101DBFSP      dbfs,
                         const char      *Path,
                         twd103InodeType  type,
                         sapdbwa_Bool       recursive )
{
    sapdbwa_Bool   ok = sapdbwa_True;
    twd106ConnP  conn = wd106CreateConn( sapdbwa_False );

    wd26ResetErr( dbfs->err );
    if (conn != NULL) {
        if (!wd106Connect( dbfs->pool, conn, dbfs->err )) {
            ok = sapdbwa_False;
        } else {
            twd104FSDescP fsDesc = NULL;
                
            ok = wd101_DeleteInode( dbfs, conn, Path, type, recursive );
            fsDesc = wd101_GetFSDescFromConnection( conn,
                                                    dbfs->partitionId,
                                                    dbfs->err );
            if (fsDesc != NULL) {
                if (!wd104DbTransaction( fsDesc, ok, dbfs->err )) {
                    ok = sapdbwa_False;
                };
            } else {
                ok = sapdbwa_False;
            };
            wd106Disconnect( conn );
        };
        wd106DestroyConn( conn );
    } else {
        wd26SetErr( dbfs->err, ERR_MEMORY_WD26, NULL, NULL );
        ok = sapdbwa_False;
    };

    return ok;
} /* wd101_Delete */


sapdbwa_Bool WINAPI wd101Delete( twd101DBFSP  dbfs, const char  *Path )
{
    return wd101_Delete( dbfs, Path, 0 /* any type */, sapdbwa_False );
} /* wd101Delete */


sapdbwa_Bool WINAPI wd101FormatDBFS( twd101DBFSP dbfs )
{
    twd104FSDescP fsDesc;
    sapdbwa_Bool    retcode = sapdbwa_True;
    twd106ConnP   conn = wd106CreateConn( sapdbwa_True );
    
    wd26ResetErr( dbfs->err );
    if (!wd106Connect( dbfs->pool, conn, dbfs->err )) {
        retcode = sapdbwa_False;
    } else {
        fsDesc = wd101_GetFSDescFromConnection( conn,
                                                dbfs->partitionId,
                                                dbfs->err );
        if (fsDesc == NULL) {
            retcode = sapdbwa_False;
        } else {
            retcode = wd104FormatDBFS( fsDesc, dbfs->err );
            if (!wd104DbTransaction( fsDesc, retcode, dbfs->err )) {
                retcode = sapdbwa_False;
            }; /* if */
        }; /* else */
        if (!retcode) {
            sapdbwa_Bool connDown = sapdbwa_False;
            /* check if connection is down */
            wd106CheckConn( conn,
                            wd26GetNativeOdbcErr( dbfs->err ),
                            sapdbwa_GetSqlState( dbfs->err ),
                            &connDown );
        }; /* if */
        wd106Disconnect( conn );
    }; /* else */
    wd106DestroyConn( conn );
        
    return retcode;
} /* wd101FormatDBFS */


tsp00_Int4 wd101_GetDirEntry( twd101DBFSP  dbfs,
                              twd101DBFile fno,
                              char        *buf,
                              tsp00_Int4   bufLen,
                              sapdbwa_Bool   details )
{
    tsp00_Int4           bytesRead;
    twd104FSDescP        fsDesc;
    struct st_db_stat    fsStat;
    struct st_db_direct  DirectoryEntry;
    char                 tmpPath[1024];
    twd102FileDescP      fdesc = NULL;
    twd103Inode          inode;
    char                 modificationTimestamp[13];
    
    wd103InitInode( &inode );
    if (!wd101_GetFileDesc( dbfs, fno, &fdesc )) {
        return -1;
    }; /* if */
    fsDesc = wd101_GetFSDescFromConnection( fdesc->connection,
                                            dbfs->partitionId,
                                            dbfs->err );
    if (fsDesc == NULL) {
        return -1;
    }; /* if */
        
    if (!wd101FStat( dbfs, fno, &fsStat )) {
        return -1;
    }; /* if */

    switch (fsStat.mode) {
    case DB_DIR:
        bytesRead = wd101_DbReadEntry( dbfs, fsDesc, fno,
                                       &DirectoryEntry, &inode );
        if (bytesRead>0) {
            if (details) {
                wd101_MakeTimestamp( inode.modificationDate,
                                     inode.modificationTime,
                                     modificationTimestamp );
                if (inode.Type == inode_directory) {
                    sprintf( tmpPath,
                             "drwxrwxrwx   1 wa       wa    %10d %s %s\n",
                             inode.Size, modificationTimestamp,
                             DirectoryEntry.name );
                } else if (inode.Type == inode_link) {
                    sprintf( tmpPath,
                             "lrwxrwxrwx   1 wa       wa    %10d %s %s\n",
                             inode.Size, modificationTimestamp,
                             DirectoryEntry.name );
                } else { /* inode_file */
                    sprintf( tmpPath,
                             "-rwxrwxrwx   1 wa       wa    %10d %s %s\n",
                             inode.Size, modificationTimestamp,
                             DirectoryEntry.name );
                }; /* else */
            } else {
                sprintf( tmpPath, "%s",
                         DirectoryEntry.name );
            }; /* else */
        } else {
            tmpPath[0] = '\0';
            wd26SetErr( dbfs->err, ERR_NO_MORE_ENTRIES_WD26,
                        NULL, NULL );
        }; /* else */
        break;
    case DB_FIL:
        return -1;
        break;
    }; /* switch */
    if (wd26GetId( dbfs->err ) != sapdbwa_Error_NoError) {
        sapdbwa_Bool connDown = sapdbwa_False;
        /* check if connection is down */
        wd106CheckConn( fdesc->connection,
                        wd26GetNativeOdbcErr( dbfs->err ),
                        sapdbwa_GetSqlState( dbfs->err ),
                        &connDown );
    }; /* if */

    strncpy( buf, tmpPath, bufLen );
    
    return (wd26GetId( dbfs->err ) == sapdbwa_Error_NoError ? strlen(buf) : -1);
} /* wd101_GetDirEntry */


tsp00_Int4 WINAPI wd101GetDirEntry( twd101DBFSP   dbfs,
                                    twd101DBFile  fno,
                                    char         *buf,
                                    tsp00_Int4    bufLen,
                                    sapdbwa_Bool    details )
{
    tsp00_Int4      bytesRead = 0;

    wd26ResetErr( dbfs->err );
    bytesRead = wd101_GetDirEntry( dbfs, fno, buf, bufLen, details );
    if (bytesRead < 0) {
        /* ROLLBACK */
        twd104FSDescP fsDesc = NULL;
        twd102FileDescP fdesc = NULL;
            
        if (!wd101_GetFileDesc( dbfs, fno, &fdesc )) {
            return -1;
        }; /* if */
        fsDesc = wd101_GetFSDescFromConnection( fdesc->connection,
                                                dbfs->partitionId,
                                                dbfs->err );
        if (fsDesc != NULL) {
            wd104DbTransaction( fsDesc, sapdbwa_False, dbfs->err );
        };
    };

    return bytesRead;
} /* wd101GetDirEntry */


sapdbwa_ErrP WINAPI wd101GetErr( twd101DBFSP dbfs )
{
    return dbfs->err;
} /* wd101GetErr */


tsp00_Uint4 WINAPI wd101GetErrorId( twd101DBFSP dbfs )
{
    return wd26GetId( dbfs->err );
} /* wd101GetErrorId */


const char* WINAPI wd101GetErrorMsg( twd101DBFSP dbfs )
{
    return wd26GetMsg( dbfs->err );
} /* wd101GetErrorMsg */


sapdbwa_Bool wd101_MkDir( twd101DBFSP  dbfs,
                        twd106ConnP  conn,
                        const char  *path )
{
    twd104FSDescP   fsDesc;
    sapdbwa_Bool      retcode = sapdbwa_True;
    twd103Inode     inode;
    twd102FileDesc  fd;
    
    if (!wd101_PathLengthOk( path, dbfs->err )) {
        retcode = sapdbwa_False;
    } else {
        fsDesc = wd101_GetFSDescFromConnection( conn,
                                                dbfs->partitionId,
                                                dbfs->err );
        if (fsDesc == NULL) {
            retcode = sapdbwa_False;
        } else {
            if (wd101_FindInInode( fsDesc, 0,
                                   (char*) path, &inode, dbfs->err )) {
                /* dir already exists */
                wd26SetErr( dbfs->err, ERR_DIR_EXISTS_WD26, path, NULL );
                retcode = sapdbwa_False;
            } else
                if (!wd101_Create( fsDesc,
                                   (char*) path,
                                   NULL,
                                   inode_directory,
                                   &fd,
                                   access_read,
                                   dbfs->err ))
                    retcode = sapdbwa_False;
        }; /* else */
        if (!retcode) {
            sapdbwa_Bool connDown = sapdbwa_False;
            /* check if connection is down */
            wd106CheckConn( conn,
                            wd26GetNativeOdbcErr( dbfs->err ),
                            sapdbwa_GetSqlState( dbfs->err ),
                            &connDown );
        }; /* if */
    }; /* else */
    
    return retcode;
} /* wd101_MkDir */


sapdbwa_Bool WINAPI wd101MkDir( twd101DBFSP dbfs, const char *path )
{
    sapdbwa_Bool   ok = sapdbwa_True;
    twd106ConnP  conn = wd106CreateConn( sapdbwa_False );

    wd26ResetErr( dbfs->err );
    if (conn != NULL) {
        if (!wd106Connect( dbfs->pool, conn, dbfs->err )) {
            ok = sapdbwa_False;
        } else {
            twd104FSDescP fsDesc = NULL;
                
            ok = wd101_MkDir( dbfs, conn, path );
            fsDesc = wd101_GetFSDescFromConnection( conn,
                                                    dbfs->partitionId,
                                                    dbfs->err );
            if (fsDesc != NULL) {
                if (!wd104DbTransaction( fsDesc, ok, dbfs->err )) {
                    ok = sapdbwa_False;
                };
            } else {
                ok = sapdbwa_False;
            };
            wd106Disconnect( conn );
        };
        wd106DestroyConn( conn );
    } else {
        wd26SetErr( dbfs->err, ERR_MEMORY_WD26, NULL, NULL );
        ok = sapdbwa_False;
    };

    return ok;
} /* wd101MkDir */


sapdbwa_Bool wd101_Mv( twd101DBFSP  dbfs,
                     twd106ConnP  conn,
                     const char  *oldPath,
                     const char  *newPath )
{
    twd104FSDescP   fsDesc;
    sapdbwa_Bool      moveOk = sapdbwa_True;
    twd103Inode     oldInode;
    twd103Inode     newParentInode;
    char            oldNameBase[MAX_FILEBASENAME_LEN_WD00];
    char            newNameBase[MAX_FILEBASENAME_LEN_WD00];
    char            newNameDir[MAX_FILENAME_LEN_WD00];
    char           *newName = NULL;
    
    if (!wd101_CmdBasename( oldNameBase,
                            oldPath,
                            DBFS_PATH_SEP_WD101,
                            dbfs->err )
        || !wd101_CmdBasename( newNameBase,
                               newPath,
                               DBFS_PATH_SEP_WD101,
                               dbfs->err )
        || !wd101_IsValidFilename( newNameBase, dbfs->err )
        || !wd101_PathLengthOk( newPath, dbfs->err )) {
        moveOk = sapdbwa_False;
    } else {
        wd101_DirName( newNameDir, sizeof( newNameDir ), newPath );
        /* Get file-system-desc that is used */
        fsDesc = wd101_GetFSDescFromConnection( conn,
                                                dbfs->partitionId,
                                                dbfs->err );
        if (fsDesc == NULL) {
            moveOk = sapdbwa_False;
        } else {
            if (!wd101_FindInInode( fsDesc, NULL,
                                    (char*) oldPath, &oldInode,
                                    dbfs->err )) {
                /* oldName does not exist */
                moveOk = sapdbwa_False;
                /* dont overwrite odbc-error */
                if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                    || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                    wd26SetErr( dbfs->err,
                                ERR_FILE_OR_DIR_NOT_FOUND_WD26,
                                oldPath,
                                NULL );
                }; /* if */
            } else if (wd101_FindInInode( fsDesc,
                                          NULL,
                                          (char*) newPath, &newParentInode,
                                          dbfs->err )) {
                if (newParentInode.Type == inode_file) {
                    /* newName already exists */
                    moveOk = sapdbwa_False;
                    /* dont overwrite odbc-error */
                    if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                        || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                        wd26SetErr( dbfs->err,
                                    ERR_FILE_OR_DIR_EXISTS_WD26,
                                    newName,
                                    NULL );
                    }; /* if */
                } else {
                    /* target is directory keep old file- or dir-name */
                    newName = oldNameBase;
                }; /* else */
            } else {
                if (!wd101_FindInInode( fsDesc,
                                        NULL,
                                        (char*) newNameDir,
                                        &newParentInode,
                                        dbfs->err )
                    || newParentInode.Type == inode_file) {
                    /* target directory does not exist */
                    moveOk = sapdbwa_False;
                    /* dont overwrite odbc-error */
                    if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                        || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                        wd26SetErr( dbfs->err,
                                    ERR_DIR_NOT_FOUND_WD26,
                                    newNameDir,
                                    NULL );
                    }; /* if */
                } else {
                    newName = newNameBase;
                }; /* else */
            }; /* else */
            if (oldInode.Type == inode_directory
                && wd101_IsSubDir( oldPath, newPath )) {
                moveOk = sapdbwa_False;
                /* cant move dir into one of its own sub-directories */
                if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                    || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                    wd26SetErr( dbfs->err,
                                ERR_DESTDIR_IS_SUBDIR_WD26,
                                newPath,
                                oldPath );
                }; /* if */
            }; /* if */                    
            
            if (moveOk) {
                moveOk = wd104MoveInode( fsDesc,
                                         &oldInode.InodeId,
                                         &newParentInode.InodeId,
                                         newName,
                                         dbfs->err );
            }; /* else */
        }; /* else */
        if (!moveOk) {
            sapdbwa_Bool connDown = sapdbwa_False;
            /* check if connection is down */
            wd106CheckConn( conn,
                            wd26GetNativeOdbcErr( dbfs->err ),
                            sapdbwa_GetSqlState( dbfs->err ),
                            &connDown );
        }; /* if */
    }; /* else */
    
    return moveOk;
} /* wd101_Mv */


sapdbwa_Bool WINAPI wd101Mv( twd101DBFSP  dbfs,
                           const char  *oldPath,
                           const char  *newPath )
{
    sapdbwa_Bool   ok = sapdbwa_True;
    twd106ConnP  conn = wd106CreateConn( sapdbwa_False );

    wd26ResetErr( dbfs->err );
    if (conn != NULL) {
        if (!wd106Connect( dbfs->pool, conn, dbfs->err )) {
            ok = sapdbwa_False;
        } else {
            twd104FSDescP fsDesc = NULL;
                
            ok = wd101_Mv( dbfs, conn, oldPath, newPath );
            fsDesc = wd101_GetFSDescFromConnection( conn,
                                                    dbfs->partitionId,
                                                    dbfs->err );
            if (fsDesc != NULL) {
                if (!wd104DbTransaction( fsDesc, ok, dbfs->err )) {
                    ok = sapdbwa_False;
                };
            } else {
                ok = sapdbwa_False;
            };
            wd106Disconnect( conn );
        };
        wd106DestroyConn( conn );
    } else {
        wd26SetErr( dbfs->err, ERR_MEMORY_WD26, NULL, NULL );
        ok = sapdbwa_False;
    };

    return ok;
} /* wd101Mv */


twd101DBFile wd101_CreateFile( twd101DBFSP     dbfs,
                               twd106ConnP     conn,
                               const char     *path,
                               twd101OpenFlag  oflag )
{
    twd102FileDescP  fd = NULL;
    sapdbwa_Bool     ok = sapdbwa_True;
    twd103Inode      Inode;
    twd104FSDescP    fsDesc;
    
    fsDesc = wd101_GetFSDescFromConnection( conn,
                                            dbfs->partitionId,
                                            dbfs->err );
    if (fsDesc == NULL) {
        ok = sapdbwa_False;
    }; /* if */
    if (ok) {
        /* First try to delete existing path */
        if ( wd101_FindInInode( fsDesc, 0,
                                (char*) path, &Inode, dbfs->err )) {
            
            if (Inode.Type != inode_file)
                ok = sapdbwa_False;
            
            if (!wd104DbDeleteInode( fsDesc, &Inode, dbfs->err ))
                ok = sapdbwa_False;
        }
        
        if (!(fd = wd101_GetFreeDesc( dbfs, conn )))
            ok = sapdbwa_False;
        else if (!wd101_Create( fsDesc,
                                (char*) path,
                                NULL,
                                inode_file,
                                fd,
                                oflag,
                                dbfs->err )) {
            wd101_FreeDesc( dbfs, fd );
            fd = NULL;
            ok = sapdbwa_False;
        }; /* else */
    }; /* if */
    
    if (!ok) {
        sapdbwa_Bool connDown = sapdbwa_False;
        
        if (fd) {
            /* check if connection is down */
            wd106CheckConn( conn,
                            wd26GetNativeOdbcErr( dbfs->err ),
                            sapdbwa_GetSqlState( dbfs->err ),
                            &connDown );
        } /* if */
    }; /* if */
    if (ok) {
        fd->connection = conn;
    } else {
        if (fd != NULL) {
            wd101_FreeDesc( dbfs, fd);
        }; /* if */
    }; /* else */
    
    return ( ok ? wd101_FileDesc2No( fd ): FILE_ERROR_WD101 );
} /* wd101_CreateFile */


twd101DBFile WINAPI wd101Create( twd101DBFSP     dbfs,
                                 const char     *path,
                                 twd101OpenFlag  oflag )
{
    sapdbwa_Bool   ok = sapdbwa_True;
    twd106ConnP  conn = wd106CreateConn( sapdbwa_False );
    twd101DBFile fno = FILE_ERROR_WD101;

    wd26ResetErr( dbfs->err );
    if (conn != NULL) {
        if (!wd106Connect( dbfs->pool, conn, dbfs->err )) {
            ok = sapdbwa_False;
            wd106DestroyConn( conn );
        } else {
            twd104FSDescP fsDesc = NULL;
            
            fno = wd101_CreateFile( dbfs, conn, path, oflag );
            ok = (fno != FILE_ERROR_WD101);
            if (!ok) {
                /* ROLLBACK */
                fsDesc = wd101_GetFSDescFromConnection( conn,
                                                        dbfs->partitionId,
                                                        dbfs->err );
                if (fsDesc != NULL) {
                    if (!wd104DbTransaction( fsDesc, sapdbwa_False, dbfs->err )) {
                        ok = sapdbwa_False;
                    };
                };
                wd106Disconnect( conn );
                wd106DestroyConn( conn );
            };
        };
    } else {
        wd26SetErr( dbfs->err, ERR_MEMORY_WD26, NULL, NULL );
        ok = sapdbwa_False;
    };

    return (ok ? fno : FILE_ERROR_WD101);
} /* wd101Create */


sapdbwa_Bool wd101_CreateLink( twd101DBFSP     dbfs,
                             twd106ConnP     conn,
                             const char     *path,
                             const char     *targetPath )
{
    sapdbwa_Bool       ok = sapdbwa_True;
    twd103Inode      targetInode;
    twd104FSDescP    fsDesc;
    
    fsDesc = wd101_GetFSDescFromConnection( conn,
                                            dbfs->partitionId,
                                            dbfs->err );
    if (fsDesc == NULL) {
        ok = sapdbwa_False;
    }; /* if */
    if (ok) {
        /* First search target path */
        if ( !wd101_FindInInode( fsDesc, 0,
                                 (char*) targetPath,
                                 &targetInode,
                                 dbfs->err )) {
            /* target does not exist */
            wd26SetErr( dbfs->err,
                        ERR_FILE_OR_DIR_NOT_FOUND_WD26,
                        targetPath,
                        NULL );                
            ok = sapdbwa_False;
        } else if (!wd101_Create( fsDesc,
                                  (char*) path,
                                  &targetInode.InodeId,
                                  inode_link,
                                  NULL,
                                  0,
                                  dbfs->err )) {
            ok = sapdbwa_False;
        }; /* else */
    }; /* if */        
    if (!ok) {
        sapdbwa_Bool connDown = sapdbwa_False;
        
        /* check if connection is down */
        wd106CheckConn( conn,
                        wd26GetNativeOdbcErr( dbfs->err ),
                        sapdbwa_GetSqlState( dbfs->err ),
                        &connDown );
    } /* if */
 
    return ok;
} /* wd101_CreateLink */


sapdbwa_Bool WINAPI wd101CreateLink( twd101DBFSP     dbfs,
                                   const char     *path,
                                   const char     *targetPath )
{
    sapdbwa_Bool   ok = sapdbwa_True;
    twd106ConnP  conn = wd106CreateConn( sapdbwa_False );

    wd26ResetErr( dbfs->err );
    if (conn != NULL) {
        if (!wd106Connect( dbfs->pool, conn, dbfs->err )) {
            ok = sapdbwa_False;
        } else {
            twd104FSDescP fsDesc = NULL;
                
            ok = wd101_CreateLink( dbfs, conn, path, targetPath );
            fsDesc = wd101_GetFSDescFromConnection( conn,
                                                    dbfs->partitionId,
                                                    dbfs->err );
            if (fsDesc != NULL) {
                if (!wd104DbTransaction( fsDesc, ok, dbfs->err )) {
                    ok = sapdbwa_False;
                };
            } else {
                ok = sapdbwa_False;
            };
            wd106Disconnect( conn );
        };
        wd106DestroyConn( conn );
    } else {
        wd26SetErr( dbfs->err, ERR_MEMORY_WD26, NULL, NULL );
        ok = sapdbwa_False;
    };

    return ok;
} /* wd101CreateLink */


sapdbwa_Bool wd101_FStat( twd101DBFSP		 dbfs,
						  twd101DBFile		 fno,
						  struct st_db_stat	*buf	)
{
    twd102FileDescP fd     = NULL;
    twd104FSDescP   fsDesc = NULL;
    sapdbwa_Bool	ok = sapdbwa_True;

    if (!wd101_GetFileDesc( dbfs, fno, &fd )) {
        ok = sapdbwa_False;
    } else {
        buf->size = 0;
        ok = wd101_SetMode( fd->inode.Type, &buf->mode, dbfs->err );
    }; /* else */

    if (!ok && fd != NULL) {
        /* Get file-system-desc that is used */
        fsDesc = wd101_GetFSDescFromConnection( fd->connection,
                                                dbfs->partitionId,
                                                dbfs->err );
        if (fsDesc == NULL) {
            ok = sapdbwa_False;
        } else {
            sapdbwa_Bool connDown = sapdbwa_False;
            
            /* check if connection is down */
            wd106CheckConn( fd->connection,
                            wd26GetNativeOdbcErr( dbfs->err ),
                            sapdbwa_GetSqlState( dbfs->err ),
                            &connDown );
        }; /* else */
    }; /* if */
    
    return(ok);
} /* wd101_FStat */


sapdbwa_Bool WINAPI wd101FStat( twd101DBFSP			 dbfs,
								twd101DBFile		 fno,
								struct st_db_stat	*buf	)
{
    sapdbwa_Bool ok = sapdbwa_True;

    wd26ResetErr( dbfs->err );
    ok = wd101_FStat( dbfs, fno, buf );
    if (!ok) {
        /* ROLLBACK */
        twd104FSDescP   fsDesc = NULL;
        twd102FileDescP fdesc  = NULL;
            
        if (!wd101_GetFileDesc( dbfs, fno, &fdesc )) {
            return -1;
        }; /* if */            
        fsDesc = wd101_GetFSDescFromConnection( fdesc->connection,
                                                dbfs->partitionId,
                                                dbfs->err );
        if (fsDesc != NULL) {
            wd104DbTransaction( fsDesc, sapdbwa_False, dbfs->err );
        };
    };

    return ok;
} /* wd101FStat */

#define MAX_LINK_DEPTH 1024

twd101DBFile WINAPI wd101_Open( twd101DBFSP     dbfs,
                                twd106ConnP     conn,
                                const char     *Path,
                                twd101OpenFlag  oflag )
{
    twd104FSDescP   fsDesc;
    twd102FileDescP fd;
    sapdbwa_Bool      ok = sapdbwa_True;
    sapdbwa_Bool      openOK = sapdbwa_True;
    
    /* Get file-system-desc that is used */
    fsDesc = wd101_GetFSDescFromConnection( conn,
                                            dbfs->partitionId,
                                            dbfs->err );
    if (fsDesc == NULL) {
        ok = sapdbwa_False;
    } else {
        /* get free file descriptor */
        if (!(fd = wd101_GetFreeDesc( dbfs, conn ))) {
            ok = sapdbwa_False;
        }; /* if */
    }; /* else */
    if (ok) {
        fd->connection = conn;
        if (!wd101_FindInInode( fsDesc, 0,
                                (char*) Path, &fd->inode, dbfs->err )) {
            /* file does not exist or odbc error */
            wd101_FreeDesc( dbfs, fd );
            fd = NULL;
            ok = sapdbwa_False;
            /* dont overwrite odbc-error */
            if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                wd26SetErr( dbfs->err,
                            ERR_FILE_NOT_FOUND_WD26,
                            Path,
                            NULL );
            }; /* if */
        } else {
            tsp00_Int4 loopCount = 0;
            
            while (fd->inode.Type == inode_link && loopCount < MAX_LINK_DEPTH) {
                if (!wd104GetInodeById( fsDesc,
                                        &fd->inode.LinkInodeId,
                                        &fd->inode,
                                        dbfs->err )) {
                    /* target inode does not exist */
                    ok = sapdbwa_False;
                    /* dont overwrite odbc-error */
                    if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                        || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                        wd26SetErr( dbfs->err,
                                    ERR_LINK_TARGET_NOT_FOUND_WD26,
                                    Path,
                                    NULL );
                    };
                };
                loopCount++;
            }; /* while */
            if (ok && loopCount >= MAX_LINK_DEPTH) {
                ok = sapdbwa_False;
                wd26SetErr( dbfs->err,
                            ERR_INTERNAL_WD26,
                            "wd101Open: Link depth exceeded",
                            NULL );
            };
        }; /* else */
    }; /* if */
    
    if (ok) {
        if (fd->inode.Type == inode_file) {
            if (oflag == access_read) {
                fd->access = access_read;
                /* open file for reading */
                openOK = wd104DbOpenContainer( fsDesc,
                                               &fd->inode,
                                               sapdbwa_True,
                                               dbfs->err );
            } else if (oflag == access_write
                       || oflag == access_write_compressed) {
                fd->access = oflag;
                openOK =
                    wd104DbOpenContForUpdate( fsDesc,
                                              &fd->inode,
                                              (sapdbwa_Bool) (oflag
                                                            == access_write_compressed),
                                              sapdbwa_False,
                                              dbfs->err );
                fd->inode.Size = 0; /* file will be overwritten */
            };
            if (!openOK) {
                wd101_FreeDesc( dbfs, fd );
                fd = NULL;
                ok = sapdbwa_False;
            } else {
                fd->inode.ContainerExists = sapdbwa_True;
                fd->filePos = 0;
            }; /* else */
        } else /* (fd->inode.Type == inode_directory) */ {
            wd26SetErr( dbfs->err,
                        ERR_NOT_FILE_WD26,
                        Path,
                        NULL );
            wd101_FreeDesc( dbfs, fd );
            fd = NULL;
            ok = sapdbwa_False;
        }
        
        if (!ok) {
            if ( fd ) {
                wd104DbCloseContainer( fsDesc,
                                       &fd->inode,
                                       dbfs->err );
                wd101_FreeDesc( dbfs, fd );
            }; /* if */
        }; /* if */
    }; /* if */
    if (!ok) {
        sapdbwa_Bool connDown = sapdbwa_False;
        /* check if connection is down */
        wd106CheckConn( conn,
                        wd26GetNativeOdbcErr( dbfs->err ),
                        sapdbwa_GetSqlState( dbfs->err ),
                        &connDown );
    }; /* if */
    
    return (ok ? wd101_FileDesc2No(fd) : FILE_ERROR_WD101);
} /* wd101_Open */


twd101DBFile WINAPI wd101Open( twd101DBFSP     dbfs,
                               const char     *path,
                               twd101OpenFlag  oflag )
{
    twd101DBFile    fno = FILE_ERROR_WD101;
    twd106ConnP     conn = wd106CreateConn( (sapdbwa_Bool)
                                            (oflag != access_read) );

    wd26ResetErr( dbfs->err );
    if (!wd106Connect( dbfs->pool, conn, dbfs->err )) {
        fno = FILE_ERROR_WD101;
        wd106DestroyConn( conn );
    } else {
        fno = wd101_Open( dbfs, conn, path, oflag );
        if (fno == FILE_ERROR_WD101) {
            twd104FSDescP fsDesc = NULL;
            
            fsDesc = wd101_GetFSDescFromConnection( conn,
                                                    dbfs->partitionId,
                                                    dbfs->err );
            if (fsDesc != NULL) {
                wd104DbTransaction( fsDesc, sapdbwa_False, dbfs->err );
            };
            wd106Disconnect( conn );
            wd106DestroyConn( conn );
        };
    };

    return fno;
} /* wd101Open */


twd101DBFile wd101_OpenDir( twd101DBFSP      dbfs,
                            twd106ConnP      conn,
                            const char      *Path,
                            twd101OrderFlag  orderFlag )
{
    twd104FSDescP   fsDesc;
    twd102FileDescP fd;
    sapdbwa_Bool      ok = sapdbwa_True;
    
    /* Get file-system-desc that is used */
    fsDesc = wd101_GetFSDescFromConnection( conn,
                                            dbfs->partitionId,
                                            dbfs->err );
    if (fsDesc == NULL) {
        ok = sapdbwa_False;
    } else {
        /* get free file descriptor */
        if (!(fd = wd101_GetFreeDesc( dbfs, conn ))) {
            ok = sapdbwa_False;
        }; /* if */
    }; /* else */

    if (ok) {
        if (!wd101_FindInInode( fsDesc, 0, (char*) Path,
                                &fd->inode, dbfs->err )) {
            /* dir does not exist */
            wd101_FreeDesc( dbfs, fd );
            fd = NULL;
            ok = sapdbwa_False;
            /* dont overwrite odbc-error */
            if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                wd26SetErr( dbfs->err,
                            ERR_DIR_NOT_FOUND_WD26,
                            Path,
                            NULL );
            }; /* if */
        } else {
            /* dir exists */
            fd->connection = conn;
        }; /* else */
    }; /* if */

    if (ok) {
        fd->access = access_read;
        if (fd->inode.Type == inode_directory) {
            /* open dir for reading */
            if (!wd101_InodeOpen( fsDesc, fd, orderFlag, dbfs->err )) {
                wd101_FreeDesc( dbfs, fd );
                fd = NULL;
                ok = sapdbwa_False;
            }; /* if */
        } else /* (fd->inode.Type == inode_file) */ {
            /* dont want to open file */
            wd101_FreeDesc( dbfs, fd );
            fd = NULL;
            ok = sapdbwa_False;
            wd26SetErr( dbfs->err,
                        ERR_DIR_NOT_FOUND_WD26,
                        Path,
                        NULL );
        }
        
        if (!ok) {
            if ( fd ) {
                wd104DbCloseContainer( fsDesc,
                                       &fd->inode,
                                       dbfs->err );
                wd101_FreeDesc( dbfs, fd );
            }; /* if */
            wd104DbTransaction( fsDesc, ok, dbfs->err );
        }; /* if */
    }; /* if */
    if (!ok) {
        sapdbwa_Bool connDown = sapdbwa_False;
        /* check if connection is down */
        wd106CheckConn( conn,
                        wd26GetNativeOdbcErr( dbfs->err ),
                        sapdbwa_GetSqlState( dbfs->err ),
                        &connDown );
    }; /* if */

    return (ok ? wd101_FileDesc2No(fd) : FILE_ERROR_WD101);
} /* wd101_OpenDir */


twd101DBFile WINAPI wd101OpenDir( twd101DBFSP     dbfs,
                                  const char     *path,
                                  twd101OrderFlag orderFlag )
{
    twd101DBFile    fno = FILE_ERROR_WD101;
    twd106ConnP     conn = wd106CreateConn( sapdbwa_False );

    wd26ResetErr( dbfs->err );
    if (conn != NULL) {
        if (!wd106Connect( dbfs->pool, conn, dbfs->err )) {
            fno = FILE_ERROR_WD101;
            wd106DestroyConn( conn );
        } else {
            fno = wd101_OpenDir( dbfs, conn, path, orderFlag );
            if (fno == FILE_ERROR_WD101) {
                wd106Disconnect( conn );
                wd106DestroyConn( conn );
            };
        };
    } else {
        wd26SetErr( dbfs->err, ERR_MEMORY_WD26, NULL, NULL );
        fno = FILE_ERROR_WD101;
    };
    
    return fno;
} /* wd101OpenDir */


tsp00_Int4 wd101_Read( twd101DBFSP  dbfs,
                       twd101DBFile fno,
                       void        *buf,
                       tsp00_Uint4  nbyte )
{
    twd104FSDescP    fsDesc = NULL;
    twd102FileDescP  fd  = NULL;
    tsp00_Uint4      rtc = 0;
    sapdbwa_Bool       ok  = sapdbwa_True;

    if (!wd101_GetFileDesc( dbfs, fno, &fd )) {
        return sapdbwa_False;
    }; /* if */    
    fsDesc = wd101_GetFSDescFromConnection( fd->connection,
                                            dbfs->partitionId,
                                            dbfs->err );
    if (fsDesc == NULL) {
        ok = sapdbwa_False;
    } else {
        switch (fd->access) {
        case access_read:
        case access_readwrite:
            if (fd->inode.Type == inode_directory)
                /* dont want to read directories */
                ok = sapdbwa_False;
            else
                ok = wd101_FileIO( fsDesc,
                                   &fd->inode,
                                   buf,
                                   nbyte,
                                   io_read,
                                   &rtc,
                                   dbfs->err );
            break;
        default:
            ok = sapdbwa_False;
        }; /* switch */
        if (!ok) {
            sapdbwa_Bool connDown = sapdbwa_False;
            
            /* check if connection is down */
            wd106CheckConn( fd->connection,
                            wd26GetNativeOdbcErr( dbfs->err ),
                            sapdbwa_GetSqlState( dbfs->err ),
                            &connDown );
        }; /* if */
    }; /* else */

    return( ok ? (tsp00_Int4) rtc : FILE_ERROR_WD101);
} /* wd101_Read */


tsp00_Int4 WINAPI wd101Read( twd101DBFSP  dbfs,
                             twd101DBFile fno,
                             void        *buf,
                             tsp00_Uint4  nByte )
{
    tsp00_Int4 bytesRead = 0;

    wd26ResetErr( dbfs->err );
    bytesRead = wd101_Read( dbfs, fno, buf, nByte );
    if (bytesRead < 0) {
        /* ROLLBACK */
        twd104FSDescP fsDesc = NULL;
        twd102FileDescP fdesc = NULL;
            
        if (!wd101_GetFileDesc( dbfs, fno, &fdesc )) {
            return -1;
        }; /* if */            
        fsDesc = wd101_GetFSDescFromConnection( fdesc->connection,
                                                dbfs->partitionId,
                                                dbfs->err );
        if (fsDesc != NULL) {
            wd104DbTransaction( fsDesc, sapdbwa_False, dbfs->err );
        };
    };

    return bytesRead;
} /* wd101Read */

struct dirent* WINAPI wd101ReadDir( twd101DBFSP    dbfs,
                                    twd101DBFile   fno  )
{
    twd104FSDescP    fsDesc;
    twd102FileDescP  fd  = NULL;
    tsp00_Uint4      rtc = 0;
    sapdbwa_Bool       ok  = sapdbwa_True;
    sapdbwa_Bool       noDataFound = sapdbwa_False;
    twd103Inode      inode;
    struct dirent   *dirInfo = NULL;
    
    wd26ResetErr( dbfs->err );
    if (!wd101_GetFileDesc( dbfs, fno, &fd )) {
        return sapdbwa_False;
    }; /* if */
    fsDesc = wd101_GetFSDescFromConnection( fd->connection,
                                            dbfs->partitionId,
                                            dbfs->err );
    if (fsDesc == NULL) {
        ok = sapdbwa_False;
    } else {
        switch (fd->access) {
        case access_read:
        case access_readwrite:
            if (fd->inode.Type == inode_directory) {
                ok = wd101_InodeRead( fsDesc,
                                      fd,
                                      NULL,
                                      sizeof( fd->extraInfo.DirectoryEntry ),
                                      &inode,
                                      &rtc,
                                      dbfs->err );
                noDataFound = !ok;
                if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                    || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                    /* SQLFetch returned SQL_NO_DATA_FOUND */
                    wd26SetErr( dbfs->err, ERR_NO_MORE_ENTRIES_WD26,
                                NULL, NULL );
                    ok = sapdbwa_True;
                };
                if (rtc > 0) {
                    /* ... set fields in dirent */
                    /* !!! temporary solution */
                    strcpy( fd->actDirent.d_name, fd->extraInfo.DirectoryEntry.name );
                }; /* if */
            } else {
                wd26SetErr( dbfs->err, ERR_INVALID_FILE_DESC_WD26,
                            NULL, NULL );
                ok = sapdbwa_False;
            }; /* else */
            break;
        default:
            ok = sapdbwa_False;
        }; /* switch */
        if (!ok) {
            sapdbwa_Bool connDown = sapdbwa_False;
            
            wd104DbTransaction( fsDesc, ok, dbfs->err );
            /* check if connection is down */
            wd106CheckConn( fd->connection,
                            wd26GetNativeOdbcErr( dbfs->err ),
                            sapdbwa_GetSqlState( dbfs->err ),
                            &connDown );
        }; /* if */
    }; /* else */

    return( !noDataFound ? &fd->actDirent : NULL);
} /* wd101ReadDir */


sapdbwa_Bool wd101_Rename( twd101DBFSP  dbfs,
                         twd106ConnP  conn,
                         const char  *oldName,
                         const char  *newName )
{
    twd104FSDescP   fsDesc;
    sapdbwa_Bool      renameOk = sapdbwa_True;
    twd103Inode     inode;
    twd103Inode     dummyInode;
    char            newNameBase[MAX_FILEBASENAME_LEN_WD00];
                                
    if (!wd101_CmdBasename( newNameBase,
                            newName,
                            DBFS_PATH_SEP_WD101,
                            dbfs->err )
        || !wd101_IsValidFilename( newNameBase, dbfs->err )
        || !wd101_CommonPathPrefix( oldName, newName, dbfs->err )
        || !wd101_PathLengthOk( newName, dbfs->err )) {
        renameOk = sapdbwa_False;
    } else {
        /* Get file-system-desc that is used */
        fsDesc = wd101_GetFSDescFromConnection( conn,
                                                dbfs->partitionId,
                                                dbfs->err );
        if (fsDesc == NULL) {
            renameOk = sapdbwa_False;
        } else {
            if (!wd101_FindInInode( fsDesc, NULL, (char*) oldName, &inode,
                                    dbfs->err )) {
                /* oldName does not exist */
                renameOk = sapdbwa_False;
                /* dont overwrite odbc-error */
                if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                    || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                    wd26SetErr( dbfs->err,
                                ERR_FILE_OR_DIR_NOT_FOUND_WD26,
                                oldName,
                                NULL );
                }; /* if */
            } else if (wd101_FindInInode( fsDesc,
                                          NULL,
                                          (char*) newName, &dummyInode,
                                          dbfs->err )) {
                /* newName already exists */
                renameOk = sapdbwa_False;
                /* dont overwrite odbc-error */
                if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                    || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                    wd26SetErr( dbfs->err,
                                ERR_FILE_OR_DIR_EXISTS_WD26,
                                newName,
                                NULL );
                }; /* if */
            } else {
                renameOk = wd104Rename( fsDesc,
                                        &inode,
                                        newNameBase,
                                        dbfs->err );
            }; /* else */
        }; /* else */
        if (!renameOk) {
            sapdbwa_Bool connDown = sapdbwa_False;
            /* check if connection is down */
            wd106CheckConn( conn,
                            wd26GetNativeOdbcErr( dbfs->err ),
                            sapdbwa_GetSqlState( dbfs->err ),
                            &connDown );
        }; /* if */
    }; /* else */
    
    return renameOk;
} /* wd101_Rename */


sapdbwa_Bool WINAPI wd101Rename( twd101DBFSP  dbfs,
                               const char  *oldName,
                               const char  *newName )
{
    sapdbwa_Bool   ok = sapdbwa_True;
    twd106ConnP  conn = wd106CreateConn( sapdbwa_False );

    wd26ResetErr( dbfs->err );
    if (conn != NULL) {
        if (!wd106Connect( dbfs->pool, conn, dbfs->err )) {
            ok = sapdbwa_False;
            wd106DestroyConn( conn );
        } else {
            twd104FSDescP fsDesc = NULL;
                
            ok = wd101_Rename( dbfs, conn, oldName, newName );
            fsDesc = wd101_GetFSDescFromConnection( conn,
                                                    dbfs->partitionId,
                                                    dbfs->err );
            if (fsDesc != NULL) {
                if (!wd104DbTransaction( fsDesc, ok, dbfs->err )) {
                    ok = sapdbwa_False;
                };
            } else {
                ok = sapdbwa_False;
            };
            wd106Disconnect( conn );
        };
        wd106DestroyConn( conn );
    } else {
        wd26SetErr( dbfs->err, ERR_MEMORY_WD26, NULL, NULL );
        ok = sapdbwa_False;
    };

    return ok;
} /* wd101Rename */


sapdbwa_Bool WINAPI wd101Rm( twd101DBFSP  dbfs, const char  *Path )
{
    return wd101_Delete( dbfs, Path, inode_file, sapdbwa_False );
} /* wd101Rm */


sapdbwa_Bool WINAPI wd101RmDir( twd101DBFSP   dbfs,
                                const char   *Path,
                                sapdbwa_Bool  recursive )
{
    sapdbwa_Bool rmDirOK = sapdbwa_True;
    
    rmDirOK = wd101_Delete( dbfs, Path, inode_directory, recursive );

    return rmDirOK;
} /* wd101RmDir */



sapdbwa_Bool WINAPI wd101Stat( twd101DBFSP        dbfs,
                             const char        *path,
                             struct st_db_stat *buf )
{
    return wd101LinkStat( dbfs, path, buf, NULL, 0 );
} /* wd101Stat */


sapdbwa_Bool WINAPI wd101_LinkStat( twd101DBFSP        dbfs,
                                  twd106ConnP        conn,
                                  const char        *path,
                                  struct st_db_stat *buf,
                                  char              *targetBuf,
                                  tsp00_Int4         bufLen )
{
    twd103Inode     inode;
    twd103Inode     linkInode;
    twd104FSDescP   fsDesc;
    sapdbwa_Bool    ok = sapdbwa_True;
    
    /* Get file-system-desc that is used */
    fsDesc = wd101_GetFSDescFromConnection( conn,
                                            dbfs->partitionId,
                                            dbfs->err );
    if (fsDesc == NULL) {
        ok = sapdbwa_False;
    } else {
        if (!wd101_FindInInode( fsDesc, NULL, (char*) path, &inode,
                                dbfs->err )) {
            ok = sapdbwa_False;
            /* dont overwrite odbc-error */
            if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                wd26SetErr( dbfs->err,
                            ERR_FILE_OR_DIR_NOT_FOUND_WD26,
                            path,
                            NULL );
            }; /* if */
        } else {
            buf->size = inode.Size;
            ok = wd101_SetMode( inode.Type, &buf->mode, dbfs->err );
            if (ok && targetBuf != NULL && inode.Type == inode_link) {
                /* get name of link target */
                ok = wd104GetInodeById( fsDesc,
                                        &inode.LinkInodeId,
                                        &linkInode,
                                        dbfs->err );
                if (!ok) {
                    /* target inode does not exist */
                    /* dont overwrite odbc-error */
                    if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                        || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                        wd26SetErr( dbfs->err,
                                    ERR_LINK_TARGET_NOT_FOUND_WD26,
                                    path,
                                    NULL );
                    };
                } else if (strlen( linkInode.Name )+1 > bufLen) {
                    wd26SetErr( dbfs->err,
                                ERR_BUFFER_TOO_SMALL_WD26,
                                linkInode.Name,
                                NULL );
                    ok = sapdbwa_False;
                } else {
                    strncpy( targetBuf, linkInode.Name, bufLen );
                    targetBuf[bufLen-1] = '\0';
                };
                while (ok && !wd103inodeIsNull(&linkInode.ParentDirectoryId)) {
                    ok = wd104GetInodeById( fsDesc,
                                            &linkInode.ParentDirectoryId,
                                            &linkInode,
                                            dbfs->err );
                    if (ok) {
                        ok = wd101_PrependPath( targetBuf, linkInode.Name, bufLen );
                        if (!ok) {
                            wd26SetErr( dbfs->err,
                                        ERR_BUFFER_TOO_SMALL_WD26,
                                        linkInode.Name,
                                        NULL );
                        };
                    };
                };
            }; /* if */
        }; /* else */
    }; /* else */
    if (!ok) {
        sapdbwa_Bool connDown = sapdbwa_False;
        /* check if connection is down */
        wd106CheckConn( conn,
                        wd26GetNativeOdbcErr( dbfs->err ),
                        sapdbwa_GetSqlState( dbfs->err ),
                        &connDown );
    }; /* if */
    
    return (ok);
} /* wd101_LinkStat */


sapdbwa_Bool WINAPI wd101LinkStat( twd101DBFSP        dbfs,
                                 const char        *path,
                                 struct st_db_stat *buf,
                                 char              *targetBuf,
                                 tsp00_Int4         bufLen )
{
    sapdbwa_Bool   ok = sapdbwa_True;
    twd106ConnP  conn = wd106CreateConn( sapdbwa_False );

    wd26ResetErr( dbfs->err );
    if (conn != NULL) {
        if (!wd106Connect( dbfs->pool, conn, dbfs->err )) {
            ok = sapdbwa_False;
        } else {
            twd104FSDescP fsDesc = NULL;
                
            ok = wd101_LinkStat( dbfs, conn, path, buf, targetBuf, bufLen );
            fsDesc = wd101_GetFSDescFromConnection( conn,
                                                    dbfs->partitionId,
                                                    dbfs->err );
            if (fsDesc != NULL) {
                if (!wd104DbTransaction( fsDesc, ok, dbfs->err )) {
                    ok = sapdbwa_False;
                };
            } else {
                ok = sapdbwa_False;
            };
            wd106Disconnect( conn );
        };
        wd106DestroyConn( conn );
    } else {
        wd26SetErr( dbfs->err, ERR_MEMORY_WD26, NULL, NULL );
        ok = sapdbwa_False;
    };

    return ok;
} /* wd101LinkStat */


sapdbwa_Bool WINAPI wd101CStat( twd101DBFSP        dbfs,
                              const char        *path,
                              struct stat       *finfo )
{
    twd103Inode     inode;
    twd104FSDescP   fsDesc;
    sapdbwa_Bool      ok = sapdbwa_True;
    twd106ConnP     conn = wd106CreateConn( sapdbwa_False );
    
    wd26ResetErr( dbfs->err );
    if (!wd106Connect( dbfs->pool, conn, dbfs->err )) {
        ok = sapdbwa_False;
    } else {
        /* Get file-system-desc that is used */
        fsDesc = wd101_GetFSDescFromConnection( conn,
                                                dbfs->partitionId,
                                                dbfs->err );
        if (fsDesc == NULL) {
            ok = sapdbwa_False;
        } else {
            if (!wd101_FindInInode( fsDesc, NULL, (char*) path, &inode,
                                    dbfs->err )) {
                ok = sapdbwa_False;
                /* dont overwrite odbc-error */
                if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                    || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                    wd26SetErr( dbfs->err,
                                ERR_FILE_OR_DIR_NOT_FOUND_WD26,
                                path,
                                NULL );
                }; /* if */
            } else {
                finfo->st_size = inode.Size;
                switch (inode.Type) {
                case inode_directory:
                    finfo->st_mode = S_IFDIR;
                    break;
                case inode_file:
                    finfo->st_mode = S_IFREG;
                    break;
                default:
                    ok = sapdbwa_False;
                }; /* switch */
                finfo->st_dev = 0; /* no devices yet */
                /* other fields in stat ... */
            }; /* else */
            if (!wd104DbTransaction( fsDesc, ok, dbfs->err ))
                ok = sapdbwa_False;
        }; /* else */
        if (!ok) {
            sapdbwa_Bool connDown = sapdbwa_False;
            /* check if connection is down */
            wd106CheckConn( conn,
                            wd26GetNativeOdbcErr( dbfs->err ),
                            sapdbwa_GetSqlState( dbfs->err ),
                            &connDown );
        }; /* if */
        wd106Disconnect( conn );
    }; /* else */
    
    return (ok);
} /* wd101CStat */


tsp00_Int4 wd101_Write( twd101DBFSP   dbfs,
                        twd101DBFile  fno,
                        void         *buf,
                        tsp00_Uint4   nbyte )
{
    twd102FileDescP fd = NULL;
    twd104FSDescP   fsDesc;
    tsp00_Uint4     Rtc = 0;
    sapdbwa_Bool      ok = sapdbwa_False;

    if (!wd101_GetFileDesc( dbfs, fno, &fd )) {
        return sapdbwa_False;
    }; /* if */
    
    fsDesc = wd101_GetFSDescFromConnection( fd->connection,
                                            dbfs->partitionId,
                                            dbfs->err );
    if (fsDesc == NULL) {
        ok = sapdbwa_False;
    } else {
        switch (fd->access) {
        case access_write_compressed:
            ok = wd105AddToBuf( fd->writeComprBuf, buf, nbyte );
            Rtc = nbyte;
            break;
        case access_write:
        case access_readwrite:
            ok = wd101_FileIO( fsDesc,
                               &fd->inode,
                               buf,
                               nbyte,
                               io_write,
                               &Rtc,
                               dbfs->err );
            break;
        default:
            ok = sapdbwa_False;
        }; /* switch */
        if (!ok) {
            sapdbwa_Bool connDown = sapdbwa_False;
            
            /* check if connection is down */
            wd106CheckConn( fd->connection,
                            wd26GetNativeOdbcErr( dbfs->err ),
                            sapdbwa_GetSqlState( dbfs->err ),
                            &connDown );
        }; /* if */    
    }; /* else */

    return ( ok ? Rtc : FILE_ERROR_WD101 );
} /* wd101_Write */


tsp00_Int4 WINAPI wd101Write( twd101DBFSP   dbfs,
                              twd101DBFile  fno,
                              void         *buf,
                              tsp00_Uint4   nByte )
{
    tsp00_Int4 bytesWritten = 0;

    wd26ResetErr( dbfs->err );
    bytesWritten = wd101_Write( dbfs, fno, buf, nByte );
    if (bytesWritten < 0) {
        /* ROLLBACK */
        twd104FSDescP fsDesc = NULL;
        twd102FileDescP fdesc = NULL;
            
        if (!wd101_GetFileDesc( dbfs, fno, &fdesc )) {
            return -1;
        }; /* if */            
        fsDesc = wd101_GetFSDescFromConnection( fdesc->connection,
                                                dbfs->partitionId,
                                                dbfs->err );
        if (fsDesc != NULL) {
            wd104DbTransaction( fsDesc, sapdbwa_False, dbfs->err );
        };
    };

    return bytesWritten;
} /* wd101Write */


/************************************************************************
 * transaction handling
 ************************************************************************/

struct st_transaction
{
    twd106ConnP      conn; /* connection used to access db */    
}; /* struct st_transaction */


twd101TransactionP WINAPI wd101StartTransaction( twd101DBFSP   dbfs )
{
    twd101TransactionP newTrans = NULL;
    sapdbwa_Bool         allocatOk = sapdbwa_True;
    
    wd26ResetErr( dbfs->err );
    sapdbwa_SQLALLOCAT( sizeof( struct st_transaction ),
                   (tsp00_Uint1**) newTrans,
                   &allocatOk );
    if (allocatOk) {
        /* get connection */
        newTrans->conn = wd106CreateConn( dbfs->exclusiveTransactions );
        if (newTrans->conn == NULL) {
            sapdbwa_SQLFREE( (tsp00_Uint1*) newTrans );
            newTrans = NULL;
        };
    };

    return newTrans;
} /* wd101StartTransaction */


sapdbwa_Bool WINAPI wd101EndTransaction( twd101DBFSP         dbfs,
                                       twd101TransactionP  trans,
                                       sapdbwa_Bool          commit )
{
    sapdbwa_Bool     ok     = sapdbwa_True;
    twd104FSDescP  fsDesc = NULL;

    wd26ResetErr( dbfs->err );
    fsDesc = wd101_GetFSDescFromConnection( trans->conn,
                                            dbfs->partitionId,
                                            dbfs->err );
    if (fsDesc != NULL) {
        if (wd104DbTransaction( fsDesc, commit, dbfs->err )) {
            /* give connection back to pool */
            wd106Disconnect( trans->conn );
            sapdbwa_SQLFREE( (tsp00_Uint1*) trans );
        } else {
            ok = sapdbwa_False;
        };
    } else {
        ok = sapdbwa_False;
    };
    
    return ok;
} /* wd101EndTransaction */


sapdbwa_Bool WINAPI wd101TrClose( twd101DBFSP        dbfs,
                                twd101TransactionP trans,
                                twd101DBFile       fno )
{
    sapdbwa_Bool       ok = sapdbwa_True;
    twd102FileDescP  fd = NULL;
    
    wd26ResetErr( dbfs->err );
    if (wd101_GetFileDesc( dbfs, fno, &fd )) {
        ok = wd101_CloseInternal( dbfs, fd );
    } else {
        ok = sapdbwa_False;
    };

    return ok;
} /* wd101TrClose */


sapdbwa_Bool WINAPI wd101TrCloseDir( twd101DBFSP         dbfs,
                                   twd101TransactionP  trans,
                                   twd101DBFile        fno )
{
    sapdbwa_Bool       ok = sapdbwa_True;
    twd102FileDescP  fd = NULL;
    
    wd26ResetErr( dbfs->err );
    if (wd101_GetFileDesc( dbfs, fno, &fd )) {
        ok = wd101_CloseDir( dbfs, fd );
    } else {
        ok = sapdbwa_False;
    };

    return ok;
} /* wd101TrCloseDir */

    
sapdbwa_Bool WINAPI wd101TrCopy( twd101DBFSP         dbfs,
                               twd101TransactionP  trans,
                               const char         *sourcePath,
                               const char         *destPath )
{
    wd26ResetErr( dbfs->err );
    return wd101_Copy( dbfs, trans->conn, sourcePath, destPath, sapdbwa_False );
} /* wd101TrCopy */


    
sapdbwa_Bool WINAPI wd101TrCopyWithLinks( twd101DBFSP         dbfs,
                                        twd101TransactionP  trans,
                                        const char         *sourcePath,
                                        const char         *destPath )
{
    wd26ResetErr( dbfs->err );
    return wd101_Copy( dbfs, trans->conn, sourcePath, destPath, sapdbwa_True );
} /* wd101TrCopyWithLinks */


twd101DBFile WINAPI wd101TrCreate( twd101DBFSP         dbfs,
                                   twd101TransactionP  trans,
                                   const char         *path,
                                   twd101OpenFlag      oflag )
{
    wd26ResetErr( dbfs->err );
    return wd101_CreateFile( dbfs, trans->conn, path, oflag );
} /* wd101TrCreate */


sapdbwa_Bool WINAPI wd101TrCreateLink( twd101DBFSP         dbfs,
                                     twd101TransactionP  trans,
                                     const char         *path,
                                     const char         *targetPath )
{
    wd26ResetErr( dbfs->err );
    return wd101_CreateLink( dbfs, trans->conn, path, targetPath );    
} /* wd101TrCreateLink */


sapdbwa_Bool WINAPI wd101TrDelete( twd101DBFSP         dbfs,
                                 twd101TransactionP  trans,
                                 const char         *path )
{
    wd26ResetErr( dbfs->err );
    return wd101_DeleteInode( dbfs, trans->conn, path,
                              0 /* any type */, sapdbwa_False );
} /* wd101TrDelete */


sapdbwa_Bool WINAPI wd101TrFStat( twd101DBFSP         dbfs,
                                twd101TransactionP  trans,
                                twd101DBFile        fno,
                                struct st_db_stat  *buf )
{
    wd26ResetErr( dbfs->err );
    return wd101_FStat( dbfs, fno, buf );
} /* wd101TrFStat */


tsp00_Int4 WINAPI wd101TrGetDirEntry( twd101DBFSP         dbfs,
                                      twd101TransactionP  trans,
                                      twd101DBFile        fno,
                                      char               *buf,
                                      tsp00_Int4          bufLen,
                                      sapdbwa_Bool          details )
{
    wd26ResetErr( dbfs->err );
    return wd101_GetDirEntry( dbfs, fno, buf, bufLen, details );
} /* wd101TrGetDirEntry */


sapdbwa_Bool WINAPI wd101TrMkDir( twd101DBFSP         dbfs,
                                twd101TransactionP  trans,
                                const char         *path )
{
    wd26ResetErr( dbfs->err );
    return wd101_MkDir( dbfs, trans->conn, path );
} /* wd101TrMkDir */

    
sapdbwa_Bool WINAPI wd101TrMv( twd101DBFSP         dbfs,
                             twd101TransactionP  trans,
                             const char         *oldPath,
                             const char         *newPath )
{
    wd26ResetErr( dbfs->err );
    return wd101_Mv( dbfs, trans->conn, oldPath, newPath );
} /* wd101TrMv */

    
twd101DBFile WINAPI wd101TrOpen( twd101DBFSP         dbfs,
                                 twd101TransactionP  trans,
                                 const char         *path,
                                 twd101OpenFlag      oflag )
{
    wd26ResetErr( dbfs->err );
    return wd101_Open( dbfs, trans->conn, path, oflag );
} /* wd101TrOpen */


twd101DBFile WINAPI wd101TrOpenDir( twd101DBFSP         dbfs,
                                    twd101TransactionP  trans,
                                    const char         *path,
                                    twd101OrderFlag     orderFlag )
{
    wd26ResetErr( dbfs->err );
    return wd101_OpenDir( dbfs, trans->conn, path, orderFlag );
} /* wd101TrOpenDir */


tsp00_Int4 WINAPI wd101TrRead( twd101DBFSP         dbfs,
                               twd101TransactionP  trans,
                               twd101DBFile        fno,
                               void               *buf,
                               tsp00_Uint4         nByte )
{
    wd26ResetErr( dbfs->err );
    return wd101_Read( dbfs, fno, buf, nByte );
} /* wd101TrRead */

sapdbwa_Bool WINAPI wd101TrRename( twd101DBFSP         dbfs,
                                 twd101TransactionP  trans,
                                 const char         *oldName,
                                 const char         *newName )
{
    wd26ResetErr( dbfs->err );
    return wd101_Rename( dbfs, trans->conn, oldName, newName );
} /* wd101TrRename */

    
sapdbwa_Bool WINAPI wd101TrRm( twd101DBFSP         dbfs,
                             twd101TransactionP  trans,
                             const char         *path )
{
    wd26ResetErr( dbfs->err );
    return wd101_DeleteInode( dbfs, trans->conn, path, inode_file, sapdbwa_False );
} /* wd101TrRm */

    
sapdbwa_Bool WINAPI wd101TrRmDir( twd101DBFSP         dbfs,
                                twd101TransactionP  trans,
                                const char         *path,
                                sapdbwa_Bool          recursive )
{
    wd26ResetErr( dbfs->err );
    return wd101_DeleteInode( dbfs, trans->conn, path, inode_directory, recursive );
} /* wd101TrRmDir */


sapdbwa_Bool WINAPI wd101TrStat( twd101DBFSP         dbfs,
                               twd101TransactionP  trans,
                               const char         *path,
                               struct st_db_stat  *buf )
{
    wd26ResetErr( dbfs->err );
    return wd101_LinkStat( dbfs, trans->conn, path, buf, NULL, 0 );
} /* wd101TrStat */

    
sapdbwa_Bool WINAPI wd101TrLinkStat( twd101DBFSP         dbfs,
                                   twd101TransactionP  trans,
                                   const char         *path,
                                   struct st_db_stat  *buf,
                                   char               *targetBuf,
                                   tsp00_Int4          bufLen )
{
    wd26ResetErr( dbfs->err );
    return wd101_LinkStat( dbfs, trans->conn, path, buf, targetBuf, bufLen );
} /* wd101TrLinkStat */


tsp00_Int4 WINAPI wd101TrWrite( twd101DBFSP         dbfs,
                                twd101TransactionP  trans,
                                twd101DBFile        fno,
                                void               *buf,
                                tsp00_Uint4         nByte )
{
    wd26ResetErr( dbfs->err );
    return wd101_Write( dbfs, fno, buf, nByte );
} /* wd101TrWrite */

/*!**********************************************************************

  EndChapter: twd101DBFS

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

/* files with these extensions are never compressed */
static char *wd101DontCompressExt[] =
{ "exe",
  "gif", "png", "mpeg", "jpeg", "mpg", "jpg", "tiff", "tif",
  "avi", "fli", "mp3",
  "gz", "zip", "arc", "lha", "car", "tgz", "z",
  NULL };


sapdbwa_Bool wd101_AllowCompression( const char *path )
{
    sapdbwa_Bool allowCompression = sapdbwa_True;
    const char *ext = &path[strlen(path)];

    while (--ext >= path && *ext != '.') {};
    
    if (ext>=path && strlen(ext) > 0) {
        tsp00_Int4 i;
        ext++; /* skip '.' */
        for ( i = 0; wd101DontCompressExt[i] != NULL; i++ ) {
            if (stricmp( wd101DontCompressExt[i], ext ) == 0) {
                allowCompression = sapdbwa_False;
                break;
            }; /* if */
        }; /* for */
    }; /* if */

    return allowCompression;
} /* wd101_AllowCompression */


sapdbwa_Bool wd101_CloseInternal( twd101DBFSP dbfs, twd102FileDescP fd )
{
    sapdbwa_Bool      ok = sapdbwa_True;
    twd104FSDescP   fsDesc;

    fsDesc = wd101_GetFSDescFromConnection( fd->connection,
                                            dbfs->partitionId,
                                            dbfs->err );
    if (fsDesc == NULL) {
        ok = sapdbwa_False;
    } else {
        if (fd->access == access_write_compressed) {
            tsp00_Uint4  bytesWritten = 0;
            
            ok = wd104DbWriteCompressed( fsDesc, &fd->inode,
                                         fd->writeComprBuf,
                                         &bytesWritten,
                                         dbfs->err );
            wd105FreeBuf( fd->writeComprBuf );
        }; /* if */
        if (fd->inode.ContainerExists)
            if (!wd104DbCloseContainer( fsDesc,
                                        &fd->inode,
                                        dbfs->err ))
                ok = sapdbwa_False;
        
        if (!ok) {
            sapdbwa_Bool connDown = sapdbwa_False;
            /* check if connection is down */
            wd106CheckConn( fd->connection,
                            wd26GetNativeOdbcErr( dbfs->err ),
                            sapdbwa_GetSqlState( dbfs->err ),
                            &connDown );
        }; /* if */
        fd->access = no_access; /* file is not opened anymore */
        wd101_FreeDesc( dbfs, fd );
    }; /* else */
    
    return (ok);
} /* wd101_CloseInternal */

    /* Copy the string after the last 'PathSeparator' in 'Path' into
     * 'Basename'.
     * If there is no 'PathSeparator' then copy 'Path'.
     * Also check if filename is valid.
     */
sapdbwa_Bool wd101_CmdBasename( char         Basename[],
                              const char   Path[],
                              char         PathSeparator,
                              twd26ErrP    err )
{
    sapdbwa_Bool  ok = sapdbwa_True;
    
    char *tmp = strrchr(Path, PathSeparator);
    if (tmp) {
        tmp++; /* skip SEP */
    } else {
        tmp = (char*) Path;
    }; /* else */
    
    if (strlen(tmp) > MAX_FILENAME_LEN_WD00) {
        /* too long */
        wd26SetErr( err, ERR_FILENAME_TOO_LONG_WD26, tmp, NULL );
        ok = sapdbwa_False;
    } else {
        strcpy( Basename, tmp );
    }; /* else */

    return ok;
} /* wd101_CmdBaseName */


sapdbwa_Bool wd101_Copy( twd101DBFSP     dbfs,
                       twd106ConnP     conn,
                       const char     *sourcePath,
                       const char     *destPath,
                       sapdbwa_Bool      withLinks )
{
    twd104FSDescP   fsDesc;
    sapdbwa_Bool      copyOk = sapdbwa_True;
    twd103Inode     sourceInode;
    twd103Inode     newParentInode;
    char            sourceNameBase[MAX_FILEBASENAME_LEN_WD00];
    char            newNameBase[MAX_FILEBASENAME_LEN_WD00];
    char            newNameDir[MAX_FILENAME_LEN_WD00];
    char           *newName = NULL;
    
    wd26ResetErr( dbfs->err );
    if (!wd101_CmdBasename( sourceNameBase,
                            sourcePath,
                            DBFS_PATH_SEP_WD101,
                            dbfs->err )
        || !wd101_CmdBasename( newNameBase,
                               destPath,
                               DBFS_PATH_SEP_WD101,
                               dbfs->err )
        || !wd101_IsValidFilename( newNameBase, dbfs->err )) {
        copyOk = sapdbwa_False;
    } else {
        wd101_DirName( newNameDir, sizeof( newNameDir ), destPath );
        /* Get file-system-desc that is used */
        fsDesc = wd101_GetFSDescFromConnection( conn,
                                                dbfs->partitionId,
                                                dbfs->err );
        if (fsDesc == NULL) {
            copyOk = sapdbwa_False;
        } else {
            if (!wd101_FindInInode( fsDesc, NULL,
                                    (char*) sourcePath, &sourceInode,
                                    dbfs->err )) {
                /* oldName does not exist */
                copyOk = sapdbwa_False;
                /* dont overwrite odbc-error */
                if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                    || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                    wd26SetErr( dbfs->err,
                                ERR_FILE_OR_DIR_NOT_FOUND_WD26,
                                sourcePath,
                                NULL );
                }; /* if */
            } else if (wd101_FindInInode( fsDesc,
                                          NULL,
                                          (char*) destPath,
                                          &newParentInode,
                                          dbfs->err )) {
                if (newParentInode.Type == inode_file) {
                    /* newName already exists */
                    copyOk = sapdbwa_False;
                    /* dont overwrite odbc-error */
                    if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                        || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                        wd26SetErr( dbfs->err,
                                    ERR_FILE_OR_DIR_EXISTS_WD26,
                                    destPath,
                                    NULL );
                    }; /* if */
                } else {
                    /* target is directory keep old file- or dir-name */
                    newName = sourceNameBase;
                }; /* else */
            } else if (!wd101_FindInInode( fsDesc,
                                           NULL,
                                           (char*) newNameDir,
                                           &newParentInode,
                                           dbfs->err )
                       || newParentInode.Type == inode_file) {
                /* target directory does not exist */
                copyOk = sapdbwa_False;
                /* dont overwrite odbc-error */
                if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                    || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                    wd26SetErr( dbfs->err,
                                ERR_DIR_NOT_FOUND_WD26,
                                newNameDir,
                                NULL );
                }; /* if */
            } else {
                newName = newNameBase;
            }; /* else */
            
            if (sourceInode.Type == inode_directory
                && wd101_IsSubDir( sourcePath, destPath )) {
                copyOk = sapdbwa_False;
                /* cant move dir into one of its own sub-directories */
                if (wd26GetId( dbfs->err ) == ERR_NO_ERROR_WD26
                    || wd26GetId( dbfs->err ) == ERR_UNKNOWN_WD26) {
                    wd26SetErr( dbfs->err,
                                ERR_DESTDIR_IS_SUBDIR_WD26,
                                destPath,
                                sourcePath );
                }; /* if */
            }; /* if */
                
            if (copyOk) {
                copyOk = wd104Copy( fsDesc,
                                    &sourceInode,
                                    &newParentInode,
                                    newName,
                                    withLinks,
                                    dbfs->err );
            }; /* else */
        }; /* else */
        if (!copyOk) {
            sapdbwa_Bool connDown = sapdbwa_False;
            /* check if connection is down */
            wd106CheckConn( conn,
                            wd26GetNativeOdbcErr( dbfs->err ),
                            sapdbwa_GetSqlState( dbfs->err ),
                            &connDown );
        }; /* if */
    }; /* else */
    
    return copyOk;
} /* wd101_Copy */


sapdbwa_Bool wd101_Create( twd104FSDescP    fsDesc,
                         char            *Path,
                         twd103InodeId   *linkInode,
                         twd103InodeType  InodeType,
                         twd102FileDescP  fd,
                         twd101OpenFlag   oflag,
                         twd26ErrP        err )
{
    char        Path2[MAX_FILENAME_LEN_WD00];
    char        Postfix[MAX_FILENAME_LEN_WD00];
    tsp00_Int2  PrefixLength, Pathlength;
    twd103Inode ParentInode;
    sapdbwa_Bool  compress;
    sapdbwa_Bool  insertOK;
    
    if (0 == (Pathlength = strlen(Path))) {
        wd26SetErr( err, ERR_INVALID_FILENAME_WD26, Path, NULL );
        return(sapdbwa_False);
    }; /* if */
    
    /* Only directory paths may end with '/'. */
    if ( InodeType != inode_directory
         && Path[Pathlength-1] == DBFS_PATH_SEP_WD101) {
        return(sapdbwa_False);
    }; /* if */
    
    if (!wd101_PathBasename( Path, &PrefixLength, Postfix, err )) {
        return(sapdbwa_False);
    }; /* if */
    
    if (PrefixLength > 0) {
        strncpy( Path2, Path, PrefixLength );
        Path2[PrefixLength] = '\0';
    }
    else
        strcpy(Path2, ".");

    if (!wd101_FindInInode( fsDesc, 0, Path2, &ParentInode, err )) {
        wd26SetErr( err, ERR_DIR_NOT_FOUND_WD26, Path2, NULL );        
        return(sapdbwa_False);
    }; /* if */
    
    if (ParentInode.Type != inode_directory && ParentInode.Type != no_inode) {
        /* cannot create subdir of a file */
        wd26SetErr( err, ERR_SUBDIR_NOT_FOUND_WD26, Path, NULL );        
        return(sapdbwa_False);
    }; /* if */
    
    if (!(wd104DbInsertInode( fsDesc,
                              &ParentInode.InodeId,
                              linkInode,
                              Postfix,
                              InodeType,
                              &fd->inode,
                              err )))
        return(sapdbwa_False);

    if (InodeType == inode_directory) {
        fd->inode.ContainerExists = sapdbwa_False;
        fd->access = access_read;
    } else if (InodeType == inode_file) {
        if (oflag == access_write_compressed
            && !wd101_AllowCompression(Path)) {
            /* some files-types should not be compressed */
            oflag = access_write;
        }; /* if */
        compress = (oflag == access_write_compressed);
        insertOK = wd104InsertContainer( fsDesc, &fd->inode,
                                         compress, sapdbwa_False, err );
        if (!insertOK)
            return (sapdbwa_False);
        fd->inode.ContainerExists = sapdbwa_True;
        fd->inode.Size = 0; /* just created so size is 0 */
        fd->access = oflag;
    }

    return(sapdbwa_True);
} /* wd101_Create */


tsp00_Int4 wd101_DbReadEntry( twd101DBFSP          dbfs,
                              twd104FSDescP        fsDesc,
                              twd101DBFile         fno,
                              struct st_db_direct *DirectoryEntry,
                              twd103Inode         *inode )
{
    tsp00_Uint4 rtc1 = 0;
    tsp00_Uint4 rtc2 = 0;
    twd102FileDescP fdesc = NULL;

    if (!wd101_GetFileDesc( dbfs, fno, &fdesc )) {
        return sapdbwa_False;
    }; /* if */
    if (!wd101_InodeRead( fsDesc,
                          fdesc,
                          DirectoryEntry,
                          sizeof( *DirectoryEntry ),
                          inode,
                          &rtc1,
                          dbfs->err )) {
        return FILE_ERROR_WD101;
    }; /* if */
    return rtc1+rtc2;
} /* wd101_DbReadEntry */


sapdbwa_Bool wd101_DestroyFSDesc( void *fsdescHandle )
{
    twd104FSDescP fsdesc = (twd104FSDescP) fsdescHandle;

    return wd104DestroyFSDesc( fsdesc );
} /* wd101_DestroyFSDesc */


sapdbwa_Bool wd101_DeleteInode( twd101DBFSP      dbfs,
                                twd106ConnP      conn,
                                const char      *Path,
                                twd103InodeType  type,
                                sapdbwa_Bool     recursive )
{
    sapdbwa_Bool    ok = sapdbwa_True;
    twd104FSDescP fsDesc;

    fsDesc = wd101_GetFSDescFromConnection( conn,
                                            dbfs->partitionId,
                                            dbfs->err );
    if (fsDesc == NULL) {
        ok = sapdbwa_False;
    } else {
        ok = wd101_Remove( fsDesc, (char*) Path, type, recursive,
                           dbfs->err );
    }; /* else */
    if (!ok) {
        sapdbwa_Bool connDown = sapdbwa_False;
        /* check if connection is down */
        wd106CheckConn( conn,
                        wd26GetNativeOdbcErr( dbfs->err ),
                        sapdbwa_GetSqlState( dbfs->err ),
                        &connDown );
    }; /* if */
    
    return (ok);
} /* wd101_DeleteInode */


sapdbwa_Bool wd101_FindInInode( twd104FSDescP  fsDesc,
                              twd103InodeId *Directory,
                              char          *Path,
                              twd103Inode   *Inode,
                              twd26ErrP      err )
{
    char            Name[MAX_FILEBASENAME_LEN_WD00];
    char           *cp;
    twd103InodeId   lDirectory;

    if (Directory == NULL && wd101_IsRoot( Path )) {
        /* the root path '/' always exists */
        wd103InitInode( Inode );
        Inode->Type = inode_directory;
        return sapdbwa_True;
    }; /* if */
    
    if (Path == NULL)
        return(sapdbwa_False);
    if (strlen(Path) > MAX_FILENAME_LEN_WD00) {
        wd26SetErr( err, ERR_PATH_TOO_LONG_WD26, Path, NULL );
        return sapdbwa_False;
    }; /* if */
    if (*Path == DBFS_PATH_SEP_WD101) {		/* Skip leading / */
        Path++;
        if (*Path == '\0')
            Path = "."; /* Replace with toplevel directory . */
    }; /* if */

    if (Directory)
        SAPDB_memcpy( &lDirectory, Directory, sizeof( twd103InodeId ) );
    else
        wd103inodeidInit(lDirectory);

    while (strlen(Path) > 0) {
        if ((cp = strchr(Path, DBFS_PATH_SEP_WD101))) {
            if ((tsp00_Int4)(cp-Path) > MAX_FILEBASENAME_LEN_WD00) {
                wd26SetErr( err, ERR_FILENAME_TOO_LONG_WD26, Path, NULL );
                return sapdbwa_False;
            }; /* if */
            strncpy(Name, Path, (tsp00_Int4)(cp-Path));
            Name[cp-Path] = '\0';
            Path = cp + 1;
        } else {
            if (strlen(Path) > MAX_FILEBASENAME_LEN_WD00) {
                wd26SetErr( err, ERR_FILENAME_TOO_LONG_WD26, Path, NULL );
                return sapdbwa_False;
            }; /* if */
            strcpy(Name, Path);
            Path += strlen(Path);
        }

        if (Name[strlen(Name)-1] == DBFS_PATH_SEP_WD101)/* remove trailing / */
            Name[strlen(Name)-2] = '\0';

        if (!wd104DbGetInodeByName( fsDesc, &lDirectory, Name, Inode, err )) {
            return (sapdbwa_False);
        }; /* if */
        
        SAPDB_memcpy( &lDirectory, &Inode->InodeId, sizeof( twd103InodeId ) );
    }; /* while */

    return(sapdbwa_True);
} /* wd101_FindInInode */


void wd101_FreeDescArray( twd101DBFSP dbfs )
{
    twd101DBFile     iDesc;
    twd102FileDescP  fdesc  = NULL;
    
    for ( iDesc=0;
          iDesc < wd35GetNumAllocElems( dbfs->fileDescPool );
          iDesc++) {
        wd35GetElemByIndex( dbfs->fileDescPool, iDesc, (void**) &fdesc );
        if (fdesc != NULL) {
            wd102DestroyFileDesc( fdesc );
        }; /* if */
    }; /* for */
} /* wd101_FreeDescArray */


sapdbwa_Bool wd101_FileIO( twd104FSDescP      fsDesc,
                         twd103InodeP       Inode,
                         void              *buf,
                         tsp00_Uint4        nbyte,
                         enum en_io         ioFlag,
                         tsp00_Uint4       *Rtc,
                         twd26ErrP          err )
{
    switch (ioFlag) {
    case io_read:
        if (!Inode->ContainerExists) {
            *Rtc = 0;
            return (sapdbwa_True);
        }
        else if (!wd104DbReadContainer( fsDesc, Inode,
                                        buf, nbyte, Rtc, err ))
            return (sapdbwa_False);
        break;
    case io_write:
        if (!wd104DbWriteContainer( fsDesc, Inode,
                                    buf, nbyte, Rtc, err ))
            return (sapdbwa_False);
        break;
    default:
        return(sapdbwa_False);
    }

    return(sapdbwa_True);
} /* wd101_FileIO */


sapdbwa_Bool wd101_GetFileDesc( twd101DBFSP      dbfs,
								twd101DBFile     fno,
								twd102FileDescP *fdesc )
{
    sapdbwa_Bool retcode = sapdbwa_True;
    
    if (!wd35GetElemByIndex( dbfs->fileDescPool, fno, (void**) fdesc )) {
        wd26SetErr( dbfs->err, ERR_INVALID_FILE_DESC_WD26, NULL, NULL );
        retcode = sapdbwa_False;
    } else if ((*fdesc)->access == no_access) {
        wd26SetErr( dbfs->err, ERR_INVALID_FILE_DESC_WD26, NULL, NULL );
        *fdesc = NULL;
        retcode = sapdbwa_False;
    }; /* else */        

    return retcode;
} /* wd101_GetFileDesc */


twd104FSDescP wd101_GetFSDescFromConnection( twd106ConnP	hconn,
											 sapdbwa_UInt4	partitionId,
                                             twd26ErrP		err			)
{
    twd104FSDescP fsDesc = NULL;
    SQLHDBC       hdbc;
    SQLHENV       henv;

    if (!wd106OdbcHandle( hconn, &henv, &hdbc )) {
        wd26SetErr( err, ERR_CANT_GET_ODBC_HANDLES_WD26, NULL, NULL );
    } else {
        fsDesc = (twd104FSDescP) wd106GetUserData( hconn );
        if (fsDesc == NULL) {
            fsDesc = wd104CreateFSDesc( henv, hdbc,
                                        wd106GetUsername( hconn ), partitionId,
                                        sapdbwa_GetSchemaInfo(wd106GetDBCCon(hconn)),
                                        sapdbwa_GetTableContainer(wd106GetDBCCon(hconn)),
                                        sapdbwa_GetTableInode(wd106GetDBCCon(hconn)),
                                        err );
            if (fsDesc != NULL) {
                if (!wd106SetUserData( hconn, fsDesc, wd101_DestroyFSDesc )) {
                    wd104DestroyFSDesc( fsDesc );
                    fsDesc = NULL;
                    wd26SetErr( err, ERR_INTERNAL_WD26,
                                "wd101_GetFSDescFromConnection", NULL );
                }; /* if */
            }; /* if */
        }; /* if */
    }; /* else */
    
    return fsDesc;
} /* wd101_GetFSDescFromConnection */


twd102FileDescP wd101_GetFreeDesc( twd101DBFSP  dbfs,
                                   twd106ConnP  conn )
{
    twd101DBFile      freeDescIndex;
    twd102FileDescP   freeDesc;
    
    if (wd35LockElem( dbfs->fileDescPool,
                      (void**) &freeDesc,
                      &freeDescIndex )) {
        if (freeDesc == NULL) {
            /* create new file descriptor */
            freeDesc = wd102CreateFileDesc( freeDescIndex );
            if (freeDesc != NULL) {
                wd35SetElemByIndex( dbfs->fileDescPool,
                                    freeDesc,
                                    freeDescIndex );
				freeDesc->index = freeDescIndex;
            } else {
                /* creation failed */
                wd35UnlockElem(  dbfs->fileDescPool, freeDescIndex );
            }; /* else */
        }; /* if */
    }; /* if */
    if (freeDesc != NULL) {
        freeDesc->connection = conn;
    }; /* if */
    
    return freeDesc;
} /* wd101_GetFreeDesc */


void wd101_FreeDesc( twd101DBFSP dbfs, twd102FileDescP fDesc )
{
    wd35UnlockElem( dbfs->fileDescPool, fDesc->index );
} /* wd101_FreeDesc */


sapdbwa_Bool wd101_InitDBFS( twd101DBFSP	dbfs,
							 sapdbwa_UInt4	partitionId	)
{
    sapdbwa_Bool retcode = sapdbwa_True;

    dbfs->err = wd26CreateErr();
    if (dbfs->err == NULL) {
        retcode = sapdbwa_False;
    } else {
        dbfs->exclusiveTransactions = sapdbwa_True;
        dbfs->partitionId = partitionId;
        dbfs->fileDescPool =
            wd35CreateResourcePool( MAX_DESC_ARRAY_SIZE );
        if (dbfs->fileDescPool == NULL) {
            retcode = sapdbwa_False;
            wd26DestroyErr( dbfs->err );
        }; /* if */
    }; /* else */
    
    if (retcode == sapdbwa_False) {
        dbfs->fileDescPool  = NULL;
    }; /* if */

    return retcode;
} /* wd101_InitDBFS */


sapdbwa_Bool wd101_InodeClose( twd104FSDescP    fsDesc,
                             twd102FileDescP  fd,
                             twd26ErrP        err )
{
    wd104DbDeselectInodeByDirectory( fsDesc, err );
    return(sapdbwa_True);
} /* wd101_InodeClose */


sapdbwa_Bool wd101_InodeOpen( twd104FSDescP    fsDesc,
                            twd102FileDescP  fd,
                            twd101OrderFlag  orderFlag,
                            twd26ErrP        err )
{
    memset(&fd->extraInfo, 0, sizeof(fd->extraInfo));
    
    return wd104DbSelectInodeByDirectory( fsDesc, &fd->inode.InodeId,
                                          orderFlag, err );
} /* wd101_InodeOpen */


sapdbwa_Bool wd101_InodeRead( twd104FSDescP    fsDesc,
                            twd102FileDescP  fd,
                            void            *buf,
                            tsp00_Uint4      nbyte,
                            twd103Inode     *inode,
                            tsp00_Uint4     *Rtc,
                            twd26ErrP        err )
{
    tsp00_Uint4  len = 0;
    struct st_db_direct *dirEntry = &fd->extraInfo.DirectoryEntry;

    if (wd104DbFetchInodeByDirectory( fsDesc,
                                      inode,
                                      err ) ) {
        memset( dirEntry, 0, sizeof(*dirEntry));
        SAPDB_memcpy( &dirEntry->ino, &inode->InodeId, sizeof(twd103InodeId) );
        dirEntry->namelen = strlen(inode->Name);
        strcpy(dirEntry->name, inode->Name);
        dirEntry->reclen = sizeof(*dirEntry) -
            (sizeof(dirEntry->name) - (dirEntry->namelen ));
        len = min( nbyte, sizeof(*dirEntry));
        if (buf != NULL) {
            SAPDB_memcpy( buf, (char*) dirEntry, len );
        }; /* if */
        *Rtc += len;
        fd->filePos += len;
    }; /* if */

    return(len>0);
} /* wd101_InodeRead */


void wd101_MakeDate( char *odbcDate, char fsDate[9] )
{
    tsp00_Int4 pos = 0;
    
    fsDate[pos++] = odbcDate[8];
    fsDate[pos++] = odbcDate[9];
    fsDate[pos++] = '/';
    fsDate[pos++] = odbcDate[5];
    fsDate[pos++] = odbcDate[6];
    fsDate[pos++] = '/';
    fsDate[pos++] = odbcDate[2];
    fsDate[pos++] = odbcDate[3];
    fsDate[pos] = '\0';
} /* wd101_MakeDate */

static char *Month[12] = { "Jan", "Feb", "Mar", "Apr", "May",
                           "Jun", "Jul", "Aug", "Sep", "Oct",
                           "Nov", "Dec" };

void wd101_MakeTimestamp( char *odbcDate,
                          char *odbcTime,
                          char  timeStamp[13] )
{
    tsp00_Uint4 pos;
    char        day[3];
    char        month[3];
    tsp00_Int4  monthIndex = -1;
    char        year[5];
    tsp00_Date  actDate;
    tsp00_Time  actTime;
    char        rawDate[9];
    
    SAPDB_memcpy( day, &odbcDate[8], 2 );
    day[2] = '\0';
    
    SAPDB_memcpy( month, &odbcDate[5], 2 );
    month[2] = '\0';
    monthIndex = atol( month )-1;
    
    SAPDB_memcpy( year, odbcDate, 4 );
    year[4] = '\0';

    sprintf( rawDate, "%s%s%s", year, month, day );
    sqldattime( actDate, actTime );
    if (wd29DiffMonth( (char*) actDate, rawDate ) > 2) {
        sprintf( timeStamp, "%s %s %5s",
                 Month[monthIndex], day, year );
    } else {
        sprintf( timeStamp, "%s %s %5s",
                 Month[monthIndex], day, odbcTime );
    }; /* else */
} /* wd101_MakeTimestamp */


/* Find the basename of Path.
 * Path is a string of names separated by slashs ('/').
 * The last name in the path is the basename.
 *
 * examples :
 *
 * basename("/pipo/een") == "een"
 * basename("/pipo/een/") == "een"
 * basename("/pipo/") == "pipo"
 * basename("pipo/") == "pipo"
 * basename("/") == no basename
 * basename("//") == no basename
 */
sapdbwa_Bool wd101_PathBasename( char  	   *Path,
                               tsp00_Int2  *PrefixLength,
                               char        *Basename,
                               twd26ErrP    err )
{
    char       *baseName = NULL;
    sapdbwa_Bool  ok  = sapdbwa_True;
    tsp00_Int4  pathLen = 0;
    tsp00_Int4  baseNameLen = 0;
    tsp00_Int4  pos     = 0;
    
    if (Path != NULL) {
        pathLen = strlen( Path );
        pos     = pathLen-1;
        /* skip trailing '/' */
        if (Path[pos] == DBFS_PATH_SEP_WD101) {
            pos--;
         }; /* if */
        /* search "last" '/' in Path */
        while (Path[pos] != DBFS_PATH_SEP_WD101 && pos>0) pos--;
        if (Path[pos] == '/') {
            pos++;
        }; /* if */
        baseName = &Path[pos];
        baseNameLen = strlen( baseName );
        if (baseNameLen > MAX_FILEBASENAME_LEN_WD00) {
            wd26SetErr( err, ERR_FILENAME_TOO_LONG_WD26, baseName, NULL );
            ok = sapdbwa_False;
        } else {
            strcpy( Basename, baseName );
            /* delete trailing '/' */
            if (Basename[baseNameLen-1] == DBFS_PATH_SEP_WD101) {
                Basename[baseNameLen-1] = '\0';
            }; /* if */
            *PrefixLength = pathLen - baseNameLen;
        }; /* else */        
    } else {
        ok = sapdbwa_False;
    }; /* else */
    if (ok) {
        ok = wd101_IsValidFilename( Basename, err );
    }; /* if */
    
    return ok;
} /* wd101_PathBaseName */


sapdbwa_Bool wd101_Remove( twd104FSDescP    fsDesc,
                           char            *Path,
                           twd103InodeType  Type,
                           sapdbwa_Bool     recursive,
                           twd26ErrP        err )
{
    twd103Inode Inode;
    sapdbwa_Bool  deleteOK = sapdbwa_True;
    
    if (!wd101_FindInInode( fsDesc, NULL, Path, &Inode, err )) {
        if (wd26GetId( err ) == ERR_NO_ERROR_WD26) {
            /* file not found */
            if (Type == inode_directory) {
                wd26SetErr( err, ERR_DIR_NOT_FOUND_WD26, Path, NULL );
            } else /* if (Type == inode_file) */ {
                wd26SetErr( err, ERR_FILE_NOT_FOUND_WD26, Path, NULL );
            }; /* if */
        }; /* if */
        return(sapdbwa_False);
    }

    /* here Type 0 means any type */
    if (Type != 0 && Inode.Type != Type) {
        /* is not dir or file */
        if (Type == inode_directory) {
            wd26SetErr( err, ERR_NOT_DIR_WD26, Path, NULL );
        } else if (Type == inode_file) {
            wd26SetErr( err, ERR_NOT_FILE_WD26, Path, NULL );
        }; /* if */
        return(sapdbwa_False);
    }; /* if */

    if (recursive) {
        deleteOK = wd104DbDeleteInodeRecursive( fsDesc, &Inode, err);
    } else {
        deleteOK = wd104DbDeleteInode( fsDesc, &Inode, err);
    }; /* else */        
    if (!deleteOK) {
        if (wd26GetId( err ) == ERR_NO_ERROR_WD26) {
            /* directory not empty */
            wd26SetErr( err, ERR_DIR_NOT_EMPTY_WD26, Path, NULL );
            return(sapdbwa_False);
        }; /* if */
    }; /* if */
    
    return deleteOK;
} /* wd101_Remove */


sapdbwa_Bool wd101_CommonPathPrefix( const char *path1,
                                   const char *path2,
                                   twd26ErrP   err )
{
    tsp00_Uint4 pos = 0;

    /* scan common prefix */
    while (pos < strlen(path1)
           && pos < strlen(path2)
           && path1[pos] == path2[pos]) {
        pos++;
    }; /* while */
    /* after the common prefix only the filename should follow in both strings */
    while (pos < strlen(path1)
           && pos < strlen(path2)) {
        if (path1[pos] == DBFS_PATH_SEP_WD101
            || path2[pos] == DBFS_PATH_SEP_WD101) {
            wd26SetErr( err, ERR_DIFFERENT_PATH_NAMES_WD26, path1, path2 );
            return sapdbwa_False;
        }; /* if */
        pos++;
    }; /* while */

    return sapdbwa_True;
} /* wd101_CommonPathPrefix */


sapdbwa_Bool wd101_IsValidFilename( const char *name, twd26ErrP err )
{
    tsp00_Uint4 pos = 0;
    sapdbwa_Bool  checkOk = sapdbwa_True;
    
    while (name[pos] != '\0' && pos < MAX_FILEBASENAME_LEN_WD00 && checkOk) {
        if (!isdigit(name[pos]) && !isalpha(name[pos])
            && !wd101_ValidFilenameChar( name[pos])) {
            wd26SetErr( err, ERR_INVALID_FILENAME_WD26, name, NULL );
            checkOk = sapdbwa_False;
        }; /* if */
        pos++;
    }; /* while */
    if (pos >= MAX_FILEBASENAME_LEN_WD00) {
        wd26SetErr( err, ERR_FILENAME_TOO_LONG_WD26, name, NULL );
        checkOk = sapdbwa_False;
    }; /* else */

    return checkOk;
} /* wd101_IsValidFilename */


sapdbwa_Bool wd101_ValidFilenameChar( char ch )
{
    switch (ch) {
    case DBFS_PATH_SEP_WD101:
        return sapdbwa_False;
    default:
        return sapdbwa_True;
    }; /* switch */
} /* wd101_ValidFilenameChar */


void wd101_DirName( char *dirName, tsp00_Int4 len, const char *path )
{
    tsp00_Int2 pos = strlen( path );
    tsp00_Int4 copyLen = 0;

    /* search last '/' */
    while (path[pos] != DBFS_PATH_SEP_WD101 && pos>0) pos--;
    /* copy at least len-1 bytes */
    copyLen = min( pos, len-1 );
    strncpy( dirName, path, copyLen );
    dirName[copyLen] = '\0';
} /* wd101_DirName */


sapdbwa_Bool wd101_IsRoot( char *path )
{
    return (*path == '\0'
            || (*path == DBFS_PATH_SEP_WD101 && path[1] == '\0'));
} /* wd101_IsRoot */


sapdbwa_Bool wd101_IsSubDir( const char *path1, const char *path2 )
{
    const char *pos1 = path1;
    const char *pos2 = path2;

    /* skip leading '/' */
    if (*pos1 == DBFS_PATH_SEP_WD101)
        pos1++;
    if (*pos2 == DBFS_PATH_SEP_WD101)
        pos2++;    
    /* see if pos1 is prefix of pos2 */
    while (*pos1 != '\0' && *pos2 != '\0' && *pos1 == *pos2) {
        pos1++;
        pos2++;
    }; /* while */

    return (*pos1 == '\0' && (*pos2 == '\0' || *pos2 == DBFS_PATH_SEP_WD101));
} /* wd101_IsSubDir */


sapdbwa_Bool wd101_PathLengthOk( const char *path, twd26ErrP err )
{
    sapdbwa_Bool ok = sapdbwa_True;
    
    if (strlen( path ) > MAX_FILENAME_LEN_WD00) {
        wd26SetErr( err, ERR_PATH_TOO_LONG_WD26, path, NULL );
        ok = sapdbwa_False;
    }; /* if */

    return ok;
} /* wd101_PathLengthOk */


sapdbwa_Bool wd101_SetMode( twd103InodeType  inodeType,
                          twd101Mode      *mode,
                          twd26ErrP        err )
{
    sapdbwa_Bool ok = sapdbwa_True;
    
    switch (inodeType) {
    case inode_directory:
        *mode = DB_DIR;
        break;
    case inode_file:
        *mode = DB_FIL;
        break;
    case inode_link:
        *mode = DB_LIN;
        break;
    default:
        wd26SetErr( err,
                    ERR_INTERNAL_WD26,
                    "wd101_SetMode: Unknown inodeType.",
                    NULL );
        ok = sapdbwa_False;
    }; /* switch */

    return ok;
} /* wd101_SetMode */


sapdbwa_Bool wd101_PrependPath( char *destStr, char *newStr, tsp00_Int4 bufLen )
{
    sapdbwa_Bool   ok = sapdbwa_True;
    tsp00_Int4   destLen = strlen( destStr );
    tsp00_Int4   newLen = strlen( newStr );
    char         buffer[MAX_FILENAME_LEN_WD00];
    
    if (destLen + newLen + 1 < bufLen && destLen+newLen+2 < MAX_FILENAME_LEN_WD00) {
        /* buffer is big enough */
        SAPDB_memcpy( buffer, destStr, destLen );
        SAPDB_memcpy( destStr, newStr, newLen );
        destStr[newLen] = '/';
        SAPDB_memcpy( destStr + newLen + 1, buffer, destLen );
        destStr[destLen + newLen +1] = '\0';
    } else {
        /* buffer too small */
        ok = sapdbwa_False;
    };

    return ok;
} /* wd101_PrependPath */

/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/
