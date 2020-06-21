/*!*********************************************************************

    @file         Proc_SessionList.cpp

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

#include "DBProc/Proc_SessionList.hpp"

#include "DBProc/Proc_ISession.hpp"

/*----------------------------------------*/

Proc_SessionList::Proc_SessionList (
    SAPDBMem_IRawAllocator * allocator)
: Proc_UDEDirectory (allocator)
{
}

/*----------------------------------------*/

static void
releaseSession (
    void * sessionObject)
{
    Proc_ISession * session = (Proc_ISession*) sessionObject;
    session->release ();
}

/*----------------------------------------*/

Proc_SessionList::~Proc_SessionList ()
{
    this->withAllDo (releaseSession);
}

/*----------------------------------------*/

Proc_ISession * Proc_SessionList::
findSession (
    Catalog_DBProcObject::Language langid,
    const char * langstring)
{
    return (Proc_ISession*) this->findEntry (langid, langstring);
}

/*----------------------------------------*/

void Proc_SessionList::
addSession (
    Catalog_DBProcObject::Language langid,
    const char * langstring,
    Proc_ISession * session)
{
    this->addEntry (langid, langstring, session);
}

/*----------------------------------------*/

