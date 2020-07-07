/*!
  @file           vos13u.c
  @author         RaymondR
  @brief          XUSER
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
#include "gos00.h"
#include "heo46.h"
#include "heo13.h"
#include "geo13.h"
#include "hos13u.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "geo43_0.h"


//
//  DEFINES
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


 static VOID   sql13u_crypt_name_to_c20    ( CRYPTNAME          alCryptedName,
                                             C20                acC20Array[MAX_CRYPT] );
 static VOID   sql13u_recrypt_name         ( C20                acReCryptName[MAX_CRYPT],
                                             CRYPTNAME          alCryptedName );
 static LONG   sql13u_read_data_from_reg   ( SAPDB_Char *       accountName,
                                             PCHAR *            ppcDataBuffer,
                                             ULONG *            pulDataLen,
                                             tsp00_ErrText      acErrorText );
 static LONG   sql13u_write_data_to_reg    ( SAPDB_Char *       accountName,
                                             PCHAR              pcDataBuffer,
                                             ULONG              ulDataLen,
                                             tsp00_ErrText      acErrorText );

 static BOOL sql13u_use_hkey_local_machine ( void ) ;

 
//
// ========================== GLOBAL FUNCTIONS ================================
//

tsp00_Int sql13u_get_all_entries (SAPDB_Char *              accountName, 
                                  char                      **pcDataBuffer,
                                  tsp00_Int4                *DataLen,
                                  PREL611_XUSER_PAGE_UNION  *pRel611XUserPages,
                                  tsp00_ErrText               acErrorText )
  {
  #undef  MF__
  #define MF__ MOD__"sql13u_get_all_entries"

  LONG   rc = NO_ERROR;

  *pRel611XUserPages = NULL;

  rc = sql13u_read_data_from_reg(accountName, 
                                 (PCHAR *)pcDataBuffer,
                                 DataLen, 
                                 acErrorText );

  if ( rc != NO_ERROR )
     *pcDataBuffer = (char *)NULL;
    
  return ( rc ) ;
  }

 /*------------------------------*/

tsp00_Int sql13u_remove_xuser_entries (SAPDB_Char *     accountName,
                                       tsp00_ErrText    acErrorText )
   {
   #undef  MF__
   #define MF__ MOD__"sql13u_remove_xuser_entries"
   LONG        rc = NO_ERROR;
   PSZ         pszTextualSID;
   PATHNAME    szSubKey;


   if (( sql02_get_platform_id()         == VER_PLATFORM_WIN32_NT ) &&
       ( sql13u_use_hkey_local_machine() == TRUE ))
   {
       rc = sql49c_get_user_textual_SID (accountName, &pszTextualSID);
       if ( rc != NO_ERROR )
       {
           DBGOUT;
           return ( rc );
       }

       strcpy ( szSubKey, REG_VN_USER );
       strcat ( szSubKey, "\\" );
       strcat ( szSubKey, pszTextualSID );


       /* Remove Registry value */
       rc = sql50_reg_del_applic_value ( NULL, HKEY_LOCAL_MACHINE,
           szSubKey, REG_VN_XUSER );

       if (NO_ERROR == rc || ERROR_FILE_NOT_FOUND == rc)
       {
           rc = sql50_RegistryDeleteApplSupKey  (HKEY_LOCAL_MACHINE, szSubKey);
       }

       FREE_MEM ( pszTextualSID );
   }
   else
   {
       rc = sql50_reg_del_applic_value ( NULL, HKEY_CURRENT_USER,
           "", REG_VN_XUSER );
   }


   if (( rc != NO_ERROR ) && ( rc !=  ERROR_FILE_NOT_FOUND ))
   {
       DBG1  (( MF__, ERRMSG_REMOVE_XUSER_REG_DATA, rc ));
       MSGD  (( ERR_REMOVE_XUSER_REG_DATA, rc ));
       sql46c_build_error_string ( acErrorText, ERRMSG_REMOVE_XUSER_REG_DATA,
           rc );

       DBGOUT;
       return ( rc );
   }


   DBGOUT;
   return ( NO_ERROR );
   }


 /*------------------------------*/

tsp00_Int sql13u_write_xuser_entries (SAPDB_Char *      accountName,
                                      char              *cDataBuffer,
                                      PXUSER_INFO_PAGE   pXUserInfoPage,
                                      tsp00_ErrText        acErrorText )
   {
   #undef  MF__
   #define MF__ MOD__"sql13u_write_xuser_entries"
   LONG                      rc  = NO_ERROR;
   ULONG                     ulDataLen ;

   ulDataLen = pXUserInfoPage->ulInfoPageSize +
               (pXUserInfoPage->ulPages * pXUserInfoPage->ulPageSize);

   rc = sql13u_write_data_to_reg(accountName, cDataBuffer, ulDataLen, acErrorText);

   DBGOUT;
   return ( rc );
   }


 /*------------------------------*/


BOOLEAN sql13u_user_is_guest ( void )
  {
  #undef  MF__
  #define MF__ MOD__"sql13u_user_is_guest"
  PSZ             pszUserName;
  LONG            rc = NO_ERROR;


  DBGIN;

  rc = sql49c_get_user_info ( &pszUserName, NULL );

  if ( rc != NO_ERROR )
    {
    DBGOUT;
    return ( FALSE );
    }

  pszUserName = CharUpper( pszUserName );

  if ( !strcmp ( SYSTEM_GUEST_NAME, pszUserName ) )
    {
    FREE_MEM ( pszUserName );
    DBGOUT;
    return ( TRUE );
    }

  FREE_MEM ( pszUserName );

  DBGOUT;
  return ( FALSE );
  }


//
// ========================== LOCAL FUNCTIONS =================================
//

static void sql13u_crypt_name_to_c20 ( tsp00_CryptName   alCryptedName,
                                       tsp00_C20         acC20Array[MAX_CRYPT] )
  {
  #undef  MF__
  #define MF__ MOD__"sql13u_crypt_name_to_c20"
  int nIndex;

  DBGIN;

  for ( nIndex = 0; nIndex < MAX_CRYPT; nIndex++ )
    {
    /* --- change crypted name to crypted string */
    sql21write_crypt ( alCryptedName [ nIndex ], acC20Array [ nIndex ] );
    }

  DBGOUT;
  return;
  }

/*------------------------------*/

static void sql13u_recrypt_name ( tsp00_C20         acReCryptName[MAX_CRYPT],
                                  tsp00_CryptName   alCryptedName )
  {
  #undef  MF__
  #define MF__ MOD__"sql13u_recrypt_name"
  int nIndex;


  DBGIN;

  for ( nIndex = 0; nIndex < MAX_CRYPT; nIndex++ )
    {
    /* --- change crypted string to crypted name */
    alCryptedName [ nIndex ] = sql21read_crypt ( acReCryptName [ nIndex ]);
    }

  DBGOUT;
  return;
  }


 static BOOL sql13u_use_hkey_local_machine ( void )
   {
   #undef  MF__
   #define MF__ MOD__"sql13u_use_hkey_local_machine"
   LONG            rc = NO_ERROR;
   REG_ENTRY_REC   RegistryEntries[1];
   DWORD           dwPos = 0;

   RegistryEntries[0].pszValueName = REG_VN_USE_XUDATA_POS;
   RegistryEntries[0].pValue       = &dwPos;
   RegistryEntries[0].ulValueSize  = sizeof ( dwPos );
   RegistryEntries[0].ulValueType  = REG_DWORD;

   rc = sql50_reg_get_applic_values ( NULL, HKEY_LOCAL_MACHINE,
                                       "", 1, RegistryEntries );

   if ( rc != NO_ERROR )
     {
     DBGOUT;
     return ( TRUE );
     }

   DBGOUT;
   return ( dwPos == 0 );
   }

 /*------------------------------*/

static LONG sql13u_read_data_from_reg (SAPDB_Char *     accountName, 
                                       PCHAR *          ppcDataBuffer,
                                       ULONG *          pulDataLen,
                                       tsp00_ErrText    acErrorText )
   {
   #undef  MF__
   #define MF__ MOD__"sql13u_read_data_from_reg"

   LONG                rc               = NO_ERROR;
   LONG                rc_free          = NO_ERROR;
   ULONG               ulDataBufferSize = XUSER_DATA_SIZE ;
   PCHAR               pcDataBuffer;
   REG_ENTRY_REC       RegistryEntries[1];
   PSZ                 pszTextualSID;
   PATHNAME            szSubKey;
   HKEY                hKey;

   *ppcDataBuffer = NULL;
   *pulDataLen    = 0;

   if (( sql02_get_platform_id()         == VER_PLATFORM_WIN32_NT ) &&
       ( sql13u_use_hkey_local_machine() == TRUE ))
   {
     rc = sql49c_get_user_textual_SID (accountName, &pszTextualSID);
     if ( rc != NO_ERROR )
       {
       sql46c_build_error_string ( acErrorText, ERRMSG_SEC_GET_USER_SID, rc );
       DBGOUT;
       return ( rc );
       }

     strcpy ( szSubKey, REG_VN_USER );
     strcat ( szSubKey, "\\" );
     strcat ( szSubKey, pszTextualSID );

     hKey = HKEY_LOCAL_MACHINE;

     FREE_MEM ( pszTextualSID );
     }
   else
     {
     hKey        = HKEY_CURRENT_USER;
     szSubKey[0] = '\0';
     }

   do
     {
     rc = ALLOC_MEM ( &pcDataBuffer, ulDataBufferSize );

     if ( rc != NO_ERROR )
       {
       sql46c_build_error_string ( acErrorText, ERRMSG_ALLOC_MEMORY, rc );
       DBGOUT;
       return( rc );
       }

     RegistryEntries[0].pszValueName = REG_VN_XUSER;
     RegistryEntries[0].pValue       = pcDataBuffer;
     RegistryEntries[0].ulValueSize  = ulDataBufferSize;
     RegistryEntries[0].ulValueType  = REG_BINARY;

     rc = sql50_reg_get_applic_values ( NULL, hKey, szSubKey, 1,
                                        RegistryEntries );

     if ( rc == ERROR_MORE_DATA )
       {
       rc_free = FREE_MEM ( pcDataBuffer );

       if ( rc_free != NO_ERROR )
         {
         sql46c_build_error_string ( acErrorText, ERRMSG_CANT_FREE_MEM, rc_free );
         DBGOUT;
         return( rc_free );
         }
       ulDataBufferSize = RegistryEntries[0].ulValueSize;
       }
     }
   while ( rc == ERROR_MORE_DATA );


   if ( rc != NO_ERROR )
     {
     FREE_MEM ( pcDataBuffer );
     }
   else
     {
     *ppcDataBuffer = pcDataBuffer;
     *pulDataLen    = RegistryEntries[0].ulValueSize;
     }

   DBGOUT;
   return ( rc );
   }

 /*------------------------------*/

static LONG sql13u_write_data_to_reg    (SAPDB_Char *      accountName, 
                                         PCHAR             pcDataBuffer,
                                         ULONG             ulDataLen,
                                         tsp00_ErrText     acErrorText )
{
#undef  MF__
#define MF__ MOD__"sql13u_write_data_to_reg"
    LONG                rc               = NO_ERROR;
    REG_ENTRY_REC       RegistryEntries[1];
    PSZ                 pszTextualSID;
    PATHNAME            szPrivateSubKey;
    PATHNAME            szSubKey;


    RegistryEntries[0].pszValueName = REG_VN_XUSER;
    RegistryEntries[0].pValue       = pcDataBuffer;
    RegistryEntries[0].ulValueSize  = ulDataLen;
    RegistryEntries[0].ulValueType  = REG_BINARY;

    if (( sql02_get_platform_id()         == VER_PLATFORM_WIN32_NT ) &&
        ( sql13u_use_hkey_local_machine() == TRUE ))
    {
        rc = sql49c_get_user_textual_SID (accountName, &pszTextualSID);

        if ( rc != NO_ERROR )
        {
            DBGOUT;
            return ( rc );
        }

        strcpy ( szPrivateSubKey, pszTextualSID );
        FREE_MEM ( pszTextualSID );

        /* Registry subkey and value are first removed */ 
        /* Remove Registry value */

        strcpy ( szSubKey, REG_VN_USER );
        strcat ( szSubKey, "\\" );
        strcat ( szSubKey, szPrivateSubKey );

        rc = sql50_reg_del_applic_value ( NULL, HKEY_LOCAL_MACHINE,
            szSubKey, REG_VN_XUSER );

       if (NO_ERROR == rc || ERROR_FILE_NOT_FOUND == rc)
       {
           rc = sql50_RegistryDeleteApplSupKey  (HKEY_LOCAL_MACHINE, szSubKey);
       }

        if (( rc != NO_ERROR ) && ( rc !=  ERROR_FILE_NOT_FOUND ))
        {
            DBG1  (( MF__, ERRMSG_REMOVE_XUSER_REG_DATA, rc ));
            MSGD  (( ERR_REMOVE_XUSER_REG_DATA, rc ));
            sql46c_build_error_string ( acErrorText, ERRMSG_REMOVE_XUSER_REG_DATA,
                rc );

            DBGOUT;
            return ( rc );
        }


        rc = sql50_reg_put_applic_values ( NULL, accountName, HKEY_LOCAL_MACHINE,
            REG_VN_USER, szPrivateSubKey, NULL,
            1, RegistryEntries );
    }
    else
    {
        rc = sql50_reg_put_applic_values ( NULL, accountName, HKEY_CURRENT_USER,
            "", "", NULL, 1, RegistryEntries );
    }

    if ( rc != NO_ERROR )
    {
        DBG1  (( MF__, ERRMSG_WRITE_XUSER_REG_DATA, rc ));
        MSGD  (( ERR_WRITE_XUSER_REG_DATA, rc ));
        sql46c_build_error_string ( acErrorText, ERRMSG_WRITE_XUSER_REG_DATA,
            rc );
        DBGOUT;
        return ( rc );
    }

    DBGOUT;
    return ( NO_ERROR );
}

 /*------------------------------*/
/*
static LONG sql13u_check_user_id (SAPDB_Char *      accountName,
                                  PXUSER_INFO_PAGE  pInfoPage,
                                  tsp00_ErrText       acErrorText )
  {
  #undef  MF__
  #define MF__ MOD__"sql13u_check_user_id"
  PSZ             pszSrc;
  PSZ             pszDest;
  tsp00_CryptName   alXUserCryptedName;
  tsp00_CryptName   alCryptedName;
  PSZ             pszTextualSID;
  tsp00_C20        acTextualSID;
  int             nIndex;
  ULONG           ulLen;
  ULONG           ulMaxLen;
  LONG            rc = NO_ERROR;
  #if defined WIN95_USER_CHECK
   PSZ            pszUserName;
   tsp00_C20        acUserName;
  #endif

  DBGIN;

  sql13u_recrypt_name ( pInfoPage->uidField, alXUserCryptedName );

  if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
    {
    rc = sql49c_get_user_textual_SID (accountName, &pszTextualSID);

    if ( rc != NO_ERROR )
      {
      sql46c_build_error_string ( acErrorText, ERRMSG_SEC_GET_USER_SID, rc );
      DBGOUT;
      return ( rc );
      }

    if ( strlen ( pszTextualSID ) > sizeof (acTextualSID) )
      {
      ulMaxLen = sizeof (acTextualSID);
      ulLen    = (ULONG)strlen ( pszTextualSID );
      pszDest  = pszTextualSID + (ulMaxLen / 2);
      pszSrc   = pszTextualSID + ulLen - (ulMaxLen / 2);
      SAPDB_memcpy ( pszDest, pszSrc, (ulMaxLen / 2) );
      pszTextualSID[sizeof(tsp00_C20)] = '\0';
      }

    eo46CtoP ( acTextualSID, pszTextualSID, sizeof (acTextualSID));
    FREE_MEM ( pszTextualSID );

    /* --- change textual SID to type "tsp00_CryptName" *
    sql21put_name ( acTextualSID, alCryptedName );
    }
  else
    {
    #if defined WIN95_USER_CHECK
     rc = sql49c_get_user_info ( &pszUserName, NULL );

     if ( rc != NO_ERROR )
       {
       sql46c_build_error_string( acErrorText, ERRMSG_GET_LOGON_USER_NAME, rc );
       DBGOUT;
       return ( rc );
       }

     if ( strlen ( pszUserName ) > 20 )
       pszUserName[sizeof(tsp00_C20)] = '\0';

     eo46CtoP ( acUserName, pszUserName, sizeof (acUserName));
     FREE_MEM ( pszUserName );

     /* --- change eff. user  from type "tsp00_C20" to type "tsp00_CryptName" *
     sql21put_name ( acUserName, alCryptedName );
    #else
     DBGOUT;
     return ( NO_ERROR );
    #endif
    }

  /* --- test of equality *
  for ( nIndex = 0; ( rc == NO_ERROR ) && ( nIndex < MAX_CRYPT ); nIndex++ )
    {
    if ( alCryptedName [ nIndex ] != alXUserCryptedName [ nIndex ] )
      {
      DBG3    (( MF__, ERRMSG_DIFFERENT_SIDS ));
      MSGD    (( ERR_DIFFERENT_SIDS ));
      sql46c_build_error_string ( acErrorText, ERRMSG_DIFFERENT_SIDS, 0 );

      DBGOUT;
      return ( ERROR_ACCESS_DENIED );
      }
    }

  DBGOUT;
  return ( rc );
  }
*/
 /*------------------------------*/

 APIRET sql13u_get_options_char_set_map ( BOOL *pfEnabled,
                                          PSZ   pszCharSetName,
                                          LONG  lMxCharSetName )
   {
   #undef  MF__
   #define MF__ MOD__"sql13u_get_options_char_set_map"
   APIRET           rc          = NO_ERROR;
   REG_ENTRY_REC    RegistryEntries[2];

   DBGPAS;

   RegistryEntries[0].pszValueName = REG_VN_CHAR_SET_ENABLED;
   RegistryEntries[0].pValue       = pfEnabled;
   RegistryEntries[0].ulValueSize  = sizeof(DWORD);
   RegistryEntries[0].ulValueType  = REG_DWORD;

   RegistryEntries[1].pszValueName = REG_VN_CHAR_SET_NAME;
   RegistryEntries[1].pValue       = pszCharSetName;
   RegistryEntries[1].ulValueSize  = lMxCharSetName + 1;
   RegistryEntries[1].ulValueType  = REG_SZ;

   rc = sql50_reg_get_applic_values ( NULL, HKEY_CURRENT_USER, "", 2,
                                      RegistryEntries );

   if (( rc != NO_ERROR ) && ( rc != ERROR_FILE_NOT_FOUND ))
     {
     MSGCD (( ERR_READ_CHAR_SET_OPT_VALS, rc ));
     return ( rc );
     }

   return ( rc );
   }

 /*------------------------------*/

 APIRET sql13u_get_options_sqlopt ( BOOL             *pfEnabled,
                                    PSZ                 pszSQLOpt,
                                    LONG                lMxSQLOpt )
   {
   #undef  MF__
   #define MF__ MOD__"sql13u_get_options_sqlopt"
   APIRET           rc = NO_ERROR;
   REG_ENTRY_REC    RegistryEntries[2];

   DBGPAS;

   RegistryEntries[0].pszValueName = REG_VN_SQLOPT_ENABLED;
   RegistryEntries[0].pValue       = pfEnabled;
   RegistryEntries[0].ulValueSize  = sizeof(DWORD);
   RegistryEntries[0].ulValueType  = REG_DWORD;

   RegistryEntries[1].pszValueName = REG_VN_SQLOPT;
   RegistryEntries[1].pValue       = pszSQLOpt;
   RegistryEntries[1].ulValueSize  = lMxSQLOpt + 1;
   RegistryEntries[1].ulValueType  = REG_SZ;

   rc = sql50_reg_get_applic_values ( NULL, HKEY_CURRENT_USER, "",
                                      2, RegistryEntries );

   if (( rc != NO_ERROR ) && ( rc != ERROR_FILE_NOT_FOUND ))
     {
     MSGCD (( ERR_READ_SQLOPT_OPT_VALS, rc ));
     return ( rc );
     }

   return ( rc );
   }


/*------------------------------*/

void _stdcall sc_sqlxuopenuser ( SAPDB_Char *   accountName,
                                 tsp00_ErrText  acErrorText,
                                 BOOLEAN     *  pbOk)
  {
  #undef  MF__
  #define MF__ MOD__"sc_sqlxuopenuser"

  DBGPAS;

  sqlxuopenuser (accountName, acErrorText, pbOk);
  }

/*------------------------------*/

void _stdcall sc_sqlxucloseuser ( SAPDB_Char *   accountName,
                                  tsp00_ErrText acErrorText,
                                  BOOLEAN     *pbOk )
  {
  #undef  MF__
  #define MF__ MOD__"sc_sqlxucloseuser"

  DBGPAS;

  sqlxucloseuser (accountName, acErrorText, pbOk );
  }

/*------------------------------*/

void _stdcall sc_sqlputuser ( tsp4_xuser_record  *  prcUserParams,
                              SAPDB_Char *          accountName,
                              tsp00_ErrText         acErrorText,
                              BOOLEAN            *  pbOk )
  {
  #undef  MF__
  #define MF__ MOD__"sc_sqlputuser"

  DBGPAS;

  sqlputuser ( prcUserParams, accountName, acErrorText, pbOk );
  }

/*------------------------------*/

void _stdcall sc_sqlgetuser ( tsp4_xuser_record  *  prcUserParams,
                              SAPDB_Char *          accountName,
                              tsp00_ErrText         acErrorText,
                              BOOLEAN            *  pbOk )
  {
  #undef  MF__
  #define MF__ MOD__"sc_sqlgetuser"

  DBGPAS;

  sqlgetuser ( prcUserParams, accountName, acErrorText, pbOk );
  }

/*------------------------------*/

void _stdcall sc_sqlindexuser ( tsp00_Int2            kUserIndex,
                                tsp4_xuser_record  *prcUserParams,
                                SAPDB_Char *          accountName,
                                tsp00_ErrText        acErrorText,
                                BOOLEAN          *pbOk )
  {
  #undef  MF__
  #define MF__ MOD__"sc_sqlindexuser"

  DBGPAS;

  sqlindexuser ( kUserIndex, prcUserParams, accountName, acErrorText, pbOk );
  }

/*------------------------------*/

int _stdcall sc_sqlclearuser (SAPDB_Char *          accountName)
  {
  #undef  MF__
  #define MF__ MOD__"sc_sqlclearuser"

  DBGPAS;

  return ( sqlclearuser (accountName) );
  }

//
// =============================== END ========================================
//
