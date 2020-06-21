/****************************************************************************

  module      : FrameCtrl_Wrapper.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: FrameCtrl_Wrapper
  description : 


  last changed: 2001-02-06  12:36
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gsp03.h"
#include "gbd00.h"
#include "hgg01_1.h"

#include "FrameControl/FrameCtrl_Manager.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "FrameControl/FrameCtrl_IDataCache.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define ALLOCATE_FAILED_MSG1 "FrameControl: bad alloc "


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 bd999FramesAvailable()
{
    return( FrameCtrl_ICommon::GetInstance().GetFreeFrameCount() );
};

/*---------------------------------------------------------------------------*/

externC tbd_nodeptr bd999GetFrame( tsp00_TaskId TaskId )
{
    const SAPDB_UInt        pageSize  = FrameCtrl_ICommon::GetInstance().GetPageSize();
    Data_PageFrame pageFrame = FrameCtrl_IDataCache::GetInstance().NewDataFrame( TaskId );

    return( reinterpret_cast<tbd_nodeptr> (pageFrame.GetPointer( 0, pageSize )));
};

/*---------------------------------------------------------------------------*/

externC void bd999ReleaseFrame(
                               tsp00_TaskId TaskId,
                               tbd_nodeptr  pNode )
{
    const SAPDB_UInt        pageSize = FrameCtrl_ICommon::GetInstance().GetPageSize();
    Data_PageFrame pageFrame( pNode, pageSize );

    FrameCtrl_IDataCache::GetInstance().FreeDataFrame( TaskId, pageFrame );
};

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 bd999GetPageSize()
{
    return( FrameCtrl_ICommon::GetInstance().GetPageSize());
};

/*---------------------------------------------------------------------------*/

externC tsp00_Int4 bd999GetCacheSize()
{
    return( FrameCtrl_ICommon::GetInstance().GetCacheSize());
};


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
