/*!
  @file           Converter_PageNoContainer.hpp
  @author         TorstenS
  @author         AlexanderK
  @ingroup        Converter
  @brief          Container which holds all page numbers that belong
                  to a back up
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end */


#ifndef CONVERTER_PAGENOCONTAINER_HPP
#define CONVERTER_PAGENOCONTAINER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "DataAccess/Data_Types.hpp"
#include "Converter/Converter_Types.hpp"
#include "Container/Container_Vector.hpp"
#include "Converter/Converter_FlagInfo.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"

/*!
  @class Converter_PageNoContainer
  @brief Container which holds all couple of page numbers in a compressed way
 */

typedef Container_Vector<Converter_FlagInfo::FlagInfoData>      FlagInfoBlocks;

// size of allocation unit: 8kb
#define ALLOCATE_SIZE (1024*8)

class Converter_PageNoContainer
{

public:

    /*!
    @brief  Deletes container structures; deallocates memory
    @return none
    */
    void Delete()
    {
        m_FlagInfoBlockAnchorList.Delete();
        while (m_FlagInfoBlocksContainer.GetSize() != 0)
        {
            destroy(m_FlagInfoBlocksContainer[0], m_Allocator);
            m_FlagInfoBlocksContainer.DeleteFront();
        }
        m_FlagInfoBlocksContainer.Delete();
        m_NumUsedFlagInfoBlocks = 0;
        m_NumFlagInfoBlocks     = 0;
        m_PageNoCount           = 0;
    }

    /*!
       @brief   Constructor
       @param   allocator [in] allocator for container resources
       @return  none
    */
    Converter_PageNoContainer( SAPDBMem_IRawAllocator &allocator )
    :
    m_FlagInfoBlockAnchorList( allocator ),
    m_FlagInfoBlocksContainer( allocator ),
    m_Allocator(allocator)
    {
        Delete();
    }

    /*!
    @brief   Initializes container which discribes all page numbers. NOTE that it is 
             assumed that only one tasks initialize the list therefore no synchronization 
             is done by this class!
    @param   errMsg                    [out] detailed info about memory consumption
    @param   maxPageNo                  [in] maximum page number to be stored in the container
    @param   numFlagInfoBlocks          [in]
    @param   numPageNosPerFlagInfoBlock [in]
    @return  Converter_ReturnCode 
             - Converter_MemoryAllocationFailed (+ errMsg)
             - Converter_Ok
     */

    Converter_ReturnCode  Initialize(
        Msg_List          &errMsg,
        const SAPDB_UInt  maxPageNo,
        const SAPDB_UInt  numFlagInfoBlocks,
        const SAPDB_UInt  numPageNosPerFlagInfoBlock );

    /*!
    @brief  Inserts a page number into the container. If the give pageNo is already in the
            container the return code Converter_DuplicatePageNo is set. NOTE that it is 
            assumed that only one tasks inserts therefore no synchronization is done by 
            this class!
    @param  pageNo [in] page number to insert
    @return Converter_ReturnCode
            - Converter_DuplicatePageNo
            - Converter_Ok
    */
    Converter_ReturnCode Insert( const Data_PageNo  pageNo );

    /*!
    @brief  Checks whether container is empty
    @return (SAPDB_Bool) true if empty
    */
    SAPDB_Bool IsEmpty() const{
        return 0 == m_PageNoCount;
    }

    /*!
    @brief  Returns total number of pageNo's stored in container
    @return (SAPDB_UInt) number of pagenos stored in container
    */
    SAPDB_UInt GetPageNoCount() const{
        return m_PageNoCount;
    }

    /*!
    @brief  Removes the given pageNo from the container. NOTE that it is 
            assumed that only one tasks rmoves entries from the list
            therefore no synchronization is done by this class!
            The Remove method assumes that the given pageNo is from a
            not trustable source therefore the pageNo validity is checked.
    @param  pageNo [in] page number to be removed
    @return (SAPDB_Bool) true if pageno found in container and removed
    */

    SAPDB_Bool Remove( const Data_PageNo  PageNo );

private: 

    /*!
    @brief  Checks whether container is initialized
    @return (SAPDB_Bool) true if initialized
    */

    SAPDB_Bool IsInitialized() const{
        return (m_FlagInfoBlockAnchorList.GetSize() > 0) && (m_FlagInfoBlocksContainer.GetSize() > 0) ;
    }

    /*!
        @brief  Calculates the number of the FlagInfoBlock the pageNo belongs too.
                No validity check is executed!
        @param  pageNo [in] page number to be used for calculation
        @return  (SAPDB_UInt) blockNo
    */

    SAPDB_UInt CalculateFlagInfoBlockNo( const Data_PageNo  pageNo ) const{
        return pageNo/m_NumPageNosPerFlagInfoBlock;
    }
    /*!
        @brief  Calculates the offset in a given FlagInfoBlock, identified
                by the flagInfoBlockNo. No validity check is executed!
        @param  pageNo          [in] page number to be used for calculation
        @param  flagInfoBlockNo [in] identifier of the FlagInfoBlock
        @return (SAPDB_UInt) blockNo
    */

    SAPDB_UInt CalculateFlagInfoBlockOffset( 
        const Data_PageNo  pageNo,
        const SAPDB_UInt   flagInfoBlockNo ) const
    {
        return pageNo - flagInfoBlockNo * m_NumPageNosPerFlagInfoBlock;
    }

    SAPDB_UInt CalculateNumFlagInfoContainer(
        const SAPDB_UInt  numFlagInfoBlocks,
        const SAPDB_UInt  sizeOfFlagInfoBlock,
              SAPDB_UInt &blocksPerContainer)
    {
        blocksPerContainer = ALLOCATE_SIZE / (sizeOfFlagInfoBlock * sizeof (Converter_FlagInfo::FlagInfoData));
        if (0 == numFlagInfoBlocks)
        {
            return 0;
        }
        else
        { 
            return (numFlagInfoBlocks - 1) / blocksPerContainer + 1;
        }
    }

private: 

    /// The AnchorList contains the entrance point for the subsequent m_FlagInfoBlocks.
    /// The number of AnchorList entries depends from the number of used pageNos
    /// at the time of creating the Converter_PageNoContainer. Note that it is not 
    // necessary that all anchors are set!
    Container_Vector<Converter_FlagInfo::FlagInfoData *>    m_FlagInfoBlockAnchorList;
    
    /// Collection of all FlagInfoBlocks. Each FlagInfoBlock is used to mark all
    /// pageNos of one corresponding converter leaf page. Normally one FlagInfoBlock
    /// is used to mark 1861 pageNos of one converte leaf page
    Container_Vector<FlagInfoBlocks*>                       m_FlagInfoBlocksContainer;

    /// Gives the number of pageNos which are part of the list. With each insert the number
    /// is incremented and each remove decrements the number.
    RTESync_InterlockedCounter<SAPDB_UInt>  m_PageNoCount;
    
    /// Number of actual used FlagInfoBlocks.
    SAPDB_UInt  m_NumUsedFlagInfoBlocks;

    /// Total number of FlagInfoBlock. This number corresponds to the
    /// number of used converter leaf pages at the time of creating the 
    // Converter_PageNoContainer.

    SAPDB_UInt  m_NumFlagInfoBlocks;
    
    /// Number of pages which could be addressed by one FlagInfoBlock. A FlagInfoBlock
    /// is used to address all pages of one converter leaf page
 
    SAPDB_UInt  m_NumPageNosPerFlagInfoBlock;
    
    /// Is the aligned size of a FlagInfoBlock which is used to hold all
    /// pages which are adressable by a converter leaf page
    SAPDB_UInt  m_SizeOfFlagInfoBlock;

    /// memory allocation for all FlagInfoBlocks is split into
    /// containers with n FlagInfoBlocks per container to fit the ALLOCATE_SIZE
    SAPDB_UInt  m_BlocksPerContainer;
    
    /// number of FlagInfoBlock container
    SAPDB_UInt  m_NumBlockContainer;

    /// allocator
    SAPDBMem_IRawAllocator &m_Allocator;
};


#endif  /* CONVERTER_PAGENOCONTAINER_HPP */

