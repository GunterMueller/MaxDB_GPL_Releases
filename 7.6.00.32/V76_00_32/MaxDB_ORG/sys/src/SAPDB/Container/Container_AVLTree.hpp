/*!**************************************************************************

  module      : Container_ALVTree.hpp
  responsible : Dirk Thomsen
  created     : 2003-01-24
  last changed: 2003-02-13  
  copyright:    Copyright (c) 2003-2005 SAP AG
  description : Class declaration and definition for template ALVTree

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#ifndef CONTAINER_AVLTREE_HPP
#define CONTAINER_AVLTREE_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "geo00.h"

/*-----------------------------------------------------------------------------*/

enum Container_AVLTreeError
{
    AVLTree_Ok              = 0,
    AVLTree_NotFound        = -1,
    AVLTree_DuplicateKey    = -2,
    AVLTree_OutOfMemory     = -8
};

enum Container_AVLTreeIterDirection
{
    AVLTree_ASCENDING,
    AVLTree_DESCENDING
};

/// Definition of Container_AVLNodeComparator                                   */
template <class T>
class Container_AVLNodeComparator
{
public :
    /*! Ths method compares the key of some node (NodeKey) with the key searched for (SearchKey). 
        For some applications the order of arguments may be important, so Container_AVLTree must
        pay attention to it!!!
    */
    inline int Compare(const T& NodeKey, const T& SearchKey)
    {
        if (NodeKey < SearchKey) return -1;
        if (SearchKey < NodeKey) return 1;
        return 0;
    }
};

/// Definition of Container_AVLNode                                             */
template <class KEY, class CMP>
class Container_AVLNode
{
    typedef Container_AVLNode<KEY,CMP>* Container_AVLNodePtr;
    //
protected:
    KEY                             m_Key;
    //
    Container_AVLNodePtr            m_LeftSon;
    Container_AVLNodePtr            m_RightSon;
    Container_AVLNodePtr            m_Parent;
    int                             m_Balance;
    //
public:
    //
    Container_AVLNode( const KEY& k );
    virtual ~Container_AVLNode( void ) {}   // must be virtual!
    //
    inline const KEY*               GetKey( void ) const;
    //
    inline int&                     Balance( void );
    inline SAPDB_Bool               Balanced( void ) const;
    inline void                     Delete_RR( Container_AVLNodePtr& p, SAPDB_Bool& balance );
    inline void                     Delete_LL( Container_AVLNodePtr& p, SAPDB_Bool& balance );
    inline SAPDB_Bool               LeftDeeper( void ) const;
    inline Container_AVLNodePtr&    LeftSon( void ) { return m_LeftSon; }
    inline SAPDB_Bool               RightDeeper( void ) const;
    inline Container_AVLNodePtr&    RightSon( void ) { return m_RightSon; }
    inline void                     SetLeftSon( Container_AVLNodePtr son );
    inline void                     SetRightSon( Container_AVLNodePtr son );
    inline Container_AVLNodePtr&    Parent( void ) { return m_Parent; }
    inline void                     Rotate_LL( Container_AVLNodePtr& p );
    inline void                     Rotate_LR( Container_AVLNodePtr& p );
    inline void                     Rotate_RL( Container_AVLNodePtr& p );
    inline void                     Rotate_RR( Container_AVLNodePtr& p );
    inline void                     SetBalanced( void );
    inline void                     SetRightDeeper( void );
    inline void                     SetLeftDeeper( void );
    virtual void CleanUp( SAPDBMem_IRawAllocator& alloc, CMP& cmp ) {};
};

template <class KEY, class CONTENT, class CMP>
class Container_AVLContentNode : public Container_AVLNode<KEY,CMP>
{
    typedef Container_AVLContentNode<KEY,CONTENT,CMP>* Container_AVLContentNodePtr;
    //
public:
    //
    Container_AVLContentNode( const KEY& k ) : Container_AVLNode<KEY,CMP>(k), m_Content() 
    {
    }
    //
    Container_AVLContentNode( const KEY& k, const CONTENT& d ) : Container_AVLNode<KEY,CMP>(k), m_Content(d) 
    {
    }
    //
    virtual ~Container_AVLContentNode( void ) {}    // must be virtual as otherwise m_Content will not be destructed when AVLTree.DeleteNode is called
    //
    inline const CONTENT* GetContent( void ) const 
    {
        return &m_Content;
    }
    //
    inline const CONTENT* SetContent( const CONTENT& d ) 
    {
        m_Content = d;
        return &m_Content;
    }
    //
    virtual void DeleteSelf( SAPDBMem_IRawAllocator& alloc, CMP& cmp ) {};
    //
private:
    CONTENT m_Content;
    //
};

/// Definition of Container_AVLTree                                             */
template<class NODE, class KEY, class CMP>
class Container_AVLTree
{
#if defined(AIX) || defined(SUN) || defined(HPUX) || defined(SOLARIS)
public :
#endif
    typedef Container_AVLNode<KEY,CMP>* Container_AVLNodePtr;
    //
public :
    //
    Container_AVLTree(CMP* c, SAPDBMem_IRawAllocator* a) : m_Root(NULL), mAllocatorPtr(a), mCmp(c), m_NodeCount(0), m_ChangeCount(0) 
    {
    }
    //
    Container_AVLTree() : m_Root(NULL), mAllocatorPtr(NULL), mCmp(NULL), m_NodeCount(0), m_ChangeCount(0) 
    {
    }
    //
    virtual ~Container_AVLTree( void ) 
    { 
        DeleteAll(); 
    }
    //
    inline void AdviseAllocator( SAPDBMem_IRawAllocator* a ) 
    { 
        mAllocatorPtr = a; 
    }
    //
    inline SAPDBMem_IRawAllocator* GetAllocator( void )
    {
        return mAllocatorPtr;
    }
    //
    inline SAPDB_ULong GetMaxSize ( void )
    {
        return 0; // return mAllocatorPtr.??? // D.T.
    }
    //
    inline void AdviseCompare( CMP* c )     
    { 
        mCmp = c; 
    }
    //
    inline NODE* Insert( const KEY& key, Container_AVLTreeError& rc ) 
    { 
        return InsertIntoTree(key, rc); 
    }
    //
    inline NODE* Find( const KEY& key ) const { return FindNode(key); }
    //
    inline NODE* InsertNode( const KEY& key, Container_AVLNodePtr& p, const Container_AVLNodePtr& parent, SAPDB_Bool& balance, Container_AVLTreeError& rc );
    //
    inline NODE* FindNode( const KEY& key ) const;
    //
    inline NODE* InsertIntoTree( const KEY& key, Container_AVLTreeError& rc )
    {
        SAPDB_Bool dummy = SAPDB_FALSE;
        rc = AVLTree_Ok;
        return InsertNode(key, m_Root, NULL, dummy, rc);
    }
    //
    inline Container_AVLTreeError Delete( const KEY& key )
    {
        SAPDB_Bool balance = SAPDB_FALSE;
        return DeleteNode (key, *REINTERPRET_CAST(Container_AVLNodePtr*, &m_Root), balance);
    }
    //
    inline void DeleteAll( void )
    {
        DeleteSubtree(m_Root);
        m_Root = NULL;
    }
    //
    inline SAPDB_ULong GetSize( void ) const    
    { 
        return m_NodeCount; 
    }
    //
    inline SAPDB_ULong GetChanges( void ) const 
    { 
        return m_ChangeCount; 
    }
    //
    inline SAPDB_Bool CheckTree( void ) const   
    { 
        return (CheckSubTree(m_Root) >= 0); 
    }

#if !defined(AIX) && !(defined(SUN) || defined(SOLARIS)) && !defined(HPUX) 
private :
#endif

#define STACK_SIZE 128

    class Stack
    {
    public :
        int              m_Bottom;
        int              m_Top;
        Container_AVLNodePtr m_Stack[STACK_SIZE];
        Stack() : m_Bottom(0), m_Top(0) {};

        inline void Push( Container_AVLNodePtr p )
        {
            ++m_Top;
            if (STACK_SIZE == m_Top)
                m_Top = 0;
            m_Stack[m_Top] = p;
            if (m_Top == m_Bottom)
            {
                ++m_Bottom;
                if (STACK_SIZE == m_Bottom)
                    m_Bottom = 0;
            }
        }

        inline SAPDB_Bool IsEmpty( void )
        {
            return (m_Bottom == m_Top);
        }

        inline Container_AVLNodePtr Pop( void )
        {
            if (!IsEmpty())
            {
                Container_AVLNodePtr p = m_Stack[m_Top];
                --m_Top;
                if (m_Top < 0)
                    m_Top = STACK_SIZE - 1;
                return p;
            }
            return NULL;
        }

        inline void Reset ( void )
        {
            while (!IsEmpty())
            {
                Pop();
            }
        }
    };

private :
    inline int         CheckSubTree ( Container_AVLNodePtr p ) const;
    inline void        BalanceLeft  ( Container_AVLNodePtr& p, SAPDB_Bool& balance );
    inline void        BalanceRight ( Container_AVLNodePtr& p, SAPDB_Bool& balance );
    inline void        Del                ( Container_AVLNodePtr& p, Container_AVLNodePtr& deleted, SAPDB_Bool& balance );
    inline void        DeleteBalanceLeft  ( Container_AVLNodePtr& p, SAPDB_Bool& balance );
    inline void        DeleteBalanceRight ( Container_AVLNodePtr& p, SAPDB_Bool& balance );
    inline Container_AVLTreeError DeleteNode   ( const KEY& key, Container_AVLNodePtr& p, SAPDB_Bool& doBalance );
    inline void        DeleteSubtree( Container_AVLNodePtr p );

    ///
    Container_AVLNodePtr m_Root;
    ///
    SAPDBMem_IRawAllocator* mAllocatorPtr;
    CMP*                    mCmp;
    /// Number of nodes within the tree
    SAPDB_ULong     m_NodeCount;
    /// This counter is incremented every time an update operation takes place on the tree
    /// and thereby it is used to validate iterators if necessary.
    SAPDB_ULong     m_ChangeCount;

public :
    class Iterator;
    friend class Iterator;
    /// Local class to iterate over all entries stored in the avl-tree
    class Iterator
    {
        /// Pointer to the current node
        Container_AVLNodePtr m_curNode;
    public :
        /// Pointer to the corresponding avl-tree
        Container_AVLTree*   m_AVLTree;
        /// Direction iterator is moving if element is deleted
        Container_AVLTreeIterDirection m_direction;

        /*!
         * When starting the iterator this counter is set to the value
         * of the change counter of the corresponding avl-tree. Before
         * increment or decrement it is checked, that these two
         * counters are still the same and if they differ, then an
         * exception is thrown, as the structure of the tree might
         * have changed.
         */
        SAPDB_ULong         m_startCount;
        /// ctor 1
        Iterator( void ) : m_AVLTree(NULL),
                           m_curNode(NULL),
                           m_direction(AVLTree_ASCENDING),
                           m_startCount(0)
        {
        }
        /// ctor 1
        Iterator( const Iterator& iter ) : m_AVLTree(iter.m_AVLTree),
                                           m_curNode(iter.m_curNode),
                                           m_direction(iter.m_direction),
                                           m_startCount(iter.m_startCount)
        {
        }
        /// cast operator
        inline operator SAPDB_Bool( void )
        {
            return m_curNode != NULL;
        }

        /// Increment Iterator (Prefix variant).
        inline void operator++( void )
        {
            //if (m_StartCount!= m_AVLTree->m_ChangeCount){
            // The structure might has been changed by an update operation
            // on the tree
            // Throwing of exception not possible, as this class is used in the kernel.
            //}

            if (m_curNode == NULL)
            {
                return;
            }

            if (m_curNode->RightSon() == NULL)
            {
                // Ascent from leaf node or right child node.
                Container_AVLNodePtr p = m_curNode;
                while (m_curNode != NULL)
                {
                    m_curNode = m_curNode->Parent();
                    if ((m_curNode != NULL) && (m_curNode->RightSon()) != p)
                        break;
                    p = m_curNode;
                }
            }
            else
            {
                // Descent to leftmost leaf in right subtree
                m_curNode = m_curNode->RightSon();
                while (NULL != m_curNode->LeftSon())
                {
                    m_curNode = m_curNode->LeftSon();
                }
            }
        }

        /// Increment iterator (Postfix variant. Parameter is only used to
        /// distinguish from prefix variant)
        inline void operator++( int i )
        {
            operator++();
        }

        /// Decrement iterator. (Prefix variant)
        inline void operator--( void )
        {
            //if (m_StartCount != m_AVLTree->m_ChangeCount){
            // The structure might has been changed by an update operation
            // on the tree.
            // Throwing of exception not possible, as this class is used in the kernel.
            //}

            if (m_curNode == NULL)
            {
                return;
            }

            if (m_curNode->LeftSon() == NULL)
            {
                // Ascent from leave node or left child node.
                Container_AVLNodePtr p = m_curNode;
                while (m_curNode != NULL)
                {
                    m_curNode = m_curNode->Parent();
                    if ((m_curNode != NULL) && (m_curNode->LeftSon()) != p)
                        break;
                    p = m_curNode;
                }
            }
            else
            {
                // Descent to rightmost leave in left subtree
                m_curNode = m_curNode->LeftSon();
                while (NULL != m_curNode->RightSon())
                {
                    m_curNode = m_curNode->RightSon();
                }
            }
        }

        /// Decrement iterator (Postfix variant. Parameter is only used to
        /// distinguish from prefix variant)
        inline void operator--( int i )
        {
            operator--();
        }
        /// deref op
        inline NODE* operator()() 
        {
            return (NODE*) GetNodePtr();
        }
        /// deref const op
        inline const NODE* operator()() const
        {
            return (NODE*) GetNodePtr();
        }
        /// returns a node in the tree
        inline Container_AVLNodePtr GetNodePtr( void ) const
        {
            return m_curNode;
        }

        /// Iterator is invalid, if there was an update operation on the
        /// corresponding avl-tree since the start of the iterator.
        inline SAPDB_Bool IsValid( void ) const
        {
            return
                (m_curNode != NULL)
                && (m_startCount == m_AVLTree->m_ChangeCount);
        }
        /// set to last node
        inline void SetLast( void )
        {
            m_curNode = m_AVLTree->GetRoot();
            while ((NULL != m_curNode) && (NULL != m_curNode->RightSon()))
            {
                m_curNode = m_curNode->RightSon();
            }
            m_startCount = m_AVLTree->m_ChangeCount;
        }
        /// set to first node
        inline void SetFirst( void )
        {
            m_curNode = m_AVLTree->GetRoot();
            while ((NULL != m_curNode) && (NULL != m_curNode->LeftSon()))
            {
                m_curNode = m_curNode->LeftSon();
            }
            m_startCount = m_AVLTree->m_ChangeCount;
        }
        /// set to specific location
        void SetLocation(Container_AVLNodePtr& p)
        {
            m_curNode = p;
            m_startCount = m_AVLTree->m_ChangeCount;
        }            
    };

    /// delete an entry
    Container_AVLTreeError Delete( Iterator& iter )
    {
        Container_AVLNodePtr node = iter.GetNodePtr();
        if ( node == 0 )
            return AVLTree_NotFound;

        const KEY& key = *iter()->GetKey();
        if (iter.m_direction == AVLTree_ASCENDING)
            ++iter;
        else
            --iter;
        Container_AVLTreeError rc = Delete (key);
        if (rc == AVLTree_Ok)
            iter.m_startCount = this->m_ChangeCount;
        return rc;
    }
    /// returns pointer to first element
    Iterator    First( void );
    /// returns pointer to last element
    Iterator    Last( void );
    /// search element
    Iterator    Locate( KEY, Container_AVLTreeIterDirection direction=AVLTree_ASCENDING );
    /// get root node
    Container_AVLNodePtr GetRoot( void ) 
    { 
        return m_Root; 
    }
};

/*-----------------------------------------------------------------------------*/
/* Definition of Container_AVLContentTree                                      */
/*-----------------------------------------------------------------------------*/

template<class NODE, class KEY, class CONTENT, class CMP>
class Container_AVLContentTree : public Container_AVLTree<NODE,KEY,CMP>
{
    typedef Container_AVLContentNode<KEY,CONTENT,CMP>* Container_AVLNodePtr;
    //
public :
    //
    Container_AVLContentTree( CMP* c, SAPDBMem_IRawAllocator* a ) : Container_AVLTree<NODE,KEY,CMP>(c, a) 
    {
    };
    //
    Container_AVLContentTree( void ) : Container_AVLTree<NODE,KEY,CMP>() 
    {
    };
    //
    inline const NODE* Insert( const KEY& key, const CONTENT& data, Container_AVLTreeError& rc )
    {
        NODE* N = (NODE*) InsertIntoTree(key, rc);
        if (N)
            N->SetContent(data);
        return N;
    }
    //
    inline const NODE* Insert( const KEY& key, Container_AVLTreeError& rc )
    {
        NODE* N = InsertIntoTree(key, rc);
        if (N)
            N->SetContent(CONTENT());
        return N;
    }
    //
    inline const NODE* Find( const KEY& key ) const             
    { 
        return (NODE*) FindNode(key);
    }
    //
    inline const CONTENT* FindContent( const KEY& key ) const
    {
        if (const NODE* N = Find(key))
            return N->GetContent();
        else
            return 0;
    }
    //
};


/*-----------------------------------------------------------------------------*/
/* Implementation of Container_AVLNode                                         */
/*-----------------------------------------------------------------------------*/

template<class KEY, class CMP>
Container_AVLNode<KEY,CMP>::Container_AVLNode(const KEY& k) : m_Key(k), m_LeftSon(NULL), m_RightSon(NULL), m_Balance(0)
{
}

template<class KEY, class CMP>
inline const KEY* Container_AVLNode<KEY,CMP>::GetKey() const
{
    return &m_Key;
}

template<class KEY, class CMP>
inline SAPDB_Bool Container_AVLNode<KEY,CMP>::Balanced() const
{
    return (0 == m_Balance);
}

template<class KEY, class CMP>
inline void Container_AVLNode<KEY,CMP>::Delete_LL (Container_AVLNodePtr& p, SAPDB_Bool& balance)
{
    p->SetLeftSon( this->RightSon() );
    this->SetRightSon( p );
    if (this->Balanced())
    {
        p->SetLeftDeeper();
        this->SetRightDeeper();
        balance = SAPDB_FALSE;
    }
    else
    {
        this->SetBalanced();
        p->SetBalanced();
    }
    p = this;
}

template<class KEY, class CMP>
inline void Container_AVLNode<KEY,CMP>::Delete_RR (Container_AVLNodePtr& p, SAPDB_Bool& balance)
{
    p->SetRightSon( this->LeftSon() );
    this->SetLeftSon( p );
    if (this->Balanced())
    {
        p->SetRightDeeper();
        this->SetLeftDeeper();
        balance = SAPDB_FALSE;
    }
    else
    {
        this->SetBalanced();
        p->SetBalanced();
    }
    p = this;
}

template<class KEY, class CMP>
inline SAPDB_Bool Container_AVLNode<KEY,CMP>::LeftDeeper() const
{
    return (-1 == m_Balance);
}

template<class KEY, class CMP>
inline SAPDB_Bool Container_AVLNode<KEY,CMP>::RightDeeper() const
{
    return (1 == m_Balance);
}

template<class KEY, class CMP>
inline void Container_AVLNode<KEY,CMP>::Rotate_LL (Container_AVLNodePtr& p)
{
    this->Parent() = p->Parent();
    p->SetLeftSon( this->RightSon() );
    this->SetRightSon( p );
    p->SetBalanced();
    p = this;
}

template<class KEY, class CMP>
inline void Container_AVLNode<KEY,CMP>::Rotate_LR (Container_AVLNodePtr& p)
{
    Container_AVLNodePtr right = RightSon();
    right->Parent() = p->Parent();
    this->SetRightSon( right->LeftSon() );
    right->SetLeftSon( this );
    p->SetLeftSon( right->RightSon() );
    right->SetRightSon( p );

    if (right->LeftDeeper())
        p->SetRightDeeper();
    else
        p->SetBalanced();

    if (right->RightDeeper())
        this->SetLeftDeeper();
    else
        this->SetBalanced();

    p = right;
    p->SetBalanced();
}

template<class KEY, class CMP>
inline void Container_AVLNode<KEY,CMP>::Rotate_RL (Container_AVLNodePtr& p)
{
    Container_AVLNodePtr left = LeftSon();
    left->Parent() = p->Parent();
    this->SetLeftSon( left->RightSon() );
    left->SetRightSon( this );
    p->SetRightSon( left->LeftSon() );
    left->SetLeftSon( p );

    if (left->RightDeeper())
        p->SetLeftDeeper();
    else
        p->SetBalanced();

    if (left->LeftDeeper())
        this->SetRightDeeper();
    else
        this->SetBalanced();

    p = left;
    p->SetBalanced();
}

template<class KEY, class CMP>
inline void Container_AVLNode<KEY,CMP>::Rotate_RR (Container_AVLNodePtr& p)
{
    this->Parent() = p->Parent();
    p->SetRightSon( this->LeftSon() );
    this->SetLeftSon( p );
    p->SetBalanced();
    p = this;
}

template<class KEY, class CMP>
inline void Container_AVLNode<KEY,CMP>::SetBalanced()
{
    m_Balance = 0;
}

template<class KEY, class CMP>
inline void Container_AVLNode<KEY,CMP>::SetRightDeeper()
{
    m_Balance = 1;
}

template<class KEY, class CMP>
inline void Container_AVLNode<KEY,CMP>::SetLeftDeeper()
{
    m_Balance = -1;
}

template<class KEY, class CMP>
inline int& Container_AVLNode<KEY,CMP>::Balance()
{
    return m_Balance;
}

/*-----------------------------------------------------------------------------*/
/* Implementation of Container_AVLTree                                         */
/*-----------------------------------------------------------------------------*/

template<class NODE, class KEY, class CMP>
inline void Container_AVLTree<NODE,KEY,CMP>::BalanceLeft (Container_AVLNodePtr& p, SAPDB_Bool& balance)
{
    if (p->RightDeeper())
    {
        p->SetBalanced();
        balance = SAPDB_FALSE;
    }
    else
    {
        if (p->Balanced())
        {
            p->SetLeftDeeper();
        }
        else
        {
            Container_AVLNodePtr left = p->LeftSon();
            if (left->LeftDeeper())
                left->Rotate_LL (p);
            else
                left->Rotate_LR(p);
            p->SetBalanced();
            balance = SAPDB_FALSE;
        }
    }
}

template<class NODE, class KEY, class CMP>
inline void Container_AVLTree<NODE,KEY,CMP>::BalanceRight(Container_AVLNodePtr& p, SAPDB_Bool& balance)
{
    if (p->LeftDeeper())
    {
        p->SetBalanced();
        balance = SAPDB_FALSE;
    }
    else {
        if (p->Balanced())
        {
            p->SetRightDeeper();
        }
        else {
            Container_AVLNodePtr right = p->RightSon();
            if (right->RightDeeper())
                right->Rotate_RR (p);
            else
                right->Rotate_RL(p);
            p->SetBalanced();
            balance = SAPDB_FALSE;
        }
    }
}

template<class NODE, class KEY, class CMP>
inline int Container_AVLTree<NODE,KEY,CMP>::CheckSubTree(Container_AVLNodePtr p) const
{
    if (0 == p)
        return 0;

    int l = CheckSubTree(p->LeftSon());
    int r = CheckSubTree(p->RightSon());
    if (l < r)
    {
        if (!p->RightDeeper())
            return -1;
        return (r + 1);
    }
    if (l > r)
    {
        if (!p->LeftDeeper())
            return -1;
        return (l + 1);
    }
    if (!p->Balanced())
        return -1;
    return (l + 1);
}

template<class NODE, class KEY, class CMP>
inline void Container_AVLTree<NODE,KEY,CMP>::Del(Container_AVLNodePtr& p, Container_AVLNodePtr& deletedNode, SAPDB_Bool& balance)
{
    if (NULL != p->RightSon())
    {
        Del (p->RightSon(), deletedNode, balance);
        p->SetRightSon(p->RightSon()); // fix parent entry of rightSon
        if (balance)
            DeleteBalanceRight (p, balance);
    }
    else
    {
        deletedNode = p;
        p           = p->LeftSon();
        balance     = SAPDB_TRUE;
    }
}

template<class NODE, class KEY, class CMP>
inline void Container_AVLTree<NODE,KEY,CMP>::DeleteBalanceLeft(Container_AVLNodePtr& p, SAPDB_Bool& balance)
{
    if (p->LeftDeeper())
    {
        p->SetBalanced();
    }
    else
    {
        if (p->Balanced())
        {
            p->SetRightDeeper();
            balance = SAPDB_FALSE;
        }
        else
        {
            Container_AVLNodePtr right = p->RightSon();
            if (!right->LeftDeeper())
            {         
                right->Parent() = p->Parent();
                right->Delete_RR (p, balance);
            }
            else
                right->Rotate_RL(p);
        }
    }
}

template<class NODE, class KEY, class CMP>
inline void Container_AVLTree<NODE,KEY,CMP>::DeleteBalanceRight (Container_AVLNodePtr& p, SAPDB_Bool& balance)
{
    if (p->RightDeeper())
    {
        p->SetBalanced();
    }
    else
    {
        if (p->Balanced())
        {
            p->SetLeftDeeper();
            balance = SAPDB_FALSE;
        }
        else
        {
            Container_AVLNodePtr left = p->LeftSon();
            if (!left->RightDeeper())
            {
                left->Parent() = p->Parent();
                left->Delete_LL (p, balance);
            }
            else
                left->Rotate_LR(p);
        }
    }
}

template<class NODE, class KEY, class CMP>
inline Container_AVLTreeError Container_AVLTree<NODE,KEY,CMP>::DeleteNode ( const KEY& key, Container_AVLNodePtr& p, SAPDB_Bool& balance )
{
    Container_AVLTreeError rc = AVLTree_Ok;
    if (NULL == p)
    {
        rc      = AVLTree_NotFound;
        balance = SAPDB_FALSE;
    }
    else
    {
        switch (mCmp->Compare(*p->GetKey(), key))
        {
            case 1 :    rc = DeleteNode (key, p->LeftSon(), balance);
                        if (balance)
                            DeleteBalanceLeft(p, balance);
                        break;
            case -1 :   rc = DeleteNode (key, p->RightSon(), balance);
                        if (balance)
                            DeleteBalanceRight (p, balance);
                        break;
            case 0 :    Container_AVLNodePtr curr = p;
                        if (NULL == curr->RightSon())
                        {
                            if (curr->LeftSon() != NULL)
                                curr->LeftSon()->Parent() = p->Parent();
                            p       = curr->LeftSon();
                            balance = SAPDB_TRUE;
                        }
                        else
                        {
                            if (NULL == curr->LeftSon())
                            {
                                if (curr->RightSon() != NULL)
                                    curr->RightSon()->Parent() = p->Parent();
                                p       = curr->RightSon();
                                balance = SAPDB_TRUE;
                            }
                            else
                            {
                                Container_AVLNodePtr deleted;
                                Del (curr->LeftSon(), deleted, balance);
                                deleted->Parent() = curr->Parent();
                                deleted->SetLeftSon( p->LeftSon() );
                                deleted->SetRightSon( p->RightSon() );
                                deleted->Balance()  = p->Balance();
                                p = deleted;
                                if (balance)
                                    DeleteBalanceLeft(p, balance);
                            }
                        }
                        curr->CleanUp(*mAllocatorPtr, *mCmp);
                        destroy(curr, *mAllocatorPtr);
                        --m_NodeCount;
                        ++m_ChangeCount;
                        break;
        }
    }
    return rc;
}

template<class NODE, class KEY, class CMP>
inline void Container_AVLTree<NODE,KEY,CMP>::DeleteSubtree(Container_AVLNodePtr p)
{
    if (NULL != p)
    {
        DeleteSubtree(p->LeftSon());
        DeleteSubtree(p->RightSon());
        p->CleanUp(*mAllocatorPtr, *mCmp);
        destroy(p, *mAllocatorPtr);
        --m_NodeCount;
        ++m_ChangeCount;
    }
}

template<class NODE, class KEY, class CMP>
inline NODE* Container_AVLTree<NODE,KEY,CMP>::InsertNode(const KEY& key, Container_AVLNodePtr& p, const Container_AVLNodePtr& parent, SAPDB_Bool& balance, Container_AVLTreeError& rc)
{
    Container_AVLNodePtr pInserted = NULL;
    if (NULL == p)
    {
        p = new(*mAllocatorPtr) NODE(key);
        if (!p)
        {
            rc = AVLTree_OutOfMemory;
            balance = SAPDB_FALSE;
            return NULL;        }
        else
        {
            rc = AVLTree_Ok;
            ++m_NodeCount;
            ++m_ChangeCount;
            pInserted   = p;
            p->Parent() = parent;
            balance     = SAPDB_TRUE;
        }
    }
    else
    {
        switch (mCmp->Compare(*p->GetKey(), key))
        {
            case 1 :    pInserted = InsertNode (key, p->LeftSon(), p, balance, rc);
                        if (pInserted && balance)
                            BalanceLeft(p, balance);
                        break;
            case -1 :   pInserted = InsertNode (key, p->RightSon(), p, balance, rc);
                        if (pInserted && balance)
                            BalanceRight(p, balance);
                        break;
            case 0 :    rc = AVLTree_DuplicateKey;
                        break;
        }
    }
    return (NODE*) pInserted;
}

template<class NODE, class KEY, class CMP>
inline NODE* Container_AVLTree<NODE,KEY,CMP>::FindNode (const KEY& key) const
{
    Container_AVLNodePtr curr = m_Root;
    while (NULL != curr)
    {
        switch (mCmp->Compare(*curr->GetKey(), key))
        {
            case 1 :    curr = curr->LeftSon();
                        break;
            case -1 :   curr = curr->RightSon();
                        break;
            case 0 :    return (NODE*) curr;
                        break;
        }
    }
    return NULL;
}

/*-----------------------------------------------------------------------------*/
/* Implementation of Container_AVLTree:Iterator                                */
/*-----------------------------------------------------------------------------*/

template<class NODE, class KEY, class CMP>
inline TYPENAME_MEO00 Container_AVLTree<NODE,KEY,CMP>::Iterator Container_AVLTree<NODE,KEY,CMP>::First( void )
{
    Iterator iter;
    iter.m_AVLTree = this;
    iter.SetFirst();

    return iter;
}

template<class NODE, class KEY, class CMP>
inline TYPENAME_MEO00 Container_AVLTree<NODE,KEY,CMP>::Iterator Container_AVLTree<NODE,KEY,CMP>::Last( void )
{
    Iterator iter;
    iter.m_AVLTree = this;
    iter->SetLast();

    return iter;
}

template<class NODE, class KEY, class CMP>
inline TYPENAME_MEO00 Container_AVLTree<NODE,KEY,CMP>::Iterator Container_AVLTree<NODE,KEY,CMP>::Locate( KEY key, Container_AVLTreeIterDirection direction )
{
    Iterator iter;
    iter.m_AVLTree   = this;
    iter.m_direction = direction;
    int cmpResult = 1;

    Container_AVLNodePtr p = m_Root;
    Container_AVLNodePtr lastGoodNode = NULL;
    while (NULL != p && cmpResult != 0)
    {
        lastGoodNode = p;
        cmpResult = mCmp->Compare(*p->GetKey(), key);
        if (cmpResult == 1)
        {
            p = p->LeftSon();
        }
        else
        {
            if (cmpResult == -1)
                p = p->RightSon();
        }
    }

    if (NULL != p)
        iter.SetLocation(p);
    else
        iter.SetLocation(lastGoodNode);

    if (cmpResult != 0 && iter)
    {
        // There is no object with the specified key. Dependend on the parameter
        // position pointer either on the next largest resp. next smallest object.
        if (direction == AVLTree_ASCENDING && cmpResult == -1)
        {
            ++iter;
        }
        else
        {
            if (direction == AVLTree_DESCENDING && cmpResult == 1)
                --iter;
        }
    }
    return iter;
}

template <class KEY, class CMP>
inline void Container_AVLNode<KEY, CMP>::SetRightSon(
    typename Container_AVLNode<KEY, CMP>::Container_AVLNodePtr son)
{
    this->RightSon() = son;
    if (son != NULL)
        son->Parent() = this;
}

template <class KEY, class CMP>
inline void Container_AVLNode<KEY, CMP>::SetLeftSon(
    typename Container_AVLNode<KEY, CMP>::Container_AVLNodePtr son)
{
    this->LeftSon() = son;
    if (son != NULL)
        son->Parent() = this;
}

#endif
