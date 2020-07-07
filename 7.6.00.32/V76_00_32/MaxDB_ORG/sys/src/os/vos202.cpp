/*!
  @file           vos202.cpp
  @author         RaymondR
  @brief          SAPDB Microsoft Cluster Server Manager
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


#if !defined(CREATE_DLL) && !defined (CREATE_DLL_LOADER)
#  error "You have to set one of the following defines: 'CREATE_DLL' or 'CREATE_DLL_LOADER' !"
#endif

#if defined(CREATE_DLL) && !defined(UNICODE)
# error "You can compile this file with -DUNICODE only!"
#endif

#if defined(UNICODE)
# define  _UNICODE 
#endif

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#define NEEDED_WIN32_WINNT 0x0500

#if !defined(_WIN32_WINNT)
# define _WIN32_WINNT  NEEDED_WIN32_WINNT
#else
# if _WIN32_WINNT < NEEDED_WIN32_WINNT
#  undef _WIN32_WINNT
#  define _WIN32_WINNT  NEEDED_WIN32_WINNT
# endif
#endif

#include <afx.h>
#include <malloc.h>

#if defined(CREATE_DLL)
# include <clusapi.h>
# include <resapi.h>
# include <stdio.h>
#endif

#include "geo200.h"
#include "gos200.h"
#include "gos204.h"
#include "gos205.h"
#include "gos202.h"
#include "geo60.h"
#include "geo007_1.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	inline
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define  OPTION_ACTIVE(_Options, _Option)  (( _Options & _Option ) != 0)

#define  RESOURCE_TYPE_PHYSICAL_DISK    _T("Physical Disk")


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

#if defined(CREATE_DLL)
 static tos202_CluManDll         CluMan;
#endif

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/


/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


#if defined(CREATE_DLL)
 /*-----------------------------------------------*/
 /*-------------------- D L L --------------------*/
 /*-----------------------------------------------*/

 tos202_CluManDll::tos202_CluManDll () : os202_EvtOptions(0)
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::tos202_CluManDll"));
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_SetEvtOptions, ( unsigned     Options,
                                                                      bool         AddOptions ),
                             tos202_CluManDll, CluMan, ( Options, AddOptions ) ) 
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_SetEvtOptions"));

   if ( AddOptions )
     os202_EvtOptions |= Options;
   else
     os202_EvtOptions  = Options;

   return false;
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_BringGroupOnline, ( DWORD        Timeout,
                                                                         LPCSTR       GroupName),
                             tos202_CluManDll, CluMan, ( Timeout, GroupName ) ) 
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_BringGroupOnline"));

   return os202_OnlineOfflineGroup ( GroupName, Timeout, online_request_eos202 );
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_BringGroupOffline, ( DWORD        Timeout,
                                                                          LPCSTR       GroupName ),
                             tos202_CluManDll, CluMan, ( Timeout, GroupName ) ) 
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_BringGroupOffline"));

   return os202_OnlineOfflineGroup ( GroupName, Timeout, offline_request_eos202 );
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_MoveDriveToGroup, ( LPCSTR       DriveName ), 
                             tos202_CluManDll, CluMan, ( DriveName ) ) 
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_MoveDriveToGroup"));
   bool                    Ok        = true;
   CString                 DriveNam  = DriveName;

   try 
   {
     tos204_Cluster          Cluster( NULL );  // NULL means local
     tos204_ClusterGroup     SAPDBGroup;
     tos204_ClusterResource  Drive( Cluster, DriveNam );
     tos204_ClusterResource  XSERVER;
     CString                 GroupNameStr;
     CString                 SAPDBGroupNameStr;
     bool                    ResourceFound;

     // --- find XSERVER resource!
     ResourceFound = os202_FindResource ( Cluster, XSERVER, RESOURCE_TYPE_XSERVER_OS00);

     if ( ResourceFound == true)
     {
       // --- get group name where XSERVER is a member
       XSERVER.os204_GetResourceState ( NULL, &GroupNameStr );
     }
     else
       GroupNameStr = GROUP_NAME_OS00;

     SAPDBGroup.os204_Open( Cluster, GroupNameStr );

     Drive.os204_ChangeGroup (SAPDBGroup);

     tos204_ClusterResource  SERVERDB;

     ResourceFound = os202_FindResource ( Cluster, SERVERDB, RESOURCE_TYPE_SERVERDB_OS00 );

     if ( ResourceFound == true )
     {
       SERVERDB.os204_GetResourceState ( NULL, &SAPDBGroupNameStr ); // - get group name where XSERVER is member

       if ( !SAPDBGroupNameStr.CompareNoCase ( GroupNameStr ))
         SERVERDB.os204_AddResourceDependency( Drive );
     }
   }
   catch ( teo200_EventList EventList )
   {
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(EventList.eo200_rc());
     Ok = false;
   }
   catch (CMemoryException *e) //   PTS 1129537
   {  
	 e->Delete();
     teo200_EventList EventList ( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY ); 
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(ERROR_NOT_ENOUGH_MEMORY);
     Ok = false;
   }

   return Ok;
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_CreateResourceTypes, (),
                             tos202_CluManDll, CluMan, () )
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_CreateResourceTypes"));

   bool                    Ok      = true;
   tos204_Cluster          Cluster;
   tos204_ResourceType     XSERVER;
   tos204_ResourceType     SERVERDB;

   try 
   {
     Cluster.os204_Open();

     XSERVER.os204_Create( Cluster,
                           RESOURCE_TYPE_XSERVER_OS00,
                           RESOURCE_TYPE_XSERVER_OS00,
                           SAPDBCLURES_DLL_NAME_OS00 );

     SERVERDB.os204_Create( Cluster,
                            RESOURCE_TYPE_SERVERDB_OS00,
                            RESOURCE_TYPE_SERVERDB_OS00,
                            SAPDBCLURES_DLL_NAME_OS00,
                            SAPDBCLUEX_DLL_NAME_OS00 );

   }
   catch ( teo200_EventList EventList )
   {
     if ( XSERVER.os204_ResourceTypeCreated())
       XSERVER.os204_Remove();

     if ( SERVERDB.os204_ResourceTypeCreated())
       SERVERDB.os204_Remove();

     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(EventList.eo200_rc());
     Ok = false;
   }
   catch (CMemoryException *e) //   PTS 1129537
   {  
	 e->Delete();
     if ( XSERVER.os204_ResourceTypeCreated())
       XSERVER.os204_Remove();

     if ( SERVERDB.os204_ResourceTypeCreated())
       SERVERDB.os204_Remove();

     teo200_EventList EventList ( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY ); 
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(ERROR_NOT_ENOUGH_MEMORY);
     Ok = false;
   }

   return Ok;
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_RemoveResourceTypes, (), 
                             tos202_CluManDll, CluMan, () ) 
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_RemoveResourceTypes"));

   bool                    Ok       = true;

   try 
   {
     tos204_Cluster          Cluster(NULL);
     tos204_ResourceType     XSERVER ( Cluster,
                                       RESOURCE_TYPE_XSERVER_OS00,
                                       RESOURCE_TYPE_XSERVER_OS00,
                                       SAPDBCLURES_DLL_NAME_OS00 );
     tos204_ResourceType     SERVERDB( Cluster,
                                       RESOURCE_TYPE_SERVERDB_OS00,
                                       RESOURCE_TYPE_SERVERDB_OS00,
                                       SAPDBCLURES_DLL_NAME_OS00 );

     XSERVER.os204_Remove ();
     SERVERDB.os204_Remove();
   }
   catch ( teo200_EventList EventList )
   {
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(EventList.eo200_rc());
     Ok = false;
   }
   catch (CMemoryException *e) //   PTS 1129537
   {  
	 e->Delete();
     teo200_EventList EventList ( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY ); 
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(ERROR_NOT_ENOUGH_MEMORY);
     Ok = false;
   }

   return Ok;
 }


 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_BuildUpGroup, (  LPCSTR   NetworkName,
                                                                      LPCSTR   IPAddress,
                                                                      LPCSTR   SubnetMask,
                                                                      LPCSTR   DriveName ), 
                             tos202_CluManDll, CluMan, ( NetworkName, 
                                                         IPAddress, 
                                                         SubnetMask, 
                                                         DriveName ) )
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_BuildUpGroup"));
   bool                    Ok                   = true;
   bool                    RemoveCreatedObjects = false;
   tos204_ClusterGroup     SAPDBGroup;
   tos204_ClusterResource  SAPDBIPAddr;
   tos204_ClusterResource  XSERVER;

   try 
   {
     CString                 NetworkNameStr     = NetworkName;
     CString                 IPAddressStr       = IPAddress;
     CString                 SubnetMaskStr      = SubnetMask;
     CString                 DriveNameStr       = DriveName;
     CString                 SAPDBGroupNameStr = GROUP_NAME_OS00;
     bool                    ResourceFound;

     // --- create Cluster object       
     tos204_Cluster          Cluster(NULL);

     // --- open network and drive resource
     tos204_ClusterNetwork   Network  ( Cluster, NetworkNameStr );
     tos204_ClusterResource  Drive    ( Cluster, DriveNameStr );

     // --- find XSERVER resource!
     ResourceFound = os202_FindResource ( Cluster, XSERVER, RESOURCE_TYPE_XSERVER_OS00 );

     if ( ResourceFound == true)
       XSERVER.os204_GetResourceState ( NULL, &SAPDBGroupNameStr ); // - get group name where XSERVER is member

     // --- check if IP Address is already in use!
     ResourceFound = os202_FindResource ( Cluster, SAPDBIPAddr, RESOURCE_TYPE_IP_ADDRESS_OS00, 
                                          _T("Address"), IPAddressStr );
     if ( ResourceFound == true)
     {
       CString GroupNameStr;
       SAPDBIPAddr.os204_GetResourceState ( NULL, &GroupNameStr );

       // --- resource not in SAPDB group?
       if ( SAPDBGroupNameStr.CompareNoCase ( GroupNameStr ))
       {
         CString ResourceNameStr = SAPDBIPAddr.os204_GetResourceName();

         // --- resource name unknown?
         if ( ResourceNameStr.CompareNoCase((CString)RESOURCE_NAME_IP_ADDRESS_OS00))
         {
           throw teo200_EventList( FUNCTION_NAME_MEO60, ERR_IP_ADDRESS_ALREADY_IN_USE,
                                   (LPCTSTR)IPAddressStr, (LPCTSTR)ResourceNameStr );
         }
         else if ( XSERVER == false )  // --- XSERVER resource not found?
           SAPDBGroupNameStr = GroupNameStr;
       }
       SAPDBIPAddr.os204_Close();
     }

     // --- open/create SAPDB Group, SAPDB IP Address and move SAPDB IP Address to 'SAPDB Group'
     //
     SAPDBGroup.os204_Create  ( Cluster, SAPDBGroupNameStr );
     SAPDBIPAddr.os204_Create ( Cluster, RESOURCE_NAME_IP_ADDRESS_OS00,
                                &SAPDBGroup, RESOURCE_TYPE_IP_ADDRESS_OS00 );
     
     // --- XSERVER already open or created?
     if ( XSERVER == false ) 
       XSERVER.os204_Create ( Cluster, SERVICE_DISPLAY_STR_XSERVER,   // - open/create XSERVER resource
                              &SAPDBGroup, RESOURCE_TYPE_XSERVER_OS00 );
       
     // --- Build IP Address property list
     tos205_PropertyList     PropList(true);
     
     PropList.os205_InsertProperty( _T("Network"), NetworkNameStr );
     PropList.os205_InsertProperty( _T("Address"),   IPAddressStr );
     PropList.os205_InsertProperty( _T("SubnetMask"), SubnetMaskStr );
     
     // --- Set known IP Address Properties
     SAPDBIPAddr.os204_SetProperties( PropList, CLUSCTL_RESOURCE_SET_PRIVATE_PROPERTIES );
       
     // --- Build XSERVER property list
     PropList.os205_ClearList();
     PropList.os205_InsertProperty( REG_MSCS_PARAM_LOG_VERBOSE_OS00, 0 );
       
     // --- Set known XSERVER Properties
     XSERVER.os204_SetProperties( PropList, CLUSCTL_RESOURCE_SET_PRIVATE_PROPERTIES );
       
     // --- Set XSERVER dependencies
     XSERVER.os204_AddResourceDependency( SAPDBIPAddr );
       
     // --- Move shared drive to the SAPDB group
     Drive.os204_ChangeGroup (SAPDBGroup);
     XSERVER.os204_AddResourceDependency( Drive );
   }
   catch (teo200_EventList EventList)
   {
     // --- remove objects if created
     RemoveCreatedObjects = true;

     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(EventList.eo200_rc());
     Ok = false;
   }
   catch (CMemoryException *e) //   PTS 1129537
   {  
	 e->Delete();
     // --- remove objects if created
     RemoveCreatedObjects = true;

     teo200_EventList EventList ( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY ); 
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(ERROR_NOT_ENOUGH_MEMORY);
     Ok = false;
   }

   if ( RemoveCreatedObjects == true )
   {
     try 
     {
       // --- remove objects if created
       if (XSERVER.os204_ResourceCreated())
         XSERVER.os204_Delete();
       
       if (SAPDBIPAddr.os204_ResourceCreated())
         SAPDBIPAddr.os204_Delete();
       
       if (SAPDBGroup.os204_GroupCreated())
         SAPDBGroup.os204_Delete();
     }
     catch (...) { /* - Ignore any errors  */ }
   }


   return Ok;
 }


 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_RemoveSAPDBResources, (), 
                             tos202_CluManDll, CluMan, () ) 
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_RemoveSAPDBResources"));
   bool                    Ok       = true;
   bool                    TypesEqual;

   try 
   {
     teo200_EventList  EventList;

     // --- open Cluster object
     tos204_Cluster          Cluster (NULL);

     CString ResourceName;
     bool    ResourceFound;

     // --- open cluster enumerator
     Cluster.os204_OpenEnum( CLUSTER_ENUM_RESOURCE );

     //
     // --- delete resource if the resource type is XSERVER or SERVERDB
     //
     do 
     {
       Cluster.os204_Enum(ResourceName, ResourceFound);

       // --- enumerate first/next resource
       if ( ResourceFound )
       {
         // --- open resource
         tos204_ClusterResource Resource( Cluster, ResourceName, &EventList );

         if ( EventList.IsEmpty() == false ) 
         {
           // --- ignoring resource which was delete just now!
           if ( EventList.eo200_rc() != ERROR_RESOURCE_NOT_FOUND )
             throw EventList;
         }
         else
         {
           Resource.os204_ResourceTypeEqual( RESOURCE_TYPE_SERVERDB_OS00, TypesEqual );

           if ( TypesEqual == true )
             Resource.os204_Delete( 20 );
         }
       }
     } while ( ResourceFound );

     Cluster.os204_EnumReset();

     //
     // --- delete resource if the resource type is XSERVER
     //
     do 
     {
       Cluster.os204_Enum(ResourceName, ResourceFound);

       // --- enumerate first/next resource
       if ( ResourceFound )
       {
         // --- open resource
         tos204_ClusterResource Resource( Cluster, ResourceName, &EventList );
         
         if ( EventList.IsEmpty() == false )
         {
           // --- ignoring resource which was delete just now!
           if ( EventList.eo200_rc() != ERROR_RESOURCE_NOT_FOUND )
             throw EventList;
         }
         else
         {
           Resource.os204_ResourceTypeEqual( RESOURCE_TYPE_XSERVER_OS00, TypesEqual );

           if ( TypesEqual == true )
             Resource.os204_Delete( 20 );
         }
       }
     } while ( ResourceFound );

     Cluster.os204_CloseEnum();

     // -- try open SAPDB ip address resource
     tos204_ClusterResource SAPDBIPAddr( Cluster, RESOURCE_NAME_IP_ADDRESS_OS00, &EventList );

     if ( EventList.IsEmpty() == true )
       SAPDBIPAddr.os204_Delete( 20 ); // -- remove SAPDB ip address resource
     else if ( EventList.eo200_rc() != ERROR_RESOURCE_NOT_FOUND )
       throw EventList;
   }
   catch ( teo200_EventList EventList )
   {
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(EventList.eo200_rc());
     Ok = false;
   }
   catch (CMemoryException *e) //   PTS 1129537
   {  
	 e->Delete();
     teo200_EventList EventList ( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY ); 
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(ERROR_NOT_ENOUGH_MEMORY);
     Ok = false;
   }

   return Ok;
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_OnlineXSERVERResource, (), 
                             tos202_CluManDll, CluMan, () ) 
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_OnlineXSERVERResource"));

   return os202_OnlineOfflineXSERVERResource ( online_request_eos202 );
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_OfflineXSERVERResource, (), 
                             tos202_CluManDll, CluMan, () )
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_OfflineXSERVERResource"));

   return os202_OnlineOfflineXSERVERResource ( offline_request_eos202 );
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_OnlineSERVERDBResource, ( LPCSTR  ServerDB ), 
                             tos202_CluManDll, CluMan, ( ServerDB ) )
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_OnlineSERVERDBResource"));
   return os202_OnlineOfflineSERVERDBResource ( ServerDB, online_request_eos202 );
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_OfflineSERVERDBResource, ( LPCSTR  ServerDB ), 
                             tos202_CluManDll, CluMan, ( ServerDB ) ) 
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_OfflineSERVERDBResource"));

   return os202_OnlineOfflineSERVERDBResource ( ServerDB, offline_request_eos202 );
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_AddSERVERDBResource, ( LPCSTR  SERVERDBName, 
                                                                            LPCSTR  Remark ), 
                             tos202_CluManDll, CluMan, ( SERVERDBName, Remark ) )
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_AddSERVERDBResource"));
   bool                    Ok       = true;

   try
   {
     bool                    ResourceFound;
     CString                 GroupNameStr;
     CString                 RemarkStr = Remark;
     tos204_Cluster          Cluster( NULL );
     tos204_ClusterResource  SERVERDB;
     CString                 SERVERDBStr;
     TCHAR                   UpperChar;

     // --- SERVERDB Resource already exists
     ResourceFound = os202_FindResource ( Cluster, SERVERDB, RESOURCE_TYPE_SERVERDB_OS00, 
                                          REG_MSCS_PARAM_SERVERDB_OS00, (CString)SERVERDBName );

     if ( ResourceFound == false )
     {
       tos204_ClusterResource  XSERVER;
       tos204_ClusterGroup     SAPDBGroup;

       // --- find XSERVER resource!
       ResourceFound = os202_FindResource ( Cluster, XSERVER, RESOURCE_TYPE_XSERVER_OS00 );

       if ( ResourceFound == true)
       {
         // --- get group name where XSERVER is a member
         XSERVER.os204_GetResourceState ( NULL, &GroupNameStr );
       }
       else
         GroupNameStr = GROUP_NAME_OS00;

       SAPDBGroup.os204_Create( Cluster, GroupNameStr );

       SERVERDBStr = SERVERDBName;
       SERVERDBStr.MakeUpper();
       UpperChar = SERVERDBStr.GetAt(0);
       SERVERDBStr.MakeLower();
       SERVERDBStr.SetAt(0, UpperChar);
       
       // --- create SERVERDB resource
       SERVERDB.os204_Create( Cluster, SERVERDBStr, &SAPDBGroup, 
                              RESOURCE_TYPE_SERVERDB_OS00 );
       
       // --- Build SERVERDB property list
       tos205_PropertyList     PropList(true);
       
       PropList.os205_InsertProperty( REG_MSCS_PARAM_AUTO_RES_SHT_OS00, 1 );
       PropList.os205_InsertProperty( REG_MSCS_PARAM_LOG_VERBOSE_OS00, 0 );
       PropList.os205_InsertProperty( REG_MSCS_PARAM_SERVERDB_OS00, (LPCTSTR)SERVERDBStr);
       
       // --- Set known SERVERDB Properties
       SERVERDB.os204_SetProperties( PropList, CLUSCTL_RESOURCE_SET_PRIVATE_PROPERTIES );
       
       // --- Set unknown SERVERDB Properties
       if ( Remark != NULL )
         SERVERDB.os204_SetParam ( REG_MSCS_PARAM_REMARK_OS00, RemarkStr);

       if (( os202_SetDependenciesToDrives ( Cluster, SERVERDB, GroupNameStr ) == false ) &&
           ( XSERVER == true ))
       {
          SERVERDB.os204_AddResourceDependency( XSERVER );
       }
     }
   }
   catch ( teo200_EventList EventList )
   {
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(EventList.eo200_rc());
     Ok = false;
   }
   catch (CMemoryException *e) //   PTS 1129537
   {  
	 e->Delete();
     teo200_EventList EventList ( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY ); 
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(ERROR_NOT_ENOUGH_MEMORY);
     Ok = false;
   }

   return Ok;
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_RemoveSERVERDBResource, ( LPCSTR  SERVERDBName,
                                                                               DWORD   OffLineTimeOut ),
                             tos202_CluManDll, CluMan, ( SERVERDBName, OffLineTimeOut ) )
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_RemoveSERVERDBResource"));
   bool                    Ok      = true;

   try
   {
     bool                    ResourceFound;
     tos204_Cluster          Cluster( NULL );
     tos204_ClusterResource  SERVERDB;

     // --- SERVERDB Resource already exists
     ResourceFound = os202_FindResource ( Cluster, SERVERDB, RESOURCE_TYPE_SERVERDB_OS00,
                                          REG_MSCS_PARAM_SERVERDB_OS00, (CString)SERVERDBName );

     if ( ResourceFound == true )
     {
       // --- delete SERVERDB resource
       SERVERDB.os204_Delete( OffLineTimeOut );
     }
   }
   catch ( teo200_EventList EventList )
   {
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(EventList.eo200_rc());
     Ok = false;
   }
   catch (CMemoryException *e) //   PTS 1129537
   {  
	 e->Delete();
     teo200_EventList EventList ( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY ); 
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(ERROR_NOT_ENOUGH_MEMORY);
     Ok = false;
   }

   return Ok;
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_EnableDisableClusterLog, ( const DWORD  MBLogSize, 
                                                                                LPCSTR       LogName ), 
                             tos202_CluManDll, CluMan, ( MBLogSize, LogName ))
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_EnableDisableClusterLog"));
   tos204_Cluster          Cluster;
   bool                    Ok        = true;

   try
   {
     if ( MBLogSize != 0 )
     {
       if ( LogName != NULL )
       {
         CString   LogNameStr = LogName;  // convert to unicode
         Cluster.os204_EnableClusterLog( MBLogSize, LogNameStr );
       }
       else
         Cluster.os204_EnableClusterLog( MBLogSize );
     }
     else
       Cluster.os204_DisableClusterLog();
   }
   catch ( teo200_EventList EventList )
   {
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(EventList.eo200_rc());
     Ok = false;
   }
   catch (CMemoryException *e) //   PTS 1129537
   {  
	 e->Delete();
     teo200_EventList EventList ( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY ); 
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(ERROR_NOT_ENOUGH_MEMORY);
     Ok = false;
   }

   return Ok;
 }

 /*---------------------------------------------------------------------------*/

 MK_C_DLL_ENTRY_POINT_OS203( bool, DMOD_OS203, os202_RegisterUnregisterDebugEx, ( const bool  Register ), 
                             tos202_CluManDll, CluMan, ( Register ) )
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_RegisterUnregisterDebugEx"));
   tos204_Cluster          Cluster(NULL);
   bool                    Ok        = true;

   try
   {
     if ( Register )
       Cluster.os204_RegisterDebugEx();
     else
       Cluster.os204_UnregisterDebugEx();
   }
   catch ( teo200_EventList EventList )
   {
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(EventList.eo200_rc());
     Ok = false;
   }
   catch (CMemoryException *e) //   PTS 1129537
   {  
	 e->Delete();
     teo200_EventList EventList ( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY ); 
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(ERROR_NOT_ENOUGH_MEMORY);
     Ok = false;
   }

   return Ok;
 }

#else
 /*-----------------------------------------------*/
 /*------------- D L L - L O A D E R -------------*/
 /*-----------------------------------------------*/

 tos202_CluManDll::tos202_CluManDll ( unsigned     EvtOptions )
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::tos202_CluManDll"));

   try 
   {
     teo200_EventList  EventList;

     // --- cluster server available?
     if ( os204_LocalClusterHiveExists() == false ) 
     {
       SetLastError(ERROR_FILE_NOT_FOUND);
       EventList = teo200_EventList( FUNCTION_NAME_MEO60, ERR_CLUSTER_NOT_INSTALLED ); 
     }
     else
     {
       os203_LoadDynDLL( SAPDBCLUMAN_DLL_NAME_OS00, &EventList );

       if ( EventList.eo200_rc() == ERROR_MOD_NOT_FOUND )
       {
         CString ResourceExDll;
         os204_GetClusterServerPath( ResourceExDll );
         
         ResourceExDll += (CString)_T('\\') + SAPDBCLUMAN_DLL_NAME_OS00;
         
         os203_LoadDynDLL( ResourceExDll, &EventList );
       }
     }

     if (( ( EvtOptions & EVTOPT_DLL_LOAD_ERROR_OFF_OS202) == 0) && 
           ( EventList.IsEmpty()                           == false ))
     {
       throw EventList;
     }

     if ( EventList.IsEmpty() == true )
     {
       // --- load dll functions
       LOAD_FUNC_OS203( os202_BringGroupOnline );
       LOAD_FUNC_OS203( os202_BringGroupOffline );
       
       LOAD_FUNC_OS203( os202_CreateResourceTypes );
       LOAD_FUNC_OS203( os202_RemoveResourceTypes );
       LOAD_FUNC_OS203( os202_AddSERVERDBResource );
       LOAD_FUNC_OS203( os202_RemoveSERVERDBResource );
       LOAD_FUNC_OS203( os202_RemoveSAPDBResources );
       
       LOAD_FUNC_OS203( os202_BuildUpGroup );
       LOAD_FUNC_OS203( os202_MoveDriveToGroup );
       
       LOAD_FUNC_OS203( os202_OnlineXSERVERResource );
       LOAD_FUNC_OS203( os202_OfflineXSERVERResource );
       
       LOAD_FUNC_OS203( os202_OnlineSERVERDBResource );
       LOAD_FUNC_OS203( os202_OfflineSERVERDBResource );
       
       LOAD_FUNC_OS203( os202_EnableDisableClusterLog );
       LOAD_FUNC_OS203( os202_RegisterUnregisterDebugEx );
       
       // --- set DLL print options
       LOAD_FUNC_OS203( os202_SetEvtOptions );
       os202_SetEvtOptions( EvtOptions & ~EVTOPT_DLL_LOAD_ERROR_OFF_OS202, false ); // PTS 1129537
     }
   }
   catch ( teo200_EventList  EventList )
   {
     os202_HandleEvt(EventList, EvtOptions);
     SetLastError(EventList.eo200_rc()); 
     os203_UnloadDynDLL();
   }
   catch (CMemoryException *e) //   PTS 1129537
   {  
	 e->Delete();
     teo200_EventList EventList ( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY ); 
     os202_HandleEvt(EventList, EvtOptions);

     SetLastError(ERROR_NOT_ENOUGH_MEMORY);
     os203_UnloadDynDLL();
   }


   return;
 }

 /*---------------------------------------------------------------------------*/

static const bool os202_ClusterServerInsalled ()
 { 
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_ClusterServerInsalled"));

   return ::os204_LocalClusterHiveExists();
 }

#endif

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

# if defined(CREATE_DLL)

 /*-----------------------------------------------*/
 /*-------------------- D L L --------------------*/
 /*-----------------------------------------------*/

 bool tos202_CluManDll::os202_OnlineOfflineXSERVERResource ( const tos202_RequestType     Request )
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_OnlineOfflineXSERVERResource"));
   bool                    Ok       = true;

   try 
   {
     tos204_Cluster          Cluster( NULL );
     tos204_ClusterResource  XSERVER;
     bool                    XSERVERFound;
     
     XSERVERFound = os202_FindResource ( Cluster, XSERVER, RESOURCE_TYPE_XSERVER_OS00 );
     
     if ( XSERVERFound == false )
     {
       throw teo200_EventList( ERROR_RESOURCE_NOT_FOUND, FUNCTION_NAME_MEO60, ERR_RESOURCE_NOT_FOUND,
                               (LPCTSTR)RESOURCE_TYPE_XSERVER_OS00 );
     }
     
     switch (Request)
     {
     case online_request_eos202:
       XSERVER.os204_BringOnline();
       break;
     case offline_request_eos202:
       XSERVER.os204_BringOffline();
       break;
     }

   }
   catch ( teo200_EventList EventList )
   {
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(EventList.eo200_rc());
     Ok = false;
   }
   catch (CMemoryException *e) //   PTS 1129537
   {  
	 e->Delete();
     teo200_EventList EventList ( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY ); 
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(ERROR_NOT_ENOUGH_MEMORY);
     Ok = false;
   }

   return Ok;
 }

 /*---------------------------------------------------------------------------*/

 bool tos202_CluManDll::os202_OnlineOfflineSERVERDBResource ( LPCSTR                     SERVERDBName,
                                                              const tos202_RequestType   Request )
 {
   ROUTINE_DBG_MEO60 (_T("os202_OnlineOfflineSERVERDBResource"));
   bool                    Ok       = true;
 
   try 
   {
     // --- open Cluster object
     tos204_Cluster          Cluster( NULL );
     tos204_ClusterResource  SERVERDB;
     bool                    ResourceFound;
     
     ResourceFound = os202_FindResource ( Cluster, SERVERDB, RESOURCE_TYPE_SERVERDB_OS00,
                                          REG_MSCS_PARAM_SERVERDB_OS00, (CString)SERVERDBName );
     
     if ( ResourceFound == false )
     {
       throw teo200_EventList( ERROR_RESOURCE_NOT_FOUND, FUNCTION_NAME_MEO60, ERR_RESOURCE_NOT_FOUND,
                            (LPCTSTR)(CString)SERVERDBName );
     }
     
     switch (Request)
     {
     case online_request_eos202:
       SERVERDB.os204_BringOnline();
       break;
     case offline_request_eos202:
       SERVERDB.os204_BringOffline();
       break;
     }
   }
   catch ( teo200_EventList EventList )
   {
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(EventList.eo200_rc());
     Ok = false;
   }
   catch (CMemoryException *e) //   PTS 1129537
   {  
	 e->Delete();
     teo200_EventList EventList ( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY ); 
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(ERROR_NOT_ENOUGH_MEMORY);
     Ok = false;
   }

   return Ok;
 }

 /*---------------------------------------------------------------------------*/

 bool tos202_CluManDll::os202_OnlineOfflineGroup ( LPCSTR                     GroupName,
                                                   DWORD                      Timeout,
                                                   const tos202_RequestType   Request )
 {
   ROUTINE_DBG_MEO60 (_T("os202_OnlineOfflineGroup"));
   bool                    Ok      = true;

   try 
   {
     tos204_Cluster          Cluster( NULL );  // NULL means local
     tos204_ClusterGroup     SAPDBGroup;
     tos204_ClusterResource  XSERVER;
     CString                 GroupNameStr;
     bool                    ResourceFound;

     if ( GroupName == NULL )
     {
       // --- find XSERVER resource!
       ResourceFound = os202_FindResource ( Cluster, XSERVER, RESOURCE_TYPE_XSERVER_OS00 );
       
       if ( ResourceFound == true)
       {
         // --- get group name where XSERVER is a member
         XSERVER.os204_GetResourceState ( NULL, &GroupNameStr );
       }
       else
         GroupNameStr = GROUP_NAME_OS00;
     }
     else
       GroupNameStr = GroupName;

     SAPDBGroup.os204_Open( Cluster, GroupNameStr );

     switch (Request)
     {
     case online_request_eos202:
       SAPDBGroup.os204_BringOnline( Timeout );
       break;
     case offline_request_eos202:
       SAPDBGroup.os204_BringOffline( Timeout );
       break;
     }

   }
   catch ( teo200_EventList EventList )
   {
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(EventList.eo200_rc());
     Ok = false;
   }
   catch (CMemoryException *e) //   PTS 1129537
   {  
	 e->Delete();
     teo200_EventList EventList ( ERROR_NOT_ENOUGH_MEMORY, FUNCTION_NAME_MEO60, ERR_OUT_OF_MEMORY ); 
     os202_HandleEvt(EventList, os202_EvtOptions);
     SetLastError(ERROR_NOT_ENOUGH_MEMORY);
     Ok = false;
   }

   return Ok;
 }

 /*---------------------------------------------------------------------------*/
 
 bool tos202_CluManDll::os202_FindResource ( tos204_Cluster&              Cluster,
                                             tos204_ClusterResource&      Resource,
                                             LPCTSTR                      ResourceTypeName,
                                             LPCTSTR                      ValueName,
                                             LPCTSTR                      Value )
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_FindResource")); 
 
   CString                 ResourceName;
   bool                    ResourceFound;
   bool                    Found = false;
   teo200_EventList        EventList;
   bool                    TypesEqual;
 
   // --- open cluster enumerator
   Cluster.os204_OpenEnum( CLUSTER_ENUM_RESOURCE );
 
   do 
   {
     // --- enumerate first/next resource
     Cluster.os204_Enum(ResourceName, ResourceFound);
     
     if ( ResourceFound == true )
     {
       // --- open resource
       Resource.os204_Open( Cluster, ResourceName, &EventList );

       if ( EventList.IsEmpty() == false )
       {
         // --- ignoring resource which was delete just now!
         if ( EventList.eo200_rc() != NO_ERROR )
           throw EventList;
       }
       else
       {
         Resource.os204_ResourceTypeEqual( ResourceTypeName, TypesEqual );
        
         // --- resource type found?
         if ( TypesEqual == true )
         {
           if (( ValueName != NULL ) && ( Value != NULL ) )
           {
             tos205_PropertyList     PropList(true);
             CString                 ValueStr         = Value;
             CString                 PropValueNameStr = ValueName;
             WORD                    PropFormat;
             DWORD                   PropValueLen;
             LPCWSTR                 PropValue;


             // --- Get known Properties
             Resource.os204_GetProperties( PropList, CLUSCTL_RESOURCE_GET_PRIVATE_PROPERTIES );

             Found = PropList.os205_GetPropertyValue( PropValueNameStr, PropFormat, PropValueLen, (void**)&PropValue );

             if (( Found      == false )              ||				// - Value not found?
                 ( PropFormat != CLUSPROP_FORMAT_SZ ) ||				// - Value type not correct?
                 ( ValueStr.CompareNoCase( (CString)PropValue ) ))	// - Value unexpected?
             {
               Found = false;
             }
           }
           else
           {
             Found = true;
           }
         }
       }
 
       if ( Found == false )
         Resource.os204_Close(); // --- close resource
     }
   } while ( ResourceFound && ( Found == false ));
   
   Cluster.os204_CloseEnum();
 
   return Found;
 }
 
 /*---------------------------------------------------------------------------*/
 
 bool tos202_CluManDll::os202_SetDependenciesToDrives ( tos204_Cluster&              Cluster,
                                                        tos204_ClusterResource&      ReqResource,
                                                        LPCTSTR                      ReqResourceGroupNameStr )
 {
   ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_SetDependenciesToDrives")); 
 
   CString                 ResourceName;
   bool                    ResourceFound;
   bool                    Found = false;
   teo200_EventList        EventList;
   bool                    TypesEqual;
   tos204_ClusterResource  Resource;
 
   // --- open cluster enumerator
   Cluster.os204_OpenEnum( CLUSTER_ENUM_RESOURCE );
 
   do 
   {
     // --- enumerate first/next resource
     Cluster.os204_Enum(ResourceName, ResourceFound);
     
     if ( ResourceFound == true )
     {
       // --- open resource
       Resource.os204_Open( Cluster, ResourceName, &EventList );

       if ( EventList.IsEmpty() == false )
       {
         // --- ignoring resource which was delete just now!
         if ( EventList.eo200_rc() != NO_ERROR )
           throw EventList;
       }
       else
       {
         Resource.os204_ResourceTypeEqual( RESOURCE_TYPE_PHYSICAL_DISK, TypesEqual );
        
         // --- resource type found?
         if ( TypesEqual == true )
         {
           CString  GroupNameStr;
           
           // --- get group name where the resource is member
           Resource.os204_GetResourceState ( NULL, &GroupNameStr );
           
           // --- resource in ReqResource group?
           if ( !GroupNameStr.CompareNoCase ( ReqResourceGroupNameStr ))
           {
             ReqResource.os204_AddResourceDependency( Resource );
             Found = true;
           }
         }
       }
       Resource.os204_Close(); // --- close resource
     }
   } while ( ResourceFound );
   
   Cluster.os204_CloseEnum();
 
   return Found;
 }
 
 /*---------------------------------------------------------------------------*/
  
 bool tos202_CluManDll::os202_FindResourceType ( tos204_Cluster&            Cluster,
                                                 LPCTSTR                    ResourceTypeName )

 {
    ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_FindResourceType"));
      
    CString ResourceTypeCompareName;
    bool    ResourceTypeFound;
  
   // --- open cluster enumerator
   Cluster.os204_OpenEnum( CLUSTER_ENUM_RESTYPE );
  
   do 
   {
     // --- enumerate first/next resource
     Cluster.os204_Enum(ResourceTypeCompareName, ResourceTypeFound);
      
     // --- enumerate first/next resource
     if (( ResourceTypeFound ) && ( ResourceTypeCompareName == ResourceTypeName ))
       return true;
   } while ( ResourceTypeFound );
    
   Cluster.os204_CloseEnum();
 
   return false; 
 }
  
#endif


/*---------------------------------------------------------------------------*/
/*------------------------------- C O M M O N -------------------------------*/
/*---------------------------------------------------------------------------*/
 
void  tos202_CluManDll::os202_HandleEvt ( teo200_EventList &   EventList,
                                          unsigned             Options )
{
  ROUTINE_DBG_MEO60 (_T("tos202_CluManDll::os202_HandleEvt"));

  LPTSTR Msg = (LPTSTR)_alloca(1024 * sizeof(TCHAR));

  if ( EventList.IsEmpty() == false )
  {
    if ( OPTION_ACTIVE (Options, EVTOPT_PRINT_ON_OS202))
    {
      if ( OPTION_ACTIVE (Options, EVTOPT_VERBOSE_OS202) )
      {
        if ( EventList.eo200_rc() != NO_ERROR )
        {
          LPTSTR SystemMsg = (LPTSTR)_alloca(256 * sizeof(TCHAR));

          FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,
                         NULL,
                         EventList.eo200_rc(),
                         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                         SystemMsg,
                         255,
                         NULL );

          _stprintf( Msg, // - Max. 1024 character!
                     _T("\n%5d %-.20s: %-.512s, rc = %d\n   - %-.256s   - (%-.128s)\n"),
                     EventList.eo200_EventID(),
                     EventList.eo200_EventType(true),
                     (LPCTSTR)EventList.eo200_EventMsg(),
                     EventList.eo200_rc(),
                     SystemMsg,
                     (LPCTSTR)EventList.eo200_FunctionName() );
        }
        else
          _stprintf( Msg, // - Max. 1024 character!
                     _T("\n%5d %-.20s: %-.512s.\n   - (%-.128s)\n"),
                     EventList.eo200_EventID(),
                     EventList.eo200_EventType(true),
                     (LPCTSTR)EventList.eo200_EventMsg(),
                     (LPCTSTR)EventList.eo200_FunctionName() );
      }
      else
      {
        if ( EventList.eo200_rc() != NO_ERROR )
          _stprintf( Msg, // - Max. 1024 character!
                     _T("\n%5d %-.20s: %-.512s, rc = %d.\n"),
                     EventList.eo200_EventID(),
                     EventList.eo200_EventType(true),
                     (LPCTSTR)EventList.eo200_EventMsg(),
                     EventList.eo200_rc() ); 
        else
          _stprintf( Msg, // - Max. 1024 character!
                     _T("\n%5d %-.20s: %-.512s.\n"),
                     EventList.eo200_EventID(),
                     EventList.eo200_EventType(true),
                     (LPCTSTR)EventList.eo200_EventMsg());
      }

      if ( OPTION_ACTIVE (Options, EVTOPT_DIALOG_BOX_OS202) )
      {
        UINT     fuStyle;

        if ( EventList.eo200_EventType() == WRN_TYPE )
        {
          MessageBeep( MB_ICONEXCLAMATION );
          fuStyle = MB_OK | MB_ICONEXCLAMATION;
        }
        else if ( EventList.eo200_EventType() == INFO_TYPE )
        {
          MessageBeep( MB_ICONHAND );
          fuStyle = MB_OK | MB_ICONINFORMATION;
        }
        else
        {
          MessageBeep( MB_ICONHAND );
          fuStyle = MB_OK | MB_ICONINFORMATION;
        }
        
        MessageBox( HWND_DESKTOP, Msg, PRODUCT_GROUP_NAME, fuStyle);
      }
      else
      {
        ULONG   ulWritten;

#       if defined(UNICODE)
         LPSTR cMsg = (LPSTR)_alloca(1024);

         WideCharToMultiByte( CP_ACP, 0, Msg, -1, cMsg, 1024, NULL, NULL );

         WriteFile( GetStdHandle(STD_OUTPUT_HANDLE),
                    cMsg, (ULONG)strlen ( cMsg ), &ulWritten, NULL );
#       else
         WriteFile( GetStdHandle(STD_OUTPUT_HANDLE),
                    Msg, (ULONG)strlen ( Msg ), &ulWritten, NULL );
#       endif
      }
    }
    if ( OPTION_ACTIVE (Options, EVTOPT_THROW_EVT_OS202))
    {
      throw EventList;
    }
  }
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/