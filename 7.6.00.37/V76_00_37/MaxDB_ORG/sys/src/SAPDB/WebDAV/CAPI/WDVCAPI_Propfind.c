/*!**********************************************************************

  module: WDVCAPI_Propfind.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: WebDAV

  description:  Property find

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

#include "SAPDB/WebDAV/CAPI/WDVCAPI_WDV.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Memory.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Common.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_ErrorInternal.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Resource.h"
#include "SAPDB/WebDAV/CAPI/WDVCAPI_Id.h"
#include "SAPDBCommon/SAPDB_string.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define PROPERTY_NAME_LOCK_ID           "lockid"
#define PROPERTY_NAME_LOCK_DISCOVERY    "lockdiscovery"
#define PROPERTY_NAME_SUPPORTED_LOCK    "supportedlock"

/* TDB (MaO):                                                   */
/* Is this one and only SQL statement enough?                   */
/* This select iterates the whole tree down from an uri         */
/* but not every case is an infinite PROPFIND!                  */
/* What about PROPFIND requests with only one property          */
/* in the property list? With this select we get all the        */
/* properties of an uri. With this infinite select the amount   */
/* of the result set could be huge!                             */
#define SQL_SELECT_NAME_SPACE_LIST                                      \
    "DECLARE C CURSOR FOR WITH RECURSIVE MyIds(MyId) AS (               \
        SELECT CId FROM WEBDAV_Inode WHERE PId = X'%s'                         \
     UNION ALL                                                          \
        SELECT CId FROM WEBDAV_Inode, MyIds WHERE PId=MyId                     \
     ) SELECT DISTINCT NS.Id, NS.Name_Space                             \
       FROM WEBDAV_Name_Space NS, WEBDAV_Property_Management PM, WEBDAV_Property P, MyIds I  \
       WHERE NS.Id = PM.Name_Space_Id AND PM.Id = P.Property_Id AND CId In (I.MyId)"

#define SQL_SELECT_NAME_SPACE_LIST_FOR_RESOURCE \
    "SELECT DISTINCT NS.* FROM WEBDAV_Name_Space NS, WEBDAV_Property_Management PM, WEBDAV_Property P    \
     WHERE NS.Id = PM.Name_Space_Id AND PM.Id = P.Property_Id AND P.CId = X'%s'"

#define SQL_SELECT_ALL_NAME_SPACES  "SELECT Name_Space FROM WEBDAV_Name_Space"

#define SQL_SELECT_ALL_PROPERTIES   \
    "SELECT NS.Name_Space, PM.Name_Prefix, P.Property_Short_Value   \
     FROM WEBDAV_Property P, WEBDAV_Property_Management PM, WEBDAV_Name_Space NS         \
     WHERE P.CId = X'%s' AND P.Property_Id = PM.Id AND PM.Name_Space_Id = NS.Id AND \
           P.Property_Id NOT IN (X'"ID_PROPERTY_SUPPORTED_LOCK_STRING"', \
                                 X'"ID_PROPERTY_LOCK_ID_STRING"',        \
                                 X'"ID_PROPERTY_LOCK_OWNER_STRING"',     \
                                 X'"ID_PROPERTY_LOCK_TYPE_STRING"',      \
                                 X'"ID_PROPERTY_LOCK_SCOPE_STRING"',     \
                                 X'"ID_PROPERTY_LOCK_TIMEOUT_STRING"',   \
                                 X'"ID_PROPERTY_LOCK_DEPTH_STRING"')"

#define SQL_SELECT_ALL_PROPERTIES_OF_NULL_RESOURCE   \
    "SELECT NS.Name_Space, PM.Name_Prefix, P.Property_Short_Value   \
     FROM WEBDAV_Property P, WEBDAV_Property_Management PM, WEBDAV_Name_Space NS         \
     WHERE P.CId = X'%s' AND P.Property_Id = PM.Id AND PM.Name_Space_Id = NS.Id AND \
           P.Property_Id NOT IN (X'"ID_PROPERTY_SUPPORTED_LOCK_STRING"', \
                                 X'"ID_PROPERTY_LOCK_ID_STRING"',        \
                                 X'"ID_PROPERTY_LOCK_OWNER_STRING"',     \
                                 X'"ID_PROPERTY_LOCK_TYPE_STRING"',      \
                                 X'"ID_PROPERTY_LOCK_SCOPE_STRING"',     \
                                 X'"ID_PROPERTY_LOCK_TIMEOUT_STRING"',   \
                                 X'"ID_PROPERTY_LOCK_DEPTH_STRING"', \
                                 X'"ID_PROPERTY_GET_CONTENT_TYPE_STRING"', \
                                 X'"ID_PROPERTY_GET_CONTENT_LENGTH_STRING"', \
                                 X'"ID_PROPERTY_GET_LAST_MODIFIED_STRING"')"

#define SQL_SELECT_ALL_PROPERTY_NAMES   \
    "SELECT NS.Name_Space, PM.Name_Prefix                           \
     FROM WEBDAV_Property P, WEBDAV_Property_Management PM, WEBDAV_Name_Space NS         \
     WHERE P.CId = X'%s' AND P.Property_Id = PM.Id AND PM.Name_Space_Id = NS.Id AND \
           P.Property_Id NOT IN (X'"ID_PROPERTY_SUPPORTED_LOCK_STRING"', \
                                 X'"ID_PROPERTY_LOCK_ID_STRING"',        \
                                 X'"ID_PROPERTY_LOCK_OWNER_STRING"',     \
                                 X'"ID_PROPERTY_LOCK_TYPE_STRING"',      \
                                 X'"ID_PROPERTY_LOCK_SCOPE_STRING"',     \
                                 X'"ID_PROPERTY_LOCK_TIMEOUT_STRING"',   \
                                 X'"ID_PROPERTY_LOCK_DEPTH_STRING"')"

#define SQL_SELECT_ALL_PROPERTY_NAMES_OF_NULL_RESOURCE   \
    "SELECT NS.Name_Space, PM.Name_Prefix                           \
     FROM WEBDAV_Property P, WEBDAV_Property_Management PM, WEBDAV_Name_Space NS         \
     WHERE P.CId = X'%s' AND P.Property_Id = PM.Id AND PM.Name_Space_Id = NS.Id AND \
           P.Property_Id NOT IN (X'"ID_PROPERTY_SUPPORTED_LOCK_STRING"', \
                                 X'"ID_PROPERTY_LOCK_ID_STRING"',        \
                                 X'"ID_PROPERTY_LOCK_OWNER_STRING"',     \
                                 X'"ID_PROPERTY_LOCK_TYPE_STRING"',      \
                                 X'"ID_PROPERTY_LOCK_SCOPE_STRING"',     \
                                 X'"ID_PROPERTY_LOCK_TIMEOUT_STRING"',   \
                                 X'"ID_PROPERTY_LOCK_DEPTH_STRING"', \
                                 X'"ID_PROPERTY_GET_CONTENT_TYPE_STRING"', \
                                 X'"ID_PROPERTY_GET_CONTENT_LENGTH_STRING"', \
                                 X'"ID_PROPERTY_GET_LAST_MODIFIED_STRING"')"

/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef WDVCAPI_Id  PropfindNameSpaceId;
typedef char        PropfindNameSpaceShortcutString[WDV_MAX_NAME_SPACE_SHORTCUT_LEN+1];
typedef char        PropfindNameSpaceString[WDV_MAX_NAME_SPACE_LEN+1];
typedef char        PropfindPropertyNameString[WDV_MAX_PROPERTY_NAME_PREFIX_LEN+1];
typedef char        PropfindPropertyShortValueString[WDV_MAX_PROPERTY_SHORT_VALUE_LEN+1];
typedef char        PropfindPropertyLongValueString[WDV_MAX_PROPERTY_LONG_VALUE_LEN+1];
typedef size_t      PropfindPropertyLongValuePos;

typedef struct st_propfind_find_property    *PropfindFindProperty;
typedef struct st_propfind_property         *PropfindProperty;
typedef struct st_propfind_property_list    *PropfindPropertyList;
typedef struct st_propfind_name_space       *PropfindNameSpace;
typedef struct st_propfind_name_space_list  *PropfindNameSpaceList;
typedef struct st_propfind_navigation_node  *PropfindNavigationNode;

struct st_propfind_find_property {
    HSTMT                               hStmt;

    PropfindNameSpaceId                 nameSpaceId;
    SQLLEN                              nameSpaceIdIndicator;

    PropfindNameSpaceShortcutString     nameSpaceShortcut;

    PropfindNameSpaceString             nameSpace;
    SQLLEN                              nameSpaceIndicator;

    PropfindPropertyNameString          name;
    SQLLEN                              nameIndicator;

    PropfindPropertyShortValueString    shortValue;
    SQLLEN                              shortValueIndicator;

    PropfindPropertyLongValueString     longValue;
    PropfindPropertyLongValuePos        longValuePos;

    WDVCAPI_PropertyStatus              status;
};

struct st_propfind_property {
    WDVCAPI_NameSpaceShortcut   nameSpaceShortcut;
    PropfindNameSpaceString     nameSpace;
    PropfindPropertyNameString  name;


    PropfindProperty            nextProperty;
};

struct st_propfind_property_list {
    PropfindProperty firstProperty;
    PropfindProperty lastProperty;
    PropfindProperty currentProperty;
};

/*---------------------------------------------------------------------------*/

struct st_propfind_name_space {
    PropfindNameSpaceId                 nameSpaceId;    /*?? Has to be deleted in the future */
    PropfindNameSpaceShortcutString     nameSpaceShortcut;
    PropfindNameSpaceString             nameSpace;
    PropfindNameSpace                   nextNameSpace;
};

struct st_propfind_name_space_list {
    PropfindNameSpace  firstNameSpace;
    PropfindNameSpace  lastNameSpace;
    PropfindNameSpace  currentNameSpace;
};

/*---------------------------------------------------------------------------*/

struct st_propfind_navigation_node {
    HSTMT                       hStmt;

    WDVCAPI_Id                  id;
    SQLLEN                      idIndicator;

    WDVCAPI_URIString           name;
    SQLLEN                      nameIndicator;

    WDVCAPI_URIString           parentName;
    WDVCAPI_URIString           uriString;

    PropfindNavigationNode      childNodes;
    PropfindNavigationNode      parentNode;
};

/*---------------------------------------------------------------------------*/

struct st_wdvcapi_propfind {
    WDVCAPI_Id                          uriId;
    WDVCAPI_URIString                   uri;
    WDVCAPI_LockHandle                  currentLockDescription;
    WDVCAPI_Depth                       depth;
    WDVCAPI_PropfindType                type;
    PropfindPropertyList                propertyList;
    PropfindNameSpaceList               nameSpaceList;
    PropfindFindProperty                findProperty;
    PropfindNavigationNode              uriNavigationTree;
    PropfindNavigationNode              currentNavigationNode;
    WDVCAPI_NameSpaceShortcutCounter    nameSpaceShortcutCounter;
    WDV_UInt2                           staticPropertyNameListIndex;
};

/*---------------------------------------------------------------------------*/

/* Static property name list */
static char *staticPropertyNameList[] = { PROPERTY_NAME_LOCK_DISCOVERY, PROPERTY_NAME_SUPPORTED_LOCK, 0 };

/***********************************************************************

	Prototypes

 ***********************************************************************/

WDV_Bool InitPropfind( WDVCAPI_WDV             wdv,
                       WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool CreatePropfindPropertyList( WDVCAPI_WDV            wdv,
                                     PropfindPropertyList  *propertyList );

WDV_Bool DestroyPropfindPropertyList( WDVCAPI_WDV           wdv,
                                      PropfindPropertyList  propertyList );

WDV_Bool InitPropfindPropertyList( WDVCAPI_WDV           wdv,
                                   PropfindPropertyList  propertyList );

WDV_Bool CreatePropfindNameSpaceList( WDVCAPI_WDV             wdv,
                                      PropfindNameSpaceList  *nameSpaceList );

WDV_Bool DestroyPropfindNameSpaceList( WDVCAPI_WDV            wdv,
                                       PropfindNameSpaceList  nameSpaceList );

WDV_Bool InitPropfindNameSpaceList( WDVCAPI_WDV            wdv,
                                    PropfindNameSpaceList  nameSpaceList );

WDV_Bool CreatePropfindNameSpace( WDVCAPI_WDV        wdv,
                                  PropfindNameSpace *nameSpace );

WDV_Bool DestroyPropfindNameSpace( WDVCAPI_WDV        wdv,
                                   PropfindNameSpace  nameSpace );

WDV_Bool InitPropfindNameSpace( WDVCAPI_WDV        wdv,
                                PropfindNameSpace  nameSpace );

WDV_Bool CreatePropfindProperty( WDVCAPI_WDV        wdv,
                                 PropfindProperty  *newProperty );

WDV_Bool DestroyPropfindProperty( WDVCAPI_WDV       wdv,
                                  PropfindProperty  property );

WDV_Bool InitPropfindProperty( WDVCAPI_WDV       wdv,
                               PropfindProperty  property );

WDV_Bool CreatePropfindFindProperty( WDVCAPI_WDV            wdv,
                                     PropfindFindProperty  *newFindProperty );

WDV_Bool DestroyPropfindFindProperty( WDVCAPI_WDV           wdv,
                                      PropfindFindProperty  findProperty );

WDV_Bool InitPropfindFindProperty( WDVCAPI_WDV           wdv,
                                   PropfindFindProperty  findProperty );

WDV_Bool PropfindOpen( WDVCAPI_WDV             wdv,
                       WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool PropfindOpenProperty( WDVCAPI_WDV             wdv,
                               WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool PropfindOpenNavigationTree( WDVCAPI_WDV             wdv,
                                     WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool CreateNavigationNode( WDVCAPI_WDV              wdv,
                               WDVCAPI_Id               pid,
                               WDVCAPI_URI              parentName,
                               PropfindNavigationNode  *newNavigationNode );

WDV_Bool DestroyNavigationNode( WDVCAPI_WDV             wdv,
                                PropfindNavigationNode  navigationNode );

WDV_Bool InitNavigationNode( WDVCAPI_WDV             wdv,
                             PropfindNavigationNode  navigationNode );

WDV_Bool PropfindOpenNextSiblingURI( WDVCAPI_WDV             wdv,
                                     WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool PropfindOpenNextChildURI( WDVCAPI_WDV             wdv,
                                   WDVCAPI_PropfindHandle  hPropfind,
                                   WDVCAPI_Id              id,
                                   WDVCAPI_URI             parentName );

WDV_Bool PropfindGetNameSpaceList( WDVCAPI_WDV             wdv,
                                   WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool AddNewNameSpaceToList( WDVCAPI_WDV                 wdv,
                                WDVCAPI_PropfindHandle      hPropfind,
                                PropfindNameSpaceId         nameSpaceId,
                                WDVCAPI_PropfindNameSpace   nameSpace );

WDV_Bool GetNameSpaceShortcutFromNameSpaceList( PropfindNameSpaceList               nameSpaceList,
                                                WDVCAPI_PropfindNameSpace           nameSpace,
                                                WDVCAPI_PropfindNameSpaceShortcut   nameSpaceShortcut );

WDV_Bool Propfind_AddToNameSpaceList( WDVCAPI_WDV                  wdv,
                                      WDVCAPI_PropfindHandle       hPropfind,
                                      WDVCAPI_NameSpace            nameSpace,
                                      WDVCAPI_NameSpaceShortcut   *nameSpaceShortcut );

WDV_Bool Propfind_InitNameSpace( WDVCAPI_WDV                wdv,
                                 WDVCAPI_PropfindHandle     hPropfind,
                                 PropfindNameSpace          newNameSpace,
                                 WDVCAPI_NameSpace          nameSpace,
                                 WDVCAPI_NameSpaceShortcut *nameSpaceShortcut );

WDV_Bool Propfind_SearchNameSpaceInList( WDVCAPI_WDV                wdv,
                                         PropfindNameSpace          nameSpaceList,
                                         WDVCAPI_NameSpace          nameSpaceToFind,
                                         WDVCAPI_NameSpaceShortcut *foundNameSpaceShortcut );

WDV_Bool Propfind_GetAllNameSpaces( WDVCAPI_WDV             wdv,
                                    WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool Propfind_Propfind( WDVCAPI_WDV             wdv,
                            WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool Propfind_PropfindAll( WDVCAPI_WDV             wdv,
                               WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool Propfind_OpenAll( WDVCAPI_WDV             wdv,
                           WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool Propfind_OpenPropertyAll( WDVCAPI_WDV             wdv,
                                   WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool Propfind_GetFirstProperty( WDVCAPI_WDV              wdv,
                                    WDVCAPI_PropfindHandle   hPropfind );

WDV_Bool Propfind_GetNextProperty( WDVCAPI_WDV              wdv,
                                   WDVCAPI_PropfindHandle   hPropfind );

WDV_Bool Propfind_GetFirstOfAllProperties( WDVCAPI_WDV              wdv,
                                           WDVCAPI_PropfindHandle   hPropfind );

WDV_Bool Propfind_GetNextOfAllProperties( WDVCAPI_WDV              wdv,
                                          WDVCAPI_PropfindHandle   hPropfind );

WDV_Bool Propfind_OpenName( WDVCAPI_WDV             wdv,
                            WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool Propfind_PropfindName( WDVCAPI_WDV             wdv,
                                WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool Propfind_GetFirstOfAllPropertyNames( WDVCAPI_WDV              wdv,
                                              WDVCAPI_PropfindHandle   hPropfind );

WDV_Bool Propfind_GetNextOfAllPropertyNames( WDVCAPI_WDV              wdv,
                                             WDVCAPI_PropfindHandle   hPropfind );

WDV_Bool Propfind_OpenPropertyNames( WDVCAPI_WDV             wdv,
                                     WDVCAPI_PropfindHandle  hPropfind );

WDV_Bool Propfind_ClearLockDescription( WDVCAPI_WDV            wdv,
				                        WDVCAPI_PropfindHandle hPropfind );

/***********************************************************************

	API Functions

 ***********************************************************************/

WDV_Bool WDVCAPI_PropfindCreate( WDVCAPI_WDV             wdv,
                                 WDVCAPI_URI             uri,
                                 WDVCAPI_Depth           depth,
                                 WDVCAPI_PropfindType    type,
                                 WDVCAPI_PropfindHandle *hPropfind )
{

	WDV_Bool                ok = WDV_False;
	WDVCAPI_PropfindHandle  newPropfind = NULL;

	if (!wdv || !hPropfind) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_wdvcapi_propfind), (WDV_UInt1**) &newPropfind, &ok );
	if (ok == WDV_True) {
		/* Init propfind object */
		InitPropfind(wdv, newPropfind);

		/* Set return value */
        strncpy(newPropfind->uri, uri, WDV_MAX_URI_LEN);
        newPropfind->uri[WDV_MAX_URI_LEN] = '\0';

        newPropfind->depth = depth;
        newPropfind->type = type;

		*hPropfind = newPropfind;

		return WDV_True;
	} else {
		*hPropfind = NULL;

		return WDV_False;
	}

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindDestroy( WDVCAPI_WDV             wdv,
                                  WDVCAPI_PropfindHandle  hPropfind )
{

	if (!wdv || !hPropfind ) {
		return WDV_False;
	}

    /* Destroy lock handle */
    WDVCAPI_LockDestroyHandle(wdv, hPropfind->currentLockDescription);

	/* Destroy propfind propery list */
    DestroyPropfindPropertyList(wdv, hPropfind->propertyList);
    hPropfind->propertyList = NULL;

	/* Destroy propfind name space list */
    DestroyPropfindNameSpaceList(wdv, hPropfind->nameSpaceList);
    hPropfind->nameSpaceList = NULL;

	/* Destroy propfind find property */
    DestroyPropfindFindProperty(wdv, hPropfind->findProperty);
    hPropfind->findProperty = NULL;

    /* Destroy navigation tree */
    DestroyNavigationNode(wdv, hPropfind->uriNavigationTree);
    hPropfind->uriNavigationTree = NULL;
    hPropfind->currentNavigationNode = NULL;

	/* Destroy propfind itself */
    WDVCAPI_Free((WDV_UInt1*) hPropfind );

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindAddPropertyToPropertyList ( WDVCAPI_WDV                    wdv,
                                                     WDVCAPI_PropfindHandle         hPropfind,
                                                     WDVCAPI_PropfindNameSpace      nameSpace,
                                                     WDVCAPI_PropfindPropertyName   propertyName )
{

    PropfindProperty  newProperty = NULL;

    if (!wdv || !hPropfind) {
        return WDV_False;
    }

    /* Create new property */
    if (!CreatePropfindProperty(wdv, &newProperty)) {
        return WDV_False;
    }

    /* Add name space to name space list and get name space shortcut */
    Propfind_AddToNameSpaceList(wdv, hPropfind, nameSpace, &newProperty->nameSpaceShortcut);

    /* Set property values */
    Common_StrMaxCopy(newProperty->nameSpace, nameSpace, WDV_MAX_NAME_SPACE_LEN);
    Common_StrMaxCopy(newProperty->name, propertyName, WDV_MAX_PROPERTY_NAME_PREFIX_LEN);

    /* Add new property to list */
    if (!hPropfind->propertyList->firstProperty) {
        hPropfind->propertyList->firstProperty = newProperty;
        hPropfind->propertyList->lastProperty = newProperty;
    } else {
        hPropfind->propertyList->lastProperty->nextProperty = newProperty;
        hPropfind->propertyList->lastProperty = newProperty;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_Propfind( WDVCAPI_WDV             wdv,
                           WDVCAPI_PropfindHandle  hPropfind )
{
    TRACE_OUT(">> WDVCAPI_Propfind()\n");

    /* Checks */
    if (!wdv || !hPropfind) {
        if (wdv) {
            Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
				                WDVCAPI_ERR_TEXT_INTERNAL_ERROR );
        }

        return WDV_False;
    };

    /* Which type of propfind has to be used? */
    switch(hPropfind->type) {
    case WDVCAPI_PROPFIND_TYPE_PROPFIND:
        return Propfind_Propfind(wdv, hPropfind);
    case WDVCAPI_PROPFIND_TYPE_PROPFIND_ALL:
        return Propfind_PropfindAll(wdv, hPropfind);
    case WDVCAPI_PROPFIND_TYPE_PROPFIND_NAME:
        return Propfind_PropfindName(wdv, hPropfind);
    default:
        Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				            WDVCAPI_ERR_CODE_UNKNOWN_PROPFIND_TYPE,
				            WDVCAPI_ERR_TEXT_UNKNOWN_PROPFIND_TYPE );

        return WDV_False;
    }

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindGetFirstNameSpace( WDVCAPI_WDV                        wdv,
                                            WDVCAPI_PropfindHandle             hPropfind,
                                            WDVCAPI_PropfindNameSpaceShortcut *nameSpaceShortcut,
                                            WDVCAPI_PropfindNameSpace         *nameSpace )
{

    /* Checks */
    if (!wdv || !nameSpace || !hPropfind || !hPropfind->nameSpaceList || !hPropfind->nameSpaceList->firstNameSpace) {
        return WDV_False;
    }

    /* Set list pointer to first name space */
    hPropfind->nameSpaceList->currentNameSpace = hPropfind->nameSpaceList->firstNameSpace;

    /* Get name space */
    if (!hPropfind->nameSpaceList->currentNameSpace) {
        return WDV_False;
    }

    /* Get name space shortcut */
    *nameSpaceShortcut = hPropfind->nameSpaceList->currentNameSpace->nameSpaceShortcut;

    /* Get name space */
    *nameSpace = hPropfind->nameSpaceList->currentNameSpace->nameSpace;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindGetNextNameSpace( WDVCAPI_WDV                        wdv,
                                           WDVCAPI_PropfindHandle             hPropfind,
                                           WDVCAPI_PropfindNameSpaceShortcut *nameSpaceShortcut,
                                           WDVCAPI_PropfindNameSpace         *nameSpace )
{

    /* Checks */
    if (!wdv || !nameSpace || !hPropfind || !hPropfind->nameSpaceList) {
        return WDV_False;
    }

    /* Start of the list? */
    if (!hPropfind->nameSpaceList->currentNameSpace) {
        hPropfind->nameSpaceList->currentNameSpace = hPropfind->nameSpaceList->firstNameSpace;
    } else {
        /* Set list pointer to the next name space */
        hPropfind->nameSpaceList->currentNameSpace = hPropfind->nameSpaceList->currentNameSpace->nextNameSpace;
    }

    /* Get name space */
    if (!hPropfind->nameSpaceList->currentNameSpace) {
        return WDV_False;
    }

    /* Get name space shortcut */
    *nameSpaceShortcut = hPropfind->nameSpaceList->currentNameSpace->nameSpaceShortcut;

    /* Get name space */
    *nameSpace = hPropfind->nameSpaceList->currentNameSpace->nameSpace;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindGetNextURI( WDVCAPI_WDV               wdv,
                                     WDVCAPI_PropfindHandle    hPropfind,
                                     WDVCAPI_URI              *uri )
{

    /* According depth? */
    if (!wdv || hPropfind->depth == WDVCAPI_DEPTH_0) {
        Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				            WDVCAPI_ERR_CODE_WRONG_DEPTH,
				            WDVCAPI_ERR_TEXT_WRONG_DEPTH );
    }

    /* Iterate URI members */
    switch (hPropfind->depth) {
    case WDVCAPI_DEPTH_1:
        if (!PropfindOpenNextSiblingURI(wdv, hPropfind)) {
		    if(WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_NO_MORE_SIBLING_NODES)) {
                Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				                    WDVCAPI_ERR_CODE_NO_MORE_RESOURCES,
				                    WDVCAPI_ERR_TEXT_NO_MORE_RESOURCES );
		    }

            return WDV_False;
        }

        break;
    case WDVCAPI_DEPTH_INFINITE:
        while(hPropfind->currentNavigationNode) {
            if (!PropfindOpenNextSiblingURI(wdv, hPropfind)) {
		        if(WDVCAPI_IsError(wdv, WDVCAPI_ERR_TYPE_CAPI, WDVCAPI_ERR_CODE_NO_MORE_SIBLING_NODES)) {
                    hPropfind->currentNavigationNode = hPropfind->currentNavigationNode->parentNode;
                    continue;
                } else {
                    return WDV_False;
                }
            }

            if (!PropfindOpenNextChildURI(wdv, hPropfind,
                                               hPropfind->currentNavigationNode->id,
                                               hPropfind->currentNavigationNode->uriString)) {
                return WDV_False;
		    }

            break;
        }

        if (!hPropfind->currentNavigationNode) {
            Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_NO_MORE_RESOURCES,
				                WDVCAPI_ERR_TEXT_NO_MORE_RESOURCES );

            return WDV_False;
        }

        break;
    default:
        Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				            WDVCAPI_ERR_CODE_UNKNOWN_PROPFIND_DEPTH,
				            WDVCAPI_ERR_TEXT_UNKNOWN_PROPFIND_DEPTH );

        return WDV_False;
    }

    /* Open result for next uri */
    switch(hPropfind->type) {
    case WDVCAPI_PROPFIND_TYPE_PROPFIND:
        if (!PropfindOpen(wdv, hPropfind)) {
            return WDV_False;
        }

        break;
    case WDVCAPI_PROPFIND_TYPE_PROPFIND_ALL:
        /* Open will be done by the call of */
        /* WDVCAPI_PropfindGetFirstProperty */
        break;
    case WDVCAPI_PROPFIND_TYPE_PROPFIND_NAME:
        /* Open will be done by the call of */
        /* WDVCAPI_PropfindGetFirstProperty */
        break;
    default:
        Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				            WDVCAPI_ERR_CODE_UNKNOWN_PROPFIND_TYPE,
				            WDVCAPI_ERR_TEXT_UNKNOWN_PROPFIND_TYPE );

        return WDV_False;
    }

    /* Set uri */
    *uri = hPropfind->currentNavigationNode->uriString;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindGetFirstProperty( WDVCAPI_WDV              wdv,
                                           WDVCAPI_PropfindHandle   hPropfind )
{

    /* Checks */
    if (!wdv || !hPropfind) {
        if (wdv) {
            Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
				                WDVCAPI_ERR_TEXT_INTERNAL_ERROR );
        }

        return WDV_False;
    };

    /* Which type of propfind has to be used? */
    switch(hPropfind->type) {
    case WDVCAPI_PROPFIND_TYPE_PROPFIND:
        return Propfind_GetFirstProperty(wdv, hPropfind);
    case WDVCAPI_PROPFIND_TYPE_PROPFIND_ALL:
        return Propfind_GetFirstOfAllProperties(wdv, hPropfind);
    case WDVCAPI_PROPFIND_TYPE_PROPFIND_NAME:
        return Propfind_GetFirstOfAllPropertyNames(wdv, hPropfind);
    default:
        Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				            WDVCAPI_ERR_CODE_UNKNOWN_PROPFIND_TYPE,
				            WDVCAPI_ERR_TEXT_UNKNOWN_PROPFIND_TYPE );

        return WDV_False;
    }

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindGetNextProperty( WDVCAPI_WDV              wdv,
                                          WDVCAPI_PropfindHandle   hPropfind )
{

    /* Checks */
    if (!wdv || !hPropfind) {
        if (wdv) {
            Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
				                WDVCAPI_ERR_TEXT_INTERNAL_ERROR );
        }

        return WDV_False;
    };

    /* Which type of propfind has to be used? */
    switch(hPropfind->type) {
    case WDVCAPI_PROPFIND_TYPE_PROPFIND:
        return Propfind_GetNextProperty(wdv, hPropfind);
    case WDVCAPI_PROPFIND_TYPE_PROPFIND_ALL:
        return Propfind_GetNextOfAllProperties(wdv, hPropfind);
    case WDVCAPI_PROPFIND_TYPE_PROPFIND_NAME:
        return Propfind_GetNextOfAllPropertyNames(wdv, hPropfind);
    default:
        Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				            WDVCAPI_ERR_CODE_UNKNOWN_PROPFIND_TYPE,
				            WDVCAPI_ERR_TEXT_UNKNOWN_PROPFIND_TYPE );

        return WDV_False;
    }

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindGetPropertyStatus( WDVCAPI_WDV              wdv,
                                            WDVCAPI_PropfindHandle   hPropfind,
                                            WDVCAPI_PropertyStatus  *propertyStatus )
{

    if (!wdv || !hPropfind || !hPropfind->findProperty || !propertyStatus) {
        return WDV_False;
    }

    *propertyStatus = hPropfind->findProperty->status;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindGetPropertyNameSpaceShortcut( WDVCAPI_WDV                        wdv,
                                                       WDVCAPI_PropfindHandle             hPropfind,
                                                       WDVCAPI_PropfindNameSpaceShortcut *nameSpaceShortcut )
{

    if (!wdv || !hPropfind || !hPropfind->findProperty || !nameSpaceShortcut) {
        return WDV_False;
    }

    *nameSpaceShortcut = hPropfind->findProperty->nameSpaceShortcut;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindGetPropertyNameSpace( WDVCAPI_WDV                   wdv,
                                               WDVCAPI_PropfindHandle        hPropfind,
                                               WDVCAPI_PropfindNameSpace    *nameSpace )
{

    if (!wdv || !hPropfind || !hPropfind->findProperty || !nameSpace) {
        return WDV_False;
    }

    *nameSpace = hPropfind->findProperty->nameSpace;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindGetPropertyName( WDVCAPI_WDV                    wdv,
                                          WDVCAPI_PropfindHandle         hPropfind,
                                          WDVCAPI_PropfindPropertyName  *propertyName )
{

    if (!wdv || !hPropfind || !hPropfind->findProperty || !propertyName) {
        return WDV_False;
    }

    *propertyName = hPropfind->findProperty->name;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindGetPropertyShortValue( WDVCAPI_WDV                          wdv,
                                                WDVCAPI_PropfindHandle               hPropfind,
                                                WDVCAPI_PropfindPropertyShortValue  *propertyShortValue )
{

    if (!wdv || !hPropfind || !hPropfind->findProperty || !propertyShortValue) {
        return WDV_False;
    }

    *propertyShortValue = hPropfind->findProperty->shortValue;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindGetLockDescription( WDVCAPI_WDV             wdv,
                                             WDVCAPI_PropfindHandle  hPropfind,
                                             WDVCAPI_LockIdString    lockIdString,
                                             WDVCAPI_LockHandle     *hLock )
{

    WDVCAPI_Id  lockId;

    if (!wdv || !hPropfind || !hPropfind->findProperty || !hLock) {
        return WDV_False;
    }

    /* Convert lock id string to an id */
    WDVCAPI_IdStringAsId(lockIdString, lockId);

    /* Do we have the lock description already? */
    if (WDVCAPI_IdsAreIdentical(lockId, hPropfind->currentLockDescription->id)) {
        *hLock = hPropfind->currentLockDescription;
    } else {
        if (!Lock_GetDescriptionByLockId(wdv, lockIdString, hLock)) {
            return WDV_False;
        }
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool WDVCAPI_PropfindOriginalLocation( WDVCAPI_WDV                          wdv,
                                           WDVCAPI_URI                          uri,
                                           WDVCAPI_PropfindPropertyShortValue   propertyShortValue  )
{

	SQLRETURN           sqlRC = 0;
    WDVCAPI_Resource    resource = NULL;

    /* Checks */
    if (!wdv || !uri || !propertyShortValue) {
        if (wdv) {
            Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
				                WDVCAPI_ERR_TEXT_INTERNAL_ERROR );
        }

        return WDV_False;
    };


    /* Create source resource object */
    if (!Resource_CreateHandle(wdv, &resource)) {
        return WDV_False;
    }

    /* Get resource by uri */
    if (!Resource_GetByUri(wdv, uri, resource)) {
        Resource_DestroyHandle(wdv, resource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    if (!Property_GetShortValue(wdv, resource->id, ID_PROPERTY_ORIGINAL_LOCATION, propertyShortValue)) {
        Resource_DestroyHandle(wdv, resource);

        /* Cancel transaction */
        WDV_CancelTransaction(wdv);

        return WDV_False;
    }

    /* Cancel transaction */
    WDV_CancelTransaction(wdv);

    Resource_DestroyHandle(wdv, resource);

    return WDV_True;

}

/***********************************************************************

	API Internal Functions

 ***********************************************************************/

/***********************************************************************

	Module Functions

 ***********************************************************************/

WDV_Bool InitPropfind( WDVCAPI_WDV             wdv,
                       WDVCAPI_PropfindHandle  hPropfind )
{

    PropfindPropertyList  newPropertyList = NULL;
    PropfindNameSpaceList newNameSpaceList = NULL;
    PropfindFindProperty  newFindProperty = NULL;

    memset(hPropfind->uriId, 0, WDVCAPI_MAX_ID_LEN + 1);
    memset(hPropfind->uri, 0, WDV_MAX_URI_LEN + 1);
    hPropfind->depth = WDVCAPI_DEPTH_0;
    hPropfind->type = WDVCAPI_PROPFIND_TYPE_UNDEFINED;

    if (!WDVCAPI_LockCreateHandle(wdv, &hPropfind->currentLockDescription)) {
        return WDV_False;
    }

    hPropfind->nameSpaceShortcutCounter = 0;

    /* Create property list object */
    if (!CreatePropfindPropertyList(wdv, &newPropertyList)) {
        return WDV_False;
    }

    hPropfind->propertyList = newPropertyList;

    /* Create name space list object */
    if (!CreatePropfindNameSpaceList(wdv, &newNameSpaceList)) {
        return WDV_False;
    }

    hPropfind->nameSpaceList = newNameSpaceList;

    /* Create find property object */
    if (!CreatePropfindFindProperty(wdv, &newFindProperty)) {
        return WDV_False;
    }

    hPropfind->findProperty = newFindProperty;

    /* Remain inits */
    hPropfind->uriNavigationTree = NULL;
    hPropfind->currentNavigationNode = NULL;

    hPropfind->staticPropertyNameListIndex = 0;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_ResetHandle( WDVCAPI_WDV             wdv,
                               WDVCAPI_PropfindHandle  hPropfind )
{
    PropfindPropertyList  newPropertyList = NULL;
    PropfindNameSpaceList newNameSpaceList = NULL;
    PropfindFindProperty  newFindProperty = NULL;

	if ( !hPropfind ) {
		return WDV_False;
	}

	/* Destroy propfind propery list */
    if (hPropfind->propertyList) {
        DestroyPropfindPropertyList(wdv, hPropfind->propertyList);
        hPropfind->propertyList = NULL;
    }

    /* Create property list object */
    if (!CreatePropfindPropertyList(wdv, &newPropertyList)) {
        return WDV_False;
    }

    hPropfind->propertyList = newPropertyList;

	/* Destroy propfind name space list */
    if (hPropfind->nameSpaceList) {
        DestroyPropfindNameSpaceList(wdv, hPropfind->nameSpaceList);
        hPropfind->nameSpaceList = NULL;
    }

    /* Create name space list object */
    if (!CreatePropfindNameSpaceList(wdv, &newNameSpaceList)) {
        return WDV_False;
    }

    hPropfind->nameSpaceList = newNameSpaceList;

	/* Init propfind find property */
    if (hPropfind->findProperty) {
        if (!InitPropfindFindProperty(wdv, hPropfind->findProperty)) {
            return WDV_False;
        }
    }

    /* Destroy navigation tree */
    if (hPropfind->uriNavigationTree) {
        DestroyNavigationNode(wdv, hPropfind->uriNavigationTree);
        hPropfind->uriNavigationTree = NULL;
    }
    hPropfind->currentNavigationNode = NULL;

	/* Reset shortcut counter */
    hPropfind->nameSpaceShortcutCounter = 0;

	return WDV_True;

}

/*===========================================================================*/


WDV_Bool CreatePropfindPropertyList( WDVCAPI_WDV            wdv,
                                     PropfindPropertyList  *propertyList )
{

	WDV_Bool             ok = WDV_False;
    PropfindPropertyList newPropfindPropertyList = NULL;

	if ( !propertyList ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_propfind_property_list), (WDV_UInt1**) &newPropfindPropertyList, &ok );
	if (ok == WDV_True) {
		/* Init hPropfind property list object */
		InitPropfindPropertyList(wdv, newPropfindPropertyList);

		*propertyList = newPropfindPropertyList;

		return WDV_True;
	} else {
		*propertyList = NULL;

		return WDV_False;
	}

}

/*===========================================================================*/

WDV_Bool DestroyPropfindPropertyList( WDVCAPI_WDV           wdv,
                                      PropfindPropertyList  propertyList )
{

	PropfindProperty    propertyToDelete = NULL;

	if ( !propertyList ) {
		return WDV_False;
	}

	/* Destroy property list */
	while(propertyList->firstProperty) {
		/* Set next property to delete */
		propertyToDelete = propertyList->firstProperty;
		propertyList->firstProperty = propertyToDelete->nextProperty;

		/* Destroy property */
        DestroyPropfindProperty(wdv, propertyToDelete);
	}

	/* Destroy property list itself */
    WDVCAPI_Free((WDV_UInt1*) propertyList );

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool InitPropfindPropertyList( WDVCAPI_WDV           wdv,
                                   PropfindPropertyList  propertyList )
{

    propertyList->firstProperty = NULL;
    propertyList->lastProperty = NULL;
    propertyList->currentProperty  = NULL;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool CreatePropfindNameSpaceList( WDVCAPI_WDV             wdv,
                                      PropfindNameSpaceList  *nameSpaceList )
{

	WDV_Bool              ok = WDV_False;
    PropfindNameSpaceList newNameSpaceList = NULL;

	if ( !nameSpaceList ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_propfind_name_space_list), (WDV_UInt1**) &newNameSpaceList, &ok );
	if (ok == WDV_True) {
		/* Init propfind name space list object */
		InitPropfindNameSpaceList(wdv, newNameSpaceList);

		*nameSpaceList = newNameSpaceList;

		return WDV_True;
	} else {
		*nameSpaceList = NULL;

		return WDV_False;
	}

}

/*===========================================================================*/

WDV_Bool DestroyPropfindNameSpaceList( WDVCAPI_WDV            wdv,
                                       PropfindNameSpaceList  nameSpaceList )
{

	PropfindNameSpace   nameSpaceToDelete = NULL;

	if ( !nameSpaceList ) {
		return WDV_False;
	}

	/* Destroy name space list */
	while(nameSpaceList->firstNameSpace) {
		/* Set next property to delete */
		nameSpaceToDelete = nameSpaceList->firstNameSpace;
		nameSpaceList->firstNameSpace = nameSpaceToDelete->nextNameSpace;

		/* Destroy name space */
        DestroyPropfindNameSpace(wdv, nameSpaceToDelete );
	}

	/* Destroy name space list itself */
    WDVCAPI_Free((WDV_UInt1*) nameSpaceList );

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool InitPropfindNameSpaceList( WDVCAPI_WDV            wdv,
                                    PropfindNameSpaceList  nameSpaceList )
{

    nameSpaceList->firstNameSpace = NULL;
    nameSpaceList->lastNameSpace = NULL;
    nameSpaceList->currentNameSpace = NULL;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool PropfindOpen( WDVCAPI_WDV             wdv,
                       WDVCAPI_PropfindHandle  hPropfind )
{

    SQLCHAR             sqlStmt[1024] = "";
    SQLHDBC             hDBC = 0;
    SQLRETURN           sqlRC;
    SQLLEN              bufferLength = 0;
    SQLINTEGER          indi = SQL_NTS;
    WDVCAPI_IdString    idString = "";
    WDVCAPI_Resource    currentResource = NULL;

    /* Checks */
    if (!wdv || !hPropfind || !hPropfind->currentNavigationNode ||
        !hPropfind->findProperty || !hPropfind->propertyList ) {

        return WDV_False;

    }

    /* Create resource object */
    if (!Resource_CreateHandle(wdv, &currentResource)) {
        return WDV_False;
    }

    /* Get URI resource */
    if (!Resource_GetByUri(wdv, hPropfind->currentNavigationNode->uriString, currentResource)) {
        Resource_DestroyHandle(wdv, currentResource);
        return WDV_False;
    }

    /* Get lock if there is one in the URI? */
    Propfind_ClearLockDescription(wdv, hPropfind);
    Resource_IsLockInURI(wdv, currentResource, hPropfind->currentLockDescription);

    /* Destroy resource handle */
    Resource_DestroyHandle(wdv, currentResource);

    /* Convert resource id to an id string */
    WDVCAPI_IdAsString(hPropfind->currentNavigationNode->id, idString);

    /* Build SQL statement */
    if (strcmp(currentResource->resourceType, "null resource") == 0) {
        sp77sprintf(sqlStmt, 1024, "Select P.Property_Short_Value \
                                    From WEBDAV_Name_Space NS, WEBDAV_Property_Management PM, WEBDAV_Property P \
                                    Where NS.Id = PM.Name_Space_Id And \
                                          PM.Id = P.Property_Id And \
                                          P.CId = X'%s' And \
                                          NS.Name_Space = ? And PM.Name_Prefix = ? AND \
                                          P.Property_Id NOT IN \
                                          ( \
                                            x'"ID_PROPERTY_GET_CONTENT_TYPE_STRING"', \
                                            x'"ID_PROPERTY_GET_CONTENT_LENGTH_STRING"', \
                                            x'"ID_PROPERTY_GET_LAST_MODIFIED_STRING"' \
                                          )", idString);
    } else {
        sp77sprintf(sqlStmt, 1024, "Select P.Property_Short_Value \
                                    From WEBDAV_Name_Space NS, WEBDAV_Property_Management PM, WEBDAV_Property P \
                                    Where NS.Id = PM.Name_Space_Id And \
                                        PM.Id = P.Property_Id And \
                                        P.CId = X'%s' And \
                                        NS.Name_Space = ? And PM.Name_Prefix = ?", idString);
    }

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Free existing statement handle */
    if (hPropfind->findProperty->hStmt) {
	    SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
        hPropfind->findProperty->hStmt = 0;
	}

    /* Reset property list pos */
    hPropfind->propertyList->currentProperty = NULL;

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hPropfind->findProperty->hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );
		return WDV_False;
	}

	/* Prepare statement */
	sqlRC = SQLPrepare( hPropfind->findProperty->hStmt, sqlStmt, SQL_NTS );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

    /* Bind input columns */
    sqlRC = SQLBindParameter( hPropfind->findProperty->hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                              hPropfind->findProperty->nameSpace, WDV_MAX_NAME_SPACE_LEN,
                              &hPropfind->findProperty->nameSpaceIndicator );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

    sqlRC = SQLBindParameter( hPropfind->findProperty->hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0,
                              hPropfind->findProperty->name, WDV_MAX_PROPERTY_NAME_PREFIX_LEN,
                              &hPropfind->findProperty->nameIndicator );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

    /* Bind output columns */
    sqlRC = SQLBindCol(hPropfind->findProperty->hStmt, 1, SQL_C_CHAR,
                       hPropfind->findProperty->shortValue, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                      &hPropfind->findProperty->shortValueIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

    hPropfind->findProperty->nameSpaceIndicator = SQL_NTS;
    hPropfind->findProperty->nameIndicator = SQL_NTS;

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool PropfindOpenProperty( WDVCAPI_WDV             wdv,
                               WDVCAPI_PropfindHandle  hPropfind )
{

    SQLRETURN   sqlRC;

    /* Checks */
    if (!wdv || !hPropfind) {
        if (wdv) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                               WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        return WDV_False;
    }

    if (hPropfind->type == WDVCAPI_PROPFIND_TYPE_PROPFIND &&
       (!hPropfind->propertyList || !hPropfind->propertyList->currentProperty)) {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_EMPTY_PROPERTY_LIST,
                           WDVCAPI_ERR_TEXT_EMPTY_PROPERTY_LIST);

        return WDV_False;
    }

    /* Inits */
    hPropfind->findProperty->status = WDVCAPI_PROPERTY_STATUS_OK;
    memset(hPropfind->findProperty->nameSpaceId, 0, WDVCAPI_MAX_ID_LEN);
    hPropfind->findProperty->nameSpaceShortcut[0] = '\0';
    hPropfind->findProperty->nameSpace[0] = '\0';
    hPropfind->findProperty->name[0] = '\0';
    hPropfind->findProperty->shortValue[0] = '\0';
    hPropfind->findProperty->longValue[0] = '\0';
    hPropfind->findProperty->longValuePos = 0;

    /* Get property info */
    if (hPropfind->type == WDVCAPI_PROPFIND_TYPE_PROPFIND) {
        Common_StrMaxCopy(hPropfind->findProperty->nameSpaceShortcut,
                          hPropfind->propertyList->currentProperty->nameSpaceShortcut, WDV_MAX_NAME_SPACE_SHORTCUT_LEN);
        Common_StrMaxCopy(hPropfind->findProperty->nameSpace,
                          hPropfind->propertyList->currentProperty->nameSpace, WDV_MAX_NAME_SPACE_LEN);
        Common_StrMaxCopy(hPropfind->findProperty->name,
                          hPropfind->propertyList->currentProperty->name, WDV_MAX_PROPERTY_NAME_PREFIX_LEN);
    }

    /* Is the asked property the lockdiscovery property,          */
    /* so check whether we already have a lock description or not */
    if (strcmp(hPropfind->findProperty->nameSpace, WDV_NAME_SPACE_DAV) == 0 &&
        strcmp(hPropfind->findProperty->name, PROPERTY_NAME_LOCK_DISCOVERY) == 0) {
        /* Get stored lock id. The client will get the whole lock description */
        /* with calling the function WDVCAPI_PropfindGetLockDescription().     */
        WDVCAPI_IdAsString(hPropfind->currentLockDescription->id, hPropfind->findProperty->shortValue);
		if (WDVCAPI_IdStringIsInitialValue(hPropfind->findProperty->shortValue) == WDV_True) {
            hPropfind->findProperty->status = WDVCAPI_PROPERTY_STATUS_FAILED;
        }
    } else if (strcmp(hPropfind->findProperty->nameSpace, WDV_NAME_SPACE_DAV) == 0 &&
        strcmp(hPropfind->findProperty->name, PROPERTY_NAME_SUPPORTED_LOCK) == 0) {
        /* Is the asked property the supported lock property? */
    
        /* Get supported lock property. This is static in this version of ou WebDAV implementation */
        sprintf(hPropfind->findProperty->shortValue, "%d", WDVCAPI_SUPPORTED_LOCK_EXCLUSIVE_WRITE);
    } else {
	    /* Close hStmt to prevent invalid cursor state          */
        /* If we have more then one row to fetch in the future  */
        /* we have to let open the hStmt                        */
        sqlRC = SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_CLOSE);

        /* Execute statement */
        sqlRC = SQLExecute( hPropfind->findProperty->hStmt );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		    SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		    hPropfind->findProperty->hStmt = 0;

		    return WDV_False;
	    }

	    /* Fetch rowset */
        sqlRC = SQLFetch( hPropfind->findProperty->hStmt );

	    /* Error handling */
	    if ( sqlRC != 0 ) {
            if (sqlRC == SQL_NO_DATA_FOUND) {
                hPropfind->findProperty->status = WDVCAPI_PROPERTY_STATUS_FAILED;

		        return WDV_True;
            } else {
                Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		        SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		        hPropfind->findProperty->hStmt = 0;

		        return WDV_False;
		    }
	    }
    }

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool PropfindOpenNavigationTree( WDVCAPI_WDV             wdv,
                                     WDVCAPI_PropfindHandle  hPropfind )
{

    WDVCAPI_Id              pid;
    WDVCAPI_Resource        resource = NULL;
    PropfindNavigationNode  newNavigationTree = NULL;

    TRACE_OUT(">> PropfindOpenNavigationTree()\n");

    /* Create resource object */
    if (!Resource_CreateHandle(wdv, &resource)) {
        return WDV_False;
    }

    /* Get URI resource */
    if (!Resource_GetByUri(wdv, hPropfind->uri, resource)) {
        Resource_DestroyHandle(wdv, resource);
        return WDV_False;
    }

    /* Get URI id */
    Resource_GetId(resource, pid);

    /* Destroy resource */
    Resource_DestroyHandle(wdv, resource);

    /* Store URI id */
    SAPDB_memcpy(hPropfind->uriId, pid, WDVCAPI_MAX_ID_LEN);

    /* Get name space list */
/*
    if (!PropfindGetNameSpaceList(wdv, hPropfind)) {
        return WDV_False;
    }
*/

    /* Create first navigation node object */
    if (!PropfindOpenNextChildURI(wdv, hPropfind, pid, hPropfind->uri)) {
        return WDV_False;
    }

    TRACE_OUT("<< PropfindOpenNavigationTree()\n");

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool CreateNavigationNode( WDVCAPI_WDV              wdv,
                               WDVCAPI_Id               pid,
                               WDVCAPI_URI              parentName,
                               PropfindNavigationNode  *newNavigationNode )
{

    SQLCHAR                 sqlStmt[128] = "";
    SQLHDBC                 hDBC = 0;
    SQLRETURN               sqlRC;
    SQLLEN                  bufferLength = 0;
	WDV_Bool                ok = WDV_False;
    PropfindNavigationNode  currentNode = NULL;
    WDVCAPI_IdString        idString = "";

    /* Allocat new navigation node */
	WDVCAPI_Allocat( sizeof(struct st_propfind_navigation_node), (WDV_UInt1**) newNavigationNode, &ok );
	if (ok == WDV_True) {
		/* Init propfind object */
		InitNavigationNode(wdv, *newNavigationNode);
	} else {
        Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                           WDVCAPI_ERR_CODE_NO_MEMORY,
                           WDVCAPI_ERR_TEXT_NO_MEMORY);

		return WDV_False;
	}

    currentNode = *newNavigationNode;

    /* Set first uri for this tree path */
    SAPDB_memcpy(currentNode->id, pid, WDVCAPI_MAX_ID_LEN);

    /* Set parentName */
    strncpy(currentNode->parentName, parentName, WDV_MAX_URI_LEN);
    currentNode->parentName[WDV_MAX_URI_LEN] = '\0';

    /* Init uriString with parent name,                 */
    /* because for the first open property              */
    /* this node will be used without fetching a child  */
    strncpy(currentNode->uriString, parentName, WDV_MAX_URI_LEN);
    currentNode->uriString[WDV_MAX_URI_LEN] = '\0';

    /* Convert pid to an id string */
    WDVCAPI_IdAsString(pid, idString);

    /* Build SQL statement */
    sp77sprintf(sqlStmt, 128, "Select CId, Name From WEBDAV_Inode Where PId = X'%s'", idString);
    /*
    sp77sprintf(sqlStmt, 128, "SELECT I.CId, I.Name, P.Property_Short_Value FROM WEBDAV_Inode I, WebDAV_Property P WHERE I.PId = X'%s' AND I.CId = P.CId AND P.Property_Id = x'"ID_PROPERTY_RESOURCE_TYPE_STRING"', idString);
    */

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &currentNode->hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, currentNode->hStmt, sqlRC );
		return WDV_False;
	}

	/* Prepare statement */
	sqlRC = SQLPrepare( currentNode->hStmt, sqlStmt, SQL_NTS );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, currentNode->hStmt, sqlRC );

		SQLFreeStmt(currentNode->hStmt, SQL_DROP);
		currentNode->hStmt = 0;

		return WDV_False;
	}

    /* Bind output columns */
    sqlRC = SQLBindCol(currentNode->hStmt, 1, SQL_C_BINARY,
                       currentNode->id, WDVCAPI_MAX_ID_LEN,
                      &currentNode->idIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, currentNode->hStmt, sqlRC );

		SQLFreeStmt(currentNode->hStmt, SQL_DROP);
		currentNode->hStmt = 0;

		return WDV_False;
	}

    /* Bind output columns */
    sqlRC = SQLBindCol(currentNode->hStmt, 2, SQL_C_CHAR,
                       currentNode->name, WDV_MAX_URI_LEN,
                      &currentNode->nameIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, currentNode->hStmt, sqlRC );

		SQLFreeStmt(currentNode->hStmt, SQL_DROP);
		currentNode->hStmt = 0;

		return WDV_False;
	}

    /*
    sqlRC = SQLBindCol(currentNode->hStmt, 3, SQL_C_CHAR,
                       currentNode->resourceType, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                      &currentNode->resourceTypeIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, currentNode->hStmt, sqlRC );

		SQLFreeStmt(currentNode->hStmt, SQL_DROP);
		currentNode->hStmt = 0;

		return WDV_False;
	}
    */

    /* Execute statement */
    sqlRC = SQLExecute( currentNode->hStmt );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, currentNode->hStmt, sqlRC );

		SQLFreeStmt(currentNode->hStmt, SQL_DROP);
		currentNode->hStmt = 0;

		return WDV_False;
	}

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool InitNavigationNode( WDVCAPI_WDV             wdv,
                             PropfindNavigationNode  navigationNode )
{

    navigationNode->hStmt = 0;

    memset(navigationNode->id, 0, WDVCAPI_MAX_ID_LEN);
    navigationNode->idIndicator = WDVCAPI_MAX_ID_LEN;

    memset(navigationNode->name, 0, WDV_MAX_URI_LEN);
    navigationNode->nameIndicator = SQL_NTS;

    memset(navigationNode->parentName, 0, WDV_MAX_URI_LEN);
    memset(navigationNode->uriString, 0, WDV_MAX_URI_LEN);

    navigationNode->childNodes = NULL;
    navigationNode->parentNode = NULL;

    return WDV_True;
}

/*===========================================================================*/

WDV_Bool DestroyNavigationNode( WDVCAPI_WDV             wdv,
                                PropfindNavigationNode  navigationNode )
{

    PropfindNavigationNode  nodeToDelete = NULL;
    PropfindNavigationNode  nextNode = NULL;

    nodeToDelete = navigationNode;
    while(nodeToDelete) {
        nextNode = nodeToDelete->childNodes;

        if (nodeToDelete->hStmt) {
            SQLFreeStmt(nodeToDelete->hStmt, SQL_DROP);
        }

        WDVCAPI_Free((WDV_UInt1*) nodeToDelete );

        nodeToDelete = nextNode;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool PropfindOpenNextSiblingURI( WDVCAPI_WDV             wdv,
                                     WDVCAPI_PropfindHandle  hPropfind )
{

    SQLRETURN sqlRC = 0;

    /* Checks */
    if (!wdv || !hPropfind || !hPropfind->currentNavigationNode ||
        !hPropfind->currentNavigationNode->hStmt) {

        Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				            WDVCAPI_ERR_CODE_INTERNAL_ERROR,
				            WDVCAPI_ERR_TEXT_INTERNAL_ERROR );

        return WDV_False;

    }


	/* Fetch rowset */
    sqlRC = SQLFetch( hPropfind->currentNavigationNode->hStmt );

	/* Error handling */
	if ( sqlRC != 0 ) {
        if (sqlRC == SQL_NO_DATA_FOUND) {
            Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
				                WDVCAPI_ERR_CODE_NO_MORE_SIBLING_NODES,
				                WDVCAPI_ERR_TEXT_NO_MORE_SIBLING_NODES );

        } else {
            Error_AddSQLErrorItem( wdv, hPropfind->currentNavigationNode->hStmt, sqlRC );
        }

		SQLFreeStmt(hPropfind->currentNavigationNode->hStmt, SQL_DROP);
		hPropfind->currentNavigationNode->hStmt = 0;

		return WDV_False;
	}

    /* Complete uri */
    if (strcmp(hPropfind->currentNavigationNode->parentName, WDV_PATH_SEPARATOR_STR) == 0) {
        sp77sprintf(hPropfind->currentNavigationNode->uriString,
                    WDV_MAX_URI_LEN, "%s%s",
                    WDV_PATH_SEPARATOR_STR,
                    hPropfind->currentNavigationNode->name);
    } else {
        sp77sprintf(hPropfind->currentNavigationNode->uriString,
                    WDV_MAX_URI_LEN, "%s%s%s",
                    hPropfind->currentNavigationNode->parentName,
                    WDV_PATH_SEPARATOR_STR,
                    hPropfind->currentNavigationNode->name);
    }

    /* Destroy child nodes */
    DestroyNavigationNode(wdv, hPropfind->currentNavigationNode->childNodes);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool PropfindOpenNextChildURI( WDVCAPI_WDV             wdv,
                                   WDVCAPI_PropfindHandle  hPropfind,
                                   WDVCAPI_Id              pid,
                                   WDVCAPI_URI             parentName )
{

    PropfindNavigationNode  newNavigationNode = NULL;
    PropfindNavigationNode  currentNavigationNode = NULL;

    if (!CreateNavigationNode(wdv, pid, parentName, &newNavigationNode)) {
        return WDV_False;
    }

    newNavigationNode->parentNode = hPropfind->currentNavigationNode;

    if (hPropfind->currentNavigationNode) {
        hPropfind->currentNavigationNode->childNodes = newNavigationNode;
    }

    hPropfind->currentNavigationNode = newNavigationNode;

    /* First node? */
    if (!hPropfind->uriNavigationTree) {
        hPropfind->uriNavigationTree = newNavigationNode;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool CreatePropfindProperty( WDVCAPI_WDV        wdv,
                                 PropfindProperty  *newProperty )
{

	WDV_Bool                ok = WDV_False;

	if ( !newProperty ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_propfind_property), (WDV_UInt1**) newProperty, &ok );
	if (ok == WDV_True) {
		/* Init hPropfind object */
		InitPropfindProperty(wdv, *newProperty);

		return WDV_True;
	} else {
		*newProperty = NULL;

		return WDV_False;
	}

}

/*===========================================================================*/

WDV_Bool DestroyPropfindProperty( WDVCAPI_WDV       wdv,
                                  PropfindProperty  property )
{

	if ( !property ) {
		return WDV_False;
	}

	/* Destroy property */
	WDVCAPI_Free((WDV_UInt1*) property);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool InitPropfindProperty( WDVCAPI_WDV       wdv,
                               PropfindProperty  property )
{

    memset(property->nameSpace, 0, WDV_MAX_NAME_SPACE_LEN+1);
    memset(property->name, 0, WDV_MAX_PROPERTY_NAME_PREFIX_LEN+1);

    property->nextProperty = NULL;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool CreatePropfindFindProperty( WDVCAPI_WDV            wdv,
                                     PropfindFindProperty  *newFindProperty )
{

	WDV_Bool                ok = WDV_False;

	if ( !newFindProperty ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_propfind_find_property), (WDV_UInt1**) newFindProperty, &ok );
	if (ok == WDV_True) {
		/* Init object */
		InitPropfindFindProperty(wdv, *newFindProperty);

		return WDV_True;
	} else {
		*newFindProperty = NULL;

		return WDV_False;
	}

}

/*===========================================================================*/

WDV_Bool DestroyPropfindFindProperty( WDVCAPI_WDV           wdv,
                                      PropfindFindProperty  findProperty )
{

	if ( !findProperty ) {
		return WDV_False;
	}

    if (findProperty->hStmt) {
        SQLFreeStmt(findProperty->hStmt, SQL_DROP);
    }

	/* Destroy find property */
	WDVCAPI_Free((WDV_UInt1*) findProperty);

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool InitPropfindFindProperty( WDVCAPI_WDV           wdv,
                                   PropfindFindProperty  findProperty )
{

    memset(findProperty->nameSpaceId, 0, WDVCAPI_MAX_ID_LEN);
    memset(findProperty->nameSpaceShortcut, 0, WDV_MAX_NAME_SPACE_SHORTCUT_LEN);
    memset(findProperty->nameSpace, 0, WDV_MAX_NAME_SPACE_LEN+1);
    memset(findProperty->name, 0, WDV_MAX_PROPERTY_NAME_PREFIX_LEN+1);

    memset(findProperty->shortValue, 0, WDV_MAX_PROPERTY_SHORT_VALUE_LEN+1);
    memset(findProperty->longValue, 0, WDV_MAX_PROPERTY_LONG_VALUE_LEN+1);
    findProperty->longValuePos = 0;

    findProperty->status = WDVCAPI_PROPERTY_STATUS_FAILED;

    findProperty->hStmt = 0;

    findProperty->nameSpaceIdIndicator = WDVCAPI_MAX_ID_LEN;
    findProperty->nameSpaceIndicator = SQL_NTS;
    findProperty->nameIndicator = SQL_NTS;
    findProperty->shortValueIndicator = SQL_NTS;

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool PropfindGetNameSpaceList( WDVCAPI_WDV             wdv,
                                   WDVCAPI_PropfindHandle  hPropfind )
{

    SQLCHAR                 sqlStmt[1024] = "";
    SQLHDBC                 hDBC = 0;
    SQLHSTMT                hStmt = 0;
    SQLRETURN               sqlRC;
    SQLLEN                  bufferLength = 0;
    WDVCAPI_IdString        idString = "";
    PropfindNameSpaceId     nameSpaceId;
    SQLLEN                  nameSpaceIdIndicator;
    PropfindNameSpaceString nameSpace;
    SQLLEN                  nameSpaceIndicator;
    PropfindProperty        currentProperty = NULL;
    PropfindNameSpace       currentNameSpace = NULL;
    WDV_Bool                found = WDV_False;

    /* Checks */
    if (!wdv || !hPropfind || !hPropfind->nameSpaceList) {
        return WDV_False;
    }

    /* Convert resource id to an id string */
    WDVCAPI_IdAsString(hPropfind->uriId, idString);

    /* Build SQL statement */
    sp77sprintf(sqlStmt, 1024, SQL_SELECT_NAME_SPACE_LIST, idString);

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		return WDV_False;
	}

    /* Execute statement */
	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

    /* Bind output columns */
    nameSpaceIdIndicator = WDVCAPI_MAX_ID_LEN;
    sqlRC = SQLBindCol(hStmt, 1, SQL_C_BINARY,
                       nameSpaceId, WDVCAPI_MAX_ID_LEN,
                      &nameSpaceIdIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );

		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

    nameSpaceIndicator = SQL_NTS;
    sqlRC = SQLBindCol(hStmt, 2, SQL_C_CHAR,
                       nameSpace, WDV_MAX_NAME_SPACE_LEN,
                      &nameSpaceIndicator );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );

		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

	/* Fetch name space list */
    while((sqlRC = SQLFetch(hStmt)) == 0) {
        if (!AddNewNameSpaceToList(wdv, hPropfind, nameSpaceId, nameSpace)) {
		    SQLFreeStmt(hStmt, SQL_DROP);

		    return WDV_False;
        }
    };

    /* Init current name space */
    hPropfind->nameSpaceList->currentNameSpace = NULL;

	/* Fetch error handling? */
	if (sqlRC != 0 && sqlRC != SQL_NO_DATA_FOUND) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );

		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

    /* Free statement handle */
	SQLFreeStmt(hStmt, SQL_DROP);

    /* If no name space could be found, we assume that the uri  */
    /* is an empty collection or an resource. In the future we  */
    /* we should get this info earlier (Resource_Get...)       */
	if (!hPropfind->nameSpaceList->firstNameSpace) {
        /* Build SQL statement */
        sp77sprintf(sqlStmt, 1024, SQL_SELECT_NAME_SPACE_LIST_FOR_RESOURCE, idString);

        /* Get ODBC handles */
        GetDBC(wdv, &hDBC);

	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hDBC, &hStmt);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		    return WDV_False;
	    }

        /* Execute statement */
	    sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		    SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		    hPropfind->findProperty->hStmt = 0;

		    return WDV_False;
	    }

        /* Bind output columns */
        nameSpaceIdIndicator = WDVCAPI_MAX_ID_LEN;
        sqlRC = SQLBindCol(hStmt, 1, SQL_C_BINARY,
                           nameSpaceId, WDVCAPI_MAX_ID_LEN,
                          &nameSpaceIdIndicator);
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hStmt, sqlRC );

		    SQLFreeStmt(hStmt, SQL_DROP);

		    return WDV_False;
	    }

        nameSpaceIndicator = SQL_NTS;
        sqlRC = SQLBindCol(hStmt, 2, SQL_C_CHAR,
                           nameSpace, WDV_MAX_NAME_SPACE_LEN,
                          &nameSpaceIndicator );
	    if ( sqlRC != 0 ) {
            Error_AddSQLErrorItem( wdv, hStmt, sqlRC );

		    SQLFreeStmt(hStmt, SQL_DROP);

		    return WDV_False;
	    }

	    /* Fetch name space list */
        while((sqlRC = SQLFetch(hStmt)) == 0) {
            if (!AddNewNameSpaceToList(wdv, hPropfind, nameSpaceId, nameSpace)) {
		        SQLFreeStmt(hStmt, SQL_DROP);

		        return WDV_False;
            }
        };

        /* Init current name space */
        hPropfind->nameSpaceList->currentNameSpace = NULL;

	    /* Fetch error handling? */
	    if (sqlRC != 0 && sqlRC != SQL_NO_DATA_FOUND) {
            Error_AddSQLErrorItem( wdv, hStmt, sqlRC );

		    SQLFreeStmt(hStmt, SQL_DROP);

		    return WDV_False;
	    }

        /* Free statement handle */
	    SQLFreeStmt(hStmt, SQL_DROP);
    }

    /* Every property found in our WebDAV server? */
    currentProperty = hPropfind->propertyList->firstProperty;
    while (currentProperty) {
        /* Try to find name space of property in the name space list */
        found = WDV_False;
        currentNameSpace = hPropfind->nameSpaceList->firstNameSpace;
        while (currentNameSpace) {
            /* Is name space of property equal with current name space? */
            if (strcmp(currentProperty->nameSpace, currentNameSpace->nameSpace) == 0) {
                found = WDV_True;
                break;
            }

            /* Get next name space in name space list */
            currentNameSpace = currentNameSpace->nextNameSpace;
        }

        /* Could we found the name space in list? */
        if (found == WDV_False) {
            /* Get next unique id */
            if (!WDVCAPI_IdGetNext(wdv, nameSpaceId)) {
                return WDV_False;
            }

            /* Add new name space to the list */
            if (!AddNewNameSpaceToList(wdv, hPropfind, nameSpaceId, currentProperty->nameSpace)) {
                return WDV_False;
            }
        }

        /* Get next property */
        currentProperty = currentProperty->nextProperty;
    }

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool AddNewNameSpaceToList( WDVCAPI_WDV                 wdv,
                                WDVCAPI_PropfindHandle      hPropfind,
                                PropfindNameSpaceId         nameSpaceId,
                                WDVCAPI_PropfindNameSpace   nameSpace )
{

    PropfindNameSpace   nameSpaceObject = NULL;

    nameSpaceObject = NULL;
    if (!CreatePropfindNameSpace(wdv, &nameSpaceObject)) {
		return WDV_False;
    }

    /* Set new name space */
    SAPDB_memcpy(nameSpaceObject->nameSpaceId, nameSpaceId, WDVCAPI_MAX_ID_LEN);
    WDVCAPI_IdAsString(nameSpaceId, nameSpaceObject->nameSpaceShortcut);

    strncpy(nameSpaceObject->nameSpace, nameSpace, WDV_MAX_NAME_SPACE_LEN);
    nameSpaceObject->nameSpace[WDV_MAX_NAME_SPACE_LEN] = '\0';

    /* Add new name space to the name space list */
    if (!hPropfind->nameSpaceList->firstNameSpace) {
        hPropfind->nameSpaceList->firstNameSpace = nameSpaceObject;
        hPropfind->nameSpaceList->lastNameSpace = nameSpaceObject;
    } else {
        hPropfind->nameSpaceList->lastNameSpace->nextNameSpace = nameSpaceObject;
        hPropfind->nameSpaceList->lastNameSpace = nameSpaceObject;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool CreatePropfindNameSpace( WDVCAPI_WDV        wdv,
                                  PropfindNameSpace *nameSpace )
{

	WDV_Bool          ok = WDV_False;
    PropfindNameSpace newNameSpace = NULL;

	if ( !nameSpace ) {
		return WDV_False;
	}
    
	WDVCAPI_Allocat( sizeof(struct st_propfind_name_space), (WDV_UInt1**) &newNameSpace, &ok );
	if (ok == WDV_True) {
		/* Init propfind name space object */
		InitPropfindNameSpace(wdv, newNameSpace);

		*nameSpace = newNameSpace;

		return WDV_True;
	} else {
		*nameSpace = NULL;

		return WDV_False;
	}

}

/*===========================================================================*/

WDV_Bool DestroyPropfindNameSpace( WDVCAPI_WDV        wdv,
                                   PropfindNameSpace  nameSpace )
{

	PropfindNameSpace   nameSpaceToDelete = NULL;

	if ( !nameSpace ) {
		return WDV_False;
	}

	/* Destroy name space */
    WDVCAPI_Free((WDV_UInt1*) nameSpace );

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool InitPropfindNameSpace( WDVCAPI_WDV        wdv,
                                PropfindNameSpace  nameSpace )
{

    memset(nameSpace->nameSpaceId, 0, WDVCAPI_MAX_ID_LEN);
    memset(nameSpace->nameSpaceShortcut, 0, WDV_MAX_NAME_SPACE_SHORTCUT_LEN+1);
    memset(nameSpace->nameSpace, 0, WDV_MAX_NAME_SPACE_LEN+1);

    nameSpace->nextNameSpace = NULL;

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool GetNameSpaceShortcutFromNameSpaceList( PropfindNameSpaceList               nameSpaceList,
                                                WDVCAPI_PropfindNameSpace           nameSpace,
                                                WDVCAPI_PropfindNameSpaceShortcut   nameSpaceShortcut )
{

    PropfindNameSpace   currentNameSpace = NULL;

    /* Search name space in name space list */
    currentNameSpace = nameSpaceList->firstNameSpace;
    while (currentNameSpace) {
        /* Name space found? */
        if (strcmp(currentNameSpace->nameSpace, nameSpace) == 0) {
            /* Get shortcut of name space */
            strcpy(nameSpaceShortcut, currentNameSpace->nameSpaceShortcut);

            return WDV_True;
        }

        /* Get next name space */
        currentNameSpace = currentNameSpace->nextNameSpace;
    }

    return WDV_False;

}

/*===========================================================================*/

WDV_Bool Propfind_AddToNameSpaceList( WDVCAPI_WDV                  wdv,
                                      WDVCAPI_PropfindHandle       hPropfind,
                                      WDVCAPI_NameSpace            nameSpace,
                                      WDVCAPI_NameSpaceShortcut   *nameSpaceShortcut )
{

	WDV_Bool                    ok = WDV_False;
    WDVCAPI_NameSpaceShortcut   foundNameSpaceShortcut = NULL;
    PropfindNameSpace           newNameSpace = NULL;

	if (!wdv || !hPropfind || !nameSpace) {
		return WDV_False;
	}

    /* Name space already exists? */
    if (!Propfind_SearchNameSpaceInList(wdv, hPropfind->nameSpaceList->firstNameSpace,
                                        nameSpace, &foundNameSpaceShortcut)) {
        return WDV_False;
    }
    if (foundNameSpaceShortcut) {
        if (nameSpaceShortcut) {
            *nameSpaceShortcut = foundNameSpaceShortcut;
        }
        return WDV_True;
    }
    
	WDVCAPI_Allocat( sizeof(struct st_propfind_name_space),
                (WDV_UInt1**) &newNameSpace, &ok );
	if (ok == WDV_False) {
		return WDV_False;
	}

	/* Init new name space */
	if (!Propfind_InitNameSpace(wdv, hPropfind, newNameSpace, nameSpace, nameSpaceShortcut)) {
        /* Free new name space */
        WDVCAPI_Free((WDV_UInt1*) newNameSpace);

        return WDV_False;
    }

    /* Do we have an initial name space? */
    if (!hPropfind->nameSpaceList->firstNameSpace) {
        hPropfind->nameSpaceList->firstNameSpace  = newNameSpace;
    } else {
        hPropfind->nameSpaceList->lastNameSpace->nextNameSpace = newNameSpace;
    }

    /* Store last name space */
    hPropfind->nameSpaceList->lastNameSpace  = newNameSpace;

	return WDV_True;


}

/*===========================================================================*/

WDV_Bool Propfind_InitNameSpace( WDVCAPI_WDV                wdv,
                                 WDVCAPI_PropfindHandle     hPropfind,
                                 PropfindNameSpace          newNameSpace,
                                 WDVCAPI_NameSpace          nameSpace,
                                 WDVCAPI_NameSpaceShortcut *nameSpaceShortcut )
{

    WDVCAPI_Id  newId;

	if (!wdv || !newNameSpace || !nameSpace) {
		return WDV_False;
	}

    /* Initialize new name space */
    strncpy(newNameSpace->nameSpace, nameSpace, WDV_MAX_NAME_SPACE_LEN);
    newNameSpace->nameSpace[WDV_MAX_NAME_SPACE_LEN] = '\0';

    /* Get new shortcut id */
    hPropfind->nameSpaceShortcutCounter++;
    sp77sprintf(newNameSpace->nameSpaceShortcut,
                WDV_MAX_NAME_SPACE_SHORTCUT_LEN,
                "%d", hPropfind->nameSpaceShortcutCounter);

    /* Init next pointer */
    newNameSpace->nextNameSpace = NULL;

    /* Set return value */
    if (nameSpaceShortcut) {
        *nameSpaceShortcut = newNameSpace->nameSpaceShortcut;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_SearchNameSpaceInList( WDVCAPI_WDV                wdv,
                                         PropfindNameSpace          nameSpaceList,
                                         WDVCAPI_NameSpace          nameSpaceToFind,
                                         WDVCAPI_NameSpaceShortcut *foundNameSpaceShortcut )
{

    PropfindNameSpace  currentNameSpace = NULL;

    /* Checks */
    if (!wdv || !nameSpaceToFind || !foundNameSpaceShortcut) {
        if (!wdv) {
		    Error_AddErrorItem( wdv, WDVCAPI_ERR_TYPE_CAPI,
                                WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                                WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        if (foundNameSpaceShortcut) {
            *foundNameSpaceShortcut = NULL;
        }

        return WDV_False;
    }

    /* Inits */
    *foundNameSpaceShortcut = NULL;

    /* Search name space list */
    currentNameSpace = nameSpaceList;
    while(currentNameSpace) {
        if (strcmp(currentNameSpace->nameSpace, nameSpaceToFind) == 0) {
            *foundNameSpaceShortcut = currentNameSpace->nameSpaceShortcut;
            return WDV_True;
        }

        /* Get next name space from list */
        currentNameSpace = currentNameSpace->nextNameSpace;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_GetAllNameSpaces( WDVCAPI_WDV             wdv,
                                    WDVCAPI_PropfindHandle  hPropfind )
{

    SQLCHAR                 sqlStmt[1024] = "";
    SQLHDBC                 hDBC = 0;
    SQLHSTMT                hStmt = 0;
    SQLRETURN               sqlRC;
    PropfindNameSpaceString nameSpace;
    SQLLEN                  nameSpaceIndicator;

    /* Checks */
    if (!wdv || !hPropfind || !hPropfind->nameSpaceList) {
        return WDV_False;
    }

    /* Build SQL statement */
    Common_StrMaxCopy((char*) sqlStmt, SQL_SELECT_ALL_NAME_SPACES, 1023);

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		return WDV_False;
	}

    /* Execute statement */
	sqlRC = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

    /* Bind output columns */
    nameSpaceIndicator = SQL_NTS;
    sqlRC = SQLBindCol(hStmt, 1, SQL_C_CHAR,
                       nameSpace, WDV_MAX_NAME_SPACE_LEN,
                      &nameSpaceIndicator );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

	/* Fetch name space list */
    while((sqlRC = SQLFetch(hStmt)) == 0) {
        if (!Propfind_AddToNameSpaceList(wdv, hPropfind, nameSpace, NULL)) {
		    SQLFreeStmt(hStmt, SQL_DROP);

		    return WDV_False;
        }
    };

    /* Init current name space */
    hPropfind->nameSpaceList->currentNameSpace = NULL;

	/* Fetch error handling? */
	if (sqlRC != 0 && sqlRC != SQL_NO_DATA_FOUND) {
        Error_AddSQLErrorItem( wdv, hStmt, sqlRC );
		SQLFreeStmt(hStmt, SQL_DROP);

		return WDV_False;
	}

    /* Free statement handle */
	SQLFreeStmt(hStmt, SQL_DROP);

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_OpenAll( WDVCAPI_WDV             wdv,
                           WDVCAPI_PropfindHandle  hPropfind )
{

    SQLCHAR             sqlStmt[2048] = "";
    SQLHDBC             hDBC = 0;
    SQLRETURN           sqlRC;
    SQLLEN              bufferLength = 0;
    SQLINTEGER          indi = SQL_NTS;
    WDVCAPI_IdString    idString = "";
    WDVCAPI_Resource    currentResource = NULL;

    /* Checks */
    if (!wdv || !hPropfind || !hPropfind->currentNavigationNode ||
        !hPropfind->findProperty || !hPropfind->propertyList ) {

        return WDV_False;

    }

    /* Create resource object */
    if (!Resource_CreateHandle(wdv, &currentResource)) {
        return WDV_False;
    }

    /* Get URI resource */
    if (!Resource_GetByUri(wdv, hPropfind->currentNavigationNode->uriString, currentResource)) {
        Resource_DestroyHandle(wdv, currentResource);
        return WDV_False;
    }

    /* Get lock if there is one in the URI? */
    Propfind_ClearLockDescription(wdv, hPropfind);
    Resource_IsLockInURI(wdv, currentResource, hPropfind->currentLockDescription);

    /* Destroy resource handle */
    Resource_DestroyHandle(wdv, currentResource);

    /* Convert resource id to an id string */
    WDVCAPI_IdAsString(hPropfind->currentNavigationNode->id, idString);

    /* Build SQL statement */
    if (strcmp(currentResource->resourceType, "null resource") == 0) {
        sp77sprintf(sqlStmt, 2048, SQL_SELECT_ALL_PROPERTIES_OF_NULL_RESOURCE, idString);
    } else {
        sp77sprintf(sqlStmt, 2048, SQL_SELECT_ALL_PROPERTIES, idString);
    }

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Free existing statement handle */
    if (hPropfind->findProperty->hStmt) {
	    SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
        hPropfind->findProperty->hStmt = 0;
	}

    /* Reset property list pos */
    hPropfind->propertyList->currentProperty = NULL;

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hPropfind->findProperty->hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );
		return WDV_False;
	}

	/* Prepare statement */
	sqlRC = SQLPrepare( hPropfind->findProperty->hStmt, sqlStmt, SQL_NTS );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

    /* Bind output columns */
    sqlRC = SQLBindCol(hPropfind->findProperty->hStmt, 1, SQL_C_CHAR,
                       hPropfind->findProperty->nameSpace, WDV_MAX_NAME_SPACE_LEN,
                      &hPropfind->findProperty->nameSpaceIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

    sqlRC = SQLBindCol(hPropfind->findProperty->hStmt, 2, SQL_C_CHAR,
                       hPropfind->findProperty->name, WDV_MAX_PROPERTY_NAME_PREFIX_LEN,
                      &hPropfind->findProperty->nameIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

    sqlRC = SQLBindCol(hPropfind->findProperty->hStmt, 3, SQL_C_CHAR,
                       hPropfind->findProperty->shortValue, WDV_MAX_PROPERTY_SHORT_VALUE_LEN,
                      &hPropfind->findProperty->shortValueIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

    /* Execute statement */
    sqlRC = SQLExecute( hPropfind->findProperty->hStmt );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_OpenName( WDVCAPI_WDV             wdv,
                            WDVCAPI_PropfindHandle  hPropfind )
{

    SQLCHAR             sqlStmt[2048] = "";
    SQLHDBC             hDBC = 0;
    SQLRETURN           sqlRC;
    SQLLEN              bufferLength = 0;
    SQLINTEGER          indi = SQL_NTS;
    WDVCAPI_IdString    idString = "";
    WDVCAPI_Resource    currentResource = NULL;

    /* Checks */
    if (!wdv || !hPropfind || !hPropfind->currentNavigationNode ||
        !hPropfind->findProperty || !hPropfind->propertyList ) {

        return WDV_False;

    }

    /* Create resource object */
    if (!Resource_CreateHandle(wdv, &currentResource)) {
        return WDV_False;
    }

    /* Get URI resource */
    if (!Resource_GetByUri(wdv, hPropfind->currentNavigationNode->uriString, currentResource)) {
        Resource_DestroyHandle(wdv, currentResource);
        return WDV_False;
    }

    /* Get lock if there is one in the URI? */
    Propfind_ClearLockDescription(wdv, hPropfind);
    Resource_IsLockInURI(wdv, currentResource, hPropfind->currentLockDescription);

    /* Destroy resource handle */
    Resource_DestroyHandle(wdv, currentResource);

    /* Convert resource id to an id string */
    WDVCAPI_IdAsString(hPropfind->currentNavigationNode->id, idString);

    /* Build SQL statement */
    if (strcmp(currentResource->resourceType, "null resource") == 0) {
        sp77sprintf(sqlStmt, 2048, SQL_SELECT_ALL_PROPERTY_NAMES_OF_NULL_RESOURCE, idString);
    } else {
        sp77sprintf(sqlStmt, 2048, SQL_SELECT_ALL_PROPERTY_NAMES, idString);
    }

    /* Get ODBC handles */
    GetDBC(wdv, &hDBC);

	/* Free existing statement handle */
    if (hPropfind->findProperty->hStmt) {
	    SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
        hPropfind->findProperty->hStmt = 0;
	}

    /* Reset property list pos */
    hPropfind->propertyList->currentProperty = NULL;

	/* Allocat statement handle */
	sqlRC = SQLAllocStmt(hDBC, &hPropfind->findProperty->hStmt);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );
		return WDV_False;
	}

	/* Prepare statement */
	sqlRC = SQLPrepare( hPropfind->findProperty->hStmt, sqlStmt, SQL_NTS );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

    /* Bind output columns */
    sqlRC = SQLBindCol(hPropfind->findProperty->hStmt, 1, SQL_C_CHAR,
                       hPropfind->findProperty->nameSpace, WDV_MAX_NAME_SPACE_LEN,
                      &hPropfind->findProperty->nameSpaceIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

    sqlRC = SQLBindCol(hPropfind->findProperty->hStmt, 2, SQL_C_CHAR,
                       hPropfind->findProperty->name, WDV_MAX_PROPERTY_NAME_PREFIX_LEN,
                      &hPropfind->findProperty->nameIndicator);
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

    /* Execute statement */
    sqlRC = SQLExecute( hPropfind->findProperty->hStmt );
	if ( sqlRC != 0 ) {
        Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		hPropfind->findProperty->hStmt = 0;

		return WDV_False;
	}

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_Propfind( WDVCAPI_WDV             wdv,
                            WDVCAPI_PropfindHandle  hPropfind )
{

    /* Open navigation tree */
    if (!PropfindOpenNavigationTree(wdv, hPropfind)) {
        return WDV_False;
    }

    /* Open result for first uri */
    if (!PropfindOpen(wdv, hPropfind)) {
        return WDV_False;
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_PropfindAll( WDVCAPI_WDV             wdv,
                               WDVCAPI_PropfindHandle  hPropfind )
{

    TRACE_OUT(">> Propfind_PropfindAll()\n");

    /* Reset handle */
    if (!Propfind_ResetHandle(wdv, hPropfind)) {
        return WDV_False;
    }

    /* Get list of name spaces */
    if (!Propfind_GetAllNameSpaces(wdv, hPropfind)) {
        return WDV_False;
    }

    /* Open navigation tree */
    if (!PropfindOpenNavigationTree(wdv, hPropfind)) {
        return WDV_False;
    }

    /* Init statement handle to force an call of GetFirstProperty */
    if (hPropfind->findProperty->hStmt) {
        SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
        hPropfind->findProperty->hStmt = 0; 
    }

    TRACE_OUT("<< Propfind_PropfindAll()\n");

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_PropfindName( WDVCAPI_WDV             wdv,
                                WDVCAPI_PropfindHandle  hPropfind )
{

    /* Reset handle */
    if (!Propfind_ResetHandle(wdv, hPropfind)) {
        return WDV_False;
    }

    /* Get list of name spaces */
    if (!Propfind_GetAllNameSpaces(wdv, hPropfind)) {
        return WDV_False;
    }

    /* Open navigation tree */
    if (!PropfindOpenNavigationTree(wdv, hPropfind)) {
        return WDV_False;
    }

    /* Init statement handle to force an call of GetFirstProperty */
    if (hPropfind->findProperty->hStmt) {
        SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
        hPropfind->findProperty->hStmt = 0; 
    }

    return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_OpenPropertyAll( WDVCAPI_WDV             wdv,
                                   WDVCAPI_PropfindHandle  hPropfind )
{

    SQLRETURN                   sqlRC;
    WDVCAPI_NameSpaceShortcut   nameSpaceShortcut = NULL;

    /* Checks */
    if (!wdv || !hPropfind || !hPropfind->findProperty->hStmt) {
        if (wdv) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                               WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        return WDV_False;
    }

    /* Inits */
    hPropfind->findProperty->status = WDVCAPI_PROPERTY_STATUS_FAILED;
    memset(hPropfind->findProperty->nameSpaceId, 0, WDVCAPI_MAX_ID_LEN);
    hPropfind->findProperty->nameSpaceShortcut[0] = '\0';
    hPropfind->findProperty->nameSpace[0] = '\0';
    hPropfind->findProperty->name[0] = '\0';
    hPropfind->findProperty->shortValue[0] = '\0';
    hPropfind->findProperty->longValue[0] = '\0';
    hPropfind->findProperty->longValuePos = 0;

    /* We handled the static property list, don't we? */
    while (hPropfind->findProperty->name[0] == '\0' && staticPropertyNameList[hPropfind->staticPropertyNameListIndex]) {
        /* Is the asked property the lockdiscovery property,          */
        /* so check whether we already have a lock description or not */
        if (strcmp(staticPropertyNameList[hPropfind->staticPropertyNameListIndex], PROPERTY_NAME_LOCK_DISCOVERY) == 0) {
            /* Get stored lock id. The client will get the whole lock description */
            /* with calling the function WDVCAPI_PropfindGetLockDescription().     */
            WDVCAPI_IdAsString(hPropfind->currentLockDescription->id, hPropfind->findProperty->shortValue);
		    if (WDVCAPI_IdStringIsInitialValue(hPropfind->findProperty->shortValue) == WDV_False) {
                strcpy(hPropfind->findProperty->nameSpace, WDV_NAME_SPACE_DAV);
                strcpy(hPropfind->findProperty->name, staticPropertyNameList[hPropfind->staticPropertyNameListIndex]);
            }
        } else if (strcmp(staticPropertyNameList[hPropfind->staticPropertyNameListIndex], PROPERTY_NAME_SUPPORTED_LOCK) == 0) {
            /* Is the asked property the supported lock property? */
    
            /* Get supported lock property. This is static in this version of our WebDAV implementation */
            sprintf(hPropfind->findProperty->shortValue, "%d", WDVCAPI_SUPPORTED_LOCK_EXCLUSIVE_WRITE);

            strcpy(hPropfind->findProperty->nameSpace, WDV_NAME_SPACE_DAV);
            strcpy(hPropfind->findProperty->name, staticPropertyNameList[hPropfind->staticPropertyNameListIndex]);
        }

        /* Increment the list index */
        hPropfind->staticPropertyNameListIndex++;
    }

    if (hPropfind->findProperty->name[0] == '\0') {
	    /* Fetch rowset */
        sqlRC = SQLFetch( hPropfind->findProperty->hStmt );

	    /* Error handling */
	    if ( sqlRC != 0 ) {
            hPropfind->findProperty->status = WDVCAPI_PROPERTY_STATUS_FAILED;
            if (sqlRC == SQL_NO_DATA_FOUND) {
                Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                   WDVCAPI_ERR_CODE_NO_MORE_RESOURCES,
                                   WDVCAPI_ERR_TEXT_NO_MORE_RESOURCES);

		        return WDV_False;
            } else {
                Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		        SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		        hPropfind->findProperty->hStmt = 0;

		        return WDV_False;
		    }
	    }
    }

    /* Get name space shortcut to given name space */
    if (!Propfind_SearchNameSpaceInList(wdv, hPropfind->nameSpaceList->firstNameSpace,
                                            hPropfind->findProperty->nameSpace,
                                            &nameSpaceShortcut)) {

        return WDV_True;
    }

    if (nameSpaceShortcut) {
        strcpy(hPropfind->findProperty->nameSpaceShortcut, nameSpaceShortcut);
        hPropfind->findProperty->status = WDVCAPI_PROPERTY_STATUS_OK;
    }

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_OpenPropertyNames( WDVCAPI_WDV             wdv,
                                     WDVCAPI_PropfindHandle  hPropfind )
{

    SQLRETURN                   sqlRC;
    WDVCAPI_NameSpaceShortcut   nameSpaceShortcut = NULL;

    /* Checks */
    if (!wdv || !hPropfind || !hPropfind->findProperty->hStmt) {
        if (wdv) {
            Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                               WDVCAPI_ERR_CODE_INTERNAL_ERROR,
                               WDVCAPI_ERR_TEXT_INTERNAL_ERROR);
        }

        return WDV_False;
    }

    /* Inits */
    hPropfind->findProperty->status = WDVCAPI_PROPERTY_STATUS_FAILED;
    memset(hPropfind->findProperty->nameSpaceId, 0, WDVCAPI_MAX_ID_LEN);
    hPropfind->findProperty->nameSpaceShortcut[0] = '\0';
    hPropfind->findProperty->nameSpace[0] = '\0';
    hPropfind->findProperty->name[0] = '\0';
    hPropfind->findProperty->shortValue[0] = '\0';
    hPropfind->findProperty->longValue[0] = '\0';
    hPropfind->findProperty->longValuePos = 0;

    /* We handled the static property list, don't we? */
    if (staticPropertyNameList[hPropfind->staticPropertyNameListIndex]) {
        strcpy(hPropfind->findProperty->nameSpace, WDV_NAME_SPACE_DAV);
        strcpy(hPropfind->findProperty->name, staticPropertyNameList[hPropfind->staticPropertyNameListIndex]);

        /* Increment the list index */
        hPropfind->staticPropertyNameListIndex++;
    } else {
	    /* Fetch rowset */
        sqlRC = SQLFetch( hPropfind->findProperty->hStmt );

	    /* Error handling */
	    if ( sqlRC != 0 ) {
            if (sqlRC == SQL_NO_DATA_FOUND) {
                hPropfind->findProperty->status = WDVCAPI_PROPERTY_STATUS_FAILED;
                Error_AddErrorItem(wdv, WDVCAPI_ERR_TYPE_CAPI,
                                   WDVCAPI_ERR_CODE_NO_MORE_RESOURCES,
                                   WDVCAPI_ERR_TEXT_NO_MORE_RESOURCES);

		        return WDV_False;
            } else {
                Error_AddSQLErrorItem( wdv, hPropfind->findProperty->hStmt, sqlRC );

		        SQLFreeStmt(hPropfind->findProperty->hStmt, SQL_DROP);
		        hPropfind->findProperty->hStmt = 0;

		        return WDV_False;
		    }
	    }
    }

    /* Get name space shortcut to given name space */
    if (!Propfind_SearchNameSpaceInList(wdv, hPropfind->nameSpaceList->firstNameSpace,
                                            hPropfind->findProperty->nameSpace,
                                            &nameSpaceShortcut)) {
        hPropfind->findProperty->status = WDVCAPI_PROPERTY_STATUS_FAILED;

        return WDV_True;
    }

    if (nameSpaceShortcut) {
        strcpy(hPropfind->findProperty->nameSpaceShortcut, nameSpaceShortcut);
    }

    /* Set status OK */
    hPropfind->findProperty->status = WDVCAPI_PROPERTY_STATUS_OK;

	/* Return */
	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_GetFirstProperty( WDVCAPI_WDV              wdv,
                                    WDVCAPI_PropfindHandle   hPropfind )
{

    /* Checks */
    if (!hPropfind || !hPropfind->propertyList || !hPropfind->propertyList->firstProperty) {
        return WDV_False;
    }

    /* Set list pointer to the first property */
    hPropfind->propertyList->currentProperty = hPropfind->propertyList->firstProperty;

    /* Get next property */
    if(!PropfindOpenProperty(wdv, hPropfind)) {
        WDV_False;
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_GetNextProperty( WDVCAPI_WDV              wdv,
                                   WDVCAPI_PropfindHandle   hPropfind )
{

    /* Checks */
    if (!hPropfind || !hPropfind->propertyList) {
        return WDV_False;
    }

    /* Start of the list? */
    if (!hPropfind->propertyList->currentProperty) {
        hPropfind->propertyList->currentProperty = hPropfind->propertyList->firstProperty;
    } else {
        /* Set list pointer to the next property */
        hPropfind->propertyList->currentProperty = hPropfind->propertyList->currentProperty->nextProperty;
    }

    /* Get next property */
    if(!PropfindOpenProperty(wdv, hPropfind)) {
        return WDV_False;
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_GetFirstOfAllProperties( WDVCAPI_WDV              wdv,
                                           WDVCAPI_PropfindHandle   hPropfind )
{

    /* Open list of properties */
    if(!Propfind_OpenAll(wdv, hPropfind)) {
        return WDV_False;
    }

    /* Set static property name list index to first entry */
    hPropfind->staticPropertyNameListIndex = 0;

    /* Get first property */
    if(!Propfind_OpenPropertyAll(wdv, hPropfind)) {
        return WDV_False;
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_GetNextOfAllProperties( WDVCAPI_WDV              wdv,
                                          WDVCAPI_PropfindHandle   hPropfind )
{

    /* Get next property */
    if(!Propfind_OpenPropertyAll(wdv, hPropfind)) {
        return WDV_False;
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_GetFirstOfAllPropertyNames( WDVCAPI_WDV              wdv,
                                              WDVCAPI_PropfindHandle   hPropfind )
{

    /* Open list of properties */
    if(!Propfind_OpenName(wdv, hPropfind)) {
        WDV_False;
    }

    /* Set static property name list index to first entry */
    hPropfind->staticPropertyNameListIndex = 0;

    /* Get first property */
    if(!Propfind_OpenPropertyNames(wdv, hPropfind)) {
        return WDV_False;
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_GetNextOfAllPropertyNames( WDVCAPI_WDV              wdv,
                                             WDVCAPI_PropfindHandle   hPropfind )
{

    /* Get next property */
    if(!Propfind_OpenPropertyNames(wdv, hPropfind)) {
        return WDV_False;
    }

	return WDV_True;

}

/*===========================================================================*/

WDV_Bool Propfind_ClearLockDescription( WDVCAPI_WDV            wdv,
				                        WDVCAPI_PropfindHandle hPropfind )
{

    if (!Lock_InitHandle(wdv, hPropfind->currentLockDescription)) {
        return WDV_False;
    }

    return WDV_True;

}

/***********************************************************************

	End

 ***********************************************************************/
