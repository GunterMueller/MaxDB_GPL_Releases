/*!

  \file    DBMSrvIO_SharedTextFile.hpp
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

#if !defined(DBMSrvIO_SharedTextFile_hpp)
#define DBMSrvIO_SharedTextFile_hpp

#include "hni34.h"

class DBMSrvIO_SharedTextFile: public tni34_ATextFile {

public:
    DBMSrvIO_SharedTextFile(
        const char* name,
        ADirection Dir=FromDisk,
        int CreateOnOpen=1,
        int OpenNow=1);

#if defined (WIN32)
protected:
    DWORD getShareMode();
#endif
};

#endif //DBMSrvIO_SharedTextFile_hpp
