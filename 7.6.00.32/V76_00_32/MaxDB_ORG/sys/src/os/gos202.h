/*!
  @file           gos202.h
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



#ifndef GOS202_H
#define GOS202_H

#if defined(CREATE_DLL) && defined (CREATE_DLL_LOADER)
#  error "'CREATE_DLL' and 'CREATE_DLL_LOADER' can't be defined together!"
#endif

#if !defined (CREATE_DLL) && !defined (CREATE_DLL_LOADER) 
# define CREATE_DLL_LOADER
#endif

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gos203.h"
#include "geo200.h"
#include "geo60.h"
#include "geo007_1.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if !defined(externC)
# define externC  extern "C"
#endif


#define   EVTOPT_PRINT_ON_OS202                    0x01
#define   EVTOPT_VERBOSE_OS202                     0x02
#define   EVTOPT_DIALOG_BOX_OS202                  0x04
#define   EVTOPT_THROW_EVT_OS202                   0x08
#define   EVTOPT_DLL_LOAD_ERROR_OFF_OS202          0x10

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


MK_CLASS ( tos202_CluManDll )
{
# if defined(CREATE_DLL)
/*-----------------------------------------------*/
/*-------------------- D L L --------------------*/
/*-----------------------------------------------*/

 public:
  tos202_CluManDll ();

 private:
   typedef enum 
   {
     online_request_eos202,
     offline_request_eos202
   } tos202_RequestType;

   bool          os202_OnlineOfflineXSERVERResource  ( const tos202_RequestType  Request );
   bool          os202_OnlineOfflineSERVERDBResource ( LPCSTR                    SERVERDBName,
                                                       const tos202_RequestType  Request );
   bool          os202_OnlineOfflineGroup            ( LPCSTR                    GroupName,
                                                       DWORD                     Timeout,
                                                       const tos202_RequestType  Request );
   bool          os202_FindResource                  ( tos204_Cluster&           Cluster,
                                                       tos204_ClusterResource&   Resource,
                                                       LPCTSTR                   ResourceTypeName,
                                                       LPCTSTR                   ValueName    = NULL,
                                                       LPCTSTR                   Value        = NULL );
   bool          os202_FindResourceType              ( tos204_Cluster&           Cluster,
                                                       LPCTSTR                   ResourceTypeName );

   bool          os202_SetDependenciesToDrives       ( tos204_Cluster&           Cluster,
                                                       tos204_ClusterResource&   ReqResource,
                                                       LPCTSTR                   ReqResourceGroupNameStr );
 private:
   unsigned      os202_EvtOptions;

# else

/*-----------------------------------------------*/
/*------------- D L L - L O A D E R -------------*/
/*-----------------------------------------------*/

 public:
   tos202_CluManDll ( unsigned     EvtOptions = EVTOPT_PRINT_ON_OS202 );

   static const bool    os202_ClusterServerInsalled                            ();
# endif

/*-----------------------------------------------*/
/*----------------- C O M M O N -----------------*/
/*-----------------------------------------------*/

public:
   ~tos202_CluManDll()                   {;}; 


   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_SetEvtOptions,            ( unsigned    Options    /*  = 0 PTS 1129537 */,
                                                                                 bool        AddOptions /*  = false PTS 1129537 */ ));
   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_BringGroupOnline,         ( DWORD       Timeout    /*  = 0 PTS 1129537 */,
                                                                                 LPCSTR      GroupName  /*  = NULL PTS 1129537 */ ));
   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_BringGroupOffline,        ( DWORD       Timeout    /*  = 0 PTS 1129537 */,
                                                                                 LPCSTR      GroupName  /*  = NULL PTS 1129537 */ ));
   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_CreateResourceTypes,      ());
   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_RemoveResourceTypes,      ());

   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_BuildUpGroup,             ( LPCSTR      NetworkName,
                                                                                 LPCSTR      IPAddress,
                                                                                 LPCSTR      SubnetMask,
                                                                                 LPCSTR      DriveName ));
   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_MoveDriveToGroup,         ( LPCSTR      DriveName ));
   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_AddSERVERDBResource,      ( LPCSTR      ServerDB, 
                                                                                 LPCSTR      Remark/* = NULL PTS 1129537 */ ));
   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_RemoveSERVERDBResource,   ( LPCSTR      SERVERDBName,
                                                                                 DWORD       OffLineTimeOut/* = 60 PTS 1129537 */ ));

   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_RemoveSAPDBResources,     ());

   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_OnlineXSERVERResource,    ());
   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_OfflineXSERVERResource,   ());

   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_OnlineSERVERDBResource,   ( LPCSTR      ServerDB ));
   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_OfflineSERVERDBResource,  ( LPCSTR      ServerDB ));

   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_EnableDisableClusterLog,  ( const DWORD MBLogSize,
                                                                                 LPCSTR      LogName   /*= NULL PTS 1129537 */ ));

   MK_PTYPE_OR_FUNCPTR_OS203(bool, DMOD_OS203, os202_RegisterUnregisterDebugEx,( const bool  Register ));
   
private:
   void          os202_HandleEvt                                               ( teo200_EventList &   EventList,
                                                                                 unsigned             Options );
};
 

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GOS202_H */
