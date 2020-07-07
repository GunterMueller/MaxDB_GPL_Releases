/*!
  @file           gos204.h
  @author         RaymondR
  @brief          Cluster Server
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



#ifndef GOS204_H
#define GOS204_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


#include "geo200.h"
#include "gos00.h"
#include "gos203.h"
#include "gos205.h"
#include <clusapi.h>
#include <resapi.h>
#include "geo007_1.h"
#include "geo60.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/*-------------------------- class tos204_Cluster ---------------------------*/
/*---------------------------------------------------------------------------*/


class tos204_Cluster
{
public:
  inline               tos204_Cluster            ();
  inline               tos204_Cluster            ( LPCWSTR                        ClusterName,
                                                   teo200_EventList *             pEventList  = NULL );
  inline               ~tos204_Cluster           ();

  bool                 os204_Open                ( LPCWSTR                        ClusterName,
                                                   teo200_EventList *             pEventList  = NULL );
  bool                 os204_Open                ( teo200_EventList *             pEventList  = NULL );


  bool                 os204_Close               ( teo200_EventList *             pEventList  = NULL );
  bool                 os204_OpenEnum            ( DWORD                          dwType,
                                                   teo200_EventList *             pEventList  = NULL );
  bool                 os204_EnumReset           ( teo200_EventList *             pEventList  = NULL );
  bool                 os204_Enum                ( CString&                       Value,
                                                   bool&                          ItemFound,
                                                   teo200_EventList *             pEventList  = NULL );
  bool                 os204_CloseEnum           ( teo200_EventList *             pEventList  = NULL );

  inline const CString& os204_GetClusterName() const;
  inline operator  HCLUSTER() const;
  inline operator  bool() const;

  // --- debugging and tracing functions
  bool                 os204_RegisterDebugEx     ( teo200_EventList *             pEventList  = NULL );
  bool                 os204_UnregisterDebugEx   ( teo200_EventList *             pEventList  = NULL );

  bool                 os204_EnableClusterLog    ( const DWORD                    MBLogSize,
                                                   LPCTSTR                        LogName     = NULL,
                                                   teo200_EventList *             pEventList  = NULL );
  bool                 os204_DisableClusterLog   ( teo200_EventList *             pEventList  = NULL );
    
private:

  HCLUSTER     os204_ClusterHandle;
  CString      os204_ClusterName;

  // --- needed by 'os204_OpenEnum', 'os204_Enum' ...
  HCLUSENUM    os204_ClusterEnumHandle;
  DWORD        os204_EnumIdx;
};

/*---------------------------------------------------------------------------*/

inline tos204_Cluster::tos204_Cluster()
                      : os204_ClusterHandle(NULL),
                        os204_ClusterEnumHandle(NULL),
                        os204_ClusterName(""),
                        os204_EnumIdx(0)
{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::tos204_Cluster"));

}

/*---------------------------------------------------------------------------*/

inline tos204_Cluster::tos204_Cluster( LPCWSTR                        ClusterName,
                                       teo200_EventList *             pEventList )
                      : os204_ClusterHandle(NULL),
                        os204_ClusterEnumHandle(NULL),
                        os204_ClusterName(ClusterName),
                        os204_EnumIdx(0)
{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::tos204_Cluster"));

  os204_Open ( ClusterName, pEventList );
}

/*---------------------------------------------------------------------------*/

inline tos204_Cluster::~tos204_Cluster()
{
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::~tos204_Cluster"));

  teo200_EventList  Trash;  // Use Trash event list because should not 
  os204_Close( &Trash );    // throw exceptions in destuctors!!!
}

/*---------------------------------------------------------------------------*/

inline const CString&  tos204_Cluster::os204_GetClusterName() const
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::os204_GetClusterName"));

  return os204_ClusterName; 
};

/*---------------------------------------------------------------------------*/

inline tos204_Cluster::operator HCLUSTER() const 
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::operator HCLUSTER()"));

  return os204_ClusterHandle; 
};

/*---------------------------------------------------------------------------*/

inline tos204_Cluster::operator bool() const
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_Cluster::operator bool()"));

  return ( os204_ClusterHandle != NULL ); 
};


/*---------------------------------------------------------------------------*/
/*----------------------- class tos204_ClusterNetwork -----------------------*/
/*---------------------------------------------------------------------------*/

class tos204_ClusterNetwork
{
public:
  inline               tos204_ClusterNetwork     ();
  inline               tos204_ClusterNetwork     ( const tos204_Cluster&          Cluster,
                                                   const LPCWSTR                  NetworkName,
                                                   teo200_EventList *             pEventList  = NULL );

  inline               ~tos204_ClusterNetwork    ();

  bool                 os204_Open                ( const tos204_Cluster&          Cluster,
                                                   const LPCWSTR                  NetworkName,
                                                   teo200_EventList *             pEventList  = NULL );
  bool                 os204_Close               ( teo200_EventList *             pEventList  = NULL );

  bool                 os204_GetClusterNetworkId ( CString&                       NetworkIdStr,
                                                   teo200_EventList *             pEventList  = NULL );

  bool                 os204_SetProperties       ( const tos205_PropertyList&     PropertyList,
                                                   const DWORD                    ControlCode,
                                                   const HNODE                    HostNode    = NULL,
                                                   teo200_EventList *             pEventList  = NULL );
  bool                 os204_GetProperties       ( tos205_PropertyList&           PropertyList,
                                                   const DWORD                    ControlCode,
                                                   const HNODE                    HostNode    = NULL,
                                                   teo200_EventList *             pEventList  = NULL );


  
  inline const CString&  os204_GetNetworkName() const;
  inline operator HNETWORK()  const;
  inline operator  bool() const;

private:
  HNETWORK     os204_NetworkHandle;
  CString      os204_NetworkName;
};

/*---------------------------------------------------------------------------*/

inline tos204_ClusterNetwork::tos204_ClusterNetwork()
                             : os204_NetworkHandle(NULL),
                               os204_NetworkName("")
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::tos204_ClusterNetwork"));
}

/*---------------------------------------------------------------------------*/

inline tos204_ClusterNetwork::tos204_ClusterNetwork( const tos204_Cluster&          Cluster,
                                                     const LPCWSTR                  NetworkName,
                                                     teo200_EventList *             pEventList )
                             : os204_NetworkHandle(NULL),
                               os204_NetworkName("")
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::tos204_ClusterNetwork"));

  os204_Open ( Cluster, NetworkName, pEventList );
}

/*---------------------------------------------------------------------------*/

inline tos204_ClusterNetwork::~tos204_ClusterNetwork()
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::~tos204_ClusterNetwork"));

  teo200_EventList  Trash;  // Use Trash event list because should not 
  os204_Close( &Trash );    // throw exceptions in destuctors!!!
}

/*---------------------------------------------------------------------------*/

inline const CString&  tos204_ClusterNetwork::os204_GetNetworkName() const
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterNetwork::os204_GetNetworkName"));

  return os204_NetworkName; 
};

/*---------------------------------------------------------------------------*/

inline tos204_ClusterNetwork::operator HNETWORK() const 
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterNetwork::operator HNETWORK()"));

  return os204_NetworkHandle; 
};


/*---------------------------------------------------------------------------*/
/*------------------------ class tos204_ClusterGroup ------------------------*/
/*---------------------------------------------------------------------------*/

class tos204_ClusterGroup
{
public:
  inline               tos204_ClusterGroup       ();
  inline               tos204_ClusterGroup       ( const tos204_Cluster&          Cluster,
                                                   const LPCWSTR                  GroupName,
                                                   teo200_EventList *             pEventList  = NULL );
  inline               ~tos204_ClusterGroup      ();

  bool                 os204_Open                ( const tos204_Cluster&          Cluster,
                                                   const LPCWSTR                  GroupName,
                                                   teo200_EventList *             pEventList  = NULL );
  bool                 os204_Create              ( const tos204_Cluster&          Cluster,
                                                   const LPCWSTR                  GroupName,
                                                   teo200_EventList *             pEventList  = NULL );
  bool                 os204_Close               ( teo200_EventList *             pEventList  = NULL );

  bool                 os204_OpenEnum            ( DWORD                          dwType      = CLUSTER_GROUP_ENUM_CONTAINS,
                                                   teo200_EventList *             pEventList  = NULL );
  bool                 os204_EnumReset           ( teo200_EventList *             pEventList  = NULL );
  bool                 os204_Enum                ( CString&                       Value,
                                                   bool&                          ItemFound,
                                                   teo200_EventList *             pEventList  = NULL );
  bool                 os204_CloseEnum           ( teo200_EventList *             pEventList  = NULL );

  bool                 os204_BringOnline         ( DWORD                          Timeout     = 0,
                                                   teo200_EventList *             pEventList  = NULL );
  bool                 os204_BringOffline        ( DWORD                          Timeout     = 0,
                                                   teo200_EventList *             pEventList  = NULL );


  bool                 os204_Delete              ( teo200_EventList *             pEventList  = NULL );
  inline bool          os204_GroupCreated        ();

  bool                 os204_SetProperties       ( const tos205_PropertyList&     PropertyList,
                                                   const DWORD                    ControlCode,
                                                   const HNODE                    HostNode    = NULL,
                                                   teo200_EventList *             pEventList  = NULL );
  bool                 os204_GetProperties       ( tos205_PropertyList&           PropertyList,
                                                   const DWORD                    ControlCode,
                                                   const HNODE                    HostNode    = NULL,
                                                   teo200_EventList *             pEventList  = NULL );

  inline const CString&  os204_GetGroupName() const;
  inline operator  HGROUP()   const;
  inline operator  bool() const;

private:
  HGROUP       os204_GroupHandle;
  CString      os204_GroupName;
  bool         os204_Created; 

  // --- needed by 'os204_OpenEnum', 'os204_Enum' ...
  HGROUPENUM   os204_GroupEnumHandle;
  DWORD        os204_EnumIdx;
};

/*---------------------------------------------------------------------------*/

inline tos204_ClusterGroup::tos204_ClusterGroup()
                           : os204_GroupHandle(NULL),
                             os204_Created(false),
                             os204_GroupName(""),
                             os204_GroupEnumHandle(NULL),
                             os204_EnumIdx(0)
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::tos204_ClusterGroup"));
}

/*---------------------------------------------------------------------------*/

inline tos204_ClusterGroup::tos204_ClusterGroup( const tos204_Cluster&          Cluster,
                                                 const LPCWSTR                  GroupName,
                                                 teo200_EventList *             pEventList )
                           : os204_GroupHandle(NULL),
                             os204_Created(false),
                             os204_GroupName(""),
                             os204_GroupEnumHandle(NULL),
                             os204_EnumIdx(0)
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::tos204_ClusterGroup"));

  os204_Open ( Cluster, GroupName, pEventList );
}
/*---------------------------------------------------------------------------*/

inline tos204_ClusterGroup::~tos204_ClusterGroup()
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::~tos204_ClusterGroup"));

  teo200_EventList  Trash;  // Use Trash event list because should not 
  os204_Close( &Trash );    // throw exceptions in destuctors!!!
}

/*---------------------------------------------------------------------------*/

inline bool tos204_ClusterGroup::os204_GroupCreated ()
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::os204_GroupCreated"));

  return os204_Created; 
}

/*---------------------------------------------------------------------------*/

inline const CString&  tos204_ClusterGroup::os204_GetGroupName() const
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::os204_GetGroupName"));

  return os204_GroupName; 
};

/*---------------------------------------------------------------------------*/

inline tos204_ClusterGroup::operator HGROUP() const 
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::operator HGROUP()"));

  return os204_GroupHandle; 
};

/*---------------------------------------------------------------------------*/

inline tos204_ClusterGroup::operator bool() const
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterGroup::operator bool()"));

  return ( os204_GroupHandle != NULL ); 
};


/*---------------------------------------------------------------------------*/
/*---------------------- class tos204_ClusterResource -----------------------*/
/*---------------------------------------------------------------------------*/

class tos204_ClusterResource
{
public:
  inline               tos204_ClusterResource     ();
  inline               tos204_ClusterResource     ( const tos204_Cluster&          Cluster,
                                                    const LPCWSTR                  ResourceName,
                                                    teo200_EventList *             pEventList  = NULL );
  inline               ~tos204_ClusterResource    ();

  bool                 os204_Open                 ( const tos204_Cluster&          Cluster,
                                                    const LPCWSTR                  ResourceName,
                                                    teo200_EventList *             pEventList  = NULL );

  bool                 os204_Create               ( const tos204_Cluster&          Cluster,
                                                    const LPCWSTR                  ResourceName,
                                                    const tos204_ClusterGroup*     Group,
                                                    const LPCWSTR                  ResourceType,
                                                    const bool                     SeparateMonitor,
                                                    teo200_EventList *             pEventList  = NULL );
  bool                 os204_Create               ( const tos204_Cluster&          Cluster,
                                                    const LPCWSTR                  ResourceName,
                                                    const tos204_ClusterGroup*     Group,
                                                    const LPCWSTR                  ResourceType,
                                                    teo200_EventList *             pEventList  = NULL );

  bool                 os204_Close                ( teo200_EventList *             pEventList  = NULL );

  bool                 os204_ResourceTypeEqual    ( const LPCWSTR                  ResourceTypeName,
                                                    bool &                         TypesEqual,    
                                                    teo200_EventList *             pEventList  = NULL );

  bool                 os204_Delete               ( DWORD                          OffLineTimeOut = 0, 
                                                    teo200_EventList *             pEventList  = NULL );
  bool                 os204_BringOnline          ( DWORD                          Timeout     = 0,
                                                    teo200_EventList *             pEventList  = NULL );
  bool                 os204_BringOffline         ( DWORD                          Timeout     = 0,
                                                    teo200_EventList *             pEventList  = NULL );
  bool                 os204_AddResourceDependency( const tos204_ClusterResource&  DependsOnResource,
                                                    teo200_EventList *             pEventList  = NULL );
  bool                 os204_ChangeGroup          ( const tos204_ClusterGroup&     Group,
                                                    teo200_EventList *             pEventList  = NULL );
  bool                 os204_SetParam             ( const LPWSTR                   ValueName,
                                                    const LPCTSTR                  ValueData,
                                                    const DWORD                    ValueType   = REG_SZ,
                                                    const LPWSTR                   SubKeyName  = NULL,
                                                    teo200_EventList *             pEventList  = NULL );
  bool                 os204_SetParam             ( const LPWSTR                   ValueName,
                                                    const DWORD                    ValueData,
                                                    const LPWSTR                   SubKeyName  = NULL,
                                                    teo200_EventList *             pEventList  = NULL );
  bool                 os204_SetParam             ( const LPWSTR                   ValueName,
                                                    const void*                    ValueData,
                                                    const DWORD                    ValueType,
                                                    const LPWSTR                   SubKeyName  = NULL,
                                                    teo200_EventList *             pEventList  = NULL );
  bool                 os204_GetParam             ( const LPWSTR                   ValueName,
                                                    CString&                       Value,
                                                    const LPWSTR                   SubKeyName  = NULL,
                                                    teo200_EventList *             pEventList  = NULL );
  bool                 os204_GetParam             ( const LPWSTR                   ValueName,
                                                    DWORD&                         Value,
                                                    const LPWSTR                   SubKeyName  = NULL,
                                                    teo200_EventList *             pEventList  = NULL );
  bool                 os204_GetResourceState     ( CLUSTER_RESOURCE_STATE *       State,
                                                    CString*                       GroupName   = NULL,
                                                    CString*                       NodeName    = NULL,
                                                    teo200_EventList *             pEventList  = NULL );
 
  bool                 os204_SetProperties        ( const tos205_PropertyList&     PropertyList,
                                                    const DWORD                    ControlCode,
                                                    const HNODE                    HostNode     = NULL,
                                                    teo200_EventList *             pEventList   = NULL );
  bool                 os204_GetProperties        ( tos205_PropertyList&           PropertyList,
                                                    const DWORD                    ControlCode,
                                                    const HNODE                    HostNode     = NULL,
                                                    teo200_EventList *             pEventList   = NULL );
 
  inline bool          os204_ResourceCreated      ();
 

  inline const CString&  os204_GetResourceName    () const;
  inline operator  HRESOURCE() const;
  inline operator  bool() const;

private:
  bool                 os204_GetParameter         ( const LPWSTR                   ValueName,
                                                    const LPWSTR                   SubKeyName,
                                                    DWORD                          ValueTypeExpected,
                                                    void **                        ValueData,
                                                    teo200_EventList *             pEventList  = NULL );

  HRESOURCE              os204_ResourceHandle;
  CString                os204_ResourceName;
  bool                   os204_Created; 

  bool                   os204_GetPendingTimout  ( DWORD&                         Timeout,
                                                   teo200_EventList *             pEventList  = NULL );
};

/*---------------------------------------------------------------------------*/

inline tos204_ClusterResource::tos204_ClusterResource( const tos204_Cluster&          Cluster,
                                                       const LPCWSTR                  ResourceName,
                                                       teo200_EventList *             pEventList )
                              : os204_ResourceHandle(NULL),
                                os204_ResourceName(""),
                                os204_Created(false)
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::tos204_ClusterResource"));

  os204_Open ( Cluster, ResourceName, pEventList );
}

/*---------------------------------------------------------------------------*/

inline tos204_ClusterResource::tos204_ClusterResource() 
                              : os204_ResourceHandle(NULL),
                                os204_ResourceName(""),
                                os204_Created(false)
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::tos204_ClusterResource"));
}

/*---------------------------------------------------------------------------*/

inline tos204_ClusterResource::~tos204_ClusterResource()
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::~tos204_ClusterResource"));

  teo200_EventList  Trash;  // Use Trash event list because should not 
  os204_Close( &Trash );    // throw exceptions in destuctors!!!
}

/*---------------------------------------------------------------------------*/

inline bool tos204_ClusterResource::os204_ResourceCreated ()
{
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_ResourceCreated"));

  return os204_Created; 
}

/*---------------------------------------------------------------------------*/

inline const CString&  tos204_ClusterResource::os204_GetResourceName() const
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::os204_GetResourceName"));

  return os204_ResourceName; 
};

/*---------------------------------------------------------------------------*/

inline tos204_ClusterResource::operator HRESOURCE() const 
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::operator HRESOURCE()"));

  return os204_ResourceHandle; 
};

/*---------------------------------------------------------------------------*/

inline tos204_ClusterResource::operator bool() const
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_ClusterResource::operator bool()"));

  return ( os204_ResourceHandle != NULL ); 
};

/*---------------------------------------------------------------------------*/
/*------------------------ class tos204_ResourceType ------------------------*/
/*---------------------------------------------------------------------------*/

class tos204_ResourceType
{
public:
  inline               tos204_ResourceType       ();
  inline               tos204_ResourceType       ( const tos204_Cluster&          Cluster,
                                                   const LPWSTR                   ResourceTypeName,
                                                   const LPWSTR                   ResourceTypeDisplayName,
                                                   const LPWSTR                   ResourceTypeDll,
                                                   const LPWSTR                   ResourceExDllName      = NULL );
  inline               ~tos204_ResourceType      ();

  bool                 os204_Open                ( const tos204_Cluster&          Cluster,
                                                   const LPWSTR                   ResourceTypeName,
                                                   const LPWSTR                   ResourceTypeDisplayName,
                                                   const LPWSTR                   ResourceTypeDll,
                                                   const LPWSTR                   ResourceExDllName      = NULL,
                                                   teo200_EventList *             pEventList             = NULL );

  bool                 os204_Close               ( teo200_EventList *             pEventList             = NULL );

  bool                 os204_Create              ( const tos204_Cluster&          Cluster,
                                                   const LPWSTR                   ResourceTypeName,
                                                   const LPWSTR                   ResourceTypeDisplayName,
                                                   const LPWSTR                   ResourceTypeDll,
                                                   const LPWSTR                   ResourceExDllName      = NULL,
                                                   DWORD                          LooksAlivePollInterval = 5000,  
                                                   DWORD                          IsAlivePollInterval    = 60000,
                                                   teo200_EventList *             pEventList             = NULL );

  bool                 os204_Remove              ( teo200_EventList *             pEventList             = NULL );


  bool                 os204_SetProperties       ( const tos205_PropertyList&     PropertyList,
                                                   const DWORD                    ControlCode,
                                                   const HNODE                    HostNode               = NULL,
                                                   teo200_EventList *             pEventList             = NULL );
  bool                 os204_GetProperties       ( tos205_PropertyList&           PropertyList,
                                                   const DWORD                    ControlCode,
                                                   const HNODE                    HostNode               = NULL,
                                                   teo200_EventList *             pEventList             = NULL );

  inline bool          os204_ResourceTypeCreated ();

private:
  HCLUSTER     os204_ClusterHandle;
  CString      os204_ResourceTypeName;
  CString      os204_ResourceTypeDisplayName;
  CString      os204_ResourceTypeDll;
  CString      os204_ResourceExDllName;

  bool         os204_Created;                 // resource created!?
};

/*---------------------------------------------------------------------------*/

inline tos204_ResourceType::tos204_ResourceType ()
                           :os204_ClusterHandle(NULL),
                            os204_ResourceTypeName(""),
                            os204_ResourceTypeDisplayName(""),
                            os204_ResourceTypeDll(""),
                            os204_ResourceExDllName(""),
                            os204_Created(false)
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_ResourceType::tos204_ResourceType"));
};

/*---------------------------------------------------------------------------*/

inline tos204_ResourceType::tos204_ResourceType ( const tos204_Cluster&          Cluster,
                                                  const LPWSTR                   ResourceTypeName,
                                                  const LPWSTR                   ResourceTypeDisplayName,
                                                  const LPWSTR                   ResourceTypeDll,
                                                  const LPWSTR                   ResourceExDllName )
                           :os204_ClusterHandle( Cluster ),
                            os204_ResourceTypeName( ResourceTypeName ),
                            os204_ResourceTypeDisplayName( ResourceTypeDisplayName ),
                            os204_ResourceTypeDll( ResourceTypeDll ),
                            os204_ResourceExDllName( ResourceExDllName ),
                            os204_Created(false)
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_ResourceType::tos204_ResourceType"));
};

/*---------------------------------------------------------------------------*/

inline tos204_ResourceType::~tos204_ResourceType ()
{ 
  ROUTINE_DBG_MEO60 (_T("tos204_ResourceType::~tos204_ResourceType"));

  teo200_EventList  Trash;  // Use Trash event list because should not 
  os204_Close( &Trash );    // throw exceptions in destuctors!!!
};

/*---------------------------------------------------------------------------*/

inline bool tos204_ResourceType::os204_ResourceTypeCreated ()
{
  ROUTINE_DBG_MEO60 (_T("tos204_ResourceType::os204_ResourceTypeCreated"));

  return os204_Created; 
}

/*---------------------------------------------------------------------------*/

//  This function must be a inline function! This prevents DLL-Stubs like
//  'CluManDll' from loading the 'clusapi.dll' statically.

inline const bool os204_GetClusterServerPath ( CString&               Path,
                                               teo200_EventList *     pEventList = NULL )
{
  ROUTINE_DBG_MEO60 (_T("os204_GetClusterServerPath"));

  DWORD        rc;
  HKEY         hCreateKey;
  LPTSTR       ImagePath;
  LPTSTR       TmpPtr;
  DWORD        TmpLen     = 0;
  DWORD        PathLen    = 0;
  DWORD        BufferSize = 1; // - use at least one byte lenght for alloca!!
  bool         Ok         = true;
  DWORD        DataType;

  Path = "";

  rc = RegOpenKeyEx( HKEY_LOCAL_MACHINE, REG_SK_NT_CLUSTER_SERVER_SERVICE,
                     0, KEY_READ, &hCreateKey );

  if ( rc == ERROR_SUCCESS)
  {
    do
    {
      ImagePath = (LPTSTR)_alloca( BufferSize );

      if ( ImagePath == NULL )
        rc = ERROR_NOT_ENOUGH_MEMORY;
      else
        rc = RegQueryValueEx( hCreateKey, REG_VN_SERVICE_IMAGE_PATH, 0, &DataType, (BYTE*)ImagePath, &BufferSize  );
    }
    while ( rc == ERROR_MORE_DATA );

    RegCloseKey ( hCreateKey );

    if (( rc == NO_ERROR ) && ( DataType != REG_SZ ) && ( DataType != REG_EXPAND_SZ ))
      rc = ERROR_BAD_FORMAT;
  }

  if ( rc != ERROR_SUCCESS)
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_READING_IMAGE_PATH );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    Ok = false;
  }
  else 
  {
    if ( DataType == REG_EXPAND_SZ )
    {
      PathLen = 1; // - use at least one byte lenght for alloca!!
      do
      {
        TmpPtr = (LPTSTR)_alloca( PathLen * sizeof(TCHAR) );

        if ( TmpPtr == NULL )
          rc = ERROR_NOT_ENOUGH_MEMORY;
        else
        {
          TmpLen = ExpandEnvironmentStrings (ImagePath, TmpPtr, PathLen);

          if ( TmpLen == 0 )
            rc = GetLastError();
          else if ( TmpLen > PathLen )
          {
            rc      = ERROR_MORE_DATA;
            PathLen = TmpLen;
          }
          else
          {
            rc        = ERROR_SUCCESS;
            ImagePath = TmpPtr;
          }
        }
      }
      while ( rc == ERROR_MORE_DATA );
    }
      
    if ( rc != ERROR_SUCCESS)
    {
      teo200_EventList EvtLst( rc, FUNCTION_NAME_MEO60, ERR_EXPANDING_IMAGE_PATH );
      COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
      Ok = false;
    }
    else
    {
      TmpPtr = _tcsrchr ( ImagePath, L'\\' );

      if ( TmpPtr ) 
        TmpPtr[0] = L'\0';

      Path = ImagePath;
    }
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

//  This function must be a inline function! This prevents DLL-Stubs like
//  'CluManDll' from loading the 'clusapi.dll' statically.

inline const bool os204_LocalClusterHiveExists()
{
  ROUTINE_DBG_MEO60 (_T("os204_LocalClusterHiveExists"));

  DWORD        rc;
  HKEY         hCreateKey;

  rc = RegOpenKeyEx( HKEY_LOCAL_MACHINE, REG_SK_NT_CLUSTER_SERVER_HIVE,
                     0, KEY_READ, &hCreateKey );

  if ( rc == ERROR_SUCCESS)
    return true;

  return false;
}

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GOS204_H */
