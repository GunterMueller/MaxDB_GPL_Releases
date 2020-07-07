/*!*********************************************************************

    @file         RTEIO_ShellUtil.cpp

    @author       DanielD

    @brief        equivalent to fileutil shell commands

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

#include "RunTime/InputOutput/RTEIO_ShellUtil.hpp"

#if defined _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <errno.h>
#endif

#include "heo06.h"
#include "RunTime/InputOutput/RTEIO_DirectoryIterator.hpp"
#include "SAPDB/Messages/Msg_List.hpp"
#include "SAPDB/Messages/Msg_Arg.hpp"

/*
 * temporary until RunTime component and subcomponents for messages are defined
 */
#define SDBMSG_RUNTIME_RMDIR_FAILED 1, "RunTime", __FILE__, __LINE__, "Failed to remove directory [$RETURNCODE$, $FILENAME$ ]"
#define SDBMSG_RUNTIME_RMFILE_FAILED 2, "RunTime", __FILE__, __LINE__, "Failed to remove file [$RETURNCODE$, $FILENAME$ ]"

/*----------------------------------------*/

bool  RTEIO_ShellUtil::
RemoveFile (
    const char * path,
    Msg_List & msgList)
{
    tsp05_RteFileError ferr;
    sqlferasec (path, &ferr);
    if (ferr.sp5fe_result != vf_ok) {
        msgList = Msg_List (Msg_List::Error, SDBMSG_RUNTIME_RMFILE_FAILED,
            Msg_Arg("RETURNCODE", ferr.sp5fe_result),
            Msg_Arg("FILENAME", path));
        return false;
    }
    return true;
}

/*----------------------------------------*/

bool  RTEIO_ShellUtil::
RemoveDir (
    const char * path,
    Msg_List & msgList)
{
    int rc = 0;
#ifdef WIN32
    if (!RemoveDirectory ( path )) {
        rc = GetLastError ();
    }
#else
    if (rmdir ( path ) != 0) {
        rc = errno;
    }
#endif
    if (rc != 0) {
        msgList = Msg_List (Msg_List::Error, SDBMSG_RUNTIME_RMDIR_FAILED,
            Msg_Arg("RETURNCODE", rc),
            Msg_Arg("FILENAME", path));
        return false;
    }
    return rc == 0;
}

/*----------------------------------------*/

static bool
removeTreeImplementation (
    const char * path,
    int options,
    Msg_List & msgList)
{
    bool result = true;
    Msg_List localList;
    RTEIO_DirectoryIterator iter (path);

    while (iter.next ()) {
        bool ok;
        if (iter.currentIsDir ()) {
            /*
             * remove subdirectory
             */
            ok = removeTreeImplementation (iter.currentFullpath (), options, msgList);
        }
        else {
            /*
             * remove regular file
             */
            ok = RTEIO_ShellUtil::RemoveFile (iter.currentFullpath (), localList);
            if (!ok) {
                msgList.AppendNewMessage (localList);
            }
        }
        if (!ok) {
            if (options & RTEIO_ShellUtil::continueOnError) {
                result = false;
            }
            else {
                return false;
            }
        }
    }
    iter.close ();
    /*
     * remove this directory
     */
    if (!RTEIO_ShellUtil::RemoveDir (path, localList)) {
        msgList.AppendNewMessage (localList);
        return false;
    }
    /*
     *
     */
    return result;
}

/*----------------------------------------*/

bool RTEIO_ShellUtil::
RemoveTree (
    const char * path,
    int options,
    Msg_List & msgList)
{
    Msg_List incrementalList;
    return removeTreeImplementation (path, options, incrementalList);
}

/*----------------------------------------*/

bool RTEIO_ShellUtil::
RemoveTree (const char * path, Msg_List & msgList)
{
    return RemoveTree (path, 0, msgList);
}

/*----------------------------------------*/

