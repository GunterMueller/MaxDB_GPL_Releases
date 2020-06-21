/*****************************************************************************/
/*!
  @file         KernelParam_DataAccess.hpp
  @author       MartinKi
  @ingroup      

  @brief        Mapping for general configuration parameter names used by the DataAccess component

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
/*****************************************************************************/

#ifndef KERNELPARAM_DATAACCESS_HPP
#define KERNELPARAM_DATAACCESS_HPP

#define KERNELPARAM_UPDATESTAT_PARALLEL_SERVERS (RTEConf_Parameter::Name)"UPDATESTAT_PARALLEL_SERVERS"
#define KERNELPARAM_UPDATESTAT_SAMPLE_ALGO (RTEConf_Parameter::Name)"UPDATESTAT_SAMPLE_ALGO"

// new with PTS 1133794 mb 2005-02-07
#define KERNELPARAM_LOG_QUEUE_COUNT (RTEConf_Parameter::Name)"LOG_QUEUE_COUNT"
#define KERNELPARAM_USED_MAX_LOG_QUEUE_COUNT (RTEConf_Parameter::Name)"USED_MAX_LOG_QUEUE_COUNT" // PTS 1135304

/*!
  @brief Name of the parameter for defining the number of temporary result files
         used by the parallel index creation
*/
#define KERNELPARAM_IDXFILE_LIST_SIZE (RTEConf_Parameter::Name)"_IDXFILE_LIST_SIZE"

/// detailed description can be found in cserv.pcf
#define KERNELPARAM_INDEX_LEAF_CACHING (RTEConf_Parameter::Name)"INDEX_LEAF_CACHING"

#endif // KERNELPARAM_DATAACCESS_HPP
