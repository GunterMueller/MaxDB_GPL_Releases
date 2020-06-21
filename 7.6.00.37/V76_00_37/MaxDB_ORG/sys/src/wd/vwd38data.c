/*!**********************************************************************

  module: vwd38data.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Managing list of user-data.

  see also:

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 2000-2005 SAP AG




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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "hwd38data.h"


/****************************************************************************
 ***
 *** Structs
 ***
 ****************************************************************************/

typedef struct st_wa_user_data
{
	sapdbwa_UInt4		 id;
    void				*data;
    sapdbwa_Destructor	 destructor;
	twd38UserDataP		 nextUserData;
} twd38UserData;

/****************************************************************************
 ***
 *** Prototypes
 ***
 ****************************************************************************/

twd38UserDataP wd38_FindUserData( twd38UserDataP	userDataList,
								  sapdbwa_UInt4		id             );

/****************************************************************************
 ***
 *** wd38NewUserData
 ***
 ****************************************************************************/

sapdbwa_Bool wd38NewUserData( twd38UserDataP	*userDataList,
                            sapdbwa_UInt4		 id,
                            void			*data,
                            sapdbwa_Destructor	 destructor   )
{

	twd38UserDataP	*currentItem;
	twd38UserDataP	 currentUserData;
    sapdbwa_Bool		 allocatOk;

	/* Is user data already in list? */
	currentUserData = wd38_FindUserData( *userDataList, id );

	/* If user data is already in list, update user data */
	if ( currentUserData ) {
		currentUserData->data = data;
		currentUserData->destructor = destructor;

		return true;
	}

	/* Find end of list */
	currentItem = userDataList;
	currentUserData = *currentItem;
	while ( currentUserData != NULL ) {
		currentItem = &(currentUserData->nextUserData);
		currentUserData = *currentItem;
	}

	/* Allocate memory for new user data container item */
    sapdbwa_SQLALLOCAT( sizeof(twd38UserData), (sapdbwa_UInt1**) currentItem, &allocatOk );
	if ( !allocatOk ) {
		return false;
	}

	currentUserData = *currentItem;
	currentUserData->id = id;
	currentUserData->data = data;
	currentUserData->destructor = destructor;
	currentUserData->nextUserData = NULL;

    return true;

}

/****************************************************************************
 ***
 *** wd38GetUserData
 ***
 ****************************************************************************/

void* wd38GetUserData( twd38UserDataP	userDataList,
                       sapdbwa_UInt4		id )
{

	twd38UserDataP	foundUserData = NULL;

	if (( foundUserData = wd38_FindUserData( userDataList, id )) == NULL ) {
        return NULL;
    };
    
    return foundUserData->data;

}

/****************************************************************************
 ***
 *** wd38FreeUserData
 ***
 ****************************************************************************/

sapdbwa_Bool wd38FreeUserData( twd38UserDataP	*userDataList,
							 sapdbwa_UInt4	 id            )
{
	twd38UserDataP	*lastItem;
	twd38UserDataP	*currentItem;
	twd38UserDataP	 lastUserData;
	twd38UserDataP	 currentUserData;
    sapdbwa_Bool		 found;

	/* Find user data in list */
	found = false;
	lastItem = userDataList;
	currentItem = userDataList;
	currentUserData = *userDataList;
	while ( !found ) {
		if ( currentUserData && currentUserData->id != id ) {
			lastItem = currentItem;
			currentItem = &(currentUserData->nextUserData);
			currentUserData = *currentItem;
		} else
			found = true;
	}

	/* Service found? */
	if ( found ) {
		/* Set new list pointers */
		if ( currentItem == userDataList ) {
			*userDataList = currentUserData->nextUserData;
		} else {
			lastUserData = *lastItem;
			lastUserData->nextUserData = currentUserData->nextUserData;
		}

        /* call the user defined destructor */
        if (currentUserData->destructor != NULL) {
            currentUserData->destructor( currentUserData->data );
        };

		/* Free memory for list item */
		sapdbwa_SQLFREE( (sapdbwa_UInt1*) currentUserData );

	    return true;
	}

    return false;

}

/****************************************************************************
 ***
 *** wd38FreeAllUserDatas
 ***
 ****************************************************************************/

sapdbwa_Bool wd38FreeAllUserDatas( twd38UserDataP	*userDataList )
{

	twd38UserDataP	currentUserData = NULL;
	sapdbwa_Bool		rc = true;

	currentUserData = *userDataList;
	while( currentUserData ) {
		if( !wd38FreeUserData( userDataList, currentUserData->id )) {
			rc = false;
		}
		currentUserData = *userDataList;
	}

	return rc;

}

/****************************************************************************
 ***
 *** wd38_FindUserData
 ***
 ****************************************************************************/

twd38UserDataP wd38_FindUserData( twd38UserDataP	userDataList,
								  sapdbwa_UInt4		id             )
{
	const char FUNCTION_NAME[] = "wd38_FindUserData";

	twd38UserDataP	currentUserData;
    sapdbwa_Bool		found;

	/* Find user data in list */
	found = false;
	currentUserData = userDataList;
	while ( !found && currentUserData ) {
		if ( currentUserData->id != id ) {
			currentUserData = currentUserData->nextUserData;
		} else
			found = true;
	}

	/* Service found? */
	if ( found ) {
		return currentUserData;
	} else {
		return NULL;
	}
}

/****************************************************************************
 ***
 *** End
 ***
 ****************************************************************************/
