/*!
  @file           vos221.cpp
  @author         RaymondR
  @brief          MSCS - Implementation of the CluApp class and DLL initialization
  @see            

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

#include "gos220.h"
#include <CluAdmEx.h>
#include <initguid.h>
#include "gos200.h"
#include "gos221.h"
#include "gos222.h"
#include "gos225.h"
#include "gos226.h"

#define IID_DEFINED

#if _MSC_VER < 1300
# pragma warning(disable : 4701) // local variable may be used without having been initialized
//#  include <atlimpl.cpp>
# pragma warning(default : 4701)
#endif
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef _DEBUG
# define new DEBUG_NEW
# undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define PRINT_OUT(_s)  if (PrintOutEnabled)  wprintf _s;


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_CoSAPDBCluEx, CExtObject)
END_OBJECT_MAP()

class CCluApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};


/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

CComModule  _Module;
CCluApp     theApp;

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/


bool PrintOutEnabled = false;



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/


/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/////////////////////////////////////////////////////////////////////////////
//++
//
//	CCluApp::InitInstance
//
//	Routine Description:
//		Initialize this instance of the application.
//
//	Arguments:
//		None.
//
//	Return Value:
//		Any return codes from CWinApp::InitInstance().
//
//--
/////////////////////////////////////////////////////////////////////////////

BOOL CCluApp::InitInstance(void)
{
	_Module.Init(ObjectMap, m_hInstance);
	return CWinApp::InitInstance();

}  //*** CCluApp::InitInstance()

/////////////////////////////////////////////////////////////////////////////
//++
//
//	CCluApp::ExitInstance
//
//	Routine Description:
//		Deinitialize this instance of the application.
//
//	Arguments:
//		None.
//
//	Return Value:
//		Any return codes from CWinApp::ExitInstance().
//
//--
/////////////////////////////////////////////////////////////////////////////

int CCluApp::ExitInstance(void)
{
	_Module.Term();
	return CWinApp::ExitInstance();

}  //*** CCluApp::ExitInstance()

/////////////////////////////////////////////////////////////////////////////
//++
//
//	FormatError
//
//	Routine Description:
//		Format an error.
//
//	Arguments:
//		rstrError	[OUT] String in which to return the error message.
//		dwError		[IN] Error code to format.
//
//	Return Value:
//		None.
//
//--
/////////////////////////////////////////////////////////////////////////////

void FormatError(CString & rstrError, DWORD dwError)
{
	DWORD		dwResult;
	TCHAR		szError[256];

	dwResult = ::FormatMessage(
					FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					dwError,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
					szError,
					sizeof(szError) / sizeof(TCHAR),
					0
					);
	if (dwResult == 0)
	{
		// Format the NT status code from CLUSAPI since some of these haven't
		// been integrated into the system yet.
		dwResult = ::FormatMessage(
						FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
						::GetModuleHandle(_T("CLUSAPI.DLL")),
						dwError,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
						szError,
						sizeof(szError) / sizeof(TCHAR),
						0
						);
		if (dwResult == 0)
		{
			// Format the NT status code from NTDLL since this hasn't been
			// integrated into the system yet.
			dwResult = ::FormatMessage(
							FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
							::GetModuleHandle(_T("NTDLL.DLL")),
							dwError,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
							szError,
							sizeof(szError) / sizeof(TCHAR),
							0
							);
		}  // if:  error formatting status code from CLUSAPI
	}  // if:  error formatting status code from system

	if (dwResult != 0)
		rstrError = szError;
	else
	{
		dwResult = ::GetLastError();
		TRACE(_T("FormatError() - Error 0x%08.8x formatting string for error code 0x%08.8x\n"), dwResult, dwError);
		rstrError.Format(_T("Error 0x%08.8x"));
	}  // else:  error formatting the message

}  //*** FormatError()


/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (AfxDllCanUnloadNow() && _Module.GetLockCount()==0) ? S_OK : S_FALSE;

}  //*** DllCanUnloadNow()

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);

}  //*** DllGetClassObject()


/////////////////////////////////////////////////////////////////////////////
// DllEnablePrintOut 

STDAPI DllEnablePrintOut(void)
{
  PrintOutEnabled = true;

  return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	HRESULT hRes = S_OK;

	// registers object, typelib and all interfaces in typelib
	hRes = _Module.RegisterServer(FALSE /*bRegTypeLib*/);
	return hRes;

}  //*** DllRegisterServer()

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	HRESULT hRes = S_OK;
	_Module.UnregisterServer();
	return hRes;

}  //*** DllUnregisterServer()

/////////////////////////////////////////////////////////////////////////////
//++
//
//	DllRegisterCluAdminExtension
//
//	Routine Description:
//		Register the extension with the cluster database.
//
//	Arguments:
//		hCluster		[IN] Handle to the cluster to modify.
//
//	Return Value:
//		S_OK			Extension registered successfully.
//		Win32 error code if another failure occurred.
//
//--
/////////////////////////////////////////////////////////////////////////////

STDAPI DllRegisterCluAdminExtension(IN HCLUSTER hCluster)
{
	HRESULT		hr = S_OK;
	LPCWSTR		pwszResTypes = g_wszResourceTypeNames;

	while (*pwszResTypes != L'\0')
	{
		hr = RegisterCluAdminResourceTypeExtension(
					hCluster,
					pwszResTypes,
					&CLSID_CoSAPDBCluEx
					);
		if (hr == S_OK)
    {
     	PRINT_OUT((L"  %-60.60s - Ok\n", pwszResTypes));
    }
    else
    {
     	PRINT_OUT((L"  %-60.60s - Failed: %d\n", pwszResTypes, hr));
		  break;
    }
		pwszResTypes += lstrlenW(pwszResTypes) + 1;
	}  // while:  more resource types

	return hr;

}  //*** DllRegisterCluAdminExtension()

/////////////////////////////////////////////////////////////////////////////
//++
//
//	DllUnregisterCluAdminExtension
//
//	Routine Description:
//		Unregister the extension with the cluster database.
//
//	Arguments:
//		hCluster		[IN] Handle to the cluster to modify.
//
//	Return Value:
//		S_OK			Extension unregistered successfully.
//		Win32 error code if another failure occurred.
//
//--
/////////////////////////////////////////////////////////////////////////////

STDAPI DllUnregisterCluAdminExtension(IN HCLUSTER hCluster)
{
	HRESULT		hr = S_OK;
	LPCWSTR		pwszResTypes = g_wszResourceTypeNames;

	while (*pwszResTypes != L'\0')
	{
		hr = UnregisterCluAdminResourceTypeExtension(
					hCluster,
					pwszResTypes,
					&CLSID_CoSAPDBCluEx
					);
		if (hr == S_OK)
    {
     	PRINT_OUT((L"  %-60.60s - Ok\n", pwszResTypes));
    }
    else
    {
     	PRINT_OUT((L"  %-60.60s - Failed: %d\n", pwszResTypes, hr));
		  break;
    }
		pwszResTypes += lstrlenW(pwszResTypes) + 1;
	}  // while:  more resource types

	return hr;

}  //*** DllUnregisterCluAdminExtension()


/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
