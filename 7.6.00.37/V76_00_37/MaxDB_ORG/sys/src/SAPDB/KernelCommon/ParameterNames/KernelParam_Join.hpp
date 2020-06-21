/*****************************************************************************/
/*!
  @file         KernelParam_Join.hpp
  @author       MartinKi
  @ingroup      Join

  @brief        Mapping for configuration parameter names used by Join component

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

#ifndef KERNELPARAM_JOIN_HPP
#define KERNELPARAM_JOIN_HPP

/// cserv name of the configuration parameter MAX_SINGLE_HASHTABLE_SIZE
#define KERNELPARAM_JOIN_MAX_SINGLE_HASHTABLE_SIZE (SAPDB_UTF8 *)"MAX_SINGLE_HASHTABLE_SIZE"

#define KERNELPARAM_JOIN_MAX_HASHTABLE_MEMORY (SAPDB_UTF8 *)"MAX_HASHTABLE_MEMORY"

#define KERNELPARAM_OPTIMIZE_JOIN_MAX_HASHTABLE_MEMORY (SAPDB_UTF8 *)"MAX_HASHTABLE_MEMORY"

#define KERNELPARAM_OPTIMIZE_JOIN_PARALLEL_SERVERS (SAPDB_UTF8 *)"OPTIMIZE_JOIN_PARALLEL_SERVERS"

#define KERNELPARAM_JOIN_MAXTAB_LEVEL4               (RTEConf_Parameter::Name)"JOIN_MAXTAB_LEVEL4"
#define KERNELPARAM_JOIN_MAXTAB_LEVEL9               (RTEConf_Parameter::Name)"JOIN_MAXTAB_LEVEL9"
#define KERNELPARAM_JOIN_OPERATOR_IMPLEMENTATION     (RTEConf_Parameter::Name)"JOIN_OPERATOR_IMPLEMENTATION"
#define KERNELPARAM_JOIN_SEARCH_LEVEL                (RTEConf_Parameter::Name)"JOIN_SEARCH_LEVEL"
#define KERNELPARAM_JOIN_TABLEBUFFER                 (RTEConf_Parameter::Name)"JOIN_TABLEBUFFER"
#define KERNELPARAM_OPTIMIZE_JOIN_HASHTABLE          (RTEConf_Parameter::Name)"OPTIMIZE_JOIN_HASHTABLE"
#define KERNELPARAM_OPTIMIZE_JOIN_HASH_MINIMAL_RATIO (RTEConf_Parameter::Name)"OPTIMIZE_JOIN_HASH_MINIMAL_RATIO"
#define KERNELPARAM_OPTIMIZE_JOIN_ONEPHASE           (RTEConf_Parameter::Name)"OPTIMIZE_JOIN_ONEPHASE"
#define KERNELPARAM_OPTIMIZE_JOIN_OUTER              (RTEConf_Parameter::Name)"OPTIMIZE_JOIN_OUTER"
#define KERNELPARAM_OPTIMIZE_JOIN_PARALLEL_MINSIZE   (RTEConf_Parameter::Name)"OPTIMIZE_JOIN_PARALLEL_MINSIZE"
#define KERNELPARAM_OPTIMIZE_OPERATOR_JOIN           (RTEConf_Parameter::Name)"OPTIMIZE_OPERATOR_JOIN"
#define KERNELPARAM_OPTIMIZE_OPERATOR_JOIN_COSTFUNC  (RTEConf_Parameter::Name)"OPTIMIZE_OPERATOR_JOIN_COSTFUNC"
#define KERNELPARAM_OPTIMIZE_JOIN_OPERATOR_SORT      (RTEConf_Parameter::Name)"OPTIMIZE_JOIN_OPERATOR_SORT"
#define KERNELPARAM_OPTIM_JOIN_FETCH                 (RTEConf_Parameter::Name)"OPTIM_JOIN_FETCH"


#endif // KERNELPARAM_JOIN_HPP
