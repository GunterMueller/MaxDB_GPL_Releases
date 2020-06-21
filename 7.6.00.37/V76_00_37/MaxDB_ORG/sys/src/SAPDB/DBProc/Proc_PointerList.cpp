/*!*********************************************************************

    @file         Proc_PointerList.cpp

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

#include "DBProc/Proc_PointerList.hpp"

#include <stddef.h>

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

typedef struct Proc_PointerList::ListPageT {
    ListPageT * next;
    int         pageSize;
    void      * data [1];
} ListPageT;

const int Proc_PointerList::noLimitC = -1;

/*----------------------------------------*/

Proc_PointerList::
Proc_PointerList (
    SAPDBMem_IRawAllocator * allocatorParm,
    int maxElementsParm)
{
    this->singleElement = NULL;
    this->allocator = allocatorParm;
    this->usedElements = 0;
    this->maxElements = maxElementsParm;
}

/*----------------------------------------*/

Proc_PointerList::
~Proc_PointerList ()
{
    if (this->maxElements != 1) {
        ListPageT * current = this->firstPage;
        while (current != NULL) {
            ListPageT * next = current->next;
            this->allocator->Deallocate (current);
            current = next;
        }
    }
}

/*----------------------------------------*/

static bool
findObject (
    ListPageT  * page,
    void       * requested,
    ListPageT ** pageFound,
    int        * indexFound)
{
    while (true) {
        if (page == NULL) {
            return false;
        }
        for (int i = 0; i < page->pageSize; ++i) {
            if (page->data [i] == requested) {
                *pageFound = page;
                *indexFound = i;
                return true;
            }
        }
        page = page->next;
    }
}

/*----------------------------------------*/

static ListPageT *
allocPage (
    SAPDBMem_IRawAllocator * allocator,
    int                      maxElements)
{
    int pageElements;
    if (maxElements != Proc_PointerList::noLimitC) {
        pageElements = maxElements;
    }
    else {
        pageElements = 32;
    }
    int pageBytes = offsetof (ListPageT, data) + pageElements * sizeof (void*);
    ListPageT * newPage = (ListPageT*) allocator->Allocate (pageBytes);
    if (newPage == NULL) {
        return NULL;
    }
    newPage->pageSize = pageElements;
    for (int i = 0; i < pageElements; ++i) {
        newPage->data [i] = NULL;
    }
    return newPage;
}

/*----------------------------------------*/

bool Proc_PointerList::
addPointer (
    void * object)
{
    if (!this->hasFreeSlot ()) {
        return false;
    }
    if (this->maxElements == 1) {
        this->singleElement = object;
    }
    else {
        ListPageT * pageFound;
        int         indexFound;

        if (!findObject (this->firstPage, NULL, &pageFound, &indexFound)) {
            pageFound = allocPage (this->allocator, this->maxElements);
            if (pageFound == NULL) {
                return false;
            }
            indexFound = 0;
            pageFound->next = this->firstPage;
            this->firstPage = pageFound;
        }
        pageFound->data [indexFound] = object;
    }
    ++this->usedElements;
    return true;
}

/*----------------------------------------*/

void Proc_PointerList::
clearPointer (
    void * object)
{
    if (this->maxElements == 1) {
        if (this->singleElement == object) {
            this->singleElement = NULL;
            --this->usedElements;
        }
    }
    else {
        ListPageT * pageFound;
        int         indexFound;

        if (findObject (this->firstPage, object, &pageFound, &indexFound)) {
            pageFound->data [indexFound] = NULL;
            --this->usedElements;
        }
    }
}

/*----------------------------------------*/

bool Proc_PointerList::
hasFreeSlot () const
{
    if (this->maxElements == Proc_PointerList::noLimitC) {
        return true;
    }
    return this->usedElements < this->maxElements;
}

/*----------------------------------------*/

bool Proc_PointerList::
hasPointer (
    void * object) const
{
    ListPageT * pageFound;
    int         indexFound;
    if (this->maxElements == 1) {
        return object == this->singleElement;
    }
    else {
        return findObject (this->firstPage, object, &pageFound, &indexFound);
    }
}

/*----------------------------------------*/

int Proc_PointerList::
slotsUsed () const
{
    return this->usedElements;
}

/*----------------------------------------*/

void Proc_PointerList::
trace (
    SAPDBTrace_Topic & topic,
    const char * comment)
{
#if !defined (SIMPLE)
    SAPDBTrace_Stream stream (&topic.GetTracePrinter ());
    Proc_ListIterator iterator (this);
    void * entry;
    stream << "PointerList ("
        << this->usedElements << "/" << this->maxElements <<"): "
        << comment << FlushLine;
    while (iterator.next (&entry)) {
        stream << "    " << ToStr (entry) << FlushLine;
    }
    stream << "=============================" << FlushLine;
#else
    fprintf (stdout, "PointerList (%d/%d): %s\n", this->usedElements,
        this->maxElements, comment);
    Proc_ListIterator iterator (this);
    void * entry;
    while (iterator.next (&entry)) {
        fprintf (stdout, "    %p\n", entry);
    }
    fprintf (stdout, "=============================\n");

#endif
}

/*----------------------------------------*/

Proc_ListIterator::
Proc_ListIterator (
    const Proc_PointerList * list)
{
    this->init (list);
}

/*----------------------------------------*/

Proc_ListIterator::
Proc_ListIterator ()
{
    /*
     * this iterator will fail on first next ()
     *  unless init () is called first
     */
    this->useSingleElement = true;
    this->singleElement = NULL;
}

/*----------------------------------------*/

Proc_ListIterator::
~Proc_ListIterator ()
{
}

/*----------------------------------------*/

void Proc_ListIterator::
init (
    const Proc_PointerList * list)
{
    if (list->maxElements == 1) {
        this->useSingleElement = true;
        this->singleElement = list->singleElement;
    }
    else {
        this->useSingleElement = false;
        this->manyElements.currentPage = list->firstPage;
        this->manyElements.indexInPage = 0;
    }
}

/*----------------------------------------*/

bool Proc_ListIterator::
next (void ** nextEntry)
{
    if (this->useSingleElement) {
        return this->nextForSingleElement (nextEntry);
    }
    else {
        return this->nextForMultipleElements (nextEntry);
    }
}

/*----------------------------------------*/

bool Proc_ListIterator::
nextForSingleElement (
    void ** nextEntry)
{
    bool result;

    if (this->singleElement == NULL) {
        result = false;
    }
    else {
        *nextEntry = this->singleElement;
        this->singleElement = NULL;
        result = true;
    }

    return result;
}

/*----------------------------------------*/

bool Proc_ListIterator::
nextForMultipleElements (
    void ** nextEntry)
{
    while (true) {
        /*
         * end of List
         */
        if (this->manyElements.currentPage == NULL) {
            return false;
        }
        /*
         * valid entry
         */
        ListPageT * currentPage = (ListPageT *) this->manyElements.currentPage;
        while (this->manyElements.indexInPage < currentPage->pageSize) {
            void * entry = currentPage->data [this->manyElements.indexInPage];
            ++this->manyElements.indexInPage;
            if (entry != NULL) {
                *nextEntry = entry;
                return true;
            }
        }
        /*
         * switch to next page
         */
        this->manyElements.currentPage = currentPage->next;
        this->manyElements.indexInPage = 0;
    }
}

/*----------------------------------------*/

