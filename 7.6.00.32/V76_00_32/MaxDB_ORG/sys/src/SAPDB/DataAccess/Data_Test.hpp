/*!
  @file           Data_Test.hpp
  @author         UweH
  @brief          defines the test call function

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
#ifndef Data_Test_HPP
#define Data_Test_HPP

#include "SAPDBCommon/SAPDB_Types.h"

class Trans_Context;

/// This calls the component tests of DataAccess
externCpp bool Data_Test (Trans_Context    &trans,
                          SAPDB_UTF8 const *utf8StringArgument,
                          SAPDB_Int4        utf8StringArgumentByteLength);

#endif // Data_Test_HPP
