/*!
  @file           Table_Exceptions.hpp
  @author         UweH
  @brief          Declaration of the exception classes for component Table

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
#ifndef Table_Exceptions_HPP
#define Table_Exceptions_HPP

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "Table/Table_Messages.hpp"

/// defines an exception class for the component DataFile

SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS (Table);

#endif // Table_Exceptions_HPP

