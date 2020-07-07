/*!**********************************************************************

  module:	Reg_Common.h

  -----------------------------------------------------------------------

  responsible:	Markus Özgen

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

#ifndef REGCOMMON_H
#define REGCOMMON_H

#if defined WIN32
#define REG_WIN32
#else
#define REG_UNIX
#endif

/*===========================================================================
 *===
 *===	Includes
 *===
 *===========================================================================*/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"
#include "hwd03waapi.h"	/* Trace functionality	*/
#include "gsp05.h"

/*===========================================================================
 *===
 *===	Defines
 *===
 *===========================================================================*/

#define REG_MAX_PATH_LEN			1023
#define REG_MAX_SECTION_LEN			1023
#define	REG_MAX_KEY_LEN				128
#define REG_MAX_VALUE_LEN			1023

#define MAX_BUFFER_LEN				4096

#ifdef WIN32
    #ifndef strncasecmp
        #define strcasecmp(s1, s2)	    stricmp(s1, s2)
    #endif
    #ifndef strncasecmp
        #define strncasecmp(s1, s2, l)  strnicmp(s1, s2, l)
    #endif
#endif

/*===========================================================================
 *===
 *===	Types & Structs
 *===
 *===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* Registry entry definitions*/
typedef struct st_registry		Reg_Registry;
typedef struct st_section_node	Reg_SectionNode;
typedef struct st_section		Reg_Section;
typedef struct st_key_node		Reg_KeyNode;
typedef struct st_key			Reg_Key;

typedef struct st_registry		*Reg_RegistryP;
typedef struct st_section_node	*Reg_SectionNodeP;
typedef struct st_section		*Reg_SectionP;
typedef struct st_key_node		*Reg_KeyNodeP;
typedef struct st_key			*Reg_KeyP;

struct st_registry {
	char		        sectionPath[REG_MAX_PATH_LEN+1];

	tsp05_RteFileMode	openFlag;
	SAPDB_Int4			fileDescriptor;
	char		        file[REG_MAX_PATH_LEN+1];

	Reg_SectionNodeP	sections;
	Reg_KeyNodeP		enumKeys;
	Reg_SectionNodeP	enumSections;
};

struct st_section_node {
	Reg_SectionP		section;
	Reg_SectionNodeP	previousSectionNode;
	Reg_SectionNodeP	nextSectionNode;
};

struct st_section {
	char				sectionPath[REG_MAX_SECTION_LEN+1];
	char				sectionName[REG_MAX_SECTION_LEN+1];
	Reg_SectionNodeP	childSections;
	Reg_KeyNodeP		keys;
};

struct st_key_node {
	Reg_KeyP		key;
	Reg_KeyNodeP	nextKeyNode;
};

struct st_key {
    char	key[REG_MAX_KEY_LEN+1];
    char	value[REG_MAX_VALUE_LEN+1];
};

#ifdef __cplusplus
}
#endif

/*===========================================================================
 *===
 *===	Interface Definition
 *===
 *===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

SAPDB_Bool Reg_CommonInsertSection ( Reg_RegistryP		 registry,
									 const char			*section,
									 Reg_SectionNodeP	*sectionNode	);

SAPDB_Bool Reg_CommonInsertKey ( Reg_RegistryP	 registry,
								 const char		*section,
								 const char		*key,
								 const char		*value		);

SAPDB_Bool Reg_CommonDropSection ( Reg_SectionP	section	);

SAPDB_Bool Reg_CommonFindSection ( Reg_RegistryP	 registry,
                                   Reg_SectionNodeP	 searchNode,
								   const char		*sectionPath,
								   Reg_SectionP		*section,
								   Reg_SectionNodeP	*sectionNode	);

SAPDB_Bool Reg_CommonFindKey ( Reg_KeyNodeP	 searchNode,
							   const char	*keyName,
							   Reg_KeyP		*key		);

SAPDB_Bool Reg_CommonCreateNewKey ( const char	*keyName,
									const char	*value,
									Reg_KeyP	*key		);

SAPDB_Bool Reg_CommonAddKey ( Reg_SectionP	section,
							  Reg_KeyP		newKey		);

SAPDB_Bool Reg_CommonWrite ( SAPDB_Int4	 fileDescriptor,
							 char		*line			);

SAPDB_Bool Reg_CommonWriteLine ( SAPDB_Int4	 fileDescriptor,
								 char		*line			);

SAPDB_Bool Reg_CommonGetNextLine ( SAPDB_Int4    file,
                                   char         *line,
                                   SAPDB_UInt4   maxLineSize,
                                   SAPDB_Bool   *eof,
                                   SAPDB_Bool    replaceDoubleBackSlash );
#ifdef __cplusplus
}
#endif

#endif
