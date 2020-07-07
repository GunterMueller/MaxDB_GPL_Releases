/*! 
    \file    Tools_List.hpp
    \author  TiloH
    \ingroup universal list for tools
    \brief   a list class, a little bit like STL list

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

#if !defined(Tools_List_HPP)
#define Tools_List_HPP

//
//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
//

#include <new.h>


//-----------------------------------------------------------------------------
// macros
//-----------------------------------------------------------------------------

#if defined(WIN32)
#define Tools_List_NEW new
#else
#if defined (HPUX)
#define Tools_List_NEW new(nothrow)
#else
#define Tools_List_NEW new(std::nothrow)
#endif
#endif


//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------

template<class T> class Tools_ListNode;
template<class T> class Tools_ListIterator;
template<class T> class Tools_ListConstIterator;
template<class T> class Tools_ListReverseIterator;
template<class T> class Tools_ListConstReverseIterator;


/*! \brief a template based list class like STL's list */
template<class T> class Tools_List
{
  public:
    /*! \brief constructor

    creates an empty list, containing a hidden and empty list node used for end
    of list handling (thus forcing class T to have a default constructor). */
    Tools_List();

    /*! \brief destructor

    destructs a list by freeing all memory used by list nodes. Note that if
    class T is pointing to memory itself, that memory must either be freed in
    the destructor of class T or before the destructor of the list is called */
    virtual ~Tools_List();

    /*! \brief function for adding the copy of something at the beginning of the list
    
    adds a copy of AnElement at the front of the list, using a new operator, that
    is not throwing an exception if not enough memory is available, and the copy
    constructor of class T or the default constructor+assignment operator of class T

    \param AnElement [IN] reference to constant instance of class T

    \returns true, if a copy could be created and added
             false, otherwise (out of memory situation) */
    bool push_front(const T & AnElement);

    /*! \brief function for adding a copy of something at the end of the list

    adds a copy of AnElement at the end of the list, using the a new operator, that
    is not throwing an exception if not enough memory is available, and the copy
    constructor of class T or the default constructor+assignment operator of class T

    \param AnElement [IN] reference to constant instance of class T

    \returns true, if a copy could be created and added
             false, otherwise (out of memory situation) */
    bool push_back(const T & AnElement);

    /*! \brief function for inserting elements in the middle of a list

        inserts a copy of anElement in front of the element, the
        iterator position is pointing to. Note that there is a similar
        insert() function using a Tools_ListReverseIterator, that is inserting
        elements behind the iterator position.

        The function insert() is using the global operator new and the copy
        constructor of class T or the default constructor+assignment operator
        of class T. 

        \see insert(Tools_ListReverseIterator<T> position, const T & anElement)

        \param position  [IN] iterator pointing to a member of the list 
        \param anElement [IN] reference to instance of class T

        \return true, if a copy could be created and inserted
                false, otherwise (out of memory situation)               */
    bool insert(Tools_ListIterator<T> position, const T & anElement);

    /*! \brief function for inserting elements in the middle of a list

        inserts a copy of anElement "behind" the element, the
        iterator position is pointing to. Hereby the meaning of
        "behind" is in the sense of the normal list order. In the sense
        of the used Tools_ListReverseIterator (and therefore most probably
        in the sense of the algorithm) the "behind" is actually a "in front".
        
        The function insert() is using the global operator new and the copy
        constructor of class T or the default constructor+assignment operator
        of class T. 

        \see insert(Tools_ListIterator<T> position, const T & anElement)

        \param position  [IN] iterator pointing to a member of the list 
        \param anElement [IN] reference to instance of class T

        \return true, if a copy could be created and inserted
                false, otherwise (out of memory situation)               */
    bool insert(Tools_ListReverseIterator<T> position, const T & anElement);

    Tools_ListIterator<T>        erase(Tools_ListIterator<T> PosToErase);
    Tools_ListReverseIterator<T> erase(Tools_ListReverseIterator<T> PosToErase);
    Tools_ListIterator<T>        erase(Tools_ListIterator<T> StartPosToErase, Tools_ListIterator<T> AfterEndPosToErase);

    void pop_front();
    void pop_back();
    void clear();

    /*! \brief function for moving the elements of another list to this list
   
        The elements of OtherList are moved to this list and added in front of
        the element pointed to by Position
        
        \param Position  [IN] iterator pointing to a member of the list
        \param OtherList [IN] a list which content should be moved */
    void splice(Tools_ListIterator<T> Position, Tools_List & OtherList);

    /*! \brief function for clearing the current list and moving the elements
         of another list to this list. Note this function is useful, but no STL function!
   
        The current list is emptied and the elements of OtherList are moved to
        this list

        \param OtherList [IN] a list which content should be moved */
    //void clearAndSplice(Tools_List & OtherList);

    bool empty() const;

    typedef Tools_ListIterator<T>             iterator;
    typedef Tools_ListConstIterator<T>        const_iterator;
    typedef Tools_ListReverseIterator<T>      reverse_iterator;
    typedef Tools_ListConstReverseIterator<T> const_reverse_iterator;
    typedef unsigned long                     size_type;

    const T& front() const;
    const T& back() const;

    iterator               begin();
    const_iterator         begin() const;

    iterator               end();
    const_iterator         end() const;

    reverse_iterator       rbegin();
    const_reverse_iterator rbegin() const;

    reverse_iterator       rend();
    const_reverse_iterator rend() const;

    size_type size() const;

  private:
    Tools_ListNode<T> FixAnchor;

    //avoid implicit copying
    Tools_List(const Tools_List& otherList) {}
};

// -----------------------------------------------------------------------------
// implementation 
// -----------------------------------------------------------------------------

template<class T> class Tools_ListNode
{
  public:
    Tools_ListNode();
    Tools_ListNode(const T &c);
 
  private:
    Tools_ListNode<T> * prev;
    T                   info;
    Tools_ListNode<T> * next;

    friend class Tools_List<T>;
    friend class Tools_ListIterator<T>;
    friend class Tools_ListConstIterator<T>;
    friend class Tools_ListReverseIterator<T>;
    friend class Tools_ListConstReverseIterator<T>;
};

template<class T> class Tools_ListIterator
{
  public:
    Tools_ListIterator();
    Tools_ListIterator(Tools_ListNode<T> *o);
    Tools_ListIterator(Tools_ListReverseIterator<T> &ri);

    Tools_ListIterator<T> & operator ++ ();
    Tools_ListIterator<T>   operator ++ (int);
    Tools_ListIterator<T> & operator -- ();
    Tools_ListIterator<T>   operator -- (int);
    int operator==(const Tools_ListIterator<T> &o) const;
    int operator!=(const Tools_ListIterator<T> &o) const;
    T & operator * ();

    Tools_ListIterator<T> & operator=(const Tools_ListReverseIterator<T> &);

    friend class Tools_List<T>;
    friend class Tools_ListReverseIterator<T>;

  private:
    Tools_ListNode<T> * NodePointer;
};

template<class T> class Tools_ListConstIterator
{
  public:
    Tools_ListConstIterator();
    Tools_ListConstIterator(const Tools_ListNode<T> *o);
    Tools_ListConstIterator(Tools_ListConstReverseIterator<T> &ri);

    Tools_ListConstIterator<T> & operator ++ ();
    Tools_ListConstIterator<T>   operator ++ (int);
    Tools_ListConstIterator<T> & operator -- ();
    Tools_ListConstIterator<T>   operator -- (int);
    int operator==(const Tools_ListConstIterator<T> &o) const;
    int operator!=(const Tools_ListConstIterator<T> &o) const;
    const T & operator * () const;

    Tools_ListConstIterator<T> & operator=(const Tools_ListConstReverseIterator<T> &);

    friend class Tools_List<T>;
    friend class Tools_ListConstReverseIterator<T>;
    //friend class Tools_ListReverseIterator<T>;

  private:
    const Tools_ListNode<T> * NodePointer;
};

template<class T> class Tools_ListReverseIterator
{
  public:
    Tools_ListReverseIterator();
    Tools_ListReverseIterator(Tools_ListNode<T> *);

    Tools_ListReverseIterator<T> & operator ++ ();
    Tools_ListReverseIterator<T>   operator ++ (int);
    Tools_ListReverseIterator<T> & operator -- ();
    Tools_ListReverseIterator<T>   operator -- (int);
    int operator==(const Tools_ListReverseIterator<T> &o) const;
    int operator!=(const Tools_ListReverseIterator<T> &o) const;
    T & operator * ();

    Tools_ListReverseIterator<T> & operator=(const Tools_ListIterator<T> &);

    friend class Tools_List<T>;
    friend class Tools_ListIterator<T>;

  private:
    Tools_ListNode<T> * NodePointer;
};

template<class T> class Tools_ListConstReverseIterator
{
  public:
    Tools_ListConstReverseIterator();
    Tools_ListConstReverseIterator(const Tools_ListNode<T> *);

    Tools_ListConstReverseIterator<T> & operator ++ ();
    Tools_ListConstReverseIterator<T>   operator ++ (int);
    Tools_ListConstReverseIterator<T> & operator -- ();
    Tools_ListConstReverseIterator<T>   operator -- (int);
    int operator==(const Tools_ListConstReverseIterator<T> &o) const;
    int operator!=(const Tools_ListConstReverseIterator<T> &o) const;
    const T & operator * () const;

    Tools_ListConstReverseIterator<T> & operator=(const Tools_ListIterator<T> &);
    Tools_ListConstReverseIterator<T> & operator=(const Tools_ListConstIterator<T> &);

    friend class Tools_List<T>;
    friend class Tools_ListConstIterator<T>;

  private:
    const Tools_ListNode<T> * NodePointer;
};


// -----------------------------------------------------------------------------
// public constructor Tools_List<T>::Tools_List
// -----------------------------------------------------------------------------

template<class T> inline Tools_List<T>::Tools_List()
{
    FixAnchor.next=&FixAnchor;
    FixAnchor.prev=&FixAnchor;
}

// -----------------------------------------------------------------------------
// public destructor Tools_List<T>::~Tools_List
// -----------------------------------------------------------------------------
 
template<class T> inline Tools_List<T>::~Tools_List()
{
    Tools_ListNode<T> *h;

    while(FixAnchor.next!=(&FixAnchor)) //while there is another element as FixAnchor
    {
        h=FixAnchor.next;               //get a pointer to the first such element
        FixAnchor.next=h->next;         //take it out of the list

        delete h;                       //and free the memory used by it
    }
}

// -----------------------------------------------------------------------------
// public member function Tools_List<T>::push_front
// -----------------------------------------------------------------------------

template<class T> inline bool Tools_List<T>::push_front(const T & AnElement)
{
    Tools_ListNode<T> *h=Tools_List_NEW Tools_ListNode<T>(AnElement);

    if(0!=h)
    {
        h->next=FixAnchor.next;
        h->prev=&FixAnchor;
        FixAnchor.next->prev=h;
        FixAnchor.next=h;
    }

    return (0!=h);
}

template<class T> inline bool Tools_List<T>::push_back(const T & AnElement)
{
    Tools_ListNode<T> *h=Tools_List_NEW Tools_ListNode<T>(AnElement);

    if(0!=h)
    {
        h->next=&FixAnchor;
        h->prev=FixAnchor.prev;
        FixAnchor.prev->next=h;
        FixAnchor.prev=h;
    }

    return (0!=h);
}

template<class T> inline bool Tools_List<T>::insert(Tools_ListIterator<T> position, const T & anElement)
{
    Tools_ListNode<T> *h=Tools_List_NEW Tools_ListNode<T>(anElement);

    if(0!=h)
    {
        h->next=position.NodePointer;
        h->prev=position.NodePointer->prev;
        position.NodePointer->prev->next=h;
        position.NodePointer->prev=h;
    }

    return (0!=h);
}

template<class T> inline bool Tools_List<T>::insert(Tools_ListReverseIterator<T> position, const T & anElement)
{
    Tools_ListNode<T> *h=Tools_List_NEW Tools_ListNode<T>(anElement);

    if(0!=h)
    {
        h->next=position.NodePointer->next;
        h->prev=position.NodePointer;
        position.NodePointer->next->prev=h;
        position.NodePointer->next=h;
    }

    return (0!=h);
}

// -----------------------------------------------------------------------------
// public member function Tools_List<T>::erase(Tools_ListIterator<T> PosToErase);
// -----------------------------------------------------------------------------

template<class T> inline Tools_ListIterator<T> Tools_List<T>::erase(Tools_ListIterator<T> PosToErase)
{
    PosToErase.NodePointer->prev->next=PosToErase.NodePointer->next;
    PosToErase.NodePointer->next->prev=PosToErase.NodePointer->prev;

    Tools_ListIterator<T> rc(PosToErase.NodePointer->next);

    delete PosToErase.NodePointer;

    return rc;
}

template<class T> inline Tools_ListReverseIterator<T> Tools_List<T>::erase(Tools_ListReverseIterator<T> PosToErase)
{
    PosToErase.NodePointer->prev->next=PosToErase.NodePointer->next;
    PosToErase.NodePointer->next->prev=PosToErase.NodePointer->prev;

    Tools_ListReverseIterator<T> rc(PosToErase.NodePointer->prev);

    delete PosToErase.NodePointer;

    return rc;
}

template<class T> inline Tools_ListIterator<T> Tools_List<T>::erase(Tools_ListIterator<T> StartPosToErase, Tools_ListIterator<T> AfterEndPosToErase)
{
    while(StartPosToErase!=AfterEndPosToErase)
        StartPosToErase=erase(StartPosToErase);

    return AfterEndPosToErase;
}

template<class T> inline void Tools_List<T>::splice(Tools_ListIterator<T> Position, Tools_List & OtherList)
{
    if(!OtherList.empty())
    {
        Position.NodePointer->prev->next=OtherList.FixAnchor.next;
        OtherList.FixAnchor.next->prev=Position.NodePointer->prev;

        Position.NodePointer->prev=OtherList.FixAnchor.prev;
        OtherList.FixAnchor.prev->next=Position.NodePointer;

        OtherList.FixAnchor.next=&OtherList.FixAnchor;
        OtherList.FixAnchor.prev=&OtherList.FixAnchor;
    }
}

/*template<class T> inline void Tools_List<T>::clearAndSplice(Tools_List & OtherList)
{
    clear();

    if(!OtherList.empty())
    {
        FixAnchor.next=OtherList.FixAnchor.next;
        OtherList.FixAnchor.next->prev=&FixAnchor;

        FixAnchor.prev=OtherList.FixAnchor.prev;
        OtherList.FixAnchor.prev->next=&FixAnchor;

        OtherList.FixAnchor.next=&OtherList.FixAnchor;
        OtherList.FixAnchor.prev=&OtherList.FixAnchor;
    }
}*/

// -----------------------------------------------------------------------------
// public member function Tools_List<T>::pop_front();
// -----------------------------------------------------------------------------

template<class T> inline void Tools_List<T>::pop_front()
{
    if(!empty())
        erase(begin());
}

// -----------------------------------------------------------------------------
// public member function Tools_List<T>::pop_back();
// -----------------------------------------------------------------------------

template<class T> inline void Tools_List<T>::pop_back()
{
    if(!empty())
        erase(rbegin());
}

// -----------------------------------------------------------------------------
// public member function Tools_List<T>::clear();
// -----------------------------------------------------------------------------

template<class T> inline void Tools_List<T>::clear()
{
    Tools_ListNode<T> *h;

    while(FixAnchor.next!=(&FixAnchor)) //while there is another element as FixAnchor
    {
        h=FixAnchor.next;               //get a pointer to the first such element
        FixAnchor.next=h->next;         //take it out of the list

        delete h;                       //and free the memory used by it
    }

    FixAnchor.prev=(&FixAnchor);
}

template<class T> inline bool Tools_List<T>::empty() const
{
    return (FixAnchor.next==(&FixAnchor));
}

template<class T> inline const T& Tools_List<T>::front() const
{
    return (FixAnchor.next->info);
}

template<class T> inline const T& Tools_List<T>::back() const
{
    return (FixAnchor.prev->info);
}

template<class T> inline typename Tools_List<T>::iterator Tools_List<T>::begin()
{
    return Tools_ListIterator<T>(FixAnchor.next);
}

template<class T> inline typename Tools_List<T>::const_iterator Tools_List<T>::begin() const
{
    return Tools_ListConstIterator<T>(FixAnchor.next);
}

template<class T> inline typename Tools_List<T>::iterator Tools_List<T>::end()
{
    return Tools_ListIterator<T>(&FixAnchor);
}

template<class T> inline typename Tools_List<T>::const_iterator Tools_List<T>::end() const
{
    return Tools_ListConstIterator<T>(&FixAnchor);
}

template<class T> inline typename Tools_List<T>::reverse_iterator Tools_List<T>::rbegin()
{
    return Tools_ListReverseIterator<T>(FixAnchor.prev);
}

template<class T> inline typename Tools_List<T>::const_reverse_iterator Tools_List<T>::rbegin() const
{
    return Tools_ListConstReverseIterator<T>(FixAnchor.prev);
}

template<class T> inline typename Tools_List<T>::reverse_iterator Tools_List<T>::rend()
{
    return Tools_ListReverseIterator<T>(&FixAnchor);
}

template<class T> inline typename Tools_List<T>::const_reverse_iterator Tools_List<T>::rend() const
{
    return Tools_ListConstReverseIterator<T>(&FixAnchor);
}

template<class T> inline typename Tools_List<T>::size_type Tools_List<T>::size() const
{
    size_type                     rc=0;
    typename Tools_List<T>::const_iterator h=begin();

    for(; h!=end(); ++h)
        ++rc;

    return rc;
}


template<class T> inline Tools_ListNode<T>::Tools_ListNode()
{
}

#if defined (SUN) || defined (SOLARIS)
template<class T> inline Tools_ListNode<T>::Tools_ListNode(const T &c)
{
    if(0!=this) //looks funny, but solves the problem that on solaris during a 'new(std::nothrow) X' 
        info=c; //the constructor X::X() is even called, if the new(std::nothrow) could not allocate
}               //the memory and returned a 0.
#else
template<class T> inline Tools_ListNode<T>::Tools_ListNode(const T &c)
:info(c)
{
}
#endif


template<class T> inline Tools_ListIterator<T>::Tools_ListIterator()
:NodePointer(0)
{
}

template<class T> inline Tools_ListIterator<T>::Tools_ListIterator(Tools_ListNode<T> *o)
:NodePointer(o)
{
}

template<class T> inline Tools_ListIterator<T>::Tools_ListIterator(
        Tools_ListReverseIterator<T> &ri)
        : NodePointer(ri.NodePointer)
{
}

template<class T> inline Tools_ListIterator<T> & Tools_ListIterator<T>::operator ++ ()
{
    NodePointer=NodePointer->next;

    return (*this);
}

template<class T> inline Tools_ListIterator<T> Tools_ListIterator<T>::operator ++ (int)
{
    Tools_ListIterator<T> TempIterator(NodePointer);
    NodePointer=NodePointer->next;

    return TempIterator;
}

template<class T> inline Tools_ListIterator<T> & Tools_ListIterator<T>::operator -- ()
{
    NodePointer=NodePointer->prev;

    return (*this);
}

template<class T> inline Tools_ListIterator<T> Tools_ListIterator<T>::operator -- (int)
{
    Tools_ListIterator<T> TempIterator(NodePointer);
    NodePointer=NodePointer->prev;

    return TempIterator;
}

template<class T> inline int Tools_ListIterator<T>::operator==(const Tools_ListIterator<T> &o) const
{
    return (NodePointer==o.NodePointer);
}

template<class T> inline int Tools_ListIterator<T>::operator!=(const Tools_ListIterator<T> &o) const
{
    return (NodePointer!=o.NodePointer);
}

template<class T> inline T & Tools_ListIterator<T>::operator * ()
{
    return NodePointer->info;
}

template<class T> inline Tools_ListIterator<T> & Tools_ListIterator<T>::operator=(const Tools_ListReverseIterator<T> &ri)
{
    NodePointer=ri.NodePointer;

    return (*this);
}


template<class T> inline Tools_ListConstIterator<T>::Tools_ListConstIterator()
:NodePointer(0)
{
}

template<class T> inline Tools_ListConstIterator<T>::Tools_ListConstIterator(const Tools_ListNode<T> *o)
:NodePointer(o)
{
}

template<class T> inline Tools_ListConstIterator<T>::Tools_ListConstIterator(
        Tools_ListConstReverseIterator<T> &ri)
        : NodePointer(ri.NodePointer)
{
}

template<class T> inline Tools_ListConstIterator<T> & Tools_ListConstIterator<T>::operator ++ ()
{
    NodePointer=NodePointer->next;

    return (*this);
}

template<class T> inline Tools_ListConstIterator<T> Tools_ListConstIterator<T>::operator ++ (int)
{
    Tools_ListConstIterator<T> TempIterator(NodePointer);
    NodePointer=NodePointer->next;

    return TempIterator;
}

template<class T> inline Tools_ListConstIterator<T> & Tools_ListConstIterator<T>::operator -- ()
{
    NodePointer=NodePointer->prev;

    return (*this);
}

template<class T> inline Tools_ListConstIterator<T> Tools_ListConstIterator<T>::operator -- (int)
{
    Tools_ListConstIterator<T> TempIterator(NodePointer);
    NodePointer=NodePointer->prev;

    return TempIterator;
}

template<class T> inline int Tools_ListConstIterator<T>::operator==(const Tools_ListConstIterator<T> &o) const
{
    return (NodePointer==o.NodePointer);
}

template<class T> inline int Tools_ListConstIterator<T>::operator!=(const Tools_ListConstIterator<T> &o) const
{
    return (NodePointer!=o.NodePointer);
}

template<class T> inline const T & Tools_ListConstIterator<T>::operator * () const
{
    return NodePointer->info;
}

template<class T> inline Tools_ListConstIterator<T> & Tools_ListConstIterator<T>::operator=(const Tools_ListConstReverseIterator<T> &ri)
{
    NodePointer=ri.NodePointer;

    return (*this);
}


template<class T> inline Tools_ListReverseIterator<T>::Tools_ListReverseIterator()
:NodePointer(0)
{
}

template<class T> inline Tools_ListReverseIterator<T>::Tools_ListReverseIterator(Tools_ListNode<T> *o)
:NodePointer(o)
{
}

template<class T> inline Tools_ListReverseIterator<T> & Tools_ListReverseIterator<T>::operator ++ ()
{
    NodePointer=NodePointer->prev;

    return (*this);
}

template<class T> inline Tools_ListReverseIterator<T> Tools_ListReverseIterator<T>::operator ++ (int)
{
    Tools_ListReverseIterator<T> TempIterator(NodePointer);
    NodePointer=NodePointer->prev;

    return TempIterator;
}

template<class T> inline Tools_ListReverseIterator<T> & Tools_ListReverseIterator<T>::operator -- ()
{
    NodePointer=NodePointer->next;

    return (*this);
}

template<class T> inline Tools_ListReverseIterator<T> Tools_ListReverseIterator<T>::operator -- (int)
{
    Tools_ListReverseIterator<T> TempIterator(NodePointer);
    NodePointer=NodePointer->next;

    return TempIterator;
}

template<class T> inline int Tools_ListReverseIterator<T>::operator==(const Tools_ListReverseIterator<T> &o) const
{
    return (NodePointer==o.NodePointer);
}

template<class T> inline int Tools_ListReverseIterator<T>::operator!=(const Tools_ListReverseIterator<T> &o) const
{
    return (NodePointer!=o.NodePointer);
}

template<class T> inline T & Tools_ListReverseIterator<T>::operator * ()
{
    return NodePointer->info;
}

template<class T> inline Tools_ListReverseIterator<T> & Tools_ListReverseIterator<T>::operator=(const Tools_ListIterator<T> &i)
{
    NodePointer=i.NodePointer;

    return (*this);
}


template<class T> inline Tools_ListConstReverseIterator<T>::Tools_ListConstReverseIterator()
:NodePointer(0)
{
}

template<class T> inline Tools_ListConstReverseIterator<T>::Tools_ListConstReverseIterator(const Tools_ListNode<T> * Original)
:NodePointer(Original)
{
}

template<class T> inline Tools_ListConstReverseIterator<T> & Tools_ListConstReverseIterator<T>::operator ++ ()
{
    NodePointer=NodePointer->prev;

    return (*this);
}

template<class T> inline Tools_ListConstReverseIterator<T> Tools_ListConstReverseIterator<T>::operator ++ (int)
{
    Tools_ListConstReverseIterator<T> TempIterator(NodePointer);
    NodePointer=NodePointer->prev;

    return TempIterator;
}

template<class T> inline Tools_ListConstReverseIterator<T> & Tools_ListConstReverseIterator<T>::operator -- ()
{
    NodePointer=NodePointer->next;

    return (*this);
}

template<class T> inline Tools_ListConstReverseIterator<T> Tools_ListConstReverseIterator<T>::operator -- (int)
{
    Tools_ListConstReverseIterator<T> TempIterator(NodePointer);
    NodePointer=NodePointer->next;

    return TempIterator;
}

template<class T> inline int Tools_ListConstReverseIterator<T>::operator==(const Tools_ListConstReverseIterator<T> &o) const
{
    return (NodePointer==o.NodePointer);
}

template<class T> inline int Tools_ListConstReverseIterator<T>::operator!=(const Tools_ListConstReverseIterator<T> &o) const
{
    return (NodePointer!=o.NodePointer);
}

template<class T> inline const T & Tools_ListConstReverseIterator<T>::operator * () const
{
    return NodePointer->info;
}

template<class T> inline Tools_ListConstReverseIterator<T> & Tools_ListConstReverseIterator<T>::operator=(const Tools_ListConstIterator<T> & i)
{
    NodePointer=i.NodePointer;

    return (*this);
}

#endif
