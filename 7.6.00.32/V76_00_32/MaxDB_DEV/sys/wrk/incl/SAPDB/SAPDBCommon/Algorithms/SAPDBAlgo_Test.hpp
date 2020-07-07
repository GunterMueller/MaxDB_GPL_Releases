/*!
  @file           SAPDBAlgo_Test.hpp
  @ingroup		  SAPDBAlgo
  @author         DirkT
  @brief          This is the component test for SAPDBAlgo
                  It can be called with: "DIAGNOSE CHECK INSTANCE 8"

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
#ifndef SAPDBAlgo_Test_HPP
#define SAPDBAlgo_Test_HPP

class Trans_Context;

/*!
    @brief entry point for this component test
    @param Trans [in&out]
 */
externCpp bool SAPDBAlgo_Test (Trans_Context &trans);

#endif /*SAPDBAlgo_Test_HPP*/
