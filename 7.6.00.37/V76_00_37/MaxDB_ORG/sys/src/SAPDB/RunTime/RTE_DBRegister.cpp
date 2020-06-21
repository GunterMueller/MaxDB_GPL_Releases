/*!---------------------------------------------------------------------
  @file           RTE_DBRegister.cpp
  @author         JoergM, RobinW
  @brief          DBM: SAPDB Instance and Database Registration and Management Interfaces
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

#ifdef _WIN32
#  include "gos00k.h" /* nocheck */
#  include "gos00.h"  /* nocheck */
extern "C"
{
#  include "heo05.h"
#  include "gos94.h"  /* nocheck */
}
#else
#  include "gen00.h"  /* nocheck */
#endif

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Types.hpp"
#include "RunTime/RTE_GetDBRootOfDB.h"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Messages.hpp"
#include "RunTime/RTE_SpeedInfo.hpp"
#include "RunTime/RTE_Database.hpp"
#include "RunTime/RTE_ActiveDatabase.hpp"
#include "RunTime/RTE_Installation.hpp"
#include "RunTime/RTE_ActiveInstallation.hpp"
#include "RunTime/RTE_DBRegister.hpp"
#include "SAPDBCommon/Messages/SDBMsg_RTEDBState.h"
#include "RunTime/RTE_IniFileHandling.h"
#include "SAPDBCommon/SAPDB_Names.h"
#include "heo01.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifndef _WIN32

#define SERVER_TYPE_SERVERDB             0 

#endif

#define maxVersionStringC	100



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

RTE_DBRegister *RTE_DBRegister::m_Instance=NULL;

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/
RTE_DBRegister &RTE_DBRegister::Instance()
{
    if(0 == RTE_DBRegister::m_Instance)
        RTE_DBRegister::m_Instance = new RTE_DBRegister();
    return *RTE_DBRegister::m_Instance;
}

//-----------------------------------------------------------

RTE_Database * RTE_DBRegister::GetDatabase( const RTE_DBName dbName,
                                            SAPDBErr_MessageList &errList)
{
    RTE_DBName name;
    strcpy(name,dbName);
    RTE_Installation * installation = GetInstallationByDBName(name,errList);
    if( NULL == installation )
    {
        errList = Msg_List(
	        Msg_List::Error,
	        SDBMSG_RTEDBSTATE_DATABASE_NOT_FOUND,
	        Msg_Arg(SDBMSGTAG_RTEDBSTATE_DATABASE_NOT_FOUND__DATABASE_NAME,name)
        );
        return NULL;
    }
    RTE_Database * database = new RTE_Database(*installation,installation->GetDBRoot(),name);
    delete installation;
    return database;
}

//-----------------------------------------------------------

RTE_ActiveDatabase * RTE_DBRegister::GetActiveDatabase( const RTE_DBName dbName,
                                                        SAPDBErr_MessageList &errList)
{
    RTE_DBName name;
    strcpy(name,dbName);
    RTE_ActiveInstallation * activeInstallation = GetActiveInstallationByDBName(name,errList);
    if( NULL == activeInstallation )
    {
        errList = Msg_List(
	        Msg_List::Error,
	        SDBMSG_RTEDBSTATE_DATABASE_NOT_FOUND,
	        Msg_Arg(SDBMSGTAG_RTEDBSTATE_DATABASE_NOT_FOUND__DATABASE_NAME,name)
        );
        return NULL;
    }
    RTE_ActiveDatabase * activeDatabase = new RTE_ActiveDatabase(*activeInstallation,activeInstallation->GetDBRoot(),name);
    delete activeInstallation;
    return activeDatabase;
}

//-----------------------------------------------------------

RTE_ActiveInstallation * RTE_DBRegister::GetActiveInstallationByDBName( const RTE_DBName dbName,
                                                                        SAPDBErr_MessageList &errList)
{
    RTE_Installation::InstVersionInfo version;
    RTE_Path dbRoot;

    if(!RTE_GetDBRootOfDB ((SAPDB_Char *)dbName, (SAPDB_Char *)&dbRoot,sizeof(RTE_Path)))
        return NULL;

    RTE_ActiveInstallation * activeInstallation = 0;
    SAPDB_Bool inRegistry;
    if ( getInstVersionFromRegistry(dbRoot,version,SERVER_TYPE_SERVERDB,inRegistry,errList) )
    {
        activeInstallation = new RTE_ActiveInstallation(dbRoot,version,inRegistry);
    }
    return activeInstallation;
}

//-----------------------------------------------------------

RTE_ActiveInstallation * RTE_DBRegister::GetActiveInstallationByDBRoot(const RTE_Path   dbRoot,
                                                                       SAPDBErr_MessageList &errList)
{
    RTE_Installation::InstVersionInfo version;
    RTE_Path copyOfdbRoot;
    strncpy(copyOfdbRoot,dbRoot,sizeof(RTE_Path));
    
    RTE_ActiveInstallation * activeInstallation = 0;
    SAPDB_Bool inRegistry;
    if ( getInstVersionFromRegistry(copyOfdbRoot,version,SERVER_TYPE_SERVERDB,inRegistry,errList) )
    {
        activeInstallation = new RTE_ActiveInstallation(copyOfdbRoot,version,inRegistry);
    }
    return activeInstallation;
}

RTE_Installation * RTE_DBRegister::GetInstallationByDBRoot(const RTE_Path   dbRoot,
                                                             SAPDBErr_MessageList &errList)
{
    RTE_Installation::InstVersionInfo version;
    RTE_Path copyOfdbRoot;
    strncpy(copyOfdbRoot,dbRoot,sizeof(RTE_Path));
    RTE_Installation * installation = 0;
    SAPDB_Bool inRegistry;
    if ( getInstVersionFromRegistry(copyOfdbRoot,version,SERVER_TYPE_SERVERDB,inRegistry,errList) )
    {
        installation = new RTE_Installation(copyOfdbRoot,version,inRegistry);
    }
    return installation;
}

//-----------------------------------------------------------

RTE_Installation * RTE_DBRegister::GetInstallationByDBName( const RTE_DBName dbName,
                                                              SAPDBErr_MessageList &errList)
{
    RTE_Installation::InstVersionInfo version;
    RTE_Path dbRoot;

    if(!RTE_GetDBRootOfDB ((SAPDB_Char *)dbName, (SAPDB_Char *)&dbRoot,sizeof(RTE_Path)))
        return NULL;

    RTE_Installation * installation = 0;
    SAPDB_Bool inRegistry;
    if ( getInstVersionFromRegistry(dbRoot,version,SERVER_TYPE_SERVERDB,inRegistry,errList) )
    {
        installation = new RTE_Installation(dbRoot,version,inRegistry);
    }
    return installation;
}

//-----------------------------------------------------------

void RTE_DBRegister::parseVersion (const char * versionString,RTE_Installation::InstVersionInfo & versionInfo)
{
    SAPDB_Int release,major,minor,build;
    SAPDB_Char FormatString[100] ;
    memset (versionInfo.key, '\0', sizeof (RTE_DBKey));
    SAPDB_sprintf(FormatString, 100,"%%d.%%d.%%d.%%d,%%%ds", sizeof(RTE_DBKey) ) ;

    sscanf (versionString, FormatString,
	    &release, &major, &minor, &build, versionInfo.key);

    versionInfo.release = (SAPDB_Byte) release;
    versionInfo.major   = (SAPDB_Byte) major;
    versionInfo.minor   = (SAPDB_Byte) minor;
    versionInfo.build   = (SAPDB_Byte) build;
}

//-----------------------------------------------------------

#ifdef _WIN32


/*------------------------------*/

SAPDB_Bool RTE_DBRegister::openInstRegistry (
        HKEY &keyHandle)
{
    const char *applicationKey;
    PATHNAME    szKey;
    APIRET      rc;
    
    applicationKey = REG_SK_APPLIC;

    strcpy (szKey, REG_SK_BASE );
    strcat (szKey, "\\" );
    strcat (szKey, applicationKey );
    strcat (szKey, "\\" );
    strcat (szKey, "Installations");
    rc = RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKey, 0,
                       KEY_ENUMERATE_SUB_KEYS, &keyHandle);
    return (NO_ERROR == rc);
}

/*------------------------------*/
void RTE_DBRegister::replaceCharsInString (SAPDB_Char* string, SAPDB_Char fromChar, SAPDB_Char toChar)
{
    while (*string != '\0') 
    {
        if(*string == fromChar && *(string+1) == fromChar)
        {
            SAPDB_Char *j=string;
            while(*j)
            {
                *j = *(j+1);
                j++;
            }
        }

        if (*string == fromChar) 
        {
            *string = toChar;
        }
        ++string;
    }
}

#define KEY_PREFIX_ "Installations\\"

SAPDB_Bool RTE_DBRegister::getInstVersionFromRegistry(
    RTE_Path instroot,
    RTE_Installation::InstVersionInfo &instversion,
    SAPDB_Int1 dbKind,
    SAPDB_Bool &inRegistry,
    SAPDBErr_MessageList &err)
{
    inRegistry = false;
    PATHNAME      szKey;
    strcpy (szKey, KEY_PREFIX_);
    strcat (szKey, instroot);
    replaceCharsInString (szKey + strlen(KEY_PREFIX_), '\\', '/');

    tsp9_release_version releaseversion;
    REG_ENTRY_REC RegistryEntries[2];
    RegistryEntries[0].pszValueName = REG_VN_VERSION_ID;
    RegistryEntries[0].pValue       = &releaseversion;
    RegistryEntries[0].ulValueSize  = sizeof(tsp9_release_version);
    RegistryEntries[0].ulValueType  = REG_BINARY;
    
    instversion.key[0] = 0;
    RegistryEntries[1].pszValueName = REG_VN_KEY_ID;
    RegistryEntries[1].pValue       = &instversion.key;
    RegistryEntries[1].ulValueSize  = sizeof(instversion.key);     
    RegistryEntries[1].ulValueType  = REG_SZ;

    APIRET            rc     = NO_ERROR;
    if (dbKind == SERVER_TYPE_SERVERDB) 
    {
        rc = sql50_reg_get_applic_values (NULL, HKEY_LOCAL_MACHINE, szKey,
            2, RegistryEntries);
        /* This retry allows to read even old registrys, not yet containing keys... */
        if ( rc != NO_ERROR )
        {
            rc = sql50_reg_get_applic_values (NULL, HKEY_LOCAL_MACHINE, szKey,
                    1, RegistryEntries);
        }
    }
    else 
    {
        rc = sql50_reg_get_oldapplic_values (NULL, HKEY_LOCAL_MACHINE, szKey,
            2, RegistryEntries);
    }
    if (rc != NO_ERROR) 
    {
        int ok;

        tsp00_ErrTextc  errtext;
        RTE_IniFileResult rc;

        char versionString [maxVersionStringC+2+sizeof(tsp00_Pathc)];
        ok = RTE_GetConfigString (SAPDB_INSTALLATIONS_INI_FILE, SAPDB_INSTALLATIONS_SECTION,
            instroot, versionString, sizeof (versionString),
            errtext, rc);
        if (ok) 
        {
            parseVersion (versionString, instversion);
            return true;
        }
        else
        {
            err = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_COULD_NOT_READ_FROM_REGISTRY,errtext.asCharp ());
            return false;
        }
            
        err = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_COULD_NOT_READ_RELEASE_VERSION);
        return false;
    }
    else
    {
        inRegistry = true;
        instversion.release = releaseversion.no[0];
        instversion.major   = releaseversion.no[1];
        instversion.minor   = releaseversion.no[2];
        instversion.build   = releaseversion.no[3];
        return true;
    }
}

//-----------------------------------------------------------

Container_List<RTE_Installation>* RTE_DBRegister::GetInstallations()    
{
    Container_List<RTE_Installation> *pliste = new Container_List<RTE_Installation>(RTEMem_Allocator::Instance());

    APIRET      rc     = NO_ERROR;

    HKEY        keyHandle;
    SAPDB_UInt4 index = 0;

    if(openInstRegistry (keyHandle))
    {
        do
        {
            RTE_Path          instroot;

            DWORD             outlen = sizeof (instroot);
            FILETIME          dummyTime;
            rc = RegEnumKeyEx (keyHandle, index,
                               (LPTSTR)&instroot, &outlen,
                               NULL, NULL, NULL, &dummyTime);
            if(NO_ERROR == rc)
            {
                ++index;
                RTE_Installation::InstVersionInfo instversion;
                SAPDBErr_MessageList err;
                SAPDB_Bool inRegistry;
                if ( getInstVersionFromRegistry(instroot,instversion,SERVER_TYPE_SERVERDB,inRegistry,err) )
                {
                    replaceCharsInString (instroot, '/', '\\');

                    RTE_Installation  *pInst;
                    pInst = new RTE_Installation(instroot,instversion,inRegistry);
                    pliste->InsertFront(*pInst);
                    delete pInst;
                }
            }
        }
        while(NO_ERROR == rc);
        RegCloseKey( keyHandle );
    }
    index = 0;

// open enum
    tsp00_Pathc u3Root;
    tsp01_RteError  rteError; 
    if(sqlGetPortableInstallationRootPath ( u3Root,
                                            TERM_WITH_DELIMITER_EO01,
                                            &rteError ))
    {
        RTE_IniFileResult rc;
        tsp00_ErrTextc errtext;
        RTE_RegistryHandle handle;
        RTE_RegistryLocation location;

        handle = RTE_OpenConfigEnum (SAPDB_INSTALLATIONS_INI_FILE, SAPDB_INSTALLATIONS_SECTION, errtext, rc);
        if (SAPDB_INIFILE_RESULT_OK == rc)
        {
            int ok;
            // iterate trough installations
            do
            {
                char        versionString [maxVersionStringC+2+sizeof(tsp00_Pathc)]; /* PTS 1104801 */
                RTE_Path    instroot;
// this is necessary for serv.exe but contraproductive for dbmsrv?
#ifdef INST_ENUM_GIVES_ABS_PATH
                SAPDB_UInt u3Len = strlen(pU3Root);
                SAPDB_sprintf(&instroot[0],sizeof(instroot),"%s%s",pU3Root,'\\' == pU3Root[u3Len-1] ? "" : "\\");
#else
                instroot[0] = 0;
#endif
                ok = RTE_NextConfigEnum (handle, &instroot[0]+strlen(instroot), sizeof(instroot)-strlen(instroot)-1, versionString, sizeof(versionString), location, errtext, rc);
                if (ok) 
                {
                    RTE_Installation::InstVersionInfo   instversion;
                    parseVersion (versionString, instversion);
                    RTE_Installation  *pInst;
                    pInst = new RTE_Installation(instroot,instversion,false);
                    pliste->InsertEnd(*pInst);

                } // end if

            } // end while
            while(ok);
            RTE_CloseConfigEnum (handle, errtext, rc);

        } // end if
    }

    return pliste;
}

//-----------------------------------------------------------

bool RTE_DBRegister::RegisterInstallation( const RTE_Path dbRoot, 
                                           const RTE_Installation::InstVersionInfo &version,
                                           SAPDBErr_MessageList &errList)
{
    tsp00_Pathc u3Root;
    tsp01_RteError  rteError; 
    if(!sqlGetPortableInstallationRootPath( u3Root,
                                        TERM_WITH_DELIMITER_EO01,
                                        &rteError ))
    {
        APIRET      rc  = NO_ERROR;
        tsp00_Pathc     szKey;
        REG_ENTRY_REC   RegistryEntries[2];
        char               *stringMark;

        tsp9_release_version releaseversion;
        releaseversion.no[0] = version.release ;
        releaseversion.no[1] = version.major   ;
        releaseversion.no[2] = version.minor   ;
        releaseversion.no[3] = version.build   ;

        /* build entry */
        strcpy (szKey, KEY_PREFIX_);
        stringMark = szKey + strlen (szKey);
        strcpy (stringMark, dbRoot);
        /* registry keys cannot contain backslashes */
        replaceCharsInString (stringMark, '\\', '/');
        RegistryEntries[0].pszValueName = REG_VN_VERSION_ID;
        RegistryEntries[0].pValue       = &releaseversion;
        RegistryEntries[0].ulValueSize  = sizeof(tsp9_release_version);
        RegistryEntries[0].ulValueType  = REG_BINARY;

        RegistryEntries[1].pszValueName = REG_VN_KEY_ID;
        RegistryEntries[1].pValue       = (void *)(version.key);
        RegistryEntries[1].ulValueSize  = (ULONG)strlen(version.key)+1;
        RegistryEntries[1].ulValueType  = REG_SZ;

        rc = sql50_reg_put_applic_values (NULL, NULL, HKEY_LOCAL_MACHINE, szKey,
                                        NULL, NULL, 2, RegistryEntries);
        if(rc != NO_ERROR)
        {
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_COULD_NOT_WRITE_TO_REGISTRY,"");
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        int ok;
        tsp00_ErrTextc errtext;
        RTE_IniFileResult rc;
        char versionString [maxVersionStringC+2+sizeof(tsp00_Pathc)]; /* PTS 1104801 */
        SAPDB_sprintf (versionString, sizeof(versionString),"%d.%d.%d.%d,%s",
            version.release, version.major, version.minor, version.build,
            version.key );

#ifndef _WIN32
        int  umask_old ;
        umask_old = umask(027); /* if the installations file is created, the write access is limited to owner */
#endif
        ok = RTE_PutConfigString (SAPDB_INSTALLATIONS_INI_FILE, SAPDB_INSTALLATIONS_SECTION,
            dbRoot, versionString, errtext, rc);
#ifndef _WIN32
        umask(umask_old);
#endif
        if(!ok)
        {
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_COULD_NOT_WRITE_TO_REGISTRY,errtext.asCharp ());
            return false;
        }
        else
        {
            return true;
        }
    }
}

//-----------------------------------------------------------

bool RTE_DBRegister::UnregisterInstallation( const RTE_Path dbRoot,
                                             SAPDBErr_MessageList &errList)
{
    APIRET      rc  = NO_ERROR;
    PATHNAME    szKey;
    char        *stringMark;

    strcpy (szKey, KEY_PREFIX_);
    stringMark = szKey + strlen (szKey);
    strcpy (stringMark, dbRoot);
    /* registry keys cannot contain backslashes */
    replaceCharsInString (stringMark, '\\', '/');
    rc =  sql50_reg_del_applic_key (NULL, HKEY_LOCAL_MACHINE, szKey);

    if( rc != NO_ERROR )// TODO: replace by FILE NOT FOUND
    {
        int ok;
        tsp00_ErrTextc errtext;
        RTE_IniFileResult rc;

        ok = RTE_RemoveConfigString (SAPDB_INSTALLATIONS_INI_FILE, SAPDB_INSTALLATIONS_SECTION,
            dbRoot, errtext, rc);

        if (!ok) 
        {
            errList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_GENERAL,errtext.asCharp ());
            return false;    
        }
    }
    return (rc != NO_ERROR) ;
}

#else   // UNIX

#include "gsp00.h"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

extern "C"
{
#include "heo44.h"
#include "heo05.h"
#include "gen41.h"  /* nocheck */ // for sql41_get_ipc_dir()
}
#define IERR_TYPE   0
#define ERR_TYPE    1
#define WRN_TYPE    2
#define INFO_TYPE   3



//-----------------------------------------------------------

Container_List<RTE_Installation>* RTE_DBRegister::GetInstallations()    
{
    Container_List<RTE_Installation> *pliste = new Container_List<RTE_Installation>(RTEMem_Allocator::Instance());
// open enum
    RTE_IniFileResult rc;
    tsp00_ErrTextc errtext;
    RTE_RegistryHandle handle;
    RTE_RegistryLocation location;

    handle = RTE_OpenConfigEnum (SAPDB_INSTALLATIONS_INI_FILE, SAPDB_INSTALLATIONS_SECTION, errtext, rc);
    if (SAPDB_INIFILE_RESULT_OK == rc)
    {
        int ok;
        // iterate trough installations
        do
        {
            char        versionString [maxVersionStringC+2+sizeof(tsp00_Pathc)]; /* PTS 1104801 */
            RTE_Path    instroot;
            ok = RTE_NextConfigEnum (handle, instroot, sizeof(instroot), versionString, sizeof(versionString), location, errtext, rc);
            if (ok) 
            {
                RTE_Installation::InstVersionInfo   instversion;
                parseVersion (versionString, instversion);
                RTE_Installation  *pInst;
                pInst = new RTE_Installation(instroot,instversion);
                pliste->InsertEnd(*pInst);

            } // end if

        } // end while
        while(ok);
        RTE_CloseConfigEnum (handle, errtext, rc);

    } // end if
    return pliste;
}

//-----------------------------------------------------------

bool RTE_DBRegister::RegisterInstallation( const RTE_Path dbRoot, 
                                           const RTE_Installation::InstVersionInfo &version,
                                           SAPDBErr_MessageList &errList)
{
    int ok;
    tsp00_ErrTextc errtext;
    RTE_IniFileResult rc;
    char versionString [maxVersionStringC+2+sizeof(tsp00_Pathc)]; /* PTS 1104801 */
    SAPDB_sprintf (versionString, sizeof(versionString),"%d.%d.%d.%d,%s",
        version.release, version.major, version.minor, version.build,
        version.key );

    int  umask_old ;

    umask_old = umask(027); /* if the installations file is created, the write access is limited to owner */
    ok = RTE_PutConfigString (SAPDB_INSTALLATIONS_INI_FILE, SAPDB_INSTALLATIONS_SECTION,
           dbRoot, versionString, errtext, rc);
    umask(umask_old);

    if(!ok)
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_COULD_NOT_WRITE_TO_REGISTRY,errtext.asCharp ());
        return false;
    }
    else
    {
        return true;
    }
}

//-----------------------------------------------------------

bool RTE_DBRegister::UnregisterInstallation( const RTE_Path dbRoot,
                                             SAPDBErr_MessageList &errList)
{
    int ok;
    tsp00_ErrTextc errtext;
    RTE_IniFileResult rc;

    ok = RTE_RemoveConfigString (SAPDB_INSTALLATIONS_INI_FILE, SAPDB_INSTALLATIONS_SECTION,
        dbRoot, errtext, rc);

    if (!ok) 
    {
        errList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_GENERAL,errtext.asCharp ());
        return false;    
    }
    return true;
}

//-----------------------------------------------------------

SAPDB_Bool RTE_DBRegister::getInstVersionFromRegistry(
    RTE_Path instroot,
    RTE_Installation::InstVersionInfo &instversion,
    SAPDB_Int1 dbKind,
    SAPDB_Bool &inRegistry,
    SAPDBErr_MessageList &err)
{
    int ok;
    RTE_DBKey key;
    tsp00_ErrTextc  errtext;
    RTE_IniFileResult rc;

    char versionString [maxVersionStringC+2+sizeof(tsp00_Pathc)];
    ok = RTE_GetConfigString (SAPDB_INSTALLATIONS_INI_FILE, SAPDB_INSTALLATIONS_SECTION,
        instroot, versionString, sizeof (versionString),
        errtext, rc);
    if (ok) 
    {
        parseVersion (versionString, instversion);
        return true;
    }
    else
    {
        err = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_COULD_NOT_READ_FROM_REGISTRY,errtext.asCharp ());
        return false;
    }
        
}

//-----------------------------------------------------------

#endif // UNIX

/*===========================================================================*
 *  INTERFACE TO OLD CODE                                                    *
 *===========================================================================*/

#include "gsp09.h"
#include "heo44.h"

struct InstallationListAndIterator
{
    Container_List<RTE_Installation>            *list;
    Container_List<RTE_Installation>::Iterator  *iterator;
    InstallationListAndIterator()
    {
    };
};

externC void sqlxopen_installation_enum (
    void** handle,
    tsp9_rte_xerror* xerror)
{
    InstallationListAndIterator *listAndIterator = new InstallationListAndIterator();
    listAndIterator->list = ((RTE_DBRegister::Instance()).GetInstallations());
    listAndIterator->iterator = new Container_List<RTE_Installation>::Iterator;
    *(listAndIterator->iterator) = listAndIterator->list->Begin();
    *handle = listAndIterator;

    xerror->xe_result = CtrlCmdOk_csp09;
}

externC void sqlxnext_installation (
    void* handle,
    tsp9_rte_installationinfo* releaseinfo,
    tsp9_rte_xerror* xerror)
{
    if(*(((InstallationListAndIterator *)handle)->iterator) == 0 )
    {
        eo44eoshowError (xerror);
        return;
    }
    Container_List<RTE_Installation>::Iterator currentElement = *(((InstallationListAndIterator *)handle)->iterator);

    eo44initError (xerror);
    strncpy(releaseinfo->dbroot,currentElement->GetDBRoot(),sizeof(releaseinfo->dbroot));
    releaseinfo->version.no[0] = currentElement->GetVersion().release;
    releaseinfo->version.no[1] = currentElement->GetVersion().major;
    releaseinfo->version.no[2] = currentElement->GetVersion().minor;
    releaseinfo->version.no[3] = currentElement->GetVersion().build;
    ++(*((InstallationListAndIterator *)handle)->iterator);
}

externC void sqlxnext_installationNew (
    void* handle,
    tsp09_RteInstallationInfoNew VAR_VALUE_REF info,
    tsp09_rte_xerror* xerror)
{
    if ( (info.RecordVersion != INSTALLATION_RECORD_VERSION_V721)
      || (info.RecordLength != sizeof(tsp09_RteInstallationInfoNew)) )
    {
        eo44anyError (xerror, "Unsupported record version");
        return;
    }
    if(*(((InstallationListAndIterator *)handle)->iterator) == 0 )
    {
        eo44eoshowError (xerror);
        return;
    }

    Container_List<RTE_Installation>::Iterator currentElement = *(((InstallationListAndIterator *)handle)->iterator);

    eo44initError (xerror);
    strncpy(info.dbroot,currentElement->GetDBRoot(),sizeof(info.dbroot));
    strncpy(info.key,currentElement->GetVersion().key,sizeof(info.key));
    info.version.no[0] = currentElement->GetVersion().release;
    info.version.no[1] = currentElement->GetVersion().major;
    info.version.no[2] = currentElement->GetVersion().minor;
    info.version.no[3] = currentElement->GetVersion().build;
    ++(*((InstallationListAndIterator *)handle)->iterator);
}    
    
externC void sqlxclose_installation_enum (
    void* handle)
{
    delete ((InstallationListAndIterator *)handle)->iterator;
    delete ((InstallationListAndIterator *)handle)->list;
    delete ((InstallationListAndIterator *)handle);
}

externC void sqlxregisterinstNew ( tsp09_RteInstallationInfoNew VAR_VALUE_REF info,
                                   tsp9_rte_xerror*              xerror)
{
    eo44initError (xerror);

    if ( (info.RecordVersion != INSTALLATION_RECORD_VERSION_V721)
      || (info.RecordLength != sizeof(tsp09_RteInstallationInfoNew)) )
    {
        eo44anyError (xerror, "Unsupported record version");
        return;
    }

    RTE_Installation::InstVersionInfo version;
    SAPDBErr_MessageList errList;

    strncpy(version.key , info.key, sizeof(RTE_DBKey));
    version.release = info.version.no[0];
    version.major   = info.version.no[1];
    version.minor   = info.version.no[2];
	version.build   = info.version.no[3];

    bool ok = (RTE_DBRegister::Instance()).RegisterInstallation( info.dbroot, version, errList);
    if(!ok)
        RTE_FillXErrorFromErrList(xerror, errList);    
}

externC void sqlxunregisterinst (
    tsp9_cstr dbroot,
    tsp9_rte_xerror* xerror)
{
    eo44initError (xerror);

    SAPDBErr_MessageList errList;
    (RTE_DBRegister::Instance()).UnregisterInstallation( dbroot, errList);
    RTE_FillXErrorFromErrList(xerror, errList);    
}

struct DatabaseListAndIterator
{
    Container_List<RTE_Installation>                    *installationList;
    Container_List<RTE_Installation>::Iterator          *installationIterator;
    Container_List<RTE_Database *>                      *databaseList;
    Container_List<RTE_Database *>::Iterator            *databaseIterator;
    Container_List<RTE_SpeedInfo>                       *speedList;
    Container_List<RTE_SpeedInfo>::Iterator             *speedIterator;
    DatabaseListAndIterator()
    {
    };
};

// routines for conversion of tsp9_rte_xerror to SAPDBErr_MessageList and vice versa
// both structures "know" errors, warnings and infos, so mapping is quite straight-forward.
// Unfortunately, tsp9_rte_xerror does not contain a field for the message type. Only 
// for error messages xe_result is csp9_xrte_notok, for warnings and errors xe_result is csp9_xrte_ok.
// Warnings and Infos can be distinguished by looking at xe_text...


void RTE_FillXErrorFromErrList(tsp9_rte_xerror* xerror,SAPDBErr_MessageList &errList)
{
    switch(errList.Type())
    {
    case SAPDBErr_MessageList::Info:
        MSG_ERR ((xerror, 11000, INFO_TYPE, (char *)"",
             (char *)errList.Message()));
        break;
    case SAPDBErr_MessageList::Warning:
        MSG_ERR ((xerror, 11000, WRN_TYPE, (char *)"",
             (char *)errList.Message()));
        break;
    case SAPDBErr_MessageList::Error:
        MSG_ERR ((xerror, 11000, ERR_TYPE, (char *)"",
             (char *)errList.Message()));
        break;
    default:
        MSG_ERR ((xerror, 11000, ERR_TYPE, (char *)"",
             (char *)"internal error"));
        break;
    }
}

void RTE_FillErrListFromXError(SAPDBErr_MessageList &errList,tsp9_rte_xerror* xerror)
{
    if(csp9_xrte_notok == xerror->xe_result)    // xe_rsult is csp9_xrte_notok only for error messages, csp9_xrte_ok for warnings and infos
    {
        errList = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_GENERAL,xerror->xe_text);
    }
    else
    {
        if(strstr(xerror->xe_text,"INFO"))
        {
            errList = SAPDBErr_MessageList(RTE_CONTEXT,RTEINFO_GENERAL,xerror->xe_text);
        }
        else
        {
            errList = SAPDBErr_MessageList(RTE_CONTEXT,RTEWARN_GENERAL,xerror->xe_text);
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

