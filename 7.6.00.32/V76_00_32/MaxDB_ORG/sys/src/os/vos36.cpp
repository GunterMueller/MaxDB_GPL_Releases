/*!
  @file           vos36.cpp
  @author         FerdinandF
  @brief          DCOM load dynamic library
  @see            

\if EMIT_LICENCE
\endif
\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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




/* ================================================================================ */
/* ==========              REPLACEMENTS BY THE PREPROCESSOR               ========= */
/* ================================================================================ */

#define MAXPARAMS_OS36 256

/* ========================================================================== */
/* ==========              DECLARATION OF TYPES                     ========= */
/* ========================================================================== */

#include "vsp001.h"
#include "geo00d.h"
#include "hsp100.h"
#include "heo38.h"
#include "heo36.h"
#include "gos00.h"
//#include "heo78.h"
#include "heo01.h"
#include "liveCache/LVC_LoadLibrary.hpp"

typedef char t_part_data[sizeof(tsp00_KnlIdentifier) + 2];
typedef tsp1_dcom_param_info *dcom_param_info_ptr;
typedef BSTR                  ParamNames_os36[MAXPARAMS_OS36];

/* ========================================================================== */
/* ==========              DECLARATION OF CONSTANTS                 ========= */
/* ========================================================================== */

#define REG_SK_COMPONENTS                      _T("Components")
#define REG_SK_INSTALLATIONS                   _T("Installations")
#define ERROR_NO_STREAM ERROR_FILE_NOT_FOUND

/* ========================================================================== */
/* ==========               EXTERN FUNCTION PROTOTYPES              ========= */
/* ========================================================================== */


/* ========================================================================== */
/* ==========               LOCAL FUNCTION PROTOTYPES               ========= */
/* ========================================================================== */



/* ========================================================================== */
/* ==========              DEFINITION OF GLOBAL FUNCTIONS           ========= */
/* ========================================================================== */

extern HRESULT sql36getInstallationKey(char* szSubKey) {
  tsp00_Pathc             szInstallKey;
  tsp01_RteError          RteError;

  if ( !sqlGetDbrootPath( szInstallKey, TERM_WITH_DELIMITER_EO01, &RteError )) {
    return GEO00D_DCOM_INSTALLATION_PATH_NOT_FOUND;
  }
  
  if ( szInstallKey[strlen(szInstallKey)-1] == '\\' )
    szInstallKey[strlen(szInstallKey)-1] = '\0';
  char *psz = &szInstallKey[0];
  while ( psz = strchr(psz,'\\')) 
    *psz = '/';
  
  strcpy( szSubKey, REG_SK_INSTALLATIONS );
  strcat( szSubKey, "\\" );
  strcat( szSubKey, szInstallKey );
  if ( sql50_reg_applic_key_exists ( NULL, HKEY_LOCAL_MACHINE, szSubKey )) {
    return GEO00D_DCOM_INSTALLATION_NOT_REGISTERED;
  };
  return S_OK;
};

/* ========================================================================== */

EXTERN_C PROC WINAPI sql36GetProcAddress(
    HMODULE hModule,
    LPCSTR lpProcName
    )
{
  return GetProcAddress(hModule, lpProcName);
};

/* ========================================================================== */

extern HRESULT 
sql36LoadTypeLibFromProgId (BOOLEAN            InProcServ,
                            char*              cProgID,
                            CLSID&             ClsID,
                            HINSTANCE          *lpHInstance,
                            tsp00_C4           session,
                            tsp00_C64&           errtext)
{
  
  HRESULT		        hr;
  char              szSubKey[256];
  char              szSubKey2[256];
  char              szValue[256];
  ULONG             ulNumOfEntries = 1;
  REG_ENTRY_REC     EntyArray[1];

  hr = sql36getInstallationKey( szSubKey );
  if ( hr ) return( hr );

  strcat(szSubKey, "\\" );
  strcat(szSubKey, REG_SK_COMPONENTS);
  strcpy(szSubKey2,szSubKey);

  strcat(szSubKey, "\\" );
  strcat(szSubKey, cProgID);
  if ( sql50_reg_applic_key_exists ( NULL, HKEY_LOCAL_MACHINE, szSubKey )) {
    return GEO00D_DCOM_PROGID_NOT_FOUND;
  };

  EntyArray[0].ulValueType  = REG_SZ;
  EntyArray[0].pszValueName = "CLSID";        
  EntyArray[0].pValue       = &szValue;
  EntyArray[0].ulValueSize  = sizeof(szValue);

  hr = sql50_reg_get_applic_values ( NULL, HKEY_LOCAL_MACHINE, szSubKey, 
    ulNumOfEntries, &EntyArray[0]);
  if (( S_OK == hr ) && ( S_OK == EntyArray[0].rc )) {

    wchar_t		        wClsIDStr[64];
    for ( unsigned int i = 0; i <= strlen((const char *)&szValue[0]); i++ ) 
      wClsIDStr[i] = szValue[i]; 
    hr = CLSIDFromString( &wClsIDStr[0], &ClsID );
    if ( S_OK != hr  ) return( hr );
    
    strcat(szSubKey2, "\\CLSID\\" );
    strcat(szSubKey2, szValue );
    EntyArray[0].ulValueType  = REG_SZ;
    EntyArray[0].pszValueName = "Path";        
    EntyArray[0].pValue       = &szValue;
    EntyArray[0].ulValueSize  = sizeof(szValue);
    
    hr = sql50_reg_get_applic_values ( NULL, HKEY_LOCAL_MACHINE, szSubKey2, 
      ulNumOfEntries, &EntyArray[0]);
    if (( S_OK == hr ) && ( S_OK == EntyArray[0].rc )) {
      char *pc = strrchr( (char*)&szValue[0], '.' );
      if ( pc ) *pc = '\0';  /* erase '.dll' from Modulename */
      LVC_LoadLibrary((char*)&szValue[0], false, (HANDLE*)lpHInstance, errtext, sizeof(tsp00_ErrText)-1);
      if ( NULL == *lpHInstance ) hr = ERROR_FILE_NOT_FOUND;
    }
    else {
      hr = GEO00D_DCOM_COULD_NOT_RETRIEVE_PATH_VAL;
    }
  }
  else {
    hr = GEO00D_DCOM_COULD_NOT_RETRIEVE_PATH_VAL;
  }
  return( hr ); 
}

