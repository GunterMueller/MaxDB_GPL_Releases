/*!

  \file    DBMSrvIO_SharedTextFile.cpp
  \author  MarcW
  \ingroup IO
  \brief   a file that can be opened by mora than one process simultaneously

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

#include <stdio.h>

#include "DBM/Srv/IO/DBMSrvIO_SharedTextFile.hpp"

DBMSrvIO_SharedTextFile::DBMSrvIO_SharedTextFile(
        const char* name,
        ADirection Dir,
        int CreateOnOpen,
        int OpenNow)
        : tni34_ATextFile(name, Dir, CreateOnOpen, 0) {
    
    // we defer opening the file, because opening it from within the constructor of the
    // base class would call the base class' implementation of getSharedMode() (if that doesn't crash...)
    if( 0 != OpenNow )
        Open();
}

#if defined (WIN32)
DWORD DBMSrvIO_SharedTextFile::getShareMode() {
    return FILE_SHARE_READ | FILE_SHARE_WRITE;
}
#endif
