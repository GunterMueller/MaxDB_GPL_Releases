/*!
  @file           veo13.c
  @author         J. Mensing (jrg)
  @special area   XUSER
  @brief          XUser file and registry API
  @first created  2000-09-26  17:38
  @Original AuthorR.Roedling

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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "geo13.h"

#include "geo00_0.h"
#if defined(_WIN32)
# include "vsp002c.h" /* nocheck */
#endif

#include "heo13.h"
#include "geo001.h"
#include "geo57.h"
#include "heo46.h"
#include "geo007_1.h"
#include "geo007_2.h"

#if defined(_WIN32)
# include "hos13u.h" /* nocheck */
#endif

#include "hsp100.h"
#include "hsp78_0.h"
#include "hsp02.h"
#include "SAPDBCommon/SAPDB_string.h"
#include "heo21.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MOD__  "veo13.c:"
#undef  MF__
#define MF__   MOD__"UNDEFINED"

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/* PTS 1001667 */
static struct
{
        XUSER_INFO_PAGE   xUserInfoPage;
        XUSER_PAGE        xUserPages[MX_XUSER_ENTRIES];
} cDataBuffer;
static PXUSER_INFO_PAGE   pXUserInfoPage    = &cDataBuffer.xUserInfoPage;
static PXUSER_PAGE        pXUserPageBuf     = &cDataBuffer.xUserPages[0];
static bool            fXuserBufEmpty    = true;
static bool            fReadDataIsNewer  = false;

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/* Used by XUser API and sqlarg3 */
static tsp00_Int4   eo13_putUser        (tsp4_xuser_record  *   prcUserParams,
                                         SAPDB_Char *           accountName,
                                         tsp00_ErrText          acErrorText );

static bool   eo13_xuserKeyIsBlank      (tsp4_xuserkey          acXUserKey );

static bool   eo13_findXuserKey         (tsp4_xuserkey          acXUserKey,
                                         tsp00_Uint4 *          pulPage );

static tsp00_Int4 eo13_readXuserEntries (SAPDB_Char *           accountName,
                                         tsp00_ErrText          acErrorText );

static tsp00_Int4   eo13_checkUserId    (SAPDB_Char *           accountName,
                                         PXUSER_INFO_PAGE       pInfoPage,
                                         tsp00_ErrText          acErrorText );

static tsp00_Int4   eo13_createInfoPage (SAPDB_Char *           accountName, 
                                         tsp00_ErrText          acErrorText);

static void   eo13_cryptNameToC20       (tsp00_CryptName        alCryptedName,
                                         tsp00_C20              acC20Array[csp_cryptname] );

static void   eo13_recryptName          (tsp00_C20              acReCryptName[csp_cryptname],
                                         tsp00_CryptName        alCryptedName );

static void cn90CalculateUncrypt        (const tsp00_CryptPw    pwCrypted,
                                         char *                 passwd,
                                         bool                   bForDatabase);

static void eo13_CopyAsciiToUCS2        (tsp00_Uint2 *          destUCS2, 
                                         const tsp00_Byte *     srcAscii, 
                                         int srcLen);

static bool eo13_CheckPureAscii         (const tsp00_Uint2 *    srcUCS2, 
                                         int                    srcLenInBytes);

/* PTS 1109583 */
static void eo13_SetupUCS2Entries( tsp00_KnlIdentifier userUCS2, 
                                   tsp00_CryptPw cryptedPasswordUCS2,
                                   tsp00_KnlIdentifier userAscii,
                                   tsp00_CryptPw cryptedPasswordAscii );
/*===========================================================================*
 *=====================  END OF DECLARATIONS ================================*
 *===========================================================================*/

/*===========================================================================*
 *  EXPORTED FUNCTIONS, EXPORTED CLASS METHODS (IMPLEMENTATION)              *
 *===========================================================================*/

void sqlxuopenuser (SAPDB_Char *   accountName,
                    tsp00_ErrText  acErrorText,
                    bool *         pbOk)
{
  #undef  MF__
  #define MF__ MOD__"sqlxuopenuser"
  tsp00_Int4      rc;

  DBGIN;

  SAPDB_memset (( char* ) acErrorText, ' ', sizeof ( tsp00_ErrText ));
  *pbOk = FALSE;

  rc = eo13_readXuserEntries (accountName, acErrorText);

  if ( rc != NO_ERROR_EO001 )
    {
    DBGOUT;
    return;
    }

  *pbOk = TRUE;

  DBGOUT;
  return;
}

/*------------------------------*/

void sqlxucloseuser (SAPDB_Char *   accountName,
                     tsp00_ErrText  acErrorText,
                     bool     *     pbOk )
{
  #undef  MF__
  #define MF__ MOD__"sqlxucloseuser"
  tsp00_Int4      rc;

  DBGIN;

  /* --- initialize outgoing parms */
  SAPDB_memset (( char* ) acErrorText, ' ', sizeof ( tsp00_ErrText ));
  *pbOk = FALSE;

  if ( fReadDataIsNewer )
   {
   eo46BuildPascalErrorStringRC ( acErrorText, ERRMSG_DATA_NEWER_THAN_COMPONENT, 0 );
   DBGOUT;
   return;
   }

  if ( fXuserBufEmpty == FALSE )
    {
    rc = sql13u_write_xuser_entries (accountName, 
                                     (char *)&cDataBuffer,
                                     pXUserInfoPage, 
                                     acErrorText );

    if ( rc != NO_ERROR_EO001 )
      {
      DBGOUT;
      return;
      }
    }

  /* --- reset global vars */
  *pbOk = TRUE;

  DBG3 (( MF__, "bReadXuserEntries = TRUE" ));

  DBGOUT;
}

/*------------------------------*/

void sqlputuser (tsp4_xuser_record *    prcUserParams,
                 SAPDB_Char *           accountName,
                 tsp00_ErrText          acErrorText,
                 bool             *     pbOk )
{
  #undef  MF__
  #define MF__ MOD__"sqlputuser"
  tsp00_Int4      rc;

  DBGIN;

  /* --- initialize outgoing parms */
  SAPDB_memset (( char* ) acErrorText, ' ', sizeof ( tsp00_ErrText ));
  *pbOk = FALSE;

  if ( fXuserBufEmpty )
    {
    rc = eo13_readXuserEntries (accountName, acErrorText);

    if ( rc != NO_ERROR_EO001 )
      {
      DBGOUT;
      return;
      }
    }

  rc = eo13_putUser (prcUserParams, accountName, acErrorText);

  if ( rc == NO_ERROR_EO001 )
   *pbOk = TRUE;


  DBGOUT;
  return;
}

/*------------------------------*/

void sqlgetuser (tsp4_xuser_record *    prcUserParams,
                 SAPDB_Char *           accountName,
                 tsp00_ErrText          acErrorText,
                 bool            *      pbOk)
{
  #undef  MF__
  #define MF__ MOD__"sqlgetuser"

  tsp00_Int4      rc;

  DBGIN;

  rc = sql13u_getuser ( prcUserParams, accountName, acErrorText );

  if ( rc != NO_ERROR_EO001 )
    {
    *pbOk = FALSE;
    DBGOUT;
    return;
    }

  *pbOk = TRUE;

  DBGOUT;
  return;
}

/*------------------------------*/

void sqlindexuser ( tsp00_Int2          kUserIndex,
                    tsp4_xuser_record  *prcUserParams,
                    SAPDB_Char *        accountName,
                    tsp00_ErrText       acErrorText,
                    bool            *   pbOk )
{
  #undef  MF__
  #define MF__ MOD__"sqlindexuser"
  tsp00_Int4      rc;

  DBGIN;

  SAPDB_memset ( acErrorText, ' ', sizeof ( tsp00_ErrText ));
  *pbOk = FALSE;

  /* --- initialize outgoing parms */
  sql13u_init_user_params ( prcUserParams );

  /* --- check user_index */
  if (( kUserIndex < 1 ) || ( kUserIndex > MX_XUSER_ENTRIES  ))
    {
    DBG1    (( MF__, ERRMSG_ILLEGAL_USER_INDEX ));
    MSGD    (( ERR_ILLEGAL_USER_INDEX, kUserIndex ));
    eo46BuildPascalErrorStringRC ( acErrorText, ERRMSG_ILLEGAL_USER_INDEX, 0 );

    DBGOUT;
    return;
    }

  if ( fXuserBufEmpty )
    {
    rc = eo13_readXuserEntries (accountName, acErrorText);

    if ( rc != NO_ERROR_EO001 )
      {
      DBGOUT;
      return;
      }
    }

  /* --- get xuser page */
  if ( (tsp00_Uint4)kUserIndex <= pXUserInfoPage->ulPages )
    {
    SAPDB_memcpy ( prcUserParams, &pXUserPageBuf[kUserIndex - 1],
             sizeof ( tsp4_xuser_record));
    *pbOk = TRUE;
    }

  DBGOUT;
  return;
}

/*------------------------------*/

int sqlclearuser (SAPDB_Char *   accountName)
{
  #undef  MF__
  #define MF__ MOD__"sqlclearuser"

  tsp00_ErrText   acErrorText;

  DBGIN;

  SAPDB_memset ( &cDataBuffer, 0, sizeof(cDataBuffer) );
  fXuserBufEmpty = TRUE;


  if ( fReadDataIsNewer )
   {
   eo46BuildPascalErrorStringRC ( acErrorText, ERRMSG_DATA_NEWER_THAN_COMPONENT, 0 );
   DBGOUT;
   return ( ERROR_ACCESS_DENIED );
   }

  if ( sql13u_remove_xuser_entries( accountName, acErrorText ) != NO_ERROR_EO001 )
    {
    DBGOUT;
    return ( FALSE );
    }


  DBGOUT;

  return ( TRUE );
}

/*---------------------------------------------------------------------------*/

void sql13u_init_user_params ( tsp4_xuser_record  *prcUserParams )
{
  #undef  MF__
  #define MF__ MOD__"sql13u_init_user_params"
  char       *pszServerDB;
  char       *pszDBLang;
  void *fillPointer;
  tsp00_Uint4 fillLength;

  DBGIN;

  /*
  // --- initialize outgoing parms
  */
  SAPDB_memset ( prcUserParams->xu_servernode, ' ',
           sizeof ( prcUserParams->xu_servernode ));
  SAPDB_memset ( prcUserParams->xu_user, ' ',
           sizeof ( prcUserParams->xu_user ));
  SAPDB_memset ( prcUserParams->xu_password, '\0',
           sizeof ( prcUserParams->xu_password ));
  SAPDB_memset ( prcUserParams->xu_sqlmode, ' ',
           sizeof ( prcUserParams->xu_sqlmode ));

  fillLength  = sizeof(prcUserParams->xu_userUCS2);
  fillPointer = &prcUserParams->xu_userUCS2[0];
  sp77encodingUCS2Native->fillString( &fillPointer, &fillLength, fillLength/2, ' ');

  SAPDB_memset ( &prcUserParams->xu_passwordUCS2[0], '\0',
           sizeof ( prcUserParams->xu_passwordUCS2 ));

  if ( GET_SERVERDB ( &pszServerDB ) )
    eo46CtoP(prcUserParams->xu_serverdb, pszServerDB,
               sizeof (prcUserParams->xu_serverdb));
  else
    SAPDB_memset ( prcUserParams->xu_serverdb, ' ',
             sizeof ( prcUserParams->xu_serverdb ));

  prcUserParams->xu_cachelimit = - 1;
  prcUserParams->xu_timeout    = - 1;
  prcUserParams->xu_isolation  = - 1;

  if ( GET_DBLANG ( &pszDBLang ) )
    eo46CtoP(prcUserParams->xu_dblang, pszDBLang,
               sizeof (prcUserParams->xu_dblang));
  else
    SAPDB_memset ( prcUserParams->xu_dblang, ' ',
             sizeof (prcUserParams->xu_dblang));

  DBGOUT;
  return;
}

/*---------------------------------------------------------------------------*/

tsp00_Int4 sql13u_getuser (tsp4_xuser_record  *prcUserParams,
                           SAPDB_Char *        accountName,
                           tsp00_ErrText       acErrorText )
{
  #undef  MF__
  #define MF__ MOD__"sql13u_getuser"

  tsp00_Int4        rc         = NO_ERROR_EO001;
  tsp00_Uint4       ulPage;

  DBGIN;

  SAPDB_memset ( acErrorText, ' ', sizeof ( tsp00_ErrText ) );

  /* --- initialize outgoing parms */
  sql13u_init_user_params ( prcUserParams );

  /* --- look for the appropriate entries identified by xuser key.
         check xuser key for blanks, it is prohibited to be clear */
  if ( eo13_xuserKeyIsBlank ( prcUserParams -> xu_key ))
    {
    DBG1    (( MF__, ERRMSG_BLANK_USERKEY ));
    MSGD    (( ERR_BLANK_USERKEY ));
    eo46BuildPascalErrorStringRC ( acErrorText, ERRMSG_BLANK_USERKEY, 0 );

    DBGOUT;
    return ( -1 );
    }

  if ( fXuserBufEmpty )
    {
    rc = eo13_readXuserEntries (accountName, acErrorText );

    if ( rc != NO_ERROR_EO001 )
      {
      DBGOUT;
      return ( rc );
      }
    }

  /* --- search for specified xuser key and get them */
  if ( eo13_findXuserKey ( prcUserParams->xu_key, &ulPage ))
    {
    SAPDB_memcpy ( prcUserParams, &pXUserPageBuf[ulPage],
             sizeof ( tsp4_xuser_record));
    }
  else
    {
    /* --- xuser key not found */
    DBG1    (( MF__, ERRMSG_NO_XUSER_ENTRY_FOUND ));
    eo46BuildPascalErrorStringRC ( acErrorText, ERRMSG_NO_XUSER_ENTRY_FOUND, 0 );

    DBGOUT;
    return ( -1 );
    }

  DBGOUT;
  return ( rc );
}

/*===========================================================================*
 *  LOCAL/INLINE FUNCTIONS, LOCAL CLASS METHODS (IMPLEMENTATION)             *
 *===========================================================================*/

static tsp00_Int4 eo13_putUser (tsp4_xuser_record  *prcUserParams,
                                SAPDB_Char *        accountName,
                                tsp00_ErrText       acErrorText )
{
  #undef  MF__
  #define MF__ MOD__"eo13_putUser"
  tsp00_Uint4   ulPage;
  char         *pszServerDB;
  char          szServerDB [ sizeof ( tsp00_DbName ) + 1 ];
  bool          fLocalOpen = false;
  bool          fAsciiOnly = false;
  tsp00_Int4    rc = NO_ERROR_EO001;
  union {
    unsigned short s;
    char c[2];
  } UCS2Blank;
  
  DBGIN;

  UCS2Blank.s = ' ';

  /* --- initialize outgoing parms */
  SAPDB_memset (( char* ) acErrorText, ' ', sizeof ( tsp00_ErrText ));
  #if defined (_WIN32)
   if ( sql13u_user_is_guest() == TRUE )
     {
     DBG1    (( MF__, ERRMSG_NO_GUEST_ACCESS ));
     MSGD    (( ERR_ACCESS_DENIED ));
     eo46BuildPascalErrorStringRC ( acErrorText, ERRMSG_NO_GUEST_ACCESS, 0 );
     DBGOUT;
     return ( ERROR_ACCESS_DENIED );
     }
  #endif

   /* PTS 1109749 */
  /*
    If no xu_userUCS2 is specified, check for given ascii user and expand ascii user/password
    into UCS2 user/UCS2 password

    If    xu_userUCS2 is specified, check if xu_userUCS2 and xu_passwordUCS2 are specified 'ascii8 only'.

    If not ascii only reset xu_user and xu_password, else set them to the ascii values
   */
  eo13_SetupUCS2Entries( prcUserParams->xu_userUCS2, prcUserParams->xu_passwordUCS2,
                         prcUserParams->xu_user,     prcUserParams->xu_password );

  /* If an UCS2 entry was filled, the corresponding ascii Entry must be filled too! */

  SAPDB_memcpy( prcUserParams->xu_user_61, prcUserParams->xu_user,
          sizeof(prcUserParams->xu_user_61) ) ;
  SAPDB_memcpy( prcUserParams->xu_dblang_61, prcUserParams->xu_dblang,
          sizeof(prcUserParams->xu_dblang_61) ) ;

  /* --- check user parameters! */
  eo46PtoC( szServerDB, prcUserParams->xu_serverdb,
               sizeof (prcUserParams->xu_serverdb) );

  /* --- no SERVERDB specified ? */
  if ( szServerDB[0] == '\0' )
    {
    if ( GET_SERVERDB ( &pszServerDB ) )
      {
      eo46CtoP( prcUserParams->xu_serverdb, pszServerDB,
                  sizeof (prcUserParams->xu_serverdb));
      }
    }

  /* --- if XUSER file exists look for the appropriate entries identified by
  //     xuser key otherwise put the info page and the "DEFAULT" user
         into the XUSER file
  */
  if ( eo13_xuserKeyIsBlank ( prcUserParams->xu_key ))
    {
    DBG1    (( MF__, ERRMSG_BLANK_USERKEY ));
    MSGD    (( ERR_BLANK_USERKEY ));
    eo46BuildPascalErrorStringRC ( acErrorText, ERRMSG_NO_XUSER_ENTRY_FOUND, 0 );

    DBGOUT;
    return ( -1 ) ;
    }

   if ( fReadDataIsNewer )
    {
    eo46BuildPascalErrorStringRC ( acErrorText, ERRMSG_DATA_NEWER_THAN_COMPONENT, 0 );

    DBGOUT;
    return ( -1 );
    }

  if ( fXuserBufEmpty )
    {
    /* --- install default user:
             1. create info page
             2. put defaultuser page */
    SAPDB_memset ( &cDataBuffer, 0, sizeof(cDataBuffer) );

    /* --- create info page */
    rc = eo13_createInfoPage (accountName, acErrorText);

    if ( rc != NO_ERROR_EO001 )
      {
      DBGOUT;
      return ( rc );
      }

    /* --- put defaultuser page */
    SAPDB_memcpy ( prcUserParams->xu_key, DEFAULT_USER, sizeof ( tsp4_xuserkey ));
    SAPDB_memcpy ( &pXUserPageBuf[0], prcUserParams, sizeof ( tsp4_xuser_record));

    pXUserInfoPage->ulPages = 1;
    fXuserBufEmpty = FALSE;
    }
  /* --- search for specified xuser key and put the parameters into the
         xuser data buffer */
  else if ( eo13_findXuserKey ( prcUserParams->xu_key, &ulPage ))
    {
    SAPDB_memcpy ( &pXUserPageBuf[ulPage], prcUserParams,
              sizeof ( tsp4_xuser_record));
    }
  else
    {
    /* --- look for free space */
    if ( pXUserInfoPage->ulPages < MX_XUSER_ENTRIES )
      {
      /* --- build up new user page */
      SAPDB_memcpy ( &pXUserPageBuf[pXUserInfoPage->ulPages], prcUserParams,
                sizeof ( tsp4_xuser_record));
      pXUserInfoPage->ulPages++;
      }
    else
      {
      DBG1    (( MF__, ERRMSG_NO_SPACE_LEFT ));
      MSGD    (( ERR_NO_SPACE_LEFT ));
      eo46BuildPascalErrorStringRC ( acErrorText, ERRMSG_NO_SPACE_LEFT, 0 );

      DBGOUT;
      return ( -1 ) ;
      }
    }

  return ( NO_ERROR_EO001 );
}

/*------------------------------*/

static bool eo13_xuserKeyIsBlank ( tsp4_xuserkey   acXUserKey )
{
  #undef  MF__
  #define MF__ MOD__"eo13_xuserKeyIsBlank"
  char szXUserKey [ sizeof(tsp4_xuserkey)+1 ] ;

  DBGPAS;

  eo46PtoC( szXUserKey, acXUserKey, sizeof(tsp4_xuserkey) );

  if ( szXUserKey[0] == '\0' )
    return ( TRUE );

  return ( FALSE );
}

/*------------------------------*/

static bool eo13_findXuserKey ( tsp4_xuserkey       acXUserKey,
                                       tsp00_Uint4     *pulPage  )
{
  #undef  MF__
  #define MF__ MOD__"eo13_findXuserKey"
  bool    fFound = FALSE;

  DBGIN;

  *pulPage  = (tsp00_Uint4)UNDEF;

  /* --- compare userkeys */
  for ( *pulPage = 0; *pulPage < MX_XUSER_ENTRIES; (*pulPage)++ )
    {
    if ( !memcmp (( char* )acXUserKey,
                  ( char* )pXUserPageBuf[*pulPage].xu_key,
                  sizeof ( tsp4_xuserkey )))
      {
      fFound = TRUE;
      break;
      }
    }


  DBGOUT;
  return ( fFound );
}

/*------------------------------*/

static tsp00_Int4 eo13_createInfoPage (SAPDB_Char *       accountName,
                                       tsp00_ErrText      acErrorText )
{
  #undef  MF__
  #define MF__ MOD__"eo13_createInfoPage"
  tsp00_CryptName      alCryptedName;
  tsp00_C20            acUserName;
  tsp00_Int4      rc = NO_ERROR_EO001;
  #if defined (_WIN32)
   char              *pszSrc;
   char              *pszDest;
   char              *pszTextualSID;
   tsp00_C20           acTextualSID;
   tsp00_Uint4    ulLen;
   tsp00_Uint4    ulMaxLen;
   #if defined WIN95_USER_CHECK
    char             *pszUserName;
   #endif
  #else
  char               acUserNameC [ sizeof ( acUserName ) + 1 ] ;
  #endif

  DBGIN;

  #if defined (_WIN32)
   if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
     {
     rc = sql49c_get_user_textual_SID (accountName, &pszTextualSID);

     if ( rc != NO_ERROR_EO001 )
       {
       eo46BuildPascalErrorStringRC ( acErrorText, ERRMSG_GET_LOGON_USER_NAME,
                                   rc );
       DBGOUT;
       return ( rc );
       }

     if ( strlen ( pszTextualSID ) > sizeof (acTextualSID) )
       {
       ulMaxLen = sizeof (acTextualSID);
       ulLen    = (tsp00_Uint4)strlen ( pszTextualSID );
       pszDest  = pszTextualSID + (ulMaxLen / 2);
       pszSrc   = pszTextualSID + ulLen - (ulMaxLen / 2);
       SAPDB_memcpy ( pszDest, pszSrc, (ulMaxLen / 2) );
       pszTextualSID[sizeof(tsp00_C20)] = '\0';
       }


     eo46CtoP ( acTextualSID, pszTextualSID, sizeof (acTextualSID));
     FREE_MEM_EO57 ( pszTextualSID );

     sql21put_name ( acTextualSID, alCryptedName );
     }
   else
     {
     #if defined WIN95_USER_CHECK
      rc = sql49c_get_user_info ( &pszUserName, NULL );

      if ( rc != NO_ERROR_EO001 )
        {
        eo46BuildPascalErrorStringRC ( acErrorText, ERRMSG_GET_LOGON_USER_NAME,
                                   rc );
        DBGOUT;
        return ( rc );
        }

      if ( strlen ( pszUserName ) > 20 )
        pszUserName[sizeof(tsp00_C20)] = '\0';


      eo46CtoP ( acUserName, pszUserName, sizeof (acUserName));
      FREE_MEM_EO57 ( pszUserName );

      sql21put_name ( acUserName, alCryptedName );
     #else
      eo46CtoP ( acUserName, "UNKOWN", sizeof (acUserName));
      sql21put_name ( acUserName, alCryptedName );
     #endif
     }
  #else
    sprintf ( acUserNameC , "%.18ld" , (long) geteuid() ) ;
    eo46CtoP ( acUserName , acUserNameC, sizeof ( acUserName ) ) ;
    sql21put_name ( acUserName, alCryptedName );
  #endif

  pXUserInfoPage->ulInfoPageSize   = sizeof (XUSER_INFO_PAGE);
  pXUserInfoPage->ulXUserVersionID = XUSER_RECORD_VERSION;
  pXUserInfoPage->ulPages          = 0;
  pXUserInfoPage->ulPageSize       = sizeof(XUSER_PAGE);
  eo13_cryptNameToC20 ( alCryptedName, pXUserInfoPage->uidField );
#ifdef WIN32
  {
  char * RTEBuildNumberEx(unsigned char * buildpraefix, unsigned long * buildnr);
  sp100_GetVersionID ( VersionIDType1_esp100,
					   RTEBuildNumberEx,
                       &pXUserInfoPage->RTEVersionID );
  }
#endif
  DBGOUT;
  return ( NO_ERROR_EO001 );
}

#define IS_USERDATA_611(_XUserVersion)   (_XUserVersion == 0 )
#define IS_USERDATA_627(_XUserVersion)   (_XUserVersion == 1 )
#define IS_USERDATA_72(_XUserVersion)    (_XUserVersion == 2 )

/*------------------------------*/

static tsp00_Int4 eo13_readXuserEntries (SAPDB_Char *       accountName, 
                                         tsp00_ErrText      acErrorText )
{
   #undef  MF__
   #define MF__ MOD__"eo13_readXuserEntries"

   tsp00_Int4             rc               = NO_ERROR_EO001;
   tsp00_Uint4            ulDataLen;
   char                   *pcDataBuffer;
   tsp00_Uint4            ulPage;
   tsp00_Uint4            ulLen;
   ULONG                     ulXUserVersionFound ; /* PTS 1104980 */
   PREL611_XUSER_PAGE_UNION  pRel611XUserPages ;

   DBGIN;

   fXuserBufEmpty = TRUE;
   SAPDB_memset ( &cDataBuffer, 0, sizeof(cDataBuffer) );

   rc = eo13_createInfoPage (accountName, acErrorText );

   if ( rc != NO_ERROR_EO001 )
     {
     DBGOUT;
     return ( rc );
     }

   rc = sql13u_get_all_entries (accountName, &pcDataBuffer, (tsp00_Int4*)&ulDataLen, &pRel611XUserPages,
                                acErrorText );

   if ( rc != NO_ERROR_EO001 )
     {
     if ( rc == ERROR_FILE_NOT_FOUND )
       { rc = NO_ERROR_EO001;
         DBGOUT;
         return ( rc );
       }
     else
       {
         DBG1    (( MF__, ERRMSG_READ_XUSER_REG_DATA ));
         MSGD    (( ERR_READ_XUSER_REG_DATA, rc ));
         eo46BuildPascalErrorStringRC ( acErrorText,
                                     ERRMSG_READ_XUSER_REG_DATA, rc );
         DBGOUT;
         return ( -1 );
       }
     }

   if ( pRel611XUserPages == NULL && pcDataBuffer != NULL )
     {
     PXUSER_INFO_PAGE         pInfoPage        ;
     pInfoPage = (PXUSER_INFO_PAGE)pcDataBuffer;
     ulLen     = pInfoPage->ulInfoPageSize +
                   (pInfoPage->ulPages * pInfoPage->ulPageSize);

     if ( ulLen > ulDataLen )
       {
       FREE_MEM_EO57 ( pcDataBuffer );

       DBG1  (( MF__, ERRMSG_WRONG_XUSER_REG_DATA_LEN ));
       MSGD  (( ERR_WRONG_XUSER_REG_DATA_LEN ));
       eo46BuildPascalErrorStringRC ( acErrorText,
                                   ERRMSG_WRONG_XUSER_REG_DATA_LEN, 0 );
       DBGOUT;
       return ( -1 );
       }

     /* --- check user-id wether you have the permissions to modify
            the XUSER content? */

     rc = eo13_checkUserId (accountName, pInfoPage, acErrorText );

     if ( rc != NO_ERROR_EO001 )
       {
       FREE_MEM_EO57 ( pcDataBuffer );
       DBGOUT;
       return ( rc );
       }

     ulXUserVersionFound = pInfoPage->ulXUserVersionID ;
     fReadDataIsNewer    = ulXUserVersionFound > XUSER_RECORD_VERSION ;

     pXUserInfoPage->ulPages = pInfoPage->ulPages;

     for ( ulPage = 0; ulPage < pXUserInfoPage->ulPages; ulPage++ )
       {
       sql13u_init_user_params ( &pXUserPageBuf[ulPage] );

       SAPDB_memcpy ( &pXUserPageBuf[ulPage],
                pcDataBuffer + pInfoPage->ulInfoPageSize +
                               (ulPage * pInfoPage->ulPageSize),
                min (pInfoPage->ulPageSize, sizeof(XUSER_PAGE) ));
       }

     FREE_MEM_EO57 ( pcDataBuffer );

     fXuserBufEmpty = FALSE;
     }
   else
     {
     ulXUserVersionFound = 0 ; 
     pXUserInfoPage->ulPages = (ulDataLen /
                                    sizeof(REL611_XUSER_PAGE_UNION)) - 1;

     for ( ulPage = 1; ulPage <= pXUserInfoPage->ulPages; ulPage++ )
       {
#ifndef WIN32
       if ( pRel611XUserPages[ulPage].xu_rec.rec_no == EOF_XUSER_PAGE )
         {
         pXUserInfoPage->ulPages = ulPage - 1 ;
         break;
         }
#endif
       sql13u_init_user_params ( &pXUserPageBuf[ulPage - 1] );

       DBG3 (( MF__, "copy old xuserentry #%d to new", ulPage  ));
       SAPDB_memcpy( &pXUserPageBuf[ulPage - 1],
               &pRel611XUserPages[ulPage].xu_rec.params,
               min (sizeof(XUSER_PAGE), sizeof(REL611_XUSER_PARAMS_REC) ));
       }

     FREE_MEM_EO57 ( pRel611XUserPages );
     fXuserBufEmpty = FALSE;
     }


   if ( IS_USERDATA_611(ulXUserVersionFound)     ||
        IS_USERDATA_627(ulXUserVersionFound) )
     {
     for ( ulPage = 1; ulPage <= pXUserInfoPage->ulPages; ulPage++ )
       {
       DBG3 (( MF__, "converting old username '%18.18s' to new", 
               pXUserPageBuf[ulPage - 1].xu_user_61  ));
       SAPDB_memcpy( pXUserPageBuf[ulPage - 1].xu_user,
               pXUserPageBuf[ulPage - 1].xu_user_61,
               sizeof(pXUserPageBuf[ulPage - 1].xu_user_61) ) ;
       if ( IS_USERDATA_627(ulXUserVersionFound) )
         { DBG3 (( MF__, "converting old dblang '%18.18s' to new", 
                   pXUserPageBuf[ulPage - 1].xu_dblang_61  ));
           SAPDB_memcpy( pXUserPageBuf[ulPage - 1].xu_dblang,
                   pXUserPageBuf[ulPage - 1].xu_dblang_61,
                   sizeof(pXUserPageBuf[ulPage - 1].xu_dblang_61) ) ;
         }
       }
   }

   if ( IS_USERDATA_611(ulXUserVersionFound) ||
        IS_USERDATA_627(ulXUserVersionFound) ||
        IS_USERDATA_72(ulXUserVersionFound) )
   {
       eo13_SetupUCS2Entries( pXUserPageBuf[ulPage - 1].xu_userUCS2, pXUserPageBuf[ulPage - 1].xu_passwordUCS2,
                              pXUserPageBuf[ulPage - 1].xu_user,     pXUserPageBuf[ulPage - 1].xu_password );
   }

   DBGOUT;
   return  ( rc );
}

/*------------------------------*/

static tsp00_Int4 eo13_checkUserId (SAPDB_Char *        accountName,
                                    PXUSER_INFO_PAGE    pInfoPage,
                                    tsp00_ErrText       acErrorText )
{
  #undef  MF__
  #define MF__ MOD__"eo13_checkUserId"
  char            *pszSrc;
  char            *pszDest;
  tsp00_CryptName   alXUserCryptedName;
  tsp00_CryptName   alCryptedName;
  char            *pszTextualSID;
  tsp00_C20        acTextualSID;
  int             nIndex;
  tsp00_Uint4  ulLen;
  tsp00_Uint4  ulMaxLen;
  tsp00_Int4   rc = NO_ERROR_EO001;
  #if defined WIN95_USER_CHECK
   char           *pszUserName;
   tsp00_C20        acUserName;
  #endif

  DBGIN;

  eo13_recryptName ( pInfoPage->uidField, alXUserCryptedName );

#ifdef WIN32
  if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
    {
    rc = sql49c_get_user_textual_SID (accountName, &pszTextualSID );

    if ( rc != NO_ERROR_EO001 )
      {
      eo46BuildPascalErrorStringRC ( acErrorText, ERRMSG_SEC_GET_USER_SID, rc );
      DBGOUT;
      return ( rc );
      }

    if ( strlen ( pszTextualSID ) > sizeof (acTextualSID) )
      {
      ulMaxLen = sizeof (acTextualSID);
      ulLen    = (tsp00_Int4)strlen ( pszTextualSID );
      pszDest  = pszTextualSID + (ulMaxLen / 2);
      pszSrc   = pszTextualSID + ulLen - (ulMaxLen / 2);
      SAPDB_memcpy ( pszDest, pszSrc, (ulMaxLen / 2) );
      pszTextualSID[sizeof(tsp00_C20)] = '\0';
      }

    eo46CtoP ( acTextualSID, pszTextualSID, sizeof (acTextualSID));
    FREE_MEM_EO57 ( pszTextualSID );

    /* --- change textual SID to type "tsp00_CryptName" */
    sql21put_name ( acTextualSID, alCryptedName );
    }
  else
    {
    #if defined WIN95_USER_CHECK
     rc = sql49c_get_user_info ( &pszUserName, NULL );

     if ( rc != NO_ERROR_EO001 )
       {
       eo46BuildPascalErrorStringRC( acErrorText, ERRMSG_GET_LOGON_USER_NAME, rc );
       DBGOUT;
       return ( rc );
       }

     if ( strlen ( pszUserName ) > 20 )
       pszUserName[sizeof(tsp00_C20)] = '\0';

     eo46CtoP ( acUserName, pszUserName, sizeof (acUserName));
     FREE_MEM_EO57 ( pszUserName );

     /* --- change eff. user  from type "tsp00_C20" to type "tsp00_CryptName" */
     sql21put_name ( acUserName, alCryptedName );
    #else
     DBGOUT;
     return ( NO_ERROR_EO001 );
    #endif
    }

#else
    { uid_t           uid ;
      tsp00_Name uid_name ;
      char     uid_name_c [ sizeof ( tsp00_Name ) + 1 ] ;
      uid = geteuid () ;
      (void) sprintf ( uid_name_c , "%.18ld" , (long) uid ) ;
      SAPDB_memset ( uid_name , ' ' , sizeof ( tsp00_Name ) ) ;
      SAPDB_memcpy ( uid_name , uid_name_c  , sizeof ( tsp00_Name ) ) ;
      sql21put_name ( uid_name, alCryptedName );
    }
#endif

  /* --- test of equality */
  for ( nIndex = 0; ( rc == NO_ERROR_EO001 ) && ( nIndex < csp_cryptname ); nIndex++ )
    {
    if ( alCryptedName [ nIndex ] != alXUserCryptedName [ nIndex ] )
      {
      DBG3    (( MF__, ERRMSG_DIFFERENT_SIDS ));
      MSGD    (( ERR_DIFFERENT_SIDS ));
      eo46BuildPascalErrorStringRC ( acErrorText, ERRMSG_DIFFERENT_SIDS, 0 );

      DBGOUT;
      return ( ERROR_ACCESS_DENIED );
      }
    }

  DBGOUT;
  return ( rc );
}


/*------------------------------*/

static void eo13_cryptNameToC20 ( tsp00_CryptName   alCryptedName,
                                       tsp00_C20         acC20Array[csp_cryptname] )
{
  #undef  MF__
  #define MF__ MOD__"eo13_cryptNameToC20"
  int nIndex;

  DBGIN;

  for ( nIndex = 0; nIndex < csp_cryptname; nIndex++ )
    {
    /* --- change crypted name to crypted string */
    sql21write_crypt ( alCryptedName [ nIndex ], acC20Array [ nIndex ] );
    }

  DBGOUT;
  return;
}


/*------------------------------*/

static void eo13_recryptName ( tsp00_C20         acReCryptName[csp_cryptname],
                                  tsp00_CryptName   alCryptedName )
{
  #undef  MF__
  #define MF__ MOD__"eo13_recryptName"
  int nIndex;


  DBGIN;

  for ( nIndex = 0; nIndex < csp_cryptname; nIndex++ )
    {
    /* --- change crypted string to crypted name */
    alCryptedName [ nIndex ] = sql21read_crypt ( acReCryptName [ nIndex ]);
    }

  DBGOUT;
  return;
}

/*---------------------------------------------------------------------------*/

/*
  Function: eo13_CopyAsciiToUCS2
  Description: Copy Pascal (space padded) Ascii8 to UCS2 string
 */
static void eo13_CopyAsciiToUCS2(tsp00_Uint2 *destUCS2, const tsp00_Byte *srcAscii, int srcLen)
{
    int i;
    for ( i = 0; (i < srcLen/2) ; i++ )
    {
        if ( srcAscii[i] != ' ' )
        {
            destUCS2[i] = (unsigned char)srcAscii[i];
        }
        else
        {
            destUCS2[i] = (unsigned short)' ';
        }
    }
}

/*
  Function: eo13_CopyUCS2ToAscii
  Description: Copy Pascal (space padded) UCS2 to Ascii8 string
 */
static void eo13_CopyUCS2ToAscii(tsp00_Byte *destAscii, const tsp00_Uint2 *srcUCS2, int srcLen)
{
    int i;
    for ( i = 0; i < (srcLen/2) ; i++ )
    {
        destAscii[i] = (unsigned char)(srcUCS2[i] & 0x0FF);
    }
    SAPDB_memset( &destAscii[srcLen/2], ' ', srcLen/2 );
}

/*
  Function: eo13_CheckPureAscii
  Description: Check on all characters in given Pascal (space padded) UCS2 string for non Ascii8 characters
  Return value: True if pure ascii, false if any non ascii contained
 */
static bool eo13_CheckPureAscii(const tsp00_Uint2 *srcUCS2, int srcLenInBytes)
{
    int i;
    for ( i = 0; (i < srcLenInBytes/2) ; i++ )
    {
        if ( (srcUCS2[i] & 0x00ff) != srcUCS2[i] )
        {
            return false;
        }
    }
    return true;
}

/* PTS 1109749 */
/*
  Function:eo13_PadSpacesAscii
  Description: Replaces all zeros at end of buffer with ascii spaces
 */
static void eo13_PadSpacesAscii( char *buffer, int len )
{
    int i = len - 1;

    while ( i >= 0 )
    {
        if ( buffer[i] == 0 )
        {
            buffer[i] = ' ';
            --i;
        }
        else
        {
            break;
        }
    }
}

/*
  Function:eo13_PadSpacesUCS2
  Description: Replaces all zeros at end of buffer with UCS2 native spaces
 */
static void eo13_PadSpacesUCS2( unsigned short *buffer, int len )
{
    int i = (len/2) - 1;

    while ( i >= 0 )
    {
        if ( buffer[i] == 0 )
        {
            buffer[i] = (unsigned short)' ';
            --i;
        }
        else
        {
            break;
        }
    }
}

/*
  Function: eo13_prepareBlankPasswords
  Description: Create crypted versions of blank ascii and UCS2 passwords
 */
static void eo13_prepareBlankPasswords( tsp00_CryptPw cryptedPasswordAscii, tsp00_CryptPw cryptedPasswordUCS2 )
{
    char asciiblanks[sizeof(tsp00_Pw)];
    unsigned short ucs2blanks[sizeof(tsp00_Pw)/2];

    SAPDB_memset(asciiblanks, ' ', sizeof(tsp00_Pw));
    s02applencrypt ( (unsigned char *)&asciiblanks[0], cryptedPasswordAscii );

    SAPDB_memset(ucs2blanks, 0, sizeof(ucs2blanks));
    eo13_PadSpacesUCS2( ucs2blanks, (int)sizeof(ucs2blanks));
    s02applencrypt ( (unsigned char *)&ucs2blanks[0], cryptedPasswordUCS2 );
}

/*
  Function: eo13_SetupUCS2Entries
  Description: Handle user/password in UCS2/Ascii

  If no xu_userUCS2 is specified, check for given ascii user and expand ascii user/password
  into UCS2 user/UCS2 password

  If    xu_userUCS2 is specified, check if xu_userUCS2 and xu_passwordUCS2 are specified 'ascii8 only'.

  If not ascii only reset xu_user and xu_password, else set them to the ascii values
 */
static void eo13_SetupUCS2Entries( tsp00_KnlIdentifier userUCS2, 
                                   tsp00_CryptPw cryptedPasswordUCS2,
                                   tsp00_KnlIdentifier userAscii,
                                   tsp00_CryptPw cryptedPasswordAscii )
{
    union {
        unsigned short s;
        char c[2];
    } UCS2Blank;
    tsp00_CryptPw blankCryptedPasswordAscii;
    tsp00_CryptPw blankCryptedPasswordUCS2;
    tsp00_CryptPw emptyCryptedPassword;

    UCS2Blank.s = (unsigned short)' ';

    eo13_prepareBlankPasswords( blankCryptedPasswordAscii, blankCryptedPasswordUCS2 );
    SAPDB_memset( emptyCryptedPassword, 0, sizeof(tsp00_CryptPw) );

    if ( (memcmp( &userUCS2[0], &UCS2Blank.c[0], 2 ) == 0) 
      || ( *(unsigned short *) &userUCS2[0] == 0 ) )
    {
        if ( userAscii[0] == ' ' )
        {
            void *ucs2String = &userUCS2[0];
            unsigned int fillLength = sizeof(tsp00_KnlIdentifier);
            sp77encodingUCS2Native->fillString(
                &ucs2String, 
                &fillLength,
                (unsigned int)sizeof(tsp00_KnlIdentifier),
                ' ');
            
            SAPDB_memcpy( cryptedPasswordAscii, blankCryptedPasswordAscii, sizeof(tsp00_CryptPw) );
            SAPDB_memcpy( cryptedPasswordUCS2,  blankCryptedPasswordUCS2,  sizeof(tsp00_CryptPw) );
            return;
        }

        eo13_CopyAsciiToUCS2( (tsp00_Uint2 *)&userUCS2[0],
                              (const tsp00_Byte *)&userAscii[0],
                              (int)sizeof(tsp00_KnlIdentifier) );

        if ( (0 != memcmp(cryptedPasswordAscii, blankCryptedPasswordAscii, sizeof(tsp00_CryptPw)) )
          && (0 != memcmp(cryptedPasswordAscii, emptyCryptedPassword, sizeof(tsp00_CryptPw)) ) )
        {
            tsp00_Name passwordAscii;
            tsp00_Name passwordUCS2;

            s02appldecrypt( passwordAscii, cryptedPasswordAscii );
            eo13_PadSpacesAscii( (char *)&passwordAscii[0], (int)sizeof(tsp00_Name) );

            eo13_CopyAsciiToUCS2( (tsp00_Uint2 *)&passwordUCS2[0],
                                  (const tsp00_Byte *)&passwordAscii[0],
                                  (int)sizeof(tsp00_Name) );

            s02applencrypt ( passwordUCS2, cryptedPasswordUCS2 );
        }
        else
        {
            SAPDB_memcpy( cryptedPasswordAscii, blankCryptedPasswordAscii, sizeof(tsp00_CryptPw) );
            SAPDB_memcpy( cryptedPasswordUCS2,  blankCryptedPasswordUCS2,  sizeof(tsp00_CryptPw) );
        }
    }
    else
    {
        bool pureAscii8 = eo13_CheckPureAscii( (tsp00_Uint2 *)&userUCS2[0], (int)sizeof(tsp00_KnlIdentifier) );

        if ( pureAscii8 )
        {
            if ( (0 != memcmp( cryptedPasswordUCS2,  blankCryptedPasswordUCS2,  sizeof(tsp00_CryptPw)) )
              && (0 != memcmp(cryptedPasswordAscii, emptyCryptedPassword, sizeof(tsp00_CryptPw)) ) )
            {
                tsp00_Name passwordUCS2;
                tsp00_Name passwordAscii;

                s02appldecrypt( passwordUCS2, cryptedPasswordUCS2 );
                eo13_PadSpacesUCS2( (unsigned short *)&passwordUCS2[0], (int)sizeof(tsp00_Name) );

                pureAscii8 = eo13_CheckPureAscii( (tsp00_Uint2 *)&passwordUCS2[0], (int)sizeof(tsp00_Pw) );

                if ( pureAscii8 )
                {
                    eo13_CopyUCS2ToAscii( (tsp00_Byte *)&passwordAscii[0],
                                          (const tsp00_Uint2 *)&passwordUCS2[0],
                                          (int)sizeof(tsp00_Name) );
                    SAPDB_memset(&passwordAscii[sizeof(tsp00_Name)/2], ' ', sizeof(tsp00_Name)/2); 
                    /* needed half of it not filled with spaces */
                    s02applencrypt ( passwordAscii, cryptedPasswordAscii );
                }
            }
            else
            {
                SAPDB_memcpy( cryptedPasswordAscii, blankCryptedPasswordAscii, sizeof(tsp00_CryptPw) );
            }
        }

        if ( pureAscii8 )
        {
            eo13_CopyUCS2ToAscii( (tsp00_Byte *)&userAscii[0],
                                  (const tsp00_Uint2 *)&userUCS2[0],
                                  (int)sizeof(tsp00_KnlIdentifier) );
            SAPDB_memset(&userAscii[sizeof(tsp00_KnlIdentifier)/2], ' ', sizeof(tsp00_KnlIdentifier)/2); 
            /* needed since rest is filled with spaces */
        }
        else
        {
            SAPDB_memset( &userAscii[0], ' ', sizeof(tsp00_KnlIdentifier) );
            SAPDB_memcpy( cryptedPasswordAscii, blankCryptedPasswordAscii, sizeof(tsp00_CryptPw) );
        }
    }
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
