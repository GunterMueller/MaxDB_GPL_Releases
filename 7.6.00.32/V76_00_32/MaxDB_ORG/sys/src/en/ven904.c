/*!
  @file           ven904.c
  @author         JoergM
  @brief          Client RunTime: Database and Installation  Registry
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
*/

#define MOD__ "ven904.c:"
#include <sys/stat.h>
#include "gen00.h"
#include "gsp01.h"
#include "gsp09.h"
#include "geo001.h"
#include "geo57.h"
#include "gen904.h"
#include "heo05.h"

#include "RunTime/RTE_GetDBRootOfDB.h"
#include "RunTime/RTE_IniFileHandling.h"
#include "SAPDBCommon/SAPDB_Names.h"

#define XOK(xerror)	((xerror)->xe_result == 0)
#define forUpdate_MC	TRUE
#define readOnly_MC	FALSE
#define DB_IPC_INTRO "db:"
#define DB_IPC_INTRO_LEN 3

#define maxVersionStringC	100

#if defined (DEBUG_TRACE) 
extern FILE* traceFileCS;
#define TRACE(x) fprintf x
#else
#define TRACE(x)
#endif

static       void en904_parseVersion ( const char * versionString,
                                       tsp9_release_version * version);

static tsp00_Bool en904_newestDBRoot ( char * dbroot,
                                       const char * serverpgm);

/*----------------------------------------*/

tsp00_Bool en904_VersionOfDBRoot (
    char * dbroot,
    tsp9_release_version* version)
{
#undef MF__
#define MF__ MOD__"en904_VersionOfDBRoot"
    int		ok;
    RTE_IniFileResult rc;
    tsp00_ErrTextc  errtext;
    char versionString [maxVersionStringC+2+sizeof(tsp00_Pathc)]; /* PTS 1104801 */

    ok = RTE_GetConfigString (SAPDB_INSTALLATIONS_INI_FILE, SAPDB_INSTALLATIONS_SECTION,
	    dbroot, versionString, sizeof (versionString),
	    errtext, &rc);
    if (ok)
    {
    	en904_parseVersion (versionString, version);
    }
    return ok;
}

/**\
--------------------------------------- MF__ MOD__ (DB Registry)
\**/
tsp00_Bool
sql904_findControlServer (
    char *executable,		    /* OUT		*/
    const int  MaxExecutableLen,/* IN       */
    const char *dbname,		    /* IN		*/
    const char *serverpgm,      /* IN       */ 
    char *dbroot,		        /* IN/OUT	*/
    const int  MaxDBRootLen,    /* IN       */
    tsp00_ErrTextc errText)     /* IN       */
{
#undef	MF__
#define	MF__	MOD__"sql904_findControlServer"
    tsp00_Bool found = FALSE;
    struct stat statInfo;
    int rc;

    if ( dbname && *dbname )
    {
        /* first precedence: explicit dbname */
        DBG1 (( MF__,"calling RTE_GetDBRootOfDB for %s\n",dbname ));
        found = RTE_GetDBRootOfDB ((char *)dbname, dbroot, MaxDBRootLen);
        if ( found )
        {
            found = RTE_CheckIfInstallationRegistered(dbroot);
            if ( !found )
            {
                strcpy(errText, "Installation for database not found");
            }
        }
        else
        {
            strcpy(errText, "database not found");
        }
    }
    else if ( dbroot && *dbroot )
    {
#ifdef HAS_WORKING_INSTALLATION_DEREGISTRATION
        /* second precedence: explicit dbroot */
        found = RTE_CheckIfInstallationRegistered(dbroot);
        if ( !found )
        {
            strcpy(errText, "Installation for dbroot not found");
        }
#else
        found = true;
#endif
    }
    else 
    {
        /* last chance: get newest installation */
        found = en904_newestDBRoot (dbroot, serverpgm);
        if ( !found )
        {
            strcpy(errText, "no installation found");
        }
    }

    if (found) 
    {
	    strcpy (executable, dbroot);
	    strcat (executable, "/pgm/");
	    strcat (executable, serverpgm);
        DBG1 (( MF__,"RTE_GetDBRootOfDB executable: %s\n",executable ));
        rc = stat (executable, &statInfo);
	    if ((rc != 0)                     || 
            !(statInfo.st_mode & S_IFREG) ||  /* PTS 1105115 */
            !(statInfo.st_mode & S_IEXEC) )
        {
            DBG1 (( MF__,"bad fileinformation (stat) of executable: %s\n",executable ));
            strcpy(errText, "executable program not accessible");
	        found = FALSE;
	    }
    }

    DBG1 (( MF__,"executable: %s %sfound",executable, found ? "" : "NOT "  ));
    return found;
}

/*----------------------------------------*/
/*           STATIC FUNCTIONS             */
/*----------------------------------------*/

/*----------------------------------------*/
    #undef MF__
    #define MF__ MOD__"sqlxnext_installationOld"
static void sqlxnext_installationOld (
    RTE_RegistryHandle handle,
    tsp9_rte_installationinfo* instinfo,
    tsp9_rte_xerror* xerror)
{
    int ok;
    RTE_IniFileResult rc;
    tsp00_ErrTextc errtext;
    RTE_RegistryLocation location;
    char versionString [maxVersionStringC+2+sizeof(tsp00_Pathc)]; /* PTS 1104801 */

    eo44initError (xerror);
    ok = RTE_NextConfigEnum (handle,
                        (char *)instinfo->dbroot,
                        sizeof(instinfo->dbroot),
                        versionString,
                        sizeof(versionString),
                        &location,
                        errtext,
                        &rc);
    if (ok) 
    {
        en904_parseVersion (versionString, &instinfo->version);
    }
    else 
    {
        if (rc == SAPDB_INIFILE_RESULT_EOF) 
        {
	        eo44eoshowError (xerror);
        }
	    else
        {
	        eo44anyError (xerror, errtext);
        }
    }
}
/*----------------------------------------*/

    #undef MF__
    #define MF__ MOD__"versionCompare_MF"
static int versionCompare_MF (
    tsp9_release_version* version1,
    tsp9_release_version* version2)
{
    int i;

    for (i = 0; i < csp9_version_digits; ++i) 
    {
	    if (version1->no [i] < version2->no [i]) 
        {
	        return -1;
	    }
	    else if (version1->no [i] > version2->no [i]) 
        {
	        return 1;
	    }
    }
    return 0;
}

/*----------------------------------------*/
/*
 * search through all installations and pick the newest.
 * This search does not need to search through old /usr/spool/sql/SAP_DBTech.ini
 * since it must have been part of a 'new' installation and only older Installations
 * are found in the old file...
 */
static tsp00_Bool en904_newestDBRoot (
    char * dbroot,
    const char * serverpgm)
{
#undef MF__
#define MF__ MOD__"en904_newestDBRoot"
    tsp9_rte_installationinfo instnfo;
    tsp9_rte_xerror	xerror;
    tsp00_Bool		found = FALSE;
    tsp9_rte_installationinfo  instinfo;
    tsp9_release_version newestVersion;
    RTE_RegistryHandle handle;
    tsp00_Pathc  testPath;
    struct stat  statInfo;
    int          statRC;
    tsp00_ErrTextc errtext;
    RTE_IniFileResult rc;

    eo44initError (&xerror);
    newestVersion.no [0] = csp9_invalid_version;

    handle = RTE_OpenConfigEnum ( SAPDB_INSTALLATIONS_INI_FILE,
                                          SAPDB_INSTALLATIONS_SECTION,
                                          errtext,
                                          &rc);
    if (rc != SAPDB_INIFILE_RESULT_OK) 
    {
    	eo44anyError (&xerror, errtext);
        return FALSE;
    }

    while (XOK (&xerror)) 
    {
    	sqlxnext_installationOld (handle, &instinfo, &xerror);
	    if (XOK (&xerror)) 
        {
            if (versionCompare_MF (&newestVersion, &instinfo.version) < 0) 
            {
                 strcpy ((char *)testPath, instinfo.dbroot);
                 strcat ((char *)testPath, "/pgm/");
                 strcat ((char *)testPath, serverpgm);
                 statRC = stat ((char *)testPath, &statInfo);
                 if ((statRC == 0)                && 
                     (statInfo.st_mode & S_IFREG) &&   /* PTS 1105115 */
                     (statInfo.st_mode & S_IEXEC) )  
                 {
                     strcpy (dbroot, instinfo.dbroot);
                     newestVersion = instinfo.version;
                     found = TRUE;
        	     }
    	    }
	    }
    }

    RTE_CloseConfigEnum (handle, errtext, &rc);
    return found;
}

/**\
--------------------------------------- MF__ MOD__ (Installation Registry)
\**/
static void
en904_parseVersion (
    const char * versionString,
    tsp9_release_version * version)
{
#undef MF__
#define MF__ MOD__"en904_parseVersion"
    int versionTmp [csp9_version_digits];
    int i;
    SAPDB_memset (versionTmp, '\0', csp9_version_digits);
    sscanf (versionString, "%d.%d.%d.%d",
	    &versionTmp [0], &versionTmp [1], &versionTmp [2], &versionTmp [3]);
    for (i = 0; i < csp9_version_digits; ++i) 
    {
        version->no [i] = (signed char) versionTmp [i];
    }
}

