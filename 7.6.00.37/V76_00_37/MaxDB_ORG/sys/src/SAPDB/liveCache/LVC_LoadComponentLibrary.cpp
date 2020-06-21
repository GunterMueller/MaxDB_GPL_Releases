/*!
 * \file    LVC_LoadComponentLibrary.cpp
 * \author  IvanS, ThomasA, FerdiF
 * \brief   Load a component library and calculate an MD5 footprint.
 */
/*

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



*/

#include "liveCache/LVC_LoadComponentLibrary.hpp"
#include "liveCache/LVC_ILoadComponentLibraryCache.hpp"
#include "liveCache/LVC_LoadLibrary.hpp"
#include "geo00d.h"                          
#include "gip00.h"                          
#include "SAPDBCommon/Algorithms/SAPDBAlgo_MD5File.h"      
#include "heo02x.h"
#include "geo573.h"
#include "hgg11.h"
#include "Oms/OMS_DbpError.hpp"

externC 
{
  typedef HRESULT (__stdcall *DLLGETCLASSOBJECT)(REFCLSID rclsid, REFIID riid, LPVOID* ppv);
}

/*----------------------------------------------------------------------------*/

HRESULT LVC_LoadComponentLibrary(char const*     libName,
                                 const tsp00_C8 &packageId,
                                 HANDLE*         hInstance,
                                 tsp00_C32      &MD5Footprint)
{
  // first check if library information has already been cached 
  if (LVC_ILoadComponentLibraryCache::GetInstance().Find(packageId, hInstance, MD5Footprint))
    return S_OK;

  /* 1) load library                                                        */
  /* truncate file-type ( .dll, .so, .sl, ... ),                            */
  /* if specified by user, because it will be appended by LVC_LoadLibrary   */

  const bool  loadWithGlobalScope = false;
  char        errtext[256];
  int         len = strlen(&errtext[0]);
  char        libpath[2048];
  strcpy(&libpath[0], libName);
  char *pc  = strrchr( &libpath[0], '.' );
  if ((NULL != pc) && !strcmp(pc,DLLEXT_IP00))*pc = '\0';
  LVC_LoadLibrary(&libpath[0], 
    loadWithGlobalScope,                      
    hInstance, 
    &errtext[len], 
    sizeof(errtext)-len-1);
  if ( NULL == *hInstance )   /* PTS 1127430, PG: dereferencing hInstance */
    return ERROR_FILE_NOT_FOUND;

  /* 2) calculate MD5 footprint */
  char sum[33];
  bool have_sum = true;
  strcpy(&libpath[0], libName);
  pc  = strrchr( &libpath[0], '.' );
  if ( !((NULL != pc) && !strcmp(pc,DLLEXT_IP00)))
    strcat(&libpath[0], DLLEXT_IP00);   // only append type if it doesn't exist yet

  if (SAPDBAlgo_MD5File (&libpath[0], sum) == 0)
  {
    const char *szEnvVar = LDLIBPATH_IP00;
    const char *szSep    = ENVSEP_IP00;  
    char *szPath = getenv(szEnvVar);
    have_sum = false;
    while (szPath && !have_sum)
    {
      /* if file not found, retry to open after prefixing filename */
      /* with one path after the over found within LDLIBPATH_IP00  */
      strcpy(&libpath[0], szPath);
      szPath   = strchr(szPath,ENVSEP_IP00[0]);
      if (szPath) szPath = szPath+1; /* skip separator */
      char *pc = strchr(&libpath[0],ENVSEP_IP00[0]);
      if ( pc) *pc = '\0';
      strcat(&libpath[0], PATHSEP_IP00);
      strcat(&libpath[0], libName);
      pc  = strrchr( &libpath[0], '.' );
      if ( !((NULL != pc) && !strcmp(pc,DLLEXT_IP00)))
        strcat(&libpath[0], DLLEXT_IP00);   // only append type if it doesn't exist yet
      if (!SAPDBAlgo_MD5File(&libpath[0], sum) == 0) 
        have_sum = true;
    }  
  }
  if ( have_sum ) 
  {
    memcpy(&MD5Footprint, sum, sizeof(MD5Footprint));
    LVC_ILoadComponentLibraryCache::GetInstance().Insert(packageId, hInstance, MD5Footprint);
  }
  else
  {
    memset(&MD5Footprint, '\00', sizeof(MD5Footprint));
    return GEO00D_DCOM_COULD_NOT_RETRIEVE_MD5_MARK;
  }
  return S_OK;
}

/*----------------------------------------------------------------------------*/

HRESULT LVC_LoadComponentLibrary(char const      *libName, 
                                 const tsp00_C8  &packageId,
                                 const GUID      &CoClsId,
                                 const tsp00_C32 &MD5Footprint,
                                 tsp00_Addr      &ifactory)
{
  HRESULT hr;
  tsp00_C32   MD5Mark;
  HANDLE      hInstance;
  hr = LVC_LoadComponentLibrary(libName, packageId, &hInstance, MD5Mark);
  if (( S_OK == hr ) && memcmp(&MD5Footprint, &MD5Mark, sizeof(MD5Mark)) )
  {
      tsp00_Version kernelVersion;
      g11kernel_version (kernelVersion);
      int extraNumber;
      g11ParseExtraNumber (kernelVersion, extraNumber);

      // meaning of extra number :
      // 1. digit : 0 = test, 1 = productive
      // 2. digit : 0 = dev, 1 = cor, 2 = ramp
      // 3. digit : 0 = undef, 1 = oltp, 2 = livecache, 3 = all
      // PTS 1121587 & 1127333, don't check MD5 in development environment (dev) 
      if (( (extraNumber / 10) % 10 ) != 0 )    
      {
          hr = GEO00D_DCOM_MD5_MARK_DOES_NOT_MATCH;
      }
  }
  if (S_OK == hr)
  {
    DLLGETCLASSOBJECT DllGetClassObject;
    tsp00_ErrText     errtext;
    DllGetClassObject = (DLLGETCLASSOBJECT)sqlGetProcAddress(hInstance, 
      "DllGetClassObject", errtext, sizeof(tsp00_ErrText)-1 );
    if (DllGetClassObject == NULL) {
      hr = GEO00D_DCOM_NO_COCLASS_OBJECT_FOUND;
    } else {
      try {  
        /* !"§$ TODO Definition ist in OMS_Defines.hpp*/
        const IID IID_IClassFactory = {0x00000001,0x0000,0x0000,
                                      {0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
        hr = DllGetClassObject(CoClsId, 
          IID_IClassFactory,
          (void**)&ifactory);
      }                                           
      catch ( BAD_ALLOC_GEO573&) {                    
        DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
        if (pCBInterface){
          pCBInterface->dbpCaughtBadAlloc();
        }        
        hr = GEO00D_DCOM_BAD_ALLOC;                   
      }                                               
    }
    // **** activate in the future !!! sqlHookCrtIOFunctions();
  }
  return hr;
};
