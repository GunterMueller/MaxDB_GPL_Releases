/*!*********************************************************************

    @file         Proc_PointerList.hpp

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

#ifndef PROC_POINTERLIST_HPP
#define PROC_POINTERLIST_HPP

class SAPDBMem_IRawAllocator;
class Proc_ListIterator;
class SAPDBTrace_Topic;

/*!

  @class Proc_PointerList

 */
class Proc_PointerList
{
public:
    Proc_PointerList (SAPDBMem_IRawAllocator* allocator, int maxElements);
    ~Proc_PointerList ();
    bool addPointer (void * object);
    void clearPointer (void * object);
    bool hasFreeSlot () const;
    bool hasPointer (void* object) const;
    int  slotsUsed () const;
    void trace (SAPDBTrace_Topic & topic, const char * comment);

public:
    const static int noLimitC;
    struct ListPageT;
protected:
    SAPDBMem_IRawAllocator * allocator;
    union {
        void      * singleElement;
        ListPageT * firstPage;
    };
    int usedElements;
    int maxElements;
    friend class Proc_ListIterator;
};

/*!

  @class Proc_ListIterator

 */
class Proc_ListIterator
{
public:
    Proc_ListIterator (const Proc_PointerList * list);
    Proc_ListIterator ();
    ~Proc_ListIterator ();
    void init (const Proc_PointerList * list);
    bool next (void ** nextEntry);
private:
    bool nextForSingleElement (void ** nextEntry);
    bool nextForMultipleElements (void ** nextEntry);

private:
    bool useSingleElement;
    union {
        void * singleElement;    /* used when maxElements == 1 */
        struct  {                /* used when maxElements != 1 */
            Proc_PointerList::ListPageT * currentPage;
            int indexInPage;
        } manyElements;
    };
};

#endif
