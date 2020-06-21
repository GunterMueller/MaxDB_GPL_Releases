/*!*********************************************************************

    @file         Proc_UDEDirectory.cpp

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

#include "DBProc/Proc_UDEDirectory.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

typedef struct ListEntryT {
    ListEntryT * next;
    void       * data;
    const char * langstring;
    char         rawstart [1];
} ListEntryT;

/*----------------------------------------*/

Proc_UDEDirectory::Proc_UDEDirectory (
    SAPDBMem_IRawAllocator * allocatorP)
{
    this->allocator = allocatorP;
    for (int lang = Catalog_DBProcObject::PL;
            lang < Catalog_DBProcObject::Unknown; ++lang)
    {
        this->standardEntries [lang] = NULL;
    }
    this->otherEntries = NULL;
}

/*----------------------------------------*/

Proc_UDEDirectory::~Proc_UDEDirectory ()
{
    ListEntryT * current = (ListEntryT*) this->otherEntries;
    while (current != NULL) {
        // TODO: close data
        ListEntryT * next = current->next;
        this->allocator->Deallocate (current);
        current = next;
    }
}

/*----------------------------------------*/

void * Proc_UDEDirectory::
findEntry (
    Catalog_DBProcObject::Language langid,
    const char * langstring)
{
    if (langid != Catalog_DBProcObject::Unknown) {
        return this->standardEntries [langid];
    }
    else {
        ListEntryT * current = (ListEntryT*) this->otherEntries;
        while (current != NULL) {
            ListEntryT * next = current->next;
            if (strcmp (langstring, current->langstring) == 0) {
                return current->data;
            }
            current = next;
        }
        return NULL;
    }
}

/*----------------------------------------*/

void Proc_UDEDirectory::
addEntry (
    Catalog_DBProcObject::Language langid,
    const char * langstring,
    void * newptr)
{
    if (langid != Catalog_DBProcObject::Unknown) {
        this->standardEntries [langid] = newptr;
    }
    else {
        int langstringLen = strlen (langstring) + 1;
        ListEntryT * newentry = (ListEntryT *) this->allocator->Allocate (
            sizeof (ListEntryT) + langstringLen);
        if (newentry != NULL) {
            newentry->next = (ListEntryT*) this->otherEntries;
            this->otherEntries = newentry;
            newentry->data = newptr;
            memcpy (newentry->rawstart, langstring, langstringLen);
            newentry->langstring = newentry->rawstart;
        }
    }
}

/*----------------------------------------*/

void Proc_UDEDirectory::
removeEntry (
    void * entry)
{
    for (int lang = Catalog_DBProcObject::PL;
            lang < Catalog_DBProcObject::Unknown; ++lang)
    {
        if (this->standardEntries [lang] == entry) {
            this->standardEntries [lang] = NULL;
            return;
        }
    }
    ListEntryT ** backlink = (ListEntryT**) &this->otherEntries;
    ListEntryT * current = (ListEntryT*) this->otherEntries;
    while (current != NULL) {
        ListEntryT * next = current->next;
        if (current->data == entry) {
            *backlink = current->next;
            this->allocator->Deallocate (current);
        }
        current = next;
    }
}

/*----------------------------------------*/

void Proc_UDEDirectory::
withAllDo (
    void (*func) (void*))
{
    for (int lang = Catalog_DBProcObject::PL;
            lang < Catalog_DBProcObject::Unknown; ++lang)
    {
        if (this->standardEntries [lang] !=  NULL) {
            func (this->standardEntries [lang]);
        }
    }
    ListEntryT * current = (ListEntryT*) this->otherEntries;
    while (current != NULL) {
        func (current->data);
        current = current->next;
    }
}

/*----------------------------------------*/

void Proc_UDEDirectory::
withAllDo (
    void (*func) (void*, void*),
    void * arg)
{
    for (int lang = Catalog_DBProcObject::PL;
            lang < Catalog_DBProcObject::Unknown; ++lang)
    {
        if (this->standardEntries [lang] !=  NULL) {
            func (this->standardEntries [lang], arg);
        }
    }
    ListEntryT * current = (ListEntryT*) this->otherEntries;
    while (current != NULL) {
        func (current->data, arg);
        current = current->next;
    }
}

/*----------------------------------------*/

Proc_UDEDirectory::Iterator::
Iterator (
    Proc_UDEDirectory * directoryP)
{
    this->directory = directoryP;
    this->inStdPart = true;
    this->stdIndex = Catalog_DBProcObject::PL;
    this->otherPos = NULL;
}

/*----------------------------------------*/

bool Proc_UDEDirectory::Iterator::
next (
    void ** element)
{
    if (this->inStdPart) {
        for (; this->stdIndex < Catalog_DBProcObject::Unknown; ++this->stdIndex) {
            /*
             * find in standard entries
             */
            if (this->directory->standardEntries [this->stdIndex] != NULL) {
                *element = this->directory->standardEntries [this->stdIndex];
                ++this->stdIndex;
                return true;
            }
            /*
             * prepare iterator to search in extended entries
             */
            this->inStdPart = false;
            this->otherPos = this->directory->otherEntries;
        }
    }
    /*
     * search in extended entries
     */
    if (this->otherPos == NULL) {
        return false;
    }
    ListEntryT * entry = static_cast<ListEntryT *> (this->otherPos);
    *element = entry->data;
    this->otherPos = entry->next;
    return true;
}

/*----------------------------------------*/

