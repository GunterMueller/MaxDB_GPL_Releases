/*
  @file         gbd493.h
  @author       TorstenS
  @ingroup      create index
  @brief        This modul contains the create index block allocator
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


#ifndef GBD493_H
#define GBD493_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_PageFrame.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/

class SAPDBMem_IRawAllocator;
class FrameCtrl_ICreateIndex;
class RTETask_ITask;
class cbd493_Lock;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!
   @class   cbd493_CreateIndexBlockAllocator
   @brief   This class implements a create index block allocator singleton which 
            is used for memory management during parallel index creation. The 
            allocator uses the FrameCtrl_ICreateIndex interface of component 
            frame control. Note the allocator holds the frames as long as the 
            EmptyAll method is called which gives all memory back to the 
            underlying frame control resp. data cache.
 */
class cbd493_CreateIndexBlockAllocator
{
public:

    /*!
       @brief   static method to create a singelton of the type cbd493_CreateIndexBlockAllocator
       @return  (bool) 
    */
    static  bool CreateInstance();

    /*!
     @brief   returns the reference to the sigelton instance of cbd493_CreateIndexBlockAllocator
     @return  (Converter_Converter&) 
    */
    static cbd493_CreateIndexBlockAllocator& GetInstance()
    {
        SAPDBERR_ASSERT_STATE( 0 != m_Instance );
        return *m_Instance;
    }

    /*!
        @brief  Provides a memory block which complies to the page size of
                component frame control.
        @param  block [in|out] which is used to return the allocated memory
        @return (bool) true if allocate was successful
    */
    bool Allocate( Data_PageFrame& block );

    /*!
        @brief  Frees the given memory block. The memory block is not given
                back to the underlying allocator (frame control)
        @param  block [in] block to be released
    */
    void Deallocate( Data_PageFrame& block );

    /*!
        @brief  Returns all memory of the allocator back to the underlying 
                allocator (frame control). After using memory of this allocator 
                the user must call this method to ensure that all allocated 
                memory is given back to the commonality.

    */
    void EmptyAll();

private:

    // ctor
    cbd493_CreateIndexBlockAllocator();

    // create read/writer lock
    bool Initialize( SAPDBMem_IRawAllocator  &allocator ){
        m_pRwLock = RTESync_CreateRWRegion( 1, m_SpinLock, allocator );
        return 0 != m_pRwLock;
    }

    // get block from underlying allocator,i.e. frame control
    bool GetBlockFromBaseAllocator(
        RTETask_ITask&  task,
        Data_PageFrame& block ) const;

    // get block from free list
    bool GetBlockFromFreeList(
        const cbd493_Lock&  lock,
        Data_PageFrame&     block );

    RTESync_IRWRegion&  GetRegion(){
        return *m_pRwLock;
    }

private:

    // Assignment operator is not supported
    cbd493_CreateIndexBlockAllocator& operator=( const cbd493_CreateIndexBlockAllocator &ciba );

    // Copy constructor is not supported
    cbd493_CreateIndexBlockAllocator( const cbd493_CreateIndexBlockAllocator &ciba );

private:

    struct Block {
        Block *pNextBlock;
    };

    // Contains pointer of the cbd493_CreateIndexBlockAllocator singleton
    static cbd493_CreateIndexBlockAllocator  *m_Instance;

    // used by cbd493_Lock
    RTESync_SpinlockPool    m_SpinLock;
    RTESync_IRWRegion*      m_pRwLock;

    // instance wide block size
    const SAPDB_UInt m_BlockSize;

    // number of free blocks
    SAPDB_UInt m_FreeBlockCount;

    // Underlying allocator
    FrameCtrl_ICreateIndex& m_BlockAllocator;

    // anchor of free block chain
    Data_PageFrame  m_FirstBlock;

};

/*===========================================================================*
 *  DEFINITION OF METHODS                                                    *
*===========================================================================*/

#endif  /* GBD493_H */

