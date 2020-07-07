/*!**********************************************************************

  module: hwd101dbfs.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  DB-Filesystem API

  see also:

  -----------------------------------------------------------------------

  copyright:              Copyright (c) 2000-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef HWD101DBFS_H
#define HWD101DBFS_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <sys/stat.h>

#include "gwd00.h"
#include "hwd05wases.h"
#include "hwd06waerr.h"
#include "hwd08dbc.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: twd101DBFSP

  description: This type is used to access a DBFS. It manages open
               file-handles, error-information etc.
  
************************************************************************/

typedef struct st_dbfs *twd101DBFSP;

typedef enum en_access {
 no_access = 0,
 access_read,
 access_write,
 access_write_compressed,
 access_readwrite
} twd101OpenFlag;

typedef enum en_order_flag {
    name_asc,
    name_desc,
    size_asc,
    size_desc,
    type_asc,
    type_desc,
    type_asc_name_asc,
    type_desc_name_desc,
    type_asc_name_desc,
    type_desc_name_asc,
    not_def
} twd101OrderFlag;
    
typedef sapdbwa_UInt2 twd101Mode;

typedef sapdbwa_Int4 twd101DBFile;

/* Database file modes */
#define DB_DIR  1
#define DB_FIL  2
#define DB_LIN  3

struct st_db_stat {
    twd101Mode   mode;
    sapdbwa_UInt4  size;
};

#define FILE_ERROR_WD101 -1

#define DBFS_PATH_SEP_WD101 '/' /* directory separator */    

#define MAX_PARTITION_ID_RESERVED_WD101 99		/* For SAP DB owned components		*/
#define START_PARTITION_ID_USER_WD101	100		/* For user defined partitions		*/
#define MAX_PARTITION_ID_USER_WD101		99999	/* Max user defined partition id	*/
#define DEFAULT_PARTITION_ID_WD101		0		/* WebAgent default partition		*/
#define DBFS_PARTITION_ID_WD101			1		/* WebAgent DBFS partition			*/

#define DBFS_STMT_VAR                   dbfsStmt
#define DBFS_STMT                       char DBFS_STMT_VAR[MAX_SQLSTR_LEN_WD00];
#define SET_DBFS_STMT(a,b,c)            sp77sprintf(DBFS_STMT_VAR, MAX_SQLSTR_LEN_WD00, "%s %s %s", (a?a:""), (b?b:""), (c?c:""));
#define SET_DBFS_STMT_EX2(a,b,c,d,e)    sp77sprintf(DBFS_STMT_VAR, MAX_SQLSTR_LEN_WD00, "%s %s %s %s %s", (a?a:""), (b?b:""), (c?c:""), (d?d:""), (e?e:""));

/************************************************************************
 * command interface
 ************************************************************************/

/*!**********************************************************************
  Function: wd101InitDBFS

  description: Initialization function for command line tools
               (dbls, ...). This function creates the resources for the
               DBFS.

  arguments:
      
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE if initialization failed.

  note: This function can only be used by single-threaded applications.
*/
sapdbwa_Bool WINAPI wd101InitDBFS();

    
/*!**********************************************************************
  Function: wd101ExitDBFS

  description: Exit function for command line tools (dbls, ...).
  
  precondition: wd101InitDBFS has been called.
  
  arguments:
      
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE if cleanup of resources failed.
  
  note: This function can only be used by single-threaded applications.
*/
sapdbwa_Bool WINAPI wd101ExitDBFS();


/*!**********************************************************************
  Function: wd101CmdCopy

  description: Copy a file or directory tree.

  arguments:
    sourcePath  [in]  The name of the source file or directory with
                      absolute path.
    destPath    [in]  The name of the new file or directory.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.

  remarks: If source path is a directoy then destPath must also be a
           directory. If sourcePath is a file then destPath may be an
           existing directory or it specifies a non-existing filename.

  note: This function can only be used by single-threaded applications.
*/
sapdbwa_Bool WINAPI wd101CmdCopy( const char     *sourcePath,
                                const char     *destPath );

    
/*!**********************************************************************
  Function: wd101CmdCpIn

  description: Copy a file from the OS-filesystem into the DB-filesystem.

  precondition: wd101InitDBFS has been called.
  
  arguments:
    SrcPath  [in]  OS-filename
    DstPath  [in]  Filename for the file to be created.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
  
  note: This function can only be used by single-threaded applications.
*/
sapdbwa_Bool WINAPI wd101CmdCpIn( const char     SrcPath[],
                                const char     DstPath[] );


/*!**********************************************************************
  Function: wd101CmdCpOut

  description: Copy a file from the DB-filesystem into the OS-filesystem.

  precondition: wd101InitDBFS has been called.
  
  arguments:
    SrcPath  [in]  DBFS-filename
    DstPath  [in]  Filename for the os-file to be created.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
  
  note: This function can only be used by single-threaded applications.
*/
sapdbwa_Bool WINAPI wd101CmdCpOut( const char    SrcPath[],
                                 const char    DstPath[] );

/*!**********************************************************************
  Function: wd101CmdGetErrorId

  description: Get error id for the last error.

  precondition: wd101InitDBFS has been called.
  
  arguments:
    
  return value: One of the error id's defined in hwd26err.h.
  
  note: This function can only be used by single-threaded applications.
*/
sapdbwa_UInt4 WINAPI wd101CmdGetErrorId();


/*!**********************************************************************
  Function: wd101CmdGetErrorMsg

  description: Get error message for the last error.

  precondition: wd101InitDBFS has been called.
  
  arguments:
    
  return value: Pointer to the error message.
  
  note: This function can only be used by single-threaded applications.
*/
const char* WINAPI wd101CmdGetErrorMsg();

    
/*!**********************************************************************
  Function: wd101CmdLs

  description: Write a listing of the given directory to stdout.

  precondition: wd101InitDBFS has been called.
  
  arguments:
    SrcPath  [in]  The directory that should be listed.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
  
  note: This function can only be used by single-threaded applications.
*/
sapdbwa_Bool WINAPI wd101CmdLs( const char    SrcPath[] );

    
/*!**********************************************************************
  Function: wd101CmdFormatDBFS

  description: Format DBFS. All required tables are created. If already a
               DBFS exists all of the contents are destroyed.

  precondition: wd101InitDBFS has been called.
  
  arguments:
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
  
  note: This function can only be used by single-threaded applications.
*/    
sapdbwa_Bool WINAPI wd101CmdFormatDBFS();


sapdbwa_Bool WINAPI wd101CmdLn( const char *targetPath, const char *newLink );

/*!**********************************************************************
  Function: wd101CmdMkDir

  description: Create a new directory in the dbfs.

  precondition: wd101InitDBFS has been called.
  
  arguments:
    path  [in]  The new directory path.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
  
  note: This function can only be used by single-threaded applications.
*/    
sapdbwa_Bool WINAPI wd101CmdMkDir( const char  *path );


/*!**********************************************************************
  Function: wd101CmdMkDir

  description: Create a file or directory tree in the dbfs.

  precondition: wd101InitDBFS has been called.
  
  arguments:
    oldPath  [in]  The old file or directory.
    oldPath  [in]  The new file or directory path.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
  
  note: This function can only be used by single-threaded applications.
*/    
sapdbwa_Bool WINAPI wd101CmdMv( const char *oldPath, const char *newPath );

    
/*!**********************************************************************
  Function: wd101CmdRm

  description: Remove a file in the dbfs.

  precondition: wd101InitDBFS has been called.
  
  arguments:
    path  [in]  The file to be deleted.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
  
  note: This function can only be used by single-threaded applications.
*/
sapdbwa_Bool WINAPI wd101CmdRm( const char *Path );


/*!**********************************************************************
  Function: wd101CmdRmDir

  description: Remove a directory in the dbfs.

  precondition: wd101InitDBFS has been called.
  
  arguments:
    path       [in]  The directory to be deleted.
    recursive  [in]  Indicates whether the contents of the directory
                     should also be removed. If this is sapdbwa_TRUE all files
                     and subdirectories in this directory are deleted.
                     
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
  
  note: This function can only be used by single-threaded applications.
*/
sapdbwa_Bool WINAPI wd101CmdRmDir( const char *Path, sapdbwa_Bool recursive );

/************************************************************************
 * programming interface
 ************************************************************************/

/*!**********************************************************************
  Function: wd101CreateDBFS

  description: Create a new DBFS object that uses a sapdbwa_SessionPoolP.

  arguments:
      pool			[in]	The sessionpool that is used by the DBFS.
							This implicitely defines the ServerDb,
							ServerNode, User and Password for the DBFS.

      partitionId	[in]	The partition id. This allows us to define
							multiple partitions for each user.

  return value: New twd101DBFSP or NULL if creation failed.
        
*/

twd101DBFSP WINAPI wd101CreateDBFS( sapdbwa_SessionPoolP	pool,
                                    sapdbwa_UInt4			partitionId );


/*!**********************************************************************
  Function: wd101CreateDbcDBFS

  description: Create a new DBFS object that uses connections from the
               sapdbwa_DBCPoolP.

  arguments:
      dbcPool    [in] The dbcPool that is used by the fs to get database
                      connections.
      datasource [in] The name of the datasource that is used to connect
                      with dbcPool.
      driver     [in] Instead of a datasource you can specify the
                      drivername to be used.
      serverNode [in] The hostname of the database server.
      serverDb   [in] The name of the server DB.
      user       [in] The user for the DBC connection.
      password   [in] The password of user.
      sqltrace   [in] An optional filename. If this is not NULL and not
                      emty it specifies the filename of the precompiler
                      SQL-trace. To prevent trace-file-open errors a
                      unique id is appended to this name for every
                      new connection.
      partitionId	[in]	The partition id. This allows us to define
							partitions for each user.
                     
  return value: New twd101DBFSP or NULL if creation failed.

*/

twd101DBFSP WINAPI wd101CreateDbcDBFS( sapdbwa_DBCPoolP	 dbcPool,
                                       sapdbwa_DBCP	     dbc,
                                       const char		*datasource,
                                       const char		*driver,
                                       const char		*serverNode,
                                       const char		*serverDb,
                                       const char		*user,
                                       const char		*password,
                                       const char		*sqltrace,
                                       sapdbwa_UInt4	 partitionId	);

    
/*!**********************************************************************
  Function: wd101DestroyDBFS

  description: Destroy DBFS ocject.

  arguments:
    dbfs [in]  The DBFS object to be destroyed.
    
  return value: always sapdbwa_TRUE.
*/
sapdbwa_Bool WINAPI wd101DestroyDBFS( twd101DBFSP dbfs );


/*!**********************************************************************
  Function: wd101Close

  description: Close a file that was opened with wd101Create or
               wd0101Open.

  arguments:
    dbfs [in]  The DBFS object used.
    fno  [in]  The handle of the file to be closed.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. The functions
                wd101GetErrorId and wd101GetErrorMsg can be used
                to retrieve the reason for a failure.
*/    
sapdbwa_Bool WINAPI wd101Close( twd101DBFSP dbfs, twd101DBFile fno );


/*!**********************************************************************
  Function: wd101CloseDir

  description: Close a directory that was opened with wd0101OpenDir.

  arguments:
    dbfs [in]  The DBFS object used.
    fno  [in]  The handle of the directory to be closed.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. The functions
                wd101GetErrorId and wd101GetErrorMsg can be used
                to retrieve the reason for a failure.
*/    
sapdbwa_Bool WINAPI wd101CloseDir( twd101DBFSP dbfs, twd101DBFile fno );


/*!**********************************************************************
  Function: wd101CopyErrorMsg

  description: Copy error message for the last error.

  arguments:
    dbfs   [in]   DBFS, that is used.
    buf    [out]  Application buffer for error text
    bufLen [in]   Length of *buf.
    
  return value: void.
*/
void WINAPI wd101CopyErrorMsg( twd101DBFSP     dbfs,
                               char           *buf,
                               sapdbwa_UInt4     bufLen );


/*!**********************************************************************
  Function: wd101Copy

  description: Copy a file or directory tree.

  arguments:
    dbfs        [in]  DBFS, that is used.
    sourcePath  [in]  The name of the source file or directory with
                      absolute path.
    destPath    [in]  The name of the new file or directory.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.

  remarks: If source path is a directoy then destPath must also be a
           directory. If sourcePath is a file then destPath may be an
           existing directory or a non-existing filename.

  error-codes:
  <UL>
  <LI> ERR_FILE_OR_DIR_NOT_FOUND_WD26: sourcePath does not exist.
  <LI> ERR_FILE_OR_DIR_EXISTS_WD26: destPath is an existing file.
  <LI> ERR_DIR_NOT_FOUND_WD26: sourcePath is a file and destPath
       specifies a non-existing file in a non-existing path or sourcePath
       is a directory and the subdirectory of destPath does not exist.
  </UL>
*/
sapdbwa_Bool WINAPI wd101Copy( twd101DBFSP     dbfs,
                             const char     *sourcePath,
                             const char     *destPath );


/*!**********************************************************************
  Function: wd101CopyWithLinks

  description: Copy a file or directory tree. Files are not copied. Only
               links to the original file are generated.

  arguments:
    dbfs        [in]  DBFS, that is used.
    sourcePath  [in]  The name of the source file or directory with
                      absolute path.
    destPath    [in]  The name of the new file or directory.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.

  remarks: If source path is a directoy then destPath must also be a
           directory. If sourcePath is a file then destPath may be an
           existing directory or a non-existing filename.

  error-codes:
  <UL>
  <LI> ERR_FILE_OR_DIR_NOT_FOUND_WD26: sourcePath does not exist.
  <LI> ERR_FILE_OR_DIR_EXISTS_WD26: destPath is an existing file.
  <LI> ERR_DIR_NOT_FOUND_WD26: sourcePath is a file and destPath
       specifies a non-existing file in a non-existing path or sourcePath
       is a directory and the subdirectory of destPath does not exist.
  </UL>
*/
sapdbwa_Bool WINAPI wd101CopyWithLinks( twd101DBFSP     dbfs,
                                      const char     *sourcePath,
                                      const char     *destPath );
    
    
/*!**********************************************************************
  Function: wd101Create

  description: Create a new file in the DBFS. If a file with the given
               name already exists it is deleted.

  arguments:
    dbfs  [in]  DBFS, that is used.
    path  [in]  The name of the new file with absolute path.
    oflag [in]  access_write or access_write_compressed.
    
  return value: A handle for the new file or FILE_ERROR_WD101 if the
                creation failed. The functions wd101GetErrorId and
                wd101GetErrorMsg can be used to retrieve the reason
                for a failure.
*/
twd101DBFile WINAPI wd101Create( twd101DBFSP     dbfs,
                                 const char     *path,
                                 twd101OpenFlag  oflag );


/*!**********************************************************************
  Function: wd101CreateLink

  description: Create a link in the DBFS. If a file or link with the given
               name already exists the function fails.

  arguments:
    dbfs       [in]  DBFS, that is used.
    path       [in]  The name of the new link with absolute path.
    targetPath [in]  The name of the target file or directory of the link.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure.
                The functions wd101GetErrorId and
                wd101GetErrorMsg can be used to get the reason
                for a failure.
*/
sapdbwa_Bool WINAPI wd101CreateLink( twd101DBFSP     dbfs,
                                   const char     *path,
                                   const char     *targetPath );


/*!**********************************************************************
  Function: wd101Delete

  description: Delete a file in the DBFS.

  arguments:
    dbfs [in]  DBFS, that is used.
    path [in]  The name of the file to delete with absolute path.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. The functions
                wd101GetErrorId and wd101GetErrorMsg can be used
                to retrieve the reason for a failure.
*/
sapdbwa_Bool WINAPI wd101Delete( twd101DBFSP  dbfs, const char  *Path );

    
/*!**********************************************************************
  Function: wd101FormatDBFS

  description: Format DBFS. All required tables are created. If already a
               DBFS exists all of the contents are destroyed.

  arguments:
    dbfs [in]  DBFS, that is formated.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information
*/
sapdbwa_Bool WINAPI wd101FormatDBFS( twd101DBFSP dbfs );


/*!**********************************************************************
  Function: wd101GetDirEntry

  description: Return a formatted line of a directory listing.

  precondition: The directory was opened with wd101OpenDir.
  
  arguments:
    dbfs    [in]  DBFS used.
    fno     [in]  Handle for the open directory.
    buf     [out] Buffer to return the formatted line.
    bufLen  [in]  The size of buf.
    details [in]  If this is sapdbwa_TRUE the dir-entry is returned with detailed
                  information (file-date etc.). The line is formatted in
                  the UNIX 'ls -l' style. If this is sapdbwa_FALSE only the file
                  or directory name is returned in buf.
                  
  return value: The number of bytes written to buf excluding the null
                termination or -1 for failure or 0 if there are no more
                entries.
                If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information
*/
sapdbwa_Int4 WINAPI wd101GetDirEntry( twd101DBFSP  dbfs,
                                    twd101DBFile fno,
                                    char        *buf,
                                    sapdbwa_Int4   bufLen,
                                    sapdbwa_Bool   details );
    
/*!**********************************************************************
  Function: wd101GetErr
  
  description: Get error information from dbfs.
  
  arguments:
    dbfs    [in]  DBFS used.
  
  return value:
  Error object of the dbfs. This can be used to retrieve
  error information if a previous operation on the dbfs failed.
  See <A HREF="hwd06waerr.html">hwd06waerr.html</A> for a description
  of the error class.
*/
sapdbwa_ErrP WINAPI wd101GetErr( twd101DBFSP dbfs );

    
/*!**********************************************************************
  Function: wd101GetErrorId

  description: Get error id for the last error.

  arguments:
    dbfs    [in]  DBFS.
    
  return value: The error id.
*/
sapdbwa_UInt4 WINAPI wd101GetErrorId( twd101DBFSP dbfs );


/*!**********************************************************************
  Function: wd101GetErrorMsg

  description: Get error message for the last error.

  arguments:
    dbfs    [in]  DBFS.
    
  return value: Pointer to the error message.
*/
const char* WINAPI wd101GetErrorMsg( twd101DBFSP dbfs );


/*!**********************************************************************
  Function: wd101FStat

  description: Get information for an open file.

  arguments:
    dbfs    [in]  DBFS, to be used.
    fno     [in]  Handle for the open file.
    buf     [out] Buffer used to return information.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
*/
sapdbwa_Bool WINAPI wd101FStat( twd101DBFSP        dbfs,
                              twd101DBFile       fno,
                              struct st_db_stat *buf );

/*!**********************************************************************
  Function: wd101MkDir

  description: Create a new directory. If a directory with the given name
               already eixts the function will fail.

  arguments:
    dbfs    [in]  DBFS, to be used.
    path    [in]  The name of the new directory with absolute path.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
*/
sapdbwa_Bool WINAPI wd101MkDir( twd101DBFSP dbfs, const char  *path );


/*!**********************************************************************
  Function: wd101Mv

  description: Move files or directory trees from one location to another.

  arguments:
    dbfs     [in]  DBFS, to be used.
    oldPath  [in]  The name of the old file or directory with absolute
                   path.
    newPath  [in]  The name of the new file or directory.                   
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.

  remarks: oldPath must be an existing file or directory. If oldPath is a
           file the following rules apply:
           <UL>
           <LI> If newPath is an existing directory then the file is
                moved to this directory with its original name.
           <LI> If newPath does not exist then a file newPath is created
                with the content of the file oldPath. If the parent
                directory of newPath does not exist you get the error
                ERR_DIR_NOT_FOUND_WD26.
           <LI> If newPath is an existing file wd101Mv returns sapdbwa_FALSE with
                the error ERR_FILE_OR_DIR_EXISTS_WD26.
           </UL>     

           If oldPath is a directory the following rules apply:
           <UL>
           <LI> If newPath is an existing directory then the directory and
                all its subdirectories and files are moved to newPath.
           <LI> If newPath does not exist then a directory newPath is created
                and all subdirectories and files of oldPath are moved to
                newPath. If the parent directory of newPath does not exist
                you get the error ERR_DIR_NOT_FOUND_WD26.
           <LI> If newPath is an existing file wd101Mv returns sapdbwa_FALSE with
                the error ERR_FILE_OR_DIR_EXISTS_WD26.
           </UL>
  error-codes:
  <UL>
  <LI> ERR_FILE_OR_DIR_NOT_FOUND_WD26: sourcePath does not exist.
  <LI> ERR_FILE_OR_DIR_EXISTS_WD26: destPath is an existing file.
  <LI> ERR_DIR_NOT_FOUND_WD26: sourcePath is a file and destPath
       specifies a non-existing file in a non-existing path or sourcePath
       is a directory and destPath is not a directory.
  </UL>           
*/
sapdbwa_Bool WINAPI wd101Mv( twd101DBFSP  dbfs,
                           const char  *oldPath,
                           const char  *newPath );

    
/*!**********************************************************************
  Function: wd101Open

  description: Open a file for reading or writing.
               If path is a link to a file, the target file of the link is
               opened.
  
  arguments:
    dbfs    [in]  DBFS used.
    path    [in]  The name of the file to be opened.
    oflag   [in]  <UL>
                  <LI> access_read: Read content of file.
                  <LI> access_write: Overwrite file.
                  </UL>
                  
  return value: The handle to the opened file or -1 for failure.
                If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
*/
twd101DBFile WINAPI wd101Open( twd101DBFSP     dbfs,
                               const char     *path,
                               twd101OpenFlag  oflag );


/*!**********************************************************************
  Function: wd101OpenDir

  description: Open a directory for reading.
  
  arguments:
    dbfs       [in]  DBFS used.
    path       [in]  The name of the directory to be opened.
    orderFlag  [in]  Defines the order of the entries. Possible values are:
                     <UL>
                     <LI> name_asc: Ordered by file-name ascending
                     <LI> name_desc: Ordered by file-name descending.
                     <LI> size_asc: Ordered by file-size ascending.
                     <LI> size_desc: Ordered by file-size descending.
                     <LI> type_asc: Ordered by type (directories first then files)
                     <LI> type_desc: Ordered ty type (files first then directories)
                     <LI> not_def: Unordered.
                     </UL>

  return value: The handle to the opened directory or -1 for failure.
                If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
*/
twd101DBFile WINAPI wd101OpenDir( twd101DBFSP      dbfs,
                                  const char      *path,
                                  twd101OrderFlag  orderFlag );


/*!**********************************************************************
  Function: wd101Read

  description: Read data from an opened file.
  
  arguments:
    dbfs    [in]  DBFS used.
    path    [in]  File to read from.
    buf     [out] Buffer in which the data is stored.
    nByte   [in]  Size of buf.
    
  return value: The number of bytes read or -1 for failure.
                If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
*/
sapdbwa_Int4 WINAPI wd101Read( twd101DBFSP   dbfs,
                             twd101DBFile  fno,
                             void         *buf,
                             sapdbwa_UInt4   nbyte );
    

/*!**********************************************************************
  Function: wd101ReadDir

  description: Get directory entry in ANSI style struct dirent.
  
  arguments:
    dbfs    [in]  DBFS used.
    path    [in]  File to read from.
    dirInfo [out] Dir info is written here.
    
  return value: The number of bytes read or -1 for failure.
                If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
*/
struct dirent* WINAPI wd101ReadDir( twd101DBFSP    dbfs,
                                    twd101DBFile   fno );
    
    
/*!**********************************************************************
  Function: wd101Rename

  description: Rename a file or directory.
  
  arguments:
    dbfs    [in]  DBFS used.
    oldName [in]  The file, link or directory that should be renamed.
    newName [in]  The new name.
    
  return value: sapdbwa_TRUE for success or sapdbwa_FALSE for failure. If the function
                fails wd101CmdGetErrorId and wd101CmdGetErrorMsg can be
                used to retrieve error information.
*/
sapdbwa_Bool WINAPI wd101Rename( twd101DBFSP  dbfs,
                               const char  *oldName,
                               const char  *newName );

/*!**********************************************************************
  Function: wd101Rm

  description: Remove a file or link.
  
  arguments:
    dbfs    [in]  DBFS used.
    path    [in]  The file or link that should be removed.
    
  return value: sapdbwa_TRUE for success or sapdbwa_FALSE for failure. If the function
                fails wd101CmdGetErrorId and wd101CmdGetErrorMsg can be
                used to retrieve error information.
*/
sapdbwa_Bool WINAPI wd101Rm( twd101DBFSP dbfs, const char *path );

    
/*!**********************************************************************
  Function: wd101RmDir

  description: Remove a directory. If the directory is not empty the
               function will fail.
  
  arguments:
    dbfs       [in]  DBFS used.
    path       [in]  The directory that should be removed.
    recursive  [in]  Indicates whether the contents of the directory
                     should also be removed. If this is sapdbwa_TRUE all files
                     and subdirectories in this directory are deleted.
    
  return value: sapdbwa_TRUE for success or sapdbwa_FALSE for failure. If the function
                fails wd101CmdGetErrorId and wd101CmdGetErrorMsg can be
                used to retrieve error information.
*/    
sapdbwa_Bool WINAPI wd101RmDir( twd101DBFSP  dbfs,
                              const char  *path,
                              sapdbwa_Bool   recursive );


/*!**********************************************************************
  Function: wd101Stat

  description: Get information for a file or link or dir.

  arguments:
    dbfs    [in]  DBFS, to be used.
    path    [in]  Filename with absolute path.
    buf     [out] Buffer used to return information.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
*/
sapdbwa_Bool WINAPI wd101Stat( twd101DBFSP        dbfs,
                             const char        *path,
                             struct st_db_stat *buf );


/*!**********************************************************************
  Function: wd101LinkStat

  description: Get information for a file or link or dir. Get target dir
               for links.

  arguments:
    dbfs          [in]  DBFS, to be used.
    path          [in]  Filename with absolute path.
    buf           [out] Buffer used to return information.
    targetBuf     [out] Buffer for target path of link.
    targetBufLen  [in]  Size of targetBuf
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
*/
sapdbwa_Bool WINAPI wd101LinkStat( twd101DBFSP        dbfs,
                                 const char        *path,
                                 struct st_db_stat *buf,
                                 char              *targetBuf,
                                 sapdbwa_Int4         bufLen );

    
/*!**********************************************************************
  Function: wd101CStat

  description: Get information for a file (ANSI C compatible struct stat).

  arguments:
    dbfs    [in]  DBFS, to be used.
    path    [in]  Filename with absolute path.
    buf     [out] Buffer used to return information.
    
  return value: sapdbwa_TRUE for success, sapdbwa_FALSE for failure. If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
*/
sapdbwa_Bool WINAPI wd101CStat( twd101DBFSP        dbfs,
                              const char        *path,
                              struct stat       *finfo );

    
/*!**********************************************************************
  Function: wd101Write

  description: Write data to an open file. The file must have been opened
               with wd101Create. It is not possible to overwrite or append
               data to a file opened with wd101Open.

  arguments:
    dbfs    [in]  DBFS, to be used.
    fno     [in]  Filehandle.
    buf     [in]  Buffer with data to write.
    nByte   [in]  Number of bytes to write.
    
  return value: The number of bytes written or -1 for failure.
                If the function fails
                wd101CmdGetErrorId and wd101CmdGetErrorMsg can be used to
                retrieve error information.
*/    
sapdbwa_Int4 WINAPI wd101Write( twd101DBFSP   dbfs,
                              twd101DBFile  fno,
                              void         *buf,
                              sapdbwa_UInt4   nbyte );

/************************************************************************
 * transaction handling
 ************************************************************************/

typedef struct st_transaction *twd101TransactionP;

twd101TransactionP WINAPI wd101StartTransaction( twd101DBFSP   dbfs );

sapdbwa_Bool WINAPI wd101EndTransaction( twd101DBFSP        dbfs,
                                       twd101TransactionP trans,
                                       sapdbwa_Bool         commit );

sapdbwa_Bool WINAPI wd101TrClose( twd101DBFSP        dbfs,
                                twd101TransactionP trans,
                                twd101DBFile       fno );

sapdbwa_Bool WINAPI wd101TrCloseDir( twd101DBFSP         dbfs,
                                 twd101TransactionP  trans,
                                 twd101DBFile        fno );
    
sapdbwa_Bool WINAPI wd101TrCopy( twd101DBFSP         dbfs,
                               twd101TransactionP  trans,
                               const char         *sourcePath,
                               const char         *destPath );

    
sapdbwa_Bool WINAPI wd101TrCopyWithLinks( twd101DBFSP         dbfs,
                                        twd101TransactionP  trans,
                                        const char         *sourcePath,
                                        const char         *destPath );    

twd101DBFile WINAPI wd101TrCreate( twd101DBFSP         dbfs,
                                   twd101TransactionP  trans,
                                   const char         *path,
                                   twd101OpenFlag      oflag );

sapdbwa_Bool WINAPI wd101TrCreateLink( twd101DBFSP         dbfs,
                                     twd101TransactionP  trans,
                                     const char         *path,
                                     const char         *targetPath );

sapdbwa_Bool WINAPI wd101TrDelete( twd101DBFSP         dbfs,
                                 twd101TransactionP  trans,
                                 const char         *path );

sapdbwa_Bool WINAPI wd101TrFStat( twd101DBFSP         dbfs,
                                twd101TransactionP  trans,
                                twd101DBFile        fno,
                                struct st_db_stat  *buf );

sapdbwa_Int4 WINAPI wd101TrGetDirEntry( twd101DBFSP         dbfs,
                                      twd101TransactionP  trans,
                                      twd101DBFile        fno,
                                      char               *buf,
                                      sapdbwa_Int4          bufLen,
                                      sapdbwa_Bool          details );
    
sapdbwa_Bool WINAPI wd101TrMkDir( twd101DBFSP         dbfs,
                                twd101TransactionP  trans,
                                const char         *path );
    
sapdbwa_Bool WINAPI wd101TrMv( twd101DBFSP         dbfs,
                             twd101TransactionP  trans,
                             const char         *oldPath,
                             const char         *newPath );
    
twd101DBFile WINAPI wd101TrOpen( twd101DBFSP         dbfs,
                                 twd101TransactionP  trans,
                                 const char         *path,
                                 twd101OpenFlag      oflag );

twd101DBFile WINAPI wd101TrOpenDir( twd101DBFSP         dbfs,
                                    twd101TransactionP  trans,
                                    const char         *path,
                                    twd101OrderFlag     orderFlag );

sapdbwa_Int4 WINAPI wd101TrRead( twd101DBFSP         dbfs,
                               twd101TransactionP  trans,
                               twd101DBFile        fno,
                               void               *buf,
                               sapdbwa_UInt4         nbyte );
    
sapdbwa_Bool WINAPI wd101TrRename( twd101DBFSP         dbfs,
                                 twd101TransactionP  trans,
                                 const char         *oldName,
                                 const char         *newName );
    
sapdbwa_Bool WINAPI wd101TrRm( twd101DBFSP         dbfs,
                             twd101TransactionP  trans,
                             const char         *path );
    
sapdbwa_Bool WINAPI wd101TrRmDir( twd101DBFSP         dbfs,
                                twd101TransactionP  trans,
                                const char         *path,
                                sapdbwa_Bool          recursive );

sapdbwa_Bool WINAPI wd101TrStat( twd101DBFSP         dbfs,
                               twd101TransactionP  trans,
                               const char         *path,
                               struct st_db_stat  *buf );
    
sapdbwa_Bool WINAPI wd101TrLinkStat( twd101DBFSP         dbfs,
                                   twd101TransactionP  trans,
                                   const char         *path,
                                   struct st_db_stat  *buf,
                                   char               *targetBuf,
                                   sapdbwa_Int4          bufLen );
    
sapdbwa_Int4 WINAPI wd101TrWrite( twd101DBFSP         dbfs,
                                twd101TransactionP  trans,
                                twd101DBFile        fno,
                                void               *buf,
                                sapdbwa_UInt4         nbyte );
    
/*!**********************************************************************

  EndChapter: twd101DBFSP

************************************************************************/
#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
