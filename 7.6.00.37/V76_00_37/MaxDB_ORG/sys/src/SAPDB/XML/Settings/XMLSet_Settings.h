/*!**********************************************************************

  module: XMLSet_Settings.h

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Settings

  description:  Registry, INI Files, Settings

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2003-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#ifndef XMLSET_SETTINGS_H
#define XMLSET_SETTINGS_H

/****************************************************************************
 ***
 *** Includes
 ***
 ****************************************************************************/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/Error/XMLError_Error.h"

/****************************************************************************
 ***
 *** Defines
 ***
 ****************************************************************************/

#ifdef WIN32
#define XMLSET_DIRPATH_SEP  '\\'
#else
#define XMLSET_DIRPATH_SEP	'/'
#endif

#define XMLSET_MAX_VALUE_LEN            1023
#define XMLSET_MAX_SERVICE_NAME_LEN     255
#define XMLSET_MAX_FILENAME_LEN         1024

#define XMLSET_REG_SECTION_DEFAULT      "SAPDBXIE"

/****************************************************************************
 ***
 *** Structs & Types
 ***
 ****************************************************************************/

typedef struct st_xmlset_settings   *XMLSet_Settings;

typedef char                         XMLSet_ServiceNameString[XMLSET_MAX_SERVICE_NAME_LEN+1];
typedef char                        *XMLSet_ServiceName;

/****************************************************************************
 ***
 *** Prototypes
 ***
 ****************************************************************************/

SAPDB_UInt2 Settings_GetMaxThreads( XMLSet_Settings hSettings );

char* Settings_GetServiceName( XMLSet_Settings hSettings );
char* Settings_GetLogFilename( XMLSet_Settings hSettings );

char* Settings_GetRegistryNode( XMLSet_Settings hSettings );
char* Settings_GetRegistryName( XMLSet_Settings hSettings );
char* Settings_GetRegistryUser( XMLSet_Settings hSettings );
char* Settings_GetRegistryPassword( XMLSet_Settings hSettings );
char* Settings_GetRegistryTraceFilename( XMLSet_Settings hSettings );
char* Settings_GetEngineNode( XMLSet_Settings hSettings );
char* Settings_GetDescription( XMLSet_Settings hSettings );
char* Settings_GetLogFilename( XMLSet_Settings hSettings );
char* Settings_GetIndexingServiceName( XMLSet_Settings hSettings );
char* Settings_GetIndexingLibFile( XMLSet_Settings hSettings );
char* Settings_GetIndexingLibVersion( XMLSet_Settings hSettings );
char* Settings_GetIndexingLibDescription( XMLSet_Settings hSettings );

SAPDB_Bool Settings_Create( XMLSet_Settings    *hNewSettings,
                            char               *exeDir,
                            char               *iniFile,
                            char               *sectionName,
                            char               *usedIniFile,
                            XMLError_Error      hError );

SAPDB_Bool Settings_Destroy( XMLSet_Settings   hSettings );

SAPDB_Bool Set_UpdateRegistration( char             *exeDir,
                                   char             *iniFile,
                                   char             *maxThreads,
                                   char             *logFilename,
                                   char             *registrationName,
                                   char             *registrationDescription,
                                   char             *registryDBNode,
                                   char             *registryDBName,
                                   char             *registryDBUser,
                                   char             *registryDBPassword,
                                   char             *registryDBTraceFilename,
                                   char             *indexingServiceName,
                                   char             *indexingLibraryName,
                                   char             *indexingLibraryVersion,
                                   char             *indexingLibraryDescription,
                                   XMLError_Error    hError );

/****************************************************************************
 ***
 *** End
 ***
 ****************************************************************************/

#endif
