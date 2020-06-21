/*!
  @file           vos50.c
  @author         RaymondR
  @brief          NT Registry
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




/*
 * INCLUDE FILES
 */
#include "gos00.h"
#include "heo46.h"
#include "RunTime/RTE_Types.h"

#include "heo01.h"

#if defined (KERNEL)
# include "gos00k.h"            /* nocheck */  
#endif



/*
 *  DEFINES
 */
#define MOD__  "VOS50C : "
#define MF__   MOD__"UNDEFINED"

#define RTE_REGISTRY_MAX_KEY_LENGTH     255

/*
 *  MACROS
 */
#define BUILD_SERVICE_SUBKEY(_Key)     strcpy ( _Key, REG_SK_SERVICE );

#define BUILD_EVENTLOG_SUBKEY(_Key)    strcpy ( _Key, REG_SK_EVENT_LOG );

#define BUILD_APPLIC_SUBKEY(_Key)      strcpy ( _Key, REG_SK_BASE );          \
                                       strcat ( _Key, "\\" );                 \
                                       strcat ( _Key, REG_SK_APPLIC );

#define BUILD_OLDAPPLIC_SUBKEY(_Key)   strcpy ( _Key, REG_SK_BASE );          \
                                       strcat ( _Key, "\\" );                 \
                                       strcat ( _Key, REG_SK_ADABAS_APPLIC);



/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */



/*
 * LOCAL FUNCTION PROTOTYPES
 */
static DWORD DoRecursiveDeleteKey( HKEY hKey, LPCTSTR pszSubKey ); /* PTS 1110194 */
static  RTE_SystemRc AddSubKey (SAPDB_Char *    key, 
                                SAPDB_Char *    subKey);

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

#if defined(_WIN32)

#if !defined(LIBOMS)

#if !defined (ENUMINST_ONLY)

LONG sql50_reg_del_service_value ( PSZ    pszNodeName,
                                   PSZ    pszSubKey,
                                   PSZ    pszValueName )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_del_service_value"
  APIRET    rc  = NO_ERROR;
  PATHNAME  szKey;

  DBGIN;

  BUILD_SERVICE_SUBKEY(szKey);
  rc = AddSubKey(szKey, pszSubKey);
  if (NO_ERROR == rc)
  {
    rc = sql50_reg_del_value ( pszNodeName,
                             HKEY_LOCAL_MACHINE,
                             szKey,
                             pszValueName );
  }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sql50_reg_del_service_key ( PSZ    pszNodeName,
                                 PSZ    pszSubKey )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_del_service_key"
  APIRET    rc  = NO_ERROR;
  PATHNAME  szKey;

  DBGIN;


  BUILD_SERVICE_SUBKEY(szKey);
  rc = AddSubKey(szKey, pszSubKey);
  if (NO_ERROR == rc)
  {
    rc = sql50_reg_del_key ( pszNodeName, HKEY_LOCAL_MACHINE, szKey );
  }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sql50_reg_get_service_values ( PSZ             pszNodeName,
                                    PSZ             pszSubKey,
                                    ULONG           ulNumOfEntries,
                                    PREG_ENTRY_REC  pEntyArray )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_get_service_values"
  APIRET    rc  = NO_ERROR;
  PATHNAME  szKey;

  DBGIN;

  BUILD_SERVICE_SUBKEY(szKey);
  rc = AddSubKey(szKey, pszSubKey);
  if (NO_ERROR == rc)
  {
    rc = sql50_reg_get_values ( pszNodeName, HKEY_LOCAL_MACHINE, szKey,
                              ulNumOfEntries, pEntyArray );
  }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sql50_reg_put_service_values ( PSZ             pszNodeName,
                                    PSZ             pszSubKey,
                                    ULONG           ulNumOfEntries,
                                    PREG_ENTRY_REC  pEntyArray )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_put_service_values"
  APIRET    rc  = NO_ERROR;
  PATHNAME  szKey;
  SECURITY_ATTRIBUTES   SA;
  ACCESS_RIGHTS_REC     Access;

  DBGIN;

  SA.nLength                = sizeof(SA);
  SA.bInheritHandle         = FALSE;
  SA.lpSecurityDescriptor   = NULL;

  memset ( &Access, 0, sizeof (Access) );  // - this must be done first!
  Access.fDescOwnerIsAdmin      = TRUE;
  Access.fAddDefaultDACL        = FALSE;
  Access.Creator.ulAccess       = KEY_ALL_ACCESS;
  Access.Creator.ulAceFlags     = CONTAINER_INHERIT_ACE;
  Access.Admin.ulAccess         = KEY_ALL_ACCESS;
  Access.Admin.ulAceFlags       = CONTAINER_INHERIT_ACE;
  Access.System.ulAccess        = KEY_ALL_ACCESS;
  Access.System.ulAceFlags      = CONTAINER_INHERIT_ACE;
  Access.SAPDBOp.ulAccess       = KEY_ALL_ACCESS;
  Access.SAPDBOp.ulAceFlags     = CONTAINER_INHERIT_ACE;
  Access.DomSAPDBOp.ulAccess    = KEY_ALL_ACCESS;
  Access.DomSAPDBOp.ulAceFlags  = CONTAINER_INHERIT_ACE;
  Access.World.ulAccess         = KEY_READ           |
                                  KEY_CREATE_SUB_KEY |
                                  KEY_ENUMERATE_SUB_KEYS;
  Access.World.ulAceFlags       = CONTAINER_INHERIT_ACE;

  rc = sql49c_alloc_and_init_SD( &Access, &SA.lpSecurityDescriptor );

  if ( rc == NO_ERROR )
    {
    BUILD_SERVICE_SUBKEY(szKey);
    rc = AddSubKey(szKey, pszSubKey);
    if (NO_ERROR == rc)
    {
        rc = sql50_reg_put_values ( pszNodeName, HKEY_LOCAL_MACHINE, szKey,
                                    ulNumOfEntries, pEntyArray, &SA );
    }

    if ( SA.lpSecurityDescriptor )
      FREE_MEM ( SA.lpSecurityDescriptor );
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/
#endif /* ENUMINST_ONLY        PTS 1107355 FF 18-AUG-2000 */
#endif /* LIBOMS               PTS 1107355 FF 18-AUG-2000 */
#if !defined (ENUMINST_ONLY) /*PTS 1107355 FF 18-AUG-2000 */

LONG  sql50_reg_applic_key_exists ( PSZ    pszNodeName,
                                    HKEY   hKey,
                                    PSZ    pszSubKey )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_applic_key_exists"
  PATHNAME  szKey;
  APIRET    rc  = NO_ERROR;

  DBGPAS;

  if (( hKey != HKEY_LOCAL_MACHINE ) && ( hKey != HKEY_CURRENT_USER ))
    return ( ERROR_INVALID_PARAMETER );

  BUILD_APPLIC_SUBKEY(szKey);
  rc = AddSubKey(szKey, pszSubKey);
  if (NO_ERROR == rc)
  {
    rc = sql50_reg_key_exists ( pszNodeName, hKey, szKey );
  }

  return ( rc );
  }

/*------------------------------*/

LONG sql50_reg_del_applic_value ( PSZ    pszNodeName,
                                  HKEY   hKey,
                                  PSZ    pszSubKey,
                                  PSZ    pszValueName )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_del_applic_value"
  APIRET    rc  = NO_ERROR;
  PATHNAME  szKey;

  DBGIN;

  if (( hKey != HKEY_LOCAL_MACHINE ) && ( hKey != HKEY_CURRENT_USER ))
    return ( ERROR_INVALID_PARAMETER );

  BUILD_APPLIC_SUBKEY(szKey);
  rc = AddSubKey(szKey, pszSubKey);
  if (NO_ERROR == rc)
  {
    rc = sql50_reg_del_value ( pszNodeName, hKey, szKey, pszValueName );
  }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sql50_reg_del_applic_key ( PSZ    pszNodeName,
                                HKEY   hKey,
                                PSZ    pszSubKey )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_del_applic_key"
  APIRET    rc  = NO_ERROR;
  PATHNAME  szKey;

  DBGIN;

  if (( hKey != HKEY_LOCAL_MACHINE ) && ( hKey != HKEY_CURRENT_USER ))
    return ( ERROR_INVALID_PARAMETER );

  BUILD_APPLIC_SUBKEY(szKey);
  rc = AddSubKey(szKey, pszSubKey);
  if (NO_ERROR == rc)
  {
    rc = sql50_reg_del_key ( pszNodeName, hKey, szKey );
  }

  DBGOUT;
  return ( rc );
  }
/*------------------------------*/

LONG sql50_RegistryDeleteApplSupKey (HKEY            hKey,
                                     SAPDB_Char *    pszSubKey)
{
#undef  MF__
#define MF__ MOD__"sql50_RegistryDeleteApplSupKey"
    APIRET    rc  = NO_ERROR;
    PATHNAME  szKey;

    DBGIN;

    BUILD_APPLIC_SUBKEY (szKey);
    rc = AddSubKey(szKey, pszSubKey);
    if (NO_ERROR == rc)
    {
        /* Remove Registry Subkey */
        rc = RegDeleteKey (hKey, szKey);
    }
    DBGOUT;
    return ( rc );
}

/*------------------------------*/
#endif  /* ENUMINST_ONLY         PTS 1107355 FF 18-AUG-2000 */

LONG sql50_reg_get_applic_values ( PSZ             pszNodeName,
                                   HKEY            hKey,
                                   PSZ             pszSubKey,
                                   ULONG           ulNumOfEntries,
                                   PREG_ENTRY_REC  pEntyArray )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_get_applic_value"
  APIRET    rc  = NO_ERROR;
  PATHNAME  szKey;

  DBGIN;

  if (( hKey != HKEY_LOCAL_MACHINE ) && ( hKey != HKEY_CURRENT_USER ))
    return ( ERROR_INVALID_PARAMETER );

  BUILD_APPLIC_SUBKEY(szKey);
  rc = AddSubKey(szKey, pszSubKey);
  if (NO_ERROR == rc)
  {
    rc = sql50_reg_get_values ( pszNodeName, hKey, szKey, ulNumOfEntries, pEntyArray );
  }

  DBGOUT;
  return ( rc );
  }

#if !defined(LIBOMS) /* LIBOMS   PTS 1107355 FF 18-AUG-2000 */
/*------------------------------*/

LONG sql50_reg_get_oldapplic_values ( PSZ             pszNodeName,
                                      HKEY            hKey,
                                      PSZ             pszSubKey,
                                      ULONG           ulNumOfEntries,
                                      PREG_ENTRY_REC  pEntyArray )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_get_applic_value"
  APIRET    rc  = NO_ERROR;
  PATHNAME  szKey;

  DBGIN;

  if (( hKey != HKEY_LOCAL_MACHINE ) && ( hKey != HKEY_CURRENT_USER ))
    return ( ERROR_INVALID_PARAMETER );

  BUILD_OLDAPPLIC_SUBKEY(szKey);
  rc = AddSubKey(szKey, pszSubKey);
  if (NO_ERROR == rc)
  {
    rc = sql50_reg_get_values ( pszNodeName, hKey, szKey, ulNumOfEntries, pEntyArray );
  }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/
#endif /* LIBOMS */

#if !defined (ENUMINST_ONLY)  

/*#if !defined(LIBOMS)   PTS 1107355 FF 18-Aug-2000 */

LONG sql50_reg_put_applic_values ( PSZ              pszNodeName,
                                   SAPDB_Char *     accountName,
                                   HKEY             hKey,
                                   PSZ              pszSubKey,
                                   PSZ              pszPrivateKey,
                                   SAPDB_Char *     subSubKey,
                                   ULONG            ulNumOfEntries,
                                   PREG_ENTRY_REC   pEntyArray )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_put_applic_values"
  APIRET    rc  = NO_ERROR;
  PATHNAME  szKey;
  ACCESS_RIGHTS_REC     Access;
  SECURITY_ATTRIBUTES   PrivateSA;
  SECURITY_ATTRIBUTES   SA;
  PSECURITY_ATTRIBUTES  pSA = NULL;

  DBGIN;

  if (( hKey != HKEY_LOCAL_MACHINE ) && ( hKey != HKEY_CURRENT_USER ))
    return ( ERROR_INVALID_PARAMETER );

  SA.nLength                     = sizeof(SA);
  SA.bInheritHandle              = FALSE;
  SA.lpSecurityDescriptor        = NULL;

  PrivateSA.nLength              = sizeof(PrivateSA);
  PrivateSA.bInheritHandle       = FALSE;
  PrivateSA.lpSecurityDescriptor = NULL;


  if ( hKey == HKEY_LOCAL_MACHINE )
    {
    pSA = &SA;

    memset ( &Access, 0, sizeof (Access) );  // - this must be done first!
    Access.fDescOwnerIsAdmin      = TRUE;
    Access.fAddDefaultDACL        = FALSE;
    Access.Creator.ulAccess       = KEY_ALL_ACCESS;
    Access.Creator.ulAceFlags     = CONTAINER_INHERIT_ACE;
    Access.Admin.ulAccess         = KEY_ALL_ACCESS;
    Access.Admin.ulAceFlags       = CONTAINER_INHERIT_ACE;
    Access.System.ulAccess        = KEY_ALL_ACCESS;
    Access.System.ulAceFlags      = CONTAINER_INHERIT_ACE;
    Access.SAPDBOp.ulAccess       = KEY_ALL_ACCESS;
    Access.SAPDBOp.ulAceFlags     = CONTAINER_INHERIT_ACE;
    Access.DomSAPDBOp.ulAccess    = KEY_ALL_ACCESS;
    Access.DomSAPDBOp.ulAceFlags  = CONTAINER_INHERIT_ACE;
    Access.World.ulAccess         = KEY_READ           |
                                    KEY_CREATE_SUB_KEY |
                                    KEY_ENUMERATE_SUB_KEYS;
    Access.World.ulAceFlags       = CONTAINER_INHERIT_ACE;

    rc = sql49c_alloc_and_init_SD( &Access, &SA.lpSecurityDescriptor );

    if ( rc != NO_ERROR )
      {
      DBGOUT;
      return ( rc );
      }
    }

  BUILD_APPLIC_SUBKEY(szKey);
  rc = AddSubKey(szKey, pszSubKey);
  if (NO_ERROR != rc)
  {
    if ( SA.lpSecurityDescriptor != NULL )
      FREE_MEM ( SA.lpSecurityDescriptor );
    DBGOUT;
    return ( rc );
  }

  rc = sql50_create_reg_key ( pszNodeName, hKey, szKey, pSA );
  if ( rc != NO_ERROR )
    {
    if ( SA.lpSecurityDescriptor != NULL )
      FREE_MEM ( SA.lpSecurityDescriptor );
    DBGOUT;
    return ( rc );
    }


  if (( pszPrivateKey != NULL ) && ( pszPrivateKey[0] != '\0'))
    {
    memset ( &Access, 0, sizeof (Access) );  // - this must be done first!
    Access.fDescOwnerIsAdmin  = FALSE;
    Access.fAddDefaultDACL    = FALSE;

    if (NULL != accountName)
    {
        Access.ignoreOwnUser  = TRUE;
        Access.pUseOtherUser  = accountName; 
        Access.OtherUser.ulAccess      = KEY_ALL_ACCESS;
        Access.OtherUser.ulAceFlags    = CONTAINER_INHERIT_ACE;
    }
    else
    {
        Access.User.ulAccess      = KEY_ALL_ACCESS;
        Access.User.ulAceFlags    = CONTAINER_INHERIT_ACE;
    }

    Access.Admin.ulAccess     = KEY_ENUMERATE_SUB_KEYS |
                                KEY_QUERY_VALUE        |
                                KEY_NOTIFY             | 
                                KEY_SET_VALUE          |
                                DELETE;                 

    Access.Admin.ulAceFlags   = CONTAINER_INHERIT_ACE;
    Access.System.ulAccess    = KEY_ALL_ACCESS;
    Access.System.ulAceFlags  = CONTAINER_INHERIT_ACE;

    rc = sql49c_alloc_and_init_SD( &Access, &PrivateSA.lpSecurityDescriptor );

    if ( rc != NO_ERROR )
      {
      if ( SA.lpSecurityDescriptor != NULL )
        FREE_MEM ( SA.lpSecurityDescriptor );

      DBGOUT;
      return ( rc );
      }

    rc = AddSubKey(szKey, pszPrivateKey);

    if (NO_ERROR == rc &&
        ( subSubKey != NULL ) && ( subSubKey[0] != '\0'))
    {
        rc = sql50_create_reg_key ( pszNodeName, hKey, szKey, &PrivateSA);

        if (NO_ERROR == rc)
        {
            rc = AddSubKey (szKey, subSubKey);
        }
    }

    if (NO_ERROR == rc)
    {
        rc = sql50_reg_put_values  ( pszNodeName, hKey, szKey, ulNumOfEntries,
                                     pEntyArray, &PrivateSA );
    }

    if ( PrivateSA.lpSecurityDescriptor != NULL )
       FREE_MEM ( PrivateSA.lpSecurityDescriptor );
    }
  else
    {
        if ( (subSubKey != NULL) && (subSubKey[0] != '\0'))
        {
            rc = AddSubKey(szKey, subSubKey);
        }

       if (NO_ERROR == rc)
       {
            rc = sql50_reg_put_values  (pszNodeName, hKey, szKey, ulNumOfEntries,
                                        pEntyArray, pSA );
       }
    }

  if ( SA.lpSecurityDescriptor != NULL )
    FREE_MEM ( SA.lpSecurityDescriptor );

  DBGOUT;
  return ( rc );
  }

/*#endif * LIBOMS * PTS 1107355 FF 18-Aug-2000 */
/*------------------------------*/

RTE_SystemRc    sql50_SetKeySecurity (SAPDB_Char *     accountName,
                                      HKEY             hKey,
                                      SAPDB_Char *     subKey, 
                                      SAPDB_Char *     privateKey,
                                      SAPDB_Char *     subSubKey)
{
#undef  MF__
#define MF__ MOD__"sql50_SetPrivateKeySecurity"
    RTE_SystemRc        rc  = RTE_SYSTEMRC_NO_ERROR;
    RTE_Path            szKey;
    ACCESS_RIGHTS_REC   Access;
    HKEY                hCreateKey;


    DBGIN;

    if ((hKey != HKEY_LOCAL_MACHINE ) && ( hKey != HKEY_CURRENT_USER))
        return ERROR_INVALID_PARAMETER;

    BUILD_APPLIC_SUBKEY(szKey);
    rc = AddSubKey(szKey, subKey);
    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        DBGOUT;
        return rc;
    }

    if ((privateKey != NULL ) && (privateKey[0] != '\0'))
    {
        SECURITY_ATTRIBUTES   PrivateSA;

        PrivateSA.nLength              = sizeof(PrivateSA);
        PrivateSA.bInheritHandle       = FALSE;
        PrivateSA.lpSecurityDescriptor = NULL;

        memset (&Access, 0, sizeof (Access));  // - this must be done first!
        Access.fDescOwnerIsAdmin  = FALSE;
        Access.fAddDefaultDACL    = FALSE;

        if (NULL != accountName)
        {
            Access.ignoreOwnUser  = TRUE;
            Access.pUseOtherUser  = accountName; 
            Access.OtherUser.ulAccess      = KEY_ALL_ACCESS;
            Access.OtherUser.ulAceFlags    = CONTAINER_INHERIT_ACE;
        }
        else
        {
            Access.User.ulAccess      = KEY_ALL_ACCESS;
            Access.User.ulAceFlags    = CONTAINER_INHERIT_ACE;
        }

        Access.Admin.ulAccess     = KEY_ENUMERATE_SUB_KEYS |
            KEY_QUERY_VALUE        |
            KEY_NOTIFY             | 
            KEY_SET_VALUE          |
            DELETE;                 

        Access.Admin.ulAceFlags   = CONTAINER_INHERIT_ACE;
        Access.System.ulAccess    = KEY_ALL_ACCESS;
        Access.System.ulAceFlags  = CONTAINER_INHERIT_ACE;

        rc = sql49c_alloc_and_init_SD(&Access, &PrivateSA.lpSecurityDescriptor);

        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            DBGOUT;
            return rc;
        }

        rc = AddSubKey(szKey, privateKey);
        if (RTE_SYSTEMRC_NO_ERROR == rc)
        {
            rc = RegOpenKeyEx(hKey, szKey, 0, WRITE_DAC, &hCreateKey);
            if (RTE_SYSTEMRC_NO_ERROR == rc)
            {
                rc = RegSetKeySecurity (hCreateKey, DACL_SECURITY_INFORMATION, PrivateSA.lpSecurityDescriptor);
                RegCloseKey ( hCreateKey );
            }
        }

        if (PrivateSA.lpSecurityDescriptor != NULL)
            FREE_MEM (PrivateSA.lpSecurityDescriptor);
    }
    else
    {
        SECURITY_ATTRIBUTES   SA;

        SA.nLength                     = sizeof(SA);
        SA.bInheritHandle              = FALSE;
        SA.lpSecurityDescriptor        = NULL;


        if (hKey == HKEY_LOCAL_MACHINE)
        {
            memset ( &Access, 0, sizeof (Access) );  // - this must be done first!
            Access.fDescOwnerIsAdmin      = TRUE;
            Access.fAddDefaultDACL        = FALSE;
            Access.Creator.ulAccess       = KEY_ALL_ACCESS;
            Access.Creator.ulAceFlags     = CONTAINER_INHERIT_ACE;
            Access.Admin.ulAccess         = KEY_ALL_ACCESS;
            Access.Admin.ulAceFlags       = CONTAINER_INHERIT_ACE;
            Access.System.ulAccess        = KEY_ALL_ACCESS;
            Access.System.ulAceFlags      = CONTAINER_INHERIT_ACE;
            Access.SAPDBOp.ulAccess       = KEY_ALL_ACCESS;
            Access.SAPDBOp.ulAceFlags     = CONTAINER_INHERIT_ACE;
            Access.DomSAPDBOp.ulAccess    = KEY_ALL_ACCESS;
            Access.DomSAPDBOp.ulAceFlags  = CONTAINER_INHERIT_ACE;
            Access.World.ulAccess         = KEY_READ           |
                KEY_CREATE_SUB_KEY |
                KEY_ENUMERATE_SUB_KEYS;
            Access.World.ulAceFlags       = CONTAINER_INHERIT_ACE;

            rc = sql49c_alloc_and_init_SD(&Access, &SA.lpSecurityDescriptor);

            if (RTE_SYSTEMRC_NO_ERROR != rc)
            {
                DBGOUT;
                return rc;
            }
        }

        if ( (subSubKey != NULL) && (subSubKey[0] != '\0'))
        {
            rc = AddSubKey(szKey, subSubKey);
        }

        if (RTE_SYSTEMRC_NO_ERROR == rc)
        {
            rc = RegOpenKeyEx(hKey, szKey, 0, WRITE_DAC, &hCreateKey);
            if (RTE_SYSTEMRC_NO_ERROR == rc)
            {
                rc = RegSetKeySecurity (hCreateKey, DACL_SECURITY_INFORMATION, SA.lpSecurityDescriptor);
                RegCloseKey (hCreateKey);
            }
        }
        if (SA.lpSecurityDescriptor != NULL)
        {
            FREE_MEM (SA.lpSecurityDescriptor);
        }
    }


    DBGOUT;
    return rc;
}

/*------------------------------*/

LONG sql50_reg_key_exists ( PSZ      pszNodeName,
                            HKEY     hKey,
                            PSZ      pszSubKey )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_key_exists"
  APIRET    rc  = NO_ERROR;
  HKEY      hCreateKey;
  CHAR      szComputerName[2 + MX_NODEID + 1];

  DBGIN;

  if ( pszNodeName != NULL )
    {
    szComputerName[sizeof(szComputerName) - 1] = '\0';
    strcpy  ( szComputerName, "\\\\" );
    strncat ( szComputerName, pszNodeName, MX_NODEID );

    rc = RegConnectRegistry ( szComputerName, hKey, &hKey );

    if ( rc != ERROR_SUCCESS)
      {
      DBGOUT;
      return ( rc );
      }
    }

  rc = RegOpenKeyEx( hKey, pszSubKey, 0, KEY_READ, &hCreateKey );

  if ( rc == ERROR_SUCCESS)
    {
    RegCloseKey ( hCreateKey );

    DBGOUT;
    return ( NO_ERROR );
    }


  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sql50_reg_del_value ( PSZ      pszNodeName,
                           HKEY     hKey,
                           PSZ      pszSubKey,
                           PSZ      pszValueName )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_del_value"
  APIRET    rc  = NO_ERROR;
  HKEY      hCreateKey;
  CHAR      szComputerName[2 + MX_NODEID + 1];

  DBGIN;

  if ( pszNodeName != NULL )
    {
    szComputerName[sizeof(szComputerName) - 1] = '\0';
    strcpy  ( szComputerName, "\\\\" );
    strncat ( szComputerName, pszNodeName, MX_NODEID );

    rc = RegConnectRegistry ( szComputerName, hKey, &hKey );

    if ( rc != ERROR_SUCCESS)
      {
      DBGOUT;
      return ( rc );
      }
    }

  rc = RegOpenKeyEx( hKey, pszSubKey, 0, KEY_SET_VALUE, &hCreateKey );

  if ( rc == ERROR_SUCCESS)
    {
    rc = RegDeleteValue ( hCreateKey, pszValueName );

    RegCloseKey ( hCreateKey );

    if ( rc == ERROR_SUCCESS )
      {
      DBGOUT;
      return ( NO_ERROR );
      }
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sql50_reg_del_key ( PSZ      pszNodeName,
                         HKEY     hKey,
                         PSZ      pszSubKey )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_del_key"
  APIRET    rc  = NO_ERROR;
  HKEY      hCreateKey;
  PATHNAME  szKey;
  PSZ       pszDelKey;
  CHAR      szComputerName[2 + MX_NODEID + 1];

  DBGIN;

  pszDelKey = strrchr( pszSubKey, '\\');

  if ( pszDelKey != NULL )
    {
    *pszDelKey = '\0';
    strcpy ( szKey, pszSubKey );
    *pszDelKey = '\\';
    pszDelKey++;

    if ( strlen(szKey) == 0 )
      pszSubKey = "\\";
    else
      pszSubKey = szKey;
    }
  else
    {
    pszDelKey = pszSubKey;
    pszSubKey = "";
    }

  if ( pszNodeName != NULL )
    {
    szComputerName[sizeof(szComputerName) - 1] = '\0';
    strcpy  ( szComputerName, "\\\\" );
    strncat ( szComputerName, pszNodeName, MX_NODEID );

    rc = RegConnectRegistry ( szComputerName, hKey, &hKey );

    if ( rc != ERROR_SUCCESS)
      {
      DBGOUT;
      return ( rc );
      }
    }

  rc = RegOpenKeyEx( hKey, pszSubKey, 0, KEY_ALL_ACCESS, &hCreateKey );

  if ( rc == ERROR_SUCCESS)
    {
    rc = RegDeleteKey ( hCreateKey, pszDelKey );

    if ( rc != ERROR_SUCCESS)
      rc = DoRecursiveDeleteKey( hCreateKey, pszDelKey ); /* PTS 1110193 */

    RegCloseKey ( hCreateKey );

    if ( rc == ERROR_SUCCESS )
      {
      DBGOUT;
      return ( NO_ERROR );
      }
    }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

#endif  

LONG sql50_reg_get_values ( PSZ                  pszNodeName,
                            HKEY                 hKey,
                            PSZ                  pszSubKey,
                            ULONG                ulNumOfEntries,
                            PREG_ENTRY_REC       pEntyArray )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_get_values"
  APIRET    rc  = NO_ERROR;
  ULONG     ulType;
  HKEY      hCreateKey;
  ULONG     ulEntry;
  CHAR      szComputerName[2 + MX_NODEID + 1];

  DBGIN;

  if ( pszNodeName != NULL )
    {
    szComputerName[sizeof(szComputerName) - 1] = '\0';
    strcpy  ( szComputerName, "\\\\" );
    strncat ( szComputerName, pszNodeName, MX_NODEID );

    rc = RegConnectRegistry ( szComputerName, hKey, &hKey );

    if ( rc != ERROR_SUCCESS)
      {
      for ( ulEntry = 0; ulEntry < ulNumOfEntries; ulEntry++ )
        pEntyArray[ulEntry].rc = rc;

      DBGOUT;
      return ( rc );
      }
    }

  rc = RegOpenKeyEx( hKey, pszSubKey, 0, KEY_READ, &hCreateKey );

  if ( rc == ERROR_SUCCESS)
    {
    for ( ulEntry = 0; ulEntry < ulNumOfEntries; ulEntry++ )
      {
      pEntyArray[ulEntry].rc = RegQueryValueEx( hCreateKey,
                                                pEntyArray[ulEntry].pszValueName,
                                                0,
                                                &ulType,
                                                pEntyArray[ulEntry].pValue,
                                                &pEntyArray[ulEntry].ulValueSize);

      if ( pEntyArray[ulEntry].rc != ERROR_SUCCESS )
        rc                     = pEntyArray[ulEntry].rc;
      else if (( ulType != pEntyArray[ulEntry].ulValueType ) &&
               ( ulType != 0 ))  // -- any type alllowed!
        {
        pEntyArray[ulEntry].rc = ERROR_BAD_FORMAT;
        rc                     = ERROR_BAD_FORMAT;
        }
      else
        pEntyArray[ulEntry].rc = NO_ERROR;
      }

    RegCloseKey ( hCreateKey );
    }
  else
    {
    for ( ulEntry = 0; ulEntry < ulNumOfEntries; ulEntry++ )
      pEntyArray[ulEntry].rc = rc;
    }

  if ( rc == ERROR_SUCCESS )
    rc = NO_ERROR;

  DBGOUT;
  return ( rc );
  }


/*------------------------------*/

#if !defined (ENUMINST_ONLY)  

LONG sql50_reg_put_values ( PSZ                  pszNodeName,
                            HKEY                 hKey,
                            PSZ                  pszSubKey,
                            ULONG                ulNumOfEntries,
                            PREG_ENTRY_REC       pEntyArray,
                            PSECURITY_ATTRIBUTES pSA )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_reg_put_values"
  APIRET    rc = NO_ERROR;
  HKEY      hCreateKey;
  ULONG     ulDisposition;
  ULONG     ulEntry;
  CHAR      szComputerName[2 + MX_NODEID + 1];

  tsp00_Pathc     Path;
  tsp01_RteError  RteError;

  DBGIN;

  if(sqlGetPortableInstallationRootPath ( Path,TERM_WITH_DELIMITER_EO01,&RteError ))
    return ERROR_ACCESS_DENIED;

  if ( pszNodeName != NULL )
    {
    szComputerName[sizeof(szComputerName) - 1] = '\0';
    strcpy  ( szComputerName, "\\\\" );
    strncat ( szComputerName, pszNodeName, MX_NODEID );

    rc = RegConnectRegistry ( szComputerName, hKey, &hKey );

    if ( rc != ERROR_SUCCESS)
      {
      for ( ulEntry = 0; ulEntry < ulNumOfEntries; ulEntry++ )
        pEntyArray[ulEntry].rc = rc;

      DBGOUT;
      return ( rc );
      }
    }

    rc = RegCreateKeyEx(hKey, pszSubKey, 0, REG_SK_APPLIC,
                        REG_OPTION_NON_VOLATILE,
                        KEY_SET_VALUE, pSA, &hCreateKey,
                        &ulDisposition );

  if ( rc == ERROR_SUCCESS )
    {
    for ( ulEntry = 0; ulEntry < ulNumOfEntries; ulEntry++ )
      {
      if ( pEntyArray[ulEntry].ulValueType == REG_SZ ||
           pEntyArray[ulEntry].ulValueType == REG_EXPAND_SZ )
        {
        pEntyArray[ulEntry].ulValueSize =
                   lstrlen (pEntyArray[ulEntry].pValue) + 1;
        }

      pEntyArray[ulEntry].rc = RegSetValueEx( hCreateKey,
                                              pEntyArray[ulEntry].pszValueName,
                                              0,
                                              pEntyArray[ulEntry].ulValueType,
                                              pEntyArray[ulEntry].pValue,
                                              pEntyArray[ulEntry].ulValueSize);

      if ( pEntyArray[ulEntry].rc != ERROR_SUCCESS )
        rc = pEntyArray[ulEntry].rc;
      else
        pEntyArray[ulEntry].rc = NO_ERROR;
      }

    RegCloseKey ( hCreateKey );

    if ( rc == ERROR_SUCCESS )
      {
      DBGOUT;
      return ( NO_ERROR );
      }
    }
  else
    {
    for ( ulEntry = 0; ulEntry < ulNumOfEntries; ulEntry++ )
      pEntyArray[ulEntry].rc = rc;
    }


  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sql50_create_reg_key ( PSZ                  pszNodeName,
                            HKEY                 hKey,
                            PSZ                  pszSubKey,
                            PSECURITY_ATTRIBUTES pSA )
  {
  #undef  MF__
  #define MF__ MOD__"sql50_create_reg_key"
  APIRET    rc  = NO_ERROR;
  HKEY      hCreateKey;
  ULONG     ulDisposition;
  CHAR      szComputerName[2 + MX_NODEID + 1];

  DBGIN;

  if ( pszNodeName != NULL )
    {
    szComputerName[sizeof(szComputerName) - 1] = '\0';
    strcpy  ( szComputerName, "\\\\" );
    strncat ( szComputerName, pszNodeName, MX_NODEID );

    rc = RegConnectRegistry ( szComputerName, hKey, &hKey );

    if ( rc != ERROR_SUCCESS)
      {
      DBGOUT;
      return ( rc );
      }
    }

  rc = RegCreateKeyEx( hKey, pszSubKey, 0, "Application Data",
                       REG_OPTION_NON_VOLATILE,
                       KEY_CREATE_SUB_KEY, pSA, &hCreateKey,
                       &ulDisposition );

  if ( rc == ERROR_SUCCESS )
    {
    RegCloseKey ( hCreateKey );
    }


  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG    sql50_RegistryEnumOpen (HKEY                 hKey,
                                SAPDB_Char *         pSubKey,
                                PHKEY                phKeyEnum)
  {
  #undef  MF__
  #define MF__ MOD__"sql50_RegistryEnumOpen"
  PATHNAME          szKey;
  RTE_SystemRc      rc = RTE_SYSTEMRC_NO_ERROR;


  if (( hKey != HKEY_LOCAL_MACHINE ) && ( hKey != HKEY_CURRENT_USER ))
    return ( ERROR_INVALID_PARAMETER );

  BUILD_APPLIC_SUBKEY(szKey);
  rc = AddSubKey(szKey, pSubKey);
  if (RTE_SYSTEMRC_NO_ERROR == rc)
  {
    rc = RegOpenKeyEx(hKey,
                      szKey,
                      0,
                      KEY_READ | KEY_ENUMERATE_SUB_KEYS,
                      phKeyEnum);
  }

  return rc;
  }

/*------------------------------*/

LONG    sql50_RegistryClose (HKEY                 hKey)
  {
  #undef  MF__
  #define MF__ MOD__"sql50_RegistryClose"
  RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;

  return RegCloseKey (hKey);
  }

/*------------------------------*/
LONG    sql50_RegistryEnumNext (HKEY                 hKey,
                                DWORD *              pIndex,
                                SAPDB_Char *         entry,
                                SAPDB_Int4           maxEntryLength,
                                SAPDB_Char *         string,
                                SAPDB_Int4           maxStringLength)
  {
  #undef  MF__
  #define MF__ MOD__"sql50_RegistryEnumNext"
  RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;
  HKEY                hkSub;
  DWORD               cbType;

  rc = RegEnumKey(hKey, *pIndex, entry, maxEntryLength);
  if (rc == RTE_SYSTEMRC_NO_ERROR) 
  {
      rc = RegOpenKeyEx(hKey, entry, 0, KEY_READ, &hkSub);
      if ( rc == RTE_SYSTEMRC_NO_ERROR ) 
      {
          rc = RegQueryValueEx( hkSub, "", 0, &cbType, string, (LPDWORD)&maxStringLength );  
          RegCloseKey(hkSub);
      }
      (*pIndex)++;
  }

  return rc;
  }
#endif

#endif

/*
 * ========================== LOCAL FUNCTIONS =================================
 */


/* PTS 1110194 */
/*
  Function: DoRecursiveDeleteKey
  Description: Delete all subSubKeys of given subKey

  On WindwosNT/Windows2000 subKeys that contain subSubKeys cannot be deleted.
  This function enumerates all subSubKeys and recursively calls itself if
  deleting the subSubKeys fails.

  Arguments: hKey [in] The Key name
             pszSubKey [in] The subKey name
 */
static DWORD DoRecursiveDeleteKey( HKEY hKey, LPCTSTR pszSubKey )
{
  LONG  rc = ERROR_SUCCESS;
  HKEY  hSubKey;
  DWORD dwSubKeyLength;
  TCHAR subSubKeyName[ MAX_PATH + 1 ];

  rc = RegOpenKeyEx( hKey,pszSubKey, 0, KEY_ENUMERATE_SUB_KEYS | DELETE, &hSubKey );

  while ( rc == ERROR_SUCCESS )
  {
    dwSubKeyLength = MAX_PATH;

    rc = RegEnumKeyEx( hSubKey, 0, subSubKeyName, &dwSubKeyLength, NULL, NULL, NULL, NULL );
    
    if ( rc == ERROR_NO_MORE_ITEMS )
    {
      rc = RegDeleteKey( hKey, pszSubKey );
      break;
    }
    else if ( rc == ERROR_SUCCESS )
      rc = DoRecursiveDeleteKey( hSubKey, subSubKeyName );
  }
  RegCloseKey( hSubKey );

  return rc;
}

/*------------------------------*/

static  RTE_SystemRc AddSubKey (SAPDB_Char *    key, 
                                SAPDB_Char *    subKey)
{
    RTE_SystemRc    rc = RTE_SYSTEMRC_NO_ERROR;
    if (NULL != key &&
        (subKey != NULL) && (subKey[0] != '\0'))
    {
        if (strlen (key) + strlen (subKey) + 1 <= RTE_REGISTRY_MAX_KEY_LENGTH)
        {
            strcat ( key, "\\" );
            strcat ( key, subKey );
        }
        else
        {
            rc = ERROR_BADKEY;
        }
    }

    return rc;
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
