/*!
  @file           Converter_PageNoContainer.cpp
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


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define ALIGN(LEN) (((((LEN) - 1) >> 1) + 1 ) << 1)

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Converter/Converter_PageNoContainer.hpp"

#include "SAPDBCommon/Messages/SDBMsg_Converter.h"


/*===========================================================================*
 *  METHODS OF THE BackUpPageNoContainer CLASS                                           *
 *===========================================================================*/

Converter_ReturnCode
Converter_PageNoContainer::Insert( const Data_PageNo  pageNo )
{
    SAPDBERR_ASSERT_STATE (IsInitialized());

    /* find the flag info which contains the flag to switch on for the given page number */
    const SAPDB_UInt blockNo     = CalculateFlagInfoBlockNo( pageNo );
    const SAPDB_UInt blockOffset = CalculateFlagInfoBlockOffset( pageNo, blockNo );

    Converter_FlagInfo::FlagInfoData *  pFlagInfoBlock = m_FlagInfoBlockAnchorList[ blockNo ];

    if( NULL == pFlagInfoBlock )
    {
        SAPDBERR_ASSERT_STATE (m_NumUsedFlagInfoBlocks < m_NumFlagInfoBlocks)

        SAPDB_UInt container = m_NumUsedFlagInfoBlocks / m_BlocksPerContainer;
        SAPDB_UInt containerOffset = m_NumUsedFlagInfoBlocks - (container * m_BlocksPerContainer);
        pFlagInfoBlock = &((*m_FlagInfoBlocksContainer[ container ])[containerOffset * m_SizeOfFlagInfoBlock ]);
        ++m_NumUsedFlagInfoBlocks;
        m_FlagInfoBlockAnchorList[ blockNo ] = pFlagInfoBlock;
    }

    Converter_FlagInfo flagInfo( pFlagInfoBlock, blockOffset );

    if( flagInfo.FlagIsSet( pageNo ))
        return Converter_DuplicatePageNo;

    flagInfo.SetFlag( pageNo ); // insert the page number

    m_PageNoCount.Increment();

    return Converter_Ok;
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_PageNoContainer::Initialize(
    Msg_List          &errMsg,
    const SAPDB_UInt  maxPageNo,
    const SAPDB_UInt  numFlagInfoBlocks,  // = number of converter pages
    const SAPDB_UInt  numPageNosPerFlagInfoBlock )
{
    /* check current state of the container */
    SAPDBERR_ASSERT_STATE (! IsInitialized());

    /* prmitive consistency check of  input parameter */
    SAPDBERR_ASSERT_STATE (((maxPageNo/numPageNosPerFlagInfoBlock)+1) >= numFlagInfoBlocks);

    m_NumFlagInfoBlocks          = numFlagInfoBlocks;
    m_NumPageNosPerFlagInfoBlock = numPageNosPerFlagInfoBlock;

    /* calulate the number of FlagInfo in a block required to accomodate   */
    /* numPageNosPerFlagInfoBlock page numbers. align the flag info blocks */
    /* to avoid inteferences during the paralell removing of page numbers  */
    m_SizeOfFlagInfoBlock = ALIGN (Converter_FlagInfo::FlagInfoBlockNo(m_NumPageNosPerFlagInfoBlock) + 1);

    /* check 8 byte alignment of flag info blocks */
    SAPDBERR_ASSERT_STATE (((m_SizeOfFlagInfoBlock * sizeof (Converter_FlagInfo::FlagInfoData)) % 8) == 0);


    m_NumBlockContainer = CalculateNumFlagInfoContainer(m_NumFlagInfoBlocks,  m_SizeOfFlagInfoBlock, m_BlocksPerContainer);

    const SAPDB_UInt allocSize = CalculateFlagInfoBlockNo( maxPageNo )+ 1;  // anchor

    const bool bOkay1 = m_FlagInfoBlockAnchorList.Resize( allocSize, 0 );
    const bool bOkay2 = m_FlagInfoBlocksContainer.Reserve( m_NumBlockContainer );

    bool bOkay3 = true;
    SAPDB_UInt blocks = m_NumFlagInfoBlocks;
    for (SAPDB_UInt container = 0; (container < m_NumBlockContainer ) && bOkay3; container++)
    {
        SAPDB_UInt allocateBlocks = SAPDB_MIN(blocks, m_BlocksPerContainer);
        FlagInfoBlocks *ptr = new (m_Allocator) FlagInfoBlocks(m_Allocator);
        bOkay3 = (NULL != ptr) && (0 != blocks) ;
        if (bOkay3)
        {
            bOkay3 = ptr->Initialize(allocateBlocks * m_SizeOfFlagInfoBlock, 0);
            blocks -= allocateBlocks;
            bOkay3 &= m_FlagInfoBlocksContainer.InsertEnd(ptr);
        }
    }

    if( ! ( bOkay1 && bOkay2 && bOkay3 ))
    {
        Delete(); // release probably used memory

        errMsg = Msg_List(
                     Msg_List::Error,
                     SDBMSG_CONVERTER_BAD_ALLOC_PNOLIST,
                     Msg_Arg( SDBMSGTAG_CONVERTER_BAD_ALLOC_PNOLIST__ALLOC_SIZE,
                               allocSize ),
                     Msg_OptArg(SDBMSGTAG_CONVERTER_BAD_ALLOC_PNOLIST__MAXUSEDDATAPAGENO,
                                maxPageNo ),
                     Msg_OptArg( SDBMSGTAG_CONVERTER_BAD_ALLOC_PNOLIST__NUMUSEDCONVPAGES,
                                 m_NumFlagInfoBlocks )
                 );

        return Converter_MemoryAllocationFailed;
    }
    return Converter_Ok;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_PageNoContainer::Remove( const Data_PageNo  pageNo )
{
    SAPDBERR_ASSERT_STATE (IsInitialized());

    /* find the flag info which contains the flag to switch on for the given page number */
    const SAPDB_UInt blockNo     = CalculateFlagInfoBlockNo( pageNo );
    const SAPDB_UInt blockOffset = CalculateFlagInfoBlockOffset( pageNo, blockNo );

    if( blockNo >= m_FlagInfoBlockAnchorList.GetSize())
        return false; // given pageNo is out of range

    Converter_FlagInfo::FlagInfoData*  pFlagInfoBlock =  m_FlagInfoBlockAnchorList[ blockNo ];
    
    if( NULL == pFlagInfoBlock )
        return false; // given pageno is in range, but not part of the list

    Converter_FlagInfo flagInfo( pFlagInfoBlock, blockOffset );

    if( ! flagInfo.FlagIsSet( pageNo ))
        return false; // given pageno is in range, but not part of the list

    /* remove the page number */
    flagInfo.DelFlag( pageNo );

    m_PageNoCount.Decrement();

    return true;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
