/*!---------------------------------------------------------------------
  @file           RTE_IniFileHandling.h
  @author         JoergM
  @ingroup        Registry for UNIX
  @brief          Handling of '.ini' files on UNIX

    Each .ini File consists of sections, which are identified by a section header.
    The section header consists of an name framed by brackets.
    Each entry in the section consists of a key an value separated by an equal sign.
    The section is seperated from the next section by an empty line.
    Example  SAPDB_Tech.ini
    
    [Database]
    SUT72=/home/remuser/releases/V72/usr
    MUT7264=/home/remuser/releases/V7264/usr
    

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


#ifndef RTE_INIFILEHANDLING_H
#define RTE_INIFILEHANDLING_H

#include "RunTime/RTE_Types.h"
#include "SAPDBCommon/SAPDB_C_Cplusplus.h"
#include "gsp00.h"


#ifdef __cplusplus
extern "C" {
#endif

/*! Result codes and errors */
typedef SAPDB_Byte RTE_IniFileResult;

/*! error codes used by config file functions */
/*! success */
#define SAPDB_INIFILE_RESULT_OK         ((RTE_IniFileResult)0)
/*! open failed */
#define SAPDB_INIFILE_RESULT_ERR_OPEN   ((RTE_IniFileResult)1)
/*! close failed */
#define SAPDB_INIFILE_RESULT_ERR_CLOSE  ((RTE_IniFileResult)2)
/*! creating temporary file failed */
#define SAPDB_INIFILE_RESULT_ERR_TEMP   ((RTE_IniFileResult)3)
/*! write file failed */
#define SAPDB_INIFILE_RESULT_ERR_WRITE  ((RTE_IniFileResult)4)
/*! out of memory */
#define SAPDB_INIFILE_RESULT_ERR_MEMORY ((RTE_IniFileResult)5)
/*! section or entry not found */
#define SAPDB_INIFILE_RESULT_NO_ENTRY   ((RTE_IniFileResult)6)
/*! key not found */
#define SAPDB_INIFILE_RESULT_ERR_KEY    ((RTE_IniFileResult)7)
/*! end of registry enumeration reached */
#define SAPDB_INIFILE_RESULT_EOF        ((RTE_IniFileResult)8)
/*! returned truncated value */
#define SAPDB_INIFILE_RESULT_TRUNCATED  ((RTE_IniFileResult)11)
/*! read failed */
#define SAPDB_INIFILE_RESULT_ERR_READ   ((RTE_IniFileResult)9)
/*! file lock failed */
#define SAPDB_INIFILE_RESULT_ERR_LOCK   ((RTE_IniFileResult)10)
/*! file seek failed */
#define SAPDB_INIFILE_RESULT_ERR_SEEK   ((RTE_IniFileResult)12)
/*! wrong parameter given or installation incomplete */
#define SAPDB_INIFILE_RESULT_ERR_PARAM  ((RTE_IniFileResult)13)
/*! could not determine user SID */
#define SAPDB_INIFILE_RESULT_ERR_GET_SID ((RTE_IniFileResult)14)
/*! remove registry value failed */
#define SAPDB_INIFILE_RESULT_ERR_REMOVE  ((RTE_IniFileResult)15)
/*! open registry for enumeration failed */
#define SAPDB_INIFILE_RESULT_ERR_ENUM_OPEN  ((RTE_IniFileResult)16)
/*! registry entry was not found */
#define SAPDB_INIFILE_RESULT_FILE_NOT_FOUND  ((RTE_IniFileResult)17)
/*! registry entry could not be accessed */
#define SAPDB_INIFILE_RESULT_ACCESS_DENIED  ((RTE_IniFileResult)18)
/*! set security for registry key failed */
#define SAPDB_INIFILE_RESULT_SET_SECURITY_FAILED  ((RTE_IniFileResult)19)

/*! The following defines are used to turn on 'printf' and 'putchar' codes in UNIX registry locking code only... */
/*! Allows single characters printouts for visibility of lock behavious */
#define SAPDB_VERBOSITY_FLAG_ACTIVITY             0x10U
/*! Mask Do enable all 'printf' (i.e. success information) */
#define SAPDB_VERBOSITY_LEVEL_MASK                0x0fU
/*! Do enable all 'printf' (i.e. success information) */
#define SAPDB_VERBOSITY_LEVEL_ALL_INFOS              4U
/*! Do 'printf' in case of warnings and errors (i.e.lock protocol violation) */
#define SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY 3U
/*! Do 'printf' in case of non fatal errors (i.e.lock protocol violation) */
#define SAPDB_VERBOSITY_LEVEL_ERROR_ONLY             2U
/*! Do 'printf' in case of fatal error (i.e. unhandable I/O error */
#define SAPDB_VERBOSITY_LEVEL_FATAL_ONLY             1U
/*! suppress all 'printf' in registry code (default) */
#define SAPDB_VERBOSITY_LEVEL_SILENT                 0U

/*! Registration locations */
typedef enum RTE_RegistryLocation_
{
    RTE_UserRegistry,    /*!< user specific registration ($HOME/.sapdb/Runtimes.ini) or HKCU:Software/SAP/SAP DBTech */
    RTE_GlobalRegistry,  /*!< global registration (indep_data/config/Runtimes.ini) or HKLM:Software/SAP/SAP DBTech */
    RTE_OldUNIXRegistry  /*!< older version global registeration (/usr/spool/sql/SAP_DBTech.ini UNIX only) */
} RTE_RegistryLocation;

/*! Advisory lock file simulation content (UNIX only) containing zero terminated C string machine name and 
    process id of current locker or 68 zero bytes if file is not locked */
typedef struct RTE_LockFileContent_
{
    SAPDB_UInt4 processId;      /*!< machine specific process id */
    SAPDB_Char machineName[64]; /*!< C string with machine name as found via 'uname()' nodename component Always zero terminated */
} RTE_LockFileContent;

/*! Registration Enumeration handle */
typedef struct RTE_RegistryHandleStruct_ {
    SAPDB_Char          *buffer;    /*!< copy of current INI file to be searched */
    SAPDB_Char          *nextEntry; /*!< pointer to next entry to use */
#ifdef WIN32
    HKEY                 hkResult;  /*!< key to be searched */
    DWORD                Index;     /*!< last index */
#endif
    char                *file;      /*!< given path to be searched */
    char                *section;   /*!< current section to be searched */
    RTE_RegistryLocation location;  /*!< current location to be searched */
    int                  parseAll;  /*!< flag set if all registries have to be parsed */
} RTE_RegistryHandleStruct, *RTE_RegistryHandle;

/*!
   @brief          Read Global Config Entry

   Gives back a string from the key=value pair /etc/opt/sapdb descripted by
   key 'szEntry'.
   It fails if either no file (SAPDB_INIFILE_RESULT_ERR_OPEN),
   the section or the entry (SAPDB_INIFILE_RESULT_NO_ENTRY)
   does not exist. If it is not accessible another error code is returned. This allows
   to program a fallback lookup in other global files if wanted.

   @param          szEntry [in] Given key of 'key=value' entry
   @param          szString [out] Place to store the value of 'key=value' entry
   @param          StringSize [in] Maximum value buffer size
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok  
       
   SAPDB_INIFILE_RESULT_ERR_OPEN    failed to open registry    
   SAPDB_INIFILE_RESULT_ERR_LOCK    failed to lock registry    
   SAPDB_INIFILE_RESULT_ERR_READ    failed to read registry    
   SAPDB_INIFILE_RESULT_NO_ENTRY    failed to find section or entry in section
   SAPDB_INIFILE_RESULT_TRUNCATED   returned truncated value    
   SAPDB_INIFILE_RESULT_ERR_PARAM   NULL pointer given
   SAPDB_INIFILE_RESULT_OK       No error
   
   @return         length of value string if Ok == SAPDB_INIFILE_RESULT_OK
 */

SAPDB_Int4 RTE_GetInstallationConfigString (
    const SAPDB_Char * szEntry,
    SAPDB_Char       * szString,
    const SAPDB_Int4   StringSize,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText,
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
   @brief          Read Common Config Entry

   Gives back a string from ini-file 'szFile' from the position descripted by
   'szSection' and 'szEntry'. It fails if either the file, the section or the
   entry does not exist.
   It fails if either no file (SAPDB_INIFILE_RESULT_ERR_OPEN),
   the section or entry  (SAPDB_INIFILE_RESULT_NO_ENTRY)
   does not exist. If it is not accessible another error code is returned. This allows
   to program a fallback lookup in other global files if wanted.

   @param          szFile [in] Name of registry file
   @param          szSection [in] Given section to read from
   @param          szEntry [in] Given key of 'key=value' entry
   @param          szString [out] Place to store the value of 'key=value' entry
   @param          StringSize [in] Maximum value buffer size
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok
       
   SAPDB_INIFILE_RESULT_ERR_OPEN    failed to open registry
   SAPDB_INIFILE_RESULT_ERR_LOCK    failed to lock registry
   SAPDB_INIFILE_RESULT_ERR_READ    failed to read registry
   SAPDB_INIFILE_RESULT_NO_ENTRY    failed to find section
   SAPDB_INIFILE_RESULT_TRUNCATED   returned truncated value
   SAPDB_INIFILE_RESULT_ERR_PARAM   NULL pointer given
   SAPDB_INIFILE_RESULT_OK       No error
   
   @return         length of value string if Ok == SAPDB_INIFILE_RESULT_OK
 */

SAPDB_Int4 RTE_GetGlobalConfigString (
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    const SAPDB_Char * szEntry,
    SAPDB_Char       * szString,
    const SAPDB_Int4   StringSize,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText,
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
   @brief          Read User Specific Config Entry

   Gives back a string from ini-file 'szFile' from the position descripted by
   'szSection' and 'szEntry'. The ini-file is searched in '$HOME/.sapdb' first.
   If no entry is found there, it DOES NOT try again in 'indepData/.sapdb'!
   It fails if either no file (config_error_open_chn09),
   the section or the entry (SAPDB_INIFILE_RESULT_NO_ENTRY)
   does not exist. If it is not accessible another error code is returned. This allows
   to program a fallback lookup in other global files if wanted.
   On Windows a member of the administrator group may also read a config string of another
   user by providing the users account name.

   @param          accountName [in] User account name or NULL ( Windows only - NULL means current user)
   @param          szFile [in] Name of registry file (UNIX only)
   @param          szSection [in] Given section to read from
   @param          szEntry [in] Given key of 'key=value' entry
   @param          szString [out] Place to store the value of 'key=value' entry
   @param          StringSize [in] Maximum value buffer size
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok
   
   SAPDB_INIFILE_RESULT_ERR_OPEN    failed to open registry
   SAPDB_INIFILE_RESULT_ERR_LOCK    failed to lock registry
   SAPDB_INIFILE_RESULT_ERR_READ    failed to read registry
   SAPDB_INIFILE_RESULT_NO_ENTRY    failed to find section or entry in section
   SAPDB_INIFILE_RESULT_TRUNCATED   returned truncated value
   SAPDB_INIFILE_RESULT_ERR_PARAM   NULL pointer given
   SAPDB_INIFILE_RESULT_OK    No error
   
   @return         length of value string if Ok == SAPDB_INIFILE_RESULT_OK
 */
SAPDB_Int4 RTE_GetUserConfigString (
    SAPDB_Char const * accountName,
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    const SAPDB_Char * szEntry,
    SAPDB_Char       * szString,
    const SAPDB_Int4   StringSize,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText,
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
   @brief          Read Config Entry

   First tries RTE_GetUserConfigString and as fallback RTE_GetGlobalConfigString is used.

   @param          szFile [in] Name of registry file
   @param          szSection [in] Given section to read from
   @param          szEntry [in] Given key of 'key=value' entry
   @param          szString [out] Place to store the value of 'key=value' entry
   @param          StringSize [in] Maximum value buffer size
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok
   
   SAPDB_INIFILE_RESULT_ERR_OPEN    failed to open registry
   SAPDB_INIFILE_RESULT_ERR_LOCK    failed to lock registry
   SAPDB_INIFILE_RESULT_ERR_READ    failed to read registry
   SAPDB_INIFILE_RESULT_NO_ENTRY    failed to find section or entry in section
   SAPDB_INIFILE_RESULT_TRUNCATED   returned truncated value
   SAPDB_INIFILE_RESULT_ERR_PARAM   NULL pointer given
   SAPDB_INIFILE_RESULT_OK    No error
   
   @return         length of value string if Ok == SAPDB_INIFILE_RESULT_OK
 */
SAPDB_Int4 RTE_GetConfigString (
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    const SAPDB_Char * szEntry,
    SAPDB_Char       * szString,
    const SAPDB_Int4   StringSize,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText,
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
   @brief          Write or update Registry Entry
   @param          szFile [in] Name of registry file
   @param          szSection [in] Section to write into
   @param          szEntry [in] Given key of 'key=value' entry
   @param          szString [in] Given value of 'key=value' entry
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok
   
   SAPDB_INIFILE_RESULT_BAD_KEY   bad key given (contains '=') or bad entry found (no '=')
   SAPDB_INIFILE_RESULT_ERR_TEMP  failed to create temp file or directory 
   SAPDB_INIFILE_RESULT_ERR_OPEN  failed to open registry
   SAPDB_INIFILE_RESULT_ERR_LOCK  failed to lock registry
   SAPDB_INIFILE_RESULT_ERR_READ  failed to read registry
   SAPDB_INIFILE_RESULT_ERR_WRITE failed to write temp file
   SAPDB_INIFILE_RESULT_ERR_SEEK  failed to seek to begin of file
   SAPDB_INIFILE_RESULT_ERR_PARAM   NULL pointer given
   SAPDB_INIFILE_RESULT_OK  No error
   
   @return         none, but err is updated to
  
      >0: failed ( inspect Ok )
      >1: ok
  


    Inserts or replaces a string into the ini-file name 'szString'
    to a position descripted by 'section'-name and 'entry'-name.
    If the file does not exists, it will be created
    files are located in 'indepData/config'.
    If this directory does not exists, the directory 'ini' will be 
    created in 'indepData'
    If 'indepData' does not exists the function failed
    This function needs write permission in 'indepData/config'
    to create a temporary file, otherwise it fails 

 */

SAPDB_Int4 RTE_PutConfigString (
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    const SAPDB_Char * szEntry,
    const SAPDB_Char * szString,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText,
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
   @brief          Write or update global registry Entry
   @param          szEntry [in] Given key of 'key=value' entry
   @param          szString [in] Given value of 'key=value' entry
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok
   
   SAPDB_INIFILE_RESULT_BAD_KEY   bad key given (contains '=') or bad entry found (no '=')
   SAPDB_INIFILE_RESULT_ERR_TEMP  failed to create temp file or directory
   SAPDB_INIFILE_RESULT_ERR_OPEN  failed to open registry
   SAPDB_INIFILE_RESULT_ERR_LOCK  failed to lock registry
   SAPDB_INIFILE_RESULT_ERR_READ  failed to read registry
   SAPDB_INIFILE_RESULT_ERR_WRITE failed to write temp file
   SAPDB_INIFILE_RESULT_ERR_SEEK  failed to seek to begin of file
   SAPDB_INIFILE_RESULT_ERR_PARAM   NULL pointer given
   SAPDB_INIFILE_RESULT_OK  No error
   
   @return         1 on success, 0 on error.
  
      >0: failed ( inspect Ok )
      >1: ok
  

    Inserts or replaces a string into the global ini-file /etc/opt/sapdb
    as a key=value pair in the Globals section.
    If the file does not exists, it will NOT be created, but the function fails.
    This function needs write permission in '/etc/opt'
    to create a temporary file, otherwise it fails 
 */

SAPDB_Int4 RTE_PutInstallationConfigString (
    const SAPDB_Char *szEntry, 
    const SAPDB_Char *szString,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText,
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
   @brief          Write or update user specific registry Entry

   @param          accountName [in] User account name or NULL ( Windows only - NULL means current user)
   @param          szFile [in] Name of registry file in $HOME/.sapdb (UNIX only)
   @param          szSection [in] Section to write into
   @param          szEntry [in] Given key of 'key=value' entry
   @param          szString [in] Given value of 'key=value' entry
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok
   
   SAPDB_INIFILE_RESULT_BAD_KEY   bad key given (contains '=') or bad entry found (no '=')
   SAPDB_INIFILE_RESULT_ERR_TEMP  failed to create temp file or directory
   SAPDB_INIFILE_RESULT_ERR_OPEN  failed to open registry
   SAPDB_INIFILE_RESULT_ERR_LOCK  failed to lock registry
   SAPDB_INIFILE_RESULT_ERR_READ  failed to read registry
   SAPDB_INIFILE_RESULT_ERR_WRITE failed to write temp file
   SAPDB_INIFILE_RESULT_ERR_SEEK  failed to seek to begin of file
   SAPDB_INIFILE_RESULT_ERR_PARAM   NULL pointer given
   SAPDB_INIFILE_RESULT_OK  No error
   
   @return         none, but err is updated to
  
      >0: failed ( inspect Ok )
      >1: ok
  


    Inserts or replaces a string into the user specific ini-file name 'szString'
    to a position descripted by 'section'-name and 'entry'-name.
    If the file does not exists, it will be created
    Files are located in '$HOME/.sapdb'.
    If this directory does not exists, the directory 'ini' will be 
    created in '$HOME'
    If '$HOME' does not exists the function failed
    This function needs write permission in '$HOME/.sapdb'
    to create a temporary file, otherwise it fails 
    On Windows a member of the administrator group may also write a config string of another
    user by providing the users account name.
 */

SAPDB_Int4 RTE_PutUserConfigString (
    SAPDB_Char const * accountName,
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    const SAPDB_Char * szEntry,
    const SAPDB_Char * szString,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText,
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
   @brief          Remove entry from Registry
     Removes the entry specified by 'szSection' and 'szEntry' from config file 
     This function needs write permission in 'indepData/config'
     to create a temporary file, otherwise it fails .

   @param          szFile [in] Name of registry file
   @param          szSection [in] Section to write into
   @param          szEntry [in] Given key of 'key=value' entry
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok
   
   SAPDB_INIFILE_RESULT_BAD_KEY   bad key given (contains '=') or bad entry found (no '=')
   SAPDB_INIFILE_RESULT_ERR_TEMP  failed to create temp file or directory
   SAPDB_INIFILE_RESULT_ERR_OPEN  failed to open registry
   SAPDB_INIFILE_RESULT_ERR_LOCK  failed to lock registry
   SAPDB_INIFILE_RESULT_ERR_READ  failed to read registry
   SAPDB_INIFILE_RESULT_ERR_WRITE failed to write temp file
   SAPDB_INIFILE_RESULT_ERR_SEEK  failed to seek to begin of file
   SAPDB_INIFILE_RESULT_OK  No error
   config_error_entry_chen09 failed to find entry
   
   @return 0 if removal failed, 1 if done
 */

SAPDB_Int4 RTE_RemoveConfigString (
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    const SAPDB_Char * szEntry,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText,
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
   @brief          Remove User specific Entry from Registry

     Removes the entry specified by 'szSection' and 'szEntry' from config file 
     This function needs write permission in '$HOME/.sapdb'
     to create a temporary file, otherwise it fails .
     On Windows a member of the administrator group may also remove a config string of another
     user by providing the users account name.

   @param          accountName [in] User account name or NULL ( Windows only - NULL means current user)
   @param          szFile [in] Name of registry file (UNIX only)
   @param          szSection [in] Section to write into
   @param          szEntry [in] Given key of 'key=value' entry
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok
   
   SAPDB_INIFILE_RESULT_BAD_KEY   bad key given (contains '=') or bad entry found (no '=')
   SAPDB_INIFILE_RESULT_ERR_TEMP  failed to create temp file or directory
   SAPDB_INIFILE_RESULT_ERR_OPEN  failed to open registry
   SAPDB_INIFILE_RESULT_ERR_LOCK  failed to lock registry
   SAPDB_INIFILE_RESULT_ERR_READ  failed to read registry
   SAPDB_INIFILE_RESULT_ERR_WRITE failed to write temp file
   SAPDB_INIFILE_RESULT_ERR_SEEK  failed to seek to begin of file
   SAPDB_INIFILE_RESULT_OK  No error
   config_error_entry_chen09 failed to find entry
   
   @return         0 if removal failed, 1 if done
 */

SAPDB_Int4 RTE_RemoveUserConfigString (
    SAPDB_Char const * accountName,
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    const SAPDB_Char * szEntry,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText,
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
   @brief          Open registry for enumeration of a section

     Open the registery and position to the given section.
     It is expected that a loop over 'RTE_NextConfigEnum()' is used to retrieve the
     entries.
     Note: The returned handle has no state information. The complete Registry is locked.
     Therefore you must call SAPDB_CloseConfigEnum() after last usage, or the Registry is locked
     until end of process.

   @param          szFile [in] Name of registry file
   @param          szSection [in] Section to write into
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok
   
   SAPDB_INIFILE_RESULT_ERR_OPEN    failed to open registry
   SAPDB_INIFILE_RESULT_ERR_LOCK    failed to lock registry
   SAPDB_INIFILE_RESULT_ERR_READ    failed to read registry
   SAPDB_INIFILE_RESULT_NO_ENTRY    failed to find section or entry in section
   SAPDB_INIFILE_RESULT_OK  No error
   
   @return handle  used in subsequent SAPDB_NextConfig calls
 */

RTE_RegistryHandle RTE_OpenGlobalConfigEnum (
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText, 
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
   @brief          Open User specific registry for enumeration of a section

     Open the registery and position to the given section.
     It is expected that a loop over 'RTE_NextConfigEnum()' is used to retrieve the
     entries.
     Note: The returned handle has no state information. The complete Registry is locked.
     Therefore you must call SAPDB_CloseConfigEnum() after last usage, or the Registry is locked
     until end of process.
     On Windows a member of the administrator group may also open a config string of another
     user by providing the users account name.


   @param          accountName [in] User account name or NULL ( Windows only - NULL means current user)
   @param          szFile [in] Name of registry file
   @param          szSection [in] Section to write into
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok
   
   SAPDB_INIFILE_RESULT_ERR_OPEN    failed to open registry
   SAPDB_INIFILE_RESULT_ERR_LOCK    failed to lock registry
   SAPDB_INIFILE_RESULT_ERR_READ    failed to read registry
   SAPDB_INIFILE_RESULT_NO_ENTRY    failed to find section or entry in section
   SAPDB_INIFILE_RESULT_OK  No error
   
   @return handle  used in subsequent SAPDB_NextConfig calls
 */
RTE_RegistryHandle RTE_OpenUserConfigEnum (
    SAPDB_Char const * accountName,
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText, 
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
   @brief          Open registry for enumeration of a section

     Open the registery and position to the given section.
     It is expected that a loop over 'RTE_NextConfigEnum()' is used to retrieve the
     entries.
     Note: The returned handle has no state information. The complete Registry is locked.
     Therefore you must call SAPDB_CloseConfigEnum() after last usage, or the Registry is locked
     until end of process.

   @param          szFile [in] Name of registry file
   @param          szSection [in] Section to write into
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok
   
   SAPDB_INIFILE_RESULT_ERR_OPEN    failed to open registry
   SAPDB_INIFILE_RESULT_ERR_LOCK    failed to lock registry
   SAPDB_INIFILE_RESULT_ERR_READ    failed to read registry
   SAPDB_INIFILE_RESULT_NO_ENTRY    failed to find section or entry in section
   SAPDB_INIFILE_RESULT_OK  No error
   
   @return handle  used in subsequent SAPDB_NextConfig calls
 */

RTE_RegistryHandle RTE_OpenConfigEnum (
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText, 
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
   @brief          Closes registry opened by SAPDB_OpenConfigFile ()

     Close the registry. This invalidates the given handle.
     Subsequent calls to SAPDB_NextConfig() must fail.

   @param          hEnum [in] Handle of registry file
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok
   
   SAPDB_INIFILE_RESULT_ERR_CLOSE   failed to close registry
   SAPDB_INIFILE_RESULT_OK   No error
   
   @return         0 if call failed, 1 if close done
 */

SAPDB_Int4 RTE_CloseConfigEnum (
    RTE_RegistryHandle hEnum,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText,
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
   @brief          Reads next entry from registry opened by SAPDB_OpenConfigFile ()
  
    Reads next 'szEntry' and 'szString' from section of config file opened by 
    SAPDB_OpenConfigEnum (). If caller like, he can pass NULL pointer, which make
    it possible ie. just to count the entries of a section...
    Gives back 'SAPDB_INIFILE_RESULT_EOF' in 'Ok' on end of section or end of file.

   @param          hEnum [in] Handle of registry file
   @param          szEntry [out] Place to store to key of 'key=value' entry (NULL means dont store)
   @param          MaxEntryLen [in] Maximum key buffer size
   @param          szString [out] Place to store the value of 'key=value' entry (NULL means dont store)
   @param          MaxStringLen [in] Maximum value buffer size
   @param          pLocation [out] Location of registry for retrieved entry
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok
    
   SAPDB_INIFILE_RESULT_ERR_READ    failed to read registry
   SAPDB_INIFILE_RESULT_BAD_KEY     entry contains no seperator
   SAPDB_INIFILE_RESULT_TRUNCATED   returned truncated value
   SAPDB_INIFILE_RESULT_EOF     at end of section or file
   SAPDB_INIFILE_RESULT_OK    No error
   
   @return 0 if failed or at end of section ( check Ok ), 1 if valid key/value available

 */

SAPDB_Int4 RTE_NextConfigEnum (
    RTE_RegistryHandle hEnum,
    SAPDB_Char *szEntry,
    const SAPDB_Int4 MaxEntryLen, 
    SAPDB_Char *szString,
    const SAPDB_Int4 MaxStringLen,
    RTE_RegistryLocation VAR_VALUE_REF pLocation,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText,
    RTE_IniFileResult  VAR_VALUE_REF Ok);

/*!
  !!!! Obsolet, use RTE_GetUserSpecificApplicationDataPath instead !!!
  @brief Get User Specific Config Path 
  @param ConfigPath [out] the returned config path ($HOME/.sdb/machine)
  @param TerminateWithDelimiter [in] forces to terminate path with path delimiter '/'
  @param ErrText [out] in case of problems filled with error text
  @return true if call succeeded, false if call failed
 */

SAPDB_Bool RTE_GetUserSpecificConfigPath ( RTE_Path           VAR_ARRAY_REF ConfigPath,
                                           SAPDB_Bool                       TerminateWithDelimiter,
                                           tsp00_ErrTextc VAR_ARRAY_REF ErrText );
/*!
  @brief Get User Specific local or global application data path  ('Application Data')
  @param applicationDataPath [out]      the returned application data path
                                        applicationDataPath is a C ( null terminated ) string  
  @param roaming [in]                   true: Data path for roaming applications is wanted
                                        false: Data path for local (nonroaming) applications is wanted
  @param TerminateWithDelimiter [in]    forces to terminate path with path delimiter '/'
  @param ErrText [out]                  in case of problems filled with error text
  @return                               true if call succeeded, false if call failed
 */

SAPDB_Bool RTE_GetUserSpecificApplicationDataPath   ( RTE_Path VAR_ARRAY_REF         applicationDataPath,
                                                      SAPDB_Bool                     roaming,
                                                      SAPDB_Bool                     terminateWithDelimiter,
                                                      tsp00_ErrTextc VAR_ARRAY_REF   errTextc );


/*!
  @brief Gets the common path for the application data. (Windows only)

         "C:\Documents and Settings\All Users\Application Data\sdb" on Windows is used as path 
         for the 'Application Data'.

  @param applicationDataPath    [out]   the returned application data path
                                        applicationDataPath is a C ( null terminated ) string  
  @param terminateWithDelimiter [in]    Forces to terminate path with path delimiter '/'
  @param ErrText                [out]   In case of problems filled with error text
  @return                               True if call succeeded, false if call failed
 */
SAPDB_Bool RTE_GetCommonApplicationDataPath         ( RTE_Path        VAR_ARRAY_REF    applicationDataPath,
                                                      SAPDB_Bool                       terminateWithDelimiter,
                                                      tsp00_ErrTextc  VAR_ARRAY_REF    errTextc );

/*!
  @brief Get User Specific document path ('My Documents') 
  @param myDocumentsPath [out] the returned document path
  @param TerminateWithDelimiter [in] forces to terminate path with path delimiter '/'
  @param ErrText [out] in case of problems filled with error text
  @return true if call succeeded, false if call failed
 */

SAPDB_Bool RTE_GetMyDocumentsPath                   (RTE_Path VAR_ARRAY_REF         myDocumentsPath,
                                                     SAPDB_Bool                     TerminateWithDelimiter,
                                                     tsp00_ErrTextc VAR_ARRAY_REF   errTextc);

/*!
  @brief Get User Independend - Common - Config Path
  @param ConfigPath [out] the path found as IndepData=path entry in /etc/opt/sapdb appened with /config
  @param TerminateWithDelimiter [in] forces to terminate path with path delimiter '/'
  @param ErrText [out] in case of problems filled with error text
  @return true if call succeeded, false if call failed
 */
SAPDB_Bool RTE_GetCommonConfigPath ( RTE_Path           VAR_ARRAY_REF ConfigPath,
                                     SAPDB_Bool                       TerminateWithDelimiter,
                                     tsp00_ErrTextc VAR_ARRAY_REF ErrText );

/*!
  @brief Get Common Independend Data Path
  @param ConfigPath [out] the path found as IndepData=path entry in /etc/opt/sapdb appened with /config
  @param TerminateWithDelimiter [in] forces to terminate path with path delimiter '/'
  @param ErrText [out] in case of problems filled with error text
  @return true if call succeeded, false if call failed
 */
SAPDB_Bool RTE_GetCommonDataPath ( RTE_Path           VAR_ARRAY_REF ConfigPath,
                                   SAPDB_Bool                       TerminateWithDelimiter,
                                   tsp00_ErrTextc VAR_ARRAY_REF ErrText );
/*!
  @brief Get User Specific Registry Locker

  This function allows to get the machine and process id of the current registry locker (if it is locked).
  On windows this function always reports registry as unlocked.

  If there is no locker, the 'FileLocked' flag is returned as false.
  Only if there is a locker, the 'IsLocalhost' and 'Content' are filled with meaningfull values (zeroed otherwise).

  @param          szFile [in] Name of registry file (UNIX only)
  @param          FileLocked [out] flag indicating file locker found
  @param          IsLocalhost [out] flag indicating localhost locks file
  @param          Content [out] returned locker pid and locker machine name
  @param          ErrText [out] Place for descriptive error message
  @param          Ok [out] Error return value  Possible values for Ok
   SAPDB_INIFILE_RESULT_ERR_OPEN    failed to open registry lock file   
   SAPDB_INIFILE_RESULT_ERR_READ    failed to read registry lock file
   SAPDB_INIFILE_RESULT_ERR_PARAM   NULL pointer given
   SAPDB_INIFILE_RESULT_OK       No error
  @return none
 */
void RTE_GetUserConfigFileLocker (
    const SAPDB_Char *szFile,
    SAPDB_Bool VAR_VALUE_REF FileLocked,
    SAPDB_Bool VAR_VALUE_REF IsLocalhost,
    RTE_LockFileContent VAR_VALUE_REF Content,
    tsp00_ErrTextc VAR_ARRAY_REF ErrText,
    RTE_IniFileResult VAR_VALUE_REF Ok);

/*!
  @brief Set timeout and retry values for advisory lock

  Retry of advisory lock is only of importance for UNIX, where the registry is simulated by a file.
  This file can set in an NFS filesystem and therefore the advisory lock used to synchronize the
  access is not always available. The 'retry' of this locking is complete switchable by 'retryWanted'
  flag. If 'pollLoops' is larger then zero, the first retries do only give up time slice and retry.
  The advisory lock can be hold by a local process in which case the alive test for the process works fine
  and kept locked by a remote machine. It is recommended to keep the default for local timeout to '0' which means
  unlimited and for 'remote timeout' to a meaningfull value depending on the network in use. The defaults are
  pollLoops (100), then unlimited number of 'one second sleep intervals' for localTimeout (0) and 5 minutes
  for remoteTimeout (300). Retry is always wanted. On Windows this always returns true.

  If lock collision occured the registry function will return    
    SAPDB_INIFILE_RESULT_ERR_LOCK    failed to lock registry
  for all registry function that work on writable files.

  Only 'localTimeout' and 'remoteTimeout' values can lead to SAPDB_INIFILE_RESULT_ERR_LOCK due to timeout error.
  Errtext in that case
    "Lock Registry:Too many local retries"
  or
    "Lock Registry:Too many remote retries"

  @param          retryWanted [in] new value for retry wanted flag
  @param          pollLoops [inout] input new value, output old value for 'poll loops'
  @param          localTimeout [inout] number of one second interval to wait in case of local lock collision
  @param          remoteTimeout [inout] number of one second interval to wait in case of remote lock collistion
  @return old value of 'retryWanted' flag.
 */
SAPDB_Bool RTE_SetRetryWanted( SAPDB_Bool  retryWanted
                             , SAPDB_UInt4 VAR_VALUE_REF pollLoops
                             , SAPDB_UInt4 VAR_VALUE_REF localTimeout
                             , SAPDB_UInt4 VAR_VALUE_REF remoteTimeout );

/*!
  @brief Set verbosity level for lock emulation code

  This interface was used during testing the lock emulation code only....
  
  @param level [in] new level (see SAPDB_VERBOSITY... defines above)
  @return old level
 */
SAPDB_UInt RegistryFile_SetVerbosity(SAPDB_UInt level);

/*!
  @brief Get current verbosity level for lock emulation code

  This interface was used during testing the lock emulation code only....
  
  @return current level (see SAPDB_VERBOSITY... defines above)
 */
SAPDB_UInt RegistryFile_GetVerbosity();

#ifdef __cplusplus
}
#endif

#endif /* RTE_INIFILEHANDLING_H */


