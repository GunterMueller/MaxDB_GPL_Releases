/*!
  @file    Log_RawDeviceIterator.hpp
  @ingroup Logging
  @author  UweH
  @author  MartinB
  @brief   Contains Log_RawDevieIterator for safe access to the Log_Volume within bounds

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
#ifndef Log_RawDeviceIterator_HPP
#define Log_RawDeviceIterator_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_Types.hpp"

/// definition of an iterator with a value range for accessing the Log_Volume
typedef SAPDB_RangeIterator<Log_RawDeviceOffset, Log_DeviceBlockCount> Log_RawDeviceIterator;

#endif  // Log_RawDeviceIterator_HPP



