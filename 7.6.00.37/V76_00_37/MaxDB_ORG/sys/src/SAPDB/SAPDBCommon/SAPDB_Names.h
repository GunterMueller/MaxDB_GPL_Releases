/*!---------------------------------------------------------------------
  @file           SAPDB_Names.h
  @author         RobinW
  @brief          a collection of commonly used name definitions

  first created  2003-05-20  09:22

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
---------------------------------------------------------------------*/
#ifndef SAPDB_NAMES_H
#define SAPDB_NAMES_H

#define SAPDB_OUTPUTFILE_STARTING_PHASE_SUFFIX        ".start"  // deprecated!
#define SAPDB_OUTPUTFILE_RUNNING_PHASE_SUFFIX         ".run"    // deprecated!
#define SAPDB_OUTPUTFILE_TERMINATING_PHASE_SUFFIX     ".stop"   // deprecated!
#define SAPDB_OUTPUTFILE_COMPLETE_SUFFIX              ""    //".comp"   // deprecated!
#define SAPDB_OUTPUTFILE_OLD_FILE_SUFFIX              ".old"    // deprecated!

/*! the name of the database message file */
#define SAPDB_DIAG_FILE_NAME            "knldiag"               // deprecated!

/*! the name of the error messages file */
#define SAPDB_ERROR_FILE_NAME           "knldiag.err"           // deprecated!

#define SAPDB_KERNEL_MESSAGES_FILE_NAME "KnlMsg"
#define SAPDB_KERNEL_MESSAGES_ARCHIVE_FILE_NAME "KnlMsgArchive"

/*! the prefix of the runtime memory dump files used by RTEMem_RteAllocator as well known name for mapping */
#define SAPDB_RTE_MEMORY_DUMP_PREFIX    "RTEMemory"

/*! the identification of the memory dump table files (appended to well known name followed by decimal number) */
#define SAPDB_MEMORY_DUMP_TABLE_ID  "_Table."

/*! the identification of the memory dump chunk files (appended to well known name followed by decimal number) */
#define SAPDB_MEMORY_DUMP_CHUNK_ID  "_Chunk."

/*! the communication shared segment crash dump file containing all in use communication segments */
#define SAPDB_COMMUNICATION_DUMP_NAME   "comseg.dump"

/*! the AK cache dump file ( filled by ak10dump ) */
#define SAPDB_AK_CACHE_DUMP_NAME        "AK.dmp"

/*! the AK buffer dump file ??? */
#define SAPDB_AK_BUFFER_DUMP_NAME       "AK.buf"

/*! the AK statement dump file ( filled by ak10dump ) */
#define SAPDB_AK_STATEMENT_DUMP_NAME    "AK.stm"

/*! the admin command protocol file */
#define SAPDB_ADMIN_COMMANDS_FILE_NAME  "dbm.utl"

/*! the prefix of the UNIX core file */
#define SAPDB_CORE_FILE_NAME_PREFIX     "core"

/*---------- Registry file path definition  ------------------*/

/*! the name of the global database registration file */
#define SAPDB_DATABASES_INI_FILE        "Databases.ini"

/*! the name of the global instalation registration file */
#define SAPDB_INSTALLATIONS_INI_FILE    "Installations.ini"

#if !defined(WIN32)

/*---------- Registry file path definition (UNIX only) ------------------*/

/*! the name of the old global configuration directory on UNIX */
#define SAPDB_OLD_GLOBAL_DIRECTORY "/usr/spool/sql/"

/*! the name of the old global configuration directory on UNIX */
#define SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/usr/spool/sql/ini"

/*! the name of the old global configuration file on UNIX which contained, globals, databases and runtimes */
#define SAPDB_OLD_GLOBAL_COMMON_CONFIG_FILE "SAP_DBTech.ini"

/*! the name of the directory containing the global config file on UNIX */
#define SAPDB_GLOBAL_CONFIG_DIRECTORY   "/etc/opt"

/*! the name of the global configuration file on UNIX */
#define SAPDB_GLOBAL_CONFIG_FILE        SAPDB_GLOBAL_CONFIG_DIRECTORY "/sdb"

/*! the name of the global precompiler runtime registration file on UNIX and WINDOWS */
#  define SAPDB_RUNTIMES_INI_FILE          "Runtimes.ini"

/*! the name of the ODBC driver registration file on UNIX */
#  define SAPDB_ODBC_INI_FILE              "odbc.ini"

/*! the name of the ODBC driver registration file on UNIX */
#  define SAPDB_ODBCINST_INI_FILE          "ODBCINST.ini"

/*! the name of the section for globals in the global configuration file on UNIX */
#define SAPDB_GLOBALS_SECTION           "Globals"

/*! the name of the DIAG console pipe subdirectory */
#define SAPDB_SPOOL_DIAG_DIRECTORY "diag"

/*! the name of the FIFO requestor pipe subdirectory */
#define SAPDB_SPOOL_FIFO_DIRECTORY "fifo"

/*! the name of the pipe subdirectory */
#define SAPDB_SPOOL_PIPE_DIRECTORY "pipe"

/*! the name of the IPC tag file subdirectory */
#define SAPDB_SPOOL_IPC_DIRECTORY  "ipc"

/*! the name of the database process id file subdirectory */
#define SAPDB_SPOOL_PID_DIRECTORY  "pid"

/*! the name of the database watchdog process id file subdirectory */
#define SAPDB_SPOOL_PPID_DIRECTORY  "ppid"

/*! the name of the database speed id file subdirectory */
#define SAPDB_SPOOL_DBSPEED_DIRECTORY  "dbspeed"

/*! the prefix of the IPC tag file directory for database tags */
#define SAPDB_DATABASE_DIRECTORY_PREFIX "db:"

/*! the prefix of the IPC tag file directory for user tags */
#define SAPDB_USER_DIRECTORY_PREFIX     "us:"

/*! the prefix of the semaphore tag files */
#define SAPDB_SEMAPHORE_FILE_PREFIX         "-s"

/*! the prefix of the shared memory tag files */
#define SAPDB_SHARED_MEMORY_FILE_PREFIX     "-m"

/*! the prefix of the shared memory tag files */
#define SAPDB_MAILSLOT_FILE_PREFIX          "MAILSLOT-"

/*---------- End of registry file path definition (UNIX only) ------------------*/

#else /* WIN32 */

/*---------- Registry path definition (WINDOWS only) ------------------*/

#define SAPDB_GLOBAL_CONFIG_DIRECTORY   "\\etc\\opt"

/*! the name of the global configuration file on UNIX */
#define SAPDB_GLOBAL_CONFIG_FILE        SAPDB_GLOBAL_CONFIG_DIRECTORY "\\sdb"

/*! the name of the section for globals in the registry on Windows */
#  define SAPDB_GLOBALS_SECTION          ""
/*! the name of the section for globals in the global configuration file on Windows */
#  define SAPDB_GLOBALS_SECTION_IN_FILE  "Globals"

/*! the name of the global precompiler runtime registration on WINDOWS */
#  define SAPDB_RUNTIMES_INI_FILE        "Software\\SAP\\SAP DBTech" 

/*! the name of the ODBC driver registration */
#  define SAPDB_ODBC_INI_FILE            "odbc.ini"

/*! the name of the ODBC datasource registration */
#  define SAPDB_ODBCINST_INI_FILE          "Software\\ODBC\\ODBCINST.INI"

/*--------End of registry path definition (WINDOWS only) --------------*/

/*! the name of the kernel mini dump file */
#  define SAPDB_KNL_MINI_DUMP             "KnlMini.dmp"

/*! the name of the serv mini dump file */
#  define SAPDB_SRV_MINI_DUMP             "SrvMini.dmp"

#endif /* WIN32 */

/*! the name of the key in the globals section for the independend data path key */
#define SAPDB_INDEPDATA_KEY             "IndepData"

/*! the name of the key in the globals section for the independend programs key */
#define SAPDB_INDEPPROGRAMS_KEY         "IndepPrograms"

/*! the name of the key in the globals section for the user name of the owner of sapdb software and database processes */
#define SAPDB_SAPDBOWNER_KEY            "SdbOwner"

/*! the name of the key in the globals section for the administration group */
#define SAPDB_SAPDBGROUP_KEY            "SdbGroup"

/*! the name of the key in the globals section for the PAM usage
    Values "0","OFF","NO","NEVER","NOT_USED" or "PAM_NOT_USED" to switch off PAM usage
    Other values define PAM usage exclusive
    If key is not set (default) dehaviour for LINUX is to use PAM, for other UNIXes not to use PAM
    unless overruled by environment variable "SDB_PAM_NOT_USED"
 */
#define SAPDB_PAM_KEY                  "PAM"

/*! the name of the section for databases in the global database registration file on UNIX */
#define SAPDB_DATABASES_SECTION         "Databases"

/*! the name of the section for installations in the global installation registration file on UNIX */
#define SAPDB_INSTALLATIONS_SECTION     "Installations"

/*! the name of the section in the global precompiler runtime registration file on UNIX */
#define SAPDB_RUNTIME_SECTION           "Runtime"

/*---------- Subdirectory names-----------------------------------*/
/*! the subdirectory name containing INI files etc. */
#define SAPDB_CONFIG_SUBPATH            "config"

/*! serves as subdirectory name for the global and local 'Application Data' folder ($HOME\.sdb and $HOME\.sdb\<machine> resp.) (UNIX) */
#define SAPDB_APPLICATION_DATA_SUBDIRECTORY     ".sdb"

/*! serves as subdirectory name of the 'My Documents' folder ($HOME\sdb\<machine>) (UNIX, Windows) and 
    as subdirectory name in the 'Application Data' folder (Windows) */
#define SAPDB_HOME_SUBDIRECTORY         "sdb"

/*! serves as subdirectory name of the rtedump folder (<Rundirectory>/rtedump_dir/)
    needed to prevent a core dump produced by 'x_cons' to overwrite the kernel 
    core dump and to hide the RTEMemory_* files */
#define SAPDB_RUNTIME_DUMPSUBDIRECTORY   "rtedump_dir"

/*---------- Environmentvariables---------------------------------*/
/*! the name of the environment variable to overrule the global user_wrk_path $HOME/.sdb */
#define SAPDB_GLOBAL_APPLICATION_DATA_PATH      "SDB_GLOBAL_APPLICATION_DATA"

/*! the name of the environment variable to overrule the local user_wrk_path $HOME/.sdb/<machine> */
#define SAPDB_LOCAL_APPLICATION_DATA_PATH       "SDB_HOMEDIRECTORY"

/*! the name of the environment variable to overrule the user_data_path $HOME/sdb/<machine> */
#define SAPDB_MY_DOCUMENTS_PATH                 "SDB_MY_DOCUMENTS"

/*! the name of the environment variable to overrule /etc/opt/sdb */
#define SAPDB_OWN_GLOBAL_CONFIG_FILE            "SDB_OWN_GLOBAL_CONFIG_FILE"

/*! the name of the environment variable to bypass PAM authentification */
#define SAPDB_PAM_NOT_USED                      "SDB_PAM_NOT_USED"

/*! the name of the environment variable to overrule PAM service name */
#define SAPDB_PAM_SERVICE_NAME                  "SDB_PAM_SERVICE_NAME"

/*! the default value if the environment variable SDB_PAM_SERVICE_NAME is not found */
#define SAPDB_PAM_DEFAULT_SERVICE_NAME          "sdb"

/*! the name of the environment variable to overrule usage of <indepPath> */
#define SAPDB_NO_INDEPPATH                      "SAPDB_NO_INDEPPATH"

/*! the name of the environment variable that prevent (if found with any value...) prevents manipulation on environment */
#define SDB_DONT_MODIFY_KERNEL_ENVIRONMENT_DURING_START "SDB_DONT_MODIFY_KERNEL_ENVIRONMENT_DURING_START"

/*--------------------------------------------------------------------*/
#endif /* SAPDB_NAMES_H */
