/*!
  @file           FBM_BlockStateList.hpp
  @author         TorstenS
  @author         AlexanderK
  @ingroup        FBM

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


#ifndef FBM_BLOCKSTATELIST_HPP
#define FBM_BLOCKSTATELIST_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FreeBlockManagement/FBM_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "KernelCommon/Kernel_Common.hpp"


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
    \brief     state list for blocks on a volume

    each block state on a volume is represented by 4 bits in the FBM_BlockStateList
*/
class FBM_BlockStateList
{
public:

    /*!
        \brief     constructor
        \param     Allocator [in] allocator
    */
    inline                   FBM_BlockStateList               (SAPDBMem_IRawAllocator &Allocator);

    /// desctructor
    inline                   ~FBM_BlockStateList              ();

    /*!
        \brief      returns the capacity of the list in blocks
        \returns    capacity of the list in blocks
    */
    inline SAPDB_Int4        Capacity                         () const;

    /*!
        \brief      create a new block state list for NumBlocks elements
        \param      NumBlocks [in] number of blocks in the volume
    */
    inline SAPDB_Bool        Create                           (SAPDB_Int4 NumBlocks);

    /// free memory
    inline void              Destroy                          ();

    /// initialize block states
    inline void              SetAllBlockStatesToFree          ();

    /*!
        \brief      return the state of a block
        \param      BlockNo [in] number of the block
        \returns    state of the block
    */
    inline FBM_BlockState    GetBlockState                    (SAPDB_Int4 BlockNo) const;

    /*!
        \brief      return the state of a block after backup
        \param      BlockNo [in] number of the block
        \returns    state of the block after backup
    */
    inline FBM_BlockState    GetBlockStateAfterBackup         (SAPDB_Int4 BlockNo) const;

    /*!
        \brief      sets the state of a block
        \param      BlockNo    [in] number of the block
        \param      BlockState [in] state of the block 
    */
    inline void              SetBlockState                    (SAPDB_Int4 BlockNo, FBM_BlockState BlockState);

    /*!
        \brief      sets the state of a block after backup
        \param      BlockNo [in] number of the block
        \param      BlockState [in] state of the block after backup
    */
    inline void              SetBlockStateAfterBackup         (SAPDB_Int4 BlockNo, FBM_BlockState BlockState);

    /*!
        \brief      transfer state from 'after backup'
        \param      BlockNo [in] number of the block
        \returns    state of the block
    */
    inline FBM_BlockState    RestoreBlockStateMarkedForBackup (SAPDB_Int4 BlockNo);

private:
    SAPDBMem_IRawAllocator &m_Allocator;
    SAPDB_Byte             *m_StateList;
    SAPDB_Int4              m_NumBlocks;

    inline SAPDB_Int4 SizeOfBlockStateList();
};

/*===========================================================================*
 *  DEFINITION OF INLINE METHODS                                             *
 *===========================================================================*/

FBM_BlockStateList::FBM_BlockStateList(SAPDBMem_IRawAllocator &Allocator)
        :
        m_Allocator(Allocator),
        m_StateList(NULL),
        m_NumBlocks(0){}

/*---------------------------------------------------------------------------*/

inline SAPDB_Bool
FBM_BlockStateList::Create (SAPDB_Int4 NumBlocks)
{
    SAPDBERR_ASSERT_ARGUMENT ((0==m_NumBlocks) && (m_StateList == NULL));

    m_NumBlocks = NumBlocks;

    /* allocata memory */
    m_StateList = reinterpret_cast <SAPDB_Byte *> (m_Allocator.Allocate (SizeOfBlockStateList()));

    /* check if memory allocation succeeded */
    if (m_StateList == NULL)
    {
        m_NumBlocks = 0;
        return false;
    }

    /* set all block states in the list to free */
    SetAllBlockStatesToFree();

    return true;
}

/*---------------------------------------------------------------------------*/

inline void
FBM_BlockStateList::Destroy()
{
    SAPDBERR_ASSERT_ARGUMENT (((0!=m_NumBlocks) && (m_StateList != NULL)) ||
                              ((0==m_NumBlocks) && (m_StateList == NULL)));

    /* release memory in case that it was allocated */
    if (m_StateList != NULL)
        m_Allocator.Deallocate (m_StateList);

    m_NumBlocks = 0;
}

/*---------------------------------------------------------------------------*/

inline
FBM_BlockStateList::~FBM_BlockStateList()
{
    Destroy();
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Int4
FBM_BlockStateList::SizeOfBlockStateList()
{
    /* use a half byte to store the state of one block */
    return (m_NumBlocks+1)/2;
}

/*---------------------------------------------------------------------------*/

inline void
FBM_BlockStateList::SetAllBlockStatesToFree()
{
    memset (m_StateList, 0, SizeOfBlockStateList());
}

/*---------------------------------------------------------------------------*/

inline FBM_BlockState
FBM_BlockStateList::GetBlockState (SAPDB_Int4 BlockNo) const
{
    SAPDBERR_ASSERT_ARGUMENT ((0<=BlockNo) && (BlockNo<m_NumBlocks));

    return ( FBM_BlockState )(( BlockNo & 1 ) == 1
                              ? ((m_StateList [ BlockNo >> 1 ]) & 0x0C )>>2
                              : ((m_StateList [ BlockNo >> 1 ]) >> 6   ) );
}

/*---------------------------------------------------------------------------*/

inline FBM_BlockState
FBM_BlockStateList::GetBlockStateAfterBackup (SAPDB_Int4 BlockNo) const
{
    SAPDBERR_ASSERT_ARGUMENT ((0<=BlockNo) && (BlockNo<m_NumBlocks));

    return ( FBM_BlockState )( ( BlockNo & 1 ) == 1
                               ? ((m_StateList [ BlockNo >> 1 ]) & 0x03 )
                               : ((m_StateList [ BlockNo >> 1 ]) & 0x30 )>>4 );
}

/*---------------------------------------------------------------------------*/

inline void
FBM_BlockStateList::SetBlockState (SAPDB_Int4     BlockNo,
                                   FBM_BlockState BlockState)
{
    SAPDBERR_ASSERT_ARGUMENT ((0<=BlockNo) && (BlockNo<m_NumBlocks));

    if ( (BlockNo & 1) == 1 )
    {
        (m_StateList [ BlockNo >> 1 ]) &= 0xF3 ;
        (m_StateList [ BlockNo >> 1 ]) += BlockState<<2;
    }
    else
    {
        (m_StateList [ BlockNo >> 1 ]) &= 0x3F ;
        (m_StateList [ BlockNo >> 1 ]) += BlockState<<6 ;
    }
}

/*---------------------------------------------------------------------------*/

inline void
FBM_BlockStateList::SetBlockStateAfterBackup (SAPDB_Int4     BlockNo,
        FBM_BlockState BlockState)
{
    SAPDBERR_ASSERT_ARGUMENT ((0<=BlockNo) && (BlockNo<m_NumBlocks));

    if ( (BlockNo & 1) == 1 )
    {
        (m_StateList [ BlockNo >> 1 ]) &= 0xFC ;
        (m_StateList [ BlockNo >> 1 ]) += BlockState   ;
    }
    else
    {
        (m_StateList [ BlockNo >> 1 ]) &= 0xCF ;
        (m_StateList [ BlockNo >> 1 ]) += BlockState<<4 ;
    }
}

/*---------------------------------------------------------------------------*/

inline FBM_BlockState
FBM_BlockStateList::RestoreBlockStateMarkedForBackup (SAPDB_Int4 BlockNo)
{
    SAPDBERR_ASSERT_ARGUMENT ((0<=BlockNo) && (BlockNo<m_NumBlocks));

    FBM_BlockState RestoredState = GetBlockStateAfterBackup (BlockNo);
    SetBlockState            (BlockNo, RestoredState);
    SetBlockStateAfterBackup (BlockNo, BlockState_Free);
    return RestoredState;
}

/*---------------------------------------------------------------------------*/

inline SAPDB_Int4
FBM_BlockStateList::Capacity                         () const
{
    return m_NumBlocks;
}

/*---------------------------------------------------------------------------*/

#endif //FBM_BLOCKSTATELIST_HPP
