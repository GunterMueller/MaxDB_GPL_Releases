/*!*********************************************************************

    @file         Proc_SessionList.hpp

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

#ifndef PROC_SESSIONLIST_HPP
#define PROC_SESSIONLIST_HPP

#include "DBProc/Proc_UDEDirectory.hpp"

class SAPDBMem_IRawAllocator;
class Proc_ISession;

/*!

  @class Proc_SessionList

 */
class Proc_SessionList
    : public Proc_UDEDirectory
{
public:
    Proc_SessionList (SAPDBMem_IRawAllocator * allocator);
    ~Proc_SessionList ();
    Proc_ISession * findSession (Catalog_DBProcObject::Language langid,
        const char * langstring);
    void addSession (Catalog_DBProcObject::Language langid,
        const char * langstring, Proc_ISession * session);
};


#endif
