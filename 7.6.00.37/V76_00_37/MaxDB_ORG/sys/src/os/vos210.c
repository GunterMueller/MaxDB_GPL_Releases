/*!
  @file           vos210.c
  @author         RaymondR
  @brief          Cluster Server DLL
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

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <clusapi.h>
#include <resapi.h>

#include "gos00.h"
#include "geo60.h"
#include "gos200.h"
#include "gos210.h"
#include "geo007_2.h"   // - messages


#pragma comment(lib, "clusapi.lib")
#pragma comment(lib, "resutils.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "netapi32.lib")


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	__inline
#endif

#define PENDING_SLEEP_TIME            500  // - msec
#define START_TIME_OUT                180  // - msec


#define INFOMSG_CS_COMMON_MSG_TAG             _T("[SAP DBTech] ")


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#if defined (DEBUG_CLU_RES_DLL)
# define LOG_VERBOSE(_values)       (os210_LogEvent)_values
#else
# define LOG_VERBOSE(_values)       if (ResourceEntry->Params.LogVerbose) (os210_LogEvent)_values
#endif

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

typedef union t_ResourceParams {
  struct COMMON
  {
    DWORD  LogVerbose;  // - common parameter
  };

  struct SERVERDB 
  {
    DWORD  LogVerbose;  // - common parameter

    LPWSTR ServerDB;
    DWORD  AutoRestartShutdown;
  } SERVERDB;

  struct XSERVER
  {
    DWORD  LogVerbose;  // - common parameter

  } XSERVER;
} t_ResourceParams;

typedef enum {
   UNKNOWN_e,
   XSERVER_e,
   SERVERDB_e,
} t_ResourceType;


typedef struct t_Resource {
    RESID                   ResId;      // --- for validation
    t_ResourceParams        Params;
    HKEY                    ResourceKey;
    HKEY                    ParametersKey;
    t_ResourceType          ResourceType;
    RESOURCE_HANDLE         ResourceHandle;
    LPWSTR                  ResourceName;
    CLUS_WORKER             OnlineThread;
    TCHAR                   ServiceName[100];
    CLUSTER_RESOURCE_STATE  State;
    DWORD                   StartTimeout;
} t_Resource;



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

// --- Forward references to the RESAPI function tables.
extern CLRES_FUNCTION_TABLE os210_SERVERDB_FunctionTable;
extern CLRES_FUNCTION_TABLE os210_XSERVER_FunctionTable;

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

// --- Event Logging routine.
static PLOG_EVENT_ROUTINE             os210_LogEvent                    = NULL;

// --- Resource Status routine for pending Online and Offline calls.
static PSET_RESOURCE_STATUS_ROUTINE   os210_SetResourceStatus           = NULL;

// --- Resource read-write private properties.
static RESUTIL_PROPERTY_ITEM          os210_SERVERDB_PrivateProperties[] = {{ REG_MSCS_PARAM_LOG_VERBOSE_OS00,
                                                                              NULL, 
                                                                              CLUSPROP_FORMAT_DWORD, 
                                                                              0, // - default
                                                                              0, // - min
                                                                              0, // - max
                                                                              0, 
                                                                              FIELD_OFFSET(t_ResourceParams,SERVERDB.LogVerbose) },
                                                                            { REG_MSCS_PARAM_SERVERDB_OS00,
                                                                              NULL, 
                                                                              CLUSPROP_FORMAT_SZ, 
                                                                              0, // - default
                                                                              0, // - min
                                                                              0, // - max
                                                                              RESUTIL_PROPITEM_REQUIRED, 
                                                                              FIELD_OFFSET(t_ResourceParams,SERVERDB.ServerDB) },
                                                                            { REG_MSCS_PARAM_AUTO_RES_SHT_OS00,
                                                                              NULL, 
                                                                              CLUSPROP_FORMAT_DWORD,
                                                                              1,  // - default
                                                                              0,  // - min
                                                                              1,  // - max
                                                                              RESUTIL_PROPITEM_REQUIRED,
                                                                              FIELD_OFFSET(t_ResourceParams,SERVERDB.AutoRestartShutdown) },
                                                                              { 0 }};

static RESUTIL_PROPERTY_ITEM          os210_XSERVER_PrivateProperties[]  = {{ REG_MSCS_PARAM_LOG_VERBOSE_OS00,
                                                                              NULL, 
                                                                              CLUSPROP_FORMAT_DWORD, 
                                                                              0, // - default
                                                                              0, // - min
                                                                              0, // - max
                                                                              0, 
                                                                              FIELD_OFFSET(t_ResourceParams,XSERVER.LogVerbose) },
                                                                              { 0 }};


// --- Define Function Tables
static CLRES_V1_FUNCTION_TABLE( os210_SERVERDB_FunctionTable, // Function Table Name
                                CLRES_VERSION_V1_00,          // Version
                                os210_SERVERDB_,              // Prefix
                                NULL,                         // Arbitrate
                                NULL,                         // Release
                                os210_ResourceControl,        // ResControl
                                os210_ResourceTypeControl);   // ResTypeControl

static CLRES_V1_FUNCTION_TABLE( os210_XSERVER_FunctionTable,  // Function Table Name
                                CLRES_VERSION_V1_00,          // Version
                                os210_XSERVER_,               // Prefix
                                NULL,                         // Arbitrate
                                NULL,                         // Release
                                os210_ResourceControl,        // ResControl
                                os210_ResourceTypeControl);   // ResTypeControl


/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

static RESID           os210_Open               ( t_ResourceType              ResourceType,
                                                  LPCWSTR                     ResourceName,
                                                  HKEY                        ResourceKey,    
                                                  RESOURCE_HANDLE             ResourceHandle );
static void            os210_Close              ( RESID                       ResourceId );
static DWORD           os210_Online             ( RESID                       ResourceId, 
                                                 PHANDLE                      EventHandle );
static DWORD WINAPI    os210_OnlineThread       ( PCLUS_WORKER                WorkerPtr,
                                                  t_Resource                  *ResourceEntry );
static DWORD           os210_Offline            ( RESID                       ResourceId, 
                                                  bool                        Gracefully );
static DWORD           os210_CheckResId         ( RESID                       ResourceId );
static DWORD           os210_CheckIsAlive       ( RESID                       ResourceId );
static t_ResourceType  os210_GetResourceType    ( LPCWSTR                     ResourceTypeName );
static DWORD           os210_ValidatePrivResProp( RESUTIL_PROPERTY_ITEM       *PrivateProperties,
                                                  t_Resource                  *ResourceEntry,
                                                  PVOID                       InBuffer,
                                                  DWORD                       InBufferSize,
                                                  t_ResourceParams            *ResourceParams );
static DWORD            os210_SetPrivResProp    ( RESUTIL_PROPERTY_ITEM       *PrivateProperties,
                                                  t_Resource                  *ResourceEntry,
                                                  PVOID                       InBuffer,
                                                  DWORD                       InBufferSize );


/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


/*!--------------------------------------------------------------------------*
 * PUBLIC FUNCTION DllMain
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Main DLL entry point.
 * 
 * description:
 *   Main DLL entry point.
 * 
 * arguments:
 *   DllHandle - [in] DLL instance handle
 *   Reason    - [in] Reason for being called (thread/proccess attach/detach)
 *   Reserved  -      Reserved argument
 * 
 * return value:
 *   TRUE  - Success.
 *   FALSE - Failure. 
 * 
 */ 

BOOLEAN WINAPI DllMain ( HINSTANCE                    DllHandle,             
                         DWORD                        Reason,                
                         LPVOID                       Reserved )             
{
  ROUTINE_DBG_MEO60 (_T("DllMain"));

  switch( Reason ) 
  {
  case DLL_PROCESS_ATTACH:
    DisableThreadLibraryCalls( DllHandle );
    break;
    
  case DLL_PROCESS_DETACH:
    break;
  }
  
  return TRUE;
}


/*!--------------------------------------------------------------------------*
 * PUBLIC FUNCTION Startup
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Startup the resource DLL
 * 
 * description:
 *   Startup the resource DLL. This routine verifies that at least one
 *   currently supported version of the resource DLL is between 
 *   MinVersionSupported and MaxVersionSupported. If not, then the resource
 *   DLL returns ERROR_REVISION_MISMATCH.
 * 
 *   If more than one version of the resource DLL interface is supported by
 *   the resource DLL, then the highest version (up to MaxVersionSupported)
 *   should be returned as the resource DLL's interface. If the returned
 *   version is not within range, then startup fails.
 * 
 *   The ResourceTypeName is passed in so that the resource DLL supports more
 *   than one ResourceType, it pass back the correct function table
 *   associated with the ResourceType.
 * 
 * 
 * arguments:
 *   ResourceTypeName    - [in]  The type of resource requesting a function 
 *                               table.
 *   MinVersionSupported - [in]  The minimum resource DLL interface version 
 *                               supported by the cluster software.
 *   MaxVersionSupported - [in]  The maximum resource DLL interface version
 *                               supported by the cluster software.
 *   SetResourceStatus   - [in]  Pointer to a routine that the resource DLL should 
 *                               call to update the state of a resource after the 
 *                               Online or Offline routine returns a Status of 
 *                               ERROR_IO_PENDING.
 *   LogEvent            - [in]  Pointer to a routine that handles the reporting of 
 *                               events from the resource DLL.
 *   FunctionTable       - [out] Returns a pointer to the function table defined for 
 *                               the version of the resource DLL interface returned 
 *                               by the resource DLL.
 * 
 * return value:
 *   ERROR_SUCCESS             - The operation was successful.
 *   ERROR_MOD_NOT_FOUND       - The resource type is unknown by this DLL. 
 *   ERROR_REVISION_MISMATCH   - The version of the cluster service doesn't 
 *                               match the versrion of the DLL. 
 *   Win32 error code          - The operation failed.
 * 
 */ 

DWORD WINAPI Startup ( LPCWSTR                        ResourceTypeName,
                       DWORD                          MinVersionSupported,   
                       DWORD                          MaxVersionSupported,   
                       PSET_RESOURCE_STATUS_ROUTINE   SetResourceStatus,     
                       PLOG_EVENT_ROUTINE             LogEvent,              
                       PCLRES_FUNCTION_TABLE          *FunctionTable )       
{

  ROUTINE_DBG_MEO60 (_T("Startup"));

  if ( (MinVersionSupported > CLRES_VERSION_V1_00) ||
       (MaxVersionSupported < CLRES_VERSION_V1_00) ) 
  {
    return(ERROR_REVISION_MISMATCH);
  }
  
  switch (os210_GetResourceType ( ResourceTypeName ))
  {
  case SERVERDB_e:
    *FunctionTable = &os210_SERVERDB_FunctionTable;
    break;
  case XSERVER_e:
    *FunctionTable = &os210_XSERVER_FunctionTable;
    break;
  case UNKNOWN_e:
    return ERROR_MOD_NOT_FOUND;
  }

  if ( !os210_LogEvent ) 
  {
    os210_LogEvent          = LogEvent;
    os210_SetResourceStatus = SetResourceStatus;
  }
  
  
  return ERROR_SUCCESS;
}


/*!--------------------------------------------------------------------------*
 * PUBLIC FUNCTION os210_XSERVER_Open, os210_SERVERDB_Open
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Open routine for SERVERDB/XSERVER resources.
 * 
 * description:
 *   Open the specified resource (create an instance of the resource).
 *   Allocate all structures necessary to bring the specified resource
 *   online.
 * 
 * arguments:
 *   ResourceName        - [in]  Supplies the name of the resource to open.
 *   ResourceKey         - [in]  Supplies handle to the resource's cluster 
 *                               configuration database key.
 *   ResourceHandle      - [in]  A handle that is passed back to the resource 
 *                               monitor when the SetResourceStatus or LogEvent 
 *                               method is called. See the description of the 
 *                               SetResourceStatus and LogEvent methods on the
 *                               os210_Statup routine. This handle should never 
 *                               be closed or used for any purpose other than 
 *                               passing it as an argument back to the Resource 
 *                               Monitor in the SetResourceStatus or LogEvent
 *                               callback.
 * 
 * return value:
 *   RESID                     - resource id of created resource.
 *   NULL                      - on failure.
 * 
 */ 

RESID WINAPI os210_XSERVER_Open ( LPCWSTR                     ResourceName,
                                  HKEY                        ResourceKey,    
                                  RESOURCE_HANDLE             ResourceHandle )
{
  ROUTINE_DBG_MEO60 (_T("os210_XSERVER_Open"));

  return os210_Open ( XSERVER_e, ResourceName, ResourceKey, ResourceHandle );
}

/*---------------------------------------------------------------------------*/

RESID WINAPI os210_SERVERDB_Open ( LPCWSTR                    ResourceName,
                                   HKEY                       ResourceKey,    
                                   RESOURCE_HANDLE            ResourceHandle )
{
  ROUTINE_DBG_MEO60 (_T("os210_SERVERDB_Open"));

  return os210_Open ( SERVERDB_e, ResourceName, ResourceKey, ResourceHandle );
}


/*!--------------------------------------------------------------------------*
 * PUBLIC FUNCTION os210_XSERVER_Close, os210_SERVERDB_Close
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Close routine for SERVERDB/XSERVER resources.
 * 
 * description:
 *   Close the specified resource and deallocate all structures, etc.,
 *   allocated in the Open call. If the resource is not in the offline state,
 *   then the resource should be taken offline (by calling Terminate) before
 *   the close operation is performed.
 * 
 * arguments:
 *   ResourceId          - [in]  Supplies the RESID of the resource to close.
 * 
 * return value:
 *   None.
 * 
 */ 

void WINAPI os210_XSERVER_Close ( RESID ResourceId )
{
  ROUTINE_DBG_MEO60 (_T("os210_XSERVER_Close"));

  os210_Close ( ResourceId );

  return;
}

/*---------------------------------------------------------------------------*/

void WINAPI os210_SERVERDB_Close ( RESID ResourceId )
{
  ROUTINE_DBG_MEO60 (_T("os210_SERVERDB_Close"));

  os210_Close ( ResourceId );

  return;
}


/*!--------------------------------------------------------------------------*
 * PUBLIC FUNCTION os210_XSERVER_Online, os210_SERVERDB_Online
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Online routine for SERVERDB/XSERVER resources.
 * 
 * description:
 *   Bring the specified resource online (available for use). The resource
 *   DLL should attempt to arbitrate for the resource if it is present on a
 *   shared medium.
 * 
 * arguments:
 *   ResourceId          - [in]     Supplies the resource id for the resource to 
 *                                  be brought online
 *   EventHandle         - [in/out] Returns a signalable handle that is signaled 
 *                                  when the resource DLL detects a failure on the 
 *                                  resource. This argument is NULL on input, and 
 *                                  the resource DLL returns NULL if asynchronous 
 *                                  notification of failures is not supported, 
 *                                  otherwise this must bethe address of a handle 
 *                                  that is signaled on resource failures.
 * 
 * return value:
 *   ERROR_SUCCESS                - The operation was successful, and the resource 
 *                                  is now online.  
 *   
 *   ERROR_RESOURCE_NOT_FOUND     - RESID is not valid.  
 *   ERROR_RESOURCE_NOT_AVAILABLE - If the resource was arbitrated with some   
 *                                  other systems and one of the other systems   
 *                                  won the arbitration.  
 *   ERROR_IO_PENDING             - The request is pending, a thread has been 
 *                                  activated to process the online request. The 
 *                                  thread that is processing the online request 
 *                                  will periodically report Status by calling 
 *                                  the SetResourceStatus callback method, until 
 *                                  the resource is placed into the 
 *                                  ClusterResourceOnline state (or the resource 
 *                                  monitor decides to timeout the online request
 *                                  and Terminate the resource. This pending timeout
 *                                  value is settable and has a default value of 3 
 *                                  minutes.).
 *   Win32 error code             - The operation failed.
 *
 */      

DWORD WINAPI os210_XSERVER_Online ( RESID    ResourceId, 
                                    PHANDLE  EventHandle )
{
  ROUTINE_DBG_MEO60 (_T("os210_XSERVER_Online"));

  return os210_Online ( ResourceId, EventHandle );
}

/*---------------------------------------------------------------------------*/

DWORD WINAPI os210_SERVERDB_Online ( RESID    ResourceId, 
                                     PHANDLE  EventHandle )
{ 
  ROUTINE_DBG_MEO60 (_T("os210_SERVERDB_Online"));

  return os210_Online ( ResourceId, EventHandle );
}


/*!--------------------------------------------------------------------------*
 * PUBLIC FUNCTION os210_XSERVER_Offline, os210_SERVERDB_Offline
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Offline routine for SERVERDB/XSERVER resources.
 * 
 * description:
 *   Take the specified resource offline gracefully (unavailable for use). 
 *   Wait for any cleanup operations to complete before returning.
 * 
 * arguments:
 *   ResourceId          - [in]     Supplies the resource id for the resource to
 *                                  be shutdown gracefully.
 * return value:
 *   ERROR_SUCCESS                - The operation was successful, and the resource 
 *                                  is now offline.
 *   ERROR_RESOURCE_NOT_FOUND     - RESID is not valid.  
 *   Win32 error code             - The operation failed.
 *
 */      

DWORD WINAPI os210_XSERVER_Offline ( RESID    ResourceId )
{
  ROUTINE_DBG_MEO60 (_T("os210_XSERVER_Offline"));

  return os210_Offline ( ResourceId, true );
}

/*---------------------------------------------------------------------------*/

DWORD WINAPI os210_SERVERDB_Offline ( RESID    ResourceId )
{ 
  ROUTINE_DBG_MEO60 (_T("os210_SERVERDB_Offline"));

  return os210_Offline ( ResourceId, true );
}


/*!--------------------------------------------------------------------------*
 * PUBLIC FUNCTION os210_XSERVER_Terminate, os210_SERVERDB_Terminate
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Terminate routine for SERVERDB/XSERVER resources.
 * 
 * description:
 *   Take the specified resource offline immediately (the resource is
 *   unavailable for use).
 * 
 * arguments:
 *   ResourceId          - [in]     Supplies the resource id for the to be brought 
 *                                  offline.
 * return value:
 *   ERROR_SUCCESS                - The operation was successful, and the resource 
 *                                  is now offline.
 *   ERROR_RESOURCE_NOT_FOUND     - RESID is not valid.  
 *   Win32 error code             - The operation failed.
 *
 */      

DWORD WINAPI os210_XSERVER_Terminate ( RESID    ResourceId )
{
  ROUTINE_DBG_MEO60 (_T("os210_XSERVER_Terminate"));

  return os210_Offline ( ResourceId, false );
}

/*---------------------------------------------------------------------------*/

DWORD WINAPI os210_SERVERDB_Terminate ( RESID    ResourceId )
{ 
  ROUTINE_DBG_MEO60 (_T("os210_SERVERDB_Terminate"));

  return os210_Offline ( ResourceId, false );
}


/*!--------------------------------------------------------------------------*
 * PUBLIC FUNCTION os210_XSERVER_LooksAlive, os210_SERVERDB_LooksAlive
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   LooksAlive routine for SERVERDB/XSERVER resources.
 * 
 * description:
 *   Perform a quick check to determine if the specified resource is probably
 *   online (available for use).  This call should not block for more than
 *   300 ms, preferably less than 50 ms.
 *
 *   LooksAlive should be a quick check to see if the resource is available 
 *   or not, whereas IsAlive should be a thorough check. Actually there
 *   are no differences between a quick check and a thorough check.
 * 
 * arguments:
 *   ResourceId          - [in]     Supplies the resource id for the resource 
 *                                  to polled.
 * return value:
 *   TRUE   - The specified resource is probably online and available for use.
 *   FALSE  - The specified resource is not functioning normally.
 *
 */      

BOOL WINAPI os210_XSERVER_LooksAlive ( RESID    ResourceId )
{
  ROUTINE_DBG_MEO60 (_T("os210_XSERVER_LooksAlive"));

  if ( os210_CheckIsAlive( ResourceId ) != ERROR_SUCCESS )
    return false;

  return true;
}

/*---------------------------------------------------------------------------*/

BOOL WINAPI os210_SERVERDB_LooksAlive ( RESID    ResourceId )
{ 
  ROUTINE_DBG_MEO60 (_T("os210_SERVERDB_LooksAlive"));

  if ( os210_CheckIsAlive( ResourceId ) != ERROR_SUCCESS )
    return false;

  return true;
}


/*!--------------------------------------------------------------------------*
 * PUBLIC FUNCTION os210_XSERVER_IsAlive, os210_SERVERDB_IsAlive
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   IsAlive routine for SERVERDB/XSERVER resources.
 * 
 * description:
 *   Perform a thorough check to determine if the specified resource is online
 *   (available for use). This call should not block for more than 400 ms,
 *   preferably less than 100 ms.
 * 
 * arguments:
 *   ResourceId          - [in]     Supplies the resource id for the resource 
 *                                  to polled.
 * return value:
 *   TRUE   - The specified resource is probably online and available for use.
 *   FALSE  - The specified resource is not functioning normally.
 *
 */      

BOOL WINAPI os210_XSERVER_IsAlive ( RESID    ResourceId )
{
  ROUTINE_DBG_MEO60 (_T("os210_XSERVER_IsAlive"));

  if ( os210_CheckIsAlive( ResourceId ) != ERROR_SUCCESS )
    return false;

  return true;
}

/*---------------------------------------------------------------------------*/

BOOL WINAPI os210_SERVERDB_IsAlive ( RESID    ResourceId )
{ 
  ROUTINE_DBG_MEO60 (_T("os210_SERVERDB_IsAlive"));

  if ( os210_CheckIsAlive( ResourceId ) != ERROR_SUCCESS )
    return false;

  return true;
}

/*!--------------------------------------------------------------------------*
 * PUBLIC FUNCTION os210_ResourceControl
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   ResourceControl routine for SERVERDB/XSERVER resources.
 * 
 * description:
 *   Perform the control request specified by ControlCode on the specified
 *   resource.
 * 
 * arguments:
 *   ResourceId          - [in]     Supplies the resource id for the resource 
 *                                  to polled.
 *   ControlCode         - [in]     Supplies the control code that defines the 
 *                                  action to be performed.
 *	 InBuffer            - [in]     Supplies a pointer to a buffer containing 
 *                                  input data.
 *   InBufferSize        - [in]     Supplies the size, in bytes, of the data 
 *                                  pointed to by InBuffer.
 *	 OutBuffer           - [out]    Supplies a pointer to the output buffer to 
 *                                  be filled in.
 *	 OutBufferSize       - [in]     Supplies the size, in bytes, of the 
 *                                  available space pointed to by OutBuffer.
 *	 BytesReturned       - [out]    Returns the number of bytes of OutBuffer 
 *                                  actually filled in by the resource. If 
 *                                  OutBuffer is too small, BytesReturned contains
 *                                  the total number of bytes for the operation
 *                                  to succeed.
 * return value:
 *   ERROR_SUCCESS             - The function completed successfully.
 *   ERROR_RESOURCE_NOT_FOUND  - RESID is not valid.
 *   ERROR_INVALID_FUNCTION    - The requested control code is not supported.
 *                               In some cases, this allows the cluster software
 *                               to perform the work.
 *   Win32 error code          - The function failed.
 *
 */      


DWORD ResourceTypeFoundCallback(  RESOURCE_HANDLE hOriginal,
                                  RESOURCE_HANDLE hResource,    
                                  PVOID           lpParams   )
{
  if ( ResUtilResourcesEqual(hOriginal, hResource ))
    return ERROR_DUP_NAME;

  return ERROR_SUCCESS;
}



DWORD WINAPI os210_ResourceControl ( RESID   ResourceId,
                                     DWORD   ControlCode,
                                     PVOID   InBuffer,
                                     DWORD   InBufferSize,
                                     PVOID   OutBuffer,
                                     DWORD   OutBufferSize,
                                     LPDWORD BytesReturned )
{
  DWORD                   Status         = ERROR_SUCCESS;
  t_Resource              *ResourceEntry = (t_Resource*)ResourceId;
  DWORD                   BufferSizeRequired;
  RESUTIL_PROPERTY_ITEM   *PrivateProperties;

  ROUTINE_DBG_MEO60 (_T("os210_ResourceControl"));

  Status = os210_CheckResId (ResourceId);

  if ( Status != ERROR_SUCCESS ) 
    return Status;

  LOG_VERBOSE(( ResourceEntry->ResourceHandle, LOG_INFORMATION, INFOMSG_CS_RES_CTRL_REQUEST _T("\n")));

  if ( ResourceEntry->ResourceType == XSERVER_e  )
    PrivateProperties = os210_XSERVER_PrivateProperties;
  else
    PrivateProperties = os210_SERVERDB_PrivateProperties;

  
  switch ( ControlCode ) 
  {
  case CLUSCTL_RESOURCE_UNKNOWN:
    LOG_VERBOSE(( ResourceEntry->ResourceHandle, LOG_INFORMATION, INFOMSG_CS_COMMON_MSG_TAG _T("Control Request: CLUSCTL_RESOURCE_UNKNOWN\n")));
    *BytesReturned = 0;
    Status = ERROR_SUCCESS;
    break;
/*
  case CLUSCTL_RESOURCE_GET_COMMON_PROPERTIES:

    if ( ResourceEntry->ResourceType == XSERVER_e  )
    {
      Status = ResUtilEnumResources( ResourceEntry->ResourceHandle, 
                                     RESOURCE_TYPE_XSERVER_OS200,
                                     ResourceTypeFoundCallback,
                                     NULL );

      if (( Status == ERROR_SUCCESS ) || ( Status == ERROR_NO_MORE_ITEMS  ))
      {
        *BytesReturned = 0;
        Status         = ERROR_INVALID_FUNCTION;
      }
    }
    else
    {
      *BytesReturned = 0;
      Status         = ERROR_INVALID_FUNCTION;
    }

    break;
*/

  case CLUSCTL_RESOURCE_ENUM_PRIVATE_PROPERTIES:
    LOG_VERBOSE(( ResourceEntry->ResourceHandle, LOG_INFORMATION, INFOMSG_CS_COMMON_MSG_TAG _T("Control Request: CLUSCTL_RESOURCE_ENUM_PRIVATE_PROPERTIES\n")));
    Status = ResUtilEnumProperties( PrivateProperties,
                                    OutBuffer,
                                    OutBufferSize,
                                    BytesReturned,
                                    &BufferSizeRequired );
    if ( Status == ERROR_MORE_DATA ) 
      *BytesReturned = BufferSizeRequired;

    break;
 
  case CLUSCTL_RESOURCE_GET_PRIVATE_PROPERTIES:
    LOG_VERBOSE(( ResourceEntry->ResourceHandle, LOG_INFORMATION, INFOMSG_CS_COMMON_MSG_TAG _T("Control Request: CLUSCTL_RESOURCE_GET_PRIVATE_PROPERTIES\n")));
    Status = ResUtilGetAllProperties( ResourceEntry->ParametersKey,
                                      PrivateProperties,
                                      OutBuffer,
                                      OutBufferSize,
                                      BytesReturned,
                                      &BufferSizeRequired );
    if ( Status == ERROR_MORE_DATA ) 
      *BytesReturned = BufferSizeRequired;

    break;
    
  case CLUSCTL_RESOURCE_VALIDATE_PRIVATE_PROPERTIES:
    LOG_VERBOSE(( ResourceEntry->ResourceHandle, LOG_INFORMATION, INFOMSG_CS_COMMON_MSG_TAG _T("Control Request: CLUSCTL_RESOURCE_VALIDATE_PRIVATE_PROPERTIES\n")));

    Status = os210_ValidatePrivResProp( PrivateProperties,
                                        ResourceEntry,
                                        InBuffer,
                                        InBufferSize,
                                        NULL );
    break;
    
  case CLUSCTL_RESOURCE_SET_PRIVATE_PROPERTIES:
    LOG_VERBOSE(( ResourceEntry->ResourceHandle, LOG_INFORMATION, INFOMSG_CS_COMMON_MSG_TAG _T("Control Request: CLUSCTL_RESOURCE_SET_PRIVATE_PROPERTIES\n") ));
    Status = os210_SetPrivResProp( PrivateProperties,
                                   ResourceEntry,
                                   InBuffer,
                                   InBufferSize );
    break;

  default:
    LOG_VERBOSE(( ResourceEntry->ResourceHandle, LOG_INFORMATION, INFOMSG_CS_COMMON_MSG_TAG _T("Control Request: %1!u!\n"), ControlCode ));
    *BytesReturned = 0;
    Status = ERROR_INVALID_FUNCTION;
    break;
  }
  
  return Status;
}


/*!--------------------------------------------------------------------------*
 * PUBLIC FUNCTION os210_ResourceTypeControl
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   ResourceTypeControl routine for SERVERDB/XSERVER resources.
 * 
 * description:
 *   Perform the control request specified by ControlCode.
 * 
 * arguments:
 *   ResourceTypeName    - [in]     Supplies the name of the resource type.
 *   ControlCode         - [in]     Supplies the control code that defines the 
 *                                  action to be performed.
 *	 InBuffer            - [in]     Supplies a pointer to a buffer containing 
 *                                  input data.
 *   InBufferSize        - [in]     Supplies the size, in bytes, of the data 
 *                                  pointed to by InBuffer.
 *	 OutBuffer           - [out]    Supplies a pointer to the output buffer to 
 *                                  be filled in.
 *	 OutBufferSize       - [in]     Supplies the size, in bytes, of the 
 *                                  available space pointed to by OutBuffer.
 *	 BytesReturned       - [out]    Returns the number of bytes of OutBuffer 
 *                                  actually filled in by the resource. If 
 *                                  OutBuffer is too small, BytesReturned contains
 *                                  the total number of bytes for the operation
 *                                  to succeed.
 * return value:
 *   ERROR_SUCCESS             - The function completed successfully.
 *   ERROR_RESOURCE_NOT_FOUND  - RESID is not valid.
 *   ERROR_INVALID_FUNCTION    - The requested control code is not supported.
 *                               In some cases, this allows the cluster software
 *                               to perform the work.
 *   Win32 error code          - The function failed.
 *
 */      

DWORD WINAPI os210_ResourceTypeControl ( LPCWSTR ResourceTypeName,
                                         DWORD   ControlCode,
                                         PVOID   InBuffer,
                                         DWORD   InBufferSize,
                                         PVOID   OutBuffer,
                                         DWORD   OutBufferSize,
                                         LPDWORD BytesReturned )
{
  DWORD                   Status         = ERROR_SUCCESS;
  DWORD                   BufferSizeRequired;
  RESUTIL_PROPERTY_ITEM   *PrivateProperties;

  ROUTINE_DBG_MEO60 (_T("os210_ResourceTypeControl"));

  switch ( ControlCode ) 
  {
  case CLUSCTL_RESOURCE_TYPE_UNKNOWN:
    *BytesReturned = 0;
    Status = ERROR_SUCCESS;
    break;

  case CLUSCTL_RESOURCE_TYPE_ENUM_PRIVATE_PROPERTIES:

    if ( os210_GetResourceType(ResourceTypeName) == XSERVER_e  )
      PrivateProperties = os210_XSERVER_PrivateProperties;
    else
      PrivateProperties = os210_SERVERDB_PrivateProperties;

    Status = ResUtilEnumProperties( PrivateProperties,
                                    OutBuffer,
                                    OutBufferSize,
                                    BytesReturned,
                                    &BufferSizeRequired );
    if ( Status == ERROR_MORE_DATA ) 
      *BytesReturned = BufferSizeRequired;

    break;
    
  default:
    Status = ERROR_INVALID_FUNCTION;
    break;
  }
  
  return Status;
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

/*---------------------------------------------------------------------------*
 * LOCAL FUNCTION os210_Open
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Open routine for SERVERDB/XSERVER resources.
 * 
 * description:
 *   View 'PUBLIC FUNCTION os210_XSERVER_Open, os210_SERVERDB_Open'
 * 
 * arguments:
 *   ResourceType        - [in]  The type of resource requesting an open.
 *   ResourceName        - [in]  Supplies the name of the resource to open.
 *   ResourceKey         - [in]  Supplies handle to the resource's cluster 
 *                               configuration database key.
 *   ResourceHandle      - [in]  A handle that is passed back to the resource 
 *                               monitor when the SetResourceStatus or LogEvent 
 *                               method is called. See the description of the 
 *                               SetResourceStatus and LogEvent methods on the
 *                               os210_Statup routine. This handle should never 
 *                               be closed or used for any purpose other than 
 *                               passing it as an argument back to the Resource 
 *                               Monitor in the SetResourceStatus or LogEvent
 *                               callback.
 * 
 * return value:
 *   RESID       - resource id of created resource.
 *   NULL        - on failure.
 * 
 */ 

static RESID os210_Open ( t_ResourceType              ResourceType,
                          LPCWSTR                     ResourceName,
                          HKEY                        ResourceKey,    
                          RESOURCE_HANDLE             ResourceHandle )
{
  DWORD                   Disposition;
  RESID                   ResID          = 0;
  DWORD                   Status         = ERROR_SUCCESS;
  HKEY                    ParametersKey  = NULL;
  t_Resource              *ResourceEntry = NULL;
  DWORD                   Size;
  RESUTIL_PROPERTY_ITEM   *PrivateProperties;
  LPWSTR                  NameOfPropInError = NULL;

  ROUTINE_DBG_MEO60 (_T("os210_Open"));

  if ( ResourceType == XSERVER_e  )
	(os210_LogEvent)( ResourceHandle, LOG_INFORMATION, INFOMSG_CS_COMMON_MSG_TAG _T("Open X_SERVER request.\n") );
  else
	(os210_LogEvent)( ResourceHandle, LOG_INFORMATION, INFOMSG_CS_COMMON_MSG_TAG _T("Open SERVERDB request.\n") );

  // --- Open the Parameters registry key for this resource.
  Status = ClusterRegCreateKey( ResourceKey,
                                _T("Parameters"),
                                0,
                                KEY_ALL_ACCESS,
                                NULL,
                                &ParametersKey,
                                &Disposition );
      
  if ( Status != ERROR_SUCCESS )
    (os210_LogEvent)(ResourceHandle, LOG_ERROR, ERRMSG_CS_OPEN_REG _T("\n"), Status );

  if ( Status == ERROR_SUCCESS )
  {
    Size = sizeof(t_Resource) + ((DWORD)_tcslen( ResourceName ) + 1) * sizeof(WCHAR);

    // ---  Allocate a resource entry.
    ResourceEntry = (t_Resource*) LocalAlloc( LMEM_FIXED, Size );

    if ( ResourceEntry == NULL ) 
    {
      Status = GetLastError();
      (os210_LogEvent)( ResourceHandle, LOG_ERROR, ERRMSG_CS_ALLOC_RESOURCE _T("\n"), Status );
    }
    else
    {
      // ---  Initialize the resource entry..
      ZeroMemory( ResourceEntry, sizeof(t_Resource) );
      
      ResourceEntry->ResId          = (RESID)ResourceEntry;   // - for validation
      ResourceEntry->ResourceHandle = ResourceHandle;
      ResourceEntry->ResourceKey    = ResourceKey;
      ResourceEntry->ParametersKey  = ParametersKey;
      ResourceEntry->ResourceType   = ResourceType;
      ResourceEntry->State          = ClusterResourceOffline;
      ResourceEntry->ResourceName   = (LPWSTR)(ResourceEntry + 1);
      ResourceEntry->StartTimeout   = START_TIME_OUT;

      _tcscpy( ResourceEntry->ResourceName, ResourceName );  // - Save the name of the resource.

      if ( ResourceType == XSERVER_e  )
          PrivateProperties = os210_XSERVER_PrivateProperties;
      else
          PrivateProperties = os210_SERVERDB_PrivateProperties;

      if ( Status == ERROR_SUCCESS ) 
          Status = ResUtilGetDwordValue( ResourceEntry->ParametersKey, REG_MSCS_PARAM_LOG_VERBOSE_OS00,
                                         &ResourceEntry->Params.LogVerbose, 0 );
      if ( Status == ERROR_SUCCESS ) 
      {
        // --- Read pending timeout parameter.
        Status = ResUtilGetDwordValue( ResourceEntry->ResourceKey, RES_STRT_STOP_PEND_TIMEOUT_OS00,
                                       &ResourceEntry->StartTimeout, START_TIME_OUT );
      }

      if ( Status != ERROR_SUCCESS ) 
      {
        (os210_LogEvent)( ResourceEntry->ResourceHandle, LOG_ERROR,
                          ERRMSG_CS_ERR_READ_PROPERTY _T("\n"),
                          (NameOfPropInError == NULL ? _T("") : NameOfPropInError),
                          Status );
      }
    }
  }

  ResID = (RESID)ResourceEntry;

  if ( ResourceEntry == NULL ) 
  {
    if ( ParametersKey != NULL ) 
      ClusterRegCloseKey( ParametersKey );
  }
  
  if ( Status != ERROR_SUCCESS ) 
    SetLastError( Status );

  return ResID;
}


/*---------------------------------------------------------------------------*
 * LOCAL FUNCTION os210_Close
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Close routine for SERVERDB/XSERVER resources.
 * 
 * description:
 *   View 'PUBLIC FUNCTION os210_XSERVER_Close, os210_SERVERDB_Close'
 * 
 * arguments:
 *   ResourceId          - [in]  Supplies the RESID of the resource to close.
 * 
 * return value:
 *   None.
 * 
 */ 

static void os210_Close ( RESID ResourceId )
{
  t_Resource          *ResourceEntry = (t_Resource*)ResourceId;
  DWORD               Status         = ERROR_SUCCESS;

  ROUTINE_DBG_MEO60 (_T("os210_Close"));
  
  Status = os210_CheckResId (ResourceId);

  if ( Status != ERROR_SUCCESS ) 
    return;
  
  LOG_VERBOSE(( ResourceEntry->ResourceHandle, LOG_INFORMATION, INFOMSG_CS_CLOSE_REQUEST _T("\n") ));
  
  // --- Close the Parameters key
  if ( ResourceEntry->ParametersKey ) 
    ClusterRegCloseKey( ResourceEntry->ParametersKey );
  
  if ( ResourceEntry->ResourceType == SERVERDB_e  )
  {
    // --- Deallocate the resource parameters.
    switch ( ResourceEntry->ResourceType )
    {
    case SERVERDB_e:
      if ( ResourceEntry->Params.SERVERDB.ServerDB != NULL )
        LocalFree( ResourceEntry->Params.SERVERDB.ServerDB );
      break;
    case XSERVER_e:
      break;
    }
  }
  LocalFree( ResourceEntry );

  return;
}

/*---------------------------------------------------------------------------*
 * LOCAL FUNCTION os210_Online
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Online routine for SERVERDB/XSERVER resources.
 * 
 * description:
 *   View 'PUBLIC FUNCTION os210_XSERVER_Online, os210_SERVERDB_Online'
 * 
 * arguments:
 *   ResourceId          - [in]      Supplies the resource id for the resource to 
 *                                   be brought online
 *   EventHandle         - [in/out]  Returns a signalable handle that is signaled 
 *                                   when the resource DLL detects a failure on the 
 *                                   resource. This argument is NULL on input, and 
 *                                   the resource DLL returns NULL if asynchronous 
 *                                   notification of failures is not supported, 
 *                                   otherwise this must bethe address of a handle 
 *                                   that is signaled on resource failures.
 * 
 * return value:
 *   ERROR_SUCCESS                - The operation was successful, and the resource 
 *                                  is now online.  
 *   
 *   ERROR_RESOURCE_NOT_FOUND     - RESID is not valid.  
 *   
 *   ERROR_RESOURCE_NOT_AVAILABLE - If the resource was arbitrated with some   
 *                                  other systems and one of the other systems   
 *                                  won the arbitration.  
 *   ERROR_IO_PENDING             - The request is pending, a thread has been 
 *                                  activated to process the online request. The 
 *                                  thread that is processing the online request 
 *                                  will periodically report Status by calling 
 *                                  the SetResourceStatus callback method, until 
 *                                  the resource is placed into the 
 *                                  ClusterResourceOnline state (or the resource 
 *                                  monitor decides to timeout the online request
 *                                  and Terminate the resource. This pending timeout
 *                                  value is settable and has a default value of 3 
 *                                  minutes.).
 *   Win32 error code             - The operation failed.
 *
 */      

static DWORD os210_Online ( RESID    ResourceId, 
                            PHANDLE  EventHandle )
{
  DWORD                   Status;
  t_Resource              *ResourceEntry = (t_Resource*)ResourceId;

  ROUTINE_DBG_MEO60 (_T("os210_Online"));
  
  Status = os210_CheckResId (ResourceId);

  if ( Status != ERROR_SUCCESS ) 
    return Status;
  
  ResourceEntry->State = ClusterResourceOffline;

  // --- terminate all pending online threads 
  ClusWorkerTerminate( &ResourceEntry->OnlineThread );

  // --- startup the thread that is processing the online request
  Status = ClusWorkerCreate( &ResourceEntry->OnlineThread,
                             os210_OnlineThread, ResourceEntry );
   
  if ( Status != ERROR_SUCCESS ) 
  {
    ResourceEntry->State = ClusterResourceFailed;
    (os210_LogEvent)( ResourceEntry->ResourceHandle, LOG_ERROR, 
                      ERRMSG_CS_START_WORKER_THRD _T("\n"), Status );
  }
  else 
  {
    Status = ERROR_IO_PENDING;
    LOG_VERBOSE(( ResourceEntry->ResourceHandle, LOG_INFORMATION, INFOMSG_CS_IO_PENDING _T("\n") ));
  }

  return Status;
}


/*---------------------------------------------------------------------------*
 * LOCAL FUNCTION os210_OnlineThread
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Online thread for SERVERDB/XSERVER resources.
 * 
 * description:
 *   Worker function which brings a resource from the resource table online.
 *   This function is executed in a separate thread.
 * 
 * arguments:
 *   WorkerPtr          - [in/out]   Supplies the worker structure
 *   ResourceEntry      - [in]       A pointer to the t_Resource block for
 *                                   this resource.
 *
 * return value:
 *   ERROR_SUCCESS      - The operation completed successfully.
 *   Win32 error code   - The operation failed.
 *
 */      

static DWORD WINAPI os210_OnlineThread ( PCLUS_WORKER      WorkerPtr,
                                         t_Resource        *ResourceEntry )
{
  RESOURCE_STATUS         ResourceStatus;
  RESUTIL_PROPERTY_ITEM   *PrivateProperties;
  SERVICE_STATUS          ssSeviceStatus;
  SC_HANDLE               hScM  = NULL;
  SC_HANDLE               hS    = NULL;
  TCHAR                   Options[20];
  LPWSTR                  NameOfPropInError = NULL;
  DWORD                   Status            = ERROR_SUCCESS;
  DWORD                   dwOptions         = MS_CLUSTER_SERVER_STARTUP;
  TCHAR                   *Args[1]          = { Options };
  DWORD                   NumOfArgs         = 1;
  DWORD                   Timeout           = ResourceEntry->StartTimeout;

  ROUTINE_DBG_MEO60 (_T("os210_OnlineThread"));

  ResourceStatus.ResourceState = ClusterResourceOnlinePending;
  ResourceStatus.WaitHint      = PENDING_SLEEP_TIME * 2;  // - msec
  ResourceStatus.CheckPoint    = 1;
  os210_SetResourceStatus( ResourceEntry->ResourceHandle, &ResourceStatus );

  if ( ResourceEntry->ResourceType == XSERVER_e  )
    PrivateProperties = os210_XSERVER_PrivateProperties;
  else
  {
    PrivateProperties = os210_SERVERDB_PrivateProperties;

    if ( ResourceEntry->Params.SERVERDB.ServerDB )
    {
        LocalFree( ResourceEntry->Params.SERVERDB.ServerDB );
        ResourceEntry->Params.SERVERDB.ServerDB = NULL;
    }
  }

  // --- Read private parameters.
  Status = ResUtilGetPropertiesToParameterBlock( ResourceEntry->ParametersKey,
                                                 PrivateProperties,
                                                 (LPBYTE) &ResourceEntry->Params,
                                                 true,             // - CheckForRequiredProperties
                                                 &NameOfPropInError );

  if ( Status == ERROR_SUCCESS ) 
  {
    LOG_VERBOSE(( ResourceEntry->ResourceHandle, LOG_INFORMATION, INFOMSG_CS_ONLINE_REQUEST _T("\n") ));

    switch ( ResourceEntry->ResourceType )
    {
    case SERVERDB_e:
        if ( ResourceEntry->Params.SERVERDB.ServerDB == NULL )
        {
            // 'ResUtilGetPropertiesToParameterBlock' interface error, workaround 
            ResUtilGetDwordValue( ResourceEntry->ParametersKey, REG_MSCS_PARAM_LOG_VERBOSE_OS00, &ResourceEntry->Params.LogVerbose, 0 );
            ResUtilGetDwordValue( ResourceEntry->ParametersKey, REG_MSCS_PARAM_AUTO_RES_SHT_OS00, &ResourceEntry->Params.SERVERDB.AutoRestartShutdown, 0 );
            ResourceEntry->Params.SERVERDB.ServerDB = ResUtilGetSzValue( ResourceEntry->ParametersKey, REG_MSCS_PARAM_SERVERDB_OS00 );
        }

        LOG_VERBOSE(( ResourceEntry->ResourceHandle, LOG_INFORMATION, INFOMSG_CS_COMMON_MSG_TAG _T("Get Properties - ParametersKey '%1!u!', AutoRestartShutdown '%2!u!', LogVerbose '%3!u!', ServerDB '%4'!\n"), 
                      &ResourceEntry->ParametersKey, ResourceEntry->Params.SERVERDB.AutoRestartShutdown, 
	                  ResourceEntry->Params.SERVERDB.LogVerbose, ResourceEntry->Params.SERVERDB.ServerDB  ));

       if ( ResourceEntry->Params.SERVERDB.ServerDB == NULL )
        {
            Status = ERROR_BAD_ARGUMENTS; // oops ...

            (os210_LogEvent)( ResourceEntry->ResourceHandle, LOG_ERROR,
                              ERRMSG_CS_ERR_READ_PROPERTY _T("\n"),
                              REG_MSCS_PARAM_SERVERDB_OS00,
                              Status );

        }
        else
        {
            // --- Build up service name
            _tcscpy(ResourceEntry->ServiceName, SERVICE_ID);
            _tcscat(ResourceEntry->ServiceName, ResourceEntry->Params.SERVERDB.ServerDB);

            if ( ResourceEntry->Params.SERVERDB.AutoRestartShutdown != 0 )
                dwOptions |= FORCE_AUTORESTART;

            if ( dwOptions != 0 ) 
                _ltow( dwOptions, Options, 10 );
        }

      break;
    case XSERVER_e:
      // 'ResUtilGetPropertiesToParameterBlock' interface error, workaround  
      ResUtilGetDwordValue( ResourceEntry->ParametersKey, REG_MSCS_PARAM_LOG_VERBOSE_OS00, &ResourceEntry->Params.LogVerbose, 0 );

      // --- Build up service name
      _tcscpy(ResourceEntry->ServiceName, SERVICE_ID_XSERVER);

      Options[0] = _T('\0');
      break;
    }

    if ( Status == ERROR_SUCCESS ) 
    {
      // --- Start the SERVERDB/XSERVER service
      if (!(hScM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT))         ||
          !(hS   = OpenService(hScM, ResourceEntry->ServiceName, 
                               SERVICE_START | SERVICE_QUERY_STATUS ))    ||
                               !StartService(hS, NumOfArgs, Args))
      {
        Status = GetLastError();
      
        if ( hS != NULL )
          CloseServiceHandle(hS);
        if ( hScM != NULL )
          CloseServiceHandle(hScM);
      }
      else
      {
        Timeout = ( Timeout * 1000 ) / PENDING_SLEEP_TIME;
        Status  = NO_ERROR;
      
        do
        {
          SleepEx( PENDING_SLEEP_TIME, false );
          Timeout--;
        
          if ( !QueryServiceStatus( hS, &ssSeviceStatus ))
            Status = GetLastError();
          else if ( ssSeviceStatus.dwCurrentState == SERVICE_RUNNING )
            break;
          else if (( ssSeviceStatus.dwCurrentState == SERVICE_STOPPED )       ||
                   ( ssSeviceStatus.dwCurrentState == SERVICE_STOP_PENDING )  ||
                   ( ssSeviceStatus.dwCurrentState == SERVICE_PAUSE_PENDING ) ||
                   ( ssSeviceStatus.dwCurrentState == SERVICE_PAUSED ))
          {
            Status = ERROR_SERVICE_NOT_ACTIVE;
            break;
          }
        
          ResourceStatus.ResourceState = ClusterResourceOnlinePending;
          ResourceStatus.WaitHint      = PENDING_SLEEP_TIME * 2;  // - msec
          ResourceStatus.CheckPoint++;
        
          os210_SetResourceStatus( ResourceEntry->ResourceHandle, &ResourceStatus );
        
          if ( Timeout == 0 )
            Status = ERROR_TIMEOUT;
        }
        while ( Status == NO_ERROR );
      
        CloseServiceHandle(hS);
        CloseServiceHandle(hScM);
      }
    
    
      if ( Status == ERROR_SUCCESS ) 
      { 
        ResourceEntry->State = ClusterResourceOnline;
      }
      else if ( Status == ERROR_SERVICE_ALREADY_RUNNING ) 
      {
        ResourceEntry->State = ClusterResourceOnline;
        Status               = ERROR_SUCCESS;
      }
      else 
      {
        ResourceEntry->State = ClusterResourceFailed;
      
        (os210_LogEvent)( ResourceEntry->ResourceHandle, LOG_ERROR, 
                          ERRMSG_CS_STARTING_RESOURCE _T("\n"), 
                          Status, ResourceEntry->ServiceName );
      }
    }
    else
    {
      ResourceEntry->State = ClusterResourceFailed;

      (os210_LogEvent)( ResourceEntry->ResourceHandle, LOG_ERROR,
                        ERRMSG_CS_ERR_READ_PROPERTY _T("\n"),
                        (NameOfPropInError == NULL ? _T("") : NameOfPropInError),
                        Status );
    }
  }


  ResourceStatus.ResourceState = ResourceEntry->State;
  ResourceStatus.WaitHint      = 0;
  ResourceStatus.CheckPoint++;
 
  os210_SetResourceStatus( ResourceEntry->ResourceHandle, &ResourceStatus );
  ResourceEntry->State = ResourceStatus.ResourceState;
  
  return Status;
}


/*---------------------------------------------------------------------------*
 * LOCAL FUNCTION os210_Offline
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Offline routine for SERVERDB/XSERVER resources.
 * 
 * description:
 *   View 'PUBLIC FUNCTION os210_XSERVER_Offline, os210_SERVERDB_Offline'
 * 
 * arguments:
 *   ResourceId         - [in]      Supplies the resource id for the resource to 
 *                                  be brought offline
 * return value:
 *   ERROR_SUCCESS                - The operation was successful, and the resource 
 *                                  is now offline.
 *   ERROR_RESOURCE_NOT_FOUND     - RESID is not valid.  
 *   Win32 error code             - The operation failed.
 */      

static DWORD os210_Offline ( RESID             ResourceId, 
                             bool              Gracefully )
{
  DWORD               Status;
  t_Resource          *ResourceEntry = (t_Resource*)ResourceId;
  
  ROUTINE_DBG_MEO60 (_T("os210_Offline"));

  Status = os210_CheckResId (ResourceId);

  if ( Status != ERROR_SUCCESS ) 
    return Status;

  // --- Resource already offline?
  if ( ResourceEntry->State == ClusterResourceOffline )
    return ERROR_SUCCESS;

  // --- Terminate all pending online threads 
  ClusWorkerTerminate( &ResourceEntry->OnlineThread );

  // NOTE: 'os210_XXXX_Offline' should try to shut the resource down gracefully, 
  //       whereas 'os210_XXXX_Terminate' must shut the resource down immediately.
  //       Actually there are no differences between a graceful shut down and an
  //       immediate shut down. 

  if ( Gracefully ) 
    LOG_VERBOSE(( ResourceEntry->ResourceHandle, LOG_INFORMATION, INFOMSG_CS_OFFLINE_REQUEST _T("\n") ));
  else
    LOG_VERBOSE(( ResourceEntry->ResourceHandle, LOG_INFORMATION, INFOMSG_CS_TERMINATE_REQUEST _T("\n") ));

  // --- Stop the SERVERDB/XSERVER service
  Status = ResUtilStopResourceService( ResourceEntry->ServiceName );
  
  if ( Status != ERROR_SUCCESS ) 
  {
    (os210_LogEvent)( ResourceEntry->ResourceHandle, LOG_ERROR, 
                      ERRMSG_CS_STOPPING_RESOURCE _T("\n"), 
					  Status, ResourceEntry->ServiceName );
  }

  ResourceEntry->State = ClusterResourceOffline;

  return Status;
}

/*---------------------------------------------------------------------------*
 * LOCAL FUNCTION os210_CheckIsAlive
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Check to see if the resource is alive for SERVERDB/XSERVER.
 * 
 * description:
 *   View 'PUBLIC FUNCTION os210_XSERVER_CheckIsAlive, os210_SERVERDB_CheckIsAlive'
 *   
 * 
 * arguments:
 *   ResourceEntry      - [in]      A pointer to the 't_Resource' block for
 *                                  this resource.
 *
 * return value:
 *   ERROR_SUCCESS             - The specified resource is online and functioning normally.
 *   ERROR_SERVICE_NOT_ACTIVE  - The specified resource is not functioning normally.
 *   ERROR_RESOURCE_NOT_FOUND  - RESID is not valid.  
 *
 */      

static DWORD os210_CheckIsAlive ( RESID             ResourceId )
{
  DWORD               Status;
  t_Resource          *ResourceEntry = (t_Resource*)ResourceId;

  ROUTINE_DBG_MEO60 (_T("os210_CheckIsAlive"));

  Status = os210_CheckResId (ResourceId);

  if ( Status != ERROR_SUCCESS ) 
    return Status;

  // --- Verify the SERVERDB/XSERVER service
  Status = ResUtilVerifyResourceService( ResourceEntry->ServiceName );
  
  if ( Status != ERROR_SUCCESS ) 
    return ERROR_SERVICE_NOT_ACTIVE;

  return Status;
}

/*---------------------------------------------------------------------------*
 * LOCAL FUNCTION os210_ValidatePrivResProp
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Processes the CLUSCTL_RESOURCE_VALIDATE_PRIVATE_PROPERTIES control
 *   function for resources of type SERVERDB/XSERVER.
 * 
 * description:
 *   
 * arguments:
 *   PrivateProperties  - [in]      Pointer to the private properties.
 *   ResourceEntry      - [in/out]  A pointer to the 't_Resource' block for
 *                                  this resource.
 *	 InBuffer           - [in]      Supplies a pointer to a buffer containing 
 *                                  input data.
 *   InBufferSize       - [in]      Supplies the size, in bytes, of the data 
 *                                  pointed to by InBuffer.
 *
 * return value:
 *   ERROR_SUCCESS             - An error occurred allocating memory.
 *   ERROR_INVALID_PARAMETER   - The data is formatted incorrectly.
 *   ERROR_NOT_ENOUGH_MEMORY   - An error occurred allocating memory.
 *   Win32 error code          - The function failed.
 */      

static DWORD os210_ValidatePrivResProp ( RESUTIL_PROPERTY_ITEM  *PrivateProperties,
                                         t_Resource             *ResourceEntry,
                                         PVOID                  InBuffer,
                                         DWORD                  InBufferSize,
                                         t_ResourceParams       *ResourceParams )
{
  DWORD                 Status;
  t_ResourceParams      ResourceParameterBlock;
  t_ResourceParams      *ResParamBlock;
  TCHAR                 ServiceName[100];

  ROUTINE_DBG_MEO60 (_T("os210_ValidatePrivResProp"));

  // --- Check if there is input data.
  if ( (InBuffer == NULL) || (InBufferSize < sizeof(DWORD)) )
    return ERROR_INVALID_DATA;

  if ( ResourceParams == NULL )
    ResParamBlock = &ResourceParameterBlock;
  else
    ResParamBlock = ResourceParams;

  // --- Duplicate the resource parameter block.
  ZeroMemory( ResParamBlock, sizeof(*ResParamBlock) );
  Status = ResUtilDupParameterBlock( (LPBYTE) ResParamBlock,
                                     (LPBYTE) &ResourceEntry->Params,
                                     PrivateProperties );
  if ( Status != ERROR_SUCCESS )
    return Status;

  Status = ResUtilVerifyPrivatePropertyList( InBuffer, InBufferSize );

  if ( Status != ERROR_SUCCESS )
    return Status;

  // --- Parse and validate the properties.
  Status = ResUtilVerifyPropertyTable( PrivateProperties,
                                       NULL,
                                       TRUE, // AllowUnknownProperties
                                       InBuffer,
                                       InBufferSize,
                                       (LPBYTE) ResParamBlock );

  if ( Status == ERROR_SUCCESS ) 
  {
    // --- Validate the parameter values.
    switch ( ResourceEntry->ResourceType )
    {
    case SERVERDB_e:
        if ( ResParamBlock->SERVERDB.ServerDB != NULL )
        {
            _tcscpy(ServiceName, SERVICE_ID);
            _tcscat(ServiceName, ResParamBlock->SERVERDB.ServerDB);

            Status = ResUtilVerifyResourceService( ServiceName );

            if ( Status == ERROR_SERVICE_DOES_NOT_EXIST ) 
                Status = ERROR_INVALID_PARAMETER;
            else
                Status = ERROR_SUCCESS;
        }
      break;

    case XSERVER_e:
      //  !!!!!!!   Code to validate interactions between parameters goes here. !!!!!!
      // Status = ERROR_INVALID_PARAMETER;
      break;
    }
  }

  if ( ResParamBlock == &ResourceParameterBlock )
  {
    // --- Cleanup our parameter block.
    ResUtilFreeParameterBlock( (LPBYTE) ResParamBlock,
                               (LPBYTE) &ResourceEntry->Params,
                               PrivateProperties );
  }

  return Status;
}

/*---------------------------------------------------------------------------*
 * LOCAL FUNCTION os210_SetPrivResProp
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Processes the CLUSCTL_RESOURCE_SET_PRIVATE_PROPERTIES control
 *   function for resources of type SERVERDB/XSERVER.
 * 
 * description:
 *   
 * arguments:
 *   PrivateProperties  - [in]      Pointer to the private properties.
 *   ResourceEntry      - [in/out]  A pointer to the 't_Resource' block for
 *                                  this resource.
 *	 InBuffer           - [in]      Supplies a pointer to a buffer containing 
 *                                  input data.
 *   InBufferSize       - [in]      Supplies the size, in bytes, of the data 
 *                                  pointed to by InBuffer.
 *
 * return value:
 *   ERROR_SUCCESS             - An error occurred allocating memory.
 *   ERROR_INVALID_PARAMETER   - The data is formatted incorrectly.
 *   ERROR_NOT_ENOUGH_MEMORY   - An error occurred allocating memory.
 *   Win32 error code          - The function failed.
 */      

static DWORD os210_SetPrivResProp ( RESUTIL_PROPERTY_ITEM  *PrivateProperties,
                                    t_Resource             *ResourceEntry,
                                    PVOID                  InBuffer,
                                    DWORD                  InBufferSize )
{
  DWORD                 Status;
  t_ResourceParams      ResourceParameterBlock;

  ROUTINE_DBG_MEO60 (_T("os210_SetPrivResProp"));

  // --- Parse the properties so they can be validated together.
  //     This routine does individual property validation.
  Status = os210_ValidatePrivResProp( PrivateProperties, ResourceEntry, 
                                      InBuffer, InBufferSize, &ResourceParameterBlock );

  if ( Status != ERROR_SUCCESS ) 
  {
    ResUtilFreeParameterBlock( (LPBYTE) &ResourceParameterBlock,
                               (LPBYTE) &ResourceEntry->Params,
                               PrivateProperties );

    return Status;
  }
 
  // --- Save the parameter values.
  Status = ResUtilSetPropertyParameterBlock( ResourceEntry->ParametersKey,
                                             PrivateProperties,
                                             NULL,
                                             (LPBYTE) &ResourceParameterBlock,
                                             InBuffer,
                                             InBufferSize,
                                             (LPBYTE) &ResourceEntry->Params );

  ResUtilFreeParameterBlock( (LPBYTE) &ResourceParameterBlock,
                             (LPBYTE) &ResourceEntry->Params,
                             PrivateProperties );

  // --- If the resource is online, return a non-success status.
  if ( Status == ERROR_SUCCESS ) 
  {
    if ( ResourceEntry->State == ClusterResourceOnline ) 
      Status = ERROR_RESOURCE_PROPERTIES_STORED;
    else if ( ResourceEntry->State == ClusterResourceOnlinePending ) 
      Status = ERROR_RESOURCE_PROPERTIES_STORED;
    else 
      Status = ERROR_SUCCESS;
  }


  return Status;
}

/*---------------------------------------------------------------------------*
 * LOCAL FUNCTION os210_CheckResourceID
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Check routine for the 'ResourceId'
 * 
 * description:
 *   This routine verifies that the 'ResourceId' passed in is valid. 
 * 
 * arguments:
 *   ResourceId         - [in]       The resource id which has to be verified.
 *
 * return value:
 *   ERROR_SUCCESS                - RESID is valid.
 *   ERROR_RESOURCE_NOT_FOUND     - RESID is not valid.  
 *   
 */      

static DWORD os210_CheckResId ( RESID       ResourceId )
{
  t_Resource          *ResourceEntry = (t_Resource*)ResourceId;

  ROUTINE_DBG_MEO60 (_T("os210_CheckResId"));

  if ( ResourceEntry == NULL ) 
  {
    (os210_LogEvent)(ResourceEntry->ResourceHandle, LOG_ERROR, 
                     ERRMSG_CS_NONEXIST_REQUEST _T("\n"), ResourceId);
    return ERROR_RESOURCE_NOT_FOUND;
  }
  
  // --- Check the resource id!
  if ( ResourceEntry->ResId != ResourceId ) 
  {
    (os210_LogEvent)(ResourceEntry->ResourceHandle, LOG_ERROR, 
                     ERRMSG_CS_SANITY_CHECK_FAILED _T("\n"), ResourceId);
    return ERROR_RESOURCE_NOT_FOUND;
  }

  return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*
 * LOCAL FUNCTION os210_GetResourceType
 *---------------------------------------------------------------------------*
 * 
 * abstract: 
 *   Supplies the resource type.
 * 
 * description:
 *   The ResourceTypeName is passed in. Corresponding to that name it passes back 
 *   the resource type value.
 * 
 * arguments:
 *   ResourceTypeName    - [in]  The type of resource requesting a function 
 *                               table.
 * return value:
 *   UNKNOWN_e                - The resource type is unknown.
 *   XSERVER_e                - XSERVER.
 *   SERVERDB_e               - SERVERDB.
 *   
 */      

static t_ResourceType os210_GetResourceType ( LPCWSTR    ResourceTypeName )
{
 
  ROUTINE_DBG_MEO60 (_T("os210_GetResourceType"));

  if ( _tcsicmp( ResourceTypeName, RESOURCE_TYPE_SERVERDB_OS00 ) == 0 )
    return SERVERDB_e;
  else if ( _tcsicmp( ResourceTypeName, RESOURCE_TYPE_XSERVER_OS00 ) == 0 ) 
    return XSERVER_e;

  return UNKNOWN_e;
} 

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
