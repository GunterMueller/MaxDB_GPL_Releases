/*!
  @file           vos03.c
  @author         RaymondR
  @brief          SAPDB Operators
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
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"


#if !defined (UNICODE)  
# undef  LPTSTR
# define LPTSTR      LPWSTR
#endif

#include           <lm.h>

// LPWKSTA_INFO_102 work around!
#if !defined (UNICODE)
# undef  LPTSTR
#endif


//
//  DEFINES
//

#define MOD__  "VOS03C : "
#define MF__   MOD__"UNDEFINED"

#define NET_API_DLL               "NETAPI32.DLL"

//
//  MACROS
//
#define MK_PA(f)       (NET_API_FUNCTION *lp##f)
#define LOAD_FUNC(f)   ((lp##f = sql03c_load_net_func(#f)) == NULL) ? \
                         -1 : NO_ERROR


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
static HINSTANCE      hinst = NULL;
static NET_API_STATUS MK_PA(NetGetDCName)     ( LPWSTR   servername,
                                                LPWSTR   domainname,
                                                LPBYTE  *bufptr );
static NET_API_STATUS MK_PA(NetLocalGroupAdd) ( LPWSTR   servername,
                                                DWORD    level,
                                                LPBYTE   buf,
                                                LPDWORD  parm_err );
static NET_API_STATUS MK_PA(NetGroupAdd)      ( LPWSTR   servername,
                                                DWORD    level,
                                                LPBYTE   buf,
                                                LPDWORD  parm_err );

static NET_API_STATUS MK_PA(NetLocalGroupDel) ( LPWSTR   servername,
                                                LPWSTR   group );
static NET_API_STATUS MK_PA(NetGroupDel)      ( LPWSTR   servername,
                                                LPWSTR   group );

static NET_API_STATUS MK_PA(NetApiBufferFree) ( LPVOID   Buffer );

static NET_API_STATUS MK_PA(NetUserModalsGet) ( LPWSTR   servername,
                                                DWORD    level,
                                                LPBYTE   *bufptr );
static NET_API_STATUS MK_PA(NetWkstaGetInfo)  ( LPWSTR   servername,  
                                                DWORD    level,        
                                                LPBYTE   *bufptr );

static NET_API_STATUS MK_PA(NetGroupGetInfo)  ( LPCWSTR  servername, 
                                                LPCWSTR  groupname,  
                                                DWORD    level,        
                                                LPBYTE   *bufptr );

static NET_API_STATUS MK_PA(NetUserGetInfo)   ( LPCWSTR  servername,  
                                                LPCWSTR  username,    
                                                DWORD    level,        
                                                LPBYTE   *bufptr );




//
//  LOCAL FUNCTION PROTOTYPES
//
static VOID*  sql03c_load_net_func              ( CHAR    *szFuncName );


//
// ========================== GLOBAL FUNCTIONS ================================
//

APIRET sql03c_add_SAPDB_operators_group ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql03c_add_SAPDB_operators_group"
  #ifdef CRASH_CLEANUP
   #undef CRASH_CLEANUP
  #endif
  #define CRASH_CLEANUP(_no,_rc)                                            \
      MSGCD(( ERR_ADD_SAPDB_ADMIN_GROUP, _no, _rc ));                       \
      if( lpwszDomainController ) lpNetApiBufferFree ( lpwszDomainController);
  LONG               rc                     = NO_ERROR;
  LPWSTR             lpwszDomainController  = NULL;
  LOCALGROUP_INFO_1  LocalGroupInfo1;

  DBGIN;

  // --- load this net functions by 'LoadLibrary'
  //     because there is no 'NETAPI32.DLL' available
  //     on Windows95 systems
  //
  rc = LOAD_FUNC( NetGetDCName );

  if ( rc == NO_ERROR )
    rc = LOAD_FUNC( NetLocalGroupAdd );

  if ( rc == NO_ERROR )
    rc = LOAD_FUNC( NetGroupAdd );

  if ( rc == NO_ERROR )
    rc = LOAD_FUNC( NetApiBufferFree );

  if ( rc != NO_ERROR )
    return ( rc );


  // --- get domain controller server name
  rc = lpNetGetDCName ( NULL, NULL, ( LPBYTE *) &lpwszDomainController );

  if (( rc != NERR_Success ) && ( rc != NERR_DCNotFound ))
    {
    CRASH_CLEANUP( 1, rc );
    DBGOUT;
    return ( rc );
    }

  if ( rc != NERR_DCNotFound )
    {
    // --- set name & comment
    LocalGroupInfo1.lgrpi1_name    = L_DOMAIN_DATABASE_OPERATOR_GROUP;
    LocalGroupInfo1.lgrpi1_comment = L_DATABASE_OPERATOR_GROUP_DESC;

    rc = lpNetGroupAdd ( lpwszDomainController, 1,
                         (LPBYTE) &LocalGroupInfo1, NULL);

    if (( rc != NERR_Success )        &&
        ( rc != NERR_GroupExists )    && // --- group already there?
        ( rc != ERROR_ALIAS_EXISTS )  && // --- group already there?
        ( rc != NERR_NotPrimary )     && // --- only allowed on the primary domain controller
        ( rc != ERROR_LOGON_FAILURE ) &&
        ( rc != ERROR_ACCESS_DENIED ))
      {
      CRASH_CLEANUP( 2, rc );
      DBGOUT;
      return ( rc );
      }

    // --- set name & comment
    LocalGroupInfo1.lgrpi1_name    = L_DATABASE_OPERATOR_GROUP;
    LocalGroupInfo1.lgrpi1_comment = L_DATABASE_OPERATOR_GROUP_DESC;

    rc = lpNetLocalGroupAdd ( lpwszDomainController, 1,
                              (LPBYTE) &LocalGroupInfo1, NULL);

    if (( rc != NERR_Success )        &&
        ( rc != NERR_GroupExists )    && // --- group already there?
        ( rc != ERROR_ALIAS_EXISTS )  && // --- check if group already there
        ( rc != NERR_NotPrimary )     && // --- only allowed on the primary domain controller
        ( rc != ERROR_LOGON_FAILURE ) &&
        ( rc != ERROR_ACCESS_DENIED ))
      {
      CRASH_CLEANUP( 3, rc );
      DBGOUT;
      return ( rc );
      }
    }

  // --- set name & comment
  LocalGroupInfo1.lgrpi1_name    = L_DATABASE_OPERATOR_GROUP;
  LocalGroupInfo1.lgrpi1_comment = L_DATABASE_OPERATOR_GROUP_DESC;

  rc = lpNetLocalGroupAdd ( NULL, 1, (LPBYTE) &LocalGroupInfo1, NULL);

  if (( rc != NERR_Success )       &&
      ( rc != NERR_GroupExists )   && // --- group already there?
      ( rc != ERROR_ALIAS_EXISTS ) && // --- check if group already there
      ( rc != ERROR_ACCESS_DENIED ))
    {
    CRASH_CLEANUP( 4, rc );
    DBGOUT;
    return ( rc );
    }

  if ( lpwszDomainController != NULL )
    lpNetApiBufferFree ( lpwszDomainController );

  if (( rc == ERROR_ALIAS_EXISTS ) || 
      ( rc == NERR_GroupExists )   ||
      ( rc == NERR_NotPrimary )    ||
      ( rc == ERROR_LOGON_FAILURE ))
  {
    rc = NO_ERROR;
  }

  DBGOUT;
  return( rc );
  }

/*------------------------------*/

APIRET sql03c_del_SAPDB_operators_group ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql03c_del_SAPDB_operators_group"
  #ifdef CRASH_CLEANUP
   #undef CRASH_CLEANUP
  #endif
  #define CRASH_CLEANUP(_no,_rc)                                            \
      MSGCD(( ERR_ADD_SAPDB_ADMIN_GROUP, _no, _rc ));                       \
      if( lpwszDomainController ) lpNetApiBufferFree ( lpwszDomainController);
  LONG               rc                     = NO_ERROR;
  LPWSTR             lpwszDomainController  = NULL;

  DBGIN;

  // --- load this net functions by 'LoadLibrary'
  //     because there is no 'NETAPI32.DLL' available
  //     on Windows95 systems
  //
  rc = LOAD_FUNC( NetGetDCName );

  if ( rc == NO_ERROR )
    rc = LOAD_FUNC( NetLocalGroupDel );

  if ( rc == NO_ERROR )
    rc = LOAD_FUNC( NetGroupDel );

  if ( rc == NO_ERROR )
    rc = LOAD_FUNC( NetApiBufferFree );

  if ( rc != NO_ERROR )
    return ( rc );


  // --- get domain controller server name
  rc = lpNetGetDCName ( NULL, NULL, ( LPBYTE *) &lpwszDomainController );

  if (( rc != NERR_Success ) && ( rc != NERR_DCNotFound ))
    {
    CRASH_CLEANUP( 1, rc );
    DBGOUT;
    return ( rc );
    }

  if ( rc != NERR_DCNotFound )
    {
    rc = lpNetGroupDel ( lpwszDomainController, L_DOMAIN_DATABASE_OPERATOR_GROUP );

    if (( rc != NERR_Success )        &&
        ( rc != NERR_GroupNotFound )  && // --- group not there?
        ( rc != ERROR_NO_SUCH_ALIAS ) && // --- check if group not there
        ( rc != NERR_NotPrimary )     && // --- only allowed on the primary domain controller
        ( rc != ERROR_LOGON_FAILURE ) &&
        ( rc != ERROR_ACCESS_DENIED ))
      {
      CRASH_CLEANUP( 2, rc );
      DBGOUT;
      return ( rc );
      }

    rc = lpNetLocalGroupDel ( lpwszDomainController, L_DATABASE_OPERATOR_GROUP );

    if (( rc != NERR_Success )        &&
        ( rc != NERR_GroupNotFound )  && // --- group not there?
        ( rc != ERROR_NO_SUCH_ALIAS ) && // --- check if group not there
        ( rc != NERR_NotPrimary )     && // --- only allowed on the primary domain controller
        ( rc != ERROR_LOGON_FAILURE ) &&
        ( rc != ERROR_ACCESS_DENIED ))
      {
      CRASH_CLEANUP( 3, rc );
      DBGOUT;
      return ( rc );
      }
    }

  rc = lpNetLocalGroupDel ( NULL, L_DATABASE_OPERATOR_GROUP );

  if (( rc != NERR_Success )        &&
      ( rc != NERR_GroupNotFound )  && // --- group not there?
      ( rc != ERROR_NO_SUCH_ALIAS ) && // --- check if group not there
      ( rc != ERROR_ACCESS_DENIED ))
    {
    CRASH_CLEANUP( 4, rc );
    DBGOUT;
    return ( rc );
    }

  if ( lpwszDomainController != NULL )
    lpNetApiBufferFree ( lpwszDomainController );

  if (( rc == ERROR_NO_SUCH_ALIAS ) || 
      ( rc == NERR_GroupNotFound )  ||
      ( rc == NERR_NotPrimary )     ||
      ( rc == ERROR_LOGON_FAILURE ))
  {
    rc = NO_ERROR;
  }

  DBGOUT;
  return( rc );
  }

/*------------------------------*/

APIRET sql03c_group_or_user_on_DC_avail ( PSZ pszGroupOrUserName )
  {
  #undef  MF__
  #define MF__ MOD__"sql03c_group_or_user_on_DC_avail"
  #ifdef CRASH_CLEANUP
   #undef CRASH_CLEANUP
  #endif
  LONG                 rc                     = NO_ERROR;
  LPWSTR               lpwszDomainController  = NULL;
  CHAR                 szNodeName[MAX_COMPUTERNAME_LENGTH + 1];

  rc = LOAD_FUNC( NetApiBufferFree );

  if ( rc == NO_ERROR )
    rc = LOAD_FUNC( NetGetDCName );

  rc = lpNetGetDCName ( NULL, NULL, ( LPBYTE *) &lpwszDomainController );

  if ( rc == NERR_Success )
    {
    WideCharToMultiByte( CP_ACP, 0, lpwszDomainController, -1,
                         szNodeName, MAX_COMPUTERNAME_LENGTH,
                         NULL, NULL );

    szNodeName[MAX_COMPUTERNAME_LENGTH] = '\0';

    rc = sql03c_group_or_user_avail ( szNodeName, pszGroupOrUserName );

    lpNetApiBufferFree ( lpwszDomainController);
    }

  DBGOUT;
  return( rc );
  } 

/*------------------------------*/

APIRET sql03c_group_or_user_avail ( PSZ pszNodeName,
                                    PSZ pszGroupOrUserName )
  {
  #undef  MF__
  #define MF__ MOD__"sql03c_group_or_user_avail"
  #ifdef CRASH_CLEANUP
   #undef CRASH_CLEANUP
  #endif
  LONG                 rc                     = NO_ERROR;
  LPBYTE               bufptr                 = NULL;
  WCHAR                wszGroupOrUserName[256];
  WCHAR                wszNodeName[MAX_COMPUTERNAME_LENGTH + 1];
  LPWSTR               lpwszNodeName          = NULL;

  DBGIN;

  rc = LOAD_FUNC( NetApiBufferFree );

  if ( rc == NO_ERROR )
    rc = LOAD_FUNC( NetGroupGetInfo );

  if ( rc == NO_ERROR )
    rc = LOAD_FUNC( NetUserGetInfo );

  if ( rc != NO_ERROR )
    return ( rc );

  if ( pszNodeName == NULL )
    lpwszNodeName = NULL;
  else
    {
    lpwszNodeName = wszNodeName;
    MultiByteToWideChar( CP_ACP, 0, pszNodeName, -1, wszNodeName, 
                         sizeof(wszNodeName)/sizeof(wszNodeName[0]) - 1);
    }

  MultiByteToWideChar( CP_ACP, 0, pszGroupOrUserName, -1, wszGroupOrUserName, 
                       sizeof(wszGroupOrUserName)/sizeof(wszGroupOrUserName[0]) - 1);


  rc = lpNetGroupGetInfo( lpwszNodeName, wszGroupOrUserName, 0, &bufptr );

  if ( bufptr != NULL )
    lpNetApiBufferFree ( bufptr );

  if ( rc != NERR_Success )
    {
    rc = lpNetUserGetInfo( lpwszNodeName, wszGroupOrUserName, 0, &bufptr );

    if ( bufptr != NULL )
      lpNetApiBufferFree ( bufptr );
    }

  DBGOUT;
  return( rc );
  } 

/*------------------------------*/

#if !defined ( ALLOW_GET_DC_NAME )

APIRET sql03c_get_domain_name ( PSZ pszDomainName )
{
  //
  // 'sql03c_get_domain_name' is not supported because of 
  // 'NetGetDCName' which consumes a lot of time if the computer 
  // is running in a workgroup without domain controller!!!!!!!!!!!!
  //

  return NERR_DCNotFound;
}

#else

APIRET sql03c_get_domain_name ( PSZ pszDomainName )
  {
  #undef  MF__
  #define MF__ MOD__"sql03c_get_domain_controller"
  #ifdef CRASH_CLEANUP
   #undef CRASH_CLEANUP
  #endif
  #define CRASH_CLEANUP(_no,_rc)                                        \
      MSGCD(( ERR_GET_DOMAIN_NAME, _no, _rc ));                         \
      if( pWkstaInfo102 )         lpNetApiBufferFree ( pWkstaInfo102 ); \
      if( lpwszDomainController ) lpNetApiBufferFree ( lpwszDomainController);
  LONG                 rc                     = NO_ERROR;
  LPWSTR               lpwszDomainController  = NULL;
  LPWKSTA_INFO_102     pWkstaInfo102          = NULL;

  
  DBGIN;

  rc = LOAD_FUNC( NetWkstaGetInfo );

  if ( rc == NO_ERROR )
    rc = LOAD_FUNC( NetApiBufferFree );

  if ( rc == NO_ERROR )
    rc = LOAD_FUNC( NetGetDCName );

  if ( rc != NO_ERROR )
    return ( rc );

  // --- get domain controller server name
  rc = lpNetGetDCName ( NULL, NULL, ( LPBYTE *) &lpwszDomainController );

  if ( rc == NERR_Success )
  {
    // -- use domain controller server name to ge the next domain name
    //
    rc = lpNetWkstaGetInfo(NULL, 102, (LPBYTE *)&pWkstaInfo102);

    if ( rc != NERR_Success ) 
    {
      pWkstaInfo102 = NULL;
      CRASH_CLEANUP( 1, rc );
      DBGOUT;
      return ( rc );
    }

    WideCharToMultiByte( CP_ACP, 0, pWkstaInfo102->wki102_langroup, -1,
                         pszDomainName, MAX_COMPUTERNAME_LENGTH,
                         NULL, NULL );
    
    pszDomainName[MAX_COMPUTERNAME_LENGTH] = '\0';
    
    lpNetApiBufferFree ( pWkstaInfo102 );
    lpNetApiBufferFree ( lpwszDomainController );
  }


  DBGOUT;
  return( rc );
  }
#endif

//
// ========================== LOCAL FUNCTIONS =================================
//

static VOID* sql03c_load_net_func( CHAR  *szFuncName )
  {
  #undef  MF__
  #define MF__ MOD__"sql03c_load_net_func"
  LONG             rc    = NO_ERROR;
  FARPROC          pFunc = NULL;

  DBGIN;

  if ( hinst == NULL )
    hinst = LoadLibrary( NET_API_DLL );

  if ( hinst != NULL )
    pFunc = GetProcAddress( hinst, szFuncName);

  if (( hinst == NULL ) || ( pFunc == NULL ))
    {
    rc = GetLastError();
    MSGD(( ERR_STARTING_FUNC_FOM_DLL, szFuncName, NET_API_DLL, rc ));
    }

  DBGOUT;
  return ( pFunc );
  }

//
// =============================== END ========================================
//
