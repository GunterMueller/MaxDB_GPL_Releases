/*!
  @file     KernelParam_IOManager.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Mapping for configuration parameter names used by the IOManagement
  @see            

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

#ifndef KERNELPARAM_IOMANAGER_HPP
#define KERNELPARAM_IOMANAGER_HPP

/*! cserv name of the configuration parameter MAXDATAVOLUMES */
#define KERNELPARAM_MAXDATAVOLUMES  (SAPDB_UTF8 *)"MAXDATAVOLUMES"

/*! cserv name of the configuration parameter MAXLOGVOLUMES */
#define KERNELPARAM_MAXLOGVOLUMES  (SAPDB_UTF8 *)"MAXLOGVOLUMES"

/*! cserv name of the configuration parameter LOG_MIRRORED */
#define KERNELPARAM_LOG_MIRRORED  (SAPDB_UTF8 *)"LOG_MIRRORED"

/*! cserv name of the configuration parameter DATA_VOLUME_GROUPS */
#define KERNELPARAM_DATA_VOLUME_GROUPS  (SAPDB_UTF8 *)"DATA_VOLUME_GROUPS"

/*! cserv name of the configuration parameter DATA_IO_BLOCK_COUNT */
#define KERNELPARAM_DATA_IO_BLOCK_COUNT  (SAPDB_UTF8 *)"DATA_IO_BLOCK_COUNT"

/*! cserv name of the configuration parameter LOG_IO_BLOCK_COUNT */
#define KERNELPARAM_LOG_IO_BLOCK_COUNT  (SAPDB_UTF8 *)"LOG_IO_BLOCK_COUNT"

#endif /* KERNELPARAM_IOMANAGER_HPP */


