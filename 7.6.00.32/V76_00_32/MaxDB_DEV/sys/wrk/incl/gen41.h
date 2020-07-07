/*!
  @file           gen41.h
  @author         JoergM
  @special area   IPC Resources
  @brief          Handling routines for IPC Resources
  @see            

\if EMIT_LICENCE

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



\endif
*/



#ifndef GEN41_H
#define GEN41_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00.h"
#include "gen00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC int	sql41_create_sem ( key_t key ,
							  int   ref ,
							  int   range ,
							  int   mode ,
							  uid_t uid ,
							  char *type ,
							  char *dbname );

externC int sql41_remove_sem ( int  *semid ,
							  char *type ,
							  char *dbname );

externC int	sql41_create_shm ( key_t key ,
							  int   ref ,
							  int   range ,
							  int   size ,
							  int   mode ,
							  uid_t uid ,
							  char *type ,
							  char *dbname );

externC int sql41_remove_shm ( int  *shmid ,
							   char *type ,
							   char *dbname );

externC	char *sql41AttachShm ( int   shmid , 
                               char *address,
                               int   shmflag );

externC char *sql41_attach_shm ( int   shmid , 
                                 char *address );

externC int   sql41_detach_shm ( char **address );

externC void  sql41_stat_sem ( int semid );

externC	SAPDB_ULong sql41_getShmSize (int shmid);

externC void  sql41_stat_shm ( int shmid );

externC int   sql41_create_fifo ( tsp00_Pathc VAR_ARRAY_REF file ,
								  int perm );
externC int   sql41_remove_fifo ( tsp00_Pathc VAR_ARRAY_REF file );

externC void  sql41_get_request_fifo_name ( tsp00_Pathc VAR_ARRAY_REF file ,
										    char *dbname );
externC void  sql41_get_reply_fifo_name ( tsp00_Pathc VAR_ARRAY_REF file ,
										  char *dbname ,
										  pid_t pid ,
										  tsp00_Int4 ref );

externC int   sql41_create_idfile ( char *usertype ,
								    char *dbname ,
									char restype ,
									int ipcid );

externC int   sql41_remove_idfile ( char *usertype ,
								    char *dbname ,
									char restype ,
									int ipcid );

externC FILE *sql41_fopen_idfile ( char *usertype ,
								   char *dbname ,
								   char  restype ,
								   int   ipcid );

externC key_t sql41_get_server_key ( tsp00_DbNamec VAR_ARRAY_REF dbname );

#ifdef OLD
externC key_t sql41_get_csl_key ( tsp00_DbNamec VAR_ARRAY_REF dbname );
#endif /* OLD */

externC void  sql41_get_idfile_name ( tsp00_Pathc VAR_ARRAY_REF file ,
									  char *usertype ,
									  char *dbname ,
									  char  restype ,
									  int   ipcid );
externC int	  sql41_check_spool_dirs ( void );

externC int   sql41_check_dir ( tsp00_Pathc VAR_ARRAY_REF file );

externC void  sql41_get_ipc_dir ( tsp00_Pathc VAR_ARRAY_REF file );

externC void  sql41_get_diag_dir ( tsp00_Pathc VAR_ARRAY_REF file );

externC void  sql41_get_fifo_dir ( tsp00_Pathc VAR_ARRAY_REF file );

externC void  sql41_get_spool_dir ( tsp00_Pathc VAR_ARRAY_REF file );

externC int   sql41_remove_ipc_resources ( char *dbname );

externC int   en41FindDatabaseProcess(const char *Source, const char *DbName, const char *kernelpgm);
externC int   en41BuildDatabaseProcessScan(const char *dbn, char *Buffer, int BufferSize);

/*
  Function: en41BuildXShowCommand
  Description: Create x_show report output command

  Since x_show has to know ALL database (even very old ones...) it is a little bit tricky to get them.

  First get all processes into a tmp file.
  ps_all is an OS dependend 'ps -ef' that will produce output where database name will be visible

  Now find all probably running databases.
  Database are found due to the directory they create in IpcPath which is named db:DBNAME and for
  older ones (before 7.4) using the link they create on start db:DBNAME.

  Use each database name to grep in the ps output. If database is not found it is shown
  as (not stopped correctly) otherwise only its name is given.

  Arguments: PgmPath [in] path to program directory
             cmdline [out] the created command line
             size    [in] size of command buffer
  Return value: 0 if ok, -1 on error
 */
externC int   en41BuildXShowCommand(const char *pgmpath, char *Buffer, int BufferSize);

/* PTS 1109407 */
/*
  Function: sql41CheckFifo
  Description: Check the given file to be a fifo with at least given mode
  This is used during reopen action of Requestor thread to check, that fifo
  still exists and is a fifo and given protection mask is still valid...
  
  Arguments: file [in] Fifo file
             mode [in] Protection mask
  Return value: NULL if ok, else some error text
 */
externC const char *en41CheckFifo(const char *file, int mode);

/*
  Function: en41SetPipeFDInFile
  Description: Set pipe file descriptor into tag file
  Arguments: dbname [in] data base name
             pipeFD [in] pipe file descriptor
  Return value: 0 if ok, -1 on error
 */
externC int en41RemovePipeFDFile(char *dbname);

/*
  Function: en41RemovePipeFDFile
  Description: Remove tag file containing pipe file descriptor 
  Arguments: dbname [in] data base name
  Return value: 0 if ok, -1 on error
 */
externC int en41SetPipeFDInFile(const char *dbname, SAPDB_Int pipeFD);

/*
  Function: en41SetPIDInFile
  Description: Set process id into tag file
  Arguments: dbname [in] data base name
             pid [in] process id set set
  Return value: 0 if ok, -1 on error
 */
externC int en41SetPIDInFile(const char *dbname, tsp00_TaskId pid);
/*
  Function: en41SetPPIDInFile
  Description: Set process id into tag file for parent process (watchdog)
  Arguments: dbname [in] data base name
             pid [in] process id set set
  Return value: 0 if ok, -1 on error
 */
externC int en41SetPPIDInFile(const char *dbname, tsp00_TaskId pid);
/*
  Function: en41GetPIDInFile
  Description: Get process id from tag file
  Arguments: dbname [in] data base name
             pid [out] process id read
  Return value: 0 if ok, -1 on error
 */
externC int en41GetPipeFDFromFile(const char *dbname, SAPDB_Int *pipeFD);
/*
  Function: en41GetPipeFDFromFile
  Description: Get process id from tag file
  Arguments: dbname [in] data base name
             pipeFD [in] pipe file descriptor
  Return value: 0 if ok, -1 on error
 */
externC int en41GetPIDFromFile(const char *dbname, tsp00_TaskId *pid);
/*
  Function: en41GetPIDDirectory
  Description: Get process id directory name
  Arguments: file [out] filled with absolute path to directory
 */
externC void en41GetPIDDirectory(tsp00_Pathc VAR_ARRAY_REF file);
/*
  Function: en41GetPPIDInFile
  Description: Get process id from tag file for parent process (watchdog)
  Arguments: dbname [in] data base name
             pid [out] process id read
  Return value: 0 if ok, -1 on error
 */
externC int en41GetPPIDFromFile(const char *dbname, tsp00_TaskId *pid);
/*
  Function: en41GetPPIDDirectory
  Description: Get process id directory name
  Arguments: file [out] filled with absolute path to directory
 */
externC void en41GetPPIDDirectory(tsp00_Pathc VAR_ARRAY_REF file);
/*
  Function: en41SetDbSpeedInFile
  Description: Set current speed for data base started
  Arguments: dbname [in] data base name
             speed [in] speed identifier
  Return value: 0 if ok, -1 on error
 */
externC int en41SetDbSpeedInFile(const char *dbname, const char *speed);
/*
  Function: en41GetDbSpeedFromFile
  Description: Get current speed for data base started
  Arguments: dbname [in] data base name
             speed [out] speed identifier
             size [in] size of buffer for speed information
  Return value: 0 if ok, -1 on error
 */
externC int en41GetDbSpeedFromFile(const char *dbname, char *speed, int size);
/*
  Function: en41CreateServerKeyAndFirstSegment
  Description: Create the initial shared memory segment for database

  Each database has its own inital shared memory segment. Its ipc key is the base 
  ipc key for all semaphores and other shared memory segments created later on.
  It is also used for preventing other databases to use the same ipc key range.
  Arguments: dbname [in] the database instance name
             initialSize [in] the initial size of the first shared memory segment
             pBaseKey [out] filled with base key on success
             pBaseShmId [out] filled with base shm identifier on success
  Return value: false if failed, true if succeeded (pBaseKey and pBaseShmId filled)
 */
externC	int 
en41CreateServerKeyAndFirstSegment ( tsp00_DbNamec VAR_ARRAY_REF dbname,
                                     SAPDB_Int4 initialSize,
                                     key_t *pBaseKey,
                                     int   *pBaseShmId );
#endif  /* GEN41_H */
