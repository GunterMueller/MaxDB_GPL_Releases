/* @lastChanged: "1998-10-06  15:03"
 * @filename:    ven38.cpp
 * @purpose:     "COM_Interface"
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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(NMP)
#define RWSTD_NO_NAMESPACE  1
#include <sys/user.h>
#endif

#include "gsp00.h"
#include "heo01.h"
#include "heo02x.h"
#include "heo00.h" //
#include "Oms/OMS_Defines.h"
#include "geo00d.h"
#include "hak341.h"
#include "geo38.h"
#include "hen38.h"

#include "heo52.h"
#include "vsp002c.h"
#include "heo46.h"
#include "geo60.h"
#include "heo60.h"
#include "geo50_0.h"
#include "gen500.h"
#include "geo60.h"
#include "heo38.h"
#include "geo573.h" // PTS 1112369
#include "liveCache/LVC_Dispatcher.hpp" // for DCOM memory
#include "liveCache/LVC_LoadLibrary.hpp"

 /*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	inline
#endif

/*---------------------------------------------------------------------------*/

//externC int en38_Eval_Exception ( tsp_dcom_dispatch_parms * parms)
//{
//  IID loc_iid;
//  IUnknown                *lpIUnknown;
//  IClassFactory           *lpIFactory;
//  LVC_DispatcherCOMClassHash           *HashPtr;
//  LVC_DispatcherHashEntryPtr           currEntry;
//  LVC_DispatcherHashEntryPtr           currSessEntry;
//  IliveCacheSink*         pIliveCacheSink;
//  char                    msgBuffer[132];
//  parms->hresult = GEO00D_DCOM_DBPROC_CRASHED;
//  if (parms->session_context != NULL )
//  {
//    tsp00_C4          session;
//    tsp1_packet*      SQLBuffer_ptr;
//    CIliveCacheSink*  pIliveCacheSink;
//    ak341GetDebugContext ((tak_all_command_glob*)parms->session_context,
//        *REINTERPRET_CAST(tgg91_SessionNo*, &session),
//        *REINTERPRET_CAST(tsp00_Addr*, &pIliveCacheSink),
//        SQLBuffer_ptr );
//    if ( !pIliveCacheSink->CallbackIsActive)
//      parms->hresult = -1; // stay alive
//  }
//  MSGD ((ERR_DCOM_HRESULT, "eo38VDcomDispatch", parms->hresult ));
//  lpIUnknown = (IUnknown *) parms->idispatch;
//  lpIFactory = (IClassFactory *) parms->ifactory;
//  
//  RTESync_LockedScope lck(LVC_Dispatcher::getDCOMMemory().lExclusive);
//  HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;
//  if ( NULL != HashPtr ) 
//  {
//    /* Release session related debugger                        */
//    HashPtr->ResetEntrySearch( 0, LVC_DispatcherCOMClassHash::SearchALL );
//    currEntry = HashPtr->NextEntry();
//    while ( NULL != currEntry)
//    {
//      if ((currEntry->getIUnknown() == lpIUnknown) ||
//        (currEntry->getIUnknown() == lpIFactory))
//      {
//        SAPDB_memcpy (&loc_iid.Data1, currEntry->getGuid(), sizeof (loc_iid));
//        if (currEntry->getIUnknown() == lpIUnknown)
//        {
//          MSGD ((ERR_DCOM_COMETHOD_CRASHED,
//            loc_iid.Data1, loc_iid.Data2, loc_iid.Data3,
//            loc_iid.Data4[0], loc_iid.Data4[1], loc_iid.Data4[2],
//            loc_iid.Data4[3], loc_iid.Data4[4], loc_iid.Data4[5],
//            loc_iid.Data4[6], loc_iid.Data4[7], parms->dispid));
//        }
//        else
//        {
//          MSGD ((ERR_DCOM_DBPROC_CRASHED,
//            loc_iid.Data1, loc_iid.Data2, loc_iid.Data3,
//            loc_iid.Data4[0], loc_iid.Data4[1], loc_iid.Data4[2],
//            loc_iid.Data4[3], loc_iid.Data4[4], loc_iid.Data4[5],
//            loc_iid.Data4[6], loc_iid.Data4[7], parms->dispid));
//        }
//        break;
//      }
//      currEntry = HashPtr->NextEntry(); 
//    }
//  }
//  return 0;
//}
