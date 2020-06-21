/*!
  @file           RTE_GetDbroot.cpp
  @author         JoergM
  @brief          DBM RunTime: Get DBROOT from DBNAME

\if EMIT_LICENCE
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

\endif
*/

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "RunTime/RTE_IniFileHandling.h"
#include "RunTime/RTE_SdbStarter.h"
#include "SAPDBCommon/SAPDB_Names.h"


/*---------------------------------------*/

/* Argument parsing */
static SAPDB_Char const *GetDbname(int argc, char *argv[]);

/*---------------------------------------*/

int main(int argc, SAPDB_Char *argv[])
{
    SAPDB_Char const *dbname;
    SAPDB_Char dbroot[PATH_MAX+1];
    SAPDB_Int4 callerUid;
    SAPDB_Int4 callerGid;
    SAPDB_Bool isAdministrator;

    RTE_CallSdbStarter(argc, argv, callerUid, callerGid, isAdministrator);

    dbname = GetDbname(argc, argv);
    if ( !dbname )
    {
        fprintf(stderr, "Usage: %s [-d] <database name>\n", argv[0]);
        exit(1);
    }

    SAPDB_Int rc;
    RTE_IniFileResult  ok;
    tsp00_ErrTextc errtext;

    rc = RTE_GetGlobalConfigString ( SAPDB_DATABASES_INI_FILE, 
                                     SAPDB_DATABASES_SECTION,
                                     dbname,
                                     &dbroot[0],
                                     PATH_MAX,
                                     errtext,
                                     ok);

    if(SAPDB_INIFILE_RESULT_OK != ok)
    {
        fprintf(stderr, "Failed to get root path for '%s'\n", dbname);
        exit(1);
    }
    else
    {
        printf("%s\n", dbroot);
    }

    return 0;
}

/*
   Argument parsing
 */
static SAPDB_Char const *GetDbname(int argc, char *argv[])
{
    SAPDB_Char const *dbname;

    if ( argc < 2 )
    {
        fprintf(stderr, "Missing database name\n");
        return (SAPDB_Char const *)0;
    }

    if ( argc > 3 )
    {
        fprintf(stderr, "Too many arguments\n");
        return (SAPDB_Char const *)0;
    }

    dbname = argv[1];

    if ( '-' == *dbname )
    {
        ++dbname;
        if ( 'd' == *dbname )
        {
            ++dbname;
            if ( 0 == *dbname )
            {
                if ( argc < 3 )
                {
                    fprintf(stderr, "Missing database name\n");
                    dbname = (SAPDB_Char const *)0;
                }
                else if ( argc > 3 )
                {
                    fprintf(stderr, "Too many arguments\n");
                    return (SAPDB_Char const *)0;
                }
                else
                {
                    dbname = argv[2];
                }
            }
        }
        else
        {
            if ( 'h' != *dbname && '?' != *dbname )
            {
                fprintf(stderr, "Wrong option '-%c'\n", *dbname);
            }
            return (SAPDB_Char const *)0;
        }
    }
    else 
    {
        if ( argc != 2 )
        {
            fprintf(stderr, "Too many arguments\n");
            return (SAPDB_Char const *)0;
        }
    }

    return dbname;
}

