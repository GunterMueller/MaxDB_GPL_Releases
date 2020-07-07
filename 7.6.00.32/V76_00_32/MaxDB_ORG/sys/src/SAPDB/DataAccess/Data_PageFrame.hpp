/*!
  @file           Data_PageFrame.hpp
  @author         UweH
  @ingroup        PageHandling
  @brief          Definition of a frame.

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
#ifndef DATA_PAGEFRAME_HPP
#define DATA_PAGEFRAME_HPP
#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"
/// frame handling
typedef  SAPDBFields_Field     Data_PageFrame;
/// size of memory block on a page in bytes
typedef SAPDB_UInt2            Data_PageFrameSizeType;
/// offset on a frame in bytes
typedef Data_PageFrameSizeType Data_PageFrameOffsetType;
#endif // DATA_PAGEFRAME_HPP
