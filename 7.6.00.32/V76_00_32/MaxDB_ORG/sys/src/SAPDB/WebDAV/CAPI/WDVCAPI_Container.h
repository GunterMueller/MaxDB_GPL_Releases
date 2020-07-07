/*!**********************************************************************

  module: WDVCAPI_Container.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  WebDAV Container Management

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

#ifndef WDVCAPI_CONTAINER_H
#define WDVCAPI_CONTAINER_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Types

 ***********************************************************************/

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Container_CreateHandle( WDVCAPI_WDV              wdv,
                                 WDVCAPI_ContainerHandle *hContainer );

WDV_Bool Container_DestroyHandle( WDVCAPI_WDV             wdv,
                                  WDVCAPI_ContainerHandle hContainer );

WDV_Bool Container_DestroyHandleFreeList( WDVCAPI_WDV              wdv,
                                          WDVCAPI_ContainerHandle *hFreeList );

WDV_Bool Container_Delete( WDVCAPI_WDV  wdv,
                           WDVCAPI_Id   id );
    
WDV_Bool Container_OpenToWrite( WDVCAPI_WDV               wdv,
                                WDVCAPI_ContainerHandle   hContainer,
                                WDVCAPI_Id                contentId );

WDV_Bool Container_Write( WDVCAPI_WDV               wdv,
                          WDVCAPI_ContainerHandle   hContainer,
                          WDVCAPI_Buffer            buffer,
                          WDVCAPI_BufferLength      bufferLength );

WDV_Bool Container_CloseWrite( WDVCAPI_WDV              wdv,
                               WDVCAPI_ContainerHandle  hContainer );
    
WDV_Bool Container_OpenToRead( WDVCAPI_WDV              wdv,
                               WDVCAPI_ContainerHandle  hContainer,
                               WDVCAPI_Id               contentId );

WDV_Bool Container_Read( WDVCAPI_WDV                wdv,
                         WDVCAPI_ContainerHandle    hContainer,
                         WDV_Bool                  *endOfContent );

WDV_Bool Container_CloseRead( WDVCAPI_WDV               wdv,
                              WDVCAPI_ContainerHandle   hContainer );

/***********************************************************************

	End

 ***********************************************************************/

#endif
