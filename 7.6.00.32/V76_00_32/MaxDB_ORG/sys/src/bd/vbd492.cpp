/*
  @file           vbd492.cpp
  @author         TorstenS
  @ingroup        create index
  @brief          This modul contains the storage area which is used
                  by the create index algorithmto held all primary
                  records to be sorted.

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

#include "gbd492.h"

#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "RunTime/RTE_ISystem.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

cbd492_StorageArea::cbd492_StorageArea( SAPDBMem_IRawAllocator&  allocator )
        :
        m_BlockAllocatorUsed( 0 ),
        m_EmergencyAllocatorUsed( 1 ),
        m_BlockSize( FrameCtrl_ICommon::GetInstance().GetPageSize()),
        m_FirstFree( m_BlockSize ),
        m_BlockAllocator( cbd493_CreateIndexBlockAllocator::GetInstance()),
        m_Blocks( allocator ),
        m_BlockOrigin( allocator )
{}

/*---------------------------------------------------------------------------*/

bool
cbd492_StorageArea::Initialize( const SAPDB_UInt numStartBlocks )
{
    // capacity = numStartBlocks and size = 0
    if( ! m_BlockOrigin.Reserve( numStartBlocks )){
        return false;
    }
    if( ! m_Blocks.Reserve( numStartBlocks )){
        return false;
    }
    Data_PageFrame block;
    if( ! this->NewBlock( block )){
        return false;
    }
    return true;
}

/*---------------------------------------------------------------------------*/

bool
cbd492_StorageArea::Add(
    const SAPDB_Byte* const pSource,
    const SAPDB_UInt        sourceLength,
    SAPDBFields_Field&      destinationField )
{
    SAPDBERR_ASSERT_STATE( 0 != pSource );
    SAPDBERR_ASSERT_STATE( 0 < sourceLength );
    SAPDBERR_ASSERT_STATE( sourceLength <= m_BlockSize );

    if( sourceLength > m_BlockSize ){
        return false;
    }
    if(( m_FirstFree + sourceLength > m_BlockSize ))
    {
        Data_PageFrame block;
        if( ! this->NewBlock( block )){
            return false;
        }
    }
    const  Blocks::SizeType lastUsed = m_Blocks.GetSize()-1;
    Blocks::Iterator        iter     = m_Blocks.Position( lastUsed );

    SAPDB_Byte* pDestination = iter->GetDataPtr() + m_FirstFree;
    SAPDB_MemCopyNoCheck( pDestination, pSource, sourceLength );
    destinationField.Assign( pDestination, sourceLength );
    m_FirstFree +=  SAPDB_ALIGN_2BYTE( sourceLength );
    return true;
}

/*---------------------------------------------------------------------------*/

void
cbd492_StorageArea::Delete()
{
    Blocks::Iterator        iter;
    Blocks::ConstIterator   endIter = m_Blocks.End();

    for( iter = m_Blocks.Begin(); iter != endIter; iter++ )
    {
        SAPDBERR_ASSERT_STATE( iter->IsAssigned());
        const Blocks::IndexType index = m_Blocks.GetIndex( iter );
        const bool bUseBlockAllocator = ( m_BlockAllocatorUsed == m_BlockOrigin[ index ]);
        this->FreeBlock( bUseBlockAllocator, *iter );
    }
    m_Blocks.Delete();
    m_BlockOrigin.Delete();
    m_FirstFree = 0;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool
cbd492_StorageArea::NewBlock( Data_PageFrame& block )
{

    if( m_BlockAllocator.Allocate( block ))
    {
        if( ! m_BlockOrigin.InsertEnd( m_BlockAllocatorUsed )){
            return false;
        }
    }
    else
    {   // Emergency allocation
        SAPDB_Byte* pBlock = reinterpret_cast< SAPDB_Byte* >
                             ( RTE_ISystem::Instance().AllocSystemPages( m_BlockSize ));
        if ( 0 == pBlock ){
            return false;
        }
        if( ! m_BlockOrigin.InsertEnd( m_EmergencyAllocatorUsed )){
            return false;
        }
        block.Assign( pBlock, m_BlockSize );
    }
    if( ! m_Blocks.InsertEnd( block )){
        return false;
    }
    m_FirstFree = 0;
    return true;
}

/*---------------------------------------------------------------------------*/

void
cbd492_StorageArea::FreeBlock(
    const bool        bUseBlockAllocator,
    Data_PageFrame&   block )
{
    if( bUseBlockAllocator ){
        m_BlockAllocator.Deallocate( block );
    }
    else{
        RTE_ISystem::Instance().FreeSystemPages( block.GetDataPtr(), m_BlockSize );
    }
    block.Deassign();
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

