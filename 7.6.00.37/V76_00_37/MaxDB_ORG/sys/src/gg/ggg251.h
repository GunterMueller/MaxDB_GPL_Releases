/*!***************************************************************************
  module:        ggg251.h
 
  responsible:   ThomasA
 
  special area:  Implementation of basic algorithms
 
  description:   Double Chained List   
 
  see:
 
  Copyright (c) 1998-2005 SAP AG 

  created:      1999-08

  updated:      1999-09-13


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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


*****************************************************************************/

#ifndef GGG251_H
#define GGG251_H

template<class T,class ALLOC> class cgg251DCList;
template<class T,class ALLOC> class cgg251dclIterator; 

template<class T,class ALLOC>
class
ListEntry {
  friend class cgg251DCList<T,ALLOC>;
  friend class cgg251dclIterator<T,ALLOC>; 
  private :
    ListEntry<T,ALLOC>* m_prev;
    ListEntry<T,ALLOC>* m_next;
    T                   m_item;
    ListEntry(const T& t) : m_prev(NULL), m_next(NULL), m_item(t) {}
    ListEntry(const T& t, ListEntry<T,ALLOC>* p, ListEntry<T,ALLOC>* n) : m_prev(p), m_next(n), m_item(t) {}
    void* operator new(size_t sz, ALLOC* a) {
      return a->allocate(sz);
    }
#if defined(PLACEMENT_DELETE_MEO00)
    void operator delete(void* p, ALLOC* a) {};
#endif
    void Delete(ALLOC* a) {
      a->deallocate(this);
    }
};

template<class T,class ALLOC> 
class cgg251dclIterator {
  friend class cgg251DCList<T,ALLOC>;
public :
  cgg251dclIterator(ListEntry<T,ALLOC>* curr, cgg251DCList<T,ALLOC>* list) : m_curr(curr), m_list(list) {}
  operator bool() {
    return (m_curr != (ListEntry<T,ALLOC>*) &m_list->m_prev);
  }
  void operator++() {
    m_curr = m_curr->m_next;
  }
  void operator++(int) {
    m_curr = m_curr->m_next;
  }
  T* operator()() {
    return &m_curr->m_item;
  }
 
  typedef void* VoidPtr;

  void Dump(VoidPtr& thisPtr, VoidPtr& nextPtr, VoidPtr& prevPtr) {
    thisPtr = m_curr; 
    nextPtr = m_curr->m_next; 
    prevPtr = m_curr->m_prev;
  }

private :
  ListEntry<T,ALLOC>*    m_curr;
  cgg251DCList<T,ALLOC>* m_list;
};

/*!
   class : cgg251DCList

   description : implements a double chained list

   template arguments :
     T      Type of list elements
     ALLOC  Allocater used for memory allocation. Must provide allocate and deallocate
   
*/
template<class T,class ALLOC>
class 
cgg251DCList { 
public:
/*!
  function: cgg251DCList
  description : constructor

  arguments : pointer to an allocator instance
*/

  EXPLICIT_MEO00 cgg251DCList(ALLOC* a = NULL) : m_prev((ListEntry<T,ALLOC>*) &m_prev), 
    m_next((ListEntry<T,ALLOC>*) &m_prev), m_alloc(a) {
  }
/*!
  function: ~cgg251DCList
  description: destructor
*/
  ~cgg251DCList();

  ListEntry<T,ALLOC>* m_prev;
  ListEntry<T,ALLOC>* m_next;
  ALLOC*              m_alloc;
/*!
  function: advise_allocator
  description: tells the list to use an allocator after the list has been constructed
  arguments : 
    a[in] pointer to allocator instance
*/
  void advise_allocator(ALLOC* a) { m_alloc = a; }
/*!
  function: begin
  description: returns an iterator pointing to the first list element
  arguments:
    none
*/
  cgg251dclIterator<T,ALLOC> begin() {
    cgg251dclIterator<T,ALLOC> iter(m_next, this);
    return iter;
  }
/*!
  function: back
  description: returns a reference to the last list element
  arguments: none
*/
  T&       back()        { return m_prev->m_item; }
/*!
  function: back const
  description: returns a const reference to the last list element
  arguments: none
*/
  const T& back() const  { return m_prev->m_item; }
/*!
  function: empty
  description: returns true, if the list is empty; false otherwise
  arguments : none
*/
  bool     empty()       { return (m_next == (ListEntry<T,ALLOC>*) &m_prev); }
/*!
  function: front
  description: returns a reference to the first list element
  arguments: none
*/
  T&       front()       { return m_next->m_item; }
/*!
  function: front const
  description: returns a const reference to the first list element
  arguments: none
*/
  const T& front() const { return m_next->m_item; }
/*!
  function: insert_before
  description: inserts an element before an elements identified by an iterator
  arguments:
  t[in] : element to be inserted
  i[in] : iterator pointing to the list element which will become the successor of 
  of t.
*/
  void     insert_before(T& t,cgg251dclIterator<T,ALLOC>& i);
/*!
  function: insert_after
  description: inserts an element after an elements identified by an iterator
  arguments:
  t[in] : element to be inserted
  i[in] : iterator pointing to the list element which will become the predeessor of 
  of t.
*/
  void     insert_after (T&,cgg251dclIterator<T,ALLOC>&);
/*!
  function: pop_back
  description: removes the last list element
  arguments: none
*/
  void     pop_back();
/*!
  function: pop_front
  description: removes the first list element
  arguments: none
*/
  void     pop_front();
/*!
  function: push_back
  description: inserts an element as last list element 
  arguments:
    t[in] : the element to be inserted
*/
  void push_back (const T& t);
/*!
  function: push_front
  description: inserts an element as first list element 
  arguments:
    t[in] : the element to be inserted
*/
  void push_front(const T& t);
/*!
  function: remove
  description: removes an element identified by an iterator from the list
  arguments:
    i[in] : iterator pointing to the Element to be removed
*/
  inline void remove(cgg251dclIterator<T,ALLOC>& i);
/*!
  function: remove_all
  description: removes all list elements. After the call the list is empty.
  arguments : none
*/
  inline void remove_all();
private :
  void delete_elem(ListEntry<T,ALLOC>*);
};

/*! endclass : cgg251DCList */

template<class T, class ALLOC>
cgg251DCList<T,ALLOC>::~cgg251DCList() {
  ListEntry<T,ALLOC>* delElem;
  cgg251dclIterator<T,ALLOC> iter = begin();
  while (iter) {
    delElem = iter.m_curr;
    ++iter;
    delElem->Delete(m_alloc);
  }
}

template<class T, class ALLOC>
inline void cgg251DCList<T,ALLOC>::delete_elem(ListEntry<T,ALLOC>* p) {
  p->m_next->m_prev = p->m_prev;
  p->m_prev->m_next = p->m_next;
  p->Delete(m_alloc);
}

template<class T, class ALLOC>
void cgg251DCList<T,ALLOC>::insert_before(T& t,cgg251dclIterator<T,ALLOC>& iter) {
  ListEntry<T,ALLOC>* curr = iter.m_curr;
  ListEntry<T,ALLOC>* prev = curr->m_prev;
  curr->m_prev = new(m_alloc) ListEntry<T,ALLOC>(t, prev, prev->m_next);
  prev->m_next = curr->m_prev;
}


template<class T, class ALLOC>
void cgg251DCList<T,ALLOC>::insert_after(T& t,cgg251dclIterator<T,ALLOC>& iter) {
  ListEntry<T,ALLOC>* curr = iter.m_curr;
  ListEntry<T,ALLOC>* next = curr->m_next;
  curr->m_next = new(m_alloc) ListEntry<T,ALLOC>(t, next->m_prev, curr->m_next);
  next->m_prev = curr->m_next;
}

template<class T, class ALLOC>
void cgg251DCList<T,ALLOC>::pop_back() {
  ListEntry<T,ALLOC>* delElem = m_prev;
  m_prev         = delElem->m_prev;
  m_prev->m_next = delElem->m_next;
  delElem->Delete(m_alloc);
}

template<class T, class ALLOC>
void cgg251DCList<T,ALLOC>::pop_front() {
  delete_elem(m_next);
}

template<class T, class ALLOC>
void cgg251DCList<T,ALLOC>::push_back(const T& t) {
  ListEntry<T,ALLOC>* prev = m_prev;
  m_prev = new(m_alloc) ListEntry<T,ALLOC>(t, prev, prev->m_next);
  prev->m_next = m_prev;
}

template<class T, class ALLOC>
void cgg251DCList<T,ALLOC>::push_front(const T& t) {
  ListEntry<T,ALLOC>* next = m_next;
  m_next = new(m_alloc) ListEntry<T,ALLOC>(t, next->m_prev, m_next);
  next->m_prev = m_next;
}

template<class T, class ALLOC>
void cgg251DCList<T,ALLOC>::remove(cgg251dclIterator<T,ALLOC>& iter) {
  delete_elem(iter.m_curr);
}

template<class T, class ALLOC>
void cgg251DCList<T,ALLOC>::remove_all() {
  ListEntry<T,ALLOC>* curr = m_next;
  ListEntry<T,ALLOC>* delElem;
  while (curr != (ListEntry<T,ALLOC>*) &m_prev) {
    delElem = curr;
    curr    = curr->m_next;
    delElem->Delete(m_alloc);
  }
  m_prev = (ListEntry<T,ALLOC>*) &m_prev;
  m_next = (ListEntry<T,ALLOC>*) &m_prev;
}

/*!
   class : cgg251DCSList

   description : implements a sorted double chained list, i.e. the list elements are sorted in
                 ascending order. 

   template arguments :
     T      Type of list elements. T must provide operator < and operator==
     ALLOC  Allocater used for memory allocation. Must provide allocate and deallocate
*/
template<class T,class ALLOC>
class
cgg251DCSList {
  public :
/*!
  function: cgg251DCSList
  description : constructor

  arguments : pointer to an allocator instance
*/
  EXPLICIT_MEO00 cgg251DCSList(ALLOC* a = NULL) : m_list(a) {}
/*!
  function: empty
  description: returns true, if the list is empty; false otherwise
  arguments : none
*/
  bool empty() { return m_list.empty(); }
/*!
  function: insert
  description: inserts an element into the list
  arguments:
  t[in] : element to be inserted
*/
  void insert(T& t);
/*!
  function: begin
  description: returns an iterator pointing to the first list element
  arguments:
    none
*/
  cgg251dclIterator<T,ALLOC> begin() {
    return m_list.begin();
  }
/*!
  function: remove
  description: removes an element identified by an iterator from the list
  arguments:
    iter[in] : iterator pointing to the Element to be removed
*/
  void remove(cgg251dclIterator<T,ALLOC>& iter) { m_list.remove(iter); }
/*!
  function: remove_all
  description: removes all list elements. After the call the list is empty.
  arguments : none
*/
  void remove_all() { m_list.remove_all(); }
  private :
    cgg251DCList<T,ALLOC> m_list;
};
/*! endclass : cgg251DCSList */

template<class T, class ALLOC>
void cgg251DCSList<T,ALLOC>::insert(T& t) {
  cgg251dclIterator<T,ALLOC> iter = m_list.begin();
  while (iter) {
    if (t < *iter()) {
      break;
    }
    ++iter;
  }
  m_list.insert_before(t, iter);
}

/*!
   class : cgg251DCUList

   description : implements a unique sorted double chained list, i.e. the list elements are sorted in
                 ascending order and must be unique . 

   template arguments :
     T      Type of list elements. T must provide operator < and operator==
     ALLOC  Allocater used for memory allocation. Must provide allocate and deallocate
   
*/
template<class T,class ALLOC>
class
cgg251DCUList {
  public :
/*!
  function: cgg251DCUList
  description : constructor

  arguments : pointer to an allocator instance
*/
  EXPLICIT_MEO00 cgg251DCUList(ALLOC* a = NULL) : m_list(a) {}
/*!
  function: advise_allocator
  description: tells the list to use an allocator after the list has been constructed
  arguments : 
    a[in] pointer to allocator instance
*/
  void advise_allocator(ALLOC* a) { m_list.advise_allocator(a); }
/*!
  function: empty
  description: returns true, if the list is empty; false otherwise
  arguments : none
*/
  bool empty() { return m_list.empty(); }
/*!
  function: insert
  description: inserts an element into the list
  arguments:
  t[in] : element to be inserted
  return value: e_ok, if successful, e_duplicate_key; if the element is already stored in the list
*/
  tgg00_BasisError insert(T& t);
/*!
  function: remove
  description: removes an element from the list
  arguments:
    t[in] : the Element to be removed
  return value: e_ok, if successful; e_key_not_found, if the element is not stored in the list
*/
  tgg00_BasisError remove(T& t);
/*!
  function: begin
  description: returns an iterator pointing to the first list element
  arguments: none
*/
  cgg251dclIterator<T,ALLOC> begin() {
    return m_list.begin();
  }
/*!
  function: remove
  description: removes an element identified by an iterator from the list
  arguments:
    iter[in] : iterator pointing to the Element to be removed
*/
  void remove(cgg251dclIterator<T,ALLOC>& iter) { m_list.remove(iter); }
/*!
  function: remove_all
  description: removes all list elements. After the call the list is empty.
  arguments : none
*/
  void remove_all() { m_list.remove_all(); }
  private :
    cgg251DCList<T,ALLOC> m_list;
};
/*! endclass : cgg251DCUList */

template<class T, class ALLOC>
tgg00_BasisError cgg251DCUList<T,ALLOC>::insert(T& t) {
  cgg251dclIterator<T,ALLOC> iter = m_list.begin();
  while (iter) {
    if (t < *iter()) {
      break;
    }
    else {
      if (t == *iter()) {
        return e_duplicate_key;
      }
    }
    ++iter;
  }
  m_list.insert_before(t, iter);
  return e_ok;
}

template<class T, class ALLOC>
tgg00_BasisError cgg251DCUList<T,ALLOC>::remove(T& t) {
  for (cgg251dclIterator<T,ALLOC> iter = m_list.begin(); iter; ++iter) {
    if (t < *iter()) {
      return e_key_not_found;
    }
    else {
      if (t == *iter()) {
        m_list.remove(iter);
        break;
      }
    }
  }
  return e_ok;
}

#endif
