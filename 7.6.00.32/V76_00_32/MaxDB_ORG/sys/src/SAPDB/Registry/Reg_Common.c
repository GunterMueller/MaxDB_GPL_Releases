/*!**********************************************************************

  module:	Reg_Common.c

  -----------------------------------------------------------------------

  responsible:  Markus Özgen

  special area: Registry
  description : Handling registry entries - Common functions

    Wraps an interface for registry entries. On Windows plattforms
	the registry will be used. On Unix plattforms an INI file will
	be used.

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

/*===========================================================================*
 *===
 *===	Includes
 *===
 *===========================================================================*/

#include "SAPDB/Registry/Reg_Common.h"

/*===========================================================================*
 *===
 *===	Defines
 *===
 *===========================================================================*/

sapdbwa_TraceDefModule( "Reg_Common.c" )

#define STR_NEWLINE	"\n"

/*===========================================================================*
 *===
 *===	Types & Structs
 *===
 *===========================================================================*/

/*===========================================================================*
 *===
 *===	Prototypes
 *===
 *===========================================================================*/

SAPDB_Bool DeleteKey ( Reg_SectionP	 section,
					   const char	*keyName	);

SAPDB_Bool AddSection ( Reg_SectionNodeP	 previousSectionNode,
						Reg_SectionNodeP	*sectionNode,
						Reg_SectionP		 newSection,
						Reg_SectionNodeP	*newSectionNode	);


SAPDB_Bool CreateNewSection ( Reg_RegistryP	 registry,
                              const char	*sectionPath,
							  const char	*sectionName,
							  Reg_SectionP	*section );

SAPDB_Bool SplitSectionPath ( const char	*sectionPath,
							  char			*sectionName,
							  char			*parentSectionPath	);

SAPDB_Bool DropKeys ( Reg_KeyNodeP	keys	);

void UpperCase( char *str );

/*===========================================================================*
 *===
 *===	Public Methods
 *===
 *===========================================================================*/

/*!***************************************************************************

	Reg_CommonAddKey

******************************************************************************/

SAPDB_Bool Reg_CommonAddKey ( Reg_SectionP	section,
							  Reg_KeyP		newKey		)
{

	SAPDB_Bool	ok = SAPDB_FALSE;

	Reg_KeyNodeP	 currentKeyNode;
	Reg_KeyNodeP	*newKeyNode;

	if ( !newKey || !section )
		return SAPDB_FALSE;

	newKeyNode = &section->keys;
	currentKeyNode = section->keys;
	while ( currentKeyNode ) {
		newKeyNode = &currentKeyNode->nextKeyNode;
		currentKeyNode = currentKeyNode->nextKeyNode;
	}

	sqlallocat( sizeof(Reg_SectionNode), (tsp00_Uint1 **) newKeyNode, &ok );

	if ( ok ) {
		currentKeyNode = *newKeyNode;
		currentKeyNode->key = newKey;
		currentKeyNode->nextKeyNode = NULL;
	} else {
		return SAPDB_FALSE;
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	GetNextLine

******************************************************************************/

SAPDB_Bool Reg_CommonGetNextLine ( SAPDB_Int4    file,
                                   char         *line,
                                   SAPDB_UInt4   maxLineSize,
                                   SAPDB_Bool   *eof,
                                   SAPDB_Bool    replaceDoubleBackSlash )
{

	char				ch;
	SAPDB_UInt4			pos = 0;
	tsp00_Longint			readLen;
	tsp05_RteFileError	err;

	sapdbwa_TraceDefFunction( "GetNextLine" )

	*eof = SAPDB_FALSE;
	ch = '\0';
	line[0] = '\0';

	do {
		sqlfreadc ( file, &ch, 1, &readLen, &err );

		/* EOF */
		if ( err.sp5fe_result == vf_eof ) {
			*eof = SAPDB_TRUE;
			line[pos] = '\0';
			return SAPDB_TRUE;
		}

		/* Error? */
		if ( err.sp5fe_result != vf_ok ) {
			sapdbwa_TraceAdd( "Exiting unexpected!" )
			return SAPDB_FALSE;
		}

		/* Skip carriage return CR */
		if ( ch == '\r' ) {
			sqlfreadc ( file, &ch, 1, &readLen, &err );
			if ( err.sp5fe_result != vf_ok ) {
				sapdbwa_TraceAdd( "Exiting unexpected!" )
				return SAPDB_FALSE;
			}
		}

		/* Read error */
		if ( readLen != 1 )	{
			line[pos] = '\0';
			sapdbwa_TraceAdd( "Exiting unexpected!" )
			return SAPDB_FALSE;
		}

		/* Get all characters except new line and non printables
		   and only if max line size is not reached				*/
        if ( ch != '\n' && !( ch < ' ' || pos > maxLineSize )) {
            /* Do we have to replace double back slashes? */
            if (!(replaceDoubleBackSlash == SAPDB_TRUE && ch == '\\' && (pos > 0 && line[pos-1] == '\\'))) {
			    line[pos++] = ch;
            }
        }
	} while ( ch != '\n' );

	line[pos] = '\0';

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_CommonCreateNewKey

******************************************************************************/

SAPDB_Bool Reg_CommonCreateNewKey ( const char	*keyName,
									const char	*value,
									Reg_KeyP	*key		)
{

	SAPDB_Bool	 ok = SAPDB_FALSE;

	Reg_KeyP	newKey;

	if ( !key || !keyName )
		return SAPDB_FALSE;

	sqlallocat( sizeof(Reg_Key), (tsp00_Uint1 **) key, &ok );

	if ( ok ) {
		newKey = *key;

		strncpy( newKey->key, keyName, REG_MAX_KEY_LEN );
		newKey->key[REG_MAX_KEY_LEN] = '\0';

		if ( value ) {
			strncpy( newKey->value, value, REG_MAX_VALUE_LEN );
			newKey->value[REG_MAX_VALUE_LEN] = '\0';
		} else {
			newKey->value[0] = '\0';
		}
	} else {
		*key = NULL;
		return SAPDB_FALSE;
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_CommonFindKey

******************************************************************************/

SAPDB_Bool Reg_CommonFindKey ( Reg_KeyNodeP	 searchNode,
							   const char	*keyName,
							   Reg_KeyP		*key		)
{
	char		srcName	[REG_MAX_KEY_LEN+1];
	char		destName[REG_MAX_KEY_LEN+1];

	Reg_KeyNode	*currentKeyNode = NULL;
	Reg_Key		*currentKey = NULL;

	/* Check input parameters */
	if ( !key )
		return SAPDB_FALSE;

	/* Init output parameter */
	*key = NULL;

	/* Check input parameters */
	if ( !searchNode || !keyName )
		return SAPDB_TRUE;

	/* Nothing to search? */
	if ( keyName[0] == '\0' )
		return SAPDB_TRUE;

	/* Init loop */
	currentKeyNode = searchNode;
	while ( currentKeyNode ) {
		if ( currentKeyNode->key ) {
			currentKey = currentKeyNode->key;

			strcpy( srcName, currentKey->key );
			UpperCase( srcName );
			strcpy( destName, keyName );
			UpperCase( destName );

			if ( !strcmp( srcName, destName )) {
				*key = currentKey;
				return SAPDB_TRUE;
			}
		}

		currentKeyNode = currentKeyNode->nextKeyNode;
	}

	return SAPDB_TRUE;
}

/*!***************************************************************************

	Reg_CommonFindSection

******************************************************************************/

SAPDB_Bool Reg_CommonFindSection ( Reg_RegistryP	 registry,
                                   Reg_SectionNodeP	 searchNode,
								   const char		*sectionPath,
								   Reg_SectionP		*section,
								   Reg_SectionNodeP	*sectionNode	)
{
	char		srcPath	[REG_MAX_SECTION_LEN+1];
	char		destPath[REG_MAX_SECTION_LEN+1];

	Reg_SectionNodeP	currentSectionNode = NULL;
	Reg_SectionP		currentSection = NULL;

	/* Check input parameters */
	if ( !section )
		return SAPDB_FALSE;

	/* Init output parameter */
	*section = NULL;

    /* Set current section path */
    strcpy(destPath, sectionPath);

	/* Nothing to search? */
	if (!searchNode || !destPath)
		return SAPDB_TRUE;

	/* Init loop */
	UpperCase( destPath );
	currentSectionNode = searchNode;
	while ( currentSectionNode ) {
		if ( currentSectionNode->section ) {
			currentSection = currentSectionNode->section;

			strcpy( srcPath, currentSection->sectionPath );
			UpperCase( srcPath );

			/* Compare pathes */
			if ( !strcmp( srcPath, destPath )) {
				/* Set return values */
				*section = currentSection;

				if ( sectionNode )
					*sectionNode = currentSectionNode;

				return SAPDB_TRUE;
			}

			/* Are there child sections */
			if ( currentSection->childSections ) {
				/* Search in child sections */
				if ( !Reg_CommonFindSection(registry, currentSection->childSections,
                                            sectionPath, section, sectionNode))
					return SAPDB_FALSE;

				/* If found return */
				if ( *section )
					return SAPDB_TRUE;
			}
		}

		/* Get next section node */
		currentSectionNode = currentSectionNode->nextSectionNode;
	}

	return SAPDB_TRUE;
}

/*!***************************************************************************

	Reg_CommonInsertSection

******************************************************************************/

SAPDB_Bool Reg_CommonInsertSection ( Reg_RegistryP		 registry,
									 const char			*section,
									 Reg_SectionNodeP	*sectionNode	)
{

	char				sectionName [REG_MAX_SECTION_LEN+1];
	char				parentSectionPath [REG_MAX_SECTION_LEN+1];
	Reg_SectionP		newSection = NULL;
	Reg_SectionP		parentSection = NULL;
	Reg_SectionNodeP	parentSectionNode = NULL;
	Reg_SectionNodeP	newSectionNode = NULL;

	/* Check the length of section */
	if ( !section )
		return SAPDB_FALSE;

	/* If input parameter isn't set, use local variable */
	if ( !sectionNode )
		sectionNode = &newSectionNode;

	/* Split section path */
	if ( !SplitSectionPath( section, sectionName, parentSectionPath ))
		return SAPDB_FALSE;

	/* Reg_Section already exists? */
	if ( registry->sections ) {
		if ( !Reg_CommonFindSection(registry, registry->sections, (const char*)section, &newSection, sectionNode ))
			return SAPDB_FALSE;
	}

	if ( !newSection ) {
		/* Create new section */
		if ( !CreateNewSection( registry, section, sectionName, &newSection ))
			return SAPDB_FALSE;
	} else {
		/* Section already exists */
		return SAPDB_TRUE;
	}

	/* Is there a parent? */
	if ( strlen(parentSectionPath) > 0 ) {
		/* Find parent section node */
		if ( !Reg_CommonFindSection(registry, registry->sections, (const char*)parentSectionPath, &parentSection, &parentSectionNode ))
			return SAPDB_FALSE;

		/* If a parent can't found */
		if ( !parentSection ) {
			/* Insert parent's parent */
			if ( !Reg_CommonInsertSection( registry, parentSectionPath, &parentSectionNode ))
				return SAPDB_FALSE;

			/* After parent is inserted, get the pointer to this section */
			if ( !Reg_CommonFindSection(registry, registry->sections, (const char*)parentSectionPath, &parentSection, &parentSectionNode ))
				return SAPDB_FALSE;

			/* Is parent now there? */
			if ( !parentSection )
				return SAPDB_FALSE;
		}

		/* Add new section to parent's child section list */
		if ( !AddSection( parentSectionNode, &parentSection->childSections, newSection, sectionNode ))
			return SAPDB_FALSE;
	} else {
		/* If section list is empty or there's no parent, add new section at the end of the section list */
		if ( !AddSection( NULL, &registry->sections, newSection, sectionNode ))
			return SAPDB_FALSE;
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_CommonInsertKey

******************************************************************************/

SAPDB_Bool Reg_CommonInsertKey ( Reg_RegistryP	 registry,
								 const char		*sectionName,
								 const char		*key,
								 const char		*value		)
{

	Reg_SectionP	section = NULL;
	Reg_KeyP		newKey = NULL;

	/* Check input parameters */
	if ( !registry || !sectionName || !key )
		return SAPDB_FALSE;

	/* Find section for new key entry */
	if ( !Reg_CommonFindSection(registry, registry->sections, sectionName, &section, NULL ))
		return SAPDB_FALSE;

	/* Reg_Section found? */
	if ( !section )
		return SAPDB_FALSE;

	/* Reg_Key already exists? */
	if ( !Reg_CommonFindKey( section->keys, key, &newKey ))
		return SAPDB_FALSE;

	/* If key doesn't exists, create new key */
	if ( !newKey ) {
		if ( !Reg_CommonCreateNewKey( key, value, &newKey ))
			return SAPDB_FALSE;

		/* Add key to section */
		if ( !Reg_CommonAddKey ( section, newKey ))
			return SAPDB_FALSE;
	} else {
		/* Set new key value */
		strcpy( newKey->value, value );
	}

	return SAPDB_TRUE;
}

/*!***************************************************************************

	Reg_CommonDropSection

******************************************************************************/

SAPDB_Bool Reg_CommonDropSection ( Reg_SectionP	section	)
{

	Reg_SectionNodeP	currentSectionNode = NULL;
	Reg_SectionNodeP	nextSectionNode = NULL;

	sapdbwa_TraceDefFunction( "Reg_CommonDropSection" );

	if ( !section )
		return SAPDB_TRUE;

	/* Drop all child sections */
	currentSectionNode = section->childSections;
	while ( currentSectionNode ) {
		sapdbwa_TraceAdd( currentSectionNode->section->sectionPath );
		nextSectionNode = currentSectionNode->nextSectionNode;

		if ( currentSectionNode->section ) {
			Reg_CommonDropSection( currentSectionNode->section );
			currentSectionNode->section = NULL;
		}

		sqlfree((tsp00_Uint1 *) currentSectionNode );
		sapdbwa_TraceAdd( "Section droped." );

		currentSectionNode = nextSectionNode;
	}
	sapdbwa_TraceAdd( "All child sections droped." );

	/* Drop all keys */
	if ( section->keys )
		if ( DropKeys( section->keys ))
			section->keys = NULL;
	sapdbwa_TraceAdd( "All keys droped." );

	/* Drop section itself */
	sqlfree((tsp00_Uint1 *) section );
	sapdbwa_TraceAdd( "Section droped." );

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_CommonWrite

******************************************************************************/

SAPDB_Bool Reg_CommonWrite ( SAPDB_Int4	 fileDescriptor,
							 char		*line			)
{

	tsp05_RteFileError	err;

	/* Write line */
	sqlfwritec( fileDescriptor, line, strlen( line ), &err );
	if ( err.sp5fe_result != vf_ok )
		return SAPDB_FALSE;

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_CommonWriteLine

******************************************************************************/

SAPDB_Bool Reg_CommonWriteLine ( SAPDB_Int4	 fileDescriptor,
								 char		*line			)
{

	/* Write line */
	if ( !Reg_CommonWrite( fileDescriptor, line ))
		return SAPDB_FALSE;

	/* Write new line */
	if ( !Reg_CommonWrite( fileDescriptor, STR_NEWLINE ))
		return SAPDB_FALSE;

	return SAPDB_TRUE;

}

/*===========================================================================*
 *===
 *===	Private Methods
 *===
 *===========================================================================*/

/*!***************************************************************************

	DeleteKey

******************************************************************************/

SAPDB_Bool DeleteKey ( Reg_SectionP	 section,
					   const char	*keyName	)
{

	Reg_KeyNodeP	*lastKeyNode = NULL;
	Reg_KeyNodeP	 currentKeyNode = NULL;
	Reg_KeyP		 currentKey = NULL;
	char			 upperKeyName [REG_MAX_KEY_LEN+1];
	char			 upperRegistryKeyName [REG_MAX_KEY_LEN+1];

	if ( !section->keys )
		return SAPDB_FALSE;

	lastKeyNode = &section->keys;
	currentKeyNode = section->keys;
	while ( currentKeyNode ) {
		if ( currentKeyNode->key ) {
			currentKey = currentKeyNode->key;

			/* Convert key name upper case */
			strncpy( upperKeyName, keyName, REG_MAX_KEY_LEN );
			upperKeyName[REG_MAX_KEY_LEN] = '\0';
			UpperCase( upperKeyName );

			strncpy( upperRegistryKeyName, currentKey->key, REG_MAX_KEY_LEN );
			upperRegistryKeyName[REG_MAX_KEY_LEN] = '\0';
			UpperCase( upperRegistryKeyName );
	
			/* Compare key name with registry entry */
			if ( !strcmp( upperRegistryKeyName, upperKeyName )) {
				*lastKeyNode = currentKeyNode->nextKeyNode;

				/* Drop key node and key */
				sqlfree((tsp00_Uint1 *) currentKey );
				sqlfree((tsp00_Uint1 *) currentKeyNode );

				return SAPDB_TRUE;
			}
		}

		lastKeyNode = &currentKeyNode->nextKeyNode;
		currentKeyNode = currentKeyNode->nextKeyNode;
	}

	return SAPDB_FALSE;

}

/*!***************************************************************************

	DropKeys

******************************************************************************/

SAPDB_Bool DropKeys ( Reg_KeyNodeP	keys	)
{

	Reg_KeyNodeP	currentKeyNode = NULL;
	Reg_KeyNodeP	nexKeyNode = NULL;

	sapdbwa_TraceDefFunction( "DropKeys" );

	if ( !keys )
		return SAPDB_TRUE;

	/* Drop all keys */
	currentKeyNode = keys;
	while ( currentKeyNode ) {
		sapdbwa_TraceAdd( currentKeyNode->key->key );
		sapdbwa_TraceAdd( currentKeyNode->key->value );
		nexKeyNode = currentKeyNode->nextKeyNode;

		if ( currentKeyNode->key ) {
			sqlfree((tsp00_Uint1 *) currentKeyNode->key );
			currentKeyNode->key = NULL;
		}

		sqlfree((tsp00_Uint1 *) currentKeyNode );

		currentKeyNode = nexKeyNode;
	}
	sapdbwa_TraceAdd( "All keys droped." );

	return SAPDB_TRUE;

}

/*!***************************************************************************

	CreateNewSection

******************************************************************************/

SAPDB_Bool CreateNewSection ( Reg_RegistryP	 registry,
                              const char	*sectionPath,
							  const char	*sectionName,
							  Reg_SectionP	*section )
{

	SAPDB_Bool	ok = SAPDB_FALSE;
	Reg_SectionP	newSection;

	if ( !section )
		return SAPDB_FALSE;

	sqlallocat( sizeof(Reg_Section), (tsp00_Uint1 **) section, &ok );

	if ( ok ) {
		newSection = *section;

#ifdef REG_UNIX
		if ( sectionPath ) {
            strncpy(newSection->sectionPath, sectionPath, REG_MAX_SECTION_LEN);
            newSection->sectionPath[REG_MAX_SECTION_LEN] = '\0';
		} else {
            newSection->sectionPath[0] = '\0';
		}
#else
		if ( sectionPath ) {
            sp77sprintf(newSection->sectionPath, REG_MAX_SECTION_LEN, "%s%s%s", registry->sectionPath, (strlen(registry->sectionPath) > 0 ? "\\" : ""), sectionPath);
		} else {
			strcpy(newSection->sectionPath, registry->sectionPath);
		}
#endif

		if ( sectionName ) {
			strncpy( newSection->sectionName, sectionName, REG_MAX_SECTION_LEN );
			newSection->sectionName[REG_MAX_SECTION_LEN] = '\0';
		} else {
			newSection->sectionName[0] = '\0';
		}

		newSection->childSections = NULL;
		newSection->keys = NULL;
	} else {
		*section = NULL;
		return SAPDB_FALSE;
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	AddSection

******************************************************************************/

SAPDB_Bool AddSection ( Reg_SectionNodeP	 previousSectionNode,
						Reg_SectionNodeP	*sectionNode,
						Reg_SectionP		 newSection,
						Reg_SectionNodeP	*newSectionNode	)
{

	SAPDB_Bool	ok = SAPDB_FALSE;

	Reg_SectionNodeP	 lastSectionNode;
	Reg_SectionNodeP	 currentSectionNode;
	Reg_SectionNodeP	*mySectionNode;

	if ( !newSection || !sectionNode )
		return SAPDB_FALSE;

	mySectionNode = sectionNode;
	currentSectionNode = *sectionNode;
	lastSectionNode = *sectionNode;
	while ( currentSectionNode ) {
		mySectionNode = &currentSectionNode->nextSectionNode;
		lastSectionNode = currentSectionNode;
		currentSectionNode = currentSectionNode->nextSectionNode;
	}

	sqlallocat( sizeof(Reg_SectionNode), (tsp00_Uint1 **) mySectionNode, &ok );

	if ( ok ) {
		/* Was there a section list directly in front of the new node? */
		if ( lastSectionNode ) {
			/* Set the node in front as the previous node for the new node */
			(*mySectionNode)->previousSectionNode = lastSectionNode;
		}
		else {
			/* Set the node in front as the previous node for the new node */
			(*mySectionNode)->previousSectionNode = previousSectionNode;
		}

		currentSectionNode = *mySectionNode;
		currentSectionNode->section = newSection;
		currentSectionNode->nextSectionNode = NULL;
	} else {
		return SAPDB_FALSE;
	}

	/* Set pointer to new section node */
	*newSectionNode = *mySectionNode;

	return SAPDB_TRUE;

}

/*!***************************************************************************

	GetParentSectionNode

******************************************************************************/

SAPDB_Bool GetParentSectionNode ( Reg_SectionNodeP	 startSectionNode,
								  Reg_SectionNodeP	 sectionNode,
								  Reg_SectionNodeP	*parentSectionNode	)
{

	Reg_SectionNodeP	currentSectionNode = NULL;

	/* Check input parameters */
	if ( !startSectionNode || !sectionNode || !parentSectionNode )
		return SAPDB_FALSE;

	/* Init output parameter */
	*parentSectionNode = NULL;

	/* If section node is root section node */
	if ( startSectionNode == sectionNode )
		return SAPDB_TRUE;

	/* Init loop */
	currentSectionNode = startSectionNode;
	while ( currentSectionNode ) {
		if ( currentSectionNode->section && currentSectionNode->section->childSections ) {
			if ( !GetParentSectionNode( currentSectionNode->section->childSections, sectionNode, parentSectionNode ))
				return SAPDB_FALSE;

			if ( parentSectionNode )
				return SAPDB_TRUE;
		}

		if ( currentSectionNode->nextSectionNode == sectionNode ) {
			*parentSectionNode = currentSectionNode;
			return SAPDB_TRUE;
		}

		/* Get next section node */
		currentSectionNode = currentSectionNode->nextSectionNode;
	}

	return SAPDB_TRUE;
}

/*!***************************************************************************

	SplitSectionPath

******************************************************************************/

SAPDB_Bool SplitSectionPath ( const char	*sectionPath,
							  char			*sectionName,
							  char			*parentSectionPath	)
{

	SAPDB_UInt4	sectionPathLen = 0;
	SAPDB_Int4	posEnd = 0;
	SAPDB_Int4	posStart = 0;
	SAPDB_Int4	posPathEnd = 0;

	/* Checks */
	if ( !sectionPath )
		return SAPDB_FALSE;

	/* Inits */
	if ( sectionName )
		sectionName[0] = '\0';
	if ( parentSectionPath )
		parentSectionPath[0] = '\0';

	/* If sectionPath is empty, return with empty strings */
	if (( sectionPathLen = strlen(sectionPath)) < 1 )
		return SAPDB_TRUE;

	/* Skip trailing '\' */
	posEnd = sectionPathLen;
	while ( sectionPath[--posEnd] == '\\' && posEnd != 0 );
	if ( posEnd == 0 ) {
		return SAPDB_TRUE;
	}

	/* Get sectionName */
	posStart = posEnd;
	while ( sectionPath[--posStart] != '\\' && posStart != 0 );
	if ( sectionName ) {
		if ( posStart == 0 ) {
			if ( sectionPath[0] != '\\' ) {
				strncpy( sectionName, sectionPath, posEnd + 1 );
				sectionName[posEnd+1] = '\0';
			} else {
				strncpy( sectionName, sectionPath + 1, posEnd + 1 );
				sectionName[posEnd+1] = '\0';
			}
			return SAPDB_TRUE;
		}

		strncpy( sectionName, sectionPath + posStart + 1, posEnd - posStart );
		sectionName[sectionPath + posStart + 1, posEnd - posStart] = '\0';
	}

	/* Get parentSectionPath */
	posPathEnd = posStart;
	while ( sectionPath[--posPathEnd] == '\\' && posPathEnd != 0 );
	if ( parentSectionPath ) {
		if ( posPathEnd == 0 ) {
			return SAPDB_TRUE;
		}

		strncpy( parentSectionPath, sectionPath, posPathEnd + 1 );
		parentSectionPath[posPathEnd+1] = '\0';
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	UpperCase

******************************************************************************/

void UpperCase( char *str )
{
    char *pos = str;
    
    while ( *pos != '\0' ) {
        *pos = toupper( *pos );
		pos++;
    };
}

/*===========================================================================*
 *===
 *===	End
 *===
 *===========================================================================*/
