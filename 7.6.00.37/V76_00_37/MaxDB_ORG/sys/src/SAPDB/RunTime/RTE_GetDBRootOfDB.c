/*!---------------------------------------------------------------------
  @file           RTE_GetDBRootOfDB.c
  @author         RobinW, RaymondR
  @brief          DBM Support: SAPDB Instance and Database Registration and Management Interfaces
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



\endif
---------------------------------------------------------------------*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"

#ifdef _WIN32      /*&if $OSSPEC = WIN32*/
#include "gos00.h" /* nocheck but_imkdep */
#else              /*&else*/
#include "RunTime/RTE_saveUNIXcalls.h"   /* nocheck but_imkdep */
#include "heo01.h"                       /* nocheck but_imkdep */
#endif             /*&endif*/

#include "RunTime/RTE_IniFileHandling.h"
#include "SAPDBCommon/SAPDB_Names.h"     /* nocheck but_imkdep */

#include "SAPDBCommon/SAPDB_sprintf.h"

#ifdef _WIN32

SAPDB_Char *RTE_getDBRootFromRegistry ( SAPDB_Char  *pszNodeName,
                                        SAPDB_ULong ulServerType,
                                        SAPDB_Char  *pszServerDB )
{
    SAPDB_Char      szService[MX_DBNAME + 80];
    RTE_Path        szSubKey;
    REG_ENTRY_REC   RegistryEntries[1];
    static RTE_Path szDBRoot;
    SAPDB_Long      rc = NO_ERROR;

    switch(ulServerType)
    {
        case SERVER_TYPE_ADABAS_SERVERDB :
            strcpy ( szService, ADABAS_SERVICE_ID);
            break;
        case SERVER_TYPE_ADABAS_GATEWAY :
            strcpy ( szService, ADABAS_SERVICE_ID_GW);
            break;
        case SERVER_TYPE_GATEWAY :
            strcpy ( szService, SERVICE_ID_GW);
            break;
        default:
            strcpy ( szService, SERVICE_ID);
            break;
    }

    strcat ( szService, pszServerDB);
    CharUpperBuff(szService, (DWORD)strlen(szService));
    strcpy ( szSubKey,  szService );
    strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

    RegistryEntries[0].pszValueName = REG_VN_DBROOT;
    RegistryEntries[0].pValue       = szDBRoot;
    RegistryEntries[0].ulValueSize  = sizeof(szDBRoot);
    RegistryEntries[0].ulValueType  = REG_SZ;
    RegistryEntries[0].rc           = NO_ERROR;


    sql50_reg_get_service_values ( pszNodeName, szSubKey, 1, RegistryEntries );

    if ( RegistryEntries[0].rc == NO_ERROR )
    {
        DBGOUT;
        return (SAPDB_Char *)RegistryEntries[0].pValue;
    }
    else
    {
        switch (ulServerType)
        {
        case SERVER_TYPE_SERVERDB:
            // look for db of 6.2 release
            DBGOUT;
            return RTE_getDBRootFromRegistry (pszNodeName, SERVER_TYPE_ADABAS_SERVERDB, pszServerDB);
            break;
        case SERVER_TYPE_GATEWAY:
            // look for gateway of 6.2 release
            DBGOUT;
            return RTE_getDBRootFromRegistry (pszNodeName, SERVER_TYPE_ADABAS_GATEWAY, pszServerDB);
            break;
        default:
            // search exhausted
            break;
        }
    }

    return NULL;
}

/*----------------------------*/

SAPDB_Bool RTE_GetDBRootOfDB( SAPDB_Char *DBName,
                              SAPDB_Char *DBRootOfDB,
                              SAPDB_Int  MaxDBRootLen)
{
    #undef MF__
    #define MF__ MOD__"sqlGetDBRootOfDB"

    SAPDB_Char        firstChar;
    tsp00_Pathc szServerDBTmp;
    SAPDB_Bool  Found = FALSE;    
	SAPDB_Char         *pszRegistryValue = RTE_getDBRootFromRegistry (NULL, SERVER_TYPE_SERVERDB, (PSZ )DBName);

    if (pszRegistryValue == NULL) 
	{
        strcpy ( szServerDBTmp, DBName);
        strcat ( szServerDBTmp, SERVICE_QUICK_EXT );
        pszRegistryValue = RTE_getDBRootFromRegistry (NULL, SERVER_TYPE_SERVERDB, szServerDBTmp);
    }
    if (pszRegistryValue == NULL)
	{
        strcpy ( szServerDBTmp, DBName);
        strcat ( szServerDBTmp, SERVICE_SLOW_EXT );
        pszRegistryValue = RTE_getDBRootFromRegistry (NULL, SERVER_TYPE_SERVERDB, szServerDBTmp);
    }
    if (pszRegistryValue == NULL)
	{
        strcpy ( szServerDBTmp, DBName);
        strcat ( szServerDBTmp, SERVICE_TEST_EXT );
        pszRegistryValue = RTE_getDBRootFromRegistry (NULL, SERVER_TYPE_SERVERDB, szServerDBTmp);
    }
    if (pszRegistryValue != NULL) 
	{
        firstChar = pszRegistryValue [0];
        if ((firstChar != '?' ) && 
          (firstChar != '\0') && 
          (strlen(pszRegistryValue) < (unsigned )MaxDBRootLen) )
        {
            strcpy (DBRootOfDB, pszRegistryValue);
            Found = TRUE;
        }
    }
    else
    {
        SAPDB_Int rc;
        RTE_IniFileResult  ok;
        tsp00_ErrTextc errtext;
/*
        SAPDB_Char *pU3Root = getenv("SDB_U3_ROOT");
        SAPDB_UInt u3RootLength = strlen(pU3Root);
        if(pU3Root)
        {
            SAPDB_sprintf(DBRootOfDB,MaxDBRootLen,"%s%s",pU3Root,'\\' == pU3Root[u3RootLength-1] ? "" : "\\");
        }
*/
        rc = RTE_GetConfigString (  SAPDB_DATABASES_INI_FILE, 
                                    SAPDB_DATABASES_SECTION,
                                    DBName,
                                    DBRootOfDB/* + strlen(DBRootOfDB)*/,
                                    MaxDBRootLen,
                                    errtext,
                                    &ok);


        if ( SAPDB_INIFILE_RESULT_OK == ok )
        {
            Found = TRUE;
        }
    }
    return ( Found ) ;
}

/*---------------------------------------------------------*/

/* Registry keys cannot contain backslashes...*/

SAPDB_Bool winPathMatch(SAPDB_Char const *instroot, SAPDB_Char const *DBRoot)
{
    while ( *instroot )
    {
        if ( toupper(*instroot) == toupper(*DBRoot) )
        {
            ++instroot;
            ++DBRoot;
        }
        else if ( *instroot == '/' && *DBRoot == '\\' )
        {
            ++instroot;
            ++DBRoot;
        }
        else if ( *instroot == '\\' && *DBRoot == '/' )
        {
            ++instroot;
            ++DBRoot;
        }
        else
        {
            return false;
        }
    }
    return ( 0 == *DBRoot );
}

/*---------------------------------------------------------------------*/

SAPDB_Bool RTE_CheckIfInstallationRegistered( SAPDB_Char *DBRoot)
{
    SAPDB_Bool  result = false;
    HKEY        keyHandle;
    const char *applicationKey;
    RTE_Path    szKey;
    APIRET      rc;
    
    applicationKey = REG_SK_APPLIC;

    strcpy (szKey, REG_SK_BASE );
    strcat (szKey, "\\" );
    strcat (szKey, applicationKey );
    strcat (szKey, "\\" );
    strcat (szKey, "Installations");
    rc = RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKey, 0,
                       KEY_ENUMERATE_SUB_KEYS, &keyHandle);
    if ( NO_ERROR == rc)
    {
        SAPDB_UInt4 index = 0;
        do
        {
            RTE_Path          instroot;

            DWORD             outlen = sizeof (instroot);
            FILETIME          dummyTime;
            rc = RegEnumKeyEx (keyHandle, index,
                               (LPTSTR)&instroot, &outlen,
                               NULL, NULL, NULL, &dummyTime);
            if( NO_ERROR == rc )
            {
                ++index;
                if ( winPathMatch(instroot, DBRoot) )
                {
                    result = true;
                    break;
                }
            }
        }
        while(NO_ERROR == rc);

        RegCloseKey(keyHandle);
    }
    
    if(!result)
    {
        RTE_IniFileResult  ok;
        tsp00_ErrTextc errtext;
        RTE_Path           dummyBuffer;

        RTE_GetConfigString ( SAPDB_INSTALLATIONS_INI_FILE, 
                            SAPDB_INSTALLATIONS_SECTION,
                            DBRoot,
                            &dummyBuffer[0],
                            sizeof(RTE_Path),
                            errtext,
                            &ok);

        result = ( SAPDB_INIFILE_RESULT_TRUNCATED == ok
            || SAPDB_INIFILE_RESULT_OK == ok );
    }
    return result;
}

#else /* now the UNIX part */

/*----------------------*/
#define RTE_GET_DB_ROOT_CALL "getdbroot"

static SAPDB_Bool
RTE_GetDatabaseRoot(
    SAPDB_Char const     *dbname,
    RTE_Path              dbRootOfDb,
    SAPDB_Int4            maxRootLen )
{
    RTE_Path     dbroot;
    RTE_SystemRc rc;
    pid_t child;
    pid_t donePid;
    SAPDB_Int exitStatus;
    int pipeClientOutput[2];
    int pipeClientError[2];
    tsp00_Pathc   binPath;
    tsp01_RteError rteError;
    SAPDB_Int4 totalBytes;

    if ( !sqlGetIndependentBinPath( binPath,
                                    TERM_WITH_DELIMITER_EO01,
                                    &rteError ) )
    {
        /* IndepProgPath/bin not found */
        return false;
    }

    if ( (strlen(binPath) 
        + strlen(RTE_GET_DB_ROOT_CALL)) > sizeof(tsp00_Path) )
    {
        /* 'getdbroot' path too long */
        return false;
    }

    strcat(binPath, RTE_GET_DB_ROOT_CALL);

    if ( RTE_save_access(binPath, X_OK) != 0 )
    {
        /* 'getdbroot' not accessible */
        return false;
    }

    if ( 0 != pipe(pipeClientOutput) )
    {
        /* pipe creation failed */
        return false;
    }

    if ( 0 != pipe(pipeClientError) )
    {
        /* pipe creation failed */
        close(pipeClientOutput[0]);
        close(pipeClientOutput[1]);
        return false;
    }

    child = fork();
    if ( child == (pid_t)-1 )
    {
        close(pipeClientOutput[0]);
        close(pipeClientOutput[1]);
        close(pipeClientError[0]);
        close(pipeClientError[1]);
        /* forking client application failed */
        return false;
    }

    if ( child == (pid_t)0 )
    {
        char *cArgv[3];
        extern char **environ;

        close(pipeClientOutput[0]);
        close(pipeClientError[0]);
        dup2(pipeClientOutput[1], 1);
        dup2(pipeClientError[1], 2);
        cArgv[0] = strdup(binPath);
        cArgv[1] = strdup(dbname);
        cArgv[2] = 0;
        RTE_save_execve(cArgv[0], cArgv, environ);
        _exit(89);
    }

    close(pipeClientOutput[1]);
    close(pipeClientError[1]);

    while ( (donePid = RTE_save_waitpid((SAPDB_Int4)child, &exitStatus, 0)) != child )
    {
        if ( donePid == (pid_t)-1
             && errno == EINTR )
        {
            continue;
        }
        else
        {
            exitStatus = -2;
            break;
        }
    }

    if ( exitStatus != 0 )
    {
        close(pipeClientOutput[0]);
        close(pipeClientError[0]);
        /* client application had problem */
        return false;
    }

    /* after command completed, get the output */
    memset(dbroot, 0, sizeof(RTE_Path));

    totalBytes = 0;
    do {
        int bytesGot = read( pipeClientOutput[0],
                             &dbroot[totalBytes], 
                             sizeof(RTE_Path)-totalBytes);
        if ( bytesGot > 0 )
        {
            totalBytes += bytesGot;
        }
        else
        {
            break;
        }
    } while ( totalBytes <= (SAPDB_Int4)sizeof(RTE_Path) );

    close(pipeClientOutput[0]);
    close(pipeClientError[0]);

    /* replace output line terminator with zero byte */
    if ( 0 == totalBytes
      || '\n' != dbroot[totalBytes-1] )
    {
        /* dbroot not found by client application 'getdbroot' */
        return false;
    }
    else
    {
      dbroot[totalBytes-1] = 0;
    }

    if ( strlen(dbroot) >= maxRootLen )
    {
        /* dbroot length exceeds input buffer */
        return false;
    }

    strcpy(dbRootOfDb, dbroot);

    return true;
}

/*---------------------------------------------------------*/


SAPDB_Bool RTE_GetDBRootOfDB( SAPDB_Char *DBName,
                              SAPDB_Char *DBRootOfDB,
                              SAPDB_Int  MaxDBRootLen)
{
    SAPDB_Int rc;
    RTE_IniFileResult  ok;
    tsp00_ErrTextc errtext;

    rc = RTE_GetConfigString ( SAPDB_DATABASES_INI_FILE, 
                               SAPDB_DATABASES_SECTION,
                               DBName,
                               DBRootOfDB,
                               MaxDBRootLen,
                               errtext,
                               &ok);

    if ( SAPDB_INIFILE_RESULT_OK == ok )
    {
        return true;
    }

    return RTE_GetDatabaseRoot( DBName,
                                DBRootOfDB,
                                MaxDBRootLen );
}

/*---------------------------------------------------------*/

SAPDB_Bool RTE_CheckIfInstallationRegistered( SAPDB_Char *DBRoot)
{
    RTE_IniFileResult  ok;
    tsp00_ErrTextc errtext;
    RTE_Path           dummyBuffer;

    RTE_GetConfigString ( SAPDB_INSTALLATIONS_INI_FILE, 
                          SAPDB_INSTALLATIONS_SECTION,
                          DBRoot,
                          &dummyBuffer[0],
                          sizeof(RTE_Path),
                          errtext,
                          &ok);

    return ( SAPDB_INIFILE_RESULT_TRUNCATED == ok
          || SAPDB_INIFILE_RESULT_OK == ok );
}

#endif
