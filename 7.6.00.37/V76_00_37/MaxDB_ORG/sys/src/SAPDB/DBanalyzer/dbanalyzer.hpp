/*!
  -----------------------------------------------------------------------------
  module: dbanalyzer.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: DBanalyzer - The performance analyse tool for SAPDB
  description:  dbanalyzer main function
  version:      7.4 and higher 
  -----------------------------------------------------------------------------

  Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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




  -----------------------------------------------------------------------------
*/

#ifndef _DBANALYZER_HPP_
#define _DBANALYZER_HPP_

// instance type strings
#define DBA_INSTANCETYPE_TXT_OLTP  "OLTP"
#define DBA_INSTANCETYPE_TXT_LVC   "LVC"

// instance type defines
#define DBA_INSTANCETYPE_OLTP 1   // only oltp instances
#define DBA_INSTANCETYPE_LVC  2   // only livecache instances

#define DBA_INSTANCETYPE_ALL    DBA_INSTANCETYPE_OLTP \
                              + DBA_INSTANCETYPE_LVC

#endif