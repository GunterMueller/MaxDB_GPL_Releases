/*!---------------------------------------------------------------------
  @file           RTE_WindowsReleaseDependend.c
  @author         RobinW
  @brief          wrapper for windows API functions that are not available in all windows releases
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

#include <windows.h> /* nocheck */

#include "RunTime/RTE_WindowsReleaseDependend.h"

/*! GetSystemTimes Windows API Prototype function pointer */
typedef BOOL (WINAPI *GETSYSTEMTIMES)  (FILETIME *,FILETIME *,FILETIME *);
/*! AttachConsole Windows API Prototype function pointer */
typedef BOOL (WINAPI *ATTACHCONSOLE)   (DWORD);

static GETSYSTEMTIMES  m_pGetSystemTimes = NULL;
static ATTACHCONSOLE   m_pAttachConsole  = NULL;
static SAPDB_Bool      m_initialized     = false;


static void initialize(void)
{
    HMODULE kernel32Handle = GetModuleHandle("kernel32");
    if(NULL == kernel32Handle)
    {
        // there must be something VERY WRONG!!!;
        return;
    }
    m_pGetSystemTimes   = (GETSYSTEMTIMES)GetProcAddress(kernel32Handle,"GetSystemTimes");
    m_pAttachConsole    = (ATTACHCONSOLE) GetProcAddress(kernel32Handle,"AttachConsole");

    m_initialized = true;
}

/* ----------------------- */

SAPDB_Bool RTE_WindowsReleaseDependendGetSystemTimes(FILETIME *t1,FILETIME *t2,FILETIME *t3,BOOL *returnValue)
{
    if(!m_initialized)
        initialize();
    if(m_pGetSystemTimes)
    {
        *returnValue = (*m_pGetSystemTimes)(t1,t2,t3);
        return true;
    }
    else
    {
        return false;
    }
}

/* ----------------------- */

SAPDB_Bool RTE_WindowsReleaseDependendAttachConsole(DWORD pid,BOOL *returnValue)
{
    if(!m_initialized)
        initialize();
    if(m_pAttachConsole)
    {
        *returnValue = (*m_pAttachConsole)(pid);
        return true;
    }
    else
    {
        return false;
    }
}

