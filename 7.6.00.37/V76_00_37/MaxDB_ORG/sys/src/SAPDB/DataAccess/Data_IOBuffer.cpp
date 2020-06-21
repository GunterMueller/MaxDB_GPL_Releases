  /*!
  @file           Data_IOBuffer.cpp
  @author         UweH
  @brief          used for blocked io 

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

#include "DataAccess/Data_IOBuffer.hpp"
#include "IOManager/IOMan_IDataInfo.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"

/* --------------------------------------------------------------------------- */
bool Data_IOBuffer::Initialize(SAPDB_Int4        ioBufferSize,
                               SAPDB_Byte* const ioBuffer)
{

    if ( IOMan_IDataInfo::GetInstance().GetDataIOBlockCount() * FrameCtrl_ICommon::GetInstance().GetPageSize()
         !=
         ioBufferSize )
        return false;

    m_Frame.Assign( ioBuffer, ioBufferSize );
    return true;
}
