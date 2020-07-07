  /*!
  @file           Data_IOBuffer.hpp
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
#ifndef DATA_IOBUFFER_HPP
#define DATA_IOBUFFER_HPP

#include "DataAccess/Data_PageFrame.hpp"

/// this class allocates io aligned buffers of frame control for multi io purposes
class Data_IOBuffer
{
public:
    /// default constructor
    Data_IOBuffer() {}
    /// default copy constructor
    Data_IOBuffer(const Data_IOBuffer &src);
    /// allocates the heap memory
    bool Initialize(SAPDB_Int4        ioBufferSize,
                    SAPDB_Byte* const ioBuffer);
    /// returns the raw memory block (pointer and length)
    Data_PageFrame& GetBuffer();
    /// if a frame was allocated it is now deallocated
    ~Data_IOBuffer();
private:
    Data_PageFrame m_Frame;
};

/* --------------------------------------------------------------------------- */
inline Data_IOBuffer::Data_IOBuffer(const Data_IOBuffer &src)
{
	m_Frame.Assign(src.m_Frame);
}

/* --------------------------------------------------------------------------- */
inline Data_PageFrame& Data_IOBuffer::GetBuffer()
{
    return m_Frame;
}

/* --------------------------------------------------------------------------- */
inline Data_IOBuffer::~Data_IOBuffer()
{
    m_Frame.Deassign();
}
#endif // DATA_IOBUFFER_HPP