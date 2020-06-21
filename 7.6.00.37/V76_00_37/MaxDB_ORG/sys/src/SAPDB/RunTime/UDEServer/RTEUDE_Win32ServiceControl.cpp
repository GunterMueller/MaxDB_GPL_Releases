/*!*********************************************************************

    @file         RTEUDE_Win32ServiceControl.cpp

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

#include "RunTime/UDEServer/RTEUDE_Win32ServiceControl.hpp"

#include <string.h>
#include <windows.h>

/*----------------------------------------*/

static const char * defaultServiceName = "sdbuderelay";
static const char * defaultServiceDisplay = "SDB-UDE-Relay";

/*----------------------------------------*/

static const char *
fullName (
    const char * defaultName,
    const char * delimiter,
    const char * name,
    char       * buf)
{
    if ((name == NULL) || (name [0] == '\0')) {
        return defaultName;
    }
    strcpy (buf, defaultName);
    strcat (buf, delimiter);
    strcat (buf, name);
    return buf;
}

/*----------------------------------------*/

const char * RTEUDE_Win32ServiceControl::
fullServiceName (
    const char * name,
    char       * buf)
{
    return fullName (defaultServiceName, "-", name, buf);
}

/*----------------------------------------*/

const char * RTEUDE_Win32ServiceControl::
fullServiceDisplay (
    const char * name,
    char       * buf)
{
    return fullName (defaultServiceDisplay, ": ", name, buf);
}

/*----------------------------------------*/

bool RTEUDE_Win32ServiceControl::
start (
    const char * serviceName)
{
    SC_HANDLE service;
    SC_HANDLE scm;
    BOOL     success;
    char     serviceNameBuf [MAX_PATH];

    scm = OpenSCManager (0, 0,
        SC_MANAGER_ALL_ACCESS | GENERIC_WRITE);
    if (!scm) {
        return false;
    }
    service = OpenService (scm,
        RTEUDE_Win32ServiceControl::fullServiceName (serviceName, serviceNameBuf),
        SERVICE_ALL_ACCESS);
    success = StartService (service, 0, NULL);
    if (!success) {
        return false;
    }
    CloseServiceHandle (service);
    CloseServiceHandle (scm);
    return true;
}

/*----------------------------------------*/

bool RTEUDE_Win32ServiceControl::
stop (
    const char * serviceName)
{
    SC_HANDLE service;
    SC_HANDLE scm;
    BOOL     success;
    char     serviceNameBuf [MAX_PATH];

    scm = OpenSCManager (0, 0,
        SC_MANAGER_ALL_ACCESS | GENERIC_WRITE);
    if (!scm) {
        return false;
    }
    service = OpenService (scm,
        RTEUDE_Win32ServiceControl::fullServiceName (serviceName, serviceNameBuf),
        SERVICE_ALL_ACCESS);
    SERVICE_STATUS serviceStatus;
    success = ControlService (service, SERVICE_CONTROL_STOP, &serviceStatus);
    if (!success) {
        return false;
    }
    CloseServiceHandle (service);
    CloseServiceHandle (scm);
    return true;
}

