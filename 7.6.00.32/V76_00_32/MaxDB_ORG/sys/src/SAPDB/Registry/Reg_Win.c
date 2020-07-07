/*!**********************************************************************

  module:	Reg_Win.c

  -----------------------------------------------------------------------

  responsible:  Markus Özgen

  special area: Registry
  description : Handling of registry entries


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

/*
 * first check if WIN32 platform is defined
 */
#if !defined WIN32
#error WIN32 not defined !
#endif


/*===========================================================================*/
/*===
/*===	Includes
/*===
/*===========================================================================*/
#include <WINDOWS.H>	/* GetPrivateProfileString */
#include <shlwapi.h>	/* Extended registry API */

#include "SAPDB/Registry/Reg_Win.h"

/*===========================================================================*/
/*===
/*===	Defines
/*===
/*===========================================================================*/

#define STR_SAPDBTECH_SECTION       "SOFTWARE\\SAP\\SAP DBTech"
#define STR_SAPDBTECH_SECTION_LEN   23

#define STR_HKEY_LOCAL_MACHINE      "HKEY_LOCAL_MACHINE"
#define STR_HKEY_LOCAL_MACHINE_LEN  18

/*===========================================================================*/
/*===
/*===	Types & Structs
/*===
/*===========================================================================*/

/*===========================================================================*/
/*===
/*===	Prototypes
/*===
/*===========================================================================*/

SAPDB_Bool ReadRegistry ( Reg_RegistryP	 registry,
						  char	        *sectionPath	);

SAPDB_Bool ReadFileAsRegistry( Reg_RegistryP    registry );

SAPDB_Bool WriteSection ( Reg_SectionP	section	);

SAPDB_Bool WriteKey ( HKEY	keyHandle,
					  Reg_KeyP	key			);

SAPDB_Bool DropRegistry( const char *path,
                         SAPDB_Bool  withCheck );

SAPDB_Bool SplitKeyLine ( char    *keyLine,
                          char          *keyName,
                          char          *value );

/*===========================================================================*/
/*===
/*===	Public Methods
/*===
/*===========================================================================*/

/*!***************************************************************************

	Reg_WinOpenRegistry

******************************************************************************/

SAPDB_Bool Reg_WinOpenRegistry ( Reg_RegistryP  *registry,
                                 const char     *path )
{
	SAPDB_Bool		ok = SAPDB_FALSE;
	Reg_RegistryP	newRegistry = NULL;

	if ( !registry )
		return SAPDB_FALSE;

	sqlallocat( sizeof(Reg_Registry), (tsp00_Uint1 **) registry, &ok );

	if ( !ok )
		return SAPDB_FALSE;

	newRegistry = *registry;

	newRegistry->sectionPath[0] = '\0';
    newRegistry->fileDescriptor = 0;
	sp77sprintf(newRegistry->file, REG_MAX_PATH_LEN, "%s", path);
    /*newRegistry->file[0] = '\0';*/
	newRegistry->sections = NULL;
	newRegistry->enumKeys = NULL;
	newRegistry->enumSections = NULL;

	if ( !ReadRegistry( newRegistry, (char*)path )) {
		Reg_WinCloseRegistry( newRegistry );
		*registry = NULL;

		return SAPDB_FALSE;
	}

	return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Reg_WinOpenFileAsRegistry ( Reg_RegistryP            *registry,
                                       const char               *file,
                                       tsp05_RteFileMode_Param   openFlag )
{

	SAPDB_Bool		ok = SAPDB_FALSE;
	Reg_RegistryP	newRegistry = NULL;

	if ( !registry )
		return SAPDB_FALSE;

	sqlallocat( sizeof(Reg_Registry), (tsp00_Uint1 **) registry, &ok );

	if ( !ok )
		return SAPDB_FALSE;

	newRegistry = *registry;

    newRegistry->sectionPath[0] = '\0';
    newRegistry->fileDescriptor = 0;
	sp77sprintf(newRegistry->file, REG_MAX_PATH_LEN, "%s", file);
	newRegistry->sections = NULL;
	newRegistry->enumKeys = NULL;
	newRegistry->enumSections = NULL;
    newRegistry->openFlag = openFlag;

	if ( !ReadFileAsRegistry(newRegistry)) {
		Reg_WinCloseRegistry(newRegistry);
		*registry = NULL;

		return SAPDB_FALSE;
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_WinCloseRegistry

******************************************************************************/

SAPDB_Bool Reg_WinCloseRegistry ( Reg_RegistryP	registry	)
{

	Reg_SectionNodeP	currentSectionNode = NULL;
	Reg_SectionNodeP	nextSectionNode = NULL;

	if ( !registry )
		return SAPDB_TRUE;

	currentSectionNode = registry->sections;
	while ( currentSectionNode ) {
		nextSectionNode = currentSectionNode->nextSectionNode;

		Reg_CommonDropSection( currentSectionNode->section );
		currentSectionNode->section = NULL;
		sqlfree((tsp00_Uint1 *) currentSectionNode );

		currentSectionNode = nextSectionNode;
	}

	sqlfree((tsp00_Uint1 *) registry );

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_WinReadRegistry

******************************************************************************/

SAPDB_Bool Reg_WinReadRegistry ( Reg_RegistryP	registry	)
{

	return ReadRegistry( registry, "" );

}

/*!***************************************************************************

	Reg_WinWrite

******************************************************************************/

SAPDB_Bool Reg_WinWriteRegistry ( Reg_RegistryP registry,
                                  SAPDB_Bool    withCheck )
{

	Reg_SectionNodeP	currentSectionNode = NULL;

	/* Make some checks */
	if ( !registry )
		return SAPDB_FALSE;

	/* Drop registry */
	if (!DropRegistry(registry->file, withCheck))
		return SAPDB_FALSE;

	/* Write all sections */
	currentSectionNode = registry->sections;
	while ( currentSectionNode ) {
		/* Write current section */
		if ( !WriteSection( currentSectionNode->section ))
			return SAPDB_FALSE;

		/* Get next section */
		currentSectionNode = currentSectionNode->nextSectionNode;
	}

	return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Reg_WinExpandSection( char           *section,
                                 char           *sectionToAdd,
                                 char           *expandedSection )
{

    if (!expandedSection) {
        return SAPDB_FALSE;
    }

    if (!section || !sectionToAdd) {
        expandedSection[0] = '\0';
        return SAPDB_TRUE;
    }

    if (sectionToAdd[0] == '\0') {
        strcpy(expandedSection, section);
    } else {
        if (section[0] == '\0') {
            strcpy(expandedSection, sectionToAdd);
        } else {
            sp77sprintf(expandedSection, REG_MAX_SECTION_LEN, "%s\\%s", section, sectionToAdd);
        }
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/
/*===
/*===	Private Methods
/*===
/*===========================================================================*/

/*!***************************************************************************

	ReadRegistry

******************************************************************************/

SAPDB_Bool ReadRegistry ( Reg_RegistryP	 registry,
						  char	        *section	)
{

	HKEY		sectionHandle = 0;
	HKEY		keyHandle = 0;
	DWORD		sectionIndex = 0;
	DWORD		keyIndex = 0;
	LONG		regRC;
	DWORD		sectionLen = 0;
	DWORD		keyLen = 0;
	DWORD		valueLen = 0;
	DWORD		type;
    FILETIME	lastWriteTime;
	SAPDB_Bool	nextSectionOk = SAPDB_FALSE;
	SAPDB_Bool	nextKeyOk = SAPDB_FALSE;
	char		currentSection	[REG_MAX_SECTION_LEN+1];
	char		newSection		[REG_MAX_SECTION_LEN+1];
	char		key				[REG_MAX_KEY_LEN+1];
	char		value			[REG_MAX_VALUE_LEN+1];
	
	if ( !registry )
		return SAPDB_FALSE;

	/* Create internal tree until given section */
	if (strlen(section) > 0) {
	    if (!Reg_CommonInsertSection(registry, (const char*)section, NULL)) {
		    RegCloseKey(sectionHandle);
		    return SAPDB_FALSE;
	    }
    }

	/* Open path */
	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, section, 0,
        KEY_READ, &sectionHandle ) != ERROR_SUCCESS) {
        /* Nothing more to do */
		return SAPDB_TRUE;
    }

	/* Get first section */
	sectionIndex = 0;
	sectionLen = REG_MAX_SECTION_LEN+1;
    currentSection[0] = '\0';
	regRC = RegEnumKeyEx( sectionHandle, sectionIndex++, currentSection,
						  &sectionLen, NULL, NULL, NULL, &lastWriteTime );

	/* Insert all sections */
	while ( regRC == ERROR_SUCCESS ) {
		/* Can we extend the path? */
		if ( strlen(section) + sectionLen + 1 > REG_MAX_SECTION_LEN ) {
		        RegCloseKey( sectionHandle );
				return SAPDB_FALSE;
		}

		/* Extend the path */
        Reg_WinExpandSection(section, currentSection, newSection);

		/* Insert section into internal tree */
		if ( !Reg_CommonInsertSection( registry, (const char*)newSection, NULL )) {
		    RegCloseKey( sectionHandle );
			return SAPDB_FALSE;
		}

		/* Open key */
		if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, newSection, 0,
						   KEY_READ, &keyHandle ) != ERROR_SUCCESS ) {
		    RegCloseKey( sectionHandle );
			return SAPDB_FALSE;
		}

		/* Get first key */
		keyIndex = 0;
		keyLen = REG_MAX_KEY_LEN+1;
		valueLen = REG_MAX_VALUE_LEN+1;
		regRC = RegEnumValue( keyHandle, keyIndex++, key, &keyLen,
							  NULL, &type, value, &valueLen );

		/* Insert all keys */
		while ( regRC == ERROR_SUCCESS ) {
			/* Insert key into internal tree */
			if ( !Reg_CommonInsertKey( registry, (const char*)newSection, key, value )) {
			    RegCloseKey( sectionHandle );
			    RegCloseKey( keyHandle );
				return SAPDB_FALSE;
			}

			/* Get next key */
			keyLen = REG_MAX_KEY_LEN+1;
			valueLen = REG_MAX_VALUE_LEN+1;
			regRC = RegEnumValue( keyHandle, keyIndex++, key, &keyLen,
								  NULL, &type, value, &valueLen );
		}

		/* Close key */
		RegCloseKey( keyHandle );

		/* Read child sections */
		if ( !ReadRegistry( registry, newSection )) {
		    RegCloseKey( sectionHandle );
			return SAPDB_FALSE;
		}

		/* Get next section */
		sectionLen = REG_MAX_SECTION_LEN+1;

		regRC = RegEnumKeyEx( sectionHandle, sectionIndex++, currentSection,
							  &sectionLen, NULL, NULL, NULL, &lastWriteTime );
	};

	/* Close key */
	RegCloseKey( sectionHandle );

	return SAPDB_TRUE;
}

/*===========================================================================*/

SAPDB_Bool ReadFileAsRegistry( Reg_RegistryP    registry )
{

    SAPDB_Int4			file = -1;
	tsp05_RteFileError	err;
	char				buffer			[MAX_BUFFER_LEN+1];
	char				currentSection	[REG_MAX_SECTION_LEN+1];
	SAPDB_UInt4			sectionLen = 0;
	SAPDB_Bool			eof = SAPDB_FALSE;

	char	key		[REG_MAX_KEY_LEN+1];
	char	value	[REG_MAX_VALUE_LEN+1];

	sapdbwa_TraceDefFunction("ReadFileAsRegistry")

	buffer[0] = '\0';
	currentSection[0] = '\0';

	/* Checks */
	if ( !registry ) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	/* Open registry */
	if ( registry->openFlag == sp5vf_readwrite )
		sqlfopenc(registry->file, sp5vf_binary, registry->openFlag, sp5bk_unbuffered, &file, &err);
	else
		sqlfopenc(registry->file, sp5vf_binary, registry->openFlag, sp5bk_buffered, &file, &err);
	if ( err.sp5fe_result != vf_ok ) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	/* Is file opened to write? */
	if ( registry->openFlag == sp5vf_readwrite ) {
		/* Store file descriptor */
		registry->fileDescriptor = file;
	} else {
		registry->fileDescriptor = -1;
	}

	do {
		if ( !Reg_CommonGetNextLine ( file, buffer, MAX_BUFFER_LEN, &eof, SAPDB_TRUE )) {
			sqlfclosec( file, sp5vf_close_normal, &err );

			sapdbwa_TraceAdd( "Exiting unexpected!" )
			return SAPDB_FALSE;
		}

		if ( eof ) break;

		if ( buffer[0] == '\0' )
			continue;

		if ( buffer[0] == '[' && (sectionLen = strlen(buffer)) > 2 ) {
			/* Get section name */
            /*
            if (strncmp(buffer+1, STR_HKEY_LOCAL_MACHINE, STR_HKEY_LOCAL_MACHINE_LEN) == 0) {
			    strcpy( currentSection, buffer + 1 + STR_HKEY_LOCAL_MACHINE_LEN );
			    currentSection[sectionLen - 2 - STR_HKEY_LOCAL_MACHINE_LEN] = '\0';
            } else */if (strncmp(buffer+1, STR_HKEY_LOCAL_MACHINE"\\\\", STR_HKEY_LOCAL_MACHINE_LEN + 2) == 0) {
			    strcpy( currentSection, buffer + 1 + STR_HKEY_LOCAL_MACHINE_LEN + 2);
			    currentSection[sectionLen - 2 - STR_HKEY_LOCAL_MACHINE_LEN - 2] = '\0';
            } else if (strncmp(buffer+1, STR_HKEY_LOCAL_MACHINE"\\", STR_HKEY_LOCAL_MACHINE_LEN + 1) == 0) {
			    strcpy( currentSection, buffer + 1 + STR_HKEY_LOCAL_MACHINE_LEN + 1);
			    currentSection[sectionLen - 2 - STR_HKEY_LOCAL_MACHINE_LEN - 1] = '\0';
            } else {
			    strcpy( currentSection, buffer + 1);
			    currentSection[sectionLen - 2] = '\0';
            }

			if ( !Reg_CommonInsertSection( registry, (const char*)currentSection, NULL )) {
				sapdbwa_TraceAdd( "Exiting unexpected!" )
				return SAPDB_FALSE;
			}
		} else {
			if ( currentSection[0] != '\0' ) {

				/* Split key line into key and value */
				if ( !SplitKeyLine( buffer, key, value )) {
					sapdbwa_TraceAdd( "Exiting unexpected!" )
					return SAPDB_FALSE;
				}

				if ( !Reg_CommonInsertKey( registry, (const char*)currentSection, (const char*)key, value )) {
					sapdbwa_TraceAdd( "Exiting unexpected!" )
					return SAPDB_FALSE;
				}
			}
		}
	} while ( 1 );

	if ( registry->openFlag == sp5vf_read ) {
		sqlfclosec( file, sp5vf_close_normal, &err );
	}

	return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SplitKeyLine ( char	*keyLine,
						  char			*keyName,
						  char			*value		)
{

	SAPDB_Int4	 posSplit = 0;
	SAPDB_Int4	 keyLineLen = 0;
    char        *copyBegin = NULL;
    char        *copyEnd = NULL;
	SAPDB_Int4	 copyLen = 0;

	sapdbwa_TraceDefFunction( "SplitKeyLine" )

	/* Checks */
	if ( !keyLine || !keyName || !value ) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	/* Inits */
	keyName[0] = '\0';
	value[0] = '\0';

	/* If key line is empty, return with empty strings */
	if (( keyLineLen = strlen(keyLine)) == 0 ) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	/* Get seperator */
	posSplit = keyLineLen;
	while ( keyLine[--posSplit] != '=' && posSplit != 0 );

	/* No key name */
	if ( posSplit == 0  ) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	/* Set key name */
    if (keyLine[0] == '"') {
        copyBegin = keyLine + 1;
    } else {
        copyBegin = keyLine;
    }

    if (keyLine[posSplit-1] == '"') {
        copyEnd = keyLine + posSplit - 1;
    } else {
        copyEnd = keyLine + posSplit;
    }

    copyLen = copyEnd - copyBegin;

    strncpy( keyName, copyBegin, copyLen);
    keyName[copyLen] = '\0';

	/* No value */
	if ( posSplit + 2 > keyLineLen ) {
		return SAPDB_TRUE;
	}

	/* Set value */
    if (keyLine[posSplit + 1] == '"') {
        copyBegin = keyLine + posSplit + 2;
    } else {
        copyBegin = keyLine + posSplit + 1;
    }

    if (keyLine[keyLineLen - 1] == '"') {
        copyEnd = keyLine + keyLineLen - 1;
    } else {
        copyEnd = keyLine + keyLineLen;
    }

    copyLen = copyEnd - copyBegin;

	strncpy( value, copyBegin, copyLen );
	value[copyLen] = '\0';

	return SAPDB_TRUE;

}

/*!***************************************************************************

	WriteSection

******************************************************************************/

SAPDB_Bool WriteSection ( Reg_SectionP	section	)
{

	HKEY			keyHandle = NULL;
	Reg_KeyNodeP		currentKeyNode = NULL;
	Reg_SectionNodeP	currentChildSectionNode = NULL;

	/* Check */
	if ( !section )
		return SAPDB_FALSE;

	/* Open and write section */
	if ( RegCreateKeyEx( HKEY_LOCAL_MACHINE, section->sectionPath, 0, "",
						 REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
						 &keyHandle, NULL) != ERROR_SUCCESS)
		return SAPDB_FALSE;

	/* Get current key node */
	currentKeyNode = section->keys;
	while( currentKeyNode ) {
		/* Write current key */
		if ( !WriteKey( keyHandle, currentKeyNode->key ))
			return SAPDB_FALSE;

		/* Get next key node */
		currentKeyNode = currentKeyNode->nextKeyNode;
	}

	/* Close section */
    RegCloseKey( keyHandle );
    keyHandle = NULL;

	/* Write all child sections */
	currentChildSectionNode = section->childSections;
	while( currentChildSectionNode ) {
		/* Write current child section */
		if ( !WriteSection ( currentChildSectionNode->section ))
			return SAPDB_FALSE;

		/* Get next child section node */
		currentChildSectionNode = currentChildSectionNode->nextSectionNode;
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	WriteKey

******************************************************************************/

SAPDB_Bool WriteKey ( HKEY	keyHandle,
					  Reg_KeyP	key			)
{

	/* Check */
	if ( !keyHandle || !key )
		return SAPDB_FALSE;

	/* Write key */
    if ( RegSetValueEx( keyHandle, key->key, 0,	REG_SZ,
						key->value,	strlen(key->value) + 1 ) != ERROR_SUCCESS )
		return SAPDB_FALSE;

	return SAPDB_TRUE;

}

/****************************************************************************
/***
/***	DropRegistry
/***
/****************************************************************************/

SAPDB_Bool DropRegistry( const char *path,
                         SAPDB_Bool  withCheck )
{

	HKEY	regHandle;
	LONG	regRC;
	char	sectionName[REG_MAX_SECTION_LEN+1];
	char	parentSectionPath[REG_MAX_SECTION_LEN+1];

	/* Checks */
	if (!path)
		return SAPDB_FALSE;

    if (withCheck) {
        if (path[0] == '\\') {
            if (strncasecmp(path+1, STR_SAPDBTECH_SECTION, STR_SAPDBTECH_SECTION_LEN) != 0) {
		        return SAPDB_FALSE;
            }
        } else {
            if (strncasecmp(path, STR_SAPDBTECH_SECTION, STR_SAPDBTECH_SECTION_LEN) != 0) {
		        return SAPDB_FALSE;
            }
        }
    }

	/* Get parent section */
	if ( !SplitSectionPath ( path, sectionName, parentSectionPath ))
		return SAPDB_FALSE;

	/* Does section to delete exist? */
	regRC = RegOpenKeyEx( HKEY_LOCAL_MACHINE, path, 0, KEY_ALL_ACCESS, &regHandle );
    if ( regRC != ERROR_SUCCESS) {
        /* Nothing to do */
		return SAPDB_TRUE;
    }
    RegCloseKey( regHandle );

	/* Open registry */
	regRC = RegOpenKeyEx( HKEY_LOCAL_MACHINE, parentSectionPath, 0, KEY_ALL_ACCESS, &regHandle );
	if ( regRC != ERROR_SUCCESS)
		return SAPDB_FALSE;

	/* Drop registry */
    regRC = SHDeleteKey( regHandle, sectionName );
    if ( regRC != ERROR_SUCCESS ) {
		RegCloseKey( regHandle );
		return SAPDB_FALSE;
    };

	/* Close registry */
    RegCloseKey( regHandle );

    return SAPDB_TRUE;

}

/*===========================================================================*/
/*===
/*===	End
/*===
/*===========================================================================*/
