/*!*********************************************************************

    @file         RTEIO_DirectoryIterator.cpp

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

#include "RunTime/InputOutput/RTEIO_DirectoryIterator.hpp"

#include <string.h>

#include "heo06.h"

const int errorStateC = 1;
const int startedStateC = 2;
const int readingStateC = 3;
const int atendStateC = 4;

const int unknownDirC = 0;
const int isDirC = 1;
const int noDirC = 2;

#if defined (WIN32)
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif

/*----------------------------------------*/

RTEIO_DirectoryIterator::
RTEIO_DirectoryIterator (
    const char * path,
    int options_in)
{
    this->dirHandle = NULL;
    this->state = errorStateC;
    int dirlen = strlen (path);
    this->options = options_in;
    if (dirlen <= (sizeof (this->fullbuf) - 1)) {
        memcpy (this->fullbuf, path, dirlen);
        this->fullbuf [dirlen] = PATH_SEP;
        tsp00_C256 firstname;
        tsp05_RteFileError  ferr;
        sqlfopendirc ((tsp00_C256*) path, &this->dirHandle, &firstname, &ferr);
        if (ferr.sp5fe_result == vf_ok) {
            this->state = startedStateC;
            this->namestart = this->fullbuf + dirlen + 1;
            strcpy (this->namestart, firstname);
        }
    }
}

/*----------------------------------------*/

RTEIO_DirectoryIterator::
~RTEIO_DirectoryIterator ()
{
    this->close ();
}

/*----------------------------------------*/

bool RTEIO_DirectoryIterator::
basicNext ()
{
    tsp00_C256 currentName;
    tsp05_RteFileError  ferr;
    sqlfreaddirc (this->dirHandle, &currentName, &ferr);
    if (ferr.sp5fe_result == vf_ok) {
        // TODO: check for bufferoverflow
        strcpy (this->namestart, currentName);
        this->isdir = unknownDirC;
        return true;
    }
    else {
        this->state = atendStateC;
        return false;
    }
}

/*----------------------------------------*/

bool RTEIO_DirectoryIterator::
currentOK () const
{
    if (this->options & withDots) {
        return true;
    }
    if (strcmp (this->namestart, ".") == 0) {
        return false;
    }
    if (strcmp (this->namestart, "..") == 0) {
        return false;
    }
    return true;
}

/*----------------------------------------*/

bool RTEIO_DirectoryIterator::
next ()
{
    switch (this->state) {
        case errorStateC:
        case atendStateC:
            return false;
        case readingStateC:
            if (!this->basicNext ()) {
                return false;
            }
            break;
        case startedStateC:
            this->state = readingStateC;
            break;
    }
    while (!this->currentOK ()) {
        if (!this->basicNext ()) {
            return false;
        }
    }
    return true;
}

/*----------------------------------------*/

const char * RTEIO_DirectoryIterator::
currentFullpath () const
{
    return this->fullbuf;
}

/*----------------------------------------*/

const char * RTEIO_DirectoryIterator::
currentName () const
{
    return this->namestart;
}

/*----------------------------------------*/

bool RTEIO_DirectoryIterator::
currentIsDir ()
{
    if (this->isdir == unknownDirC) {
        tsp05_RteFileInfo  finfo;
        tsp05_RteFileError ferr;
        sqlfinfoc (this->fullbuf, &finfo, &ferr);
        if (finfo.sp5fi_media_kind == 7) {
            this->isdir = isDirC;
        }
        else {
            this->isdir = noDirC;
        }
    }
    return this->isdir == isDirC;
}

/*----------------------------------------*/

bool RTEIO_DirectoryIterator::
isValid () const
{
    return this->state != errorStateC;
}

/*----------------------------------------*/

void RTEIO_DirectoryIterator::
close ()
{
    if (this->dirHandle != NULL) {
        tsp05_RteFileError  ferr;
        sqlfclosedirc (this->dirHandle, &ferr);
        this->dirHandle = NULL;
    }
}

