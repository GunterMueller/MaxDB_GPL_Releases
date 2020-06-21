/*



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





*/
#ifndef CSMUTEX_H
#define CSMUTEX_H
// CSMutex.h: Platform independent mutex functions

#ifdef WIN32
#include <WINDOWS.H>
#else
#include "heo07.h" // mutex
#endif

class CSMutex
{
public:
    CSMutex()
        : mutex(0)
        {
#ifdef WIN32
            this->mutex = CreateSemaphore(NULL,1,1,NULL);
#else
            sqlcreatemutex( &this->mutex );
#endif
        };

    ~CSMutex()
        {
#ifdef WIN32
            CloseHandle(this->mutex);
#else
            sqldestroymutex( &this->mutex );
#endif
        };

    void StartMutex()
        {
#ifdef WIN32
            WaitForSingleObject( this->mutex, INFINITE);
#else
            sqlbeginmutex( &this->mutex );
#endif
        };

    void EndMutex()
        {
#ifdef WIN32
            ReleaseSemaphore( this->mutex, 1, NULL);
#else
            sqlendmutex( &this->mutex );
#endif
        };

private:
#ifdef WIN32
    HANDLE        mutex;
#else
    teo07_Mutex   mutex;
#endif
}; // class CSMutex

#endif


