/*!*********************************************************************

    @file         Proc_UDEDirectory.hpp

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

#ifndef PROC_UDEDIRECTORY_HPP
#define PROC_UDEDIRECTORY_HPP

#include "SQLManager/Catalog/Catalog_DBProcObject.hpp"

/*!

  @class Proc_UDEDirectory

 */
class Proc_UDEDirectory
{
protected:
    Proc_UDEDirectory (SAPDBMem_IRawAllocator * allocator);
    ~Proc_UDEDirectory ();
    void * findEntry (Catalog_DBProcObject::Language langid,
        const char * langstring);
    void addEntry (Catalog_DBProcObject::Language langid,
        const char * langstring, void * newptr);
    void removeEntry (void * entry);
    void withAllDo (void (*func) (void *));
    void withAllDo (void (*func) (void *, void *), void * arg);

public:
    class Iterator
    {
    public:
        Iterator (Proc_UDEDirectory * directory);
        bool next (void ** element);
    private:
        Proc_UDEDirectory * directory;
        bool inStdPart;
        int stdIndex;
        void * otherPos;
    };

protected:
    SAPDBMem_IRawAllocator * allocator;
    void  * standardEntries [Catalog_DBProcObject::Unknown];
    void  * otherEntries;
    friend class Proc_UDEDirectory::Iterator;
};

#endif

