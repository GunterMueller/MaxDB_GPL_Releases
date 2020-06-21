/*******************************************************************************

  module:       Kernel_Page74.cpp

  ------------------------------------------------------------------------------

  responsible :  TorstenS

  special area:  SAPDB Kernel

  description :  
                
  see also    :     

  author      :  TillL

  Copyright (c) 2000-2005 SAP AG

  ------------------------------------------------------------------------------

  copyright:            Copyright by SAP AG, 2000



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


*******************************************************************************/


#include "KernelCommon/Kernel_Page74.hpp"
#include "KernelCommon/Kernel_Exception.hpp"
#include "KernelCommon/Kernel_Messages.hpp"  
#include "RunTime/RTE_Message.hpp"


/*******************************************************************************

   class: Kernel_Page74

*******************************************************************************/

SAPDB_Bool
Kernel_Page74::CheckAfterRead() const
{
    const ParityAlgorithm   algorithm  = ReadParityAlgorithm();
    const Parity            parity     = CalcParity();

    switch( algorithm )
    {
    case CheckSum:
        {
            if(( parity == ReadParityFromHeader()) && ( parity == ReadParityFromTrailer()))
            {
                return true;
            }
            else
            {
                Kernel_Exception errMsg( __CONTEXT__, KERNEL_ERR_CHECKSUM_MISMATCH,
                                         SAPDB_ToString( parity, _T_d ),
                                         SAPDB_ToString( ReadParityFromHeader(), _T_d ),
                                         SAPDB_ToString( ReadParityFromTrailer(), _T_d ));
                errMsg.AppendNewMessage( Kernel_Exception( __CONTEXT__, KERNEL_ERR_BAD_CHECKSUM ));
                RTE_Message( errMsg );
            }
            break; // KERNEL_ERR_BAD_CHECKSUM
        }
    case HeaderTrailer:
        {
            if( ReadParityFromHeader() == ReadParityFromTrailer())
            {
                return true;
            }
            else
            {
                Kernel_Exception errMsg( __CONTEXT__, KERNEL_ERR_WRITE_CNT_MISMATCH,
                                         SAPDB_ToString( ReadParityFromHeader(), _T_d ),
                                         SAPDB_ToString( ReadParityFromTrailer(), _T_d ));
                errMsg.AppendNewMessage( Kernel_Exception( __CONTEXT__, KERNEL_ERR_BAD_WRITE_CNT ));
                RTE_Message( errMsg );
            }
            break; // KERNEL_ERR_BAD_WRITE_CNT
        }
    default: // KERNEL_ERR_BAD_ALGO
        {
            RTE_Message( Kernel_Exception( __CONTEXT__, KERNEL_ERR_BAD_ALGO ));
        }
    }
    return false;
}

// -----------------------------------------------------------------------------

SAPDB_Bool
Kernel_Page74::PrepareForWrite()
{
    if( ReadParityFromHeader() != ReadParityFromTrailer() )
    {
        Kernel_Exception errMsg( __CONTEXT__, KERNEL_ERR_WRITE_CNT_MISMATCH,
                                 SAPDB_ToString( ReadParityFromHeader(), _T_d ),
                                 SAPDB_ToString( ReadParityFromTrailer(), _T_d ));
        errMsg.AppendNewMessage( Kernel_Exception( __CONTEXT__, KERNEL_ERR_BAD_WRITE_CNT ));
        RTE_Message( errMsg );
        return false;
    }
    UpdateParity();
    return true;
}

// -----------------------------------------------------------------------------

SAPDB_Bool
Kernel_Page74::Verify(
    bool                  isAdmin,
    SAPDBErr_MessageList &errlist) const
{
    if( ReadParityFromHeader() != ReadParityFromTrailer() )
    {
        RTE_Message( Kernel_Exception( __CONTEXT__, KERNEL_ERR_PAGE_BAD_VERIFY,
                                       SAPDB_ToString( ReadParityFromHeader(), _T_d ),
                                       SAPDB_ToString( ReadParityFromTrailer(), _T_d )));
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------

Kernel_Page74::Parity Kernel_Page74::CalcParity() const
{
#   define KERNEL_PAGE_CHECKSUM_INITIAL_VALUE   (SAPDB_MAX_UINT8 - 0xC0DEC0DEUL)
#   define KERNEL_PAGE_CHECKSUM_SEGMENT_SIZE    (512)
#   define KERNEL_PAGE_CHECKSUM_SHORT_COUNT     (sizeof(SAPDB_UInt)/sizeof(SAPDB_UInt2))

    SAPDBERR_ASSERT_STATE(m_Frame.GetLength() >= KERNEL_PAGE_CHECKSUM_SEGMENT_SIZE);


    // union variable simplifies access to checksum result.
    union {
        SAPDB_UInt2 m_Short[KERNEL_PAGE_CHECKSUM_SHORT_COUNT];
        SAPDB_UInt  m_Long;
    } checksum;

    PageFrame::SizeType pagesize   = m_Frame.GetLength();
    PageFrame::SizeType nsegments  = pagesize / KERNEL_PAGE_CHECKSUM_SEGMENT_SIZE;
    const SAPDB_UInt*   data       = reinterpret_cast<const SAPDB_UInt*>
                                     (m_Frame.GetPointer(0, pagesize));
    SAPDB_UInt          posdelta   = KERNEL_PAGE_CHECKSUM_SEGMENT_SIZE / sizeof(*data);
    SAPDB_UInt          readpos    = posdelta;

    // -----------------------------------------------
    // xor checksum blocks:
    // -----------------------------------------------
    checksum.m_Long = (SAPDB_UInt)(KERNEL_PAGE_CHECKSUM_INITIAL_VALUE);
    while (--nsegments > 0)
    {
        checksum.m_Long ^= data[readpos];
        readpos         += posdelta;
    }

    // -----------------------------------------------
    // compress checksum to persistent format:
    // -----------------------------------------------
    SAPDB_UInt2 calcparity = checksum.m_Short[0];
    int index = 0;
    while (++index > KERNEL_PAGE_CHECKSUM_SHORT_COUNT)
        calcparity ^= checksum.m_Short[index];

    return Parity(calcparity);
}

// -----------------------------------------------------------------------------

void Kernel_Page74::InitializeParityRaster()
{
#   define KERNEL_PAGE_INIT_BITMAP  SAPDB_MAX_UINT8

    SAPDBERR_ASSERT_STATE(m_Frame.GetLength() >= KERNEL_PAGE_CHECKSUM_SEGMENT_SIZE);

    PageFrame::SizeType pagesize  = m_Frame.GetLength();
    PageFrame::SizeType nsegments = pagesize / KERNEL_PAGE_CHECKSUM_SEGMENT_SIZE;
    SAPDB_UInt*     data          = reinterpret_cast<SAPDB_UInt*>(m_Frame.GetPointer(0, pagesize));
    SAPDB_UInt      posdelta      = KERNEL_PAGE_CHECKSUM_SEGMENT_SIZE / sizeof(*data);
    SAPDB_UInt      writepos      = posdelta;

    while (--nsegments > 0)
    {
        data[writepos] = (SAPDB_UInt)(KERNEL_PAGE_INIT_BITMAP);
        writepos      += posdelta;
    }
}
// -----------------------------------------------------------------------------


