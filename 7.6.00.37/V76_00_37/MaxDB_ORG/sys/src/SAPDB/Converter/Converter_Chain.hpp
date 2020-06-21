/*!
  @file           Converter_Chain.hpp
  @author         TorstenS
  @author         TillL
  @ingroup        Converter
  @brief          Doubly linked chain of entries

  - Used in converter map and converter index.
  - Does not perform memory allocation.
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2000-2004 SAP AG

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


#ifndef CONVERTER_CHAIN_HPP
#define CONVERTER_CHAIN_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"


/*!
   @class          Converter_ChainNode
   @brief          Node of converter chain

   - Arguments: T    link type
   (must be derived from template SAPDB_Invalidatable&lt;>)
 */

template <class T>
class Converter_ChainNode
{
public:

    /// my linktype
    typedef T LinkType;

    /*!
       @brief          Default constructor
       @return         none
     */
    Converter_ChainNode()
    {}

    /*!
       @brief          Sets successor
       @param          succ [in] successor to be set
       @return         none
     */
    void SetSuccessor( const LinkType   &succ )
    {
#ifdef SAPDB_QUICK
        if(( m_Successor.IsValid()) && ( succ.IsValid()))
            SAPDBERR_ASSERT_STATE(m_Successor != succ );
#endif
        m_Successor = succ;
    }

    /*!
       @brief          Sets predecessor
       @param          pred [in] predecessor to be set
       @return         none
     */
    void SetPredecessor( const LinkType &pred )
    {
        m_Predecessor = pred;
    }

    /*!
       @brief          Returns predecessor
       @return         (const LinkType&amp;) link to predecessor
     */
    const LinkType& GetPredecessor() const
    {
        return m_Predecessor;
    }

    /*!
       @brief          Returns successor
       @return         (const LinkType&amp;) link to successor
     */
    const LinkType& GetSuccessor() const
    {
        return m_Successor;
    }

    /*!
       @brief          Assigns an invalid value
       @return         none
     */
    void Invalidate()
    {
        m_Predecessor.Invalidate();
        m_Successor.Invalidate();
    }

    /*!
       @brief          Checks whether actual node is invalid
       @return         (SAPDB_Bool) true if invalid
     */
    SAPDB_Bool IsInvalid() const
    {
        return m_Predecessor.IsInvalid() && m_Successor.IsInvalid();
    }

private:

    ///  link to preceding node
    LinkType                        m_Predecessor;

    ///  link to successive node
    LinkType                        m_Successor;
};


/*!
   @class          Converter_ChainIteratorTPL
   @brief          (do not use directly)

   - Arguments: NodeContainer  container of nodes
     (must provide a method
     NodeType&amp; GetNode(const LinkType&amp;)
     returning the node associated with a given link)
     NodeType node type (must be derived from template Converter_ChainNode&lt;>)
 */

template <class NodeSequence, class NodeType>
class Converter_ChainIteratorTPL
{
public:

    /// ...
    typedef typename NodeType::LinkType         LinkType;
    /// iterator
    typedef Converter_ChainIteratorTPL
         <NodeSequence, NodeType>               ChainIterator;

    /*!
       @brief          Constructor
       @param          nodes [in] reference to node sequence
       @param          link [in] reference to link in the chain
       @return         none
     */
    Converter_ChainIteratorTPL
    (NodeSequence&                  nodes,
     const LinkType&                link)
    :
    m_pNodes(&nodes),
    m_Pos(link)
    {}

    /*!
       @brief          Compares with other iterator
       @param          iter [in] other
       @return         (SAPDB_Bool) true if unequal
     */
    SAPDB_Bool operator!=(const ChainIterator& iter) const
    {
        SAPDBERR_ASSERT_ARGUMENT(m_pNodes == iter.m_pNodes);
        return m_Pos != iter.m_Pos;
    }

    /*!
       @brief          Dereferences the iterator
       @return         (NodeType&amp;) reference to actual node
     */
    NodeType& operator*() const
    {
        SAPDBERR_ASSERT_STATE(m_Pos.IsValid());
        return (*m_pNodes)[m_Pos];
    }

    /*!
       @brief          Prefix increment; advances iterator
       @return         (ChainIterator&amp;) this instance
     */
    ChainIterator& operator++()
    {
        SAPDBERR_ASSERT_STATE(m_Pos.IsValid());
        m_Pos = (*m_pNodes)[m_Pos].GetSuccessor();
        return *this;
    }

    /*!
       @brief          Prefix decrement; pushes back iterator
       @return         (ChainIterator&amp;) this instance
     */
    ChainIterator& operator--()
    {
        SAPDBERR_ASSERT_STATE(m_Pos.IsValid());
        m_Pos = (*m_pNodes)[m_Pos].GetPredecessor();
        return *this;
    }

    /*!
       @brief          Returns actual link
       @return         (LinkType) actual link
     */
    LinkType GetPosition() const
    {
        return m_Pos;
    }

protected:
    ///  node sequence in iterator
    NodeSequence*                  m_pNodes;

    ///  current position
    LinkType                        m_Pos;
};




/*!
   @class          Converter_Chain
   @brief          Node of converter chain

   - Arguments: see Converter_ChainIterator
 */
template <class NodeSequence, class NodeType>
class Converter_Chain
{
public:

    /// ...
    typedef typename NodeType::LinkType               LinkType;

    /// number of nodes
    typedef SAPDB_UInt                                NodeCount;

    /// local iterator
    typedef Converter_ChainIteratorTPL
            <NodeSequence, NodeType>                  Iterator;

    /// local iterator
    typedef Converter_ChainIteratorTPL
            <NodeSequence, const NodeType>            ConstIterator;

    /*!
       @brief          Default constructor
       @return         none
     */
    Converter_Chain()
    :
    m_Nodes(0),            // m_Head, m_Tail auto-initialize to "invalid"
    m_NodeCount(0)
    {}

    /*!
       @brief          Constructor
       @param          nodes [in] reference to node sequence
       @return         none
     */
    Converter_Chain( NodeSequence   &nodes )
    :
    m_Nodes(&nodes),
    m_Head(LinkType()),
    m_Tail(LinkType()),
    m_NodeCount(0)
    {}

    /*!
       @brief          Assigns new sequence
       @param          nodes [in] reference to node sequence
       @return         none
     */
    void Assign( NodeSequence   &nodes )
    {
        m_Nodes = &nodes;
        Reset();
    }

    /*!
       @brief          Initializes head and tail; sets node count to 0
       @return         none
     */
    void Reset()
    {
        m_Head      = LinkType();
        m_Tail      = LinkType();
        m_NodeCount = 0;
    }

    /*!
       @brief          Inserts a node at the beginning of the chain
       @param          link [in] link to node to be inserted
       @return         (Iterator) iterator pointing to inserted node
     */
    Iterator InsertFront( const LinkType    &link )
    {
        SAPDBERR_ASSERT_STATE( ! IsMember( link ));

        // -------------------------------------------------------------------------
        // get node handles
        // -------------------------------------------------------------------------
        NodeType & node     = GetNode(link);

        // -------------------------------------------------------------------------
        // chain in the node at the beginning of the list
        // -------------------------------------------------------------------------
        if (m_Head.IsValid())
        {
            NodeType & head     = GetNode(m_Head);
            node.SetSuccessor   (m_Head);
            node.SetPredecessor (link);   // points to itself
            head.SetPredecessor (link);
            m_Head = link;
        }
        else // list yet empty
        {
            node.SetSuccessor   (LinkType());
            node.SetPredecessor (link);   // points to itself
            m_Head = link;
            m_Tail = link;
        }

        ++m_NodeCount;

        SAPDBERR_ASSERT_STATE( IsMember( link ));

        return Begin();
    }

    /*!
       @brief          Inserts a node at the end of the chain
       @param          link [in] link to node to be inserted
       @return         (Iterator) iterator pointing to inserted node
     */
    Iterator InsertEnd( const LinkType  &link )
    {
        SAPDBERR_ASSERT_STATE( ! IsMember( link ));

        // -------------------------------------------------------------------------
        // get node handles
        // -------------------------------------------------------------------------
        NodeType& node = GetNode(link);

        // -------------------------------------------------------------------------
        // chain in the node at the end of the list
        // -------------------------------------------------------------------------
        if (m_Tail.IsValid())
        {
            NodeType& tail = GetNode(m_Tail);

            node.SetPredecessor (m_Tail);
            node.SetSuccessor   (LinkType());
            tail.SetSuccessor   (link);
            m_Tail = link;
        }
        else // list yet empty
        {
            node.SetPredecessor (link);   // points to itself
            node.SetSuccessor   (LinkType());
            m_Tail = link;
            m_Head = link;
        }

        ++m_NodeCount;

        SAPDBERR_ASSERT_STATE( IsMember( link ));

        return Iterator( *m_Nodes, m_Tail );;
    }

    /*!
       @brief          Removes the first element of the chain
       @return         (Iterator) iterator pointing to element behind removed element
                         or invalid iterator if next element doesn't exist

       - The chain must not be empty.
     */
    Iterator RemoveFront()
    {
        SAPDBERR_ASSERT_STATE(!IsEmpty());

        NodeType& node    = GetNode(m_Head);

        // -------------------------------------------------------------------------
        // special case: only one node in the list
        // -------------------------------------------------------------------------
        if (m_NodeCount == 1)
        {
            m_Head.Invalidate();
            m_Tail.Invalidate();
        }
        // -------------------------------------------------------------------------
        // at least 2 nodes in the list
        // -------------------------------------------------------------------------
        else
        {
            NodeType& nextnode = GetNode(node.GetSuccessor());
            m_Head             = node.GetSuccessor();
            nextnode.SetPredecessor(m_Head);    // points to itself
        }
        node.Invalidate();
        --m_NodeCount;

        return Iterator(*m_Nodes, m_Head);
    }

    /*!
       @brief          Removes the last element of the chain
       @return         (Iterator) invalid iterator (pointing behind tail)

       - The chain must not be empty.
     */

    Iterator RemoveEnd()
    {
        SAPDBERR_ASSERT_STATE(!IsEmpty());

        NodeType& node    = GetNode(m_Tail);

        // -------------------------------------------------------------------------
        // special case: only one node in the list
        // -------------------------------------------------------------------------
        if (m_NodeCount == 1)
        {
            m_Head.Invalidate();
            m_Tail.Invalidate();
        }
        // -------------------------------------------------------------------------
        // at least 2 nodes in the list
        // -------------------------------------------------------------------------
        else
        {
            NodeType& prevnode = GetNode(node.GetPredecessor());
            m_Tail             = node.GetPredecessor();
            prevnode.SetSuccessor(node.GetSuccessor());
        }
        node.Invalidate();
        --m_NodeCount;

        return Iterator(*m_Nodes, LinkType());
    }

    /*!
       @brief          Removes the node at a given iterator position
       @param          link [in] link to node to be removed
       @return         (Iterator) iterator pointing to element behind removed element
                         or invalid iterator if next element doesn't exist
     */
    Iterator Remove( const LinkType &link )
    {
        SAPDBERR_ASSERT_STATE( !IsEmpty() );
        SAPDBERR_ASSERT_ARGUMENT( link.IsValid() );
        SAPDBERR_ASSERT_STATE( IsMember( link ));

        // -------------------------------------------------------------------------
        // check whether RemoveFront() can be used
        // -------------------------------------------------------------------------
        if (link == m_Head)
        {
            // Remove the first chain element
            return RemoveFront();
        }
        // -------------------------------------------------------------------------
        // check whether RemoveEnd() can be used
        // -------------------------------------------------------------------------
        else
            if (link == m_Tail)
            {
                // Remove the last list element
                return RemoveEnd();
            }
        // -------------------------------------------------------------------------
        // delete element from the middle of the list
        // -------------------------------------------------------------------------
            else
            {
                // at this point, we are sure that there is a precedessor to the node
                // indicated by 'link' in the chain
                // -------------------------------------------------------------------------
                // get node handles
                // -------------------------------------------------------------------------
                NodeType& node     = GetNode(link);
                NodeType& prevnode = GetNode(node.GetPredecessor());

                // -------------------------------------------------------------------------
                // chain the node out of the list
                // -------------------------------------------------------------------------
                prevnode.SetSuccessor(node.GetSuccessor());
                if (node.GetSuccessor().IsValid())
                {
                    NodeType& nextnode = GetNode(node.GetSuccessor());
                    nextnode.SetPredecessor(node.GetPredecessor());
                }
                LinkType succ = node.GetSuccessor();
                node.Invalidate();
                --m_NodeCount;

                SAPDBERR_ASSERT_STATE( ! IsMember( link ));

                return Iterator(*m_Nodes, succ);
            }
    }

    /* -----------------------------------------------------------------------*/
    /*! @name Status functions */

    /*!
       @brief          Checks whether chain is empty
       @return         (bool) true if empty
     */
    bool IsEmpty() const
    {
        return m_Head.IsInvalid();
    }

    /*!
       @brief          Returns the actual node count of the chain
       @return         (NodeCount) node count
     */
    NodeCount GetNodeCount() const
    {
        return m_NodeCount;
    }

    /* -----------------------------------------------------------------------*/

    /*! @name ConstIterator functions */

    /*!
       @brief          Returns an iterator to the first chain node
       @return         (Iterator/ConstIterator)
     */
    Iterator Begin()
    {
        return Iterator(*m_Nodes, m_Head);
    }

    ConstIterator Begin() const
    {
        return ConstIterator(*m_Nodes, m_Head);
    }

    /*!
       @brief          Returns an iterator behind the last chain node
       @return         (Iterator/ConstIterator)
     */
    Iterator End()
    {
        return Iterator(*m_Nodes, LinkType());
    }

    ConstIterator End() const
    {
        return ConstIterator(*m_Nodes, LinkType());
    }

    /*!
      @brief  Checks whether the given node is member of the chain.
      @param  link [in]   link to node to be checked
      @return (SAPDB_Bool) true if node is member else false
    */

    SAPDB_Bool IsMember( const LinkType   &link )
    {
        Iterator        iter        = Begin();
        Iterator        endIter     = End();
        NodeCount       loopCnt     = 0;
        const NodeCount maxEntryCnt = ( *m_Nodes ).GetSize();

        for( ; iter != endIter; ++iter, ++loopCnt )
        {
            if( link == iter.GetPosition() )
                return( SAPDB_TRUE );

            SAPDBERR_ASSERT_STATE(  loopCnt <  maxEntryCnt );
        }
        return( SAPDB_FALSE );
    }

private:

    /*-----------------------------------------------------------------------
    function:     GetNode()
    description:  Returns the requested node
    arguments:    link [in]   link to node to be requested
    return value: NodeType
    ------------------------------------------------------------------------*/

    NodeType& GetNode( const LinkType   &link )
    {
        return (*m_Nodes)[link];
    }

    /*--------------------------------------------------------------------------
    declaration : m_Nodes
    description:  link to node sequence
    --------------------------------------------------------------------------*/
    NodeSequence*                   m_Nodes;

    /*--------------------------------------------------------------------------
    declaration : m_NodeCount
    description:  count of nodes in chain
    --------------------------------------------------------------------------*/
    NodeCount                       m_NodeCount;

    /*--------------------------------------------------------------------------
    declaration : m_Head
    description:  chain head
    --------------------------------------------------------------------------*/
    LinkType                        m_Head;

    /*--------------------------------------------------------------------------
    declaration : m_Head
    description:  chain tail
    --------------------------------------------------------------------------*/
    LinkType                        m_Tail;
};

#endif // CONVERTER_CHAIN_HPP
