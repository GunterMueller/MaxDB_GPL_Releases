/*!
  @file           vos01i.c
  @author         RaymondR
  @brief          Installation Functions
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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




//
//  INCLUDE FILES
//
#include "heo01.h"
#include "gos00.h"
#include "gos969.h"
#include "gsp09.h"
#include "gos94.h"


//
//  DEFINES
//

#define MOD__  "VOS01IC : "
#define MF__   MOD__"UNDEFINED"

#define XSERVER_IS_ACTIVE      0x0001
#define SERVERDB_IS_ACTIVE     0x0002
#define GATEWAY_IS_ACTIVE      0x0004

#define ACCESS_ALL             0x0001
#define ACCESS_READ            0x0002
#define ACCESS_WRITE           0x0004
#define ACCESS_EXECUTE         0x0008
#define ACCESS_DELETE          0x0010
#define ACCESS_CHANGE_OWNER    0x0020
#define ACCESS_TAKE_OWNERSHIP  0x0040

#define ACCESS_FILE            0x8000

#define REGISTRY_SZ            0
#define REGISTRY_EXPAND_SZ     1
#define REGISTRY_DWORD         2
#define REGISTRY_BINARY        3

//
//  MACROS
//


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
//  EXTERNAL VARIABLES
//


//
//  EXPORTED VARIABLES
//


//
//  LOCAL VARIABLES
//

//
//  LOCAL FUNCTION PROTOTYPES
//
static LONG sql01ic_map_access ( ULONG ulFileOrPathAccess );



//
// ========================== GLOBAL FUNCTIONS ================================
//

LONG sqlCheckActivePgms ( PULONG  pulPgmTypes )
  {
  #undef  MF__
  #define MF__ MOD__"sqlCheckActivePgms"
  LONG             rc    = NO_ERROR;
  BOOL             fActive;
  SQL_DBNAMEC      szServerDB;

  DBGIN;

  *pulPgmTypes = 0;

  rc = sql97_update( NULL );

  if ( rc != NO_ERROR )
    return ( rc );

  // --- check SERVERDBs
  rc = sql97_first_db_gw_state ( FALSE, szServerDB, &fActive );

  if (( rc == NO_ERROR ) && ( fActive ))
    *pulPgmTypes |= SERVERDB_IS_ACTIVE;
  else
    {
    while ( rc == NO_ERROR )
      {
      rc = sql97_next_db_gw_state ( szServerDB, &fActive );

      if (( rc == NO_ERROR ) && ( fActive ))
        {
        *pulPgmTypes |= SERVERDB_IS_ACTIVE;
        break;
        }
      }
    }

  if (( rc == NO_ERROR ) || ( rc == ERROR_NO_MORE_FILES  ))
    {
    // --- check GATEWAYs
    rc = sql97_first_db_gw_state ( FALSE, szServerDB, &fActive );

    if (( rc == NO_ERROR ) && ( fActive ))
      *pulPgmTypes |= GATEWAY_IS_ACTIVE;
    else
      {
      while ( rc == NO_ERROR )
        {
        rc = sql97_next_db_gw_state ( szServerDB, &fActive );

        if (( rc == NO_ERROR ) && ( fActive ))
          {
          *pulPgmTypes |= GATEWAY_IS_ACTIVE;
          break;
          }
        }
      }
    }

  if (( rc == NO_ERROR ) || ( rc == ERROR_NO_MORE_FILES  ))
    {
    CHAR      szPort[10];
    BOOL      fNoService;
    rc = sql97_first_xserver_state ( szPort, sizeof(szPort), &fNoService, &fActive );

    *pulPgmTypes |= ( fActive && '\0' == szPort[0]) ? XSERVER_IS_ACTIVE : 0;
    }


  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sqlInstallXServer ( PSZ pszDBRoot, BOOL fAutostart )
  {
  #undef  MF__
  #define MF__ MOD__"sqlInstallXServer"

  DBGPAS;

  sql60_disable_message_box ();

  //
  // WARNING: This prevents the message box output FROM NOW ON!!!
  //
  // Use sql60_enable_message_box () to reenable output if you need it..
  //
  /* PTS 1104712 */
  return ( os969UpdateAnyServerService ( NULL, SERVICE_ID_XSERVER, fAutostart, FALSE, 0, 0, 0 ) );
  }

/*------------------------------*/

LONG sqlStartXServer ( void )
  {
  #undef  MF__
  #define MF__ MOD__"sqlStartXServer"

  DBGPAS;

  sql60_disable_message_box ();

  return ( os969StartAnyServerService ( NULL, SERVICE_ID_XSERVER ) );
  }

/*------------------------------*/

LONG sqlStopXServer ( void )
  {
  #undef  MF__
  #define MF__ MOD__"sqlStopXServer"

  DBGPAS;

  sql60_disable_message_box ();

  return ( os969StopAnyServerService ( NULL, SERVICE_ID_XSERVER ) );
  }

/*------------------------------*/

LONG sqlInstallSERVERDB( PSZ pszDBRoot,
                         PSZ pszServerDB,
						 PSZ pszUserName,
                         PSZ pszPassword )
  {
  #undef  MF__
  #define MF__ MOD__"sqlInstallSERVERDB"

  DBGPAS;

  return ( sql94_service_entry ( UPDATE_SAPDB_SERVICE,
                                 KERNEL_TYPE_FAST, FALSE,
                                 NULL, pszDBRoot,
                                 pszServerDB, 
								 pszUserName,
								 pszPassword,
								 NULL,
								 NULL,
								 NULL,
								 NULL ));
  }

/*------------------------------*/

LONG sqlSetupSetIndependentDataPath ( PSZ pszDataPath )
  {
  #undef  MF__
  #define MF__ MOD__"sqlSetupSetIndependentDataPath"

  tsp01_RteError  RteError ; 

  DBGPAS;

  sqlSetIndependentDataPath ( pszDataPath, &RteError ); 

  return (RteError.OsErrCode);
  }

/*------------------------------*/

LONG sqlSetupGetIndependentDataPath ( PSZ pszDataPath )
  {
  #undef  MF__
  #define MF__ MOD__"sqlSetupGetIndependentDataPath"

  tsp01_RteError  RteError ; 

  DBGPAS;

  sqlGetIndependentDataPath ( pszDataPath, TERM_WITH_DELIMITER_EO01,&RteError ); 

  return (RteError.OsErrCode);
  }


/*------------------------------*/

LONG sqlSetupSetIndependentProgramsPath ( PSZ pszProgramsPath )
  {
  #undef  MF__
  #define MF__ MOD__"sqlSetupSetIndependentProgramsPath"

  tsp01_RteError  RteError ; 

  DBGPAS;

  sqlSetIndependentProgramsPath ( pszProgramsPath, &RteError ); 

  return (RteError.OsErrCode);
  }


/*------------------------------*/

LONG sqlSetupGetIndependentProgramsPath ( PSZ pszProgramsPath )
  {
  #undef  MF__
  #define MF__ MOD__"sqlSetupGetIndependentProgramsPath"

  tsp01_RteError  RteError ; 

  DBGPAS;

  sqlGetIndependentProgramsPath ( pszProgramsPath, TERM_WITH_DELIMITER_EO01, &RteError ); 

  return (RteError.OsErrCode);
  }

/*------------------------------*/

LONG sqlSetTCPIPDll( PSZ  pszTCPIPDLLName )
  {
  #undef  MF__
  #define MF__ MOD__"sqlSetTCPIPDll"

  DBGPAS;

  return (sql40c_set_tcpip_dll_path ( pszTCPIPDLLName ));
  }

/*------------------------------*/

LONG sqlGetTCPIPDll ( PSZ  *ppszTCPIPDLLName )
  {
  #undef  MF__
  #define MF__ MOD__"sqlGetTCPIPDll"

  DBGPAS;

  return (sql40c_get_tcpip_dll_path ( ppszTCPIPDLLName ));
  }

/*------------------------------*/

LONG sqlAddOperatorGroup ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sqlAddOperatorGroup"

  DBGPAS;

  return (sql03c_add_SAPDB_operators_group ());
  }

/*------------------------------*/

ULONG sqlSetLocMachRegValue ( PSZ    pszRelativePath,
                              PSZ    pszValueName,
                              PVOID  pValue,
                              ULONG  ulDataLen,    // - REG_BINARY ONLY!
                              DWORD  dwValueType )
  {
  #undef  MF__
  #define MF__ MOD__"sqlSetLocMachRegValue"
  REG_ENTRY_REC    RegistryEntries[1];
  LONG             rc    = NO_ERROR;

  DBGIN;

  RegistryEntries[0].pszValueName = pszValueName;
  RegistryEntries[0].pValue       = pValue;


  switch ( dwValueType )
    {
    case REGISTRY_SZ :
      RegistryEntries[0].ulValueSize  = (ULONG)strlen((PSZ)pValue);
      RegistryEntries[0].ulValueType  = REG_SZ;
      break;
    case REGISTRY_EXPAND_SZ :
      RegistryEntries[0].ulValueSize  = (ULONG)strlen((PSZ)pValue);
      RegistryEntries[0].ulValueType  = REG_EXPAND_SZ;
      break;
    case REGISTRY_DWORD :
      RegistryEntries[0].ulValueSize  = sizeof(DWORD);
      RegistryEntries[0].ulValueType  = REG_DWORD;
      break;
    case REGISTRY_BINARY :
      RegistryEntries[0].ulValueSize  = ulDataLen;
      RegistryEntries[0].ulValueType  = REG_BINARY;
      break;
    default:
      DBGOUT;
      return ( ERROR_INVALID_PARAMETER );
    }


  rc = sql50_reg_put_applic_values ( NULL, NULL, HKEY_LOCAL_MACHINE,
                                     pszRelativePath, NULL, NULL, 1, RegistryEntries );

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

ULONG sqlGetLocMachRegValue ( PSZ     pszRelativePath,
                              PSZ     pszValueName,
                              PVOID   pValue,
                              PULONG  pulDataLen,
                              DWORD   dwValueType )
  {
  #undef  MF__
  #define MF__ MOD__"sqlGetLocMachRegValue"
  REG_ENTRY_REC    RegistryEntries[1];
  LONG             rc    = NO_ERROR;

  DBGIN;

  switch ( dwValueType )
    {
    case REGISTRY_SZ :
      RegistryEntries[0].ulValueType  = REG_SZ;
      RegistryEntries[0].ulValueSize  = *pulDataLen;
      break;
    case REGISTRY_EXPAND_SZ :
      RegistryEntries[0].ulValueType  = REG_EXPAND_SZ;
      RegistryEntries[0].ulValueSize  = *pulDataLen;
      break;
    case REGISTRY_DWORD :
      RegistryEntries[0].ulValueType  = REG_DWORD;
      RegistryEntries[0].ulValueSize  = sizeof(DWORD);
      break;
    case REGISTRY_BINARY :
      RegistryEntries[0].ulValueType  = REG_BINARY;
      RegistryEntries[0].ulValueSize  = *pulDataLen;
      break;
    default:
      DBGOUT;
      return ( ERROR_INVALID_PARAMETER );
    }

  RegistryEntries[0].pszValueName = pszValueName;
  RegistryEntries[0].pValue       = pValue;

  rc = sql50_reg_get_applic_values ( NULL, HKEY_LOCAL_MACHINE,
                                     pszRelativePath, 1, RegistryEntries );

  if ( rc == NO_ERROR )
    *pulDataLen = RegistryEntries[0].ulValueSize;
  else
    *pulDataLen = 0;

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sqlSetFileOrPathSecurity ( PSZ    pszFileOrPathName,
                                ULONG  ulWorldDirAccess,
                                ULONG  ulBackupOpDirAccess,
                                ULONG  ulWorldFileAccess,
                                ULONG  ulBackupOpFileAccess )
  {
  #undef  MF__
  #define MF__ MOD__"sqlSetFileOrPathSecurity"
  LONG                rc    = NO_ERROR;
  ACCESS_RIGHTS_REC   Access;
  SECURITY_ATTRIBUTES SA;

  DBGIN;

  SA.nLength         = sizeof(SA);
  SA.bInheritHandle  = FALSE;


  memset ( &Access, 0, sizeof (Access) );  // - this must be done first!
  Access.fDescOwnerIsAdmin             = TRUE;
   
  Access.Creator.ulAccess              = FILE_ALL_ACCESS;
  Access.Creator.ulAceFlags            = CONTAINER_INHERIT_ACE;
  Access.Creator.ulInheritAccess       = GENERIC_ALL;
  Access.Creator.ulInheritAceFlags     = INHERIT_ONLY_ACE|OBJECT_INHERIT_ACE;
   
  Access.Admin.ulAccess                = FILE_ALL_ACCESS;
  Access.Admin.ulAceFlags              = CONTAINER_INHERIT_ACE;
  Access.Admin.ulInheritAccess         = GENERIC_ALL;
  Access.Admin.ulInheritAceFlags       = INHERIT_ONLY_ACE|OBJECT_INHERIT_ACE;

  Access.System.ulAccess               = FILE_ALL_ACCESS;
  Access.System.ulAceFlags             = CONTAINER_INHERIT_ACE;
  Access.System.ulInheritAccess        = GENERIC_ALL;
  Access.System.ulInheritAceFlags      = INHERIT_ONLY_ACE|OBJECT_INHERIT_ACE;

  Access.SAPDBOp.ulAccess              = FILE_ALL_ACCESS;
  Access.SAPDBOp.ulAceFlags            = CONTAINER_INHERIT_ACE;
  Access.SAPDBOp.ulInheritAccess       = GENERIC_ALL;
  Access.SAPDBOp.ulInheritAceFlags     = INHERIT_ONLY_ACE|OBJECT_INHERIT_ACE;

  Access.World.ulAccess                = sql01ic_map_access(ulWorldDirAccess);
  Access.World.ulAceFlags              = CONTAINER_INHERIT_ACE;
  Access.World.ulInheritAccess         = sql01ic_map_access(ulWorldFileAccess|ACCESS_FILE);
  Access.World.ulInheritAceFlags       = INHERIT_ONLY_ACE|OBJECT_INHERIT_ACE;

  Access.BackupOp.ulAccess             = sql01ic_map_access(ulBackupOpDirAccess);
  Access.BackupOp.ulAceFlags           = CONTAINER_INHERIT_ACE;
  Access.BackupOp.ulInheritAccess      = sql01ic_map_access(ulBackupOpFileAccess|ACCESS_FILE);
  Access.BackupOp.ulInheritAceFlags    = INHERIT_ONLY_ACE|OBJECT_INHERIT_ACE;

  rc = sql49c_alloc_and_init_SD( &Access, &SA.lpSecurityDescriptor );

  if ( rc == NO_ERROR )
    {
    rc = sql49c_set_file_security ( pszFileOrPathName,
                                    SA.lpSecurityDescriptor );
    FREE_MEM (SA.lpSecurityDescriptor);
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sqlRegisterInst( PSZ pszPath, PSZ pszVersion )
{
    tsp09_RteInstallationInfoNew relInfo;
    tsp9_rte_xerror xError;
    tsp00_Int4 pathLen;

	for ( pathLen = (tsp00_Int4)strlen(pszPath); pathLen > 0; pathLen-- )
    {
       if ( pszPath[pathLen-1] != '/' && pszPath[pathLen-1] != '\\' )
           break;
    }

    if ( pathLen > sizeof(tsp00_Path) )
    {
        return ERROR_INVALID_PARAMETER;
    }
    
    xError.xe_result = 0;

    /* PTS 1110181 */
	memset( &relInfo, 0, sizeof(relInfo) );
    relInfo.RecordVersion = INSTALLATION_RECORD_VERSION_V721;
    relInfo.RecordLength  = sizeof(relInfo);
	SAPDB_memcpy( relInfo.version.no, pszVersion, sizeof(tsp09_release_version) );
    SAPDB_memcpy( relInfo.dbroot, pszPath, pathLen );
    SAPDB_memcpy( relInfo.key, pszPath, pathLen );

    sqlxregisterinstNew (&relInfo, &xError);

	// Return 0 on success, 1 on error
	return xError.xe_result;
}

/*------------------------------*/

LONG sqlAddWellKnownService( PSZ  pszServiceName, 
                             PSZ  pszPortNumber, 
                             PSZ  pszProtocol,
                             PSZ  pszAliases,
                             PSZ  pszComment,
                             BOOL fCreateBackup )
{
  CHAR             szServiceFile[MAX_PATH];
  CHAR             szTmpServiceFile[MAX_PATH];
  FILE             *fpIn          = NULL;
  FILE             *fpOut         = NULL;
  PSZ              pszSystemRoot  = NULL;
  BOOL             Found          = FALSE;
  PSZ              pszTmp;
  CHAR             szLine[256];
  CHAR             szInsertLine[256];
  ULONG            ulLen;
  int              Err; 
  int              i;

  if ( sql02_get_platform_id() != VER_PLATFORM_WIN32_NT )
  {
    pszSystemRoot  = getenv("WINDIR");

    if ( pszSystemRoot == NULL ) 
      pszSystemRoot = "c:\\windows";

    strcpy( szServiceFile, pszSystemRoot );
    strcat( szServiceFile, "\\services" );
  }
  else
  {
    pszSystemRoot  = getenv("SYSTEMROOT");

    if ( pszSystemRoot == NULL ) 
      pszSystemRoot = "c:\\winnt";

    strcpy( szServiceFile, pszSystemRoot );
    strcat( szServiceFile, "\\system32\\drivers\\etc\\services" );
  }


  strcpy( szTmpServiceFile, szServiceFile );
  strcat( szTmpServiceFile, ".$$$" );

  ulLen = (ULONG)strlen(pszServiceName)  + 
          (ULONG)strlen(pszPortNumber)   + 
          (ULONG)strlen(pszProtocol)     + 
          (ULONG)strlen(pszAliases)      + 
          (ULONG)strlen(pszComment);

  if ( ulLen > sizeof( szInsertLine ) - 40 )
    return ERROR_INVALID_PARAMETER;

  sprintf ( szInsertLine, "%-15s %s/%-s %14s # %s\n",pszServiceName,
                          pszPortNumber, pszProtocol, pszAliases, pszComment );

  remove ( szTmpServiceFile );
  fpIn  = fopen ( szServiceFile, "r" );
  fpOut = fopen ( szTmpServiceFile, "wb" );

  if (( fpIn == NULL ) || ( fpOut == NULL ))
    return errno;

  while ( fgets( szLine, sizeof(szLine) - 1, fpIn ) != NULL )
  {
    pszTmp = strstr( szLine, pszServiceName );

    while (( pszTmp != NULL ) && ( pszTmp > szLine ))
    {
      --pszTmp;
      if (( *pszTmp != '\t' ) && ( *pszTmp != '  ' ))
        break;
    }

    if ( pszTmp == szLine )
    {
      if ( Found == FALSE )  // - remove duplicate lines
      {
        Found = TRUE;
        Err   = fputs ( szInsertLine, fpOut );
      }
    }
    else
      Err = fputs ( szLine, fpOut );

    if ( Err == EOF )
    {
      // --- output file error!
      Err = errno;
      fclose(fpIn);
      fclose(fpOut);
      remove ( szTmpServiceFile );

      return Err;
    }
  }

  // --- eof or input file error ?
  if ( feof(fpIn) != 0 )
  {
    // --- EOF
    //
    if ( Found == FALSE )
      fputs ( szInsertLine, fpOut );

    fclose(fpIn);
    fclose(fpOut);

    if ( fCreateBackup )
    {
      i = 1;
      do
      {
        sprintf ( szTmpServiceFile, "%s.b%d", szServiceFile, i++ );
        if ( rename( szServiceFile, szTmpServiceFile ) != 0 )
          Err = errno;
        else
          Err = 0;
      } 
      while (( Err != 0 ) && ( i < 99 ));

      strcpy( szTmpServiceFile, szServiceFile );
      strcat( szTmpServiceFile, ".$$$" );

      if ( Err == 0 )
      {
        remove ( szServiceFile );
        for (i = 0; (rename( szTmpServiceFile, szServiceFile ) != 0) && (i < 10); i++)
          Sleep (1000);
      }
      else
        remove ( szTmpServiceFile );
    }
    else
    {
      strcpy( szTmpServiceFile, szServiceFile );
      strcat( szTmpServiceFile, ".$$$" );

      remove ( szServiceFile );
      for (i = 0; (rename( szTmpServiceFile, szServiceFile ) != 0) && (i < 10); i++)
        Sleep (1000);
    }
  }
  else
  {
    // --- input file error!
    //
    Err = errno;
    fclose(fpIn);
    fclose(fpOut);
    remove ( szTmpServiceFile );
  }

  return Err;
}

//
// ========================== LOCAL FUNCTIONS =================================
//

static LONG sql01ic_map_access ( ULONG ulFileOrPathAccess )
  {
  #undef  MF__
  #define MF__ MOD__"sql01ic_map_access"
  ULONG            ulAccess = 0;

  DBGPAS;

  if ( ACCESS_FILE & ulFileOrPathAccess)
    {
    if ( ulFileOrPathAccess & ACCESS_ALL )
      ulAccess = FILE_ALL_ACCESS;
    else
      {
      if ( ulFileOrPathAccess & ACCESS_READ )
        ulAccess |= FILE_GENERIC_READ;

      if ( ulFileOrPathAccess & ACCESS_WRITE )
        ulAccess |= FILE_GENERIC_WRITE;

      if ( ulFileOrPathAccess & ACCESS_EXECUTE )
        ulAccess |= FILE_GENERIC_EXECUTE;

      if ( ulFileOrPathAccess & ACCESS_DELETE )
        ulAccess |= DELETE;

      if ( ulFileOrPathAccess & ACCESS_TAKE_OWNERSHIP )
        ulAccess |= WRITE_OWNER;

      if ( ulFileOrPathAccess & ACCESS_CHANGE_OWNER )
        ulAccess |= WRITE_DAC;
      }
    }
  else
    {
    if ( ulFileOrPathAccess & ACCESS_ALL )
      ulAccess = GENERIC_ALL;
    else
      {
      if ( ulFileOrPathAccess & ACCESS_READ )
        ulAccess |= GENERIC_READ;

      if ( ulFileOrPathAccess & ACCESS_WRITE )
        ulAccess |= GENERIC_WRITE;

      if ( ulFileOrPathAccess & ACCESS_EXECUTE )
        ulAccess |= GENERIC_EXECUTE;

      if ( ulFileOrPathAccess & ACCESS_DELETE )
        ulAccess |= DELETE;

      if ( ulFileOrPathAccess & ACCESS_TAKE_OWNERSHIP )
        ulAccess |= WRITE_OWNER;

      if ( ulFileOrPathAccess & ACCESS_CHANGE_OWNER )
        ulAccess |= WRITE_DAC;
      }
    }

  return ( ulAccess );
  }


//
// =============================== END ========================================
//
