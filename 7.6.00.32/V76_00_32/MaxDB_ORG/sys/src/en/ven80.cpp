/*!---------------------------------------------------------------------
  @file           ven80.cpp
  @author         JoergM
  @brief          C++ Kernel Main

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
---------------------------------------------------------------------*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gen81.h"
#include "geo50_0.h"
#include "geo007_1.h"
#include "gen38.h"
#include <stdlib.h>
#include "RunTime/RTE_KernelMain.h"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "gen60.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageOutput.h"
#include "SAPDBCommon/SAPDB_Names.h"

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
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

int
main ( int argc , char *argv[] , char *envp[] )
{
    SAPDBErr_SetMessageOutputCall( en60MessageOutputCall );

    /*
     *  Avoid buffering, this must be called before any io is done
     */
    setbuf ( stdin  , (char*)0 );
    setbuf ( stdout , (char*)0 );
    setbuf ( stderr , (char*)0 );

    /*
     *  File permission mode is always specified absolutely
     *  within the kernel.
     */
    (void) umask ( 007 );

    {
        uid_t euid = geteuid();
        gid_t egid = getegid();
        /* root user must become effectiv root user before it can set to 'any' user/group combination! */
        if ( getuid() == 0 )
        {
            setuid(0);
        }
        setregid(egid, egid);
        setreuid(euid, euid);
    }

    int exit_code;
    /* Parse the command line -> this allows to read use ParameterAccess instance... */
    exit_code = en81ParseCommandLine(argc, argv);
    if ( exit_code != 0 )
    {
        exit(exit_code);
    }

    /* make sure first three descriptor are opened... */
    int devNullFd = 0;
    do
    {
        devNullFd = open("/dev/null", O_WRONLY);
    } while ( devNullFd >= 0 && devNullFd <= 2 );
    if ( devNullFd > 2 ) close(devNullFd);

    /* After this we are able to synchronize output of different threads ... */
    exit_code = en60InitializeMessageOutput();
    if ( exit_code != 0 )
    {
        printf("Failed to open %s\n",SAPDB_DIAG_FILE_NAME);
        exit(exit_code);
    }

    /*
     * New code main routine. Performs i.e. Allocator initialization
     */
    exit_code = RTE_KernelMain(argc, argv);

    if ( exit_code != 0 )
    {
        perror("Global initialization failed");
    }
    else
    {
        /*
         * Old code main routine (calls < 7.3 code...) Maybe one day it can be removed...
         */
        exit_code = en81_kernel_main ( argc , argv  , envp );
    }

    exit(exit_code) ;
}

/*----------*/

extern "C" int
en80RunKernel( struct ten50_KT_Control *pMain )
{
    int rc = 111;
    try 
    {
        RTE_KGS::Instance().SetKGS(kgs);
        RTE_KGS::Instance().InitStatistic();

        rc = en81RunKernel( pMain );
    }
    catch (SAPDBErr_MessageList &lastMsg)
    {
        RTE_Crash(lastMsg);
    }
    return rc;
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/