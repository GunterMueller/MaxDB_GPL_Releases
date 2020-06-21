/*!**********************************************************************

  module: XMLSet_Settings.c

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

/****************************************************************************
 ***
 *** Includes
 ***
 ****************************************************************************/

#include "SAPDB/XML/Settings/XMLSet_Settings.h"
#include "SAPDB/XML/Settings/XMLSet_ErrorCodes.h"

#include "SAPDB/XML/SessionPool/XMLSP_SessionPool.h"
#include "SAPDB/XML/Memory/XMLMem_Memory.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/Registry/Reg_Registry.h"

/*---------------------------------------------------------------------------*/

/****************************************************************************
 ***
 *** Defines
 ***
 ****************************************************************************/

#ifdef REG_WIN32
    #define XMLSET_REGISTRY                 "SOFTWARE\\SAP\\SAP DBTech\\XIE"
#else
    #define XMLSET_REGISTRY                 "/usr/spool/sql/ini/xie.ini"
#endif

#define XMLSET_REG_KEY_MAX_THREADS                  "MaxThreads"
#define XMLSET_REG_KEY_ENGINE_NODE                  "EngineNode"
#define XMLSET_REG_KEY_LOG_FILENAME                 "LogFilename"
#define XMLSET_REG_KEY_DESCRIPTION                  "Description"
#define XMLSET_REG_KEY_SERVICE_DB_NAME              "RegistryName"
#define XMLSET_REG_KEY_SERVICE_DB_NODE              "RegistryNode"
#define XMLSET_REG_KEY_SERVICE_DB_USER              "RegistryUser"
#define XMLSET_REG_KEY_SERVICE_DB_PASSWORD          "RegistryPassword"
#define XMLSET_REG_KEY_SERVICE_DB_TRACE_FILENAME    "RegistryTraceFilename"
#define XMLSET_REG_KEY_INDEXING_SERVICE_NAME        "IndexingServiceName"
#define XMLSET_REG_KEY_INDEXING_LIB_FILE            "IndexingLibraryFile"
#define XMLSET_REG_KEY_INDEXING_LIB_VERSION         "IndexingLibraryVersion"
#define XMLSET_REG_KEY_INDEXING_LIB_DESCRIPTION     "IndexingLibraryDescription"

/*---------------------------------------------------------------------------*/

/****************************************************************************
 ***
 *** Structs & Types
 ***
 ****************************************************************************/

struct st_xmlset_settings {
    Reg_RegistryP               registry;

    SAPDB_UInt2                 maxThreads;
    XMLError_LogFilenameString  logFilename;
    char                        engineNode[XMLSET_MAX_VALUE_LEN+1];
    char                        description[XMLSET_MAX_VALUE_LEN+1];
    XMLSP_DBServerNameString    registryName;
    XMLSP_DBServerNodeString    registryNode;
    XMLSP_DBUserString          registryUser;
    XMLSP_DBPasswordString      registryPassword;
    XMLSP_DBTraceFilenameString registryTraceFilename;
    XMLSet_ServiceNameString    indexingServiceName;
    XMLLib_LibraryNameString    indexingLibFile;
    char                        indexingLibVersion[XMLSET_MAX_VALUE_LEN+1];
    char                        indexingLibDescription[XMLSET_MAX_VALUE_LEN+1];
};

/****************************************************************************
 ***
 *** Prototypes
 ***
 ****************************************************************************/

SAPDB_Bool Settings_Init( XMLSet_Settings    hSettings,
                          char              *exeDir,
                          char              *iniFile,
                          char              *sectionName,
                          char              *usedIniFile,
                          XMLError_Error     hError )
;

SAPDB_Bool Settings_ReadConf( char            *exePath,
                              char            *confPath,
                              SAPDB_UInt2      confPathSize,
                              XMLError_Error   hError );

/****************************************************************************
 ***
 *** Variables
 ***
 ****************************************************************************/

/****************************************************************************
 ***
 *** Public Functions
 ***
 ****************************************************************************/


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
                                   XMLError_Error    hError )
{

    char registryPath[XMLSET_MAX_FILENAME_LEN+1];

    /* Is ini path given? */
    if (!iniFile) {
        /*                                                          */
        /* Get ini file to use from sapdbxie.conf                   */
        /*                                                          */
        /* On Windows systems ini file means registry path,         */
        /* whereby the main key HKEY_LOCAL_MACHINE is predefined    */
        /*                                                          */
        /* On Unix systems ini file means ini file,                 */
        /* with its whole path.                                     */
        /*                                                          */
        if (!Settings_ReadConf(exeDir, registryPath, XMLSET_MAX_FILENAME_LEN, hError)) {
            return SAPDB_FALSE;
        }
    } else {
        strcpy(registryPath, iniFile);
    }

    /* If ini file is not described by sapdbxie.conf use default ini file */
    if (strlen(registryPath) < 1) {
        strcpy(registryPath, XMLSET_REGISTRY);
    }

    /* Set registration description value */
    if (!Reg_SetRegistryKey (registryPath, registrationName, XMLSET_REG_KEY_DESCRIPTION, registrationDescription)) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_UPDATE_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_UPDATE_REGISTRY);

        return SAPDB_FALSE;
    }

    /* Set maxThreads value */
    if (!Reg_SetRegistryKey (registryPath, registrationName, XMLSET_REG_KEY_MAX_THREADS, maxThreads)) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_UPDATE_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_UPDATE_REGISTRY);

        return SAPDB_FALSE;
    }

    /* Set log filename value */
    if (!Reg_SetRegistryKey (registryPath, registrationName, XMLSET_REG_KEY_LOG_FILENAME, logFilename)) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_UPDATE_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_UPDATE_REGISTRY);

        return SAPDB_FALSE;
    }

    /* Set service DB name value */
    if (!Reg_SetRegistryKey (registryPath, registrationName, XMLSET_REG_KEY_SERVICE_DB_NAME, registryDBName)) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_UPDATE_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_UPDATE_REGISTRY);

        return SAPDB_FALSE;
    }

    /* Set service DB node value */
    if (!Reg_SetRegistryKey (registryPath, registrationName, XMLSET_REG_KEY_SERVICE_DB_NODE, registryDBNode)) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_UPDATE_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_UPDATE_REGISTRY);

        return SAPDB_FALSE;
    }

    /* Set service DB user value */
    if (!Reg_SetRegistryKey (registryPath, registrationName, XMLSET_REG_KEY_SERVICE_DB_USER, registryDBUser)) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_UPDATE_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_UPDATE_REGISTRY);

        return SAPDB_FALSE;
    }

    /* Set service DB password value */
    if (!Reg_SetRegistryKey (registryPath, registrationName, XMLSET_REG_KEY_SERVICE_DB_PASSWORD, registryDBPassword)) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_UPDATE_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_UPDATE_REGISTRY);

        return SAPDB_FALSE;
    }

    /* Set service DB trace filename value */
    if (!Reg_SetRegistryKey (registryPath, registrationName, XMLSET_REG_KEY_SERVICE_DB_TRACE_FILENAME, registryDBTraceFilename)) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_UPDATE_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_UPDATE_REGISTRY);

        return SAPDB_FALSE;
    }

    /* Set indexing service name */
    if (!Reg_SetRegistryKey (registryPath, registrationName, XMLSET_REG_KEY_INDEXING_SERVICE_NAME, indexingServiceName)) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_UPDATE_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_UPDATE_REGISTRY);

        return SAPDB_FALSE;
    }

    /* Set indexing library name */
    if (!Reg_SetRegistryKey (registryPath, registrationName, XMLSET_REG_KEY_INDEXING_LIB_FILE, indexingLibraryName)) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_UPDATE_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_UPDATE_REGISTRY);

        return SAPDB_FALSE;
    }

    /* Set indexing library version */
    if (!Reg_SetRegistryKey (registryPath, registrationName, XMLSET_REG_KEY_INDEXING_LIB_VERSION, indexingLibraryVersion)) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_UPDATE_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_UPDATE_REGISTRY);

        return SAPDB_FALSE;
    }

    /* Set indexing library description */
    if (!Reg_SetRegistryKey (registryPath, registrationName, XMLSET_REG_KEY_INDEXING_LIB_DESCRIPTION, indexingLibraryDescription)) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_UPDATE_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_UPDATE_REGISTRY);

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;
}

/*===========================================================================*/

SAPDB_UInt2 Settings_GetMaxThreads( XMLSet_Settings hSettings )
{

    if (!hSettings) {
        return 0;
    }

    return hSettings->maxThreads;

}

/*===========================================================================*/

char* Settings_GetLogFilename( XMLSet_Settings hSettings )
{

    if (!hSettings) {
        return NULL;
    }

    return hSettings->logFilename;

}

/*===========================================================================*/

char* Settings_GetEngineNode( XMLSet_Settings hSettings )
{

    if (!hSettings) {
        return NULL;
    }

    return hSettings->engineNode;

}

/*===========================================================================*/

char* Settings_GetDescription( XMLSet_Settings hSettings )
{

    if (!hSettings) {
        return NULL;
    }

    return hSettings->description;

}

/*===========================================================================*/

char* Settings_GetRegistryName( XMLSet_Settings hSettings )
{

    if (!hSettings) {
        return NULL;
    }

    return hSettings->registryName;

}

/*===========================================================================*/

char* Settings_GetRegistryNode( XMLSet_Settings hSettings )
{

    if (!hSettings) {
        return NULL;
    }

    return hSettings->registryNode;

}

/*===========================================================================*/

char* Settings_GetRegistryUser( XMLSet_Settings hSettings )
{

    if (!hSettings) {
        return NULL;
    }

    return hSettings->registryUser;

}

/*===========================================================================*/

char* Settings_GetRegistryPassword( XMLSet_Settings hSettings )
{

    if (!hSettings) {
        return NULL;
    }

    return hSettings->registryPassword;

}

/*===========================================================================*/

char* Settings_GetRegistryTraceFilename( XMLSet_Settings hSettings )
{

    if (!hSettings) {
        return NULL;
    }

    return hSettings->registryTraceFilename;

}

/*===========================================================================*/

char* Settings_GetIndexingServiceName( XMLSet_Settings hSettings )
{

    if (!hSettings) {
        return NULL;
    }

    return hSettings->indexingServiceName;

}

/*===========================================================================*/

char* Settings_GetIndexingLibFile( XMLSet_Settings hSettings )
{

    if (!hSettings) {
        return NULL;
    }

    return hSettings->indexingLibFile;

}

/*===========================================================================*/

char* Settings_GetIndexingLibVersion( XMLSet_Settings hSettings )
{

    if (!hSettings) {
        return NULL;
    }

    return hSettings->indexingLibVersion;

}

/*===========================================================================*/

char* Settings_GetIndexingLibDescription( XMLSet_Settings hSettings )
{

    if (!hSettings) {
        return NULL;
    }

    return hSettings->indexingLibDescription;

}

/*===========================================================================*/

SAPDB_Bool Settings_Create( XMLSet_Settings    *hNewSettings,
                            char               *exeDir,
                            char               *iniFile,
                            char               *sectionName,
                            char               *usedIniFile,
                            XMLError_Error      hError )
{

    SAPDB_Bool ok;

    /* Construct new doc class */
	XMLMem_Allocat(sizeof(struct st_xmlset_settings), (SAPDB_UInt1**) hNewSettings, &ok );
	if (!ok) {
        *hNewSettings = NULL;

        return SAPDB_FALSE;
	}

	/* Init handle */
    if (!Settings_Init(*hNewSettings, exeDir, iniFile, sectionName, usedIniFile, hError)) {
        XMLMem_Free(*hNewSettings);
        *hNewSettings = NULL;

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Settings_Destroy( XMLSet_Settings   hSettings )
{

    if (hSettings->registry) {
        Reg_CloseRegistry(hSettings->registry);
    }

    /* Destroy settings itself */
    XMLMem_Free((SAPDB_UInt1*) hSettings);

    return SAPDB_TRUE;

}

/****************************************************************************
 ***
 *** Private Functions
 ***
 ****************************************************************************/

SAPDB_Bool Settings_Init( XMLSet_Settings    hSettings,
                          char              *exeDir,
                          char              *iniFile,
                          char              *sectionName,
                          char              *usedIniFile,
                          XMLError_Error     hError )
{

    char            valueBuf[REG_MAX_VALUE_LEN+1];
    char            usedSectionName[REG_MAX_SECTION_LEN+1];

    /* Is ini path given? */
    if (!iniFile) {
        /*                                                          */
        /* Get ini file to use from sapdbxie.conf                   */
        /*                                                          */
        /* On Windows systems ini file means registry path,         */
        /* whereby the main key HKEY_LOCAL_MACHINE is predefined    */
        /*                                                          */
        /* On Unix systems ini file means ini file,                 */
        /* with its whole path.                                     */
        /*                                                          */
        if (!Settings_ReadConf(exeDir, usedIniFile, XMLSET_MAX_FILENAME_LEN, hError)) {
            return SAPDB_FALSE;
        }
    } else {
        strcpy(usedIniFile, iniFile);
    }

    /* If ini file is not described by sapdbxie.conf use default ini file */
    if (strlen(usedIniFile) < 1) {
        strcpy(usedIniFile, XMLSET_REGISTRY);
    }

    /* If section is not given use default section */
#ifdef WIN32
    if (!sectionName) {
        sp77sprintf(usedSectionName, REG_MAX_SECTION_LEN, "%s\\%s", usedIniFile, XMLSET_REG_SECTION_DEFAULT);
    } else {
        sp77sprintf(usedSectionName, REG_MAX_SECTION_LEN, "%s\\%s", usedIniFile, sectionName);
    }
#else
    if (!sectionName) {
        strcpy(usedSectionName, XMLSET_REG_SECTION_DEFAULT);
    } else {
        strcpy(usedSectionName, sectionName);
    }
#endif

    /* Init settings */
    hSettings->registry = NULL;
	hSettings->maxThreads = 1;
    hSettings->logFilename[0] = '\0';
	hSettings->indexingServiceName[0] = '\0';

    /* Open registry */
    if ( !Reg_OpenRegistry( &hSettings->registry, usedIniFile )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_OPEN_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_OPEN_REGISTRY);

        return SAPDB_FALSE;
    }

    /* Get maxThreads value */
    if ( !Reg_GetRegistryKey( hSettings->registry, usedSectionName, XMLSET_REG_KEY_MAX_THREADS, valueBuf, REG_MAX_VALUE_LEN )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY);
        Reg_CloseRegistry( hSettings->registry );
        hSettings->registry = NULL;

        return SAPDB_FALSE;
    } else {
        if ((hSettings->maxThreads = atoi(valueBuf)) < 1) {
            hSettings->maxThreads = 1;
        }
    }

    /* Get log filename value */
    if ( !Reg_GetRegistryKey( hSettings->registry, usedSectionName, XMLSET_REG_KEY_LOG_FILENAME, valueBuf, REG_MAX_VALUE_LEN )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY);
        Reg_CloseRegistry( hSettings->registry );
        hSettings->registry = NULL;

        return SAPDB_FALSE;
    } else {
        Com_StrMaxCopy(hSettings->logFilename, valueBuf, XMLERROR_MAX_LOG_FILENAME_LEN);
    }

    /* Get engine node */
    if ( !Reg_GetRegistryKey( hSettings->registry, usedSectionName, XMLSET_REG_KEY_ENGINE_NODE, valueBuf, REG_MAX_VALUE_LEN )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY);
        Reg_CloseRegistry( hSettings->registry );
        hSettings->registry = NULL;

        return SAPDB_FALSE;
    } else {
        Com_StrMaxCopy(hSettings->engineNode, valueBuf, XMLSET_MAX_SERVICE_NAME_LEN);
    }

    /* Get description */
    if ( !Reg_GetRegistryKey( hSettings->registry, usedSectionName, XMLSET_REG_KEY_DESCRIPTION, valueBuf, REG_MAX_VALUE_LEN )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY);
        Reg_CloseRegistry( hSettings->registry );
        hSettings->registry = NULL;

        return SAPDB_FALSE;
    } else {
        Com_StrMaxCopy(hSettings->description, valueBuf, XMLSET_MAX_SERVICE_NAME_LEN);
    }

    /* Get service DB name value */
    if ( !Reg_GetRegistryKey( hSettings->registry, usedSectionName, XMLSET_REG_KEY_SERVICE_DB_NAME, valueBuf, REG_MAX_VALUE_LEN )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY);
        Reg_CloseRegistry( hSettings->registry );
        hSettings->registry = NULL;

        return SAPDB_FALSE;
    } else {
        Com_StrMaxCopy(hSettings->registryName, valueBuf, XMLSP_MAX_DB_SERVER_NAME_LEN);
    }

    /* Get service DB node value */
    if ( !Reg_GetRegistryKey( hSettings->registry, usedSectionName, XMLSET_REG_KEY_SERVICE_DB_NODE, valueBuf, REG_MAX_VALUE_LEN )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY);
        Reg_CloseRegistry( hSettings->registry );
        hSettings->registry = NULL;

        return SAPDB_FALSE;
    } else {
        Com_StrMaxCopy(hSettings->registryNode, valueBuf, XMLSP_MAX_DB_SERVER_NODE_LEN);
    }

    /* Get service DB user value */
    if ( !Reg_GetRegistryKey( hSettings->registry, usedSectionName, XMLSET_REG_KEY_SERVICE_DB_USER, valueBuf, REG_MAX_VALUE_LEN )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY);
        Reg_CloseRegistry( hSettings->registry );
        hSettings->registry = NULL;

        return SAPDB_FALSE;
    } else {
        Com_StrMaxCopy(hSettings->registryUser, valueBuf, XMLSP_MAX_DB_USER_LEN);
    }

    /* Get service DB password value */
    if ( !Reg_GetRegistryKey( hSettings->registry, usedSectionName, XMLSET_REG_KEY_SERVICE_DB_PASSWORD, valueBuf, REG_MAX_VALUE_LEN )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY);
        Reg_CloseRegistry( hSettings->registry );
        hSettings->registry = NULL;

        return SAPDB_FALSE;
    } else {
        Com_StrMaxCopy(hSettings->registryPassword, valueBuf, XMLSP_MAX_DB_PASSWORD_LEN);
    }

    /* Get service DB trace filename value */
    if ( !Reg_GetRegistryKey( hSettings->registry, usedSectionName, XMLSET_REG_KEY_SERVICE_DB_TRACE_FILENAME, valueBuf, REG_MAX_VALUE_LEN )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY);
        Reg_CloseRegistry( hSettings->registry );
        hSettings->registry = NULL;

        return SAPDB_FALSE;
    } else {
        Com_StrMaxCopy(hSettings->registryTraceFilename, valueBuf, XMLSP_MAX_DB_TRACE_FILENAME_LEN);
    }

    /* Get indexing service name */
    if ( !Reg_GetRegistryKey( hSettings->registry, usedSectionName, XMLSET_REG_KEY_INDEXING_SERVICE_NAME, valueBuf, REG_MAX_VALUE_LEN )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY);
        Reg_CloseRegistry( hSettings->registry );
        hSettings->registry = NULL;

        return SAPDB_FALSE;
    } else {
        Com_StrMaxCopy(hSettings->indexingServiceName, valueBuf, XMLSET_MAX_VALUE_LEN);
    }

    /* Get indexing library name */
    if ( !Reg_GetRegistryKey( hSettings->registry, usedSectionName, XMLSET_REG_KEY_INDEXING_LIB_FILE, valueBuf, REG_MAX_VALUE_LEN )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY);
        Reg_CloseRegistry( hSettings->registry );
        hSettings->registry = NULL;

        return SAPDB_FALSE;
    } else {
        Com_StrMaxCopy(hSettings->indexingLibFile, valueBuf, XMLLIB_MAX_LIBRARY_NAME_LEN);
    }

    /* Get indexing library version */
    if ( !Reg_GetRegistryKey( hSettings->registry, usedSectionName, XMLSET_REG_KEY_INDEXING_LIB_VERSION, valueBuf, REG_MAX_VALUE_LEN )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY);
        Reg_CloseRegistry( hSettings->registry );
        hSettings->registry = NULL;

        return SAPDB_FALSE;
    } else {
        Com_StrMaxCopy(hSettings->indexingLibVersion, valueBuf, XMLSET_MAX_VALUE_LEN);
    }

    /* Get indexing library description */
    if ( !Reg_GetRegistryKey( hSettings->registry, usedSectionName, XMLSET_REG_KEY_INDEXING_LIB_DESCRIPTION, valueBuf, REG_MAX_VALUE_LEN )) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_COULD_NOT_READ_REGISTRY,
                                        XMLSET_ERR_TEXT_COULD_NOT_READ_REGISTRY);
        Reg_CloseRegistry( hSettings->registry );
        hSettings->registry = NULL;

        return SAPDB_FALSE;
    } else {
        Com_StrMaxCopy(hSettings->indexingLibDescription, valueBuf, XMLSET_MAX_VALUE_LEN);
    }

    /* Close registry */
    Reg_CloseRegistry( hSettings->registry );
    hSettings->registry = NULL;

    return SAPDB_TRUE;
}

/*===========================================================================*/

SAPDB_Bool Settings_ReadConf( char            *exePath,
                              char            *confPath,
                              SAPDB_UInt2      confPathSize,
                              XMLError_Error   hError )
{

    static const BUF_SIZE = 4095;

    SAPDB_Int4           file = -1;
    tsp05_RteFileError   fError;
    SAPDB_Long           readLen = 0;
    char                 fileName[XMLSET_MAX_FILENAME_LEN+1] = "";
    char                *slash = NULL;

    /* Checks   */
    if ( !exePath || !confPath ) {
        return SAPDB_FALSE;
    }

    /* Build filename   */
    slash = exePath+strlen(exePath)-1;
    if (*slash == XMLSET_DIRPATH_SEP) {
        sp77sprintf( fileName, XMLSET_MAX_FILENAME_LEN, "%ssapdbxie.conf", exePath );
    } else {
        sp77sprintf( fileName, XMLSET_MAX_FILENAME_LEN, "%s%csapdbxie.conf", exePath, XMLSET_DIRPATH_SEP );
    }

    /* Open file for reading in binary mode */
    sqlfopenc( fileName, sp5vf_text, sp5vf_read, sp5bk_buffered, &file, &fError );
    if ( fError.sp5fe_result != vf_ok ) {
        Error_Set(XMLERROR_POS, hError, XMLSET_ERR_CODE_CANNOT_OPEN_CONF_FILE,
                                        XMLSET_ERR_TEXT_CANNOT_OPEN_CONF_FILE);

        return SAPDB_FALSE;
    }

    /* Read file into buffer    */
    sqlfreadc ( file, confPath, confPathSize, &readLen, &fError );
    confPath[confPathSize-1] = '\0';

    /* Close file   */
    sqlfclosec( file, sp5vf_close_normal, &fError );

    return SAPDB_TRUE;

}

/*===========================================================================*/

/****************************************************************************
 ***
 *** End
 ***
 ****************************************************************************/
