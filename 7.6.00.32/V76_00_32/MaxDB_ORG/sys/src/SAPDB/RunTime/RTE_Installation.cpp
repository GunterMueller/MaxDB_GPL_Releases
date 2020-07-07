/*!---------------------------------------------------------------------
  @file           RTE_Installation.cpp
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

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Types.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Messages.hpp"
#include "RunTime/RTE_DBRegister.hpp"
#include "RunTime/RTE_Installation.hpp"
#include "RunTime/RTE_Database.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "SAPDBCommon/SAPDB_Names.h"
#include "RunTime/RTE_IniFileHandling.h"                    /* nocheck */

#ifndef _WIN32

#include "heo01.h"                                          /* nocheck */
#include "gen41.h"                                          /* nocheck */ // for en41Get*FromFile()
#include "RunTime/RTE_PidListForExecutable.hpp"             /* nocheck */
#else
#include "gos00.h"                                          /* nocheck */
#include "gos41.h"                                          /* nocheck */
#include "RunTime/RTE_NTService.hpp"                        /* nocheck */
#include "RunTime/RTE_GetDBRootOfDB.h"                      /* nocheck */
#endif

#ifdef RTEInternal
using namespace RTEInternal;
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/



#ifdef _WIN32
//-----------------------------------------------------------

Container_List<RTE_Database>* RTE_Installation::GetDatabases(SAPDBErr_MessageList &errList)
{
    Container_List<RTE_Database> *liste = new Container_List<RTE_Database>(RTEMem_Allocator::Instance());

    SAPDB_Int     active;
    APIRET        rc     = NO_ERROR;
    SQL_DBNAMEC   szServerDB;
    SAPDB_Char   *pszDBRoot;

    SAPDB_Int     serverType;
    if(m_Version.release < 7)
    {
        serverType = SERVER_TYPE_ADABAS_SERVERDB;
    }
    else
    {
        serverType = SERVER_TYPE_SERVERDB;
    }
    
    if(m_inRegistry)
    {
        sql97_update(NULL);
        rc = sql97_first_db_gw_state ( serverType, szServerDB, &active );
        RTE_Database  *currentDataBase = NULL;
        while((ERROR_NO_MORE_FILES != rc) && (ERROR_PATH_NOT_FOUND != rc))
        {
            pszDBRoot = RTE_getDBRootFromRegistry ( NULL, SERVER_TYPE_SERVERDB, szServerDB );
            if( NULL != pszDBRoot && 0 == _stricmp(pszDBRoot,m_dbRoot))
            {
                SAPDB_Char *modeName = strrchr(szServerDB,'(');

                RTE_SpeedInfo speedInfo = speedNone;

                if(NULL != modeName)
                {
                    modeName--;
                    *(modeName) = 0;
                    modeName += 2;
                    SAPDB_Char *modeEnd = strchr(modeName , ')' );
                    if(0 != modeEnd)
                    {
                        *modeEnd = 0;
                    }
                }
                else
                {
                    modeName = "fast";
                }

                Container_List<RTE_SpeedInfo>::Iterator speedIterator; 
                speedIterator = m_speedList.Begin();
                
                while(speedIterator != 0)
                {
                    if (strcmp (modeName, (char *)speedIterator->Name()) == 0) 
                    {
                        speedInfo = *speedIterator;
                        break;
                    }
                    ++speedIterator;
                }

                if(!(speedInfo == speedNone))
                {
                    if((NULL == currentDataBase) || (0 != strcmp(currentDataBase->GetDBName(),szServerDB)))
                    {
                        if(NULL != currentDataBase)
                        {
                            liste->InsertFront(*currentDataBase);
                            delete currentDataBase;
                        }
                        currentDataBase = new RTE_Database(*this,pszDBRoot,szServerDB);
                        currentDataBase->m_speedList.Delete();  // we don't like the pre-filled speed list here as it may contain speeds that are not registered for this special database...
                    }
                    currentDataBase->m_speedList.InsertEnd(speedInfo);
                    if(active)
                    {
                        currentDataBase->SetActiveSpeed(speedInfo);
                    }
                }
            }
            rc = sql97_next_db_gw_state (szServerDB, &active);
        }
    /* finally, don't forget the last database found... The previous ones have been inserted every time a new one was created*/
        if(NULL != currentDataBase)
        {
            liste->InsertFront(*currentDataBase);
            delete currentDataBase;
        }
    }
    else
    {
        RTE_RegistryHandle registry;
        tsp00_ErrTextc errtext;
        RTE_IniFileResult rc;

        registry = RTE_OpenConfigEnum (SAPDB_DATABASES_INI_FILE, SAPDB_DATABASES_SECTION, errtext, rc);
        if (rc != SAPDB_INIFILE_RESULT_OK) 
        {
            return liste;
        }
#ifndef _WIN32
        SAPDB_Bool buildProcessList;
        if ( m_Version.release > 7
        || ( m_Version.release == 7
            && m_Version.major >= 3 ))
            buildProcessList = false;
        else
            buildProcessList = true;

        if (buildProcessList) 
        {
            /* build process list ( needed only to support releases < 7.4 on UNIX) */
            char		cmdBuf [255];
            // TODO: try to avoid calling an external program!
            SAPDB_sprintf (cmdBuf, sizeof(cmdBuf),"ps_all > /tmp/db.%d 2>/dev/null", getpid ());
            system (cmdBuf);
        }
#endif
        RTE_RegistryLocation location;
#ifndef _WIN32
        char real_mdbroot[PATH_MAX];
        if(0 != realpath(m_dbRoot,real_mdbroot))    // this means that m_dbRoot is not okay. Forget it.
#endif
        {
#ifndef _WIN32
            RTE_Path path;

            SAPDB_sprintf(path,sizeof(RTE_Path),"%s/pgm/%s",m_dbRoot,speedFast.ExecutableName());
            new(&m_pidListFast)RTE_PidListForExecutable(path);

            SAPDB_sprintf(path,sizeof(RTE_Path),"%s/pgm/%s",m_dbRoot,speedQuick.ExecutableName());
            new(&m_pidListQuick)RTE_PidListForExecutable(path);

            SAPDB_sprintf(path,sizeof(RTE_Path),"%s/pgm/%s",m_dbRoot,speedSlow.ExecutableName());
            new(&m_pidListSlow)RTE_PidListForExecutable(path);

            SAPDB_sprintf(path,sizeof(RTE_Path),"%s/pgm/%s",m_dbRoot,speedTest.ExecutableName());
            new(&m_pidListTest)RTE_PidListForExecutable(path);
#endif
            int ok;
            RTE_Database  *currentDataBase = NULL;
            do
            {
                RTE_DBName    dbname;
                RTE_Path      dbroot;

                SAPDB_Char *pPosToFill = &dbroot[0];

#ifdef INST_ENUM_GIVES_ABS_PATH
                SAPDB_Char *pU3Root = getenv("SDB_U3_ROOT");
//                if(!pU3Root)    // TODO REMOVE!!
//                    pU3Root = "C:";

                if(pU3Root)
                {
                    SAPDB_UInt u3Len = strlen(pU3Root);
                    SAPDB_sprintf(&dbroot[0],sizeof(dbroot),"%s%s",pU3Root,'\\' == pU3Root[u3Len-1] ? "" : "\\");
                    pPosToFill += strlen(pPosToFill);
                }
#endif
                tsp00_ErrTextc errtext;
                RTE_IniFileResult rc;

                ok = RTE_NextConfigEnum (registry, dbname, sizeof(dbname), pPosToFill, sizeof(dbroot)-(pPosToFill-&dbroot[0]), location, errtext, rc);
                if (ok)
                {
#ifdef _WIN32
                    if ( 0 == strcmp(dbroot,m_dbRoot))  
#else
                    char real_dbroot[PATH_MAX];
                    realpath(dbroot,real_dbroot);
                    if ( 0 == strcmp(real_dbroot,real_mdbroot))
#endif
                    {
                        currentDataBase = new RTE_Database(*this,dbroot,dbname);
                        currentDataBase->GetDBSpeeds().Initialize(m_speedList);

                        Container_List<RTE_SpeedInfo>::Iterator speedIterator; 
                        speedIterator = (currentDataBase->GetDBSpeeds()).Begin();

                        while(speedIterator != 0)
                        {
                            SAPDB_Bool running = false;                 
#ifndef _WIN32
                            if(buildProcessList)
                            {
                                running =  dbHasSpecificProcess (dbname, (*speedIterator).ExecutableName()) ;
                            }
                            else
#endif
                            {
                                running = dbHasRunningProcess (dbname,*speedIterator);
                            }
                            if(running)
                            {
                                currentDataBase->SetActiveSpeed(*speedIterator);
                                break;
                            }
                            ++speedIterator;
                        }
                        liste->InsertFront(*currentDataBase);
                    }
                }
            }
            while(ok);
        }
        RTE_CloseConfigEnum (registry, errtext, rc);
	    registry = 0;


    }
    return liste;
}


#else   // UNIX

/*! define */
#define RTE_DBREGISTRATION_MAX_SPEED_NAME_LENGTH   24

/*----------------------------------------*/

SAPDB_Bool RTE_Installation::dbHasSpecificProcess (
    SAPDB_Char *dbname,
    const SAPDB_Char *kernelpgm)
{
    SAPDB_Char   source [30];

    SAPDB_sprintf (source,30, "cat /tmp/db.%d", getpid() );

    return en41FindDatabaseProcess(source, dbname, kernelpgm);
}

Container_List<RTE_Database>* RTE_Installation::GetDatabases(SAPDBErr_MessageList &errList)
{
    Container_List<RTE_Database> *liste = new Container_List<RTE_Database>(RTEMem_Allocator::Instance());

    RTE_RegistryHandle registry;
    tsp00_ErrTextc errtext;
    RTE_IniFileResult rc;

    registry = RTE_OpenConfigEnum (SAPDB_DATABASES_INI_FILE, SAPDB_DATABASES_SECTION, errtext, rc);
    if (rc != SAPDB_INIFILE_RESULT_OK) 
    {
        return liste;
    }

    SAPDB_Bool buildProcessList;
    if ( m_Version.release > 7
      || ( m_Version.release == 7
        && m_Version.major >= 3 ))
        buildProcessList = false;
    else
        buildProcessList = true;

    if (buildProcessList) 
    {
        /* build process list ( needed only to support releases < 7.4 ) */
        char		cmdBuf [255];
        // TODO: try to avoid calling an external program!
        SAPDB_sprintf (cmdBuf, sizeof(cmdBuf),"ps_all > /tmp/db.%d 2>/dev/null", getpid ());
        system (cmdBuf);
    }

    RTE_RegistryLocation location;
    char real_mdbroot[PATH_MAX];
    if(0 != realpath(m_dbRoot,real_mdbroot))    // this means that m_dbRoot is not okay. Forget it.
    {
        RTE_Path path;

        SAPDB_sprintf(path,sizeof(RTE_Path),"%s/pgm/%s",m_dbRoot,speedFast.ExecutableName());
        new(&m_pidListFast)RTE_PidListForExecutable(path);

        SAPDB_sprintf(path,sizeof(RTE_Path),"%s/pgm/%s",m_dbRoot,speedQuick.ExecutableName());
        new(&m_pidListQuick)RTE_PidListForExecutable(path);

        SAPDB_sprintf(path,sizeof(RTE_Path),"%s/pgm/%s",m_dbRoot,speedSlow.ExecutableName());
        new(&m_pidListSlow)RTE_PidListForExecutable(path);

        SAPDB_sprintf(path,sizeof(RTE_Path),"%s/pgm/%s",m_dbRoot,speedTest.ExecutableName());
        new(&m_pidListTest)RTE_PidListForExecutable(path);

        int ok;
        RTE_Database  *currentDataBase = NULL;
        do
        {
            RTE_DBName    dbname;
            RTE_Path      dbroot;
            tsp00_ErrTextc errtext;
            RTE_IniFileResult rc;

            ok = RTE_NextConfigEnum (registry, dbname, sizeof(dbname), dbroot, sizeof(dbroot), location, errtext, rc);
            if (ok)
            {
                char real_dbroot[PATH_MAX];
                realpath(dbroot,real_dbroot);

                if ( 0 == strcmp(real_dbroot,real_mdbroot))
                {
                    currentDataBase = new RTE_Database(*this,dbroot,dbname);
                    currentDataBase->GetDBSpeeds().Initialize(m_speedList);

                    Container_List<RTE_SpeedInfo>::Iterator speedIterator; 
                    speedIterator = (currentDataBase->GetDBSpeeds()).Begin();

                    while(speedIterator != 0)
                    {
                        SAPDB_Bool running = false;                 
                        if(buildProcessList)
                        {
                            running =  dbHasSpecificProcess (dbname, (*speedIterator).ExecutableName()) ;
                        }
                        else
                        {
                            running = dbHasRunningProcess (dbname,*speedIterator);
                        }
                        if(running)
                        {
                            currentDataBase->SetActiveSpeed(*speedIterator);
                        }
                        ++speedIterator;
                    }
                    liste->InsertFront(*currentDataBase);
                }
            }
        }
        while(ok);
    }
    RTE_CloseConfigEnum (registry, errtext, rc);
	registry = 0;

    if (buildProcessList) 
    {
        RTE_Path	tmpName;
        SAPDB_sprintf ((char *)tmpName, sizeof(RTE_Path), "/tmp/db.%d", getpid () );
        unlink ((char *)tmpName);
    }
    return liste;
}

//-----------------------------------------------------------

#endif

SAPDB_Bool RTE_Installation::dbHasRunningProcess ( RTE_DBName &dbname,RTE_SpeedInfo speed )
{
#ifdef _WIN32
    SAPDB_Bool retval   = false;
    HEV        hevSem   = 0;

    APIRET rc = sql41c_open_event_sem ( &hevSem, SEM_COORD,
                                 dbname, ERROR_ACCESS_DENIED);

    if ((rc == NO_ERROR) || (rc == ERROR_ACCESS_DENIED))
    {
        retval = true;

        if (rc == NO_ERROR)
            sql41c_close_event_sem ( hevSem, "COORD" );
    }
    return retval;
#else
    SAPDB_UTF8 speedFromFile[RTE_DBREGISTRATION_MAX_SPEED_NAME_LENGTH];

    tsp00_TaskId pid;
    if ( 0 != en41GetPPIDFromFile(dbname, &pid) // does the ppid file exist?
      || pid == 0                               // does the ppid file contain a pid?
      || 0 != kill(pid,0)                       // is the process with that pid alive?
      || 0 != en41GetPIDFromFile(dbname, &pid)  // does the pid file exist?
      || pid == 0                               // does the pid file contain a pid?
      || 0 != kill(pid,0)                       // is the process with that pid alive?
      || ! getPidList(speed).IsPidForExecutableAndArgument(pid, dbname)
      || 0 != en41GetDbSpeedFromFile(dbname, (char *)speedFromFile, RTE_DBREGISTRATION_MAX_SPEED_NAME_LENGTH)   // does the speed file exist?
      || 0 != strcasecmp((const char *)speedFromFile, (const char *)speed.Name() ) 
       )
    {                                           // any of conditions above not true --> not running
        return false;
    }
    else
    {
        return true;
    }
#endif
}

/*!
  database list and iteration
 */
struct InstallationAndDatabaseListAndIterator
{
    Container_List<RTE_Installation>                    *installationList; ///< installation list
    Container_List<RTE_Installation>::Iterator          *installationIterator; ///< installation iterator
    Container_List<RTE_Database>                        *databaseList; ///< database list
    Container_List<RTE_Database>::Iterator              *databaseIterator; ///< database iterator
    Container_List<RTE_SpeedInfo>                       *speedList; ///< speed list
    Container_List<RTE_SpeedInfo>::Iterator             *speedIterator; ///< speed iterator
    /// ctor
    InstallationAndDatabaseListAndIterator()
    {
        installationList = NULL;
        installationIterator = NULL;
        databaseList = NULL;
        databaseIterator = NULL;
        speedList = NULL;
        speedIterator = NULL;
    };
};

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
