/*!
  @file           vos02.c
  @author         RaymondR
  @brief          Version and OS-ID
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
#include "geo57.h"
#include "geo001.h"
#if defined (KERNEL)
# include "gos00k.h"            /* nocheck */  
#endif


//
//  DEFINES
//

#define MOD__  "VOS02C : "
#define MF__   MOD__"UNDEFINED"

static PSZ  pszProductSuites = NULL;
static PSZ  pszServicePack   = NULL;

//
//  MACROS
//
# define ENTER_CRIT_SEC()                                          \
            if (!CritSecInitialized) { CritSecInitialized = TRUE;  \
              InitializeCriticalSection(&CritSec); }               \
            EnterCriticalSection(&CritSec); CritSecCount++;

# define EXIT_CRIT_SEC()                                           \
            if (CritSecCount) { CritSecCount--;                    \
              LeaveCriticalSection(&CritSec); }

# define DELETE_CRIT_SEC()                                         \
            { DeleteCriticalSection(&CritSec);                     \
              CritSecInitialized = FALSE; }

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
#if defined (OSVERSIONINFOEX)
 static OSVERSIONINFOEX  VerInfo;
#else
 static OSVERSIONINFO    VerInfo;
#endif

static CRITICAL_SECTION  CritSec;
static BOOL              CritSecInitialized = FALSE;
static DWORD             CritSecCount       = 0; 
static PSZ               os02GetProductSuiteString();

//
//  LOCAL FUNCTION PROTOTYPES
//


//
// ========================== GLOBAL FUNCTIONS ================================
//

ULONG sql02_get_platform_id ( VOID )
{
#undef  MF__
#define MF__ MOD__"sql02_get_platform_id"
  
  DBGPAS;
  
  if ( VerInfo.dwOSVersionInfoSize != sizeof(VerInfo))  // -- get version info only once.
  {
    ENTER_CRIT_SEC();

    if ( VerInfo.dwOSVersionInfoSize != sizeof(VerInfo))
    {
      memset ( &VerInfo, 0, sizeof(VerInfo) );
      VerInfo.dwOSVersionInfoSize = sizeof(VerInfo);
      
      if ( !GetVersionEx (&VerInfo))
        memset ( &VerInfo, 0, sizeof(VerInfo) );
    }

    EXIT_CRIT_SEC();
  }
  
  return ( (ULONG)VerInfo.dwPlatformId );
}

/*------------------------------*/

VOID sql02_get_os_version ( PULONG pulMajorVersion,
                            PULONG pulMinorVersion )
{
#undef  MF__
#define MF__ MOD__"sql02_get_os_version"
  
  DBGPAS;
  
  if ( VerInfo.dwOSVersionInfoSize != sizeof(VerInfo))  // -- get version info only once.
  {
    ENTER_CRIT_SEC();

    if ( VerInfo.dwOSVersionInfoSize != sizeof(VerInfo))
    {
      memset ( &VerInfo, 0, sizeof(VerInfo) );
      VerInfo.dwOSVersionInfoSize = sizeof(VerInfo);
      
      if ( !GetVersionEx (&VerInfo))
        memset ( &VerInfo, 0, sizeof(VerInfo) );
    }

    EXIT_CRIT_SEC();
  }
  
  if ( pulMajorVersion != NULL )
    *pulMajorVersion = VerInfo.dwMajorVersion;

  if ( pulMinorVersion != NULL )
    *pulMinorVersion = VerInfo.dwMinorVersion;
  
  return;
}

/*------------------------------*/

#if defined (OSVERSIONINFOEX)
 WORD sql02_get_os_suite_mask( VOID )
 {
 #undef  MF__
 #define MF__ MOD__"sql02_get_os_suite_mask"
   
   DBGPAS;
   
   if ( VerInfo.dwOSVersionInfoSize != sizeof(VerInfo)) // -- get version info only once.
   {
     ENTER_CRIT_SEC();

     if ( VerInfo.dwOSVersionInfoSize != sizeof(VerInfo))
     {
       memset ( &VerInfo, 0, sizeof(VerInfo) );
       VerInfo.dwOSVersionInfoSize = sizeof(VerInfo);
       
       if ( !GetVersionEx (&VerInfo))
         memset ( &VerInfo, 0, sizeof(VerInfo) );
     } 

     EXIT_CRIT_SEC();
   }
   
   return VerInfo.wSuiteMask;
 }
#endif 

/*------------------------------*/

PSZ sql02_get_product_suite()
{
  PSZ pszSuite; 
 
  pszSuite = os02GetProductSuiteString();

  if ( pszSuite != NULL && pszSuite[0] == _T('\0') )
    pszSuite = NULL;

  return (pszSuite);
}

/*------------------------------*/

BOOL sql02_validate_product_suite( PSZ pszSuiteName )
{
  HKEY        hKey             = NULL;
  DWORD       dwType           = 0;
  DWORD       dwSize           = 0;
  PSZ         pszSuite;
  BOOL        fValidated       = FALSE ;

  pszSuite = os02GetProductSuiteString() ; 

  if (( pszSuite        == NULL )     ||
      ( pszSuiteName    == NULL )     ||
      ( pszSuiteName[0] == _T('\0') ))
  {
    return FALSE;
  }

  // --- Search for suite name in array of strings.
  while (*pszSuite) 
  {
    if ( _tcsstr( pszSuite, pszSuiteName ) != NULL ) 
    {
      fValidated = TRUE;
      break;
    }
    pszSuite += (_tcslen( pszSuite ) + 1);
  }

  return fValidated;
}

/*------------------------------*/

DWORD sql02_get_service_pack( void )
{
  HKEY          hKey             = NULL;
  DWORD         dwType           = 0;
  DWORD         dwSize           = 0;
  LONG          lResult;
  BOOL          fError           = FALSE;
  static DWORD  dwServicePack    = 0;

  if ( pszServicePack == NULL ) // -- get service pack string only once.
  {
    ENTER_CRIT_SEC();
    
    if ( pszServicePack == NULL ) 
    {
      // -- Open the ProductOptions key.
      lResult = RegOpenKey( HKEY_LOCAL_MACHINE, REG_SK_NT_CURRENT_VERSION, &hKey );
      
      if (lResult == ERROR_SUCCESS) 
      {
        // --- Determine required size of ProductSuites buffer.
        lResult = RegQueryValueEx( hKey, REG_VN_SERVICE_PACK, NULL, &dwType, 
                                   NULL, &dwSize );
        
        if ((lResult == ERROR_SUCCESS) && ( dwSize != 0 ))
        {
          // -- Allocate buffer.
          if ( ALLOC_MEM_EO57( &pszServicePack, dwSize ) == NO_ERROR_EO001 )
          {
            // -- Retrieve array of product suite strings.
            lResult = RegQueryValueEx( hKey, REG_VN_SERVICE_PACK, NULL,
                                       &dwType, (LPBYTE) pszServicePack, 
                                       &dwSize );
            
            if (( lResult != ERROR_SUCCESS ) || ( dwType != REG_SZ ))
              fError = TRUE;
          }
          else
            fError = TRUE;
        }
        else
          fError = TRUE;
      }
      else
        fError = TRUE;

      if ( hKey != NULL )
        RegCloseKey( hKey );

      if ( fError == TRUE )
      {
        if ( ALLOC_MEM_EO57( &pszServicePack, 1 ) == NO_ERROR_EO001 )
        {
          pszServicePack[0] = _T('\0');
        }
        else
        {
          EXIT_CRIT_SEC();
          return 0;
        }
      }
    }
    EXIT_CRIT_SEC();

    if ( pszServicePack != NULL ) 
      sscanf ( pszServicePack, "Service Pack %d", &dwServicePack );
  }

  return dwServicePack;
}

/*------------------------------*/

BOOL sql02_release_resources( VOID )
{
#undef  MF__
#define MF__ MOD__"sql02_release_resources"
  
  BOOL  Ok = TRUE;

  DBGPAS;

  DELETE_CRIT_SEC();

  if ( pszProductSuites != NULL )
  {
    FREE_MEM_EO57( pszProductSuites );
    Ok = TRUE;
    pszProductSuites = NULL;
  }

  if ( pszServicePack != NULL )
  {
    FREE_MEM_EO57( pszServicePack );
    Ok = TRUE;
    pszServicePack = NULL;
  }
  
  return Ok;
 }


//
// ========================== LOCAL FUNCTIONS =================================
//

static PSZ os02GetProductSuiteString()
{
  HKEY          hKey             = NULL;
  DWORD         dwType           = 0;
  DWORD         dwSize           = 0;
  LONG          lResult;
  BOOL          fError           = FALSE;

  if ( pszProductSuites == NULL ) // -- get suite string only once.
  {
    ENTER_CRIT_SEC();
    
    if ( pszProductSuites == NULL ) 
    {
      // -- Open the ProductOptions key.
      lResult = RegOpenKey( HKEY_LOCAL_MACHINE, REG_SK_PRODUCTOPTIONS, &hKey );
      
      if (lResult == ERROR_SUCCESS) 
      {
        // --- Determine required size of ProductSuites buffer.
        lResult = RegQueryValueEx( hKey, REG_VN_PRODUCT_SUITE, NULL, &dwType, 
                                   NULL, &dwSize );
        
        if ((lResult == ERROR_SUCCESS) && ( dwSize != 0 ))
        {
          // -- Allocate buffer.
          if ( ALLOC_MEM_EO57( &pszProductSuites, dwSize ) == NO_ERROR_EO001 )
          {
            // -- Retrieve array of product suite strings.
            lResult = RegQueryValueEx( hKey, REG_VN_PRODUCT_SUITE, NULL,
                                       &dwType, (LPBYTE) pszProductSuites, 
                                       &dwSize );
            
            if (( lResult != ERROR_SUCCESS ) || ( dwType != REG_MULTI_SZ ))
              fError = TRUE;
          }
          else
            fError = TRUE;
        }
        else
          fError = TRUE;
      }
      else
        fError = TRUE;

      if ( hKey != NULL )
        RegCloseKey( hKey );

      if ( fError == TRUE )
      {
        if ( ALLOC_MEM_EO57( &pszProductSuites, 1 ) == NO_ERROR_EO001 )
        {
          pszProductSuites[0] = _T('\0');
        }
        else
        {
          EXIT_CRIT_SEC();
          return "";
        }
      }
    }
    EXIT_CRIT_SEC();
  }
  return pszProductSuites;
}


//
// =============================== END ========================================
//
