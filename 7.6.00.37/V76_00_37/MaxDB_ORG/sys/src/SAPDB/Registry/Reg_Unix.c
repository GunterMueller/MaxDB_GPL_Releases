/*!**********************************************************************

  module:	Reg_Unix.c

  -----------------------------------------------------------------------

  responsible:  Markus Özgen

  special area: Registry
  description : Handling of registry entries - Unix functions

	For Unix registry entries will be stored in INI files
    Each .ini File consists of sections, which are identified by a section header.
    The section header consists of a name framed by brackets.
    Each entry in the section consists of a key and value separated by an equal sign,
	or is a sub section.

    Example

    [Database]
    SUT72=/home/remuser/releases/V72/usr
    MUT7264=/home/remuser/releases/V7264/usr
	[Database/Services]
	Service1=MyService


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

#   FreeBSD portions added by Kai Mosebach, 
#   For more informations see : http://www.komadev.de/sapdb
#

************************************************************************/

/*
 * first check if any UNIX platform is defined
 */
#if !(defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined HPUX || defined(LINUX) || defined FREEBSD)
#error SUN, SOLARIS, AIX, OSF1, NMP, HPUX, FREEBSD or LINUX not defined !
#endif


/*===========================================================================*
 *===
 *===	Includes
 *===
 *===========================================================================*/

#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>

#include "SAPDB/Registry/Reg_Unix.h"

/* Include if module has been made in QUICK mode and		*/
/* only to debug as long as registry is part of WebAgent	*/
/* This concern also TRACE macros							*/
#include "hwd03waapi.h"

/****************************************************************************
 ***
 *** Module
 ***
 ****************************************************************************/

sapdbwa_TraceDefModule ("Reg_Registry.c")

/*===========================================================================*
 *===
 *===	Defines
 *===
 *===========================================================================*/

#define WITH_TRACE              0
#if WITH_TRACE
    #define WriteTraceMethod(a) printf("\n\n%s\n",a)

    #define WriteTrace(a)       printf(a)
    #define WriteTrace1(a,b)    printf(a,b)
    #define WriteTrace2(a,b,c)  printf(a,b,c)
#else
    #define WriteTraceMethod(a)

    #define WriteTrace(a)
    #define WriteTrace1(a,b)
    #define WriteTrace2(a,b,c)
#endif

/*---------------------------------------------------------------------------*/

#define STR_NEWLINE	"\n"

#define STR_SECTION_OPEN "["
#define STR_SECTION_CLOSE "]"
#define STR_EQUAL "="

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

SAPDB_Bool WriteSection ( SAPDB_Int4	fileDescriptor,
						  Reg_SectionP	section			);

SAPDB_Bool WriteKey ( SAPDB_Int4	fileDescriptor,
					  Reg_KeyP		key				);


SAPDB_Bool SplitKeyLine ( const char	*keyLine,
						  char			*keyName,
						  char			*value		);

/*===========================================================================*
 *===
 *===	Public Methods
 *===
 *===========================================================================*/

/*!***************************************************************************

	Reg_UnixOpenRegistry

******************************************************************************/

SAPDB_Bool Reg_UnixOpenRegistry ( Reg_RegistryP		*registry,
								  const char		*path,
								  tsp05_RteFileMode_Param openFlag	)
{
	Reg_RegistryP		newRegistry = NULL;
	SAPDB_Bool			ok = SAPDB_FALSE;

    WriteTraceMethod("Reg_UnixOpenRegistry\n");

	if ( !registry ) {
        WriteTrace("Reg_UnixOpenRegistry::Exiting unexpected!\n");
        WriteTrace1("Reg_UnixOpenRegistry::!registry => %d\n", !registry);

        return SAPDB_FALSE;
	}

	sqlallocat( sizeof(Reg_Registry), (tsp00_Uint1 **) registry, &ok );

	if ( !ok ) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	newRegistry = *registry;

	newRegistry->sectionPath[0] = '\0';
	sp77sprintf(newRegistry->file, REG_MAX_PATH_LEN, "%s", path);
    /*
    strncpy( newRegistry->path, path, REG_MAX_PATH_LEN );
	newRegistry->path[REG_MAX_PATH_LEN] = '\0';
    */
	newRegistry->openFlag = openFlag;
	newRegistry->fileDescriptor = -1;
	newRegistry->sections = NULL;
	newRegistry->enumKeys = NULL;
	newRegistry->enumSections = NULL;

    WriteTrace("Reg_UnixOpenRegistry::Reg_UnixReadRegistry\n");
	if ( !Reg_UnixReadRegistry( newRegistry )) {
        WriteTrace("Reg_UnixOpenRegistry::Exiting unexpected!\n");
        WriteTrace("Reg_UnixOpenRegistry::!Reg_UnixReadRegistry => 1\n");

        Reg_UnixCloseRegistry( newRegistry );
		*registry = NULL;

		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_UnixCloseRegistry

******************************************************************************/

SAPDB_Bool Reg_UnixCloseRegistry ( Reg_RegistryP	registry	)
{

	Reg_SectionNodeP	currentSectionNode = NULL;
	Reg_SectionNodeP	nextSectionNode = NULL;
	tsp05_RteFileError	err;

	sapdbwa_TraceDefFunction( "Reg_UnixCloseRegistry" )

	if ( !registry ) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_TRUE;
	}

	/* Close file descriptor, if exists */
	if ( registry->openFlag == sp5vf_readwrite && registry->fileDescriptor != -1 ) {
		sqlfclosec( registry->fileDescriptor, sp5vf_close_normal, &err );
		registry->fileDescriptor = -1;
	}

	currentSectionNode = registry->sections;
	while ( currentSectionNode ) {
		nextSectionNode = currentSectionNode->nextSectionNode;

		if ( currentSectionNode->section ) {
			Reg_CommonDropSection( currentSectionNode->section );
		}

		sqlfree((tsp00_Uint1 *) currentSectionNode );

		currentSectionNode = nextSectionNode;
	}

	sqlfree((tsp00_Uint1 *) registry );

	return SAPDB_TRUE;

}

/*!***************************************************************************

	Reg_UnixReadRegistry

******************************************************************************/

SAPDB_Bool Reg_UnixReadRegistry ( Reg_RegistryP	registry	)
{
	SAPDB_Int4			file = -1;
	tsp05_RteFileError	err;
	char				buffer			[MAX_BUFFER_LEN+1];
	char				currentSection	[REG_MAX_SECTION_LEN+1];
	SAPDB_UInt4			sectionLen = 0;
	SAPDB_Bool			eof = SAPDB_FALSE;

	char	key		[REG_MAX_KEY_LEN+1];
	char	value	[REG_MAX_VALUE_LEN+1];

    WriteTraceMethod("Reg_UnixReadRegistry\n");

	buffer[0] = '\0';
	currentSection[0] = '\0';

	/* Checks */
	if ( !registry ) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	/* Open registry */
    WriteTrace("Reg_UnixReadRegistry::sqlfopen\n");
    WriteTrace1("Reg_UnixReadRegistry::registry->file => %d\n", registry->file);
	if ( registry->openFlag == sp5vf_readwrite )
		sqlfopenc( registry->file, sp5vf_binary, registry->openFlag, sp5bk_unbuffered, &file, &err );
	else
		sqlfopenc( registry->file, sp5vf_binary, registry->openFlag, sp5bk_buffered, &file, &err );
	if ( err.sp5fe_result != vf_ok ) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	/* Is file opened to write? */
    WriteTrace1("Reg_UnixReadRegistry::registry->openFlag == sp5vf_readwrite => %d\n", registry->openFlag == sp5vf_readwrite);
	if ( registry->openFlag == sp5vf_readwrite ) {
		/* Store file descriptor */
		registry->fileDescriptor = file;
	} else {
		registry->fileDescriptor = -1;
	}

	do {
		if ( !Reg_CommonGetNextLine(file, buffer, MAX_BUFFER_LEN, &eof, SAPDB_FALSE)) {
			sqlfclosec( file, sp5vf_close_normal, &err );

			sapdbwa_TraceAdd( "Exiting unexpected!" )
			return SAPDB_FALSE;
		}

		if ( eof ) break;

		if ( buffer[0] == '\0' )
			continue;

		if ( buffer[0] == '[' && (sectionLen = strlen(buffer)) > 2 ) {
			/* Get section name */
			strcpy( currentSection, buffer+1 );
			currentSection[sectionLen-2] = '\0';

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

/*!***************************************************************************

	Reg_UnixWriteRegistry

******************************************************************************/

SAPDB_Bool Reg_UnixWriteRegistry ( Reg_RegistryP	registry	)
{

	Reg_SectionNodeP	currentSectionNode = NULL;
	SAPDB_Int4			file;
	tsp05_RteFileError	err;

    WriteTraceMethod("Reg_UnixWriteRegistry>>\n");

	/* Make some checks */
	if (!registry || registry->fileDescriptor == -1 || registry->openFlag != sp5vf_readwrite) {
        WriteTrace("Reg_UnixWriteRegistry::Exiting unexpected!<<\n");
        WriteTrace1("Reg_UnixWriteRegistry::!registry => %d\n", !registry);
        WriteTrace1("Reg_UnixWriteRegistry::registry->fileDescriptor == -1 => %d\n", registry->fileDescriptor == -1);
        WriteTrace1("Reg_UnixWriteRegistry::registry->openFlag != sp5vf_readwrite => %d\n", registry->openFlag != sp5vf_readwrite);

        return SAPDB_FALSE;
	}

	/* Delete content of current file */
    WriteTrace("Reg_UnixWriteRegistry::sqlftruncatec>>\n");
	sqlftruncatec( registry->fileDescriptor, 0, &err );
	if ( err.sp5fe_result != vf_ok ) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

    WriteTrace("Reg_UnixWriteRegistry::sqlfseekc>>\n");
	sqlfseekc( registry->fileDescriptor, 0, sp5vf_seek_begin, &err );
	if ( err.sp5fe_result != vf_ok ) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}
 
	/* Write all sections */
    WriteTrace("Reg_UnixWriteRegistry::WriteAllSections>>\n");
	currentSectionNode = registry->sections;
	while ( currentSectionNode ) {
		/* Write current section */
		if ( !WriteSection( registry->fileDescriptor, currentSectionNode->section )) {
			sapdbwa_TraceAdd( "Exiting unexpected!" )
			return SAPDB_FALSE;
		}

		/* Get next section */
		currentSectionNode = currentSectionNode->nextSectionNode;
	}

	return SAPDB_TRUE;

}

/*===========================================================================*
 *===
 *===	Private Methods
 *===
 *===========================================================================*/

/*!***************************************************************************

	WriteSection

******************************************************************************/

SAPDB_Bool WriteSection ( SAPDB_Int4	fileDescriptor,
						  Reg_SectionP	section			)
{

	Reg_KeyNodeP		currentKeyNode = NULL;
	Reg_SectionNodeP	currentChildSectionNode = NULL;

	sapdbwa_TraceDefFunction( "WriteSection" )

	/* Check */
	if ( !section || fileDescriptor == -1 ) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	/* Write section name */
	if ( !Reg_CommonWrite( fileDescriptor, STR_SECTION_OPEN )) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}
	if ( !Reg_CommonWrite( fileDescriptor, section->sectionPath )) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}
	if ( !Reg_CommonWriteLine( fileDescriptor, STR_SECTION_CLOSE )) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	/* Get current key node */
	currentKeyNode = section->keys;
	while( currentKeyNode ) {
		/* Write current key */
		if ( !WriteKey( fileDescriptor, currentKeyNode->key )) {
			sapdbwa_TraceAdd( "Exiting unexpected!" )
			return SAPDB_FALSE;
		}

		/* Get next key node */
		currentKeyNode = currentKeyNode->nextKeyNode;
	}

	/* Write blank line */
	if ( !Reg_CommonWrite( fileDescriptor, STR_NEWLINE )) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	/* Write all child sections */
	currentChildSectionNode = section->childSections;
	while( currentChildSectionNode ) {
		/* Write current child section */
		if ( !WriteSection ( fileDescriptor, currentChildSectionNode->section )) {
			sapdbwa_TraceAdd( "Exiting unexpected!" )
			return SAPDB_FALSE;
		}

		/* Get next child section node */
		currentChildSectionNode = currentChildSectionNode->nextSectionNode;
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	WriteKey

******************************************************************************/

SAPDB_Bool WriteKey ( SAPDB_Int4	fileDescriptor,
					  Reg_KeyP			key				)
{

	sapdbwa_TraceDefFunction( "WriteKey" )

	/* Check */
	if ( !key || fileDescriptor == -1 ) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	/* Write key */
	if ( !Reg_CommonWrite( fileDescriptor, key->key)) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	/* Write equal sign */
	if ( !Reg_CommonWrite( fileDescriptor, STR_EQUAL )) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	/* Write value */
	if ( !Reg_CommonWriteLine( fileDescriptor, key->value )) {
		sapdbwa_TraceAdd( "Exiting unexpected!" )
		return SAPDB_FALSE;
	}

	return SAPDB_TRUE;

}

/*!***************************************************************************

	SplitKeyLine

******************************************************************************/

SAPDB_Bool SplitKeyLine ( const char	*keyLine,
						  char			*keyName,
						  char			*value		)
{

	SAPDB_Int4	posSplit = 0;
	SAPDB_Int4	keyLineLen = 0;

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
	strncpy( keyName, keyLine, posSplit );
	keyName[posSplit] = '\0';

	/* No value */
	if ( posSplit + 2 > keyLineLen ) {
		return SAPDB_TRUE;
	}

	/* Set value */
	strncpy( value, keyLine + posSplit + 1, keyLineLen - posSplit - 1 );
	value[keyLineLen - posSplit - 1] = '\0';

	return SAPDB_TRUE;

}

/*===========================================================================*
 *===
 *===	End
 *===
 *===========================================================================*/
