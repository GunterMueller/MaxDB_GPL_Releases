/*!
    @file           DBMSrvShM_InitialisationSynchroniser.cpp
    @author         MarcW
    @brief          sync processes when creating the shared memory

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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



*/

#include "DBM/Srv/SharedMemory/DBMSrvShM_InitialisationSynchroniser.hpp"

#if defined(WIN32)
    #include <windows.h>
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

bool DBMSrvShM_InitialisationSynchroniser::permissionToInitialize(const char* syncFile) {

    if( syncFile == NULL )
        return false;

#if defined(WIN32)
    HANDLE syncHandle =
            CreateFile(
                syncFile,
                GENERIC_READ|GENERIC_WRITE,
                0,
                0,
                CREATE_NEW,            // <- makes call return INVALID_HANDLE_VALUE, if
                FILE_ATTRIBUTE_NORMAL, // syncFile already exists
                0);

    if( syncHandle == INVALID_HANDLE_VALUE )
        return false; // call failed, it existed already
    else {
        CloseHandle(syncHandle);
        return true;
    }
#else
    int syncHandle = open(
                        syncFile,
                        O_CREAT|O_RDWR|O_EXCL,            // create file, fail if it exists
                        S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP); // r/w for user and group
    if( syncHandle == -1 )
        return false; // call failed, it existed already
    else {
        close(syncHandle);
        return true;
    }
#endif
}
