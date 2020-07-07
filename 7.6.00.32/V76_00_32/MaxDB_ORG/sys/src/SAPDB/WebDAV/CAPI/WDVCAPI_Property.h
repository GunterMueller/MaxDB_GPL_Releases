/*!**********************************************************************

  module: WDVCAPI_Property.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  WebDAV Property Management

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

#ifndef WDVCAPI_PROPERTY_H
#define WDVCAPI_PROPERTY_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include <WDVCAPI_Definition.h>

#include "SAPDB/XML/XPathEngine/XMLXPath_Interface.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Indexing.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define PROPERTY_VALUE_COLLECTION               "collection"
#define PROPERTY_VALUE_SAPDBWWW_DIRECTORY       "sapdbwww/directory"

/***********************************************************************

	Types

 ***********************************************************************/

typedef struct st_property_handle_set_xml_index         *WDVCAPI_PropertyHandleSetXmlIndex;
typedef struct st_property_handle_set_short_value       *WDVCAPI_PropertyHandleSetShortValue;
typedef struct st_property_handle_get_short_value       *WDVCAPI_PropertyHandleGetShortValue;
typedef struct st_property_handle_update_short_value    *WDVCAPI_PropertyHandleUpdateShortValue;
typedef struct st_property_handle_set_last_modified     *WDVCAPI_PropertyHandleSetLastModified;
typedef struct st_property_handle_set_creation_date     *WDVCAPI_PropertyHandleSetCreationDate;

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool Property_CreateHandle( WDVCAPI_WDV             wdv,
                                WDVCAPI_PropertyHandle *hProperty );

WDV_Bool Property_DestroyHandle( WDVCAPI_WDV            wdv,
                                 WDVCAPI_PropertyHandle hProperty );

WDV_Bool Property_CreateHandleSetXmlIndex( WDVCAPI_WDV                         wdv,
                                           WDVCAPI_PropertyHandleSetXmlIndex  *hPropertySetXmlIndex );

WDV_Bool Property_DestroyHandleSetXmlIndex( WDVCAPI_WDV                        wdv,
                                            WDVCAPI_PropertyHandleSetXmlIndex  hPropertySetXmlIndex );

WDV_Bool Property_CreateHandleSetShortValue( WDVCAPI_WDV                            wdv,
                                             WDVCAPI_PropertyHandleSetShortValue   *hPropertySetShortValue);

WDV_Bool Property_CreateHandleGetShortValue( WDVCAPI_WDV                            wdv,
                                             WDVCAPI_PropertyHandleGetShortValue   *hPropertyGetShortValue);

WDV_Bool Property_DestroyHandleSetShortValue( WDVCAPI_WDV                           wdv,
                                              WDVCAPI_PropertyHandleSetShortValue   hPropertySetShortValue );

WDV_Bool Property_DestroyHandleGetShortValue( WDVCAPI_WDV                           wdv,
                                              WDVCAPI_PropertyHandleGetShortValue   hPropertyGetShortValue );

WDV_Bool Property_CreateHandleUpdateShortValue( WDVCAPI_WDV                               wdv,
                                                WDVCAPI_PropertyHandleUpdateShortValue   *hPropertyUpdateShortValue);

WDV_Bool Property_DestroyHandleUpdateShortValue( WDVCAPI_WDV                              wdv,
                                                 WDVCAPI_PropertyHandleUpdateShortValue   hPropertyUpdateShortValue );

WDV_Bool Property_CreateHandleSetLastModified( WDVCAPI_WDV                            wdv,
                                               WDVCAPI_PropertyHandleSetLastModified *hPropertySetLastModified );

WDV_Bool Property_DestroyHandleSetLastModified( WDVCAPI_WDV                           wdv,
                                                WDVCAPI_PropertyHandleSetLastModified hPropertySetLastModified );

WDV_Bool Property_DeleteAll( WDVCAPI_WDV    wdv,
                             WDVCAPI_Id     cId,
                             WDV_Bool       withDisplayName );

WDV_Bool Property_DeleteAllIndexValues ( WDVCAPI_WDV    wdv,
                                         WDVCAPI_Id     resourceId );

WDV_Bool Property_CopyAll( WDVCAPI_WDV             wdv,
                           WDVCAPI_PropertyHandle  hProperty,
                           WDVCAPI_Id              sourceId,
                           WDVCAPI_Id              destinationId,
                           WDV_Bool                withDisplayName );

WDV_Bool Property_GetShortValue( WDVCAPI_WDV                wdv,
                                 WDVCAPI_Id                 resourceId,
                                 WDVCAPI_Id                 propertyId,
                                 WDVCAPI_PropertyShortValue propertyShortValue );

WDV_Bool Property_GetDescriptionByShortValue( WDVCAPI_WDV                wdv,
                                              WDVCAPI_PropertyShortValue propertyShortValue,
                                              WDVCAPI_Id                 cId,
                                              WDVCAPI_Id                 propertyId );

WDV_Bool Property_GetResourceIdByShortValueAndPropertyId( WDVCAPI_WDV                wdv,
                                                          WDVCAPI_Id                 propertyId,
                                                          WDVCAPI_PropertyShortValue propertyShortValue,
                                                          WDVCAPI_Id                 cId );

WDV_Bool Property_SetShortValue( WDVCAPI_WDV                wdv,
                                 WDVCAPI_Id                 resourceId,
                                 WDVCAPI_Id                 propertyId,
                                 WDVCAPI_PropertyShortValue propertyShortValue);

WDV_Bool Property_DeleteProperty( WDVCAPI_WDV   wdv,
                                  WDVCAPI_Id    resourceId,
                                  WDVCAPI_Id    propertyId );

WDV_Bool Property_CreateDefaultProperties( WDVCAPI_WDV  wdv,
                                           WDVCAPI_Id   cId,
                                           WDV_Bool     withDisplayName );

WDV_Bool Property_SetLastModified( WDVCAPI_WDV   wdv,
                                   WDVCAPI_Id    resourceId );

WDV_Bool Property_UpdateLastModified( WDVCAPI_WDV   wdv,
                                      WDVCAPI_Id    resourceId );

WDV_Bool Property_UpdateShortValue( WDVCAPI_WDV                wdv,
                                    WDVCAPI_Id                 resourceId,
                                    WDVCAPI_Id                 propertyId,
                                    WDVCAPI_PropertyShortValue propertyShortValue);

XMLXPath_Idx_RetCode Property_SetIndex( void             *parserUserData,
                                        void	         *indexUserData,
                                        const SAPDB_UTF8 *indexValue );

/***********************************************************************

	End

 ***********************************************************************/

#endif
