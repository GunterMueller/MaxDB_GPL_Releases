/*!
  @file           ProcRTE_Runtime.hpp
  @author         DanielD
  @brief          

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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


#ifndef PROCRTE_RUNTIME_HPP
#define PROCRTE_RUNTIME_HPP

class SAPDBMem_IRawAllocator;

/*!
  @class          ProcRTE_Runtime

 */

class ProcRTE_Runtime
{
    
public:
    static char * alloc (int size);
    static void * free (void * ptr);
    static SAPDBMem_IRawAllocator * allocator ();
    static void   setLogPrefix (const char * prefix);
    static void   log (const char * format, ...);
    static void   logError (const char * format, ...);
    static void   logToDo (const char * format, ...);
    static void   enableToDoLog ();
    static void   disableToDoLog ();
    template <class T>
    static void destroy(T* & ptr)
    {
        ptr->~T();
        ProcRTE_Runtime::free (ptr);
        ptr = 0;
    }

    
private:
    ProcRTE_Runtime ();
    ~ProcRTE_Runtime ();
};

#define PROCRTE_RUNTIME_NEW(classtype) new (ProcRTE_Runtime::alloc (sizeof (classtype))) classtype

/*!

  @class ProcRTE_AutoPtr

 */
template <class T>
class ProcRTE_AutoPtr
{
public:
    ProcRTE_AutoPtr (T * initptr) {
        this->ptr = initptr;
    }
    ProcRTE_AutoPtr (int size) {
        this->ptr = (T *) ProcRTE_Runtime::alloc (size);
    }
    ~ProcRTE_AutoPtr () {
        if (this->ptr != 0) {
            ProcRTE_Runtime::free (this->ptr);
        }
    }
    T * operator-> () {
        return this->ptr;
    }
    T & operator * () {
        return *this->ptr;
    }
    operator T* () {
        return this->ptr;
    }
private:
    T * ptr;
    ProcRTE_AutoPtr & operator= (ProcRTE_AutoPtr & other) {}
};


#endif
