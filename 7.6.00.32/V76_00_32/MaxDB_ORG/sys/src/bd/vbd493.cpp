/*
  @file     vbd493.cpp
  @author   TorstenS
  @ingroup  create index
  @brief    This modul contains the create index block allocator
            singleton which is used for memory management during
            parallel index creation.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gbd493.h"

#include "FrameControl/FrameCtrl_ICreateIndex.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

class cbd493_Lock
{
public:

    // get exclusive lock
    cbd493_Lock( RTESync_IRWRegion& rwRegion ) : m_RWRegion( rwRegion )
    {
        m_IsLocked = true;
        m_RWRegion.enter( true );
    }

    // release exclusive lock
    ~cbd493_Lock(){
        this->Unlock();
    }

    // release exclusive lock
    void Unlock()
    {
        if( m_IsLocked )
        {
            m_RWRegion.leave( true );
            m_IsLocked = false;
        }
    }

private:

    // Assignment operator is not supported
    cbd493_Lock& operator=( const cbd493_Lock &myLock );

    // Copy constructor is not supported
    cbd493_Lock( const cbd493_Lock &myLock );

private:

    RTESync_IRWRegion& m_RWRegion;
    bool    m_IsLocked;
};

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

cbd493_CreateIndexBlockAllocator* cbd493_CreateIndexBlockAllocator::m_Instance = 0;

/*---------------------------------------------------------------------------*/

bool
cbd493_CreateIndexBlockAllocator::CreateInstance()
{
    if( 0 != m_Instance ){
        return true;
    }
    m_Instance = new( RTEMem_Allocator::Instance())cbd493_CreateIndexBlockAllocator();
    if( 0 == m_Instance ){
        return false;
    }
    return m_Instance->Initialize( RTEMem_Allocator::Instance());
}

/*---------------------------------------------------------------------------*/

bool
cbd493_CreateIndexBlockAllocator::Allocate( Data_PageFrame& block )
{
    RTETask_ITask  &task = *RTETask_ITask::Self();

    if( 0 == m_FreeBlockCount ){
        return this->GetBlockFromBaseAllocator( task, block );
    }
    {
        cbd493_Lock lockedScope( this->GetRegion());

        if( 0 == m_FreeBlockCount )
        {
            lockedScope.Unlock();
            return this->GetBlockFromBaseAllocator( task, block );
        }
        return this->GetBlockFromFreeList( lockedScope, block );
    }
}

/*---------------------------------------------------------------------------*/

void
cbd493_CreateIndexBlockAllocator::Deallocate( Data_PageFrame& block )
{
    Block* pBlock = reinterpret_cast< Block* >( block.GetDataPtr());
    {
        cbd493_Lock lockedScope( this->GetRegion());

        pBlock->pNextBlock = reinterpret_cast< Block* >( m_FirstBlock.GetDataPtr());
        m_FirstBlock.Assign( block );
        m_FreeBlockCount+=1;
    }
}

/*---------------------------------------------------------------------------*/

void
cbd493_CreateIndexBlockAllocator::EmptyAll()
{
    RTETask_ITask  &task = *RTETask_ITask::Self();
    Data_PageFrame block;
    do
    {
        cbd493_Lock lockedScope(  this->GetRegion());

        const bool bAvailable = this->GetBlockFromFreeList( lockedScope, block );
        if( ! bAvailable ){
            return;
        }
        lockedScope.Unlock();
        m_BlockAllocator.FreeIndexFrame( task, block );
    }while( true );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

cbd493_CreateIndexBlockAllocator::cbd493_CreateIndexBlockAllocator()
        :
        m_SpinLock( UTF8( "CreateIndexBlockAllocator" ), 1 ),
        m_pRwLock( 0 ),
        m_BlockSize( FrameCtrl_ICommon::GetInstance().GetPageSize()),
        m_FreeBlockCount( 0 ),
        m_BlockAllocator( FrameCtrl_ICreateIndex::GetInstance())
{}

/*---------------------------------------------------------------------------*/

bool
cbd493_CreateIndexBlockAllocator::GetBlockFromBaseAllocator(
    RTETask_ITask&  task,
    Data_PageFrame& block ) const
{
    block = m_BlockAllocator.NewIndexFrame( task );
    return block.IsAssigned();
}

/*---------------------------------------------------------------------------*/

bool
cbd493_CreateIndexBlockAllocator::GetBlockFromFreeList(
    const cbd493_Lock&  lockedScope,
    Data_PageFrame&     block )
{
    if( 0 >= m_FreeBlockCount ){
        return false;
    }
    SAPDBERR_ASSERT_STATE( m_FirstBlock.IsAssigned());

    block.Assign( m_FirstBlock );
    m_FreeBlockCount-=1;

    if( 0 == m_FreeBlockCount ){
        m_FirstBlock.Deassign(); // last free block is gone
        return true;
    }
    // update free chain
    Block* const pBlock = reinterpret_cast< Block* >( block.GetDataPtr());
    Block* const pNextFree = pBlock->pNextBlock;
    SAPDBERR_ASSERT_STATE( 0 != pNextFree );
    Data_PageFrame::Pointer ptr = reinterpret_cast< Data_PageFrame::Pointer >( pNextFree );
    m_FirstBlock.Assign( ptr, m_BlockSize );
    return true;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

