/*!
  @file           IFRUtil_LinkedList.h
  @author         MarcoP
  @ingroup        IFR_Util_LinkedList
  @brief          Linked List
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#ifndef IFRUtil_LinkedList_H
#define IFRUtil_LinkedList_H

//#include "SAPDB/Interfaces/Runtime/IFR_Trace.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_Internals.h"
#include "Interfaces/Runtime/IFR_Types.h"                 // #ifdef __GNUC__ >= 4
#include "Interfaces/Runtime/Util/IFRUtil_RuntimeItem.h"  // #ifdef __GNUC__ >= 4

#ifdef IFR_NAMESPACE
namespace IFR_NAMESPACE {
#endif

/*!
@class          IFRUtil_LinkedList
@brief          This class implements a doubly linked list

template arguments<T>  type of the elements to be held in the list

*/
class IFRUtil_LinkedListItemBasis {
};

template <class T> class IFRUtil_LinkedListItem : public IFRUtil_LinkedListItemBasis
{
public:
  IFRUtil_LinkedListItem<T> *m_Next;
  IFRUtil_LinkedListItem<T> *m_Prev;

  IFRUtil_LinkedListItem():
  m_Next(0),
  m_Prev(0)
    {}
};


/*!
@class          IFRUtil_LinkedList
@brief          This class implements a doubly linked list

template arguments<T>  type of the elements to be held in the list

*/
class IFRUtil_RuntimeItem;

template <class T> class IFRUtil_LinkedList 
{
public:
	/*!
    @brief Constructor
    @param alloc [in] reference to raw allocator
    */
  IFRUtil_LinkedList (SQLDBC_Spinlock* lock);
	/// Destructs the list and deallocates all elements
    ~IFRUtil_LinkedList();
	/*!
    @brief sets the spinlock
    @param alloc [in] spinlock
    */
    void setLock (SQLDBC_Spinlock* lock);

    void deleteList (IFRUtil_RuntimeItem& runtimeItem);

	/// Checks whether list is empty, returns true if empty
    bool IsEmpty() const;
    /*!
    @brief          Inserts a new element before at the end of the list
    @param          elem [in] element to be inserted
    */
  void Insert (T& elem);

    /*!
    @brief          Removes the given element from the list
    @param          elem [in] element to be removed
    */
void Remove (T& elem, SQLDBC_Bool lock = SQLDBC_TRUE);
    /*!
    @brief          Returns the first element of the list
    @return         first elemet of the list or 0 if the list is empty
    */
    IFRUtil_LinkedListItem<T>* First();
    /*!
    @brief          Returns the last element of the list
    @return         last elemet of the list or 0 if the list is empty
    */
    IFRUtil_LinkedListItem<T>* Last();

    IFRUtil_LinkedListItem<T>* Next (const T& elem);
    IFRUtil_LinkedListItem<T>* Prev (const T& elem);
private:
    inline void lock() { if(m_spinlock) m_spinlock->lock(); }
    inline void unlock() { if(m_spinlock) m_spinlock->unlock(); }
    
	/// Pointer to first node in the list
    IFRUtil_LinkedListItem<T> m_Head;
    SQLDBC_Spinlock* m_spinlock;
};



/*!
  @brief Constructor
  @param alloc [in] reference to raw allocator
*/
template <class T>  
IFRUtil_LinkedList<T>::IFRUtil_LinkedList (SQLDBC_Spinlock* lock)
:m_spinlock(lock)
{
    m_Head.m_Next = &m_Head;
    m_Head.m_Prev = &m_Head;
}

/// Destructs the list and deallocates all elements
template <class T>  IFRUtil_LinkedList<T>::~IFRUtil_LinkedList()
{
    m_Head.m_Next = 0;
    m_Head.m_Prev = 0;
}
/*!
  @brief sets the spinlock
  @param alloc [in] spinlock
*/
template <class T>
void IFRUtil_LinkedList<T>::setLock (SQLDBC_Spinlock* lock){
    m_spinlock = lock;
}
template <class T>
    void IFRUtil_LinkedList<T>::deleteList (IFRUtil_RuntimeItem& runtimeItem){
//      DBUG_CONTEXT_METHOD_ENTER(IFRUtil_LinkedList, deleteList, runtime); 
    lock();
    while (! this->IsEmpty()){
        //IFR_SQL_TRACE << endl << "WARNING ITEM "
        //          << " [0x" << (void*)m_Head.m_Prev << "] NOT CLOSED" << endl;
        this->Remove( static_cast <T&> (*m_Head.m_Prev), SQLDBC_FALSE);  
    }
    unlock();
    if(m_spinlock) {
        runtimeItem.runtime.releaseSpinlock(runtimeItem.allocator, m_spinlock);
    }
    m_spinlock = 0;
}

/// Checks whether list is empty, returns true if empty
template <class T>  
bool IFRUtil_LinkedList<T>::IsEmpty() const
{ 
    return (m_Head.m_Next == &m_Head && m_Head.m_Prev == &m_Head); 
}

/*!
    @brief          Inserts a new element before at the end of the list
    @param          elem [in] element to be inserted
    */
  template <class T>
  void IFRUtil_LinkedList<T>::Insert (T& elem){
      lock();
    elem.m_Next = &m_Head;
    elem.m_Prev = m_Head.m_Prev;
    m_Head.m_Prev->m_Next = &elem;
    m_Head.m_Prev = &elem;
    unlock();
  }

    /*!
    @brief          Removes the given element from the list
    @param          elem [in] element to be removed
    */
template <class T>
void IFRUtil_LinkedList<T>::Remove (T& elem, SQLDBC_Bool lock)
{
    this->lock();
    elem.m_Next->m_Prev = elem.m_Prev;
    elem.m_Prev->m_Next = elem.m_Next;
    elem.m_Prev = 0;
    elem.m_Next = 0;
    unlock();
  }
    /*!
    @brief          Returns the first element of the list
    @return         first elemet of the list or 0 if the list is empty
    */
template <class T>
    IFRUtil_LinkedListItem<T>* IFRUtil_LinkedList<T>::First()
    {   
        lock(); 
        if (IsEmpty()) {
         unlock(); 
         return 0;
       }
        IFRUtil_LinkedListItem<T>*  erg = m_Head.m_Next;
        unlock(); 
        return erg;
    }
    /*!
    @brief          Returns the last element of the list
    @return         last elemet of the list or 0 if the list is empty
    */
  template <class T>
    IFRUtil_LinkedListItem<T>* IFRUtil_LinkedList<T>::Last()
    { 
       lock(); 
       if (IsEmpty()) {
         unlock(); 
         return 0;
       }
       IFRUtil_LinkedListItem<T>*  erg = m_Head.m_Prev;
       unlock(); 
       return erg;
    }

  template <class T>
    IFRUtil_LinkedListItem<T>* IFRUtil_LinkedList<T>::Next (const T& elem){
      lock(); 
      if (elem.m_Next == &m_Head ) {
         unlock(); 
         return 0;
       }
      IFRUtil_LinkedListItem<T>*  erg = elem.m_Next;
      unlock(); 
      return erg;
    }

template <class T>
IFRUtil_LinkedListItem<T>*  IFRUtil_LinkedList<T>::Prev (const T& elem){
      lock(); 
      if (  elem.m_Prev == &m_Head) {
         unlock(); 
         return 0;
       }
      IFRUtil_LinkedListItem<T>*  erg = elem.m_Prev;
      unlock(); 
      return erg;
    }



/*
class xxx:public IFRUtil_LinkedListItem<xxx>{
private:
  int mymember;
} 

class yyy {
private:
  IFRUtil_LinkedList<xxx> list;
} 
*/

#ifdef IFR_NAMESPACE
};
#endif

#endif 


