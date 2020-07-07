/*!
  @file           vos49.c
  @author         RaymondR
  @brief          NT Security
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
           Security Descriptor:

               +---------------------------------+
               |                                 |
               |       SECURITY DESCRIPTOR       |
               |                                 |
               +---------------------------------+
               |                                 |
               |          * Owner SID            |
               |          * Group SID            |
               |                                 |
               | +-----------------------------+ |
               | |            ACL              | |
               | +-----------------------------+ |
               | | +-------------------------+ | |
               | | |          ACE            | | |
               | | +-------------------------+ | |
               | | +-------------------------+ | |
               | | |          ACE            | | |
               | | +-------------------------+ | |
               | | +-------------------------+ | |
               | | |          ACE            | | |
               | | +-------------------------+ | |
               | | +-------------------------+ | |
               | | |          ACE            | | |
               | | +-------------------------+ | |
               | +-----------------------------+ |
               |                                 |
               +---------------------------------+

*/

/*
 * INCLUDE FILES
 */
#include "gos00.h"
#include "heo00.h"
#include "heo46.h"

#if defined (KERNEL)
# include "gos00k.h"            /* nocheck */  
#endif

#include "geo007_1.h"
#include "gos00sec.h"

/*
 *  DEFINES
 */
#define MOD__  "VOS49C : "
#define MF__   MOD__"UNDEFINED"

#define DEFAULT_SERVICE_SD_BUFF_SIZE     512
#define DEFAULT_SD_BUFF_SIZE             512
#define DEFAULT_SID_SIZE                 512
#define DEFAULT_DOMAIN_NAME_SIZE         80
#define DEFAULT_MAKE_ABS_BUFF_SIZE       1024
#define DEFAULT_USER_INFO_BUFF_SIZE      128
#define DEFAULT_GROUPS_INFO_BUFF_SIZE    128
#define DEFAULT_DEFAULT_DACL_BUFF_SIZE   256
#define DEFAULT_LOGON_NAME_SIZE          21


#define LSA_DLL_NAME                     "advapi32.dll"

/*
 *  MACROS
 */
#define MK_P_NTAPI(f)       (NTAPI *lp##f)

#define LOAD_FUNC(pH,f)     ( (pH->lp##f != NULL) || (pH->lp##f = sql49c_load_func(pH,#f)) != NULL) ?  NO_ERROR : GetLastError()
#define CALL_FUNC(pH,f,p)   ( pH->lp##f == NULL ) ? sql49c_func_err(#f),-1 : pH-> lp##f##p


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */

typedef struct  { LSA_HANDLE   Hdl;
                  HINSTANCE    hinst;

                  ULONG    MK_P_NTAPI(LsaNtStatusToWinError)( NTSTATUS Status );


                  NTSTATUS MK_P_NTAPI(LsaOpenPolicy)   ( PLSA_UNICODE_STRING       SystemName,
                                                         PLSA_OBJECT_ATTRIBUTES    ObjectAttributes,
                                                         ACCESS_MASK               DesiredAccess,
                                                         PLSA_HANDLE               pLSAHandle );

                  NTSTATUS MK_P_NTAPI(LsaEnumerateAccountRights)
                                                       ( LSA_HANDLE                LSAHandle,
                                                         PSID                      AccountSid,
                                                         PLSA_UNICODE_STRING       *UserRights,
                                                         PULONG                    CountOfRights );

                  NTSTATUS MK_P_NTAPI(LsaAddAccountRights)
                                                       ( LSA_HANDLE                LSAHandle,
                                                         PSID                      AccountSid,
                                                         PLSA_UNICODE_STRING       UserRights,
                                                         ULONG                     CountOfRights );

                  NTSTATUS MK_P_NTAPI(LsaRemoveAccountRights)
                                                       ( LSA_HANDLE                LSAHandle,
                                                         PSID                      AccountSid,
                                                         BOOLEAN                   AllRights,
                                                         PLSA_UNICODE_STRING       UserRights,
                                                         ULONG                     CountOfRights );

                  NTSTATUS MK_P_NTAPI(LsaFreeMemory)   ( PVOID                     Buffer );

                  NTSTATUS MK_P_NTAPI(LsaClose)        ( LSA_HANDLE                LSAHandle );

                } t_PolicyHdl;



/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */
static LONG   sql49c_get_own_textual_SID (PSZ *                   ppszTextualSID);

static LONG   sql49c_lookup_account_name( PSZ                     pszNodeName,
                                          PSZ                     pszAccountName,
                                          PSID                    *ppSid,
                                          PSZ                     *ppszRefDomain,
                                          PSID_NAME_USE           psnuType );
static LONG   sql49c_get_textual_SID    ( PSID                    psidSID,
                                          PSZ                     *ppszTextualSID );
static LONG   sql49c_open_policy        ( PSZ                     pszNodeName,
                                          DWORD                   DesiredAccess,
                                          t_PolicyHdl             *pPolicyHandle );
static LONG   sql49c_close_policy       ( t_PolicyHdl             *pPolicyHandle );
static LONG   sql49c_set_priv_on_account( t_PolicyHdl             *pPolicyHandle,
                                          PSZ                     pszNodeName,
                                          PSZ                     pszAccountName,
                                          PSZ                     pszPrivilegeName,
                                          BOOL                    fEnable );
_INLINE LONG  __sql49c_get_ace_sizes    ( PUSER_ACCESS_RIGHTS_REC pAccRights,
                                          PSID                    pSid );
_INLINE LONG  __sql49c_add_aces         ( PACL                    pNewDacl,
                                          PSID                    pSid,
                                          PUSER_ACCESS_RIGHTS_REC pAccRights );
_INLINE LONG  __sql49c_init_lsa_string  ( PSZ                     pszString,
                                                                  PLSA_UNICODE_STRING     *ppLsaString );

static LONG  sql49c_get_operators_SID   ( PSID                    *pplOperSid,
                                          PSID                    *ppDomOperSid );


static VOID* sql49c_load_func           ( t_PolicyHdl             *pPolicyHandle,
                                          PSZ                     pszFuncName );

_INLINE VOID sql49c_func_err            ( PSZ                     pszFuncName );

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

#if defined(_WIN32)

LONG   sql49c_set_sync_access (VOID)
  {
  #undef  MF__
  #define MF__ MOD__"sql49c_set_sync_access"
  #define CRASH_CLEANUP(_no,_rc) MSGCD(( ERR_SET_SYNC_ACCESS, _no, _rc ));     \
                                 if( pWorldSid )  FreeSid  ( pWorldSid );      \
                                 if( pTmp )       FREE_MEM ( pTmp );           \
                                 if( pOldSD )     FREE_MEM ( pOldSD );         \
                                 if( pNewDacl )   FREE_MEM ( pNewDacl );

  ULONG                        ulSize;
  PSECURITY_DESCRIPTOR         pOldSD            = NULL;
  PSECURITY_DESCRIPTOR         pNewSD            = NULL;
  PCHAR                        pBufDacl;
  PCHAR                        pBufSacl;
  PCHAR                        pBufOwner;
  PCHAR                        pBufGroup;
  ULONG                        ulNewSdSize;
  ULONG                        ulBufDaclSize;
  ULONG                        ulBufSaclSize;
  ULONG                        ulBufOwnerSize;
  ULONG                        ulBufGroupSize;
  SECURITY_DESCRIPTOR_CONTROL  Control;
  BOOL                         AclPresent,
                               AclDefaulted;
  PACL                         pOldAcl;
  PSID                         pWorldSid         = NULL;
  PACL                         pNewDacl          = NULL;
  ACL_SIZE_INFORMATION         AclSizeStruct;
  SID_IDENTIFIER_AUTHORITY     WorldSidAuthority = SECURITY_WORLD_SID_AUTHORITY;
  PCHAR                        pTmp              = NULL;
  LONG                         lNewDaclSize,
                               i;
  HANDLE                       hProcessSelf;
  LONG                         rc = NO_ERROR;

  DBGIN;


  //
  // --- first, check for necessary privileges...
  //
  hProcessSelf = OpenProcess( READ_CONTROL | WRITE_DAC, FALSE,
                              GetCurrentProcessId());
  if ( !hProcessSelf )
    {
    rc = GetLastError ();
    CRASH_CLEANUP( 1, rc );
    DBGOUT;
    return ( rc );
    }

  CloseHandle( hProcessSelf );



  //
  //
  // --- allocate space and get current dacl information
  //
  //
  ulSize = DEFAULT_SD_BUFF_SIZE;

  for ( ;; )
    {
    // --- allocate space for the current dacl
    rc = ALLOC_MEM ( &pOldSD, ulSize );

    if ( rc != NO_ERROR )
      {
      CRASH_CLEANUP( 2, rc );
      DBGOUT;
      return( rc );
      }

    // --- get current dacl information
    if (!GetKernelObjectSecurity(GetCurrentProcess(), DACL_SECURITY_INFORMATION,
                                 pOldSD, ulSize, &ulSize))
      {
      rc = GetLastError ();

      if ( ( rc != NO_ERROR ) && ( rc != ERROR_INSUFFICIENT_BUFFER ))
        {
        CRASH_CLEANUP( 3, rc );
        DBGOUT;
        return ( rc );
        }

      rc = FREE_MEM ( pOldSD );

      if ( rc != NO_ERROR )
        {
        CRASH_CLEANUP( 4, rc );
        DBGOUT;
        return( rc );
        }
      }
    else
      break;
    }


  if (!GetSecurityDescriptorDacl(pOldSD, &AclPresent, &pOldAcl, &AclDefaulted))
    {
    rc = GetLastError ();
    CRASH_CLEANUP( 5, rc );
    DBGOUT;
    return ( rc );
    }


  //
  //
  // --- allocate and initialize "world SID"
  //
  //

  if (!AllocateAndInitializeSid(&WorldSidAuthority, 1, SECURITY_WORLD_RID,
                                0, 0, 0, 0, 0, 0, 0, &pWorldSid))
    {
    rc = GetLastError ();
    CRASH_CLEANUP( 6, rc );
    DBGOUT;
    return ( rc );
    }

  //
  //
  // --- build up a new ACL
  //
  //
  if (AclPresent && pOldAcl)
    {
    if (!GetAclInformation(pOldAcl, &AclSizeStruct,
                           sizeof(AclSizeStruct), AclSizeInformation))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 7, rc );
      DBGOUT;
      return ( rc );
      }

    lNewDaclSize = AclSizeStruct.AclBytesInUse + sizeof(ACCESS_ALLOWED_ACE) +
                   GetLengthSid(pWorldSid) - sizeof(DWORD);

    rc = ALLOC_MEM(&pNewDacl, lNewDaclSize);

    if ( rc != NO_ERROR )
      {
      CRASH_CLEANUP( 8, rc );
      DBGOUT;
      return ( rc );
      }

    if (!InitializeAcl(pNewDacl, lNewDaclSize, ACL_REVISION2))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 9, rc );
      DBGOUT;
      return ( rc );
      }

    if (!AddAccessAllowedAce(pNewDacl, ACL_REVISION2, SYNCHRONIZE, pWorldSid))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 10, rc );
      DBGOUT;
      return ( rc );
      }

    if (!GetAce(pOldAcl, 0, &pTmp))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 11, rc );
      DBGOUT;
      return ( rc );
      }

    if (!AddAce(pNewDacl, ACL_REVISION2, 1, pTmp,
                AclSizeStruct.AclBytesInUse - sizeof(ACL)))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 12, rc );
      DBGOUT;
      return ( rc );
      }
    }
  else
    {
    lNewDaclSize = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) +
                   GetLengthSid(pWorldSid) - sizeof(DWORD);

    rc = ALLOC_MEM(&pNewDacl, lNewDaclSize);

    if ( rc != NO_ERROR )
      {
      CRASH_CLEANUP( 13, rc );
      DBGOUT;
      return ( rc );
      }

    if (!InitializeAcl(pNewDacl, lNewDaclSize, ACL_REVISION2))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 14, rc );
      DBGOUT;
      return ( rc );
      }

    if (!AddAccessAllowedAce(pNewDacl, ACL_REVISION2,
                             SYNCHRONIZE, pWorldSid))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 15, rc );
      DBGOUT;
      return ( rc );
      }
    }

  FreeSid (pWorldSid);
  pWorldSid = NULL;


  //
  //
  // --- make the security descriptor 'absolute'
  //
  //
  if (!GetSecurityDescriptorControl(pOldSD, &Control, &i))
    {
    rc = GetLastError ();
    CRASH_CLEANUP( 16, rc );
    DBGOUT;
    return ( rc );
    }


  if (Control & SE_SELF_RELATIVE)
    {
    // --- set default buffer sizes
    ulNewSdSize    = sizeof(SECURITY_DESCRIPTOR);
    ulBufDaclSize  = DEFAULT_MAKE_ABS_BUFF_SIZE / 4;
    ulBufSaclSize  = DEFAULT_MAKE_ABS_BUFF_SIZE / 4;
    ulBufOwnerSize = DEFAULT_MAKE_ABS_BUFF_SIZE / 4;
    ulBufGroupSize = DEFAULT_MAKE_ABS_BUFF_SIZE / 4;

    for ( ;; )
      {
      ulNewSdSize    = ALIGN( ulNewSdSize,    ALIGNMENT_VALUE );
      ulBufDaclSize  = ALIGN( ulBufDaclSize,  ALIGNMENT_VALUE );
      ulBufSaclSize  = ALIGN( ulBufSaclSize,  ALIGNMENT_VALUE );
      ulBufOwnerSize = ALIGN( ulBufOwnerSize, ALIGNMENT_VALUE );
      ulBufGroupSize = ALIGN( ulBufGroupSize, ALIGNMENT_VALUE );

      ulSize = ulNewSdSize + ulBufDaclSize + ulBufSaclSize +
               ulBufOwnerSize + ulBufGroupSize;

      rc = ALLOC_MEM ( &pTmp, ulSize );

      if ( rc != NO_ERROR )
        {
        CRASH_CLEANUP( 17, rc );
        DBGOUT;
        return( rc );
        }

      pNewSD    = (PSECURITY_DESCRIPTOR) pTmp;
      pBufDacl  = pTmp      + ulNewSdSize;
      pBufSacl  = pBufDacl  + ulBufDaclSize;
      pBufOwner = pBufSacl  + ulBufSaclSize;
      pBufGroup = pBufOwner + ulBufOwnerSize;

      if (!MakeAbsoluteSD(pOldSD,
                          pNewSD,          &ulNewSdSize,
                          (PACL)pBufDacl,  &ulBufDaclSize,
                          (PACL)pBufSacl,  &ulBufSaclSize,
                          (PSID)pBufOwner, &ulBufOwnerSize,
                          (PSID)pBufGroup, &ulBufGroupSize))
        {
        rc = GetLastError ();

        if ( ( rc != NO_ERROR ) && ( rc != ERROR_INSUFFICIENT_BUFFER ))
          {
          CRASH_CLEANUP( 18, rc );
          DBGOUT;
          return ( rc );
          }

        rc   = FREE_MEM ( pTmp );
        pTmp = NULL;

        if ( rc != NO_ERROR )
          {
          CRASH_CLEANUP( 19, rc );
          DBGOUT;
          return( rc );
          }
        }
      else
        break;
      }
    }
  else
    {
    pTmp = NULL;
    }

  if (!SetSecurityDescriptorDacl(pNewSD, TRUE, pNewDacl, FALSE))
    {
    rc = GetLastError ();
    CRASH_CLEANUP( 20, rc );
    DBGOUT;
    return ( rc );
    }

  if (!SetKernelObjectSecurity(GetCurrentProcess(), DACL_SECURITY_INFORMATION,
                               pNewSD))
    {
    rc = GetLastError ();
    CRASH_CLEANUP( 21, rc );
    DBGOUT;
    return ( rc );
    }

  if ( pTmp )
    {
    rc   = FREE_MEM ( pTmp );
    pTmp = NULL;

    if ( rc != NO_ERROR )
      {
      CRASH_CLEANUP( 21, rc );
      DBGOUT;
      return( rc );
      }
    }

  rc     = FREE_MEM ( pOldSD );
  pOldSD = NULL;

  if ( rc != NO_ERROR )
    {
    CRASH_CLEANUP( 22, rc );
    DBGOUT;
    return( rc );
    }

  rc       = FREE_MEM ( pNewDacl );
  pNewDacl = NULL;

  if ( rc != NO_ERROR )
    {
    CRASH_CLEANUP( 23, rc );
    DBGOUT;
    return( rc );
    }


  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

LONG sql49c_alloc_and_init_service_SD ( PSECURITY_DESCRIPTOR   *ppSD,
                                        SC_HANDLE              schService )
  {
  #undef  MF__
  #define MF__ MOD__"sql49c_alloc_and_init_service_SD"
  #ifdef CRASH_CLEANUP
   #undef CRASH_CLEANUP
  #endif
  #define CRASH_CLEANUP(_no,_rc) MSGCD(( ERR_GEN_SEC_DESC_SERVICE, _no, _rc ));         \
                                 if( pOldSD )            FREE_MEM ( pOldSD );           \
                                 if( plOperSid )         FREE_MEM ( plOperSid );        \
                                 if( pDomOperSid )       FREE_MEM ( pDomOperSid );      \
                                 if( *ppSD )             FREE_MEM ( *ppSD );

  LONG                    rc                = NO_ERROR;
  PSECURITY_DESCRIPTOR    pOldSD            = NULL;
  ULONG                   ulSize            = DEFAULT_SERVICE_SD_BUFF_SIZE;
  PSID                    plOperSid         = NULL;
  PSID                    pDomOperSid       = NULL;
  PACL                    pNewDacl          = NULL;
  PSID                    pOldOwnerSid      = NULL;
  ULONG                   ulMaxDaclSize     = 0;
  PSID                    pNewOwnerSid;
  PCHAR                   pTmp;
  BOOL                    AclPresent,
                          Defaulted;
  PACL                    pOldAcl;
  ULONG                   ulNewOwnerSidSize;
  ACL_SIZE_INFORMATION    AclSizeStruct;

  DBGIN;


  *ppSD = NULL;

  for ( ;; )
    {
    // --- allocate space for the security descriptor
    rc = ALLOC_MEM ( &pOldSD, ulSize );

    if ( rc != NO_ERROR )
      {
      pOldSD = NULL;
      DBGOUT;
      return( rc );
      }

    // --- now we ask for the service security descritptor
    if (!QueryServiceObjectSecurity( schService,
                                     OWNER_SECURITY_INFORMATION |
                                     DACL_SECURITY_INFORMATION,
                                     pOldSD, ulSize, &ulSize))
      {
      rc = GetLastError ();

      if (( rc != NO_ERROR ) && ( rc != ERROR_INSUFFICIENT_BUFFER ))
        {
        CRASH_CLEANUP( 1, rc );
        DBGOUT;
        return ( rc );
        }

      rc = FREE_MEM ( pOldSD );

      if ( rc != NO_ERROR )
        {
        pOldSD = NULL;
        DBGOUT;
        return( rc );
        }
      }
    else
      break;
    }

  //
  //
  // --- get the current DACL and Security descriptor owner
  //
  //
  if (!GetSecurityDescriptorDacl (pOldSD, &AclPresent, &pOldAcl, &Defaulted)||
      !GetSecurityDescriptorOwner(pOldSD, &pOldOwnerSid,  &Defaulted) )
    {
    rc = GetLastError ();
    CRASH_CLEANUP( 2, rc );
    DBGOUT;
    return ( rc );
    }

  //
  //
  // --- get the sid of the database administrator group
  //
  rc = sql49c_get_operators_SID ( &plOperSid, &pDomOperSid );

  if (( rc != NO_ERROR ) && ( rc != ERROR_NONE_MAPPED ))
    {
    CRASH_CLEANUP( 3, rc );

    DBGOUT;
    return ( rc );
    }

  if ( plOperSid != NULL && pDomOperSid != NULL )
    {
    FREE_MEM ( pOldSD );

    //
    // --- no special security discriptor needed!!!!!
    //
    *ppSD = NULL;
    DBGOUT;
    return ( NO_ERROR );
    }

  if ( pOldOwnerSid == NULL )
    pOldOwnerSid = plOperSid;

  if (AclPresent && pOldAcl)
    {
    if (!GetAclInformation(pOldAcl, &AclSizeStruct,
                           sizeof(AclSizeStruct), AclSizeInformation))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 4, rc );
      DBGOUT;
      return ( rc );
      }

    ulMaxDaclSize = AclSizeStruct.AclBytesInUse;
    }

  if ( plOperSid )
   {
   ulMaxDaclSize += ALIGN( GetLengthSid( plOperSid )    +
                           sizeof(ACCESS_ALLOWED_ACE)   -
                           sizeof(DWORD),
                           ALIGNMENT_VALUE );
   }
  if ( pDomOperSid )
   {
   ulMaxDaclSize += ALIGN( GetLengthSid( pDomOperSid )    +
                           sizeof(ACCESS_ALLOWED_ACE)     -
                           sizeof(DWORD),
                           ALIGNMENT_VALUE );
   }

  ulNewOwnerSidSize = ALIGN(GetLengthSid(pOldOwnerSid), ALIGNMENT_VALUE);

  rc = ALLOC_MEM( ppSD, ALIGN(sizeof(SECURITY_DESCRIPTOR),ALIGNMENT_VALUE) +
                  ulNewOwnerSidSize + ulMaxDaclSize );


  if ( rc != NO_ERROR )
    {
    rc = GetLastError();

    *ppSD = NULL;
    CRASH_CLEANUP( 5, rc );

    DBGOUT;
    return( rc );
    }

  pNewOwnerSid = (PSID)((PCHAR)*ppSD +
                         ALIGN(sizeof(SECURITY_DESCRIPTOR),ALIGNMENT_VALUE));
  pNewDacl     = (PACL)((PCHAR)pNewOwnerSid + ulNewOwnerSidSize );

  if ( !InitializeSecurityDescriptor( *ppSD, SECURITY_DESCRIPTOR_REVISION1 ))
    {
    rc = GetLastError();

    CRASH_CLEANUP( 6, rc );

    DBGOUT;
    return( rc );
    }

  if (!CopySid ( ulNewOwnerSidSize, pNewOwnerSid, pOldOwnerSid ))
    {
    rc = GetLastError();
    CRASH_CLEANUP( 7, rc );

    DBGOUT;
    return( rc );
    }

  if (!InitializeAcl(pNewDacl, ulMaxDaclSize, ACL_REVISION2))
    {
    rc = GetLastError ();
    CRASH_CLEANUP( 8, rc );
    DBGOUT;
    return ( rc );
    }

  if (AclPresent && pOldAcl)
    {
    if (!GetAce(pOldAcl, 0, &pTmp))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 9, rc );
      DBGOUT;
      return ( rc );
      }

    if (!AddAce( pNewDacl, ACL_REVISION2,
                 0, //- POSITION (must be the first ACE after 'InitializeAcl')
                 pTmp, AclSizeStruct.AclBytesInUse - sizeof(ACL)))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 10, rc );
      DBGOUT;
      return ( rc );
      }
    }

  if ( plOperSid )
    {
    if (!AddAccessAllowedAce(pNewDacl, ACL_REVISION2,
                             (SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL) &
                             ~DELETE,
                             plOperSid ))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 11, rc );
      DBGOUT;
      return ( rc );
      }
   }

  if ( pDomOperSid )
    {
    if (!AddAccessAllowedAce(pNewDacl, ACL_REVISION2,
                             (SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL) &
                             ~DELETE,
                             pDomOperSid ))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 12, rc );
      DBGOUT;
      return ( rc );
      }
    }

  //
  //
  // --- set the new access control list
  //
  //
  if ( !SetSecurityDescriptorDacl ( *ppSD, TRUE, pNewDacl, FALSE ) )
    {
    rc = GetLastError ();

    CRASH_CLEANUP( 13, rc );

    DBGOUT;
    return ( rc );
    }


  if ( !IsValidSecurityDescriptor ( *ppSD ))
    {
    rc = GetLastError ();

    CRASH_CLEANUP( 14, rc );

    DBGOUT;
    return ( rc );
    }

  if ( pDomOperSid ) FREE_MEM ( pDomOperSid );
  if ( plOperSid )   FREE_MEM ( plOperSid );
  if ( pOldSD )      FREE_MEM ( pOldSD );


  DBGOUT;
  return (rc);
  }

/*------------------------------*/

LONG sql49c_alloc_and_init_SD ( PACCESS_RIGHTS_REC     pAccess,
                                PSECURITY_DESCRIPTOR   *ppSD )
  {
  #undef  MF__
  #define MF__ MOD__"sql49c_alloc_and_init_SD"
  #ifdef CRASH_CLEANUP
   #undef CRASH_CLEANUP
  #endif
  #define CRASH_CLEANUP(_no,_rc) MSGCD((ERR_GEN_SEC_DESC_COMMON, _no, _rc ));      \
                             if( pSystemSid )       FreeSid  ( pSystemSid );       \
                             if( pAdminsSid )       FreeSid  ( pAdminsSid );       \
                             if( pWorldSid )        FreeSid  ( pWorldSid );        \
                             if( pBackupSid )       FreeSid  ( pBackupSid );       \
                             if( pCreatorSid)       FreeSid  ( pCreatorSid);       \
                             if( pTDacl )           FREE_MEM ( pTDacl );           \
                             if( pTU )              FREE_MEM ( pTU );              \
                             if (pOtherUserSid)     FREE_MEM (pOtherUserSid);      \
                             if( *ppSD )            FREE_MEM ( *ppSD );            \
                             if( plOperSid )        FREE_MEM ( plOperSid );        \
                             if( pDomOperSid )      FREE_MEM ( pDomOperSid );      \
                             *ppSD = NULL;

  #define ADD_ACES( _pDacl, _pSid, _Access, _ulCrashNo );                    \
                                 rc = __sql49c_add_aces(_pDacl, _pSid,       \
                                                       _Access );            \
                                 if ( rc != NO_ERROR )                       \
                                  {                                          \
                                  CRASH_CLEANUP( _ulCrashNo, rc );           \
                                  DBGOUT;                                    \
                                  return ( rc );                             \
                                  }

  LONG                      rc                  = NO_ERROR;
  BOOL                      fAddDefaultDACL     = FALSE;
  SID_IDENTIFIER_AUTHORITY  SystemSidAuthority  = SECURITY_NT_AUTHORITY;
  SID_IDENTIFIER_AUTHORITY  BackupSidAuthority  = SECURITY_NT_AUTHORITY;
  SID_IDENTIFIER_AUTHORITY  WorldSidAuthority   = SECURITY_WORLD_SID_AUTHORITY;
  SID_IDENTIFIER_AUTHORITY  CreatorSidAuthority = SECURITY_CREATOR_SID_AUTHORITY;
  PSID                      pOtherUserSid       = NULL;
  PSID                      pAdminsSid          = NULL;
  PSID                      plOperSid           = NULL;
  PSID                      pDomOperSid         = NULL;
  PSID                      pSystemSid          = NULL;
  PSID                      pCreatorSid         = NULL;
  PTOKEN_DEFAULT_DACL       pTDacl              = NULL;
  PTOKEN_USER               pTU                 = NULL;
  PSID                      pWorldSid           = NULL;
  PSID                      pBackupSid          = NULL;
  PSID                      pTmpDescOwnerSid    = NULL;
  PACL                      pNewDacl;
  ULONG                     ulMaxDaclSize;
  PSID                      pDescOwnerSid;
  ULONG                     ulDescOwnerSidSize;
  ACL_SIZE_INFORMATION      AclSizeStruct;
  ACCESS_ALLOWED_ACE        *paaAllowedAce;

  DBGIN;

  *ppSD = NULL;


  // --- no access right?
  if ( !pAccess->User.ulAccess     && !pAccess->OtherUser.ulAccess  && !pAccess->Creator.ulAccess &&
       !pAccess->Admin.ulAccess    && !pAccess->System.ulAccess     &&
       !pAccess->SAPDBOp.ulAccess  && !pAccess->BackupOp.ulAccess )
    {
    // --- check world access?
    switch ( pAccess->World.ulAccess )
      {
      case 0 :  // - CREATE NO ACCESS SD

        rc = ALLOC_MEM( ppSD, ALIGN(sizeof(SECURITY_DESCRIPTOR),ALIGNMENT_VALUE) +
                        sizeof(ACL) );

        if ( rc == NO_ERROR )
          {
          pNewDacl = (PACL)((PCHAR) *ppSD +
                        ALIGN( sizeof(SECURITY_DESCRIPTOR), ALIGNMENT_VALUE ));

          if (!InitializeSecurityDescriptor((PSECURITY_DESCRIPTOR) *ppSD,
                                            SECURITY_DESCRIPTOR_REVISION1 )      ||
              !InitializeAcl               (pNewDacl, sizeof(ACL), ACL_REVISION2)||
              !SetSecurityDescriptorDacl   ((PSECURITY_DESCRIPTOR) *ppSD,
                                            TRUE, (PACL) pNewDacl, FALSE ) )
            {
            rc = GetLastError();
            CRASH_CLEANUP( 0, rc );
            }
          }

        DBGOUT;
        return( rc );

      case SPECIFIC_RIGHTS_ALL |
           STANDARD_RIGHTS_ALL:    // - CREATE WORLD ACCESS

        rc = ALLOC_MEM ( ppSD, sizeof (SECURITY_DESCRIPTOR) );

        if ( rc == NO_ERROR )
          {
          if ( !InitializeSecurityDescriptor( *ppSD, SECURITY_DESCRIPTOR_REVISION1 ) ||
              !SetSecurityDescriptorDacl   ( *ppSD, TRUE, (PACL) NULL, FALSE ) )
            {
            rc = GetLastError();
            CRASH_CLEANUP( 0, rc );
            }
          }
        DBGOUT;
        return( rc );
      }
    }


  //
  //
  // --- get the default DACL and and the current users security identifier
  //     (SID) associated with the process/thread access token.
  //
  //

    if (pAccess->ignoreOwnUser && NULL == pAccess->pUseOtherUser)
    {
        DBG1  (( MF__, ERRMSG_SEC_NO_USER));
        MSGCD  (( ERR_SEC_NO_USER ));
        CRASH_CLEANUP( 1, 0 );
        DBGOUT;
        return -1;
    }

    if (!pAccess->ignoreOwnUser)
    {
        rc = sql49c_get_token_information ( &pTU, NULL, &pTDacl);
    }
    else
    {
        rc = sql49c_get_token_information ( NULL, NULL, &pTDacl);
    }

    if ( rc != NO_ERROR )
    {
        DBGOUT;
        CRASH_CLEANUP( 2, rc );
        return ( rc );
    }


    if (NULL != pAccess->pUseOtherUser)
    {
       SID_NAME_USE        snuType;

       rc = sql49c_lookup_account_name (NULL, pAccess->pUseOtherUser,
                                        &pOtherUserSid, NULL, &snuType );
  
       if ((rc == NO_ERROR) && (snuType != SidTypeUser))
       {
           rc = ERROR_NONE_MAPPED;
       }

       if (NO_ERROR != rc)
       {
            DBGOUT;
            CRASH_CLEANUP( 3, rc );
            return ( rc );
       }
    }
    
  //
  //
  // --- get the sid of the database administrator groups
  //
  //
  rc = sql49c_get_operators_SID ( &plOperSid, &pDomOperSid );

  if (( rc != NO_ERROR ) && ( rc != ERROR_NONE_MAPPED ))
    {
    CRASH_CLEANUP( 4, rc );

    DBGOUT;
    return ( rc );
    }

  // --- is the local database administrators SID invalid?
  if ( plOperSid == NULL )
    pAccess->SAPDBOp.ulAccess = 0;

  // --- is the domain database administrators SID invalid?
  if ( pDomOperSid == NULL ) 
    pAccess->DomSAPDBOp.ulAccess = 0;


  if ( pAccess->fAddDefaultDACL )
    {
    // --- get the size of the default DACL
    if (!GetAclInformation(pTDacl->DefaultDacl, &AclSizeStruct,
                           sizeof(AclSizeStruct), AclSizeInformation))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 5, rc );

      DBGOUT;
      return ( rc );
      }
    }

  //
  //
  // --- create a SID to represent the Admin group
  //
  //
  if ((!AllocateAndInitializeSid ( &SystemSidAuthority,
                                   1, SECURITY_LOCAL_SYSTEM_RID,
                                   0, 0, 0, 0, 0, 0, 0, &pSystemSid ))   ||
      (!AllocateAndInitializeSid ( &SystemSidAuthority,
                                   2, SECURITY_BUILTIN_DOMAIN_RID,
                                   DOMAIN_ALIAS_RID_ADMINS,
                                   0, 0, 0, 0, 0, 0, &pAdminsSid ))      ||
      (!AllocateAndInitializeSid ( &CreatorSidAuthority,
                                   1, SECURITY_CREATOR_OWNER_RID,
                                   0, 0, 0, 0, 0, 0, 0, &pCreatorSid ))  ||
      (!AllocateAndInitializeSid ( &WorldSidAuthority,
                                   1, SECURITY_WORLD_RID,
                                   0, 0, 0, 0, 0, 0, 0, &pWorldSid))     ||
      (!AllocateAndInitializeSid ( &BackupSidAuthority,
                                   2, SECURITY_BUILTIN_DOMAIN_RID,
                                   DOMAIN_ALIAS_RID_BACKUP_OPS,
                                   0, 0, 0, 0, 0, 0, &pBackupSid)))
    {
    rc = GetLastError();

    CRASH_CLEANUP( 6, rc );
    DBGOUT;
    return( rc );
    }


  //
  //
  // --- allocate a buffer which is used to hold the new security descriptor,
  //     the current users SID and the new DACL.
  //
  //

  if ( pAccess->fDescOwnerIsAdmin || pAccess->ignoreOwnUser)
  {
    pTmpDescOwnerSid = pAdminsSid;
  }
  else
  {
    pTmpDescOwnerSid = pTU->User.Sid;
  }

  ulDescOwnerSidSize  = ALIGN(GetLengthSid(pTmpDescOwnerSid), ALIGNMENT_VALUE);
  ulMaxDaclSize       = sizeof(ACL);

  if ( pAccess->fAddDefaultDACL )
   ulMaxDaclSize     += AclSizeStruct.AclBytesInUse;

  if (NULL != pAccess->pUseOtherUser)
  {
      ulMaxDaclSize += __sql49c_get_ace_sizes (&pAccess->OtherUser, pOtherUserSid );
  }

  if (!pAccess->ignoreOwnUser)
  {
      ulMaxDaclSize += __sql49c_get_ace_sizes (&pAccess->User, pTU->User.Sid );
  }

  ulMaxDaclSize += __sql49c_get_ace_sizes(&pAccess->Creator,       pCreatorSid );
  ulMaxDaclSize += __sql49c_get_ace_sizes(&pAccess->System,        pSystemSid );
  ulMaxDaclSize += __sql49c_get_ace_sizes(&pAccess->Admin,         pAdminsSid );
  ulMaxDaclSize += __sql49c_get_ace_sizes(&pAccess->World,         pWorldSid );
  ulMaxDaclSize += __sql49c_get_ace_sizes(&pAccess->BackupOp,      pBackupSid );
  ulMaxDaclSize += __sql49c_get_ace_sizes(&pAccess->SAPDBOp,       plOperSid );
  ulMaxDaclSize += __sql49c_get_ace_sizes(&pAccess->DomSAPDBOp,    pDomOperSid );

  rc = ALLOC_MEM ( ppSD, ALIGN(sizeof(SECURITY_DESCRIPTOR), ALIGNMENT_VALUE) +
                   ulDescOwnerSidSize + ulMaxDaclSize  );

  if ( rc != NO_ERROR )
    {
    CRASH_CLEANUP( 7, rc );
    DBGOUT;
    return ( rc );
    }

  pDescOwnerSid = (PSID)((PCHAR)*ppSD +
                         ALIGN(sizeof(SECURITY_DESCRIPTOR),ALIGNMENT_VALUE));
  pNewDacl      = (PACL)((PCHAR)pDescOwnerSid + ulDescOwnerSidSize);


  //
  //
  // --- initialize the new security descriptor and set its owner
  //
  //
  if ( !InitializeSecurityDescriptor( *ppSD, SECURITY_DESCRIPTOR_REVISION1 ))
    {
    rc = GetLastError();
    CRASH_CLEANUP( 8, rc );

    DBGOUT;
    return( rc );
    }

  if (!CopySid ( ulDescOwnerSidSize, pDescOwnerSid, pTmpDescOwnerSid ))
    {
    rc = GetLastError();
    CRASH_CLEANUP( 9, rc );

    DBGOUT;
    return( rc );
    }

    if (!SetSecurityDescriptorOwner(*ppSD, pDescOwnerSid, 0))
    {
    rc = GetLastError();
    CRASH_CLEANUP( 10, rc );

    DBGOUT;
    return( rc );
    }

  //
  //
  // --- initialize and construct the new access control list (DACL)
  //
  //
  if (!InitializeAcl ( pNewDacl, ulMaxDaclSize, ACL_REVISION2) )
    {
    rc = GetLastError ();
    CRASH_CLEANUP( 11, rc );

    DBGOUT;
    return ( rc );
    }

  if ( pAccess->fAddDefaultDACL )
    {
    if ((!GetAce(pTDacl->DefaultDacl, 0, (PVOID*)&paaAllowedAce)) ||
        (!AddAce(pNewDacl, ACL_REVISION2, 0, (PVOID)paaAllowedAce,
                 AclSizeStruct.AclBytesInUse - sizeof(ACL))))
      {
      rc = GetLastError ();
      CRASH_CLEANUP( 12, rc );

      DBGOUT;
      return ( rc );
      }
    }

  if (NULL != pAccess->pUseOtherUser)
  {
        ADD_ACES( pNewDacl, pOtherUserSid, &pAccess->OtherUser, 101);
  }

  if (!pAccess->ignoreOwnUser)
  {
        ADD_ACES( pNewDacl, pTU->User.Sid, &pAccess->User, 102);
  }

  ADD_ACES( pNewDacl, pCreatorSid,   &pAccess->Creator,          103 );
  ADD_ACES( pNewDacl, pAdminsSid,    &pAccess->Admin,            104 );
  ADD_ACES( pNewDacl, pSystemSid,    &pAccess->System,           105 );
  ADD_ACES( pNewDacl, pWorldSid,     &pAccess->World,            106 );
  ADD_ACES( pNewDacl, pBackupSid,    &pAccess->BackupOp,         107 );
  ADD_ACES( pNewDacl, plOperSid,     &pAccess->SAPDBOp,          108 );
  ADD_ACES( pNewDacl, pDomOperSid,   &pAccess->DomSAPDBOp,       109 );

  //
  //
  // --- set the new access control list
  //
  //
  if ( !SetSecurityDescriptorDacl ( *ppSD, TRUE, pNewDacl, FALSE ) )
    {
    rc = GetLastError ();
    CRASH_CLEANUP( 13, rc );

    DBGOUT;
    return ( rc );
    }


  if ( !IsValidSecurityDescriptor ( *ppSD ))
    {
    rc = GetLastError ();
    CRASH_CLEANUP( 14, rc );

    DBGOUT;
    return ( rc );
    }

  if ( pAdminsSid )       FreeSid  ( pAdminsSid );
  if ( pWorldSid )        FreeSid  ( pWorldSid );
  if ( pBackupSid )       FreeSid  ( pBackupSid );
  if ( pSystemSid )       FreeSid  ( pSystemSid );
  if ( pCreatorSid)       FreeSid  ( pCreatorSid);
  if ( plOperSid )        FREE_MEM ( plOperSid );
  if ( pDomOperSid )      FREE_MEM ( pDomOperSid );
  if ( pTU )              FREE_MEM ( pTU );
  if ( pOtherUserSid )    FREE_MEM ( pOtherUserSid );
  if ( pTDacl )           FREE_MEM ( pTDacl );

  DBGOUT;
  return (rc);
  }

/*------------------------------*/

LONG sql49c_get_kernel_obj_SD ( PSECURITY_DESCRIPTOR   *ppSD,
                                HANDLE                 hObject)
  {
  #undef  MF__
  #define MF__ MOD__"sql49c_get_kernel_obj_SD"
  LONG              rc     = NO_ERROR;
  ULONG             ulSize;

  DBGIN;


  rc = ALLOC_MEM ( ppSD, DEFAULT_SD_BUFF_SIZE );

  if ( rc != NO_ERROR )
    {
    DBGOUT;
    return( rc );
    }

  if ( !GetKernelObjectSecurity ( hObject,
                                  OWNER_SECURITY_INFORMATION |
                                  DACL_SECURITY_INFORMATION,
                                  *ppSD,
                                  DEFAULT_SD_BUFF_SIZE,
                                  &ulSize ))
     {
     if ( ulSize > 0 )
       {
       rc = FREE_MEM ( *ppSD );

       if ( rc != NO_ERROR )
         {
         DBGOUT;
         return( rc );
         }

       rc = ALLOC_MEM ( ppSD, ulSize );

       if ( rc != NO_ERROR )
         {
         DBGOUT;
         return( rc );
         }

       if ( !GetKernelObjectSecurity ( hObject,
                                       OWNER_SECURITY_INFORMATION |
                                       DACL_SECURITY_INFORMATION,
                                       *ppSD,
                                       ulSize,
                                       &ulSize ))
          {
          rc = GetLastError();
          FREE_MEM ( *ppSD );
          MSGCD (( ERR_GET_SEC_DESC, rc ));
          DBGOUT;
          return( rc );
          }
       }
     else
       {
       rc = GetLastError();
       FREE_MEM ( *ppSD );
       MSGCD (( ERR_GET_SEC_DESC, rc ));
       DBGOUT;
       return( rc );
       }
     }


  DBGOUT;
  return (rc);
  }

/*------------------------------*/


LONG sql49c_set_kernel_obj_sec ( PSECURITY_DESCRIPTOR   pSD,
                                 HANDLE                 hObject)
  {
  #undef  MF__
  #define MF__ MOD__"sql49c_set_kernel_obj_sec"
  LONG                    rc           = NO_ERROR;

  DBGIN;


  if ( !SetKernelObjectSecurity ( hObject,
                                  DACL_SECURITY_INFORMATION,
                                  pSD ))
     {
     rc = GetLastError();
     MSGCD (( ERR_SET_OBJ_SEC, rc ));
     }

  DBGOUT;
  return (rc);
  }

/*------------------------------*/

LONG sql49c_get_user_info ( PSZ   *ppszLogonName,
                            PSZ   *ppszRefDomain )
  {
  #undef  MF__
  #define MF__ MOD__"sql49c_get_user_info"
  LONG          rc           = NO_ERROR;
  ULONG         ulBufferSize;
  SID_NAME_USE  snuType;
  PSZ           pszLogonName = NULL;

  DBGIN;

  if ( ppszLogonName != NULL )
    *ppszLogonName = NULL;

  if ( ppszRefDomain != NULL )
    *ppszRefDomain = NULL;

  ulBufferSize = DEFAULT_LOGON_NAME_SIZE;

  for ( ;; )
    {
    rc = ALLOC_MEM ( &pszLogonName, ulBufferSize );

    if ( rc != NO_ERROR )
      {
      DBGOUT;
      return (rc);
      }

    if ( !GetUserName ( pszLogonName, &ulBufferSize ))
      {
      rc = GetLastError ();

      if ( ( rc != NO_ERROR ) && ( rc != ERROR_INSUFFICIENT_BUFFER ))
        {
        FREE_MEM ( pszLogonName );
        MSGCD (( ERR_GET_USER_NAME, 1, rc ));

        DBGOUT;
        return ( rc );
        }

      rc = FREE_MEM ( pszLogonName );

      if ( rc != NO_ERROR )
        {
        DBGOUT;
        return( rc );
        }
      }
    else
      break;
    }

  if ( ppszRefDomain != NULL )
    {
    //
    //
    // --- get Domain name
    //
    //
    rc = sql49c_lookup_account_name ( NULL, pszLogonName,
                                      NULL, ppszRefDomain, &snuType );

    if ( rc != NO_ERROR )
      {
      FREE_MEM ( pszLogonName );

      DBGOUT;
      return ( rc );
      }
    }

  if ( ppszLogonName != NULL )
    *ppszLogonName = pszLogonName;
  else
    FREE_MEM ( pszLogonName );

  DBGOUT;
  return ( rc );
  }


/*------------------------------*/

LONG sql49c_set_file_security ( PSZ                   pszFileName,
                                PSECURITY_DESCRIPTOR  pSD )
  {
  #undef  MF__
  #define MF__ MOD__"sql49c_set_file_security"
  LONG                  rc  = NO_ERROR;

  DBGIN;


  if ( !SetFileSecurity( pszFileName,
                         DACL_SECURITY_INFORMATION  |
                         OWNER_SECURITY_INFORMATION, pSD ))
    {
    rc = GetLastError ();
    MSGCD (( ERR_SET_FILE_OWNER, 1, rc ));
    DBGOUT;
    return( rc );
    }

  DBGOUT;
  return (rc);
  }

/*------------------------------*/

LONG sql49c_set_service_logon_right ( PSZ           pszNodeName,
                                      PSZ           pszAccountName )
  {
  #undef  MF__
  #define MF__ MOD__"sql49c_set_service_logon_right"
  LONG                  rc  = NO_ERROR;
  t_PolicyHdl           PolicyHandle;

  DBGIN;


  rc = sql49c_open_policy ( pszNodeName,
                            POLICY_CREATE_ACCOUNT | POLICY_LOOKUP_NAMES,
                            &PolicyHandle );

  if ( rc == NO_ERROR )
    rc = sql49c_set_priv_on_account ( &PolicyHandle, pszNodeName, pszAccountName,
                                      "SeServiceLogonRight", TRUE );

  if ( rc == NO_ERROR )
    rc = sql49c_close_policy( &PolicyHandle );
  else
    sql49c_close_policy( &PolicyHandle ); // Ignore errors!

  DBGOUT;
  return (rc);
  }

/*------------------------------*/

LONG   sql49c_get_token_information ( PTOKEN_USER          *ppTU,
                                      PTOKEN_GROUPS        *ppTG,
                                      PTOKEN_DEFAULT_DACL  *ppTDacl)
  {
  #undef  MF__
  #define MF__ MOD__"sql49c_get_token_information"
  HANDLE                    hToken = NULL;
  ULONG                     ulSize;
  LONG                      rc = NO_ERROR;

  if (ppTU    != NULL) *ppTU    = NULL;
  if (ppTG    != NULL) *ppTG    = NULL;
  if (ppTDacl != NULL) *ppTDacl = NULL;


  //
  //
  // --- open a handle to the access token for this thread or process
  //
  //
  if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken))
    {
    rc = GetLastError ();

    if ( rc != ERROR_NO_TOKEN )
      {
      MSGCD (( ERR_GET_TOKEN_INFO, 1, rc ));
      DBGOUT;
      return ( rc );
      }

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
      {
      rc = GetLastError ();

      MSGCD (( ERR_GET_TOKEN_INFO, 2, rc ));
      DBGOUT;
      return ( rc );
      }
    }

  if (ppTU != NULL)
    {
    //
    //
    // --- allocate space and get user information
    //
    //
    ulSize = DEFAULT_USER_INFO_BUFF_SIZE;

    for ( ;; )
      {
      // --- allocate space for the user information
      rc = ALLOC_MEM ( ppTU, ulSize );

      if ( rc != NO_ERROR )
        {
        CloseHandle(hToken);

        *ppTU = NULL;
        DBGOUT;
        return( rc );
        }

      // --- now we ask for the user information
      if (!GetTokenInformation(hToken, TokenUser, *ppTU, ulSize, &ulSize))
        {
        rc = GetLastError ();

        if ( ( rc != NO_ERROR ) && ( rc != ERROR_INSUFFICIENT_BUFFER ))
          {
          CloseHandle(hToken);

          FREE_MEM ( *ppTU );
          *ppTU = NULL;
          MSGCD (( ERR_GET_TOKEN_INFO, 3, rc ));
          DBGOUT;
          return ( rc );
          }

        rc = FREE_MEM ( *ppTU );

        if ( rc != NO_ERROR )
          {
          CloseHandle(hToken);

          *ppTU = NULL;
          DBGOUT;
          return( rc );
          }
        }
      else
        break;
      }
    }

  if (ppTG != NULL)
    {
    //
    //
    // --- allocate space and get groups information
    //
    //
    ulSize = DEFAULT_GROUPS_INFO_BUFF_SIZE;

    for ( ;; )
      {
      // --- allocate space for the user information
      rc = ALLOC_MEM ( ppTG, ulSize );

      if ( rc != NO_ERROR )
        {
        if (ppTU != NULL)
          {
          FREE_MEM ( *ppTU );
          *ppTU = NULL;
          }
        *ppTG = NULL;

        CloseHandle(hToken);

        DBGOUT;
        return( rc );
        }

      // --- now we ask for the groups information
      if (!GetTokenInformation(hToken, TokenGroups, *ppTG, ulSize, &ulSize))
        {
        rc = GetLastError ();

        if ( ( rc != NO_ERROR ) && ( rc != ERROR_INSUFFICIENT_BUFFER ))
          {
          if (ppTU != NULL)
            {
            FREE_MEM ( *ppTU );
            *ppTU = NULL;
            }
          FREE_MEM ( *ppTG );
          *ppTG = NULL;
          MSGCD (( ERR_GET_TOKEN_INFO, 4, rc ));

          CloseHandle(hToken);

          DBGOUT;
          return ( rc );
          }

        rc = FREE_MEM ( *ppTG );

        if ( rc != NO_ERROR )
          {
          if (ppTU != NULL)
            {
            FREE_MEM ( *ppTU );
            *ppTU = NULL;
            }
          *ppTG = NULL;

          CloseHandle(hToken);

          DBGOUT;
          return( rc );
          }
        }
      else
        break;
      }
    }

  if (ppTDacl != NULL)
    {
    //
    //
    // --- allocate space and get default DACL
    //
    //
    ulSize = DEFAULT_DEFAULT_DACL_BUFF_SIZE;

    for ( ;; )
      {
      rc = ALLOC_MEM ( ppTDacl, ulSize );

      if ( rc != NO_ERROR )
        {
        if (ppTU != NULL)
          {
          FREE_MEM ( *ppTU );
          *ppTU = NULL;
          }
        if (ppTG != NULL)
          {
          FREE_MEM ( *ppTG );
          *ppTG = NULL;
          }
        *ppTDacl = NULL;

        CloseHandle(hToken);

        DBGOUT;
        return( rc );
        }

      // --- now we ask for the default DACL
      if (!GetTokenInformation(hToken, TokenDefaultDacl, *ppTDacl, ulSize, &ulSize))
        {
        rc = GetLastError ();

        if ( ( rc != NO_ERROR ) && ( rc != ERROR_INSUFFICIENT_BUFFER ))
          {
          if (ppTU != NULL)
            {
            FREE_MEM ( *ppTU );
            *ppTU = NULL;
            }
          if (ppTG != NULL)
            {
            FREE_MEM ( *ppTG );
            *ppTG = NULL;
            }
          FREE_MEM ( *ppTDacl );
          *ppTDacl = NULL;
          MSGCD (( ERR_GET_TOKEN_INFO, 5, rc ));

          CloseHandle(hToken);

          DBGOUT;
          return ( rc );
          }

        rc = FREE_MEM ( *ppTDacl );

        if ( rc != NO_ERROR )
          {
          if (ppTU != NULL)
            {
            FREE_MEM ( *ppTU );
            *ppTU = NULL;
            }
          if (ppTG != NULL)
            {
            FREE_MEM ( *ppTG );
            *ppTG = NULL;
            }
          *ppTDacl = NULL;

          CloseHandle(hToken);

          DBGOUT;
          return( rc );
          }
        }
      else
        break;
      }
    }

  CloseHandle(hToken);

  DBGOUT;
  return ( NO_ERROR );
  }

/*------------------------------*/

LONG sql49c_user_is_admin ( BOOL *pfIsAdmin )
   {
   #undef  MF__
   #define MF__ MOD__"sql49c_is_admin"
   #ifdef CRASH_CLEANUP
    #undef CRASH_CLEANUP
   #endif
   #define CRASH_CLEANUP(_no,_rc) MSGCD(( ERR_USER_IS_ADMIN, _no, _rc ));    \
                                  if( pAdminSid ) FreeSid  ( pAdminSid );    \
                                  if( pTG )       FREE_MEM ( pTG );

   LONG                     rc                  = NO_ERROR;
   SID_IDENTIFIER_AUTHORITY SystemSidAuthority  = SECURITY_NT_AUTHORITY;
   PTOKEN_GROUPS            pTG                 = NULL;
   PSID                     pAdminSid           = NULL;
   ULONG                    ulGroup;

   DBGIN;

   *pfIsAdmin = FALSE;

   if (!AllocateAndInitializeSid ( &SystemSidAuthority,
                                   2, SECURITY_BUILTIN_DOMAIN_RID,
                                   DOMAIN_ALIAS_RID_ADMINS,
                                   0, 0, 0, 0, 0, 0, &pAdminSid ))
     {
     rc = GetLastError();

     CRASH_CLEANUP( 1,rc );

     DBGOUT;
     return( rc );
     }

   rc = sql49c_get_token_information ( NULL, &pTG, NULL );

   if ( rc != NO_ERROR )
     {
     CRASH_CLEANUP( 2, rc );

     DBGOUT;
     return ( rc );
     }

   //
   // --- is the current user in the system administrators group?
   //
   for (ulGroup = 0; ulGroup < pTG->GroupCount; ulGroup++)
     {
     if (EqualSid(pTG->Groups[ulGroup].Sid, pAdminSid))
       {
       *pfIsAdmin = TRUE;
       break;
       }
     }

   if ( pAdminSid ) FreeSid  ( pAdminSid );
   if ( pTG )       FREE_MEM ( pTG );

   DBGOUT;
   return ( NO_ERROR );
   }

/*------------------------------*/

LONG sql49c_user_privilege_check ( BOOL *pfIsOk )
   {
   #undef  MF__
   #define MF__ MOD__"sql49c_user_privilege_check"
   #ifdef CRASH_CLEANUP
    #undef CRASH_CLEANUP
   #endif
   #define CRASH_CLEANUP(_no,_rc) MSGCD(( ERR_USER_IS_ADMIN, _no, _rc ));               \
                                  if( pSystemSid )       FreeSid  ( pSystemSid );       \
                                  if( pAdminSid )        FreeSid  ( pAdminSid );        \
                                  if( plOperSid )        FREE_MEM ( plOperSid );   \
                                  if( pDomOperSid )      FREE_MEM ( pDomOperSid );\
                                  if( pTG )              FREE_MEM ( pTG );

   LONG                     rc                  = NO_ERROR;
   SID_IDENTIFIER_AUTHORITY SystemSidAuthority  = SECURITY_NT_AUTHORITY;
   PTOKEN_GROUPS            pTG                 = NULL;
   PSID                     pAdminSid           = NULL;
   PSID                     pDomOperSid         = NULL;
   PSID                     plOperSid           = NULL;
   PSID                     pSystemSid          = NULL;
   ULONG                    ulGroup;

   DBGIN;

   *pfIsOk = FALSE;


   rc = sql49c_get_operators_SID ( &plOperSid, &pDomOperSid );

   if (( rc != NO_ERROR ) && ( rc != ERROR_NONE_MAPPED )) 
     {
     CRASH_CLEANUP( 1, rc );

     DBGOUT;
     return ( rc );
     }


   if ((!AllocateAndInitializeSid ( &SystemSidAuthority,
                                    1, SECURITY_LOCAL_SYSTEM_RID,
                                    0, 0, 0, 0, 0, 0, 0, &pSystemSid ))   ||
       (!AllocateAndInitializeSid ( &SystemSidAuthority,
                                    2, SECURITY_BUILTIN_DOMAIN_RID,
                                    DOMAIN_ALIAS_RID_ADMINS,
                                    0, 0, 0, 0, 0, 0, &pAdminSid )))
     {
     rc = GetLastError();

     CRASH_CLEANUP( 3,rc );

     DBGOUT;
     return( rc );
     }

   rc = sql49c_get_token_information ( NULL, &pTG, NULL );

   if ( rc != NO_ERROR )
     {
     CRASH_CLEANUP( 4, rc );

     DBGOUT;
     return ( rc );
     }

   //
   // --- is the current user in the administrators or SAPDB operators group?
   //
   for (ulGroup = 0; ulGroup < pTG->GroupCount && !(*pfIsOk); ulGroup++)
     {
     if (( pDomOperSid != NULL ) &&
         (EqualSid(pTG->Groups[ulGroup].Sid, pDomOperSid)))
       {
       *pfIsOk = TRUE;
       }
     else if (( plOperSid != NULL ) &&
              (EqualSid(pTG->Groups[ulGroup].Sid, plOperSid)))
       {
       *pfIsOk = TRUE;
       }
     else if ((EqualSid(pTG->Groups[ulGroup].Sid, pAdminSid))  ||
              (EqualSid(pTG->Groups[ulGroup].Sid, pSystemSid)))
       {
       *pfIsOk = TRUE;
       }
     }

   if ( pAdminSid )        FreeSid  ( pAdminSid );
   if ( pSystemSid )       FreeSid  ( pSystemSid );
   if ( pTG )              FREE_MEM ( pTG );
   if ( pDomOperSid )      FREE_MEM ( pDomOperSid );
   if ( plOperSid )        FREE_MEM ( plOperSid );

   DBGOUT;
   return ( NO_ERROR );
   }

/*------------------------------*/
LONG sql49c_get_user_textual_SID (SAPDB_Char *   accountName,
                                  PSZ *          ppszTextualSID)
 {
   #undef  MF__
   #define MF__ MOD__"sql49c_get_user_textual_SID"
   LONG                rc  = NO_ERROR;
   SAPDB_Char *        pOwnTextualSID = NULL;

   DBGIN;

    rc = sql49c_get_own_textual_SID (&pOwnTextualSID);
    if (NO_ERROR == rc)
    {
        if (NULL != accountName && '\0' != accountName[0])
        {
            PSID                pAccountSid       = NULL;
            SID_NAME_USE        snuType;

            rc = sql49c_lookup_account_name (NULL, accountName,
                &pAccountSid, NULL, &snuType );

            if ((rc == NO_ERROR) && (snuType != SidTypeUser))
            {
                rc = ERROR_NONE_MAPPED;
                FREE_MEM (pOwnTextualSID);
            }
            else if (NO_ERROR == rc)
            {
                rc =  sql49c_get_textual_SID (pAccountSid, ppszTextualSID );

                if (NO_ERROR == rc)
                {
                    if (0 != strcmp (*ppszTextualSID, pOwnTextualSID))
                    {
                        BOOL  isAdmin = false;
                        /* Only Administrators are allowed to get the SID from another user */
                        rc = sql49c_user_is_admin (&isAdmin);
                        if (NO_ERROR != rc)
                        {
                            FREE_MEM (*ppszTextualSID);
                            *ppszTextualSID = NULL;
                        }

                        if (NO_ERROR == rc && !isAdmin)
                        {
                            FREE_MEM (*ppszTextualSID);
                            *ppszTextualSID = NULL;
                            rc = ERROR_ACCESS_DENIED;
                        }
                    }
                }
                else
                {
                    *ppszTextualSID = NULL;
                }
            }

            if ( pAccountSid != NULL )
            {
                FREE_MEM ( pAccountSid );
            }

            if ( pOwnTextualSID != NULL )
            {
                FREE_MEM (pOwnTextualSID);
            }
        }
        else
        {
            *ppszTextualSID = pOwnTextualSID;
        }
    }

   DBGOUT;
   return( rc );
 }

#endif


/*
 * ========================== LOCAL FUNCTIONS =================================
 */

#if defined(_WIN32)

 static LONG sql49c_get_own_textual_SID (PSZ *ppszTextualSID )
 {
#undef  MF__
#define MF__ MOD__"sql49c_get_own_textual_SID"
     PTOKEN_USER               pTU = NULL;
     LONG                      rc  = NO_ERROR;

     DBGIN;


     //
     // --- get the default DACL and and the current users security identifier
     //     (SID) associated with the process/thread access token.
     //
     rc = sql49c_get_token_information ( &pTU, NULL, NULL);

     if ( rc != NO_ERROR )
     {
         *ppszTextualSID = NULL;
         DBGOUT;
         return ( rc );
     }

     rc =  sql49c_get_textual_SID( pTU->User.Sid, ppszTextualSID );

     if ( pTU ) FREE_MEM ( pTU );


     if ( rc != NO_ERROR )
     {
         *ppszTextualSID = NULL;
         DBGOUT;
         return( rc );
     }

     DBGOUT;
     return( rc );
 }

/*------------------------------*/

static LONG sql49c_lookup_account_name ( PSZ           pszNodeName,
                                         PSZ           pszAccountName,
                                         PSID          *ppSid,
                                         PSZ           *ppszRefDomain,
                                         PSID_NAME_USE psnuType )
  {
  #undef  MF__
  #define MF__ MOD__"sql49c_lookup_account_name"
  LONG                     rc  = NO_ERROR;
  PSID                     pSid;
  PSZ                      pszRefDomain;
  ULONG                    ulDomainSize;
  ULONG                    ulSidSize;

  DBGIN;

  if ( ppSid != NULL )
    *ppSid = NULL;

  if ( ppszRefDomain != NULL )
    *ppszRefDomain = NULL;

  if (( pszNodeName ) && ( pszNodeName[0] == '\0' ))
    pszNodeName = NULL;

  /*
  // --- lookup on local or dedicated computer first
  rc = sql03c_group_or_user_avail ( pszNodeName, pszAccountName );

  if ( rc != NO_ERROR )
    rc = sql03c_group_or_user_on_DC_avail ( pszAccountName );

  if ( rc != NO_ERROR )
  {
    DBGOUT;
    return( ERROR_NONE_MAPPED );
  } 
  */

  //
  //
  // --- allocate space and get account name
  //
  //
  ulDomainSize = DEFAULT_DOMAIN_NAME_SIZE;
  ulSidSize    = DEFAULT_SID_SIZE;

  for ( ;; )
    {
    // --- allocate space
    rc = ALLOC_MEM ( &pSid, ulSidSize );

    if ( rc != NO_ERROR )
      {
      DBGOUT;
      return( rc );
      }

    rc = ALLOC_MEM ( &pszRefDomain, ulDomainSize );

    if ( rc != NO_ERROR )
      {
      FREE_MEM ( pSid );
      DBGOUT;
      return( rc );
      }

    // --- now we ask for the account name
    if (!LookupAccountName( (LPSTR) pszNodeName, pszAccountName, pSid, &ulSidSize,
                            pszRefDomain, &ulDomainSize, psnuType ) )
      {
      rc = GetLastError ();

      if (( rc != NO_ERROR ) && ( rc != ERROR_INSUFFICIENT_BUFFER ))
        {
        if ( rc != ERROR_NONE_MAPPED )
          MSGCD (( ERR_LOOKUP_ACCOUNT_NAME, 1, rc ));

        // --- PTS: 1107756
        if (( rc == ERROR_TRUSTED_DOMAIN_FAILURE ) || ( rc == ERROR_TRUSTED_RELATIONSHIP_FAILURE ))
          rc = ERROR_NONE_MAPPED;

        FREE_MEM ( pSid );
        FREE_MEM ( pszRefDomain );
        DBGOUT;
        return ( rc );
        }

      rc = FREE_MEM ( pSid );

      if ( rc != NO_ERROR )
        {
        FREE_MEM ( pszRefDomain );
        DBGOUT;
        return( rc );
        }

      rc = FREE_MEM ( pszRefDomain );

      if ( rc != NO_ERROR )
        {
        DBGOUT;
        return( rc );
        }
      }
    else
      break;
    }

  if ( ppSid != NULL )
    *ppSid = pSid;
  else
    FREE_MEM ( pSid );

  if ( ppszRefDomain != NULL )
    *ppszRefDomain = pszRefDomain;
  else
    FREE_MEM ( pszRefDomain );


  DBGOUT;
  return ( NO_ERROR );
  }

/*------------------------------*/

static LONG sql49c_open_policy ( PSZ            pszNodeName,
                                 DWORD          DesiredAccess,
                                 t_PolicyHdl    *pPolicyHandle )
  {
# undef  MF__
# define MF__ MOD__"sql49c_open_policy"
  LSA_OBJECT_ATTRIBUTES     ObjectAttributes;
  LONG                      rc           = NO_ERROR;
  PLSA_UNICODE_STRING       pLsaNodeName = NULL;
  NTSTATUS                  Status;
  
  DBGIN;
  
  memset( pPolicyHandle, 0, sizeof( *pPolicyHandle ) );
  pPolicyHandle->Hdl = (LSA_HANDLE)UNDEF;

  // --- always initialize the object attributes to all zeroes.
  memset(&ObjectAttributes, 0, sizeof(ObjectAttributes));
  
  if (pszNodeName != NULL) 
    __sql49c_init_lsa_string ( pszNodeName, &pLsaNodeName );
  
  rc = LOAD_FUNC( pPolicyHandle, LsaOpenPolicy );

  if ( rc == NO_ERROR ) rc = LOAD_FUNC( pPolicyHandle, LsaFreeMemory );
  if ( rc == NO_ERROR ) rc = LOAD_FUNC( pPolicyHandle, LsaAddAccountRights );
  if ( rc == NO_ERROR ) rc = LOAD_FUNC( pPolicyHandle, LsaRemoveAccountRights );
  if ( rc == NO_ERROR ) rc = LOAD_FUNC( pPolicyHandle, LsaEnumerateAccountRights );
  if ( rc == NO_ERROR ) rc = LOAD_FUNC( pPolicyHandle, LsaClose );
  if ( rc == NO_ERROR ) rc = LOAD_FUNC( pPolicyHandle, LsaNtStatusToWinError );

  if ( rc == NO_ERROR )
    {
    // --- attempt to open the policy.
    Status = CALL_FUNC( pPolicyHandle, LsaOpenPolicy, ( pLsaNodeName, &ObjectAttributes, DesiredAccess, &pPolicyHandle->Hdl ));
  
    if ( Status ) 
      {
      rc = CALL_FUNC( pPolicyHandle, LsaNtStatusToWinError, ( Status ));
      MSGCD (( ERR_OPEN_POLICY, pszNodeName, rc ));
      SetLastError(rc);
      }
  
    if ( pLsaNodeName != NULL )
      FREE_MEM ( pLsaNodeName );
    }
  
  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

static LONG sql49c_close_policy ( t_PolicyHdl   *pPolicyHandle )
  {
# undef  MF__
# define MF__ MOD__"sql49c_close_policy"
  NTSTATUS            Status;
  LONG                rc                = NO_ERROR;

  if ( pPolicyHandle->hinst != NULL )
    {
    if ( pPolicyHandle->Hdl != (LSA_HANDLE)UNDEF )
    {
      Status = CALL_FUNC( pPolicyHandle, LsaClose, ( pPolicyHandle->Hdl ));

      if ( Status ) 
        rc = CALL_FUNC( pPolicyHandle, LsaNtStatusToWinError, ( Status ));
    }

    if ( !FreeLibrary( pPolicyHandle->hinst ) )
        rc = GetLastError();

    pPolicyHandle->hinst = NULL;
    }

  return rc;
  }

/*------------------------------*/

static LONG sql49c_set_priv_on_account ( t_PolicyHdl  *pPolicyHandle,
                                         PSZ          pszNodeName,
                                         PSZ          pszAccountName,
                                         PSZ          pszPrivilegeName,
                                         BOOL         fEnable )
  {
# undef  MF__
# define MF__ MOD__"sql49c_set_priv_on_account"

  PLSA_UNICODE_STRING pUserRights;
  ULONG               ulCountOfRights;
  PLSA_UNICODE_STRING pLsaPrivilegeName = NULL;
  LONG                rc                = NO_ERROR;
  PSID                pAccountSid       = NULL;
  NTSTATUS            Status;
  SID_NAME_USE        snuType;

  DBGIN;

  if ( pPolicyHandle->hinst == NULL )
    {
    MSGCD (( ERR_INVALID_PARAMETER, "pPolicyHandle" ))

    DBGOUT;
    return ( ERROR_INVALID_PARAMETER );
    }

  if (( pszAccountName[0] == '.' ) && ( pszAccountName[1] == '\\' ))
    pszAccountName += 2;
  
  rc = sql49c_lookup_account_name ( pszNodeName, pszAccountName,
                                    &pAccountSid, NULL, &snuType );
  
  if ((rc == NO_ERROR) && (snuType != SidTypeUser))
    rc = ERROR_NONE_MAPPED;
  
  if ( rc == ERROR_NONE_MAPPED )
    MSGCD (( ERR_WRONG_USERNAME, pszAccountName ))
    else if ( rc == NO_ERROR )
  {
    // --- create a LSA_UNICODE_STRING for the privilege name.
    if ( __sql49c_init_lsa_string ( pszPrivilegeName, &pLsaPrivilegeName ) != NO_ERROR )
    {
      if ( pAccountSid != NULL )
        FREE_MEM ( pAccountSid );

      DBGOUT;
      return ( rc );
    }
    
    // --- Account right already set?
    Status = CALL_FUNC( pPolicyHandle, LsaEnumerateAccountRights, ( pPolicyHandle->Hdl, pAccountSid, &pUserRights, &ulCountOfRights ));
    
    if ( Status )
    {
      rc = CALL_FUNC( pPolicyHandle, LsaNtStatusToWinError, ( Status ));
      SetLastError(rc);
    }
    else
    {
      for ( ; ulCountOfRights; ulCountOfRights-- )
      {
        if ( !memcmp( pUserRights[ulCountOfRights-1].Buffer, 
                      pLsaPrivilegeName->Buffer,
                      min( pUserRights[ulCountOfRights-1].Length,
                           pLsaPrivilegeName->Length ) ))
        {
          rc = ERROR_ALREADY_INITIALIZED;
        }
      }
      CALL_FUNC( pPolicyHandle, LsaFreeMemory, (pUserRights));
    }
    
    if ( rc != ERROR_ALREADY_INITIALIZED )
    {
      // --- grant or revoke the privilege, accordingly
      if(fEnable) 
        Status = CALL_FUNC( pPolicyHandle, LsaAddAccountRights, ( pPolicyHandle->Hdl, pAccountSid, pLsaPrivilegeName, 1 ));
      else 
        Status = CALL_FUNC( pPolicyHandle, LsaRemoveAccountRights, ( pPolicyHandle->Hdl, pAccountSid, FALSE, pLsaPrivilegeName, 1 ));
      
      if ( Status ) 
      {
        rc = CALL_FUNC( pPolicyHandle, LsaNtStatusToWinError, ( Status ));
        SetLastError(rc);
        
        if ( pszNodeName == NULL )
          MSGCD (( ERR_CHANGING_LOC_ACC_RIGHTS, pszAccountName, rc ))
          else
          MSGCD (( ERR_CHANGING_ACC_RIGHTS, pszNodeName, pszAccountName, rc ));
      }
    }
    else
      rc = NO_ERROR;
    
    if ( pLsaPrivilegeName != NULL )
      FREE_MEM ( pLsaPrivilegeName );
  }
  
  if ( pAccountSid != NULL )
    FREE_MEM ( pAccountSid );
  
  DBGOUT;
  return ( rc );
} 

/*------------------------------*/

static LONG sql49c_get_textual_SID( PSID psidSID, PSZ *ppszTextualSID )
  {
  #undef  MF__
  #define MF__ MOD__"sql49c_get_textual_SID"
  ULONG                    ulNumSubAuthorities;
  ULONG                    ulLen;
  ULONG                    ulSubAuthorityI;
  ULONG                    ulMaxSIDStringLen;
  SID_IDENTIFIER_AUTHORITY siaSidAuthority;
  LONG                     rc                     = NO_ERROR;

  DBGIN;


  //
  //    An SID could be written in this notation as follows:
  //
  //     S-R-I-S-S...
  //
  //    In the notation shown above,
  //
  //    S identifies the series of digits as an SID,
  //    R is the revision level,
  //    I is the identifier-authority value,
  //    S is subauthority value(s).
  //
  //    Example:
  //
  //     S-1-5-32-544
  //
  //    In this example,
  //    the SID has a revision level of 1,
  //    an identifier-authority value of 5,
  //    first subauthority value of 32,
  //    second subauthority value of 544.
  //    (Note that the above Sid represents the local Administrators group)
  //
  //    The resulting string will take one of two forms.  If the
  //    IdentifierAuthority value is not greater than 2^32, then the SID
  //    will be in the form:
  //
  //    S-1-5-21-2127521184-1604012920-1887927527-19009
  //      ^ ^ ^^ ^^^^^^^^^^ ^^^^^^^^^^ ^^^^^^^^^^ ^^^^^
  //      | | |      |          |          |        |
  //      +-+-+------+----------+----------+--------+--- Decimal
  //
  //    Otherwise it will take the form:
  //
  //    S-1-0x206C277C6666-21-2127521184-1604012920-1887927527-19009
  //      ^ ^^^^^^^^^^^^^^ ^^ ^^^^^^^^^^ ^^^^^^^^^^ ^^^^^^^^^^ ^^^^^
  //      |       |        |      |          |          |        |
  //      |   Hexadecimal  |      |          |          |        |
  //      +----------------+------+----------+----------+--------+--- Decimal
  //


  ulNumSubAuthorities = (DWORD)( *(GetSidSubAuthorityCount(psidSID)) );
  siaSidAuthority     = *(GetSidIdentifierAuthority(psidSID));

  //
  // --- compute buffer length
  //     S-SID_REVISION- + identifierauthority- + subauthorities- + NULL
  //
  ulMaxSIDStringLen = (15 + 12 + (12 * ulNumSubAuthorities) + 1) * sizeof(TCHAR);

  rc = ALLOC_MEM ( ppszTextualSID, ulMaxSIDStringLen );

  if ( rc != NO_ERROR )
    {
    DBGOUT;
    return( rc );
    }

  //
  // --- prepare S-SID_REVISION-
  //
  ulLen = wsprintf( *ppszTextualSID, TEXT("S-%lu"),
                    ((PISID)psidSID)->Revision );

  //
  // --- prepare SidIdentifierAuthority
  //
  if ( (siaSidAuthority.Value[0] != 0) || (siaSidAuthority.Value[1] != 0) )
    {
    ulLen += wsprintf( *ppszTextualSID + ulLen,
                       TEXT("-0x%02hx%02hx%02hx%02hx%02hx%02hx"),
                       (USHORT)siaSidAuthority.Value[0],
                       (USHORT)siaSidAuthority.Value[1],
                       (USHORT)siaSidAuthority.Value[2],
                       (USHORT)siaSidAuthority.Value[3],
                       (USHORT)siaSidAuthority.Value[4],
                       (USHORT)siaSidAuthority.Value[5] );
    }
  else
    {
    ulLen += wsprintf( *ppszTextualSID + ulLen, TEXT("-%lu"),
                       (ULONG)(siaSidAuthority.Value[5]      )   +
                       (ULONG)(siaSidAuthority.Value[4] <<  8)   +
                       (ULONG)(siaSidAuthority.Value[3] << 16)   +
                       (ULONG)(siaSidAuthority.Value[2] << 24) );
    }

  //
  // --- loop through SidSubAuthorities
  //
  for ( ulSubAuthorityI = 0;
        ulSubAuthorityI < ulNumSubAuthorities;
        ulSubAuthorityI++ )
  {
  ulLen += wsprintf( *ppszTextualSID + ulLen, TEXT("-%lu"),
                    *(GetSidSubAuthority(psidSID,ulSubAuthorityI)));
  }

  DBGOUT;
  return ( NO_ERROR );
  }

/*------------------------------*/

_INLINE LONG __sql49c_add_ace ( PACL    pNewDacl,
                                PSID    pSid,
                                ULONG   ulAccess,
                                ULONG   ulAceFlags )
  {
  #undef  MF__
  #define MF__  MOD__"__sql49c_add_ace"
  ACCESS_ALLOWED_ACE *paaAllowedAce;

  DBGIN;

  if ( ulAccess )
    {
    if ( ulAceFlags )
      {
      if ((!AddAccessAllowedAce( pNewDacl, ACL_REVISION2,
                                 ulAccess, pSid)) ||
          (!GetAce(pNewDacl, pNewDacl->AceCount - 1, (PVOID*)&paaAllowedAce)))
        {
        DBGOUT;
        return (GetLastError());
        }
      paaAllowedAce->Header.AceFlags = (BYTE)ulAceFlags;
      }
    else
      {
      if ((!AddAccessAllowedAce( pNewDacl, ACL_REVISION2,
                                 ulAccess, pSid)))
        {
        DBGOUT;
        return (GetLastError());
        }
      }
    }
  DBGOUT;
  return ( NO_ERROR );
  }

/*------------------------------*/

_INLINE LONG __sql49c_get_ace_sizes ( PUSER_ACCESS_RIGHTS_REC pAccRights,
                                      PSID                    pSid )
  {
  #undef  MF__
  #define MF__  MOD__"__sql49c_get_ace_sizes"
  ULONG    ulSize = 0;

  DBGIN;

  if ( pAccRights->ulAccess )
    {
    ulSize = ALIGN( GetLengthSid( pSid ) +
                    sizeof(ACCESS_ALLOWED_ACE)   -
                    sizeof(DWORD),
                    ALIGNMENT_VALUE );
    }

  if ( pAccRights->ulInheritAccess )
    ulSize *= 2;

  DBGOUT;
  return ( ulSize );
  }

/*------------------------------*/

_INLINE LONG __sql49c_init_lsa_string( PSZ                  pszString,
                                       PLSA_UNICODE_STRING  *ppLsaString )
{ 
#undef  MF__
#define MF__  MOD__"__sql49c_init_lsa_string"
  
  LONG  rc        = NO_ERROR;
  DWORD dwStrLen;
  
  DBGIN;
  
  if (pszString == NULL) 
  {
    rc = ALLOC_MEM ( ppLsaString, sizeof(LSA_UNICODE_STRING) );
    
    if ( rc == NO_ERROR )
    {
      (*ppLsaString)->Buffer        = NULL;
      (*ppLsaString)->Length        = 0;
      (*ppLsaString)->MaximumLength = 0;
    }
    
    DBGOUT;
    return ( rc );
  }
  
  
  dwStrLen = (DWORD)strlen(pszString);
  
  for ( ;; )
  {
    // --- allocate space
    rc = ALLOC_MEM ( ppLsaString, sizeof(**ppLsaString) + ((dwStrLen + 1) * sizeof(WCHAR)) );
    
    if ( rc != NO_ERROR )
    {
      DBGOUT;
      return( rc );
    }
    
    (*ppLsaString)->Buffer          = (PWSTR)((BYTE*)(*ppLsaString) + sizeof(**ppLsaString));
    (*ppLsaString)->Length          = (USHORT)(dwStrLen       * sizeof(WCHAR));
    (*ppLsaString)->MaximumLength   = (USHORT)((dwStrLen + 1) * sizeof(WCHAR));
    
    if ( !MultiByteToWideChar( CP_ACP, 0, pszString, -1, (*ppLsaString)->Buffer, dwStrLen + 1 ))
    {
      rc = GetLastError ();
      
      if (( rc != NO_ERROR ) && ( rc != ERROR_INSUFFICIENT_BUFFER ))
      {
        FREE_MEM ( *ppLsaString );
        DBGOUT;
        return ( rc );
      }
      
      rc = FREE_MEM ( *ppLsaString );
      
      if ( rc != NO_ERROR )
      {
        DBGOUT;
        return( rc );
      }
      
      dwStrLen += 1;
    }
    else
      break;
  }
  
  
  
  DBGOUT;
  return ( rc );
} 

/*------------------------------*/

_INLINE LONG __sql49c_add_aces ( PACL                    pNewDacl,
                                 PSID                    pSid,
                                 PUSER_ACCESS_RIGHTS_REC pAccRights )
  {
  #undef  MF__
  #define MF__  MOD__"__sql49c_add_aces"
  ACCESS_ALLOWED_ACE *paaAllowedAce;

  DBGIN;

  if ( pAccRights->ulAccess )
    {
    if ((!AddAccessAllowedAce( pNewDacl, ACL_REVISION2,
                               pAccRights->ulAccess, pSid)) ||
        (!GetAce(pNewDacl, pNewDacl->AceCount - 1,
                 (PVOID*)&paaAllowedAce)))
      {
      DBGOUT;
      return (GetLastError());
      }

    if ( pAccRights->ulAceFlags )
      paaAllowedAce->Header.AceFlags = (BYTE)pAccRights->ulAceFlags;

    if ( pAccRights->ulInheritAccess )
      {
      if ((!AddAccessAllowedAce( pNewDacl, ACL_REVISION2,
                                 pAccRights->ulInheritAccess, pSid)) ||
          (!GetAce(pNewDacl, pNewDacl->AceCount - 1,
                   (PVOID*)&paaAllowedAce)))
        {
        DBGOUT;
        return (GetLastError());
        }
      if ( pAccRights->ulInheritAceFlags )
        paaAllowedAce->Header.AceFlags = (BYTE)pAccRights->ulInheritAceFlags;
      }
    }


  DBGOUT;
  return ( NO_ERROR );
  }
#endif

/*------------------------------*/

static LONG sql49c_get_operators_SID ( PSID                    *pplOperSid,
                                       PSID                    *ppDomOperSid )
  {
  #undef  MF__
  #define MF__ MOD__"sql49c_get_operators_SID"
#if 1
 return ERROR_NONE_MAPPED;
#else
  LONG                    rc  = NO_ERROR;
  CHAR                    szOperator[MAX_COMPUTERNAME_LENGTH    + 1 + sizeof(DATABASE_OPERATOR_GROUP) + 1];
  CHAR                    szDomOperator[MAX_COMPUTERNAME_LENGTH + 1 + sizeof(DOMAIN_DATABASE_OPERATOR_GROUP) + 1];
  DWORD                   Len;
  SID_NAME_USE            snuType;
  SID_NAME_USE            snuDomType;

  DBGIN;

  //
  // --- get the sid of the database administrator groups
  //

  if ( pplOperSid != NULL )
    {
    Len = sizeof(szOperator);

    if ( GetComputerName ( szOperator, &Len ))
      strcat( szOperator, "\\" );
    else
      szOperator[0] = '\0';
    
    strcat( szOperator, DATABASE_OPERATOR_GROUP );

    rc = sql49c_lookup_account_name ( NULL, szOperator,
                                      pplOperSid, NULL, &snuType );

    if (( rc != NO_ERROR ) && ( rc != ERROR_NONE_MAPPED ))
      {
      *pplOperSid = NULL;

      DBGOUT;
      return ( rc );
      }

    // --- is the local database administrators SID valid?
    if (( *pplOperSid != NULL ) &&
        ( snuType != SidTypeGroup  &&  snuType != SidTypeAlias ))
      {
      FREE_MEM ( *pplOperSid );
      *pplOperSid = NULL;
      }
    }


  if ( ppDomOperSid != NULL )
    {
    if ( sql03c_get_domain_name ( szDomOperator ) == NO_ERROR )
      strcat ( szDomOperator, "\\" );
    else
      szDomOperator[0] = '\0';

    strcat( szDomOperator, DOMAIN_DATABASE_OPERATOR_GROUP );

    rc = sql49c_lookup_account_name ( NULL, szDomOperator,
                                      ppDomOperSid, NULL, &snuDomType );

    if (( rc != NO_ERROR ) && ( rc != ERROR_NONE_MAPPED ))
      {
      if (( pplOperSid != NULL ) && ( *pplOperSid != NULL ))
        {
        FREE_MEM ( *pplOperSid );
        *pplOperSid    = NULL;
        }
      *ppDomOperSid = NULL;

      DBGOUT;
      return ( rc );
      }

    // --- is the domain database administrators SID valid?
    if (( *ppDomOperSid != NULL ) &&
        ( snuDomType != SidTypeGroup  &&  snuDomType != SidTypeAlias ))
      {
      FREE_MEM ( *ppDomOperSid );
      *ppDomOperSid = NULL;
      }
    }

  DBGOUT;
  return (rc);
#endif
  }

/*------------------------------*/

static VOID* sql49c_load_func( t_PolicyHdl             *pPolicyHandle,
                               PSZ                     pszFuncName )
  {
  #undef  MF__
  #define MF__ MOD__"sql49c_load_func"
  static PSZ              pszDLL = LSA_DLL_NAME;
  LONG                    rc     = NO_ERROR;
  FARPROC                 pFunc;

  DBGIN;

  if ( pPolicyHandle->hinst == NULL )
    pPolicyHandle->hinst = LoadLibrary( pszDLL );

  if ( pPolicyHandle->hinst == NULL )
    {
    rc = GetLastError();

    MSGCD (( ERR_LOADING_FUNCTION, pszFuncName, pszDLL ));

    DBGOUT;
    return ( NULL );
    }

  pFunc = GetProcAddress( pPolicyHandle->hinst, pszFuncName);

  if ( pFunc == NULL )
    {
    MSGCD (( ERR_LOADING_FUNCTION, pszFuncName, pszDLL ));
    }

  DBGOUT;
  return ( pFunc );
  }

/*------------------------------*/

_INLINE VOID sql49c_func_err ( PSZ  pszFuncName ) 
  { 
  #undef  MF__
  #define MF__ MOD__"sql49c_func_err"
  MSGCD (( ERR_FUNCTION_NOT_LOADED, pszFuncName )); 
  };


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
