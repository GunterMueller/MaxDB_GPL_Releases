/*!
  @file           vos204.cpp
  @author         RaymondR
  @brief          MSCS
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

#if !defined(UNICODE)
# error "You can compile this file with -DUNICODE only!"
#endif
#define  _UNICODE 

#define NEEDED_WIN32_WINNT 0x0500

#if !defined(_WIN32_WINNT)
# define _WIN32_WINNT  NEEDED_WIN32_WINNT
#else
# if _WIN32_WINNT < NEEDED_WIN32_WINNT
#  undef _WIN32_WINNT
#  define _WIN32_WINNT  NEEDED_WIN32_WINNT
# endif
#endif

#include "gos200.h"
#include <afxwin.h>			
#include <basetyps.h>
#include <malloc.h>
#include "geo200.h"
#include "gos00.h"
#include "gos204.h"
#include "geo007_1.h"
#include "geo60.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	inline
#endif

#define DEFAULT_NAME_BUFFER_SIZE     128
#define DEBUGEX_DLL_NAME             _T("DebugEx.Dll")

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *

 *===========================================================================*/



/*---------------------------------------------------------------------------*/
/*--------------------------- class tos203_DynDLL ---------------------------*/
/*---------------------------------------------------------------------------*/

class ResourceExDll : public tos203_DynDLL 
{
public:
  ResourceExDll                          ( LPCTSTR              DllFullName, 
                                           const HMODULE        ModuleHdl   = NULL,
                                           teo200_EventList *   pEventList  = NULL );
  ~ResourceExDll() {;};

  MK_FUNCPTR_OS203(HRESULT, STDAPICALLTYPE, DllRegisterCluAdminExtension,   ( HCLUSTER ClusterHandle ));
  MK_FUNCPTR_OS203(HRESULT, STDAPICALLTYPE, DllRegisterServer,              ( void ));
  MK_FUNCPTR_OS203(HRESULT, STDAPICALLTYPE, DllUnregisterCluAdminExtension, ( HCLUSTER ClusterHandle ));
  MK_FUNCPTR_OS203(HRESULT, STDAPICALLTYPE, DllUnregisterServer,            ( void ));
};



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/*-------------------------- class tos204_Cluster ---------------------------*/
/*---------------------------------------------------------------------------*/

bool tos204_Cluster::os204_Open( teo200_EventList *   pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::os204_Open"));

  return os204_Open( NULL, pEventList );
}

/*---------------------------------------------------------------------------*/

bool tos204_Cluster::os204_Open( LPCWSTR              ClusterName,
                                 teo200_EventList *   pEventList )

{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::os204_Open"));
  bool Ok = true;

  CLEAR_EVENTLIST_EO200(pEventList);

  // --- close old cluster handle
  Ok = os204_Close( pEventList );
  
  if ( Ok == true )
  {
    os204_ClusterHandle = OpenCluster ( ClusterName );

    if ( os204_ClusterHandle == NULL )
    {
      CString       ClusterNameStr;

      if ( ClusterName == NULL )
      {
        ClusterNameStr    = _T(" ");
        os204_ClusterName = "<local>";
      }
      else
        ClusterNameStr =  _T(" '") + (CString)ClusterName + _T("' ");

      if (( GetLastError() == RPC_S_SERVER_UNAVAILABLE ) || ( GetLastError() == RPC_S_UNKNOWN_IF ))
      {
        teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, ERR_CLUSTER_UNAVAILABLE, 
                                 (LPCTSTR)ClusterNameStr );
        COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      }
      else
      {
        teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, ERR_OPEN_CLUSTER_FAILED,
                                 (LPCTSTR)ClusterNameStr );
        COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      }

      Ok = false;
    }
    else
      os204_ClusterName = ClusterName;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos204_Cluster::os204_Close( teo200_EventList *             pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::os204_Close"));
  bool Ok;

  Ok = os204_CloseEnum( pEventList );

  if ( os204_ClusterHandle != NULL )
    CloseCluster(os204_ClusterHandle);

  os204_ClusterName   = "";
  os204_ClusterHandle = NULL;
  
  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos204_Cluster::os204_EnableClusterLog( const DWORD          MBLogSize,
                                             LPCTSTR              LogName,
                                             teo200_EventList *   pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::os204_EnableClusterLog"));

  DWORD        rc;
  HKEY         hCreateKey;

  rc = RegOpenKeyEx( HKEY_LOCAL_MACHINE, REG_SK_NT_CLUSTER_SERVER_HIVE, 0, KEY_ALL_ACCESS, &hCreateKey );

  if ( rc == ERROR_SUCCESS)
  {
    rc = RegSetValueEx( hCreateKey, REG_VN_CLUSTER_SERVER_LOGSIZE, 0, REG_DWORD, 
                        (CONST BYTE*)&MBLogSize, sizeof(MBLogSize));

    RegCloseKey ( hCreateKey );
  }

  if ( rc != ERROR_SUCCESS)
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_SETTING_CLUSTER_LOG_SIZE );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( LogName != NULL )
  {
    rc = RegOpenKeyEx( HKEY_LOCAL_MACHINE, REG_SK_NT_SYSTEM_ENVIRONMENT, 0, 
                       KEY_ALL_ACCESS, &hCreateKey );
    
    if ( rc == ERROR_SUCCESS)
    {
      rc = RegSetValueEx( hCreateKey, CLUSTERLOG_ENV_VAR, 0,
                          REG_SZ, (CONST BYTE*)LogName, (DWORD)_tcslen(LogName) * sizeof(TCHAR));
      
      RegCloseKey ( hCreateKey );
    }

    if ( rc != ERROR_SUCCESS)
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_SETTING_LOG_ENV_VAR );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_Cluster::os204_DisableClusterLog( teo200_EventList *             pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::os204_DisableClusterLog"));

  DWORD        rc;
  HKEY         hCreateKey;
  DWORD        MBLogSize = 0; 

  rc = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("Cluster"), 0, KEY_ALL_ACCESS, &hCreateKey );

  if ( rc == ERROR_SUCCESS)
  {
    rc = RegSetValueEx( hCreateKey, _T("ClusterLogSize"), 0, REG_DWORD, (CONST BYTE*)&MBLogSize, sizeof(MBLogSize));

    RegCloseKey ( hCreateKey );
  }

  if ( rc != ERROR_SUCCESS)
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_SETTING_CLUSTER_LOG_SIZE );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_Cluster::os204_RegisterDebugEx( teo200_EventList *             pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::os204_RegisterDebugEx"));

  DWORD        rc;

  if ( os204_ClusterHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_CLUSTER_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  // --- load the dll and all its functions
  ResourceExDll DebugExDll( DEBUGEX_DLL_NAME );

  rc = DebugExDll.DllRegisterCluAdminExtension(os204_ClusterHandle);
  
  if ( rc != NO_ERROR ) 
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_REGISTER_CLU_ADMIN_EXT,
                             DEBUGEX_DLL_NAME );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }
  
  rc = DebugExDll.DllRegisterServer ();
  
  if ( rc != NO_ERROR ) 
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_REGISTER_SERVER,
                             DEBUGEX_DLL_NAME );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_Cluster::os204_UnregisterDebugEx( teo200_EventList *             pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::os204_UnregisterDebugEx"));

  DWORD        rc;

  if ( os204_ClusterHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_CLUSTER_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  // --- load the dll and all its functions
  ResourceExDll DebugExDll( DEBUGEX_DLL_NAME );
    
  rc = DebugExDll.DllUnregisterCluAdminExtension(os204_ClusterHandle);
    
  if (( rc != NO_ERROR ) && ( rc != ERROR_FILE_NOT_FOUND ))
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_UNREGISTER_CLU_ADMIN_EXT,
                             DEBUGEX_DLL_NAME );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }
    
  rc = DebugExDll.DllUnregisterServer();
    
  if ( rc != NO_ERROR ) 
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_UNREGISTER_SERVER,
                             DEBUGEX_DLL_NAME );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_Cluster::os204_OpenEnum( DWORD                dwType, 
                                     teo200_EventList *   pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::os204_OpenEnum"));
  bool Ok = true;

  if ( os204_ClusterHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_CLUSTER_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }
               
  if ( os204_ClusterEnumHandle != NULL )
    Ok = os204_CloseEnum( pEventList );

  if ( Ok == true )
  {
    os204_ClusterEnumHandle = ClusterOpenEnum( os204_ClusterHandle, dwType );

    if ( os204_ClusterEnumHandle == NULL )
    {
      teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, ERR_OPEN_CLUSTER_ENUMERATOR );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos204_Cluster::os204_EnumReset( teo200_EventList *   pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::os204_EnumReset"));

  if ( os204_ClusterHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_CLUSTER_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  os204_EnumIdx  = 0;

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_Cluster::os204_Enum( CString&             Value,
                                 bool&                ItemFound,
                                 teo200_EventList *   pEventList)
{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::os204_Enum"));
  DWORD        rc;
  DWORD        Type;
  LPTSTR       Name;
  DWORD        NameLen;

  ItemFound = false;

  if ( os204_ClusterHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_CLUSTER_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  NameLen = DEFAULT_NAME_BUFFER_SIZE;

  do
  {
    NameLen += sizeof(WCHAR);
    Name     = (LPTSTR)_alloca(NameLen);
    
    rc = ClusterEnum(  os204_ClusterEnumHandle,
                       os204_EnumIdx,
                       &Type, Name, &NameLen );
  }
  while ( rc == ERROR_MORE_DATA );


  if ( rc != NO_ERROR )
  {
    Value = "";

    if  ( rc != ERROR_NO_MORE_ITEMS )
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CLUSTER_ENUM_FAILED );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }
  }
  else
  {
    Value     = Name;
    ItemFound = true;
    os204_EnumIdx++;
  }

  return true;
}


/*---------------------------------------------------------------------------*/

bool tos204_Cluster::os204_CloseEnum( teo200_EventList *   pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::CloseEnum"));
  DWORD        rc;

  if ( os204_ClusterEnumHandle == NULL )
    return true;

  if ( os204_ClusterHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_CLUSTER_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  rc = ClusterCloseEnum( os204_ClusterEnumHandle );

  if ( rc != NO_ERROR )
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_CLOSE_CLUSTER_ENUMERATOR );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }
 
  os204_EnumIdx           = 0;
  os204_ClusterEnumHandle = NULL;

  return true;
}


/*---------------------------------------------------------------------------*/
/*----------------------- class tos204_ClusterNetwork -----------------------*/
/*---------------------------------------------------------------------------*/

bool tos204_ClusterNetwork::os204_Open( const tos204_Cluster&    Cluster,
                                        const LPCWSTR            NetworkName,
                                        teo200_EventList *       pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterNetwork::os204_Open"));

  bool  Ok;

  ASSERT(NetworkName != NULL);
  ASSERT((HCLUSTER)Cluster != NULL);

  CLEAR_EVENTLIST_EO200(pEventList);

  os204_NetworkHandle = OpenClusterNetwork( Cluster, NetworkName );

  if ( os204_NetworkHandle == NULL )
  {
    if ( GetLastError() ==  ERROR_NETWORK_NOT_FOUND )
    {
      teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                               ERR_NETWORK_NOT_FOUND, (LPCTSTR)NetworkName );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    }
    else
    {
      teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                               ERR_OPEN_CLUSTER_NETWORK, (LPCTSTR)NetworkName );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    }
    
    Ok = false;
  }
  else
  {
    Ok                = true;
    os204_NetworkName = NetworkName;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterNetwork::os204_Close( teo200_EventList *             pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterNetwork::os204_Close"));

  if ( os204_NetworkHandle != NULL )
    CloseClusterNetwork(os204_NetworkHandle);

  os204_NetworkName   = "";
  os204_NetworkHandle = NULL;

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterNetwork::os204_GetClusterNetworkId( CString&             NetworkIdStr,
                                                       teo200_EventList *   pEventList)
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterNetwork::os204_GetClusterNetworkId"));

  DWORD        rc;
  LPTSTR       NetworkId;
  DWORD        NetworkIdLen;

  if ( os204_NetworkHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_NETWORK_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  NetworkIdLen = DEFAULT_NAME_BUFFER_SIZE;

  do
  {
    NetworkIdLen += sizeof(WCHAR);
    NetworkId     = (LPTSTR)_alloca(NetworkIdLen);
    
    rc = GetClusterNetworkId( os204_NetworkHandle, NetworkId, &NetworkIdLen );
  }
  while ( rc == ERROR_MORE_DATA );

  if ( rc != NO_ERROR )
  {
    NetworkIdStr = "";

    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_GET_CLUSTER_NETWORK_ID, (LPCTSTR)os204_NetworkName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  NetworkIdStr = NetworkId;

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterNetwork::os204_SetProperties( const tos205_PropertyList&  PropertyList,
                                                 const DWORD                 ControlCode,
                                                 const HNODE                 HostNode,
                                                 teo200_EventList *          pEventList)
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterNetwork::os204_SetProperties"));
  DWORD        rc         = NO_ERROR;

  ASSERT((ControlCode & (CLUSCTL_OBJECT_MASK << CLUSCTL_OBJECT_SHIFT))
      == (CLUS_OBJECT_NETWORK << CLUSCTL_OBJECT_SHIFT));
  ASSERT(PropertyList.os205_PropertyListPtr()->pb != NULL);

  if ( os204_NetworkHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_NETWORK_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  // --- Set properties.
  rc = ClusterNetworkControl( os204_NetworkHandle,
                              HostNode,
                              ControlCode,
                              PropertyList.os205_PropertyListPtr()->pb,
                              PropertyList.os205_PropertyListSize(),
                              NULL,
                              0,
                              NULL );

  if ((rc != ERROR_SUCCESS) && ( rc != ERROR_RESOURCE_PROPERTIES_STORED ))
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_SETTING_NETWORK_PROP, (LPCTSTR)os204_NetworkName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterNetwork::os204_GetProperties( tos205_PropertyList&        PropertyList,
                                                 const DWORD                 ControlCode,
                                                 const HNODE                 HostNode,
                                                 teo200_EventList *          pEventList)
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterNetwork::os204_GetProperties"));

  DWORD                   rc         = NO_ERROR;
  DWORD                   Props      = 512;
  CLUSPROP_BUFFER_HELPER  NewProplist;

  ASSERT((ControlCode & (CLUSCTL_OBJECT_MASK << CLUSCTL_OBJECT_SHIFT))
      == (CLUS_OBJECT_NETWORK << CLUSCTL_OBJECT_SHIFT));

  if ( os204_NetworkHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_NETWORK_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  do
  {
    // --- Allocate and zero a new buffer.
    NewProplist.pb = new BYTE[Props];

    if ( NewProplist.pb == NULL )
    {
      teo200_EventList EvtLst( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, 
                               ERR_OUT_OF_MEMORY );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }

    // --- Get properties.
    rc = ClusterNetworkControl( os204_NetworkHandle,
                                HostNode,
                                ControlCode,
                                NULL,
                                0,
                                NewProplist.pb,
                                Props,
                                &Props );

    if ( rc != ERROR_SUCCESS )
      delete [] NewProplist.pb;
  } 
  while ( rc == ERROR_MORE_DATA );

  if ( rc != ERROR_SUCCESS )
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_GETTING_NETWORK_PROP, (LPCTSTR)os204_NetworkName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  PropertyList.os205_FillPropertyList( NewProplist, Props, pEventList );
  
  delete [] NewProplist.pb;

  return true;
}

/*---------------------------------------------------------------------------*/
/*------------------------ class tos204_ClusterGroup ------------------------*/
/*---------------------------------------------------------------------------*/

bool tos204_ClusterGroup::os204_Open( const tos204_Cluster&    Cluster,                                      
                                      const LPCWSTR            GroupName,
                                      teo200_EventList *       pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::os204_Open"));

  bool Ok = true;

  ASSERT(GroupName != NULL);
  ASSERT((HCLUSTER)Cluster != NULL);

  CLEAR_EVENTLIST_EO200(pEventList);

  Ok = os204_Close( pEventList );

  if ( Ok == true )
  {
    os204_GroupHandle  = OpenClusterGroup( Cluster, GroupName );
  
    if ( os204_GroupHandle == NULL ) 
    {
      if ( GetLastError() == ERROR_GROUP_NOT_FOUND )
      {
        teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                                 ERR_GROUP_NOT_FOUND, (LPCTSTR)GroupName );
        COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      }
      else
      {
        teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                                 ERR_OPEN_GROUP_FAILED, (LPCTSTR)GroupName );
        COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      }
  
      Ok = false;
    }
    else
    {
      os204_GroupName = GroupName;
      os204_Created   = false;
    }
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterGroup::os204_Create( const tos204_Cluster&    Cluster,
                                        const LPCWSTR            GroupName,
                                        teo200_EventList *       pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::os204_Create"));
  bool Ok = true;

  ASSERT(GroupName != NULL);
  ASSERT((HCLUSTER)Cluster != NULL);

  CLEAR_EVENTLIST_EO200(pEventList);

  // --- close old cluster group
  Ok = os204_Close( pEventList );

  if ( Ok == true )
  {
    // --- try to open existing group
    os204_GroupHandle  = OpenClusterGroup( Cluster, GroupName );

    if (( os204_GroupHandle == NULL ) &&
        ( GetLastError()    == ERROR_GROUP_NOT_FOUND ))
    {
      // --- create group
      os204_GroupHandle = CreateClusterGroup( Cluster, GroupName );

      if ( os204_GroupHandle != NULL )
        os204_Created   = true;
    }

    if ( os204_GroupHandle == NULL )
    {
      teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                               ERR_CREATING_GROUP, (LPCTSTR)GroupName );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      Ok = false;
    }
    else
      os204_GroupName = GroupName;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterGroup::os204_Close( teo200_EventList *       pEventList )

{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::os204_Close"));
  bool Ok = true;

  Ok = os204_CloseEnum( pEventList );

  if ( os204_GroupHandle != NULL )
    CloseClusterGroup(os204_GroupHandle);

  os204_Created     = false;
  os204_GroupName   = "";
  os204_GroupHandle = NULL;

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterGroup::os204_Delete( teo200_EventList *       pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::os204_Delete"));

  DWORD        rc;
  bool         Ok = true;

  if ( os204_GroupHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_GROUP_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  rc = DeleteClusterGroup( os204_GroupHandle );

  if ( rc != NO_ERROR )
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_DELETING_GROUP, (LPCTSTR)os204_GroupName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  Ok = os204_Close( pEventList );

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterGroup::os204_BringOnline( DWORD                    Timeout,
                                             teo200_EventList *       pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::os204_BringOnline"));

  DWORD                   rc;
  CLUSTER_GROUP_STATE     State;

  if ( os204_GroupHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_GROUP_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( Timeout == 0 )
    Timeout  = 360;
  else
    Timeout /= 1000;
  
  rc = OnlineClusterGroup( os204_GroupHandle, NULL );
  
  if (( rc == ERROR_IO_PENDING )    || 
      ( rc == ERROR_INVALID_STATE ) ||
      ( rc == ERROR_SUCCESS ))
  {
    DWORD   Retry = 10;

    do
    {
      State = GetClusterGroupState( os204_GroupHandle, NULL, NULL );
      
      if ((( State != ClusterGroupOnline )       && 
           ( State != ClusterGroupFailed )       &&
           ( State != ClusterGroupStateUnknown )) || ( --Retry ))
      {
        SleepEx ( 1000, false );
      }
      else
        break;
      
    } while ( Timeout-- );
    
    if ( State == ClusterGroupOnline )
      rc = NO_ERROR;
    else
      rc = GetLastError();
  }

  if ( Timeout == 0 )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, 
                             ERR_GROUP_ONLINE_TIMEOUT, (LPCTSTR)os204_GroupName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if (( rc != NO_ERROR )                     ||
      ( State == ClusterGroupPartialOnline ))
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_BRINGING_GROUP_ONLINE, (LPCTSTR)os204_GroupName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}
 
/*---------------------------------------------------------------------------*/

bool tos204_ClusterGroup::os204_BringOffline( DWORD                    Timeout,
                                              teo200_EventList *       pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::os204_BringOffline"));

  DWORD                   rc;
  CLUSTER_GROUP_STATE     State;

  if ( os204_GroupHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_GROUP_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( Timeout == 0 )
    Timeout  = 360;
  else
    Timeout /= 1000;
  
  rc = OfflineClusterGroup( os204_GroupHandle );
  
  if (( rc == ERROR_IO_PENDING )      || 
      ( rc == ERROR_INVALID_STATE )   ||
      ( rc == ERROR_QUORUM_RESOURCE ) ||
      ( rc == ERROR_SUCCESS ))
  {
    DWORD   Retry = 10;

    do
    {
      State = GetClusterGroupState( os204_GroupHandle, NULL, NULL );
      
      if ((( State != ClusterGroupOffline )      && 
           ( State != ClusterGroupFailed )       &&
           ( State != ClusterGroupStateUnknown )) || ( --Retry )) 
      {
        SleepEx ( 1000, false );
      }
      else
        break;
      
    } while ( Timeout-- );
    
    if ( State == ClusterGroupStateUnknown )
      rc = GetLastError();
    else if (( State == ClusterGroupOffline ) || ( State == ClusterGroupFailed ))
      rc = NO_ERROR;
  }
  
  if ( Timeout == 0 )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, 
                             ERR_GROUP_OFFLINE_TIMEOUT, (LPCTSTR)os204_GroupName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if (( rc == ERROR_QUORUM_RESOURCE )        &&
      ( State == ClusterGroupPartialOnline ))
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, 
                             INFO_GRP_PART_OFFLINE_QUORUM, (LPCTSTR)os204_GroupName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }
  if (( rc != NO_ERROR )                     ||
      ( State == ClusterGroupPartialOnline ))
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_BRINGING_GROUP_OFFLINE, (LPCTSTR)os204_GroupName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterGroup::os204_OpenEnum( DWORD                          dwType,
                                          teo200_EventList *             pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::os204_OpenEnum"));
  bool Ok = true;

  if ( os204_GroupHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_GROUP_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( os204_GroupEnumHandle != NULL )
    Ok = os204_CloseEnum( pEventList );

  if ( Ok == true )
  {
    os204_GroupEnumHandle = ClusterGroupOpenEnum( os204_GroupHandle, dwType );

    if ( os204_GroupEnumHandle == NULL )
    {
      teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                               ERR_OPEN_GROUP_ENUMERATOR, (LPCTSTR)os204_GroupName );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterGroup::os204_EnumReset( teo200_EventList *             pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::os204_EnumReset"));

  if ( os204_GroupHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_GROUP_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  os204_EnumIdx  = 0;

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterGroup::os204_Enum( CString&                       Value,
                                      bool&                          ItemFound,
                                      teo200_EventList *             pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::os204_Enum"));
  DWORD        rc;
  DWORD        Type;
  LPTSTR       Name;
  DWORD        NameLen;

  ItemFound = false;

  if ( os204_GroupHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_GROUP_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  NameLen = DEFAULT_NAME_BUFFER_SIZE;

  do
  {
    NameLen += sizeof(WCHAR);
    Name     = (LPTSTR)_alloca(NameLen);
    
    rc = ClusterGroupEnum( os204_GroupEnumHandle,
                           os204_EnumIdx,
                           &Type, Name, &NameLen );
  }
  while ( rc == ERROR_MORE_DATA );

  if ( rc != NO_ERROR )
  {
    Value = "";

    if  ( rc != ERROR_NO_MORE_ITEMS )
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                               ERR_GROUP_ENUMERATORATION, (LPCTSTR)os204_GroupName );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }
  }
  else
  {
    Value     = Name;
    ItemFound = true;
    os204_EnumIdx++;
  }

  return true;
}


/*---------------------------------------------------------------------------*/

bool tos204_ClusterGroup::os204_CloseEnum( teo200_EventList *             pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::CloseEnum"));
  DWORD        rc;

  if ( os204_GroupEnumHandle == NULL )
    return true;

  if ( os204_GroupHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_GROUP_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  rc = ClusterGroupCloseEnum( os204_GroupEnumHandle );

  if ( rc != NO_ERROR )
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_CLOSE_GROUP_ENUMERATOR, (LPCTSTR)os204_GroupName );
   COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  os204_EnumIdx         = 0;
  os204_GroupEnumHandle = NULL;

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterGroup::os204_SetProperties( const tos205_PropertyList&  PropertyList,
                                               const DWORD                 ControlCode,
                                               const HNODE                 HostNode,
                                               teo200_EventList *          pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::os204_SetProperties"));
  DWORD        rc         = NO_ERROR;

  ASSERT((ControlCode & (CLUSCTL_OBJECT_MASK << CLUSCTL_OBJECT_SHIFT))
      == (CLUS_OBJECT_GROUP << CLUSCTL_OBJECT_SHIFT));
  ASSERT(PropertyList.os205_PropertyListPtr()->pb != NULL);

  if ( os204_GroupHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_GROUP_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  // --- Set properties.
  rc = ClusterGroupControl( os204_GroupHandle,
                            HostNode,
                            ControlCode,
                            PropertyList.os205_PropertyListPtr()->pb,
                            PropertyList.os205_PropertyListSize(),
                            NULL,
                            0,
                            NULL );

  if ((rc != ERROR_SUCCESS) && ( rc != ERROR_RESOURCE_PROPERTIES_STORED ))
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_SETTING_GROUP_PROP, (LPCTSTR)os204_GroupName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterGroup::os204_GetProperties( tos205_PropertyList&        PropertyList,
                                               const DWORD                 ControlCode,
                                               const HNODE                 HostNode,
                                               teo200_EventList *          pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::os204_GetProperties"));

  DWORD                   rc         = NO_ERROR;
  DWORD                   Props      = 512;
  CLUSPROP_BUFFER_HELPER  NewProplist;

  ASSERT((ControlCode & (CLUSCTL_OBJECT_MASK << CLUSCTL_OBJECT_SHIFT))
      == (CLUS_OBJECT_GROUP << CLUSCTL_OBJECT_SHIFT));

  if ( os204_GroupHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_GROUP_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  do
  {
    // --- Allocate and zero a new buffer.
    NewProplist.pb = new BYTE[Props];

    if ( NewProplist.pb == NULL )
    {
      teo200_EventList EvtLst( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, 
                               ERR_OUT_OF_MEMORY );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }

    // --- Get properties.
    rc = ClusterGroupControl( os204_GroupHandle,
                              HostNode,
                              ControlCode,
                              NULL,
                              0,
                              NewProplist.pb,
                              Props,
                              &Props );

    if (rc != ERROR_SUCCESS)
      delete [] NewProplist.pb;
  } 
  while (rc == ERROR_MORE_DATA);

  if (rc != ERROR_SUCCESS)
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_GETTING_GROUP_PROP, (LPCTSTR)os204_GroupName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  PropertyList.os205_FillPropertyList( NewProplist, Props, pEventList );
  
  delete [] NewProplist.pb;

  return true;
}
 
/*---------------------------------------------------------------------------*/
/*---------------------- class tos204_ClusterResource -----------------------*/
/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_Open( const tos204_Cluster&       Cluster,
                                         const LPCWSTR               ResourceName,
                                         teo200_EventList *          pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_Open"));

  DWORD Flags = 0;
  bool  Ok    = true;

  ASSERT((HCLUSTER)Cluster != NULL);
  ASSERT(ResourceName != NULL);

  CLEAR_EVENTLIST_EO200(pEventList);

  // --- close old resource
  Ok = os204_Close( pEventList );

  if ( Ok == true )
  {
    os204_ResourceHandle = OpenClusterResource( Cluster, ResourceName );

    if ( os204_ResourceHandle == NULL ) 
    {
      if ( GetLastError() == ERROR_RESOURCE_NOT_FOUND )
      {
        teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                                 ERR_RESOURCE_NOT_FOUND, (LPCTSTR)ResourceName );
        COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      }
      else
      {
        teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                                 ERR_OPEN_RESOURCE_FAILED, (LPCTSTR)ResourceName );
        COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      }
      Ok = false;
    }
    else
      os204_ResourceName = ResourceName;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_Create( const tos204_Cluster&       Cluster,
                                           const LPCWSTR               ResourceName,
                                           const tos204_ClusterGroup*  Group,
                                           const LPCWSTR               ResourceType,
                                           teo200_EventList *          pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_Create"));

  return os204_Create( Cluster, ResourceName, Group, ResourceType, false, pEventList );

}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_Create( const tos204_Cluster&       Cluster,
                                           const LPCWSTR               ResourceName,
                                           const tos204_ClusterGroup*  Group,
                                           const LPCWSTR               ResourceType,
                                           const bool                  SeparateMonitor,
                                           teo200_EventList *          pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_Create"));

  DWORD Flags = 0;
  bool  Ok    = true;

  ASSERT((HCLUSTER)Cluster != NULL);
  ASSERT(ResourceName != NULL);
  ASSERT(Group != NULL);
  ASSERT(ResourceType != NULL);

  CLEAR_EVENTLIST_EO200(pEventList);

  // --- close old resource
  Ok = os204_Close( pEventList );

  if ( Ok == true )
  {
    if ( SeparateMonitor == false )
    {
      // --- try to open existing resource
      os204_ResourceHandle = OpenClusterResource( Cluster, ResourceName );
    }

    if ( os204_ResourceHandle != NULL )  // --- resource opened?
    {
      bool TypesEqual;

      Ok = os204_ResourceTypeEqual( ResourceType, TypesEqual, pEventList );

      if ( Ok == true )
      {
        if ( TypesEqual == false ) 
        {
          // --- Resource types not equal. Closing the cluster resource will force an error
          //     during the following 'CreateClusterResource' call.
          Ok = os204_Close( pEventList );
        }
        else
          Ok = os204_ChangeGroup( *Group, pEventList );
      }

      if ( Ok == false )
        return false;
    }

    if (( os204_ResourceHandle == NULL ) && ( GetLastError() == ERROR_RESOURCE_NOT_FOUND ))
    {
      if (SeparateMonitor)
        Flags = CLUSTER_RESOURCE_SEPARATE_MONITOR;
      
      os204_ResourceHandle = CreateClusterResource( *Group, ResourceName, ResourceType, Flags );

      if ( os204_ResourceHandle != NULL )
        os204_Created   = true;
      else if ( GetLastError() == ERROR_FILE_NOT_FOUND )
        SetLastError( ERROR_RESOURCE_TYPE_NOT_FOUND );
    }
   

    if ( os204_ResourceHandle == NULL )
    {
      if ( GetLastError() == ERROR_RESOURCE_TYPE_NOT_FOUND )
      {
        teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                                 ERR_UNKOWN_RESOURCE_TYPE, (LPCTSTR)ResourceType );
        COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      }
      else
      {
        teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                                 ERR_CREATE_RESOURCE_FAILED, (LPCTSTR)ResourceName );
        COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      }
      
      Ok = false;
    } 
    else
      os204_ResourceName = ResourceName;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_Close( teo200_EventList *          pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_Close"));

  if ( os204_ResourceHandle != NULL )
    CloseClusterResource(os204_ResourceHandle);

  os204_ResourceHandle = NULL;
  os204_ResourceName   = "";
  os204_Created        = false; 

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_Delete( DWORD                          OffLineTimeOut,
                                           teo200_EventList *             pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_Delete"));

  DWORD  rc;
  bool   Ok = true;

  if ( os204_ResourceHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_RESOURCE_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( OffLineTimeOut > 0 )
    os204_BringOffline( OffLineTimeOut );

  rc = DeleteClusterResource( os204_ResourceHandle );
  
  if ( rc == ERROR_RESOURCE_ONLINE )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, 
                             ERR_DELETING_ONLINE_RESOURCE, (LPCTSTR)os204_ResourceName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( rc == ERROR_DEPENDENT_RESOURCE_EXISTS )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, 
                             ERR_DELETING_DEPEND_RESOURCE, (LPCTSTR)os204_ResourceName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( rc != NO_ERROR )
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_DELETING_RESOURCE, (LPCTSTR)os204_ResourceName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }
  
  Ok = os204_Close( pEventList );

  return Ok;
}
 
/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_AddResourceDependency( const tos204_ClusterResource&  DependsOnResource,
                                                          teo200_EventList *             pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_AddResourceDependency"));

  DWORD rc;

  ASSERT((HRESOURCE)DependsOnResource != NULL);

  if ( os204_ResourceHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_RESOURCE_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  rc = AddClusterResourceDependency( os204_ResourceHandle, DependsOnResource );
    
  if (( rc != NO_ERROR ) && ( rc != ERROR_DEPENDENCY_ALREADY_EXISTS ))
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_ADD_RESOURCE_DEPENDENCY, (LPCTSTR)os204_ResourceName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}
 
/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_ChangeGroup( const tos204_ClusterGroup&     Group,
                                                teo200_EventList *             pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_ChangeGroup"));

  DWORD rc;

  ASSERT((HGROUP)Group != NULL);

  if ( os204_ResourceHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_RESOURCE_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  rc = ChangeClusterResourceGroup( os204_ResourceHandle, Group );
  
  if (( rc != NO_ERROR ) && ( rc !=  ERROR_ALREADY_EXISTS ))
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_CHANGE_RESOURCE_TO_GROUP, (LPCTSTR)os204_ResourceName, (LPCTSTR)Group.os204_GetGroupName() );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}
 
/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_ResourceTypeEqual( const LPCWSTR         ResourceTypeName,
                                                      bool &                TypesEqual,    
                                                      teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_ResourceTypeEqual"));

  TypesEqual = false;

  if ( os204_ResourceHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_RESOURCE_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( ResUtilResourceTypesEqual( ResourceTypeName, os204_ResourceHandle  ))
    TypesEqual = true;

  return true;
}
 
/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_BringOnline( DWORD                 Timeout,
                                                teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_BringOnline"));

  DWORD   rc;

  if ( os204_ResourceHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_RESOURCE_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( Timeout == 0 )
  {
    teo200_EventList TmpEventList;  // - use temp. event list to ignore any errors

    os204_GetPendingTimout ( Timeout, &TmpEventList );
    
    if ( Timeout == 0 )
      Timeout  = 180;
    else
      Timeout /= 1000;
  }
  
  rc = OnlineClusterResource( os204_ResourceHandle );
  
  if (( rc == ERROR_IO_PENDING )    || 
      ( rc == ERROR_INVALID_STATE ) ||
      ( rc == ERROR_SUCCESS ))
  {
    CLUSTER_RESOURCE_STATE  State;
    DWORD                   Retry = 10;
    
    do
    {
      State = GetClusterResourceState( os204_ResourceHandle, NULL, NULL, NULL, NULL );
      
      if ((( State != ClusterResourceOnline ) && 
           ( State != ClusterResourceFailed ) &&
           ( State != ClusterResourceStateUnknown )) || ( --Retry ))
      {
        SleepEx ( 1000, false );
      }
      else
        break;

    } while ( Timeout-- );
    
    if ( State == ClusterResourceStateUnknown )
      rc = GetLastError();
    else if ( State == ClusterResourceOnline )
      rc = NO_ERROR;
  }

  if ( rc != NO_ERROR )
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_BRINGING_RESOURCE_ONLINE, (LPCTSTR)os204_ResourceName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}
 
/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_BringOffline( DWORD                 Timeout,
                                                 teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_BringOffline"));

  DWORD   rc;

  if ( os204_ResourceHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_RESOURCE_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( Timeout == 0 )
  {
    teo200_EventList TmpEventList;  // - use temp. event list to ignore any errors

    os204_GetPendingTimout ( Timeout, &TmpEventList );
    
    if ( Timeout == 0 )
      Timeout  = 180;
    else
      Timeout /= 1000;
  }
  
  rc = OfflineClusterResource( os204_ResourceHandle );
  
  if (( rc == ERROR_IO_PENDING )    || 
      ( rc == ERROR_INVALID_STATE ) ||
      ( rc == ERROR_SUCCESS ))
  {
    CLUSTER_RESOURCE_STATE  State;
    DWORD                   Retry = 10;
    
    do
    {
      State = GetClusterResourceState( os204_ResourceHandle, NULL, NULL, NULL, NULL );
      
      if ((( State != ClusterResourceOffline ) && 
           ( State != ClusterResourceFailed )  &&
           ( State != ClusterResourceStateUnknown )) || ( --Retry ))
      {
        SleepEx ( 1000, false );
      }
      else
        break;
      
    } while ( Timeout-- );
    
    if ( State == ClusterResourceStateUnknown )
      rc = GetLastError();
    else if (( State == ClusterResourceOffline ) || ( State == ClusterResourceFailed ))
      rc = NO_ERROR;
  }
  
  if ( rc != NO_ERROR )
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_BRINGING_RESOURCE_OFFLINE, (LPCTSTR)os204_ResourceName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_GetPendingTimout ( DWORD&                Timeout,
                                                      teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_GetParam"));
  #undef  HANDLE_CLEAN_UP
  #define HANDLE_CLEAN_UP()                                                        \
            if (ResourceKey)          ClusterRegCloseKey(ResourceKey);             \

  HKEY    ResourceKey         = NULL;
  LONG    rc                  = NO_ERROR;
  DWORD   ValueLen            = sizeof(Timeout);
  DWORD   ValueType;

  if ( os204_ResourceHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_RESOURCE_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  ResourceKey = GetClusterResourceKey( os204_ResourceHandle, KEY_READ );

  if (ResourceKey == NULL) 
  {
    HANDLE_CLEAN_UP();

    teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                             ERR_GET_RESOURCE_KEY, (LPCTSTR)os204_ResourceName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  rc = ClusterRegQueryValue(ResourceKey,
                            RES_STRT_STOP_PEND_TIMEOUT_OS00,
                            &ValueType,
                            (BYTE *)&Timeout,
                            &ValueLen );

  if (( rc != ERROR_SUCCESS )   && 
      ( rc != ERROR_MORE_DATA ) && 
      ( rc != ERROR_FILE_NOT_FOUND ))
  {
    HANDLE_CLEAN_UP();

    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_QUERY_VALUE, (LPCTSTR)_T("PendingTimeout") );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( rc == ERROR_MORE_DATA )
  {
    HANDLE_CLEAN_UP();

    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_INVALID_DATA_TYPE, (LPCTSTR)_T("PendingTimeout") );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  HANDLE_CLEAN_UP();

  return true;
}
 
/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_SetParam ( const LPWSTR          ValueName,
                                              const LPCTSTR         ValueData,
                                              const DWORD           ValueType,
                                              const LPWSTR          SubKeyName,
                                              teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_SetParam"));

  return os204_SetParam ( ValueName, (const void*)ValueData, ValueType, SubKeyName, pEventList );

}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_SetParam ( const LPWSTR          ValueName,
                                              const DWORD           ValueData,
                                              const LPWSTR          SubKeyName,
                                              teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_SetParam"));

  return os204_SetParam ( ValueName, (const void*)&ValueData, REG_DWORD, SubKeyName, pEventList );
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_SetParam ( const LPWSTR          ValueName,
                                              const void*           ValueData,
                                              const DWORD           ValueType,
                                              const LPWSTR          SubKeyName,
                                              teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_SetParam"));

  #undef  HANDLE_CLEAN_UP
  #define HANDLE_CLEAN_UP()                                                        \
            {                                                                      \
            if (RegResourceParamKey)  ClusterRegCloseKey(RegResourceParamKey);     \
            if (RegResourceKey)       ClusterRegCloseKey(RegResourceKey);          \
            if (ResourceKey)          ClusterRegCloseKey(ResourceKey);             \
            }

  HKEY    ResourceKey         = NULL;
  HKEY    RegResourceKey      = NULL;
  HKEY    RegResourceParamKey = NULL;
  LONG    rc                  = NO_ERROR;
  HKEY    pParamKey;      
  DWORD   Disposition;
  DWORD   ValueSize;

  ASSERT(ValueName != NULL);
  ASSERT(ValueData != NULL);

  if ( os204_ResourceHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_RESOURCE_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  ResourceKey = GetClusterResourceKey( os204_ResourceHandle, KEY_READ | KEY_WRITE );

  if (ResourceKey == NULL) 
  {
    HANDLE_CLEAN_UP();

    teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                             ERR_GET_RESOURCE_KEY, (LPCTSTR)os204_ResourceName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  rc = ClusterRegOpenKey(ResourceKey, _T("Parameters"), KEY_WRITE, &RegResourceKey);

  if ( rc != ERROR_SUCCESS ) 
  {
    HANDLE_CLEAN_UP();

    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_OPEN_RESOURCE_REG_KEY, (LPCTSTR)os204_ResourceName );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( SubKeyName != NULL )
  {
    rc = ClusterRegCreateKey(RegResourceKey,
                             SubKeyName,
                             0,
                             KEY_WRITE,
                             NULL,
                             &RegResourceParamKey,
                             &Disposition);

    if ( rc != ERROR_SUCCESS ) 
    {
      HANDLE_CLEAN_UP();

      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                               ERR_CREATE_RESOURCE_REG_KEY, 
                               (LPCTSTR)os204_ResourceName, SubKeyName );

      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }
    pParamKey = RegResourceParamKey;
  }
  else
    pParamKey = RegResourceKey;
  

  switch (ValueType)
  {
  case REG_DWORD:
    ValueSize = sizeof(ValueData);
    break;
    
  case REG_SZ:
    ValueSize = ((DWORD)_tcslen((const unsigned short*)ValueData) + 1) * sizeof(TCHAR);
      break;
    
  default:
    ASSERT(false);
  }

  rc = ClusterRegSetValue(pParamKey,
                          ValueName,
                          ValueType,
                          (CONST BYTE *)ValueData,
                          ValueSize );

  if (rc != ERROR_SUCCESS) 
  {
    HANDLE_CLEAN_UP();

    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_SET_RESOURCE_REG_KEY, 
                             (LPCTSTR)os204_ResourceName, ValueName );

    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  HANDLE_CLEAN_UP();

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_GetParam ( const LPWSTR          ValueName,
                                              CString&              Value,
                                              const LPWSTR          SubKeyName,
                                              teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_GetParam"));
  LPTSTR     pValue;
  bool       Ok;

  Ok = os204_GetParameter ( ValueName, SubKeyName, REG_SZ, (void**)&pValue, pEventList );
  Value  = pValue;

  delete pValue;

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_GetParam ( const LPWSTR          ValueName,
                                              DWORD&                Value,
                                              const LPWSTR          SubKeyName,
                                              teo200_EventList *    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_GetParam"));
  DWORD      *pValue;
  bool       Ok;

  Ok = os204_GetParameter ( ValueName, SubKeyName, REG_DWORD, (void**)&pValue, pEventList );
  Value  = *pValue;

  delete pValue;

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_GetParameter ( const LPWSTR         ValueName,
                                                  const LPWSTR         SubKeyName,
                                                  DWORD                ValueTypeExpected,
                                                  void **              ValueData,
                                                  teo200_EventList *   pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_GetParameter"));

  #undef  HANDLE_CLEAN_UP
  #define HANDLE_CLEAN_UP()                                                        \
            {                                                                      \
            if (RegResourceParamKey)  ClusterRegCloseKey(RegResourceParamKey);     \
            if (RegResourceKey)       ClusterRegCloseKey(RegResourceKey);          \
            if (ResourceKey)          ClusterRegCloseKey(ResourceKey);             \
            }

  HKEY    ResourceKey         = NULL;
  HKEY    RegResourceKey      = NULL;
  HKEY    RegResourceParamKey = NULL;
  LONG    rc                  = NO_ERROR;
  HKEY    pParamKey;      
  DWORD   ValueDataLen        = sizeof(DWORD);
  DWORD   ValueType;

  ASSERT(ValueName != NULL);

  if ( os204_ResourceHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_RESOURCE_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  *ValueData  = NULL;

  ResourceKey = GetClusterResourceKey( os204_ResourceHandle, KEY_READ );

  if (ResourceKey == NULL) 
  {
    HANDLE_CLEAN_UP();

    teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                             ERR_GET_RESOURCE_KEY, 
                             (LPCTSTR)os204_ResourceName );

    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  rc = ClusterRegOpenKey(ResourceKey, _T("Parameters"), KEY_READ, &RegResourceKey);

  if ( rc != ERROR_SUCCESS ) 
  {
    HANDLE_CLEAN_UP();

    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_OPEN_RESOURCE_REG_KEY, 
                             (LPCTSTR)os204_ResourceName, _T("Parameters") );

    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( SubKeyName != NULL )
  {
    rc = ClusterRegOpenKey(RegResourceKey, SubKeyName, KEY_READ, &RegResourceParamKey);

    if ( rc != ERROR_SUCCESS ) 
    {
      HANDLE_CLEAN_UP();

      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                               ERR_OPEN_RESOURCE_REG_KEY, 
                              (LPCTSTR)os204_ResourceName, SubKeyName );
      
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }
    pParamKey = RegResourceParamKey;
  }
  else
    pParamKey = RegResourceKey;

  do
  {
    *ValueData = new char[ValueDataLen];

    rc = ClusterRegQueryValue(pParamKey,
                              ValueName,
                              &ValueType,
                              (BYTE *)*ValueData,
                              &ValueDataLen );

    if ( rc == ERROR_MORE_DATA )
    {
      delete *ValueData;
      *ValueData = NULL;
    }
  } while ( rc == ERROR_MORE_DATA );


  if (( rc != ERROR_SUCCESS )   && 
      ( rc != ERROR_FILE_NOT_FOUND ))
  {
    HANDLE_CLEAN_UP();
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_QUERY_RESOURCE_REG_KEY, 
                             (LPCTSTR)os204_ResourceName, ValueName );
      
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( ValueType != ValueTypeExpected )
  {
    HANDLE_CLEAN_UP();
    teo200_EventList EvtLst( ERROR_INVALID_PARAMETER, FUNCTION_NAME_MEO60, 
                             ERR_INVALID_DATA_TYPE, ValueName );
      
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  HANDLE_CLEAN_UP();

  return true;
}


/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_GetResourceState ( CLUSTER_RESOURCE_STATE *      State,
                                                      CString*                      Group,
                                                      CString*                      Node,
                                                      teo200_EventList *            pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_GetResourceState"));
  LPWSTR                  NodeName;
  DWORD                   NodeNameLen      = DEFAULT_NAME_BUFFER_SIZE;
  DWORD                   PrevNodeNameLen  = 0;
  LPWSTR                  GroupName;
  DWORD                   GroupNameLen     = DEFAULT_NAME_BUFFER_SIZE;
  DWORD                   PrevGroupNameLen = 0;
  CLUSTER_RESOURCE_STATE  TmpState;

  if ( os204_ResourceHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_RESOURCE_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  do
  {
    if ( PrevNodeNameLen < NodeNameLen )
    {
      NodeNameLen      += sizeof(WCHAR);
      PrevNodeNameLen   = NodeNameLen;
      NodeName          = (LPWSTR)_alloca(NodeNameLen);
    }

    if ( PrevGroupNameLen < GroupNameLen )
    {
      GroupNameLen     += sizeof(WCHAR);
      PrevGroupNameLen  = GroupNameLen;
      GroupName         = (LPWSTR)_alloca(GroupNameLen);
    }

    
    TmpState = GetClusterResourceState(  os204_ResourceHandle, NodeName, &NodeNameLen, 
                                         GroupName, &GroupNameLen );

  } while ( GetLastError() == ERROR_MORE_DATA );

  if ( TmpState == ClusterResourceStateUnknown )
  {
    teo200_EventList EvtLst( GetLastError(), FUNCTION_NAME_MEO60, 
                             ERR_GET_RESOURCE_STATE, 
                             (LPCTSTR)os204_ResourceName );
      
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  if ( State != NULL )
    *State = TmpState;
  if ( Group != NULL )
    *Group = GroupName;
  if ( Node != NULL )
    *Node  = NodeName;
  
  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_SetProperties( const tos205_PropertyList&  PropertyList,
                                                  const DWORD                 ControlCode,
                                                  const HNODE                 HostNode,
                                                  teo200_EventList *          pEventList )

{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_SetProperties"));
  DWORD    rc         = NO_ERROR;

  ASSERT((ControlCode & (CLUSCTL_OBJECT_MASK << CLUSCTL_OBJECT_SHIFT))
      == (CLUS_OBJECT_RESOURCE << CLUSCTL_OBJECT_SHIFT));
  ASSERT(PropertyList.os205_PropertyListPtr()->pb != NULL);

  if ( os204_ResourceHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_RESOURCE_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  // --- Set properties.
  rc = ClusterResourceControl( os204_ResourceHandle,
                               HostNode,
                               ControlCode,
                               PropertyList.os205_PropertyListPtr()->pb,
                               PropertyList.os205_PropertyListSize(),
                               NULL,
                               0,
                               NULL );

  if ((rc != ERROR_SUCCESS) && ( rc != ERROR_RESOURCE_PROPERTIES_STORED ))
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_SETTING_RESOURCE_PROP, 
                             (LPCTSTR)os204_ResourceName );
      
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ClusterResource::os204_GetProperties( tos205_PropertyList&        PropertyList,
                                                  const DWORD                 ControlCode,
                                                  const HNODE                 HostNode,
                                                  teo200_EventList *          pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_GetProperties"));

  DWORD                   rc         = NO_ERROR;
  DWORD                   Props      = 512;
  CLUSPROP_BUFFER_HELPER  NewProplist;

  ASSERT((ControlCode & (CLUSCTL_OBJECT_MASK << CLUSCTL_OBJECT_SHIFT))
      == (CLUS_OBJECT_RESOURCE << CLUSCTL_OBJECT_SHIFT));

  if ( os204_ResourceHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, ERR_RESOURCE_HANDLE_NOT_VALID );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  do
  {
    // --- Allocate and zero a new buffer.
    NewProplist.pb = new BYTE[Props];

    if ( NewProplist.pb == NULL )
    {
      teo200_EventList EvtLst( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, 
                               ERR_OUT_OF_MEMORY );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }

    // --- Get properties.
    rc = ClusterResourceControl( os204_ResourceHandle,
                                 HostNode,
                                 ControlCode,
                                 NULL,
                                 0,
                                 NewProplist.pb,
                                 Props,
                                 &Props );

    if ( rc != ERROR_SUCCESS )
      delete [] NewProplist.pb;
  } 
  while ( rc == ERROR_MORE_DATA );

  if ( rc != ERROR_SUCCESS )
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_GETTING_RESOURCE_PROP,
                             (LPCTSTR)os204_ResourceName );
    
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  PropertyList.os205_FillPropertyList( NewProplist, Props, pEventList );

  delete [] NewProplist.pb;

  return true;
}


/*---------------------------------------------------------------------------*/
/*------------------------ class tos204_ResourceType ------------------------*/
/*---------------------------------------------------------------------------*/

bool tos204_ResourceType::os204_Open ( const tos204_Cluster&      Cluster, 
                                       const LPWSTR               ResourceTypeName,
                                       const LPWSTR               ResourceTypeDisplayName,
                                       const LPWSTR               ResourceTypeDll,
                                       const LPWSTR               ResourceExDllName,
                                       teo200_EventList *         pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ResourceType::os204_Open"));

  ASSERT(Cluster != NULL);
  ASSERT(ResourceTypeName != NULL);
  ASSERT(ResourceTypeDisplayName != NULL);
  ASSERT(ResourceTypeDll != NULL);

  os204_ClusterHandle              = Cluster;
  os204_ResourceTypeName           = ResourceTypeName;
  os204_ResourceTypeDisplayName    = ResourceTypeDisplayName;
  os204_ResourceTypeDll            = ResourceTypeDll;
  os204_ResourceExDllName          = ResourceExDllName;

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ResourceType::os204_Close( teo200_EventList *         pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ResourceType::os204_Close"));

  os204_ClusterHandle              = NULL;
  os204_ResourceTypeName           = "";
  os204_ResourceTypeDisplayName    = "";
  os204_ResourceTypeDll            = "";
  os204_ResourceExDllName          = "";
  os204_Created                    = false;

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ResourceType::os204_Create ( const tos204_Cluster&      Cluster, 
                                         const LPWSTR               ResourceTypeName,
                                         const LPWSTR               ResourceTypeDisplayName,
                                         const LPWSTR               ResourceTypeDll,
                                         const LPWSTR               ResourceExDllName,
                                         DWORD                      LooksAlivePollInterval,  
                                         DWORD                      IsAlivePollInterval,
                                         teo200_EventList *         pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ResourceType::os204_Create"));

  DWORD        rc;

  ASSERT(Cluster != NULL);
  ASSERT(ResourceTypeName != NULL);
  ASSERT(ResourceTypeDisplayName != NULL);
  ASSERT(ResourceTypeDll != NULL);

  os204_ClusterHandle              = Cluster;
  os204_ResourceTypeName           = ResourceTypeName;
  os204_ResourceTypeDisplayName    = ResourceTypeDisplayName;
  os204_ResourceTypeDll            = ResourceTypeDll;
  os204_ResourceExDllName          = ResourceExDllName;

  // --- try to load the resource extension dll and 
  //     all its functions
  ResourceExDll ExDll( ResourceExDllName );
  
  rc = CreateClusterResourceType( os204_ClusterHandle, os204_ResourceTypeName, 
                                  os204_ResourceTypeDisplayName, ResourceTypeDll, 
                                  LooksAlivePollInterval, IsAlivePollInterval );
  
  if ( rc != NO_ERROR && rc != ERROR_ALREADY_EXISTS)
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_CREATING_RESOURCE_TYPE, 
                             (LPCTSTR)os204_ResourceTypeDisplayName );
    
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;

  }
  
  if ( ExDll.os203_DLLLoaded() == true )
  {
    rc = ExDll.DllRegisterCluAdminExtension(os204_ClusterHandle);
    
    if ( rc != NO_ERROR ) 
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                               ERR_REGISTER_CLU_ADMIN_EXT, 
                               (LPCTSTR)ResourceExDllName );
      
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }

    rc = ExDll.DllRegisterServer();
    
    if ( rc != NO_ERROR ) 
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                               ERR_REGISTER_SERVER, 
                               (LPCTSTR)ResourceExDllName );
      
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }
  }

  os204_Created = true;

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ResourceType::os204_Remove ( teo200_EventList *             pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ResourceType::os204_Remove"));

  DWORD        rc;

  if ( os204_ClusterHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, 
                             ERR_CLUSTER_HANDLE_NOT_VALID );
    
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  // --- try to load the resource extension dll and 
  //     all its functions
  ResourceExDll ExDll( os204_ResourceExDllName );
  
  rc = DeleteClusterResourceType( os204_ClusterHandle, os204_ResourceTypeName );
  
  if (( rc != NO_ERROR ) && ( rc != ERROR_RESOURCE_TYPE_NOT_FOUND ))
  {
    if ( rc == ERROR_DIR_NOT_EMPTY )
    {
      teo200_EventList EvtLst( FUNCTION_NAME_MEO60, 
                               ERR_DEL_IN_USE_RESOURCE_TYPE, 
                               (LPCTSTR)os204_ResourceTypeDisplayName );
      
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }
    else
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                               ERR_DEL_RESOURCE_TYPE, 
                               (LPCTSTR)os204_ResourceTypeDisplayName );
      
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }
  }
  
  if ( ExDll.os203_DLLLoaded() == true )
  {
    rc = ExDll.DllUnregisterCluAdminExtension(os204_ClusterHandle);
    
    if (( rc != NO_ERROR ) && ( rc != ERROR_FILE_NOT_FOUND ))
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                               ERR_UNREGISTER_CLU_ADMIN_EXT, 
                               (LPCTSTR)os204_ResourceExDllName );
      
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }
    
    rc = ExDll.DllUnregisterServer ();
    
    if ( rc != NO_ERROR ) 
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                               ERR_UNREGISTER_SERVER, 
                               (LPCTSTR)os204_ResourceExDllName );
      
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ResourceType::os204_SetProperties( const tos205_PropertyList&  PropertyList,
                                               const DWORD                 ControlCode,
                                               const HNODE                 HostNode,
                                               teo200_EventList *          pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ResourceType::os204_SetProperties"));
  DWORD        rc         = NO_ERROR;

  ASSERT((ControlCode & (CLUSCTL_OBJECT_MASK << CLUSCTL_OBJECT_SHIFT))
         == (CLUS_OBJECT_RESOURCE_TYPE << CLUSCTL_OBJECT_SHIFT));
  ASSERT(PropertyList.os205_PropertyListPtr()->pb != NULL);

  if ( os204_ClusterHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, 
                             ERR_CLUSTER_HANDLE_NOT_VALID );
    
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  // --- Set properties.
  rc = ClusterResourceTypeControl( os204_ClusterHandle,
                                   os204_ResourceTypeName,
                                   HostNode,
                                   ControlCode,
                                   PropertyList.os205_PropertyListPtr()->pb,
                                   PropertyList.os205_PropertyListSize(),
                                   NULL,
                                   0,
                                   NULL );

  if ((rc != ERROR_SUCCESS) && ( rc != ERROR_RESOURCE_PROPERTIES_STORED ))
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_SETTING_RESOURCE_TYPE_PROP, 
                             (LPCTSTR)os204_ResourceTypeName );
    
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------*/

bool tos204_ResourceType::os204_GetProperties( tos205_PropertyList&        PropertyList,
                                                const DWORD                ControlCode,
                                                const HNODE                HostNode,
                                               teo200_EventList *          pEventList )
{
  ROUTINE_DBG_MEO60 (_T("tos204_ResourceType::os204_GetProperties"));

  DWORD                   rc         = NO_ERROR;
  DWORD                   Props      = 512;
  bool                    Ok         = true;
  CLUSPROP_BUFFER_HELPER  NewProplist;

  ASSERT((ControlCode & (CLUSCTL_OBJECT_MASK << CLUSCTL_OBJECT_SHIFT))
      == (CLUS_OBJECT_RESOURCE_TYPE << CLUSCTL_OBJECT_SHIFT));

  if ( os204_ClusterHandle == NULL )
  {
    teo200_EventList EvtLst( ERROR_INVALID_HANDLE, FUNCTION_NAME_MEO60, 
                             ERR_CLUSTER_HANDLE_NOT_VALID );
    
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  do
  {
    // --- Allocate and zero a new buffer.
    NewProplist.pb = new BYTE[Props];

    if ( NewProplist.pb == NULL )
    {
      teo200_EventList EvtLst( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, 
                               ERR_OUT_OF_MEMORY );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      return false;
    }

    // --- Get properties.
    rc = ClusterResourceTypeControl( os204_ClusterHandle,
                                     os204_ResourceTypeName,
                                     HostNode,
                                     ControlCode,
                                     NULL,
                                     0,
                                     NewProplist.pb,
                                     Props,
                                     &Props );

    if (rc != ERROR_SUCCESS)
      delete [] NewProplist.pb;
  } 
  while (rc == ERROR_MORE_DATA);

  if ( rc != ERROR_SUCCESS )
  {
    teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, 
                             ERR_GETTING_RESOURCE_TYPE_PROP, 
                             (LPCTSTR)os204_ResourceTypeName );
    
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }
  
  Ok = PropertyList.os205_FillPropertyList( NewProplist, Props, pEventList );
  
  delete [] NewProplist.pb;

  return Ok;
}


/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

ResourceExDll::ResourceExDll( LPCTSTR              DllFullName,
                              const HMODULE        ModuleHdl,
                              teo200_EventList *   pEventList )
{
  ROUTINE_DBG_MEO60 (_T("ResourceExDll::ResourceExDll"));
  teo200_EventList     TmpEventList;

  if (( DllFullName == NULL ) || ( DllFullName[0] == _T('\0') ))
    return;

  CLEAR_EVENTLIST_EO200(pEventList);

  CString ResourceExDll;

  if ( _tcsrchr(DllFullName, _T('\\')) == NULL )
  {
    os204_GetClusterServerPath( ResourceExDll );
    
    ResourceExDll += (CString)_T('\\') + DllFullName;
    
    os203_LoadDynDLL( ResourceExDll, &TmpEventList );
    
    if ( TmpEventList.eo200_rc() == ERROR_MOD_NOT_FOUND )
    {
      CLEAR_EVENTLIST_EO200( &TmpEventList );
      os203_LoadDynDLL( DllFullName, &TmpEventList );
    }

    if ( EVENTLIST_IS_EMPTY_EO200( &TmpEventList ) == false ) 
    {
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, TmpEventList );
    }
  }
  else
  {
    ResourceExDll = DllFullName;

    os203_LoadDynDLL( ResourceExDll, pEventList );
  }


  // --- load dll functions
  LOAD_FUNC_EX_OS203( DllRegisterCluAdminExtension, pEventList );
  LOAD_FUNC_EX_OS203( DllRegisterServer, pEventList );
  LOAD_FUNC_EX_OS203( DllUnregisterCluAdminExtension, pEventList );
  LOAD_FUNC_EX_OS203( DllUnregisterServer, pEventList );
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/