/*!**********************************************************************

  module:	Reg_Registry.h

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

#ifndef REG_REGISTRY_H
#define REG_REGISTRY_H

/*===========================================================================
 *===
 *===	Includes
 *===
 *===========================================================================*/

#include "SAPDB/Registry/Reg_Common.h"

/*===========================================================================
 *===
 *===	Defines
 *===
 *===========================================================================*/

#ifdef REG_UNIX
    #define REG_SAPDB_REGISTRY "/usr/spool/sql/ini/SAPDB73.ini"
#else
    #define REG_SAPDB_REGISTRY "SOFTWARE\\SAP\\SAP DBTech\\"
#endif

/*===========================================================================
 *===
 *===	Types & Structs
 *===
 *===========================================================================*/

/*===========================================================================
 *===
 *===	Interface Definition
 *===
 *===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

SAPDB_Bool Reg_ExportRegistry ( const char	*registryPath,
								const char	*exportPath		);

SAPDB_Bool Reg_OpenRegistry ( Reg_RegistryP *registry,
							  const char    *path );

SAPDB_Bool Reg_OpenRegistryForUpdate ( Reg_RegistryP    *registry,
                                       const char       *path		);

SAPDB_Bool Reg_OpenRegistryFile ( Reg_RegistryP *registry,
                                  const char    *file );

SAPDB_Bool Reg_WriteRegistry ( Reg_RegistryP    registry,
                               SAPDB_Bool       withCheck );

SAPDB_Bool Reg_CloseRegistry ( Reg_RegistryP	registry	);

SAPDB_Bool Reg_ReadRegistry ( Reg_RegistryP	registry	);

SAPDB_Bool Reg_SectionExists( Reg_RegistryP  registry,
                              const char    *sectionName,
                              SAPDB_Bool    *found );

SAPDB_Bool Reg_KeyExists( Reg_RegistryP		 registry,
						  const char	*sectionName,
						  const char	*keyName,
						  SAPDB_Bool	*found		);

SAPDB_Bool Reg_DeleteRegistryKey ( const char	*registryPath,
								   const char	*sectionName,
								   const char	*keyName		);

SAPDB_Bool Reg_DeleteRegistrySection ( const char	*registryPath,
									   const char	*sectionName	);

SAPDB_Bool Reg_GetRegistryKey ( Reg_RegistryP	 registry,
								const char	*sectionName,
								const char	*keyName,
								char		*value,
								SAPDB_UInt4	 maxValueLen	);

SAPDB_Bool Reg_SetRegistryKey ( const char	*registryPath,
								const char	*sectionName,
								const char	*keyName,
								const char	*value		);

SAPDB_Bool Reg_SetRegistryKeyInMemory ( Reg_RegistryP    registry,
                                        const char      *sectionName,
                                        const char      *keyName,
                                        const char      *value );

SAPDB_Bool Reg_EnumRegistryKeys ( Reg_RegistryP		 registry,
								  const char	*sectionName,
								  SAPDB_Bool	*keysExists		);

SAPDB_Bool Reg_GetNextKey ( Reg_RegistryP	 registry,
							char		*keyName,
							SAPDB_UInt4	 maxKeyNameLen,
							char		*value,
							SAPDB_UInt4	 maxValueLen,
							SAPDB_Bool	*eok			);

SAPDB_Bool Reg_EnumRegistrySections ( Reg_RegistryP  registry,
                                      const char    *sectionName,
                                      SAPDB_Bool    *childSectionsExists );


SAPDB_Bool Reg_GetNextSection ( Reg_RegistryP    registry,
                                char            *sectionPath,
                                SAPDB_UInt4      maxSectionPathLen,
                                char            *sectionName,
                                SAPDB_UInt4      maxSectionNameLen,
                                SAPDB_Bool      *eos );


#ifdef __cplusplus
}
#endif

#endif
