/*!---------------------------------------------------------------------
  @file           RTE_ActiveInstallation.cpp
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
#include "RunTime/RTE_Database.hpp"
#include "RunTime/RTE_ActiveInstallation.hpp"
#include "RunTime/RTE_IniFileHandling.h"
#include "SAPDBCommon/SAPDB_Names.h"

#ifdef _WIN32
#include "gos00.h"  /* nocheck */
#include "RunTime/RTE_NTService.hpp"  /* nocheck */
#else
#include "SAPDBCommon/SAPDB_sprintf.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include "heo01.h"  /* nocheck */
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

RTE_Database * RTE_ActiveInstallation::GetServiceDB
  ( SAPDBErr_MessageList & errList )
{
    RTE_Database * pServiceDB = NULL;

    Container_List<RTE_Database> * pDatabases = GetDatabases(errList);
    if (pDatabases) {
        Container_List<RTE_Database>::Iterator oIterator = pDatabases->Begin();
        while (oIterator != pDatabases->End()) {
            if ( ((*oIterator).GetDBName()[0] == '_') ||  ((*oIterator).GetDBName()[0] == '.') ) {
                pServiceDB =  RTE_DBRegister::Instance().GetDatabase((*oIterator).GetDBName(), errList);
                break;
            } // end if
            ++oIterator;
        } // end while
        delete pDatabases;
     } // end if

    return pServiceDB;
} // end RTE_ActiveInstallation::GetServiceDB

#ifdef _WIN32

/*------------------------------*/


bool RTE_ActiveInstallation::RegisterDatabase( const RTE_DBName dbName, RTE_StartOption startOption,SAPDB_Char *account,SAPDB_Char *password,SAPDBErr_MessageList &errList)
{
/* PTS 1126011 no automatic start on cluster server installations */
#ifdef CLUSTER_SERVER_VERSION
    startOption = RTE_StartOnDemand;
#endif
    SAPDB_Bool registrationSuccessful = false;
    if(m_speedList.IsEmpty())
    {
        errList = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_DBREG_NO_KERNEL_EXE);
    }
    else
    {
        Container_List<RTE_SpeedInfo>::Iterator speedIterator = m_speedList.Begin();
        SAPDBErr_MessageList newMessageList;

        errList.ClearMessageList();
        if(m_inRegistry)
        {
            while(speedIterator != 0)
            {
                    RTE_NTService service(dbName,GetDBRoot(),NULL,NULL,*speedIterator);
                    if(service.UpdateServiceEntry ( startOption, account, password, NULL, NULL, newMessageList))
                    {
                        registrationSuccessful = true;
                        startOption = RTE_StartOnDemand;    // only one kernel may be registered with autoStart == true http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1123902
                    }
                    else
                    {
                        errList = errList + newMessageList; // an operator += is available anywhere but on windows...
                    }
                    ++speedIterator;
                }
        }
        else
        {
            RTE_IniFileResult rc;
            tsp00_ErrTextc  errtext;

            registrationSuccessful = ( 0 != RTE_PutConfigString (  SAPDB_DATABASES_INI_FILE,
                                                            SAPDB_DATABASES_SECTION,
                                                            dbName, 
                                                            GetDBRoot(),
                                                            errtext, 
                                                            rc) );
        }
    }
    return registrationSuccessful;
}

bool RTE_ActiveInstallation::UnregisterDatabase( const RTE_DBName dbName,SAPDBErr_MessageList &errList)
{
    SAPDB_Bool removalSuccessful = false;
    Container_List<RTE_SpeedInfo>::Iterator speedIterator = m_speedList.Begin();
    SAPDBErr_MessageList newMessageList;

    errList.ClearMessageList();
    if(m_inRegistry)
    {
        while(speedIterator != 0)
        {
            RTE_NTService service(dbName,"",NULL,NULL,*speedIterator);
            if(service.RemoveServiceEntry ( newMessageList))
            {
                removalSuccessful = true;
            }
            else
            {
                errList = errList + newMessageList; // an operator += is available anywhere but on windows...
            }
            ++speedIterator;
        }
    }
    else
    {
        RTE_IniFileResult rc;
        tsp00_ErrTextc errtext;
        int ok;
        ok = RTE_RemoveConfigString (SAPDB_DATABASES_INI_FILE, SAPDB_DATABASES_SECTION, dbName, errtext, rc);
        if(0 == ok)
        {
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_COULD_NOT_REMOVE_FROM_REGISTRY,errtext.asCharp ());
            return false;
        }
        else
        {
            return true;
        }
    }
    return removalSuccessful;
}

#else   // UNIX


#define DO_START_SUCCEEDED              0
#define DO_START_CANT_REDIRECT_STDOUT   1
#define DO_START_CANT_REDIRECT_STDERR   2
#define DO_START_CANT_OPEN_DEV_NULL     3
#define DO_START_CANNOT_FORK            4
#define DO_START_DIED_BEFORE_COLD_STATE 6
/* DO_START_KERNEL_EXIT_CODE_OFFSET must be highest number, since exit_code is added */
#define DO_START_KERNEL_EXIT_CODE_OFFSET 7

bool RTE_ActiveInstallation::RegisterDatabase( const RTE_DBName dbName, RTE_StartOption startOption,SAPDB_Char *account,SAPDB_Char *password,SAPDBErr_MessageList &errList)
{
    Container_List<RTE_SpeedInfo>::Iterator speedIterator = m_speedList.Begin();
    SAPDBErr_MessageList newMessageList;
    RTE_Path fullKernelPath;
    struct stat buf;
    SAPDB_Bool atLeastOneExecutablePresent = false;

    // check if at least one executable is present for this database
    // it would not make any sense to register a database that could
    // not be started.... (PTS 1106274)
    tsp00_Pathc     PgmPath;
    tsp01_RteError  RteError;
    if(!sqlGetDbrootPgmPath ( PgmPath,TERM_WITH_DELIMITER_EO01 , &RteError))
    {
		errList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_GET_DBROOT_PGM_PATH,
                  SAPDB_ToString (RteError.OsErrCode), SAPDB_ToString (RteError.RteErrCode));
        return false;
    }
    else
    {
        while(speedIterator != 0)
        {
            SAPDB_sprintf ( fullKernelPath, sizeof(RTE_Path), "%s%s", PgmPath.asCharp(), (*speedIterator).ExecutableName() ) ;

            if(0 == stat(fullKernelPath , &buf))
            {
                atLeastOneExecutablePresent = true;
                break;
            }
            ++speedIterator;
        }

        if(atLeastOneExecutablePresent)
        {
            RTE_IniFileResult rc;
            tsp00_ErrTextc  errtext;
            int ok;
            int  umask_old ;
            umask_old = umask(007); /* if the registeration file is created, the write access is limited to owner and group */
            ok = RTE_PutConfigString ( SAPDB_DATABASES_INI_FILE,
                                       SAPDB_DATABASES_SECTION,
                                       dbName, 
                                       GetDBRoot(),
                                       errtext, 
                                       rc);
            umask(umask_old);
            if(0 == ok)
            {
                errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_COULD_NOT_WRITE_TO_REGISTRY,errtext.asCharp ());
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_NO_KERNEL_EXE );
            return false;
        }
    }
}

bool RTE_ActiveInstallation::UnregisterDatabase( const RTE_DBName /*&*/dbName,SAPDBErr_MessageList &errList)
{
    RTE_IniFileResult rc;
    tsp00_ErrTextc errtext;
    int ok;
    ok = RTE_RemoveConfigString (SAPDB_DATABASES_INI_FILE, SAPDB_DATABASES_SECTION, dbName, errtext, rc);
    if(0 == ok)
    {
        errList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_DBREG_COULD_NOT_REMOVE_FROM_REGISTRY,errtext.asCharp ());
        return false;
    }
    else
    {
        return true;
    }
}

#endif
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
