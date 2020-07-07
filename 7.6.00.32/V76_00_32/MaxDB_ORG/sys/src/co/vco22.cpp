/* @lastChanged: "1998-07-10  11:26"
 * @filename:    vco22.cpp
 * @purpose:     "System Stored Procedures"
 * @release:     7.2.0.0
 * @see:         ""
 *
 * @Copyright (c) 1998-2005 SAP AG"



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/****************************************************************************/
/* DbpInstall.cpp : Dllmain + Factory + registration of COM-Object            */
/****************************************************************************/
#define   INITGUID

#include "lcbasetypes.h"     // nocheck
#include "livecachetypes.h"  // nocheck
#include "vco20.h"           // nocheck
#include "hco20.h"           // nocheck
#include "vco20_i.c"         // nocheck

GLOBAL_DLL_DATA

/****************************************************************************/
/* DLL global data                                                          */

class t_CModulGlobal {
public:
  long        m_nLockCnt;
  char        m_szModule[_MAX_PATH];
  
  VOID  init() {
    m_nLockCnt  = 0;
    memset(m_szModule,'\0',_MAX_PATH);
  };
};

t_CModulGlobal	g_CModul;

/****************************************************************************/
/*  ClassFactory Implemention                                               */

class CDbpInstallFactory : public IClassFactory {
private:
	long m_cRef;
public:
	CDbpInstallFactory() : m_cRef(1) {}
	~CDbpInstallFactory() {}

  STDMETHODIMP QueryInterface(const IID& iid, LPVOID* ppv) {    
    if ((iid == IID_IUnknown) || (iid == IID_IClassFactory))
      *ppv = STATIC_CAST(IClassFactory*, this); 
    else {
      *ppv = NULL ;
      return E_NOINTERFACE ;
    }
    (REINTERPRET_CAST(IUnknown*,*ppv))->AddRef() ;
    return S_OK ;
  }

  STDMETHODIMP_(ULONG) AddRef() {
    return co90InterlockedIncrement(&m_cRef) ;
  }
  
  STDMETHODIMP_(ULONG) Release() {
    if (co90InterlockedDecrement(&m_cRef) == 0) {
      delete this ;
      return 0 ;
    }
    return m_cRef ;
  }
  
  STDMETHODIMP CreateInstance(IUnknown* pUnknownOuter,
                                        const IID& iid,
                                        LPVOID* ppv) {
    if (pUnknownOuter != NULL)
      return CLASS_E_NOAGGREGATION ;
    
    CDbpInstall* pCDbpInstall = new CDbpInstall ;
    if (pCDbpInstall == NULL)
      return E_OUTOFMEMORY ;
    
    return pCDbpInstall->QueryInterface(iid, ppv);
  }
  
  STDMETHODIMP LockServer(BOOL bLock) 
  {
    if (bLock)
    {
      co90InterlockedIncrement(&g_CModul.m_nLockCnt); 
      co90InterlockedIncrement(&m_cRef);
    }
    else
    {
      co90InterlockedDecrement(&g_CModul.m_nLockCnt);
      if ( 0 == co90InterlockedDecrement(&m_cRef))
      {
        delete this ;
        return 0 ;
      }
    }
    return S_OK ;
  }
};


/****************************************************************************/
/* DLL Main Entry Point                                                     */

STDAPI_(BOOL) DllMain(HINSTANCE hInstance, 
                      DWORD dwReason, 
                      LPVOID szModule) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    DWORD retLength;
    g_CModul.init();
//    Init_PsODll();
#if defined (WIN32)
    retLength =	GetModuleFileName(hInstance, g_CModul.m_szModule, _MAX_PATH ); 
    if ( 0 == retLength ) return FALSE;
#else
    char  *szCC = (char*) szModule;
    if ( strlen(szCC) > _MAX_PATH) return FALSE;
    strcpy(g_CModul.m_szModule, szCC);
#endif
  }
  else 
    if (dwReason == DLL_PROCESS_DETACH)	{
	}
	return TRUE; 
}

/****************************************************************************/
/* Used to determine whether the DLL can be unloaded by COM                 */

STDAPI DllCanUnloadNow(void)
{
	return (g_CModul.m_nLockCnt == 0) ? S_OK : S_FALSE;
}

/****************************************************************************/
/* Returns a class factory to create an object of the requested type        */

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
  if (rclsid != CLSID_DbpInstall)
  {
    return CLASS_E_CLASSNOTAVAILABLE ;
  }
  
  CDbpInstallFactory* pFactory = new CDbpInstallFactory;
  if (pFactory == NULL)  
  {
    return E_OUTOFMEMORY ;
  }
  
  HRESULT hr = pFactory->QueryInterface(riid, ppv) ;
  pFactory->Release() ;
  return hr ;
}

/****************************************************************************/
/* DllRegisterServer - Adds entries to the system registry                  */

STDAPI DllRegisterServer(void)
{
  return co90RegisterComObject ((char*)PROGIDPFX, (char*)"DbpInstall", (char*)LiveCacheApplVersion, 
    CLSID_DbpInstall, g_CModul.m_szModule );
}

/****************************************************************************/
/* DllUnregisterServer - Removes entries from the system registry           */

STDAPI DllUnregisterServer(void)
{
  return co90UnregisterComObject ((char*)PROGIDPFX, (char*)"DbpInstall", (char*)LiveCacheApplVersion, 
    CLSID_DbpInstall, g_CModul.m_szModule );
}
