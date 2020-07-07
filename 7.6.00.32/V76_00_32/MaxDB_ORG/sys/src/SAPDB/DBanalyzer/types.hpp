/*!
  -----------------------------------------------------------------------------
  module: types.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: DBanalyzer - The performance analyse tool for SAPDB
  description:  type mapping for dbanalyzer
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

#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#include <stdio.h>
#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"

typedef  SAPDB_Char              DBA_char;
typedef  SAPDB_Bool              DBA_bool; 
typedef  SAPDB_Real8             DBA_double;
typedef  SAPDB_Int               DBA_int;
typedef  SAPDB_UInt              DBA_uint;  
typedef  SAPDB_Long              DBA_long; 
typedef  SAPDB_ULong             DBA_ulong; 
typedef  SAPDB_Int8              DBA_int8; 
typedef  void                    DBA_void; 
typedef  SAPDB_Int2              DBA_short; 
typedef  Tools_DynamicUTF8String DBA_string;
typedef  FILE*                   DBA_file;

#endif