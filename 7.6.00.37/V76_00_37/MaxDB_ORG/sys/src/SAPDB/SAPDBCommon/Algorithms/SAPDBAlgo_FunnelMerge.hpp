/*!
    @file     SAPDBAlgo_FunnelMerge.hpp
    @ingroup  SAPDBAlgo
    @author   DirkT
    @brief    Funnel merge - merge of n sorted sources

\if EMIT_LICENCE
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

\endif
*/


#ifndef SAPDBALGO_FUNNELMERGE_HPP
#define SAPDBALGO_FUNNELMERGE_HPP

// -----------------------------------------------------------------------------

#include "SAPDBCommon/SAPDB_Types.hpp"

#include "SAPDBCommon/Algorithms/SAPDBAlgo_Comparator.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SAPDBAlgo.h"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

// -----------------------------------------------------------------------------

/*!
    @class          SAPDBAlgo_IMergeSource
    @brief          Interface for the sources to be merged by the funnel merge
 */
template <class TData>
class SAPDBAlgo_IMergeSource
{
public:
    /*!
       @brief   Returns the number of sources to be merged
       @return  unsigned int 
    */
    virtual unsigned int GetListCount( void ) = 0;
    /*!
       @brief   Get the current element of the source specified by SourceIndex. Must not be called if the source is invalid.
       @param   SourceIndex [in]
       @return  TData&
    */
    virtual TData& GetListElement( unsigned int SourceIndex ) = 0;
    /*!
       @brief   Go to the next element of the source specified by SourceIndex. Must not be called if the source is already invalid.
       @param   SourceIndex [in]
       @return  TRUE if the source is still valid, else FALSE
    */
    virtual bool NextListElement( unsigned int SourceIndex ) = 0;
    /*!
       @brief   Returns wheater or not the source specified by SourceIndex is valid
       @param   SourceIndex [in]
       @return  TRUE if valid, else FALSE
    */
    virtual bool Valid( unsigned int SourceIndex ) = 0;
};

/*!
    @class          SAPDBAlgo_IMergeTarget
    @brief          Interface for the target to be used by the funnel merge
 */
template <class TData>
class SAPDBAlgo_IMergeTarget
{
public:
    /*!
       @brief   Appends Data to the target
       @param   Data [in]
       @return  TRUE if successfull, else FALSE
    */
    virtual bool Append( TData& Data ) = 0;
};

// -----------------------------------------------------------------------------

/*!
    @class          SAPDBAlgo_MergeNode
    @brief          Node to be used by SAPDBAlgo_FunnelQueue
 */
template <class TData>
class SAPDBAlgo_MergeNode
{
public:
    /// Stored data
    TData         Data;
    /// Source Data belongs to
    unsigned int  SourceNo;
};

/*!
    @class          SAPDBAlgo_FunnelMergeIterator
    @brief          Iterate over all elements the merge sources contain. In contrast to SAPDBAlgo_FunnelMergeIteratorIdx
                    this iterator makes use of SAPDBAlgo_MergeNode. This means that data of each source will
                    be copied when rearranging the funnelqueue but accessing will be fast as source methods need not
                    be called when the elements have to be accessed.
    @see            SAPDBAlgo_FunnelMergeIteratorIdx
 */
template <class TData, class TSource=SAPDBAlgo_IMergeSource<TData>, class TComparator=SAPDBAlgo_IComparator<TData> >
class SAPDBAlgo_FunnelMergeIterator
{
public:
    /*!
       @brief   Constructor
       @param   Allocator   [in]
       @param   Source      [in]
       @param   Comparator  [in]
       @param   ErrorList   [out]
    */
    SAPDBAlgo_FunnelMergeIterator(  SAPDBMem_IRawAllocator& Allocator,
                                    TSource&                Source,
                                    TComparator&            Comparator,
                                    Msg_List&               ErrorList)
        : mSource(Source), mFunnelQueue(Allocator, Source, Comparator, ErrorList)
    {
        if ( mFunnelQueue.CompletelyConstructed() )
        {
            for ( unsigned int i=0; i<mSource.GetListCount(); i++ )
            {
                if ( mSource.Valid(i) )
                {
                    SAPDBAlgo_MergeNode<TData> Node;
                    Node.SourceNo   = i;
                    Node.Data       = mSource.GetListElement(i);
                    mFunnelQueue.Insert(Node);
                }
            }
        }
    }

    /*!
        @brief  Destructor - does nothing
    */
    ~SAPDBAlgo_FunnelMergeIterator( void ) { }

    /*!
        @brief  Returns wheather or not all dynamic allocations during constuction were successfull
        @return TRUE is all allocations were successfull, else FALSE
    */
    inline bool CompletelyConstructed( void )
    {
        return mFunnelQueue.CompletelyConstructed();
    }

    /*!
        @brief  Returns wheather or not the iterator is valid
        @return TRUE if valid, else FALSE
    */
    inline bool IsValid( void )    
    { 
        return (mFunnelQueue.GetSize() > 0); 
    }

    /*!
        @brief  Dereferences funnel merge iterator
        @return Reference to actual data

     */
    inline TData& operator*( void )
    { 
        return mFunnelQueue.TopEntry().Data; 
    }

    /*!
        @brief  Prefix increment; advances iterator to next data
        @return SAPDBAlgo_FunnelMergeIterator& *this
     */
    inline SAPDBAlgo_FunnelMergeIterator& operator++()
    {
        SAPDBAlgo_MergeNode<TData> N;
        N.SourceNo = mFunnelQueue.TopEntry().SourceNo;

        if ( mSource.NextListElement(N.SourceNo) )
        {
            N.Data = mSource.GetListElement(N.SourceNo);
		    mFunnelQueue.ReplaceTopEntry(N);
        }
		else
		    mFunnelQueue.RemoveTopEntry();

        return *this;
    }

private:

    /*!
        @class  SAPDBAlgo_FunnelQueue
        @brief  Priority queue to be used by SAPDBAlgo_FunnelMergeIterator
    */
    class SAPDBAlgo_FunnelQueue
    {
    public:
        /*!
            @brief   Constructor
            @param   Allocator   [in]
            @param   Source      [in]
            @param   Comparator  [in]
            @param   ErrorList   [out]
        */
        SAPDBAlgo_FunnelQueue(  SAPDBMem_IRawAllocator& Allocator,
                                TSource&                Source,
                                TComparator&            Comparator,
                                Msg_List&               ErrorList)
            : mAllocator(Allocator)
            , mSource(Source)
            , mComparator(Comparator)
            , mQueue(0)
            , mCapacity(0)
            , mSize(0)
        {
            mCapacity = Source.GetListCount();
            if ( !(mQueue = newarray(mQueue, mCapacity, mAllocator)) )
            {
                ErrorList = Msg_List(Msg_List::Error, SDBMSG_SAPDBALGO_FUNNELMERGE_NOQUEUE);
            }
        }

        /*!
            @brief  Destructor
        */
        ~SAPDBAlgo_FunnelQueue( void )
        {
            if ( mQueue )
                destroyarray(mQueue, mCapacity, mAllocator);
        }

        /*!
            @brief  Returns wheather or not all dynamic allocations during constuction were successfull
            @return TRUE is all allocations were successfull, else FALSE
        */
        inline bool CompletelyConstructed( void )
        {
            return (mQueue!=0);
        }

        /*!
            @brief  Get the size of the queue
            @return unsigned int
        */
        inline unsigned int GetSize( void )
        {
            return mSize;
        }

        /*!
            @brief  Insert Source into the funnel queue
            @param  Source [in]
            @return TRUE if successsfull, else FALSE
        */
        inline SAPDB_Bool Insert( const SAPDBAlgo_MergeNode<TData>& Source )
        {
            SAPDBERR_ASSERT_STATE( mQueue );

            if ( mSize  >= mCapacity )
                return false;

            mQueue[mSize] = Source;
            ++mSize;
            MoveUp(mSize-1);
            Check();

            return true; 
        }

        /*!
            @brief  Returns a reference to the first entry within the funnel queue
            @return SAPDBAlgo_MergeNode<TData>&
        */
        inline SAPDBAlgo_MergeNode<TData>& TopEntry( void )
        {
            SAPDBERR_ASSERT_STATE( mQueue );

            return mQueue[0];
        }

        /*!
            @brief  Remove the top entry from the funnel queue
        */
        inline void RemoveTopEntry( void )
        {
            SAPDBERR_ASSERT_STATE( mQueue );

            if (mSize==0)
                return;

            if (--mSize)
            {
                mQueue[0] = mQueue[mSize];
                MoveDownTopEntry();
            }
            Check();
        }

        /*!
            @brief  Replace the top entry of the funnel queue by D
            @param  D [in]
        */
        inline void ReplaceTopEntry( const SAPDBAlgo_MergeNode<TData>& D )
        {
            SAPDBERR_ASSERT_STATE( mQueue );

            mQueue[0] = D;
            MoveDownTopEntry();
            Check();
        }

    private:

        inline void Check( void )
        {
#if defined (SAPDB_SLOW)
            for ( unsigned int i=1; i<mSize; i++ )
            {
                SAPDBERR_ASSERT_STATE( !mComparator.IsPredecessor(mQueue[i].Data, mQueue[(i-1)/2].Data) );
            }
#endif
        }

        /*!
            @brief  Move up the k-th entry of the funnel queue to the correct position
        */
        inline void MoveUp( unsigned int k )
        {
            SAPDBERR_ASSERT_STATE( mQueue );

            /* element to be moved up */
            SAPDBAlgo_MergeNode<TData> D = mQueue[k];

            while ( k != 0 )
            { /* has a parent */

                /* parent node */
                unsigned int j = (k-1) / 2;

                if ( !mComparator.IsPredecessor(D.Data, mQueue[j].Data) )
                { /* heap condition fulfilled */
    	            break;
                }

                /* move down parent */
                mQueue[k] = mQueue[j];
                k = j;
            }

            mQueue[k] = D;
        }

        /*!
            @brief  Move down the first entry of the funnel queue to the correct position
        */
        inline void MoveDownTopEntry( void )
        {
            SAPDBERR_ASSERT_STATE( mQueue );

            /* to be moved down */
            unsigned int k = 0;
            SAPDBAlgo_MergeNode<TData> D = mQueue[k];

            while ( k < (mSize/2) )
            {  
            /* Node 'k' has children. The condition we intuitively might
	            want to test is '1 + 2*k < usedSize'. But this is the proper
	            condition only in an infinite cyclic ring and we must take care of
	            overflow in our finite cyclic ring. */
            	   
	            /* first child */
	            unsigned int j = 1 + 2*k;
            	   
	            if ( j < (mSize - 1) )      // 'usedSize' may not be 0 here. 
                    if ( mComparator.IsPredecessor(mQueue[1+j].Data, mQueue[j].Data) ) 
	                { /* there is a child '1+j' and it is smaller than child 'j' */
		                ++j;
	                }
            	   
                if ( !mComparator.IsPredecessor(mQueue[j].Data, D.Data) )
	            { /* heap condition fulfilled */
		            break;
	            }
            	   
	            /* move up smallest child */
	            mQueue[k] = mQueue[j];
            	   
	            k = j;
            }
               
            mQueue[k] = D;
        }

        /// Allocator used to allocate the queue
        SAPDBMem_IRawAllocator&     mAllocator;
        /// Source to merge
        TSource&                    mSource;
        /// Comparator to compare the data
        TComparator&                mComparator;
        /// Pointer to the queue
        SAPDBAlgo_MergeNode<TData>* mQueue;
        /// Actual size of the queue
        unsigned int                mSize;
        /// Capacity of the queue
        unsigned int                mCapacity;

    };  // class SAPDBAlgo_FunnelQueue

    // ---

    /// Funnel queue
    SAPDBAlgo_FunnelQueue   mFunnelQueue;
    /// Source to merge
    TSource&                mSource;

};  // class SAPDBAlgo_FunnelMergeIterator

// -----------------------------------------------------------------------------

/*!
    @class          SAPDBAlgo_FunnelMergeIteratorIdx
    @brief          Iterate over all elements the merge sources contain. In contrast to SAPDBAlgo_FunnelMergeIterator
                    this iterator does not make use of SAPDBAlgo_MergeNode. This means that data of each source will
                    not be copied when rearranging the funnelqueue but accessing the source should be fast
    @see            SAPDBAlgo_FunnelMergeIterator
 */
template <class TData, class TSource=SAPDBAlgo_IMergeSource<TData>, class TComparator=SAPDBAlgo_IComparator<TData> >
class SAPDBAlgo_FunnelMergeIteratorIdx
{
public:
    /*!
       @brief   Constructor
       @param   Allocator   [in]
       @param   Source      [in]
       @param   Comparator  [in]
       @param   ErrorList   [out]
    */
    SAPDBAlgo_FunnelMergeIteratorIdx(  SAPDBMem_IRawAllocator&  Allocator,
                                       TSource&                 Source,
                                       TComparator&             Comparator,
                                       Msg_List&                ErrorList)
        : mSource(Source), mFunnelQueue(Allocator, Source, Comparator, ErrorList)
    {
        if ( mFunnelQueue.CompletelyConstructed() )
        {
            for ( unsigned int i=0; i<mSource.GetListCount(); i++ )
            {
                if ( mSource.Valid(i) )
                    mFunnelQueue.Insert(i);
            }
        }
    }

    /*!
        @brief  Destructor - does nothing
    */
    ~SAPDBAlgo_FunnelMergeIteratorIdx( void ) { }

    /*!
        @brief  Returns wheather or not all dynamic allocations during constuction were successfull
        @return TRUE is all allocations were successfull, else FALSE
    */
    inline bool CompletelyConstructed( void )
    {
        return mFunnelQueue.CompletelyConstructed();
    }

    /*!
        @brief  Returns wheather or not the iterator is valid
        @return TRUE if valid, else FALSE
    */
    inline bool IsValid( void )    
    { 
        return (mFunnelQueue.GetSize() > 0); 
    }

    /*!
       @brief   Dereferences funnel merge iterator
       @return  Reference to actual data

     */
    inline TData& operator*( void )
    {
        return mSource.GetListElement(mFunnelQueue.TopEntry());
    }

    /*!
        @brief  Prefix increment; advances iterator to next data
        @return SAPDBAlgo_FunnelMergeIterator& *this
     */
    inline SAPDBAlgo_FunnelMergeIteratorIdx& operator++()
    {
        if ( mSource.NextListElement(mFunnelQueue.TopEntry()) )
	        mFunnelQueue.ReplaceTopEntry();
		else
            mFunnelQueue.RemoveTopEntry();

        return *this;
    }

private:

    /*!
        @class  SAPDBAlgo_FunnelQueueIdx
        @brief  Priority queue to be used by SAPDBAlgo_FunnelMergeIteratorIdx
    */
    class SAPDBAlgo_FunnelQueueIdx
    {
    public:
        /*!
            @brief   Constructor
            @param   Allocator   [in]
            @param   Source      [in]
            @param   Comparator  [in]
            @param   ErrorList   [out]
        */
        SAPDBAlgo_FunnelQueueIdx(  SAPDBMem_IRawAllocator&  Allocator,
                                TSource&                    Source,
                                TComparator&                Comparator,
                                Msg_List&                   ErrorList)
            : mAllocator(Allocator)
            , mSource(Source)
            , mComparator(Comparator)
            , mQueue(0)
            , mCapacity(0)
            , mSize(0)
        {
            mCapacity = Source.GetListCount();
            if ( !(mQueue = newarray(mQueue, mCapacity, mAllocator)) )
            {
                ErrorList = Msg_List(Msg_List::Error, SDBMSG_SAPDBALGO_FUNNELMERGE_NOQUEUE);
            }
        }

        /*!
            @brief  Destructor
        */
        ~SAPDBAlgo_FunnelQueueIdx( void )
        {
            if ( mQueue)
               destroyarray(mQueue, mCapacity, mAllocator);
        }

        /*!
            @brief  Returns wheather or not all dynamic allocations during constuction were successfull
            @return TRUE is all allocations were successfull, else FALSE
        */
        inline bool CompletelyConstructed( void )
        {
            return (mQueue!=0);
        }

        /*!
            @brief  Get the size of the queue
            @return unsigned int
        */
        inline unsigned int GetSize( void )
        {
            return mSize;
        }

        /*!
            @brief  Insert SourceNo into the funnel queue
            @param  SourceNo [in]
            @return TRUE if successsfull, else FALSE
        */
        inline SAPDB_Bool Insert( unsigned int SourceNo )
        {
            SAPDBERR_ASSERT_STATE( mQueue );

            if ( mSize  >= mCapacity )
                return false;

            mQueue[mSize] = SourceNo;
            ++mSize;
            MoveUp(mSize-1);
            Check();

            return true; 
        }

        /*!
            @brief  Returns the SourceNo of the first entry within the funnel queue
            @return unsigned int
        */
        inline unsigned int TopEntry( void )
        {
            SAPDBERR_ASSERT_STATE( mQueue );

            return mQueue[0];
        }

        /*!
            @brief  Remove the top entry from the funnel queue
        */
        inline void RemoveTopEntry( void )
        {
            SAPDBERR_ASSERT_STATE( mQueue );

            if (mSize==0)
                return;

            if (--mSize)
            {
                mQueue[0] = mQueue[mSize];
                MoveDownTopEntry();
            }
            Check();
        }

        /*!
            @brief  Replace the top entry of the funnel queue 
        */
        inline void ReplaceTopEntry( void )
        {
            SAPDBERR_ASSERT_STATE( mQueue );

            MoveDownTopEntry();
            Check();
        }

    private:

        inline void Check( void )
        {
#if defined (SAPDB_SLOW)
            for ( unsigned int i=1; i<mSize; i++ )
            {
                SAPDBERR_ASSERT_STATE( !mComparator.IsPredecessor(mSource.GetListElement(mQueue[i]), mSource.GetListElement(mQueue[(i-1)/2])) );
            }
#endif
        }

        /*!
            @brief  Move up the k-th entry of the funnel queue to the correct position
        */
        inline void MoveUp( unsigned int k )
        {
            SAPDBERR_ASSERT_STATE( mQueue );

            /* element to be moved up */
            unsigned int SourceIdx = mQueue[k];

            while ( k != 0 )
            { /* has a parent */

                /* parent node */
                unsigned int j = (k-1) / 2;

                if ( !mComparator.IsPredecessor(mSource.GetListElement(SourceIdx), mSource.GetListElement(mQueue[j])) )
                { /* heap condition fulfilled */
    	            break;
                }

                /* move down parent */
                mQueue[k] = mQueue[j];
                k = j;
            }

            mQueue[k] = SourceIdx;
        }

        /*!
            @brief  Move down the first entry of the funnel queue to the correct position
        */
        inline void MoveDownTopEntry( void )
        {
            SAPDBERR_ASSERT_STATE( mQueue );

            /* to be moved down */
            unsigned int k = 0;
            unsigned int SourceIdx = mQueue[k];

            while ( k < (mSize/2) )
            {  
            /* Node 'k' has children. The condition we intuitively might
	            want to test is '1 + 2*k < usedSize'. But this is the proper
	            condition only in an infinite cyclic ring and we must take care of
	            overflow in our finite cyclic ring. */
            	   
	            /* first child */
	            unsigned int j = 1 + 2*k;
            	   
	            if ( j < (mSize - 1) )      // 'usedSize' may not be 0 here. 
                    if ( mComparator.IsPredecessor(mSource.GetListElement(mQueue[1+j]), mSource.GetListElement(mQueue[j])) ) 
	                { /* there is a child '1+j' and it is smaller than child 'j' */
		                ++j;
	                }

				if ( !mComparator.IsPredecessor(mSource.GetListElement(mQueue[j]), mSource.GetListElement(SourceIdx)) )
	            { /* heap condition fulfilled */
		            break;
	            }
            	   
	            /* move up smallest child */
	            mQueue[k] = mQueue[j];
            	   
	            k = j;
            }
               
            mQueue[k] = SourceIdx;
        }

        /// Allocator used to allocate the queue
        SAPDBMem_IRawAllocator& mAllocator;
        /// Source to merge
        TSource&                mSource;
        /// Comparator to compare the data
        TComparator&            mComparator;
        /// Pointer to the queue
        unsigned int*           mQueue;
        /// Actual size of the queue
        unsigned int            mSize;
        /// Capacity of the queue
        unsigned int            mCapacity;

    };  // class SAPDBAlgo_FunnelQueueIdx

    // ---

    /// Funnel queue
    SAPDBAlgo_FunnelQueueIdx    mFunnelQueue;
    /// Source to merge
    TSource&                    mSource;

};  // class SAPDBAlgo_FunnelMergeIteratorIdx

// -----------------------------------------------------------------------------

/// Functions to merge into a given target

template <class TData, class TSource, class TTarget, class TComparator>
bool SAPDBAlgo_FunnelMerge( SAPDBMem_IRawAllocator& Allocator,
                            TSource&                Source,
                            TTarget&                Target,
                            TComparator&            Comparator,
                            Msg_List&               ErrorList)
{
    SAPDBAlgo_FunnelMergeIterator<TData, TSource, TComparator> Iter(Allocator, Source, Comparator, ErrorList);

    if ( Iter.CompletelyConstructed() )
    {
        while ( Iter.IsValid() && Target.Append(*Iter++) );
        return true;
    }
    else
    {
        ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_SAPDBALGO_FUNNELMERGE_FAILED);
        return false;
    }
}   // SAPDBAlgo_FunnelMerge

template <class TData>
bool SAPDBAlgo_FunnelMerge( SAPDBMem_IRawAllocator&         Allocator,
                            SAPDBAlgo_IMergeSource<TData>&  Source,
                            SAPDBAlgo_IMergeTarget<TData>&  Target,
                            SAPDBAlgo_IComparator<TData>&   Comparator,
                            Msg_List&                       ErrorList)
{
    return SAPDBAlgo_FunnelMerge<TData, SAPDBAlgo_IMergeSource<TData>, SAPDBAlgo_IMergeTarget<TData>, SAPDBAlgo_IComparator<TData> >(Allocator, Source, Target, Comparator, ErrorList);
}   // SAPDBAlgo_FunnelMerge

template <class TData, class TSource, class TTarget, class TComparator>
bool SAPDBAlgo_FunnelMergeIdx(  SAPDBMem_IRawAllocator&     Allocator,
                                TSource&                    Source,
                                TTarget&                    Target,
                                TComparator&                Comparator,
                                Msg_List&                   ErrorList)
{
    SAPDBAlgo_FunnelMergeIteratorIdx<TData, TSource, TComparator> Iter(Allocator, Source, Comparator, ErrorList);

    if ( Iter.CompletelyConstructed() )
    {
        while ( Iter.IsValid() && Target.Append(*Iter++) );
        return true;
    }
    else
    {
        ErrorList = ErrorList + Msg_List(Msg_List::Error, SDBMSG_SAPDBALGO_FUNNELMERGE_FAILED);
        return false;
    }
}   // SAPDBAlgo_FunnelMergeIdx

template <class TData>
bool SAPDBAlgo_FunnelMergeIdx(  SAPDBMem_IRawAllocator&         Allocator,
                                SAPDBAlgo_IMergeSource<TData>&  Source,
                                SAPDBAlgo_IMergeTarget<TData>&  Target,
                                SAPDBAlgo_IComparator<TData>&   Comparator,
                                Msg_List&                       ErrorList)
{
    return SAPDBAlgo_FunnelMerge<TData, SAPDBAlgo_IMergeSource<TData>, SAPDBAlgo_IMergeTarget<TData>, SAPDBAlgo_IComparator<TData> >(Allocator, Source, Target, Comparator, ErrorList);
}   // SAPDBAlgo_FunnelMergeIdx

// -----------------------------------------------------------------------------

#endif