/*!*********************************************************************

    @file         RTEUDE_RelayUnixMain.cpp

    @author       DanielD

    @brief

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
***********************************************************************/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_DefaultRawAllocator.hpp"
#include "RunTime/UDEServer/RTEUDE_CommandInterpreter.hpp"
#include "RunTime/UDEServer/RTEUDE_StarterRelay.hpp"
#include "RunTime/UDEServer/RTEUDE_PipeIPC.hpp"
#include "RunTime/UDEServer/RTEUDE_SocketIPC.hpp"


/*----------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

/*
 * generated by genOptparser.py n,name,
 */

/*!
  Argument count + vector
 */
typedef struct ArgvT_ {
    int argc;
    const char ** argv ;
} ArgvT;

/*----------------------------------------*/
/*!
  Options
 */
typedef struct OptionsT_ {
    ArgvT all;
    ArgvT args;
    const char *  name;
} OptionsT;


static int
parseOptions (
    OptionsT   * options,
    int          argc,
    const char * argv [])
{
    int argi = 1;
    bool inOptions = true;

    options->all.argc = argc;
    options->all.argv = argv;
    options->name = NULL;
    while (inOptions && (argi < argc)) {
        const char * currarg = argv [argi];
        if (currarg [0] != '-') {
            break;
        }
        switch (currarg [1]) {
            case '-':
                inOptions = false;
                break;
            case 'n':
                options->name = argv [argi + 1];
                ++argi;
                break;
            default:
                fprintf (stderr, "invalid option -%c\n", currarg [1]);
                exit (2);
                break;
        }
        ++argi;
    }

    options->args.argc = argc - argi;
    options->args.argv = argv + argi;
    return argi;
}

/*----------------------------------------*/

static void
startRelay (
    OptionsT * options)
{
    /*
     * create listener
     */
    //RTEUDE_PipeServer   pipe (serviceData.servicename,
    //    &SAPDBMem_DefaultRawAllocator::Instance ());
    RTEUDE_SocketServer server (&SAPDBMem_DefaultRawAllocator::Instance (), 7211);
    /*
     * detach from terminal
     */
    /*
     * start relay
     */
    RTEUDE_MessageIPC ipc (server);
    char errortext [255];

    RTEUDE_StarterRelay relay;
    relay.serve (ipc, errortext);
}

/*----------------------------------------*/

int
main (
    int argc,
    const char * argv [])
{
    OptionsT options;
    int argi = parseOptions (&options, argc, argv);
    return 0;
}
