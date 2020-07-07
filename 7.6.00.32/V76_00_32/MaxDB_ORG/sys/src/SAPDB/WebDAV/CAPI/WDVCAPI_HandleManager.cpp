/*!**********************************************************************

  module: WDVCAPI_Handle Manager.cpp

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Handle Manager

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2000-2005 SAP AG




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




************************************************************************/



/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

#include "RunTime/RTE_HandleManager.hpp"

#include "WebDAV/CAPI/WDVCAPI_HandleManager.h"

/***********************************************************************

	Variables

 ***********************************************************************/

RTE_HandleManager   *handleManager;

/***********************************************************************

	API Functions

 ***********************************************************************/

extern "C" void   WDVCAPI_InitHandleManager()
{

    /* Init Handle Manager */
    handleManager = new RTE_HandleManager();

}

/***********************************************************************

	Functions

 ***********************************************************************/

extern "C" void HdlMan_NewHandle( WDVCAPI_Handle   *handle,
                                  void             *pHandleData )
{

    if (!handleManager || !handle) {
        return;
    }

    handleManager->NewHandle(static_cast <RTE_HandleManager::Hdl &> (*handle), pHandleData);

}

/*===========================================================================*/

extern "C" void HdlMan_LeaseHandleData( WDVCAPI_Handle   handle,
                                        void           **ppHandleData )
{

    if (!handleManager) {
        return;
    }

    handleManager->LeaseHandleData(handle, ppHandleData);

}

/*===========================================================================*/

extern "C" void HdlMan_ReleaseHandleData( WDVCAPI_Handle   handle )
{

    if (!handleManager) {
        return;
    }

    handleManager->ReleaseHandleData(handle);

}

/*===========================================================================*/

extern "C" void HdlMan_FreeHandle( WDVCAPI_Handle  handle )
{

    if (!handleManager) {
        return;
    }

    handleManager->FreeHandle(handle);

}

/*===========================================================================*/

/***********************************************************************

	End

 ***********************************************************************/
