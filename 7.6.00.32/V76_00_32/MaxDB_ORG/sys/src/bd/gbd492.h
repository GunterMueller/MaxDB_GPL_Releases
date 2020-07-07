/*!
  @file         gbd492.h
  @author       TorstenS
  @ingroup      create index
  @brief        This modul contains the storage area which is used
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


#ifndef GBD492_H
#define GBD492_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gbd493.h"  // cbd493_CreateIndexBlockAllocator

#include "Container/Container_Vector.hpp"
#include "DataAccess/Data_PageFrame.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"


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

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
   @class   cbd492_StorageArea
   @brief   This class implements a simple storage area which is used
            by the index creation to store primary records. The storage
            memory will be extracted from the frame control i.e. data cache.
            If the frame control could not provid more memory the system
            page allocator is used.
 */
class cbd492_StorageArea
{
public:

    /*!
       @brief  constructor
       @param  allocator [in|out] allocator for memory requests in bytes
    */
    cbd492_StorageArea( SAPDBMem_IRawAllocator& allocator );

    /*!
       @brief  destructor returns all allocated memory
    */
    ~cbd492_StorageArea(){
        this->Delete();
    }

    /*!
        @brief  Initialization to manage the given number of memory blocks.
                Note that at first only one memory block is allocated. 
                Further blocks will be allocated on demand due to the call
                of the Add method.

        @param  numStartBlocks [in] number of blocks to be managed per default
        @return (bool) true if successful
    */
    bool Initialize( const SAPDB_UInt numStartBlocks );

    /*!
        @brief  Copies a given source data into the storage area. The storage area 
                cannot store data which is larger than 8192 bytes. It is expected
                that the given source data has at least one byte. The source data
                is stored with a otow byte alignment.
        @param  pSource          [in]  pointer of data to be stored
        @param  sourceLength     [in]  length of data to be stored
        @param  destinationField [out] contains the new location of the given data
        @return (bool) true if data was stored successfuly
    */
    bool Add(
        const SAPDB_Byte* const pSource,
        const SAPDB_UInt        sourceLength,
        SAPDBFields_Field&      destinationField );

    /*!
       @brief  Copies a given source data into the storage area. The storage area 
               cannot store data which is larger than 8192 bytes. It is expected
               that the given source data has at least one byte. The source data
               is stored with a otow byte alignment.
       @param  sourceField      [in]  contains the source data
       @param  destinationField [out] contains the new location of the given data
       @return (bool) true if data was stored successfuly
    */
    bool Add(
        const SAPDBFields_Field&    sourceField,
        SAPDBFields_Field&          destinationField )
    {
        return this->Add( sourceField.GetDataPtr(),
                          sourceField.GetLength(), destinationField );
    }

    /*!
        @brief  Returns all allocated memory
    */
    void Delete();

private:

    bool NewBlock( Data_PageFrame& block );

    void FreeBlock(
        const bool      bUseBlockAllocator,
        Data_PageFrame& block );

private:

    // Assignment operator is not supported
    cbd492_StorageArea& operator=( const cbd492_StorageArea &sta );

    // Copy constructor is not supported
    cbd492_StorageArea( const cbd492_StorageArea &sta );

private:

    typedef Container_Vector< Data_PageFrame > Blocks;
    typedef Container_Vector< SAPDB_UInt1 > BlockOrigin;


    // see m_BlockOrigin
    const SAPDB_UInt1   m_BlockAllocatorUsed;

    // see m_BlockOrigin
    const SAPDB_UInt1   m_EmergencyAllocatorUsed;

    // instance wide block size
    const SAPDB_UInt    m_BlockSize;

    // between 0 and 8192; 8192 indicates out of range!
    SAPDB_UInt  m_FirstFree;

    // base allocator to get chunks of "m_BlockSize" blocks
    cbd493_CreateIndexBlockAllocator&   m_BlockAllocator;

    // collection of blocks, which are allocated
    Blocks      m_Blocks;

    // corresponding member data to m_Blocks; value m_BlockAllocatorUsed
    // means that frame was allocated by m_BlockAllocator else
    // frame was allocated by emergency allocator (RTE_ISystem)
    BlockOrigin  m_BlockOrigin;
};

/*===========================================================================*
 *  DEFINITION OF METHODS                                                    *
*===========================================================================*/

#endif  /* GBD492_H */

