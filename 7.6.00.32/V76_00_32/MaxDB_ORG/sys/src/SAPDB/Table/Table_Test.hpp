/*!
  @file           Table_Test.hpp
  @ingroup		  Table
  @author         UweH
  @brief          This is the component test for the Table_Table class.
                  It can be called with: "DIAGNOSE CHECK INSTANCE 5"

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
#ifndef Table_Test_HPP
#define Table_Test_HPP

class Trans_Context;

/*!
    @brief entry point for this component test
    @param trans [in&out]
 */
externCpp bool Table_Test (Trans_Context &trans);

#endif /*Table_Test_HPP*/