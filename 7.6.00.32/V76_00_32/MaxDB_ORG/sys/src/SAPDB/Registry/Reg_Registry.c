/*!**********************************************************************

  module:	Reg_Registry.c

  -----------------------------------------------------------------------

  responsible:	Markus Özgen

  special area: Registry
  description : Handling registry entries

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

/*===========================================================================
 *===
 *===	Includes
 *===
 *===========================================================================*/

#include <fcntl.h>

#include "SAPDB/Registry/Reg_Registry.h"

#ifdef REG_WIN32
#include "SAPDB/Registry/Reg_Win.h"
#else
#include "SAPDB/Registry/Reg_Unix.h"
#endif

/*===========================================================================
 *===
 *===	Defines
 *===
 *===========================================================================*/

#define STR_XML_HEADER						"<?xml version=\"1.0\"?>"
#define STR_XML_REGISTRY_START				"<Registry>"
#define STR_XML_REGISTRY_END				"</Registry>"
#define STR_XML_REGISTRY_PATH_START			"<Path>"
#define STR_XML_REGISTRY_PATH_END			"</Path>"
#define STR_XML_SECTION_LIST_START			"<Sections>"
#define STR_XML_SECTION_LIST_END			"</Sections>"
#define STR_XML_SECTION_START				"<Section>"
#define STR_XML_SECTION_END					"</Section>"
#define STR_XML_SECTION_NAME_START			"<Name>"
#define STR_XML_SECTION_NAME_END			"</Name>"
#define STR_XML_SECTION_PATH_START			"<Path>"
#define STR_XML_SECTION_PATH_END			"</Path>"
#define STR_XML_CHILD_SECTION_LIST_START	"<ChildSections>"
#define STR_XML_CHILD_SECTION_LIST_END		"</ChildSections>"
#define STR_XML_KEY_LIST_START				"<Keys>"
#define STR_XML_KEY_LIST_END				"</Keys>"
#define STR_XML_KEY_START					"<Key>"
#define STR_XML_KEY_END						"</Key>"
#define STR_XML_KEY_NAME_START				"<Name>"
#define STR_XML_KEY_NAME_END				"</Name>"
#define STR_XML_KEY_VALUE_START				"<Value>"
#define STR_XML_KEY_VALUE_END				"</Value>"

/*===========================================================================
 *===
 *===	Types & Structs
 *===
 *===========================================================================*/

/*===========================================================================
 *===
 *===	Prototypes
 *===
 *===========================================================================*/

SAPDB_Bool OpenRegistryWithOpenFlag ( Reg_RegistryP			*registry,
									  const char		*path,
									  tsp05_RteFileMode	 openFlag	);

SAPDB_Bool OpenFileAsRegistry ( Reg_RegistryP   *registry,
                                const char      *file );

SAPDB_Bool WriteRegistryXML ( Reg_RegistryP		 registry,
							  const char	*exportPath	);

SAPDB_Bool WriteExportHeaderXML ( SAPDB_Int4	fileDescriptor,
								  Reg_RegistryP		registry		);

SAPDB_Bool WriteSectionXML ( SAPDB_Int4	fileDescriptor,
							 Reg_SectionP	section			);

SAPDB_Bool WriteKeyXML ( SAPDB_Int4	fileDescriptor,
						 Reg_KeyP		key				);

/*===========================================================================
 *===
 *===	Public Methods
 *===
 *===========================================================================*/

/*!***************************************************************************

	Reg_ExportRegistry

******************************************************************************/

SAPDB_Bool Reg_ExportRegistry ( const char	*registryPath,
								const char	*exportPath		)
{

	Reg_RegistryP	registry = NULL;

	/* Open registry */
	if ( !Reg_OpenRegistry( &registry, registryPath ))
		return SAPDB_FALSE;

	/* Export registry as an XML stream */
	if ( !WriteRegistryXML ( registry, exportPath )) {
		Reg_CloseRegistry( registry );
		registry = NULL;
		return SAPDB_FALSE;
	}

	/* Close registry */
	Reg_CloseRegistry( registry );

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_OpenRegistry

******************************************************************************/

SAPDB_Bool Reg_OpenRegistry ( Reg_RegistryP	*registry,
							  const char	*path		)
{

	return OpenRegistryWithOpenFlag ( registry, path, sp5vf_read );

}

/*===========================================================================*/

SAPDB_Bool Reg_OpenRegistryForUpdate ( Reg_RegistryP    *registry,
                                       const char       *path		)
{

	return OpenRegistryWithOpenFlag(registry, path, sp5vf_readwrite);

}

/*===========================================================================*/

SAPDB_Bool Reg_OpenRegistryFile ( Reg_RegistryP *registry,
                                  const char    *file )
{

	return OpenFileAsRegistry(registry, file);

}

/*!***************************************************************************

	WriteRegistry

******************************************************************************/

SAPDB_Bool Reg_WriteRegistry ( Reg_RegistryP    registry,
                               SAPDB_Bool       withCheck )
{

#ifdef REG_UNIX
	return Reg_UnixWriteRegistry( registry );
#else
	return Reg_WinWriteRegistry( registry, withCheck );
#endif
}

/*!***************************************************************************

	Reg_CloseRegistry

******************************************************************************/

SAPDB_Bool Reg_CloseRegistry ( Reg_RegistryP	registry	)
{

#ifdef REG_UNIX
	return Reg_UnixCloseRegistry( registry );
#else
	return Reg_WinCloseRegistry( registry );
#endif

}

/*!***************************************************************************

	Reg_ReadRegistry

******************************************************************************/

SAPDB_Bool Reg_ReadRegistry ( Reg_RegistryP	registry	)
{

#ifdef REG_UNIX
	return Reg_UnixReadRegistry( registry );
#else
	return Reg_WinReadRegistry( registry );
#endif

}

/*!***************************************************************************

	Reg_GetRegistryKey

******************************************************************************/

SAPDB_Bool Reg_GetRegistryKey ( Reg_RegistryP	 registry,
								const char		*sectionName,
								const char		*keyName,
								char			*value,
								SAPDB_UInt4		 maxValueLen	)
{

	Reg_SectionP	section = NULL;
	Reg_KeyP		key = NULL;

	/* Check input parameters */
	if ( !registry || !sectionName || !keyName || !value )
		return SAPDB_FALSE;

	/* Initialize ouput parameters */
	value[0] = '\0';

	/* Find section */
	if ( !Reg_CommonFindSection( registry, registry->sections, sectionName, &section, NULL ))
		return SAPDB_FALSE;

	/* Section found? */
	if ( !section )
		return SAPDB_FALSE;

	/* Find key */
	if ( !Reg_CommonFindKey( section->keys, keyName, &key ))
		return SAPDB_FALSE;

	/* Key found? */
	if ( !key )
		return SAPDB_FALSE;

	/* Get value */
	strncpy( value, key->value, maxValueLen );
	value[maxValueLen-1] = '\0';

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_SectionExists

******************************************************************************/

SAPDB_Bool Reg_SectionExists( Reg_RegistryP	 registry,
							  const char	*sectionName,
							  SAPDB_Bool	*found		)
{

	Reg_SectionP	section = NULL;

	/* Inits */
	*found = SAPDB_FALSE;

	/* Check input parameters */
	if ( !registry || !sectionName || !found )
		return SAPDB_FALSE;

	/* Find section */
	if ( !Reg_CommonFindSection( registry, registry->sections, sectionName, &section, NULL ))
		return SAPDB_FALSE;

	/* Section found? */
	if ( section )
		*found = SAPDB_TRUE;

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_KeyExists

******************************************************************************/

SAPDB_Bool Reg_KeyExists( Reg_RegistryP	 registry,
						  const char	*sectionName,
						  const char	*keyName,
						  SAPDB_Bool	*found		)
{

	Reg_SectionP	section = NULL;
	Reg_KeyP		key = NULL;

	/* Inits */
	*found = SAPDB_FALSE;

	/* Check input parameters */
	if ( !registry || !sectionName || !keyName || !found)
		return SAPDB_FALSE;

	/* Find section */
	if ( !Reg_CommonFindSection( registry, registry->sections, sectionName, &section, NULL ))
		return SAPDB_FALSE;

	/* Section found? */
	if ( !section )
		return SAPDB_TRUE;

	/* Find key */
	if ( !Reg_CommonFindKey( section->keys, keyName, &key ))
		return SAPDB_FALSE;

	if ( key )
		*found = SAPDB_TRUE;

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_EnumRegistryKeys

******************************************************************************/

SAPDB_Bool Reg_EnumRegistryKeys ( Reg_RegistryP  registry,
								  const char    *sectionName,
								  SAPDB_Bool    *keysExists		)
{

	Reg_SectionP	section = NULL;
	Reg_KeyP		key = NULL;

	/* Check input parameters */
	if ( !registry || !sectionName )
		return SAPDB_FALSE;

	/* Find section */
	if ( !Reg_CommonFindSection( registry, registry->sections, sectionName, &section, NULL ))
		return SAPDB_FALSE;

	/* Section found? */
	if ( !section )
		return SAPDB_FALSE;

	/* Is there at least one key? */
	if ( section->keys ) {
		if ( keysExists )
			*keysExists = SAPDB_TRUE;

		/* Set enumeration pointer */
		registry->enumKeys = section->keys;
	} else {
		if ( keysExists )
			*keysExists = SAPDB_FALSE;

		/* Set enumeration pointer */
		registry->enumKeys = NULL;
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_GetNextKey

******************************************************************************/

SAPDB_Bool Reg_GetNextKey ( Reg_RegistryP	 registry,
							char		*keyName,
							SAPDB_UInt4	 maxKeyNameLen,
							char		*value,
							SAPDB_UInt4	 maxValueLen,
							SAPDB_Bool	*eok			)
{

	Reg_KeyNodeP	currentKeyNode = NULL;
	Reg_KeyP		currentKey = NULL;

	/* Check input parameters */
	if ( !registry )
		return SAPDB_FALSE;

	/* Initialize ouput parameters */
	if ( keyName )
		keyName[0] = '\0';

	if ( value )
		value[0] = '\0';

	*eok = SAPDB_FALSE;

	/* Is there a key node? */
	if ( !registry->enumKeys ) {
		*eok = SAPDB_TRUE;
		return SAPDB_FALSE;
	}

	/* Get current key node */
	currentKeyNode = registry->enumKeys;

	/* Are there more key nodes?*/
	if ( currentKeyNode->nextKeyNode ) {
		registry->enumKeys = currentKeyNode->nextKeyNode;
	} else {
		registry->enumKeys = NULL;
		*eok = SAPDB_TRUE;
	}

	/* Get current key */
	if ( !currentKeyNode->key )
		return SAPDB_FALSE;

	currentKey = currentKeyNode->key;

	/* Set key */
	if ( keyName ) {
		strncpy( keyName, currentKey->key, maxKeyNameLen );
		keyName[maxKeyNameLen-1] = '\0';
	}

	/* Set value */
	if ( value ) {
		strncpy( value, currentKey->value, maxValueLen );
		value[maxValueLen-1] = '\0';
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_EnumRegistrySections

******************************************************************************/

SAPDB_Bool Reg_EnumRegistrySections ( Reg_RegistryP  registry,
                                      const char    *sectionName,
                                      SAPDB_Bool    *childSectionsExists )
{

	Reg_SectionP	section = NULL;

	/* Check input parameters */
	if ( !registry || !sectionName )
		return SAPDB_FALSE;

    /* Iterate from root? */
    if (sectionName[0] == '\0') {
		registry->enumSections = registry->sections;
		if ( childSectionsExists )
			*childSectionsExists = SAPDB_TRUE;
        else
			*childSectionsExists = SAPDB_FALSE;

        return SAPDB_TRUE;
    }

	/* Find section */
	if ( !Reg_CommonFindSection( registry, registry->sections, sectionName, &section, NULL ))
		return SAPDB_FALSE;

	/* Section found? */
	if ( !section )
		return SAPDB_FALSE;

	/* Is there at least one child section? */
	if ( section->childSections ) {
		if ( childSectionsExists )
			*childSectionsExists = SAPDB_TRUE;

		/* Set enumeration pointer */
		registry->enumSections = section->childSections;
	} else {
		if ( childSectionsExists )
			*childSectionsExists = SAPDB_FALSE;

		/* Set enumeration pointer */
		registry->enumSections = NULL;
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_GetNextSection

******************************************************************************/

SAPDB_Bool Reg_GetNextSection ( Reg_RegistryP    registry,
                                char            *sectionPath,
                                SAPDB_UInt4      maxSectionPathLen,
                                char            *sectionName,
                                SAPDB_UInt4      maxSectionNameLen,
                                SAPDB_Bool      *eos )
{

	Reg_SectionNodeP	currentSectionNode = NULL;
	Reg_SectionP		currentSection = NULL;

	/* Check input parameters */
	if ( !registry )
		return SAPDB_FALSE;

	/* Initialize ouput parameters */
	if ( sectionName )
		sectionName[0] = '\0';

	*eos = SAPDB_FALSE;

	/* Is there a section node? */
	if ( !registry->enumSections ) {
		*eos = SAPDB_TRUE;
		return SAPDB_FALSE;
	}

	/* Get current section node */
	currentSectionNode = registry->enumSections;

	/* Are there more section nodes?*/
	if ( currentSectionNode->nextSectionNode ) {
		registry->enumSections = currentSectionNode->nextSectionNode;
	} else {
		registry->enumSections = NULL;
		*eos = SAPDB_TRUE;
	}

	/* Get current section */
	if ( !currentSectionNode->section )
		return SAPDB_FALSE;

	currentSection = currentSectionNode->section;

	/* Set section path */
	if ( sectionPath ) {
		strncpy( sectionPath, currentSection->sectionPath, maxSectionPathLen );
		sectionPath[maxSectionPathLen-1] = '\0';
	}

	/* Set section name */
	if ( sectionName ) {
		strncpy( sectionName, currentSection->sectionName, maxSectionNameLen );
		sectionName[maxSectionNameLen-1] = '\0';
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_DeleteRegistryKey

******************************************************************************/

SAPDB_Bool Reg_DeleteRegistryKey ( const char	*registryPath,
								   const char	*sectionName,
								   const char	*keyName		)
{

	Reg_RegistryP	writeRegistry = NULL;
	Reg_SectionP	section = NULL;

	/* Open registry */
	if ( !OpenRegistryWithOpenFlag( &writeRegistry, registryPath, sp5vf_readwrite ))
		return SAPDB_FALSE;

	/* Find section to modify */
	if ( !Reg_CommonFindSection( writeRegistry, writeRegistry->sections, sectionName, &section, NULL ))
		return SAPDB_FALSE;

	/* Delete key */
	if ( !DeleteKey( section, keyName ))
		return SAPDB_FALSE;

	/* Write registry */
	if ( !Reg_WriteRegistry(writeRegistry, SAPDB_TRUE))
		return SAPDB_FALSE;

	/* Close registry */
	Reg_CloseRegistry( writeRegistry );

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_DeleteRegistrySection

******************************************************************************/

SAPDB_Bool Reg_DeleteRegistrySection ( const char	*registryPath,
									   const char	*sectionName	)
{

	Reg_RegistryP		writeRegistry = NULL;
	Reg_SectionP		section = NULL;
	Reg_SectionNodeP	sectionNode = NULL;

	/* Open registry */
	if ( !OpenRegistryWithOpenFlag( &writeRegistry, registryPath, sp5vf_readwrite ))
		return SAPDB_FALSE;

	/* Find section to delete */
	if ( !Reg_CommonFindSection( writeRegistry, writeRegistry->sections, sectionName, &section, &sectionNode ))
		return SAPDB_FALSE;

	/* Drop section */
	if ( !Reg_CommonDropSection( section ))
		return SAPDB_FALSE;

	if ( sectionNode ) {
		/* Is section node a child or the next node of its parent node? */
		if ( sectionNode->previousSectionNode->section->childSections == sectionNode ) {
			/* Skip section node, which is to be deleted */
			sectionNode->previousSectionNode->section->childSections = sectionNode->nextSectionNode;
		} else if ( sectionNode->previousSectionNode->nextSectionNode == sectionNode ) {
			/* Skip section node, which is to be deleted */
			sectionNode->previousSectionNode->nextSectionNode = sectionNode->nextSectionNode;
		}
		/* There is no parent. The node which is to be deleted is the root one */
		else {
			/* Set new root node */
			writeRegistry->sections = sectionNode->nextSectionNode;
		}

		/* Free section node */
		sqlfree((tsp00_Uint1 *) sectionNode );
	}

	/* Write registry */
	if ( !Reg_WriteRegistry(writeRegistry, SAPDB_TRUE))
		return SAPDB_FALSE;

	/* Close registry */
	Reg_CloseRegistry( writeRegistry );

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_SetRegistryKey

******************************************************************************/

SAPDB_Bool Reg_SetRegistryKey ( const char	*registryPath,
								const char	*sectionName,
								const char	*keyName,
								const char	*value		)
{

	Reg_RegistryP		writeRegistry = NULL;
	Reg_SectionP		section = NULL;
	Reg_SectionNodeP	sectionNode = NULL;
	Reg_KeyP			key = NULL;

	/* Open registry */
	if ( !OpenRegistryWithOpenFlag( &writeRegistry, registryPath, sp5vf_readwrite ))
		return SAPDB_FALSE;

	/* Find section to modify */
	if ( !Reg_CommonFindSection( writeRegistry, writeRegistry->sections, sectionName, &section, NULL ))
		return SAPDB_FALSE;

	/* Section found? */
	if ( section ) {
		/* Find key to modify */
		if ( !Reg_CommonFindKey( section->keys, keyName, &key ))
			return SAPDB_FALSE;

		/* Key found? */
		if ( key ) {
			/* Set new value */
			strncpy( key->value, value, REG_MAX_VALUE_LEN );
			key->value[REG_MAX_VALUE_LEN] = '\0';
		} else {
			/* Create key */
			if ( !Reg_CommonCreateNewKey( keyName, value, &key ))
				return SAPDB_FALSE;

			/* Add key to section */
			if ( !Reg_CommonAddKey ( section, key ))
				return SAPDB_FALSE;
		}
	} else {

		/* Add section */
		if ( !Reg_CommonInsertSection( writeRegistry, sectionName, &sectionNode ))
			return SAPDB_FALSE;

		if ( !sectionNode || !sectionNode->section) {

			return SAPDB_FALSE;
		}

		/* Create key */
		if ( !Reg_CommonCreateNewKey( keyName, value, &key ))
			return SAPDB_FALSE;

		/* Add key to section */
		if ( !Reg_CommonAddKey ( sectionNode->section, key ))
			return SAPDB_FALSE;
	}

	/* Write registry */
	if (!Reg_WriteRegistry(writeRegistry, SAPDB_TRUE))
		return SAPDB_FALSE;

	/* Close registry */
	Reg_CloseRegistry( writeRegistry );

	return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Reg_SetRegistryKeyInMemory ( Reg_RegistryP    registry,
                                        const char      *sectionName,
                                        const char      *keyName,
                                        const char      *value )
{

	Reg_SectionP		section = NULL;
	Reg_SectionNodeP	sectionNode = NULL;
	Reg_KeyP			key = NULL;

	/* Find section to modify */
	if ( !Reg_CommonFindSection( registry, registry->sections, sectionName, &section, NULL ))
		return SAPDB_FALSE;

	/* Section found? */
	if ( section ) {
		/* Find key to modify */
		if ( !Reg_CommonFindKey( section->keys, keyName, &key ))
			return SAPDB_FALSE;

		/* Key found? */
		if ( key ) {
			/* Set new value */
			strncpy( key->value, value, REG_MAX_VALUE_LEN );
			key->value[REG_MAX_VALUE_LEN] = '\0';
		} else {
			/* Create key */
			if ( !Reg_CommonCreateNewKey( keyName, value, &key ))
				return SAPDB_FALSE;

			/* Add key to section */
			if ( !Reg_CommonAddKey ( section, key ))
				return SAPDB_FALSE;
		}
	} else {

		/* Add section */
		if ( !Reg_CommonInsertSection( registry, sectionName, &sectionNode ))
			return SAPDB_FALSE;

		if ( !sectionNode || !sectionNode->section) {

			return SAPDB_FALSE;
		}

		/* Create key */
		if ( !Reg_CommonCreateNewKey( keyName, value, &key ))
			return SAPDB_FALSE;

		/* Add key to section */
		if ( !Reg_CommonAddKey ( sectionNode->section, key ))
			return SAPDB_FALSE;
	}

	return SAPDB_TRUE;

}

/*===========================================================================*
 *===
 *===	Private Methods
 *===
 *===========================================================================*/

/*!***************************************************************************

	OpenRegistryWithOpenFlag

******************************************************************************/

SAPDB_Bool OpenRegistryWithOpenFlag ( Reg_RegistryP		*registry,
									  const char		*path,
									  tsp05_RteFileMode	 openFlag	)
{

#ifdef REG_UNIX
	return Reg_UnixOpenRegistry ( registry, path, openFlag );
#else
	return Reg_WinOpenRegistry ( registry, path );
#endif

}

/*===========================================================================*/

SAPDB_Bool OpenFileAsRegistry ( Reg_RegistryP   *registry,
                                const char      *file )
{

#ifdef REG_UNIX
	return Reg_UnixOpenRegistry(registry, file, sp5vf_read);
#else
	return Reg_WinOpenFileAsRegistry(registry, file, sp5vf_read);
#endif

}

/*!***************************************************************************

	WriteRegistryXML

******************************************************************************/

SAPDB_Bool WriteRegistryXML ( Reg_RegistryP		 registry,
							  const char	*exportPath	)
{

	SAPDB_Int4			exportFile = 0;
	tsp05_RteFileError	err;
	Reg_SectionNodeP		sectionNode = NULL;

	/* Open export file */
	sqlfopenc( exportPath, sp5vf_binary, sp5vf_write, sp5bk_buffered, &exportFile, &err );
	if ( err.sp5fe_result != vf_ok ) {
		return SAPDB_FALSE;
	}

	/* Go to the begin of the file */
	sqlfseekc( exportFile, 0, sp5vf_seek_begin, &err );
	if ( err.sp5fe_result != vf_ok ) {
		return SAPDB_FALSE;
	}

	/* Write XML header */
	if ( !WriteExportHeaderXML ( exportFile, registry )) {
		return SAPDB_FALSE;
	}

	/* Write sections header */
	if ( !Reg_CommonWriteLine( exportFile, STR_XML_SECTION_LIST_START ))
		return SAPDB_FALSE;

	/* Write all sections */
	sectionNode = registry->sections;
	while ( sectionNode ) {
		/* Write current section */
		if ( !WriteSectionXML( exportFile, sectionNode->section )) {
			return SAPDB_FALSE;
		}

		/* Get next section */
		sectionNode = sectionNode->nextSectionNode;
	}

	/* Write sections header */
	if ( !Reg_CommonWriteLine( exportFile, STR_XML_SECTION_LIST_END ))
		return SAPDB_FALSE;

	/* Write registry tag */
	if ( !Reg_CommonWriteLine( exportFile, STR_XML_REGISTRY_END ))
		return SAPDB_FALSE;

	/* Close export file */
	sqlfclosec( exportFile, sp5vf_close_normal, &err );

	return SAPDB_TRUE;

}

/*!***************************************************************************

	WriteExportHeaderXML

******************************************************************************/

SAPDB_Bool WriteExportHeaderXML ( SAPDB_Int4	fileDescriptor,
								  Reg_RegistryP		registry		)
{

	/* Check */
	if ( !registry || fileDescriptor == -1 )
		return SAPDB_FALSE;

	/* Write XML header */
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_HEADER ))
		return SAPDB_FALSE;

	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_REGISTRY_START ))
		return SAPDB_FALSE;

	if ( !Reg_CommonWrite( fileDescriptor, STR_XML_REGISTRY_PATH_START ))
		return SAPDB_FALSE;
	if ( !Reg_CommonWrite( fileDescriptor, registry->file ))
		return SAPDB_FALSE;
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_REGISTRY_PATH_END ))
		return SAPDB_FALSE;

	return SAPDB_TRUE;

}

/*!***************************************************************************

	WriteSectionXML

******************************************************************************/

SAPDB_Bool WriteSectionXML ( SAPDB_Int4	fileDescriptor,
							 Reg_SectionP	section			)
{

	Reg_KeyNodeP		currentKeyNode = NULL;
	Reg_SectionNodeP	currentChildSectionNode = NULL;

	/* Check */
	if ( !section || fileDescriptor == -1 )
		return SAPDB_FALSE;

	/* Write section start */
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_SECTION_START ))
		return SAPDB_FALSE;

	/* Write section name */
	if ( !Reg_CommonWrite( fileDescriptor, STR_XML_SECTION_NAME_START ))
		return SAPDB_FALSE;
	if ( !Reg_CommonWrite( fileDescriptor, section->sectionName ))
		return SAPDB_FALSE;
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_SECTION_NAME_END ))
		return SAPDB_FALSE;

	/* Write section path */
	if ( !Reg_CommonWrite( fileDescriptor, STR_XML_SECTION_PATH_START ))
		return SAPDB_FALSE;
	if ( !Reg_CommonWrite( fileDescriptor, section->sectionPath ))
		return SAPDB_FALSE;
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_SECTION_PATH_END ))
		return SAPDB_FALSE;

	/* Write key list start */
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_KEY_LIST_START ))
		return SAPDB_FALSE;

	/* Get current key node */
	currentKeyNode = section->keys;
	while( currentKeyNode ) {
		/* Write current key */
		if ( !WriteKeyXML( fileDescriptor, currentKeyNode->key ))
			return SAPDB_FALSE;

		/* Get next key node */
		currentKeyNode = currentKeyNode->nextKeyNode;
	}

	/* Write key list end */
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_KEY_LIST_END ))
		return SAPDB_FALSE;

	/* Write section list start */
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_CHILD_SECTION_LIST_START ))
		return SAPDB_FALSE;

	/* Write all child sections */
	currentChildSectionNode = section->childSections;
	while( currentChildSectionNode ) {
		/* Write current child section */
		if ( !WriteSectionXML ( fileDescriptor, currentChildSectionNode->section ))
			return SAPDB_FALSE;

		/* Get next child section node */
		currentChildSectionNode = currentChildSectionNode->nextSectionNode;
	}

	/* Write section list end */
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_CHILD_SECTION_LIST_END ))
		return SAPDB_FALSE;

	/* Write section end */
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_SECTION_END ))
		return SAPDB_FALSE;

	return SAPDB_TRUE;

}

/*!***************************************************************************

	WriteKeyXML

******************************************************************************/

SAPDB_Bool WriteKeyXML ( SAPDB_Int4	fileDescriptor,
						 Reg_KeyP		key				)
{

	/* Check */
	if ( !key || fileDescriptor == -1 )
		return SAPDB_FALSE;

	/* Write key start */
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_KEY_START ))
		return SAPDB_FALSE;

	/* Write key name */
	if ( !Reg_CommonWrite( fileDescriptor, STR_XML_KEY_NAME_START ))
		return SAPDB_FALSE;
	if ( !Reg_CommonWrite( fileDescriptor, key->key ))
		return SAPDB_FALSE;
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_KEY_NAME_END ))
		return SAPDB_FALSE;

	/* Write key value */
	if ( !Reg_CommonWrite( fileDescriptor, STR_XML_KEY_VALUE_START ))
		return SAPDB_FALSE;
	if ( !Reg_CommonWriteLine( fileDescriptor, key->value ))
		return SAPDB_FALSE;
	if ( !Reg_CommonWrite( fileDescriptor, STR_XML_KEY_VALUE_END ))
		return SAPDB_FALSE;

	/* Write key end */
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_XML_KEY_END ))
		return SAPDB_FALSE;

	return SAPDB_TRUE;

}

/*===========================================================================
 *===
 *===	End
 *===
 *===========================================================================*/
