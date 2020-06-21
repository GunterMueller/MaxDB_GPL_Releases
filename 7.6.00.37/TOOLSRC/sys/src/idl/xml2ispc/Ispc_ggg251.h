/*!******************************************************************************

  module:       Ispc_ggg251.h

  -----------------------------------------------------------------------------

  responsible:  ThomasA

  special area: Implementation of basic algorithms
  description:  Double Chained List

  -----------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (C) 2000 SAP AG

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

********************************************************************************/


#ifndef ISPC_GGG251_H
#define ISPC_GGG251_H


#define e_key_not_found 1200


/*******************************************************************************/
/*     ListEntry, cgg251dclIterator                                            */
/*******************************************************************************/

template< class T, class ALLOC > class cgg251DCList;

template< class T, class ALLOC > class cgg251dclIterator; 

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
class ListEntry
{
  friend class cgg251DCList< T, ALLOC >;
  friend class cgg251dclIterator< T, ALLOC >; 
private:
  /* Constructors */
  ListEntry(const T & t) : m_prev(NULL), m_next(NULL), m_item(t) {};
  ListEntry( const T & t, ListEntry< T, ALLOC > * p, ListEntry< T, ALLOC > * n) : m_prev(p), m_next(n), m_item(t) {};
  /* Memory management */
  void * operator new( size_t sz, ALLOC * a ) {
    return a->allocate(sz);
  }
#if defined(PLACEMENT_DELETE_MEO00)
  void operator delete( void * p, ALLOC * a ) {};
#endif
  void Delete(ALLOC * a) {
    a->deallocate(this);
  }
  /* Members */
  ListEntry< T, ALLOC > * m_prev;
  ListEntry< T, ALLOC > * m_next;
  T                       m_item;
};

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC > 
class cgg251dclIterator
{
  friend class cgg251DCList< T, ALLOC >;
public:
  /* Constructor */
  cgg251dclIterator( ListEntry< T, ALLOC > * curr, cgg251DCList< T, ALLOC > * list ) : m_curr(curr), m_list(list) {};
  /* Operators */
  operator bool() {
    return (m_curr != (ListEntry< T, ALLOC > *)&m_list->m_prev);
  }
  void operator++() {
    m_curr = m_curr->m_next;
  }
  void operator++(int) {
    m_curr = m_curr->m_next;
  }
  T * operator()() {
    return &m_curr->m_item;
  }
  /* Other */
  typedef void * VoidPtr;
  void Dump( VoidPtr & thisPtr, VoidPtr & nextPtr, VoidPtr & prevPtr) {
    thisPtr = m_curr; 
    nextPtr = m_curr->m_next;
    prevPtr = m_curr->m_prev;
  }
private:
  ListEntry< T, ALLOC > *    m_curr;
  cgg251DCList< T, ALLOC > * m_list;
};


/*******************************************************************************/
/*     cgg251DCList (double chained list)                                      */
/*******************************************************************************/

template< class T, class ALLOC >
class cgg251DCList
{
public:
  /* Constructor */
  EXPLICIT_MEO00 cgg251DCList(ALLOC * a = NULL)
  /* Destructor */
   : m_prev((ListEntry< T, ALLOC > *)&m_prev), m_next((ListEntry< T, ALLOC > *)&m_prev), m_alloc(a) {};
  /* Destructor */
  ~cgg251DCList();
  /* Allocator */
  void advise_allocator(ALLOC * a) {m_alloc = a;}
  /* Iterator */
  cgg251dclIterator< T, ALLOC > begin() {
    cgg251dclIterator< T, ALLOC > iter( m_next, this );
    return iter;
  }
  /* Accessors */
  T &         back() {return m_prev->m_item;}
  const T &   back() const {return m_prev->m_item;}
  bool        empty() {return (m_next == (ListEntry< T, ALLOC > *)&m_prev);}
  T &         front() { return m_next->m_item; }
  const T &   front() const {return m_next->m_item;}
  void        insert_before( T & t, cgg251dclIterator< T, ALLOC > & i );
  void        insert_after( T & t, cgg251dclIterator< T, ALLOC > & i );
  void        pop_back();
  void        pop_front();
  void        push_back(const T & t);
  void        push_front(const T & t);
  inline void remove(cgg251dclIterator< T, ALLOC > & i);
  inline void remove_all();
  /* Members */
  ListEntry< T, ALLOC > * m_prev;
  ListEntry< T, ALLOC > * m_next;
  ALLOC *                 m_alloc;
private:
  void delete_elem(ListEntry< T, ALLOC > *);
};

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
cgg251DCList< T, ALLOC >::~cgg251DCList() {
  ListEntry< T, ALLOC > * delElem;
  cgg251dclIterator< T, ALLOC > iter = begin();
  while (iter) {
    delElem = iter.m_curr;
    ++iter;
    delElem->Delete(m_alloc);
  }
}

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
inline void cgg251DCList< T, ALLOC >::delete_elem(ListEntry< T, ALLOC > * p) {
  p->m_next->m_prev = p->m_prev;
  p->m_prev->m_next = p->m_next;
  p->Delete(m_alloc);
}

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
void cgg251DCList< T, ALLOC >::insert_before( T & t, cgg251dclIterator< T, ALLOC > & iter ) {
  ListEntry< T, ALLOC > * curr = iter.m_curr;
  ListEntry< T, ALLOC > * prev = curr->m_prev;
  curr->m_prev = new(m_alloc) ListEntry< T, ALLOC >( t, prev, prev->m_next );
  prev->m_next = curr->m_prev;
}

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
void cgg251DCList< T, ALLOC >::insert_after( T & t, cgg251dclIterator< T, ALLOC > & iter ) {
  ListEntry< T, ALLOC > * curr = iter.m_curr;
  ListEntry< T, ALLOC > * next = curr->m_next;
  curr->m_next = new(m_alloc) ListEntry< T, ALLOC >( t, next->m_prev, curr->m_next );
  next->m_prev = curr->m_next;
}

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
void cgg251DCList< T, ALLOC >::pop_back() {
  ListEntry< T, ALLOC > * delElem = m_prev;
  m_prev = delElem->m_prev;
  m_prev->m_next = delElem->m_next;
  delElem->Delete(m_alloc);
}

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
void cgg251DCList< T, ALLOC >::pop_front() {
  delete_elem(m_next);
}

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
void cgg251DCList< T, ALLOC >::push_back(const T & t) {
  ListEntry< T, ALLOC > * prev = m_prev;
  m_prev = new(m_alloc) ListEntry< T, ALLOC >( t, prev, prev->m_next );
  prev->m_next = m_prev;
}

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
void cgg251DCList< T, ALLOC >::push_front(const T & t) {
  ListEntry< T, ALLOC > * next = m_next;
  m_next = new(m_alloc) ListEntry< T, ALLOC >( t, next->m_prev, m_next );
  next->m_prev = m_next;
}

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
void cgg251DCList< T, ALLOC >::remove(cgg251dclIterator< T, ALLOC > & iter) {
  delete_elem(iter.m_curr);
}

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
void cgg251DCList< T, ALLOC >::remove_all() {
  ListEntry< T, ALLOC > * curr = m_next;
  ListEntry< T, ALLOC > * delElem;
  while (curr != (ListEntry< T, ALLOC > *)&m_prev) {
    delElem = curr;
    curr    = curr->m_next;
    delElem->Delete(m_alloc);
  }
  m_prev = (ListEntry< T, ALLOC > *)&m_prev;
  m_next = (ListEntry< T, ALLOC > *)&m_prev;
}


/*******************************************************************************/
/*     cgg251DCSList (sorted double chained list)                              */
/*******************************************************************************/

template< class T, class ALLOC >
class cgg251DCSList
{
public:
  /* Constructor */
  EXPLICIT_MEO00 cgg251DCSList(ALLOC * a = NULL) : m_list(a) {};
  /* Iterator */
  cgg251dclIterator< T, ALLOC > begin() {
    return m_list.begin();
  }
  /* Accessors */
  bool empty() {return m_list.empty();}
  void insert(T & t);
  void remove(cgg251dclIterator< T, ALLOC > & iter) {m_list.remove(iter);}
  void remove_all() {m_list.remove_all();}
private:
  cgg251DCList< T, ALLOC > m_list;
};

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
void cgg251DCSList< T, ALLOC >::insert(T & t) {
  cgg251dclIterator< T, ALLOC > iter = m_list.begin();
  while (iter) {
    if (t < *iter()) {
      break;
    }
    ++iter;
  }
  m_list.insert_before( t, iter );
}


/*******************************************************************************/
/*     cgg251DCUList (unique sorted double chained list)                       */
/*******************************************************************************/

template< class T, class ALLOC >
class cgg251DCUList
{
public:
  /* Constructor */
  EXPLICIT_MEO00 cgg251DCUList(ALLOC * a = NULL) : m_list(a) {};
  /* Allocator */
  void advise_allocator(ALLOC * a) {m_list.advise_allocator(a);}
  /* Iterator */
  cgg251dclIterator<T,ALLOC> begin() {
    return m_list.begin();
  }
  /* Accessors */
  bool empty() {return m_list.empty();}
  tgg00_BasisError insert(T & t);
  tgg00_BasisError remove(T & t);
  void remove(cgg251dclIterator< T, ALLOC > & iter) {m_list.remove(iter);}
  void remove_all() {m_list.remove_all();}
private:
  cgg251DCList< T, ALLOC > m_list;
};

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
tgg00_BasisError cgg251DCUList< T, ALLOC >::insert(T & t) {
  cgg251dclIterator< T, ALLOC > iter = m_list.begin();
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
  m_list.insert_before( t, iter );
  return e_ok;
}

/*-----------------------------------------------------------------------------*/

template< class T, class ALLOC >
tgg00_BasisError cgg251DCUList< T, ALLOC >::remove(T & t) {
  for ( cgg251dclIterator< T, ALLOC > iter = m_list.begin(); iter; ++iter ) {
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


#endif /* ISPC_GGG251_H */

