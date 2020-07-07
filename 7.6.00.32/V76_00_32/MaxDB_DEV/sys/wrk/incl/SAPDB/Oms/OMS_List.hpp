/*!
 * \file    OMS_List.hpp
 * \author  MarkusSi
 * \brief   Template class for a double chained list.
 */
/*



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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





*/

#ifndef OMS_LIST_HPP
#define OMS_LIST_HPP

class OmsObjectContainer;

/*! The entries which are chained must have two additional pointers at the
**  end, which can be used to store the pointers for the chain. (Alignment!)
**  The first pointer points to the previous object ('prev') and the second 
**  one to the next object ('next') in the list.
*/
template <class T>
class OMS_List
{
public:
  class Iter{
  private:
    T*           m_curr;
    OMS_List<T>* m_pList;

  public:
    /*--------------------------------------------------------*/
    Iter()
      : m_curr(NULL)
      , m_pList(NULL)
    {}
    void Reset(OMS_List<T>* pList)
    { 
      m_pList = pList;
      m_curr  = m_pList->m_head; 
    }
    void operator++()
    {
      if (m_curr != NULL){
        T** currChainPtr = m_pList->GetChainPtr(m_curr);
        m_curr = currChainPtr[1];
      }
    }
    void operator++(int)
    { this++; }
    void operator--()
    {
      if (m_curr != NULL){
        T** currChainPtr = m_pList->GetChainPtr(m_curr);
        m_curr = currChainPtr[0];
      }
    }
    void operator--(int)
    { this--; }
    T*   operator()()
    { return m_curr; }
    operator bool() const
    { return m_curr != NULL; }
  };

  /*--------------------------------------------------------*/

  OMS_List(size_t size); 

  bool IsEmpty() const;
  /*! Insert new entry at the beginning of the list */
  void Insert(T *p);
  /*! Removes the given object from the list */
  bool Remove(T *p);
  /*! Reset the structure to the empty state. The objects, which are stored in the
  **  structure are not deleted! */
  void SetEmpty();
  /*! This method computes the size of the list entry for a given object size.
  **  The computation considers the alignment and adds the size for two additional 
  **  pointers (prev and next).
  */
  static size_t GetListEntrySize(size_t size);

private:
  /*! For a given object determine the position where the pointers for the
  **  linkage can be found. (Considers the correct alignment for pointers)
  */
  T** GetChainPtr(T* p);

  /// Pointer to the first element in the list
  T      *m_head;
  /// Offset where the pointers for the linkage can be found
  size_t  m_offset;

  friend class OMS_List<T>::Iter;
};


/*-----------------------------------------------------------------------*/

template<class T>
OMS_List<T>::OMS_List(size_t size) 
: m_head(NULL)
{
  m_offset = OMS_List<T>::GetListEntrySize(size) - 2*sizeof(void*);
}

/*-----------------------------------------------------------------------*/

template<class T>
bool OMS_List<T>::IsEmpty() const
{
  return m_head == NULL;
}

/*-----------------------------------------------------------------------*/

template<class T>
void OMS_List<T>::SetEmpty()
{
  m_head   = NULL;
}

/*-----------------------------------------------------------------------*/

template<class T>
void OMS_List<T>::Insert(T *p)
{
  T** currChainPtr = GetChainPtr(p);
  currChainPtr[0] = NULL;      // curr->prev = NULL
  currChainPtr[1] = m_head;    // curr->next = head
  if (m_head) {
    T** headChainPtr = GetChainPtr(m_head);
    headChainPtr[0] = p;       // first->prev = curr
  }
  m_head = p;                  // head = curr
}

/*-----------------------------------------------------------------------*/

template<class T>
bool OMS_List<T>::Remove(T *p)
{
  if (m_head == NULL){
    // list is completely empty
    return true;
  }

#if defined(OMSTST)
  {
    // Check whether object is contained in the list
    T* pp = m_head;
    while (pp){
      if (pp == p){
        break;
      }
      T** ppp = GetChainPtr(pp);
      pp = ppp[1];
    }
    if (pp != p){
      // Object is not in list
      return true;
    }
  }
#endif

  T** currChainPtr = GetChainPtr(p);

  if (currChainPtr[0]) {
    // There is a predecessor in the list
    T** prevChainPtr = GetChainPtr(currChainPtr[0]);
    prevChainPtr[1] = currChainPtr[1];          // prev->next = curr->next
  } else if (m_head == p) {
    // The object is the first one in the list
    m_head = currChainPtr[1];                   // head = curr->next
  } else {
    // Error occurred. m_head is not set correctly.
    return true;
  }

  if (currChainPtr[1]) {
    // There is a successor in the list
    T** nextChainPtr = GetChainPtr(currChainPtr[1]);
    nextChainPtr[0] = currChainPtr[0];          // next->prev = curr->prev
  }
  
  // Initialize pointers of deleted object 
  currChainPtr[0] = currChainPtr[1] = NULL;     // curr->prev = curr->next = NULL
  
  return false;
}

/*-----------------------------------------------------------------------*/

template<class T>
size_t OMS_List<T>::GetListEntrySize(size_t size)
{
  return ((size + sizeof(void*) - 1) & ~(sizeof(void*) - 1)) + 2*sizeof(void*);
}

/*-----------------------------------------------------------------------*/

template<class T>
T** OMS_List<T>::GetChainPtr(T* p){
  return reinterpret_cast<OmsObjectContainer**>(reinterpret_cast<char*>(p) + m_offset);
}

/*-----------------------------------------------------------------------*/

#endif  // OMS_LIST_HPP
