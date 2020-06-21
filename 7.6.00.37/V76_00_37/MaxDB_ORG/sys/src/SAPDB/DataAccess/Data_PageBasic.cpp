/*!
@file           Data_PageBasic.cpp
@author         UweH
@ingroup        PageHandling
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
#include "DataAccess/Data_PageBasic.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "RunTime/RTE_Message.hpp"

// -----------------------------------------------------------------------------
void Data_PageBasic::FrameIsNotValid(SAPDB_Char  const * const  filename,
                                     SAPDB_UInt4 const          linenumber)
{
	RTE_Crash(Data_Exception(filename,linenumber,DATA_PAGE_BASIC_FRAME_NOT_VALID));
}
// -----------------------------------------------------------------------------
SAPDB_Bool Data_PageBasic::CheckAfterRead() const
{
    const CheckAlgorithm algorithm  = ReadCheckAlgorithm();
    const Parity         parity     = CalcParity();

	if ( ! CheckTrailer() )
    {
        RTE_Message(Data_Exception(__CONTEXT__, DATA_PAGE_BASIC_CHECK_TRAILER));
        return false;
    }

	if ( (algorithm == NoCheck)
         ||
         (algorithm == CheckSumCheck 
         &&
         parity == HeaderPtr()->parity) )
    {
        return true;
    }

    RTE_Message( Data_Exception(__CONTEXT__, DATA_PAGE_BASIC_CHECK_AFTER_READ_FAILED,
                                (algorithm==CheckSumCheck?"CheckSum":(SAPDB_Char const *)SAPDB_ToString(algorithm)),
                                SAPDB_ToString(parity),
                                SAPDB_ToString(HeaderPtr()->parity) ) );
    return false;
}

// -----------------------------------------------------------------------------
SAPDB_Bool Data_PageBasic::PrepareForWrite()
{
	if ( ! CheckTrailer() )
    {
        RTE_Message(Data_Exception(__CONTEXT__, DATA_PAGE_BASIC_CHECK_TRAILER));
        return false;
    }
    UpdateParity();
    return true;
}

// -----------------------------------------------------------------------------

Data_PageBasic::Parity Data_PageBasic::CalcParity() const
{
#   define CHECKSUM_INITIAL_VALUE   (SAPDB_MAX_UINT8 - 0xC0DEC0DEUL)
#   define CHECKSUM_SEGMENT_SIZE    (512)
#   define CHECKSUM_SHORT_COUNT     (sizeof(SAPDB_UInt)/sizeof(SAPDB_UInt2))

    SAPDBERR_ASSERT_STATE(m_Frame.GetLength() >= CHECKSUM_SEGMENT_SIZE);

    // union variable simplifies access to checksum result.
    union {
        SAPDB_UInt2 m_Short[CHECKSUM_SHORT_COUNT];
        SAPDB_UInt  m_Long;
    } checksum;

    PageFrame::SizeType pagesize   = m_Frame.GetLength();
    PageFrame::SizeType nsegments  = pagesize / CHECKSUM_SEGMENT_SIZE;
    const SAPDB_UInt*   data       = reinterpret_cast<const SAPDB_UInt*>
                                         (m_Frame.GetPointer(0, pagesize));
    SAPDB_UInt          posdelta   = CHECKSUM_SEGMENT_SIZE / sizeof(*data);
    SAPDB_UInt          readpos    = posdelta;

    // -----------------------------------------------
    // xor checksum blocks:
    // -----------------------------------------------
    checksum.m_Long = (SAPDB_UInt)(CHECKSUM_INITIAL_VALUE);
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
    while (++index > CHECKSUM_SHORT_COUNT)
        calcparity ^= checksum.m_Short[index];

    return Parity(calcparity);
}

// -----------------------------------------------------------------------------

void Data_PageBasic::InitializeParityRaster()
{
#   define INIT_BITMAP  SAPDB_MAX_UINT8

    SAPDBERR_ASSERT_STATE(m_Frame.GetLength() >= CHECKSUM_SEGMENT_SIZE);

    PageFrame::SizeType pagesize  = m_Frame.GetLength();
    PageFrame::SizeType nsegments = pagesize / CHECKSUM_SEGMENT_SIZE;
    SAPDB_UInt*     data          = reinterpret_cast<SAPDB_UInt*>(m_Frame.GetPointer(0, pagesize));
    SAPDB_UInt      posdelta      = CHECKSUM_SEGMENT_SIZE / sizeof(*data);
    SAPDB_UInt      writepos      = posdelta;

    while (--nsegments > 0)
    {
        data[writepos] = (SAPDB_UInt)(INIT_BITMAP);
        writepos      += posdelta;
    }
}
// -----------------------------------------------------------------------------


