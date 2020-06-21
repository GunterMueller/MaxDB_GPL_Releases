/*!******************************************************************************

  module:       Ispc_ggg250.h

  -----------------------------------------------------------------------------

  responsible:  ThomasA

  special area: Implementation of basic algorithms
  description:  AVL Tree

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


#ifndef ISPC_GGG250_H
#define ISPC_GGG250_H


#include <stdio.h>
#include <memory.h>
#include <malloc.h>
//#include "ggg00.h"

/*+++++++++++++++++++ FF +++++++++++++++++++*/

#define e_ok              0
#define e_duplicate_key 530
typedef short tgg00_BasisError;

#if !defined(AIX) || __IBMCPP__ >= 500
/* WIN32, SUN, SOLARIS, HPUX, DEC/OSF1, SNI/NMP, LINUX */
#define TYPENAME_MEO00 typename
#define EXPLICIT_MEO00 explicit
#else
#define TYPENAME_MEO00
#define EXPLICIT_MEO00
#endif
#define REFCAST_MEO00(type_) *(type_ *)

#if !defined(HPUX) && !defined(AIX) && !defined(LINUX)
#define PLACEMENT_DELETE_MEO00
#endif

#if !defined(HPUX)
#define TEMPLATE_CLASS_MEO00
#define TEMPLATE_TYPE_MEO00
#else
#define TEMPLATE_CLASS_MEO00 class
#define TEMPLATE_TYPE_MEO00  typename
#endif

/*------------------- FF -------------------*/

#if defined(_WIN32)
#pragma warning ( disable : 4786 )
#endif


/*******************************************************************************/
/*     cgg250Comparator                                                        */
/*******************************************************************************/

template<class T>
class cgg250Comparator
{
public :
  int Compare( const T & t1, const T & t2 ) {
    if (t1 < t2) {
      return -1;
    }
    if (t2 < t1) {
      return 1;
    }
    return 0;
  }
};


/*******************************************************************************/
/*     cgg250AvlNode                                                           */
/*******************************************************************************/

template< class KEY, class CMP, class ALLOC >
class cgg250AvlNode
{
  typedef cgg250AvlNode< KEY, CMP, ALLOC > * tgg250AvlNodePtr;
public:
  /* Constructor */
  cgg250AvlNode(const KEY & k);
  /* Accessors */
  inline int &              Balance();
  inline bool               Balanced() const;
  void                      Delete_RR( tgg250AvlNodePtr & p, bool & balance );
  void                      Delete_LL( tgg250AvlNodePtr & p, bool & balance );
  inline const KEY *        GetKey() const;
  inline bool               LeftDeeper() const;
  inline tgg250AvlNodePtr & LeftSon();
  inline bool               RightDeeper() const;
  inline tgg250AvlNodePtr & RightSon();
  void                      Rotate_LL(tgg250AvlNodePtr & p);
  void                      Rotate_LR(tgg250AvlNodePtr & p);
  void                      Rotate_RL(tgg250AvlNodePtr & p);
  void                      Rotate_RR(tgg250AvlNodePtr & p);
  inline void               SetBalanced();
  inline void               SetRightDeeper();
  inline void               SetLeftDeeper();
  /* Memory management */
  inline void *             operator new( size_t size, ALLOC * a );
#if defined(PLACEMENT_DELETE_MEO00)
  inline void               operator delete( void * p, ALLOC * a );
#endif
  inline void               deleteSelf(ALLOC * a);
protected:
  KEY              m_key;
  tgg250AvlNodePtr m_leftSon;
  tgg250AvlNodePtr m_rightSon;
  int              m_balance;
};

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
cgg250AvlNode< KEY, CMP, ALLOC >::cgg250AvlNode(const KEY & k)
: m_key(k)
, m_leftSon(NULL)
, m_rightSon(NULL)
, m_balance(0)
{
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline int & cgg250AvlNode< KEY, CMP, ALLOC >::Balance() {
  return m_balance;
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline bool cgg250AvlNode< KEY, CMP, ALLOC >::Balanced() const {
  return (0 == m_balance);
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
void cgg250AvlNode< KEY, CMP, ALLOC >::Delete_RR( tgg250AvlNodePtr & p, bool & balance ) {
  p->RightSon() = LeftSon();
  LeftSon()     = p;
  if (this->Balanced()) {
    p->SetRightDeeper();
    this->SetLeftDeeper();
    balance = false;
  }
  else {
    this->SetBalanced();
    p->SetBalanced();
  }
  p = this;
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
void cgg250AvlNode< KEY, CMP, ALLOC >::Delete_LL( tgg250AvlNodePtr & p, bool & balance) {
  p->LeftSon() = RightSon();
  RightSon()   = p;
  if (this->Balanced()) {
    p->SetLeftDeeper();
    this->SetRightDeeper();
    balance = false;
  }
  else {
    this->SetBalanced();
    p->SetBalanced();
  }
  p = this;
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline const KEY * cgg250AvlNode< KEY, CMP, ALLOC >::GetKey() const {
  return &m_key;
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline bool cgg250AvlNode< KEY, CMP, ALLOC >::LeftDeeper() const {
  return (-1 == m_balance);
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline TYPENAME_MEO00 cgg250AvlNode< KEY, CMP, ALLOC >::tgg250AvlNodePtr & cgg250AvlNode< KEY, CMP, ALLOC >::LeftSon() {
  return m_leftSon;
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline bool cgg250AvlNode< KEY, CMP, ALLOC >::RightDeeper() const {
  return (1 == m_balance);
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline TYPENAME_MEO00 cgg250AvlNode< KEY, CMP, ALLOC >::tgg250AvlNodePtr & cgg250AvlNode< KEY, CMP, ALLOC >::RightSon() {
  return m_rightSon;
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
void cgg250AvlNode< KEY, CMP, ALLOC >::Rotate_LL(tgg250AvlNodePtr & p) {
  p->LeftSon() = RightSon();
  RightSon()   = p;
  p->SetBalanced();
  p = this;
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
void cgg250AvlNode< KEY, CMP, ALLOC >::Rotate_LR(tgg250AvlNodePtr & p) {
  tgg250AvlNodePtr right = RightSon();
  RightSon()             = right->LeftSon();
  right->LeftSon()       = this;
  p->LeftSon()           = right->RightSon();
  right->RightSon()      = p;
  if (right->LeftDeeper()) {
    p->SetRightDeeper();
  }
  else {
    p->SetBalanced();
  }
  if (right->RightDeeper()) {
    this->SetLeftDeeper();
  }
  else {
    this->SetBalanced();
  }
  p = right;
  p->SetBalanced();
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
void cgg250AvlNode< KEY, CMP, ALLOC >::Rotate_RL(tgg250AvlNodePtr & p) {
  tgg250AvlNodePtr left = LeftSon();
  LeftSon()             = left->RightSon();
  left->RightSon()      = this;
  p->RightSon()         = left->LeftSon();
  left->LeftSon()       = p;
  if (left->RightDeeper()) {
    p->SetLeftDeeper();
  }
  else {
    p->SetBalanced();
  }
  if (left->LeftDeeper()) {
    this->SetRightDeeper();
  }
  else {
    this->SetBalanced();
  }
  p = left;
  p->SetBalanced();
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
void cgg250AvlNode< KEY, CMP, ALLOC >::Rotate_RR(tgg250AvlNodePtr & p) {
  p->RightSon() = LeftSon();
  LeftSon()     = p;
  p->SetBalanced();
  p = this;
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline void cgg250AvlNode< KEY, CMP, ALLOC >::SetBalanced() {
  m_balance = 0;
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline void cgg250AvlNode< KEY, CMP, ALLOC >::SetRightDeeper() {
  m_balance = 1;
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline void cgg250AvlNode< KEY, CMP, ALLOC >::SetLeftDeeper() {
  m_balance = -1;
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline void * cgg250AvlNode< KEY, CMP, ALLOC >::operator new( size_t size, ALLOC * a ) {
  return a->allocate(size);
}

/*-----------------------------------------------------------------------------*/

#if defined(PLACEMENT_DELETE_MEO00)
template< class KEY, class CMP, class ALLOC >
inline void cgg250AvlNode< KEY, CMP, ALLOC >::operator delete( void * p, ALLOC * a ) {
}
#endif

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline void cgg250AvlNode< KEY, CMP, ALLOC >::deleteSelf(ALLOC * a) {
  this->m_key.~KEY();
  a->deallocate(this);
}


/*******************************************************************************/
/*     cgg250AvlInfoNode                                                       */
/*******************************************************************************/

template< class KEY, class INFO, class CMP, class ALLOC >
class cgg250AvlInfoNode
: public cgg250AvlNode< KEY, CMP, ALLOC >
{
public:
  /* Constructor */
  cgg250AvlInfoNode(const KEY & k) : cgg250AvlNode< KEY, CMP, ALLOC >(k) {};
  /* Memory management */
#if defined(PLACEMENT_DELETE_MEO00)
  void operator delete( void * p, ALLOC * a ){};
#endif
  /* Member */
  INFO m_info;
};


/*******************************************************************************/
/*     cgg250AvlBase                                                           */
/*******************************************************************************/

template< class NODE, class KEY, class CMP, class ALLOC >
class cgg250AvlBase
{
#if defined(AIX) || defined(SUN) || defined(SOLARIS) || defined(HPUX)
public:
#endif
  typedef cgg250AvlNode< KEY, CMP, ALLOC > * tgg250AvlNodePtr;
public:
  cgg250AvlBase();
  cgg250AvlBase( CMP * c, ALLOC * a );
  virtual ~cgg250AvlBase();
  void   AdviseAllocator(ALLOC * a);
  void   AdviseCompare(CMP * c);
  bool   CheckTree();
  int    Delete(const KEY & key);
  void   DeleteAll();
  NODE * FindNode(const KEY & key) const;
  NODE * InsertIntoTree( const KEY & key, tgg00_BasisError & rc );
#if !defined(AIX) && !defined(SUN) && !defined(SOLARIS) && !defined(HPUX)
private:
#endif
#define STACK_SIZE 128
  class Stack {
  public:
    int              m_bottom;
    int              m_top;
    tgg250AvlNodePtr m_stack[STACK_SIZE];
    Stack() : m_bottom(0), m_top(0) {};
    void Push(tgg250AvlNodePtr p) {
      ++m_top;
      if (STACK_SIZE == m_top) {
        m_top = 0;
      }
      m_stack[m_top] = p;
      if (m_top == m_bottom) {
        ++m_bottom;
        if (STACK_SIZE == m_bottom) {
          m_bottom = 0;
        }
      }
    }
    bool IsEmpty() {
      return (m_bottom == m_top);
    }
    tgg250AvlNodePtr Pop() {
      if (!IsEmpty()) {
        tgg250AvlNodePtr p = m_stack[m_top];
        --m_top;
        if (m_top < 0) {
          m_top = STACK_SIZE - 1;
        }
        return p;
      }
      return NULL;
    }
  };
private:
  int    CheckSubTree(tgg250AvlNodePtr p);
  void   BalanceLeft( tgg250AvlNodePtr & p, bool & balance );
  void   BalanceRight( tgg250AvlNodePtr & p, bool & balance );
  void   Del( tgg250AvlNodePtr & p, tgg250AvlNodePtr & deleted, bool & balance );
  void   DeleteBalanceLeft( tgg250AvlNodePtr & p, bool & balance );
  void   DeleteBalanceRight( tgg250AvlNodePtr & p, bool & balance );
  int    DeleteNode( CMP * cmp, const KEY & key, tgg250AvlNodePtr & p, bool & doBalance);
  NODE * InsertNode( const KEY & key, tgg250AvlNodePtr & p, bool & balance, tgg00_BasisError & rc );
  void   DeleteSubtree(tgg250AvlNodePtr p);
  tgg250AvlNodePtr m_root;
  CMP *            m_cmp;
  ALLOC *          m_allocator;
public:
  class Iterator {
  public:
    Stack            m_stack;
    tgg250AvlNodePtr m_root;
    Iterator() : m_root(NULL) {};
    Iterator(tgg250AvlNodePtr p) : m_root(p) {};
    Iterator(const Iterator & iter) : m_root(iter.m_root) {
      m_stack.m_bottom = iter.m_stack.m_bottom;
      m_stack.m_top    = iter.m_stack.m_top;
      memcpy( &m_stack.m_stack, &iter.m_stack.m_stack, sizeof(m_stack.m_stack) );
    }
    operator bool() {
      return (!m_stack.IsEmpty());
    }
    void operator++() {
      if (m_stack.IsEmpty()) {
      }
      tgg250AvlNodePtr p = m_stack.Pop();
      p = REINTERPRET_CAST(tgg250AvlNodePtr, p->RightSon());
      while (NULL != p) {
        m_stack.Push(p);
        p = REINTERPRET_CAST(tgg250AvlNodePtr, p->LeftSon());
      }
    }
    void operator++(int i) {
      operator++();
    }
    const KEY * operator()() {
      return GetNodePtr()->GetKey();
    }
    tgg250AvlNodePtr GetNodePtr() {
      return m_stack.m_stack[m_stack.m_top];
    }
  };
  Iterator First();
};

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
cgg250AvlBase< NODE, KEY, CMP, ALLOC >::cgg250AvlBase()
: m_root(NULL)
, m_allocator(NULL)
, m_cmp(NULL)
{
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
cgg250AvlBase< NODE, KEY, CMP, ALLOC >::cgg250AvlBase( CMP * c, ALLOC * a)
: m_root(NULL)
, m_allocator(a)
, m_cmp(c)
{
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
void cgg250AvlBase< NODE, KEY, CMP, ALLOC >::AdviseAllocator(ALLOC * a) {
  m_allocator = a;
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
void cgg250AvlBase< NODE, KEY, CMP, ALLOC >::AdviseCompare(CMP * c) {
  m_cmp = c;
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
cgg250AvlBase< NODE, KEY, CMP, ALLOC >::~cgg250AvlBase() {
  DeleteAll();
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
void cgg250AvlBase< NODE, KEY, CMP, ALLOC >::BalanceLeft( tgg250AvlNodePtr & p, bool & balance) {
  if (p->RightDeeper()) {
    p->SetBalanced();
    balance = false;
  }
  else {
    if (p->Balanced()) {
      p->SetLeftDeeper();
    }
    else {
      tgg250AvlNodePtr left = p->LeftSon();
      if (left->LeftDeeper()) {
        left->Rotate_LL (p);
      }
      else {
        left->Rotate_LR(p);
      }
      p->SetBalanced();
      balance = false;
    }
  }
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
void cgg250AvlBase< NODE, KEY, CMP, ALLOC >::BalanceRight( tgg250AvlNodePtr & p, bool & balance) {
  if (p->LeftDeeper()) {
    p->SetBalanced();
    balance = false;
  }
  else {
    if (p->Balanced()) {
      p->SetRightDeeper();
    }
    else {
      tgg250AvlNodePtr right = p->RightSon();
      if (right->RightDeeper()) {
        right->Rotate_RR (p);
      }
      else {
        right->Rotate_RL(p);
      }
      p->SetBalanced();
      balance = false;
    }
  }
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
bool cgg250AvlBase< NODE, KEY, CMP, ALLOC >::CheckTree () {
  return (CheckSubTree(m_root) >= 0);
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
int cgg250AvlBase< NODE, KEY, CMP, ALLOC >::CheckSubTree(tgg250AvlNodePtr p) {
  if (0 == p) {
    return 0;
  }
  int l = CheckSubTree(p->LeftSon());
  int r = CheckSubTree(p->RightSon());
  if (l < r) {
    if (!p->RightDeeper()) {
      return -1;
    }
    return (r + 1);
  }
  if (l > r) {
    if (!p->LeftDeeper()) {
      return -1;
    }
    return (l + 1);
  }
  if (!p->Balanced()) {
    return -1;
  }
  return (l + 1);
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
void cgg250AvlBase< NODE, KEY, CMP, ALLOC >::Del( tgg250AvlNodePtr & p, tgg250AvlNodePtr & deletedNode, bool& balance) {
  if (NULL != p->RightSon()) {
    Del( p->RightSon(), deletedNode, balance );
    if (balance) {
      DeleteBalanceRight( p, balance );
    }
  }
  else {
    deletedNode = p;
    p           = p->LeftSon();
    balance     = true;
  }
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
int cgg250AvlBase< NODE, KEY, CMP, ALLOC >::Delete(const KEY & key) {
  bool balance = false;
  return DeleteNode( m_cmp, key, *REINTERPRET_CAST( tgg250AvlNodePtr *, &m_root ), balance );
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
void cgg250AvlBase< NODE, KEY, CMP, ALLOC >::DeleteAll() {
  DeleteSubtree(m_root);
  m_root = NULL;
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
void cgg250AvlBase< NODE, KEY, CMP, ALLOC >::DeleteBalanceLeft( tgg250AvlNodePtr & p, bool & balance ) {
  if (p->LeftDeeper()) {
    p->SetBalanced();
  }
  else {
    if (p->Balanced()) {
      p->SetRightDeeper();
      balance = false;
    }
    else {
      tgg250AvlNodePtr right = p->RightSon();
      if (!right->LeftDeeper()) {
        right->Delete_RR (p, balance);
      }
      else {
        right->Rotate_RL(p);
      }
    }
  }
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
void cgg250AvlBase< NODE, KEY, CMP, ALLOC >::DeleteBalanceRight( tgg250AvlNodePtr & p, bool & balance ) {
  if (p->RightDeeper()) {
    p->SetBalanced();
  }
  else {
    if (p->Balanced()) {
      p->SetLeftDeeper();
      balance = false;
    }
    else {
      tgg250AvlNodePtr left = p->LeftSon();
      if (!left->RightDeeper()) {
        left->Delete_LL (p, balance);
      }
      else {
        left->Rotate_LR(p);
      }
    }
  }
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
int cgg250AvlBase< NODE, KEY, CMP, ALLOC >::DeleteNode( CMP * cmp, const KEY & key, tgg250AvlNodePtr & p, bool & balance) {
  int rc = 0;
  if (NULL == p) {
    rc      = -2;
    balance = false;
  }
  else {
    switch (cmp->Compare(*p->GetKey(), key)) {
      case 1 : {
        rc = DeleteNode( cmp, key, p->LeftSon(), balance );
        if (balance) {
          DeleteBalanceLeft( p, balance );
        }
        break;
      }
      case -1 : {
        rc = DeleteNode( cmp, key, p->RightSon(), balance );
        if (balance) {
          DeleteBalanceRight( p, balance );
        }
        break;
      }
      case 0 : {
        tgg250AvlNodePtr curr = p;
        if (NULL == curr->RightSon()) {
          p       = curr->LeftSon();
          balance = true;
        }
        else {
          if (NULL == curr->LeftSon()) {
            p       = curr->RightSon();
            balance = true;
          }
          else {
            tgg250AvlNodePtr deleted;
            Del( curr->LeftSon(), deleted, balance );
            deleted->LeftSon()  = p->LeftSon();
            deleted->RightSon() = p->RightSon();
            deleted->Balance()  = p->Balance();
            p = deleted;
            if (balance) {
              DeleteBalanceLeft( p, balance);
            }
          }
        }
        ((NODE*)curr)->deleteSelf(m_allocator);
        break;
      }
    }
  }
  return rc;
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
void cgg250AvlBase< NODE, KEY, CMP, ALLOC >::DeleteSubtree(tgg250AvlNodePtr p) {
  if (NULL != p) {
    DeleteSubtree(p->LeftSon());
    DeleteSubtree(p->RightSon());
    p->deleteSelf(m_allocator);
  }
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
NODE * cgg250AvlBase< NODE, KEY, CMP, ALLOC >::InsertIntoTree( const KEY & key, tgg00_BasisError & rc) {
  bool dummy = false;
  rc = e_ok;
  return InsertNode( key, m_root, dummy, rc );
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
TYPENAME_MEO00 cgg250AvlBase< NODE, KEY, CMP, ALLOC >::Iterator cgg250AvlBase< NODE, KEY, CMP, ALLOC >::First() {
  Iterator iter(m_root);
  tgg250AvlNodePtr p(m_root);
  while (NULL != p) {
    iter.m_stack.Push(p);
    p = p->LeftSon();
  }
  return iter;
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
NODE * cgg250AvlBase< NODE, KEY, CMP, ALLOC >::InsertNode( const KEY & key, tgg250AvlNodePtr & p, bool & balance, tgg00_BasisError & rc) {
  tgg250AvlNodePtr pInserted = NULL;
  if (NULL == p) {
    p         = new(m_allocator) NODE(key);
    pInserted = p;
    balance   = true;
  }
  else {
    switch (m_cmp->Compare(*p->GetKey(), key)) {
      case 1 : {
        pInserted = InsertNode( key, p->LeftSon(), balance, rc );
        if (balance) {
          BalanceLeft( p, balance );
        }
        break;
      }
      case -1 : {
        pInserted = InsertNode( key, p->RightSon(), balance, rc );
        if (balance) {
          BalanceRight( p, balance );
        }
        break;
      }
      case 0 : {
        rc = e_duplicate_key;
        break;
      }
    }
  }
  return (NODE*)pInserted;
}

/*-----------------------------------------------------------------------------*/

template< class NODE, class KEY, class CMP, class ALLOC >
NODE * cgg250AvlBase< NODE, KEY, CMP, ALLOC >::FindNode(const KEY & key) const {
  tgg250AvlNodePtr curr = m_root;
  while (NULL != curr) {
    switch (m_cmp->Compare(*curr->GetKey(), key)) {
      case 1 : {
        curr = curr->LeftSon();
        break;
      }
      case -1 : {
        curr = curr->RightSon();
        break;
      }
      case 0 : {
        return REINTERPRET_CAST( NODE *, curr );
      }
    }
  }
  return NULL;
}


/*******************************************************************************/
/*     cgg250AvlTree                                                           */
/*******************************************************************************/

template< class KEY, class CMP, class ALLOC >
class cgg250AvlTree
: public cgg250AvlBase< cgg250AvlNode< KEY, CMP, ALLOC >, KEY, CMP, ALLOC >
{
public:
  cgg250AvlTree( CMP * c = NULL, ALLOC * a = NULL ) :
    cgg250AvlBase< cgg250AvlNode< KEY, CMP, ALLOC >, KEY, CMP, ALLOC >(c,a) {}
//  virtual ~cgg250AvlTree();
//  bool CheckTree();
//  int  Delete(const KEY & key);
  inline const KEY * Find(const KEY & key) const;
  inline tgg00_BasisError Insert(const KEY & key);
};

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline const KEY * cgg250AvlTree< KEY, CMP, ALLOC >::Find(const KEY & key) const {
  const cgg250AvlNode< KEY, CMP, ALLOC > * p = FindNode(key);
  if (NULL != p) {
    return p->GetKey();
  }
  else {
    return NULL;
  }
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class CMP, class ALLOC >
inline tgg00_BasisError cgg250AvlTree< KEY, CMP, ALLOC >::Insert(const KEY & key) {
  tgg00_BasisError rc;
  InsertIntoTree( key,rc );
  return rc;
}


/*******************************************************************************/
/*     CAvlInfoTreeIterator                                                    */
/*******************************************************************************/

template< class KEY, class INFO, class CMP, class ALLOC >
class CAvlInfoTreeIterator
: public cgg250AvlBase< cgg250AvlInfoNode< KEY, INFO, CMP, ALLOC >, KEY, CMP, ALLOC >::Iterator
{
public:
  INFO * GetInfo() {
    typedef cgg250AvlNode< KEY, CMP, ALLOC > * tgg250AvlNodePtr;
    tgg250AvlNodePtr curr = REINTERPRET_CAST( tgg250AvlNodePtr, this->GetNodePtr() );
    if (curr) {
      return &(((cgg250AvlInfoNode< KEY, INFO, CMP, ALLOC >*)curr)->m_info);
    }
    return NULL;
  }
};


/*******************************************************************************/
/*     cgg250AvlInfoTree                                                       */
/*******************************************************************************/

template< class KEY, class INFO, class CMP, class ALLOC >
class cgg250AvlInfoTree
: public cgg250AvlBase< cgg250AvlInfoNode< KEY, INFO, CMP, ALLOC >, KEY, CMP, ALLOC >
{
public:
  cgg250AvlInfoTree( CMP * c = NULL, ALLOC * a = NULL) :
    cgg250AvlBase< cgg250AvlInfoNode< KEY, INFO, CMP, ALLOC >, KEY, CMP, ALLOC >(c,a) {};
  inline INFO * Find(const KEY& key);
  inline INFO * Insert( const KEY & key, tgg00_BasisError & rc );
  typedef CAvlInfoTreeIterator< KEY, INFO, CMP, ALLOC > Iterator;
  Iterator First() {
#if defined(HPUX) || defined(LINUX)
    typename
#endif
    cgg250AvlBase< cgg250AvlInfoNode< KEY, INFO, CMP, ALLOC >, KEY, CMP, ALLOC >::Iterator iter =
      ((cgg250AvlBase< cgg250AvlInfoNode< KEY, INFO, CMP, ALLOC >, KEY, CMP, ALLOC >*)this)->First();
    return *REINTERPRET_CAST( Iterator *, &iter );
  }
};

/*-----------------------------------------------------------------------------*/

template< class KEY, class INFO, class CMP, class ALLOC >
inline INFO * cgg250AvlInfoTree< KEY, INFO, CMP, ALLOC >::Find(const KEY & key) {
  cgg250AvlInfoNode<KEY,INFO,CMP,ALLOC>* p = FindNode(key);
  if (NULL != p) {
    return &p->m_info;
  }
  else {
    return NULL;
  }
}

/*-----------------------------------------------------------------------------*/

template< class KEY, class INFO, class CMP, class ALLOC >
inline INFO * cgg250AvlInfoTree< KEY, INFO, CMP, ALLOC >::Insert( const KEY & key, tgg00_BasisError & rc ) {
  cgg250AvlInfoNode< KEY, INFO, CMP, ALLOC > * p = InsertIntoTree( key, rc );
  if (NULL != p) {
    return &p->m_info;
  }
  else {
    return NULL;
  }
}


#endif /* ISPC_GGG250_H */

