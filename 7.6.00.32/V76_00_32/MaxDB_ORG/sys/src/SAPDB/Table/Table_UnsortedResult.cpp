/*!
  @file           Table_UnsortedResult.cpp
  @ingroup		  Table
  @author         UweH

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
#include "Table/Table_UnsortedResult.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp"

#include "SAPDBCommon/Messages/SDBMsg_Table.h"

/* --------------------------------------------------------------------------- */
bool Table_UnsortedResult::GetFirst ( Table_IResult::Iterator &iter,
                                      Msg_List                &errlist ) const
{
    errlist = Msg_List(Msg_List::Error, SDBMSG_TABLE_NOT_IMPLEMENTED, Msg_Arg("DESCRIPTION","Table_UnsortedResult::GetFirst"));
    return false;
}

/* --------------------------------------------------------------------------- */
bool Table_UnsortedResult::GetLast ( Table_IResult::Iterator &iter,
                                     Msg_List                &errlist ) const
{
	errlist = Msg_List(Msg_List::Error, SDBMSG_TABLE_NOT_IMPLEMENTED, Msg_Arg("DESCRIPTION","Table_UnsortedResult::GetLast"));
    return false;
}
